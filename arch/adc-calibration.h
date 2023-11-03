// tk-calibration.h: Attiny calibration header.
// Copyright (C) 2015-2023 Selene ToyKeeper
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// This allows using a single set of hardcoded values across multiple projects.

/********************** Voltage ADC calibration **************************/
// These values were measured using RMM's FET+7135.
// See battcheck/readings.txt for reference values.
// the ADC values we expect for specific voltages
#define ADC_44     194
#define ADC_43     189
#define ADC_42     184
#define ADC_41     178
#define ADC_40     173
#define ADC_39     168
#define ADC_38     163
#define ADC_37     158
#define ADC_36     152
#define ADC_35     147
#define ADC_34     142
#define ADC_33     137
#define ADC_32     131
#define ADC_31     126
#define ADC_30     121
#define ADC_29     116
#define ADC_28     111
#define ADC_27     105
#define ADC_26     100
#define ADC_25     95
#define ADC_24     90
#define ADC_23     84
#define ADC_22     79
#define ADC_21     74
#define ADC_20     69

#define ADC_100p   ADC_42  // the ADC value for 100% full (resting)
#define ADC_75p    ADC_40  // the ADC value for 75% full (resting)
#define ADC_50p    ADC_38  // the ADC value for 50% full (resting)
#define ADC_25p    ADC_35  // the ADC value for 25% full (resting)
#define ADC_0p     ADC_30  // the ADC value for 0% full (resting)
#define ADC_LOW    ADC_30  // When do we start ramping down
#define ADC_CRIT   ADC_27  // When do we shut the light off


/********************** Offtime capacitor calibration ********************/
// Values are between 1 and 255, and can be measured with offtime-cap.c
// See battcheck/otc-readings.txt for reference values.
// These #defines are the edge boundaries, not the center of the target.
#ifdef OFFTIM3
// The OTC value 0.5s after being disconnected from power
// (anything higher than this is a "short press")
#define CAP_SHORT           190
// The OTC value 1.5s after being disconnected from power
// Between CAP_MED and CAP_SHORT is a "medium press"
#define CAP_MED             94
// Below CAP_MED is a long press
#else
// The OTC value 1.0s after being disconnected from power
// Anything higher than this is a short press, lower is a long press
#define CAP_SHORT           115
#endif

