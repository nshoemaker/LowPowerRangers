#include <Arduino.h>
#include <HardwareSerial.h>

#include "common_example.h"

int main(void)
{
   init();
   Serial.begin(9600);
   while (1) {
      Serial.println(getNumber());
      delay(500);
   }
   return 0;
}
