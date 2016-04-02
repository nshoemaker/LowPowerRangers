#include <Arduino.h>
#include "tinyserial.h"

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ 0

bool sending = false;

void txCallback(Timestamp* t) {
   sending = false;
   ts_puts("sent at: ");
   printTime(t);
}

int main(void)
{
   init();
   ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
   DW_init(SELECT_PIN, IRQ, NETWORK_ID, CHIP_ADDR, 0);
   DW_setSentCallback(&txCallback);
   int counter = 0;
   Timestamp t;
   t.time = 0;
   Timestamp delayTime;
   delayTime.time = 0xEE0980000;
   while (1) {
      if (!sending) {
         sending = true;
         DW_sendBroadcast((byte*)&counter, 2, &t);
         addTime(&t, &delayTime);
         counter++;
      }
      delay(500);
   }
   return 0;
}
