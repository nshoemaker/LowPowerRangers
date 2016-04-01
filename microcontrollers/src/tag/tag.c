#include <Arduino.h>
//#include <HardwareSerial.h>

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ 0

bool sending = false;

void txCallback(Timestamp* t) {
   sending = false;
   //Serial.print("sent at: ");
   printTime(t);
}

int main(void)
{
   init();
   //Serial.begin(9600);
   DW_init(SELECT_PIN, IRQ, NETWORK_ID, CHIP_ADDR);
   DW_setSentCallback(&txCallback);
   int counter = 0;
   Timestamp t;
   setTime(&t, 0);
   Timestamp delayTime;
   setTime(&delayTime, 1000000);
   while (1) {
      if (!sending) {
         sending = true;
         DW_sendBroadcast((byte*)&counter, 2, &t);
         printTime(&t);
         addTime(&t, &delayTime);
         printTime(&t);
         counter++;
      }
      delay(500);
   }
   return 0;
}
