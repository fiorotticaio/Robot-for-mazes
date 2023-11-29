/*======================== IMPORTAÇÕES ========================*/
#include "bibliotecas/lcd/lcd.h"
#include "bibliotecas/sensor_ultrasson/sensor_ultrasson.h"
#include "bibliotecas/motor/motor.h"



/*======================== CONSTANTES ========================*/



/*======================== Variáveis ========================*/
/*===== Sensor ultrasson =====*/
long distancia_parede = 0; // Distância anterior medida pelo sensor ultrassom




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
  char distancia[16];
  sprintf(distancia, "%d", distancia_parede);
  writeData(distancia);
}