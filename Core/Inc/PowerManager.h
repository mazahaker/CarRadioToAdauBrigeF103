#include "stm32f1xx_hal.h"

class PowerManager
{
public:
	PowerManager(
			GPIO_TypeDef* _accGPIO,
			uint16_t _accPin,
			GPIO_TypeDef* _relayGPIO,
			uint16_t _relayPin,
			UART_HandleTypeDef *uart,
			void (*_accOnCallback)(),
			void (*_accOffCallback)(),
			void (*_waitOffCallback)(),
			void (*_offCallback)());

	void powerProcessing();

private:
	GPIO_TypeDef* relayGPIO;
	uint16_t relayPin;
	GPIO_TypeDef* accGPIO;
	uint16_t accPin;
	UART_HandleTypeDef *uart;

	void (*accOnCallback)();
	void (*accOffCallback)();
	void (*waitOffCallback)();
	void (*offCallback)();

	enum PowerState {
		ACC_ON,
		ACC_OFF,
		WAIT_ACC,
		WAIT_OFF,
		OFF
	} powerState = WAIT_ACC;

	//in ms
	int waitAccTime;
	int maxAccWaitTime = 15000;

	int waitPowerOffTime;
	int maxPowerOffTime = 10000;

	bool stateChanged = false;

	GPIO_PinState checkAcc();
	void changeState(PowerState newState);
	void uartLog(char *str);

};
