# nrf52840 blinky demo

A Particle xeon driving an Adafruit 128x32 OLED display.

# Build Notes

Using gdb with a black magic debug probe

arm-none-eabi-gdb -ex "target extended-remote \\\\.\\COM4" pinky.elf

at the gdb prompt use the bmp command to attach to the target cpu

