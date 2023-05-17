//#include <bits/stdc++.h>
#include "stm32f1xx_hal.h"

class Serial
{
public:
	Serial(UART_HandleTypeDef *uart);
    void print(char *str);
    void println(char *str);
    void print(uint8_t *str, int size);
    void println();
    void print(int i);
    void print(uint32_t i);
    void print(float f);
    void print(double f);

  private:
    UART_HandleTypeDef *_uart;
};
