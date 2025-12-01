#ifndef ADHAN_COORDINATES_H
#define ADHAN_COORDINATES_H

#include <stdbool.h>

/**
 * @brief Geographic coordinates structure
 */
typedef struct {
  double latitude;  /**< Latitude in degrees (-90 to 90) */
  double longitude; /**< Longitude in degrees (-180 to 180) */
} coordinates_t;

/**
 * @brief Initialize coordinates structure
 * @param[out] coords Pointer to coordinates structure to initialize
 * @param[in] latitude Latitude in degrees (-90 to 90)
 * @param[in] longitude Longitude in degrees (-180 to 180)
 * @return true if coordinates are valid, false otherwise
 */
static inline bool init_coordinates(coordinates_t *coords, double latitude,
                                    double longitude) {
  if (!coords || latitude < -90.0 || latitude > 90.0 || longitude < -180.0 ||
      longitude > 180.0) {
    return false;
  }
  coords->latitude = latitude;
  coords->longitude = longitude;
  return true;
}

#endif /* ADHAN_COORDINATES_H */
