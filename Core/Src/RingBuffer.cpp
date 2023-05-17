#include "RingBuffer.h"
#include "string.h"

RingBuffer::RingBuffer() {
}

bool RingBuffer::add(const char *str) {
	if (!writeEnabled) {
		return false;
	}

	if (writeIdx == readIdx) {
		if (empty) {
			empty = false;
		} else {
			// full
			return false;
		}
	}

	for (int i = 0; i < bufferItemSize; i++) {
		buffer[writeIdx][i] = '\r';
	}

	int len = strlen(str);
	if (len > bufferItemSize - 2) {
		len = bufferItemSize - 2;
	}

	buffer[writeIdx][len] = '\n';
	buffer[writeIdx][len + 1] = '\0';

	for (int i = 0; i < len; i++) {
		buffer[writeIdx][i] = str[i];
	}

	// set new writeIdx
	writeIdx = writeIdx == bufferSize - 1 ? 0 : writeIdx + 1;

	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
	return true; //0;
}

int RingBuffer::getReadIdx() {
	// empty
	if (readIdx == writeIdx && empty)
		return -1;

	int ret = readIdx;

	readIdx = readIdx == bufferSize - 1 ? 0 : readIdx + 1;

	// if after read buffer is empty reset 'need read' flag
	if (readIdx == writeIdx) {
		empty = true;
		//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
	}
	return ret;
}

char* RingBuffer::get() {
	int idx = getReadIdx();
	if (idx >= 0) {
		return buffer[idx];
	}
	return noop;
}

bool RingBuffer::isEmpty() {
	return empty;
}

void RingBuffer::enableWrite(bool enable) {
	writeEnabled = enable;
}

bool RingBuffer::isWriteEnabled() {
	return writeEnabled;
}
