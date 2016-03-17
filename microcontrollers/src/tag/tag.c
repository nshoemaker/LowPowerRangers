#include <Arduino.h>
#include <HardwareSerial.h>

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ_PIN 2

int main(void)
{
   init();
   Serial.begin(9600);
   DW_init(SELECT_PIN);
   byte devId[4];
   while (1) {
      DW_getDevID(devId);
      printBytes(devId, 4);

      DW_setAddr(CHIP_ADDR);
      DW_setNetworkId(NETWORK_ID);

      Serial.print("Address: ");
      Serial.println(DW_getAddr());
      Serial.print("Network: ");
      Serial.println(DW_getNetworkId());

      delay(500);
   }
   return 0;
}
