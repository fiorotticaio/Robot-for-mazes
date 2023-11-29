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

  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);
  pinMode()
}

void loop() {
  long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU, distancia_parede);
}