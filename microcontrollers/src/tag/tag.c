#include <Arduino.h>
#include <HardwareSerial.h>

#include "dwm1000.h"

#define SELECT_PIN 10

int main(void)
{
   init();
   Serial.begin(9600);
   DW_init(SELECT_PIN);
   byte devId[4];
   char tmp[3];
   while (1) {
      DW_getDevID(devId);
      for (int i = 3; i >= 0; --i) {
      	sprintf(tmp, "%.2X", devId[i]);
      	Serial.print(tmp);
      }
      Serial.println("");
      delay(500);
   }
   return 0;
}
