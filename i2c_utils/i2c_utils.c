#include "i2c_utils.h"
I2C_ERROR priv_i2c_utils_error= I2C_ERROR_NONE;
I2C_ERROR priv_i2c_utils_checkTimeout(uint32_t deadline)
{
    if(deadline==0) return priv_i2c_utils_error;
    if(deadline<millis()) 
        priv_i2c_utils_error= I2C_ERROR_TIMED_OUT; //<= Enter breakpoint here to catch I2C error immediately!
    return priv_i2c_utils_error;   
}
uint32_t calculateDeadline(uint32_t timeout)
{
    if(timeout==0) return 0;
    return timeout+millis();
}
void I2C1_Enable()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2; // PC1 - SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // opendrain
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1,ENABLE);
}
I2C_ERROR I2C1_GetError()
{
   return priv_i2c_utils_error;
}
void I2C1_ClearError()
{
    priv_i2c_utils_error = I2C_ERROR_NONE;
    I2C1_EndTransmission();
}
void I2C1_BeginTransmission(uint8_t address,uint8_t I2C_Direction,uint32_t timeout)
{
if(priv_i2c_utils_error) return;
uint32_t deadline=calculateDeadline(timeout);
while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)!=RESET)
{
    if(priv_i2c_utils_checkTimeout(deadline)) return;
}
I2C_GenerateSTART(I2C1,ENABLE);
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
{
  if(priv_i2c_utils_checkTimeout(deadline)) return;
}
I2C_Send7bitAddress(I2C1,address<<1,I2C_Direction);
while(!I2C_CheckEvent(I2C1,I2C_Direction==I2C_Direction_Transmitter?I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
{
   if(priv_i2c_utils_checkTimeout(deadline)) return;
}
}
void I2C1_ReceiveByte(uint8_t* receivedByte,uint32_t timeout)
{
    if(priv_i2c_utils_error) return;
   uint32_t deadline=calculateDeadline(timeout);
    while(I2C_GetFlagStatus(I2C1,I2C_FLAG_RXNE)==RESET)
    if(priv_i2c_utils_checkTimeout(deadline)) return;
    *receivedByte = I2C_ReceiveData(I2C1);
}
void I2C1_SendByte(uint8_t dataByte,uint32_t timeout)
{
    if(priv_i2c_utils_error) return;
    uint32_t deadline=calculateDeadline(timeout);
    while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)== RESET) if(priv_i2c_utils_checkTimeout(deadline)) return;
    I2C_SendData(I2C1,dataByte);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)) if(priv_i2c_utils_checkTimeout(deadline)) return;
}
void I2C1_EndTransmission()
{  
    if(priv_i2c_utils_error) return;
    I2C_GenerateSTOP(I2C1,ENABLE);
}