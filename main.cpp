/*======================== IMPORTAÇÕES ========================*/
#include <LiquidCrystal_I2C.h>


/*======================== CONSTANTES ========================*/
#define PINO_TRIGGER_SU 7 // Trigger sensor ultrassom
#define PINO_ECHO_SU 8 // Echo sensor ultrassom


/*======================== PROGRAMA PRINCIPAL ========================*/
/*===== Variáveis globais =====*/
int colunas_lcd = 16; // Número de colunas do display LCD
int linhas_lcd = 2; // Número de linhas do display LCD
int endereco_lcd = 0x27; // Endereço do display LCD
LiquidCrystal_I2C lcd(0x27, lcd_colunas, lcd_linhas); // Inicializando o display LCD

void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);

  lcd.init(); // Inicializando o display LCD
  lcd.backlight(); // Acendendo o backlight do display LCD
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