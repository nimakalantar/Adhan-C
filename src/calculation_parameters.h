#ifndef ADHAN_CALCULATION_PARAMETERS_H
#define ADHAN_CALCULATION_PARAMETERS_H

#include "calculation_method.h"
#include "high_latitude_rule.h"
#include "madhab.h"
#include "prayer_adjustments.h"

typedef struct {
  calculation_method method;
  double fajrAngle;
  double ishaAngle;
  int ishaInterval;
  madhab_t madhab;
  high_latitude_rule_t highLatitudeRule;
  prayer_adjustments_t adjustments;
} calculation_parameters_t;

typedef struct {
  double fajr;
  double isha;
} night_portions_t;

static inline calculation_parameters_t INIT_CALCULATION_PARAMETERS(void) {
  return (calculation_parameters_t){
      OTHER, 0, 0, 0, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
}

night_portions_t new_night_portions(double fajr, double isha);
night_portions_t get_night_portions(calculation_parameters_t *params);

calculation_parameters_t getParameters(calculation_method method);

#endif // ADHAN_CALCULATION_PARAMETERS_H
