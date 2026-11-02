/*
 * Interface of the PPM_atm shadow function used by PreviSat (librairies/corps/satellite/conditioneclipse.cpp).
 *
 * The prebuilt lib/PPM_atm library is not part of the repository: for CI builds it is compiled from
 * https://github.com/whulizhen/PPM_atm_shadow_function together with ppm_atm.cpp (see .github/workflows).
 */

#ifndef PPM_ATM_H
#define PPM_ATM_H

#include <array>


namespace PPM_atm {

/**
 * @brief shadow_factor Fraction of the solar disk seen by the satellite (1 = full sunlight, 0 = umbra),
 * with a 13 km atmosphere model
 * @param[in] sunPosition position of the Sun (km, geocentric equatorial)
 * @param[in] satellitePosition position of the satellite (km, geocentric equatorial)
 * @return shadow factor
 */
double shadow_factor(const std::array<double, 3> &sunPosition,
                     const std::array<double, 3> &satellitePosition);

}

#endif // PPM_ATM_H
