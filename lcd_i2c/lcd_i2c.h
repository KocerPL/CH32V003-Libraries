#pragma once
#include "ch32v00x_conf.h"
/* 
* Lcd_I2C Library for ch32v003 by Kacper Saletnik
* WARNING: If not overriding functions I2C1_BeginTransmission(uint8_t address, uint32_t timeout),I2C1_SendByte(uint8_t dataByte) , I2C1_EndTransmission(), (Available in my i2c_utils library)
* library will not timeout i2c commands!
* NEXT WARNING: This library requires Debug.h included in project(for now), and ch32v00x_conf.h in included folder!
* Below are lcd functions, that enable corresponding lcd functions, for example if you want to enable led,cursor,cursor blinking,and enable display then before initialization
* set lcd_i2c_functions to LCD_I2C_ENABLE | LCD_I2C_FUNCTION_LED | LCD_i2C_FUNCTION_BLINKING_ON. You can also do this using corresponding functions after initialization.
* 
* Please feel free to add any suggestions to my github repository.
*/
#define LCD_I2C_ENABLE 1
#define LCD_I2C_DISABLE 0
#define LCD_I2C_FUNCTION_LED 0b10000000
#define LCD_I2C_FUNCTION_CURSOR_ON 0b01000000
#define LCD_I2C_FUNCTION_BLINKING_ON 0b00100000
//Edit this define to change I2C commands timeout
#define LCD_I2C_TIMEOUT 5000
/**
 * @brief Lcd configuration structure
 * Overwrite only parameters without priv on beggining!
 */
typedef struct 
{
    uint8_t address;
    uint8_t rows;
    uint8_t columns;
    uint8_t lcd_i2c_functions;
    uint8_t priv_row_offsets[4];
    uint8_t priv_current_Cursor_pos;
} Lcd_I2C;
/**
 * @brief Sets cursor on specified row and column
 * If row is out of the display, library automatically jumps to first row("Wraps around").
 * @param lcd Lcd_I2C object
 * @param col Column ordered from 0
 * @param row Row ordered from 0
 */
void Lcd_I2C_SetCursor(Lcd_I2C* lcd, uint8_t col, uint8_t row);
/**
 * @brief Initializes lcd with configuration in Lcd_I2C structure
 * @param lcd Lcd configuration structure
 * @details 
 * Example initialization:
 * Lcd_I2C lcd = {0};
 * lcd.address= 0x27;
 * lcd.columns=16;
 * lcd.rows=2;
 * lcd.lcd_i2c_functions = LCD_I2C_ENABLE | LCD_I2C_FUNCTION_CURSOR_ON | LCD_I2C_FUNCTION_LED;
 * Lcd_I2C_Init(&lcd);
 */
void Lcd_I2C_Init(Lcd_I2C* lcd);
/**
 * @brief Writes one char on specified Lcd
 * Writes on current cursor position.
 * @param lcd Lcd configuration structure
 * @param character Character to write
 */
void Lcd_I2C_Write(Lcd_I2C* lcd,char character);
/**
 * @brief Prints character with choosen delay.
 * Writes on current cursor position.
 * @param lcd Lcd configuration structure
 * @param character Character to write
 */
void Lcd_I2C_PrintDelayed(Lcd_I2C* lcd,const char* str,uint16_t charDelay);
/**
 * @brief Works like regular printf, but prints on lcd
 * 
 * @param lcd Lcd configuration structure
 * @param format Format like in printf(Supports: %c %d %o %s %x).
 * @param ... Arguments like in regular printf.
 */
void Lcd_I2C_Printf(Lcd_I2C* lcd,char* format,...);
/**
 * @brief Prints C string on lcd.
 * 
 * @param lcd Lcd configuration structure
 * @param str String to append.
 */
void Lcd_I2C_Print(Lcd_I2C* lcd,const char* str);
/**
 * @brief Enables cursor visibility on lcd.
 * 
 * @param lcd Lcd configuration structure
 */
void Lcd_I2C_EnableCursor(Lcd_I2C* lcd);
/**
 * @brief Disables cursor visibility on lcd.
 * @param lcd Lcd configuration structure
 */
void Lcd_I2C_DisableCursor(Lcd_I2C* lcd);
/**
 * @brief Clears all lcd, and sets cursor to 0,0.
 * @param lcd Lcd configuration structure
 */
void Lcd_I2C_Clear(Lcd_I2C* lcd);
/**
 * @brief Enables lcd backlight
 * @param lcd Lcd configuration structure
 */
void Lcd_I2C_EnableBacklight(Lcd_I2C* lcd);
/**
 * @brief Disables lcd backlight
 * @param lcd Lcd configuration structure
 */
void Lcd_I2C_DisableBacklight(Lcd_I2C* lcd);
/**
 * @brief Returns zero if backlight is disabled.
 * @param lcd Lcd configuration structure
 */
uint8_t Lcd_I2C_IsBacklightEnabled(Lcd_I2C* lcd);
/**
 * @brief Appends unsigned integer on lcd.
 * 
 * @param lcd Lcd configuration structure
 * @param digits how many digits to append(from right side).
 * @param value Value to show.
 * @param deleteZeros Do delete leading zeros?(0 means no)
 */
void Lcd_I2C_WriteUint(Lcd_I2C* lcd,uint8_t digits, uint32_t value,uint8_t deleteZeros);
/**
 * @brief Writes float on lcd.
 * WARNING: VERY MEMORY EXPENSIVE 10% on ch32v003f4p6.
 * @param lcd Lcd configuration structure
 * @param digits how many digits to append(from right side).
 * @param postDigits how many digits to append after dot(from left side).
 * @param value Value to show.
 * @param deleteZeros Do delete leading zeros?(0 means no)
 */
void Lcd_I2C_WriteFloat(Lcd_I2C* lcd,uint8_t digits,uint8_t postDigits, float value,uint8_t deleteZeros);
/**
 * @brief Writes integer on lcd.
 * @param lcd Lcd configuration structure
 * @param digits how many digits to append(from right side).
 * @param value Value to show.
 * @param deleteZeros Do delete leading zeros?(0 means no)
 */
void Lcd_I2C_WriteInt(Lcd_I2C* lcd,uint8_t digits, int32_t value,uint8_t deleteZeros);
