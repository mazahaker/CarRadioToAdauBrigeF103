/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void addToBuffer(char *str);
int getBufferReadIdx();

void dataRequestInterrupt();
void mBusRequestInterrupt();
void volumeProcessing();
void clearPendingInterrupt();
void timeProcessing();
void amplifierProcessing();
void monitorProcessing();

void playTrack();
void stopTrack();
void nextTrack();
void prevTrack();

void accOn();
void accOff();
void waitOff();
void off();

void uartRequestInterrupt();
void resetAdauCmd();
void cmdInfoToSerial();
void cmdInfoToBuffer();

void muteProcessing();
void muteRadioFunc(bool mute);
void muteAndroidFunc(bool mute);
void processI2C();
void processUART();
void executeInputCmd();

void setupReadFlagGPIO(bool enable);
void checkConnection();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define MONITOR_OPEN_Pin GPIO_PIN_15
#define MONITOR_OPEN_GPIO_Port GPIOC
#define ADAU_RESET_Pin GPIO_PIN_0
#define ADAU_RESET_GPIO_Port GPIOA
#define AC_Pin GPIO_PIN_1
#define AC_GPIO_Port GPIOA
#define POWER_RELAY_Pin GPIO_PIN_4
#define POWER_RELAY_GPIO_Port GPIOA
#define AMPLIFIER_RELAY_Pin GPIO_PIN_5
#define AMPLIFIER_RELAY_GPIO_Port GPIOA
#define DSP_CLOCK_Pin GPIO_PIN_6
#define DSP_CLOCK_GPIO_Port GPIOA
#define DSP_CLOCK_EXTI_IRQn EXTI9_5_IRQn
#define DSP_DATA_Pin GPIO_PIN_7
#define DSP_DATA_GPIO_Port GPIOA
#define MBUS_RX_Pin GPIO_PIN_0
#define MBUS_RX_GPIO_Port GPIOB
#define MBUS_RX_EXTI_IRQn EXTI0_IRQn
#define MBUS_TX_Pin GPIO_PIN_1
#define MBUS_TX_GPIO_Port GPIOB
#define ADAU_SCL_Pin GPIO_PIN_10
#define ADAU_SCL_GPIO_Port GPIOB
#define ADAU_SDA_Pin GPIO_PIN_11
#define ADAU_SDA_GPIO_Port GPIOB
#define I2C_EVENT_Pin GPIO_PIN_8
#define I2C_EVENT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
