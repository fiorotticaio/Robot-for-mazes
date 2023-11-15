/*======================== IMPORTAÇÕES ========================*/
#include <LiquidCrystal_I2C.h>
#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;

/*======================== CONSTANTES ========================*/
#define PINO_TRIGGER_SU 7 // Trigger sensor ultrassom
#define PINO_ECHO_SU 8 // Echo sensor ultrassom
#define NUM_SENSORS 5 // Número de sensores do robô (mudar se for 3)

/*======================== PROGRAMA PRINCIPAL ========================*/
/*===== Variáveis globais =====*/
int colunas_lcd = 16; // Número de colunas do display LCD
int linhas_lcd = 2; // Número de linhas do display LCD
int endereco_lcd = 0x27; // Endereço do display LCD
unsigned int valores_sensores[NUM_SENSORS];
LiquidCrystal_I2C lcd(0x27, lcd_colunas, lcd_linhas); // Inicializando o display LCD
Buzzer buzzer;
LineSensors sensores_linha;
Motors motores;
ButtonA botao_a;
ButtonB botao_b;
ButtonC botao_c;
int16_t ultimo_erro = 0;
uint16_t velocidade_max = 200;                   // Velocidade máxima dos motores
int16_t velocidade_min  = 0;                     // Velocidade mínima dos motores
uint16_t velocidade_base = velocidade_max;       // Velocidade base dos motores quando o robô está centralizado na linha. Deixar em 0 para testar sem velocidade base.
uint16_t velocidade_calibracao = 60;             // Velocidade dos motores durante a calibração.
uint16_t coef_proporcional = 64;                 // Coeficiente do termo proporcional
uint16_t coef_derivativo = 256;                  // Coeficiente do termo derivativo

void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);

  lcd.init(); // Inicializando o display LCD
  lcd.backlight(); // Acendendo o backlight do display LCD

  while(!botao_b.getSingleDebouncedPress()); // Esperando o botão B ser pressionado para começar a calibração
  calibra_sensores(); 
}

void loop() {
  long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU);

  lcd.setCursor(0, 0); // Posicionando o cursor do LCD na primeira coluna, primeira linha
  lcd.print("Distancia: "); // Escrevendo a distância da parede no LCD
  lcd.print(distancia_parede); 
  

  if (distancia_parede < 5) { // Se a distancia for menor que 5 cm
    // Vira o robo para a esqurda 90° e mede de novo
  }

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

  motors.setSpeeds(velocidade_esq, velocidade_dir);
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

  /* Converter o tempo em distância */
  long distancia = microssegundos_para_cm(duracao);
  return distancia;
}

long microssegundos_para_cm(long microssegundos) {
  return microssegundos / 29 / 2;
}

void calibra_sensores()
{
  //todo implementar display
  //display.clear();

  // espera 1 segundo e começa a calibração
  // rodando e passando os sensores por cima da linha
  delay(1000);
  for(uint16_t i = 0; i < 80; i++)
  {
    if (i > 20 && i <= 60)
    {
      motores.setSpeeds(-(int16_t)velocidade_calibracao, velocidade_calibracao);
    }
    else
    {
      motors.setSpeeds(velocidade_calibracao, -(int16_t)velocidade_calibracao);
    }

    sensores_linha.calibrate();
  }
  motores.setSpeeds(0, 0);
}