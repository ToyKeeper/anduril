#ifdef USE_SET_LEVEL_GRADUALLY
//#undef USE_SET_LEVEL_GRADUALLY
#endif

inline void set_level_override(uint8_t level) {
    if (level == 0) {
        PWM1_LVL = 0;
        PWM2_LVL = 0;
        PWM3_LVL = 0;
        // disable the power channel
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
        LED2_ENABLE_PORT &= ~(1 << LED2_ENABLE_PIN);
    } else {
        level --;

        if (! tint) {  // main LEDs
            // enable this power channel
            LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
            // disable other power channel
            LED2_ENABLE_PORT &= ~(1 << LED2_ENABLE_PIN);
            // set levels
            PWM1_LVL = PWM_GET(pwm1_levels, level);
            PWM2_LVL = (uint8_t)(PWM_GET(pwm2_levels, level) >> 2);
            PWM3_LVL = 0;
        } else {  // 2nd LEDs
            // disable other power channel
            LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
            // enable this power channel
            LED2_ENABLE_PORT |= (1 << LED2_ENABLE_PIN);
            // set levels
            PWM1_LVL = 0;
            PWM2_LVL = 0;
            PWM3_LVL = PWM_GET(pwm3_levels, level);
        }
    }
}
