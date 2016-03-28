#include <Arduino.h>
#include <HardwareSerial.h>

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ 0

bool sending = false;

void txCallback(Timestamp* t) {
   sending = false;
   Serial.print("sent at: ");
   printBytes(t->time, 5);
}

int main(void)
{
   init();
   Serial.begin(9600);
   DW_init(SELECT_PIN, IRQ, NETWORK_ID, CHIP_ADDR);
   DW_setSentCallback(&txCallback);
   int counter = 0;

   while (1) {
      if (!sending) {
         sending = true;
         DW_sendBroadcast((byte*)&counter, 2);
         counter++;
      }
      delay(500);
   }
   return 0;
}
