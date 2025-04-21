
#define F_CPU 16000000UL  // 16 MHz (Arduino Nano)
#include "pwm2.h"
#include <util/delay.h>
#include <avr/interrupt.h>

// función interna para calcular valor absoluto
static inline int16_t valor_absoluto(int16_t valor) {
    return valor < 0 ? -valor : valor;
}

// valores actuales de los servos para suavizado
static uint16_t valor_servo1_actual = 3000;  // centro
static uint16_t valor_servo2_actual = 3000;  // centro

void iniciar_servos(void) {
    // inicializar ADC
    ADMUX = (1 << REFS0);  // referencia = AVCC
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // habilitar ADC, prescaler 128
    
    // configurar pines de los servos como salidas
    DDRB |= (1 << PB1) | (1 << PB2);  // OC1A (PB1/D9) y OC1B (PB2/D10)
    
    // configurar Timer1 para ambos canales PWM
    // modo 14: Fast PWM con TOP = ICR1
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);  // PWM no invertido en ambos canales
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);     // Modo 14, prescaler = 8
    
    // período de 20ms (50Hz)
    ICR1 = 39999;  // (16MHz/8)/50Hz - 1
    
    // posición inicial: centro (90 grados)
    OCR1A = valor_servo1_actual;  // servo 1
    OCR1B = valor_servo2_actual;  // servo 2
    
    // estabilización inicial
    _delay_ms(500);
    
    // leer ADC varias veces para estabilizarlo
    for (uint8_t i = 0; i < 8; i++) {
        leer_adc_filtrado(0);
        leer_adc_filtrado(1);
        _delay_ms(5);
    }
}

void establecer_angulo_servo1(uint8_t angulo) {
    // limitar ángulo a 0-180
    if (angulo > 180) 
        angulo = 180;
    
    // convertir ángulo a ancho de pulso (mapeo lineal)
    uint16_t objetivo = SERVO_MIN_PULSO + ((uint32_t)(SERVO_MAX_PULSO - SERVO_MIN_PULSO) * angulo / 180);
    
    // suavizado para movimientos más fluidos
    valor_servo1_actual = (valor_servo1_actual * 7 + objetivo * 3) / 10;
    
    // actualizar registro de comparación
    OCR1A = valor_servo1_actual;
}

void establecer_angulo_servo2(uint8_t angulo) {
    // limitar ángulo a 0-180
    if (angulo > 180) 
        angulo = 180;
    
    // convertir ángulo a ancho de pulso (mapeo lineal)
    uint16_t objetivo = SERVO_MIN_PULSO + ((uint32_t)(SERVO_MAX_PULSO - SERVO_MIN_PULSO) * angulo / 180);
    
    // suavizado para movimientos más fluidos
    valor_servo2_actual = (valor_servo2_actual * 7 + objetivo * 3) / 10;
    
    // actualizar registro de comparación
    OCR1B = valor_servo2_actual;
}

uint16_t leer_adc_filtrado(uint8_t canal) {
    static uint16_t valores_anteriores[8] = {0};  // para los 8 canales posibles
    
    // leer con promedio
    uint32_t suma = 0;
    for (uint8_t i = 0; i < NUM_MUESTRAS; i++) {
        // seleccionar canal (0-7)
        ADMUX = (ADMUX & 0xF8) | (canal & 0x07);
        _delay_us(10);  // estabilizar multiplexor
        
        // iniciar conversión
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));  // esperar a que termine
        
        // acumular
        suma += ADC;
        _delay_ms(1);
    }
    
    // calcular promedio
    uint16_t valor_actual = suma / NUM_MUESTRAS;
    
    // solo actualizar si hay cambio significativo
    if (valor_absoluto((int16_t)valor_actual - (int16_t)valores_anteriores[canal]) > UMBRAL_CAMBIO) {
        valores_anteriores[canal] = valor_actual;
    }
    
    // retornar valor filtrado
    return valores_anteriores[canal];
}