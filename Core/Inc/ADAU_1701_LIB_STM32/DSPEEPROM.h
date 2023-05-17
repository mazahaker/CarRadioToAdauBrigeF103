#ifndef DSPEEPROM_H
#define DSPEEPROM_H

#include "SigmaDSP.h"
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stdlib.h"

class DSPEEPROM
{
  public:
    DSPEEPROM(I2C_HandleTypeDef *hi2c, uint8_t i2cAddress, uint16_t kbitSize, int8_t ledPin = -1);
    void begin();
    bool ping();
    uint8_t getFirmwareVersion();
    uint8_t writeFirmware(const uint8_t *firmware, uint16_t size, int8_t firmwareVersion = -1);

  private:
    I2C_HandleTypeDef *_hi2c;
    const uint8_t _eepromAddress;     // i2c address for our EEPROM
    const uint16_t _kbitSize;         // Size of our EEPROM in kilobits
    const int8_t _ledPin;             // Pin to toggle while writing to EEPROM

    uint16_t _firmwareVersionAddress; // Holds the current DSP firmware version  
    uint8_t ledCounter = 0x00;        // Keeps track of the LED toggle speed
};

#endif
