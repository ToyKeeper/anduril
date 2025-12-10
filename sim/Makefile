# Makefile for D4K-3ch Anduril Simulator
# Tests REAL channel mode calculations from firmware
#
# Two modes:
#   1. Test harness (default) - runs real firmware C calculations
#   2. AVR emulator - uses local simavr with ATtiny1634 core

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 -g
LDFLAGS = -lm

# Test harness - uses REAL firmware channel mode code
TEST_SRCS = sim-test-harness.c sim-render.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_TARGET = sim-d4k-3ch

# Local simavr build (with ATtiny1634 support)
SIMAVR_DIR = ../simavr
SIMAVR_OBJ = $(SIMAVR_DIR)/simavr/obj-x86_64-linux-gnu
SIMAVR_INC = -I$(SIMAVR_DIR)/simavr/sim -I$(SIMAVR_DIR)/simavr/cores

# AVR emulator using local simavr
AVR_SRCS = sim-avr.c sim-render.c
AVR_OBJS = $(AVR_SRCS:.c=.o)
AVR_TARGET = sim-d4k-3ch-avr
AVR_CFLAGS = $(CFLAGS) $(SIMAVR_INC)
# Use absolute rpath and link directly to .so.1 to avoid system libsimavr
SIMAVR_RPATH = $(shell cd $(SIMAVR_OBJ) && pwd)
AVR_LDFLAGS = -L$(SIMAVR_OBJ) -Wl,-rpath,$(SIMAVR_RPATH) $(SIMAVR_OBJ)/libsimavr.so.1 -lpthread -lm -lelf

# Firmware hex file
HEX_FILE = ../hex/anduril.hank-emisar-d4k-3ch.hex

# Automated test binaries
TEST_DIR = tests
TEST_BASIC_UI = test_basic_ui
TEST_RAMPING = test_ramping
TEST_CHANNEL_MODES = test_channel_modes
ALL_TESTS = $(TEST_BASIC_UI) $(TEST_RAMPING) $(TEST_CHANNEL_MODES)

# Default target - build the test harness
all: $(TEST_TARGET)

# Build test harness (uses real firmware channel mode calculations)
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(LDFLAGS)

# Build AVR emulator using local simavr with ATtiny1634
avr: $(AVR_TARGET)

$(AVR_TARGET): sim-avr-local.o sim-render-avr.o
	$(CC) $(AVR_CFLAGS) -o $@ $^ $(AVR_LDFLAGS)

# Compile rules for test harness
sim-test-harness.o: sim-test-harness.c sim-render.h
	$(CC) $(CFLAGS) -c $< -o $@

sim-render.o: sim-render.c sim-render.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rules for AVR emulator (with simavr includes)
sim-avr-local.o: sim-avr.c sim-render.h
	$(CC) $(AVR_CFLAGS) -c $< -o $@

sim-render-avr.o: sim-render.c sim-render.h
	$(CC) $(AVR_CFLAGS) -c $< -o $@

# Run targets
run: $(TEST_TARGET)
	./$(TEST_TARGET)

run-avr: $(AVR_TARGET) $(HEX_FILE)
	./$(AVR_TARGET) $(HEX_FILE)

# Build firmware first if needed
$(HEX_FILE):
	cd .. && ./make emisar-d4k-3ch

# =============================================================================
# Automated Tests (using simavr)
# =============================================================================

# Build and run all tests
test: $(ALL_TESTS) $(HEX_FILE)
	@echo "========================================="
	@echo "Running Anduril Test Suite"
	@echo "========================================="
	@failed=0; \
	for t in $(ALL_TESTS); do \
		echo ""; \
		./$$t $(HEX_FILE) || failed=$$((failed + 1)); \
	done; \
	echo ""; \
	echo "========================================="; \
	if [ $$failed -gt 0 ]; then \
		echo "$$failed test file(s) had failures"; \
		exit 1; \
	fi

# Run individual test suites
test-basic: $(TEST_BASIC_UI) $(HEX_FILE)
	./$(TEST_BASIC_UI) $(HEX_FILE)

test-ramping: $(TEST_RAMPING) $(HEX_FILE)
	./$(TEST_RAMPING) $(HEX_FILE)

test-channel: $(TEST_CHANNEL_MODES) $(HEX_FILE)
	./$(TEST_CHANNEL_MODES) $(HEX_FILE)

# Build test binaries
$(TEST_BASIC_UI): $(TEST_DIR)/test_basic_ui.c $(TEST_DIR)/anduril-test.c $(TEST_DIR)/anduril-test.h
	$(CC) $(AVR_CFLAGS) -o $@ $(TEST_DIR)/test_basic_ui.c $(TEST_DIR)/anduril-test.c $(AVR_LDFLAGS)

$(TEST_RAMPING): $(TEST_DIR)/test_ramping.c $(TEST_DIR)/anduril-test.c $(TEST_DIR)/anduril-test.h
	$(CC) $(AVR_CFLAGS) -o $@ $(TEST_DIR)/test_ramping.c $(TEST_DIR)/anduril-test.c $(AVR_LDFLAGS)

$(TEST_CHANNEL_MODES): $(TEST_DIR)/test_channel_modes.c $(TEST_DIR)/anduril-test.c $(TEST_DIR)/anduril-test.h
	$(CC) $(AVR_CFLAGS) -o $@ $(TEST_DIR)/test_channel_modes.c $(TEST_DIR)/anduril-test.c $(AVR_LDFLAGS)

# Build and run tests with verbose debug output
test-verbose: clean
	$(MAKE) AVR_CFLAGS="$(AVR_CFLAGS) -DTEST_VERBOSE=1" test

# Clean build artifacts
clean:
	rm -f *.o $(TEST_TARGET) $(AVR_TARGET) $(ALL_TESTS)

# Build local simavr first if needed
$(SIMAVR_OBJ)/libsimavr.a:
	cd $(SIMAVR_DIR)/simavr && make

# Debug build
debug: CFLAGS += -g -DDEBUG -O0
debug: clean $(TEST_TARGET)

# Help
help:
	@echo "Anduril Firmware Test Suite"
	@echo ""
	@echo "Setup:"
	@echo "  git clone https://github.com/GlassOnTin/simavr.git ../simavr"
	@echo "  cd ../simavr/simavr && make"
	@echo ""
	@echo "Test Targets:"
	@echo "  test           Run all automated tests (18 tests)"
	@echo "  test-basic     Run basic UI tests (8 tests)"
	@echo "  test-ramping   Run ramping tests (6 tests)"
	@echo "  test-channel   Run channel mode tests (4 tests)"
	@echo "  test-verbose   Run tests with debug output"
	@echo ""
	@echo "Other Targets:"
	@echo "  avr            Build AVR emulator binary"
	@echo "  clean          Remove build artifacts"
	@echo ""
	@echo "See README.md for full documentation."

.PHONY: all avr run run-avr clean debug help test test-basic test-ramping test-channel test-verbose
