// aw2016.c: Device driver for AW2016 peripheral
// Copyright (C) 2025-2026 Loneoceans,
//               2026 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later

#include <util/delay.h>  // import _delay_ms()


////////// misc internal stuff //////////

#define AW2016_ADDR  0x64

static void aw2016_sleep ();
static void aw2016_wake ();
static void aw2016_enable_leds ();
static void aw2016_disable_leds ();

static void aw2016_set_manual_or_pattern_mode (uint8_t channel, bool animated);
static void aw2016_set_animation_mode (bool animated);
static void aw2016_set_led (uint8_t channel, uint8_t pwm);

static void aw2016_set_global_current (aw2016_current_t cur);
static void aw2016_set_rgb_calibration (uint8_t r, uint8_t g, uint8_t b);
static void aw2016_set_global_pwm (uint8_t pwm);


////////// TWI / I2C stuff //////////

#define TWI_ERROR  255
#define TWI_TIMEOUT  50000
#define AW2016_TWI_BAUD  400000
#define TWI_BAUD(F_SCL)  ((((F_CPU / F_SCL) - 10) / 2))

static uint8_t twi_read_byte (uint8_t addr, uint8_t reg);
static uint8_t twi_write (uint8_t addr, uint8_t *data, uint8_t len);
static void aw2016_write_reg (uint8_t reg, uint8_t val);


////////// external API functions //////////

inline void aw2016_hwdef_setup () {
    TWI0.MBAUD = TWI_BAUD(AW2016_TWI_BAUD);
    TWI0.MCTRLA = TWI_ENABLE_bm | TWI_SMEN_bm;  // smart mode (auto sends ACK)
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;        // release bus
    aw2016_init();
}

void aw2016_init () {
    aw2016_write_reg(0x00, 0x55);  // reset internal registers
    _delay_ms(2);
    aw2016_wake();

    // default power and rgb calibration
    aw2016_set_ramp_current(AW2016_RAMP_DEFAULT);

    // default to all 3 channels as manual mode
    aw2016_set_animation_mode(false);

    aw2016_set_global_pwm(255);
    aw2016_sleep();
}

void aw2016_set_ramp_current (uint8_t row) {
    uint8_t current = aw2016_ramp_get(row, 0);
    uint8_t r_cal   = aw2016_ramp_get(row, 1);
    uint8_t g_cal   = aw2016_ramp_get(row, 2);
    uint8_t b_cal   = aw2016_ramp_get(row, 3);
    //uint8_t pwm     = aw2016_ramp_get(row, 4);
    aw2016_set_rgb_calibration(r_cal, g_cal, b_cal);
    aw2016_set_global_current(current);
    aw2016_ramp_row = row;
}

uint8_t aw2016_next_ramp_row (uint8_t row, bool nopwm) {
    row = (row + 1) % AW2016_RAMP_LEVELS;
    if (nopwm && (row < AW2016_RAMP_MIN_NOPWM))
        row = AW2016_RAMP_MIN_NOPWM;
    return row;
}


////////// common auxrgb API stuff //////////

// set a color and brightness in "passive" auxrgb mode
// power: 0b00BBGGRR
void aw2016_set_auxrgb_power (uint8_t power) {
    if (aw2016_animation_running) {
        aw2016_init();
    }

    else if (aw2016_is_pwm_mode) {
        aw2016_disable_auxrgb_pwm();
    }

    // add handling for sleep and wake if power = 0x00
    if (! power) {
        if (aw2016_is_awake) {
            // driver is awake but power is now 0; set power to 0; go to sleep
            // but don't sleep when in some modes like red/blue strobe for fast response
            aw2016_set_global_pwm(0);
            if (! aw2016_do_not_sleep) {
                aw2016_disable_leds();
                aw2016_sleep();
            }
        }
        return;
    }

    if (! aw2016_is_awake) {
        aw2016_wake();
        aw2016_set_global_pwm(0);  // ensure no rgb blip before turning on
        aw2016_enable_leds();
    }

    // configure power level and color balance
    //if (go_to_standby || setting_rgb_mode_now) {
    //if (go_to_standby || setting_rgb_mode_now || (! channel_is_aux(channel_mode))) {
    StatePtr state = current_state;
    if ((state == off_state) || (state == lockout_state)) {
        if (power & 0b101010)
            aw2016_set_ramp_current(cfg.aw2016_level_hi);
        else
            aw2016_set_ramp_current(cfg.aw2016_level_lo);
    } else {
        aw2016_set_ramp_current(cfg.aw2016_level_on);
    }

    // set PWM values
    uint8_t leds[] = { AW_R_CH, AW_G_CH, AW_B_CH };
    for (uint8_t i = 0; i < 3; i ++) {
        uint8_t led = leds[i];
        uint8_t lvl = (power >> (i<<1)) & 0x03;
        switch (lvl) {
            case 0:  // off
                aw2016_set_led(led, 0);
                break;
            case 1:  // low
                aw2016_set_led(led, aw2016_ramp_get(cfg.aw2016_level_lo, 4));
                break;
            default:  // high
                aw2016_set_led(led, aw2016_ramp_get(cfg.aw2016_level_hi, 4));
                break;
        }
    }
}

void aw2016_enable_auxrgb_pwm () {
    if (aw2016_animation_running) {
        aw2016_init();
    }
    if (! aw2016_is_awake) {
        aw2016_wake();
        aw2016_set_global_pwm(0);  // ensure no rgb blip before turning on
        aw2016_enable_leds();
    }
    if (channel_is_aux(channel_mode)) {
        aw2016_set_ramp_current(cfg.aw2016_level_on);
    }
    aw2016_is_pwm_mode = 1;
}

void aw2016_disable_auxrgb_pwm () {
    aw2016_init();
    //aw2016_is_pwm_mode = 0;
}

void aw2016_set_auxrgb_pwm (RGB8_t color) {
    aw2016_last_pwm = color;
    if (! aw2016_is_pwm_mode) { enable_auxrgb_pwm(); }
    aw2016_set_led(AW_R_CH, color.r);
    aw2016_set_led(AW_G_CH, color.g);
    aw2016_set_led(AW_B_CH, color.b);
}

bool aw2016_gradual_adjust_auxrgb_pwm (RGB8_t target) {
    GRADUAL_ADJUST_SIMPLE(target.r, aw2016_last_pwm.r);
    GRADUAL_ADJUST_SIMPLE(target.g, aw2016_last_pwm.g);
    GRADUAL_ADJUST_SIMPLE(target.b, aw2016_last_pwm.b);

    aw2016_set_auxrgb_pwm (aw2016_last_pwm);

    if ((target.r == aw2016_last_pwm.r)
     && (target.g == aw2016_last_pwm.g)
     && (target.b == aw2016_last_pwm.b)) {
        return true;  // done
    }
    return false;  // not done yet
}

void aw2016_set_channel_mode (uint8_t channel) {
    if (channel_is_aux(channel))
        aw2016_set_ramp_current(cfg.aw2016_level_on);
}


////////// misc internal stuff //////////

// chip enters standby mode of 10uA
static void aw2016_sleep () {
    aw2016_write_reg(0x01, 0x00);
    aw2016_is_awake = 0;
    aw2016_animation_running = 0;
    aw2016_is_pwm_mode = 0;
    aw2016_ramp_row = 0;
}

// chip exits standby mode
static void aw2016_wake () {
    aw2016_write_reg(0x01, 0x01);
    aw2016_is_awake = 1;
}

// enables all leds
static void aw2016_enable_leds () { aw2016_write_reg(0x30, 0x07); }

// disables all leds but leaves chip awake
static void aw2016_disable_leds () { aw2016_write_reg(0x30, 0x00); }

// animated = false : disable animation, true: enable animation
void aw2016_set_animation_mode (bool animated) {
    for (uint8_t i=0; i<3; i++)
        aw2016_set_manual_or_pattern_mode(i, animated);
}

// set individual pattern mode for each RGB channel
void aw2016_set_manual_or_pattern_mode (uint8_t channel, bool animated) {
    uint8_t reg;
    reg = twi_read_byte(AW2016_ADDR, 0x31 + channel);
    if (animated) {  // animations on
        reg |= (1 << 4);   // set bit 4
    } else {  // animations off
        reg &= ~(1 << 4);  // clear bit 4
    }
    aw2016_write_reg(0x31 + channel, reg);
}

// sets pwm of a single channel
void aw2016_set_led (uint8_t channel, uint8_t pwm) {
    aw2016_write_reg(0x34 + channel, pwm);
}

#define AW2016_REG_GCR2 0x04
// set the global current limit from 5mA to 30mA
static void aw2016_set_global_current (aw2016_current_t cur) {
    uint8_t reg;
    // read the current value of GCR2
    reg = twi_read_byte(AW2016_ADDR, AW2016_REG_GCR2);
    // clear bits 1:0 (mask 0b11111100)
    reg &= ~0x03;
    // insert new setting into bits 1:0
    reg |= (uint8_t)cur;
    // write back
    aw2016_write_reg(AW2016_REG_GCR2, reg);
}

// adjust the color balance and power, with values of 0 to 15 per color
static void aw2016_set_rgb_calibration (uint8_t r, uint8_t g, uint8_t b) {
    // get register values
    uint8_t reg_r = twi_read_byte(AW2016_ADDR, 0x31 + AW_R_CH);
    uint8_t reg_g = twi_read_byte(AW2016_ADDR, 0x31 + AW_G_CH);
    uint8_t reg_b = twi_read_byte(AW2016_ADDR, 0x31 + AW_B_CH);

    // replace lower 4 bits with new value
    reg_r = (reg_r & 0xf0) | (r & 0x0f);
    reg_g = (reg_g & 0xf0) | (g & 0x0f);
    reg_b = (reg_b & 0xf0) | (b & 0x0f);

    // save new register values
    aw2016_write_reg(0x31 + AW_R_CH, reg_r);
    aw2016_write_reg(0x31 + AW_G_CH, reg_g);
    aw2016_write_reg(0x31 + AW_B_CH, reg_b);
}

// sets all RGB channels to the same PWM value
static void aw2016_set_global_pwm (uint8_t pwm) {
    for (uint8_t i=0; i<3; i++) aw2016_set_led(i, pwm);
}


////////// stock animations //////////

// rainbow fade but pauses at r, g, and b
void aw2016_rainbow_rgb () {
    // animations must be off during setup
    aw2016_set_animation_mode(false);

    // only "255" works; anything else makes the chip glitch
    aw2016_set_global_pwm(255);

    // set T1/T2
    aw2016_write_reg(0x37 + (AW_R_CH * 3), 0x66);
    aw2016_write_reg(0x37 + (AW_G_CH * 3), 0x66);
    aw2016_write_reg(0x37 + (AW_B_CH * 3), 0x66);

    // set T3/T4
    aw2016_write_reg(0x38 + (AW_R_CH * 3), 0x6A);
    aw2016_write_reg(0x38 + (AW_G_CH * 3), 0x6A);
    aw2016_write_reg(0x38 + (AW_B_CH * 3), 0x6A);

    // set T0/Repeat
    aw2016_write_reg(0x39 + (AW_R_CH * 3), 0x00);
    aw2016_write_reg(0x39 + (AW_G_CH * 3), 0x80);
    aw2016_write_reg(0x39 + (AW_B_CH * 3), 0xB0);

    // start the animation
    aw2016_set_animation_mode(true);
    aw2016_enable_leds();

    aw2016_animation_running = 1;
}

// smooth rainbow fade
// use analog control to adjust brightness
void aw2016_rainbow_blend () {
    // animations must be off during setup
    aw2016_set_animation_mode(false);

    // only "255" works; anything else makes the chip glitch
    aw2016_set_global_pwm(255);

    // set T1/T2
    aw2016_write_reg(0x37 + (AW_R_CH * 3), 0x80);
    aw2016_write_reg(0x37 + (AW_G_CH * 3), 0x80);
    aw2016_write_reg(0x37 + (AW_B_CH * 3), 0x80);

    // set T3/T4
    aw2016_write_reg(0x38 + (AW_R_CH * 3), 0x88);
    aw2016_write_reg(0x38 + (AW_G_CH * 3), 0x88);
    aw2016_write_reg(0x38 + (AW_B_CH * 3), 0x88);

    // set T0/Repeat
    aw2016_write_reg(0x39 + (AW_R_CH * 3), 0x00);
    aw2016_write_reg(0x39 + (AW_G_CH * 3), 0x80);
    aw2016_write_reg(0x39 + (AW_B_CH * 3), 0xB0);

    // start the animation
    aw2016_set_animation_mode(true);
    aw2016_enable_leds();

    aw2016_animation_running = 1;
}


////////// TWI / I2C stuff //////////

static uint8_t twi_read_byte (uint8_t addr, uint8_t reg) {
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

static uint8_t twi_write (uint8_t addr, uint8_t *data, uint8_t len) {
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

static void aw2016_write_reg (uint8_t reg, uint8_t val) {
    uint8_t buf[2] = { reg, val };
    twi_write(AW2016_ADDR, buf, 2);
}

