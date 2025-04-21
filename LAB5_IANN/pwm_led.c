/**
 * pwm_led.c - implementación de la librería para control de intensidad LED
 */

#define F_CPU 16000000UL  // 16 MHz (Arduino Nano)
#include "pwm_led.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// Variables para el PWM manual del LED
volatile uint8_t led_intensity = 0;     // Valor de intensidad (0-255)
volatile uint8_t pwm_counter = 0;       // Contador para el ciclo PWM

// Inicializa el módulo PWM para el LED
void pwm_led_init(void) {
    // Configurar pin del LED como salida
    LED_DDR |= (1 << LED_PIN);
    
    // Inicialmente, LED apagado
    LED_PORT &= ~(1 << LED_PIN);
    
    // Configurar Timer2 para generar interrupciones periódicas
    // Limpiar registros de control
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    
    // Configurar modo CTC (Clear Timer on Compare)
    TCCR2A |= (1 << WGM21);
    
    // Prescaler = 64
    TCCR2B |= (1 << CS22);
    
    // Valor de comparación para frecuencia aproximada de 1kHz
    OCR2A = 249;  // (16MHz/64/1000Hz) - 1
    
    // Habilitar interrupción de comparación
    TIMSK2 |= (1 << OCIE2A);
    
    // Habilitar interrupciones globales
    sei();
    
    // Test inicial - encender y apagar lentamente el LED
    for (uint8_t i = 0; i < 255; i++) {
        pwm_led_set_intensity(i);
        _delay_ms(5);
    }
    for (uint8_t i = 255; i > 0; i--) {
        pwm_led_set_intensity(i);
        _delay_ms(5);
    }
    
    // Dejar LED apagado
    pwm_led_set_intensity(0);
}

// Establece la intensidad del LED (0-255)
void pwm_led_set_intensity(uint8_t intensity) {
    led_intensity = intensity;
}

// Rutina de servicio de interrupción para el Timer2 (PWM del LED)
ISR(TIMER2_COMPA_vect) {
    // Incrementar contador
    pwm_counter++;
    
    // Si contador >= 255, reiniciar
    if (pwm_counter >= 255) {
        pwm_counter = 0;
    }
    
    // Implementación PWM: si contador < intensidad, encender LED
    if (pwm_counter < led_intensity) {
        LED_PORT |= (1 << LED_PIN);
    } else {
        LED_PORT &= ~(1 << LED_PIN);
    }
}