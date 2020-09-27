#include "Local.h"
#include <Adafruit_GPS.h>
#include <EEPROM.h>

#define GPSSerial Serial7

Adafruit_GPS GPS(&GPSSerial);

Local::Local() {
  dark = EEPROM.read(0);
  brightness = EEPROM.read(1);
  time_zone = EEPROM.read(2);

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
}

uint8_t Local::get_brightness() { return brightness; }

void Local::set_brightness(uint8_t h) {
  EEPROM.write(1, h);
  brightness = h;
}

void Local::increment_time_zone() {
  time_zone++;
  EEPROM.write(2, time_zone);
}

void Local::decrement_time_zone() {
  time_zone--;
  EEPROM.write(2, time_zone);
}

void Local::update() {
  ts = millis();

  GPS.read();
  if (GPS.newNMEAreceived()) {
    GPS.parse(GPS.lastNMEA());
    gps.hour = GPS.hour + time_zone;
    if (gps.hour >= 24) gps.hour -= 24;
    gps.minute = GPS.minute;
    gps.second = GPS.seconds;
    gps.milliseconds = GPS.milliseconds;
    gps.day = GPS.day;
    gps.month = GPS.month;
    gps.year = GPS.year;
    if (GPS.fix) {
      gps.latitude = GPS.latitude_fixed / 10000000.0;
      gps.longitude = GPS.longitude_fixed / 10000000.0;
      gps.fixquality_3d = GPS.fixquality_3d;
      gps.satellites = GPS.satellites;
    }
  }
}
