#include "tk-attiny.h"

//
typedef uint8_t (*EventCallbackPtr)(EventPtr event, uint16_t arg);
typedef uint8_t EventCallback(EventPtr event, uint16_t arg);
// FIXME: Why does a state need a number?  Why not just a function pointer?
//        (I don't think it actually needs a number...)
typedef struct State {
    uint8_t num;
    EventCallback event_callback;
} State;
typedef struct State* StatePtr;

volatile StatePtr current_state;
#define EV_MAX_LEN 16
volatile uint8_t current_event[EV_MAX_LEN];

volatile int16_t voltage;
#ifdef USE_THERMAL_REGULATION
volatile int16_t temperature;
#endif

#define A_ENTER_STATE     1
#define A_LEAVE_STATE     2
#define A_TICK            3
#define A_PRESS           4
#define A_HOLD_START      5
#define A_HOLD_TICK       6
#define A_RELEASE         7
#define A_RELEASE_TIMEOUT 8
// TODO: add events for over/under-heat conditions (with parameter for severity)
#define A_OVERHEATING     9
#define A_UNDERHEATING    10
// TODO: add events for low voltage conditions
#define A_VOLTAGE_LOW     11
#define A_VOLTAGE_CRITICAL 12

// TODO: maybe compare events by number instead of pointer?
//       (number = index in event types array)
//       (comparison would use full event content, but send off index to callbacks)
//       (saves space by using uint8_t instead of a pointer)
//       (also eliminates the need to duplicate single-entry events like for voltage or timer tick)

// Event types
typedef PROGMEM const uint8_t Event;
typedef Event* EventPtr;
Event EV_enter_state[] = {
    A_ENTER_STATE,
    0 } ;
Event EV_leave_state[] = {
    A_LEAVE_STATE,
    0 } ;
Event EV_tick[] = {
    A_TICK,
    0 } ;
Event EV_press[] = {
    A_PRESS,
    0 };
// shouldn't normally happen, but UI might reset event while button is down
// so a release with no recorded prior hold could be possible
Event EV_release[] = {
    A_RELEASE,
    0 };
Event EV_press_release[] = {
    A_PRESS,
    A_RELEASE,
    0 };
#define EV_1click EV_press_release_timeout
Event EV_press_release_timeout[] = {
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
#define EV_hold EV_press_hold
// FIXME: Should holds use "start+tick" or just "tick" with a tick number?
//        Or "start+tick" with a tick number?
Event EV_press_hold[] = {
    A_PRESS,
    A_HOLD_TIMEOUT,
    0 };
Event EV_press_hold_release[] = {
    A_PRESS,
    A_HOLD_TIMEOUT,
    A_RELEASE,
    0 };
Event EV_press_release_press[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    0 };
Event EV_press_release_press_release[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    0 };
#define EV_2clicks EV_press_release_press_release_timeout
Event EV_press_release_press_release_timeout[] = {
    A_PRESS,
    A_RELEASE,
    A_PRESS,
    A_RELEASE,
    A_RELEASE_TIMEOUT,
    0 };
// ... and so on

// A list of event types for easy iteration
EventPtr event_sequences[] = {
    EV_press,
    EV_press_release,
    EV_press_release_timeout,
    EV_press_release_press,
    EV_press_release_press_release,
    EV_press_release_press_release_timeout,
    // ...
};

// TODO: move this to a separate UI-specific file
/*
State states[] = {

};
*/

// TODO? add events to a queue when inside an interrupt
//       instead of calling the event functions directly?
//       (then empty the queue in main loop?)

// TODO? new delay() functions which handle queue consumption?
// TODO? new interruptible delay() functions?

//static uint8_t ticks_since_last_event = 0;  // TODO: 16 bits?

void WDT_tick() {
    timer ++;

    //static uint8_t hold_ticks = 0;  // TODO: 16 bits?

    // callback on each timer tick
    emit(EV_tick, timer);

    // if time since last event exceeds timeout,
    // append timeout to current event sequence, then
    // send event to current state callback
    // //hold_event(ticks)
    // //emit(EV_press_hold, hold_ticks);
    // emit_current(hold_ticks);
    // or
    // //release_timeout()
    // //emit(EV_press_release_timeout, 0);
    // emit_current(0);

    // add 4-step voltage / temperature thing?
    // with averaged values,
    // once every N ticks?
}

void button_change_interrupt() {
    // TODO: debounce a little

    //ticks_since_last_event = 0;  // something happened

    // TODO: add event to current sequence

    // check if sequence matches any defined sequences
    // if so, send event to current state callback
    emit_current(0);
}

uint8_t emit_current(uint16_t arg) {
    uint8_t err = 1;
    for (uint8_t i=0; i<sizeof(event_sequences); i++) {
        if (events_match(event_sequences[i], current_event)) {
            err = emit(event_sequences[i], arg);
            break;
        }
    }
    return err;
}

#define events_match(a,b) compare_event_sequences(a,b)
uint8_t compare_event_sequences(uint8_t *a, uint8_t *b) {
}

void empty_event_sequence() {
    for(uint8_t i=0; i<EV_MAX_LEN; i++) current_event[i] = 0;
}

void append_event(uint8_t ev_type) {
    ticks_since_last_event = 0;  // something happened
    uint8_t i;
    for(i=0; current_event[i], i<EV_MAX_LEN; i++);
    if (i < EV_MAX_LEN) {
        current_event[i] = ev_type;
    } else {
        // TODO: ... something?
    }
}

// TODO: stack for states, to allow shared utility states like "input a number"
//       and such, which return to the previous state after finishing
// TODO: if callback doesn't handle current event,
//       pass event to next state on stack?
//       Callback return values:
//       0: event handled normally
//       1: event not handled
//       255: error (not sure what this would even mean though, or what difference it would make)
// TODO: function to call stacked callbacks until one returns "handled"
uint8_t emit(EventPtr event, uint16_t arg) {
    // TODO: implement
    for(;;) {
        uint8_t err = foo.callback(event, arg);
        if (! err) return 0;
    }
    return 1;  // event not handled
}

uint8_t push_state(State *new_state) {
    // TODO: implement
}

State * pop_state() {
    // TODO: implement
}

uint8_t set_state(State *new_state) {
    pop_state();
    return push_state(new_state);
}

uint8_t _set_state(State *new_state) {
    // call old state-exit hook (don't use stack)
    current_state.callback(EV_leave_state, 0);
    // set new state
    current_state = new_state;
    // call new state-enter hook (don't use stack)
    current_state.callback(EV_enter_state, 0);
}

// TODO: implement
ISR(WDT_vect) {
}

// TODO: implement?  (or is it better done in main()?)
ISR(ADC_vect) {
    static uint8_t adc_step = 0;
    static uint8_t lvp_timer = 0;
    #define LVP_TIMER_START 255  // TODO: calibrate this value

    #ifdef USE_THERMAL_REGULATION
    #define NUM_THERMAL_VALUES 4
    #define ADC_STEPS 4
    static int16_t temperature_values[NUM_THERMAL_VALUES];
    #else
    #define ADC_STEPS 2
    #endif

    int16_t measurement = ADC;  // latest 10-bit ADC reading

    adc_step = (adc_step + 1) & (ADC_STEPS-1);

    // TODO: voltage
    // TODO: if low, callback EV_voltage_low / EV_voltage_critical
    //       (but only if it has been more than N ticks since last call)
    // if (lvp_timer) {
    //     lvp_timer --;
    // } else {
    //     if (voltage is low) {
    //         uint8_t err = emit(EV_voltage_low, 0);
    //         if (!err) lvp_timer = LVP_TIMER_START;
    //     }
    // }

    // TODO: temperature

    // start another measurement for next time
    #ifdef USE_THERMAL_REGULATION
    if (adc_step < 2) ADMUX = ADMUX_VCC;
    else ADMUX = ADMUX_THERM;
    #else
    ADMUX = ADMUX_VCC;
    #endif
}

inline void ADC_on()
{
    // read voltage on VCC by default
    // disable digital input on VCC pin to reduce power consumption
    //DIDR0 |= (1 << ADC_DIDR);  // FIXME: unsure how to handle for VCC pin
    // VCC / 1.1V reference
    ADMUX = ADMUX_VCC;
    // enable, start, prescale
    ADCSRA = (1 << ADEN) | (1 << ADSC) | ADC_PRSCL;
}

// low-power standby mode used while off but power still connected
#define standby_mode sleep_until_eswitch_pressed
void sleep_until_eswitch_pressed()
{
    WDT_off();
    ADC_off();

    // TODO: make sure switch isn't currently pressed
    while (switch_pressed()) {}

    PCINT_on();  // wake on e-switch event

    sleep_enable();
    sleep_bod_disable();
    sleep_cpu();  // wait here

    // something happened; wake up
    sleep_disable();
    PCINT_off();
    ADC_on();
    WDT_on();
}

// boot-time tasks
/* Define this in your RoundTable recipe
void setup() {
}
*/

void main() {
    // Don't allow interrupts while booting
    cli();
    WDT_off();
    PCINT_off();

    // TODO: configure PWM channels
    #if PWM_CHANNELS == 1
    #elif PWM_CHANNELS == 2
    #elif PWM_CHANNELS == 3
    #elif PWM_CHANNELS == 4
    // FIXME: How exactly do we do PWM on channel 4?
    #endif

    // TODO: turn on ADC?
    // TODO: configure e-switch
    //PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    //PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin

    // TODO: configure sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Read config values and saved state
    // restore_state();  // TODO

    // TODO: handle long press vs short press (or even medium press)?

    // TODO: call recipe's setup
    setup();

    // all booted -- turn interrupts back on
    PCINT_on();
    WDT_on();
    sei();

    // main loop
    while (1) {
        // TODO: update e-switch press state
        // TODO: if wait queue not empty, process and pop first item in queue
        // TODO: check voltage?
        // TODO: check temperature?
    }
}
