/*
 * Wrapper of the PPM_atm shadow function (https://github.com/whulizhen/PPM_atm_shadow_function)
 * exposing the interface expected by PreviSat.
 */

#include "ppm_atm.h"


// Defined in PPM_atm_ctypes.cpp (satellite position first, then Sun position, in km)
extern "C" double shadow_function(double xsat, double ysat, double zsat, double xsun, double ysun, double zsun);


namespace PPM_atm {

double shadow_factor(const std::array<double, 3> &sunPosition,
                     const std::array<double, 3> &satellitePosition)
{
    return shadow_function(satellitePosition[0], satellitePosition[1], satellitePosition[2],
                           sunPosition[0], sunPosition[1], sunPosition[2]);
}

}
