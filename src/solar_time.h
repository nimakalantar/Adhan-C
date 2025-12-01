#ifndef ADHAN_SOLAR_TIME_H
#define ADHAN_SOLAR_TIME_H

#include "astronomical.h"
#include "calendrical_helper.h"
#include "coordinates.h"
#include "shadow.h"
#include "solar_coordinates.h"
#include <stdbool.h>

/**
 * @brief Solar time structure
 */
typedef struct {
  double transit;
  double sunrise;
  double sunset;
  const coordinates_t *observer;
  solar_coordinates_t solar;
  solar_coordinates_t prevSolar;
  solar_coordinates_t nextSolar;
  double approximateTransit;
} solar_time_t;

solar_time_t new_solar_time(const time_t today, coordinates_t *coordinates);

double hour_angle(solar_time_t *solar_time, double angle, bool after_transit);

double afternoon(solar_time_t *solar_time, shadow_length shadow_length);

#endif // ADHAN_SOLAR_TIME_H
