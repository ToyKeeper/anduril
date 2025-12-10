/*
 * sim_tiny1634.h - ATtiny1634 core header for simavr
 *
 * Copyright (C) 2024 Ian Dobbie
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * ATtiny1634 specifications:
 * - 16KB Flash, 1KB SRAM, 256B EEPROM
 * - 3 GPIO ports (A: 8 pins, B: 4 pins, C: 6 pins)
 * - Timer0: 8-bit with PWM
 * - Timer1: 16-bit with PWM
 * - 12-channel 10-bit ADC
 * - WDT, 2x USART, USI
 */

#ifndef __SIM_TINY1634_H__
#define __SIM_TINY1634_H__

#if defined(SIM_CORENAME) && !defined(SIM_TINY1634_H_INCLUDED)
#define SIM_TINY1634_H_INCLUDED

#include "sim_core_declare.h"
#include "avr_eeprom.h"
#include "avr_flash.h"
#include "avr_watchdog.h"
#include "avr_extint.h"
#include "avr_ioport.h"
#include "avr_timer.h"
#include "avr_adc.h"

void tn1634_init(struct avr_t * avr);
void tn1634_reset(struct avr_t * avr);

/*
 * ATtiny1634 MCU structure
 * Note: No UART/USI/SPI in this minimal implementation - add as needed
 */
const struct mcu_t {
    avr_t core;
    avr_eeprom_t eeprom;
    avr_flash_t selfprog;
    avr_watchdog_t watchdog;
    avr_extint_t extint;
    avr_ioport_t porta, portb, portc;
    avr_timer_t timer0, timer1;
    avr_adc_t adc;
} SIM_CORENAME = {
    .core = {
        .mmcu = SIM_MMCU,
        DEFAULT_CORE(SIM_VECTOR_SIZE),

        .init = tn1634_init,
        .reset = tn1634_reset,
    },
    AVR_EEPROM_DECLARE_NOEEPM(EE_READY_vect),
    AVR_SELFPROG_DECLARE(SPMCSR, SPMEN, SPM_PAGESIZE),
    AVR_WATCHDOG_DECLARE(WDTCSR, WDT_vect),
    .extint = {
        AVR_EXTINT_TINY_DECLARE(0, 'A', 2, GIFR),
    },
    .porta = {
        .name = 'A', .r_port = PORTA, .r_ddr = DDRA, .r_pin = PINA,
        .pcint = {
            .enable = AVR_IO_REGBIT(GIMSK, PCIE0),
            .raised = AVR_IO_REGBIT(GIFR, PCIF0),
            .vector = PCINT0_vect,
        },
        .r_pcint = PCMSK0,
    },
    .portb = {
        .name = 'B', .r_port = PORTB, .r_ddr = DDRB, .r_pin = PINB,
        .pcint = {
            .enable = AVR_IO_REGBIT(GIMSK, PCIE1),
            .raised = AVR_IO_REGBIT(GIFR, PCIF1),
            .vector = PCINT1_vect,
        },
        .r_pcint = PCMSK1,
    },
    .portc = {
        .name = 'C', .r_port = PORTC, .r_ddr = DDRC, .r_pin = PINC,
        .pcint = {
            .enable = AVR_IO_REGBIT(GIMSK, PCIE2),
            .raised = AVR_IO_REGBIT(GIFR, PCIF2),
            .vector = PCINT2_vect,
        },
        .r_pcint = PCMSK2,
    },

    // Timer0 - 8-bit with PWM on OC0A (PC0) and OC0B (PA5)
    .timer0 = {
        .name = '0',
        .disabled = AVR_IO_REGBIT(PRR, PRTIM0),
        .wgm = { AVR_IO_REGBIT(TCCR0A, WGM00), AVR_IO_REGBIT(TCCR0A, WGM01),
                 AVR_IO_REGBIT(TCCR0B, WGM02) },
        .wgm_op = {
            [0] = AVR_TIMER_WGM_NORMAL8(),
            [1] = AVR_TIMER_WGM_PCPWM8(),
            [2] = AVR_TIMER_WGM_CTC(),
            [3] = AVR_TIMER_WGM_FASTPWM8(),
            [5] = AVR_TIMER_WGM_PCPWM(),
            [7] = AVR_TIMER_WGM_FASTPWM(),
        },
        .cs = { AVR_IO_REGBIT(TCCR0B, CS00), AVR_IO_REGBIT(TCCR0B, CS01),
                AVR_IO_REGBIT(TCCR0B, CS02) },
        .cs_div = { 0, 0, 3, 6, 8, 10 },
        .r_tcnt = TCNT0,

        .overflow = {
            .enable = AVR_IO_REGBIT(TIMSK, TOIE0),
            .raised = AVR_IO_REGBIT(TIFR, TOV0),
            .vector = TIM0_OVF_vect,
        },
        .comp = {
            [AVR_TIMER_COMPA] = {
                .r_ocr = OCR0A,
                .com = AVR_IO_REGBITS(TCCR0A, COM0A0, 0x3),
                .com_pin = AVR_IO_REGBIT(PORTC, 0),
                .interrupt = {
                    .enable = AVR_IO_REGBIT(TIMSK, OCIE0A),
                    .raised = AVR_IO_REGBIT(TIFR, OCF0A),
                    .vector = TIM0_COMPA_vect,
                },
            },
            [AVR_TIMER_COMPB] = {
                .r_ocr = OCR0B,
                .com = AVR_IO_REGBITS(TCCR0A, COM0B0, 0x3),
                .com_pin = AVR_IO_REGBIT(PORTA, 5),
                .interrupt = {
                    .enable = AVR_IO_REGBIT(TIMSK, OCIE0B),
                    .raised = AVR_IO_REGBIT(TIFR, OCF0B),
                    .vector = TIM0_COMPB_vect,
                },
            },
        },
    },

    // Timer1 - 16-bit with PWM on OC1A (PB3) and OC1B (PA6)
    .timer1 = {
        .name = '1',
        .disabled = AVR_IO_REGBIT(PRR, PRTIM1),
        .wgm = { AVR_IO_REGBIT(TCCR1A, WGM10), AVR_IO_REGBIT(TCCR1A, WGM11),
                 AVR_IO_REGBIT(TCCR1B, WGM12), AVR_IO_REGBIT(TCCR1B, WGM13) },
        .wgm_op = {
            [0]  = AVR_TIMER_WGM_NORMAL16(),
            [1]  = AVR_TIMER_WGM_PCPWM8(),
            [2]  = AVR_TIMER_WGM_PCPWM9(),
            [3]  = AVR_TIMER_WGM_PCPWM10(),
            [4]  = AVR_TIMER_WGM_CTC(),
            [5]  = AVR_TIMER_WGM_FASTPWM8(),
            [6]  = AVR_TIMER_WGM_FASTPWM9(),
            [7]  = AVR_TIMER_WGM_FASTPWM10(),
            [8]  = AVR_TIMER_WGM_ICPWM(),
            [9]  = AVR_TIMER_WGM_OCPWM(),
            [10] = AVR_TIMER_WGM_ICPWM(),
            [11] = AVR_TIMER_WGM_OCPWM(),
            [12] = AVR_TIMER_WGM_ICCTC(),
            [14] = AVR_TIMER_WGM_ICFASTPWM(),
            [15] = AVR_TIMER_WGM_OCFASTPWM(),
        },
        .cs = { AVR_IO_REGBIT(TCCR1B, CS10), AVR_IO_REGBIT(TCCR1B, CS11),
                AVR_IO_REGBIT(TCCR1B, CS12) },
        .cs_div = { 0, 0, 3, 6, 8, 10 },

        .r_tcnt = TCNT1L,
        .r_tcnth = TCNT1H,
        .r_icr = ICR1L,
        .r_icrh = ICR1H,

        .overflow = {
            .enable = AVR_IO_REGBIT(TIMSK, TOIE1),
            .raised = AVR_IO_REGBIT(TIFR, TOV1),
            .vector = TIM1_OVF_vect,
        },
        .icr = {
            .enable = AVR_IO_REGBIT(TIMSK, ICIE1),
            .raised = AVR_IO_REGBIT(TIFR, ICF1),
            .vector = TIM1_CAPT_vect,
        },
        .comp = {
            [AVR_TIMER_COMPA] = {
                .r_ocr = OCR1AL,
                .r_ocrh = OCR1AH,
                .com = AVR_IO_REGBITS(TCCR1A, COM1A0, 0x3),
                .com_pin = AVR_IO_REGBIT(PORTB, 3),
                .interrupt = {
                    .enable = AVR_IO_REGBIT(TIMSK, OCIE1A),
                    .raised = AVR_IO_REGBIT(TIFR, OCF1A),
                    .vector = TIM1_COMPA_vect,
                },
            },
            [AVR_TIMER_COMPB] = {
                .r_ocr = OCR1BL,
                .r_ocrh = OCR1BH,
                .com = AVR_IO_REGBITS(TCCR1A, COM1B0, 0x3),
                .com_pin = AVR_IO_REGBIT(PORTA, 6),
                .interrupt = {
                    .enable = AVR_IO_REGBIT(TIMSK, OCIE1B),
                    .raised = AVR_IO_REGBIT(TIFR, OCF1B),
                    .vector = TIM1_COMPB_vect,
                },
            },
        },
    },

    // ADC - 12 channels, 10-bit
    .adc = {
        .r_admux = ADMUX,
        .mux = { AVR_IO_REGBIT(ADMUX, MUX0), AVR_IO_REGBIT(ADMUX, MUX1),
                 AVR_IO_REGBIT(ADMUX, MUX2), AVR_IO_REGBIT(ADMUX, MUX3) },
        .ref = { AVR_IO_REGBIT(ADMUX, REFS0), AVR_IO_REGBIT(ADMUX, REFS1) },
        .ref_values = { [0] = ADC_VREF_VCC, [1] = ADC_VREF_V110 },

        .adlar = AVR_IO_REGBIT(ADCSRB, ADLAR),
        .r_adcsra = ADCSRA,
        .aden = AVR_IO_REGBIT(ADCSRA, ADEN),
        .adsc = AVR_IO_REGBIT(ADCSRA, ADSC),
        .adate = AVR_IO_REGBIT(ADCSRA, ADATE),
        .adps = { AVR_IO_REGBIT(ADCSRA, ADPS0), AVR_IO_REGBIT(ADCSRA, ADPS1),
                  AVR_IO_REGBIT(ADCSRA, ADPS2) },

        .r_adch = ADCH,
        .r_adcl = ADCL,

        .r_adcsrb = ADCSRB,
        .adts = { AVR_IO_REGBIT(ADCSRB, ADTS0), AVR_IO_REGBIT(ADCSRB, ADTS1),
                  AVR_IO_REGBIT(ADCSRB, ADTS2) },

        .adc = {
            .enable = AVR_IO_REGBIT(ADCSRA, ADIE),
            .raised = AVR_IO_REGBIT(ADCSRA, ADIF),
            .vector = ADC_vect,
        },
        .muxmode = {
            [0] = AVR_ADC_SINGLE(0), [1] = AVR_ADC_SINGLE(1),
            [2] = AVR_ADC_SINGLE(2), [3] = AVR_ADC_SINGLE(3),
            [4] = AVR_ADC_SINGLE(4), [5] = AVR_ADC_SINGLE(5),
            [6] = AVR_ADC_SINGLE(6), [7] = AVR_ADC_SINGLE(7),
            [8] = AVR_ADC_SINGLE(8), [9] = AVR_ADC_SINGLE(9),
            [10] = AVR_ADC_SINGLE(10), [11] = AVR_ADC_SINGLE(11),
            [14] = AVR_ADC_TEMP(),
        },
    },
};

#endif /* SIM_CORENAME && !SIM_TINY1634_H_INCLUDED */

#endif /* __SIM_TINY1634_H__ */
