#ifndef LOCAL
#define LOCAL

#include <Arduino.h>
#include "Data.h"

class Local : public Data {
 public:
  Local();
  void update();

  void set_brightness(uint8_t h);
  void increment_time_zone();
  void decrement_time_zone();

  uint8_t get_brightness();

  uint8_t brightness;
  boolean dark;
  int8_t time_zone;

  uint32_t rf_strength[5];
  boolean sd_ready;
  uint32_t sd_space;
  uint32_t flash_space;

 private:
};

#endif
