// Noctigon KR4 tint-ramping config options for Anduril
// (basically the same as Emisar D4S V2 tint-ramping,
// but switch on a different pin, and no lighted button)
// ATTINY: 1634
#include "hwdef-Noctigon_KR4-tintramp.h"
#include "cfg-emisar-d4sv2-tintramp.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0215"

// the button doesn't light up
#undef USE_BUTTON_LED

