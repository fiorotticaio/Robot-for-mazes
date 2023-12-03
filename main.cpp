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
int direcao_esta_virando = 1; // 1 direita, 0 esquerda, -1 trás

int achou_linha_virando = 0;  //  0: ainda nao achou a linha
                              //  1: achou a linha, está quase terminando de virar
                              // -1: terminou de virar

float kp = 0.1;               // constante proporcional
                              //FIXME: achar um valor bom para kp e kd
float kd = 0.05;              // constante derivativa

float ultimo_erro = 0;        // erro anterior (apenas para termo derivativo)
float erro = 0;               // erro atual

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
  
  //FIXME: adicionar um threshold se pa
  // Maior que 0: lendo linha
  // Igual a 0: lendo branco

  // caso não esteja virando, apenas siga a linha
  if (!esta_virando) {

    /* Verifica cruzamento */
    if (sc > 0 && sl > 0 && sr > 0) {
      // TODO: talvez aqui deve ter que adicionar um delay para o robo nao ficar virando varias vezes sem parar
      esta_virando = 1;
      if(slc > 0 && src > 0) {
        // cruzamento em T
        direcao_esta_virando = 0;   //sempre vira para esquerda
      } else if (slc > 0) {
        direcao_esta_virando = 0;
      } else if (src > 0) {
        direcao_esta_virando = 1;
      } else {
        direcao_esta_virando = 0;   //sempre vira para esquerda
      }
    }
    
    /* Seguir linha */
    if (slc == 0 && src == 0) {
      anda_pra_frente();
    } else if (slc > 0 && src == 0 || slc == 0 && src > 0) {
      // Virar a direita com controle proporcional
      erro = medir_erro(slc, sc, src);
      float controle = kp*erro + kd*(erro - ultimo_erro);
      int velocidadeEsq = 127 + controle;
      int velocidadeDir = 127 - controle;
      
      ultimo_erro = erro;

      ledcWrite(PWM1_CH, velocidadeEsq);
      ledcWrite(PWM2_CH, velocidadeDir);

    } else if (slc > 0 && src > 0) {
      // ??????
      Serial.println("cruzamento chegando");
    } else if(sl == 0 && sc == 0 && sr == 0 && slc == 0 && src == 0) {
      //caminho sem saida
      esta_virando = 1;
      direcao_esta_virando = -1;
    }
  // caso esteja virando, então continue virando até terminar
  } else {
    
    // virando para esquerda
    if (direcao_esta_virando == 0) {
      achou_linha_virando = vira_pra_esquerda(sl, sc, achou_linha_virando);
      
      // terminou de virar
      if (achou_linha_virando==-1){
        esta_virando = 0;
      }

    }
    
    // virando para a direita
    else if (direcao_esta_virando == 1) {
      achou_linha_virando = vira_pra_direita(sr, sc, achou_linha_virando);
      
      // terminou de virar
      if (achou_linha_virando==-1){
        esta_virando = 0;
      }
    }

    // virando para trás
    else if (direcao_esta_virando == -1) {
      achou_linha_virando = vira_180_graus(sl, sr, sc, achou_linha_virando);
      
      // terminou de virar
      if (achou_linha_virando==-1){
        esta_virando = 0;
      }
    }
  }


  // TODO:
  delay(500); // Só para testes
}

/*======================== FUNÇÕES ========================*/

float medir_erro(int slc, int sc, int src) {
  return sc - (slc + src)/2;    
}