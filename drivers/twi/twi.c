// twi.c: basic twi driver
// Copyright (C) 2025 - 2026 stcarlso, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

#include "twi.h"

uint8_t twi_read_byte (uint8_t addr, uint8_t reg) 
{
    uint16_t timeout;

    // START + slave addr (write)
    TWI0.MADDR = (addr << 1) | 0;   // LSB=0 for WRITE

    timeout = TWI_TIMEOUT;
    while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))) {
        if (! (--timeout)) return TWI_ERROR;
    }
    if (TWI0.MSTATUS & TWI_RXACK_bm) return TWI_ERROR; // NACK

    // send register address
    TWI0.MDATA = reg;

    timeout = TWI_TIMEOUT;
    while (!(TWI0.MSTATUS & TWI_WIF_bm)) {
        if (! (--timeout)) return TWI_ERROR;
    }
    if (TWI0.MSTATUS & TWI_RXACK_bm) return TWI_ERROR;

    // repeated START + slave addr (read)
    TWI0.MADDR = (addr << 1) | 1;   // LSB=1 for READ

    timeout = TWI_TIMEOUT;
    while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))) {
        if (! (--timeout)) return TWI_ERROR;
    }
    if (TWI0.MSTATUS & TWI_RXACK_bm) return TWI_ERROR;

    // read 1 byte
    timeout = TWI_TIMEOUT;
    while (!(TWI0.MSTATUS & TWI_RIF_bm)) {
        if (! (--timeout)) return TWI_ERROR;
    }

    uint8_t result = TWI0.MDATA;

    // last byte, send NACK + STOP
    TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;

    return result;
}

uint8_t twi_write (uint8_t addr, uint8_t *data, uint8_t len) 
{
    TWI0.MADDR = (addr << 1);                   // send address (write)
    while (! (TWI0.MSTATUS & TWI_WIF_bm));      // wait for write interrupt flag
    if (TWI0.MSTATUS & TWI_RXACK_bm) return TWI_ERROR;  // NACK received
    for (uint8_t i = 0; i < len; i++)
    {
        TWI0.MDATA = data[i];
        while (! (TWI0.MSTATUS & TWI_WIF_bm));  // wait
        if (TWI0.MSTATUS & TWI_RXACK_bm) return TWI_ERROR;
    }
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;             // send stop
    return 0;                                   // success
}

