# Software Archaeology: FutureNet Dash

I had heard of FutureNet, but had never used it.  It is mentioned as the
first schematic capture program usable for designing Xilinx FPGAs. 
Recently I discovered that a version is available, and this my attempt to
learn this $6000 program without manuals.

# Available information

"FutureNet DASH was a schematic capture program written for IBM DOS PCs
released in the early 1980s.  It was an extremely expensive package - over
$5000 at the time.  After passing through various hands the software was
released as version 6.10 in a de-restricted version."

This is from Contrelec, which hosts this final version:

[http://www.contrelec.co.uk/pages/dashcon](http://www.contrelec.co.uk/pages/dashcon)

There is a review of it in the October 1983 issue of PC Magazine.  This
article mentions some old "mainframe" electronic design pacakges I will have
to look into at some point:

[https://books.google.com/books?id=qURs4j9vKn4C&pg=PA532&lpg=PA532&dq=futurenet+dash+review+pc+magazine&source=bl&ots=ZL4rZR97KA&sig=ACfU3U2lhqr6opEOlPX2aa_zcWXXt0bmBg&hl=en&sa=X&ved=2ahUKEwjYx6bGpePvAhXpdN8KHX9rApoQ6AEwCHoECAEQAw#v=onepage&q=futurenet%20dash%20review%20pc%20magazine&f=false](https://books.google.com/books?id=qURs4j9vKn4C&pg=PA532&lpg=PA532&dq=futurenet+dash+review+pc+magazine&source=bl&ots=ZL4rZR97KA&sig=ACfU3U2lhqr6opEOlPX2aa_zcWXXt0bmBg&hl=en&sa=X&ved=2ahUKEwjYx6bGpePvAhXpdN8KHX9rApoQ6AEwCHoECAEQAw#v=onepage&q=futurenet%20dash%20review%20pc%20magazine&f=false)

Bitsavers has some brochures and a price-list, it sure was expensive:

[http://www.bitsavers.org/pdf/futureNet](http://www.bitsavers.org/pdf/futureNet)

There is no further documentation that I can find.  However, the program has
on-line help.  I've extracted it here:

[dash-online-help.md](dash-online-help.md)

The commands are organized by function by prefix.  I grouped them together
here:

[dashlist.md](dashlist.md)

These are the groups:

	. commands are for symbols / parts
	/ commands are for wires
	# commands are for hierarchy
	[ commands are for area cut and paste
	' commands are for text
	Commands with no prefix are for everything else, such as file operations.

## Using the program

These macros are assigned to function keys, so they are probably important:

F1	HELP
F2	QUIT
F3	'P
F4	'L
F5	/D	Draw or delete a junction
F6	/ES	Delete line segment
F7	/P
F8	/ET	Erase temporary lines?
F9	'0
F10	/0	Cycle through line styles


### Menus

When you are in no other mode, right click to bring up menus.

### Setup

grid		Turn the grid on or off
grid snap	Turn snap to grid on

### Zooming

Hit PgUp and PgDn.

### Drawing wires

Postion the mouse over a pin (valid pins are highlighted when you hover over
them) and left-click (no need to hold).  Draw the line segment- it shows up
as a dashed red line when you move the mouse.  Left-click again to solidify
the segment and start a new one from the current point.  To stop drawing,
right-click the mouse.

Do not hit ESC to complete drawing- it switches to text mode.  If you do
this, hit ESC again to return to graphics mode.

You can also type / L \<Enter> to start drawing a wire- I'm not sure why
they have this since you can just left-click.

### Connecting wires

Use / d \<Enter> or hit F5 to draw or delete a junction dot.

### Deleting wires

Position mouse on wire, then hit F6 or type / E S \<Enter>.

### Adding a part

Type * \<Part number> \<Enter>

Also you can type . L \<Enter> and it will prompt for the part name.

Single device per package parts are named as you would expect, for example
use "8088" for an 8088 microprocessor.  But for a TTL inverter, you need to
use "7404A" to get the first one of the package, or "7404B" for the second
one, etc.

### Moving a part

Left-click in the center of the part and it should enter move part mode.

Also you can type . M \<Enter>

### Rotating / Mirroring part

There are a number of commands for this:

    .R	Rotate part
    .RE	Reflect part

### Duplicating a part

Position mouse over part and enter . C \<Enter>.  A copy of the part will be
attached to the mouse.

### Deleting a part

Position mouse over part and enter . E \<Enter>.

### View a list of available parts

Type . D I R \<Enter>

A window with a list of symbol libraries along with a list of symbols and a preview
pops up.

### Load a symbol library

You need to do this to make the symbols in a library available to you.

Type . L I B \<Enter>

You will be prompted for the name of the library.  You can enter a complete
path, such as C:\\DATAIO\\FNLIB\\GENERIC.LIB.

### Some important parts

These are in SYSTEM.LIB, which seems to be loaded by default:

TBLOCKA, TBLOCKB, TBLOCKC, TBLOCKD, and TITL are title blocks.

TX1, TX2, TX3 are transformers.

SWNC, SWNO are switches.

RES is a resistor.

POT is a potentiometer.

PCAP is a polarized capacitor.

CON1, CON3, CON4, etc. are generic connectors.

DIODE is a diode.

JUMPER is a jumper

These are in GENERIC.LIB:

BULLETIO   I/O port (inter-sheet connector?)

BULLETR  (inter-sheet connector?)

CONnn  Generic connectors

EGND   Normal earth ground

GND1, GND2, GND3.. other grounds

E4  power?

