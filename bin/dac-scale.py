#!/usr/bin/env python

#
# Calculates DAC values and Vref values from a list of raw intensity values
# Usage: dac-scale.py 5,11,18,25,...,370474,384985,400000
# Output: #defines suitable for use in hw/*/anduril.h
# Assumptions:
# - DAC data range is 0 to 1023
# - DAC Vrefs are 1.024V and 2.5V
# - HDR is available and has 2 steps
# - HDR channel ratio is "highest_ramp_value / 2500"
#
# Output values thus have 4 "engine gears":
# - HDR low , 1.024V Vref
# - HDR low , 2.5  V Vref
# - HDR high, 1.024V Vref
# - HDR high, 2.5  V Vref
#

def main(args):
    max_pwm = 1023
    raw_pwm = [int(x) for x in args[0].split(',')]
    ratio = raw_pwm[-1] / 2500.0
    cooked = [[]]

    def limit(p):
        return min(max_pwm, int(p))

    phase = 0
    for raw in raw_pwm:
        if 0 == phase:
            if raw <= 1023:
                cooked[-1].append(limit(raw))
            else:
                phase += 1
                cooked.append([])
        if 1 == phase:
            if raw <= int(2500):
                cooked[-1].append(limit(raw * 1.024 / 2.5))
            else:
                phase += 1
                cooked.append([])
        if 2 == phase:
            if raw <= int(1024 * ratio):
                cooked[-1].append(limit(raw / ratio))
            else:
                phase += 1
                cooked.append([])
        if 3 == phase:
            cooked[-1].append(limit(raw * 1.024 / 2.5 / ratio))

    # "gear change" boundaries
    b1 =      len(cooked[0])
    b2 = b1 + len(cooked[1])
    b3 = b2 + len(cooked[2])
    b4 = b3 + len(cooked[3])

    #print(','.join(['%4i' % n for n in cooked]))

    def fmt_pwms(l):
        return ','.join(['%4i' % n for n in l])

    def fmt_tops(v, l):
        return ','.join([' %s' % v for n in l])

    lines = []

    lines.append('// top level for each "gear": %i %i %i %i' % (b1, b2, b3, b4))

    lines.append('#define PWM1_LEVELS \\')
    lines.append('        ' + fmt_pwms(cooked[0]) + ', \\')
    lines.append('        ' + fmt_pwms(cooked[1]) + ', \\')
    lines.append('        ' + fmt_pwms(cooked[2]) + ', \\')
    lines.append('        ' + fmt_pwms(cooked[3]))
    lines.append('#define PWM2_LEVELS \\')
    lines.append('        ' + fmt_tops('V10', cooked[0]) + ', \\')
    lines.append('        ' + fmt_tops('V25', cooked[1]) + ', \\')
    lines.append('        ' + fmt_tops('V10', cooked[2]) + ', \\')
    lines.append('        ' + fmt_tops('V25', cooked[3]))

    lines.append('#define MAX_1x7135           %3i' % b2)
    lines.append('#define HDR_ENABLE_LEVEL_MIN %3i' % (b2+1))

    print('\n'.join(lines))

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

