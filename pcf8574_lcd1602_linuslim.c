/*
 * P0 ---> RS
 * P1 ---> RW
 * P2 ---> E
 * P3 ---> Back light
 * P4 ---> D4
 * P5 ---> D5
 * P6 ---> D6
 * P7 ---> D7
 */

/*
 *   P7 |  P6 |  P5 |  P4 |  P3 |  P2 |  P1 |  P0
 *  BD7 | BD6 | BD5 | BD4 |  BL |   E |  RW |  RS
 */

#include "pcf8574_lcd1602_linuslim.h"
#include <stdio.h>
#include <stdarg.h>

#define LCD1602_BL 0b00001000
#define LCD1602_E  0b00000100
#define LCD1602_RW 0b00000010
#define LCD1602_RS 0b00000001

#define LCD1602_BL_E    ( LCD1602_BL | LCD1602_E )
#define LCD1602_BL_e      LCD1602_BL
#define LCD1602_BL_E_RS ( LCD1602_BL | LCD1602_E ) | LCD1602_RS
#define LCD1602_BL_e_RS   LCD1602_BL               | LCD1602_RS

#define LCD1602_DL_4BIT   0b00000000
#define LCD1602_DL_8BIT   0b00010000
#define LCD1602_N_1LINE   0b00000000
#define LCD1602_N_2LINE   0b00001000
#define LCD1602_F_5X8     0b00000000
#define LCD1602_F_5X11    0b00000100

#define LCD1602_WAKE_UP           0b00110000
#define LCD1602_FUNCTION_SET_DL   ( 0x20 | LCD1602_DL_4BIT )
#define LCD1602_FUNCTION_SET_N_F  ( 0x20 | LCD1602_N_2LINE | LCD1602_F_5X8 )
#define LCD1602_DISPLAY_OFF       0b00001000
#define LCD1602_DISPLAY_CLEAR     0b00000001
#define LCD1602_ENTRY_MODE_SET    0b00000110
#define LCD1602_DISPLAY_ON        0b00001100



/*
 * Just For Wake Up Command in The First Three Times
 * The LCD is initially treated as an 8-bit interface.
 * Only DB7~DB4 are used during the Wake-Up sequence.
 * LCD receives:
 * DB7 DB6 DB5 DB4
 *  0   0   1   1
 * E is pulsed:
 * E = 1 → Enable
 * E = 0 → Disable
 */
void LCD1602_Send_WakeUp_Command (LCD1602_HandleTypeDef *LCD1602, uint8_t command)
{
	// Example: command = 0b0011 0000 = Wake Up Comme*nd
	uint8_t data_array[2];

    /*
     * Write Wake Up Command to LCD
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   0   0   1   1   0   0   0   0
     */

    data_array[0] = command |  LCD1602_E; // 0b0011 0100
    data_array[1] = command & ~LCD1602_E; // 0b0011 0000

    HAL_I2C_Master_Transmit((*LCD1602).hi2c, (*LCD1602).address, data_array, 2, HAL_MAX_DELAY);
}



/*
 * Send Command
 * RS = 0
 */
void LCD1602_Send_Command (LCD1602_HandleTypeDef *LCD1602, uint8_t command)
{
	// Example: command = 0b0010 0000 = Set Interface to 4-Bits
    uint8_t upper_nibble;
    uint8_t lower_nibble;
    uint8_t data_array[4];

    upper_nibble = command & 0xF0;        // 0b0010 0000 & 0b1111 0000 = 0b0010 0000
    lower_nibble = (command << 4) & 0xF0; // 0b0000 0000 & 0b1111 0000 = 0b0000 0000

    /*
     * Write Command to LCD
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   x   x   x   x   1   x   0   0
     */

    data_array[0] = upper_nibble | LCD1602_BL_E; // 0b0010 1100
    data_array[1] = upper_nibble | LCD1602_BL_e; // 0b0010 1000
    data_array[2] = lower_nibble | LCD1602_BL_E; // 0b0000 1100
    data_array[3] = lower_nibble | LCD1602_BL_e; // 0b0000 1000

    HAL_I2C_Master_Transmit((*LCD1602).hi2c, (*LCD1602).address, data_array, 4, HAL_MAX_DELAY);
}



/*
 * Send Data
 * RS = 1
 */
void LCD1602_Send_Data (LCD1602_HandleTypeDef *LCD1602, uint8_t data)
{
	// Example: data = 0b0100 0001 = 'A'
    uint8_t upper_nibble;
    uint8_t lower_nibble;
    uint8_t data_array[4];

    upper_nibble = data & 0xF0;        // 0b0100 0001 & 0b1111 0000 = 0b0100 0000
    lower_nibble = (data << 4) & 0xF0; // 0b0001 0000 & 0b1111 0000 = 0b0001 0000

    /*
     * Write Data to RAM
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   x   x   x   x   1   x   0   1
     */

    data_array[0] = upper_nibble | LCD1602_BL_E_RS; // 0b0100 1101
    data_array[1] = upper_nibble | LCD1602_BL_e_RS; // 0b0100 1001
    data_array[2] = lower_nibble | LCD1602_BL_E_RS; // 0b0100 1101
    data_array[3] = lower_nibble | LCD1602_BL_e_RS; // 0b0100 1001

    HAL_I2C_Master_Transmit((*LCD1602).hi2c, (*LCD1602).address, data_array, 4, HAL_MAX_DELAY);
}



/*
 * Send String
 * C String "ABC" = 'A','B','C','\0'
 * Example: LCD1602_Send_String(&MyLCD1602, "Hello World!");
 */
void LCD1602_Send_String(LCD1602_HandleTypeDef *LCD1602, const char *String)
{
    while (*String != '\0')
    {
        LCD1602_Send_Data(LCD1602, *String);
        String++;
    }
}



/*
 * Like <stdio.h> printf
 * Uses vsnprintf() from <stdio.h>
 * Example: LCD1602_Printf(&MyLCD1602, "Temperature: %d", 25);
 */
void LCD1602_Printf(LCD1602_HandleTypeDef *LCD1602, const char *format, ...) //format = "Temperature: %d"
{
    char buffer[32];
    va_list args; //建立参数管理工具
    va_start(args, format); //后面那些可变参数从这里开始
    vsnprintf(buffer, sizeof(buffer), format, args); //负责解析 Temperature = %d 发现%d 从args拿25
    va_end(args);
    LCD1602_Send_String(LCD1602, buffer);
}



/*
 * DDRAM ADDRESS
 *      1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
 * 1 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F
 * 2 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4A 0x4B 0x4C 0x4D 0x4E 0x4F
 *
 *     AC6 AC5 AC4 = 0x07 max
 * AC3 AC2 AC1 AC0 = 0x0F max
 *
 *  P7  P6  P5  P4  P7  P6  P5  P4
 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
 *   1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
 *
 * Example: LCD1602_Set_Cursor(&MyLCD1602, 2, 16);
 */
void LCD1602_Set_Cursor(LCD1602_HandleTypeDef *LCD1602, uint8_t Row, uint8_t Column)
{
	uint8_t DDRAM_Address = 0;

    if (Row < 1 || Row > 2 || Column < 1 || Column > 16) {return; }

	switch(Row)
	{
		case 1: DDRAM_Address = 0x00 + Column - 1; break;
		case 2: DDRAM_Address = 0x40 + Column - 1; break;
		default: return;
	}

	DDRAM_Address |= 0x80; //0b0100 1111 | 0b1000 0000 = 0b1100 1111
	LCD1602_Send_Command(LCD1602, DDRAM_Address);
	HAL_Delay(1);
}



/*
 * Clear Display
 *  P7  P6  P5  P4  P7  P6  P5  P4
 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
 *   0   0   0   0   0   0   0   1
 *
 * Example: LCD1602_Clear_Display(&MyLCD1602);
 */
void LCD1602_Clear_Display(LCD1602_HandleTypeDef *LCD1602)
{
	LCD1602_Send_Command(LCD1602, LCD1602_DISPLAY_CLEAR);
	HAL_Delay(2);
}



/*
 * LCD Configuration Bits
 *
 * Function Set:
 *   DL = 1 ---> 8-bit interface
 *   DL = 0 ---> 4-bit interface /
 *
 *   N = 1 ---> 2-line display /
 *   N = 0 ---> 1-line display
 *
 *   F = 1 ---> 5x11 dots
 *   F = 0 ---> 5x8 dots /
 *
 * Display ON/OFF Control:
 *   D = 1 ---> Display ON /
 *   D = 0 ---> Display OFF
 *
 *   C = 1 ---> Cursor ON
 *   C = 0 ---> Cursor OFF /
 *
 *   B = 1 ---> Cursor blinking ON
 *   B = 0 ---> Cursor blinking OFF /
 *
 * Entry Mode Set:
 *   I/D = 1 ---> Increment DDRAM address /
 *   I/D = 0 ---> Decrement DDRAM address
 *
 *   SH = 1 ---> Shift display
 *   SH = 0 ---> No display shift /
 */
void LCD1602_Initialization_4BitsMode (LCD1602_HandleTypeDef *LCD1602)
{
	// Power On
	HAL_Delay(20);

	/*
	 * Wake Up 1
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   0   0   1   1   -   -   0   0
	 * Interface = 8-Bits
	 */
	LCD1602_Send_WakeUp_Command(LCD1602, LCD1602_WAKE_UP);
	HAL_Delay(1);

	/*
	 * Wake Up 2
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   0   0   1   1   -   -   0   0
	 * Interface = 8-Bits
	 */
	LCD1602_Send_WakeUp_Command(LCD1602, LCD1602_WAKE_UP);
	HAL_Delay(1);

	/*
	 * Wake Up 3
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   0   0   1   1   -   -   0   0
	 * Interface = 8-Bits
	 */
	LCD1602_Send_WakeUp_Command(LCD1602, LCD1602_WAKE_UP);
	HAL_Delay(1);

	/*
	 * Function Set
	 * Set to 4-Bits Interface
	 *  P7  P6  P5  P4  P3  P2  P1  P0
	 * BD7 BD6 BD5 BD4  BL   E  RW  RS
	 *   0   0   1   DL  N   F   -   -
	 * Data Length, DL = 1(8-Bits) / 0(4-Bits)
	 * Interface = 8-Bits ---> 4-Bits
	 */
	LCD1602_Send_WakeUp_Command(LCD1602, LCD1602_FUNCTION_SET_DL);
	HAL_Delay(1);

	/*
	 * Function Set
	 * Set Number of Line and Character Font
	 *  P7  P6  P5  P4  P7  P6  P5  P4
	 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
	 *   0   0   1   DL  N   F   -   -
	 * Number of Display Line,  N = 1(2-Line) / 0(1-Line)
	 *      Display Font Type,  F = 1(5*11 dots) / 0(5*8 dots)
	 * Interface = 4-Bits
	 */
	LCD1602_Send_Command(LCD1602, LCD1602_FUNCTION_SET_N_F);
	HAL_Delay(1);

	/*
	 * Display Off
	 *  P7  P6  P5  P4  P7  P6  P5  P4
	 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
	 *   0   0   0   0   1   D   C   B
	 *    Display, D = 1(On) / 0(Off)
	 *     Cursor, C = 1(On) / 0(Off)
	 *   Blinking, B = 1(On) / 0(Off)
	 * Interface = 4-Bits
	 */
	LCD1602_Send_Command(LCD1602, LCD1602_DISPLAY_OFF);
	HAL_Delay(2);

	/*
	 * Display Clear
	 *  P7  P6  P5  P4  P7  P6  P5  P4
	 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
	 *   0   0   0   0   0   0   0   1
	 * Interface = 4-Bits
	 */
	LCD1602_Send_Command(LCD1602, LCD1602_DISPLAY_CLEAR);
	HAL_Delay(2);

	/*
	 * Entry Mode Set
	 *  P7  P6  P5  P4  P7  P6  P5  P4
	 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
	 *   0   0   0   0   0   1 I/D  SH
	 * Increment/Decrement, I/D = 1(Increment) / 0(Decrement)
	 *       Shift Display,  SH = 1(Shift)     / 0(Hold)
	 * Interface = 4-Bits
	 */
	LCD1602_Send_Command(LCD1602, LCD1602_ENTRY_MODE_SET);
	HAL_Delay(1);

	/*
	 * Display On
	 *  P7  P6  P5  P4  P7  P6  P5  P4
	 * BD7 BD6 BD5 BD4 BD3 BD2 BD1 BD0
	 *   0   0   0   0   1   D   C   B
	 *  Display, D = 1(On) / 0(Off)
	 *   Cursor, C = 1(On) / 0(Off)
	 * Blinking, B = 1(On) / 0(Off)
	 * Interface = 4-Bits
	 */
	LCD1602_Send_Command(LCD1602, LCD1602_DISPLAY_ON);
	HAL_Delay(2);
}
