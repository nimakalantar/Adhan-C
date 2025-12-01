#include "astronomical.h"
#include "double_utils.h"
#include <math.h>

double to_radians(double deg) { return deg * (M_PI / 180.0); }

double to_degrees(double radians) { return radians * (180.0 / M_PI); }

double mean_solar_longitude(double T) {
  /* Equation from Astronomical Algorithms page 163 */
  const double term1 = 280.4664567;
  const double term2 = 36000.76983 * T;
  const double term3 = 0.0003032 * pow(T, 2);
  const double L0 = term1 + term2 + term3;
  return unwind_angle(L0);
}

double mean_lunar_longitude(double T) {
  /* Equation from Astronomical Algorithms page 144 */
  const double term1 = 218.3165;
  const double term2 = 481267.8813 * T;
  const double Lp = term1 + term2;
  return unwind_angle(Lp);
}

double apparent_solar_longitude(double T, double L0) {
  const double longitude =
      L0 + solar_equation_of_the_center(T, mean_solar_anomaly(T));
  const double omega = 125.04 - (1934.136 * T);
  const double lambda =
      longitude - 0.00569 - (0.00478 * sin(to_radians(omega)));
  return unwind_angle(lambda);
}

double ascending_lunar_node_longitude(double T) {
  /* Equation from Astronomical Algorithms page 144 */
  const double term1 = 125.04452;
  const double term2 = 1934.136261 * T;
  const double term3 = 0.0020708 * pow(T, 2);
  const double term4 = pow(T, 3) / 450000;
  const double omega = term1 - term2 + term3 + term4;
  return unwind_angle(omega);
}

double mean_solar_anomaly(double T) {
  /* Equation from Astronomical Algorithms page 163 */
  const double term1 = 357.52911;
  const double term2 = 35999.05029 * T;
  const double term3 = 0.0001537 * pow(T, 2);
  const double M = term1 + term2 - term3;
  return unwind_angle(M);
}

double solar_equation_of_the_center(double T, double M) {
  /* Equation from Astronomical Algorithms page 164 */
  const double Mrad = to_radians(M);
  const double term1 =
      (1.914602 - (0.004817 * T) - (0.000014 * pow(T, 2))) * sin(Mrad);
  const double term2 = (0.019993 - (0.000101 * T)) * sin(2 * Mrad);
  const double term3 = 0.000289 * sin(3 * Mrad);
  return term1 + term2 + term3;
}

double mean_obliquity_of_the_ecliptic(double T) {
  /* Equation from Astronomical Algorithms page 147 */
  const double term1 = 23.439291;
  const double term2 = 0.013004167 * T;
  const double term3 = 0.0000001639 * pow(T, 2);
  const double term4 = 0.0000005036 * pow(T, 3);
  return term1 - term2 - term3 + term4;
}

double apparent_obliquity_of_the_ecliptic(double T, double epsilon0) {
  /* Equation from Astronomical Algorithms page 165 */
  const double O = 125.04 - (1934.136 * T);
  return epsilon0 + (0.00256 * cos(to_radians(O)));
}

double mean_sidereal_time(double T) {
  /* Equation from Astronomical Algorithms page 165 */
  const double JD = (T * 36525) + 2451545.0;
  const double term1 = 280.46061837;
  const double term2 = 360.98564736629 * (JD - 2451545);
  const double term3 = 0.000387933 * pow(T, 2);
  const double term4 = pow(T, 3) / 38710000;
  const double theta = term1 + term2 + term3 - term4;
  return unwind_angle(theta);
}

double nutation_in_longitude(double T, double L0, double Lp, double omega) {
  (void)T; // suppress unused parameter warning
  /* Equation from Astronomical Algorithms page 144 */
  const double term1 = (-17.2 / 3600) * sin(to_radians(omega));
  const double term2 = (1.32 / 3600) * sin(2 * to_radians(L0));
  const double term3 = (0.23 / 3600) * sin(2 * to_radians(Lp));
  const double term4 = (0.21 / 3600) * sin(2 * to_radians(omega));
  return term1 - term2 - term3 + term4;
}

double nutation_in_obliquity(double T, double L0, double Lp, double omega) {
  (void)T; // suppress unused parameter warning
  /* Equation from Astronomical Algorithms page 144 */
  const double term1 = (9.2 / 3600) * cos(to_radians(omega));
  const double term2 = (0.57 / 3600) * cos(2 * to_radians(L0));
  const double term3 = (0.10 / 3600) * cos(2 * to_radians(Lp));
  const double term4 = (0.09 / 3600) * cos(2 * to_radians(omega));
  return term1 + term2 + term3 - term4;
}

double altitude_of_celestial_body(double phi, double delta, double H) {
  /* Equation from Astronomical Algorithms page 93 */
  const double term1 = sin(to_radians(phi)) * sin(to_radians(delta));
  const double term2 =
      cos(to_radians(phi)) * cos(to_radians(delta)) * cos(to_radians(H));
  return to_degrees(safe_asin(term1 + term2));
}

/**
 * Estimates the fractional day (m) of the approximate transit (meridian
 * crossing) of a celestial body.
 *
 * Implements the formula from Jean Meeus, Astronomical Algorithms (2nd
 * Edition), page 102. This calculation provides an initial estimate for the
 * time of transit (when the celestial body crosses the local meridian), based
 * on the observer's longitude, the sidereal time at 0h UT, and the right
 * ascension of the body.
 *
 * Formula:
 *   m = (α + Lw - θ0) / 360
 * where:
 *   α  = right ascension of the body (degrees)
 *   Lw = observer's longitude (degrees, west is positive; here, Lw =
 * -longitude) θ0 = sidereal time at 0h UT (degrees)
 *
 * The result is normalized to the range [0, 1).
 *
 * Parameters:
 *   L        - Observer's longitude (degrees, east positive)
 *   theta0   - Sidereal time at 0h UT (degrees)
 *   alpha2   - Right ascension of the body (degrees)
 *
 * Returns:
 *   Fractional day (m) of approximate transit, normalized to [0, 1).
 *
 * Reference:
 *   Jean Meeus, Astronomical Algorithms, 2nd Edition, 1998, page 102.
 */
double get_approximate_transit(double L, double theta0, double alpha2) {
  const double Lw = L * -1;
  return normalize_with_bound((alpha2 + Lw - theta0) / 360, 1);
}

/**
 * Refines the fractional day (m) for the time of transit (meridian crossing) of
 * a celestial body.
 *
 * Implements the iterative correction from Jean Meeus, Astronomical Algorithms
 * (2nd Edition), page 102. This method improves the initial estimate of transit
 * time by accounting for the change in right ascension over the day and the
 * observer's longitude.
 *
 * Steps:
 *  1. Calculate the sidereal time (θ) at the estimated transit time.
 *  2. Interpolate the right ascension (α) for the estimated transit time.
 *  3. Compute the local hour angle (H) at the estimated transit time.
 *  4. Apply a correction (Δm) to the fractional day based on the hour angle.
 *  5. Return the corrected event time in hours (UT).
 *
 * Formula:
 *   θ = θ0 + 360.985647 * m0
 *   α = interpolated right ascension at m0
 *   H = θ - Lw - α
 *   Δm = H / -360
 *   Corrected transit time = (m0 + Δm) * 24
 *
 * Parameters:
 *   m0      - Initial fractional day estimate for transit
 *   L       - Observer's longitude (degrees, east positive)
 *   theta0  - Sidereal time at 0h UT (degrees)
 *   alpha2  - Right ascension at event day (degrees)
 *   alpha1  - Right ascension at previous day (degrees)
 *   alpha3  - Right ascension at next day (degrees)
 *
 * Returns:
 *   Corrected transit time in hours (UT).
 *
 * Reference:
 *   Jean Meeus, Astronomical Algorithms, 2nd Edition, 1998, page 102.
 */
double corrected_transit(double m0, double L, double theta0, double alpha2,
                         double alpha1, double alpha3) {
  const double Lw = L * -1;
  const double theta = unwind_angle(theta0 + (360.985647 * m0));
  const double alpha = unwind_angle(interpolate_angles(
      /* value */ alpha2, /* previousValue */ alpha1, /* nextValue */ alpha3,
      /* factor */ m0));
  const double H = closest_angle(theta - Lw - alpha);
  const double deltam = H / -360;
  return (m0 + deltam) * 24;
}

/**
 * Calculates the corrected hour angle for a celestial event (e.g., sunrise,
 * sunset, twilight).
 *
 * Implements the iterative correction method described in Jean Meeus,
 * Astronomical Algorithms (2nd Edition), page 102. This method refines the
 * fractional day (m) at which the event occurs, accounting for the observer's
 * position, the declination and right ascension of the celestial body, and the
 * desired altitude (h0).
 *
 * Steps:
 *  1. Compute the initial hour angle H0 using the observer's latitude,
 * declination, and desired altitude.
 *  2. Estimate the fractional day (m) for the event, adjusting for whether it
 * occurs after or before transit.
 *  3. Calculate the sidereal time (theta) and interpolate the right ascension
 * (alpha) and declination (delta) for the event time.
 *  4. Compute the local hour angle (H) and the true altitude (h) at the event
 * time.
 *  5. Apply a correction (deltam) to the fractional day based on the difference
 * between computed and desired altitude.
 *  6. Return the corrected event time in hours (UT).
 *
 * Parameters:
 *   m0           - Initial fractional day estimate for the event (e.g.,
 * sunrise/sunset). h0           - Desired altitude of the celestial body
 * (degrees, e.g., -0.833 for sunrise/sunset). coordinates  - Observer's
 * geographic coordinates (latitude, longitude). afterTransit - true if the
 * event occurs after transit (e.g., sunset), false for before (e.g., sunrise).
 *   theta0       - Sidereal time at 0h UT.
 *   alpha2       - Right ascension at event day.
 *   alpha1       - Right ascension at previous day.
 *   alpha3       - Right ascension at next day.
 *   delta2       - Declination at event day.
 *   delta1       - Declination at previous day.
 *   delta3       - Declination at next day.
 *
 * Returns:
 *   Corrected event time in hours (UT).
 *
 * Reference:
 *   Jean Meeus, Astronomical Algorithms, 2nd Edition, 1998, page 102.
 */
double corrected_hour_angle(double m0, double h0,
                            const coordinates_t *coordinates, bool afterTransit,
                            double theta0, double alpha2, double alpha1,
                            double alpha3, double delta2, double delta1,
                            double delta3) {
  const double Lw = coordinates->longitude * -1;
  const double term1 =
      sin(to_radians(h0)) -
      (sin(to_radians(coordinates->latitude)) * sin(to_radians(delta2)));
  const double term2 =
      cos(to_radians(coordinates->latitude)) * cos(to_radians(delta2));

  // Check for division by zero or very small denominator
  if (fabs(term2) < 1e-10) {
    // Return a safe default value when calculation is not possible
    return afterTransit ? (m0 + 0.25) * 24 : (m0 - 0.25) * 24;
  }

  const double ratio = term1 / term2;
  // Additional check for the acos argument validity
  if (fabs(ratio) > 1.0) {
    // Sun doesn't rise/set at this location/time - use approximate times
    return afterTransit ? (m0 + 0.25) * 24 : (m0 - 0.25) * 24;
  }

  const double H0 = to_degrees(safe_acos(ratio));
  const double m = afterTransit ? m0 + (H0 / 360) : m0 - (H0 / 360);
  const double theta = unwind_angle(theta0 + (360.985647 * m));
  const double alpha = unwind_angle(interpolate_angles(
      /* value */ alpha2, /* previousValue */ alpha1, /* nextValue */ alpha3,
      /* factor */ m));
  const double delta =
      interpolate_value(/* value */ delta2, /* previousValue */ delta1,
                        /* nextValue */ delta3, /* factor */ m);
  const double H = (theta - Lw - alpha);
  const double h = altitude_of_celestial_body(
      /* observerLatitude */ coordinates->latitude,
      /* declination */ delta, /* localHourAngle */ H);
  const double term3 = h - h0;
  const double term4 = 360 * cos(to_radians(delta)) *
                       cos(to_radians(coordinates->latitude)) *
                       sin(to_radians(H));

  // Check for division by zero in deltam calculation
  double deltam = 0.0;
  if (fabs(term4) > 1e-10) {
    deltam = term3 / term4;
    // Clamp deltam to reasonable bounds to prevent extreme corrections
    if (deltam > 0.5)
      deltam = 0.5;
    if (deltam < -0.5)
      deltam = -0.5;
  }

  return (m + deltam) * 24;
}

double interpolate_value(double y2, double y1, double y3, double n) {
  /* Equation from Astronomical Algorithms page 24 */
  const double a = y2 - y1;
  const double b = y3 - y2;
  const double c = b - a;
  return y2 + ((n / 2) * (a + b + (n * c)));
}

double interpolate_angles(double y2, double y1, double y3, double n) {
  /* Equation from Astronomical Algorithms page 24 */
  const double a = unwind_angle(y2 - y1);
  const double b = unwind_angle(y3 - y2);
  const double c = b - a;
  return y2 + ((n / 2) * (a + b + (n * c)));
}

double safe_acos(double x) {
  if (x < -1.0)
    x = -1.0;
  if (x > 1.0)
    x = 1.0;
  return acos(x);
}

double safe_asin(double x) {
  if (x < -1.0)
    x = -1.0;
  if (x > 1.0)
    x = 1.0;
  return asin(x);
}

double safe_tan(double x) {
  // Avoid values very close to π/2 multiples where tan becomes infinite
  double normalized = fmod(x, M_PI);
  if (fabs(normalized - M_PI / 2) < 1e-10 ||
      fabs(normalized + M_PI / 2) < 1e-10) {
    return (normalized > 0) ? 1e10 : -1e10; // Large but finite value
  }
  return tan(x);
}

double safe_atan(double x) {
  // atan is naturally bounded, but check for extreme values
  if (x > 1e10)
    return M_PI / 2 - 1e-10;
  if (x < -1e10)
    return -M_PI / 2 + 1e-10;
  return atan(x);
}

double safe_atan2(double y, double x) {
  // Check for both arguments being zero
  if (fabs(x) < 1e-15 && fabs(y) < 1e-15) {
    return 0.0;
  }
  return atan2(y, x);
}
