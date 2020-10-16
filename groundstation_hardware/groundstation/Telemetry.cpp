#include "Telemetry.h"

Telemetry::Telemetry(HardwareSerial& Ser, uint32_t ind) {
  ser = Ser;
  index = ind;
}

void Telemetry::begin(){
  ser.begin(115200);
}

void Telemetry::update() {
  ts = millis();
  while(ser.available()){
    inbuffer[counter] = ser.read();
    counter++;
    if (counter == 172){
      File dataFile = SD.open("Log" + index + ".txt", FILE_WRITE);
      if (dataFile) {
        for(int i = 0; i < 172; i++) {
          dataFile.print(inbuffer[i], HEX);
          dataFile.print(' ');
          //Serial.print(inbuffer1[i], HEX);
          //Serial.print(" ");
        }
        dataFile.println();
        dataFile.close();
        digitalWrite(LED1, !digitalRead(LED1));
        //Serial.println();
      }
      counter = 0;
    }
  }
}
