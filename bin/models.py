#!/usr/bin/env python

import os
import re

def main(args):
    """models.py: scan build targets to generate the MODELS file
    """

    models = []

    # load all cfg-*.h files
    for p, dirs, files in os.walk('hw'):
        for d in dirs:
            if '/' not in p:  # skip top-level brand dirs
                continue
            #print('... %s' % d)
            m = load_model(os.path.join(p, d))
            if m.mcu and m.model:
                models.append(m)

    # sort by model number
    foo = [(m.model, m.name, m) for m in models]
    foo.sort()
    models = [x[-1] for x in foo]

    print('Models: %i\n' % len(models))
    fmt = '%s\t%-30s\t%s'
    print(fmt % ('Model', 'Name', 'MCU'))
    print(fmt % ('-----', '----', '---'))
    for m in models:
        print(fmt % (m.model, m.name, m.mcu))

    print('\nDuplicates:')
    for i, m in enumerate(models):
        for m2 in models[i+1:]:
            #if (m.model == m2.model) and (m is not m2):
            if m.model == m2.model:
                print('%s\t%s, %s' % (m.model, m.name, m2.name))

    print('\nMissing:')
    for m in models:
        if not m.model:
            print(m.name)


class Empty:
    pass


def load_model(path):
    #print('load_model(%s)' % path)
    m = Empty()
    m.name, m.model, m.mcu = '', '', '???'

    m.name = path.replace('hw/','').replace('/', '-')
    m.mcu = inherit(path, 'arch')
    m.model = inherit(path, 'model')

    return m


def inherit(path, field):
    #print('inherit(%s, %s)' % (path, field))
    check = os.path.join(path, field)
    if os.path.exists(check):
        line = open(check).readline().strip()
        return line
    else:
        parent = os.path.join(*os.path.split(path)[:-1])
        if parent:
            return inherit(parent, field)
    return None


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

