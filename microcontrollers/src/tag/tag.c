#include <Arduino.h>
#include "tinyserial.h"

#include "dwm1000.h"
#include "ranging.h"

#define NETWORK_ID 0

#define SELECT_PIN 10
#define RESET_PIN 9
#define IRQ 0

#define MSG_WAIT_TIME 0xBE6E00000

typedef enum {POLL, RESP_A, RESP_B, FINAL} Stage;
struct TagState {
   Stage stage;
   bool stageStarted;
   Timestamp pollTx;
   Timestamp respRxA;
   Timestamp respRxB;
};

TagState state;

void initState(TagState* a) {
   a->stage = POLL;
   a->stageStarted = false;
}

void handlePollSent(TagState* a, Timestamp* t) {
   a->stage = RESP_A;
   a->stageStarted = false;
   a->pollTx.time = t->time;
}

void handleResp(TagState* a, Timestamp* t) {
   if (a->stage == RESP_A) {
      ts_puts("RESP_A\r\n");
      a->stage = RESP_B;
      a->respRxA.time = t->time;
   } else {
      ts_puts("RESP_B\r\n");
      a->stage = FINAL;
      a->respRxB.time = t->time;
   }

   a->stageStarted = false;
   
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
      ts_puts("Sent final\r\n");
      handleFinalSent(&state);
   }
}

void failCallback() {
   ts_puts("LOL fail\r\n");
   startOver(&state);
}

int main(void) {
   init();
   ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
   ts_puts("Hello\r\n");
   DW_init(SELECT_PIN, RESET_PIN, IRQ, NETWORK_ID, CHIP_ADDR, 2000);
   initState(&state);
   DW_setReceivedCallback(&rxCallback);
   DW_setReceiveFailedCallback(&rxFailCallback);
   DW_setSentCallback(&txCallback);
   DW_setGeneralFailCallback(&failCallback);
   Timestamp msgDelay;
   msgDelay.time = MSG_WAIT_TIME;
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
      } else if (state.stage == RESP_A || state.stage == RESP_B) {
         if(!state.stageStarted) {
            //ts_puts("Wait for resp\r\n");
            state.stageStarted = true;
            Timestamp t;
            if (state.stage == RESP_A) {
               t.time = RESP_A_DELAY;
            } else {
               t.time = RESP_B_DELAY;
            }
            addTime(&t, &(state.pollTx));
            DW_receiveMessage(&t);
         }
      } else { // state.stage == FINAL
         if(!state.stageStarted) {
            //ts_puts("Sending final\r\n");
            state.stageStarted = true;
            Timestamp t, tReply2A, tReply2B;
            t.time = state.respRxB.time;
            addTime(&t, &finalDelay);
            tReply2A.time = t.time & 0x000000FFFFFFFE00;
            tReply2B.time = tReply2A.time;
            timeDiff(&tReply2A, &(state.respRxA));
            timeDiff(&tReply2B, &(state.respRxB));
            byte msg[21];
            msg[0] = FINAL_MSG;
            // Put T_round1X in respRxX
            timeDiff(&(state.respRxA), &(state.pollTx));
            timeDiff(&(state.respRxB), &(state.pollTx));

            writeTimestamp(msg, T_ROUND_1A_OFFSET, &(state.respRxA));
            writeTimestamp(msg, T_REPLY_2A_OFFSET, &(tReply2A));
            writeTimestamp(msg, T_ROUND_1B_OFFSET, &(state.respRxB));
            writeTimestamp(msg, T_REPLY_2B_OFFSET, &(tReply2B));

            DW_sendBroadcast(msg, 21, &t);
         }
      }
      DW_enableInterrupt();
      delayMicroseconds(10);
   }
   return 0;
}
