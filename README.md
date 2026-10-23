# arduino-rtos

FreeRTOS on the Arduino Uno (ATmega328P): a small demo that shows how an RTOS
works on a bare 8-bit MCU with 2 KB of RAM, plus diagrams of the full path from
reset to task switching.

This repository is a self-contained Arduino **sketchbook**: the FreeRTOS library
and the AVR core are git submodules, so a clone builds with no other library setup.

## Purpose

`rtos_demo` demonstrates, on real hardware:

- **ISR to task handoff**: button interrupt (INT0) gives a binary semaphore to a task.
- **Priority preemption**: `btn` (3) > `sns` (2) > `log` (1) > idle (0).
- **Mutex with priority inheritance**: one LED shared by two tasks.
- **Producer/consumer queue**: two producers, one logger task printing on UART.
- **Boot tracing**: `DBG()` prints `file:line function` on USART0 from the reset code up to the scheduler start.

| Task | Priority | Job |
|------|----------|-----|
| `btn` | 3 | Waits on `button_sem`, queues `BUTTON: 1`, triple LED pulse |
| `sns` | 2 | Reads A0 every 500 ms, queues `SENSOR: <adc>`, single LED pulse |
| `log` | 1 | Drains the queue, prints on Serial (9600 baud) |

The FreeRTOS tick is the AVR watchdog timer (about 15 ms).

## Layout

```
arduino-rtos/
├── rtos_demo/                 the sketch: .ino + button, sensor, shared_led drivers
├── libraries/FreeRTOS/        submodule: albertminasyan/Arduino_FreeRTOS_Library
├── hardware/arduino/avr/      submodule: albertminasyan/ArduinoCore-avr (core 1.8.8)
│   └── cores/arduino/dbg.h    DBG() trace macro (polled USART0, 9600 baud)
├── env.sh                     points arduino-cli at this directory as the sketchbook
└── docs/                      flow diagrams (HTML)
```

| Submodule | Fork of |
|-----------|---------|
| [Arduino_FreeRTOS_Library](https://github.com/albertminasyan/Arduino_FreeRTOS_Library) | [feilipu/Arduino_FreeRTOS_Library](https://github.com/feilipu/Arduino_FreeRTOS_Library) |
| [ArduinoCore-avr](https://github.com/albertminasyan/ArduinoCore-avr) | [arduino/ArduinoCore-avr](https://github.com/arduino/ArduinoCore-avr) |

## Hardware

- Arduino Uno or compatible (ATmega328P, 16 MHz). CH340 clones appear as `/dev/ttyUSB0`, boards with a 16U2 as `/dev/ttyACM0`.
- 1 push button
- 1 LED + 330 ohm resistor
- 1 potentiometer, 10 kohm (or any analog sensor)
- Breadboard, jumper wires, USB cable

## Wiring

| Part | Uno pin | Notes |
|------|---------|-------|
| Button, leg 1 | D2 (INT0) | `INPUT_PULLUP`, falling edge triggers the interrupt |
| Button, leg 2 | GND | Pressed = LOW |
| LED anode (+) | D8 | Through the 330 ohm resistor |
| LED cathode (-) | GND | |
| Potentiometer, ends | 5V and GND | |
| Potentiometer, wiper | A0 | Read with `analogRead()` |

```
D2 ----[ button ]---- GND
D8 ----[ 330R ]----|>|---- GND      (LED)
5V ----[ 10k pot ]---- GND
              |
              +---- A0
```

## Clone

```bash
git clone --recurse-submodules https://github.com/albertminasyan/arduino-rtos.git
cd arduino-rtos
```

The submodules in `.gitmodules` use SSH URLs (`git@github.com:`). Without an SSH
key on GitHub the clone fails on them; fetch over HTTPS instead:

```bash
git clone https://github.com/albertminasyan/arduino-rtos.git
cd arduino-rtos
git -c url."https://github.com/".insteadOf=git@github.com: submodule update --init
```

## Build

### 1. Install arduino-cli and the AVR toolchain

```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
sudo cp ./bin/arduino-cli /usr/local/bin
arduino-cli core update-index
arduino-cli core install arduino:avr      # avr-gcc, avr-libc, avrdude
```

The toolchain comes from the installed package. The core sources and the FreeRTOS
library come from this repo's submodules.

### 2. Point arduino-cli at this repo

```bash
source env.sh
```

`env.sh` sets `ARDUINO_DIRECTORIES_USER` to the repo root (so `libraries/` and
`hardware/` are used) and keeps the toolchain in `~/.arduino15`. Run it in every
new shell before compiling.

### 3. Compile

The sketch folder name must equal the `.ino` name.

```bash
arduino-cli compile --fqbn arduino:avr:uno rtos_demo
```

## Run

```bash
arduino-cli board list                     # find the port
sudo usermod -a -G dialout $USER           # once, then log out and in
arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:uno rtos_demo
arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

Do not use `sudo` for the upload; fix the group membership instead.

Expected output, in order:

1. `##` from `early()` in `.init0`, before the C runtime is set up.
2. `FREE RAM:` and `MCUSR=` from `main()`.
3. `DBG()` lines (`file:line function`) from `main()`, `init()`, `initVariant()`, `setup()`.
4. After the scheduler starts: a `SENSOR: <0..1023>` line about every 500 ms.

Press the button and `BUTTON: 1` appears at once, between the sensor lines. The
LED shows a single pulse for each sensor sample and a triple pulse for a button press.

## Debug tracing

`DBG()` is defined in `hardware/arduino/avr/cores/arduino/dbg.h` and is used by the
forked core (`main.cpp`, `wiring.c`), the FreeRTOS library (`variantHooks.cpp`) and
the sketch. It writes straight to USART0 with polling, so it works before
`Serial.begin()` and with interrupts disabled. It needs the forked core: with the
stock `arduino:avr` core, `<dbg.h>` is not found.

## Troubleshooting

| Symptom | Cause |
|---------|-------|
| `Permission denied` on the port | Not in `dialout`; see the `usermod` line above |
| `dbg.h: No such file or directory` | Stock core in use; run `source env.sh` so the forked core in `hardware/` is used |
| Submodule folders are empty | Clone without `--recurse-submodules`; run `git submodule update --init` |
| `Permission denied (publickey)` on clone | SSH submodule URLs; use the HTTPS variant above |
| Upload times out | Wrong port or a different bootloader; check `arduino-cli board list` |
| Onboard LED (pin 13) blinks slowly | FreeRTOS stack overflow: raise the stack size (128 words in `xTaskCreate`) |
| Onboard LED blinks fast | FreeRTOS `malloc()` failure: not enough heap |
| Banner printed only once | The monitor toggles DTR and resets the board; use `-c baudrate=9600,dtr=off` |

## Documentation

`docs/` contains interactive one-page diagrams (open in a browser):

- `boot_flow_one_page.html`: reset, Optiboot, C runtime, `main()`, scheduler start, task switching.
- `sensor_task_flow.html`: one full control-flow cycle of the sensor task.

## Credits

FreeRTOS kernel by Richard Barry. AVR port and Arduino library by Phillip Stevens
([feilipu/Arduino_FreeRTOS_Library](https://github.com/feilipu/Arduino_FreeRTOS_Library)).
Demo by Albert Minasyan.
