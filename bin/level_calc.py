#!/usr/bin/env python

import math

interactive = False


def main(args):
    """Calculates PWM levels for visually-linear steps.
    """
    # Get parameters from the user
    questions_main = [
            (int, 'num_channels', 1, 'How many power channels?'),
            (int, 'num_levels', 4, 'How many total levels do you want?'),
            ]
    questions_per_channel = [
            (str, 'type', '7135', 'Type of channel - 7135 or FET:'),
            (int, 'pwm_min', 6, 'Lowest visible PWM level:'),
            (float, 'lm_min', 0.25, 'How bright is the lowest level, in lumens?'),
            #(int, 'pwm_max', 255, 'Highest PWM level:'),
            (float, 'lm_max', 1000, 'How bright is the highest level, in lumens?'),
            ]

    def ask(questions, ans):
        for typ, name, default, text in questions:
            value = get_value(text, default, args)
            if not value:
                value = default
            else:
                value = typ(value)
            setattr(ans, name, value)

    answers = Empty()
    ask(questions_main, answers)
    channels = []
    if not args:
        print('Describe the channels in order of lowest to highest power.')
    for chan_num in range(answers.num_channels):
        if not args:
            print('===== Channel %s =====' % (chan_num+1))
        chan = Empty()
        chan.pwm_max = 255
        ask(questions_per_channel, chan)
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
        print 'Press Enter to exit:'
        raw_input()


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
            raise ValueError, "FET channel isn't the most powerful?"

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
            # This channel already is maxed out
            if goal_lm >= (channel.lm_max + channel.prev_lm):
                # This shouldn't happen, the FET is assumed to be the highest channel
                if channel.type == 'FET':
                    # this would be a stupid driver design
                    raise ValueError, "FET channel isn't the most powerful?"

                # Handle FET turbo specially
                if (i == (answers.num_levels - 1)) \
                        and (cnum < (len(channels)-1)) \
                        and (channels[-1].type == 'FET'):
                    channel.modes.append(0.0)
                # Normal non-turbo mode or non-FET turbo
                else:
                    channel.modes.append(channel.pwm_max)
            # This channel's active ramp-up range
            #elif goal_lm > (channel.prev_lm + channel.lm_min):
            elif goal_lm > channel.prev_lm:
                # assume 7135 channels all add together
                if channel.type == '7135':
                    diff = channel.lm_max - channel.lm_min
                # assume FET channel gets higher output on its own
                elif channel.type == 'FET':
                    diff = channel.lm_max - channel.prev_lm - channel.lm_min

                needed = goal_lm - channel.prev_lm - channel.lm_min

                ratio = needed / diff * (channel.pwm_max-channel.pwm_min)
                pwm = max(0, ratio + channel.pwm_min)
                channel.modes.append(pwm)
            # This channel isn't active yet, output too low
            else:
                channel.modes.append(0)

    # Show individual levels in detail
    for i in range(answers.num_levels):
        goal_vis, goal_lm = goals[i]
        pwms = []
        for channel in channels:
            pwms.append('%.2f/%i' % (channel.modes[i], channel.pwm_max))
        print('%i: visually %.2f (%.2f lm): %s' % 
              (i+1, goal_vis, goal_lm, ', '.join(pwms)))

    # Show values we can paste into source code
    for cnum, channel in enumerate(channels):
        print('PWM%s values: %s' % 
                (cnum+1,
                 ','.join([str(int(round(i))) for i in channel.modes])))


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


def power(x):
    #return x**5
    return x**3
    #return x**2
    #return math.e**x
    #return 2.0**x


def invpower(x):
    #return math.pow(x, 1/5.0)
    return math.pow(x, 1/3.0)
    #return math.pow(x, 1/2.0)
    #return math.log(x, math.e)
    #return math.log(x, 2.0)


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

