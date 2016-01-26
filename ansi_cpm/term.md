
Kaypro was originally designed to imitate most of the control sequences of a
Lear-Siegler ADM-3A terminal.

Kaypro 2, 4 or 10:

07		ring bell
08		cursor left
0C		cursor right
0A		cursor down
0B		cursor up
17		erase to end of screen
18		erase to end of line
1A		clear screen / home cursor
1E		home cursor

ESC R		insert line
ESC E		delete line
ESC = (row+32) (col+32)		goto

Kaypro 2/84, 2x, 4/84, 4x, 10 and robie also have these:

ESC B 0		inverse start
ESC C 0		inverse stop
ESC B 1		dim start
ESC C 1		dim stop
ESC B 2		blinking start
ESC C 2		blinking stop
ESC B 3		underline start
ESC C 3		underline stop
ESC B 4		cursor on
ESC C 4		cursor off
ESC B 5		video mode on
ESC C 5		video mode off
ESC B 6		save cursor position
ESC C 6		restore cursor position
ESC B 7		status line preservation on
ESC C 7		status line preservation off
ESC * V H 	set pixel
ESC SPACE V H	clear pixel
ESC L V1 H1 V2 H2 	set line
ESC D V1 H1 V2 H2 	delete line
