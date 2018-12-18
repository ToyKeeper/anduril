#ifndef HWDEF_BLF_GT_MINI_H
#define HWDEF_BLF_GT_MINI_H

/* BLF/Lumintop GT Mini driver layout
 */
// exactly the same as a D1S, but with a lighted button
#include "hwdef-Emisar_D1S.h"

// lighted button
#ifndef AUXLED_PIN
#define AUXLED_PIN   PB4    // pin 3
#endif

#endif
