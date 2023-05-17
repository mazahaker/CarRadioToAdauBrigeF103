#include "PowerManager.h"
#include <string.h>

PowerManager::PowerManager(GPIO_TypeDef *_accGPIO, uint16_t _accPin, GPIO_TypeDef *_relayGPIO, uint16_t _relayPin, UART_HandleTypeDef *_uart, void (*_accOnCallback)(), void (*_accOffCallback)(),
		void (*_waitOffCallback)(), void (*_offCallback)(), void (*_addMessage)(char *msg)) :
		accGPIO(_accGPIO), accPin(_accPin), relayGPIO(_relayGPIO), relayPin(_relayPin), uart(_uart), accOnCallback(_accOnCallback), accOffCallback(_accOffCallback), waitOffCallback(_waitOffCallback), offCallback(
				_offCallback), addMessage(_addMessage) {
}

void PowerManager::powerProcessing() {
	if (HAL_GetTick() - time < 1000) {
		return;
	} else {
		time = HAL_GetTick();
	}

	switch (powerState) {
	case ACC_ON: {
		if (!checkAcc()) {
			changeState(ACC_OFF);
		}
		break;
	}
	case ACC_OFF: {
		waitAccTime = HAL_GetTick();
		changeState(WAIT_ACC);
		break;
	}
	case WAIT_ACC: {
		if (checkAcc()) {
			changeState(ACC_ON);
		} else {
			if (maxAccWaitTime < HAL_GetTick() - waitAccTime) {
				waitPowerOffTime = HAL_GetTick();
				changeState(WAIT_OFF);
			}
		}
		break;
	}
	case WAIT_OFF: {
		if (checkAcc()) {
			changeState(ACC_ON);
		} else {
			if (maxPowerOffTime < HAL_GetTick() - waitPowerOffTime) {
				changeState(OFF);
			}
		}
		break;
	}
	case OFF: {
		if (checkAcc()) {
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
	if (powerState == newState) {
		return;
	}
	powerState = newState;

	switch (newState) {
	case ACC_ON: {
		(*accOnCallback)();
		HAL_GPIO_WritePin(relayGPIO, relayPin, GPIO_PIN_SET);
		if (uart != NULL) {
			HAL_UART_Transmit(uart, (uint8_t*) "[power] acc on", 14, 2);
		}
		if (addMessage != NULL) {
			addMessage("[power] acc on");
		}
		break;
	}
	case ACC_OFF: {
		(*accOffCallback)();
		if (uart != NULL) {
			HAL_UART_Transmit(uart, (uint8_t*) "[power] acc off", 15, 2);
		}
		if (addMessage != NULL) {
			addMessage("[power] acc off");
		}
		break;
	}
	case WAIT_ACC: {
		if (uart != NULL) {
			HAL_UART_Transmit(uart, (uint8_t*) "[power] wait acc", 16, 2);
		}
		if (addMessage != NULL) {
			addMessage("[power] wait acc");
		}
		break;
	}
	case WAIT_OFF: {
		(*waitOffCallback)();
		if (uart != NULL) {
			HAL_UART_Transmit(uart, (uint8_t*) "[power] wait off", 16, 2);
		}
		if (addMessage != NULL) {
			addMessage("[power] wait off");
		}
		break;
	}
	case OFF: {
		(*offCallback)();
		HAL_GPIO_WritePin(relayGPIO, relayPin, GPIO_PIN_RESET);
		if (uart != NULL) {
			HAL_UART_Transmit(uart, (uint8_t*) "[power] off", 11, 2);
		}
		if (addMessage != NULL) {
			addMessage("[power] off");
		}
		break;
	}
	}

}

