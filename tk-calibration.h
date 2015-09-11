#ifndef TK_CALIBRATION_H
#define TK_CALIBRATION_H
/*
 * Attiny calibration header.
 * This allows using a single set of hardcoded values across multiple projects.
 *
 * Copyright (C) 2015 Selene ToyKeeper
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
 *
 */

/********************** Voltage ADC calibration **************************/
// These values were measured using RMM's FET+7135.
// See battcheck/readings.txt for reference values.
#define ADC_42          195 // the ADC value we expect for 4.20 volts
#define ADC_100         195 // the ADC value for 100% full (4.2V resting)
#define ADC_75          186 // the ADC value for 75% full (4.0V resting)
#define ADC_50          176 // the ADC value for 50% full (3.8V resting)
#define ADC_25          162 // the ADC value for 25% full (3.5V resting)
#define ADC_0           138 // the ADC value for 0% full (3.0V resting)
#define ADC_LOW         129 // When do we start ramping down (2.8V)
#define ADC_CRIT        124 // When do we shut the light off (2.7V)


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


#endif  // TK_CALIBRATION_H
