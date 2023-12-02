#include "bibliotecas/motor_dc/motor.h"

void init_motores_dc() {
  // Configurando pinos de enable dos motores 
  pinMode(PINO_E_M1, OUTPUT);
  pinMode(PINO_E_M2, OUTPUT);

  // Configurando pinos do PWM dos motores
  pinMode(PINO_M1, OUTPUT);
  pinMode(PINO_M2, OUTPUT);

  // Sincronizando um pino a um canal
  ledcAttachPin(PINO_M1, PWM1_CH);
  ledcSetup(PWM1_CH, PWM1_FREQ, PWM1_RES);

  // Sincronizando um pino a um canal
  ledcAttachPin(PINO_M2, PWM2_CH);
  ledcSetup(PWM2_CH, PWM1_FREQ, PWM1_RES);
}

// Função que liga os motores (sinal de enable HIGH)
void liga_motores(){
  digitalWrite(PINO_E_M1, HIGH);
  digitalWrite(PINO_E_M2, HIGH);
}

// Função que liga os motores (sinal de enable LOW)
void desliga_motores(){
  digitalWrite(PINO_E_M1, LOW);
  digitalWrite(PINO_E_M2, LOW);
}

// Função para andar pra frente sem parar
void anda_pra_frente(){
  ledcWrite(PWM1_CH, 200);
  ledcWrite(PWM2_CH, 200);
}

// Função que freia, lendo sensores para saber se já pode parar de desacelerar
void freia_ate_parar() {
  // TODO: precisa adicionar dinamica com sensores
  ledcWrite(PWM1_CH, 50);
  ledcWrite(PWM2_CH, 50);
}

// Função que vira o carrinho para esquerda, usando sensores
int vira_pra_esquerda(int sensorInfraLeft, int sensorInfraCenter, int achouLinha){

  // setando enable nos motores
  liga_motores();
  int achou_linha_update = achouLinha;

  // Verificando se o sensor da esquerda achou a linha (significa que deve continuar a virar mas ta quase chegando)
  if (sensorInfraLeft>0) {
    achou_linha_update = 1;  
  }

  // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  if (sensorInfraCenter>0){
    achou_linha_update = -1;
  }

  // Continuar virando para esquerda
  if (achou_linha_update>0){
    ledcWrite(PWM1_CH, 50);
    ledcWrite(PWM2_CH, 200);
  }

  return achou_linha_update;
}

// Função que vira  o carrinho para direita, usando sensores
void vira_pra_direita(){
  // TODO: precisa adicionar dinamica com sensores
  ledcWrite(PWM1_CH, 50);
  ledcWrite(PWM2_CH, 200);
}