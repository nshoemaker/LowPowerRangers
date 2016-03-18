#include "dwm1000.h"

int _makeHeader(byte RW, byte addr, bool isOffset, unsigned int offset, byte* header);
void _readRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n);
void _writeRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n);
void _setTxConfig(unsigned int msg_len);
void _valToBytes(long val, byte* bytes, int n);
void _setNetworkAddr(unsigned int networkId, unsigned int addr);
void _setAntennaDelays();
void _setChannel();
void _setTransmitPower();
void _setWeirdRegisters();
void _setInterruptMasks();
void _handleInterrupt();
void _sendMessage(byte* data, int len, int destination, int network);

byte msgData[MSG_LEN];

int _selectPin;
int _networkId;
int _addr;

void DW_init(int selectPin, int irq, int networkId, int address) {
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	_networkId = networkId;
	_addr = address;

	_selectPin = selectPin;
	attachInterrupt(irq, _handleInterrupt, RISING);
	pinMode(selectPin, OUTPUT);
	// First SPI interaction is always garabge, for some reason
	DW_getAddr();
	delay(100);

	long settings = CONFIG_SETTINGS;
	_writeRegister(SYS_CONFIG_ADDR, false, 0, (byte*)&settings, 4);

	_setInterruptMasks();
	_setNetworkAddr(networkId, address);
	_setAntennaDelays();
	_setChannel();
	_setTransmitPower();
	_setWeirdRegisters();
}

void DW_getDevID(byte* devId) {
	_readRegister(DEVICE_ID_ADDR, false, 0, devId, 4);
}

void _setNetworkAddr(unsigned int networkId, unsigned int addr) {
	byte data[4];
	_valToBytes(addr, data, 2);
	_valToBytes(networkId, data + 2, 2);
	_writeRegister(NETWORK_ADDR_ADDR, false, 0, data, 4);
}

unsigned int DW_getAddr() {
	int addr;
	_readRegister(NETWORK_ADDR_ADDR, true, ADDR_SUB, (byte*)&addr, 2);
	return addr;
}

unsigned int DW_getNetworkId() {
	int id;
	_readRegister(NETWORK_ADDR_ADDR, true, NETWORK_ID_SUB, (byte*)&id, 2);
	return id;
}

void _setTxConfig(unsigned int msg_len) {
	byte config[5];
	long modifiedSettings = TX_SETTINGS | (unsigned long)msg_len;
	_valToBytes(modifiedSettings, config, 4);
	config[4] = IFS_DELAY;
	_writeRegister(TX_CONFIG_ADDR, false, 0, config, 5);
}

void _valToBytes(long val, byte* bytes, int n) {
	for (int i = 0; i < n; ++i) {
		bytes[i] = ((val >> (i * 8)) & 0xFF);
	}
}

void _setAntennaDelays() {
	int delay = ANT_DELAY;
	_writeRegister(TX_ANT_DELAY_ADDR, false, 0, (byte*)&delay, 2);
	_writeRegister(LDE_ADDR, true, RX_ANT_DELAY_SUB, (byte*)&delay, 2);
}

void _setTransmitPower() {
	long power = TX_POWER;
	_writeRegister(TX_POWER_ADDR, false, 0, (byte*)&power, 4);
}

void _setChannel() {
	long channel = CHANNEL_SETTINGS;
	_writeRegister(CHANNEL_ADDR, false, 0, (byte*)&channel, 4);
}

void _setWeirdRegisters() {
	long data;
	
	data = AGC1;
	_writeRegister(AGC_ADDR, true, AGC1_SUB, (byte*)&data, 2);
	data = AGC2;
	_writeRegister(AGC_ADDR, true, AGC2_SUB, (byte*)&data, 4);
	data = AGC3;
	_writeRegister(AGC_ADDR, true, AGC3_SUB, (byte*)&data, 2);

	data = DRX0B;
	_writeRegister(DRX_ADDR, true, DRX0B_SUB, (byte*)&data, 2);
	data = DRX1A;
	_writeRegister(DRX_ADDR, true, DRX1A_SUB, (byte*)&data, 2);
	data = DRX1B;
	_writeRegister(DRX_ADDR, true, DRX1B_SUB, (byte*)&data, 2);
	data = DRX2;
	_writeRegister(DRX_ADDR, true, DRX2_SUB, (byte*)&data, 4);
	data = DRX4;
	_writeRegister(DRX_ADDR, true, DRX4_SUB, (byte*)&data, 2);

	data = RFRX;
	_writeRegister(RF_CONF_ADDR, true, RFRX_SUB, (byte*)&data, 1);
	data = RFTX;
	_writeRegister(RF_CONF_ADDR, true, RFTX_SUB, (byte*)&data, 4);

	data = TC_PGDELAY;
	_writeRegister(TX_CAL_ADDR, true, TC_PGDELAY_SUB, (byte*)&data, 1);

	data = FS_PLL_CFG;
	_writeRegister(FS_CTRL_ADDR, true, FS_PLL_CFG_SUB, (byte*)&data, 4);
	data = FS_PLL_TUNE;
	_writeRegister(FS_CTRL_ADDR, true, FS_PLL_TUNE_SUB, (byte*)&data, 1);

	data = LDE2;
	_writeRegister(LDE_ADDR, true, LDE2_SUB, (byte*)&delay, 2);
	data = LDE_REPC;
	_writeRegister(LDE_ADDR, true, LDE_REPC_SUB, (byte*)&delay, 2);
}

void _setInterruptMasks() {
	byte data[5];
	long mask = 0;
	mask |= 1L << TX_DONE_BIT;
	mask |= 1L << RX_DONE_BIT;
	_valToBytes(mask, data, 4);
	data[4] = 0;
	_writeRegister(INTERRUPT_MASK_ADDR, false, 0, data, 5);
}

void _handleInterrupt() {
	byte status[5];
	_readRegister(STATUS_ADDR, false, 0, status, 5);
	Serial.print("Status: ");
	printBytes(status, 5);	
}

void DW_sendMessage(byte* data, int len, int destination) {
	_sendMessage(data, len, destination, _networkId);
}

void _sendMessage(byte* data, int len, int destination, int network) {
	// Generate the frame
	_valToBytes(FRAME_CONTROL, msgData, 2);
	msgData[SEQ_NUM_IND] = 0;
	_valToBytes(network, msgData + PAN_IND, 2);
	_valToBytes(destination, msgData + DEST_IND, 2);
	_valToBytes(_addr, msgData + SOURCE_IND, 2);
	memcpy(msgData + DATA_IND, data, len);

	_writeRegister(TX_BUFF_ADDR, false, 0, msgData, DATA_IND + len);
	_setTxConfig(DATA_IND + len);
	long control = 0;
	control |= 1L << TX_START_BIT;
	_writeRegister(CONTROL_ADDR, false, 0, (byte*)&control, 4);
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
}

void printBytes(byte* data, int n) {
	char tmp[3];
	for (int i = n - 1; i >= 0; --i) {
      	sprintf(tmp, "%.2X", data[i]);
      	Serial.print(tmp);
      }
      Serial.println();
}