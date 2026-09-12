#ifndef PCF8574_LCD1602_LINUSLIM_H
#define PCF8574_LCD1602_LINUSLIM_H

#include "main.h"

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t address;
} LCD1602_HandleTypeDef;

void LCD1602_Send_Command (
		LCD1602_HandleTypeDef *LCD1602,
		uint8_t command
		);

void LCD1602_Send_Data (
		LCD1602_HandleTypeDef *LCD1602,
		uint8_t data
		);

void LCD1602_Send_String(
		LCD1602_HandleTypeDef *LCD1602,
		const char *String
		);

void LCD1602_Printf(
		LCD1602_HandleTypeDef *LCD1602,
		const char *format,
		...
		);

void LCD1602_Set_Cursor(
		LCD1602_HandleTypeDef *LCD1602,
		uint8_t Row,
		uint8_t Column
		);

void LCD1602_Clear_Display(
		LCD1602_HandleTypeDef *LCD1602
		);

void LCD1602_Initialization_4BitsMode (
		LCD1602_HandleTypeDef *LCD1602
		);

#endif
