#ifndef RANGING_H
#define RANGING_H

#include "dwm1000.h"
#include "tinyserial.h"

#define POLL_MSG 0
#define RESP_MSG 1
#define FINAL_MSG 2

#define MSG_TYPE_IND 0

#define T_ROUND_1_OFFSET 1
#define T_REPLY_2_OFFSET 6

#define T_REPLY_1_GOAL 0x26160000
#define RESP_DELAY 0x22470000
#define T_REPLY_2_GOAL 0x26160000

//#define T_REPLY_1_GOAL 0xBE6E0000
//#define RESP_DELAY 0xB6D00000
//#define T_REPLY_2_GOAL 0xBE6E0000

void readTimestamp(byte* data, int offset, Timestamp* t);
void writeTimestamp(byte* data, int offset, Timestamp* t);
void printInt(unsigned int n);

#endif