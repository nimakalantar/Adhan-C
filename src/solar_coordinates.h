#ifndef ADHAN_SOLAR_COORDINATES_H
#define ADHAN_SOLAR_COORDINATES_H

typedef struct {
  double declination;
  double rightAscension;
  double apparentSiderealTime;
} solar_coordinates_t;

solar_coordinates_t new_solar_coordinates(double julian_day);

#endif // ADHAN_SOLAR_COORDINATES_H
