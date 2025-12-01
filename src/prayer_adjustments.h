#ifndef ADHAN_PRAYER_ADJUSTMENTS_H
#define ADHAN_PRAYER_ADJUSTMENTS_H

typedef struct {
  int fajr;     /**< Fajr offset in minutes */
  int sunrise;  /**< Sunrise offset in minutes */
  int dhuhr;    /**< Dhuhr offset in minutes */
  int asr;      /**< Asr offset in minutes */
  int maghrib;  /**< Maghrib offset in minutes */
  int isha;     /**< Isha offset in minutes */
  int midnight; /**< Midnight offset in minutes */
} prayer_adjustments_t;

#define INIT_PRAYER_ADJUSTMENTS() ((prayer_adjustments_t){0, 0, 0, 0, 0, 0, 0})

#endif /* ADHAN_PRAYER_ADJUSTMENTS_H */
