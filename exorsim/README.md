# EXORsim - Motorola EXORciser Simulator

### Links

[MDOS quick start](doc/mdos-intro.md)
<br>
[MDOS reference](doc/mdos-ref.md)
<br>
[MDOS technical information](doc/mdos-tech.md)
<br>
[EXORsim usage guide](doc/usage.md)
<br>
[MPL language](mpl/readme.md)
<br>
[EXORterm info](doc/exorterm.md)

## Introduction

<p>EXORsim simulates a Motorola EXORciser, which was a development system
that <a href="http://en.wikipedia.org/wiki/Motorola">Motorola</a> sold in
the late 1970s and early 1980s for the 8-bit <a
href="http://en.wikipedia.org/wiki/Motorola_6800">6800</a> microprocessor. 
Some pictures of this development system can be found at Pekka Tanskanen's
website <a href="http://www.exorciser.net/index_en.htm">here.</a>

<p>EXORsim also simulates a <a
href="http://www.swtpc.com/mholley/swtpc_6800.htm">SWTPC 6800 Computer
System</a>, which allows you to run the <a
href="http://en.wikipedia.org/wiki/FLEX_%28operating_system%29">TSC FLEX</a>
operating system.

<p>EXORsim now emulates an EXORterm!  I was finally able to find
manuals for the EXORterm 155, so I've implemented an emulator for it.  This
allows the <a href="https://github.com/jhallen/joes-sandbox/blob/master/exorsim/doc/mdos-intro.md#e-editorm-resident-editor">EDITORM Resident Editor</a> to operate in screen mode.
</p>

<p>The EXORterm emulator operates as a filter between the EXORciser and the
user's ANSI terminal emulator (such as XTerm).  This allows you to use
terminal emulator features such as scroll-back.  I find this to be more
useful than some emulators which run under MS-Windows, but provide only a
very rudimentary terimnal.</li>

<p>EXORsim provides these features:

<ul>
<li>Simulates 6800 CPU and the following peripherals:
<ul>
<li>ACIA serial port.</li>
<li>Line printer port.</li>
<li>Floppy disk controller:
<ul><li>EXORdisk-II floppy diskette controller emulated by
intercepting calls to the controller ROM</li>

<li><a href="http://en.wikipedia.org/wiki/Western_Digital_FD1771">FD1771</a> hardware emulation for <a
href="http://www.swtpc.com/mholley/MF_68/MF_68_Index.htm">MF68
with DC_1 controller</a> floppy drive</li>
</ul>
</ul>

<li>Provides a separate command, MDOS, which allows you to read and write
files to MDOS diskette images.</li>

<li>Provides a stand-alone 6800 disassembler.</li>

<li>Includes a powerful debug monitor which allows you to:

<ul><li>Assemble 6800 directly to memory.  The assembler is
symbolic and powerful enough to assemble programs in the
<a
href="http://test.dankohn.info/~myhome/projects/68HC11/AXIOM_HC11/Source/Users%20Group/">Motorola User's Group
Library</a>.  One way is to select the entire source listing
from the web-browser and paste into the monitor after
starting the "a" command.</li>
		
<li>Disassemble memory to the screen.</li>

<li>Load and save Motorola <a href="http://en.wikipedia.org/wiki/SREC_%28file_format%29">S19</a> files
/ tapes.</li>

<li>Load and save binary images.</li>

<li>Trace program execution.</li>

<li>Single step and continue until breakpoint.  A trace
buffer is provided, so you can see the instructions which
led up to the breakpoint.</li>

<li>Call a 6800 subroutine and return to debug monitor when
it returns.</li>

<li>Hex dump and memory/register modifiction.</li>
</ul>
</li>

<li>The output from the disassembler and the tracing features of the debug
monitor are annotated with a information from a "facts" file.  For example,
this file will have known hardware addresses, MDOS system calls, etc.  It's
not as powerful as <a href="http://www.hex-rays.com/idapro/">IDA</a>, but
it works well enough for small programs.</li>

</ul>
<br>

# Build instructions

Just type "make" to compile it:

	exor	This is the simulator

	mdos	This provides file access to MDOS diskette images

	unasm	This is a stand-alone 6800 disassember

Right now the exbug.bin and swtbug.bin files must be in the current
directory.

To run the simulator:

	./exor mdos.dsk

You can get MDOS disk images here:

[ftp://bitsavers.informatik.uni-stuttgart.de/bits/Motorola/mdos/](ftp://bitsavers.informatik.uni-stuttgart.de/bits/Motorola/mdos/)

You need to use "ImageDisk" to extract the binary data from from the .IMD
files.

	imdu disk.imd disk.dsk /b

You can get ImageDisk from here:

[http://www.classiccmp.org/dunfield/img/index.htm](http://www.classiccmp.org/dunfield/img/index.htm)

Documentation about MDOS can be found here:

[http://www.bitsavers.org/pdf/motorola](http://www.bitsavers.org/pdf/motorola/)

		M68MDOS3_MDOS3um_Jun79.pdf	Operating system user's manual.
		MEK6800D2.pdf			Microprocessor datasheet
		M6800_Microprocessor_Applications_Manual_1975.pdf
		Motorola_M6800_Programming_Reference_Manual_M68PRM(D)_Nov76.pdf

Some text documentation and notes can be found here:

[EXORsim usage guide](doc/usage.md)

To run FLEX2:

	./exor --swtpc flex2.dsk

You can get FLEX2 disk images here:

[http://www.evenson-consulting.com/swtpc/Downloads.htm](http://www.evenson-consulting.com/swtpc/Downloads.htm)

Download the "FULL kit", install it and find the FLEX2 .DSK files.
