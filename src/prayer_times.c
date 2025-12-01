#include "prayer_times.h"
#include "calculation_parameters.h"
#include "calendrical_helper.h"
#include "solar_time.h"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

static time_t time_from_double(double value, time_t date) {
  // Check for invalid double values (NaN, infinity, or extreme values)
  if (value != value || value == DBL_MAX || value == DBL_MIN ||
      !isfinite(value)) {
    return 0;
  }

  // Allow negative values and values >= 24.0 as they can represent times in
  // adjacent days Only reject completely unreasonable values
  if (value < -24.0 || value > 48.0) {
    return 0;
  }

  time_t day = date_from_time(date);
  if (day == 0) {
    return 0; // date_from_time failed
  }

  int hours = (int)floor(value);
  double minutes_double = (value - hours) * 60.0;
  int minutes = (int)round(minutes_double);

  // Handle negative hours (previous day)
  if (hours < 0) {
    day = add_days(day, -1);
    hours += 24;
  }

  // Handle hours >= 24 (next day)
  if (hours >= 24) {
    day = add_days(day, 1);
    hours -= 24;
  }

  // Handle minute overflow
  if (minutes >= 60) {
    hours += 1;
    minutes = 0;
    if (hours >= 24) {
      day = add_days(day, 1);
      hours = 0;
    }
  }

  // Handle negative minutes
  if (minutes < 0) {
    hours -= 1;
    minutes += 60;
    if (hours < 0) {
      day = add_days(day, -1);
      hours = 23;
    }
  }

  return add_seconds(add_minutes(add_hours(day, hours), minutes), 0);
}

static bool validate_coordinates(const coordinates_t *coordinates) {
  if (!coordinates)
    return false;
  return (coordinates->latitude >= -90.0 && coordinates->latitude <= 90.0 &&
          coordinates->longitude >= -180.0 && coordinates->longitude <= 180.0);
}

prayer_times_t new_prayer_times(coordinates_t *coordinates, time_t date,
                                calculation_parameters_t *parameters) {
  if (!validate_coordinates(coordinates) || !parameters) {
    return (prayer_times_t)NULL_PRAYER_TIMES;
  }

  time_t tempFajr = 0;
  time_t tempSunrise = 0;
  time_t tempDhuhr = 0;
  time_t tempAsr = 0;
  time_t tempMaghrib = 0;
  time_t tempIsha = 0;
  time_t tempMidnight = 0;

  struct tm *tm_date = gmtime(&date);
  const int year = tm_date->tm_year + 1900;
  const int dayOfYear = tm_date->tm_yday + 1;

  solar_time_t solar_time = new_solar_time(date, coordinates);

  time_t transit = time_from_double(solar_time.transit, date);
  time_t sunriseComponents = time_from_double(solar_time.sunrise, date);
  time_t sunsetComponents = time_from_double(solar_time.sunset, date);

  bool error =
      (transit == 0 || sunriseComponents == 0 || sunsetComponents == 0);

  if (!error) {
    tempDhuhr = transit;
    tempSunrise = sunriseComponents;
    tempMaghrib = sunsetComponents;

    time_t asr_time = time_from_double(
        afternoon(&solar_time, getShadowLength(parameters->madhab)), date);
    if (asr_time != 0) {
      tempAsr = asr_time;
    } else {
      error = true; // Asr calculation failed
    }

    // Use the new function for Fajr calculation
    tempFajr = calculate_fajr_time(coordinates, date, parameters);
    if (tempFajr == 0) {
      error = true; // Fajr calculation failed
    }

    // Isha calculation with check against safe value
    if (parameters->ishaInterval > 0) {
      tempIsha = add_minutes(tempMaghrib, parameters->ishaInterval);
    } else {
      time_t isha_time = time_from_double(
          hour_angle(&solar_time, -parameters->ishaAngle, true), date);
      if (isha_time != 0) {
        tempIsha = isha_time;
      }

      if (parameters->method == MOON_SIGHTING_COMMITTEE &&
          coordinates->latitude >= 55) {
        long night_length =
            (add_days(sunriseComponents, 1) - sunsetComponents) / 60;
        tempIsha = add_minutes(sunsetComponents, night_length * 0.4);
      }

      const night_portions_t nightPortions = get_night_portions(parameters);

      time_t safeIsha;
      if (parameters->method == MOON_SIGHTING_COMMITTEE) {
        safeIsha = seasonAdjustedEveningTwilight(
            coordinates->latitude, dayOfYear, year, sunsetComponents);
      } else {
        long night = add_days(sunriseComponents, 1) - sunsetComponents;
        long portion = (long)(nightPortions.isha * night);
        safeIsha = add_seconds(sunsetComponents, portion);
      }

      if (!tempIsha || difftime(tempIsha, safeIsha) > 0) {
        tempIsha = safeIsha;
      }
    }
  }

  // Midnight calculation - halfway between maghrib and next day's fajr
  if (!error && tempMaghrib > 0) {
    struct tm date_tm = *gmtime(&date);
    date_tm.tm_mday += 1;
    // Use portable UTC conversion instead of mktime (which assumes local time)
    time_t temp_result = mktime(&date_tm);
    struct tm *gmt = gmtime(&temp_result);
    time_t offset = mktime(gmt) - temp_result;
    time_t next_date = temp_result - offset;

    if (next_date > 0) {
      time_t tomorrowFajr =
          calculate_fajr_time(coordinates, next_date, parameters);

      if (tomorrowFajr > 0) {
        time_t adjusted_maghrib =
            add_minutes(tempMaghrib, parameters->adjustments.maghrib);
        double midnight_seconds =
            ((double)adjusted_maghrib + (double)tomorrowFajr) / 2.0;

        // Validate the calculated midnight time
        if (isfinite(midnight_seconds) && midnight_seconds > 0) {
          tempMidnight = (time_t)midnight_seconds;
        } else {
          // Fallback: set midnight to 6 hours after maghrib
          tempMidnight = add_hours(tempMaghrib, 6);
        }
      } else {
        // Fallback if tomorrow's fajr calculation fails
        tempMidnight = add_hours(tempMaghrib, 6);
      }
    } else {
      // Fallback if date calculation fails
      tempMidnight = add_hours(tempMaghrib, 6);
    }
  }

  // Final validation - ensure we have all required prayer times
  if (error || !tempFajr || !tempSunrise || !tempDhuhr || !tempAsr ||
      !tempMaghrib || !tempIsha || !tempMidnight) {
    // if we don't have all prayer times then initialization failed
    return (prayer_times_t)NULL_PRAYER_TIMES;
  } else {
    return (prayer_times_t){
        add_minutes(tempFajr, parameters->adjustments.fajr),
        add_minutes(tempSunrise, parameters->adjustments.sunrise),
        add_minutes(tempDhuhr, parameters->adjustments.dhuhr),
        add_minutes(tempAsr, parameters->adjustments.asr),
        add_minutes(tempMaghrib, parameters->adjustments.maghrib),
        add_minutes(tempIsha, parameters->adjustments.isha),
        add_minutes(tempMidnight, parameters->adjustments.midnight)};
  }
}

prayer_t currentPrayer(prayer_times_t *prayer_times, time_t when) {
  if (prayer_times->midnight - when <= 0) {
    return MIDNIGHT;
  } else if (prayer_times->isha - when <= 0) {
    return ISHA;
  } else if (prayer_times->maghrib - when <= 0) {
    return MAGHRIB;
  } else if (prayer_times->asr - when <= 0) {
    return ASR;
  } else if (prayer_times->dhuhr - when <= 0) {
    return DHUHR;
  } else if (prayer_times->sunrise - when <= 0) {
    return SUNRISE;
  } else if (prayer_times->fajr - when <= 0) {
    return FAJR;
  } else {
    return NONE;
  }
}

prayer_t next_prayer(prayer_times_t *prayer_times, time_t when) {
  if (prayer_times->midnight - when <= 0) {
    return NONE;
  } else if (prayer_times->isha - when <= 0) {
    return MIDNIGHT;
  } else if (prayer_times->maghrib - when <= 0) {
    return ISHA;
  } else if (prayer_times->asr - when <= 0) {
    return MAGHRIB;
  } else if (prayer_times->dhuhr - when <= 0) {
    return ASR;
  } else if (prayer_times->sunrise - when <= 0) {
    return DHUHR;
  } else if (prayer_times->fajr - when <= 0) {
    return SUNRISE;
  } else {
    return FAJR;
  }
}

time_t timeForPrayer(prayer_times_t *prayer_times, prayer_t prayer) {
  switch (prayer) {
  case FAJR:
    return prayer_times->fajr;
  case SUNRISE:
    return prayer_times->sunrise;
  case DHUHR:
    return prayer_times->dhuhr;
  case ASR:
    return prayer_times->asr;
  case MAGHRIB:
    return prayer_times->maghrib;
  case ISHA:
    return prayer_times->isha;
  case MIDNIGHT:
    return prayer_times->midnight;
  case NONE:
    return 0;
  default:
    return 0;
  }
}

time_t seasonAdjustedMorningTwilight(double latitude, int day, int year,
                                     time_t sunrise) {
  const double a = 75 + ((28.65 / 55.0) * fabs(latitude));
  const double b = 75 + ((19.44 / 55.0) * fabs(latitude));
  const double c = 75 + ((32.74 / 55.0) * fabs(latitude));
  const double d = 75 + ((48.10 / 55.0) * fabs(latitude));

  double adjustment;
  const int dyy = daysSinceSolstice(day, year, latitude);
  if (dyy < 91) {
    adjustment = a + (b - a) / 91.0 * dyy;
  } else if (dyy < 137) {
    adjustment = b + (c - b) / 46.0 * (dyy - 91);
  } else if (dyy < 183) {
    adjustment = c + (d - c) / 46.0 * (dyy - 137);
  } else if (dyy < 229) {
    adjustment = d + (c - d) / 46.0 * (dyy - 183);
  } else if (dyy < 275) {
    adjustment = c + (b - c) / 46.0 * (dyy - 229);
  } else {
    adjustment = b + (a - b) / 91.0 * (dyy - 275);
  }

  return add_seconds(sunrise, -(int)round(adjustment * 60.0));
}

time_t seasonAdjustedEveningTwilight(double latitude, int day, int year,
                                     time_t sunset) {
  const double a = 75 + ((25.60 / 55.0) * fabs(latitude));
  const double b = 75 + ((2.050 / 55.0) * fabs(latitude));
  const double c = 75 - ((9.210 / 55.0) * fabs(latitude));
  const double d = 75 + ((6.140 / 55.0) * fabs(latitude));

  double adjustment;
  int dyy = daysSinceSolstice(day, year, latitude);
  if (dyy < 91) {
    adjustment = a + (b - a) / 91.0 * dyy;
  } else if (dyy < 137) {
    adjustment = b + (c - b) / 46.0 * (dyy - 91);
  } else if (dyy < 183) {
    adjustment = c + (d - c) / 46.0 * (dyy - 137);
  } else if (dyy < 229) {
    adjustment = d + (c - d) / 46.0 * (dyy - 183);
  } else if (dyy < 275) {
    adjustment = c + (b - c) / 46.0 * (dyy - 229);
  } else {
    adjustment = b + (a - b) / 91.0 * (dyy - 275);
  }

  return add_seconds(sunset, (int)round(adjustment * 60.0));
}

int daysSinceSolstice(int dayOfYear, int year, double latitude) {
  int daysSinceSolistice;
  const bool isLeapYear = is_leap_year(year);
  const int northernOffset = 10;
  const int southernOffset = isLeapYear ? 173 : 172;
  const int daysInYear = isLeapYear ? 366 : 365;

  if (latitude >= 0) {
    daysSinceSolistice = dayOfYear + northernOffset;
    if (daysSinceSolistice >= daysInYear) {
      daysSinceSolistice = daysSinceSolistice - daysInYear;
    }
  } else {
    daysSinceSolistice = dayOfYear - southernOffset;
    if (daysSinceSolistice < 0) {
      daysSinceSolistice = daysSinceSolistice + daysInYear;
    }
  }
  return daysSinceSolistice;
}

time_t calculate_fajr_time(coordinates_t *coordinates, time_t date,
                           calculation_parameters_t *parameters) {
  struct tm *tm_date = gmtime(&date);
  const int year = tm_date->tm_year + 1900;
  const int dayOfYear = tm_date->tm_yday + 1;

  solar_time_t solar_time = new_solar_time(date, coordinates);

  time_t sunriseComponents = time_from_double(solar_time.sunrise, date);
  time_t sunsetComponents = time_from_double(solar_time.sunset, date);

  bool error = (sunriseComponents == 0 || sunsetComponents == 0);

  if (error)
    return 0;

  // get night length
  time_t tomorrowSunrise = add_days(sunriseComponents, 1);
  long night = tomorrowSunrise - sunsetComponents;

  time_t fajr_time = time_from_double(
      hour_angle(&solar_time, -parameters->fajrAngle, false), date);

  if (parameters->method == MOON_SIGHTING_COMMITTEE &&
      coordinates->latitude >= 55) {
    fajr_time = add_seconds(sunriseComponents, -90 * 60);
  }

  const night_portions_t nightPortions = get_night_portions(parameters);

  time_t safeFajr;
  if (parameters->method == MOON_SIGHTING_COMMITTEE) {
    safeFajr = seasonAdjustedMorningTwilight(coordinates->latitude, dayOfYear,
                                             year, sunriseComponents);
  } else {
    long portion = (long)(nightPortions.fajr * night);
    safeFajr = add_seconds(sunriseComponents, -portion);
  }

  // Use safeFajr if fajr_time is invalid, or if fajr_time is after sunrise
  if (!fajr_time || difftime(fajr_time, sunriseComponents) > 0) {
    fajr_time = safeFajr;
  }

  return fajr_time;
}
