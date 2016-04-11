#include "ranging.h"

void readTimestamp(byte* data, int offset, Timestamp* t) {
   t->time = *((long long*)(data + offset)) & 0xFFFFFFFFFF;
}

void writeTimestamp(byte* data, int offset, Timestamp* t) {
	for (int i = 0; i < 5; ++i) {
		data[offset + i] = (t->time >> (8 * i)) & 0xFF;
	}
}

void printInt(unsigned int n) {
   char tmp[5];
   sprintf(tmp, "%u", n);
   ts_puts(tmp);
}