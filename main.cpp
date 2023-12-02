/*======================== IMPORTAÇÕES ========================*/
#include "bibliotecas/lcd/lcd.h"
#include "bibliotecas/sensor_ultrasson/sensor_ultrasson.h"
#include "bibliotecas/motor/motor.h"
#include "bibliotecas/servo/servo.h"
#include "bibliotecas/sensor_infra/sensor_infra.h"



/*======================== CONSTANTES ========================*/



/*======================== Variáveis ========================*/
long distancia_parede = 0; // Distância anterior medida pelo sensor ultrassom
char distancia_parede_lcd[16];
int esta_virando = 0;         // 1 está virando, 0 não está virando
int direcao_esta_virando = 1; // 1 direita, 0 esquerda

int achou_linha_virando = 0;  //  0: ainda nao achou a linha
                              //  1: achou a linha, está quase terminando de virar
                              // -1: terminou de virar


/*======================== PROGRAMA PRINCIPAL ========================*/
void setup() {
  Serial.begin(9600); // Inicializando a comunicação serial

  /* Inicialização dos pinos */
  pinMode(PINO_TRIGGER_SU, OUTPUT);
  pinMode(PINO_ECHO_SU, INPUT);
  pinMode(RS, OUTPUT);
  pinMode(EN, OUTPUT);
  for(int i = 0; i < 4; i++) {
    pinMode(DATA[i], OUTPUT);
  }
  pinMode(PINO_SL, INPUT);
  pinMode(PINO_SC, INPUT);
  pinMode(PINO_SR, INPUT);
  pinMode(PINO_SLC, INPUT);
  pinMode(PINO_SRC, INPUT);

  /* Inicialização do LCD */
  lcd_init();
  write8bits(CLEAR_DISPLAY);
  write8bits(RETURN_HOME);
  delay(2); // 1.52 ms de delay necessários para o comando de Return Home
  writeData("Distancia us: ");
}

void loop() {
  long distancia_parede = mede_distancia_su(PINO_TRIGGER_SU, PINO_ECHO_SU, distancia_parede);

  // Escrevendo a distância do ultrassom no display
  sprintf(distancia_parede_lcd, "%d", distancia_parede);
  writeData(distancia_parede_lcd);

  /* Dependendo da quantidade de casas decimais, tem que voltar mais o cursor */
  if (i < 10) {
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    
  } else if (i < 100) {
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
  
  } else {
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
    write8bits(MOVE_CURSOR_LEFT);
    delay(2);
  }


  /* Lendo os sensores infra vermelho */
  int sl = analogRead(PINO_SL); 
  int sc = analogRead(PINO_SC);
  int sr = analogRead(PINO_SR);
  int slc = analogRead(PINO_SLC);
  int src = analogRead(PINO_SRC);
  // Pino analógico de cada um deles? acho q sim

  // Maior que 0: lendo linha
  // Igual a 0: lendo branco

  // caso não esteja virando, apenas siga a linha
  if (!esta_virando) {

    /* Verifica cruzamento */
    if (sc > 0 && sl > 0 && sr > 0) {
      // TODO: talvez aqui deve ter que adicionar um delay para o robo nao ficar virando varias vezes sem parar
      esta_virando = 1;
    }
    
    /* Seguir linha */
    if (srl == 0 && src == 0) {
      anda_pra_frente();
    } else if (srl > 0 && src == 0) {
      // Esquerda
    } else if (srl == 0 && src > 0) {
      // Direita
    }
    
  // caso esteja virando, então continue virando até terminar
  } else {
    
    // virando para esquerda
    if (direcao == 0) {
      achou_linha_virando = vira_pra_esquerda(srl, src, achou_linha_virando);
      
      // terminou de virar
      if (achou_linha_virando==-1){
        esta_virando = 0;
      }

    }
    
    // virando para a direita
    else if (direcao == 1) {
      // vira_pra_direita (tem que implementar)
    }
  }


  // TODO:
  delay(500); // Só para testes
}