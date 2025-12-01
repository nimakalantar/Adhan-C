#include "calculation_parameters.h"

night_portions_t new_night_portions(double fajr, double isha) {
  return (night_portions_t){fajr, isha};
}

night_portions_t get_night_portions(calculation_parameters_t *params) {
  switch (params->highLatitudeRule) {
  case MIDDLE_OF_THE_NIGHT:
    return new_night_portions(0.5, 0.5);
  case SEVENTH_OF_THE_NIGHT:
    return new_night_portions(1.0 / 7.0, 1.0 / 7.0);
  case TWILIGHT_ANGLE:
    return new_night_portions(params->fajrAngle / 60.0,
                              params->ishaAngle / 60.0);
  default:
    return new_night_portions(0.5, 0.5);
  }
}

calculation_parameters_t getParameters(calculation_method method) {
  switch (method) {
  case MUSLIM_WORLD_LEAGUE:
    return (calculation_parameters_t){
        method, 18.0, 17.0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  case EGYPTIAN:
    return (calculation_parameters_t){
        method, 20.0, 18.0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  case KARACHI:
    return (calculation_parameters_t){
        method, 18.0, 18.0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  case UMM_AL_QURA:
    return (calculation_parameters_t){
        method, 18.5, 0, 90, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
  case GULF:
    return (calculation_parameters_t){
        method, 19.5, 0, 90, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
  case MOON_SIGHTING_COMMITTEE:
    return (calculation_parameters_t){
        method, 18.0, 18.0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 5, 0, 3, 0, 0}};
  case NORTH_AMERICA:
    return (calculation_parameters_t){
        method, 15.0, 15.0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  case KUWAIT:
    return (calculation_parameters_t){
        method, 18.0, 17.5, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  case QATAR:
    return (calculation_parameters_t){
        method, 18.0, 0, 90, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
  case OTHER:
    return (calculation_parameters_t){
        method, 0, 0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  default:
    return (calculation_parameters_t){
        OTHER, 0, 0, 0, SHAFI, TWILIGHT_ANGLE, {0, 0, 1, 0, 0, 0, 0}};
  }
}
