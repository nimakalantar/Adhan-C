#include <stdlib.h>
#include <time.h>

static time_t get_date(int year, int month, int day) {
  struct tm tmp = {0};
  tmp.tm_year = year - 1900;
  tmp.tm_mon = month - 1;
  tmp.tm_mday = day;
  return mktime(&tmp);
}

static time_t get_utc_date(int year, int month, int day) {
  struct tm tmp = {0};
  tmp.tm_year = year - 1900;
  tmp.tm_mon = month - 1;
  tmp.tm_mday = day;
  return timegm(&tmp);
}
