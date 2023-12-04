#include "bibliotecas/sensor_ultrasson/sensor_ultrasson.h"

long mede_distancia_su(int pino_trigger, int pino_echo, long distancia_anterior) {
  /*===== HC­SR04 Ultrasonic Sensor =====*/
  
    /* Fazendo o sinal de trigger para o sensor antes da medição de fato */
  digitalWrite(pino_trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pino_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pino_trigger, LOW);

  long duracao = pulseIn(pino_echo, HIGH); // Medindo a duração do pulso

  long distancia = microssegundos_para_cm(duracao); // Converter o tempo em distância

  /* Filtragem com média móvel */
  distancia = (distancia + distancia_anterior) / 2;
  distancia_anterior = distancia;

  return distancia;
}

long microssegundos_para_cm(long microssegundos) {
  // velocidade do som: 334 m/s 
  // 343 m/s == 0.0343 cm/uS = 1/29.1 cm/uS
  // (tempo_viagem / 2) * velocidade do som = distancia
  return microssegundos/2 / 29 ;
}


// programa principal que tava fazendo
  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);


  
long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU, distancia_parede);