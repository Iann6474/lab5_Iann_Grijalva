

#ifndef PWM2_H_
#define PWM2_H_

#include <avr/io.h>

// definiciones para los servos
#define SERVO_MIN_PULSO 1000  // pulso m�nimo (0 grados)
#define SERVO_MAX_PULSO 5000  // pulso m�ximo (180 grados)
#define SERVO_PERIODO 20000   // per�odo (20ms)

// constantes para filtrado
#define NUM_MUESTRAS 8        // muestras para promedio
#define UMBRAL_CAMBIO 8       // umbral para considerar cambio v�lido

// inicializa los m�dulos PWM para los dos servos
void iniciar_servos(void);

// establece el �ngulo para el servo 1 (0-180 grados)
void establecer_angulo_servo1(uint8_t angulo);

// establece el �ngulo para el servo 2 (0-180 grados)
void establecer_angulo_servo2(uint8_t angulo);

// lee un canal ADC con filtrado
uint16_t leer_adc_filtrado(uint8_t canal);

#endif /* PWM2_H_ */