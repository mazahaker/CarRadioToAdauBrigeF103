#include "DspCommandReciver.h"
#include "LevelMapping.h"

DspCommandReciver::DspCommandReciver(GPIO_TypeDef *clockGPIO, uint16_t clockPin, GPIO_TypeDef *dataGPIO, uint16_t dataPin, UART_HandleTypeDef *uart, void (*_addMessage)(char *msg)) :
		_clockGPIO(clockGPIO), _clockPin(clockPin), _dataGPIO(dataGPIO), _dataPin(dataPin), _uart(uart), addMessage(_addMessage) {
	clearData(buff);

	volumeRead = false;
	faderCh1Read = false;
	faderCh2Read = false;
	equalizerRead = false;

	volumeChng = false;
	faderCh1Chng = false;
	faderCh2Chng = false;
	equalizerChng = false;

	changed = false;

	//	clearData(volume);
	//	clearData(faderCh1);
	//	clearData(faderCh2);
	//	clearData(equalizer);
}

//deprecated
void DspCommandReciver::readDataOld(void) {
	data = HAL_GPIO_ReadPin(_dataGPIO, _dataPin);	// data_Read;
	clock = HAL_GPIO_ReadPin(_clockGPIO, _clockPin);	// clock_Read;
	if (clock) {
		buff[iterator] = data;
		iterator++;

		do {
			data = HAL_GPIO_ReadPin(_dataGPIO, _dataPin);
			endOfData = !HAL_GPIO_ReadPin(_clockGPIO, _clockPin);
		} while (!endOfData);

		if (data) {
			if (iterator == M_SIZE) {
				typeDetectAndSave();
			}
			iterator = 0;
			endOfData = 0;

			clearData(buff);
		}
	}
}

//Вход с clock == 1
bool DspCommandReciver::readPackage(int iterator) {
	//Если итератор больше номера последнего бита, то пакет битый. Выходим
	if (iterator > M_SIZE - 1) {
		return false;
	}

	//считываем значение data в текущую позицию буфера
	buff[iterator] = HAL_GPIO_ReadPin(_dataGPIO, _dataPin);

	//ждем пока опустится clock
	while (HAL_GPIO_ReadPin(_clockGPIO, _clockPin)) {
	}

	//когда clock опустился и data != 1, значит не конец пакета
	//если clock опустился и data == 1, значит конец пакета
	if (!HAL_GPIO_ReadPin(_dataGPIO, _dataPin)) {
		//ждем пока clock снова поднимется
		while (!HAL_GPIO_ReadPin(_clockGPIO, _clockPin))
			;
		//когда clock поднялся - нужно читать след бит, уходим в след уровень рекурсии для чтения
		return readPackage(++iterator);
	} else {
		//Если итератор равен 16 (номер последнего бита в пакете 0...16) то все впорядке иначе пакет битый

//		char tmp[20];
//		itoa(iterator, tmp, 10);
//		if (_uart != NULL) {
//			HAL_UART_Transmit(_uart, (uint8_t*) tmp, strlen(tmp), 10);
//		}
//		if (addMessage != null) {
//			addMessage(tmp);
//		}
		if (iterator == M_SIZE - 1) {
//			if (_uart != NULL) {
//				HAL_UART_Transmit(_uart, (uint8_t*) "PACK\n", 5, 10);
//			}
//			if (addMessage != null) {
//				addMessage("PACK");
//			}
			return true;
		} else {
//			if (_uart != NULL) {
//				HAL_UART_Transmit(_uart, (uint8_t*) "NO PACK\n", 8, 10);
//			}
//			if (addMessage != null) {
//				addMessage("NO PACK");
//			}
			return false;
		}

		//		return iterator == M_SIZE - 1 ? true : false;
		//		HAL_UART_Transmit(_uart, (uint8_t *)"PACK\n", 5, 10);
		//конец пакета
	}
}

void DspCommandReciver::readPackage() {
	//перед чтением пакета проверяем, что clock поднят
	if (!HAL_GPIO_ReadPin(_clockGPIO, _clockPin)) {
//		if (_uart != NULL) {
//			HAL_UART_Transmit(_uart, (uint8_t*) "[m-dsp] R-ERROR: clock not up\n", 30, 2);
//		}
//		if (addMessage != NULL) {
//			addMessage("[m-dsp] R-ERROR: clock not up");
//		}
		return;
	}

	if (readPackage(0)) {
		typeDetectAndSave();
	}

	clearData(buff);
}

void DspCommandReciver::printMsgFunc() {
	int8_t total;
	int8_t vol_attv1 = getLevel(VOLUME_ATTV1, CH2);
	int8_t vol_attv2 = getLevel(VOLUME_ATTV2, CH2);
	int8_t fader_attv1 = getLevel(FADER_FRONT_ATTV1, CH2);
	int8_t fader_attv2 = getLevel(FADER_FRONT_ATTV2, CH2);
	total = vol_attv1 + vol_attv2 + fader_attv1 + fader_attv2;

	char tmp[200];
	for (int i = 0; i < 200; i++) {
		tmp[i] = '\0';
	}
	snprintf(tmp, sizeof tmp, "%s%d%s%d%s%d%s%d%s%d", "[m-dsp] vol_attv1: ", vol_attv1, " vol_attv2: ", vol_attv2, " fader_attv1: ", fader_attv1, " fader_attv2: ", fader_attv2, " total: ", total);

	if (_uart != NULL) {
		HAL_UART_Transmit(_uart, (uint8_t*) tmp, sizeof tmp, 50);
	}

}

void DspCommandReciver::typeDetectAndSave() {
	if (buff[15] == 0 && buff[16] == 0) {
		volumeChng = tranferDataFromBuffer(volume, false);
		volumeRead = true;
	}
	if (buff[15] == 1 && buff[16] == 0) {
		if (buff[0] == 0) {
			faderCh1Chng = tranferDataFromBuffer(faderCh1, false);
			faderCh1Read = true;
		} else {
			faderCh2Chng = tranferDataFromBuffer(faderCh2, false);
			faderCh2Read = true;
		}

	}
	if (buff[15] == 0 && buff[16] == 1) {
		equalizerChng = tranferDataFromBuffer(equalizer, true);
		equalizerRead = true;

		if ((volumeChng || faderCh1Chng || faderCh2Chng || (equalizerChng && eqChangedDetect)) && (volumeRead && faderCh1Read && faderCh2Read && equalizerRead)) {
			volumeChng = faderCh1Chng = faderCh2Chng = equalizerChng = false;
			volumeRead = faderCh1Read = faderCh2Read = equalizerRead = false;
			changed = true;
			printMsgFunc();
			//			printToSerial(_uart);
		}
	}
}

bool DspCommandReciver::tranferDataFromBuffer(int8_t *target, bool eq) {
	bool targetChanged = false;
	for (int i = 0; i < M_SIZE; i++) {
		if (target[i] != buff[i]) {
			target[i] = buff[i];
			if (eq || (i != 0 && i != 1 && i != 14)) {
				targetChanged = true;
			}
		}
	}
	//	if(targetChanged) HAL_UART_Transmit(_uart, (uint8_t *)"Changed\n", 8, 10);
	return targetChanged;
}

void DspCommandReciver::printToSerial(UART_HandleTypeDef *uart) {
	if (uart == NULL) {
		return;
	}
	char nextSrt[] = "\n";
	char space[] = " ";
	char tmp[M_SIZE];
	//	for(int i = 0; i < M_SIZE; i++) {
	//		tmp[i] = target[i] == 1 ? '1' : '0';
	//	}

	int8_t total;

	//	HAL_UART_Transmit(uart, (uint8_t *)tmp, M_SIZE, 10);
	//	HAL_UART_Transmit(uart, (uint8_t *)space, sizeof(space), 10);
	//	clearData(tmp);

	int8_t vol_attv1 = getLevel(VOLUME_ATTV1, CH2);
	HAL_UART_Transmit(uart, (uint8_t*) itoa(vol_attv1, tmp, 10), strlen(tmp), 10);
	HAL_UART_Transmit(uart, (uint8_t*) space, sizeof(space), 10);
	clearData(tmp);

	int8_t vol_attv2 = getLevel(VOLUME_ATTV2, CH2);
	HAL_UART_Transmit(uart, (uint8_t*) itoa(vol_attv2, tmp, 10), strlen(tmp), 10);
	HAL_UART_Transmit(uart, (uint8_t*) space, sizeof(space), 10);
	clearData(tmp);

	int8_t fader_attv1 = getLevel(FADER_FRONT_ATTV1, CH2);
	HAL_UART_Transmit(uart, (uint8_t*) itoa(fader_attv1, tmp, 10), strlen(tmp), 10);
	HAL_UART_Transmit(uart, (uint8_t*) space, sizeof(space), 10);
	clearData(tmp);

	int8_t fader_attv2 = getLevel(FADER_FRONT_ATTV2, CH2);
	HAL_UART_Transmit(uart, (uint8_t*) itoa(fader_attv2, tmp, 10), strlen(tmp), 10);
	HAL_UART_Transmit(uart, (uint8_t*) space, sizeof(space), 10);
	clearData(tmp);

	total = vol_attv1 + vol_attv2 + fader_attv1 + fader_attv2;
	HAL_UART_Transmit(uart, (uint8_t*) itoa(total, tmp, 10), strlen(tmp), 10);

	HAL_UART_Transmit(uart, (uint8_t*) nextSrt, sizeof(nextSrt), 10);
	//
	//		if (target[2] == target[7] &&
	//				target[3] == target[8] &&
	//				target[4] == target[9] &&
	//				target[5] == target[10] &&
	//				target[6] == target[11] &&
	//				target[1] == 1 &&
	//				target[12] == 0 &&
	//				target[13] == 0 &&
	//				target[14] == 0 &&
	//				target[15] == 0 &&
	//				target[16] == 0) {
	//			HAL_UART_Transmit(&huart1, ok, sizeof(ok), 10);
	//		} else {
	//			HAL_UART_Transmit(&huart1, fail, sizeof(fail), 10);
	//		}
}

void DspCommandReciver::clearData(char *array) {
	for (uint8_t i = 0; i < sizeof(array); i++) {
		array[i] = '\0';
	}
}

void DspCommandReciver::clearData(int8_t *array) {
	for (int8_t i = 0; i < M_SIZE; i++) {
		array[i] = -2;
	}
}

//функция получения уровня громкости. параметры тип пакета, канал 1 или 2, массив данных для разбора
int8_t DspCommandReciver::getLevel(uint8_t type, uint8_t channel) {
	//в этом массиве хранятся значения для анализа
	int8_t valuesArrayForCheck[] = { -1, -1, -1, -1 };

	//Ссылка на целевой массив
	int8_t *target;

	//Размеры массива с эталонными значениями для сверки данных из пакета с ними
	uint8_t height;
	uint8_t width;

	//складываем значения для анализа в зависимости от типа пакета
	//из каждого пакета бере данные в нужных позициях
	//кол-во проверяемых элементов до 4 в завистимости от типа пакета
	switch (type) {
	//громкость атт1 канал 1
	case VOLUME_ATTV1:
		if (channel == CH1) {
			valuesArrayForCheck[0] = volume[2];
			valuesArrayForCheck[1] = volume[3];
			valuesArrayForCheck[2] = volume[4];
		} else {
			//громкость атт1 канал 2
			valuesArrayForCheck[0] = volume[7];
			valuesArrayForCheck[1] = volume[8];
			valuesArrayForCheck[2] = volume[9];
		}

		//Так же установим значения размерности целевого массива
		height = VOLUME_ATTV1_H;
		width = VOLUME_ATTV1_W;

		target = (int8_t*) volumeATTV1;
		break;
	case VOLUME_ATTV2:
		if (channel == CH1) {
			valuesArrayForCheck[0] = volume[5];
			valuesArrayForCheck[1] = volume[6];
		} else {
			valuesArrayForCheck[0] = volume[10];
			valuesArrayForCheck[1] = volume[11];
		}

		height = VOLUME_ATTV2_H;
		width = VOLUME_ATTV2_W;

		target = (int8_t*) volumeATTV2;
		break;
	case FADER_FRONT_ATTV1:
		if (channel == CH1) {
			valuesArrayForCheck[0] = faderCh1[2];
			valuesArrayForCheck[1] = faderCh1[3];
			valuesArrayForCheck[2] = faderCh1[4];
			valuesArrayForCheck[3] = faderCh1[5];
		} else {
			valuesArrayForCheck[0] = faderCh2[2];
			valuesArrayForCheck[1] = faderCh2[3];
			valuesArrayForCheck[2] = faderCh2[4];
			valuesArrayForCheck[3] = faderCh2[5];
		}

		height = FADER_ATTV1_H;
		width = FADER_ATTV1_W;

		target = (int8_t*) faderATTV1;
		break;
	case FADER_FRONT_ATTV2:
		if (channel == CH1) {
			valuesArrayForCheck[0] = faderCh1[6];
			valuesArrayForCheck[1] = faderCh1[7];
		} else {
			valuesArrayForCheck[0] = faderCh2[6];
			valuesArrayForCheck[1] = faderCh2[7];
		}

		height = FADER_ATTV2_H;
		width = FADER_ATTV2_W;

		target = (int8_t*) faderATTV2;
		break;
	default:
		break;
	}

	int8_t result = -1;
	//Будем идти по строчно в целевом массиве на каждой итерации
	//отдавая данные в функцию сверки текщих данных с iтой строкой массива
	for (int i = 0; i < height; i++) {
		if (checkElement(valuesArrayForCheck, &target[i * width], i, 0)) {
			result = target[i * width];
			break;
		}
	}

	//	if(result == 0) {
	//		char a[] = "res0 ";
	//		HAL_UART_Transmit(_uart, (uint8_t *)a, sizeof(a), 10);
	//
	//	}

	return result;
}

int8_t DspCommandReciver::getAbsoluteLevel(uint8_t channel) {
	int8_t vol_attv1 = getLevel(VOLUME_ATTV1, channel);
	int8_t vol_attv2 = getLevel(VOLUME_ATTV2, channel);
	int8_t fader_attv1 = getLevel(FADER_FRONT_ATTV1, channel);
	int8_t fader_attv2 = getLevel(FADER_FRONT_ATTV2, channel);

	return vol_attv1 + vol_attv2 + fader_attv1 + fader_attv2;

	//	int8_t absoluteLevel = vol_attv1 + vol_attv2 + fader_attv1 + fader_attv2;
	//	return getCorrectedLevel(absoluteLevel);
}

//функция сверки прешедших данных состокой массива
//Параметры: массив с данными для сверки, ссылка на строку из эталонного массива, индекс текущей обрабатываемой позиции в массивах
int8_t DspCommandReciver::checkElement(int8_t *valuesArrayForCheck, int8_t *targetArray, int8_t targetArrayWidth, int8_t wIdx) {
	//если wIdx равен 4, то уже рекурсивно пройдены все элементы и стоит вернуть успех
	if (wIdx == 4) {
		return 1;
	}

	//если встретили -1 в массиве для сверки, значит его значения уже пройдены и совпали с эталном, возвращаем успех
	if (valuesArrayForCheck[wIdx] == -1) {
		return 1;
	}

	//сравниваем текщий элемент из массива для сверки с эталоным (у эталоннного индекс + 1 тк первым идет значение громкости)
	if (valuesArrayForCheck[wIdx] == targetArray[wIdx + 1]) {
		//если совпали, то проваливаемся в рекурсию для сравнения следующей позиции в массиве
		return checkElement(valuesArrayForCheck, targetArray, targetArrayWidth, wIdx + 1);
	} else {
		return 0;
	}
}

bool DspCommandReciver::isChanged() {
	return changed;
	//	if(changed) {
	//		changed = false;
	//		return true;
	//	}
	//	return false;
}

void DspCommandReciver::resetChangedFlag() {
	changed = false;
}

void DspCommandReciver::setChangedFlag() {
	changed = true;
}
