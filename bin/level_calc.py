#!/usr/bin/env python

from __future__ import print_function

import math

interactive = False
# supported shapes: ninth, seventh, fifth, cube, square, log
#ramp_shape = 'cube'

max_pwm = 255
max_pwms = []
dyn_pwm = False


def main(args):
    """Calculates PWM levels for visually-linear steps.
    """
    cli_answers = []
    global max_pwm, max_pwms, dyn_pwm
    pwm_arg = str(max_pwm)
    clock_arg = '8:16:1'  # quarter/half speed levels and rise time

    i = 0
    while i < len(args):
        a = args[i]
        if a in ('--pwm',):
            i += 1
            pwm_arg = args[i]
        elif a in ('--clock',):
            i += 1
            clock_arg = args[i]
        else:
            #print('unrecognized option: "%s"' % (a,))
            cli_answers.append(a)

        i += 1

    # Get parameters from the user
    questions_main = [
            (str, 'ramp_shape', 'cube', 'Ramp shape? [cube, square, fifth, seventh, ninth, log, N.NN]'),
            (int, 'num_channels', 1, 'How many power channels?'),
            (int, 'num_levels', 4, 'How many total levels do you want?'),
            ]
    questions_per_channel = [
            (str, 'type', '7135', 'Type of channel - 7135 or FET:'),
            (int, 'pwm_min', 6, 'Lowest visible PWM level:'),
            (float, 'lm_min', 0.25, 'How bright is the lowest level, in lumens?'),
            #(int, 'pwm_max', max_pwm, 'Highest PWM level:'),
            (float, 'lm_max', 1000, 'How bright is the highest level, in lumens?'),
            ]

    def ask(questions, ans):
        for typ, name, default, text in questions:
            value = get_value(text, default, cli_answers)
            if not value:
                value = default
            else:
                value = typ(value)
            setattr(ans, name, value)

    answers = Empty()
    ask(questions_main, answers)

    if pwm_arg:
        if pwm_arg.startswith('dyn:'):
            dyn_pwm = True
            parts = pwm_arg.split(':')
            dpwm_steps = int(parts[1])
            dpwn_max = int(parts[2])
            dpwn_min = int(parts[3])
            dpwm_shape = 'linear'
            if len(parts) > 4:
                dpwm_shape = float(parts[4])
            max_pwms = [dpwn_min] * answers.num_levels
            for i in range(dpwm_steps):
                span = dpwn_max - dpwn_min
                if dpwm_shape == 'linear':
                    x = dpwn_min + (span * (float(dpwm_steps - i) / dpwm_steps))
                else:  # variable curve
                    x = dpwn_min + (span * ((float(dpwm_steps - i) / dpwm_steps) ** dpwm_shape))
                max_pwms[i] = int(x)
            max_pwm = dpwn_min

        else:
            val = int(pwm_arg)
            max_pwm = val
            max_pwms = [val] * answers.num_levels

    if clock_arg:
        parts = clock_arg.split(':')
        answers.quarterspeed_level, answers.halfspeed_level = [int(x) for x in parts[:2]]
        answers.rise_time_base = float(parts[2])
    else:
        answers.quarterspeed_level, answers.halfspeed_level, answers.rise_time_base = 0,0,0

    global ramp_shape
    ramp_shape = answers.ramp_shape

    channels = []
    if not answers:
        print('Describe the channels in order of lowest to highest power.')
    for chan_num in range(answers.num_channels):
        if not answers:
            print('===== Channel %s =====' % (chan_num+1))
        chan = Empty()
        chan.pwm_max = max_pwm
        ask(questions_per_channel, chan)
        chan.type = chan.type.upper()
        if chan.type not in ('7135', 'FET'):
            raise ValueError('Invalid channel type: %s' % (chan.type,))
        channels.append(chan)

    # calculate total output of all previous channels
    for i, channel in enumerate(channels):
        channel.prev_lm = 0.0
        for j in range(i):
            if channels[j].type == '7135':
                channel.prev_lm += channels[j].lm_max

    # figure out the desired PWM values
    multi_pwm(answers, channels)

    if interactive: # Wait on exit, in case user invoked us by clicking an icon
        print('Press Enter to exit:')
        input_text()


class Empty:
    pass


def multi_pwm(answers, channels):
    lm_min = channels[0].lm_min
    # figure out the highest mode
    lm_max = max([(c.lm_max+c.prev_lm) for c in channels])
    if channels[-1].type == 'FET':
        if channels[-1].lm_max > channels[-1].prev_lm:
            # assume the highest output is with only the FET enabled
            lm_max = channels[-1].lm_max
        else:
            # this would be a stupid driver design
            raise ValueError("FET channel isn't the most powerful?")

    visual_min = invpower(lm_min)
    visual_max = invpower(lm_max)
    step_size = (visual_max - visual_min) / (answers.num_levels-1)

    # Determine ideal lumen levels
    goals = []
    goal_vis = visual_min
    for i in range(answers.num_levels):
        goal_lm = power(goal_vis)
        goals.append((goal_vis, goal_lm))
        goal_vis += step_size

    # Calculate each channel's output for each level
    for cnum, channel in enumerate(channels):
        channel.modes = []
        for i in range(answers.num_levels):
            goal_vis, goal_lm = goals[i]
            rise_time = calc_rise_time(i, answers)
            # This channel already is maxed out
            if goal_lm >= (channel.lm_max + channel.prev_lm):
                # This shouldn't happen, the FET is assumed to be the highest channel
                if channel.type == 'FET':
                    # this would be a stupid driver design
                    raise ValueError("FET channel isn't the most powerful?")

                # Handle FET turbo specially
                if (i == (answers.num_levels - 1)) \
                        and (cnum < (len(channels)-1)) \
                        and (channels[-1].type == 'FET'):
                    channel.modes.append(0.0)
                # Normal non-turbo mode or non-FET turbo
                else:
                    channel.modes.append(max_pwms[i])
            # This channel's active ramp-up range
            #elif goal_lm > (channel.prev_lm + channel.lm_min):
            elif goal_lm > channel.prev_lm:
                # assume 7135 channels all add together
                if channel.type == '7135':
                    lm_avail = channel.lm_max - channel.lm_min
                # assume FET channel gets higher output on its own
                elif channel.type == 'FET':
                    lm_avail = channel.lm_max - channel.prev_lm - channel.lm_min

                lm_needed = goal_lm - channel.prev_lm - channel.lm_min

                pwm_top = max_pwms[i]
                pwm_avail = pwm_top - channel.pwm_min - rise_time
                pwm_needed = pwm_avail * lm_needed / lm_avail
                #pwm_needed = min(pwm_needed, pwm_avail)
                if dyn_pwm and (pwm_top > max_pwm):
                    this_step = max(1, math.floor(pwm_needed))
                    next_step = this_step + 1
                    fpart = pwm_needed - math.floor(pwm_needed)
                    correction = (next_step - fpart) / next_step
                    pwm_top = int(pwm_avail * correction) + channel.pwm_min
                    pwm_avail = pwm_top - channel.pwm_min - rise_time
                    pwm_needed = pwm_avail * lm_needed / lm_avail
                    max_pwms[i] = pwm_top
                # save the result
                pwm = max(0, pwm_needed + channel.pwm_min + rise_time)
                channel.modes.append(pwm)
                # how close did we get?
                #ptop = int(round(pwm - channel.pwm_min))
                #pbot = pwm_top - channel.pwm_min
                #print('%.3f%% needed, %.3f%% actual' % (
                #    100.0 * lm_needed / lm_avail,
                #    100.0 * ptop / pbot,
                #    ))
            # This channel isn't active yet, output too low
            else:
                channel.modes.append(0)

    # Show individual levels in detail
    prev_ratios = [0.0] * len(channels)
    for i in range(answers.num_levels):
        goal_vis, goal_lm = goals[i]
        pwms = []
        rise_time = calc_rise_time(i, answers)
        for c, channel in enumerate(channels):
            #top = channel.modes[i] - channel.pwm_min - rise_time
            top = channel.modes[i] - channel.pwm_min
            if top < 0: top = 0
            #bot = max_pwms[i] - channel.pwm_min - rise_time
            bot = max_pwms[i]
            #ratio = 100 * (int(round(top)) / float(bot))
            topf, bot = channel.modes[i], max_pwms[i]
            top = int(round(topf))
            ratio = 100 * top / float(bot)
            pwms.append('(%.2f) %i/%i (%.3f%%)' % (topf, top, bot, ratio))
            if (ratio < prev_ratios[c]) and (ratio > 0):
                pwms.append('WARN')
            prev_ratios[c] = ratio
        print('%i: visually %.2f (%.2f lm): %s' % 
              (i+1, goal_vis, goal_lm, ', '.join(pwms)))

    # Show values we can paste into source code
    for cnum, channel in enumerate(channels):
        print('PWM%s values: %s' % 
                (cnum+1,
                 ','.join([str(int(round(i))) for i in channel.modes])))

    # Show PFM values (PWM TOP)
    if dyn_pwm:
        print('PWM_TOP: %s' % (','.join(str(x) for x in max_pwms)))

    # Show highest level for each channel before next channel starts
    for cnum, channel in enumerate(channels[:-1]):
        prev = 0
        i = 1
        while (i < answers.num_levels) \
                and (channels[cnum+1].modes[i] == 0):
                #and (channel.modes[i] >= channel.modes[i-1]) \
            i += 1
        print('Ch%i max: %i (%.2f/%s)' % (cnum, i, channel.modes[i-1], max_pwms[i]))


def calc_rise_time(i, answers):
    base = answers.rise_time_base

    if (i+1) < answers.quarterspeed_level:
        rise_time = base / 4.0
    elif (i+1) < answers.halfspeed_level:
        rise_time = base / 2.0
    else:
        ratio = 1.0 - math.sqrt((i - base) / (answers.num_levels - base))
        rise_time = answers.rise_time_base * ratio
    return rise_time


def get_value(text, default, args):
    """Get input from the user, or from the command line args."""
    if args:
        result = args[0]
        del args[0]
    else:
        global interactive
        interactive = True
        print(text + ' (%s) ' % (default), end='')
        result = input_text()
    result = result.strip()
    return result


shapes = dict(
        ninth  = (lambda x: x**9,      lambda x: math.pow(x, 1/9.0)),
        seventh= (lambda x: x**7,      lambda x: math.pow(x, 1/7.0)),
        fifth  = (lambda x: x**5,      lambda x: math.pow(x, 1/5.0)),
        cube   = (lambda x: x**3,      lambda x: math.pow(x, 1/3.0)),
        square = (lambda x: x**2,      lambda x: math.pow(x, 1/2.0)),
        log    = (lambda x: math.e**x, lambda x: math.log(x, math.e)),
        # makes no difference; all logs have the same curve
        #log_2  = (lambda x: 2.0**x,    lambda x: math.log(x, 2.0)),
        )

def power(x):
    try:
        factor = float(ramp_shape)
        return math.pow(x, factor)
    except ValueError:
        return shapes[ramp_shape][0](x)


def invpower(x):
    try:
        factor = float(ramp_shape)
        return math.pow(x, 1.0 / factor)
    except ValueError:
        return shapes[ramp_shape][1](x)


def input_text():
    try:
        value = raw_input()  # python2
    except NameError:
        value = input()  # python3
    return value


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

