
# I2C Bus / SMBUS Monitor

This converts a cheap Cyclone-II EP2C5T144 "minimum system board" into a
nice I2C bus monitor.  It converts I2C into a readable ASCII format and
sends it over a 3.3V TTL-level RS-232 interface.  You can use a USB to
TTL-level RS-232 cable to connect this to a PC.  You could perform long-term
monitoring of an I2C bus by enabling logging in Minicom or some other
similar serial communications program.

## Board

Here is an Altera "Minimum system board" that you can buy on EBay.  Search
for "Altera Board".

[EP2C5T144 Board](https://github.com/jhallen/joes-sandbox/tree/master/fpga/i2c_monitor/mp2c5board.jpg)

## Pins

### I2C

* Connect SCL to pin 79
* Connect SDA to pin 75

SCL and SDA have de-glitch filters so that single cycle (50 MHz) pulses are
ignored.

### LEDs

* LED0 (D2) blinks at 1 Hz
* LED1 (D4) is SDA (on if high)
* LED2 (D5) is SCL (on if high)

### RS-232

BAUD rate is 921600, but you can change this: look for the .baud_rate signal
in i2cmon.v.  BAUD rates up to 921600 work well with cheap USB to serial
converters.  Faster rates might be possible with real serial ports.  There
is an 8K FIFO buffer to help buffer short bursts of high-speed I2C.  The
serial port supports XON/XOFF (^S/^Q) flow control.

* Connect TTL serial output is to pin 72
* Connect TTL serial input is from pin 71

Output on serial will look like this:

Time check, printed every 10 seconds:

	[0040.0] CD

* ___C___ if SCL is high. ___c___ if SCL is low
* ___D___ if SDA is high. ___d___ if SDA is low

Time is in [seconds . tenths of seconds].

Received data:

	[0043.9] A0+ 0B+ R A1+ 08-

* ___+___ means ACK after 8 bits
* ___-___ means NAK after 8 bits

* ___?___ means short byte: less than 8-bits + ACK received before
          Stop or Repeated Start.

* ___R___ means repeated start detected (normally indicates
          that the following byte is an address with R/~W (bit 0) high
          to indicate read, then followed by read bytes).

Empty transaction:

	[0043.9] E

* It means we got Start and then Stop, but no data.

End of transaction is indicated by CR-LF.  If cursor is not at beginning of
line, it means transaction did not end.  (it would be nice to add time
checks showing SCL and SDA in the middle of transactions).
