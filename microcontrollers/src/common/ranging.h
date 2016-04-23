#ifndef RANGING_H
#define RANGING_H

#include "dwm1000.h"
#include "tinyserial.h"

#define POLL_MSG 0
#define RESP_MSG 1
#define FINAL_MSG 2

#define MSG_TYPE_IND 0

#define T_ROUND_1A_OFFSET 1
#define T_REPLY_2A_OFFSET 6
#define T_ROUND_1B_OFFSET 11
#define T_REPLY_2B_OFFSET 16

/*#define T_REPLY_1A_GOAL 0x26160000
#define T_REPLY_1B_GOAL 0x4C2C0000
#define RESP_A_DELAY 0x22470000
#define RESP_B_DELAY 0x485D0000
#define T_REPLY_2_GOAL 0x26160000*/

#define T_REPLY_1A_GOAL 0xF3C0000
#define T_REPLY_1B_GOAL 0x1E780000
#define RESP_A_DELAY 0xB6D0000
#define RESP_B_DELAY 0x1AA90000
#define T_REPLY_2_GOAL 0xF3C0000

void readTimestamp(byte* data, int offset, Timestamp* t);
void writeTimestamp(byte* data, int offset, Timestamp* t);
void printInt(int n);

#endif