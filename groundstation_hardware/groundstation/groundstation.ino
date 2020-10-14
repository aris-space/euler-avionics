#include "display.h"
#include "Local.h"
#include "Compass.h"
#include "Telemetry.h"

#include <SD.h>


#define LED1 27
#define LED2 30

#define BL_PIN 9


Local local;

Display display(local);
Compass compass(local);
Telemetry xbee1(Serial1, 1);

int rssi1_timer = 0;
int rssi1;


void setup() {
  pinMode(BL_PIN,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(36, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(36), rssiISR_1, RISING);

  
  init_display();
  xbee1.begin();
  Serial.begin(9600);
 
  Serial2.begin(115200);
  Serial3.begin(115200);
  Serial4.begin(115200);
  Serial5.begin(115200);
  SD.begin(BUILTIN_SDCARD);
  //compass.calibrate();
}

void loop() {
  update_display();
  local.update();
  compass.update();
  xbee1.update();
}




void rssiISR_1() {
  rssi1 = micros() - rssi1_timer; 
  rssi1_timer = micros();
  Serial.println(rssi1);
}
