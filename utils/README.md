## Short utilities:

* exotobin: Convert Motorola S19 files to binary

* tabify: Replace spaces with tabs in indentation

* tab: Replace tabs with spaces given a list of field widths

* hd: Hex dump (prints ... for zeros)

* crlf: Add or remove CR-LF line endings

* base64: Decode base64

* csv: Clean up CSV files

* csvcmp: Simple compare CSV tables

* bomcmp: Compare two hierarchical BOMs in CSV format

* bintoc: Convert binary file into C source code (for FPGA image)

* lfsr: Show linear feedback shift register tap points and find code for a particular count.

* lpf: Convert PCL / ASCII to postscript

* reindent: Change indentation

* rev: Reverse lines

* subtotal: Compute subtotals

* subtotal1: Compute subtotals

* upc: Generate UPC bar codes

* upccheck: Check UPC bar code / compute check digit

These all compute checksum in the same way that the Xilinx HW-130
programmer:

* hex: Convert Xilinx .hex file to binary (computes checksum)
* chk: Compute Xilinx checksum of binary file
* exo: Convert Xilinx .exo file to binary (computes checksum)
* mcs: Convert Xilinx .mcs file to binary (computes checksum)

## Serial connect: c.c

This is a replacement for the very old "cu" program (original part of
UUCP!).

Use this to turn an old Linux box into a console server, so that you can
"telnet linux-box 2002" to connect to ttyS2.  Also, it can be used to
connect to the serial port directly from the shell.

There is on-line help.  You will see when you run the program.

### For console server:

Put this into /etc/xinetd.d/direct_ttyS2

	# default: on
	# description: The telnet server serves telnet sessions; it uses \
	#       unencrypted username/password pairs for authentication.
	service direct_ttyS2
	{
	        flags           = REUSE
	        type            = UNLISTED
	        socket_type     = stream        
	        wait            = no
	        user            = root
	        port            = 2002
	        protocol        = tcp
	        server          = /usr/sbin/in.telnetd
	        server_args     = -L /root/c2
	        log_on_failure  += USERID
	        disable         = no
	}

We add more serial ports to a PC like this:

/etc/rc.local:

	# assign Siig/oxford serial ports

	/bin/setserial /dev/ttyS2 port 0x4020 irq 96 uart 16950
	/bin/setserial /dev/ttyS3 port 0x4028 irq 96 uart 16950
	/bin/setserial /dev/ttyS4 port 0x40A0 irq 104 uart 16950
	/bin/setserial /dev/ttyS5 port 0x40A8 irq 104 uart 16950

Build instructions:

	su root
	cd
	cc -o c c.c
	ln -s c c0
	ln -s c c1
	ln -s c c2

Link to c for each serial port you have.  For example c2 for ttyS2.  Create
an rc file for each of these.  For example, this is for c2:

/root/.c2rc

	port = "/dev/ttyS4"
	baud = "9600"
	flow = "rtscts"
	off = "/root/power off 1"
	on = "/root/power on 1"

### For direct use on command line:

Just type:
	./c /dev/ttyS2 --baud=19200
or:
	./c /dev/ttyUSB0 --baud=9600

Here is a shell script to control CPS Power string (uses 300 baud serial
port):

	power
