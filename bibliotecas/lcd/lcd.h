#ifndef LCD_H
#define LCD_H

#define RS 13
#define EN 12
#define DB7 11
#define DB6 10
#define DB5 9 
#define DB4 8
#define RL_LCD 7

void lcd_init();
void lcd_command(byte command);
void lcd_print_char(char c);
void lcd_print(char *s);

#endif LCD_H