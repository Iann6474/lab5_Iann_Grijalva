
#ifndef PWM_LED_H_
#define PWM_LED_H_

#include <avr/io.h>

// Definiciones para el LED
#define LED_PIN     PB0     // pin del LED 
#define LED_PORT    PORTB   // puerto del LED
#define LED_DDR     DDRB    // dirección del puerto del LED

// Inicializa el sistema PWM para el LED
void pwm_led_init(void);

// Establece la intensidad del LED (0-100%)
void pwm_led_set_intensity(uint8_t intensity);

#endif 