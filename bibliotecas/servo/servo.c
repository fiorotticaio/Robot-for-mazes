#include "servo.h"
#include <ESP32Servo.h>

Servo servo;

void setup(){
  servo.attach(PINO_SERVO);
  servo.write(90);
}