#!/usr/bin/env python3

#
# Calculates DAC values and Vref values from a list of raw intensity values
# Usage: dac-scale.py 5,11,18,25,...,370474,384985,400000
# Output: #defines suitable for use in hw/*/anduril.h
# Assumptions:
# - If DAC max is 1023:
#   - DAC data range is 0 to 1023
#   - DAC Vrefs are 1.024V and 2.5V
#   - HDR is available and has 2 steps
#   - HDR channel ratio is "highest_ramp_value / 2500"
# - If DAC max is 255:
#   - DAC data range is 0 to 255
#   - DAC Vrefs are 0.55V and 2.5V
#   - HDR is available and has 2 steps
#   - HDR channel ratio is "highest_ramp_value / 2500"
#
# Output values thus have 4 "engine gears":
# - HDR low , 1.024V Vref  (or 0.55V)
# - HDR low , 2.5  V Vref
# - HDR high, 1.024V Vref  (or 0.55V)
# - HDR high, 2.5  V Vref
#

def main(args):
    # determine highest value for ramp and MCU internal limit
    max_pwm = 1023
    mcu_max_pwm = 1023
    if (len(args) > 1):
        max_pwm = int(args[0])
        del args[0]
    if (len(args) > 1):
        mcu_max_pwm = int(args[0])
        del args[0]

    if mcu_max_pwm < 256:
        low_vref_name = 'V05'
        low_vref_float = 0.55
        low_vref_int = 550
    else:
        low_vref_name = 'V10'
        low_vref_float = 1.024
        low_vref_int = 1024

    hi_vref_name = 'V25'
    hi_vref_float = 2.5
    hi_vref_int = 2500

    # get desired brightness levels and set up vars
    raw_pwm = [int(x) for x in args[0].split(',')]
    ratio = raw_pwm[-1] / float(hi_vref_int)
    cooked = [[]]

    def limit(p):
        return min(mcu_max_pwm, int(p))

    def turbo_limit(p):
        return min(max_pwm, int(p))

    phase = 0
    for raw in raw_pwm:
        # limit maximum power maybe
        if max_pwm != mcu_max_pwm:
            raw = raw * max_pwm / mcu_max_pwm
        # low gear, low vref
        if 0 == phase:
            if raw <= low_vref_int:
                cooked[-1].append(limit(raw * mcu_max_pwm / low_vref_int))
            else:
                phase += 1
                cooked.append([])
        # low gear, high vref
        if 1 == phase:
            if raw <= hi_vref_int:
                cooked[-1].append(limit(raw * mcu_max_pwm / hi_vref_int))
            else:
                phase += 1
                cooked.append([])
        # high gear, low vref
        if 2 == phase:
            if raw <= int(low_vref_int * ratio):
            #if raw <= int(mcu_max_pwm * ratio):
                cooked[-1].append(limit(raw * mcu_max_pwm / low_vref_int / ratio))
            else:
                phase += 1
                cooked.append([])
        # high gear, high vref
        if 3 == phase:
            cooked[-1].append(turbo_limit(raw * mcu_max_pwm / hi_vref_int / ratio))

        #print(f'{phase}: {raw}: {cooked[-1][-1]}')

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
    lines.append('        ' + fmt_tops(low_vref_name, cooked[0]) + ', \\')
    lines.append('        ' + fmt_tops('V25',         cooked[1]) + ', \\')
    lines.append('        ' + fmt_tops(low_vref_name, cooked[2]) + ', \\')
    lines.append('        ' + fmt_tops('V25',         cooked[3]))

    lines.append('#define MAX_1x7135           %3i' % b2)
    lines.append('#define HDR_ENABLE_LEVEL_MIN %3i' % (b2+1))

    print('\n'.join(lines))

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

