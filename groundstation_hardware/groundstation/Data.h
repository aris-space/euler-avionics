#ifndef DATA
#define DATA

#include <Arduino.h>

class Data {
 public:
  Data();

  struct Gps {
    uint8_t hour;           ///< GMT hours
    uint8_t minute;         ///< GMT minutes
    uint8_t second;         ///< GMT seconds
    uint16_t milliseconds;  ///< GMT milliseconds
    uint8_t year;           ///< GMT year
    uint8_t month;          ///< GMT month
    uint8_t day;            ///< GMT day

    double latitude;   ///< Fixed point latitude in degrees
    double longitude;  ///< Fixed point longitude in degrees

    uint8_t
        fixquality_3d;   ///< 3D fix quality (1, 3, 3 = Nofix, 2D fix, 3D fix)
    uint8_t satellites;  ///< Number of satellites in use
  };

  struct Baro {
    uint32_t pressure;
    int32_t temperature;
  };

  struct Accel {
    int32_t x;
    int32_t y;
    int32_t z;
  };

  struct Gyro {
    int32_t x;
    int32_t y;
    int32_t z;
  };

  struct Magneto {
    int32_t x;
    int32_t y;
    int32_t z;
    float heading;
  };

  struct Imu {
    struct Gyro gyro;
    struct Accel accel;
    struct Magneto magneto;
  };

  uint32_t ts;  // Timestep in ms
  struct Gps gps;
  struct Baro baro;
  struct Imu imu;
  struct Magneto magneto;

 private:
 protected:
};

#endif
