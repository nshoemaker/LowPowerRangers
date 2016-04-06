#include <Arduino.h>
#include "tinyserial.h"

#include "dwm1000.h"
#include "ranging.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define RESET_PIN 9
#define IRQ 0

#define MSG_WAIT_TIME 0xBE6E00000

typedef enum {POLL, RESP, FINAL} Stage;
struct TagState {
   Stage stage;
   bool stageStarted;
   Timestamp pollTx;
   Timestamp respRx;
};

TagState state;

void initState(TagState* a) {
   a->stage = POLL;
   a->stageStarted = false;
}

void handlePollSent(TagState* a, Timestamp* t) {
   a->stage = RESP;
   a->stageStarted = false;
   a->pollTx.time = t->time;
}

void handleResp(TagState* a, Timestamp* t) {
   a->stage = FINAL;
   a->stageStarted = false;
   a->respRx.time = t->time;
}

void startOver(TagState* a) {
   a->stage = POLL;
   a->stageStarted = false;
}

void handleFinalSent(TagState* a) {
   startOver(a);
}

void rxCallback(Timestamp* t, byte* data, int len, int srcAddr) {
   if (data[MSG_TYPE_IND] == RESP_MSG) {
      handleResp(&state, t);
   } else {
      state.stageStarted = false;
   }
}

void rxFailCallback() {
   //ts_puts("No Message\r\n");
   startOver(&state);
}

void txCallback(Timestamp* t) {
   //ts_puts("Sent Message\r\n");
   if (state.stage == POLL) {
      //ts_puts("Sent poll\r\n");
      handlePollSent(&state, t);
   } else if (state.stage == FINAL) {
      //ts_puts("Sent final\r\n");
      handleFinalSent(&state);
   }
}

int main(void) {
   init();
   ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
   DW_init(SELECT_PIN, RESET_PIN, IRQ, NETWORK_ID, CHIP_ADDR, 2000);
   initState(&state);
   DW_setReceivedCallback(&rxCallback);
   DW_setReceiveFailedCallback(&rxFailCallback);
   DW_setSentCallback(&txCallback);
   Timestamp msgDelay;
   msgDelay.time = MSG_WAIT_TIME;
   Timestamp respDelay;
   respDelay.time = RESP_DELAY;
   Timestamp finalDelay;
   finalDelay.time = T_REPLY_2_GOAL;
   while (1) {
      DW_disableInterrupt();
      if (state.stage == POLL) {
         if(!state.stageStarted) {
            //ts_puts("Sending poll\r\n");
            state.stageStarted = true;
            Timestamp t;
            DW_reset();
            getTime(&t);
            addTime(&t, &msgDelay);
            byte msg_type = POLL_MSG;
            DW_sendBroadcast(&msg_type, 1, &t);
         }
      } else if (state.stage == RESP) {
         if(!state.stageStarted) {
            //ts_puts("Wait for resp\r\n");
            state.stageStarted = true;
            Timestamp t;
            t.time = state.pollTx.time;
            addTime(&t, &respDelay);
            DW_receiveMessage(&t);
         }
      } else { // state.stage == FINAL
         if(!state.stageStarted) {
            //ts_puts("Sending final\r\n");
            state.stageStarted = true;
            Timestamp t, t2;
            t.time = state.respRx.time;
            addTime(&t, &finalDelay);
            t2.time = t.time & 0x000000FFFFFFFE00;
            timeDiff(&t2, &(state.respRx));
            byte msg[11];
            msg[0] = FINAL_MSG;
            // Put T_round1 in respRx
            timeDiff(&(state.respRx), &(state.pollTx));
            writeTimestamp(msg, T_ROUND_1_OFFSET, &(state.respRx));
            writeTimestamp(msg, T_REPLY_2_OFFSET, &(t2));
            DW_sendBroadcast(msg, 11, &t);
         }
      }
      DW_enableInterrupt();
      delayMicroseconds(10);
   }
   return 0;
}
