#ifndef DWM1000_H
#define DWM1000_H

#include "tinyserial.h"
#include <SPI.h>

struct Timestamp {
	long long time;
};

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
#define SYS_CONFIG_ADDR 0x04
#define SYS_TIME_ADDR 0x06
#define TX_CONFIG_ADDR 0x08
#define TX_BUFF_ADDR 0x09
#define DELAY_ADDR 0x0A
#define TO_ADDR 0x0C
#define CONTROL_ADDR 0x0D
#define INTERRUPT_MASK_ADDR 0x0E
#define STATUS_ADDR 0x0F
#define RX_INFO_ADDR 0x10
#define RX_BUFF_ADDR 0x11
#define RX_TIMESTAMP_ADDR 0x15
#define TX_TIMESTAMP_ADDR 0x17
#define TX_ANT_DELAY_ADDR 0x18
#define TX_POWER_ADDR 0x1E
#define CHANNEL_ADDR 0x1F
#define AGC_ADDR 0x23
#define DRX_ADDR 0x27
#define RF_CONF_ADDR 0x28
#define TX_CAL_ADDR 0x2A
#define FS_CTRL_ADDR 0x2B
#define OTP_ADDR 0x2D
#define LDE_ADDR 0x2E
#define PWR_MGMT_ADDR 0x36

// Sub-addresses
// NETWORK_ADDR_ADDR
#define ADDR_SUB 0x00
#define NETWORK_ID_SUB 0x02
// CONTROL_ADDR
#define TX_CONTROL_SUB 0x00
#define RX_CONTROL_SUB 0x01
// RX_INFO_ADDR
#define RX_LEN_SUB 0x00
// TX/RX_TIMESTAMP_ADDR
#define TIMESTAMP_SUB 0x00
// AGC_ADDR
#define AGC1_SUB 0x04
#define AGC2_SUB 0x0C
#define AGC3_SUB 0x12
// DRX_ADDR
#define DRX0B_SUB 0x02
#define DRX1A_SUB 0x04
#define DRX1B_SUB 0x06
#define DRX2_SUB 0x08
#define DRX4_SUB 0x26
// RF_CONF_ADDR
#define RFRX_SUB 0x0B
#define RFTX_SUB 0x0C
// TX_CAL_ADDR
#define TC_PGDELAY_SUB 0x0B
// FS_CTRL_ADDR
#define FS_PLL_CFG_SUB 0x07
#define FS_PLL_TUNE_SUB 0x0B
// LDE_ADDR
#define RX_ANT_DELAY_SUB 0x1804
#define LDE2_SUB 0x1806
#define LDE_REPC_SUB 0x2804
// PWR_MGMT_ADDR
#define PWR_MGMT0_SUB 0x00
//OTP_ADDR
#define OTP_CTRL_SUB 0x06

// Interrupt Bits
#define TX_DONE_BIT 7
#define RX_DONE_BIT 13
#define RX_VALID_BIT 14
#define RX_TO_BIT 17
#define CLOCK_ERR_BIT 25
#define PREAMBLE_ERR_BIT 26
#define HEADER_ERR_BIT 12
//				  3322 2222 2222 1111 1111 1100 0000 0000
//				  1098 7654 3210 9876 5432 1098 7654 3210

//				  0000 0110 0000 0111 0001 0000 0000 0000
#define RX_ERRS 0b00000110000001110001000000000000L
#define RX_BITS 0x0407FF00L

// System Control Bits
#define TX_START_BIT 1
#define TX_DELAY_BIT 2
#define CANCEL_BIT 6
#define RX_START_BIT 8
#define RX_DELAY_BIT 9

// Configuration
#define CONFIG_SETTINGS 0b00100000000000000001011111111101L
#define TX_SETTINGS 0b00000000000101101100000000000000L
#define IFS_DELAY 0b00000000
#define ANT_DELAY 16384
#define TX_POWER 0x15355575L
#define CHANNEL_SETTINGS 0b01001010010010000000000000100010L
#define AGC1 0x889B
#define AGC2 0X2502A907L
#define AGC3 0x0035
#define DRX0B 0x0001
#define DRX1A 0x008D
#define DRX1B 0x0020
#define DRX2 0x313B006BL
#define DRX4 0x0028
#define RFRX 0xD8
#define RFTX 0x00045CA0L
#define TC_PGDELAY 0xC2
#define FS_PLL_CFG 0x08400508L
#define FS_PLL_TUNE 0x26
#define LDE2 0x0607
#define LDE_REPC 0x28F4
#define OTP_CTRL 0x8000

// Message Constants
#define FRAME_CONTROL 0b1000100001000001
#define MSG_LEN 127
#define SEQ_NUM_IND 2
#define PAN_IND 3
#define DEST_IND 5
#define SOURCE_IND 7
#define DATA_IND 9

// Misc
#define TO_ENABLE_BIT 28

// Timestamp Stuff
#define US_TO_TIMESTAMP 63897.6

// Misc
#define TO_ENABLE_BIT 28

extern byte msgData[MSG_LEN];
extern byte msgLen;

void printBytes(byte* data, int n);

void DW_init(int selectPin, int irq, int networkId, int address, unsigned int timeout);

void DW_getDevID(byte* devId);
unsigned int DW_getAddr();
unsigned int DW_getNetworkId();

void DW_sendMessage(byte* data, int len, int destination, Timestamp* t);
void DW_sendBroadcast(byte* data, int len, Timestamp* t);

void DW_receiveMessage(Timestamp* d);

void DW_setSentCallback(void (*cb)(Timestamp*));
void DW_setReceivedCallback(void (*cb)(Timestamp*, byte*, int, int));
void DW_setReceiveFailedCallback(void (*cb)(void));

// Timestamp functions
void printTime(Timestamp* t);
//void setTime(Timestamp* t, long us);
void addTime(Timestamp* t1, Timestamp* t2);
// Returns t1-t2 in t1
void timeDiff(Timestamp* t1, Timestamp* t2);
void getTime(Timestamp* t);

void DW_disableInterrupt();
void DW_enableInterrupt();

#endif