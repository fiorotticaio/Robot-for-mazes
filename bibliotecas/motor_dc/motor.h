#ifndef MOTORES_DC_H
#define MOTORES_DC_H

#define PINO_E_M1 33
#define PINO_E_M2 26
#define PINO_M1 25
#define PINO_M2 27
#define PWM1_CH 0
#define PWM2_CH 1
#define PWM1_FREQ 1000  // 1 kHz
#define PWM1_RES 8      // 8 bits

void init_motores_dc() ;
void liga_motores();
void desliga_motores();
void anda_pra_frente();
void freia_ate_parar() ;
void vira_pra_direita();

#endif