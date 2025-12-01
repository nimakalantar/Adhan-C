#ifndef ADHAN_PRAYER_TIMES_H
#define ADHAN_PRAYER_TIMES_H

#include "calculation_parameters.h"
#include "coordinates.h"
#include "prayer.h"
#include <time.h>

typedef struct {
  time_t fajr;
  time_t sunrise;
  time_t dhuhr;
  time_t asr;
  time_t maghrib;
  time_t isha;
  time_t midnight;
} prayer_times_t;

#define NULL_PRAYER_TIMES {0, 0, 0, 0, 0, 0, 0};

prayer_times_t new_prayer_times(coordinates_t *coordinates, time_t date,
                                calculation_parameters_t *parameters);

prayer_t currentPrayer(prayer_times_t *prayer_times, time_t when);

prayer_t next_prayer(prayer_times_t *prayer_times, time_t when);

time_t timeForPrayer(prayer_times_t *prayer_times, prayer_t prayer);

time_t seasonAdjustedMorningTwilight(double latitude, int day, int year,
                                     time_t sunrise);

time_t seasonAdjustedEveningTwilight(double latitude, int day, int year,
                                     time_t sunset);

int daysSinceSolstice(int dayOfYear, int year, double latitude);

time_t calculate_fajr_time(coordinates_t *coordinates, time_t date,
                           calculation_parameters_t *parameters);
#endif /* ADHAN_PRAYER_TIMES_H */
