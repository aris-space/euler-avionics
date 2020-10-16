#ifndef TELEMETRY
#define TELEMETRY

#include <Arduino.h>
#include <HardwareSerial.h>
#include "Data.h"

class Telemetry : public Data {
 public:
  Telemetry(HardwareSerial &Ser, uint32_t ind);
  void update();
  void begin();

 private:
  HardwareSerial ser;
  int32_t index;

  int inbuffer [172];
  int counter = 0;
};

#endif
