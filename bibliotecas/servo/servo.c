#include "servo.h"
#include <ESP32Servo.h>

#define OLHANDO_ESQUERDA 180
#define OLHANDO_CENTRO 90
#define OLHANDO_DIREITA 0
#define PINO 9
#define MIN 500
#define MAX 2500

int pos = 0;
Servo servo_9;

void setup(){
  servo_9.attach(PINO, MIN, MAX);
}

void loop(){
  servo_9.write(OLHANDO_CENTRO);
  delay(2000);
  servo_9.write(OLHANDO_ESQUERDA);
  delay(2000);
  servo_9.write(OLHANDO_CENTRO);
  delay(2000);
  servo_9.write(OLHANDO_DIREITA);
  delay(2000);
  servo_9.write(OLHANDO_CENTRO);
  delay(2000);
  
}