#include "bibliotecas/lcd/lcd.h"

void pulseEnable(){
  // Certificando-se de que o pino está BAIXO inicialmente
  digitalWrite(E, LOW);
  delayMicroseconds(1);
  
  // Pulsando o pino de Habilitação (Enable)
  digitalWrite(E, HIGH);
  delayMicroseconds(1);
  digitalWrite(E, LOW);
  delayMicroseconds(100);
}

/* ------------------------------------------------------
	Inicializa o display conforme a Figura 24 do datasheet 
    HD44780U
*/
void initLCD(){
  // Aguardando inicialmente
  delay(40);
  
  //Serial.println("Função set 4 bits 0b0010.");
  digitalWrite(RS, LOW);
  
  // Define a função de interface com 4 bits
  write4bits(FUNCTION_SET >> 4);
  delayMicroseconds(4500); // Um pouco mais que 4,1 ms
  
  // Agora, configuramos:
  // - Número de linhas no display (2 linhas: 16x2)
  // - Tamanho da matriz de pixels (5x8)
  // RS permanece 0 (apenas é 1 ao escrever)
  //Serial.println("Função set 4 bits 0b0010 1000.");
  write8bits(FUNCTION_SET | DISPLAY_FUNCTION);
  
  // Desliga o display
  //Serial.println("Controle ON/OFF do Display 0b0000 1100.");
  write8bits(DISPLAY_CONTROL); 
  
  // Define o modo de entrada
  //Serial.println("Modo de entrada setado 0b0000 0110.");
  write8bits(ENTRY_MODE_SET); 
  
  // Limpa e retorna à posição inicial
  write8bits(CLEAR_DISPLAY);
  write8bits(RETURN_HOME);
  delay(2); // Necessário um atraso de 1,52ms para o comando Return Home
  
  // Agora você está livre para usar o display
}

/* ------------------------------------------------------
	Envia de fato os comandos para o display
*/
void write4bits(int valor) {
  for(int i = 0; i < 4; i++) {
    // Somente o valor correspondente ao bit de interesse
    digitalWrite(DATA[i], (valor>>(3-i)) & 0x1);
  }
  pulseEnable();
}

/* ------------------------------------------------------
	Escreve metade dos dados primeiro, depois a segunda metade
*/
void write8bits(int valor) {
  // Envia a primeira metade dos dados (parte superior):
  write4bits(valor>>4);
  // Envia a última metade dos dados (parte inferior):
  write4bits(valor);
}

void writeData(const char* valor) {
  char c;
  for (int i = 0; i < strlen(valor); i++) {
    c = valor[i];
    
    digitalWrite(RS, HIGH);
    write8bits(c);
    digitalWrite(RS, LOW);
  }
}
