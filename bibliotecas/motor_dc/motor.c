#include "bibliotecas/motor_dc/motor.h"

void motor_init(){
  pinMode(PWMIN1, INPUT);
  pinMode(PWMIN2, INPUT);
  pinMode(PWMOUT1, OUTPUT);
  pinMode(PWMOUT2, OUTPUT);
}

// função principal de acionar os motores
void set_motors(int m1, int m2){
  
  //TODO: fazer uma relação proporcional entre m1 e pwmout1
  float pot = analogRead(PWMIN1)/4;
  analogWrite(PWMOUT1, pot);


  //TODO: fazer uma relação proporcional entre m2 e pwmout2
  float pot = analogRead(PWMIN2)/4;
  analogWrite(PWMOUT2, pot);
 
}