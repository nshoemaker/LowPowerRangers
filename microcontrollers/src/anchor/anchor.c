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

   while (1) {
      if (!DW_isReceiving()) {
      	Serial.println("Receiving");
      	DW_receiveMessage();
      }
      delay(500);
   }
   return 0;
}
