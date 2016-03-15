#include <Arduino.h>
#include <HardwareSerial.h>

#include "dwm1000.h"

#define NETWORK_ID 0

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
      DW_setAddr(CHIP_ADDR);
      Serial.print("Address: ");
      Serial.println(DW_getAddr());
      DW_setNetworkId(NETWORK_ID);
      Serial.print("Network: ");
      Serial.println(DW_getNetworkId());
      delay(500);
   }
   return 0;
}
