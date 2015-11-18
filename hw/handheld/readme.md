# Handheld computer

This is a handheld computer for driving a Zebra-140 label printer I made in
1996.

Specs:

	IDT R3041 32-bit MIPS CPU
	512KB SRAM
	32KB EEPROM
	Keypad
	2x16 LCD
	RTC72421 Real time clock
	Barcode reader
	Speaker interface
	RS232 serial interface
	Powered with 4 1.2V NiCAD

The FPGA and RAM are always on.  The CPU, clock oscillator and EPROM are
powered off between keypresses to save power.

Both the FPGA config file and the software for the CPU are stored in the
EPROM.
