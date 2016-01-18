
<h2>Summary of EXORdisk II/III Operating System User's Guide, Second edition, 1979
(First edition was 1978), which describes:</h2>

<p>	M6800 Diskette Operating System (MDOS) or
	M6809 Diskette Operating System (MDOS09).
</p>

<h2>Diskette operating system</h2>
<h3>Boot up</h3>
<p>When you power up the EXORciser, the M6800 runs the EXbug
monitor ROM and the EXbug prompt is displayed on the terminal.  To boot into
MDOS, you must jump to the diskette controller ROM by typing:</p>

<p><code><b>MAID<br>E800;G</b></code></p>

<h3>Drives</h3>
<p>There can be up to four drives connected to the system, numbered :0 - :3.</p>
<h3>Sign-on Message and command line prompt look like this:</h3>
<p><code>MDOS 3.00 (M6800)<br>
=</code></p>
<h3>Upper case</h3>
<p>Althought you can edit files with both upper and lower case
letters, commands must be in all uppercase.  Lowercase letters are not
allowed in file names.</p>
<h3>3. Command format:</h3>
<p>NAME1 NAME2 [,] NAME3 [,] NAME4 ... [ ; OPTIONS ]</p>
<p>Each name is a file name.  NAME1 is the command and the others are
file name arguments for the command.  The comma separators between the
arguments are optional.  The semicolon before the options is mandatory.</p>
<h4>File names</h4>
<p>File names in MDOS are up to eight characters with a two
character extension.  The first character of both the name and extension
must be a letter.  The others can be letters or numbers.  Only uppercase
letters are valid for filenames.</p>
<h4>Drive specifier</h4>
<p>Each name can be post-fixed with a drive specifier as shown
in the following examples:</p>
<p>FOO:0<br>BAR:1<br>FRED.SA:2<br></p>
<p>Default drive is :0.  There is no concept of "current drive" as with MS-DOS.</p>
<h4>Special keys</h4>
<p>DEL deletes the last character.</p>
<p>Ctrl-W pauses the output.  Hit any other key to restart it.</p>
<p>Ctrl-X cancels the line.</p>
<p>Ctrl-D redisplays the line.</p>
<p>Break aborts a command in progress.</p>
<h4>Device names</h4>
<p>Devices may sometimes be used in place of file names:</p>
<dl>
<dt>#LP<dd>Line printer
<dt>#CN<dd>Console output
<dt>#CP<dd>Console punch
<dt>#UD<dd>User-defined device (for COPY command)
<dt>#CR<dd>Console reader
<dt>#HR<dd>EXORtape
</dl>
<h2>Command reference</h2>
<h3>DIR commmand - get a directory listing</h3>

<p>Summary: DIR [&lt;name&gt;] [;&lt;options&gt;]</p>
<dl><dt>Options:<dt>
<dl>
<dt>L<dd>Direct output to printer
<dt>S<dd>Include hidden system files in listing
<dt>E<dd>Display entire directory entry
<dt>A<dd>Display allocation information for each entry
</dl></dl>
<p>Use DIR :1 to get a listing from drive 1.</p>
<p>Wildcards are allowed, so: DIR AB*.SA lists all .SA files beginning
with AB.  The wildcard character only works in the last position of the name
or extension.</p>
<h3>BACKUP command - copy entire diskette</h3>
<p>Summary: BACKUP [[&lt;source-drive&gt;] &lt;dest-drive&gt;] [; &lt;options&gt;]</p>
<dl><dt>Options:
<dl><dt>none<dd>Copy all allocated space to destination
<dt>R<dd>Reorganize diskette to eliminate fragmentation
<dt>A<dd>Copy selective files to the destination
<dt>V<dd>Verify: compare source and dest diskettes.
<dt>C<dd>Continue if read/write errors occur
<dt>D<dd>Continue if deleted data mark errors occur
<dt>I<dd>Change ID sector during copy
<dt>L<dd>Use line printer for bulk of message printing
<dt>N<dd>Suppress printing of file names
<dt>S<dd>Suppress printing of mis-compare byte offsets
<dt>U<dd>Include unallocated space during copy
<dt>Y<dd>If duplicate exists, delete old copy new
<dt>Z<dd>If duplicate exists, keep old ignore new
</dl></dl>

<h3>BINEX command: convert memory image file to S-records file</h3>
<p>Summary: BINEX &lt;name1&gt; [&lt;name2&gt;]</p>
<h3>BLOKEDIT command: stream editor</h3>
<p>Summary: BLOKEDIT &lt;command-file&gt; &lt;new-file&gt;</p>

<dl><dt>The command file looks like this:
<dl><dt>* Comment line<dd>
<dt>FILENAME:1<dd>Open a source file
<dt>5,75-80<dd>Copy lines from source file to new-file
<dt>" comment<dd>Begin quoted matter: the following lines are copied into the new file.
<dt>copy<dd>
<dt>to<dd>
<dt>new<dd>
<dt>file<dd>
<dt>" comment<dd>End of quoted matter
</dl></dl>

<h3>CHAIN command: batch files</h3>
<p>Summary: CHAIN &lt;command-file&gt; [;&lt;arg1&gt;,&lt;arg2&gt;,arg3&gt;,...]</p>
<p>Also: CHAIN N* (restart last batch file with line after one
which caused abort)</p>
<p>Also; CHAIN * (restart last batch file with line which caused abort)</p>
<p>Arguments have this syntax: &lt;tag&gt;[%&lt;value&gt;%]</p>
<dl><dt>Command file format:
<dl><dt>/* Comment line<dd>First pass comments
<dt>/IFS &lt;expression&gt;<dd>If set (comma means OR)
<dt>/IFC &lt;expression&gt;<dd>If not set
<dt>/ELSE<dd>Else
<dt>/XIF<dd>End if
<dt>/ABORT<dd>Abort batch file
<dt>@* Comment line<dd>Second pass comments
<dt>@. Comment<dd>Breakpoint
<dt>@SET 800<dd>Set error status word: ignore errors
<dt>@SET 0<dd>Clear error status word: abort on errors (default)
<dt>@TST &lt;test-expression&gt;<dd>Test command error status word, skip next line if true
<dt>@JMP &lt;label&gt;<dd>Jump to label
<dt>@LBL &lt;label&gt;<dd>Define a label
<dt>@CMD ON<dd>Echo commands
<dt>@CMD OFF<dd>Echo off
</dl></dl>

<dl><dt>&lt;expression&gt; is:
<dl><dt>&lt;tag&gt;<dd>True if tag exists
<dt>&lt;tag&gt; . &lt;tag&gt;<dd>True if both tags exist
<dt>&lt;expression&gt; &lt;expression&gt;<dd>True if either is true.
</dl></dl>

<dl><dt>&lt;test-expression&gt; is:
<dl><dt>[,&lt;j&gt;] &lt;condition&gt; [,&lt;value&gt;]

<dl><dt>&lt;j&gt; is:
<dl><dt>W<dd>whole word: bits F-0
<dt>T<dd>error type: bits 7-0
<dt>M<dd>error mask: bits B-8
<dt>S<dd>error status: bits F-C
</dl></dl>

<dl><dt>&lt;condition&gt; is:
<dl><dt>EQ
<dt>NE
<dt>GT
<dt>LT
<dt>GE
<dt>LE
<dt>BS<dd>bit set
<dt>BC<dd>bit clear
</dl></dl>

<dl><dt>&lt;value&gt; is hexadecimal number
</dl>
</dl>
</dl>

<h3>COPY command - copy files</h3>
<p>COPY &lt;name1&gt;[, &lt;name2&gt;] [; &lt;options&gt;]</p>
<dl><dt>Options:
<dl><dt>B<dd>Copy and verify
<dt>C<dd>Binary record conversion for copying to non diskette device
<dt>D=&lt;name3&gt;<dd>Load user's device driver
<dt>L<dd>List error on printer
<dt>M<dd>Goto debug monitor after loading driver
<dt>N<dd>Use non-file format mode for non-diskette device.
<dt>V<dd>Verify files (no copy)
<dt>W<dd>Force overwrite of existing files
</dl></dl>

<h3>DEL command - delete files</h3>

<p>DEL &lt;name1&gt; [,&lt;name2&gt;...] [;&lt;options&gt;]</p>

<dl><dt>Options:
<dl><dt>S<dd>Allow system files
<dt>Y<dd>Do not ask for confirmation to delete
</dl></dl>

<h3>DOSGEN command - install system on formatted floppy</h3>
<p>DOSGEN [:&lt;drive&gt;] [;&lt;options&gt;]</p>
<dl><dt>Options:
<dl><dt>T<dd>Perform read/write test
<dt>U<dd>Generate minimum system (a user disk)
</dl></dl>

<h3>DUMP command - sector editor</h3>

<p>DUMP [&lt;name&gt;]</p>

<dl><dt>Commands:
<dl><dt>Q<dd>Quit
<dt>U i<dd>Select drive
<dt>O &lt;name&gt;<dd>Open a file
<dt>C<dd>Close file
<dt>S<dd>Show buffer
<dt>SB<dd>Show CAT
<dt>S [m[,n]]<dd>Show sector(s)
<dt>SD [m[,n]]<dd>Show directory sectors
<dt>SC [m[,n]]<dd>Show clusters
<dt>LB<dd>Print CAT
<dt>L [m[,n]]<dd>Print sector(s)
<dt>LD [m[,n]]<dd>Print directory sectors.
<dt>LC [m,n]]<dd>Print clusters
<dt>RB<dd>Read CAT
<dt>RD m<dd>Read directory sector
<dt>R m<dd>Read sector
<dt>W<dd>Write buffer back to disk
<dt>WB<dd>Write buffer to cat
<dt>WD m<dd>Write buffer to directory sector
<dt>W m<dd>Write buffer to sector m
<dt>F c<dd>Fill buffer
<dt>F "a"<dd>Fill buffer
<dt>hh/<dd>examine/change buffer
</dl></dl>

<h3>ECHO command - echo console output to printer</h3>
<p>ECHO [;&lt;options&gt;]</p>

<dl><dt>Options:
<dl><dt>N<dd>Turn echo off
</dl></dl>

<h3>EMCOPY command - copy files from EDOS 2 system disk to MDOS disk</h3>
<p>EDOS is some earlier operating system with 5 character filenames.</p>
<h3>EXBIN command - convert S record to binary image</h3>
<p>EXBIN &lt;name1&gt;[,&lt;name2&gt;] [;HHHH]</p>
<dl><dt>Options:
<dl><dt>HHHH<dd>Gives starting execution address
</dl></dl>

<h3>FORMAT command - format diskette</h3>
<p>FORMAT [:&lt;drive&gt;]</p>


<h3>FREE command - show number of free sectors</h3>
<p>FREE [:&lt;drive&gt;] [;&lt;options&gt;]
<dl><dt>Options:
<dl><dt>L<dd>Direct output to line printer
</dl></dl>

<h3>LIST command - show file on console or printer</h3>
<p>Summary: LIST &lt;name&gt;[, [&lt;start&gt;][, &lt;end&gt;]] [;&lt;options&gt;]</p>

<dl><dt>Options:
<dl><dt>L<dd>Direct output to line printer
<dt>H<dd>Get heading information from console
<dt>N<dd>Display line numbers for each line
<dt>F[ccc].[pp]<dd>Use a non-standard page format.  ccc is no. columns (80 is default).  pp is no. rows (66 is default).
</dl></dl>


<dl><dt>&lt;start&gt; and &lt;end&gt; have this format:
<dl><dt>Lnnnn<dd>Start on line nnnnn
<dt>Smmm<dd>Start on sector mmm
</dl></dl>

<h3>LOAD command - load image file into memory</h3>

<p>Summary: LOAD [&lt;name&gt;] [;&lt;options&gt;]</p>
<p>Default extension is .LO</p>


<dl><dt>Options:
<dl><dt>&lt;none&gt;<dd>Load program into contiguous memory above MDOS, leave MDOS SWI handler intact.
<dt>U<dd>Load program into user memory map of an EXORcister II with a dual memory map configuration.
<dt>V<dd>Allow programs to load over MDOS or anywhere else in memory (disables MDOS SWI).
<dt>G<dd>Execute program after loading (otherwise goes to EXbug monitor).
<dt>(&lt;string&gt;)<dd>Initialize MDOS command line buffer with the character string.
</dl></dl>

<h3>MERGE command - merge files together</h3>

<p>Summary: MERGE &lt;name1&gt;[,&lt;name2&gt;,...],&lt;dname&gt;[;&lt;options&gt;]</p>

<p>If &lt;dname&gt; is the same as &lt;name1&gt;, then &lt;name2&gt;.. are appended to &lt;name1&gt;.</p>

<dl><dt>Options:
<dl><dt>W<dd>Use automatic overwrite if dname already exists.
<dt>&lt;addr&gt;<dd>Use hex address as starting execution address of destination file.
</dl></dl>
<p>Merges text files similar to UNIX cat.</p>
<p>Merges binary image files with different addresses
into a single file containing all files at their original addresses.</p>

<h3>NAME command - rename files</h3>
<p>Summary: NAME &lt;name1&gt;[,&lt;name2&gt;] [;&lt;options&gt;]</p>

<dl><dt>Options:
<dl><dt>D<dd>Set delete protection
<dt>W<dd>Set write protection
<dt>X<dd>Remove protection
<dt>S<dd>Set system attribute
<dt>N<dd>Remove system attribute
</dl></dl>


<h3>PATCH command - modify memory image files</h3>
<p>Summary: PATCH &lt;name&gt;</p>

<dl><dt>Options:<dl>
<dt>Q<dd>quit
<dt>O<dd>display current offset
<dt>O m<dd>set offset to m
<dt>O m,n<dd>set offset to m*n
<dt>&lt;cr&gt;<dd>Show current location
<dt>L<dd>display lowest address
<dt>H<dd>display highest address
<dt>m[,n]R<dd>calculate relative address
<dt>cI<dd>dis-assemble opcode
<dt>[m][,n]M<dd>set search mask
<dt>m,nS<dd>search for byte
<dt>m,nW<dd>search for word
<dt>N<dd>search for non-matching byte
<dt>X<dd>search for non-matching word
<dt>m,nP<dd>hex dump
<dt>[m[,n]]G<dd>set/display execution address
<dt>m[,n]/&lt;string&gt;<dd>examine/change increment mode
<dt>m[,n]\&lt;string&gt;<dd>examine/change decrement mode			
</dl></dl>

<h3>REPAIR command - like chkdsk</h3>
<p>Summary: REPAIR [:&lt;drive&gt;]</p>

<h3>ROLLOUT command - save memory to a file</h3>
<p>Summary: ROLLOUT [&lt;name&gt;] [;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>U<dd>Rollout from user memory map of EXOR II
<dt>&lt;none&gt;<dd>Write memory to file
<dt>V<dd>Write memory to scratch diskette (in drive 1): This copies ROLLOUT code to an address nnnn you specify at the prompt and then returns to MDOS. Next load your file into memory with LOAD. Finally, execute ROLLOUT command with nnnn;G from EXbug.
<dt>D<dd>Copy scratch diskette to a file
</dl></dl>

<h3>ASM command - assembler</h3>
<p>Summary: ASM &lt;name&gt; [;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>G<dd>Print all generated code for FCB, FDB, and FCC
<dt>-G<dd>Do not print all generated code for FCB, FDB and FCC
<dt>L<dd>Print listing on printer
<dt>-L<dd>Do not print listing
<dt>L=#CN,<dd>Print listing on console
<dt>O=&lt;name&gt;,<dd>Create object file with given name
<dt>S<dd>Print symbol table
<dt>-S<dd>Don't print symbol table
</dl></dl>

<h3>ASM1000 command - M141000 cross assembler</h3>
<p>Summary: ASM1000 &lt;name1&gt;[,&lt;name2&gt;,...][;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>C<dd>Print macro calls
<dt>-C<dd>Don't print macro calls
<dt>D<dd>Print macro definitions
<dt>-D<dd>Don't print macro definitions
<dt>E<dd>Print macro expansions
<dt>-E<dd>Don't print macro expansions
<dt>G<dd>Print generated code from OPLA
<dt>-G<dd>Don't generate code from OPLA
<dt>H<dd>Input initial heading from console
<dt>-H<dd>Don't get heading from console
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>L=#CN,<dd>Print source listing to console
<dt>L=&lt;name&gt;,<dd>Print source listing to file
<dt>M<dd>Print errors only on line printer
<dt>-M<dd>Print errors on console and printer
<dt>N=ddd,<dd>Set printer line length to ddd
<dt>O=&lt;name&gt;,<dd>Save object file name
<dt>P=dd,<dd>Set number of lines per page
<dt>S<dd>Print symbol table
<dt>-S<dd>Don't print symbol table
<dt>T<dd>Print opcode usage statistics table
<dt>-T<dd>Don't print table
<dt>U<dd>Print unassembled code between directives
<dt>-U<dd>Don't print unassembled code
<dt>X<dd>Print cross-reference table
<dt>-X<dd>Don't print cross-reference table
</dl></dl>

<h3>ASM3870 command - M3870 cross assembler</h3>
<p>Summary: ASM3870 &lt;name1&gt;[,&lt;name2&gt;...][;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>C<dd>Print macro calls
<dt>-C<dd>Don't print macro calls
<dt>D<dd>Print macro definitions
<dt>-D<dd>Don't print macro definitions
<dt>E<dd>Print macro expansions
<dt>-E<dd>Don't print macro expansions
<dt>G<dd>Print generated code from OPLA
<dt>-G<dd>Don't generate code from OPLA
<dt>H<dd>Input initial heading from console
<dt>-H<dd>Don't get heading from console
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>L=#CN,<dd>Print source listing to console
<dt>L=&lt;name&gt;,<dd>Print source listing to file
<dt>M<dd>Print errors only on line printer
<dt>-M<dd>Print errors on console and printer
<dt>N=ddd,<dd>Set printer line length to ddd
<dt>O=&lt;name&gt;,<dd>Save object file name
<dt>P=dd,<dd>Set number of lines per page
<dt>S<dd>Print symbol table
<dt>-S<dd>Don't print symbol table
<dt>U<dd>Print unassembled code between directives
<dt>-U<dd>Don't print unassembled code
<dt>X<dd>Print cross-reference table
<dt>-X<dd>Don't print cross-reference table
</dl></dl>

<h3>BASIC command - basic interpreter</h3>
<p>Summary: BASIC &lt;name1&gt;[,&lt;name2&gt;]</p>

<h3>E command - CRT text editor</h3>
<p>Summary: E &lt;name&gt;[;&lt;options&gt;]</p>
<p>Used for EXORterm 200/220 or EXORterm 150</p>

<h3>EDIT command - text editor</h3>
<p>Summary: EDIT &lt;name1&gt;[,&lt;name2&gt;]</p>

<h3>EM3870 command - M3870 emulator</h3>
<p>Summary: EM3870</p>

<h3>FORM1000 command - M141000 Object file conversion</h3>
<p>Summary: FORM1000 &lt;name1&gt;[,&lt;name2&gt;]</p>

<h3>FORT command - Fortran compiler</h3>
<p>Summary: FORT &lt;name1&gt;[,&lt;name2&gt;,...] [;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>H<dd>Input initial heading from console
<dt>-H<dd>Don't get heading from console
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>L=#CN,<dd>Print source listing to console
<dt>L=&lt;name&gt;,<dd>Print source listing to file
<dt>N=ddd,<dd>Set printer line length to ddd
<dt>O=&lt;name&gt;,<dd>Save object file name
<dt>P=dd,<dd>Set number of lines per page
<dt>S<dd>Print symbol table
<dt>-S<dd>Don't print symbol table
<dt>X<dd>Print cross-reference table
<dt>-X<dd>Don't print cross-reference table
</dl></dl>

<h3>MASM command - MACE cross assembler</h3>
<p>Summary: MASM &lt;name1&gt;[,&lt;name2&gt;...][;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>D=&lt;name&gt;,<dd>Name to use for build definition table file
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>L=#CN,<dd>Print source listing to console
<dt>L=&lt;name&gt;,<dd>Print source listing to file
<dt>M<dd>Print errors only on line printer
<dt>-M<dd>Print errors on console and printer
<dt>N=ddd,<dd>Set printer line length to ddd
<dt>O=&lt;name&gt;,<dd>Save object file name
<dt>P=dd,<dd>Set number of lines per page
<dt>T=&lt;name&gt;,<dd>Name of definitions table file
<dt>-T<dd>Definitions table is in memory
<dt>X<dd>Print cross-reference table
<dt>-X<dd>Don't print cross-reference table
</dl></dl>

<h3>MBUG command - MACE loader and debug module</h3>
<p>Summary: MBUG [&lt;name1&gt;][,&lt;name2&gt;][;&lt;options&gt;]</p>

<h3>MOTEST command - Component tester executive</h3>
<p>Summary: LOAD MOTST;VG</p>

<h3>MPL command - MPL compiler</h3>
<p>Summary: MPL &lt;name1&gt;[,&lt;name2&gt;...][;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>M<dd>Print errors only on line printer
<dt>-M<dd>Print errors on console and printer
<dt>N<dd>Print line numbers
<dt>-N<dd>No line numbers
<dt>O=&lt;name&gt;<dd>Save object file name (give as last option)
<dt>S<dd>Include MPL statements as comment in output file
</dl></dl>

<h3>PPLO/PPHI command - PROM Programmer I</h3>
<p>Summary: LOAD PPHI;VG or</p>
<p>Also: LOAD PPLO;VG</p>

<h3>PROMPROG command - PROM Programmer II/III</h3>
<p>Summary: PROMPROG</p>

<h3>RASM command - Relocatable M6800 Macro Assembler</h3>
<p>Summary: RASM &lt;name1&gt;[,&lt;name2&gt;...][;&lt;options&gt;]</p>

<dl><dt>Options:<dl>
<dt>A<dd>Memory-image object file output
<dt>C<dd>Print macro calls
<dt>D<dd>Print macro definitions
<dt>E<dd>Print macro expansions
<dt>F<dd>Print conditional directives
<dt>G<dd>Print generated code from FCB, FDB, FCC
<dt>H<dd>Input initial heading from console
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>L=#CN,<dd>Print source listing to console
<dt>L=&lt;name&gt;,<dd>Print source listing to file
<dt>M<dd>Print errors only on line printer
<dt>-M<dd>Print errors on console and printer
<dt>N=ddd,<dd>Set printer line length to ddd
<dt>O=&lt;name&gt;,<dd>Save object file name
<dt>P=dd,<dd>Set number of lines per page
<dt>R<dd>Relocatable object file output
<dt>S<dd>Print symbol table
<dt>U<dd>Print unassembled code between directives
<dt>X<dd>Print cross reference table
<dt>Z<dd>Use M6801 mnemonics
</dl></dl>

<h3>RASM09 command - Relocatable M6809 Macro Assembler</h3>
<p>Summary: RASM09 &lt;name1&gt;[,&lt;name2&gt;...][;&lt;options&gt;]</p>

<dl><dt>Options:
<dl><dt>A<dd>Memory-image object file output
<dt>C<dd>Print macro calls
<dt>D<dd>Print macro definitions
<dt>E<dd>Print macro expansions
<dt>F<dd>Print conditional directives
<dt>G<dd>Print generated code from FCB, FDB, FCC
<dt>H<dd>Input initial heading from console
<dt>L<dd>Print source listing on printer
<dt>-L<dd>Don't print source listing
<dt>L=#CN,<dd>Print source listing to console
<dt>L=&lt;name&gt;,<dd>Print source listing to file
<dt>M<dd>Print errors only on line printer
<dt>-M<dd>Print errors on console and printer
<dt>N=ddd,<dd>Set printer line length to ddd
<dt>O=&lt;name&gt;,<dd>Save object file name
<dt>P=dd,<dd>Set number of lines per page
<dt>R<dd>Relocatable object file output
<dt>S<dd>Print symbol table
<dt>U<dd>Print unassembled code between directives
<dt>X<dd>Print cross reference table
</dl></dl>

<h3>RLOAD command - Linking loader</h3>
<p>Summary:  RLOAD</p>

<h3>SIM1000 command - 141000 Simulator</h3>
<p>Summary: SIM1000</p>
