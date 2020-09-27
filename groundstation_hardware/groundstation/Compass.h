#ifndef COMPASS
#define COMPASS

#define COMPASS_OFFSET 20

#include <Arduino.h>
#include "Compass.h"

class Compass {
 public:
  Compass(Local& local_ref);
  void calibrate();
  void update();

 private:
  struct Magnetometer {
    float XAxis = 0;
    float YAxis = 0;
    float ZAxis = 0;
  };

  float filterAxis(float value, double* registers);
  void filter(Magnetometer* filtered, Magnetometer* raw);
  float getHeading(Magnetometer* c);

  Magnetometer compassOffset;
  Magnetometer compassRaw;
  Magnetometer compassFiltered;

  float declinationAngle = 0.050905442;

  // filter const
  const double B1const = 2.4000;
  const double B2const = -2.1600;
  const double B3const = 0.8640;
  const double B4const = -0.1296;
  const double A0const = 0.0016;
  const double A1const = 0.0064;
  const double A2const = 0.0096;
  const double A3const = 0.0064;
  const double A4const = 0.0016;

  double registersX[4] = {0};
  double registersY[4] = {0};
  double registersZ[4] = {0};

  Local& local;
};

#endif
