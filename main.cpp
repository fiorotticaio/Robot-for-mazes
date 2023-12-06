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
#define SI_CS_SENSOLCD_RS 5
#define SI_THRESHOLD 100

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
#define FRENTE 2
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


/*============================== SERVO ==============================*/
#include <ESP32Servo.h> // não sei se precisa

#define SERV_ANGULO_ESQUERDA 0
#define SERV_ANGULO_CENTRO 90
#define SERV_ANGULO_DIREITA 180
#define SERV_PINO 13
#define SERV_MIN 500
#define SERV_MAX 2500

int SERV_decide_para_onde_virar();

Servo SERV_servo;


/*============================ ULTRASSOM ============================*/
#define USOM_TRIGGER_PIN 16
#define USOM_ECHO_PIN 19
#define USOM_THRESHOLD_PAREDE = 15 // centimetros (em tese)

void USOM_init_sensor_ultrasom();
int USOM_le_distancia();
int USOM_media_das_distancias() ;

int USOM_distancia_parede = 0;


/*============================ LCD ============================*/
#define LCD_RS 13
#define LCD_EN 12
#define LCD_DATA[] = {};
#define LCD_RL 7
#define LCD_FUNCTION_SET 0x20 // 0010 0000 - 4 bits
#define LCD_DISPLAY_CONTROL 0x0C // 0000 1100
#define LCD_CLEAR_DISPLAY 0x01 // 0000 0001
#define LCD_RETURN_HOME 0x02 // 0000 0010
#define LCD_ENTRY_MODE_SET 0x06 // 0000 0110
#define LCD_MOVE_DISPLAY_RIGHT 0x1C // 0001 1100
#define LCD_MOVE_CURSOR_DOWN 0xC0 // 1100 0000
#define LCD_MOVE_CURSOR_LEFT 0x10 // 0001 0000
#define LCD_LCD_DISPLAYON 0x04
#define LCD_LCD_DISPLAYOFF 0x00
#define LCD_DISPLAY_FUNCTION 0x08

void LCD_pulse_enable();
void LCD_init();
void LCD_write4bits(int value);
void LCD_write8bits(int value);
void LCD_writeData(char* value);
void LCD_mostra_dados(int sl, int sc, int sr, int slc, int src);

char LCD_distancia_parede[10];
char LCD_sl[10];
char LCD_sc[10];
char LCD_sr[10];
char LCD_slc[10];
char LCD_src[10];


/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  SI_init_sensor_infra();      // Inicializando os sensores infra vermelho
  MDC_init_motores_dc();       // Inicializando os motores DC
  USOM_init_sensor_ultrasom(); // Inicializando o sensor ultrassom
  LCD_init();                  // Inicializando o LCD

  SERV_servo.attach(SERV_PINO, SERV_MIN, SERV_MAX); // Inicializando o servo motor
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


  LCD_mostra_dados(sl, sc, sr, slc, src); // Mostra dados no LCD

  
  /* Verificação se precisa virar (encontrou cruzamento) */
  if (!MDC_esta_virando) {
    if (sc > SI_THRESHOLD && slc > SI_THRESHOLD && src > SI_THRESHOLD) { // Verifica cruzamento

      // TODO: talvez precise freiar antes de desligar os motores?
      // ledcWrite(MDC_PWM1_CH, 128); 
      // ledcWrite(MDC_PWM2_CH, 128);

      MDC_desliga_motores(); // Desliga motores para ler o ultrassom

      MDC_direcao_esta_virando = SERV_decide_para_onde_virar();

      // Se a resposta do ultrassom nao for "pra frente" então precisa virar
      if (MDC_direcao_esta_virando != FRENTE) MDC_esta_virando = 1; 

      MDC_liga_motores(); // Religa motores
    }
  }


  /* Caso não precise virar, apenas siga em linha reta */
  if (!MDC_esta_virando) {
    
    /* Seguir linha */
    if (sl < SI_THRESHOLD && sr < SI_THRESHOLD) {
      MDC_anda_pra_frente();

    } else if ((sl > SI_THRESHOLD && sr < SI_THRESHOLD) || (sl < SI_THRESHOLD && sr > SI_THRESHOLD)) {
      /* Virar a direita com controle proporcional */
      int velocidadeDir = 170 + MDC_kp * sl;
      int velocidadeEsq = 84 - MDC_kp * sr;
        
      ledcWrite(MDC_PWM1_CH, velocidadeDir);
      ledcWrite(MDC_PWM2_CH, velocidadeEsq);

    } else if (slc < SI_THRESHOLD && sc < SI_THRESHOLD && src < SI_THRESHOLD && sl < SI_THRESHOLD && sr < SI_THRESHOLD) { // Caminho sem saida, fica parado
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
  
  tempo = pulseIn(USOM_ECHO_PIN, HIGH); // Recebe o tempo que demorou para o pulso percorrer tudo (ida e volta) em ms

  /* velocidade do som: 343m/s = 0.0343 cm/us (divide por dois pois é ida e volta) */
  distancia = 0.01723 * tempo;

  Serial.print("ULTRASOM: ")
  Serial.println(distancia)

  return distancia; 
}

/* Função que retorna a média dos últimos n valores lidos no ultrassom a partir do momento em que foi chamada */
int USOM_media_das_distancias() {
  int n = 5, soma = 0, i = 0;
  
  for(i=0;i<n;i++){
    soma+=USOM_le_distancia();
    
    //TODO: talvez precise de delay para estabilizar
    // delay(10); 
  }

  return soma/n;
}


/*=========================== SERVO ===========================*/
/* Função que retorna a direção em que não há parede de acordo com o sensor ultrassom */
int SERV_decide_para_onde_virar(){
  SERV_servo.write(SERV_ANGULO_CENTRO);             // Aponta pra frente do carrinho
  USOM_distancia_parede = USOM_media_das_distancias();  // Lê distancia do ultrassom
  if (USOM_distancia_parede > USOM_THRESHOLD_PAREDE) return FRENTE;

  SERV_servo.write(SERV_ANGULO_ESQUERDA);           // Aponta pra esquerda do carrinho
  USOM_distancia_parede = USOM_media_das_distancias();  // Lê distancia do ultrassom
  if (USOM_distancia_parede > USOM_THRESHOLD_PAREDE) return ESQUERDA;

  SERV_servo.write(SERV_ANGULO_DIREITA);            // Aponta pra direita do carrinho
  USOM_distancia_parede = USOM_media_das_distancias();  // Lê distancia do ultrassom
  if (USOM_distancia_parede > USOM_THRESHOLD_PAREDE) return DIREITA;

  return TRAS;
}


/*=========================== LCD ===========================*/
/* Função que mostra os dados no LCD */
void LCD_pulse_enable() {
  // Certificando-se de que o pino está BAIXO inicialmente
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(1);
  
  // Pulsando o pino de Habilitação (Enable)
  digitalWrite(LCD_EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(100);
}

/* Função que inicializa o display conforme a Figura 24 do datasheet  HD44780U */
void LCD_init() {
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_EN, OUTPUT);
  for(int i = 0; i < 4; i++) {
    pinMode(LCD_DATA[i], OUTPUT);
  }

  // Aguardando inicialmente
  delay(40);
  
  //Serial.println("Função set 4 bits 0b0010.");
  digitalWrite(LCD_RS, LOW);
  
  // Define a função de interface com 4 bits
  write4bits(LCD_FUNCTION_SET >> 4);
  delayMicroseconds(4500); // Um pouco mais que 4,1 ms
  
  // Agora, configuramos:
  // - Número de linhas no display (2 linhas: 16x2)
  // - Tamanho da matriz de pixels (5x8)
  // LCD_RS permanece 0 (apenas é 1 ao escrever)
  //Serial.println("Função set 4 bits 0b0010 1000.");
  write8bits(LCD_FUNCTION_SET | LCD_DISPLAY_FUNCTION);
  
  // Desliga o display
  //Serial.println("Controle ON/OFF do Display 0b0000 1100.");
  write8bits(LCD_DISPLAY_CONTROL); 
  
  // Define o modo de entrada
  //Serial.println("Modo de entrada setado 0b0000 0110.");
  write8bits(LCD_ENTRY_MODE_SET); 
  
  // Limpa e retorna à posição inicial
  write8bits(LCD_CLEAR_DISPLAY);
  write8bits(LCD_RETURN_HOME);
  delay(2); // Necessário um atraso de 1,52ms para o comando Return Home
}

/* Função que envia de fato os comandos para o display */
void LCD_write4bits(int valor) {
  for(int i = 0; i < 4; i++) {
    // Somente o valor correspondente ao bit de interesse
    digitalWrite(LCD_DATA[i], (valor>>(3-i)) & 0x1);
  }
  pulseEnable();
}

/* Função que escreve metade dos dados primeiro, depois a segunda metade */
void LCD_write8bits(int valor) {
  // Envia a primeira metade dos dados (parte superior):
  write4bits(valor>>4);
  // Envia a última metade dos dados (parte inferior):
  write4bits(valor);
}

/* Função que escreve os dados no display */
void LCD_writeData(char* valor) {
  char c;
  for (int i = 0; i < strlen(valor); i++) {
    c = valor[i];
    
    digitalWrite(LCD_RS, HIGH);
    write8bits(c);
    digitalWrite(LCD_RS, LOW);
  }
}

/* Função que mostra os dados no LCD */
void LCD_mostra_dados(int sl, int sc, int sr, int slc, int src) {
  write8bits(LCD_CLEAR_DISPLAY);
  write8bits(LCD_RETURN_HOME);
  delay(2); // 1.52 ms de delay necessários para o comando de Return Home
  writeData("Distancia us: ");

  int dist = USOM_media_das_distancias();

  /* Escrevendo a distância do ultrassom no display */
  sprintf(LCD_distancia_parede, "%d", dist);
  writeData(LCD_distancia_parede);

  write8bits(LCD_MOVE_CURSOR_DOWN); // Quabra de linha
  delay(2); //FIXME: não sei se precisa

  /* Escrevendo os dados dos sensores de linha no display */
  sprintf(LCD_sl, "%d", sl);
  sprintf(LCD_sc, "%d", sc);
  sprintf(LCD_sr, "%d", sr);
  sprintf(LCD_slc, "%d", slc);
  sprintf(LCD_src, "%d", src);
  writeData(LCD_sl);
  writeData(" ");
  writeData(LCD_sc);
  writeData(" ");
  writeData(LCD_sr);
  writeData(" ");
  writeData(LCD_slc);
  writeData(" ");
  writeData(LCD_src);

  //TODO: Escrever o nível de bateria no display
}