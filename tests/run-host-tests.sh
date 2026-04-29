#!/usr/bin/env bash
# Run host-side regression tests which do not need avr-gcc.
# Copyright (C) 2026 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

set -euo pipefail

cc -Wall -Wextra -std=c99 -I ui -o /tmp/anduril-lockout-momentary-level-test \
  tests/lockout-momentary-level.c

/tmp/anduril-lockout-momentary-level-test
