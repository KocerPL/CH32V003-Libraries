#pragma once
#include "ch32v00x_conf.h"
#include "millis/millis.h"
typedef enum 
{
    I2C_ERROR_NONE=0,
    I2C_ERROR_TIMED_OUT=1
} I2C_ERROR;

void I2C1_Enable();
void I2C1_EndTransmission();
I2C_ERROR I2C1_GetError();
void I2C1_ClearError();
void I2C1_BeginTransmission(uint8_t address,uint8_t I2C_Direction,uint32_t timeout);
void I2C1_SendByte(uint8_t dataByte,uint32_t timeout);
void I2C1_ReceiveByte(uint8_t* receivedByte,uint32_t timeout);

