#include <Arduino.h>
#include <HardwareSerial.h>

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ 0

int main(void)
{
   init();
   Serial.begin(9600);
   DW_init(SELECT_PIN, IRQ, NETWORK_ID, CHIP_ADDR);
   byte devId[4];
   DW_sendMessage(devId, 4, 0xFFFF);
   while (1) {
      DW_getDevID(devId);
      printBytes(devId, 4);

      Serial.print("Address: ");
      Serial.println(DW_getAddr());
      Serial.print("Network: ");
      Serial.println(DW_getNetworkId());

      delay(500);
   }
   return 0;
}
