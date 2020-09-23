#include "display.h"
#include "Local.h"
#include "Compass.h"


#define LED1 27
#define LED2 30

#define BL_PIN 9

int counter;
Local local;

Display display(local);
Compass compass(local);


void setup() {
  pinMode(BL_PIN,OUTPUT);
  init_display();
  //compass.calibrate();
}

void loop() {
  update_display();
  delay(1);
  local.update();
  compass.update();
}
