#include "display.h"
#include "Local.h"
#include "Compass.h"

#include <SD.h>


#define LED1 27
#define LED2 30

#define BL_PIN 9


Local local;

Display display(local);
Compass compass(local);

int rssi1_timer = 0;
int rssi1;

int inbuffer1 [172];
int inbuffer2 [172];
int inbuffer3 [172];
int inbuffer4 [172];
int inbuffer5 [172];

int counter1 = 0;
int counter2 = 0;
int counter3 = 0;
int counter4 = 0;
int counter5 = 0;


void setup() {
  pinMode(BL_PIN,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(36, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(36), rssiISR_1, RISING);

  
  init_display();
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  Serial4.begin(115200);
  Serial5.begin(115200);
  SD.begin(BUILTIN_SDCARD);
  //compass.calibrate();
}

void loop() {
  //update_display();
  //local.update();
  //compass.update();
  while(Serial1.available()){
    inbuffer1[counter1] = Serial1.read();
    counter1++;
    if (counter1 == 172){
      File dataFile = SD.open("Log1.txt", FILE_WRITE);
      if (dataFile) {
        for(int i = 0; i < 172; i++) {
          dataFile.print(inbuffer1[i], HEX);
          dataFile.print(' ');
          //Serial.print(inbuffer1[i], HEX);
          //Serial.print(" ");
        }
        dataFile.println();
        dataFile.close();
        digitalWrite(LED1, !digitalRead(LED1));
        //Serial.println();
      }
      counter1 = 0;
    }
  }
  while(Serial2.available()){
    inbuffer2[counter2] = Serial2.read();
    counter2++;
    if (counter2 == 172){
      File dataFile = SD.open("Log2.txt", FILE_WRITE);
      if (dataFile) {
        for(int i = 0; i < 172; i++) {
          dataFile.print(inbuffer2[i], HEX);
          dataFile.print(' ');
          //Serial.print(inbuffer2[i], HEX);
          //Serial.print(" ");
        }
        dataFile.println();
        dataFile.close();
        //Serial.println();
      }
      counter2 = 0;
    }
  }
  while(Serial3.available()){
    inbuffer3[counter3] = Serial3.read();
    counter3++;
    if (counter3 == 172){
      File dataFile = SD.open("Log3.txt", FILE_WRITE);
      if (dataFile) {
        for(int i = 0; i < 172; i++) {
          dataFile.print(inbuffer3[i], HEX);
          dataFile.print(' ');
          //Serial.print(inbuffer3[i], HEX);
          //Serial.print(" ");
        }
        dataFile.println();
        dataFile.close();
        //Serial.println();
      }
      counter3 = 0;
    }
  }
  while(Serial4.available()){
    inbuffer4[counter4] = Serial4.read();
    counter4++;
    if (counter4 == 172){
      File dataFile = SD.open("Log4.txt", FILE_WRITE);
      if (dataFile) {
        for(int i = 0; i < 172; i++) {
          dataFile.print(inbuffer4[i], HEX);
          dataFile.print(' ');
          //Serial.print(inbuffer4[i], HEX);
          //Serial.print(" ");
        }
        dataFile.println();
        dataFile.close();
        //Serial.println();
      }
      counter4 = 0;
    }
  }
  while(Serial5.available()){
    inbuffer5[counter5] = Serial5.read();
    counter5++;
    if (counter5 == 172){
      File dataFile = SD.open("Log5.txt", FILE_WRITE);
      if (dataFile) {
        for(int i = 0; i < 172; i++) {
          dataFile.print(inbuffer5[i], HEX);
          dataFile.print(' ');
          //Serial.print(inbuffer5[i], HEX);
          //Serial.print(" ");
        }
        dataFile.println();
        dataFile.close();
        //Serial.println();
      }
      counter5 = 0;
    }
  }
}




void rssiISR_1() {
  rssi1 = micros() - rssi1_timer; 
  rssi1_timer = micros();
  Serial.println(rssi1);
}
