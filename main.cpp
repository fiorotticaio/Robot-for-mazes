/*======================== IMPORTAÇÕES ========================*/
#include "bibliotecas/lcd/lcd.h"
#include "bibliotecas/sensor_ultrasson/sensor_ultrasson.h"
#include "bibliotecas/motor/motor.h"
#include "bibliotecas/servo/servo.h"
#include "bibliotecas/sensor_infra/sensor_infra.h"



/*======================== CONSTANTES ========================*/



/*======================== Variáveis ========================*/
long distancia_parede = 0; // Distância anterior medida pelo sensor ultrassom
char distancia_parede_lcd[16];




/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  /* Inicialização dos pinos */
  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);
  pinMode(RS, OUTPUT);
  pinMode(EN, OUTPUT);
  for(int i = 0; i < 4; i++) {
    pinMode(DATA[i], OUTPUT);
  }
  pinMode(PINO_SL, INPUT);
  pinMode(PINO_SC, INPUT);
  pinMode(PINO_SR, INPUT);
  pinMode(PINO_SLC, INPUT);
  pinMode(PINO_SRC, INPUT);

  /* Inicialização do LCD */
  lcd_init();
  write8bits(CLEAR_DISPLAY);
  write8bits(RETURN_HOME);
  delay(2); // 1.52 ms de delay necessários para o comando de Return Home
  writeData("Distancia us: ");
}

void loop() {
  long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU, distancia_parede);

  // Escrevendo a distância do ultrassom no display
  sprintf(distancia_parede_lcd, "%d", distancia_parede);
  writeData(distancia_parede_lcd);

  /* Dependendo da quantidade de casas decimais, tem que voltar mais o cursor */
  if (i < 10) {
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    
  } else if (i < 100) {
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
  
  } else {
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
  }




  /* Lendo os sensores infra vermelho */
  int sl = digitalRead(PINO_SL);
  int sc = digitalRead(PINO_SC);
  int sr = digitalRead(PINO_SR);
  int slc = digitalRead(PINO_SLC);
  int src = digitalRead(PINO_SRC);
  // Pino analógico de cada um deles?

  /* Seguir linha */
  if (srl == 1 && src == 1) {
    frente();
  } else if (srl == 0 && src == 1) {
    esquerda();
  } else if (srl == 1 && src == 0) {
    direita();
  } else if (srl == 0 && src == 0) {
    parar();
  }

  /* Verifica cruzamento */
  if (sc == 1 && sl == 1 && sr == 1) {
    // É cruzamento
  }

  









  delay(500); // Só para testes
}