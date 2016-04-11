#include <Arduino.h>
#include "tinyserial.h"

#include "dwm1000.h"
#include "ranging.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define RESET_PIN 9
#define IRQ 0

typedef enum {POLL, RESP, FINAL} Stage;
struct AnchorState {
	Stage stage;
	bool stageStarted;
	int tagAddr;
	Timestamp pollRx;
	Timestamp respTX;
};

AnchorState state;

void initState(AnchorState* a) {
	a->stage = POLL;
	a->stageStarted = false;
	a->tagAddr = 0xFF;
}

void handlePoll(AnchorState* a, int srcAddr, Timestamp* t) {
	a->stage = RESP;
	a->stageStarted = false;
	a->tagAddr = srcAddr;
	a->pollRx.time = t->time;
}

void handleRespSent(AnchorState* a, Timestamp* t) {
	a->stage = FINAL;	
	a->stageStarted = false;
	a->respTX.time = t->time;
}

void handleFinal(AnchorState* a, Timestamp* t, byte* data) {
	a->stage = POLL;
	a->stageStarted = false;
	
	//printTime(&(a->respTX));
	//printTime(&(a->pollRx));
	// Put T_round2X in t
	timeDiff(t, &(a->respTX));
	// Put T_reply1X in respTx
	timeDiff(&(a->respTX), &(a->pollRx));
	Timestamp tRound1, tReply2;
	if (POS == 0) {
		readTimestamp(data, T_ROUND_1A_OFFSET, &tRound1);
		readTimestamp(data, T_REPLY_2A_OFFSET, &tReply2);
	} else {
		readTimestamp(data, T_ROUND_1B_OFFSET, &tRound1);
		readTimestamp(data, T_REPLY_2B_OFFSET, &tReply2);
	}
	
	/*printBytes((byte*)&(tRound1.time), 5);
	printBytes((byte*)&(a->respTX.time), 5);
	printBytes((byte*)&(tReply2.time), 5);
	printBytes((byte*)&(t->time), 5);*/

	a->pollRx.time = (tRound1.time*t->time - a->respTX.time*tReply2.time) / (tRound1.time + t->time + a->respTX.time + tReply2.time);
	int dist = a->pollRx.time / (US_TO_TIMESTAMP / 29979);
	printInt(dist);
	ts_puts("\r\n");
	DW_reset();
	//printBytes((byte*)&a->pollRx.time, 5);
}

void rxCallback(Timestamp* t, byte* data, int len, int srcAddr) {
	//ts_puts("Got Message\r\n");
	if (data[MSG_TYPE_IND] == POLL_MSG) {
		//ts_puts("p\r\n");
		handlePoll(&state, srcAddr, t);
	} else if (data[MSG_TYPE_IND] == FINAL_MSG && state.stage == FINAL) {
		//ts_puts("Got final\r\n");
		handleFinal(&state, t, data);
	} else {
		state.stageStarted = false;
	}
}

void txCallback(Timestamp* t) {
	//ts_puts("Sent resp\r\n");
	handleRespSent(&state, t);
}

void failCallback() {
	//ts_puts("FAIL\r\n");
	state.stage = POLL;
	state.stageStarted = false;
}

int main(void) {
	init();
	ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
	DW_init(SELECT_PIN, RESET_PIN, IRQ, NETWORK_ID, CHIP_ADDR, 0);
	initState(&state);
	DW_setReceivedCallback(&rxCallback);
	DW_setSentCallback(&txCallback);
	DW_setGeneralFailCallback(&failCallback);
   	while (1) {
   		DW_disableInterrupt();
		if (state.stage == POLL) {
			if(!state.stageStarted) {
				//ts_puts("Wait for poll\r\n");
				state.stageStarted = true;
				DW_receiveMessage(NULL);
			}
		} else if (state.stage == RESP) {
			if(!state.stageStarted) {
				//ts_puts("Sending resp\r\n");
				state.stageStarted = true;
				Timestamp t;
				if (POS == 0) {
					t.time = T_REPLY_1A_GOAL;
				} else {
					t.time = T_REPLY_1B_GOAL;
				}
				addTime(&t, &(state.pollRx));
				byte msg_type = RESP_MSG;
				DW_sendMessage(&msg_type, 1, state.tagAddr, &t);
			}
		} else { // state.stage == FINAL
			if(!state.stageStarted) {
				//ts_puts("Wait for final\r\n");
				state.stageStarted = true;
				DW_receiveMessage(NULL);
			}
		}
		DW_enableInterrupt();
		delayMicroseconds(1000);
   }
   return 0;
}
