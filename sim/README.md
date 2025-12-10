# Anduril Simulator and Test Suite

Automated testing infrastructure for Anduril flashlight firmware using
cycle-accurate AVR emulation.

## Quick Start

```bash
# 1. Clone simavr with ATtiny1634 support (one-time setup)
cd /path/to/anduril
git clone https://github.com/GlassOnTin/simavr.git

# 2. Build simavr
cd simavr/simavr
make

# 3. Build firmware and run tests
cd ../../sim
make test
```

## Requirements

- **simavr** with ATtiny1634 core support
  - Fork: https://github.com/GlassOnTin/simavr
  - Upstream PR: https://github.com/buserror/simavr/pull/568
- **avr-gcc** toolchain (for building firmware)
- **libelf-dev** (simavr dependency)

### Ubuntu/Debian

```bash
sudo apt install gcc make libelf-dev avr-libc gcc-avr
```

## Directory Structure

```
anduril/
├── sim/                    # This directory
│   ├── Makefile
│   ├── tests/
│   │   ├── anduril-test.h  # Test framework API
│   │   ├── anduril-test.c  # Test framework implementation
│   │   ├── test_basic_ui.c # Basic UI tests (8 tests)
│   │   ├── test_ramping.c  # Ramping tests (6 tests)
│   │   └── test_channel_modes.c  # Channel mode tests (4 tests)
│   └── simavr-core/
│       └── sim_tiny1634.*  # ATtiny1634 core (also in simavr fork)
├── simavr/                 # Clone simavr here (sibling to sim/)
│   └── simavr/
│       └── obj-x86_64-linux-gnu/
│           └── libsimavr.so.1
└── hex/
    └── anduril.*.hex       # Compiled firmware
```

## Usage

### Run All Tests

```bash
make test
```

Output:
```
=========================================
Running Anduril Test Suite
=========================================

TEST: starts_off... PASS
TEST: click_on... PASS
...
=========================================
Anduril Basic UI Tests: 8 passed, 0 failed
=========================================
...
```

### Run Individual Test Suites

```bash
make test-basic     # Basic UI tests (on/off, hold, turbo, lockout)
make test-ramping   # Ramping behavior tests
make test-channel   # Channel mode switching tests
```

### Verbose Debug Output

```bash
make test-verbose
```

Shows button presses, interrupt timing, and PWM state changes.

## Test Framework API

### Test Structure

```c
#include "anduril-test.h"

void test_example(void) {
    TEST_BEGIN("example_test");

    // Setup: initialize emulator
    anduril_test_reset();

    // Action: simulate user input
    anduril_click();                    // Single click
    anduril_multi_click(3);             // Triple click
    anduril_button_set(1);              // Press and hold
    anduril_run_ticks(50);              // Wait 50 WDT ticks
    anduril_button_set(0);              // Release

    // Assert: check state
    ASSERT(anduril_is_light_on(), "Light should be on");

    pwm_state_t pwm = anduril_get_pwm();
    ASSERT(pwm.main2 > 0, "Main2 PWM should be active");

    TEST_PASS();
}
```

### Key Functions

| Function | Description |
|----------|-------------|
| `anduril_test_init(hex_file)` | Load firmware hex into emulator |
| `anduril_test_reset()` | Reset emulator state |
| `anduril_run_ticks(n)` | Run n WDT tick periods (~16ms each) |
| `anduril_click()` | Simulate single button click |
| `anduril_multi_click(n)` | Simulate n clicks in sequence |
| `anduril_button_set(state)` | Set button state (1=pressed, 0=released) |
| `anduril_get_pwm()` | Read current PWM values |
| `anduril_is_light_on()` | Check if any PWM output is active |

### Timing Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `WDT_CYCLES` | 128000 | CPU cycles per WDT tick (~16ms at 8MHz) |
| `CLICK_TICKS` | 5 | Button press duration (~80ms) |
| `CLICK_GAP` | 8 | Gap between multi-clicks (~128ms) |
| `HOLD_THRESHOLD` | 25 | Ticks before hold is registered (~400ms) |
| `LONG_TIMEOUT` | 150 | Ticks for long operations (~2.4s) |

## Writing New Tests

1. Create a new test file in `sim/tests/`
2. Include `anduril-test.h`
3. Write test functions using the API
4. Add to `Makefile` following existing patterns

Example test file structure:

```c
#include "anduril-test.h"

static int tests_passed = 0;
static int tests_failed = 0;

void test_my_feature(void) {
    TEST_BEGIN("my_feature");
    anduril_test_reset();

    // ... test code ...

    TEST_PASS();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <firmware.hex>\n", argv[0]);
        return 1;
    }

    if (!anduril_test_init(argv[1])) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }

    test_my_feature();
    // ... more tests ...

    printf("Tests: %d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
```

## Troubleshooting

### "cannot find -lsimavr"

simavr not built or not in expected location. Ensure:
```bash
ls ../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.so.1
```

### "undefined reference to sim_tiny1634"

The simavr fork doesn't have ATtiny1634 support. Use:
```bash
git clone https://github.com/GlassOnTin/simavr.git ../simavr
```

### Tests fail with "target does not answer"

This error is for physical hardware flashing, not simulation.
The emulator should work without connected hardware.
