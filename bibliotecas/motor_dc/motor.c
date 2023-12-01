#include "bibliotecas/motor_dc/motor.h"

void setup() {
  pinMode(PINO_E_M1, OUTPUT);
  pinMode(PINO_E_M2, OUTPUT);
  pinMode(PINO_M1, OUTPUT);
  pinMode(PINO_M2, OUTPUT);

  ledcAttachPin(PINO_M1, PWM1_CH);
  ledcSetup(PWM1_CH, PWM1_FREQ, PWM1_RES);

  ledcAttachPin(PINO_M2, PWM2_CH);
  ledcSetup(PWM2_CH, PWM1_FREQ, PWM1_RES);
}


void loop() {
  digitalWrite(PINO_E_M1, HIGH);
  digitalWrite(PINO_E_M2, HIGH);

  ledcWrite(PWM1_CH, 50);
  ledcWrite(PWM2_Ch, 200);

  delay(1000);

  ledcWrite(PWM1_CH, 200);
  ledcWrite(PWM2_Ch, 50);
  delay(1000);
}