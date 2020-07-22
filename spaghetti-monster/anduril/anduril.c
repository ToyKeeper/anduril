/*
 * Anduril: Narsil-inspired UI for SpaghettiMonster.
 * (Anduril is Aragorn's sword, the blade Narsil reforged)
 *
 * Copyright (C) 2017-2019 Selene ToyKeeper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Usually a program would be structured like this...
// - Library headers
// - App headers
// - App code
//
// ... in each source file.
// ... and each library and part of the program would be linked together.
//
// But this doesn't follow that pattern, because it's using the
//   -fwhole-program
// flag to reduce the compiled size.  It lets us fit more features
// in a tiny MCU chip's ROM.
//
// So the structure is like this instead...
// - App-level configuration headers
//   - Default config
//   - Per build target config
// - Library-level configuration headers
// - Library code (FSM itself)
// - App headers
// - App code (all of it, inline)
//
// Don't do this in regular programs.  It's weird and kind of gross.
// But in this case it gives us a bunch of much-needed space, so... woot.
//
// Also, there are a ton of compile-time flags because it needs to build
// a bunch of different versions and each one needs to be trimmed as small
// as possible.  These are mostly "USE" flags.

/********* User-configurable options *********/
#include "config-default.h"

/********* specific settings for known driver types *********/
// Anduril config file name (set it here or define it at the gcc command line)
//#define CONFIGFILE cfg-blf-q8.h

#include "tk.h"
#include incfile(CONFIGFILE)


/********* Include headers which need to be before FSM *********/

// enable FSM features needed by basic ramping functions
#include "ramping-fsm.h"

#ifdef USE_FACTORY_RESET
#include "factory-reset-fsm.h"
#endif

#ifdef USE_BATTCHECK_MODE
#include "battcheck-mode-fsm.h"
#endif

// enable FSM features needed by strobe modes
#include "strobes-fsm.h"

// figure out how many bytes of eeprom are needed,
// based on which UI features are enabled
// (include this one last)
#include "load-save-config-fsm.h"


/********* bring in FSM / SpaghettiMonster *********/
#define USE_IDLE_MODE  // reduce power use while awake and no tasks are pending

#include "spaghetti-monster.h"


/********* Include all the regular app headers *********/

#include "off-state.h"
#include "ramping.h"
#include "load-save-config.h"
#include "config-mode.h"
#include "misc.h"

#ifdef USE_VERSION_CHECK
#include "version-check-mode.h"
#endif

#ifdef USE_BATTCHECK_MODE
#include "battcheck-mode.h"
#endif

#ifdef USE_GOODNIGHT_MODE
#include "goodnight-mode.h"
#endif

#ifdef USE_BEACON_MODE
#include "beacon-mode.h"
#endif

#ifdef USE_THERMAL_REGULATION
#include "tempcheck-mode.h"
#endif

#ifdef USE_LOCKOUT_MODE
#include "lockout.h"
#endif

#ifdef USE_MOMENTARY_MODE
#include "momentary-mode.h"
#endif

#ifdef USE_TINT_RAMPING
#include "tint-ramping.h"
#endif

#ifdef USE_FACTORY_RESET
#include "factory-reset.h"
#endif

// this one detects its own enable/disable settings
#include "strobes.h"

#ifdef USE_SOS_MODE
#include "sos-mode.h"
#endif

// FSM states

#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
void indicator_blink(uint8_t arg);
#endif
#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t setting_rgb_mode_now = 0;
void rgb_led_update(uint8_t mode, uint8_t arg);
void rgb_led_voltage_readout(uint8_t bright);
/*
 * 0: R
 * 1: RG
 * 2:  G
 * 3:  GB
 * 4:   B
 * 5: R B
 * 6: RGB
 * 7: rainbow
 * 8: voltage
 */
const PROGMEM uint8_t rgb_led_colors[] = {
    0b00000001,  // 0: red
    0b00000101,  // 1: yellow
    0b00000100,  // 2: green
    0b00010100,  // 3: cyan
    0b00010000,  // 4: blue
    0b00010001,  // 5: purple
    0b00010101,  // 6: white
};
#define RGB_LED_NUM_COLORS 10
#define RGB_LED_NUM_PATTERNS 4
#ifndef RGB_LED_OFF_DEFAULT
//#define RGB_LED_OFF_DEFAULT 0x18  // low, voltage
#define RGB_LED_OFF_DEFAULT 0x17  // low, rainbow
#endif
#ifndef RGB_LED_LOCKOUT_DEFAULT
#define RGB_LED_LOCKOUT_DEFAULT 0x37  // blinking, rainbow
#endif
#ifndef RGB_RAINBOW_SPEED
#define RGB_RAINBOW_SPEED 0x0f  // change color every 16 frames
#endif
uint8_t rgb_led_off_mode = RGB_LED_OFF_DEFAULT;
uint8_t rgb_led_lockout_mode = RGB_LED_LOCKOUT_DEFAULT;
#endif


#ifdef USE_INDICATOR_LED
    // bits 2-3 control lockout mode
    // bits 0-1 control "off" mode
    // modes are: 0=off, 1=low, 2=high, 3=blinking (if TICK_DURING_STANDBY enabled)
    #ifdef INDICATOR_LED_DEFAULT_MODE
    uint8_t indicator_led_mode = INDICATOR_LED_DEFAULT_MODE;
    #else
        #ifdef USE_INDICATOR_LED_WHILE_RAMPING
        //uint8_t indicator_led_mode = (1<<2) + 2;
        uint8_t indicator_led_mode = (2<<2) + 1;
        #else
        uint8_t indicator_led_mode = (3<<2) + 1;
        #endif
    #endif
#endif


/********* Include all the app logic source files *********/
// (is a bit weird to do things this way,
//  but it saves a lot of space by letting use use the -fwhole-program flag)

#include "off-state.c"
#include "ramping.c"
#include "load-save-config.c"
#include "config-mode.c"
#include "misc.c"

#ifdef USE_VERSION_CHECK
#include "version-check-mode.c"
#endif

#ifdef USE_BATTCHECK_MODE
#include "battcheck-mode.c"
#endif

#ifdef USE_GOODNIGHT_MODE
#include "goodnight-mode.c"
#endif

#ifdef USE_BEACON_MODE
#include "beacon-mode.c"
#endif

#ifdef USE_THERMAL_REGULATION
#include "tempcheck-mode.c"
#endif

#ifdef USE_LOCKOUT_MODE
#include "lockout.c"
#endif

#ifdef USE_MOMENTARY_MODE
#include "momentary-mode.c"
#endif

#ifdef USE_TINT_RAMPING
#include "tint-ramping.c"
#endif

#ifdef USE_FACTORY_RESET
#include "factory-reset.c"
#endif

#ifdef USE_STROBE_STATE
#include "strobes.c"
#endif

#ifdef USE_SOS_MODE
#include "sos-mode.c"
#endif


#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
// beacon-like mode for the indicator LED
void indicator_blink(uint8_t arg) {
    // turn off aux LEDs when battery is empty
    if (voltage < VOLTAGE_LOW) { indicator_led(0); return; }

    #ifdef USE_FANCIER_BLINKING_INDICATOR

    // fancy blink, set off/low/high levels here:
    uint8_t seq[] = {0, 1, 2, 1,  0, 0, 0, 0,
                     0, 0, 1, 0,  0, 0, 0, 0};
    indicator_led(seq[arg & 15]);

    #else  // basic blink, 1/8th duty cycle

    if (! (arg & 7)) {
        indicator_led(2);
    }
    else {
        indicator_led(0);
    }

    #endif
}
#endif

#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t voltage_to_rgb() {
    uint8_t levels[] = {
    // voltage, color
          0, 0, // 0, R
         33, 1, // 1, R+G
         35, 2, // 2,   G
         37, 3, // 3,   G+B
         39, 4, // 4,     B
         41, 5, // 5, R + B
         44, 6, // 6, R+G+B  // skip; looks too similar to G+B
        255, 6, // 7, R+G+B
    };
    uint8_t volts = voltage;
    if (volts < 29) return 0;

    uint8_t i;
    for (i = 0;  volts >= levels[i];  i += 2) {}
    uint8_t color_num = levels[(i - 2) + 1];
    return pgm_read_byte(rgb_led_colors + color_num);
}

// do fancy stuff with the RGB aux LEDs
// mode: 0bPPPPCCCC where PPPP is the pattern and CCCC is the color
// arg: time slice number
void rgb_led_update(uint8_t mode, uint8_t arg) {
    static uint8_t rainbow = 0;  // track state of rainbow mode
    static uint8_t frame = 0;  // track state of animation mode

    // turn off aux LEDs when battery is empty
    // (but if voltage==0, that means we just booted and don't know yet)
    uint8_t volts = voltage;  // save a few bytes by caching volatile value
    if ((volts) && (volts < VOLTAGE_LOW)) {
        rgb_led_set(0);
        #ifdef USE_BUTTON_LED
        button_led_set(0);
        #endif
        return;
    }

    uint8_t pattern = (mode>>4);  // off, low, high, blinking, ... more?
    uint8_t color = mode & 0x0f;

    // preview in blinking mode is awkward... use high instead
    if ((! go_to_standby) && (pattern > 2)) { pattern = 2; }


    const uint8_t *colors = rgb_led_colors;
    uint8_t actual_color = 0;
    if (color < 7) {  // normal color
        actual_color = pgm_read_byte(colors + color);
    }
    else if (color == 7) {  // rainbow
        uint8_t speed = 0x03;  // awake speed
        if (go_to_standby) speed = RGB_RAINBOW_SPEED;  // asleep speed
        if (0 == (arg & speed)) {
            rainbow = (rainbow + 1) % 6;
        }
        actual_color = pgm_read_byte(colors + rainbow);
    }
    else {  // voltage
        // show actual voltage while asleep...
        if (go_to_standby) {
            actual_color = voltage_to_rgb();
            // choose a color based on battery voltage
            //if (volts >= 38) actual_color = pgm_read_byte(colors + 4);
            //else if (volts >= 33) actual_color = pgm_read_byte(colors + 2);
            //else actual_color = pgm_read_byte(colors + 0);
        }
        // ... but during preview, cycle colors quickly
        else {
            actual_color = pgm_read_byte(colors + (((arg>>1) % 3) << 1));
        }
    }

    // pick a brightness from the animation sequence
    if (pattern == 3) {
        // uses an odd length to avoid lining up with rainbow loop
        uint8_t animation[] = {2, 1, 0, 0,  0, 0, 0, 0,  0,
                               1, 0, 0, 0,  0, 0, 0, 0,  0, 1};
        frame = (frame + 1) % sizeof(animation);
        pattern = animation[frame];
    }
    uint8_t result;
    #ifdef USE_BUTTON_LED
    uint8_t button_led_result;
    #endif
    switch (pattern) {
        case 0:  // off
            result = 0;
            #ifdef USE_BUTTON_LED
            button_led_result = 0;
            #endif
            break;
        case 1:  // low
            result = actual_color;
            #ifdef USE_BUTTON_LED
            button_led_result = 1;
            #endif
            break;
        default:  // high
            result = (actual_color << 1);
            #ifdef USE_BUTTON_LED
            button_led_result = 2;
            #endif
            break;
    }
    rgb_led_set(result);
    #ifdef USE_BUTTON_LED
    button_led_set(button_led_result);
    #endif
}

void rgb_led_voltage_readout(uint8_t bright) {
    uint8_t color = voltage_to_rgb();
    if (bright) color = color << 1;
    rgb_led_set(color);
}
#endif


void low_voltage() {
    StatePtr state = current_state;

    // TODO: turn off aux LED(s) when power is really low

    if (0) {}  // placeholder

    #ifdef USE_STROBE_STATE
    // "step down" from strobe to something low
    else if (state == strobe_state) {
        set_state(steady_state, RAMP_SIZE/6);
    }
    #endif

    // in normal mode, step down or turn off
    else if (state == steady_state) {
        if (actual_level > 1) {
            uint8_t lvl = (actual_level >> 1) + (actual_level >> 2);
            set_level_and_therm_target(lvl);
        }
        else {
            set_state(off_state, 0);
        }
    }
    // all other modes, just turn off when voltage is low
    else {
        set_state(off_state, 0);
    }
}


void setup() {
    #ifdef START_AT_MEMORIZED_LEVEL
    // dual switch: e-switch + power clicky
    // power clicky acts as a momentary mode
    load_config();

    if (button_is_pressed())
        // hold button to go to moon
        push_state(steady_state, 1);
    else
        // otherwise use memory
        push_state(steady_state, memorized_level);

    #else  // if not START_AT_MEMORIZED_LEVEL

    // blink at power-on to let user know power is connected
    set_level(RAMP_SIZE/8);
    delay_4ms(3);
    set_level(0);

    #ifdef USE_FACTORY_RESET
    if (button_is_pressed())
        factory_reset();
    #endif

    load_config();

    #ifdef USE_TINT_RAMPING
    // add tint ramping underneath every other state
    push_state(tint_ramping_state, 0);
    #endif  // ifdef USE_TINT_RAMPING

    push_state(off_state, 1);

    #endif  // ifdef START_AT_MEMORIZED_LEVEL
}


void loop() {

    StatePtr state = current_state;

    #ifdef USE_AUX_RGB_LEDS_WHILE_ON
        if (! setting_rgb_mode_now) rgb_led_voltage_readout(1);
    #endif

    if (0) {}

    #ifdef USE_VERSION_CHECK
    else if (state == version_check_state) {
        version_check_iter();
    }
    #endif

    #ifdef USE_STROBE_STATE
    else if ((state == strobe_state)
         #ifdef USE_MOMENTARY_MODE
         // also handle momentary strobes
         ||  ((state == momentary_state) && (momentary_mode == 1) && (momentary_active))
         #endif
         ) {
        strobe_state_iter();
    }
    #endif  // #ifdef USE_STROBE_STATE

    #ifdef USE_BORING_STROBE_STATE
    else if (state == boring_strobe_state) {
        switch(boring_strobe_type) {
            #ifdef USE_POLICE_STROBE_MODE
            case 0: // police strobe
                police_strobe_iter();
                break;
            #endif

            #ifdef USE_SOS_MODE_IN_FF_GROUP
            default: // SOS
                sos_mode_iter();
                break;
            #endif
        }
    }
    #endif  // #ifdef USE_BORING_STROBE_STATE

    #ifdef USE_BATTCHECK
    else if (state == battcheck_state) {
        battcheck();
        #ifdef USE_SIMPLE_UI
        // in simple mode, turn off after one readout
        if (simple_ui_active) set_state(off_state, 0);
        #endif
    }
    #endif

    #ifdef USE_BEACON_MODE
    else if (state == beacon_state) {
        beacon_mode_iter();
    }
    #endif

    #ifdef USE_SOS_MODE_IN_BLINKY_GROUP
    else if (state == sos_state) {
        sos_mode_iter();
    }
    #endif

    #ifdef USE_THERMAL_REGULATION
    // TODO: blink out therm_ceil during thermal_config_state?
    else if (state == tempcheck_state) {
        blink_num(temperature);
        nice_delay_ms(1000);
    }
    #endif

    #ifdef USE_IDLE_MODE
    else {
        // doze until next clock tick
        idle_mode();
    }
    #endif

}
