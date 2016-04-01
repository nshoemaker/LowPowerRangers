#include <Arduino.h>
#include <HardwareSerial.h>

#include "dwm1000.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define IRQ 0

bool receiving = false;

void rxFailCallback() {
	receiving = false;
	Serial.println("Bad Message!!!");
}

void rxCallback(Timestamp* t, byte* data, int len, int srcAddr) {
	receiving = false;
	int count = *((int*)data);
	Serial.print(srcAddr);
	Serial.print(" sent ");
	Serial.print(count);
	Serial.print(" at: ");
	printTime(t);
}

int main(void)
{
   init();
   Serial.begin(9600);
   DW_init(SELECT_PIN, IRQ, NETWORK_ID, CHIP_ADDR);
   DW_setReceivedCallback(&rxCallback);
   DW_setReceiveFailedCallback(&rxFailCallback);
   while (1) {
      if (!receiving) {
      	receiving = true;
      	DW_receiveMessage();
      }
      delay(500);
   }
   return 0;
}
