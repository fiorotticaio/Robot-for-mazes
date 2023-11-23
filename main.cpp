/*======================== IMPORTAÇÕES ========================*/
#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;


/*======================== CONSTANTES ========================*/
#define PINO_TRIGGER_SU 7 // Trigger sensor ultrassom
#define PINO_ECHO_SU 8 // Echo sensor ultrassom

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
long distancia_anterior = 0; // Distância anterior medida pelo sensor ultrassom

/*===== LCD =====*/
int colunas_lcd = 16; // Número de colunas do display LCD
int linhas_lcd = 2; // Número de linhas do display LCD
int endereco_lcd = 0x27; // Endereço do display LCD



/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);

  while(!botao_b.getSingleDebouncedPress()); // Esperando o botão B ser pressionado para começar a calibração
  calibra_sensores(); 
}

void loop() {
  long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU);
  
  ehCruzamento = verificaCruzamento();
  

  /* Caminho do robo livre, segue reto */
  //código do 3pi

  int16_t posicao = sensores_linha.readLineBlack(valores_sensores);
  int16_t erro = posicao - 2000;

  int16_t diff_velocidade = erro * (int32_t)coef_proporcional / 256  + (erro - ultimo_erro) * (int32_t)coef_derivativo / 256;

  ultimo_erro = erro;

  int16_t velocidade_esq = (int16_t)velocidade_base + diff_velocidade;
  int16_t velocidade_dir = (int16_t)velocidade_base - diff_velocidade;

  velocidade_esq = constrain(velocidade_esq, velocidade_min, (int16_t)velocidade_max);
  velocidade_dir = constrain(velocidade_dir, velocidade_min, (int16_t)velocidade_max);

  motores.setSpeeds(velocidade_esq, velocidade_dir);
}


/*======================== FUNÇÕES AUXILIARES ========================*/
long mede_distancia_su(int pino_trigger, int pino_echo) {
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

void calibra_sensores(){
  //TODO: implementar display
  //display.clear();

  // espera 1 segundo e começa a calibração
  // rodando e passando os sensores por cima da linha
  delay(1000);
  for(uint16_t i = 0; i < 80; i++){
    if (i > 20 && i <= 60)
    {
      motores.setSpeeds(-(int16_t)velocidade_calibracao, velocidade_calibracao);
    }
    else
    {
      motores.setSpeeds(velocidade_calibracao, -(int16_t)velocidade_calibracao);
    }

    sensores_linha.calibrate();
  }

  motores.setSpeeds(0, 0);
}

bool verificaCruzamento(){

  // lê sensor de cruzamento esquerdo 
  // se tiver detectando linha preta, então é cruzamento

  // Check for left and right exits.
  if(valores_sensores[0] > 100) found_left = 1;
  if(valores_sensores[4] > 100) found_right = 1;

  // Drive straight a bit more - this is enough to line up our
  // wheels with the intersection.
  motores.setSpeeds(40,40);
  delay(200);

  // Check for a straight exit.
  int16_t posicao = sensores_linha.readLineBlack(valores_sensores);

  if(valores_sensores[1] > 200 || valores_sensores[2] > 200 || valores_sensores[3] > 200) {
    found_straight = 1;
  }

  // Check for the ending spot.
  // If all three middle sensors are on dark black, we have
  // solved the maze.
  if(valores_sensores[1] > 600 && valores_sensores[2] > 600 && valores_sensores[3] > 600) {
    exit(0);
  }

  // Intersection identification is complete.
  // If the maze has been solved, we can follow the existing
  // path. Otherwise, we need to learn the solution.
  unsigned char dir = select_turn(found_left, found_straight, found_right);
  // Make the turn indicated by the path.
  turn(dir);
}



// This function decides which way to turn during the learning phase of
// maze solving. It uses the variables found_left, found_straight, and
// found_right, which indicate whether there is an exit in each of the
// three directions, applying the "left hand on the wall" strategy.
char select_turn(unsigned char found_left, unsigned char found_straight, unsigned char found_right) {
  // Make a decision about how to turn. The following code
  // implements a left-hand-on-the-wall strategy, where we always
  // turn as far to the left as possible.
  if      (found_left)     return 'L';
  else if (found_straight) return 'S';
  else if (found_right)    return 'R';
  else                     return 'B';
}


void turn(char dir) {
  switch(dir) {
    case 'L':
      // Turn left.
      setSpeed(-80,80);
      delay(200);
      break;
    case 'R':
      // Turn right.
      setSpeed(80,-80);
      delay(200);
      break;
    case 'B':
      // Turn around.
      setSpeed(80,-80);
      delay(400);
      break;
    case 'S':
      // Don't do anything!
      break;
  }
}