# Mount control: roadmap and strategies

Status: the first version (goto "leapfrog" tracking for OnStepX, Meade LX200 classic and SynScan) is on
branch `matt_mount_control` and builds in GitHub Actions. This document covers what is left, with most of
the space given to smooth (continuous) tracking.

All new code, comments and UI strings are in English. New `tr()` strings are English source text; they
need no translation and will show in English even when the rest of the UI is French.

---

## 1. What SkyTrack does (from heavenscape.com)

Sources: the update history (`STHistory.txt`), the v2.0.2 User Reference Manual and the
"TCP API Closed Loop Specification v1.3" PDFs on heavenscape.com.

| Feature | What the material says | Version |
|---|---|---|
| Two tracking methods | **Continuous**: repeatedly sends new axis rates and re-centres using the measured mount position; needs variable-rate `MoveAxis` and near-real-time position feedback. **Leap Frog**: slews ahead of the satellite, waits while it crosses the field, and repeats. | 1.5.1 onwards |
| Native ("legacy") interfaces | Manufacturer protocols (Meade LX200 Classic, LX200GPS, Celestron NexStar/CGE/AVX, SynScan via the SynScan Pro app, iOptron, Astro-Physics). The manual says they usually track better than ASCOM, and they do continuous tracking only, with no leapfrog. | 1.4.x onwards |
| Controller tuning | Two settings: an update frequency (how often a new rate is computed and sent), and minimum and maximum "aggression" (how hard the mount-to-satellite separation is corrected). Oscillation means aggression is too high; lag or jumps come from queued position feedback or an update rate that is too fast. | 1.6.2 (aggression settings added) |
| Calibration | A per-mount routine checks axis directions and side-of-pier behaviour, and shows whether the mount reports its position quickly enough for closed-loop correction. It also covers `RightAscensionRate`/`DeclinationRate`. | 1.7.6, 1.8.8b, 1.9.1 |
| Manual corrections | **Timing** adjustment (seconds, along the track) and **perpendicular** adjustment (arcmin, across the track), in fine and coarse steps, from the keyboard, mouse or a gamepad. Constant along-track error comes from the clock or element age; constant cross-track error from alignment or site. | 1.5.1, 1.7.1 |
| Closed-loop API | A localhost TCP server (newline-delimited JSON, default port 3292). An external program, typically camera-based, sets **absolute** `timing_seconds` and `perpendicular_arcmin` offsets on top of SkyTrack's prediction and receives events (`tracking_started`, `satellite_acquired`, `meridian_flip_started`, ...). | 2.0.2 (Oct 2026) |
| Pass pointing model | Plate-solved model points along the predicted path, plus an RMS figure to assess continuous tracking. | 1.6.3, 1.6.4 |
| Start before AOS | Slew to the AOS point and wait for the satellite. | 1.5.9 |
| Safety and setup | Upper altitude limit, sun avoidance, cord-wrap prevention, delayed meridian flip, and an alert when the mount's sidereal time disagrees with the PC's (catches setup errors). | 1.5.8, 1.6.7, 1.6.8, 1.7.4 |
| Open-loop custom rates | Sends computed RA/Dec rates (ASCOM `RightAscensionRate`/`DeclinationRate`) for slow targets; PHD2 can guide on a star while SkyTrack supplies the target motion. | 1.6.9b, 1.7.0, 1.7.2 |

**Takeaway.** Smooth tracking is a feedback loop. It sends axis **rates** every update, made of the
predicted rate plus a bounded correction proportional to the measured error, using fresh position readback
from the mount. Two offsets can be added on top: time along the track and distance across it. Getting good
results depends on calibration (directions, latency) and on tuning (update rate, gain limits).

---

## 2. Smooth tracking design for PreviSat

### 2.1 Controller (mount-independent)

Each cycle (start at 4 Hz, configurable):

1. **Predict.** Compute the target position in **mount axis coordinates** at `t + latency` and
   `t + latency + dt`:
   - Alt-az mounts: (azimuth, altitude).
   - Equatorial mounts: (hour angle, declination) and the pier side.

   The difference gives the **feed-forward rate** on each axis. Compute it numerically from SGP4
   (`SatellitePointing`), not from a fixed 1 s table.
2. **Measure.** Read the mount position, noting when the request went out and when the reply came back.
   Use the midpoint as the measurement time, and compute the error in axis coordinates (scale azimuth by
   cos(altitude) for the on-sky error).
3. **Correct.** Rate = feed-forward + clamp(K · error, ±max correction rate).
   - K moves between a *minimum* and a *maximum aggression* (SkyTrack's two parameters): high when the error
     is large, low near zero. A small dead band stops the mount hunting.
4. **Limit.** Clamp to the mount's maximum rate and maximum acceleration (rate change per cycle). Enforce the
   altitude, keyhole and pier-side limits before sending anything.
5. **Fall back.** If the error goes over a threshold (for example 1–2°), use one goto to catch up
   (the existing leapfrog code), then resume rate control.
6. **Watchdog.** If a cycle overruns or readback fails twice in a row, stop the axes. OnStep also times out
   guide moves by itself (`GUIDE_TIME_LIMIT`), which is a useful second safety layer.

The two **user offsets** (timing in seconds, cross-track in arcmin) change the target *before* step 1:
evaluate the satellite at `t + timing`, then move it perpendicular to its apparent direction of motion. Arrow
keys or a gamepad drive them, and they are also exposed for closed-loop clients (see 2.5).

**Geometry hazards.** These are handled by checking the whole pass in advance, the way `PassSummary` already
does:

- *Alt-az near the zenith*: the azimuth rate goes to infinity, so keep the current maximum-altitude limit,
  or accept a gap while the mount swings round.
- *Equatorial near the pole*: the HA rate blows up. This is rare for satellites.
- *Meridian on a German equatorial mount*: choose the pier side for the **whole pass** before AOS, so that no
  flip is needed. OnStepX can be told which side of the pier to use (`:MNe#` / `:MNw#`, within the meridian
  overlap). If the pass can't fit on one side, warn and plan one controlled flip at the slowest point.

### 2.2 OnStepX (first target, best features)

From the OnStepX source (`Guide.command.cpp`, `Mount.command.cpp`):

| Command | Effect | Use |
|---|---|---|
| `:RA[n.n]#`, `:RE[n.n]#` | Set the axis 1 / axis 2 rate in **degrees per second**, up to the goto rate. "Tracking is suspended in this mode." No reply. | Variable-rate moves (equivalent to `MoveAxis`) |
| `:Me#` `:Mw#` / `:Mn#` `:Ms#` | Move the axis at that rate, east/west or north/south. Each move times out after `GUIDE_TIME_LIMIT`. | Start or keep motion; the sign sets the direction |
| `:Qe#` `:Qw#` `:Qn#` `:Qs#`, `:Q#` | Stop one axis, or everything. | Zero rate and emergency stop |
| `:SXTR,n.n#`, `:SXTD,n.n#` | RA/Dec **tracking-rate offsets** in arcsec per sidereal second, clamped to ±1800 (about 0.5°/s), added to sidereal tracking. | Smooth for slow targets (GEO, high orbits, the slow ends of a LEO pass) |
| `:GX42#`, `:GX43#` | Instrument (axis) angles as decimal degrees. | Fast, unambiguous feedback for the controller |
| `:GRH#`, `:GDH#` | High-precision RA/Dec readback. | Logging and error display |
| `:D#` | Returns `\x7f#` while moving, `#` otherwise. | Already used |

Plan:

- **Use `:RA`/`:RE` with `:M?`** for LEO, with `:GX42/43` feedback.
- **Use the `:SXTR`/`:SXTD` offsets** whenever the needed rate fits within ±0.5°/s. This is gentler, and
  sidereal tracking stays on.

Things to check on your mount before writing the driver:

1. Does a new `:RA` value take effect **while** `:Me#` is running, or does the move have to be sent again?
2. What `GUIDE_TIME_LIMIT` is set in your `Config.h`? The controller must resend moves before it expires.
3. On a German equatorial mount, does the axis 2 north/south direction reverse with the pier side? (The
   calibration step should detect this automatically.)
4. How fresh is `:GX42#`? Move at a known rate and compare the readback with elapsed time.
5. Round-trip time over Wi-Fi compared with USB serial.

### 2.3 SynScan hand controller (and possibly the Orion TeleTrack)

- **Variable-rate move**: `P`, 3, axis (16 = azimuth/RA, 17 = altitude/Dec), 6 (positive) or 7 (negative),
  rate high byte, rate low byte, 0, 0. The rate is in **arcsec/s × 4**. This is documented for alt-az
  tracking with recent hand-controller firmware.
- **Feedback**: `z` (precise azimuth/altitude) or `e` (precise RA/Dec).
- The hand controller's own tracking probably needs to be off (`T` 0) while PreviSat sets the rates.
- **TeleTrack**: first check whether it answers the SynScan echo test (`Kx` → `x#`). If it doesn't, try
  sniffing the hand-controller traffic, or treat it as goto-only.
- Longer term: the **Sky-Watcher motor-controller protocol**, used over Wi-Fi (UDP 11880) or the hand
  controller's PC-direct mode, sets step periods directly. That gives the smoothest control of
  Sky-Watcher mounts, but it's a different driver.

### 2.4 Meade LX200 classic

The classic command set has **no variable rate**: only four preset speeds (`:RG#` guide, `:RC#` centre,
`:RM#` find, `:RS#` slew) combined with `:Me#`/`:Mw#`/`:Mn#`/`:Ms#` and `:Q?#`.

Options, from best to worst:

1. **Leapfrog**, as now. Reliable, and the satellite crosses the field at each stop.
2. **Hybrid**: leapfrog with short leads, plus a fixed-speed nudge (`:RC#`/`:RM#` pulses) to trim the
   along-track error between gotos.
3. **Duty-cycle ("bang-bang") rate control**: switch between two preset speeds in proportion to the wanted
   rate. It moves, but jerkily, it's hard on the gears, and it depends on latency.

SkyTrack lists an "LX200 Classic legacy interface", but its method isn't documented. A cheap test is worth
doing: time how long a 1° move takes at each preset speed, and measure how quickly `:GR#`/`:GD#` follow.

### 2.5 Closed loop and interoperability (later)

- Build the timing/cross-track correction channel first, driven by the keyboard or a gamepad.
- Then consider a localhost TCP server that follows the **SkyTrack closed-loop API v1** message format
  (`get_adjustment`, `set_adjustment`, `get_tracking_status`, plus events). Camera-based correction tools
  written for SkyTrack could then drive PreviSat too.
- **ASCOM Alpaca** (HTTP/REST, no COM, cross-platform) as one more driver would cover many mounts at once
  through `MoveAxis` and `RightAscensionRate`/`DeclinationRate`.

---

## 3. Testing without the sky

Most controller work can be done before going near the hardware:

- **Mount simulator**: an in-process `MountDriver` that models axis position, maximum rate, acceleration,
  command latency, and position readback delay and noise. Run whole passes faster than real time against it,
  so the controller can be tuned (gains, update rate, lead) with no hardware.
- **Unit tests** in the existing Qt Test target, run in CI:
  - LX200 formatting and parsing (RA/Dec, latitude/longitude, UTC offset)
  - SynScan angle encoding
  - Pass summary and meridian detection
  - The controller against the simulator, failing if the error goes over a threshold
- **Pass logs**: write a CSV for each pass (time, target, mount position, error, commanded rates, latency).
  It is the main tool for tuning on real hardware, and plays the part of SkyTrack's RMS figure.
- **Pre-pass checks**:
  - Compare the mount's clock and sidereal time (`:GL#`, `:GS#`, `:GC#`) with the PC's, and warn on a mismatch
    (as SkyTrack does since 1.6.8).
  - Report the element age (SkyTrack recommends less than about 1 day for close work).

---

## 4. Other open items

| Item | Approach |
|---|---|
| Unit tests and CI | Add `src/mount` and the tests to the test target, and run `PreviSatTest` in the workflow. |
| Qt 6.11 in CI | Change back from 6.10.3 once aqtinstall can read the Qt 6.11 repository index. |
| SynScan over Wi-Fi | Needs the motor-controller protocol (section 2.3); separate driver. |
| Sun avoidance, cord wrap, horizon file | Extra limits checked by the controller and at pass validation. |
| Translating the existing French code | Separate, step-by-step effort. Order: (1) fill in the remaining `PreviSat_en.ts` UI strings; (2) translate comments and Doxygen file by file, keeping CI green; (3) rename identifiers only in code being actively maintained, as mechanical commits. Large renames make upstream merges from Astropedia painful. |

## 5. Suggested order

1. Mount simulator, pass CSV log, unit tests in CI.
2. Rate controller core with leapfrog fallback; timing/cross-track offsets with keyboard control.
3. OnStepX rate driver (`:RA`/`:RE` + moves, `:GX42/43` feedback, `:SXTR/TD` for slow segments) with a
   calibration wizard (directions, pier side, latency).
4. Pier-side planning for whole passes; clock and sidereal-time check.
5. SynScan `P` variable-rate tracking; TeleTrack probe.
6. LX200 classic hybrid nudging (if testing shows it helps).
7. Closed-loop TCP API compatible with SkyTrack v1; ASCOM Alpaca driver.
