#include "ExtDeviceCommandReciver.h"
#include <string.h>
#include "ExtDeviceCommandReciverCommands.h"

ExtDeviceCommandReciver::ExtDeviceCommandReciver(GPIO_TypeDef *_rxGPIO, uint16_t _rxPin, GPIO_TypeDef *_txGPIO, uint16_t _txPin, TIM_HandleTypeDef *_timer, UART_HandleTypeDef *_uart,
		void (*_playCallback)(), void (*_stopCallback)(), void (*_nextCallback)(), void (*_prevCallback)(), void (*_addMessage)(char *msg)) :
		rxGPIO(_rxGPIO), rxPin(_rxPin), txGPIO(_txGPIO), txPin(_txPin), timer(_timer), uart(_uart), playCallback(_playCallback), stopCallback(_stopCallback), nextCallback(_nextCallback), prevCallback(
				_prevCallback), addMessage(_addMessage) {

}

//Вход с rxPin == 0
uint32_t ExtDeviceCommandReciver::readCommand() {
	return readCommand(0);
}

void ExtDeviceCommandReciver::convertToHex(uint64_t value, char *str) {
	sprintf(str, "%X", value);
}

//Вход с rxPin == 0
uint32_t ExtDeviceCommandReciver::readCommand(uint32_t result) {
	//ждем пока сигнал поднимается
	resetMicros();
	while (!HAL_GPIO_ReadPin(rxGPIO, rxPin)) {
	}

	//проверяем за какое время поднялся
	uint16_t time = getMicros() < 1000 ? 0 : 1;
	uint8_t currentBit = time;
	result = result << 1 | currentBit;
	//ждем пока сигнал опустится или пройдет время больше ожидаемого времени между командами
	bool nonExitFromTimer = true;
	resetMicros();
	while (HAL_GPIO_ReadPin(rxGPIO, rxPin) && nonExitFromTimer) {
		nonExitFromTimer = getMicros() < 3100;
	}
	//если время меньше, то значит данные еще идут иначе данных больше нет
	if (nonExitFromTimer) {
		result = readCommand(result);
	}
	return result;
}

void ExtDeviceCommandReciver::writeZero() {
	HAL_GPIO_WritePin(txGPIO, txPin, GPIO_PIN_SET);
	delayMicros(600);
	HAL_GPIO_WritePin(txGPIO, txPin, GPIO_PIN_RESET);
	delayMicros(2400);
}

void ExtDeviceCommandReciver::writeOne() {
	HAL_GPIO_WritePin(txGPIO, txPin, GPIO_PIN_SET);
	delayMicros(1800);
	HAL_GPIO_WritePin(txGPIO, txPin, GPIO_PIN_RESET);
	delayMicros(1200);
}

void ExtDeviceCommandReciver::writeCommand(char *command, int size) {
	if (uart != NULL) {
		HAL_UART_Transmit(uart, (uint8_t*) "[mbus] S: ", 10, 2);
		HAL_UART_Transmit(uart, (uint8_t*) command, size, 2);
		HAL_UART_Transmit(uart, (uint8_t*) "\n", 1, 2);
	}

	if (addMessage != NULL) {
		char tmp[200];
		for (int i = 0; i < 200; i++) {
			tmp[i] = '\0';
		}
		snprintf(tmp, sizeof tmp, "%s%s", "[mbus] S: ", command);
		addMessage(tmp);
	}

	delayMicros(5000);
	uint8_t controlSum = 0;
	for (int i = 0; i < size - 1; i++) {
		uint8_t output = toHex(command[i]);
		controlSum = controlSum ^ output;
		writeHexBitWise(output);
	}
	controlSum += 1;
	writeHexBitWise(controlSum);
}

void ExtDeviceCommandReciver::writeHexBitWise(uint8_t command) {
	for (int8_t i = 3; i > -1; i--) {
		uint8_t value = ((command & (1 << i)) >> i);
		if (value == 1) {
			writeOne();
		} else {
			writeZero();
		}
	}
}

//разбор комад
uint32_t ExtDeviceCommandReciver::commandProcessing() {
	//перед чтением пакета проверяем, что clock поднят
	if (HAL_GPIO_ReadPin(rxGPIO, rxPin)) {
//		if (uart != NULL) {
//			HAL_UART_Transmit(uart, (uint8_t*) "[mbus] R-ERROR: line not down\n", 30, 2);
//		}
//		if (addMessage != NULL) {
//			addMessage("[mbus] R-ERROR: line not down");
//		}
		return 0;
	}

	uint32_t command = readCommand();
	char res[20];
	convertToHex(command, res);

	if (uart != NULL) {

		HAL_UART_Transmit(uart, (uint8_t*) "[mbus] R: ", 10, 2);
		HAL_UART_Transmit(uart, (uint8_t*) res, strlen(res), 2);
		HAL_UART_Transmit(uart, (uint8_t*) "\n", 1, 2);
	}

	if (addMessage != NULL) {
		char tmp[200];
		for (int i = 0; i < 200; i++) {
			tmp[i] = '\0';
		}
		snprintf(tmp, sizeof tmp, "%s%s", "[mbus] R: ", res);
		addMessage(tmp);
	}

	switch (command) {
	case 0x58E: {
		pong();
		return command;
	}
	case 0x59D: {
		wakeUp();
		return command;
	}
	default:
		;
	}

	char commandBuf[10];
	//	for (int i = 0; i < bufSize; i++) {
	//		commandBuf[i] = 'X';
	//	}
	convertToHex(command, commandBuf);

	//если мы пришли не по адресу 5 или не по команде Control
	if (commandBuf[0] != '5' || commandBuf[1] != '1') { //0x5130021(5)
		return command;
	}
	//анализ подкоманды
	switch (commandBuf[2]) {
	case '1': { // управление воспроизведением
		if (command == 0x5110127) { //команда играй
			play();
		} else if (command == 0x5114024) { // команда стоп (возможно переход на другое устройство)
			stop();
		}
		break;
	}
	case '3': { // переключение треков
		updateTrack(commandBuf);
		break;
	}
	default:
		;
	}
	return command;
}

void ExtDeviceCommandReciver::sendTime(uint8_t _min, uint8_t _sec) {
	min = _min;
	sec = _sec;
	updateTrackTime();

	if (!playing) {
		return;
	}

	if (uart != NULL) {
		HAL_UART_Transmit(uart, (uint8_t*) "[mbus] send time: ", 18, 2);
	}

	if (addMessage != NULL) {
		addMessage("[mbus] send time: ");
	}

	writeCommand(STATUS_PLAY, sizeof(STATUS_PLAY));
}

void ExtDeviceCommandReciver::resetPlayingFlag() {
	playing = false;
}

bool ExtDeviceCommandReciver::isPlaying() {
	return playing;
}

void ExtDeviceCommandReciver::pong() {
	writeCommand(PONG, sizeof(PONG));
}

void ExtDeviceCommandReciver::wakeUp() {
	writeCommand(HARDWARE_STATUS, sizeof(HARDWARE_STATUS));
	writeCommand(DISC_COUNT, sizeof(DISC_COUNT));
	writeCommand(DISC_INFO_WITH_TRACK_WAKEUP, sizeof(DISC_INFO_WITH_TRACK_WAKEUP));
	writeCommand(STATUS_STOP, sizeof(STATUS_STOP));
	//	writeCommand(STATUS_PLAY, sizeof(STATUS_PLAY));
}

void ExtDeviceCommandReciver::play() {
	writeCommand(STATUS_SEEKING, sizeof(STATUS_SEEKING));
	writeCommand(DISC_INFO_WITH_TRACK_WAKEUP, sizeof(DISC_INFO_WITH_TRACK_WAKEUP));
	writeCommand(STATUS_PLAY, sizeof(STATUS_PLAY));

	updatePlayFlag(true);
}

void ExtDeviceCommandReciver::stop() {
	updateStop();
	writeCommand(STATUS_STOP, sizeof(STATUS_STOP));

	updatePlayFlag(false);
}

void ExtDeviceCommandReciver::updateTrack(char *commandBuf) {
	int oldTrack = track;
	int oldDisk = disk;

	bool changed = false;
	bool next = false;

	//проверяем пришел ли новый номер трека
	int newTrack = toInt(commandBuf[4]) * 10 + toInt(commandBuf[5]);
	if (newTrack != 0 && track != newTrack) {
		track = newTrack;
		updateTrackNumber();
		changed = true;

		if (oldTrack == 99 && newTrack == 1) {
			next = true;
		} else if (oldTrack == 1 && newTrack == 99) {
			next = false;
		} else {
			next = newTrack > oldTrack ? true : false;
		}
	}

	//проверяем пришел ли новый номер диска
	int newDisk = toInt(commandBuf[3]);
	if (newDisk != 0 && disk != newDisk) {
		disk = newDisk;
		updateDiskNumber();
		changed = true;

		if (oldDisk == 6 && newDisk == 1) {
			next = true;
		} else if (oldDisk == 1 && newDisk == 6) {
			next = false;
		} else {
			next = newDisk > oldDisk ? true : false;
		}

		writeCommand(CHANGE_DISK1, sizeof(CHANGE_DISK1));
		writeCommand(CHANGE_DISK2, sizeof(CHANGE_DISK2));
		writeCommand(STATUS_SEEKING, sizeof(STATUS_SEEKING));
		writeCommand(DISC_INFO_WITH_TRACK, sizeof(DISC_INFO_WITH_TRACK));
	}
	writeCommand(STATUS_PLAY, sizeof(STATUS_PLAY));

	updatePlayFlag(true);

	if (changed) {
		if (next) {
			(*nextCallback)();
		} else {
			(*prevCallback)();
		}
	}
}

inline void ExtDeviceCommandReciver::updatePlayFlag(bool play) {
	if (play && playing || !play && !playing) {
		return;
	}

	if (play) {
		playing = true;
		(*playCallback)();
	} else {
		playing = false;
		(*stopCallback)();
	}
}

inline void ExtDeviceCommandReciver::updateTrackTime() {
	STATUS_PLAY[MIN_POS_F] = min / 10 + '0';
	STATUS_PLAY[MIN_POS_S] = min % 10 + '0';
	STATUS_PLAY[SEC_POS_F] = sec / 10 + '0';
	STATUS_PLAY[SEC_POS_S] = sec % 10 + '0';
}

inline void ExtDeviceCommandReciver::updateStop() {
	STATUS_STOP[DISK_POS] = disk + '0';
}

inline void ExtDeviceCommandReciver::updateDiskNumber() {
	CHANGE_DISK2[3] = disk + '0';
	STATUS_SEEKING[DISK_POS] = disk + '0';
	DISC_INFO_WITH_TRACK[2] = disk + '0';
	//	DISC_INFO[2] = disk + '0';
	STATUS_PLAY[DISK_POS] = disk + '0';
	STATUS_STOP[DISK_POS] = disk + '0';
	HARDWARE_STATUS[3] = disk + '0';
}

inline void ExtDeviceCommandReciver::updateTrackNumber() {
	STATUS_SEEKING[TRACK_POS_F] = track / 10 + '0';
	STATUS_SEEKING[TRACK_POS_S] = track % 10 + '0';
	STATUS_PLAY[TRACK_POS_F] = track / 10 + '0';
	STATUS_PLAY[TRACK_POS_S] = track % 10 + '0';
	DISC_INFO_WITH_TRACK[3] = track / 10 + '0';
	DISC_INFO_WITH_TRACK[4] = track % 10 + '0';
}

//TIMER FUNC
void ExtDeviceCommandReciver::startTimer() {
	HAL_TIM_Base_Start_IT(timer); // запуск таймера
}

uint16_t ExtDeviceCommandReciver::getMicros() {
	return __HAL_TIM_GET_COUNTER(timer);
}

void ExtDeviceCommandReciver::resetMicros() {
	__HAL_TIM_SET_COUNTER(timer, 0);  // устанавливаем значение счетчика на 0
}

void ExtDeviceCommandReciver::delayMicros(uint16_t us) {
	__HAL_TIM_SET_COUNTER(timer, 0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(timer) < us)
		;  // wait for the counter to reach the us input in the parameter
}

