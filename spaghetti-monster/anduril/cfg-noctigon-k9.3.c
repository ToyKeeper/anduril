/*
 * K9.3 has unusual power channels, so it must override some of FSM's code.
 * There are two sets of LEDs:
 *   1. Main LEDs: (9 x white LEDs)
 *      PWM1 (10-bit, linear)
 *      PWM2 (8-bit, FET, only used on some K9.3 models)
 *   2. 2nd LEDs: (3 x white or color LEDs)
 *      PWM3 (10-bit, linear)
 *
 * The two sets are not used at the same time...  just one or the other,
 * depending on the "tint" variable.  (0 = main LEDs, other value = 2nd LEDs)
 */
// if the gradual adjustment mechanism doesn't work, disable it here:
//#ifdef USE_SET_LEVEL_GRADUALLY
//#undef USE_SET_LEVEL_GRADUALLY
//#endif

// this is inserted into fsm-ramping.c :: set_level()
// (it overrides part of the function, but not all of it)
inline void set_level_override(uint8_t level) {
    if (level == 0) {  // off
        PWM1_LVL = 0;
        PWM2_LVL = 0;
        PWM3_LVL = 0;
        // disable both power channels
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
            #ifndef K93_NO_FET
            PWM2_LVL = (uint8_t)(PWM_GET(pwm2_levels, level) >> 2);  // 8 bits
            #endif
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

// override fsm-ramping.c :: gradual_tick()
// (because the power handling on this light is weird)
// call this every frame or every few frames to change brightness very smoothly
void gradual_tick() {
    // go by only one ramp level at a time instead of directly to the target
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;

    gt --;  // convert 1-based number to 0-based

    PWM_DATATYPE target;

    if (! tint) {  // main LED channel
        target = PWM_GET(pwm1_levels, gt);
        if ((gt < actual_level)     // special case for FET-only turbo
                && (PWM1_LVL == 0)  // (bypass adjustment period for first step)
                && (target == PWM_TOP)) PWM1_LVL = PWM_TOP;
        else if (PWM1_LVL < target) PWM1_LVL ++;
        else if (PWM1_LVL > target) PWM1_LVL --;

        #ifndef K93_NO_FET  // skip this on E21A model
        target = PWM_GET(pwm2_levels, gt) >> 2;  // 8 bits, not 10
        if (PWM2_LVL < target) PWM2_LVL ++;
        else if (PWM2_LVL > target) PWM2_LVL --;
        #endif

        // did we go far enough to hit the next defined ramp level?
        // if so, update the main ramp level tracking var
        if ((PWM1_LVL == PWM_GET(pwm1_levels, gt))
                #ifndef K93_NO_FET
                && (PWM2_LVL == (PWM_GET(pwm2_levels, gt) >> 2))
                #endif
            )
        { actual_level = gt + 1; }
    } else {  // 2nd LED channel
        target = PWM_GET(pwm3_levels, gt);
        if (PWM3_LVL < target) PWM3_LVL ++;
        else if (PWM3_LVL > target) PWM3_LVL --;

        // did we go far enough to hit the next defined ramp level?
        // if so, update the main ramp level tracking var
        if ( PWM3_LVL == PWM_GET(pwm3_levels, gt) )
        { actual_level = gt + 1; }
    }

}

