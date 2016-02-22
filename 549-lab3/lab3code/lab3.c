#include <Arduino.h>
#include <HardwareSerial.h>

#define NUM_PINS 3

#define RED_PIN 0
#define GREEN_PIN 1
#define BLUE_PIN 2

#define TEMP_PIN A0

int pins[NUM_PINS] = {9, 10, 3};

void setOnlyColor(int pin) {
   for (int i = 0; i < NUM_PINS; ++i) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], LOW);
   }
   if (pin < NUM_PINS && pin >= 0) {
      digitalWrite(pins[pin], HIGH);
   }
}

void setAutoColor(int temperature) {
   int minReading = 135;
   int maxReading = 155;
   float scale = ((float)(temperature - minReading)) / (maxReading - minReading);
   scale = (scale < 0.0) ? 0.0 : scale;
   scale = (scale > 1.0) ? 1.0 : scale;
   analogWrite(pins[RED_PIN], scale * 255);
   analogWrite(pins[BLUE_PIN], (1.0 - scale) * 255);
}

int main(void)
{
   char byteRead;
   int temperature;
   bool autoMode = false;
   init();
   Serial.begin(115200);
   setOnlyColor(-1);
   while (1) {
      temperature = analogRead(TEMP_PIN);
      Serial.println(temperature);
      if (Serial.available() > 0) {
         autoMode = false;
         byteRead = Serial.read();
         if (byteRead == 'r') {
            setOnlyColor(RED_PIN);
         } else if (byteRead == 'g') {
            setOnlyColor(GREEN_PIN);
         } else if (byteRead == 'b') {
            setOnlyColor(BLUE_PIN);
         } else if (byteRead == 'o') {
            setOnlyColor(-1);
         } else if (byteRead == 'a') {
            setOnlyColor(-1);
            autoMode = true;
         }
      }
      if (autoMode) {
         setAutoColor(temperature);
      }
      delay(100);
   }

   return 0;
}
