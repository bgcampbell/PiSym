# PiSYM
## Overview
PiSYM is a Synertek SYM-1 emulator that runs on a Raspberry Pi Pico 2. It allows SYM-1 hardware to be added in order to provide the full experience of using a mid-1970's style computer.

Emulation of some hardware using modern capabilities is included to make it easier to use. The Raspberry Pi Pico 2's USB port simulates a CRT allowing terminal access with no additional hardware. The Raspberry Pi Pico 2's internal flash storage can be used to simulate a cassette recorder with multiple tapes.

PiSYM is licenced under the GPLv3 licence. Other modules used within PiSYM have their own licences.
## Installation
The code was created for the Raspberry Pi Pico extension in Visual Studio.

The memory requirements of PiSYM require the use of the RP2350-based Raspberry Pi Pico 2 rather than the RP2040-based Raspberry Pi Pico.

The minimum installation requires just the Raspberry Pi Pico 2. With just this device, most functionality will be available, including:-
- CRT access via the Raspberry Pi Pico 2 USB port
- Cassette simulation using the Raspberry Pi Pico 2 flash memory
- SY6532 I/O port and DDR functionality (i.e. LDA and STA to the SY6532 register addresses drives the Raspberry Pi Pico 2 GP0-15)
- Supermon V1.1
- Supermon debugger
- Supermon write protection mechanism
- BASIC including the trigonometric function extension
- RAE
- Access to Raspberry Pi Pico 2's on-board LED from 6502 code (JSR $9FFF with A = 0 for off, A != 0 for on)

In addition, PiSYM offers its own command line functions including a more modern 6502 debugger offering breakpoints and watches.

You can add a keyboard and seven segment display as per the standard SYM-1 circuit diagram to access the computer without any terminal. For many who owned the SYM-1, this is all they had.

## Limitations
This version of PiSYM is an initial release with some functionality not yet implemented and some implemented but not tested. So far, PiSYM has only been tested when connected to a Windows computer.

The following functionality should be available but is not yet tested:-
- RS232 and TTY interface (additional hardware required)
- True cassette I/O (additional hardware required)
- SY6532 timer
- SY6532 PA7 interrupt
- Physical IRQ input and interrupt mechanism
- User NMI (alternative to the SYM-1 debugger NMI)

The following functionality is not yet implemented:-
- All VIAs
- Oscilloscope output

## Usage
After installation, with nothing but the Raspberry Pi Pico 2, PiSYM will boot looking for a device to connect to. If you have a terminal program like PiSYM, use it to conect to the Pico's COM port and press a key. The terminal program should display **Redirecting I/O via USB** followed by Supermon's "." prompt. From here, simply use standard Supermon commands to operate PiSYM. The command **J 0** or **G C000** runs BASIC. **G B000** runs RAE.

Pressing the F1 key enters the PiSYM command line interface. Here you can alter the configuration options, manage the simulated tapes and files, and access the 6502 debugger. See the document **PISYM COMMAND LINE** document in the Documents directory for further details.

While running the SYM-1 code normally (i.e. not in or under control of the debugger), PiSYM offers two ways of monitoring performance. PiSYM executes every 6502 instruction in the same amount of time it does on a real 1MHz 6502. Since the Raspberry Pi Pico 2 runs a lot faster, PiSYM executes the instruction then waits until the normal execution time is up. This active/wait time is indicated in two places:-
- Raspberry Pi Pico 2 on-board LED. It lights during the wait time and is off while an instruction is executing. When operating properly, this LED will be lit but dimmed.
- Via the SYNC output on GP22 of the Raspberry Pi Pico 2. This signal is high while the instruction is being executed and low during the wait time. It can be monitored with an oscilloscope.

There are two compiler options located in **include/PiSYM.h**.

Defining FULLSPEED skips the delay loop and runs 6502 code as fast as the Raspberry Pi Pico 2 can achieve. Be aware that any functions that rely on delay loops for timing will likely fail.

Defining SPEEDTEST and a value from 2 to 7 will create a special version that loops executing a single psuedo-instruction that takes the nominates number of clock cycles to execute and does not increment the program counter. Ensure that FULLSPEED is undefined. This version of PiSYM is useful for monitoring SYNC to measure the accuracy of PiSYM's emulation. The frequency corresponds to the instruction speed. SPEEDTEST will have to be undefined, the code recompiled and uploaded to the Raspberry Pi Pico 2 to restor normal functionality.

## Blink
In the Blink folder is the sample code for a 6502 program that blinks the Raspberry Pi Pico 2's LED. This overrides the standard operation of this LED.

The Blink directory contaions the following files:-
- **Blink.asm** : Source code for the Blink program
- **0010.10** and **0020.10** : RAE files for the Blink program for transfer and use from an SD Card. Use **get f10** from within RAE to load them.

## Final words
As stated before, this is very much a work in progress. Some functionality is untested and other is not even implemented.

There is no warranty for the program, to the extent permitted by applicable law. Except when otherwise stated in writing the copyright holders and/or other parties provide the program “as is” without warranty of any kind, either expressed or implied, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose. The entire risk as to the quality and performance of the program is with you. Should the program prove defective, you assume the cost of all necessary servicing, repair or correction.
