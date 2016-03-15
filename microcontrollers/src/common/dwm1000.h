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

#define EXTENDED_SUB 0b10000000
#define NORMAL_SUB 0b00000000

// SPI General Constants
#define BLANK 0x00

// Registers
#define DEVICE_ID_ADDR 0x00
#define NETWORK_ADDR_ADDR 0x03

// Sub-addresses
#define ADDR_SUB 0x00
#define NETWORK_ID_SUB 0x02

void DW_init(int selectPin);

void DW_getDevID(byte* devId);
unsigned int DW_getAddr();
unsigned int DW_getNetworkId();

void DW_setAddr(unsigned int addr);
void DW_setNetworkId(unsigned int id);

void _readRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n);
void _writeRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n);
int _makeHeader(byte RW, byte addr, bool isOffset, unsigned int offset, byte* header);

#endif