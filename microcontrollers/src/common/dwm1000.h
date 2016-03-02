#ifndef DWM1000_H
#define DWM1000_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>

// SPI Header Values
#define READ 0b00000000
#define WRITE 0b10000000

#define SUB 0b01000000
#define NO_SUB 0b00000000

// SPI General Constants
#define BLANK 0x00

// Registers
#define DEVICE_ID 0x00

void DW_init(int selectPin);
void DW_getDevID(byte* devId);

void _readRegister(byte addr, byte offset, byte* data, unsigned int n);

#endif