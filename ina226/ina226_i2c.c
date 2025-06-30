#include "ina226_i2c.h"

//  REGISTERS
#define INA226_CONFIGURATION              0x00
#define INA226_SHUNT_VOLTAGE              0x01
#define INA226_BUS_VOLTAGE                0x02
#define INA226_POWER                      0x03
#define INA226_CURRENT                    0x04
#define INA226_CALIBRATION                0x05
#define INA226_MASK_ENABLE                0x06
#define INA226_ALERT_LIMIT                0x07
#define INA226_MANUFACTURER               0xFE
#define INA226_DIE_ID                     0xFF
__attribute__((weak)) void I2C1_BeginTransmission(uint8_t address,uint8_t I2C_Direction,uint32_t timeout)
{
while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)!=RESET);
I2C_GenerateSTART(I2C1,ENABLE);
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
I2C_Send7bitAddress(I2C1,address<<1,I2C_Direction);
while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
}
__attribute__((weak)) void I2C1_EndTransmission()
{
    I2C_GenerateSTOP(I2C1,ENABLE);
}
__attribute__((weak)) void I2C1_SendByte(uint8_t dataByte,uint32_t timeout)
{
    while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)== RESET);
    I2C_SendData(I2C1,dataByte);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
__attribute__((weak)) void I2C1_ReceiveByte(uint8_t* receivedByte,uint32_t timeout)
{
     while(I2C_GetFlagStatus(I2C1,I2C_FLAG_RXNE)==RESET);
    *receivedByte = I2C_ReceiveData(I2C1);
}

uint16_t INA226_I2C_ReadRegister(INA226_I2C* ina226, uint8_t registerAddress)
{
    uint16_t receivedData;
    I2C1_BeginTransmission(ina226->address,I2C_Direction_Transmitter,INA226_I2C_TIMEOUT);
    I2C1_SendByte(registerAddress,INA226_I2C_TIMEOUT);
    I2C1_EndTransmission();
    ///
    I2C1_BeginTransmission(ina226->address,I2C_Direction_Receiver,INA226_I2C_TIMEOUT);
    I2C1_ReceiveByte((uint8_t*)&receivedData,INA226_I2C_TIMEOUT);
    I2C1_ReceiveByte(((uint8_t*)&receivedData)+1,INA226_I2C_TIMEOUT);
    I2C1_EndTransmission();
    return receivedData;
}
uint16_t INA226_I2C_WriteRegister(INA226_I2C* ina226, uint8_t registerAddress,uint16_t value)
{
    I2C1_BeginTransmission(ina226->address,I2C_Direction_Transmitter,INA226_I2C_TIMEOUT);
    I2C1_SendByte(registerAddress,INA226_I2C_TIMEOUT);
    I2C1_SendByte(*((uint8_t*)&value),INA226_I2C_TIMEOUT);
    I2C1_SendByte(*(((uint8_t*)&value)+1),INA226_I2C_TIMEOUT);
    I2C1_EndTransmission();
}
void INA226_I2C_Init(INA226_I2C* ina226,uint16_t calib)
{
    
    uint16_t conf = 0b0100000100100111;
    INA226_I2C_WriteRegister(ina226,INA226_CONFIGURATION,conf);
    Delay_Ms(100);
    INA226_I2C_WriteRegister(ina226,INA226_CALIBRATION,calib);
    Delay_Ms(100);
}
int16_t INA226_GetShuntUVoltage(INA226_I2C* ina226)
{
    int16_t val =INA226_I2C_ReadRegister(ina226,INA226_SHUNT_VOLTAGE);
    return (val*25)/10;
}
uint16_t INA226_GetBusVoltage(INA226_I2C* ina226)
{
    //Delay_Ms(100);
    return  (((uint32_t)INA226_I2C_ReadRegister(ina226,INA226_BUS_VOLTAGE))*125)/(uint32_t)100;
    //(val*1.25f)-1.0f;
}
int16_t INA226_GetCurrent(INA226_I2C* ina226)
{
    int16_t val = INA226_I2C_ReadRegister(ina226,INA226_CURRENT);
    return val;
}
uint16_t INA226_GetPower(INA226_I2C* ina226)
{
    return ((uint32_t)INA226_I2C_ReadRegister(ina226,INA226_POWER)*125)/(uint32_t)10;
}
uint16_t INA226_GetAlertFlag(INA226_I2C* ina226)
{
  return INA226_I2C_ReadRegister(ina226,INA226_MASK_ENABLE) & 0x001F;
}
void INA226_Configure(INA226_I2C* ina226,uint16_t conf)
{
    INA226_I2C_WriteRegister(ina226,INA226_CONFIGURATION,conf|0b0100000000000000);
}
uint16_t INA226_GetConfiguration(INA226_I2C* ina226)
{
    return INA226_I2C_ReadRegister(ina226,INA226_CONFIGURATION);
}