#!/usr/bin/env python

"""steps.py: Calculate the stepped ramp levels used by Anduril.
Usage: steps.py floor ceiling num_steps
For example:
    > ./steps.py 1 150 3
    1: 1
    2: 75
    3: 150
"""

def main(args):
    floor, ceil, steps = [int(x) for x in args[:3]]
    for i in range(steps):
        guess = floor + (i * (float(ceil-floor)/(steps-1)))
        this = nearest_level(guess, floor, ceil, steps)
        #print('%i: %i (guess: %i)' % (i+1, this, guess))
        print('%i: %i' % (i+1, this))


def nearest_level(target, floor, ceil, steps):
    """Copied/adapted from anduril.c"""
    # bounds check
    # using int16_t here saves us a bunch of logic elsewhere,
    # by allowing us to correct for numbers < 0 or > 255 in one central place
    mode_min = floor;
    mode_max = ceil;

    if (target < mode_min): return mode_min;
    if (target > mode_max): return mode_max;
    # the rest isn't relevant for smooth ramping
    #if (! ramp_style): return target;

    ramp_range = ceil - floor;
    ramp_discrete_step_size = int(ramp_range / (steps-1));
    this_level = floor;

    for i in range(steps):
        this_level = floor + (i * int(ramp_range) / (steps-1));
        diff = int(target - this_level);
        if (diff < 0): diff = -diff;
        if (diff <= (ramp_discrete_step_size>>1)):
            return this_level;

    return this_level;


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

