#include "stm32f1xx_hal.h"

class RingBuffer {
public:
	RingBuffer();

	bool add(const char *str);
	int getReadIdx();
	char* get();
	bool isEmpty();
	void enableWrite(bool enable);
	bool isWriteEnabled();

	static const int getItemSize() {
		return bufferItemSize;
	}

	static void clearArray(char *str, int size) {
		for (int i = 0; i < size; i++) {
			str[i] = '\0';
		}
	}

private:
	// Работа с буфером исходящих сообщений
	static const int bufferSize = 20;
	static const int bufferItemSize = 50;

	char buffer[bufferSize][bufferItemSize];
	int readIdx = 0;
	int writeIdx = 0;
	bool empty = true;
	bool writeEnabled = false;

	// пустой ответ
	char noop[bufferItemSize] = "noop\n";
};

