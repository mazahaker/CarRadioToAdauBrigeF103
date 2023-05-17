#include "Serial.h"
#include <string.h>
#include <stdio.h>

Serial::Serial(UART_HandleTypeDef *uart):_uart(uart) {}

void Serial::print(char *str) {
	HAL_UART_Transmit(_uart, (uint8_t*)str, strlen(str), 20);
}

void Serial::print(uint8_t *str, int size) {
	HAL_UART_Transmit(_uart, str, size, 20);
}

void Serial::println(char *str) {
	int len = strlen(str);
	char char_array[len + 1];
	for(int i = 0; i < len; i++) {
		char_array[i] = str[i];
	}
	char_array[len] = '\n';
	HAL_UART_Transmit(_uart, (uint8_t*)char_array, ++len, 20);
}

void Serial::println() {
	char char_array[] = "\n";
	HAL_UART_Transmit(_uart, (uint8_t*)char_array, sizeof(char_array), 5);
}

void Serial::print(uint32_t i) {
	char buffer[64];
	sprintf(buffer, "%d", i);
	print(buffer);
	//	print(std::to_string(i));
}

void Serial::print(int i) {
	char buffer[64];
	sprintf(buffer, "%d", i);
	print(buffer);
	//	print(std::to_string(i));
}

void Serial::print(float f) {
	//	char buffer[64];
	//	sprintf(buffer, "%f", f);
	//	print(buffer);

	//	print(std::to_string(f));
}

void Serial::print(double d) {
	//	char buffer[64];
	//	sprintf(buffer, "%f", d);
	//	print(buffer);

	//	print(std::to_string(d));
}
