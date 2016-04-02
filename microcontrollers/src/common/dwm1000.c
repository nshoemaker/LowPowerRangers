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
void _sendMessage(byte* data, int len, int destination, int network, Timestamp* t);
void _softReset();
void _handleError();
void _getRxTimestamp(Timestamp* t);
void _getTxTimestamp(Timestamp* t);
void _reverseBytes(byte* b, int len);

byte msgData[MSG_LEN];
byte msgLen = 0;

int _selectPin;
int _networkId;
int _addr;
byte _irq;

void (*rxFailCallback)(void) = NULL;
void (*rxCallback)(Timestamp* t, byte* data, int len, int srcAddr) = NULL;
void (*txCallback)(Timestamp* t) = NULL;

void DW_init(int selectPin, int irq, int networkId, int address, unsigned int timeout) {
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	_irq = irq;
	_networkId = networkId;
	_addr = address;

	_selectPin = selectPin;
	attachInterrupt(irq, _handleInterrupt, RISING);
	pinMode(selectPin, OUTPUT);
	// First SPI interaction is always garabge, for some reason
	DW_getAddr();
	delay(100);

	_softReset();

	long settings = CONFIG_SETTINGS;
	if (timeout > 0) {
		_writeRegister(TO_ADDR, false, 0, (byte*)&timeout, 2);
		settings |= 1L << TO_ENABLE_BIT;
	}
	_writeRegister(SYS_CONFIG_ADDR, false, 0, (byte*)&settings, 4);

	_setInterruptMasks();
	_setNetworkAddr(networkId, address);
	_setAntennaDelays();
	_setChannel();
	_setTransmitPower();
	_setWeirdRegisters();

	long control = 0;
	control |= 1L << CANCEL_BIT;
	_writeRegister(CONTROL_ADDR, false, 0, (byte*)&control, 4);
	byte p[2];
	p[0] = 0x01;
	p[1] = 0x03;
	_writeRegister(PWR_MGMT_ADDR, true, PWR_MGMT0_SUB, p, 2);
	long d = OTP_CTRL;
	_writeRegister(OTP_ADDR, true, OTP_CTRL_SUB, (byte*)&d, 2);
	delay(10);
	p[0] = 0x00;
	p[1] = 0x02;
	_writeRegister(PWR_MGMT_ADDR, true, PWR_MGMT0_SUB, p, 2);
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
	long modifiedSettings = TX_SETTINGS | (unsigned long)(msg_len + 2);
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
	int d = ANT_DELAY;
	_writeRegister(TX_ANT_DELAY_ADDR, false, 0, (byte*)&d, 2);
	_writeRegister(LDE_ADDR, true, RX_ANT_DELAY_SUB, (byte*)&d, 2);
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
	_writeRegister(LDE_ADDR, true, LDE2_SUB, (byte*)&data, 2);
	data = LDE_REPC;
	_writeRegister(LDE_ADDR, true, LDE_REPC_SUB, (byte*)&data, 2);

	data = OTP_CTRL;
	_writeRegister(OTP_ADDR, true, OTP_CTRL_SUB, (byte*)&data, 2);	
}

void _setInterruptMasks() {
	byte data[4];
	long mask = 0;
	mask |= 1L << TX_DONE_BIT;
	mask |= 1L << RX_DONE_BIT;
	mask |= 1L << PREAMBLE_ERR_BIT;
	mask |= 1L << HEADER_ERR_BIT;
	mask |= 1L << RX_TO_BIT;
	_valToBytes(mask, data, 4);
	long status = 0;
	_writeRegister(STATUS_ADDR, false, 0, (byte*)&status, 4);
	_writeRegister(INTERRUPT_MASK_ADDR, false, 0, data, 4);
}

void _handleInterrupt() {
	long status;
	_readRegister(STATUS_ADDR, false, 0, (byte*)&status, 4);

	ts_puts("interrupt\r\n");
	printBytes((byte*)&status, 4);
	// TX_DONE
	if (status & (1L << TX_DONE_BIT)) {
		ts_puts("tx\r\n");
		if (txCallback) {
			Timestamp t;
			_getTxTimestamp(&t);
			txCallback(&t);
		}
	}
	// RX Timeout
	if (status & (1L << RX_TO_BIT)) {
		if (rxFailCallback) {
			rxFailCallback();
		}
		_handleError();
 	}
	if (status & (RX_ERRS)) {
		ts_puts("rx error\r\n");
		printBytes((byte*)&status, 4);
		_handleError();
		return;
	}
	if (status & (1L << RX_DONE_BIT)) {
		ts_puts("rx done\r\n");
		if (status & (1L << RX_VALID_BIT)) {
			_readRegister(RX_INFO_ADDR, true, RX_LEN_SUB, &msgLen, 1);
			msgLen &= 0x7f;
			_readRegister(RX_BUFF_ADDR, false, 0, msgData, msgLen);
			status = RX_BITS;
			_writeRegister(STATUS_ADDR, false, 0, (byte*)&status, 4);
			if (rxCallback) {
				Timestamp t;
				_getRxTimestamp(&t);
				int srcAddr = *((int*)(msgData + SOURCE_IND));
				int dataLen = msgLen - DATA_IND - 2;
				//_reverseBytes(msgData + DATA_IND, dataLen);
				rxCallback(&t, msgData + DATA_IND, dataLen, srcAddr);
			}
		} else {
			printBytes((byte*)&status, 4);
			_handleError();
		}
	}
}

void _handleError() {
	long status = RX_ERRS | (1L << RX_VALID_BIT);
	_writeRegister(STATUS_ADDR, false, 0, (byte*)&status, 4);
	_softReset();
	_readRegister(STATUS_ADDR, false, 0, (byte*)&status, 4);
	printBytes((byte*)&status, 4);

}

void DW_sendBroadcast(byte* data, int len, Timestamp* t) {
	_sendMessage(data, len, 0xFFFF, 0xFFFF, t);	
}

void DW_sendMessage(byte* data, int len, int destination, Timestamp* t) {
	_sendMessage(data, len, destination, _networkId, t);
}

void _sendMessage(byte* data, int len, int destination, int network, Timestamp* t) {
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
	if (t){
		control |= 1L << TX_DELAY_BIT;
		t->time &= 0x000000FFFFFFFE00;
		_writeRegister(DELAY_ADDR, false, 0, (byte*)&(t->time), 5);	
	}
	_writeRegister(CONTROL_ADDR, false, 0, (byte*)&control, 4);
}

void DW_receiveMessage(Timestamp* d) {
	msgLen = 0;
	long control = 0;

	if (d) {
		control |= 1L << RX_DELAY_BIT;
		d->time &= 0x000000FFFFFFFE00;
		_writeRegister(DELAY_ADDR, false, 0, (byte*)&(d->time), 5);	
	}
	control |= 1L << RX_START_BIT;
	_writeRegister(CONTROL_ADDR, false, 0, (byte*)&control, 4);
}

void _softReset() {
	byte data[4];
	_readRegister(PWR_MGMT_ADDR, true, PWR_MGMT0_SUB, data, 4);
	data[0] = 0x01;
	_writeRegister(PWR_MGMT_ADDR, true, PWR_MGMT0_SUB, data, 4);
	data[3] = 0xE0;
	_writeRegister(PWR_MGMT_ADDR, true, PWR_MGMT0_SUB, data, 4);
	data[0] = 0x00;
	data[3] = 0xF0;
	_writeRegister(PWR_MGMT_ADDR, true, PWR_MGMT0_SUB, data, 4);
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

void _getRxTimestamp(Timestamp* t) {
	t->time = 0;
	_readRegister(RX_TIMESTAMP_ADDR, true, TIMESTAMP_SUB, (byte*)&(t->time), 5);
}

void _getTxTimestamp(Timestamp* t) {
	t->time = 0;
	_readRegister(TX_TIMESTAMP_ADDR, true, TIMESTAMP_SUB, (byte*)&(t->time), 5);
}

void _readRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n) {
	byte header[3];
	int headerLen = _makeHeader(READ, addr, isOffset, offset, header);

	//detachInterrupt(_irq);
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
	//attachInterrupt(_irq, _handleInterrupt, RISING);
}

void _writeRegister(byte addr, bool isOffset, unsigned int offset, byte* data, unsigned int n) {
	byte header[3];
	int headerLen = _makeHeader(WRITE, addr, isOffset, offset, header);

	//detachInterrupt(_irq);
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
	//attachInterrupt(_irq, _handleInterrupt, RISING);
}

void printBytes(byte* data, int n) {
	char tmp[3];
	for (int i = n - 1; i >= 0; --i) {
      	sprintf(tmp, "%.2X", data[i]);
      	ts_puts(tmp);
      }
      ts_puts("\r\n");
}

void _reverseBytes(byte* b, int len) {
	byte temp;
	for (int i = 0; i < len/2; i++) {
		temp = b[i];
		b[i] = b[len - i - 1];
		b[len - i - 1] = temp;
	}
}

void DW_setSentCallback(void (*cb)(Timestamp*)) {
	txCallback = cb;
}

void DW_setReceivedCallback(void (*cb)(Timestamp*, byte*, int, int)) {
	rxCallback = cb;
}

void DW_setReceiveFailedCallback(void (*cb)(void)) {
	rxFailCallback = cb;
}

void printTime(Timestamp* t) {
	long temp = t->time / US_TO_TIMESTAMP;
	char tmp[8];
	sprintf(tmp, "%ld", temp);
	ts_puts(tmp);
	ts_puts("\r\n");
}

/*void setTime(Timestamp* t, long us) {
	t->time = (long long int) (us) * TEN_US_TO_TIMESTAMP;
	t->time /= 10;
}*/

void addTime(Timestamp* t1, Timestamp* t2) {
	t1->time += t2->time;
	t1->time %= 0xFFFFFFFFFF;
}

// Returns t1-t2 in t1
void timeDiff(Timestamp* t1, Timestamp* t2) {
	t1->time -= t2->time;
}