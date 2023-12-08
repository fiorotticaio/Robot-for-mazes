/*======================== Sensor infravermelho (SI) ========================*/
/*
Valor do pino lendo total branco: 0
Valor do pino lendo total preto: 3800
*/
#define SI_PINO_SL 35
#define SI_PINO_SC 34
#define SI_PINO_SR 39
#define SI_PINO_SLC 32
#define SI_PINO_SRC 36
#define SI_CS_SENSORS 5
#define SI_THRESHOLD 500

void SI_init_sensor_infra();


/*======================== Motor DC (MDC) ========================*/
#define MDC_PINO_E_M1 33
#define MDC_PINO_E_M2 26
#define MDC_PINO_M1 25
#define MDC_PINO_M2 27
#define MDC_PWM1_CH 2 // Direita
#define MDC_PWM2_CH 3 // Esquerda
#define MDC_PWM_FREQ 1000  // 1 kHz
#define MDC_PWM_RES 8      // 8 bits
#define DIREITA 1
#define ESQUERDA 0
#define TRAS -1
#define FRENTE 2
#define TEMPO_DE_VIRAR 750
#define VEL_PADRAO_DIREITA 162
#define VEL_PADRAO_ESQUERDA 92
#define VEL_FREIANDO_DIREITA 148
#define VEL_FREIANDO_ESQUERDA 112

void MDC_init_motores_dc() ;
void MDC_liga_motores();
void MDC_desliga_motores();
void MDC_anda_pra_frente();
void MDC_vira(char direcao);

int MDC_esta_virando = 0;         // 1 está virando, 0 não está virando
int MDC_direcao_esta_virando = ESQUERDA;
int MDC_achou_linha_virando = 0;  //  0: ainda nao achou a linha
                                  //  1: achou a linha, está quase terminando de virar
                                  // -1: terminou de virar
float MDC_kp = 0.0078947368;      // constante proporcional
/* 30 / 3800 = 0.0078947368 */

int vel_m1 = VEL_PADRAO_DIREITA;
int vel_m2 = VEL_PADRAO_ESQUERDA;


/*============================== GIROSCOPIO ==============================*/
#include "Wire.h"
#include <MPU6050_light.h>

#define GIRO_ANGULO_VIRAR 83

void GIRO_init_giroscopio();

MPU6050 mpu(Wire);


/*============================== SERVO ==============================*/
#include <ESP32Servo.h> // não sei se precisa

#define SERV_ANGULO_ESQUERDA 170
#define SERV_ANGULO_CENTRO 90
#define SERV_ANGULO_DIREITA 0
#define SERV_PINO 13
#define SERV_MIN 500
#define SERV_MAX 2500

int SERV_decide_para_onde_virar();

Servo SERV_servo;


/*============================ ULTRASSOM ============================*/
#define USOM_TRIGGER_PIN 16
#define USOM_ECHO_PIN 19
#define USOM_THRESHOLD_PAREDE 30 // centimetros

void USOM_init_sensor_ultrasom();
int USOM_le_distancia();
int USOM_media_das_distancias() ;

int USOM_distancia_parede = 0;


/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  USOM_init_sensor_ultrasom(); // Inicializando o sensor ultrassom
  SERV_servo.attach(SERV_PINO, SERV_MIN, SERV_MAX); // Inicializando o servo motor
  SERV_servo.write(SERV_ANGULO_CENTRO); // Começar com o ultrassom virado pra frente
  SI_init_sensor_infra();      // Inicializando os sensores infra vermelho
  MDC_init_motores_dc();       // Inicializando os motores DC
  GIRO_init_giroscopio();      // Inicializando o giroscópio
}

void loop() {
  /* Lendo os sensores infra vermelho */
  int sl = analogRead(SI_PINO_SL); 
  int sc = analogRead(SI_PINO_SC);
  int sr = analogRead(SI_PINO_SR);
  int slc = analogRead(SI_PINO_SLC);
  int src = analogRead(SI_PINO_SRC);

  Serial.print(sl);
  Serial.print(" ");
  Serial.print(sc);
  Serial.print(" ");
  Serial.print(sr);
  Serial.print(" ");
  Serial.print(slc);
  Serial.print(" ");
  Serial.println(src);


  /* Verificação se precisa virar (encontrou cruzamento) */
  if (!MDC_esta_virando) {
    if (sc > SI_THRESHOLD && slc > SI_THRESHOLD && src > SI_THRESHOLD) { // Verifica cruzamento
            
      MDC_desliga_motores(); // Desliga motores para ler o ultrassom

      MDC_direcao_esta_virando = SERV_decide_para_onde_virar();

      // Se a resposta do ultrassom nao for "pra frente" então precisa virar
      if (MDC_direcao_esta_virando != FRENTE) MDC_esta_virando = 1; 

      SERV_servo.write(SERV_ANGULO_CENTRO); // Voltar com o ultrassom virado pra frente

      MDC_liga_motores(); // Religa motores

      /* Volta velocidade ao normal */
      vel_m1 = VEL_PADRAO_DIREITA;
      vel_m2 = VEL_PADRAO_ESQUERDA;
    }
  }


  /* Caso não precise virar, apenas siga em linha reta */
  if (!MDC_esta_virando) {

    if (slc < SI_THRESHOLD && sc < SI_THRESHOLD && src < SI_THRESHOLD && sl < SI_THRESHOLD && sr < SI_THRESHOLD) { // Está fora da linha, fica parado
      ledcWrite(MDC_PWM1_CH, 128); 
      ledcWrite(MDC_PWM2_CH, 128);

    } else if (sc > SI_THRESHOLD && sl > SI_THRESHOLD && sr > SI_THRESHOLD) { // Está chegando cruzamento, diminui a velocidade
      /* Segue a linha com controle proporcional mas velocidade menor */
      vel_m1 = VEL_FREIANDO_DIREITA;
      vel_m2 = VEL_FREIANDO_ESQUERDA;

      int velocidadeDir = vel_m1 + MDC_kp * sl;
      int velocidadeEsq = vel_m2 - MDC_kp * sr;
        
      ledcWrite(MDC_PWM1_CH, velocidadeDir);
      ledcWrite(MDC_PWM2_CH, velocidadeEsq);

    } else {
      /* Segue a linha com controle proporcional */
      int velocidadeDir = vel_m1 + MDC_kp * sl;
      int velocidadeEsq = vel_m2 - MDC_kp * sr;
        
      ledcWrite(MDC_PWM1_CH, velocidadeDir);
      ledcWrite(MDC_PWM2_CH, velocidadeEsq);
    }


  } else { // Caso esteja virando, então continue virando até terminar
    
    if      (MDC_direcao_esta_virando == ESQUERDA) MDC_vira('L');
    else if (MDC_direcao_esta_virando == DIREITA)  MDC_vira('R');
    else if (MDC_direcao_esta_virando == TRAS)     MDC_vira('B');
    
    /* Volta velocidade ao normal */
    vel_m1 = VEL_PADRAO_DIREITA;
    vel_m2 = VEL_PADRAO_ESQUERDA;

    /* Andada pra frente */
    ledcWrite(MDC_PWM1_CH, vel_m1);
    ledcWrite(MDC_PWM2_CH, vel_m2);
    delay(300);
  }
}


/*======================== Sensor infravermelho (SI) ========================*/
/* Função que inicializa os sensores infravermelhos */
void SI_init_sensor_infra() {
  pinMode(SI_PINO_SL, INPUT);
  pinMode(SI_PINO_SC, INPUT);
  pinMode(SI_PINO_SR, INPUT);
  pinMode(SI_PINO_SLC, INPUT);
  pinMode(SI_PINO_SRC, INPUT);
  pinMode(SI_CS_SENSORS, OUTPUT);
  digitalWrite(SI_CS_SENSORS, HIGH);
}


/*======================== Motor DC (MDC) ========================*/
/* Função que inicializa os motores DC */
void MDC_init_motores_dc() {
  // Configurando pinos de enable dos motores 
  pinMode(MDC_PINO_E_M1, OUTPUT);
  pinMode(MDC_PINO_E_M2, OUTPUT);

  // Configurando pinos do PWM dos motores
  pinMode(MDC_PINO_M1, OUTPUT);
  pinMode(MDC_PINO_M2, OUTPUT);

  // Sincronizando um pino a um canal
  ledcAttachPin(MDC_PINO_M1, MDC_PWM1_CH);
  ledcSetup(MDC_PWM1_CH, MDC_PWM_FREQ, MDC_PWM_RES);

  // Sincronizando um pino a um canal
  ledcAttachPin(MDC_PINO_M2, MDC_PWM2_CH);
  ledcSetup(MDC_PWM2_CH, MDC_PWM_FREQ, MDC_PWM_RES);

  ledcWrite(MDC_PWM1_CH, 128);
  ledcWrite(MDC_PWM2_CH, 128);

  MDC_liga_motores();
}

/* Função que liga os motores (sinal de enable HIGH) */
void MDC_liga_motores(){
  digitalWrite(MDC_PINO_E_M1, HIGH);
  digitalWrite(MDC_PINO_E_M2, HIGH);
}

/* Função que liga os motores (sinal de enable LOW) */
void MDC_desliga_motores(){
  digitalWrite(MDC_PINO_E_M1, LOW);
  digitalWrite(MDC_PINO_E_M2, LOW);
}

/* Função para andar pra frente sem parar */
void MDC_anda_pra_frente(){
  ledcWrite(MDC_PWM1_CH, 157);
  ledcWrite(MDC_PWM2_CH, 97);
}

/* Função que vira o robô para esquerda, direita ou 180 graus */
void MDC_vira(char direcao){
  // 0 - 127 = trás
  // 128 - 255 = frente
  mpu.update();
  float pos_inicial = mpu.getAngleZ();
  float pos_atual = pos_inicial;

  switch(direcao) {
    case 'L':
      // Vira para esquerda
      ledcWrite(MDC_PWM1_CH, 170);
      ledcWrite(MDC_PWM2_CH, 170);
      
      /* Vira um angulo de 85 graus */
      while (pos_atual < pos_inicial + GIRO_ANGULO_VIRAR) {
        mpu.update();
        pos_atual = mpu.getAngleZ();
      }
      break;

    case 'R':
      // Virar para direita
      ledcWrite(MDC_PWM1_CH, 84);
      ledcWrite(MDC_PWM2_CH, 84);

      /* Vira um angulo de 85 graus */
      while (pos_atual > pos_inicial - GIRO_ANGULO_VIRAR) {
        mpu.update();
        pos_atual = mpu.getAngleZ();
      }
      break;

    case 'B':
      // Vira 180 graus
      ledcWrite(MDC_PWM1_CH, 170);
      ledcWrite(MDC_PWM2_CH, 170);
      
      /* Vira um angulo de 85*2 graus */
      while (pos_atual < pos_inicial + GIRO_ANGULO_VIRAR*2) {
        mpu.update();
        pos_atual = mpu.getAngleZ();
      }
      break;

    case 'S':
        // Faz nada
        break;
  }
}


/*=========================== ULTRASSOM ===========================*/
/* Função que inicializa o sensor ultrassom */
void USOM_init_sensor_ultrasom(){
  pinMode(USOM_TRIGGER_PIN, OUTPUT);    // Configura o trigger (manda o pulso)
  pinMode(USOM_ECHO_PIN, INPUT);        // Configura o echo (recebe o pulso)
}

/* Função que retorna a distancia vista no sensor ultrassom */
int USOM_le_distancia(){

  digitalWrite(USOM_TRIGGER_PIN, LOW);  // Começa em zero o pulso
  delayMicroseconds(2);       

  digitalWrite(USOM_TRIGGER_PIN, HIGH); // Manda pulso
  delayMicroseconds(10);

  digitalWrite(USOM_TRIGGER_PIN, LOW);  // Volta pra zero o pulso
  
  float tempo = pulseIn(USOM_ECHO_PIN, HIGH); // Recebe o tempo que demorou para o pulso percorrer tudo (ida e volta) em ms

  /* velocidade do som: 343m/s = 0.0343 cm/us (divide por dois pois é ida e volta) */
  float distancia = 0.01723 * tempo;

  return distancia; 
}

/* Função que retorna a média dos últimos n valores lidos no ultrassom a partir do momento em que foi chamada */
int USOM_media_das_distancias() {
  int n = 5, soma = 0, i = 0;
  for(i = 0; i < n; i++) soma += USOM_le_distancia();
  return soma/n;
}


/*=========================== SERVO ===========================*/
/* Função que retorna a direção em que não há parede de acordo com o sensor ultrassom */
int SERV_decide_para_onde_virar(){
  SERV_servo.write(SERV_ANGULO_ESQUERDA);           // Aponta pra esquerda do carrinho
  delay(500);
  USOM_distancia_parede = USOM_media_das_distancias();  // Lê distancia do ultrassom
  if (USOM_distancia_parede > USOM_THRESHOLD_PAREDE) return ESQUERDA;

  SERV_servo.write(SERV_ANGULO_CENTRO);           // Aponta pra esquerda do carrinho
  delay(500);
  USOM_distancia_parede = USOM_media_das_distancias();  // Lê distancia do ultrassom
  if (USOM_distancia_parede > USOM_THRESHOLD_PAREDE) return FRENTE;

  SERV_servo.write(SERV_ANGULO_DIREITA);            // Aponta pra direita do carrinho
  delay(500);
  USOM_distancia_parede = USOM_media_das_distancias();  // Lê distancia do ultrassom
  if (USOM_distancia_parede > USOM_THRESHOLD_PAREDE) return DIREITA;

  return TRAS;
}


/*============================ GIROSCOPIO ============================*/
/* Função que inicializa os pinos do giroscópio */
void GIRO_init_giroscopio() {
  Wire.begin();
  mpu.begin();
  mpu.calcGyroOffsets();
}