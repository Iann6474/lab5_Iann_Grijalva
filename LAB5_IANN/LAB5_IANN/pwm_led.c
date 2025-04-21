

#define F_CPU 16000000UL
#include "pwm_led.h"
#include <util/delay.h>
#include <avr/interrupt.h>

// Variables globales
volatile uint8_t led_duty_cycle = 0;    // ciclo de trabajo del LED 
volatile uint8_t timer_counter = 0;     // contador para el timer

// Inicializa el LED y el timer
void pwm_led_init(void) {
    // Configurar pin del LED como salida
    LED_DDR |= (1 << LED_PIN);
    
    // Apagar LED inicialmente
    LED_PORT &= ~(1 << LED_PIN);
    
    // Configurar Timer2 para interrupciones 
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    
    // Modo CTC, prescaler = 8 
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS21);
    
    // Valor de comparación más bajo = interrupciones más frecuentes
    OCR2A = 49;  // ~40kHz de frecuencia PWM
    
    // Habilitar interrupción por comparación
    TIMSK2 |= (1 << OCIE2A);
    
    // Habilitar interrupciones globales
    sei();
    
    // encender gradualmente
    for(uint8_t i = 0; i <= 100; i++) {
        pwm_led_set_intensity(i);
        _delay_ms(20);
    }
    
    // Mantener en máximo brillo 
    _delay_ms(500);
    
    // Apagar gradualmente
    for(uint8_t i = 100; i > 0; i--) {
        pwm_led_set_intensity(i);
        _delay_ms(20);
    }
    
    // Asegurar que el LED esté completamente apagado
    pwm_led_set_intensity(0);
    _delay_ms(500);
}


void pwm_led_set_intensity(uint8_t intensity) {
    
    if(intensity > 100) intensity = 100;
    
    // Actualizar ciclo de trabajo
    led_duty_cycle = intensity;
    
    // Caso especial para asegurar apagado completo
    if(intensity == 0) {
        // Desactivar directamente el LED sin PWM
        LED_PORT &= ~(1 << LED_PIN);
    }
}

// Rutina de interrupción del Timer2
ISR(TIMER2_COMPA_vect) {
    // Si intensidad es 0, mantenemos el LED apagado
    if(led_duty_cycle == 0) {
        LED_PORT &= ~(1 << LED_PIN);
        return;
    }
    
    // Incrementar contador
    timer_counter++;
    
    // Un ciclo completo cada 100 interrupciones
    if(timer_counter >= 100) {
        timer_counter = 0;
    }
    
    // PWM manual: si contador < ciclo de trabajo, LED encendido
    if(timer_counter < led_duty_cycle) {
        LED_PORT |= (1 << LED_PIN);
    } else {
        LED_PORT &= ~(1 << LED_PIN);
    }
}