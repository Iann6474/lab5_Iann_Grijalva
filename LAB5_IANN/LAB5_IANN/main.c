/*
*Iann Grijalva-23055
* Universidad del Valle de Guatemala
* IE2023: Programación de Microcontroladores
*
* Descripción: Mover servor con señal pwm y disminuir intensidad del led 
*
*
* Hardware: ATMega328P
*/


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "pwm2.h"
#include "pwm_led.h"

int main(void) {
	// Variables
	uint8_t angulo_servo1, angulo_servo2, intensidad_led;
	
	// Inicializar servos
	iniciar_servos();
	
	// Inicializar PWM del LED
	pwm_led_init();
	
	// Bucle principal
	while (1) {
		// Leer potenciómetros
		uint16_t valor_adc1 = leer_adc_filtrado(7);  // ADC7
		uint16_t valor_adc2 = leer_adc_filtrado(6);  // ADC6
		uint16_t valor_adc3 = leer_adc_filtrado(5);  // ADC5 
		
		// Convertir a ángulos (0-180) e intensidad (0-100)
		angulo_servo1 = (uint8_t)((uint32_t)valor_adc1 * 180 / 1023);
		angulo_servo2 = (uint8_t)((uint32_t)valor_adc2 * 180 / 1023);
		intensidad_led = (uint8_t)((uint32_t)valor_adc3 * 100 / 1023);
		
		// Actualizar servos
		establecer_angulo_servo1(angulo_servo1);
		establecer_angulo_servo2(angulo_servo2);
		
		// Actualizar LED
		pwm_led_set_intensity(intensidad_led);
		
		// Pequeña pausa
		_delay_ms(20);
	}
	
	return 0;
}