#include "Local.h"
#include "Compass.h"
#include <EEPROM.h>
#include <Wire.h>
#include "Adafruit_MLX90393.h"

Adafruit_MLX90393 sensor = Adafruit_MLX90393();

Compass::Compass(Local &local_ref) : local(local_ref) {
  int i;
  while (!sensor.begin()) {
    if (i == 10)
      continue;
    else
      i++;
    delay(100);
  }
  EEPROM.get(COMPASS_OFFSET, compassOffset.XAxis);
  EEPROM.get(COMPASS_OFFSET + sizeof(float), compassOffset.YAxis);
  EEPROM.get(COMPASS_OFFSET + 2 * sizeof(float), compassOffset.ZAxis);

  Serial.println(compassOffset.XAxis);
  Serial.println(compassOffset.YAxis);
  Serial.println(compassOffset.ZAxis);
}

void Compass::calibrate() {
  Serial.println("calibrate the compass");
  Magnetometer valueMax;
  Magnetometer valueMin;
  Magnetometer value;

  int xcount = 0;
  int ycount = 0;
  int zcount = 0;

  Serial.println("please rotate the compass");

  sensor.readData(&value.XAxis, &value.YAxis, &value.ZAxis);

  valueMax.XAxis = value.XAxis;
  valueMax.YAxis = value.YAxis;
  valueMax.ZAxis = value.YAxis;

  valueMin.XAxis = value.XAxis;
  valueMin.YAxis = value.YAxis;
  valueMin.ZAxis = value.YAxis;

  while (xcount < 1000 && ycount < 1000 && zcount < 1000) {
    sensor.readData(&value.XAxis, &value.YAxis, &value.ZAxis);

    if (valueMin.XAxis > value.XAxis) {
      xcount = 0;
      valueMin.XAxis = value.XAxis;
    } else if (valueMax.XAxis < value.XAxis) {
      xcount = 0;
      valueMax.XAxis = value.XAxis;
    } else
      xcount++;

    if (valueMin.YAxis > value.YAxis) {
      ycount = 0;
      valueMin.YAxis = value.YAxis;
    } else if (valueMax.YAxis < value.YAxis) {
      ycount = 0;
      valueMax.YAxis = value.YAxis;
    } else
      ycount++;

    if (valueMin.ZAxis > value.ZAxis) {
      zcount = 0;
      valueMin.ZAxis = value.ZAxis;
    } else if (valueMax.ZAxis < value.ZAxis) {
      zcount = 0;
      valueMax.ZAxis = value.ZAxis;
    } else
      zcount++;

    delay(10);
  }

  compassOffset.XAxis = (valueMax.XAxis + valueMin.XAxis) / 2;
  compassOffset.YAxis = (valueMax.YAxis + valueMin.YAxis) / 2;
  compassOffset.ZAxis = (valueMax.ZAxis + valueMin.ZAxis) / 2;

  EEPROM.put(COMPASS_OFFSET, compassOffset.XAxis);
  EEPROM.put(COMPASS_OFFSET + sizeof(float), compassOffset.YAxis);
  EEPROM.put(COMPASS_OFFSET + 2 * sizeof(float), compassOffset.ZAxis);

  Serial.println("Calibration Done!");
}

void Compass::update() {
  sensor.readData(&compassRaw.XAxis, &compassRaw.YAxis, &compassRaw.ZAxis);
  compassRaw.XAxis -= compassOffset.XAxis;
  compassRaw.YAxis -= compassOffset.YAxis;
  compassRaw.ZAxis -= compassOffset.ZAxis;
  filter(&compassFiltered, &compassRaw);
  local.magneto.heading = getHeading(&compassFiltered);
}

float Compass::getHeading(Magnetometer *c) {
  float heading = atan2(c->YAxis, c->XAxis);
  heading += declinationAngle;
  heading += PI / 2;
  if (heading < 0) heading += 2 * PI;
  if (heading > 2 * PI) heading -= 2 * PI;
  return (heading * 180 / M_PI);
}

void Compass::filter(Magnetometer *filtered, Magnetometer *raw) {
  filtered->XAxis = filterAxis(raw->XAxis, registersX);
  filtered->YAxis = filterAxis(raw->YAxis, registersY);
  filtered->ZAxis = filterAxis(raw->ZAxis, registersZ);
}

float Compass::filterAxis(float value, double *registers) {
  float filtered;
  filtered = A0 * (value + B1const * registers[0] + B2const * registers[1] +
                   B3const * registers[2] + B4const * registers[3]) +
             A1const * registers[0] + A2const * registers[1] +
             A3const * registers[2] + A4const * registers[3];
  registers[3] = registers[2];
  registers[2] = registers[1];
  registers[1] = registers[0];
  registers[0] = value + B1const * registers[0] + B2const * registers[1] +
                 B3const * registers[2] + B4const * registers[3];
  return filtered;
}
