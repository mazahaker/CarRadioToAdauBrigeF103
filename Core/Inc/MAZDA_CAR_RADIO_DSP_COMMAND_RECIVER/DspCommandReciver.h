#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>

#define M_SIZE 17

//Types of data
#define VOLUME_ATTV1 0
#define VOLUME_ATTV2 1
#define FADER_FRONT_ATTV1 2
#define FADER_FRONT_ATTV2 3

//Channels
#define CH1 0
#define CH2 1

#define VOLUME_ATTV1_H 8
#define VOLUME_ATTV1_W 4
#define VOLUME_ATTV2_H 4
#define VOLUME_ATTV2_W 3
#define FADER_ATTV1_H 15
#define FADER_ATTV1_W 5
#define FADER_ATTV2_H 4
#define FADER_ATTV2_W 3

class DspCommandReciver
{
public:
	DspCommandReciver(GPIO_TypeDef* clockGPIO, uint16_t clockPin, GPIO_TypeDef* dataGPIO, uint16_t dataPin, UART_HandleTypeDef *uart, void (*_addMessage)(char* msg));
	//deprecated
	void readDataOld();
	void readPackage();
	int8_t getLevel(uint8_t type, uint8_t channel);
	int8_t getAbsoluteLevel(uint8_t channel);
	bool isChanged();
	void resetChangedFlag();
	void setChangedFlag();
	void printToSerial(UART_HandleTypeDef *uart);
	void printMsgFunc();

private:
	void (*addMessage)(char* msg);
	GPIO_TypeDef* _clockGPIO;
	uint16_t _clockPin;
	GPIO_TypeDef* _dataGPIO;
	uint16_t _dataPin;

	int8_t volume[M_SIZE];
	int8_t faderCh1[M_SIZE];
	int8_t faderCh2[M_SIZE];
	int8_t equalizer[M_SIZE];

	bool volumeRead;
	bool faderCh1Read;
	bool faderCh2Read;
	bool equalizerRead;

	bool volumeChng;
	bool faderCh1Chng;
	bool faderCh2Chng;
	bool equalizerChng;

	bool changed = false;
	bool eqChangedDetect = false;

	int8_t buff[M_SIZE];

	//for old read func
	uint8_t iterator;
	uint8_t data;
	uint8_t clock;
	uint8_t endOfData;

	UART_HandleTypeDef *_uart;

	bool readPackage(int iterator);
	void typeDetectAndSave();
	bool tranferDataFromBuffer(int8_t *target, bool eq);
	void clearData(char *array);
	void clearData(int8_t *array);
	int8_t checkElement(int8_t *valuesArrayForCheck, int8_t *targetArray, int8_t targetArrayWidth, int8_t wIdx);

	//level ATTV1 d2,d7 d3,d8 d4,d9
	int8_t volumeATTV1[VOLUME_ATTV1_H][VOLUME_ATTV1_W] = {
			0, 1, 0, 1,
			-4, 0, 0, 1,
			-8, 1, 1, 0,
			-12, 0, 1, 0,
			-16, 1, 0, 0,
			-20, 0, 0, 0,
			-24, 0, 1, 1,
			-28, 1, 1, 1};

	//level ATTV2 d5,d10 d6,d11
	int8_t volumeATTV2[VOLUME_ATTV2_H][VOLUME_ATTV2_W] = {
			0, 1, 1,
			-1, 0, 1,
			-2, 1, 0,
			-3, 0, 0,};

	//fader ATTV1 d2,d8 d3,d9 d4,d10 d5,d11
	int8_t faderATTV1[FADER_ATTV1_H][FADER_ATTV1_W] = {
			0, 1, 0, 0, 1,
			-2, 0, 0, 0, 1,
			-4, 0, 1, 0, 1,
			-6, 1, 1, 0, 1,
			-8, 1, 1, 1, 0,
			-10, 0, 0, 1, 1,
			-12, 1, 0, 1, 1,
			-14, 0, 1, 1, 1,
			-16, 0, 1, 1, 0,
			-24, 1, 0, 1, 0,
			-32, 0, 0, 1, 0,
			-40, 1, 1, 0, 0,
			-48, 0, 1, 0, 0,
			-56, 1, 0, 0, 0,
			-64, 0, 0, 0, 0};

	//fader ATTV2 d6,d12 d7,d13
	int8_t faderATTV2[FADER_ATTV2_H][FADER_ATTV2_W] = {
			0, 1, 1,
			-2, 0, 1,
			-4, 1, 0,
			-6, 0, 0,};
};
