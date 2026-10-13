# Anduril Simulator Interface Protocol

Version 1.0

## Overview

The `sim-interface` program provides a text-based command/response protocol over stdin/stdout for controlling the Anduril firmware emulator. This allows external programs (GUIs, test scripts, etc.) to interact with the simulator.

## Communication Model

**Transport**: stdin (commands) / stdout (responses)

**Format**: Line-delimited text (newline-terminated)

**Encoding**: UTF-8 (ASCII-compatible)

**Error Channel**: stderr (used for startup banner and debug messages only)

## Command Format

```
COMMAND[:args]\n
```

- Commands are case-sensitive uppercase
- Optional arguments follow a colon `:`
- Commands must be newline-terminated
- Empty lines are ignored

## Response Format

```
STATUS[:data]\n
```

- Status is uppercase (OK, ERROR, PWM, etc.)
- Optional data follows a colon `:`
- Responses are newline-terminated
- Always flushed immediately (line-buffered)

---

## Commands

### INIT

**Syntax**: `INIT:hex_file_path`

**Description**: Initialize the AVR emulator with the specified firmware hex file.

**Arguments**:
- `hex_file_path`: Path to Intel HEX firmware file (relative or absolute)

**Response**:
- `OK` - Emulator initialized successfully
- `ERROR:Already initialized` - INIT called twice
- `ERROR:Missing hex file path` - No argument provided
- `ERROR:Failed to initialize emulator` - Hex file not found or invalid

**Example**:
```
> INIT:../hex/anduril.hank-emisar-d4k-3ch.hex
< OK
```

**Notes**:
- Must be called before any other command (except QUIT)
- Can only be called once per session
- Loads firmware into virtual ATtiny1634 flash memory
- Automatically performs initial reset

---

### RESET

**Syntax**: `RESET`

**Description**: Reset the AVR to power-on state (like removing and reinserting battery).

**Response**:
- `OK` - Reset successful
- `ERROR:Not initialized` - INIT not called yet

**Example**:
```
> RESET
< OK
```

**Notes**:
- Resets all registers, timers, interrupts
- Clears SRAM (but not EEPROM)
- Firmware re-runs initialization code
- Button is set to released state

---

### CLICK

**Syntax**: `CLICK`

**Description**: Simulate a single button click (press, hold ~80ms, release, wait ~128ms).

**Response**:
- `OK` - Click completed
- `ERROR:Not initialized` - INIT not called yet

**Example**:
```
> CLICK
< OK
```

**Notes**:
- Timing: 5 WDT ticks press + 8 WDT ticks gap ≈ 208ms total
- Triggers PCINT interrupt in firmware
- Equivalent to quick tap on real hardware
- Does NOT wait for firmware to process (use RUN for that)

---

### HOLD

**Syntax**: `HOLD:ticks`

**Description**: Press button, hold for specified WDT ticks, then release.

**Arguments**:
- `ticks`: Number of WDT tick periods to hold (1 tick ≈ 16ms)

**Response**:
- `OK` - Hold completed
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing tick count` - No argument
- `ERROR:Invalid tick count` - Tick count ≤ 0

**Example**:
```
> HOLD:50
< OK
```

**Notes**:
- `HOLD:25` ≈ 400ms (threshold for "hold" detection)
- `HOLD:100` ≈ 1.6 seconds
- Automatically releases button and waits gap period

---

### PRESS

**Syntax**: `PRESS`

**Description**: Press button and hold (don't release).

**Response**:
- `OK` - Button pressed
- `ERROR:Not initialized` - INIT not called yet

**Example**:
```
> PRESS
< OK
> RUN:50
< OK
> RELEASE
< OK
```

**Notes**:
- Use with RELEASE for custom timing
- Useful for testing exact hold durations
- Firmware sees button state on every WDT tick

---

### RELEASE

**Syntax**: `RELEASE`

**Description**: Release button (set to unpressed state).

**Response**:
- `OK` - Button released
- `ERROR:Not initialized` - INIT not called yet

**Example**:
```
> RELEASE
< OK
```

**Notes**:
- No effect if button already released
- Use after PRESS for manual timing control

---

### MULTI

**Syntax**: `MULTI:count`

**Description**: Perform multi-click sequence (2C, 3C, 4C, etc.).

**Arguments**:
- `count`: Number of clicks (1-20)

**Response**:
- `OK` - Multi-click completed
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing click count` - No argument
- `ERROR:Invalid click count (1-20)` - Out of range

**Example**:
```
> MULTI:2
< OK
```

**Notes**:
- Automatically performs click sequence with proper timing
- Waits LONG_TIMEOUT (150 ticks ≈ 2.4s) after sequence
- `MULTI:2` = double-click (turbo)
- `MULTI:3` = triple-click (channel switch)
- `MULTI:4` = quad-click (lockout)

---

### RUN

**Syntax**: `RUN:ticks`

**Description**: Advance emulator time by specified WDT tick periods.

**Arguments**:
- `ticks`: Number of WDT ticks to advance (1 tick ≈ 16ms)

**Response**:
- `OK` - Simulation advanced
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing tick count` - No argument
- `ERROR:Invalid tick count` - Tick count ≤ 0

**Example**:
```
> RUN:50
< OK
```

**Notes**:
- 1 tick = 128000 CPU cycles at 8MHz ≈ 16ms
- Use after button commands to let firmware process events
- `RUN:1` ≈ one WDT tick (one iteration of main loop)
- `RUN:150` ≈ 2.4 seconds (typical timeout period)

---

### GETPWM

**Syntax**: `GETPWM`

**Description**: Read current PWM values from timer registers.

**Response**:
- `PWM:main2,led3,led4` - Current PWM values
- `ERROR:Not initialized` - INIT not called yet

**Response Format**: `PWM:main2,led3,led4`
- `main2`: OCR0A scaled to ~15-bit (0-32640, 8-bit * 128)
- `led3`: OCR1A 16-bit (0-65535)
- `led4`: OCR1B 16-bit (0-65535)

**Example**:
```
> GETPWM
< PWM:2560,1200,1200
```

**Notes**:
- Returns instantaneous register values
- 0,0,0 = light off
- Higher values = brighter LEDs
- Hardware-specific mapping (currently D4K-3ch only)

---

### SETVOLTAGE

**Syntax**: `SETVOLTAGE:millivolts`

**Description**: Set battery voltage in millivolts (simulates battery state).

**Arguments**:
- `millivolts`: Battery voltage in mV (2000-4500)

**Response**:
- `OK` - Voltage set successfully
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing voltage in millivolts` - No argument
- `ERROR:Voltage out of range (2000-4500mV)` - Invalid voltage

**Example**:
```
> SETVOLTAGE:3700
< OK
> SETVOLTAGE:2900
< OK
```

**Notes**:
- Typical range: 4.2V (full) → 2.5V (dead)
- LVP triggers around 2.9V
- Voltage affects ADC readings in firmware
- Use for testing battery protection features

**Common Values**:
- 4200mV - Fully charged Li-ion
- 3700mV - Nominal voltage
- 3300mV - Low battery
- 2900mV - LVP threshold
- 2500mV - Dead battery

---

### SETTEMP

**Syntax**: `SETTEMP:celsius`

**Description**: Set temperature in Celsius (simulates thermal conditions).

**Arguments**:
- `celsius`: Temperature in °C (-40 to 85)

**Response**:
- `OK` - Temperature set successfully
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing temperature in Celsius` - No argument
- `ERROR:Temperature out of range (-40 to 85°C)` - Invalid temperature

**Example**:
```
> SETTEMP:25
< OK
> SETTEMP:60
< OK
```

**Notes**:
- Typical range: -40°C → 85°C (MCU operating range)
- Thermal regulation may activate at high temps
- Temperature affects ADC channel 14 (internal temp sensor)
- Use for testing thermal stepdown features

**Common Values**:
- 0°C - Cold conditions
- 25°C - Room temperature (default)
- 40°C - Warm/outdoor use
- 60°C - Hot (may trigger thermal regulation)
- 80°C - Very hot (should trigger thermal stepdown)

---

### GETTEMP

**Syntax**: `GETTEMP`

**Description**: Read current temperature setting.

**Response**:
- `TEMP:celsius` - Current temperature in Celsius
- `ERROR:Not initialized` - INIT not called yet

**Response Format**: `TEMP:celsius`
- `celsius`: Temperature in °C

**Example**:
```
> GETTEMP
< TEMP:25
> SETTEMP:60
< OK
> GETTEMP
< TEMP:60
```

**Notes**:
- Returns the temperature set via SETTEMP
- Default is 25°C after RESET

---

### GETEEPROM

**Syntax**: `GETEEPROM:offset,length`

**Description**: Read bytes from EEPROM.

**Arguments**:
- `offset`: Starting offset (0-255)
- `length`: Number of bytes to read (1-256)

**Response**:
- `EEPROM:hexdata` - Hex string of EEPROM data
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing offset,length` - Missing arguments
- `ERROR:Invalid format, expected offset,length` - Bad format
- `ERROR:Invalid offset or length` - Out of range

**Example**:
```
> GETEEPROM:0,16
< EEPROM:000000000000000060D83E0652720000
> GETEEPROM:100,4
< EEPROM:01234567
```

**Notes**:
- Data returned as uppercase hex string
- 2 hex chars per byte (e.g., "FF" = 255)
- Useful for reading config without dumping all 256 bytes

---

### DUMPEEPROM

**Syntax**: `DUMPEEPROM`

**Description**: Dump entire EEPROM (all 256 bytes).

**Response**:
- `EEPROM:hexdata` - 512 hex characters (256 bytes)
- `ERROR:Not initialized` - INIT not called yet

**Example**:
```
> DUMPEEPROM
< EEPROM:0000000000000000...  (512 hex chars)
```

**Notes**:
- Returns all 256 bytes as 512-character hex string
- Faster than calling GETEEPROM:0,256
- Use for config backup/restore

---

### SETEEPROM

**Syntax**: `SETEEPROM:offset,value`

**Description**: Write single byte to EEPROM.

**Arguments**:
- `offset`: EEPROM offset (0-255)
- `value`: Byte value to write (0-255)

**Response**:
- `OK` - Byte written successfully
- `ERROR:Not initialized` - INIT not called yet
- `ERROR:Missing offset,value` - Missing arguments
- `ERROR:Invalid format, expected offset,value` - Bad format
- `ERROR:Invalid offset (0-255)` - Offset out of range
- `ERROR:Invalid value (0-255)` - Value out of range

**Example**:
```
> SETEEPROM:10,42
< OK
> SETEEPROM:0,255
< OK
```

**Notes**:
- Writes directly to EEPROM memory
- Changes take effect immediately
- Use for testing different configs
- No wear-leveling in simulator (instant write)

**Warning**: Writing invalid config data may cause firmware crashes or unexpected behavior.

---

### QUIT

**Syntax**: `QUIT`

**Description**: Shutdown simulator and exit.

**Response**:
- `BYE` - Shutdown acknowledgment

**Example**:
```
> QUIT
< BYE
```

**Notes**:
- Gracefully shuts down emulator
- Can be called at any time (even before INIT)
- Process exits after sending BYE

---

## Example Sessions

### Basic Turn On/Off Test

```
> INIT:../hex/anduril.hank-emisar-d4k-3ch.hex
< OK
> RESET
< OK
> RUN:50
< OK
> GETPWM
< PWM:0,0,0
> CLICK
< OK
> RUN:150
< OK
> GETPWM
< PWM:2560,1200,1200
> CLICK
< OK
> RUN:150
< OK
> GETPWM
< PWM:0,0,0
> QUIT
< BYE
```

### Turbo Test (Double-Click)

```
> INIT:../hex/anduril.hank-emisar-d4k-3ch.hex
< OK
> RESET
< OK
> RUN:10
< OK
> MULTI:2
< OK
> GETPWM
< PWM:32640,20000,20000
> QUIT
< BYE
```

### Manual Hold Test

```
> INIT:../hex/anduril.hank-emisar-d4k-3ch.hex
< OK
> RESET
< OK
> RUN:50
< OK
> PRESS
< OK
> RUN:5
< OK
> GETPWM
< PWM:0,0,0
> RUN:20
< OK
> GETPWM
< PWM:128,50,50
> RUN:25
< OK
> GETPWM
< PWM:2560,800,800
> RELEASE
< OK
> QUIT
< BYE
```

---

## Error Handling

All errors follow this format:

```
ERROR:descriptive message
```

**Common Errors**:
- `ERROR:Not initialized` - Call INIT first
- `ERROR:Already initialized` - INIT called multiple times
- `ERROR:Unknown command: FOO` - Invalid command name
- `ERROR:Missing <arg>` - Required argument missing
- `ERROR:Invalid <arg>` - Argument out of range or malformed

**Error Recovery**:
- Errors are non-fatal (except initialization failures)
- Emulator state unchanged on error
- Continue sending commands after error response

---

## Timing Reference

| Constant | Ticks | Milliseconds | Purpose |
|----------|-------|--------------|---------|
| WDT tick | 1 | ~16ms | Main loop period |
| CLICK_TICKS | 5 | ~80ms | Click press duration |
| CLICK_GAP | 8 | ~128ms | Gap between clicks |
| HOLD_THRESHOLD | 25 | ~400ms | Hold detection threshold |
| LONG_TIMEOUT | 150 | ~2400ms | Click sequence timeout |

**Typical Usage**:
- After RESET: `RUN:50` (let firmware initialize)
- After CLICK: `RUN:150` (let state machine process)
- After MULTI: Built-in wait, just read PWM
- For smooth ramping: `RUN:1` in loop (advance tick-by-tick)

---

## Hardware Mapping (D4K-3ch)

Current implementation supports `hank-emisar-d4k-3ch` only:

| Channel | Register | Bits | Range | LED |
|---------|----------|------|-------|-----|
| main2 | OCR0A | 8 | 0-32640 | Main emitters (cool white) |
| led3 | OCR1A | 16 | 0-65535 | Channel 3 (warm white) |
| led4 | OCR1B | 16 | 0-65535 | Channel 4 (warm white) |

**PWM Interpretation**:
- 0 = LED off
- Low values (< 500) = Moon mode
- Medium (500-10000) = Low-mid brightness
- High (10000-30000) = High brightness
- Max (> 30000) = Turbo

**Future**: Hardware configs will be selectable via LOADHW command.

---

## Implementation Notes

### For UI Developers

**Recommended Workflow**:
1. Launch `sim-interface` as subprocess
2. Send `INIT:path/to/firmware.hex`
3. Send `RESET` and `RUN:50` to initialize
4. Use `PRESS`/`RELEASE` + `RUN:1` for smooth button hold simulation
5. Read `GETPWM` at 60Hz for live display updates
6. Send `QUIT` on UI exit

**Python Example**:
```python
import subprocess

proc = subprocess.Popen(['./sim-interface'],
                       stdin=subprocess.PIPE,
                       stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE,
                       text=True,
                       bufsize=1)  # Line buffered

def send(cmd):
    proc.stdin.write(cmd + '\n')
    proc.stdin.flush()
    return proc.stdout.readline().strip()

send('INIT:../hex/anduril.hank-emisar-d4k-3ch.hex')
send('RESET')
send('RUN:50')

response = send('GETPWM')  # Returns "PWM:0,0,0"
main2, led3, led4 = map(int, response.split(':')[1].split(','))
```

### For Anduril Developers

**Adding New Commands**:
1. Add handler function in `sim-interface.c`
2. Add case in `process_command()`
3. Update this documentation
4. Update UI repositories

**Future Enhancements**:
- `GETSTATE` - Read current FSM state name
- `GETADC` - Read raw ADC values
- `LOADHW` - Load different hardware configuration
- `GETVOLTAGE` - Read current voltage setting

---

## Version History

**v1.1** (2025-12-26)
- Added SETTEMP/GETTEMP commands for temperature simulation
- Added SETVOLTAGE command for battery voltage simulation
- Added GETEEPROM/DUMPEEPROM/SETEEPROM commands for EEPROM access
- Fixed ADC emulation using simavr IRQ system
- D4K-3ch hardware support

**v1.0** (2025-12-25)
- Initial release
- Basic commands: INIT, RESET, CLICK, HOLD, PRESS, RELEASE, MULTI, RUN, GETPWM, QUIT
- D4K-3ch hardware support only

---

**End of protocol documentation**
