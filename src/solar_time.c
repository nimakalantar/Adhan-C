#include "solar_time.h"
#include "calendrical_helper.h"
#include <math.h>
#include <time.h>

solar_time_t new_solar_time(const time_t today_time,
                            coordinates_t *coordinates) {
  time_t tomorrow_time = add_days(today_time, 1);
  time_t yesterday_time = add_days(today_time, -1);

  solar_coordinates_t solar =
      new_solar_coordinates(julian_day_from_time_t(today_time));
  solar_coordinates_t prevSolar =
      new_solar_coordinates(julian_day_from_time_t(yesterday_time));
  solar_coordinates_t nextSolar =
      new_solar_coordinates(julian_day_from_time_t(tomorrow_time));

  double approximateTransit = get_approximate_transit(
      coordinates->longitude, solar.apparentSiderealTime, solar.rightAscension);
  double solarAltitude = -50.0 / 60.0;

  double transit = corrected_transit(
      approximateTransit, coordinates->longitude, solar.apparentSiderealTime,
      solar.rightAscension, prevSolar.rightAscension, nextSolar.rightAscension);
  double sunrise = corrected_hour_angle(
      approximateTransit, solarAltitude, coordinates, false,
      solar.apparentSiderealTime, solar.rightAscension,
      prevSolar.rightAscension, nextSolar.rightAscension, solar.declination,
      prevSolar.declination, nextSolar.declination);
  double sunset = corrected_hour_angle(
      approximateTransit, solarAltitude, coordinates, true,
      solar.apparentSiderealTime, solar.rightAscension,
      prevSolar.rightAscension, nextSolar.rightAscension, solar.declination,
      prevSolar.declination, nextSolar.declination);

  return (solar_time_t){transit, sunrise,   sunset,    coordinates,
                        solar,   prevSolar, nextSolar, approximateTransit};
}

double hour_angle(solar_time_t *solar_time, double angle, bool after_transit) {
  return corrected_hour_angle(
      solar_time->approximateTransit, angle, solar_time->observer,
      after_transit, solar_time->solar.apparentSiderealTime,
      solar_time->solar.rightAscension, solar_time->prevSolar.rightAscension,
      solar_time->nextSolar.rightAscension, solar_time->solar.declination,
      solar_time->prevSolar.declination, solar_time->nextSolar.declination);
}

double afternoon(solar_time_t *solar_time, shadow_length shadow_length) {
  double tangent =
      fabs(solar_time->observer->latitude - solar_time->solar.declination);
  double inverse = shadow_length + safe_tan(to_radians(tangent));
  double angle = to_degrees(safe_atan(1.0 / inverse));

  return hour_angle(solar_time, angle, true);
}
