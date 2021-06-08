#include "SerialReciver.h"

SerialReciver::SerialReciver(UART_HandleTypeDef *_uart): uart(_uart) {}

int SerialReciver::getData(char *data) {
	if(!hasData || HAL_GetTick() - lastReciveTime < 10) {
		return 0;
	}

	int size = idx == 0 ? BUFFER_SIZE : idx;
	for(int i = 0; i < size; i++) {
		data[i] = buffer[i];
		buffer[i] = '\0';
	}

	hasData = 0;
	idx = 0;
	return size;
}

void SerialReciver::reciveInterruptHandler() {
	if((uart->Instance->SR & USART_SR_RXNE) != RESET)
	{
		buffer[idx] = (uint8_t)(uart->Instance->DR & (uint8_t)0x00FF); // читает байт из регистра
	}
	//		if(sim[idx] == end){
	//			hasData = 1;
	//		}
	idx = idx == BUFFER_SIZE-1 ? 0 : idx + 1;
	hasData=1;
	lastReciveTime = HAL_GetTick();
}

int SerialReciver::getMaxResultLenght() {
	int maxSize = BUFFER_SIZE;
	return maxSize;
}

