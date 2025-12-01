#include "solar_coordinates.h"
#include "astronomical.h"
#include "calendrical_helper.h"
#include "double_utils.h"
#include <math.h>
#include <stdlib.h>

solar_coordinates_t new_solar_coordinates(double julian_day) {
  double T = julian_century(julian_day);
  double L0 = mean_solar_longitude(T);
  double Lp = mean_lunar_longitude(T);
  double omega = ascending_lunar_node_longitude(T);
  double lambda = to_radians(apparent_solar_longitude(T, L0));

  double theta0 = mean_sidereal_time(T);
  double delta_psi = nutation_in_longitude(T, L0, Lp, omega);
  double delta_epsilon = nutation_in_obliquity(T, L0, Lp, omega);

  double epsilon0 = mean_obliquity_of_the_ecliptic(T);
  double epsilonapp =
      to_radians(apparent_obliquity_of_the_ecliptic(T, epsilon0));

  /* Equation from Astronomical Algorithms page 165 */
  double declination = to_degrees(safe_asin(sin(epsilonapp) * sin(lambda)));

  /* Equation from Astronomical Algorithms page 165 */
  double rightAscension = unwind_angle(
      to_degrees(safe_atan2(cos(epsilonapp) * sin(lambda), cos(lambda))));

  /* Equation from Astronomical Algorithms page 88 */
  double apparentSiderealTime =
      theta0 +
      (((delta_psi * 3600) * cos(to_radians(epsilon0 + delta_epsilon))) / 3600);

  return (solar_coordinates_t){declination, rightAscension,
                               apparentSiderealTime};
}
