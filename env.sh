ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# sources: main repo = sketchbook (libraries/FreeRTOS and hardware/arduino/avr submodules)
export ARDUINO_DIRECTORIES_USER="$ROOT"
# toolchain: default arduino-cli install, not part of any repo
export ARDUINO_DIRECTORIES_DATA=~/.arduino15
export ARDUINO_DIRECTORIES_DOWNLOADS=~/.arduino15/staging
