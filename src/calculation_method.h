#ifndef ADHAN_CALCULATION_METHOD_H
#define ADHAN_CALCULATION_METHOD_H

typedef enum {
  /*
   * Muslim World League
   * Uses Fajr angle of 18 and an Isha angle of 17
   */
  MUSLIM_WORLD_LEAGUE,
  /**
   * Egyptian General Authority of Survey
   * Uses Fajr angle of 19.5 and an Isha angle of 17.5
   */
  EGYPTIAN,
  /**
   * University of Islamic Sciences, Karachi
   * Uses Fajr angle of 18 and an Isha angle of 18
   */
  KARACHI,
  /**
   * Umm al-Qura University, Makkah
   * Uses a Fajr angle of 18.5 and an Isha angle of 90. Note: You should add a
   * +30 minute custom adjustment of Isha during Ramadan.
   */
  UMM_AL_QURA,
  /**
   * The Gulf Region
   * Modified version of Umm al-Qura that uses a Fajr angle of 19.5.
   */
  GULF,
  /**
   * Moonsighting Committee
   * Uses a Fajr angle of 18 and an Isha angle of 18. Also uses seasonal
   * adjustment values.
   */
  MOON_SIGHTING_COMMITTEE,
  /**
   * Referred to as the ISNA method
   * This method is included for completeness, but is not recommended.
   * Uses a Fajr angle of 15 and an Isha angle of 15.
   */
  NORTH_AMERICA,
  /**
   * Kuwait
   * Uses a Fajr angle of 18 and an Isha angle of 17.5
   */
  KUWAIT,
  /**
   * Qatar
   * Modified version of Umm al-Qura that uses a Fajr angle of 18.
   */
  QATAR,
  /**
   * The default value for {@link CalculationParameters#method} when
   * initializing a
   * {@link CalculationParameters} object. Sets a Fajr angle of 0 and an Isha
   * angle of 0.
   */
  OTHER
} calculation_method;

static inline const char *
get_calculation_method_name(calculation_method method) {
  switch (method) {
  case MUSLIM_WORLD_LEAGUE:
    return (const char *)"Muslim World League";
  case EGYPTIAN:
    return (const char *)"Egyptian";
  case KARACHI:
    return (const char *)"Karachi";
  case UMM_AL_QURA:
    return (const char *)"Umm Al Qura";
  case GULF:
    return (const char *)"Gulf";
  case MOON_SIGHTING_COMMITTEE:
    return (const char *)"Moon Sighting Committee";
  case KUWAIT:
    return (const char *)"Kuwait";
  case QATAR:
    return (const char *)"Qatar";
  case OTHER:
    return (const char *)"Custom";
  default:
    return (const char *)"Unknown method";
  }
}

#endif // ADHAN_CALCULATION_METHOD_H
