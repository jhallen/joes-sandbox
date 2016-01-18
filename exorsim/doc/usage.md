<h3>Usage guide</h3>

<p>Summary: exor [options] [disk0 [disk1 [disk2 [disk3]]]]</p>

<dl><dt>Options:<dl>
<dt>--trace<dd>Produce instruction trace on stderr
<dt>--dtrace<dd>Produce disk access trace on stderr
<dt>--skip nnn<dd>Skip first nnn insns in trace
<dt>--swtpc<dd>Simulate SWTPC instead of EXORciser
<dt>--exbug name<dd>Give name for ROM if not 'exbug.bin'/'swtbug.bin'
<dt>-x<dd>Go into EXBUG/SWTBUG instead of directly into MDOS/FLEX
<dt>--facts file<dd>Process facts files for commented disassembly
<dt>--lower<dd>Allow lowercase
<dt>--mon<dd>Start at monitor prompt
</dl></dl>

<p>Default disk0 is mdos.dsk / flex.dsk</p>

<p>To load MDOS from EXBUG, type MAID followed by E800;G</p>
<p>To load FLEX from SWTBUG, type D</p>

<p>For example, to simulate an EXORciser with exbug.bin and mdos.dsk in the
current directory:</p>

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ ./exor
Load facts file 'facts'
'exbug.bin' loaded.
'mdos.dsk' opened for drive 0 (single sided)

Hit Ctrl-C for simulator command line.  Starting simulation...

Floppy error: attempt to access non-existent disk 1

MDOS 03.00
=
</code></pre>

<p>To simulate SWTPC instead, run with the --swtpc option:</p>

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ ./exor --swtpc
Load facts file 'facts'
'swtbug.bin' loaded.
'flex.dsk' opened for drive 0 (tracks=80 sectors=72)

Hit Ctrl-C for simulator command line.  Starting simulation...

FLEX 2.0

DATE (MM,DD,YY)? 02,07,11

+++
</code></pre>

<h3>About exbug.bin and swtbug.bin</h3>

<p>These memory images must each be exactly 64K bytes.  They contain the
initial memory image to be used by the simulator.  I saved the images right
at the point where the disk boot function is called so that EXORsim can
immediately boot the DOS instead of going into the monitor.</p>

<p>swtbug.bin contains <a
href="http://www.swtpc.com/mholley/swtbug/SWTBUG_Index.htm">SWTBUG</a> at
$E000 - $E3FF, but patched to boot the floppy to $A100 for FLEX2 instead of
$2400 for miniFlex.</p>

<p>exbug.bin contains EXBUG at $F000 - $FBFF and the diskette controller at
$E800 - $EBFF.</p>

<h3>About diskette images</h3>

<p>EXORsim supports two MDOS disk image formats:</p>
<ul>
<li>Single sided: 2002 128-bit sectors, image size is 256,256 bytes (77 tracks of 26 sectors)</li>
<li>Double sided: 4004 128-bit sectors, image size is 512,512 bytes (154 tracks of 26 sectors)</li>
</ul>

<p>EXORsim supports these eight FLEX image formats:</p>
<ul>
<li>35 tracks of 10 sectors, image size is 89,600 bytes</li>
<li>40 tracks of 10 sectors, image size is 102,400 bytes</li>
<li>35 tracks of 20 sectors, image size is 179,200 bytes</li>
<li>40 tracks of 20 sectors, image size is 204,800 bytes</li>
<li>80 tracks of 18 sectors, image size is 368,640 bytes</li>
<li>80 tracks of 20 sectors, image size is 409,600 bytes</li>
<li>80 tracks of 36 sectors, image size is 737,280 bytes</li>
<li>80 tracks of 72 sectors, image size is 1,474,560 bytes</li>
</ul>

<h3>Monitor</h3>

<p>To enter the debug monitor, hit Ctrl-C.  The 128 instruction trace buffer
will be displayed and then the monitor prompt is presented.</p>

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ ./exor
Load facts file 'facts'
'exbug.bin' loaded.
'mdos.dsk' opened for drive 0 (single sided)

Hit Ctrl-C for simulator command line.  Starting simulation...

Floppy error: attempt to access non-existent disk 1

MDOS 03.00
=Interrupt!

- - - - - -

     610433 A=83 B=00 X=06A7 SP=E7EB ------          1B1E: EE 02    LDX 02,X  EA=06A9 D=0953 
     610434 A=83 B=00 X=0953 SP=E7EB ------          1B20: AD 0C    JSR 0C,X  EA=095F        

     610435 A=83 B=00 X=0953 SP=E7E9 ------          095F: 30       TSX                      
     610436 A=83 B=00 X=E7EA SP=E7E9 ------          0960: EE 00    LDX 00,X  EA=E7EA D=1B22 
     610437 A=83 B=00 X=1B22 SP=E7E9 ------          0962: EE 00    LDX 00,X  EA=1B22 D=1BE5 
     610438 A=83 B=00 X=1BE5 SP=E7E9 ------          0964: EE 00    LDX 00,X  EA=1BE5 D=0120 
     610439 A=83 B=00 X=0120 SP=E7E9 ------          0966: A6 01    LDA 01,X  EA=0121 D=83   
     610440 A=83 B=00 X=0120 SP=E7E9 --N---          0968: 2A 0B    BPL 0975  EA=0975        
     610441 A=83 B=00 X=0120 SP=E7E9 --N---          096A: 7C FF 53 INC FF53  EA=FF53(AECHO) 
     610442 A=83 B=00 X=0120 SP=E7E9 ------          096D: BD F0 15 JSR F015  EA=F015(INCHV) 

     610443 A=83 B=00 X=0120 SP=E7E7 ------ INCHV    F015: 7E FA A0 JMP FAA0  EA=FAA0(INCH)  Input char with echo (strip bit 7)
     610444 A=83 B=00 X=0120 SP=E7E7 ------ INCH     FAA0: 8D E9    BSR FA8B  EA=FA8B(INBYTE) Input character

     610445 ---- Subroutine at FA8B processed by simulator ---- RTS executed ---

>    610446 A=00 B=00 X=0120 SP=E7E7 ------          FAA2: 84 7F    ANDA #7F                 

Type 'help'
% 
</code></pre>

<p>The '>' before the last line of the instruction trace indicates the
current PC value, $FAA2 in this case.  It's the next instruction to be
executed if you step or continue.</p>

<p>You can hit Ctrl-C again to exit EXORsim.</p>

<h3>Monitor commands</h3>

<p>The follow commands are available:</p>

<dl>
<dt>q<dd>Quit simulator
<dt>reset<dd>Reset processor and continue simulating
<dt>abort<dd>Abort (send NMI) to processor
<dt>t on<dd>Turn tracing on
<dt>t off<dd>Turn tracing off
<dt>c<dd>Continue execution with current PC address
<dt>c hhhh<dd>Continue execution, but with new PC address hhhh
<dt>s<dd>Execute one instruction and return to monitor
<dt>s hhhh<dd>Execute one instruction at new PC address hhhh and return to
monitor
<dt>x hhhh<dd>Call subroute at <i>hhhh</i> and return to monitor when it
returns
<dt>b<dd>Clear breakpoint
<dt>b hhhh<dd>Set breakpoint at address hhhh
<dt>r<dd>Show registers
<dt>r reg hhhh<dd>Set register <i>reg</i> to <i>hhhh</i>
<dt>caps on<dd>Force conversion to uppercase when simulation is running
<dt>caps off<dd>Allow lower case letters to reach simulation
<dt>d hhhh [nnnn]<dd>Hex dump beginning at hhhh of size nnnn
<dt>m hhhh<dd>Modify memory beginning at hhhh
<dt>a hhhh<dd>Assemble beginning at hhhh
<dt>u hhhh<dd>Disassemble beginning at hhhh
<dt>u<dd>Disassemble next 20 instructions
<dt>clr<dd>Clear symbol table
<dt>sy<dd>Show symbol table
<dt>p hhhh nnnn [ssss]<dd>"Punch" (to paper tape) memory image in S19 format starting at
hhhh, size nnnn.  Put starting execution address ssss in S9 record.
<dt>l<dd>Load (from paper tape) S19 formatted image into memory.
<dt>save<dd>Save all 64K of memory in file called "dump"
<dt>save <i>file</i><dd>Save 64K of memory in <i>file</i>
<dt>save <i>file</i> <i>start</i> <i>size</i><dd>Save memory block in
<i>file</i>
<dt>read<dd>Read file "dump" into memory
<dt>read <i>file</i><dd>Read <i>file</i> into memory starting at 0
<dt>read <i>file</i> <i>start</i><dd>Read <i>file</i> into memory starting
at <i>start</i>
<dt>poll on<dd>Turn ACIA (serial port) polling on.  This means that when the
program reads the ACIA status register, the simulator will check for
keyboard input and sleep for a few milliseconds.

<dt>poll off<dd>Turn ACIA (serial port) polling off.  This means that when
the program reads the ACIA status register, the simulator will immediately
indicate that input is available.  When the program reads the data register,
the simulator executes a blocking Linux read to get the character.  In this
way, less trace output is generated and less CPU time is used.  However,
some programs read from the ACIA data register even when no input is
expected, so the simulation can sometimes appear to be stuck.  </dl>

<p>The above commands can be modified with redirection operators:</p>

<p>Append <i>>file</i> to redirect output to <i>file</i></p>
<p>Append <i>>>file</i> to append output to <i>file</i>
<p>Append <i>&lt;file</i> to redirect input from <i>file</i>

<h3>Examples</h3>

<p>Here is an example monitor session where we type in a program from the
Motorola User's Group Library- no. 72 by Don L. Jackson, which computes
integer square root:</p>

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ <b>./exor --mon</b>
Load facts file 'facts'
'exbug.bin' loaded.
'mdos.dsk' opened for drive 0 (single sided)

Hit Ctrl-C for simulator command line.  Starting simulation...
>         0 A=00 B=00 X=0000 SP=FF8A ------ OSLOAD   E800: 8E FF 8A LDS #$FF8A                Load OS

Type 'help'
% <b>a 0</b>		<i>Assemble</i>
0000:<b>  NAM ISQRT</b>
0000:<b> *********************************************************</b>
0000:<b> * THIS SUBROUTINE CALCULATES THE INTEGER</b>
0000:<b> * SQUARE ROOT OF ANY POSITIVE NUMBER UP</b>
0000:<b> * TO 255 DECIMAL.  ENTER WITH NUMBER IN</b>
0000:<b> * ACCUMULATOR A, RESULT WILL BE RETURNED</b>
0000:<b> * IN ACCUMULATOR A.  B AND CC REGISTERS</b>
0000:<b> * ARE AFFECTED.  PROGRAM IS RELOCATABLE.</b>
0000:<b> *</b>
0000:<b> * PROGRAMMED BY DON L. JACKSON 11/27/76</b>
0000:<b> ***************************************************************</b>
0000:<b>  SPC 1</b>
0000:<b>  ORG $2100</b>	<i>Notice that ORG changes address</i>
2100:<b> ISQRT LDA B #$FF</b>
2102:<b> ISQRT2 ADD B #2</b>
2104:<b>  SBA</b>
2105:<b>  BCC ISQRT2</b>
2107:<b>  TBA</b>
2108:<b>  LSR A</b>
2109:<b>  RTS</b>
210a:<b>  SPC 1</b>
210a:<b>  END ISQRT</b>
210a: 
% <b>u 2100</b>	<i>Unassemble</i>
2100: C6 FF               LDB #$FF
2102: CB 02               ADDB #$02
2104: 10                  SBA
2105: 24 FB               BCC $2102
2107: 17                  TBA
2108: 44                  LSRA
2109: 39                  RTS
210A: 00                  ???
210B: 00                  ???
210C: 00                  ???
210D: 00                  ???
210E: 00                  ???
210F: 00                  ???
2110: 00                  ???
2111: 00                  ???
2112: 00                  ???
2113: 00                  ???
2114: 00                  ???
2115: 00                  ???
2116: 00                  ???
2117: 00                  ???
2118: 00                  ???
% <b>sy</b>		<i>Show symbol table</i>
2102 ISQRT2
2100 ISQRT
% <b>r a 19</b>	<i>Set register A to decimal 25</i>
% <b>t on</b>		<i>Enable tracing</i>
% <b>x 2100</b>	<i>Call subroutine</i>
          0 A=19 B=00 X=0000 SP=FF8A ------ ISQRT    2100: C6 FF    LDB #FF   EA=2101 D=FF   
          1 A=19 B=FF X=0000 SP=FF8A --N--- ISQRT2   2102: CB 02    ADDB #02  EA=2103 D=02   
          2 A=19 B=01 X=0000 SP=FF8A H----C          2104: 10       SBA                      
          3 A=18 B=01 X=0000 SP=FF8A H-----          2105: 24 FB    BCC 2102  EA=2102(ISQRT2) 
          4 A=18 B=01 X=0000 SP=FF8A H----- ISQRT2   2102: CB 02    ADDB #02  EA=2103 D=02   
          5 A=18 B=03 X=0000 SP=FF8A ------          2104: 10       SBA                      
          6 A=15 B=03 X=0000 SP=FF8A ------          2105: 24 FB    BCC 2102  EA=2102(ISQRT2) 
          7 A=15 B=03 X=0000 SP=FF8A ------ ISQRT2   2102: CB 02    ADDB #02  EA=2103 D=02   
          8 A=15 B=05 X=0000 SP=FF8A ------          2104: 10       SBA                      
          9 A=10 B=05 X=0000 SP=FF8A ------          2105: 24 FB    BCC 2102  EA=2102(ISQRT2) 
         10 A=10 B=05 X=0000 SP=FF8A ------ ISQRT2   2102: CB 02    ADDB #02  EA=2103 D=02   
         11 A=10 B=07 X=0000 SP=FF8A ------          2104: 10       SBA                      
         12 A=09 B=07 X=0000 SP=FF8A ------          2105: 24 FB    BCC 2102  EA=2102(ISQRT2) 
         13 A=09 B=07 X=0000 SP=FF8A ------ ISQRT2   2102: CB 02    ADDB #02  EA=2103 D=02   
         14 A=09 B=09 X=0000 SP=FF8A ------          2104: 10       SBA                      
         15 A=00 B=09 X=0000 SP=FF8A ---Z--          2105: 24 FB    BCC 2102  EA=2102(ISQRT2) 
         16 A=00 B=09 X=0000 SP=FF8A ---Z-- ISQRT2   2102: CB 02    ADDB #02  EA=2103 D=02   
         17 A=00 B=0B X=0000 SP=FF8A ------          2104: 10       SBA                      
         18 A=F5 B=0B X=0000 SP=FF8A --N--C          2105: 24 FB    BCC 2102  EA=2102(ISQRT2) 
         19 A=F5 B=0B X=0000 SP=FF8A --N--C          2107: 17       TBA                      
         20 A=0B B=0B X=0000 SP=FF8A -----C          2108: 44       LSRA                     
         21 A=05 B=0B X=0000 SP=FF8A ----VC          2109: 39       RTS                      

>        22 A=05 B=0B X=0000 SP=FF8A ----VC          210A: 00       Huh?                     

Type 'help'
% <b>r</b>		<i>Show answer in A...</i>
PC=210A <b>A=05</b> B=0B X=0000 SP=FF8A CC=C3
% 
</code></pre>
