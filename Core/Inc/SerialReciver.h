#include "stm32f1xx_hal.h"
#define BUFFER_SIZE 200

class SerialReciver
{
public:
	SerialReciver(UART_HandleTypeDef *uart);
	int getData(char *data);
	void reciveInterruptHandler();
	int getMaxResultLenght();

  private:
    UART_HandleTypeDef *uart;
    bool hasData = false;
    char buffer[BUFFER_SIZE];
    int lastReciveTime = 0;
    int idx = 0;
};

//#include "stm32f1xx.h"
//
//#define MAX_SIZE 100
//
//int hasData = 0;
//char sim[MAX_SIZE];
//
//volatile int reciveTime = 0;
//int idx = 0;
//
//char end = '#';
//
//int getData(char *data) {
//	if(!hasData || HAL_GetTick() - reciveTime < 10) {
//		return 0;
//	}
//
//	int size = idx == 0 ? MAX_SIZE : idx;
//
//	for(int i = 0; i < size; i++) {
//		data[i] = sim[i];
//	}
//	clearBuffer();
//	hasData = 0;
//	idx = 0;
//	return size;
//}
//
//
//void callbackU(UART_HandleTypeDef *uart) {
//	if((uart->Instance->SR & USART_SR_RXNE) != RESET)
//	{
//		sim[idx] = (uint8_t)(uart->Instance->DR & (uint8_t)0x00FF); // читает байт из регистра
//	}
//	//		if(sim[idx] == end){
//	//			hasData = 1;
//	//		}
//	idx = idx == MAX_SIZE-1 ? 0 : idx + 1;
//	hasData=1;
//	reciveTime = HAL_GetTick();
//}
//
//void clearBuffer() {
//	for(int i = 0; i < MAX_SIZE; i++) {
//		sim[i] = i == MAX_SIZE-1 ? '!' : '.';
//	}
//}
