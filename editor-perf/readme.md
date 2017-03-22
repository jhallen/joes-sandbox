# Text Editor Performance Comparison

This started out as a check on some performance problems which were fixed
for the latest version of JOE (version 4.3), but is interesting in its own
right as a comparison between some text editors.

[Joe's Own Editor - Mercurial Source Repository](https://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/)
 
## The system

Lenovo G570 laptop on AC/mains power

Ubuntu 14.04 LTS 64-bit

Swap = 6 MB

Hitachi HTS545050B9A300 500 GB drive 5400 RPM, 8 MB cache, SATA 3.0 Gb/s

Memory 4 GB (2x 2 GB 1333 MHz DDR3 SODIMMs)

Intel Pentium B970 2.3 GHz Two cores, 64-bit, 2 MB L3 cache

## The Editors

|Editor                                       |Screen|Highlighting|Unicode|Windows* |
|---------------------------------------------|------|------------|-------|------|
|Joe 4.3                                      |Yes   |Yes         |Yes    |Yes   |
|VIM 7.4.52                                   |Yes   |Yes         |Yes    |Yes   |
|gnu-emacs 24.3.1                             |Yes   |Yes         |Yes    |Yes   |
|Notepad++ 6.9.2 (running on Ubuntu in Wine)  |Yes   |Yes         |Yes    |Yes   |
|Jedit 5.1.0                                  |Yes   |Yes         |Yes    |Yes   |
|Sublime Text Build 3114                      |Yes   |Yes         |Yes    |Yes   |
|Atom 1.9.6                                   |Yes   |Yes         |Yes    |Yes   |
|Visual Studio Code 1.4.0                     |Yes   |Yes         |Yes    |Yes   |
|ne, the nice editor 2.5                      |Yes   |Yes         |Yes    |No    |
|mcedit (Midnight Commander) 4.8.11           |Yes   |Yes         |Yes    |No    |
|Nano 2.2.6                                   |Yes   |Yes         |Yes    |No    |
|Gedit 3.10.4                                 |Yes   |Yes         |Yes    |No    |
|Micro 1.01                                   |Yes   |Yes         |Yes    |No    |
|nedit 5.6                                    |Yes   |Yes         |No     |Yes   |
|JED 0.99.19U                                 |Yes   |Yes         |No     |Yes   |
|Mg mg_20110905-1.1_amd64                     |Yes   |No          |No     |Yes   |
|NVI 1.81.6                                   |Yes   |No          |No     |No    |
|Gnu Ed 1.9                                   |No    |No          |No     |No    |

(*) Windows: yes if the editor allows multiple views of the same buffer on
the screen at the same time.

ne uses the syntax highligher code from Joe.

Micro is written in Go.

Jedit is written in Java.

## The files

* hello.c Tiny "hello, world!" program
* longlines.txt Two 120KB lines
* test.xml 5.8 MB XML file
* huge 3 GB file (3M 1K lines)

## Memory used when loading "hello.c" with syntax highlighting enabled

|Editor		|RSS|
|---------------|---|
|ne             |1,396|
|nano		|2,208|
|jed		|3,512|
|mcedit         |3,904|
|joe		|4,772|
|vim		|5,336|
|nedit          |7,036|
|micro          |8,740|
|emacs -nw	|17,060|
|notepad++      |21,092|
|gedit          |26,368|
|emacs		|34,924|
|sublime	|45,800|
|jedit          |86,928|
|atom		|250,404|
|code		|339,512|

RSS is amount of physical memory used in KiB.  If the editor starts multiple
processes, all are included.

## Memory used when loading "hello.c" with no syntax highlighting

|Editor|RSS|
|------|---|
|ed    |680|
|mg    |932|
|ne    |1,204|
|nano  |1,684|
|nvi   |2,224|
|jed   |3,352|
|mcedit|3,880|
|joe   |4,988|
|vim   |5,180|
|nedit |6,940|
|micro |8,792|
|emacs -nw |15,584|
|notepad++      |20,804|
|emacs	  |33,752|
|sublime   |43,444|
|jedit |84,416|
|code|341,116|

## Memory used for loading test.xml with highlighting enabled

|Editor|RSS|
|------|---|
|mcedit|9,604|
|vim   |11,952|
|joe   |11,968|
|ne    |14,228|
|nano  |17,336|
|emacs -nw |23,216|
|nedit|24,332|
|notepad++ |34,752|
|micro     |38,212|
|emacs	  |42,892|
|sublime   |64,608|
|jedit|99,372|
|gedit|132,336|
|code |391,496|

## Memory used for loading test.xml with no highlighting

|Editor|RSS|
|------|---|
|nvi   |2,348|
|ed    |5,368|
|mcedit|9,584|
|joe   |11,496|
|vim   |11,824|
|nedit|12,696|
|mg    |13,420|
|ne    |13,864|
|nano  |14,172|
|jed   |14,356|
|emacs -nw |21,320|
|micro  |26,192|
|notepad++ |34,176|
|emacs	  |39,492|
|gedit |48,460|
|sublime   |63,800|
|jedit|94,876|
|code |383,440|
|atom	  |825,232|

## Time used to load test.xml, jump to end of file and exit

|Editor|Time (seconds)|Comments|
|------|--------------|--------|
|jed      |.048|No highlighting|
|mg	  |.088|No highlighting|
|ed       |.107|No highlighting|
|ne       |.174||
|nvi      |.231|No highlighting|
|joe	  |.347||
|emacs -nw |.437||
|mcedit|.460||
|nano	  |.492||
|emacs	  |.852||
|sublime   |1||
|nedit    |2.213||
|vim	  |4.288||
|jedit    |5.322||
|micro    |8.6||
|notepad++|12.43||
|gedit    |14.929||
|atom	  |18||
|code     |22||

Older versions of JOE had trouble with JSON and XML files.  The issue was
that the context display (the part of the status line which shows the name
of the current code function you're in) used a bad algorithm.

Visual Studio Code jumps to the end of the file quickly, but then takes many
seconds for the highlighting to complete.

Note that time is total accumulated CPU time of all processes started by the
editor.  I skip the "exit the editor" part for editors which are indirectly
launched (sublime, atom, code, notepad++).  For these the CPU time is
determined from "ps" or "top -p" after the operation is complete, but while
the editor is still running.  For editors which are directly launched, I
prefix the shell command with "time".

## Rehighlight test

Time used to load test.xml, split window, jump to end of file in other
window, insert '\<!--' at beginning (so that highlighting of the entire file
changes and appears in the window at the end of file) and then exit.

|Editor|Time (seconds)|
|------|---|
|mcedit|.343|
|ne    |.349|
|joe   |.627|
|nedit |3.389|
|sublime |5|
|emacs -nw |8.036|
|jedit|9.114|
|vim	  |10.01|
|gedit|17.431|
|code |28|
|micro |34.9|
|atom |82|
|notepad++|at least 5 minutes|

I could not figure out how to have two views on the same buffer in
Micro, NE, mcedit or gedit, so instead I inserted the '\<!--' and
then jumped to the end of the buffer.

Micro did not recolor the when I inserted '\<!--'.  It wants to recolor only
if the final '--\>' exists.  In fact Micro did not recolor when I put the
'--\>' at the end of the file either.  Perhaps it gives up recoloring when
the file is too large.  In any case, I notice that Micro is very slow when
you insert characters at the end of the test.xml file.

Jedit did not recolor the other window until after I switched to it and
moved the cursor around a little.

Jed is not in this test because it can not highlight XML.

Atom does not seem to highlight large XML files until you "split down" to
open a second windows onto the file.

Notepad++ was very slow when I had multiple views open of the same large XML
file.

## Simple Search and Replace

Time used to load test.xml, and then execute 100,000 replacements (of "thing"
with "thang"), and then exit.

|Editor|Time (seconds)|
|------|-------|
|joe	| .683|
|ed    |.685 |
|mcedit|1.009 |
|jed    |1.02 |
|ne     |2.67 |
|nvi    |3.44|
|nedit  |4.228|
|vim	| 4.613|
|sublime| 6 |
|jedit|9.152|
|emacs	| 9.354|
|emacs -nw | 9.738|
|notepad++ |31.30|
|gedit|44.016|
|code |72|
|mg	| 467.989|
|micro  | at least 10 minutes|
|nano	| at least 10 minutes|
|atom	| at least 10 minutes|

In emacs, I used ESC %.

mg has a memory leak in its search and replace code.

## Regular Expression Search and Replace

Time used to load test.xml, and then replace the regular expression
"100|200" with "EXACT".

|Editor|Time (seconds)|
|------|-------|
|ne    | .307|
|mcedit|.565|
|joe   | .683|
|nvi   |1.146|
|notepad++|1.70|
|nedit  |4.638|
|vim    |4.647 |
|emacs -nw|4.76 |
|jedit|6.911|
|sublime|9|
|code|24|
|gedit|24.28|
|micro|176.4 |
|nano |185.6 |

In emacs, I used replace-regexp.  It's interesting that this is faster than
query replace.

## Time to load 3 GB file, insert character at start and exit

|Editor|Time (seconds)|
|------|--------------|
|mcedit|8.654|
|joe |43|
|ed|50.08|
|nvi|53|
|sublime|75|
|nedit|Complains "file is too large to edit"|
|notepad++|Complains "file is too big"|
|ne    |Complains "Can't open file (file is too large)."|
|code|Complains "file is very large"|
|jedit|Complains "can not load, negative array size exception"|
|gedit|very slow to load|
|mg  |system hangs|
|vim |system hangs|
|emacs |system hangs: but emacs warns file is "really huge"|
|nano  |system hangs|
|micro |micro crashes|
|atom  |atom crashes: atom warns "may be unresponsive loading very large files"|

JOE, ED and NVI swap large files to disk, so this test is no problem for them. 
JOE's RSS is 65756 KiB when the huge file is loaded.  NVI's is 2088!

MCedit loads the file quickly into its simple gap buffer.  However, the gap
is locked to the cursor and distant cursor motions are slow since all of the
intervening text has to be copied.  Even so, for quick file viewing it's
impressive.  MCedit's RSS is 3 GB when the file is loaded.

I'm amazed that Sublime Text is also able to load a 3 GB file.  When loaded,
Sublime's RSS is 1384944 KiB.  Sublime is nicer than JOE in that it shows a
progress bar while the huge file is loading.

Gedit shows the beginning of the file and a progress bar while the file is
loading.

## Time to reformat paragraph composed of two 120K long lines

|Editor|Time (seconds)|
|------|--------------|
|jed   |.047|
|nedit |.110|
|joe	 |.142|
|emacs -nw|1.811|
|ne      |3.025|
|jedit   |6.242|
|vim	 |29.632|
|mg	 |35.552|
|nano	 |54.502|
|micro   |65*|
|mcedit|crashes|


This was slow in older versions of JOE.

I could not quickly figure out how to reformat a paragraph in the other
editors.

(*) Micro does not have a paragraph reformat capability, but I notice that
it takes 65 seconds to load longlines.txt.
