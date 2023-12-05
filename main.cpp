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

void SI_init_sensor_infra();


/*======================== Motor DC (MDC) ========================*/
#define MDC_PINO_E_M1 33
#define MDC_PINO_E_M2 26
#define MDC_PINO_M1 25
#define MDC_PINO_M2 27
#define MDC_PWM1_CH 0 // Direita
#define MDC_PWM2_CH 1 // Esquerda
#define MDC_PWM_FREQ 1000  // 1 kHz
#define MDC_PWM_RES 8      // 8 bits
#define DIREITA 1
#define ESQUERDA 0
#define TRAS -1
#define TEMPO_DE_VIRAR 900

void MDC_init_motores_dc() ;
void MDC_liga_motores();
void MDC_desliga_motores();
void MDC_anda_pra_frente();
void MDC_freia_ate_parar();
void MDC_vira(char direcao);
int MDC_vira_pra_esquerda(int sensorInfraLeft, int sensorInfraCenter, int achouLinha);
int MDC_vira_pra_direita(int sensorInfraRight, int sensorInfraCenter, int achouLinha);
int MDC_vira_180_graus(int sensorInfraLeft, int sensorInfraRight, int sensorInfraCenter, int achouLinha);

int MDC_esta_virando = 0;         // 1 está virando, 0 não está virando
int MDC_direcao_esta_virando = ESQUERDA;
int MDC_achou_linha_virando = 0;  //  0: ainda nao achou a linha
                                  //  1: achou a linha, está quase terminando de virar
                                  // -1: terminou de virar
float MDC_kp = 0.0078947368;      // constante proporcional
/* 30 / 3800 = 0.0078947368 */



/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  SI_init_sensor_infra(); // Inicializando os sensores infra vermelho
  MDC_init_motores_dc(); // Inicializando os motores DC
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


  /* Caso não esteja virando, apenas siga a linha */
  if (!MDC_esta_virando) {
    if (sc > 100 && slc > 100 && src > 100) { // Verifica cruzamento
      Serial.println("Virar!");
      MDC_esta_virando = 1;
      if(slc > 100 && src > 100) { // Cruzamento em T
        MDC_direcao_esta_virando = ESQUERDA; // Sempre vira para esquerda
      } else if (slc > 100) {
        MDC_direcao_esta_virando = ESQUERDA;
      } else if (src > 100) {
        MDC_direcao_esta_virando = DIREITA;
      } else {
        MDC_esta_virando = 0; // Não é um cruzamento
      }
    }
  }
    
  if (!MDC_esta_virando) {
    /* Seguir linha */
    if (sl < 100 && sr < 100) {
      MDC_anda_pra_frente();

    } else if ((sl > 100 && sr < 100) || (sl < 100 && sr > 100)) {
      /* Virar a direita com controle proporcional */
      int velocidadeDir = 170 + MDC_kp * sl;
      int velocidadeEsq = 84 - MDC_kp * sr;
        
      ledcWrite(MDC_PWM1_CH, velocidadeDir);
      ledcWrite(MDC_PWM2_CH, velocidadeEsq);

    } else if (slc < 100 && sc < 100 && src < 100 && sl < 100 && sr < 100) { // Caminho sem saida, fica parado
      ledcWrite(MDC_PWM1_CH, 128);
      ledcWrite(MDC_PWM2_CH, 128);
    }

  } else { // Caso esteja virando, então continue virando até terminar
    
    /* Virando para esquerda */
    if (MDC_direcao_esta_virando == ESQUERDA) {
      MDC_achou_linha_virando = MDC_vira_pra_esquerda(slc, sc, MDC_achou_linha_virando);
      
      /* Terminou de virar */
      if (MDC_achou_linha_virando == -1){
        MDC_esta_virando = 0;
      }

    } else if (MDC_direcao_esta_virando == 1) { // Virando para a direita
      MDC_achou_linha_virando = MDC_vira_pra_direita(src, sc, MDC_achou_linha_virando);
      
      /* Terminou de virar */
      if (MDC_achou_linha_virando == -1){
        MDC_esta_virando = 0;
      }
    }

    /* Virando para trás */
    else if (MDC_direcao_esta_virando == -1) {
      MDC_achou_linha_virando = MDC_vira_180_graus(slc, src, sc, MDC_achou_linha_virando);
      
      /* Terminou de virar */
      if (MDC_achou_linha_virando == -1) {
        MDC_esta_virando = 0;
      }
    }
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
  Serial.println("Entrou");
  ledcWrite(MDC_PWM1_CH, 170);
  ledcWrite(MDC_PWM2_CH, 84);
}

/* Função que freia, lendo sensores para saber se já pode parar de desacelerar */
void MDC_freia_ate_parar() {
  // TODO: precisa adicionar dinamica com sensores
  ledcWrite(MDC_PWM1_CH, 50);
  ledcWrite(MDC_PWM2_CH, 50);
}

/* Função que vira o robô para esquerda, direita ou 180 graus */
void MDC_vira(char direcao){
  // 0 - 127 = trás
  // 128 - 255 = frente
  switch(direcao) {
    case 'L':
        // Vira para esquerda
        ledcWrite(MDC_PWM1_CH, 170);
        ledcWrite(MDC_PWM2_CH, 170);
        delay(TEMPO_DE_VIRAR);
        break;
    case 'R':
        // Virar para direita
        ledcWrite(MDC_PWM1_CH, 84);
        ledcWrite(MDC_PWM2_CH, 84);
        delay(TEMPO_DE_VIRAR);
        break;
    case 'B':
        // Vira 180 graus
        ledcWrite(MDC_PWM1_CH, 170);
        ledcWrite(MDC_PWM2_CH, 170);
        delay(2*TEMPO_DE_VIRAR);
        break;
    case 'S':
        // Faz nada
        break;
  }
}

/* Função que vira o robô para esquerda, usando sensores */
int MDC_vira_pra_esquerda(int sensorInfraLeft, int sensorInfraCenter, int achouLinha){

  MDC_vira('L');

  // int achou_linha_update = achouLinha;

  // // Verificando se o sensor da esquerda achou a linha (significa que deve continuar a virar mas ta quase chegando)
  // if (sensorInfraLeft > 100) {
  //   achou_linha_update = 1;  
  // }

  // // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  // if (sensorInfraCenter > 100) {
  //   achou_linha_update = -1;
  // }

  // // Continuar virando para esquerda
  // if (achou_linha_update > 0){
  //   Serial.println("VIRA!!")
  //   MDC_vira('L');
  // }

  return -1;
}

/* Função que vira  o robô para direita, usando sensores */
int MDC_vira_pra_direita(int sensorInfraRight, int sensorInfraCenter, int achouLinha){
  
  MDC_vira('R');

  // int achou_linha_update = achouLinha;

  // Verificando se o sensor da direita achou a linha (significa que deve continuar a virar mas ta quase chegando)
  // if (sensorInfraRight > 100) {
  //   achou_linha_update = 1;  
  // }

  // // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  // if (sensorInfraCenter > 100) {
  //   achou_linha_update = -1;
  // }

  // // Continuar virando para esquerda
  // if (achou_linha_update > 0) {
  //   MDC_vira('R');
  // }

  return -1;
}

/* Função que vira o robô 180 graus, usando sensores */
int MDC_vira_180_graus(int sensorInfraLeft, int sensorInfraRight, int sensorInfraCenter, int achouLinha){

  MDC_vira('B');

  // int achou_linha_update = achouLinha;

  // Verificando se o sensor da esquerda achou a linha (significa que deve continuar a virar mas ta quase chegando)
  // if (sensorInfraRight > 0 || sensorInfraLeft > 0) {
  //   achou_linha_update = 1;  
  // }

  // // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  // if (sensorInfraCenter > 0) {
  //   achou_linha_update = -1;
  // }

  // // Continuar virando para esquerda
  // if (achou_linha_update > 0) {
  //   MDC_vira('B');
  // }

  return -1;
}