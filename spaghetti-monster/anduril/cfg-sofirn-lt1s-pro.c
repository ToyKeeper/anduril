// this is inserted into fsm-ramping.c :: set_level()
// (it overrides part of the function, but not all of it)
uint8_t output_mux; // pre-define this variable since the overrides file gets included before the ramp-mode.h file
inline void set_level_override(uint8_t level) {
    if (level == 0) {  // off
        TINT1_LVL = 0;  // disable the first white channel
        TINT2_LVL = 0;  // disable the second white channel
        PWM3_LVL = 0;   // disable the red LEDs
    } else {
        level --;

        if (output_mux == 0) {  // main white LEDs
            PWM3_LVL = 0;  // disable the red LEDs
            PWM1_LVL = PWM_GET(pwm1_levels, level);  // get the PWM value
            update_tint();  // set the warm-cool level balance
        } else {  // red LEDs
            TINT1_LVL = 0;  // disable the first white channel
            TINT2_LVL = 0;  // disable the second white channel
            PWM3_LVL = PWM_GET(pwm1_levels, level);  // set the red LED PWM
        }
    }
}