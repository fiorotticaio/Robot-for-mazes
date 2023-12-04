/*======================== Sensor infravermelho (SI) ========================*/
/*
Valor do pino lendo total branco: 0
Valor do pino lendo total preto: 3800
*/

#define SI_PINO_SL 6
#define SI_PINO_SC 5
#define SI_PINO_SR 4
#define SI_PINO_SLC 3
#define SI_PINO_SRC 7

void SI_init_sensor_infra();


/*======================== Motor DC (MDC) ========================*/
#define MDC_PINO_E_M1 33
#define MDC_PINO_E_M2 26
#define MDC_PINO_M1 25
#define MDC_PINO_M2 27
#define MDC_PWM1_CH 0 //FIXME: Esquerda ou direita? 
#define MDC_PWM2_CH 1 //FIXME: Esquerda ou direita? vamos descobrir...
#define MDC_PWM1_FREQ 1000  // 1 kHz
#define MDC_PWM1_RES 8      // 8 bits
#define DIREITA 1
#define ESQUERDA 0
#define TRAS -1

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
int MDC_direcao_esta_virando = DIREITA;
int MDC_achou_linha_virando = 0;  //  0: ainda nao achou a linha
                                  //  1: achou a linha, está quase terminando de virar
                                  // -1: terminou de virar
float MDC_kp = 0.0334210526;      // constante proporcional
/* 127 / 3800 = 0.0334210526 */



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
  
  // FIXME: adicionar um threshold se pa
  // Maior que 0: lendo linha
  // Igual a 0: lendo branco

  /* Caso não esteja virando, apenas siga a linha */
  if (!MDC_esta_virando) {
    if (sc > 0 && sl > 0 && sr > 0) { // Verifica cruzamento
      delay(200);
      MDC_esta_virando = 1;
      if(sl > 0 && sr > 0) { // Cruzamento em T
        MDC_direcao_esta_virando = ESQUERDA; // Sempre vira para esquerda
      } else if (sl > 0) {
        MDC_direcao_esta_virando = ESQUERDA;
      } else if (sr > 0) {
        MDC_direcao_esta_virando = DIREITA;
      } else {
        MDC_esta_virando = 0; // Não é um cruzamento
      }
    }
    
    /* Seguir linha */
    if (slc == 0 && src == 0) {
      MDC_anda_pra_frente();

    } else if ((slc > 0 && src == 0) || (slc == 0 && src > 0)) {
      /* Virar a direita com controle proporcional */
      int velocidadeDir = 127 + MDC_kp * slc;
      int velocidadeEsq = 127 + MDC_kp * src;
      
      ledcWrite(MDC_PWM1_CH, velocidadeEsq);
      ledcWrite(MDC_PWM2_CH, velocidadeDir);

    } else if (sl == 0 && sc == 0 && sr == 0 && slc == 0 && src == 0) { // Caminho sem saida, fica parado
      ledcWrite(MDC_PWM1_CH, 0);
      ledcWrite(MDC_PWM2_CH, 0);
    }
    
  } else { // Caso esteja virando, então continue virando até terminar
    
    /* Virando para esquerda */
    if (MDC_direcao_esta_virando == ESQUERDA) {
      MDC_achou_linha_virando = MDC_vira_pra_esquerda(sl, sc, MDC_achou_linha_virando);
      
      /* Terminou de virar */
      if (MDC_achou_linha_virando == -1){
        MDC_esta_virando = 0;
      }

    } else if (MDC_direcao_esta_virando == 1) { // Virando para a direita
      MDC_achou_linha_virando = MDC_vira_pra_direita(sr, sc, MDC_achou_linha_virando);
      
      /* Terminou de virar */
      if (MDC_achou_linha_virando == -1){
        MDC_esta_virando = 0;
      }
    }

    /* Virando para trás */
    else if (MDC_direcao_esta_virando == -1) {
      MDC_achou_linha_virando = MDC_vira_180_graus(sl, sr, sc, MDC_achou_linha_virando);
      
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
  ledcSetup(MDC_PWM1_CH, MDC_PWM1_FREQ, MDC_PWM1_RES);

  // Sincronizando um pino a um canal
  ledcAttachPin(MDC_PINO_M2, MDC_PWM2_CH);
  ledcSetup(MDC_PWM2_CH, MDC_PWM1_FREQ, MDC_PWM1_RES);
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
  ledcWrite(MDC_PWM1_CH, 200);
  ledcWrite(MDC_PWM2_CH, 200);
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
        ledcWrite(MDC_PWM1_CH, 54);
        ledcWrite(MDC_PWM2_CH, 200);
        delay(200);
        break;
    case 'R':
        // Virar para direita
        ledcWrite(MDC_PWM1_CH, 200);
        ledcWrite(MDC_PWM2_CH, 54);
        delay(200);
        break;
    case 'B':
        // Vira 180 graus
        ledcWrite(MDC_PWM1_CH, 54);
        ledcWrite(MDC_PWM2_CH, 200);
        delay(400);
        break;
    case 'S':
        // Faz nada
        break;
  }
}

/* Função que vira o robô para esquerda, usando sensores */
int MDC_vira_pra_esquerda(int sensorInfraLeft, int sensorInfraCenter, int achouLinha){

  // setando enable nos motores
  MDC_liga_motores();
  int achou_linha_update = achouLinha;

  // Verificando se o sensor da esquerda achou a linha (significa que deve continuar a virar mas ta quase chegando)
  if (sensorInfraLeft > 0) {
    achou_linha_update = 1;  
  }

  // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  if (sensorInfraCenter > 0) {
    achou_linha_update = -1;
  }

  // Continuar virando para esquerda
  if (achou_linha_update > 0){
    MDC_vira('L');
  }

  return achou_linha_update;
}

/* Função que vira  o robô para direita, usando sensores */
int MDC_vira_pra_direita(int sensorInfraRight, int sensorInfraCenter, int achouLinha){
  
  // setando enable nos motores
  MDC_liga_motores();
  int achou_linha_update = achouLinha;

  // Verificando se o sensor da direita achou a linha (significa que deve continuar a virar mas ta quase chegando)
  if (sensorInfraRight > 0) {
    achou_linha_update = 1;  
  }

  // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  if (sensorInfraCenter > 0) {
    achou_linha_update = -1;
  }

  // Continuar virando para esquerda
  if (achou_linha_update > 0) {
    MDC_vira('R');
  }

  return achou_linha_update;
}

/* Função que vira o robô 180 graus, usando sensores */
int MDC_vira_180_graus(int sensorInfraLeft, int sensorInfraRight, int sensorInfraCenter, int achouLinha){
  
  // setando enable nos motores
  MDC_liga_motores();
  int achou_linha_update = achouLinha;

  // Verificando se o sensor da esquerda achou a linha (significa que deve continuar a virar mas ta quase chegando)
  if (sensorInfraRight > 0 || sensorInfraLeft > 0) {
    achou_linha_update = 1;  
  }

  // Verificando se o sensor do centro achou a linha (significa que deve parar de virar)
  if (sensorInfraCenter > 0) {
    achou_linha_update = -1;
  }

  // Continuar virando para esquerda
  if (achou_linha_update > 0) {
    MDC_vira('B');
  }

  return achou_linha_update;
}