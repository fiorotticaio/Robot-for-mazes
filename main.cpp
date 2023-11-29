/*======================== IMPORTAÇÕES ========================*/
#include "bibliotecas/lcd/lcd.h"
#include "bibliotecas/sensor_ultrasson/sensor_ultrasson.h"



/*======================== CONSTANTES ========================*/
#define NUM_SENSORS 5 // Número de sensores do robô (mudar se for 3)


/*======================== Variáveis ========================*/
/*===== Segue a linha =====*/
unsigned int valores_sensores[NUM_SENSORS];
Buzzer buzzer;
LineSensors sensores_linha;
Motors motores;
ButtonA botao_a;
ButtonB botao_b;
ButtonC botao_c;
int16_t ultimo_erro = 0;
uint16_t velocidade_max = 200;             // Velocidade máxima dos motores
int16_t velocidade_min  = 0;               // Velocidade mínima dos motores
uint16_t velocidade_base = velocidade_max; // Velocidade base dos motores quando o robô está centralizado na linha. Deixar em 0 para testar sem velocidade base.
uint16_t velocidade_calibracao = 60;       // Velocidade dos motores durante a calibração.
uint16_t coef_proporcional = 64;           // Coeficiente do termo proporcional
uint16_t coef_derivativo = 256;            // Coeficiente do termo derivativo

/*===== Realizar cruzamentos =====*/
unsigned char found_left = '\0';
unsigned char found_right = '\0';
unsigned char found_straight = '\0';


/*===== Sensor ultrasson =====*/
long distancia_parede = 0; // Distância anterior medida pelo sensor ultrassom




/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);
}

void loop() {
  long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU, distancia_parede);
}