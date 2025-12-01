#include "calculation_parameters.h"
#include "calendrical_helper.h"
#include "coordinates.h"
#include "prayer_times.h"
#include <stdio.h>

#define PARIS_COORDINATES                                                      \
  (coordinates_t) { 48.866667, 2.333333 }

int main(void) {
  coordinates_t coordinates = PARIS_COORDINATES;
  high_latitude_rule_t highLatitudeRule = MIDDLE_OF_THE_NIGHT;
  calculation_parameters_t calculation_parameters = (calculation_parameters_t){
      OTHER, 15.0, 13.0, 0, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
  calculation_parameters.highLatitudeRule = highLatitudeRule;

  printf("Using calculation high lat: %s\n",
         get_high_latitude_rule_name(calculation_parameters.highLatitudeRule));
  printf("Using coordinates: (lat:%f, long:%f)\n", coordinates.latitude,
         coordinates.longitude);
  printf("Using calculation method: %s\n",
         get_calculation_method_name(calculation_parameters.method));
  printf("Using calculation angles: fajr:%d ishaa:%d - %f\n",
         (int)calculation_parameters.fajrAngle,
         calculation_parameters.ishaInterval, calculation_parameters.ishaAngle);
  printf("Using calculation asr: %s\n",
         get_madhab_name(calculation_parameters.madhab));
  printf("Using calculation high lat: %s\n",
         get_high_latitude_rule_name(calculation_parameters.highLatitudeRule));

  char buffer[80];
  printf("Calculating prayer times...\n");

  struct tm start_date_tm = {0};
  start_date_tm.tm_year = 2017 - 1900;
  start_date_tm.tm_mon = 10 - 1;
  start_date_tm.tm_mday = 1;
  time_t now = mktime(&start_date_tm);
  time_t start_time = add_days(now, -46);

  struct tm *start_tm = localtime(&start_time);
  printf("Starting from date: %d/%d/%d\n", start_tm->tm_mday,
         start_tm->tm_mon + 1, start_tm->tm_year + 1900);

  printf(" Date \t\t Fajr \t\t Sunrise \t Dhuhr \t\t Asr \t\t Maghrib \t\t "
         "Ishaa \t Midnight\n");

  for (int i = 1; i < 31; i++) {
    time_t ref_date = add_days(start_time, i);

    prayer_times_t prayer_times =
        new_prayer_times(&coordinates, ref_date, &calculation_parameters);

    strftime(buffer, 80, "%x", localtime(&ref_date));
    printf(" %s\t", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.fajr));
    printf(" %s\t", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.sunrise));
    printf(" %s\t", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.dhuhr));
    printf(" %s\t", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.asr));
    printf(" %s\t", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.maghrib));
    printf(" %s\t", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.isha));
    printf(" %s\n", buffer);

    strftime(buffer, 80, "%I:%M%p", localtime(&prayer_times.midnight));
    printf(" %s\n", buffer);
  }
}
