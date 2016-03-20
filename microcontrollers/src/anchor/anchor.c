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
   long status;

   while (1) {
   	_readRegister(STATUS_ADDR, false, 0, (byte*)&status, 4);
   	printBytes((byte*)&status, 4);

      if (!DW_isReceiving()) {
      	if (DW_receiveFailed()) {
      		Serial.println("Bad Message");
      	} else if (msgLen > 0) {
      		printBytes(msgData, msgLen);
      	}

      	DW_receiveMessage();
      }
      delay(500);
   }
   return 0;
}
