#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "stdio.h"

class ExtDeviceCommandReciver
{
public:
	ExtDeviceCommandReciver(
			GPIO_TypeDef* _rxGPIO,
			uint16_t _rxPin,
			GPIO_TypeDef* _txGPIO,
			uint16_t _txPin,
			TIM_HandleTypeDef *_timer,
			UART_HandleTypeDef *_uart,
			void (*_playCallback)(),
			void (*_stopCallback)(),
			void (*_nextCallback)(),
			void (*_prevCallback)(),
			void (*_addMessage)(char* msg));

	void startTimer();

	uint32_t readCommand();
	void writeCommand(char *command, int size);
	void convertToHex(uint64_t value, char *str);
	uint32_t commandProcessing();

	void resetPlayingFlag();
	bool isPlaying();

	//Timer
	uint16_t getMicros();
	void resetMicros();
	void delayMicros(uint16_t us);
	void sendTime(uint8_t _min, uint8_t _sec);

private:
	void (*addMessage)(char* msg);

	GPIO_TypeDef* rxGPIO;
	uint16_t rxPin;
	GPIO_TypeDef* txGPIO;
	uint16_t txPin;

	UART_HandleTypeDef *uart;
	TIM_HandleTypeDef *timer;

	uint8_t track = 1;
	uint8_t disk = 1;
	uint8_t min = 0;
	uint8_t sec = 0;

	bool playing = false;

	uint32_t readCommand(uint32_t result);
	void writeZero();
	void writeOne();
	void writeHexBitWise(uint8_t command);

	void pong();
	void wakeUp();
	void play();
	void stop();
	void updateTrack(char *commandBuf);

	void (*playCallback)();
	void (*stopCallback)();
	void (*nextCallback)();
	void (*prevCallback)();

	inline void updateTrackTime();
	inline void updateStop();
	inline void updateDiskNumber();
	inline void updateTrackNumber();
	inline void updatePlayFlag(bool play);

	inline int toInt(char ch) {
		return ch - 48;
	}

	inline unsigned char toHex(char ch) {
		if (ch >= 'A') {
			return ch - 'A' + 0xA & 0x0F;
		} else {
			return ch - '0' & 0x0F;
		}
	}

};
