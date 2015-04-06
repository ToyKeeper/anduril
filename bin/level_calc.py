#!/usr/bin/env python

import math

interactive = False

def main(args):
    """Calculates PWM levels for visually-linear steps.
    """
    # Get parameters from the user
    v = dict(pwm_max=255, pwm2_max=255)
    questions = [
            (int, 'num_levels', 4, 'How many total levels do you want?'),
            (int, 'pwm_min', 6, 'Lowest visible PWM level, for moon mode:'),
            (float, 'lm_min', 0.25, 'How bright is moon mode, in lumens?'),
            #(int, 'pwm_max', 255, 'Highest PWM level:'),
            (float, 'lm_max', 1000, 'How bright is the highest level, in lumens?'),
            (str, 'dual_pwm', 'n', 'Use dual PWM? [y/n]'),
            (float, 'pwm2_min', 6, 'Second channel, lowest visible PWM level:'),
            (float, 'lm2_min', 0.25, 'Second channel, how bright is the lowest mode, in lumens?'),
            #(float, 'pwm2_max', 255, 'Second channel, highest PWM level:'),
            (float, 'lm2_max', 140, 'Second channel, how bright is maximum, in lumens?'),
            ]
    for typ, name, default, text in questions:
        value = get_value(text, default, args)
        if not value:
            value = default
        else:
            value = typ(value)
        v[name] = value
        if (name == 'dual_pwm'  and  value == 'n'):
            # skip remaining questions if not using dual PWM
            break

    if v['dual_pwm'] == 'y':
        dual_pwm(v)
    else:
        single_pwm(v)

    if interactive: # Wait on exit, in case user invoked us by clicking an icon
        print 'Press Enter to exit:'
        raw_input()

def single_pwm(v):
    """Estimate the PWM levels for a one-channel driver."""
    visual_min = math.pow(v['lm_min'], 1.0/3)
    visual_max = math.pow(v['lm_max'], 1.0/3)
    step_size = (visual_max - visual_min) / (v['num_levels']-1)
    modes = []
    goal = visual_min
    for i in range(v['num_levels']):
        goal_lm = goal**3
        #pwm_float = ((goal_lm / v['lm_max']) * (256-v['pwm_min'])) + v['pwm_min'] - 1
        pwm_float = (((goal_lm-v['lm_min']) / (v['lm_max']-v['lm_min'])) \
                        * (255-v['pwm_min'])) \
                    + v['pwm_min']
        pwm = int(round(pwm_float))
        pwm = max(min(pwm,v['pwm_max']),v['pwm_min'])
        modes.append(pwm)
        print '%i: visually %.2f (%.2f lm): %.2f/255' % (i+1, goal, goal_lm, pwm_float)
        goal += step_size

    print 'PWM values:', ','.join([str(i) for i in modes])

def dual_pwm(v):
    """Estimate the PWM levels for a two-channel driver.
    Assume the first channel is the brighter one, and second will be used for moon/low modes.
    """
    visual_min = math.pow(v['lm2_min'], 1.0/3)
    visual_max = math.pow(v['lm_max'], 1.0/3)
    step_size = (visual_max - visual_min) / (v['num_levels']-1)
    modes = []
    goal = visual_min
    for i in range(v['num_levels']):
        goal_lm = goal**3
        # Up to the second channel's limit, calculate things just like a 
        # single-channel driver (first channel will be zero)
        if goal_lm <= v['lm2_max']:
            pwm1_float = 0.0
            #pwm2_float = ((goal_lm / v['lm2_max']) * (256-v['pwm2_min'])) + v['pwm2_min'] - 1
            pwm2_float = (((goal_lm-v['lm2_min']) / (v['lm2_max']-v['lm2_min'])) \
                             * (255-v['pwm2_min'])) \
                         + v['pwm2_min']
            pwm1 = int(round(pwm1_float))
            pwm2 = int(round(pwm2_float))
            pwm2 = max(min(pwm2,v['pwm2_max']),v['pwm2_min'])
            modes.append((int(pwm1),int(pwm2)))
        # Above the second channel's limit, things get a little more 
        # complicated (second channel will be 255, first channel will be 
        # adjusted down by the max output of the second channel)
        else:
            if len(modes) == v['num_levels'] -1: # turbo is special
                #pwm1_float = ((goal_lm / v['lm_max']) * (256-v['pwm_min'])) + v['pwm_min'] - 1
                pwm1_float = float(v['pwm_max'])
                # on a FET+7135 driver, turbo works better without the 7135
                # (we're assuming FET+7135 here)
                pwm2_float = 0.0
            else: # not the highest mode yet
                #pwm1_float = (((goal_lm-v['lm2_max']) / v['lm_max']) * (256-v['pwm_min'])) + v['pwm_min'] - 1
                pwm1_float = (((goal_lm-v['lm_min']-v['lm2_max']) / (v['lm_max']-v['lm_min'])) \
                                 * (255-v['pwm_min'])) \
                             + v['pwm_min']
                pwm2_float = 255.0
            pwm1 = int(round(pwm1_float))
            pwm2 = int(round(pwm2_float))
            pwm1 = max(min(pwm1,v['pwm_max']),v['pwm_min'])
            modes.append((int(pwm1),int(pwm2)))
        print '%i: visually %.2f (%.2f lm): %.2f/255, %.2f/255' % (i+1, goal, goal_lm, pwm1_float, pwm2_float)
        goal += step_size

    print 'PWM1/FET  values:', ','.join([str(i[0]) for i in modes])
    print 'PWM2/7135 values:', ','.join([str(i[1]) for i in modes])
    print 'On a non-FET driver, the last mode should be 255 on both channels.'

def get_value(text, default, args):
    """Get input from the user, or from the command line args."""
    if args:
        result = args[0]
        del args[0]
    else:
        global interactive
        interactive = True
        print text, '(%s)' % (default),
        result = raw_input()
    result = result.strip()
    return result

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

