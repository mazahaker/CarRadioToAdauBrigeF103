/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <SERIAL_LIB_STM32/Serial.h>
#include <MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.h>
#include <MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER/DspCommandReciver.h>
#include <ADAU_1701_LIB_STM32/SigmaDSP_parameters.h>
#include <PowerManager.h>
//#include <Android.h>
#include <main.h>
#include <SerialReciver.h>
//#include "string.h"
#include "RingBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_MODE true
#define I2C_MODE false
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// android commands
const char connected[] = "kg.serial.manager.connection_established";
const char disconnected[] = "kg.serial.manager.connection_lost";

const char *play = "<MEDIA:PLAY>";
const char *stop = "<MEDIA:STOP>";
const char *next = "<MEDIA:NEXT>";
const char *prev = "<MEDIA:PREV>";
const char *shutdown = "<MEDIA:SHUTDOWN>";
const char *ping = UART_MODE ? "<MEDIA:PING>\n" : "<MEDIA:PING>";

const char *adauResetCmd = "rAdau";
const char *stmResetCmd = "rStm";
const char *debugCmd = "debug";
const char *pong = "pong";

char tmp[50];

bool debug = true;
bool playing = false;

uint32_t lastPingTime = 0;
uint32_t lastPongTime = 0;
bool androidConnected = false;

// i2c состояние
int state = 0;

//отправка данных на экран
uint32_t sysTime = 0;
int volumeLastValue = 0;
bool sendAdauLastStatus = false;
static uint32_t sendTimePeriod = 5300;

//управление каналом включения услилителей (и глобальным mute)
uint32_t amplifierStartTimePeriod = 10000;
bool amplifierReadyToStart = false;
bool amplifierStarted = false;
uint32_t amplifierStartTime = 0;

//Управление открытием монитора
bool monitorOpen = false;

bool muteRadio = false;
bool muteAndroid = false;

int lastVolumeChanged = 0;

int lastInfoSend = 0;

SigmaDSP dsp(&hi2c2, DSP_I2C_ADDRESS, ADAU1701, GPIOA, ADAU_RESET_Pin, NULL, addToBuffer);
DspCommandReciver dspCommandReciver(GPIOA, DSP_CLOCK_Pin, GPIOA, DSP_DATA_Pin, NULL, addToBuffer);
ExtDeviceCommandReciver extDeviceCommandReciver(GPIOB, MBUS_RX_Pin, GPIOB, MBUS_TX_Pin, &htim1, NULL, playTrack, stopTrack, nextTrack, prevTrack, addToBuffer);
PowerManager powerManager(GPIOA, AC_Pin, GPIOA, POWER_RELAY_Pin, NULL, accOn, accOff, waitOff, off, addToBuffer);
SerialReciver serialReciver(&huart2);
Serial serial(&huart2);

RingBuffer infoBuffer;
RingBuffer commandBuffer;
RingBuffer inputDataBuffer;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM4_Init();
	MX_I2C2_Init();
	MX_TIM1_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();

	/* Initialize interrupts */
	MX_NVIC_Init();
	/* USER CODE BEGIN 2 */

	extDeviceCommandReciver.startTimer();

	HAL_Delay(100);

	inputDataBuffer.enableWrite(true);
	commandBuffer.enableWrite(androidConnected);
	infoBuffer.enableWrite(debug);
	infoBuffer.add("[stm32] started");

	resetAdauCmd();

	if (UART_MODE) {
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); // включить прерывания usart'a
	}
	if (I2C_MODE) {
		HAL_I2C_EnableListen_IT(&hi2c1);
	}
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		powerManager.powerProcessing();
		volumeProcessing();
		muteProcessing();
		timeProcessing();
		monitorProcessing();
		amplifierProcessing();
		executeInputCmd();

		cmdInfoToBuffer();

		if (I2C_MODE) {
			if (androidConnected) { //
				setupReadFlagGPIO(!infoBuffer.isEmpty() || !commandBuffer.isEmpty());
			} else if (HAL_GetTick() - lastPingTime > 2000) {
				setupReadFlagGPIO(true);
			}
			processI2C();
		}

		if (UART_MODE) {
			processUART();
		}

		checkConnection();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief NVIC Configuration.
 * @retval None
 */
static void MX_NVIC_Init(void) {
	/* EXTI0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	/* EXTI9_5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 16;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void) {

	/* USER CODE BEGIN I2C2_Init 0 */

	/* USER CODE END I2C2_Init 0 */

	/* USER CODE BEGIN I2C2_Init 1 */

	/* USER CODE END I2C2_Init 1 */
	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 100000;
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C2_Init 2 */

	/* USER CODE END I2C2_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 72 - 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 36000 - 1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 65535;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, LED_Pin | MONITOR_OPEN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, ADAU_RESET_Pin | POWER_RELAY_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(AMPLIFIER_RELAY_GPIO_Port, AMPLIFIER_RELAY_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, MBUS_TX_Pin | I2C_EVENT_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : LED_Pin */
	GPIO_InitStruct.Pin = LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : MONITOR_OPEN_Pin */
	GPIO_InitStruct.Pin = MONITOR_OPEN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(MONITOR_OPEN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : ADAU_RESET_Pin */
	GPIO_InitStruct.Pin = ADAU_RESET_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADAU_RESET_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : AC_Pin DSP_DATA_Pin */
	GPIO_InitStruct.Pin = AC_Pin | DSP_DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : POWER_RELAY_Pin AMPLIFIER_RELAY_Pin */
	GPIO_InitStruct.Pin = POWER_RELAY_Pin | AMPLIFIER_RELAY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : DSP_CLOCK_Pin */
	GPIO_InitStruct.Pin = DSP_CLOCK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(DSP_CLOCK_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : MBUS_RX_Pin */
	GPIO_InitStruct.Pin = MBUS_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(MBUS_RX_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : MBUS_TX_Pin */
	GPIO_InitStruct.Pin = MBUS_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MBUS_TX_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : I2C_EVENT_Pin */
	GPIO_InitStruct.Pin = I2C_EVENT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(I2C_EVENT_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
	HAL_I2C_EnableListen_IT(hi2c); // slave is ready again
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {

}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	UNUSED(AddrMatchCode);

	if (hi2c->Instance == I2C1) {
		switch (TransferDirection) {
		case I2C_DIRECTION_RECEIVE:
			state = 1;
			break;
		case I2C_DIRECTION_TRANSMIT:
			state = 2;
			break;
		};
	};
}

void setupReadFlagGPIO(bool enable) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void executeInputCmd() {
	if (!inputDataBuffer.isEmpty()) {
		bool state = infoBuffer.isWriteEnabled();
		infoBuffer.enableWrite(true);

		char *buffItem = inputDataBuffer.get();
		if (strstr(buffItem, pong) != NULL || strstr(buffItem, connected) != NULL) {
			if (!androidConnected) {
				infoBuffer.add("[action] connected");
				androidConnected = true;
				commandBuffer.enableWrite(true);
				if (playing) {
					playTrack();
				}
			}
			lastPongTime = HAL_GetTick();
		} else if (strstr(buffItem, disconnected) != NULL) {
			infoBuffer.add("[action] disconnected");
			androidConnected = false;
			commandBuffer.enableWrite(false);
		} else if (strstr(buffItem, debugCmd) != NULL) {
			debug = !debug;
			state = debug;
			if (debug) {
				infoBuffer.add("[action] debug on");
			} else {
				infoBuffer.add("[action] debug off");
			}
		} else if (strstr(buffItem, adauResetCmd) != NULL) {
			infoBuffer.add("[action] reset adau1701");
			resetAdauCmd();
		} else if (strstr(buffItem, stmResetCmd) != NULL) {
			infoBuffer.add("[action] reset stm32");
			NVIC_SystemReset();
		}

		infoBuffer.enableWrite(state);
	}

}

void addToBuffer(char *str) {
	infoBuffer.add(str);
}

void processI2C() {
	switch (state) {
	case 1: {
		setupReadFlagGPIO(false);
		if (!androidConnected) {
			RingBuffer::clearArray(tmp, RingBuffer::getItemSize());
			for (uint i = 0; i < strlen(ping); i++) {
				tmp[i] = ping[i];
			}
			HAL_I2C_Slave_Sequential_Transmit_IT(&hi2c1, (uint8_t*) tmp, RingBuffer::getItemSize(), I2C_LAST_FRAME);
			lastPingTime = HAL_GetTick();
		} else {
			setupReadFlagGPIO(false);

			if (!commandBuffer.isEmpty()) {
				HAL_I2C_Slave_Sequential_Transmit_IT(&hi2c1, (uint8_t*) commandBuffer.get(), RingBuffer::getItemSize(), I2C_LAST_FRAME);
			} else {
				HAL_I2C_Slave_Sequential_Transmit_IT(&hi2c1, (uint8_t*) infoBuffer.get(), RingBuffer::getItemSize(), I2C_LAST_FRAME);
			}

			setupReadFlagGPIO(!infoBuffer.isEmpty() || !commandBuffer.isEmpty());
		}
		state = 0;
		break;
	}
	case 2: {
		RingBuffer::clearArray(tmp, RingBuffer::getItemSize());

		HAL_I2C_Slave_Sequential_Receive_IT(&hi2c1, (uint8_t*) &tmp, RingBuffer::getItemSize(), I2C_FIRST_FRAME);
		HAL_Delay(100);
		bool state = infoBuffer.isWriteEnabled();
		infoBuffer.enableWrite(true);
		infoBuffer.add("Received");
		infoBuffer.add(tmp);
		infoBuffer.enableWrite(state);

		inputDataBuffer.add(tmp);
		state = 0;
		break;
	}
	default:
		;
	}
}

void processUART() {
	if (!serialReciver.isEmpty()) {
		RingBuffer::clearArray(tmp, RingBuffer::getItemSize());
		serialReciver.getData(tmp, RingBuffer::getItemSize());

		bool state = infoBuffer.isWriteEnabled();
		infoBuffer.enableWrite(true);
		infoBuffer.add("Received");
		infoBuffer.add(tmp);
		infoBuffer.enableWrite(state);

		inputDataBuffer.add(tmp);
	}

	if (!androidConnected) {
		if (HAL_GetTick() - lastPingTime > 3000) {
			HAL_UART_Transmit(&huart2, (uint8_t*) ping, strlen(ping), 20);
			lastPingTime = HAL_GetTick();
		}
	} else {
		char *item;
		if (!commandBuffer.isEmpty()) {
			item = commandBuffer.get();
			HAL_UART_Transmit(&huart2, (uint8_t*) item, strlen(item), 20);
		} else if (!infoBuffer.isEmpty()) {
			item = infoBuffer.get();
			HAL_UART_Transmit(&huart2, (uint8_t*) item, strlen(item), 20);
		}
	}
}

void checkConnection() {
	if (androidConnected) {
		if (HAL_GetTick() - lastPongTime > 25000) {
			androidConnected = false;
			commandBuffer.enableWrite(false);
			infoBuffer.add("[action] disconnected");
		} else {
			if (HAL_GetTick() - lastPingTime > 15000) {
				infoBuffer.add(ping);
				lastPingTime = HAL_GetTick();
			}
		}
	}
}

void clearPendingInterrupt() {
	EXTI->PR = EXTI_PR_PR0; //Сбрасываем флаг прерывания
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	EXTI->PR = EXTI_PR_PR9; //Сбрасываем флаг прерывания
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void dataRequestInterrupt() {
	dspCommandReciver.readPackage();
//	clearPendingInterrupt();
}

void mBusRequestInterrupt() {
	extDeviceCommandReciver.commandProcessing();
	sysTime = HAL_GetTick();
//	clearPendingInterrupt();
}

void timeProcessing() {
	uint32_t newSysTime = HAL_GetTick();
	if (newSysTime - sysTime > sendTimePeriod && extDeviceCommandReciver.isPlaying()) {
		sysTime = newSysTime;
		HAL_NVIC_DisableIRQ(EXTI0_IRQn);
		extDeviceCommandReciver.sendTime(abs(volumeLastValue), sendAdauLastStatus ? 1 : 2);
		clearPendingInterrupt();
		HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	}
}

void volumeProcessing() {
	if (dspCommandReciver.isChanged()) {
		if (HAL_GetTick() - lastVolumeChanged < 50) {
			return;
		}

		int8_t level1 = dspCommandReciver.getAbsoluteLevel(CH1);
		int8_t level2 = dspCommandReciver.getAbsoluteLevel(CH2);

		RingBuffer::clearArray(tmp, RingBuffer::getItemSize());
		snprintf(tmp, RingBuffer::getItemSize(), "%s%d%s%d", "[volume] changed (DB) CH1:", level1, " CH2:", level2);
		infoBuffer.add(tmp);

		bool connected = dsp.ping();
		if (!connected) {
			for (int i = 0; i < 5 && !connected; i++) {
				infoBuffer.add("[adau] reconnect");
				HAL_I2C_DeInit(&hi2c2);
				HAL_Delay(20);
				HAL_I2C_Init(&hi2c2);
				HAL_Delay(20);
				connected = dsp.ping();
			}
		}

		dspCommandReciver.resetChangedFlag();

		volumeLastValue = level1;
		sendAdauLastStatus = connected;

		if (!connected) {
			return;
		}
		HAL_Delay(20);
		dsp.twoChannelGain(MOD_DIGITALINPUTGAIN_ALG0_GAIN1940ALGNS8_ADDR, level1, level2);
		lastVolumeChanged = HAL_GetTick();
	}
}

void amplifierProcessing() {
	if (amplifierReadyToStart) {
		if (HAL_GetTick() - amplifierStartTime > amplifierStartTimePeriod && !HAL_GPIO_ReadPin(AMPLIFIER_RELAY_GPIO_Port, AMPLIFIER_RELAY_Pin)) {
			HAL_GPIO_WritePin(AMPLIFIER_RELAY_GPIO_Port, AMPLIFIER_RELAY_Pin, GPIO_PIN_SET);
			amplifierStarted = true;
			infoBuffer.add("[amplifier] power on");
		}
	} else if (HAL_GPIO_ReadPin(AMPLIFIER_RELAY_GPIO_Port, AMPLIFIER_RELAY_Pin)) {
		HAL_GPIO_WritePin(AMPLIFIER_RELAY_GPIO_Port, AMPLIFIER_RELAY_Pin, GPIO_PIN_RESET);
		amplifierStarted = false;
		infoBuffer.add("[amplifier] power off");
	}
}

void monitorProcessing() {
	if (monitorOpen) {
		if (!HAL_GPIO_ReadPin(MONITOR_OPEN_GPIO_Port, MONITOR_OPEN_Pin)) {
			HAL_GPIO_WritePin(MONITOR_OPEN_GPIO_Port, MONITOR_OPEN_Pin, GPIO_PIN_SET);
			infoBuffer.add("[monitor] open");
		}
	} else if (HAL_GPIO_ReadPin(MONITOR_OPEN_GPIO_Port, MONITOR_OPEN_Pin)) {
		HAL_GPIO_WritePin(MONITOR_OPEN_GPIO_Port, MONITOR_OPEN_Pin, GPIO_PIN_RESET);
		infoBuffer.add("[monitor] close");
	}
}

void accOn() {
	//через 10 сек вкл усилитель и снять mute
	amplifierReadyToStart = true;
	amplifierStartTime = HAL_GetTick();
	monitorOpen = true;
}

void accOff() {
	amplifierReadyToStart = false;
	extDeviceCommandReciver.resetPlayingFlag();

	if (androidConnected) {
		commandBuffer.add(stop);
	}
//	android.stop();
}

void waitOff() {
	if (androidConnected) {
		commandBuffer.add(shutdown);
	}
	monitorOpen = false;
//	android.shutdown();
}

void off() {
}

void playTrack() {
	playing = true;
	if (androidConnected) {
		commandBuffer.add(play);
	}
//	android.play();
}

void stopTrack() {
	playing = false;
	if (androidConnected) {
		commandBuffer.add(stop);
	}
//	android.stop();
}

void nextTrack() {
	if (androidConnected) {
		commandBuffer.add(next);
	}
//	android.next();
}

void prevTrack() {
	if (androidConnected) {
		commandBuffer.add(prev);
	}
//	android.prev();
}

void uartRequestInterrupt() {
	serialReciver.reciveInterruptHandler();
}

void resetAdauCmd() {
	dsp.reset();
	dspCommandReciver.setChangedFlag();
	HAL_Delay(100);
	lastVolumeChanged = HAL_GetTick();
}

void cmdInfoToBuffer() {
	if (HAL_GetTick() - lastInfoSend < 30000) {
		return;
	}

	RingBuffer::clearArray(tmp, RingBuffer::getItemSize());
	snprintf(tmp, RingBuffer::getItemSize(), "%s%s%s%s%s%s%c", "[Available commands] ", adauResetCmd, ", ", stmResetCmd, ", ", debugCmd, '\0');
	infoBuffer.add(tmp);

	lastInfoSend = HAL_GetTick();
}

void muteProcessing() {
	// если магнитола устанавливает уровень -94db и ниже, то это == общему mute
	if (volumeLastValue <= -94 || !amplifierStarted) {
		muteRadioFunc(true);
		muteAndroidFunc(true);
	} else {
//		режим с отключаемым андроидом
//		if (extDeviceCommandReciver.isPlaying()) { // Если на чейнджер была послана команда play, то считаем, что звук должен идти с него
//			muteRadioFunc(true);
//			muteAndroidFunc(false);
//		} else if (muteAndroid && muteRadio) {// если пришли сюда, то магнитола только включилась и везде mute, при этом уровень громкости не -94, усилители включены и к чейджеру не обращались
//			muteRadio = true; // принудительно взведем флаг muteRadio и скажем отключить mute
//			muteRadioFunc(false);
//			muteAndroidFunc(true);
//		} else {
//			muteRadioFunc(false);
//			muteAndroidFunc(true);
//		}

		//режим с не отключаемым андроидом
		//если на чейнджер была команда плей, то выключим радио, иначе включим радио
		if (extDeviceCommandReciver.isPlaying()) {
			muteRadioFunc(true);
		} else {
			if (muteAndroid && muteRadio) {
				// если пришли сюда, то магнитола только включилась и везде mute, при этом уровень громкости не -94, усилители включены и к чейджеру не обращались
				muteRadio = true;
			}
			muteRadioFunc(false);
		}
		// андроид включим всегда
		muteAndroidFunc(!androidConnected);
	}
}

void muteRadioFunc(bool mute) {
	if (!muteRadio && mute) {
		dsp.mute(MOD_MUTEANALOG_ALG0_MUTENOSLEWALG1MUTE_ADDR, true,
		MOD_MUTEANALOG_COUNT);
		muteRadio = true;
		infoBuffer.add("[volume] radio mute on");
	} else if (muteRadio && !mute) {
		dsp.mute(MOD_MUTEANALOG_ALG0_MUTENOSLEWALG1MUTE_ADDR, false,
		MOD_MUTEANALOG_COUNT);
		muteRadio = false;
		infoBuffer.add("[volume] radio mute off");
	}
}

void muteAndroidFunc(bool mute) {
	if (!muteAndroid && mute) {
		dsp.mute(MOD_MUTEDIGITAL_ALG0_MUTENOSLEWALG3MUTE_ADDR, true,
		MOD_MUTEDIGITAL_COUNT);
		muteAndroid = true;
		infoBuffer.add("[volume] android mute on");
	} else if (muteAndroid && !mute) {
		dsp.mute(MOD_MUTEDIGITAL_ALG0_MUTENOSLEWALG3MUTE_ADDR, false,
		MOD_MUTEDIGITAL_COUNT);
		muteAndroid = false;
		infoBuffer.add("[volume] android mute off");
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
