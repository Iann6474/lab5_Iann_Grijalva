
#include <avr/io.h>
#include <avr/interrupt.h>
#include "PMW_ServoC.h"

// Constantes para el control del servo
#define SERVO_MIN 2000   // 1ms = 0 grados 
#define SERVO_MAX 4000   // 2ms = 180 grados 
#define SERVO_PERIOD 20000  // 20ms período

// Variables globales para el control del PWM 
volatile uint16_t g_duty_cycle = 3000; // valor inicial 1.5ms = posición central
volatile uint16_t g_counter = 0;
volatile uint8_t g_servo_moving = 0;

void PMW_init(){
    // configura pin PB1 como salida para el servo
    DDRB |= (1 << PB1);
    
    // inicializamos Timer1 para generar señal PWM 
    // Modo CTC con OCR1A 
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11); // modo CTC prescaler  8
    
    // interrupciones cada 10us para precisión
    OCR1A = 19; // (16MHz/8/100kHz) - 1 = 19
    
    // habilitar interrupción de comparación
    TIMSK1 |= (1 << OCIE1A);
    
    // habilitar interrupciones globales
    sei();
    
    // posicionar servo en centro al inicio
    PMW_SetDutyCycle(3000); // 1.5ms = 90 grados
    
    // pequeño retardo para estabilización inicial
    for(volatile uint16_t i = 0; i < 30000; i++);
}

void PMW_SetDutyCycle(uint16_t duty){
    // comprobar límites
    if (duty < SERVO_MIN) duty = SERVO_MIN;
    if (duty > SERVO_MAX) duty = SERVO_MAX;
    
    // actualizar ciclo de trabajo
    cli(); // desactivar interrupciones durante la actualización
    g_duty_cycle = duty;
    g_servo_moving = 1; // indicar que el servo debe moverse
    sei(); // reactivar interrupciones
}

// interrupción del Timer1 para generar PWM 
ISR(TIMER1_COMPA_vect) {
    // incrementamos contador (cada 10us)
    g_counter += 10;
    
    // generamos el pulso PWM
    if (g_counter < g_duty_cycle) {
        PORTB |= (1 << PB1); // pin alto
    } else {
        PORTB &= ~(1 << PB1); // pin bajo
    }
    
    // reiniciamos el contador al final del periodo (20ms = 20000us)
    if (g_counter >= SERVO_PERIOD) {
        g_counter = 0;
    }
}