#ifndef SENSOR_ULTRASSON_H
#define SENSOR_ULTRASSON_H

#define PINO_TRIGGER_SU 7 // Trigger sensor ultrassom
#define PINO_ECHO_SU 8 // Echo sensor ultrassom

long mede_distancia_su(int pino_trigger, int pino_echo, long distancia_anterior);
long microssegundos_para_cm(long microssegundos)

#endif // SENSOR_ULTRASSON_H