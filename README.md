# nrf52840 blinky demo

Using gdb with a black magic debug probe

arm-none-eabi-gdb -ex "target extended-remote \\\\.\\COMxxx" pinky.elf

at the gdb prompt use the bmp command to attach to the target cpu

