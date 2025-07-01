#include "lcd_i2c.h"
#include "string.h"
#include "stdarg.h"
#include "i2c_utils/i2c_utils.h"
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00
//WARNING: if this voids are not defined, then it will use functions below;
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
__attribute__((weak)) uint32_t uintpow(uint32_t base, const uint8_t power)
{
    uint32_t result=base;
    for(uint8_t i=1;i<power;i++)
        result*=base;
    return result;

}
/*__attribute__((weak)) void I2C1_ReceiveByte(uint8_t* receivedByte,uint32_t timeout)
{

}*/
void  setRowOffsets(Lcd_I2C* lcd,uint8_t row0,uint8_t row1, uint8_t row2, uint8_t row3)
{
    lcd->priv_row_offsets[0] = row0;
    lcd->priv_row_offsets[1] = row1;
    lcd->priv_row_offsets[2] = row2;
    lcd->priv_row_offsets[3] = row3;
}
//Nic = Nothing
//Bits: NIC NIC NIC NIC DB7 DB6 DB5 DB4
void privLcdI2Csend4Bits(uint8_t address,uint8_t data,uint8_t ledEn, uint8_t RS)
{
    // DB7 , DB 6 DB 5 DB 4, LED, EN, RW, RS 
    uint8_t dataToSend =(data<<4)&0xF0;
    dataToSend|=(ledEn>0)?0b00001000:0;
    dataToSend|=(RS>0)?0b00000001:0;
    I2C1_BeginTransmission(address,I2C_Direction_Transmitter,LCD_I2C_TIMEOUT);
    I2C1_SendByte(dataToSend,LCD_I2C_TIMEOUT);
    dataToSend|=0b00000100;
    Delay_Us(100);
    I2C1_SendByte(dataToSend,LCD_I2C_TIMEOUT);
    dataToSend&=0b11111011;
    Delay_Us(100);
    I2C1_SendByte(dataToSend,LCD_I2C_TIMEOUT);
    Delay_Us(100);
    I2C1_EndTransmission();
}

void privLcdI2CsendCommand(Lcd_I2C* lcd,uint8_t value)
{
    privLcdI2Csend4Bits(lcd->address,((value>>4)&0x0F),lcd->lcd_i2c_functions&0b10000000,0);  
    privLcdI2Csend4Bits(lcd->address,((value)&0x0F),lcd->lcd_i2c_functions&0b10000000,0);  
}
void privLcdSendDisplayControl(Lcd_I2C* lcd)
{
 privLcdI2CsendCommand(lcd,LCD_DISPLAYCONTROL | (((lcd->lcd_i2c_functions&LCD_I2C_FUNCTION_CURSOR_ON)>0)?LCD_CURSORON:0) | (((lcd->lcd_i2c_functions&LCD_I2C_FUNCTION_BLINKING_ON)>0)?LCD_BLINKON:0) | LCD_DISPLAYON);
}
void Lcd_I2C_Write(Lcd_I2C* lcd,char character)
{
    privLcdI2Csend4Bits(lcd->address,((character>>4)&0x0F),lcd->lcd_i2c_functions&0b10000000,1);  
    privLcdI2Csend4Bits(lcd->address,((character)&0x0F),lcd->lcd_i2c_functions&0b10000000,1);
    //DEBUG ONLY: printf("Sending char: %c,\n",character);
    lcd->priv_current_Cursor_pos++;
    if((lcd->priv_current_Cursor_pos)%(lcd->columns)==0)
    {
            Lcd_I2C_SetCursor(lcd,0,(lcd->priv_current_Cursor_pos)/(lcd->columns));
    }
}
void Lcd_I2C_Init(Lcd_I2C* lcd)
{
    setRowOffsets(lcd,0x00,0x40,0x00+lcd->columns,0x40+lcd->columns);
    lcd->priv_current_Cursor_pos =0;
     privLcdI2Csend4Bits(lcd->address,0b00000011,lcd->lcd_i2c_functions&0b10000000,0); //GO TO 4 BIT MODE TRY ONE
    Delay_Us(4500); //Delay needed
     privLcdI2Csend4Bits(lcd->address,0b00000011,lcd->lcd_i2c_functions&0b10000000,0);//GO TO 4 BIT MODE DISPLAY TRY TWO
    Delay_Us(4500);//Delay needed
     privLcdI2Csend4Bits(lcd->address,0b00000011,lcd->lcd_i2c_functions&0b10000000,0);//GO TO 4 BIT MODE DISPLAY TRY THREE
    Delay_Us(4500);//Delay needed
     privLcdI2Csend4Bits(lcd->address,0b00000010,lcd->lcd_i2c_functions&0b10000000,0);// INITIALIZE
     privLcdI2Csend4Bits(lcd->address,0b00000000,lcd->lcd_i2c_functions&0b10000000,0);// -||-
     privLcdI2Csend4Bits(lcd->address,0b00000110,lcd->lcd_i2c_functions&0b10000000,0);// -||-
     privLcdI2Csend4Bits(lcd->address,0b00000100,lcd->lcd_i2c_functions&0b10000000,0);
     privLcdI2Csend4Bits(lcd->address,0b00001000,lcd->lcd_i2c_functions&0b10000000,0);
    privLcdI2CsendCommand(lcd,LCD_FUNCTIONSET| LCD_4BITMODE |LCD_2LINE | LCD_5x8DOTS);
   // sendCommand(LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON);
    privLcdSendDisplayControl(lcd);
    Lcd_I2C_Clear(lcd);
    privLcdI2CsendCommand(lcd,LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

}
uint8_t Lcd_I2C_IsBacklightEnabled(Lcd_I2C* lcd)
{
    return lcd->lcd_i2c_functions&LCD_I2C_FUNCTION_LED;
}
void Lcd_I2C_EnableBacklight(Lcd_I2C* lcd)
{
    lcd->lcd_i2c_functions|= LCD_I2C_FUNCTION_LED;
    privLcdSendDisplayControl(lcd);
}
void Lcd_I2C_DisableBacklight(Lcd_I2C* lcd)
{
    lcd->lcd_i2c_functions&= ~LCD_I2C_FUNCTION_LED;
    privLcdSendDisplayControl(lcd);
}
void Lcd_I2C_EnableCursor(Lcd_I2C* lcd)
{
    lcd->lcd_i2c_functions|=LCD_I2C_FUNCTION_CURSOR_ON;
    lcd->lcd_i2c_functions|=LCD_I2C_FUNCTION_BLINKING_ON;
    privLcdSendDisplayControl(lcd);
}
void Lcd_I2C_DisableCursor(Lcd_I2C* lcd)
{
    lcd->lcd_i2c_functions&= ~LCD_I2C_FUNCTION_CURSOR_ON;
    lcd->lcd_i2c_functions&= ~LCD_I2C_FUNCTION_BLINKING_ON;
    privLcdSendDisplayControl(lcd);
}
void Lcd_I2C_Print(Lcd_I2C* lcd,const char* str)
{
    Lcd_I2C_PrintDelayed( lcd,str,0);
}
void Lcd_I2C_PrintDelayed(Lcd_I2C* lcd,const char* str,uint16_t charDelay)
{
    for(int i=0;i<strlen(str);i++)
    {
        Lcd_I2C_Write(lcd,str[i]);
        Delay_Ms(charDelay);
    }
}
char *convert(uint32_t num, uint8_t base) 
{ 
    static char Representation[]= "0123456789ABCDEF";
    static char buffer[50]; 
    char *ptr; 

    ptr = &buffer[49]; 
    *ptr = '\0'; 

    do 
    { 
        *--ptr = Representation[num%base]; 
        num /= base; 
    }while(num != 0);
    return(ptr); 
}
void Lcd_I2C_Printf(Lcd_I2C* lcd,char* format,...)
{
    char *traverse; 
    int i; 
    char *s; 

    //Module 1: Initializing Myprintf's arguments 
    va_list arg; 
    va_start(arg, format); 

    for(traverse = format; *traverse != '\0'; traverse++) 
    { 
        while( *traverse != '%' ) 
        { 
            if(*traverse == '\0')
            {
                va_end(arg);
                return;
            }
            else if(*traverse == '\n')
            {
                Lcd_I2C_SetCursor(lcd,0,(lcd->priv_current_Cursor_pos%lcd->columns)+1);
            }
            else
            {
                Lcd_I2C_Write(lcd,*traverse);
            }
            traverse++; 
        } 

        traverse++; 

        //Module 2: Fetching and executing arguments
        switch(*traverse) 
        { 
            case 'c' : i = va_arg(arg,int);     //Fetch char argument
                        Lcd_I2C_Write(lcd,i);
                        break; 

            case 'd' : i = va_arg(arg,int);         //Fetch Decimal/Integer argument
                        if(i<0) 
                        { 
                            i = -i;
                            Lcd_I2C_Write(lcd,'-'); 
                        } 
                        Lcd_I2C_Print(lcd,convert(i,10));
                        break; 

            case 'o': i = va_arg(arg,unsigned int); //Fetch Octal representation
                        Lcd_I2C_Print(lcd,convert(i,8));
                        break; 

            case 's': s = va_arg(arg,char *);       //Fetch string
                        Lcd_I2C_Print(lcd,s); 
                        break; 

            case 'x': i = va_arg(arg,unsigned int); //Fetch Hexadecimal representation
                        Lcd_I2C_Print(lcd,convert(i,16));
                        break; 
        }   
    } 
    //Module 3: Closing argument list to necessary clean-up
    va_end(arg); 
}

void Lcd_I2C_SetCursor(Lcd_I2C* lcd, uint8_t col, uint8_t row)
{
    if(row>=(lcd->rows))
    {
        row=0;
    }
    if(row>=4)
    {
        row=3;
    }
    lcd->priv_current_Cursor_pos=col+(lcd->columns*row);
    privLcdI2CsendCommand(lcd,LCD_SETDDRAMADDR |(col+lcd->priv_row_offsets[row]));
}
void Lcd_I2C_Clear(Lcd_I2C* lcd)
{
    privLcdI2CsendCommand(lcd,LCD_CLEARDISPLAY);
    Delay_Ms(100);
}

void Lcd_I2C_WriteUint(Lcd_I2C* lcd,uint8_t digits, uint32_t value,uint8_t deleteZeros)
{
            uint32_t i=uintpow(10,digits);
            do{
                i/=10;
                uint32_t nValue = ((value/i)%10);
                if(i!=1 && deleteZeros && nValue==0)
                Lcd_I2C_Write(lcd,' ');
                else 
                {
                deleteZeros =0;
                Lcd_I2C_Write(lcd,nValue+'0');
                }
            }
            while(i>1);       
}
void Lcd_I2C_WriteFloat(Lcd_I2C* lcd,uint8_t digits,uint8_t postDigits, float value,uint8_t deleteZeros)
{
    Lcd_I2C_WriteUint(lcd,digits,(uint16_t)value,deleteZeros);
    Lcd_I2C_Write(lcd,'.');
    for(uint8_t j =0;j<postDigits;j++) value*=10;
    Lcd_I2C_WriteUint(lcd,postDigits,(uint32_t)(value),0);
}
void Lcd_I2C_WriteFixedPoint(Lcd_I2C* lcd,uint8_t digits,uint8_t postDigits, int32_t value,uint8_t fixedPoint, uint8_t deleteZeros)
{
    uint32_t divider = uintpow(10,fixedPoint);
    {
    int32_t beforeDot = value/(int32_t)divider;
    Lcd_I2C_WriteInt(lcd,digits,beforeDot,deleteZeros);
    }
    Lcd_I2C_Write(lcd,'.');
    if(value<0) value*=-1;
    uint32_t afterDot = (value)%divider;
    Lcd_I2C_WriteUint(lcd,postDigits,afterDot,0);
}
void Lcd_I2C_WriteInt(Lcd_I2C* lcd,uint8_t digits, int32_t value,uint8_t deleteZeros)
{
    if(value>0)
    Lcd_I2C_Write(lcd,' ');
    else
    {
    Lcd_I2C_Write(lcd,'-');
    value*=-1;
    }
    Lcd_I2C_WriteUint(lcd,digits,value,deleteZeros);
}