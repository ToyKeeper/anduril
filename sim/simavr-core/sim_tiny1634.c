/*
 * sim_tiny1634.c - ATtiny1634 core for simavr
 *
 * Copyright (C) 2024 Ian Dobbie
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * ATtiny1634 specifications:
 * - 16KB Flash, 1KB SRAM, 256B EEPROM
 * - 3 GPIO ports (A: 8 pins, B: 4 pins, C: 6 pins)
 * - Timer0: 8-bit with PWM on OC0A (PC0), OC0B (PA5)
 * - Timer1: 16-bit with PWM on OC1A (PB3), OC1B (PA6)
 * - 12-channel 10-bit ADC
 * - WDT, PCINT on all 3 ports
 */

#include "sim_avr.h"

#define SIM_VECTOR_SIZE    2
#define SIM_MMCU           "attiny1634"
#define SIM_CORENAME       mcu_tiny1634

#define _AVR_IO_H_
#define __ASSEMBLER__
#include "avr/iotn1634.h"

#include "sim_tiny1634.h"

static avr_t * make(void)
{
    return avr_core_allocate(&SIM_CORENAME.core, sizeof(struct mcu_t));
}

avr_kind_t tiny1634 = {
    .names = { "attiny1634" },
    .make = make
};

AVR_MCU_DECLARE(tiny1634, SIM_CORENAME);

void tn1634_init(struct avr_t * avr)
{
    struct mcu_t * mcu = (struct mcu_t*)avr;

    avr_eeprom_init(avr, &mcu->eeprom);
    avr_flash_init(avr, &mcu->selfprog);
    avr_watchdog_init(avr, &mcu->watchdog);
    avr_extint_init(avr, &mcu->extint);
    avr_ioport_init(avr, &mcu->porta);
    avr_ioport_init(avr, &mcu->portb);
    avr_ioport_init(avr, &mcu->portc);
    avr_timer_init(avr, &mcu->timer0);
    avr_timer_init(avr, &mcu->timer1);
    avr_adc_init(avr, &mcu->adc);
}

void tn1634_reset(struct avr_t * avr)
{
    (void)avr;
    // Nothing special needed for reset
}
