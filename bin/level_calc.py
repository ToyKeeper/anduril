#!/usr/bin/env python

import math

def main(args):
    """Calculates PWM levels for visually-linear steps.
    """
    # Get parameters from the user
    v = dict()
    questions = [
            (int, 'pwm_min', 6, 'Lowest visible PWM level, for moon mode:'),
            (float, 'lm_min', 0.25, 'How bright is moon mode, in lumens?'),
            (int, 'pwm_max', 255, 'Highest PWM level:'),
            (float, 'lm_max', 1000, 'How bright is the highest level, in lumens?'),
            (int, 'num_levels', 4, 'How many total levels do you want?'),
            ]
    for typ, name, default, text in questions:
        value = get_value(text, default, args)
        if not value:
            value = default
        else:
            value = typ(value)
        v[name] = value

    # change these values for each device:
    #pwm_min = 0     # lowest visible PWM level, for moon mode
    #lm_min = 10.0   # how bright is moon mode, in lumens?
    #pwm_max = 255   # highest PWM level
    #lm_max = 1300   # how bright is the highest level, in lumens?
    #num_levels = 4  # how many total levels do you want?

    # The rest should work fine without changes
    visual_min = math.pow(v['lm_min'], 1.0/3)
    visual_max = math.pow(v['lm_max'], 1.0/3)
    step_size = (visual_max - visual_min) / (v['num_levels']-1)
    modes = []
    goal = visual_min
    for i in range(v['num_levels']):
        pwm_float = (((goal**3) / v['lm_max']) * (256-v['pwm_min'])) + v['pwm_min'] - 1
        pwm = int(round(pwm_float))
        pwm = max(min(pwm,v['pwm_max']),v['pwm_min'])
        modes.append(pwm)
        print '%i: visually %.2f (%.2f lm): %.2f/255' % (i+1, goal, goal ** 3, pwm_float)
        goal += step_size

    print 'PWM values:', ','.join([str(i) for i in modes])

def get_value(text, default, args):
    if args:
        result = args[0]
        del args[0]
    else:
        print text, '(%s)' % (default),
        result = raw_input()
    result = result.strip()
    return result

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

