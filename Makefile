# Tiny Makefile to simply "exec ./bin/make.sh $*"
# Copyright (C) 2023 Selene ToyKeeper
# SPDX-License-Identifier: GPL-3.0-or-later

# Note: Does not pass args in the form "-a", "--arg", or "VAR=value"
# because 'make' does not put those into $MAKECMDGOALS

# for 'make foo bar baz', don't run 3 times
# (generate no-op rules for args 2 and later)
ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
$(eval $(ARGS):;@:)

# handle the case with no args
all:
	@./bin/make.sh

# catch everything else and parse the command line ourselves
.phony: %
%:
	@./bin/make.sh $(MAKECMDGOALS)

