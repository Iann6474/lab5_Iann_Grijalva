
#ifndef PMW_SERVOC_H_
#define PMW_SERVOC_H_

// incluye la biblioteca de entrada/salida
#include <avr/io.h>

// funciones para controlar el servo
void PMW_init(void);
void PMW_SetDutyCycle(uint16_t duty);

#endif 