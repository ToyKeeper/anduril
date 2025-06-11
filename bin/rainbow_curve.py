#!/bin/env python3
import sys

def rainbow_curve(
        max_sec_per_cycle=180, min_sec_per_cycle=1/10,
        min_delay=4, max_delay=16, offset=2):
    """Generate suitable combinations of delays and increments for rainbow mode.

    Creates a list of delays and hue increments over a range of hue cycling
    speeds in rainbow strobe mode. Matches a curve to the desired minimum
    and maximum speeds, then for values along the curve from 0 to 126
    (inclusive), it it tries to produce small integer ratios that approximate
    the ideal number of hue increments per second to match that speed.

    Args:
        max_sec_per_cycle: The slowest speed in terms of the number of seconds
            to complete a cycle.
        min_sec_per_cycle: The fastest speed in terms of the number of seconds
            to complete a cycle (can be less than 1).
        min_delay: The minimum duration delay (in ms) to apply between value
            increments. Avoids updates that are much more frequent than
            necessary to produce a smooth animation.
        max_delay: The maximum duration delay (in ms) to apply between value
            increments. Higher values match the desired speed more accurately,
            but potentially reduce the smoothness of the animation. 16 (approx.
            60 hz) is suitable to retain smooth animation.
        offset: A low value produces a steep curve which allows for very fine
            adjustments at higher speeds, and a higher value produces an overall
            flatter curve, favoring fine adjustments at slower speeds.
    """
    max_sec_per_cycle = float(max_sec_per_cycle)
    min_sec_per_cycle = float(min_sec_per_cycle)
    min_delay = int(min_delay)
    max_delay = int(max_delay)
    offset = float(offset)
    # z = max_sec_per_cycle
    # n = min_sec_per_cycle
    # o = offset
    # f(x)     = a/x + b
    # f(o)     = a/o + b       = z
    # f(126+o) = a/(126+o) + b = n
    # algebra, simultaneous equations, etc.
    # a = (z-n) * o * (1 + o/126)
    # b = z - a/o
    a = (max_sec_per_cycle - min_sec_per_cycle) * offset * (1 + offset/126)
    b = max_sec_per_cycle - a / offset
    increments = []
    delays = []
    for val in range(127):
        sec_per_cycle = a / (val+offset) + b
        hues_per_sec = 256 / sec_per_cycle
        ideal_delay_ms = 1000 / hues_per_sec
        if ideal_delay_ms > max_delay:
            increment = 1
            delay = int(round(ideal_delay_ms))
        else:
            delay = 0
            increment = 0
            error = 100
            # Brute force different combinations of delay and increment per
            # delay to find the one that most closely approximates the desired
            # speed.
            for d in range(min_delay, max_delay+1):
                low_inc = int(d*hues_per_sec/1000)
                for inc in (low_inc, low_inc+1):
                    if inc == 0:
                        continue
                    effective_delay = d / inc
                    result_sec_per_cycle = effective_delay * 256 / 1000
                    e = abs(1 - result_sec_per_cycle / sec_per_cycle)
                    if e < error:
                        delay = d
                        increment = inc
                        error = e

        final_effective_delay = delay / increment
        final_sec_per_cycle = final_effective_delay * 256 / 1000
        error = abs(1 - final_sec_per_cycle / sec_per_cycle)
        prefix = '[91m' if error > 0.03 else ''
        print('%s%3d: h/s %7.2f s/c %6.2f: d/i %3d/%2d, effective delay: %6.2f, ideal delay: %6.2f, error: %5.2f%%[0m' % (
            prefix, val, hues_per_sec, sec_per_cycle, delay, increment, final_effective_delay, ideal_delay_ms, error*100))

        increments.append(str(increment))
        delays.append(str(delay))

    print()
    print(f'// {sys.argv[0]} {max_sec_per_cycle} {min_sec_per_cycle} {min_delay} {max_delay} {offset}')
    print('const uint16_t rainbow_delays[127] = {%s};' % ','.join(delays))
    print('const uint8_t rainbow_increments[127] = {%s};' % ','.join(increments))

if __name__ == '__main__':
    rainbow_curve(*sys.argv[1:])
