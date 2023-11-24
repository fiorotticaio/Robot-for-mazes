#include "bibliotecas/lcd/lcd.h"

void lcd_init() {
  /* Configurando os pinos do display */
  pinMode(RS, OUTPUT); 
  pinMode(EN, OUTPUT); 
  pinMode(RL_LCD, OUTPUT); 
  digitalWrite(RL_LCD, HIGH);

  /* Configurando os pinos da comunicação 4-bit */
  pinMode(DB4, OUTPUT);
  pinMode(DB5, OUTPUT);
  pinMode(DB6, OUTPUT);
  pinMode(DB7, OUTPUT);

  delay(20); // Inicialização do LCD

  lcd_command(0x02);	// modo 4 bits
  lcd_command(0x28);	// Inicialização do display
  lcd_command(0x0C);	// Display ON Cursor OFF
  lcd_command(0x06);	// Cursor com auto-incremento 
  lcd_command(0x01);	// Limpa o display
  lcd_command(0x80);	// Cursos na posição inicial
}

void lcd_command(byte command) {
  digitalWrite(RS, LOW);
  digitalWrite(DB4, (command >> 4) & 0x01);
  digitalWrite(DB5, ((command >> 4) >> 1) & 0x01);
  digitalWrite(DB6, ((command >> 4) >> 2) & 0x01);
  digitalWrite(DB7, ((command >> 4) >> 3) & 0x01);

  /* Pulse enable */
  digitalWrite(EN, LOW);
  delayMicroseconds(1);
  digitalWrite(EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(EN, LOW);
  delayMicroseconds(100);

  digitalWrite(DB4, command & 0x01);
  digitalWrite(DB5, (command >> 1) & 0x01);
  digitalWrite(DB6, (command >> 2) & 0x01);
  digitalWrite(DB7, (command >> 3) & 0x01);

  /* Pulse enable */
  digitalWrite(EN, LOW);
  delayMicroseconds(1);
  digitalWrite(EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(EN, LOW);
  delayMicroseconds(100);
}

void lcd_print_char(char c) {
  digitalWrite(RS, HIGH);

  digitalWrite(DB4, (c >> 4) & 0x01);
  digitalWrite(DB5, ((c >> 4) >> 1) & 0x01);
  digitalWrite(DB6, ((c >> 4) >> 2) & 0x01);
  digitalWrite(DB7, ((c >> 4) >> 3) & 0x01);

  /* Pulse enable */
  digitalWrite(EN, LOW);
  delayMicroseconds(1);
  digitalWrite(EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(EN, LOW);
  delayMicroseconds(100);

  digitalWrite(DB4, c & 0x01);
  digitalWrite(DB5, (c >> 1) & 0x01);
  digitalWrite(DB6, (c >> 2) & 0x01);
  digitalWrite(DB7, (c >> 3) & 0x01);
  
  /* Pulse enable */
  digitalWrite(EN, LOW);
  delayMicroseconds(1);
  digitalWrite(EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(EN, LOW);
  delayMicroseconds(100);
}

void lcd_print(char *s) {
  char c;
  while((c = *s++)) {
    lcd_print_char(c);
  }
}