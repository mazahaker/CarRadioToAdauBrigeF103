#include "Android.h"
#include "string.h"

Android::Android(UART_HandleTypeDef *_uartAndroid, UART_HandleTypeDef *_uartLog): uartAndroid(_uartAndroid), uartLog(_uartLog)
{
}

void Android::connect() {
	switch (connectionState) {
	case PING_SEND : {
		ping();
		connectionState = PONG_WAIT;
		pingSysTime = HAL_GetTick();
		break;
	}
	case PONG_WAIT : {
		if(tryPong()) {
			connectionState = CONNECTED;
			if(playing) {
				play();
			}
		} else if(HAL_GetTick() - pingSysTime > tryPongTime) {
			connectionState = PING_SEND;
		}
		break;
	}
	default:;
	}

}

bool Android::isConnected() {
	return connectionState == CONNECTED;
}

void Android::resetState() {
	connectionState = PING_SEND;
}

void Android::processComand(char *command) {
	HAL_UART_Transmit(uartLog, (uint8_t *)"[android] received: ", 20, 2);
	HAL_UART_Transmit(uartLog, (uint8_t *)command, strlen(command), 2);
	HAL_UART_Transmit(uartLog, (uint8_t *)"\n", 1, 2);
	if(strstr(command,"PONG") != NULL) {
		lastCommand = PONG;
		HAL_Delay(30);
		HAL_UART_Transmit(uartLog, (uint8_t *)"[android] PONG received\n", 24, 2);
	}


	//	char tmp[1000];
	//	int size = getData(tmp);
	//	if(size > 0) {
	//		HAL_UART_Transmit(&huart1, (uint8_t*)tmp, size, 10);
	//		HAL_UART_Transmit(&huart1, (uint8_t*)"\n", 1, 5);
	//
	//		char *istr = strstr(tmp,"PING:PONG");
	//		if(istr != NULL) {
	//			HAL_UART_Transmit(&huart1, (uint8_t*)"\nнашлась строка\n", 16, 10);
	//		}
	//	}
}

void Android::ping() {
	HAL_Delay(10);
	HAL_UART_Transmit(uartAndroid, (uint8_t *)"<MEDIA:PING>\n", 13, 2);
	HAL_Delay(30);
	HAL_UART_Transmit(uartLog, (uint8_t *)"[android] ping\n", 15, 2);
}

bool Android::tryPong() {
	if(lastCommand == PONG) {
		lastCommand = EMPTY;
		return true;
	}
	return false;
}

void Android::play() {
	playing = true;
	if(connectionState == CONNECTED) {
		HAL_Delay(10);
		HAL_UART_Transmit(uartAndroid, (uint8_t *)"<MEDIA:PLAY>\n", 13, 2);
		HAL_Delay(30);
		HAL_UART_Transmit(uartLog, (uint8_t *)"[android] play\n", 15, 2);
	}
}
void Android::stop() {
	playing = false;
	if(connectionState == CONNECTED) {
		HAL_Delay(10);
		HAL_UART_Transmit(uartAndroid, (uint8_t *)"<MEDIA:STOP>\n", 13, 2);
		HAL_Delay(30);
		HAL_UART_Transmit(uartLog, (uint8_t *)"[android] stop\n", 15, 2);
	}
}
void Android::next() {
	if(connectionState == CONNECTED) {
		HAL_Delay(10);
		HAL_UART_Transmit(uartAndroid, (uint8_t *)"<MEDIA:NEXT>\n", 13, 2);
		HAL_Delay(30);
		HAL_UART_Transmit(uartLog, (uint8_t *)"[android] next\n", 15, 2);
	}
}
void Android::prev() {
	if(connectionState == CONNECTED) {
		HAL_Delay(10);
		HAL_UART_Transmit(uartAndroid, (uint8_t *)"<MEDIA:PREV>\n", 13, 2);
		HAL_Delay(30);
		HAL_UART_Transmit(uartLog, (uint8_t *)"[android] prev\n", 15, 2);
	}
}
void Android::shutdown() {
	if(connectionState == CONNECTED) {
		HAL_Delay(10);
		HAL_UART_Transmit(uartAndroid, (uint8_t *)"<MEDIA:SHUTDOWN>\n", 17, 2);
		HAL_Delay(30);
		HAL_UART_Transmit(uartLog, (uint8_t *)"[android] shutdown\n", 19, 2);
	}
}

