#include "test_utils.h"
#include "gtest/gtest.h"

extern "C" {
#include "../src/calculation_parameters.h"
#include "../src/calendrical_helper.h"
#include "../src/prayer.h"
#include "../src/prayer_times.h"
}

static int get_days_since_solstice(int year, int month, int day,
                                   double latitude) {
  time_t date = get_utc_date(year, month, day);
  int day_of_year = gmtime(&date)->tm_yday + 1;
  return daysSinceSolstice(day_of_year, year, latitude);
}

TEST(PrayerTimesTest, DaysSinceSolstice) {
  ASSERT_EQ(get_days_since_solstice(2016, 1, 1, 1), 11);
  ASSERT_EQ(get_days_since_solstice(2015, 12, 31, 1), 10);
  ASSERT_EQ(get_days_since_solstice(2016, 12, 31, 1), 10);
  ASSERT_EQ(get_days_since_solstice(2016, 12, 21, 1), 0);
  ASSERT_EQ(get_days_since_solstice(2016, 12, 22, 1), 1);
  ASSERT_EQ(get_days_since_solstice(2016, 3, 1, 1), 71);
  ASSERT_EQ(get_days_since_solstice(2015, 3, 1, 1), 70);
  ASSERT_EQ(get_days_since_solstice(2016, 12, 20, 1), 365);
  ASSERT_EQ(get_days_since_solstice(2015, 12, 20, 1), 364);

  ASSERT_EQ(get_days_since_solstice(2015, 6, 21, -1), 0);
  ASSERT_EQ(get_days_since_solstice(2016, 6, 21, -1), 0);
  ASSERT_EQ(get_days_since_solstice(2015, 6, 20, -1), 364);
  ASSERT_EQ(get_days_since_solstice(2016, 6, 20, -1), 365);
}

static void get_local_str_time(time_t timestamp, char buffer[],
                               const char *tz) {
  char original_tz[64];
  char *current_tz = getenv("TZ");
  if (current_tz) {
    strncpy(original_tz, current_tz, sizeof(original_tz) - 1);
    original_tz[sizeof(original_tz) - 1] = '\0';
  } else {
    original_tz[0] = '\0';
  }

  setenv("TZ", tz, 1);
  tzset();

  tm *local_date = localtime(&timestamp);
  strftime(buffer, 9, "%I:%M %p", local_date);

  if (current_tz) {
    setenv("TZ", original_tz, 1);
  } else {
    unsetenv("TZ");
  }
  tzset();
}

TEST(PrayerTimesTest, testPrayerTimes) {
  time_t date = get_utc_date(2015, 7, 12);
  calculation_method method = NORTH_AMERICA;
  calculation_parameters_t params = getParameters(method);
  params.madhab = HANAFI;

  coordinates_t coordinates = {35.7750, -78.6336};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  char destString[9];

  get_local_str_time(prayerTimes.fajr, destString, "America/New_York");
  ASSERT_STREQ(destString, "04:42 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "America/New_York");
  ASSERT_STREQ(destString, "06:08 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "America/New_York");
  ASSERT_STREQ(destString, "01:21 PM");

  get_local_str_time(prayerTimes.asr, destString, "America/New_York");
  ASSERT_STREQ(destString, "06:22 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "America/New_York");
  ASSERT_STREQ(destString, "08:32 PM");

  get_local_str_time(prayerTimes.isha, destString, "America/New_York");
  ASSERT_STREQ(destString, "09:57 PM");

  get_local_str_time(prayerTimes.midnight, destString, "America/New_York");
  ASSERT_STREQ(destString, "12:37 AM");

  ASSERT_GT(prayerTimes.midnight, prayerTimes.maghrib);

  time_t next_date = date + 24 * 60 * 60;
  prayer_times_t next_day_times =
      new_prayer_times(&coordinates, next_date, &params);
  ASSERT_LT(prayerTimes.midnight, next_day_times.sunrise);
}

TEST(PrayerTimesTest, testOffsets) {
  time_t date = get_utc_date(2015, 12, 1);
  calculation_method method = MUSLIM_WORLD_LEAGUE;
  calculation_parameters_t params = getParameters(method);

  coordinates_t coordinates = {35.7750, -78.6336};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  char destString[9];

  get_local_str_time(prayerTimes.fajr, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:35 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "America/New_York");
  ASSERT_STREQ(destString, "07:06 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "America/New_York");
  ASSERT_STREQ(destString, "12:05 PM");

  get_local_str_time(prayerTimes.asr, destString, "America/New_York");
  ASSERT_STREQ(destString, "02:42 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:01 PM");

  get_local_str_time(prayerTimes.isha, destString, "America/New_York");
  ASSERT_STREQ(destString, "06:26 PM");

  params.adjustments = {10, 10, 10, 10, 10, 10};
  prayerTimes = new_prayer_times(&coordinates, date, &params);

  get_local_str_time(prayerTimes.fajr, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:45 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "America/New_York");
  ASSERT_STREQ(destString, "07:16 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "America/New_York");
  ASSERT_STREQ(destString, "12:14 PM");

  get_local_str_time(prayerTimes.asr, destString, "America/New_York");
  ASSERT_STREQ(destString, "02:52 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:11 PM");

  get_local_str_time(prayerTimes.isha, destString, "America/New_York");
  ASSERT_STREQ(destString, "06:36 PM");

  params.adjustments = INIT_PRAYER_ADJUSTMENTS();

  prayerTimes = new_prayer_times(&coordinates, date, &params);

  get_local_str_time(prayerTimes.fajr, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:35 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "America/New_York");
  ASSERT_STREQ(destString, "07:06 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "America/New_York");
  ASSERT_STREQ(destString, "12:04 PM");

  get_local_str_time(prayerTimes.asr, destString, "America/New_York");
  ASSERT_STREQ(destString, "02:42 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:01 PM");

  get_local_str_time(prayerTimes.isha, destString, "America/New_York");
  ASSERT_STREQ(destString, "06:26 PM");

  get_local_str_time(prayerTimes.midnight, destString, "America/New_York");
  ASSERT_STREQ(destString, "11:18 PM");

  ASSERT_GT(prayerTimes.midnight, prayerTimes.maghrib);
  time_t next_date = date + 24 * 60 * 60;
  prayer_times_t next_day_times =
      new_prayer_times(&coordinates, next_date, &params);
  ASSERT_LT(prayerTimes.midnight, next_day_times.sunrise);
}

TEST(PrayerTimesTest, testMoonsightingMethod) {
  time_t date = get_utc_date(2016, 1, 31);
  calculation_method method = MOON_SIGHTING_COMMITTEE;
  calculation_parameters_t params = getParameters(method);

  coordinates_t coordinates = {35.7750, -78.6336};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  char destString[9];

  get_local_str_time(prayerTimes.fajr, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:48 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "America/New_York");
  ASSERT_STREQ(destString, "07:16 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "America/New_York");
  ASSERT_STREQ(destString, "12:33 PM");

  get_local_str_time(prayerTimes.asr, destString, "America/New_York");
  ASSERT_STREQ(destString, "03:20 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "America/New_York");
  ASSERT_STREQ(destString, "05:43 PM");

  get_local_str_time(prayerTimes.isha, destString, "America/New_York");
  ASSERT_STREQ(destString, "07:04 PM");
}

TEST(PrayerTimesTest, testMoonsightingMethodHighLat) {
  time_t date = get_utc_date(2016, 1, 1);
  calculation_method method = MOON_SIGHTING_COMMITTEE;
  calculation_parameters_t params = getParameters(method);
  params.madhab = HANAFI;

  coordinates_t coordinates = {59.9094, 10.7349};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  char destString[9];

  get_local_str_time(prayerTimes.fajr, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "07:49 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "09:19 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "12:25 PM");

  get_local_str_time(prayerTimes.asr, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "01:36 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "03:25 PM");

  get_local_str_time(prayerTimes.isha, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "05:01 PM");
}

TEST(PrayerTimesTest, testTimeForPrayer) {
  time_t date = get_utc_date(2016, 7, 1);
  calculation_method method = MUSLIM_WORLD_LEAGUE;
  calculation_parameters_t params = getParameters(method);
  params.madhab = HANAFI;
  params.highLatitudeRule = TWILIGHT_ANGLE;

  coordinates_t coordinates = {59.9094, 10.7349};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  putenv((char *)"TZ=Europe/Oslo");

  ASSERT_EQ(prayerTimes.fajr, timeForPrayer(&prayerTimes, FAJR));
  ASSERT_EQ(prayerTimes.sunrise, timeForPrayer(&prayerTimes, SUNRISE));
  ASSERT_EQ(prayerTimes.dhuhr, timeForPrayer(&prayerTimes, DHUHR));
  ASSERT_EQ(prayerTimes.asr, timeForPrayer(&prayerTimes, ASR));
  ASSERT_EQ(prayerTimes.maghrib, timeForPrayer(&prayerTimes, MAGHRIB));
  ASSERT_EQ(prayerTimes.isha, timeForPrayer(&prayerTimes, ISHA));
}

TEST(PrayerTimesTest, testCurrentPrayer) {
  time_t date = get_utc_date(2015, 9, 1);
  calculation_method method = KARACHI;
  calculation_parameters_t params = getParameters(method);
  params.madhab = HANAFI;
  params.highLatitudeRule = TWILIGHT_ANGLE;

  coordinates_t coordinates = {33.720817, 73.090032};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.fajr, -1)),
            NONE);
  ASSERT_EQ(currentPrayer(&prayerTimes, prayerTimes.fajr), FAJR);
  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.fajr, 1)),
            FAJR);
  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.sunrise, 1)),
            SUNRISE);
  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.dhuhr, 1)),
            DHUHR);
  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.asr, 1)), ASR);
  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.maghrib, 1)),
            MAGHRIB);
  ASSERT_EQ(currentPrayer(&prayerTimes, add_seconds(prayerTimes.isha, 1)),
            ISHA);
}

TEST(PrayerTimesTest, testNextPrayer) {
  time_t date = get_utc_date(2015, 9, 1);
  calculation_method method = KARACHI;
  calculation_parameters_t params = getParameters(method);
  params.madhab = HANAFI;
  params.highLatitudeRule = TWILIGHT_ANGLE;

  coordinates_t coordinates = {33.720817, 73.090032};
  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.fajr, -1)), FAJR);
  ASSERT_EQ(next_prayer(&prayerTimes, prayerTimes.fajr), SUNRISE);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.fajr, 1)),
            SUNRISE);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.sunrise, 1)),
            DHUHR);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.dhuhr, 1)), ASR);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.asr, 1)),
            MAGHRIB);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.maghrib, 1)),
            ISHA);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.isha, 1)),
            MIDNIGHT);
  ASSERT_EQ(next_prayer(&prayerTimes, add_seconds(prayerTimes.midnight, 1)),
            NONE);
}

TEST(PrayerTimesTest, testPrayerTimesInTimezone) {
  time_t date = get_utc_date(2016, 1, 1);
  calculation_method method = MOON_SIGHTING_COMMITTEE;
  calculation_parameters_t params = getParameters(method);
  params.madhab = HANAFI;

  coordinates_t coordinates = {59.9094, 10.7349};

  prayer_times_t prayerTimes = new_prayer_times(&coordinates, date, &params);

  char destString[9];

  get_local_str_time(prayerTimes.fajr, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "07:49 AM");

  get_local_str_time(prayerTimes.sunrise, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "09:19 AM");

  get_local_str_time(prayerTimes.dhuhr, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "12:25 PM");

  get_local_str_time(prayerTimes.asr, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "01:36 PM");

  get_local_str_time(prayerTimes.maghrib, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "03:25 PM");

  get_local_str_time(prayerTimes.isha, destString, "Europe/Oslo");
  ASSERT_STREQ(destString, "05:01 PM");
}

// Test robustness of prayer time calculation with normal coordinates
TEST(PrayerTimesTest, testCalculationRobustness) {
  // Test with normal coordinates (New York)
  coordinates_t coords = {40.7128, -74.0060};
  calculation_parameters_t params = getParameters(MUSLIM_WORLD_LEAGUE);

  // Test with current time
  time_t now =
      get_utc_date(2023, 7, 15); // Use a fixed date for reproducibility

  prayer_times_t prayer_times = new_prayer_times(&coords, now, &params);

  // Check if calculation succeeded (no times should be 0)
  ASSERT_NE(prayer_times.fajr, 0) << "Fajr calculation failed";
  ASSERT_NE(prayer_times.dhuhr, 0) << "Dhuhr calculation failed";
  ASSERT_NE(prayer_times.asr, 0) << "Asr calculation failed";
  ASSERT_NE(prayer_times.maghrib, 0) << "Maghrib calculation failed";
  ASSERT_NE(prayer_times.isha, 0) << "Isha calculation failed";
  ASSERT_NE(prayer_times.sunrise, 0) << "Sunrise calculation failed";
  ASSERT_NE(prayer_times.midnight, 0) << "Midnight calculation failed";

  // Verify times are in logical order
  ASSERT_LT(prayer_times.fajr, prayer_times.sunrise);
  ASSERT_LT(prayer_times.sunrise, prayer_times.dhuhr);
  ASSERT_LT(prayer_times.dhuhr, prayer_times.asr);
  ASSERT_LT(prayer_times.asr, prayer_times.maghrib);
  ASSERT_LT(prayer_times.maghrib, prayer_times.isha);
}

// Test edge cases with extreme coordinates that might cause division by zero
TEST(PrayerTimesTest, testExtremeCoordinates) {
  calculation_parameters_t params = getParameters(MUSLIM_WORLD_LEAGUE);
  time_t date = get_utc_date(2023, 7, 15);

  // Test very high latitude (near Arctic) - might not have normal
  // sunrise/sunset
  coordinates_t arctic_coords = {85.0, 0.0};
  prayer_times_t arctic_times = new_prayer_times(&arctic_coords, date, &params);

  // At extreme latitudes, the calculation might return zeros (which is valid),
  // but it should not crash due to division by zero
  // We just verify the function completes without crashing
  SUCCEED() << "Arctic coordinates handled without crashing";

  // Test coordinates at the poles
  coordinates_t north_pole = {90.0, 0.0};
  prayer_times_t north_pole_times =
      new_prayer_times(&north_pole, date, &params);
  SUCCEED() << "North pole coordinates handled without crashing";

  coordinates_t south_pole = {-90.0, 0.0};
  prayer_times_t south_pole_times =
      new_prayer_times(&south_pole, date, &params);
  SUCCEED() << "South pole coordinates handled without crashing";
}

// Test invalid coordinates are properly handled
TEST(PrayerTimesTest, testInvalidCoordinates) {
  calculation_parameters_t params = getParameters(MUSLIM_WORLD_LEAGUE);
  time_t date = get_utc_date(2023, 7, 15);

  // Test with invalid coordinates (out of range)
  coordinates_t invalid_coords = {200.0, 300.0}; // Invalid lat/lng
  prayer_times_t invalid_times =
      new_prayer_times(&invalid_coords, date, &params);

  // Should return NULL_PRAYER_TIMES (all zeros)
  ASSERT_EQ(invalid_times.fajr, 0);
  ASSERT_EQ(invalid_times.dhuhr, 0);
  ASSERT_EQ(invalid_times.asr, 0);
  ASSERT_EQ(invalid_times.maghrib, 0);
  ASSERT_EQ(invalid_times.isha, 0);
  ASSERT_EQ(invalid_times.sunrise, 0);
  ASSERT_EQ(invalid_times.midnight, 0);

  // Test with NULL coordinates
  prayer_times_t null_times = new_prayer_times(nullptr, date, &params);
  ASSERT_EQ(null_times.fajr, 0);
  ASSERT_EQ(null_times.dhuhr, 0);

  // Test with NULL parameters
  coordinates_t valid_coords = {40.7128, -74.0060};
  prayer_times_t null_param_times =
      new_prayer_times(&valid_coords, date, nullptr);
  ASSERT_EQ(null_param_times.fajr, 0);
  ASSERT_EQ(null_param_times.dhuhr, 0);
}

// Test calculation stability across different dates and locations
TEST(PrayerTimesTest, testCalculationStability) {
  calculation_parameters_t params = getParameters(MUSLIM_WORLD_LEAGUE);

  // Test various locations around the world (avoiding extreme latitudes)
  coordinates_t locations[] = {
      {40.7128, -74.0060},  // New York
      {35.6762, 139.6503},  // Tokyo
      {-33.8688, 151.2093}, // Sydney
      {25.2048, 55.2708},   // Dubai
      {21.3099, -157.8581}, // Honolulu (corrected longitude)
  };

  int num_locations = sizeof(locations) / sizeof(locations[0]);

  // Test different times of year (avoiding extreme solstice dates)
  time_t test_dates[] = {
      get_utc_date(2023, 3, 21), // Spring equinox
      get_utc_date(2023, 9, 23), // Fall equinox
      get_utc_date(2023, 1, 15), // Mid winter
      get_utc_date(2023, 7, 15), // Mid summer
  };

  int num_dates = sizeof(test_dates) / sizeof(test_dates[0]);

  for (int i = 0; i < num_locations; i++) {
    for (int j = 0; j < num_dates; j++) {
      prayer_times_t times =
          new_prayer_times(&locations[i], test_dates[j], &params);

      // For most reasonable locations and dates, we should get valid prayer
      // times The main goal is to ensure calculations don't crash due to
      // division by zero
      if (times.dhuhr != 0) { // If we got a valid calculation
        // Verify basic ordering constraints when times are valid
        if (times.sunrise != 0 && times.dhuhr != 0) {
          ASSERT_LT(times.sunrise, times.dhuhr)
              << "Sunrise should be before dhuhr at location " << i << ", date "
              << j;
        }
        if (times.asr != 0 && times.dhuhr != 0) {
          ASSERT_LT(times.dhuhr, times.asr)
              << "Dhuhr should be before asr at location " << i << ", date "
              << j;
        }
      }
    }
  }
}
