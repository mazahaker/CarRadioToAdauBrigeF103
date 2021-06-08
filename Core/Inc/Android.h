#include "stm32f1xx_hal.h"

class Android
{
public:
	Android(UART_HandleTypeDef *_uartAndroid, UART_HandleTypeDef *_uartLog);

	void connect();
	bool isConnected();
	void resetState();
	void processComand(char *command);

	void play();
	void stop();
	void next();
	void prev();
	void shutdown();

private:
	UART_HandleTypeDef *uartAndroid;
	UART_HandleTypeDef *uartLog;

	bool playing;
	uint32_t pingSysTime = 0;

	void ping();
	bool tryPong();
	uint16_t tryPongTime = 2000;

	enum ConnectionState {
		PING_SEND,
		PONG_WAIT,
		CONNECTED
	} connectionState = PING_SEND;

	enum Commands {
		EMPTY,
		PONG
	} lastCommand = EMPTY;
};
