#include <Arduino.h>
#include "tinyserial.h"

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ 0

bool receiving = false;

void printInt(unsigned int n) {
	char tmp[5];
	sprintf(tmp, "%u", n);
	ts_puts(tmp);
}

void rxFailCallback() {
	receiving = false;
	ts_puts("Bad Message!!!\r\n");
}

void rxCallback(Timestamp* t, byte* data, int len, int srcAddr) {
	receiving = false;
	int count = *((int*)data);
	printInt(srcAddr);
	ts_puts(" sent ");
	printInt(count);
	ts_puts(" at: ");
	printTime(t);
}

int main(void)
{
   init();
   ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
   DW_init(SELECT_PIN, IRQ, NETWORK_ID, CHIP_ADDR);
   DW_setReceivedCallback(&rxCallback);
   DW_setReceiveFailedCallback(&rxFailCallback);
   while (1) {
      if (!receiving) {
      	receiving = true;
      	DW_receiveMessage();
      }
      delay(5);
   }
   return 0;
}
