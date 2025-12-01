#include "shadow.h"

#ifndef ADHAN_MADHAB_H
#define ADHAN_MADHAB_H

typedef enum {
  /**
   * Shafi Madhab
   */
  SHAFI,

  /**
   * Hanafi Madhab
   */
  HANAFI
} madhab_t;

static inline shadow_length getShadowLength(madhab_t madhab) {
  switch (madhab) {
  case SHAFI:
    return SINGLE;
  case HANAFI:
    return DOUBLE;
  default:
    return SINGLE;
  }
}

static inline const char *get_madhab_name(madhab_t madhab) {
  switch (madhab) {
  case SHAFI:
    return (const char *)"Shafi";
  case HANAFI:
    return (const char *)"hanafi";
  default:
    return (const char *)"Unknow madhab";
  }
}
#endif // ADHAN_MADHAB_H
