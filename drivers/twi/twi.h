// twi.h: basic twi driver
// Copyright (C) 2025 - 2026 stcarlso, Loneoceans
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TWI_H
#define TWI_H
#define TWI_C drivers/twi/twi.c

#include <stdint.h>
#define TWI_ERROR 255
#define TWI_TIMEOUT 30000
#define TWI_BAUD(F_SCL)  ((((F_CPU / F_SCL) - 10) / 2))

uint8_t twi_read_byte (uint8_t addr, uint8_t reg);
uint8_t twi_write (uint8_t addr, uint8_t *data, uint8_t len);

#endif