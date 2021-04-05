# Software Archaeology: FutureNet Dash

I had heard of FutureNet Dash, but had never used it.  It is mentioned as
the first schematic capture program usable for designing Xilinx FPGAs and
maybe the first schematic capture program available for IBM PCs.  Recently I
discovered that a version is available, and this my attempt to learn this
$3850 (in 1988) program without manuals.

# Available information

Someone wrote about FutureNet, but the site is down.  This is from the
Wayback machine:

[FutureNet_Artifacts.pdf](FutureNet_Artifacts.pdf)

Some ASICs used in the Atari ST were designed in FutureNet, see here:

[https://www.chzsoft.de/asic-web/](https://www.chzsoft.de/asic-web/)

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

You need to set up the environment.  It's a good idea to create a batch file
for this:

    SET FNMOUSE=M
    SET FNDISPLAY=C:\DATAIO\FN\VGA12.DG
    SET FNLIB=C:\DATAIO\FNLIB
    SET FNLOG=FNLOG.CMD
    SET FNELOG=FNERR.LOG
    SET FNPRO=FNPRO.CMD
    path=c:\dataio\fn;%path%

Then type "fn" or "dash" to start the program.

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

    grid	- Turn the grid on or off
    grid snap	- Turn snap to grid on

I've found that you probably do not want snap to grid on.  Many symbols have
pins which are not aligned with the grid.  Also the native grid is not much
finer than the displayed grid, so you don't really need it.

### Zooming

Hit PgUp and PgDn to jump between different magnifications.  You can also
use the "zoom" command to for custom ones.

Zooming works fairly well in that a correctly scaled font is chosen for the
zoom level.  Well at least two of the prefined magnifications have legible
text.  OrCAD had only a single good zoom level.

### Adding a part

    * <Part number> <Enter>
    .L <Part number> <Enter>
    * <Enter>                  - It will prompt for the part number
    .L <Enter>                 - It will prompt for the part number

Single device per package parts are named as you would expect, for example
use 8088 for an 8088 microprocessor.  But for a TTL inverter, you need to
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

Type .DIR \<Enter>

A window with a list of symbol libraries along with a list of symbols and a preview
pops up.

### Load a symbol library

You need to do this to make the symbols in a library available to you.

Type .LIB \<Enter>

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

### Place a label on a wire

FutureNet calls all text "Alphanumeric Fields".  What the text actually does
depends on where it is located and what its "attribute" is.  I think we
would call attributes types today.  Attributes are mostly predefined (but there
are also user defined attributes), and are numbered and named.

First, decide on the purpose of the text.  For a label on a wire, we want a
"SIG"- a Signal, which is attribute number 5.  So enter the command to
select this attribute:

    'a sig <Enter>       - Choose attribute by name
    'a 5 <Enter>         - Choose attribute by number
    'a <Enter>           - Brings up a menu of attributes

From now on any mew "Alphanumeric Fields" we enter will be signals.  In the
rather likely event that you forgot to select the correct attribute type,
you can change the attribute of existing text like this:

    'ch a sig <Enter>    - Change attribute of text under mouse to SIG

Now to actually write some text, position mouse on a wire and hit ESC.  This
switches to text mode and you can type the label.  The "Point of Effect" (as
FutureNet calls it) must be on the wire.  When you are done typing, don't
hit \<Enter>.  Instead hit ESC again to return to graphics mode.

### Attribute List

Number  |Name           |Description
--------|---------------|------------
0	|COM		|Comment?
1	|PIN		|"40" pin number
2	|LOC		|"UNNN" reference designator
3	|PART		|"8088"
5	|SIG		|Signal (wire label)
20	|PINT		|"26" Tristate pin
22	|PNBT		|"9" Bidirectional pin
23	|PINI		|"33" Input pin
24	|PINO		|"24" Output pin
50	|TITL		|Title in title block
51	|DNUM		|"000000" Drawing number in title block
52	|DREV		|"A" Rev number in title block
53	|DPAG		|"1 OF 1" in title block
54	|DATE		|"July 26, 1991" Date in title block
100	|GND		|Auto-connect to power net (shows in lower right corner of 7404 symbol)
101	|+5V		|Auto-connect to power net (shows in upper right corner of 7404 symbol)

### Connect to a power rail

OrCAD has a special power symbols and you give the name of the rail as the
part value.  FutureNet works differently: you label a wire with the name of
the power rail and set the attribute of the label to "PWRS" (for "Named
Power Signal").

It is common to have hidden and automatically connected power pins on 74xx
series TTL gates.  The symbols for such gates include pin numbers as two
text fields (often pins 14 and 7).  The attributes for these fields are
"GND" (attribute 100) and "+5V" (attribute 101).

Questions: Are all attributes above 100 global?  How do you have a hidden
+3.3V rail?

### Inter-sheet connections in hierarchical designs

The subsheet symbol (a "functional block" created with .F) includes pins
(create them with .-).  The pins should be labeled with text that has one of
the pin attributes: PINI (for inputs), PINO (for outputs), PNBT (for
bidirectional), etc.

The subsheet itself should have a wire labeled with text matching the pin. 
"SIG" works as the attribute for the text (and I suspect other attributes
will also work).

### Flat designs

A single function block can be made of multiple sheets, so one way is to
create a top-level sheet with a single functional block (create with .F) in
it.  Within the functional block symbol, place multiple "alphanumeric
fields" containing the file names of the sub-sheets, each with their
attribute set to FILE (attribute number 8).

Point to the first of these and type "#D" to descend hierarchy.  You can hit
"#U" to return to the top-level.  The first time you descend into each file
FutureNet will indicate that it is new and ask if it should create it.

When you are in one of the subsheets, you can traverse between its siblings
with the #R and #L commands.  All of the siblings share the same namespace
for signals- two wires labeled with the same signal name will be connected.

Speculation: the DCM.EXE program accepts a list of top-level sheets.  I
think you can dispense with the top-level sheet and just give a list of .DWG
files to DCM.EXE for a flat design.  But you will not have the #R and #L
commands this way.

### How do you annotate?

OrCAD has an automatic annotation process to replace the default reference
designators (such as U?  in OrCAD or UNNN in FutureNet) with unique numbers
(U1, U2, etc.).  I have found no such thing in FutureNet.  It seems you have
to set them by hand.

### How do you generate the netlist?

The sequence of commands is as follows:

First you preprocess the .DWG files into a single .DCM file with:

    DCM -itop -lfirst+second

Next you convert the .DCM file into a .NET file with:

    NETC top

You may convert the .NET file into a PADS ASCII file with:

    NET2PADS     (it prompts for the name of the .NET file)

### How do you create new parts?

### How do you select sheet size?

There is a command:

    size a     Select A size sheet

The default size is b.

An annoying limitation is that you can not reduce the size of a non-empty drawing.

### How do busses work?

I see them named like this; QB\<7:0>

Bus breakout wires are labeled 0, 1, 2, etc.

Can you have multiple labels on the same wire?  There are multiple busses on
the same sheet, and the wires for each of them will be labeled 0, 1, 2, etc.

Symbol types:

block symbol

.F create functional block

ALso, .G toggle between graphic symbol (for comments only), and circuit
symbol (electrically significant)

"alphanumeric fields" have attribute numbers
