#ifndef TELEMETRY
#define TELEMETRY

#include <Arduino.h>
#include "Data.h"

class Telemetry : public Data {
  public:
    Telemetry();
    void update();
  private:
    int32_t altitude;
    int32_t speed;
};

#endif
