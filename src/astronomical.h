#ifndef ADHAN_ASTRONOMICAL_H
#define ADHAN_ASTRONOMICAL_H

#include "coordinates.h"
#include <math.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double to_radians(double degrees);
double to_degrees(double radians);
double safe_acos(double x);
double safe_asin(double x);
double safe_tan(double x);
double safe_atan(double x);
double safe_atan2(double y, double x);

double mean_solar_longitude(double julian_century);
double mean_lunar_longitude(double julian_century);
double apparent_solar_longitude(double julian_century, double mean_longitude);
double ascending_lunar_node_longitude(double julian_century);
double mean_solar_anomaly(double julian_century);
double solar_equation_of_the_center(double julian_century, double mean_anomaly);
double mean_obliquity_of_the_ecliptic(double julian_century);
double apparent_obliquity_of_the_ecliptic(double julian_century,
                                          double mean_obliquity);
double mean_sidereal_time(double julian_century);

double nutation_in_longitude(double julian_century, double solar_longitude,
                             double lunar_longitude, double ascending_node);
double nutation_in_obliquity(double julian_century, double solar_longitude,
                             double lunar_longitude, double ascending_node);

double altitude_of_celestial_body(double observer_latitude, double declination,
                                  double local_hour_angle);

double get_approximate_transit(double longitude, double sidereal_time,
                               double right_ascension);

double corrected_transit(double approximate_transit, double longitude,
                         double sidereal_time, double right_ascension,
                         double previous_right_ascension,
                         double next_right_ascension);

double corrected_hour_angle(
    double approximate_transit, double angle, const coordinates_t *coordinates,
    bool after_transit, double sidereal_time, double right_ascension,
    double previous_right_ascension, double next_right_ascension,
    double declination, double previous_declination, double next_declination);

double interpolate_value(double current, double previous, double next,
                         double factor);
double interpolate_angles(double current, double previous, double next,
                          double factor);

#endif /* ADHAN_ASTRONOMICAL_H */
