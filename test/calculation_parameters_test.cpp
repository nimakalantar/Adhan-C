#include "gtest/gtest.h"

extern "C" {
#include "../src/calculation_parameters.h"
}

TEST(CalculationMethodTest, NightPortion) {
  calculation_parameters_t parameters = (calculation_parameters_t){
      OTHER, 18, 18, 0, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
  parameters.highLatitudeRule = MIDDLE_OF_THE_NIGHT;
  night_portions_t night_portions = get_night_portions(&parameters);
  ASSERT_NEAR(night_portions.fajr, 0.5, 0.001);
  ASSERT_NEAR(night_portions.isha, 0.5, 0.001);

  parameters.highLatitudeRule = SEVENTH_OF_THE_NIGHT;
  night_portions_t night_portions2 = get_night_portions(&parameters);
  ASSERT_NEAR(night_portions2.fajr, 1.0 / 7.0, 0.001);
  ASSERT_NEAR(night_portions2.isha, 1.0 / 7.0, 0.001);

  parameters = (calculation_parameters_t){
      OTHER, 10, 15, 0, SHAFI, TWILIGHT_ANGLE, INIT_PRAYER_ADJUSTMENTS()};
  parameters.highLatitudeRule = TWILIGHT_ANGLE;
  night_portions_t night_portions3 = get_night_portions(&parameters);
  ASSERT_NEAR(night_portions3.fajr, 10.0 / 60.0, 0.001);
  ASSERT_NEAR(night_portions3.isha, 15.0 / 60.0, 0.001);
}
