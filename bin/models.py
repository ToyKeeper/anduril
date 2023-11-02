#!/usr/bin/env python

import os
import re

def main(args):
    """models.py: scan build targets to generate the MODELS file
    """

    models = []

    # load all cfg-*.h files
    paths = os.listdir('.')
    for p in paths:
        if p.startswith('cfg-') and p.endswith('.h'):
            m = load_cfg(p)
            models.append(m)

    # sort by model number
    foo = [(m.num, m.name, m) for m in models]
    foo.sort()
    models = [x[-1] for x in foo]

    fmt = '%s\t%-30s\t%s'
    print(fmt % ('Model', 'Name', 'MCU'))
    print(fmt % ('-----', '----', '---'))
    for m in models:
        print(fmt % (m.num, m.name, m.attiny))

    print('\nDuplicates:')
    for i, m in enumerate(models):
        for m2 in models[i+1:]:
            #if (m.num == m2.num) and (m is not m2):
            if m.num == m2.num:
                print('%s\t%s, %s' % (m.num, m.name, m2.name))

    print('\nMissing:')
    for m in models:
        if not m.num:
            print(m.name)


class Empty:
    pass


def load_cfg(path):
    m = Empty()
    m.name, m.num, m.attiny = '', '', 'attiny85'

    m.name = path.replace('cfg-', '').replace('.h', '')

    num_pat = re.compile(r'#define\s+MODEL_NUMBER\s+"(\d+)"')
    mcu_pat = re.compile(r'ATTINY:\s+(\d+)')
    # TODO? use C preprocessor to generate more complete file to scan
    with open(path) as fp:
        for line in fp:
            found = num_pat.search(line)
            if found:
                m.num = found.group(1)
            found = mcu_pat.search(line)
            if found:
                m.attiny = 'attiny' + found.group(1)

    return m


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

