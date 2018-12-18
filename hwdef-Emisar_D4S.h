#ifndef HWDEF_EMISAR_D4S_H
#define HWDEF_EMISAR_D4S_H

/* Emisar D4S driver layout
 */
// same as a D4, basically
#include "hwdef-Emisar_D4.h"

// ... except the D4S has aux LEDs under the optic
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

#endif
