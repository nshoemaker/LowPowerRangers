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
   int counter = 0;

   while (1) {
      if (!DW_isSending()) {
         DW_sendBroadcast((byte*)&counter, 2);
      }
      delay(500);
   }
   return 0;
}
