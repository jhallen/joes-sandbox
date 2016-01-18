
# EXORterm 155 or EXORterm 200 quick reference

Here is some information about Motorola's EXORterm 155 and EXORterm 200.

Here is a picture of an EXORterm: [http://digitaltmuseum.no/011015371771/motorola-datamaskin-exorciser-exorterm-exordisk](http://digitaltmuseum.no/011015371771/motorola-datamaskin-exorciser-exorterm-exordisk)

The EXORterm 155 is a CRT-based display console designed to work with the
EXORciser:

	* 12 inch screen
	* 80 column x 24 line display
	* 128 character font
		* 96 ASCII characters
		* 24 lower case Greek characters
		* 2 uppercase Greek characers and 6 special characters
	* RS-232C and 20/60 mA current loop interface
	* Up to 9600 BAUD
	* Support for 193 and 202 modem controls (support for half duplex)

The EXORterm 155 does not support the DEC VT100 / ANSI control sequences. 
Instead it has its own control language.  It supports "page mode", which
many terminals of its era had (inspired by IBM's 3270).

The EXORterm 200 is just like the EXORterm 155 but it has a small EXORciser
development system built in.  It has a card cage in the back which accepts
EXORciser 86-pin boards.

## Keys

These keys are on the keyboard (in addition to the usual ASCII keys):

Key | Code or Function
----|-----
ALL CAPS (with light) | (Force all caps)
AUTO LF (with light) | (Automatically line feed<br>after carriage return)
ON LINE (with light) | (Switch between on line<br>and local modes)
PAGE MODE (with light) | (Switch between page and<br>scroll modes)
LF | 0x0A
ESC | 0x1B
BREAK | 300 ms "space" condition
RETURN | 0x0D
HOME (shift CLEAR) * | CLEAR=0xD8, HOME=0xC0.  Allowed in scroll mode.
PAGE SEND (shift LINE SEND) | Ignored in scroll mode.<br>Send Page or Line in<br>page or protected mode.
INS CHAR * | 0xD0
DEL CHAR * | 0xD1
SET TABS * | 0xDC
Back tab * | 0xDB
Forward tab | 0xDA
DEL LINE * | 0xD7
INS LINE * | 0xD6
PAGE ERASE (shift LINE ERASE) * | PAGE ERASE=0xD4, LINE ERASE=0xD5.  Allowed in scroll mode.
Left arrow | 0x08
Right arrow | 0x0C
Up arrow | 0x0B
Down arrow | 0x0A
F1 - F12 | 0xA0 - 0xAB
Shift F1 - F12 | 0xB0 - 0xBB

(*) I'm fairly sure that these keys never send codes to the application. 
They edit the terminal screen only (and then only in page or protected mode,
except where noted).

## Definitions

Page rollover: it means that when you try to move the cursor down when it's
on the last line, it moves to the first line.  Also, if you try to move the
cursor up while it's on the first line, it moves to the last line.

Line and page rollover: it means when you try to move the cursor right when
it's on the last column, it moves to the first column of the next line.  If
the cursor was on the last line, it's moved to the first line.  Also, if you
try to move the cursor left while it's on the first column, it's moved to
the last column of the previous line.  If the cursor was on the first line,
it's moved to the last line.

Cursor position codes:

   line/column 0: space / 0x20
   line/column 1: ! / 0x21
   line/column 2: " / 0x22
     ...
   line/column 79: o / 0x6F

Some commands require both a line and column number.  For these commands the
line number is provided first:

_command_ _line_ _column_

Scroll mode: This is the normal mode of the terminal and it operates as you
would expect: keyboard codes are sent directly to the application and the
terminal scrolls the screen when it receives linefeed when the cursor is on
the last line.  There are very few screen editing commands available in this
mode.  There is no page or line rollover.

Page mode: There is page and line rollover.  There is no scrolling (linefeed
acts as cursor down with rollover).  The terminal has built-in editing so
that the keyboard can be used to edit the contents of the screen.  You hit
the _page send_ or _line send_ keys to transmit the screen or line contents
to the application (also the application can send commands for _page send_
and _line send_).  Many editing commands become available.

Protect mode: This is just like page mode, except that certain fields can be
set up to be protected from editing.  The cursor is not allowed to appear in
protected fields (it jumps over them).  FAC (field attribute codes / AKA
"magic cookies") are used to delineate the protected fields.

## FACs / Field attribute codes / "Magic cookies"

Screen locations can have magic cookies which set or reset display
attributes.  When a location has a magic cookie, it appears blank on the
screen.  If you overwrite the location with the magic cookie with a normal
character, the attribute settings are deleted.  On the other hand, the codes
to set attributes do not quite act like characters: they do overwrite the
character in the location (changing it to a space), but they do not cause
the cursor to move.  Also several attributes can be placed into the same
character location.  Once the attributes have been set, move off the
location by issuing a cursor movement command, not by sending a space.

Attributes do not cross lines: they are reset to their defaults after each
line.

	0xE0 or ESC `		set blink
	0xE1 or ESC a		reset blink
	0xE2 or ESC b		set inverse
	0xE3 or ESC c		reset inverse
	0xE4 or ESC d		set dim
	0xE5 or ESC e		reset dim
	0xE6 or ESC f		set underline
	0xE7 or ESC g		reset underline
	0xE8 or ESC h		set non-display
	0xE9 or ESC i		reset non-display
	0xEA or ESC j		set field protect
	0xEB or ESC k		reset field protect

## CRT Editor Mode

This mode is for support of the EXORmacs M68000 CRT Editor, the M6800
EXORciser resident editor does not need it.

If the CRT editor mode is enabled, the CRT enters a pseudo half-duplex mode
requiring no echo from the host, and performs certain text editing
functions.  Optionally, the terminal notifies the host system when the
cursor has been moved off the current line.  If the current line has
changes, the new data is always transmitted as part of a message packet.

All data and command message packets sent by the terminal cause the keyboard
to be locked.  The host needs to send an unlock command.  BREAK key will
also unlock.

Key|		Code|	CRT Action                     |Message to host
---|----------------|----------------------------------|----------------
HOME|		0xC0|	Normal|				Yes
Up|		0xC1|	Normal|				Yes
Down|		0xC2|	Normal|				Yes
Left|		0xC3|	Cursor stays on line, no wrap|	No
Right|		0xC4|	Cursor stays on line, no wrap|	No
Set scroll mode|0xC8|	Ignored|
Set page mode|	0xC7|	Ignored|
Tab|		0xDA|	Cursor stays on line|		No
Back tab|	0xDB|	Cursor stays on line|		No
Set tabs|	0xDC|	Next char becomes tab char.|	No
ASCII CR|	0x0D|	Current line sent if changed, CR-LF is performed.|Yes, with 0xDF term
Insert char|	0xD0|	Insert space in line|		No
Delete char|	0xD1|	Delete in line|			No
Page erase|	0xD4|	Ignored|			No
Line erase|	0xD5|	Line blank, cursor at left edge|No
Line insert|	0xD6|	Normal|				Yes (1)
Line delete|	0xD7|	Normal|				Yes (1)
Clear/Home|	0xD8|	Only home function|		Yes (1)
Send page|	0xD9|	Ignored|			No
Send line|	0xDF|	Ignored|			No
Func keys|	0xA0 - 0xBF|	No CRT action|		Yes (1)
Ctrl-W|		0x17|	No CRT action|			Yes (1)
Delete last|	0x7F|	move left and blank (backspace)|No

  (1) Sent only if in "return command mode" or if data has been changed on
line

  (2) All other ASCII commands and escape sequences ignored

  (3) If data is being entered on the last line and operator enters a
character into the last character position and hit RETURN or LF, all lines
scrolled up, bottom line blanked, and message sent.  The terminating char
with be 0xD9.

Enable CRT editor more: 0x8E followed by ASCII char for line repeat
function.  If no repeat function desired, use 0xFF.

Disable CRT editor mode: 0x8F.  Back to scroll mode.

Return command mode: 0x90.  Packet sent each time cursor moved from one line
to another.

Non-return command mode: 0x91.  Packet sent only when cursor moves off a
line that has been changed or when Return or LineFeed issued while cursor is
on the last line of the display.

Packet format:

  Offset|  Code| Name
--------|------|------
  0|        0xDD|   Start of data
  1|        0xFF|   Filler byte
  2|        0xFF|   Change flag: FF for no change or number of lines changed.
  3|            |   Terminating character: function which caused packet to be sent
  4|            |   Cursor line
  5|            |   Cursor column
  6 - col. M|   |   characters from line which changed
          | 0xDE|   End of data

# Table of command codes

Command codes for normal scroll / page / protect mode

Function | Send | Receive 
--------|----------|--------
Disable keyboard | 0xD3 (or ESC S) | 
Enable keyboard | 0xD2 (or ESC R) | 
Set page mode | 0xC7 (or ESC G).  Page mode: Ignored.<br>Scroll or Protect mode: Change to page mode and<br>move cursor to virtual home. | 
Set scroll mode | 0xC8 (or ESC H).  Scroll mode: Ignored.<br>Page or Protect mode: Change to scroll mode and<br>move cursor to virtual home. | 
Set protect mode | 0xCD (or ESC M).  Change to protect mode<br>and home cursor (find first unprotected<br>columnon first virtual line). |
Set transparent mode | 0xEC (or ESC 1) | 
Reset transparent mode | 0xED (or ESC m) | 
Set video invert | 0xEE (or ESC n) | 
Reset video invert | 0xEF (or ESC o) | 
Set display special | 0xFC (or ESC :).  Codes between 0x00<br>and 0x1F treated as greek characters. |
Reset display special | 0xFD (or ESC }) | 
Reset terminal | 0xF1 (or ESC q).  Clear screen, scroll mode,<br>home cursor. | 
Home | 0xC0 (or ESC @).  Home cursor to virtual area.<br>In protect mode, find first unprotected column<br>of first virtual line.  If none, cursor lands in<br>first column of first line. |
Cursor up | 0xC1 (0x0B in scroll mode) or ESC A.<br>Cursor rolls over virtual area in protect or page mode. | 
Cursor down | 0xC2 (0x0A in scroll mode) or ESC B.<br>Cursor rolls over virtual area in protect or page mode.| 
Cursor left | 0xC3 (0x08 in scroll mode) or ESC C.<br>Cursor rolls over virtual area in protect or page mode. | 
Cursor right | 0xC4 (0x0C in scroll mode) or ESC D.<br>Cursor rolls over virtual area in protect or page mode. | 
Load cursor | 0xC5 (or ESC E) _line_ _col_ |
Read cursor | 0xC6 (or ESC F) | 0xDD _line_ _col_ 0xDE
Auto linefeed | 0xF7 |
Forward tab | 0xDA (or ESC Z).  Cursor moves to next<br>tab stop (with page rollover in Page mode).  In protect<br>mode, moves to next unprotected field (with<br>rollover). | 
Back tab | 0xDB (or ESC [). Cursor moves to previous<br>tab stop (with page rollover in Page mode).  In protect<br> mode, moves to start of previous field (with<br>rollover).|
set tabs | 0xDC (or ESC \\) _stops_ 0x0A.   _stops_<br>include spaces, tabs (0x09) or cancel (0x18) to clear<br>or set tabs stops at various positions.<br>0xDC 0x0A cancels all tabs.<br>0xDC 0x09 0x0A resets them to defaults.|
Set top line of virtual area | 0xC9 (or ESC I) _line_.  If cursor is<br> not in the virtual area, move cursor<br>right (with page rollover) until cursor is in the<br>new area. |
Set bottom line of virtual area | 0xCA (or ESC J) _line_.  If cursor is<br> not in the virtual area, move cursor<br>right (with page rollover) until cursor is in the<br>new area.  |
Set left column of virtual area | 0xCB (or ESC K) _col_.  If cursor is<br> not in the virtual area, move cursor<br>right (with page rollover) until cursor is in the<br>new area.  |
Set right column of virtual area | 0xCC (or ESC L) _col_.  If cursor is<br> not in the virtual area, move cursor<br>right (with page rollover) until cursor is in the<br>new area.  |
Delete character | 0xD1 (or ESC Q). Scroll mode: ignored.<br>Page mode: Delete character in virtual line.<br>Protect mode: Delete character in field. |
Page erase | 0xD4 (or ESC T).  Scroll and page mode:  Erase<br>from cursor to end of virtual area.  Protect<br>mode: Erase from cursor to end, but only in unprotected areas.|
Line erase | 0xD5 (or ESC U).  Scroll and page mode:  Erase<br>from cursor to end of virtual line.  Protect<br>mode: Same, but only in unprotected areas. |
Line insert | 0xD6 (or ESC V).  Scroll and protect mode:  Ignored.<br>Page mode: insert blank line and move<br>cursor to left virtual edge. |
Insert character | 0xD0 (or ESC P). Scroll mode: ignored.  <br>Page: insert next character and move to<br>the right with rollover.  Protect: insert next<br>character within unprotected field.|
Line delete | 0xD7 (or ESC W).  Scroll and protect mode: Ignored.<br>Page mode: delete line and move cursor<br>to left virtual edge.|
Clear | 0xD8 (or ESC X).  Scroll and page mode: Erase screen and<br>move cursor to virtual home.<br>Protect mode: erase all unprotected area, move cursor to<br>first unproteted column of first line (or home if none).|
Write absolute | 0xCE (or ESC N) 0xDD _line_ _column_ _text_ 0xDE.<br>Writes to absolute screen location, not virtual. |
Read absolute | 0xCF (or ESC 0) _start-line_ _start-column_ _end-line_ _end-column_ | 0xDD _data_ 0xDE (with 0xF9 at<br>the end of each line)
Display status on | 0xF2 (or ESC r) |
Display status off | 0xF3 (or ESC s) |
Send line | 0xDF (or ESC \_).  Scroll mode: ignored.  Page mode:<br>entire virtual line sent.  Protect<br>mode: unprotected fields in virtual line sent. |0xDD _data_ 0xDE (end of each protected field marked with 0xF9)
Send page | 0xD9 (or ESC Y).  Scroll mode: ignored.  Page mode:<br>from virtual home to current cursor is sent. |0xDD _data 0xDE (0xF9 at end of each line)

## Various codes

Function | Send | Receive 
--------|----------|--------
Start data | 0xDD (or ESC ]) |
End data | 0xDE (or ESC ^) |
End data indicator | 0xF9 (or ESC y) |
Enable load | 0xFA (or ESC z) |
Disable load | 0xFB (or ESC \{) |
End of line FAC | 0xFE (or ESC ~) |
Break | 0x8C |

## Notes

The set scroll/page/protect commands reset the virtual window back to the
size of the full screen.
