# Tiny Makefile to simply "exec ./make $*"
# Copyright (C) 2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Note: Does not pass args in the form "-a", "--arg", or "VAR=value"
# because 'make' does not put those into $MAKECMDGOALS
# but this is still helpful in case the user forgets the './' before 'make'

# for 'make foo bar baz', don't run 3 times
# (generate no-op rules for args 2+)
ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
$(eval $(ARGS):;@:)

# handle the case with no args
all:
	@./make

# catch everything else and parse the command line ourselves
.PHONY: %
%:
	@./make $(MAKECMDGOALS)

# handle targets with the same name as a dir,
# because 'make' skips those otherwise
.PHONY: docs
docs:
	@./make docs

