#include "PowerManager.h"
#include <string.h>

PowerManager::PowerManager(
		GPIO_TypeDef* _accGPIO,
		uint16_t _accPin,
		GPIO_TypeDef* _relayGPIO,
		uint16_t _relayPin,
		UART_HandleTypeDef *_uart,
		void (*_accOnCallback)(),
		void (*_accOffCallback)(),
		void (*_waitOffCallback)(),
		void (*_offCallback)()):
		accGPIO(_accGPIO),
		accPin(_accPin),
		relayGPIO(_relayGPIO),
		relayPin(_relayPin),
		uart(_uart),
		accOnCallback(_accOnCallback),
		accOffCallback(_accOffCallback),
		waitOffCallback(_waitOffCallback),
		offCallback(_offCallback)
{
}

void PowerManager::powerProcessing() {
	switch (powerState) {
	case ACC_ON : {
		if(!checkAcc()) {
			changeState(ACC_OFF);
		}
		break;
	}
	case ACC_OFF : {
		waitAccTime = HAL_GetTick();
		changeState(WAIT_ACC);
		break;
	}
	case WAIT_ACC : {
		if(checkAcc()) {
			changeState(ACC_ON);
		} else {
			if(maxAccWaitTime < HAL_GetTick() - waitAccTime) {
				waitPowerOffTime = HAL_GetTick();
				changeState(WAIT_OFF);
			}
		}
		break;
	}
	case WAIT_OFF : {
		if(checkAcc()) {
			changeState(ACC_ON);
		} else {
			if(maxPowerOffTime < HAL_GetTick() - waitPowerOffTime) {
				changeState(OFF);
			}
		}
		break;
	}
	case OFF : {
		if(checkAcc()) {
			changeState(WAIT_ACC);
		}
		break;
	}
	}
}

GPIO_PinState PowerManager::checkAcc() {
	return HAL_GPIO_ReadPin(accGPIO, accPin);
}

void PowerManager::changeState(PowerState newState) {
	powerState = newState;

	switch (newState) {
	case ACC_ON : {
		uartLog("acc on");
		(*accOnCallback)();
		HAL_GPIO_WritePin(relayGPIO, relayPin, GPIO_PIN_SET);
		break;
	}
	case ACC_OFF : {
		uartLog("acc off");
		(*accOffCallback)();
		break;
	}
	case WAIT_ACC : {
		uartLog("wait acc");
		break;
	}
	case WAIT_OFF : {
		uartLog("wait off");
		(*waitOffCallback)();
		break;
	}
	case OFF : {
		uartLog("off");
		(*offCallback)();
		HAL_GPIO_WritePin(relayGPIO, relayPin, GPIO_PIN_RESET);
		break;
	}
	}


}

void PowerManager::uartLog(char *str) {
	HAL_UART_Transmit(uart, (uint8_t *)"[power] ", 8, 2);
	HAL_UART_Transmit(uart, (uint8_t *)str, strlen(str), 2);
	HAL_UART_Transmit(uart, (uint8_t *)"\n", 1, 2);
}



