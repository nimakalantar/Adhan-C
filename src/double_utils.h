#ifndef ADHAN_DOUBLE_UTILS_H
#define ADHAN_DOUBLE_UTILS_H

#include <math.h>

/**
 * @brief Normalize value to [0, max_value)
 */
static inline double normalize_with_bound(double value, double max_value) {
  return value - (max_value * floor(value / max_value));
}

/**
 * @brief Unwind angle to [0, 360)
 */
static inline double unwind_angle(double angle_degrees) {
  return normalize_with_bound(angle_degrees, 360.0);
}

/**
 * @brief Get closest angle in [-180, 180]
 */
static inline double closest_angle(double angle_degrees) {
  if (angle_degrees >= -180.0 && angle_degrees <= 180.0) {
    return angle_degrees;
  }
  return angle_degrees - (360.0 * round(angle_degrees / 360.0));
}

#endif /* ADHAN_DOUBLE_UTILS_H */
