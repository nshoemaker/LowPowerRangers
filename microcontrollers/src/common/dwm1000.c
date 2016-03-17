#include "dwm1000.h"

int _makeHeader(byte RW, byte addr, bool isOffset, unsigned int offset, byte* header);
void _readRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n);
void _writeRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n);
void _setTxConfig(unsigned int msg_len);

int _selectPin;

void DW_init(int selectPin) {
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	_selectPin = selectPin;
	pinMode(selectPin, OUTPUT);
	// First SPI interaction is always garabge, for some reason
	DW_getAddr();
	delay(100);

	long settings = CONFIG_SETTINGS;
	_writeRegister(SYS_CONFIG_ADDR, false, 0, (byte*)&settings, 4);

	_setTxConfig(0);
}

void DW_getDevID(byte* devId) {
	_readRegister(DEVICE_ID_ADDR, false, 0, devId, 4);
}

void DW_setAddr(unsigned int addr) {
	_writeRegister(NETWORK_ADDR_ADDR, true, ADDR_SUB, (byte*)&addr, 2);
}

unsigned int DW_getAddr() {
	int addr;
	_readRegister(NETWORK_ADDR_ADDR, true, ADDR_SUB, (byte*)&addr, 2);
	return addr;
}

void DW_setNetworkId(unsigned int id) {
	_writeRegister(NETWORK_ADDR_ADDR, true, NETWORK_ID_SUB, (byte*)&id, 2);
}

unsigned int DW_getNetworkId() {
	int id;
	_readRegister(NETWORK_ADDR_ADDR, true, NETWORK_ID_SUB, (byte*)&id, 2);
	return id;
}

void _setTxConfig(unsigned int msg_len) {
	byte config[5];
	*((unsigned long*)config) = TX_SETTINGS | (unsigned long)msg_len;
	config[4] = IFS_DELAY;
	_writeRegister(TX_CONFIG_ADDR, false, 0, config, 5);
}

int _makeHeader(byte RW, byte addr, bool isOffset, unsigned int offset, byte* header) {
	int headerLen = 1;

	if (!isOffset) {
		header[0] = RW | NO_SUB | addr;
	} else {
		headerLen++;
		header[0] = RW | SUB | addr;
		if (offset < 128) {
			header[1] = NORMAL_SUB | offset;
		} else {
			headerLen++;
			header[1] = EXTENDED_SUB | offset;
			header[2] = (byte)(offset >> 7);
		}
	}
	return headerLen;
}

void _readRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n) {
	byte header[3];
	int headerLen = _makeHeader(READ, addr, isOffset, offset, header);

	SPI.begin();
	digitalWrite(_selectPin, LOW);
	for (int i = 0; i < headerLen; ++i) {
		SPI.transfer(header[i]);
	}
	for (unsigned int i = 0; i < n; ++i) {
		data[i] = SPI.transfer(BLANK);
	}
	digitalWrite(_selectPin, HIGH);
	SPI.end();
	delayMicroseconds(50);
}

void _writeRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n) {
	byte header[3];
	int headerLen = _makeHeader(WRITE, addr, isOffset, offset, header);

	SPI.begin();
	digitalWrite(_selectPin, LOW);
	for (int i = 0; i < headerLen; ++i) {
		SPI.transfer(header[i]);
	}
	for (unsigned int i = 0; i < n; ++i) {
		SPI.transfer(data[i]);
	}
	digitalWrite(_selectPin, HIGH);
	SPI.end();
	delayMicroseconds(50);
}

void printBytes(byte* data, int n) {
	char tmp[3];
	for (int i = n - 1; i >= 0; --i) {
      	sprintf(tmp, "%.2X", data[i]);
      	Serial.print(tmp);
      }
      Serial.println();
}