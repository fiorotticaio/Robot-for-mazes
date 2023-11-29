#ifndef LCD_H
#define LCD_H

#define RS 13
#define EN 12
#define DATA[] = {};
#define RL_LCD 7

/* Funções */
#define FUNCTION_SET 0x20 // 0010 0000 - 4 bits
#define DISPLAY_CONTROL 0x0C // 0000 1100
#define CLEAR_DISPLAY 0x01 // 0000 0001
#define RETURN_HOME 0x02 // 0000 0010
#define ENTRY_MODE_SET 0x06 // 0000 0110
#define MOVE_DISPLAY_RIGHT 0x1C // 0001 1100
#define MOVE_CURSOR_DOWN 0xC0 // 1100 0000
#define MOVE_CURSOR_LEFT 0x10 // 0001 0000

/* Estados */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define DISPLAY_FUNCTION 0x08


void pulseEnable();
void initLCD();
void write4bits(int value);
void write8bits(int value);
void writeData(char* value);

#endif // LCD_H