#include "dwm1000.h"

int _selectPin;

void DW_init(int selectPin) {
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	_selectPin = selectPin;
	pinMode(selectPin, OUTPUT);
}

void DW_getDevID(byte* devId) {
		_readRegister(DEVICE_ID, 0, devId, 4);
}

void _readRegister(byte addr, byte offset, byte* data, unsigned int n) {
	byte header[3];
	int headerLen = 1;

	if (offset == 0) {
		header[0] = READ | NO_SUB;
	} else {
		header[0] = READ | SUB;
		// TODO More stuff with offsets
	}

	SPI.begin();
	digitalWrite(_selectPin, LOW);
	for (int i = 0; i < headerLen; ++i) {
		SPI.transfer(header[i]);
	}
	for (unsigned int i = 0; i < n; ++i) {
		data[i] = SPI.transfer(BLANK);
	}
	delayMicroseconds(10);
	digitalWrite(_selectPin, HIGH);
	SPI.end();
}
