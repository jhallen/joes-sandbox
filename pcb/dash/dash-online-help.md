## FutureNet Dash Command Reference 

This chapter contains all of the FutureNet Dash commands grouped by type and
in alphanumeric order.

- Symbol commands
    + [.- .> .= .-O .>O -  Add/Replace Pin Stub](#-----o-o----addreplace-pin-stub)
    + [.-AI .-AO .>A .\<A -   Add/Replace IEC/ANSI Pin Stub](#-ai--ao-a-a-----addreplace-iecansi-pin-stub)
    + [.A -  Create IEC/ANSI Symbol Outline](#a----create-iecansi-symbol-outline)
    + [.B -  Create Block Symbol](#b----create-block-symbol)
    + [.C -  Copy Symbol](#c----copy-symbol)
    + [.CLR -  Erase Symbol Definition (Symbol Definition Mode)](#clr----erase-symbol-definition-symbol-definition-mode)
    + [.D -  Delete Pin Stub](#d----delete-pin-stub)
    + [.D -  Delete Target Line (Symbol Definition Mode)](#d----delete-target-line-symbol-definition-mode)
    + [.DCON -  Enable/Disable Display of Direct Connections](#dcon----enabledisable-display-of-direct-connections)
    + [.DEL -  Delete Symbol from Update Library](#del----delete-symbol-from-update-library)
    + [.DIR -  Display Symbol Library Directory](#dir----display-symbol-library-directory)
    + [.DIRPR -  Print Symbol Library Directory](#dirpr----print-symbol-library-directory)
    + [.E -  Erase Symbol](#e----erase-symbol)
    + [.F -  Create Functional Block](#f----create-functional-block)
    + [.G -  Change Symbol Type (Symbol Definition Mode)](#g----change-symbol-type-symbol-definition-mode)
    + [.I -  Enter/Exit Insert Mode (Symbol Definition Mode)](#i----enterexit-insert-mode-symbol-definition-mode)
    + [.K -  Cancel Symbol Tag](#k----cancel-symbol-tag)
    + [.L -  Load Symbol from Library (\*)](#l----load-symbol-from-library-)
    + [.L - Print the Symbol Definition List (Symbol Definition Mode)](#l---print-the-symbol-definition-list-symbol-definition-mode)
    + [.LIB -  Update Symbol Library .NOLIB](#lib----update-symbol-library-nolib)
    + [.M -  Move Symbol](#m----move-symbol)
    + [.PRINT -  Print All Text for Symbol](#print----print-all-text-for-symbol)
    + [.Q -  Exit Symbol Definition Mode (Symbol Definition Mode)](#q----exit-symbol-definition-mode-symbol-definition-mode)
    + [.R -  Rotate a Symbol](#r----rotate-a-symbol)
    + [.RE -  Reflect a Symbol](#re----reflect-a-symbol)
    + [.S -  Enter Symbol Definition Mode (Symbol Definition Mode)](#s----enter-symbol-definition-mode-symbol-definition-mode)
    + [.SAVE -  Save Symbol in Current Update Library](#save----save-symbol-in-current-update-library)
    + [.SBS -  Set Block Symbol Bit (Symbol Definition Mode)](#sbs----set-block-symbol-bit-symbol-definition-mode)
    + [.w,h -  Define Symbol Cell Size](#wh----define-symbol-cell-size)

- Alphanumeric field commands
    + ['0...'7 -  Select Font](#07----select-font)
    + ['A -  Set Attribute for New Alphanumeric Field](#a----set-attribute-for-new-alphanumeric-field)
    + ['B -  Enable/Disable Alphanumeric Field Boundary Display](#b----enabledisable-alphanumeric-field-boundary-display)
    + ['C -  Copy Alphanumeric Field](#c----copy-alphanumeric-field)
    + ['CH A -  Change Attribute for Existing Field](#ch-a----change-attribute-for-existing-field)
    + ['CH F -  Change Font](#ch-f----change-font)
    + ['CH J -  Change Justification](#ch-j----change-justification)
    + ['CH O -  Change Orientation](#ch-o----change-orientation)
    + ['CH P -  Change Printability](#ch-p----change-printability)
    + ['CH R -  Change Reverse Video State of Alphanumeric Field](#ch-r----change-reverse-video-state-of-alphanumeric-field)
    + ['CH V -  Change Visibility of Alphanumeric Field](#ch-v----change-visibility-of-alphanumeric-field)
    + ['D -  Enable/Disable Attribute Display](#d----enabledisable-attribute-display)
    + ['E -  Erase Alphanumeric Field](#e----erase-alphanumeric-field)
    + ['F -  Find Alphanumeric String](#f----find-alphanumeric-string)
    + ['FA -  Find Attribute](#fa----find-attribute)
    + ['I -  Insert Alphanumeric String and Increment Numbering](#i----insert-alphanumeric-string-and-increment-numbering)
    + ['J -  Set Justification](#j----set-justification)
    + ['K -  Cancel Alphanumeric Field Tag](#k----cancel-alphanumeric-field-tag)
    + ['L -  Create, Edit or View Layered Text](#l----create-edit-or-view-layered-text)
    + ['LE -  Erase Layered Text](#le----erase-layered-text)
    + ['LR -  Replace/Insert Layered Text](#lr----replaceinsert-layered-text)
    + ['M -  Move Alphanumeric Field](#m----move-alphanumeric-field)
    + ['NAME -  Assign a Name to an Unused Attribute](#name----assign-a-name-to-an-unused-attribute)
    + ['O -  Set Default Orientation for Alphanumeric Fields](#o----set-default-orientation-for-alphanumeric-fields)
    + ['OVER -  Add/Delete Overbar to/from Alphanumeric Field (Signal Inversion)](#over----adddelete-overbar-tofrom-alphanumeric-field-signal-inversion)
    + ['P -  Set Point of Effect](#p----set-point-of-effect)
    + ['PD -  Reassign a Point of Effect to Its Default Location](#pd----reassign-a-point-of-effect-to-its-default-location)
    + ['PRINT -  Print All Text for Alphanumeric Field](#print----print-all-text-for-alphanumeric-field)
    + ['R -  Replace Alphanumeric String](#r----replace-alphanumeric-string)
    + ['S -  Set Attribute for Existing Field](#s----set-attribute-for-existing-field)
    + ['UNDER -  Add/Delete Underscore to Alphanumeric Field](#under----adddelete-underscore-to-alphanumeric-field)

- Wiring commands
    + [/0.../10 -  Select Line Type](#010----select-line-type)
    + [/AD /AL /AR /AU -  Draw/Erase Arrowhead](#ad-al-ar-au----drawerase-arrowhead)
    + [/C -  Draw Direct Connection Through Symbol](#c----draw-direct-connection-through-symbol)
    + [/D -  Insert/Delete Interconnect Dot](#d----insertdelete-interconnect-dot)
    + [/E -  Erase Line](#e----erase-line)
    + [/EL -  Erase Line Segments](#el----erase-line-segments)
    + [/EN -  Erase Line Network](#en----erase-line-network)
    + [/ES -  Erase Line Segment](#es----erase-line-segment)
    + [/ET -  Erase Temporary Lines](#et----erase-temporary-lines)
    + [/J -  Draw Junction Segment](#j----draw-junction-segment)
    + [/K -  Escape Line Drawing](#k----escape-line-drawing)
    + [/L -  Draw Lines](#l----draw-lines)
    + [/LE -  Draw/Erase Line](#le----drawerase-line)
    + [/P -  Convert Temporary Lines to Permanent](#p----convert-temporary-lines-to-permanent)
    + [/R -  Change Line Routing](#r----change-line-routing)
    + [/V -  Move Vertex](#v----move-vertex)

- Area commands
    + [[C -  Copy Area](#c----copy-area)
    + [[D -  Define Area](#d----define-area)
    + [[ERASE -  Erase Area](#erase----erase-area)
    + [[K -  Cancel Area Definition or Area Tag](#k----cancel-area-definition-or-area-tag)
    + [[LOAD -  Load Area](#load----load-area)
    + [[M -  Move Area](#m----move-area)
    + [[R -  Rotate an Area](#r----rotate-an-area)
    + [[RE -  Reflect an Area](#re----reflect-an-area)
    + [[SAVE -  Save Area](#save----save-area)

- Hierarchy commands
    + [#D -  Move Down in Design Hierarchy](#d----move-down-in-design-hierarchy)
    + [#L and #R -  Move Left or Right in Design Hierarchy](#l-and-r----move-left-or-right-in-design-hierarchy)
    + [#U -  Move Up in Design Hierarchy](#u----move-up-in-design-hierarchy)

- Keys
    + [Arrow Keys -  Move Multiple Display Units](#arrow-keys----move-multiple-display-units)
    + [Arrow Up/Down Keys -  Move Symbol Definition List Up/Down (Symbol Definition Mode)](#arrow-updown-keys----move-symbol-definition-list-updown-symbol-definition-mode)
    + [\<Ctrl>\<Backspace> Keys -  Erase Alphanumeric Data](#ctrlbackspace-keys----erase-alphanumeric-data)
    + [\<Ctrl>\<Home> or \<Ctrl>\<R7> Keys - Move Cursor to Beginning of Alphanumeric Field](#ctrlhome-or-ctrlr7-keys---move-cursor-to-beginning-of-alphanumeric-field)
    + [Esc Key -  Enter/Exit Alphanumeric Mode](#esc-key----enterexit-alphanumeric-mode)
    + [HOME -  Home Cursor](#home----home-cursor)
    + [HOME - (Symbol Definition Mode)](#home---symbol-definition-mode)
    + [\<Ins> and \<R11> Keys -  Insert Alphanumeric Character](#ins-and-r11-keys----insert-alphanumeric-character)
    + [PgUp or R9 Keys - Move to Beginning of Command List PgDn or R15 Keys - Move to End of Command List (Symbol Definition Mode)](#pgup-or-r9-keys---move-to-beginning-of-command-list-pgdn-or-r15-keys---move-to-end-of-command-list-symbol-definition-mode)
    + [Tab, Shift  Tab Keys -  Move Cursor Down/Up a Field](#tab-shift--tab-keys----move-cursor-downup-a-field)

- Regular commands
    + [! -  Enter Operating System Command](#----enter-operating-system-command)
    + [? -  Read Custom Menus](#----read-custom-menus)
    + [A1...D8 -  Move to Coordinates](#a1d8----move-to-coordinates)
    + [Add/Replace Target Line - (Symbol Definition Mode)](#addreplace-target-line---symbol-definition-mode)
    + [AUTO -  Automatic Command Execution](#auto----automatic-command-execution)
    + [AUTOPAN -  Enable/Disable Automatic Panning](#autopan----enabledisable-automatic-panning)
    + [AUTOSAVE -  Save Drawing Automatically](#autosave----save-drawing-automatically)
    + [BLINK -  Enable/Disable Blinking](#blink----enabledisable-blinking)
    + [BX, BY -  Draw Broken Line (Symbol Definition Mode)](#bx-by----draw-broken-line-symbol-definition-mode)
    + [BXD, BYD -  Draw Broken Line in Dot Units (Symbol Definition Mode)](#bxd-byd----draw-broken-line-in-dot-units-symbol-definition-mode)
    + [BXL, BXR -  Draw x to Symbol Cell Boundary with Bubble (Symbol Definition Mode)](#bxl-bxr----draw-x-to-symbol-cell-boundary-with-bubble-symbol-definition-mode)
    + [BYU, BYL -  Draw y to Symbol Cell Boundary with Bubble (Symbol Definition Mode)](#byu-byl----draw-y-to-symbol-cell-boundary-with-bubble-symbol-definition-mode)
    + [CD -  Change Current Directory](#cd----change-current-directory)
    + [CLEAR/ERASE -  Clear Drawing Hierarchy or Erase Current Drawing](#clearerase----clear-drawing-hierarchy-or-erase-current-drawing)
    + [COLOR -  Modify Color Palette](#color----modify-color-palette)
    + [CONNECT -  Enable/Disable Maintenance of Line Connections (Rubberbanding)](#connect----enabledisable-maintenance-of-line-connections-rubberbanding)
    + [CONTEXT -  Restore Drawing Context](#context----restore-drawing-context)
    + [CURSOR -  Set Cursor to Absolute Location](#cursor----set-cursor-to-absolute-location)
    + [DA -  Draw Arc in Display Units (Symbol Definition Mode)](#da----draw-arc-in-display-units-symbol-definition-mode)
    + [DAD -  Draw Arc in Dot Units (Symbol Definition Mode)](#dad----draw-arc-in-dot-units-symbol-definition-mode)
    + [DC -  Draw Circle in Display Units (Symbol Definition Mode)](#dc----draw-circle-in-display-units-symbol-definition-mode)
    + [DCD -  Draw Circle in Dot Units (Symbol Definition Mode)](#dcd----draw-circle-in-dot-units-symbol-definition-mode)
    + [DD -  Draw Dots (Symbol Definition Mode)](#dd----draw-dots-symbol-definition-mode)
    + [DEL -   Delete File](#del-----delete-file)
    + [DIR -  List Directory](#dir----list-directory)
    + [DISPGRPn -  Display Group Setup](#dispgrpn----display-group-setup)
    + [DOS -  Enter DOS Command](#dos----enter-dos-command)
    + [DR -  Draw Rectangle in Display Units (Symbol Definition Mode)](#dr----draw-rectangle-in-display-units-symbol-definition-mode)
    + [DRD -  Draw Rectangle in Dot Units (Symbol Definition Mode)](#drd----draw-rectangle-in-dot-units-symbol-definition-mode)
    + [DX, DY -  Draw Line in Display Units (Symbol Definition Mode)](#dx-dy----draw-line-in-display-units-symbol-definition-mode)
    + [DXD, DYD -  Draw Line in Dot Units (Symbol Definition Mode)](#dxd-dyd----draw-line-in-dot-units-symbol-definition-mode)
    + [DXL, DXR -  Draw x to Symbol Cell Boundary (Symbol Definition Mode)](#dxl-dxr----draw-x-to-symbol-cell-boundary-symbol-definition-mode)
    + [DXS, DYS -  Draw Line Short in Display Units (Symbol Definition Mode)](#dxs-dys----draw-line-short-in-display-units-symbol-definition-mode)
    + [DXY -  Draw Diagonal in Display Units  (Symbol Definition Mode)](#dxy----draw-diagonal-in-display-units--symbol-definition-mode)
    + [DXYD -  Draw Diagonal in Dot Units (Symbol Definition Mode)](#dxyd----draw-diagonal-in-dot-units-symbol-definition-mode)
    + [DYU, DYL -  Draw y to Symbol Cell Boundary  (Symbol Definition Mode)](#dyu-dyl----draw-y-to-symbol-cell-boundary--symbol-definition-mode)
    + [EXEC -  Single-step Command Execution](#exec----single-step-command-execution)
    + [Format](#format)
    + [Format](#format-1)
    + [FILE -  Display Filenames](#file----display-filenames)
    + [fn -  Start FutureNet](#fn----start-futurenet)
    + [FN.CFG - Configuration File](#fncfg---configuration-file)
    + [GRID -  Display Grid/Toggle Grid Snap](#grid----display-gridtoggle-grid-snap)
    + [HELP -  Read Online Command Reference](#help----read-online-command-reference)
    + [HELPFILE -  Open Online Command Reference File](#helpfile----open-online-command-reference-file)
    + [HELPSAVE -  Save Custom Menu or Help Screen](#helpsave----save-custom-menu-or-help-screen)
    + [ID -  Insert Dot Matrix (Symbol Definition Mode)](#id----insert-dot-matrix-symbol-definition-mode)
    + [IG -  Insert Graphics Element (Symbol Definition Mode)](#ig----insert-graphics-element-symbol-definition-mode)
    + [ISxx -  Insert Symbol Element (Symbol Definition Mode)](#isxx----insert-symbol-element-symbol-definition-mode)
    + [KEY -  Assign a Function to a Function Key](#key----assign-a-function-to-a-function-key)
    + [!LB, !MB and !RB -  Emulate a Mouse Button Click](#lb-mb-and-rb----emulate-a-mouse-button-click)
    + [LIB -  Specify Symbol Library for Reference NOLIB](#lib----specify-symbol-library-for-reference-nolib)
    + [LOAD -  Load Drawing](#load----load-drawing)
    + [ls -  List Directory](#ls----list-directory)
    + [Format](#format-2)
    + [MOVEPOEE -  Enable/Disable Point of Effect Movement for Electrical Items](#movepoee----enabledisable-point-of-effect-movement-for-electrical-items)
    + [MOVEPOEG - Enable/Disable Point of Effect Movement for General Items](#movepoeg---enabledisable-point-of-effect-movement-for-general-items)
    + [MX, MY - Move Coordinate in Display Units (Symbol Definition Mode)](#mx-my---move-coordinate-in-display-units-symbol-definition-mode)
    + [MXD, MYD - Move Coordinate in Dot Units (Symbol Definition Mode)](#mxd-myd---move-coordinate-in-dot-units-symbol-definition-mode)
    + [MXS, MYS - Move Short in Display Units (Symbol Definition Mode)](#mxs-mys---move-short-in-display-units-symbol-definition-mode)
    + [MXY -  Move to New Coordinates in Display Units  (Symbol Definition Mode)](#mxy----move-to-new-coordinates-in-display-units--symbol-definition-mode)
    + [MXYA - Move to Absolute x,y Coordinates (Symbol Definition Mode)](#mxya---move-to-absolute-xy-coordinates-symbol-definition-mode)
    + [MXYD -  Move to New Coordinates in Dot Units  (Symbol Definition Mode)](#mxyd----move-to-new-coordinates-in-dot-units--symbol-definition-mode)
    + [N -  Move to Symbol Using Reference Number](#n----move-to-symbol-using-reference-number)
    + [NOTE -  Insert Prompt](#note----insert-prompt)
    + [OVERLAP - Allow Alphanumeric Fields to Overlap Symbol Boundaries](#overlap---allow-alphanumeric-fields-to-overlap-symbol-boundaries)
    + [PALETTE -  Assign FutureNet Colors](#palette----assign-futurenet-colors)
    + [PAN -  Pan to Window Locations](#pan----pan-to-window-locations)
    + [PAUSE -  Change from Automatic to Single-step Execution](#pause----change-from-automatic-to-single-step-execution)
    + [PINSNAP -  Snap Line to Pin](#pinsnap----snap-line-to-pin)
    + [POEDISP -  Display Points of Effect](#poedisp----display-points-of-effect)
    + [POER -  Point of Effect Range](#poer----point-of-effect-range)
    + [PRINT -  Print Drawing](#print----print-drawing)
    + [PRINTOPT -   Set Print Options](#printopt-----set-print-options)
    + [PROFILE -  Display Profile Options](#profile----display-profile-options)
    + [QUIT -  Quit Editing Session](#quit----quit-editing-session)
    + [REFRESH -  Refresh Screen](#refresh----refresh-screen)
    + [RENUM -  Resequence Symbol Reference Numbers](#renum----resequence-symbol-reference-numbers)
    + [rm -   Remove File](#rm-----remove-file)
    + [RS -  Reset to Default Symbol Element Set (Symbol Definition Mode)](#rs----reset-to-default-symbol-element-set-symbol-definition-mode)
    + [SAVE -   Save Drawing](#save-----save-drawing)
    + [SAVEALL -  Save All Changed Drawing Files](#saveall----save-all-changed-drawing-files)
    + [SD -  Select Decrement Mode (Symbol Definition Mode)](#sd----select-decrement-mode-symbol-definition-mode)
    + [SI -  Select Increment Mode  (Symbol Definition Mode)](#si----select-increment-mode--symbol-definition-mode)
    + [SIZE -  Set Drawing Size](#size----set-drawing-size)
    + [SL -  Select Broken Line Type (Symbol Definition Mode)](#sl----select-broken-line-type-symbol-definition-mode)
    + [SN -  Select No Increment/Decrement Mode (Symbol Definition Mode)](#sn----select-no-incrementdecrement-mode-symbol-definition-mode)
    + [ST -  Select Symbol Element Table (Symbol Definition Mode)](#st----select-symbol-element-table-symbol-definition-mode)
    + [STOP -  Stop Command File Run](#stop----stop-command-file-run)
    + [SXY, RXY -  Save/Restore x and y Coordinates  (Symbol Definition Mode)](#sxy-rxy----saverestore-x-and-y-coordinates--symbol-definition-mode)
    + [UNDO/REDO -  Reverse/Restore Commands](#undoredo----reverserestore-commands)
    + [VERSION -  Display Software Version](#version----display-software-version)
    + [VIEW -  View Drawing](#view----view-drawing)
    + [WINDOW -  Set Window Locations](#window----set-window-locations)
    + [ZIN/ZOUT -  Change Zoom Level](#zinzout----change-zoom-level)
    + [ZOOM -  Dynamic Zoom Level](#zoom----dynamic-zoom-level)

### ! -  Enter Operating System Command 

Format 

! [command]

Purpose 

Run an operating system command, or (on PC only) enter a 
DOS shell to enter commands.

Remarks 

The ! command enters the command given, or temporarily 
leaves FutureNet to create a DOS shell if no command is given.

This command can also be used in the Symbol Definition Mode. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: For PC systems, if there isn't enough disk space to 
      operate the DOS shell, you will be returned to 
      FutureNet.  FutureNet requires about 500KB of disk space 
      to start a DOS shell.  (No error message is given when 
      you are returned to FutureNet.) 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: You cannot run FutureNet (fn command) from within a DOS 
      shell called from FutureNet; you can lose files and 
      autologging. 

Related Commands 

DOS


### .- .> .= .-O .>O -  Add/Replace Pin Stub    

Format 

.- | .> | .= | .-O | .>O

Purpose 

Add or replace the specified pin stub at the edge of a symbol 
outline.  The pin stub commands are intended to be used in 
symbols created with the .A, .B and .F commands.

Remarks 

.-                       Creates a pin stub. 

.>                       Creates a pin stub with a clock 
                         designation. 

.=                       Creates a bus pin stub. 

.-O                      Creates a pin stub with an inversion 
                         bubble. 

.>O                      Creates a clock pin stub with an 
                         inversion bubble. 

The graphics cursor must be positioned between the symbol cell 
and the symbol block outline where the pin stub is to be 
drawn.  

ANSI/IEC rules do not allow addition of any pin stubs to the top
or bottom of an ANSI/IEC symbol.  If attempts are made to add
pin stubs to the top or bottom of a symbol created with the .A
command, the following message will appear and the pin stub 
command will be ignored:

          ANSI/IEC design rules prohibit pins on the top or
                 bottom of symbol

The stub is drawn from the symbol cell to the symbol cell 
boundary at the location of the graphics cursor. 

If a stub is already present at the graphics cursor position, 
it is replaced by the specified stub.  

If the cursor is on the corner of the symbol cell or is inside
the symbol cell, then the following message appears and the 
command is ignored:

          Cursor not on symbol outline

If the cursor is not in a symbol boundary, then the command is 
ignored and the system displays 

          Cursor not in symbol 

The error message 

          Block symbol bit not set 

means that the special identifier flag for block symbols is 
not set.  Using the symbol editor on a block symbol will clear 
the flag.  Use the .SBS command inside the symbol editor to 
set the flag. 

The error message 

          Not a block symbol 

means that the symbol was created in such a way that it was 
never intended to have block symbol pin stubs.  You will be 
unable to add pins stubs to this symbol.  The .SBS command 
will be unable to convert this symbol to a block symbol.  

It is possible to exceed the maximum size of symbols for 
versions of FutureNet previous to version 6.10.  The following 
confirmation message will appear if the current pin stub 
addition will cause the symbol to exceed that limit:

    WARNING: Size of symbol exceeds pre-FutureNet 6.1 maximum. 
             	Continue (Y/N)?

Typing 'Y' will accept the pin stub command and the symbol will
be incompatible with pre-FutureNet 6.1 versions.  The warning
message will not appear again.  Drawings containing this symbol
will cause unpredictable problems with pre-FutureNet 6.1 versions,
including possible crashing the system.  Typing 'N' will abandon
the current pin stub command, the symbol will remain compatible
with pre-FutureNet 6.1 versions and the message WILL appear 
if the limit will be exceeded by future pin stub commands.

Related Commands 

.-AI

.A

.B

.F

.SBS


### .-AI .-AO .>A .\<A -   Add/Replace IEC/ANSI Pin Stub    

Format 

.-AI | .-AO | .>A | .\<A

Purpose 

Add or replace the specified pin stub to the left or right side 
of IEC/ANSI symbol outline.  The pin stub commands are intended 
to be used in symbols created with the .A, .B and .F commands.

Remarks 

.-AI                     Creates an input pin stub. 

.-AO                     Creates an output pin stub. 

.>A | .\<A                Creates a pin stub with a clock 
                         designator. 

The graphics cursor must be positioned between the symbol cell 
and the symbol block outline on the left or right side of the 
symbol. 

The stub is drawn from the symbol cell to the symbol cell 
boundary at the location of the graphics cursor. 

If a stub is already present at the graphics cursor position, 
it is replaced by the specified stub. 

If the cursor is not in a symbol boundary, then the command is 
ignored and the system displays 

          Cursor not in symbol 

IEC/ANSI rules specify that pin stubs cannot be added at to the
top or bottom of a symbol.  If an attempt is made to add an ANSI/
IEC pin stub to the top or bottom of any block symbol, the 
following message will appear and the pin stub command will be 
ignored:

          ANSI/IEC design rules prohibit pins on the top or
                 bottom of symbol

If the cursor is on the corner of the symbol cell or inside the 
symbol cell, the the following message will appear and the command
will be ignored:

          Cursor not on symbol outline

The error message 

          Block symbol bit not set 

means that the special identifier flag for block symbols is 
not set.  Using the symbol editor on a block symbol will clear 
the flag.  Use the .SBS command inside the symbol editor to 
set the flag. 

The error message 

          Not a block symbol 

means that the symbol was created in such a way that it was 
never intended to have block symbol pin stubs.  You will be 
unable to add pins stubs to this symbol.  The .SBS command 
will be unable to convert this symbol to a block symbol.  

It is possible to exceed the maximum size of symbols for 
versions of FutureNet previous to version 6.10.  The following 
confirmation message will appear if the current pin stub 
addition will cause the symbol to exceed that limit:

    WARNING: Size of symbol exceeds pre-FutureNet 6.1 maximum. 
             	Continue (Y/N)?

Typing 'Y' will accept the pin stub command and the symbol will
be incompatible with pre-FutureNet 6.1 versions.  The warning
message will not appear again.  Drawings containing this symbol
will cause unpredictable problems with pre-FutureNet 6.1 versions,
including possible crashing the system.  Typing 'N' will abandon
the current pin stub command, the symbol will remain compatible
with pre-FutureNet 6.1 versions and the message WILL appear 
if the limit will be exceeded by future pin stub commands.


### '0...'7 -  Select Font 

Format 

'0 | '1 | '2 | '3 | '4 | '5 | '6 | '7

Purpose 

Select font.

Remarks 

The name of the current font is displayed in the ALPH status 
field.

When entering text between lines, either horizontally or 
vertically, leave sufficient room between the lines to 
accommodate the size font being used. 

'0                       Cycle through the available font 
                         sizes.  Each time the command is 
                         given, the font size will be changed 
                         to the next higher font number.  Font 
                         7 wraps to font 1. 

'1                       2x2 display unit font 

'2                       2x3 display unit font 

'3                       2x4 display unit font 

'7                       2x2 display unit font.  For font 7, 
                         all characters are uppercase.  
                         However, any characters entered 
                         without either the Shift key or the 
                         Caps Lock key on will be underlined 
                         to indicate they were entered in 
                         lowercase.  If the 'CH F command is 
                         used to change these characters to 
                         another font having a lowercase, the 
                         characters will appear as lowercase 
                         characters in the new font. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: The following fonts, Fonts '4 through '6, are intended 
      for use when conforming to MIL-D-1000 specifications. 

'4                       3x5 display unit font. 

'5                       5x6 display unit font. 

'6                       6x7 display unit font. 

Fonts cannot be changed while in alphanumeric mode.  

The font of an existing alphanumeric field can be changed 
using the 'CH F command. 

Related Commands 

'CH F


### /0.../10 -  Select Line Type 

Format 

/0 | /1 | /2 | /3 | /4 | /5 | /6 | /7 | /8 | /9 | /10

Purpose 

Select the style of line to be used when lines are drawn.

Remarks 

The name and sample of the current line type are displayed 
in the LINE status field.

/0                       Cycles through the available 
                         selections (/1 through /10).  Line 
                         type 10 wraps to line type 1. 

/1                       Selects wires.  This is the default 
                         value when FutureNet is started.  /1 
                         lines can be rubberbanded. 

/2                       Selects bus lines, which are the 
                         thickness of three wires.  /2 lines 
                         can be rubberbanded. 

/3 - /10                 Selects a variety of dashed or dotted 
                         lines.  These lines have no 
                         electrical significance and cannot be 
                         used to establish connectivity.  They 
                         are available as graphics only and 
                         can be used as comment lines. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Although line type /6 and /7 look the same as /1 and /2, 
      they are treated differently by post processing.  Line 
      types /1 and /2 are treated as electrically significant, 
      but /6 and /7 are for graphics only. 


### ? -  Read Custom Menus 

Format 

? [["]help\_string["]] 

Purpose 

Access custom menus and help screens.

Remarks 

The ? command provides access to custom menus and help screens
that are created with FutureNet and the HELPSAVE command.  An 
example of a custom menu library is supplied with FutureNet 
(SAMPLE.MNU).  Refer to the  FutureNet User Manual for more 
information on creating custom menus and help screens.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: This command does not provide access to the online 
      command reference.  Use the HELP command to access the 
      online command reference. 

If a "help\_string" is not entered, then the default menu, 
"?HELP" is used.  If it does not exist, then following error 
message appears on the status line: 

          Symbol not found 

If a "help\_string" is included in the "?" command, then the 
the help symbol "?help\_string" is loaded if it is in either 
the update library or one of the reference libraries (see .LIB 
or LIB commands). 

If the "?help\_string" symbol does not exist in an open 
library, then the symbol "help\_string" is used.  If neither 
symbol is found then the command you will receive the 
following error message: 

          Symbol not found 

If the SAMPLE.MNU library is loaded then the sample custom 
menu can be used. 

Note that any symbol can be loaded as a help screen. 

You can choose a custom menu selection the same way the 
standard FutureNet menu selections are chosen, by pressing the 
left mouse button or the \<Enter> key.

Exit a custom menu by pressing the right or middle mouse 
button or the \<Esc> key.

Move in the menus by moving the mouse or using the arrow keys. 
All other keys on the keyboard are disabled during custom 
menu mode. 

Related Commands 

HELP

HELPSAVE

LIB

.LIB

.DIR


### 'A -  Set Attribute for New Alphanumeric Field 

Format 

'A name | number

Purpose 

Set the default attribute to be assigned to new alphanumeric 
fields.

Remarks 

Specify the default attribute type using an attribute name or 
number. Attribute names and numbers are explained in the chapter
"Understanding FutureNet" in the FutureNet User Manual.

Until the system default attribute type is changed using the 
'A or 'S commands, the system default of COM 0 will be 
assigned. 

The attribute specified with 'A will be assigned to future 
alphanumeric fields.  It does not affect the attribute 
assignments of existing fields. 

When the cursor is not on an existing alphanumeric field, the 
attribute specified with the 'A command is shown in the ATTR 
status field.  Both the attribute's mnemonic and numeric 
values are displayed. 

If 'A is entered without specifying an attribute, the screen 
will display a menu of available attributes.  Use the mouse to 
select an attribute on this menu. 

To change the attribute of an existing field, use 'S or 'CH A. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: The 'D command can be used to display the numeric 
      attributes of all existing fields in the drawing.  
      Another way to check the attribute assignment for an 
      alphanumeric field is to place the graphics cursor on 
      the field and check the value in the ATTR status field.  
      The ALPH status field will change from NEW to OLD or ONP 
      to verify that you are in an existing field. 

Related Commands 

'CH A

'D

'S


### .A -  Create IEC/ANSI Symbol Outline 

Format 

.A w,hh,hr[,ws[,hs]]

Purpose 

Create an IEC/ANSI block symbol outline with a ``hat.''

Remarks 

Not supported by FutureNet OEM products.

w                        The width of the rectangle and hat in 
                         display units. 

hh                       The height of the hat. 

hr                       The height of the rectangle. 

ws, hs                   The distance from the sides (ws), and 
                         from the top or bottom (hs) of the 
                         rectangle to the symbol cell.  If ws 
                         is not specified, the system default 
                         of 7 display units is used.  hs has a 
                         default of 3 display units. 

The combined numeric value of w and two times ws (one ws per 
side) cannot exceed the size of the X-coordinate of the drawing.
Similarly, the combined numeric value of hh, hr, and two times 
hs cannot exceed the size of the Y-coordinate of the drawing.  If 
the value exceeds either of the drawing dimensions the following
message will be displayed: 

          Symbol boundary conflict 

and the command will be ignored. 

Compatibility Note:

Modifications were made in FutureNet 6.1 to allow block symbols
larger than previous versions of FutureNet.  Although block 
symbols larger than allowed previously will be compatible with
previous versions of FutureNet, they will not be recognized as
block symbols in those versions.  For exact definitions of the
older limits, see the documentation for that version of FutureNet.

Negative values are not meaningful for any of the parameters and 
zero (0) is only meaningful for ws and hs.  The following message
will appear for negative and zero values which are not appropriate:

        Value out of range

The symbol is created with the top left corner of the symbol 
cell positioned at the graphics cursor.  A new reference number
is assigned to the block symbol. 

If another symbol, an alphanumeric field, a line, or the 
drawing edge is in the area to be allocated to the new symbol 
cell, a symbol boundary conflict will prevent the symbol from 
being placed.  The message 

          Symbol boundary conflict 

will be displayed.  The conflict must be resolved before the 
new symbol can be entered into the drawing.  

.A uses symbol definition instructions to generate a symbol. 
Symbol definition instructions are described in the chapters 
on symbol definition in the FutureNet User Manual.  The list 
of instructions used to define the block symbol can be viewed 
and edited by entering Symbol Definition Mode (.s command).  
However, once a symbol has been edited in Symbol Definition 
Mode, a bit is set that prevents it from being edited using 
drawing mode commands, such as the Add/Replace Pin Stub 
commands.  In order to use these commands on symbols that have 
been edited in Symbol Definition Mode, enter the .SBS command 
prior to exiting Symbol Definition Mode. 

Caution: Modifying the block symbol definition list can cause
the block symbol to be unaccessible to the pin stub commands.

Related Commands 

.-                       .B 

.>                       .F 

.-AI                     .SBS 


### /AD /AL /AR /AU -  Draw/Erase Arrowhead 

Format 

/AD | /AL | /AR | /AU

Purpose 

Insert or delete the arrow type specified.

Remarks 

/AU                      Inserts/deletes an arrow pointing up. 

/AD                      Inserts/deletes an arrow pointing 
                         down. 

/AL                      Inserts/deletes an arrow pointing 
                         left. 

/AR                      Inserts/deletes an arrow pointing 
                         right. 

Arrows may be inserted anywhere on any type of line. The point 
of the arrow will be located at the graphics cursor. Arrows 
can be deleted by placing graphics cursor at the point of the 
arrow and entering the command option that corresponds to the 
direction of the arrow. 

An arrow pointing into a line cannot be at the end of the 
line. Also, an arrow cannot be drawn on a line segment shorter 
than three display units. 

If the graphics cursor is not located on a line, the system 
will display the error message 

Cursor not on a vertex or line 

Related Commands 

/D


### A1...D8 -  Move to Coordinates 

Format 

A1 | A2 | . . . D8

Purpose 

Move the graphics cursor and drawing viewpoint to a particular 
region of the screen.

Remarks 

The coordinates available varies with the drawing size.  The 
grid of the coordinates is about 1/8 of the drawing width by 
1/4 of the drawing height.  The A1 command will place the 
drawing viewpoint in the lower right corner, approximately
1/8 the drawing width from the drawing's right edge and 1/4 
of the drawing height from the bottom.

The D8 command will place the drawing viewpoint in the upper 
left corner of the drawing. 

In full or intermediate zoom, this command will, if possible, 
update the display so that the cursor is centered on the 
screen. 

In fit zoom, this command moves the cursor to the intersection 
of the requested coordinates. 

Related Commands 

PAN

WINDOW

CURSOR


### Add/Replace Target Line - (Symbol Definition Mode) 

Format 

instruction\<Enter>

Purpose 

Add a new instruction to the symbol definition instruction 
list.

Remarks 

Not supported by FutureNet OEM products. 

This command adds a new instruction to the symbol definition 
list or replaces an existing instruction. 

To add an instruction, enter the instruction on the command 
line and enter. The instruction list will move up, leaving a 
blank target line that is ready for the next instruction. 

To replace an existing instruction, position the instruction 
to be replaced on the target line and enter a new instruction 
on the command line and enter. Note that the instruction list 
will not move up, but will remain at the newly added 
instruction.  You can replace this instruction, move to 
another instruction and replace it, or move the target line to 
the end of the list and continue adding instructions. 

The entire symbol definition instruction must be retyped if 
any changes are to be made to it; no provisions are made for 
editing an existing symbol definition line. 

The instruction is checked for syntax errors before the target 
line is replaced. 

Related Commands 

.D

.I

.CLR


### Arrow Keys -  Move Multiple Display Units 

Format 

LEFT number

RIGHT number

UP number

DOWN number

Keys 

[number]\<left arrow>
[number]\<right arrow>
[number]\<up arrow>
[number]\<down arrow>

Purpose 

Move the graphics cursor the specified number of display 
units or grid units in the direction indicated.

Remarks 

Default cursor movement is one display unit, or one grid 
unit when Snap to Grid is enabled.

Typing a number on the command line and then pressing any of 
the arrow keys moves the cursor the specified number of 
display units or grid units in the direction of the arrow.  
While the number remains on the command line, subsequent use 
of any of the arrow keys moves the cursor the number of 
display units or grid units specified. 

When the graphics cursor is being operated from the command 
line, it can move in any direction until it encounters the 
edge of the drawing screen.  At that point, continuing to 
enter the command in the same direction has no effect.  Once 
the graphics cursor encounters the edge of the screen, it can 
be centered again using the HOME command, then moved further 
in the same direction until the edge of the drawing or screen 
is again encountered. 

When automatic panning is on, and the cursor reaches the edge 
of the screen, the cursor is automatically centered to allow 
further cursor movement.  The cursor can move in any direction 
until the edge of the drawing is reached. 

If the number value given would take the cursor past the edge 
of the drawing, the cursor moves to the edge of the drawing. 

For command line or command file operation, use the commands 
LEFT, RIGHT, UP and DOWN.  

Related Commands 

CURSOR

GRID


### Arrow Up/Down Keys -  Move Symbol Definition List Up/Down (Symbol Definition Mode) 

Format 

UP number

DOWN number

Keys 

[number]\<up arrow>

[number]\<down arrow>

Purpose 

Move the symbol definition instruction list up or down past 
the target line.

Remarks 

Not supported by FutureNet OEM products.

The symbol definition list moves up or down one line at a 
time, if no number is specified. If a number is specified, the 
list moves up or down by number of lines specified. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: If you specify 8\<up arrow> and there are only 5 lines 
      above the cursor, the cursor will move up only 5 lines. 

The symbol definition list can also be traversed using the 
mouse.  Clicking the left button on the desired command will 
move that command to the target line.  Clicking the left 
button above or below the list will cause a Page Up or Page 
Down. 

For command line or command file operation, use the commands 
UP and DOWN. 

Related Commands 

PC | SUN 
---|----
\<PgUp>  |    \<R9>
\<PgDn>  |    \<R15>


### AUTO -  Automatic Command Execution 

Format 

AUTO filename

AUTOEX filename

Purpose 

Run the specified command file automatically.

Remarks 

A filename is required.  AUTO will search for command files 
with the default .cmd command file extension unless a different
file extension is specified.

This command can also be used in the Symbol Definition Mode. 

The command file named must be a standard ASCII text file and 
must follow the command file format.  That is, commands on the 
same line must be separated by semicolons (;), a line cannot 
exceed 256 characters, and commands must include all required 
parameters.  Commands are entered in sequence. 

During automatic command file execution, commands can be typed 
on the command line.  After the command on the command line 
has been entered, automatic command execution will resume, 
unless the command is one that affects command execution, such 
as PAUSE or STOP. 

A second command file can be initiated from within the first 
by including the AUTO filename command within the first 
command file.  Naming one command file within another command 
file stops the execution of the first command file and starts 
execution of the second command file.  It does not clear the 
current drawing work space, and there is no return to the 
first file. 

Note, however, that AUTO filename must end the line it is 
entered on.  Any commands on the same line that follow AUTO 
will not be entered. 

When a command file is being processed with AUTO, commands 
that require a yes or no response are automatically given a 
yes so execution is not halted. 

Commands which call dialog boxes are ignored. 

To automatically run a command file when initiating FutureNet, 
see the reference for the fn command. 

Related Commands 

EXEC

NOTE

PAUSE

STOP

VIEW


### AUTOPAN -  Enable/Disable Automatic Panning 

Format 

AUTOPAN [ON | OFF]

Purpose 

Turn automatic panning on and off.

Remarks 

Autopanning is enabled by default.

When automatic panning is enabled, and the graphics cursor 
moved with the mouse reaches the edge of the display screen, 
the display shifts so that the cursor is centered on the 
screen.  Automatic panning occurs at the edge of the drawing 
space when the graphics cursor is moved with the arrow keys. 

When automatic panning is off, the HOME command must be used 
to reposition the display. 

Entering AUTOPAN without parameters toggles the setting. 

When AUTOPAN is entered, a status message appears indicating 
whether automatic panning is enabled or disabled. 

Related Commands 

PROFILE


### AUTOSAVE -  Save Drawing Automatically 

Format 

AUTOSAVE [[commands ][,minutes ] | [ON] | [OFF] | [STAT]]

Purpose 

Periodically save drawing contents to temporary file.

Remarks 

The drawing saved automatically is written to a temporary file.
If the current drawing session should terminate abnormally, this
temporary file can be copied and used in lieu of the original 
drawing file.

On PC systems, drawing data is saved to the current directory. 
On Sun systems, drawing data is saved to the user's home 
directory.  The files are given a filename following these 
conventions: 

    dwgfilename.nnn   (If a drawing filename has been given) 

    autotemp.nnn      (If no drawing filename has been given) 

where dwgfilename is the name of the drawing file being 
edited, and nnn is the number of successful automatic saves of 
the drawing, ranging between 1 and 999.  Each time an 
automatic save is successful, the previous AUTOSAVE file is 
deleted and a new one created. 

AUTOSAVE ON is the default, with the commands and minutes 
parameters set as outlined below. 

Command parameters work in this fashion: 

commands                 Causes an automatic save after the 
                         given number of commands have been 
                         entered.  The default is 50. 

minutes                  Causes an automatic save after the 
                         given number of minutes have passed.  
                         The default is 30 minutes. 

ON                       Turns on AUTOSAVE with 
                         current/default parameters active. 

OFF                      Turns off AUTOSAVE. 

STAT                     Returns the current status of 
                         AUTOSAVE. 

no options               Toggles AUTOSAVE on or off. 

Entering AUTOSAVE with one option changes only that option.  
For instance, the following command causes an automatic save 
every 5 minutes, and the number of commands between saves is 
unaffected: 

AUTOSAVE ,5 

Automatic saves occur when either option is reached, then both 
counters are reset.  Using the default options as an example, 
if an automatic save happens at 30 minutes and only 15 
commands have been entered, both the command and minute 
counters are reset. 

Automatic saves occur only if the drawing has been modified, 
and only if a command has been issued since the last automatic 
save. 

You may standardize how AUTOSAVE functions for you by 
including the command in your fnpro.cmd file.  Temporary files 
created by AUTOSAVE are not deleted automatically at the end 
of a session. 


### 'B -  Enable/Disable Alphanumeric Field Boundary Display 

Format 

'B [ON | OFF]

Purpose 

Display or hide all alphanumeric field boundaries.

Remarks 

The default setting is OFF.

When ON, each alphanumeric field is displayed with a dotted 
boundary and its point of justification.  The point of 
justification is the round dot that appears in the lower left, 
center, or right of the field. 

When OFF, alphanumeric field boundaries and points of 
justification are not displayed. 

If no parameter is specified, 'B toggles the current setting.

Displaying alphanumeric field boundaries aids in the 
positioning of text fields, particularly signal name or pin 
name fields.  

The boundaries are displayed at all zoom levels. 

This feature can be particularly useful for solving boundry 
conflict situations at zoom levels greater than full zoom 
(when alphanumeric fields are not visible). 


### .B -  Create Block Symbol 

Format 

.B w,h [,ws [,hs ] ]

Purpose 

Create a block symbol of the specified size.

Remarks 

Not supported by FutureNet OEM products. 

w is the width of the symbol rectangle expressed in display 
units; h is the height of the rectangle. 

ws is the distance between the sides of the rectangle and the 
symbol cell.  If ws is not specified, the system default of 5 
display units is used. 

hs is the distance between the top or bottom of the rectangle 
and the symbol cell.  If hs is not specified, the system 
default of 3 display units is used. 

The combined numeric value of w and two times ws (one ws per 
side) cannot exceed the size of the X-coordinate of the drawing. 
Similarly, the combined numeric value of h and two times hs 
cannot exceed the size of the Y-coordinate of the drawing.  If 
the value exceeds either of the drawing dimensions the following
message will be displayed: 

          Symbol boundary conflict 

and the command will be ignored. 

Compatibility Note:

Modifications were made in FutureNet 6.1 to allow block symbols
larger than previous versions of FutureNet.  Although block 
symbols larger than allowed previously will be compatible with
previous versions of FutureNet, they will not be recognized as
block symbols in those versions.  For exact definitions of the
older limits, see the documentation for that version of FutureNet.

Negative values are not meaningful for any of the parameters and 
zero (0) is only meaningful for ws and hs.  The following message
will appear for negative and zero values which are not appropriate:

        Value out of range

The command will be ignored.

The symbol is created with the top left corner of the symbol 
cell positioned at the graphics cursor location.  A new 
reference number is assigned to the block symbol. 

If another symbol, an alphanumeric field, a line, or the 
drawing edge is in the area to be allocated to the new symbol 
cell, a symbol boundary conflict will prevent the symbol from 
being placed.  The message 

          Symbol boundary conflict 

will be displayed.  The conflict must be resolved before the 
new symbol can be entered into the drawing.  

.B uses symbol definition instructions to generate a symbol. 
Symbol definition instructions are described in the chapters 
on symbol definition in the FutureNet User Manual.  The list 
of instructions used to define the block symbol can be viewed 
and edited by entering Symbol Definition Mode (.s command).  
However, once a symbol has been edited in Symbol Definition 
Mode, a bit is set that prevents it from being edited using 
drawing mode commands, such as the Add/Replace Pin Stub 
commands.  In order to use these commands on symbols that have 
been edited in Symbol Definition Mode, enter the .SBS command 
prior to exiting Symbol Definition Mode. 

Caution: Modifying the block symbol definition list can cause
the block symbol to be unaccessible to the pin stub commands.

Related Commands 

.-

.-AI

.A

.D 

.F 

.SBS 


### BLINK -  Enable/Disable Blinking 

Format 

BLINK [ON | OFF]

Purpose 

Turn blinking on and off.

Remarks 

The BLINK command controls blinking for the following:

\*     Zoom window (visible when in fit zoom). 

\*     Alphanumeric fields the graphics cursor is located in. 

\*     Tagged alphanumeric fields, symbols, and areas. 

\*     The direct-connection cursor (/C command). 

\*     Temporary lines. 

Blinking is the default value on monochrome systems.  
Non-blinking is the default value on color systems. 

Entering BLINK without parameters toggles the setting. 


### BX, BY -  Draw Broken Line (Symbol Definition Mode) 

Format 

BX length

BY length

Purpose 

Draw a textured line of the specified length along 
the x or y axis.

Remarks 

Not supported by FutureNet OEM products. 

The texture of the line to be drawn is determined by the SL 
command.  The default texture type is a dotted line.  The line 
type can be seen in the Line Status box, along with the 
appropriate SL command. 

The line begins at the current x,y coordinates. 

Length is in display units and can have values between +127 
and -128. 

BX length draws a textured horizontal line along the x axis 
from the current x,y coordinates.  If length is negative, the 
line is drawn to the left; if length is positive, the line is 
drawn to the right. 

BY length draws a textured vertical line along the y axis from 
the current x,y coordinates. If length is negative, the line 
goes up; if length is positive, the line goes down. 

The symbol editor coordinate marker is relocated to the end of 
the line drawn as a result of this instruction. 

Related Commands 

SL

BXD

BYD


### BXD, BYD -  Draw Broken Line in Dot Units (Symbol Definition Mode) 

Format 

BXD length

BYD length

Purpose 

Draw a textured line of the specified length along 
the x or y axis.

Remarks 

Not supported by FutureNet OEM products. 

The texture of the line to be drawn is determined by the SL 
command.  The default texture type is a dotted line.  The line 
type can be seen in the Line Status box, along with the 
appropriate SL command. 

The line begins at the current x,y coordinates. 

Length is in dot units and can have values between +127 and 
-128. 

BXD length draws a textured horizontal line along the x axis 
from the current x,y coordinates.  If length is negative, the 
line is drawn to the left; if length is positive, the line is 
drawn to the right. 

BYD length draws a textured vertical line along the y axis 
from the current x,y coordinates. If length is negative, the 
line goes up; if length is positive, the line goes down. 

The symbol editor coordinate marker is relocated to the end of 
the line drawn as a result of this instruction. 

Related Commands 

SL

BX

BY


### BXL, BXR -  Draw x to Symbol Cell Boundary with Bubble (Symbol Definition Mode) 

Format 

BXL

BXR

Purpose 

Draw a line to the left or right symbol cell boundary along 
the x axis from the current x,y coordinates, placing 
a bubble at the x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

This command is particularly useful in drawing line stubs to 
the symbol cell boundary. 

BXL places a bubble on the symbol cell at the current x,y 
coordinates and draws a line along the x axis to the left 
symbol cell boundary. 

BXR places a bubble on the symbol cell at the current x,y 
coordinates and draws a line along the x axis to the right 
symbol cell boundary. 

The symbol editor coordinate marker location is not changed by 
these instructions. 

Related Commands 

.w, h

DXL

DXR

BYU

BYL


### BYU, BYL -  Draw y to Symbol Cell Boundary with Bubble (Symbol Definition Mode) 

Format 

BYU

BYL

Purpose 

Draw a line to the upper or lower symbol cell boundary along 
the y axis from the current x,y coordinates, placing 
a bubble at the x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

This command is primarily used to draw line stubs within the 
symbol cell. 

BYU places a bubble at the current x,y coordinates and draws a 
line along the y axis to the upper symbol cell boundary. 

BYL places a bubble at the current x,y coordinates and draws a 
line along the y axis to the lower symbol cell boundary. 

The symbol editor coordinate marker location is not changed by 
these instructions. 

Related Commands 

.w, h

DYU

DYL

BXL

BXR


### 'C -  Copy Alphanumeric Field 

Format 

'C

Purpose 

Tag the current alphanumeric field and move a copy of it 
to a different location.

Remarks 

Place the cursor within the target field's boundary, and 
enter 'C.  When entered, the MODE status field changes from the
current mode to COPY and the graphics cursor becomes the tag 
cursor.

When the tag cursor is moved, a dotted boundary detaches from 
the field and can be moved around the drawing using the mouse 
or cursor movement commands.  The tagged field retains a 
highlighted boundary.  The ghosted boundary aids in locating 
the text field so that it can be positioned without causing a 
boundary conflict.  You can also use 'B to display boundaries 
of all alphanumeric fields.  On monochrome systems, BLINK 
causes the tagged field to blink so that it is easier to see.
On color systems, the tagged field changes color. 

When 'C is entered again, the field is copied into the new 
cursor location, assuming there are no boundary conflicts with 
other symbol cells, alphanumeric fields, lines, or the edge of 
the drawing.  To resolve boundary conflicts, move the copy to 
a valid location, or move or erase conflicting objects. 

The location of the original alphanumeric field remains 
unchanged.  The tag adjusts only the location of the copied 
field. 

After the field is copied and tag and drag are canceled, 
control returns to the previous mode, and the graphics cursor 
is restored. 

Related Commands 

'B

'K

BLINK

'M


### 'CH A -  Change Attribute for Existing Field  

Format 

'CH A name | number

'CH ATTRIBUTE name | number

Purpose 

Change the attribute of an existing alphanumeric field.

Remarks 

This command changes the attribute for the field where the 
graphics cursor is located.  The attribute is shown in the ATTR
status field.

Attribute names and numbers are listed and explained in the 
chapter on "Understanding FutureNet" in the FutureNet User 
Manual. 

If the name or number is not specified, a menu of available 
attributes will be displayed and can be selected with the 
mouse. 

Note that the current attribute assignments of all fields can 
be seen by entering 'D command. 

Related Commands 

'A

'D

'S


### 'CH F -  Change Font 

Format 

'CH F [0 | 1 | 2 | 3 | 4 | 5 | 6 | 7]

Purpose 

Change the font size of an existing alphanumeric field.

Remarks 

This command changes the font size for the current alphanumeric 
field.  The font size is shown in the ALPH status field.

Any of the seven font sizes can be selected.  If you do not 
specify a font size, 'CH F will cycle through the font sizes 
in ascending order, beginning with the current font size, 
showing them in the ALPH field. 

Changing a field's font to a larger size means the size of the 
field will increase to accommodate the larger font. If this is 
impossible because of the field's proximity to other fields or 
graphic items, a boundary conflict will prevent the change 
from occurring until the boundary conflict is eliminated. 

Changing a field's font to a smaller size means the size of 
the field will decrease to accommodate the smaller font.  As 
the field decreases in size, the graphics cursor may be left 
outside the field boundary.  This may be important when a 
command file is being used because it can result in a 
previously selected alphanumeric field no longer being 
selected and commands left unentered. 

Related Commands 

'0...'7


### 'CH J -  Change Justification 

Format 

'CH J [L | R | C]

'CH JUSTIFICATION [L | R | C]

Purpose 

Change the justification (left, right, or center) of an existing 
alphanumeric field.

Remarks 

This command changes the justification for the current 
alphanumeric field. The minimum entry is 'CH J, which cycles 
through the options from left to right to center.  The 
justification is shown on the center line of the ALPH status 
field.

Select left, right, or center justification.  Left, L, enters 
text to the right of the initial cursor position.  Right, R, 
enters text to the left of the cursor position.  Center, C, 
enters text evenly on either side of the initial cursor 
position. 

Note that the 'B command displays the field boundary and the 
point of justification, which is the dot appearing at the 
lower left, center, or right of the field. 

Related Commands 

'B

'J


### 'CH O -  Change Orientation 

Format 

'CH O [H | V]

'CH ORIENTATION [H | V]

Purpose 

Change the orientation of an existing alphanumeric field.

Remarks 

This command changes the orientation of the current alphanumeric 
field.  The orientation, which is shown on the fourth line of the
ALPH status field, can be either horizontal (H) or vertical (V).

If no orientation is specified, the command toggles the 
setting.  The default setting is horizontal. 

If changing a field's orientation is impossible because of the 
field's proximity to other fields or graphic items, a boundary 
conflict message is displayed, and the orientation is not 
changed.  The center of the field is the pivot point for 
changing orientation.  Ensure that there is adequate space for 
the text field in the new orientation or a boundary conflict 
will prevent the field from being placed. 

In the vertical orientation, text is displayed and read from 
bottom to top.  In the horizontal orientation, text is 
displayed and read from left to right. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: When using 'CH O in command files, make sure the 
      graphics cursor is positioned in the center of the text 
      field it is to change.  Otherwise, the command will be 
      ignored. 


### 'CH P -  Change Printability 

Format 

'CH P [ON | OFF]

'CH PRINTABILITY [ON | OFF]

Purpose 

Identify an existing alphanumeric field as being printable 
or non-printable.

Remarks 

This command operates on the current alphanumeric field.  
Printability is shown on the top line of the ALPH status field.
OLD = printable.  ONP = old non-printable.

ON sets a field to printable; OFF sets a field to non-printable. 

If no parameter is specified, entering 'CH P toggles the current
setting. 

Printability may be determined by the default attribute. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: All non-printable fields can be forced to print, without 
      changing each individual field, using the R/RNO option 
      of the printopt command.  This is useful for checking a 
      drawing for completeness of information not normally 
      printed on a finished drawing. 

Related Commands 

PRINTOPT

PROFILE


### 'CH R -  Change Reverse Video State of Alphanumeric Field 

Format 

'CH R [ON | OFF]

Purpose 

Toggle the reverse video display of an existing alphanumeric 
field.

Remarks 

This command toggles the reverse video display on the current 
alphanumeric field.  

ON causes the field to appear in reverse video in the drawing. 

OFF causes the field to appear in normal video. 

If no parameter is specified, reverse video toggles on and 
off. 

Reverse video is useful for emphasis in custom menus or help 
screens. Refer to the FutureNet User Manual for more 
information on creating custom menus and help screens. 

Related Commands 

HELPSAVE

?

'CH V


### 'CH V -  Change Visibility of Alphanumeric Field 

Format 

'CH V [ON | OFF]

Purpose 

Toggle the visibility of an existing alphanumeric field

Remarks 

This command toggles the visibility for the current alphanumeric 
field.  

ON causes the field to be visible during the editing session. 

OFF causes the field to invisible during the editing session. 

If no parameter is specified the visibility toggles on and 
off. 

Invisible alphanumeric fields follow the same boundary 
conflict rules as visible text. 

To edit invisible text, place the cursor on the text and the 
text will appear.  When the cursor is not on the field, it is 
invisible. 

To locate invisible text, use the 'B (Display Alphanumeric 
Boundary Display) command.  The boundaries of all alphanumeric 
fields, visible or invisible, are displayed. 

Invisible text is used in custom menus and help screens. Refer 
to the  FutureNet User Manual for more information on creating 
custom menus and help screens. 

Related Commands 

HELPSAVE

?

'CH V


### .C -  Copy Symbol 

Format 

.C

Purpose 

Copy the tagged symbol to the current cursor location.

Remarks 

When the symbol to be copied is not tagged, first move the 
cursor into the symbol and enter .C or press the left mouse
button to tag the symbol and press middle mouse button to cycle
through MOVE/COPY/ERASE mode.

When the symbol to be moved is tagged and the cursor is moved, 
the boundary detaches from the symbol and moves around the 
drawing with the mouse or cursor movement commands.  The 
tagged symbol boundary and cursor blink on monochrome systems 
or change color on color systems so that the symbol and its 
new location are obvious.  When .C is entered again, the 
symbol is copied into the boundary. 

The Copy command enters only if the symbol is being copied to 
a location clear of symbol cells, alphanumeric fields, and 
lines, and does not overlap the edge of the drawing.  If there 
are any interfering drawing elements, the command is ignored 
and the system displays 

          Symbol boundary conflict 

A new reference number is assigned to the copied symbol. 

The original symbol remains unchanged. When a symbol is 
copied, MOVE/COPY/ERASE mode is canceled. 

Related Commands 

.K

.M

BLINK


### .CLR -  Erase Symbol Definition (Symbol Definition Mode) 

Format 

.CLR

Purpose 

Clear the symbol definition list and the symbol definition 
workspace, so that a new symbol can be created.

Remarks 

Not supported by FutureNet OEM products. 

The symbol cell size remains as previously defined and the 
symbol cell boundary is still displayed. 

All symbol definition instructions are deleted from the symbol 
definition workspace. 

Following a .CLR command, the symbol definition coordinate 
marker is in the upper left corner of the symbol cell. 

Related Commands 

.D

.S


### /C -  Draw Direct Connection Through Symbol 

Format 

/C

Purpose 

Draw intra-symbol connections.

Remarks 

/C is used to electrically connect two pins on the same symbol 
by drawing a line through the symbol.  /C cannot be used to
connect pins on different symbols.

If a pin is connected to a signal and the pin is also directly 
connected to other pins in the symbol, all of those pins are 
considered to be connected in the same net. 

Position the graphics cursor on the desired pin stub and then 
enter the /C command.  Draw a line to connect the two pin 
stubs and re-enter the command or click the left mouse button. 

Direct connections are only displayed when the .DCON command 
is on.  The direct connection appears as a dotted line inside 
a symbol. 

If the graphics cursor is not located on a symbol boundary 
when the first /C command is entered, the command is ignored 
and the message line displays 

          Invalid line start location 

If the direct connection does not end on a symbol boundary, 
the line is not drawn and the system displays 

          Direct connect segment must end on a pin stub or 
          symbol boundary 

If a second /C is entered when the cursor is in a different 
symbol than the first /C, the system displays 

          Direct connect segment must end in same symbol 

If two symbols are 2 display units apart or closer, then when 
the first /C command is entered, the line drawing may snap to 
an adjacent symbol. You may need to turn PINSNAP off 
temporarily or move the symbols farther apart. 

Related Commands 

.DCON

/ES

/EL

/EN


### [C -  Copy Area 

Format 

[C

Purpose 

Copy the tagged area to the current cursor location.

Remarks 

The area to be copied must first be defined using [D, and 
then tagged using the [C command or the left mouse button.

When [C is entered, the area to be copied is tagged and a 
boundary detaches from the area.  The boundary can be moved to 
the desired location using the mouse or cursor movement 
commands.  When [C is entered again, the area is copied to the 
new cursor location, assuming there are no boundary conflicts. 

Symbols and alphanumeric fields are treated as being within 
the area to be copied only if they are completely within the 
area boundary.  Symbols and alphanumeric fields intersecting 
the area boundary are excluded from the copy. Line segments 
are broken at the area boundary and the segments inside are 
copied.  The densely dotted area boundary is moved with the 
area. 

New symbol reference numbers are assigned to copied symbols. 

The Copy command works only if the new location is clear of 
symbol cells, alphanumeric fields and lines, and does not 
overlap the edge of the drawing. 

If the area cannot be copied because of a boundary conflict, 
the command is ignored and the system displays 

          Boundary conflict 

To resolve the boundary conflict, move or erase the 
conflicting drawing elements or reposition the area. 

The original area remains unchanged.  The dotted boundary 
stays with the copied area.  After the area has been copied, 
tag and drag is canceled and control returns to the previous 
mode. 

Related Commands 

[D

[K

[M


### CD -  Change Current Directory 

Format 

CD [drive][path]

CHDIR [drive][path]

Purpose 

Change the current path to the one specified.

Remarks 

The CD command operates a little differently in FutureNet than 
in DOS.  If a drive is specified in the FutureNet CD command, 
then the current drive, as well as the current directory, is 
modified.  Under DOS, only the current directory of the 
specified drive is modified.  The current drive is not modified.

The CD command operates similar to the UNIX CD command on a 
Sun platform. 

Related Commands 

PC                       Sun 

DIR                      LS 

DEL                      RM 


### CLEAR/ERASE -  Clear Drawing Hierarchy or Erase Current Drawing     

Format 

CLEAR

ERASE

Purpose 

CLEAR clears the current drawing in the work space and all 
drawings in the hierarchy that have been accessed and modified.

ERASE erases only the current drawing from the work space, and 
does not erase other drawings in the hierarchy that may have 
been accessed and modified. 

Remarks 

Use the CLEAR command to clear the current drawing in the work 
space and any drawings in a structured design hierarchy that 
have been accessed using the \#D, \#U, \#R, and \#L commands and 
then modified. The portions of the drawing structure that have
not been accessed or modified will not be affected.

To help avoid clearing drawings that may have been accessed 
and modified elsewhere in the hierarchy, the CLEAR command 
will ask for verification before clearing the memory. If no 
files have been modified, the command clears the memory 
without asking for verification. 

If a current version of the drawing has not been saved when 
the ERASE command is entered, the ERASE command will ask for 
verification before erasing the drawing from memory.  If the 
drawing file in memory has not been modified, the command 
erases the drawing without asking for verification. 

Neither CLEAR nor ERASE removes the last-saved version of the 
drawing from the hard disk. 

The CLEAR command clears the UNDO/REDO stacks.  The ERASE 
command can be reversed using the UNDO command, if UNDO is 
enabled. 


### COLOR -  Modify Color Palette 

Format 

COLOR [color1,color2,...color8]

Purpose 

Select the background color and modify the current color 
palette used by the DISPGRPn command.

Remarks 

This command performs two functions: selects the background 
color, which is always assigned to position five (color5) on the
palette; and selects which colors are assigned to the remaining
seven positions on color palette.

The colors of the color palette are used by the DISPGRPn 
command, which assigns colors to the elements of the FutureNet 
display. 

The COLOR command changes the color settings of the current 
palette.  The color palette has eight color positions, 
numbered color1 through color8.  One color can be assigned to 
each position. 

The COLOR command has eight positional parameters, separated 
by commas  (,). Each position represents one of the colors on 
the color palette.  To change a color on the palette, assign a 
different color to a given position.  For example 

COLOR ,,,,,G 

only affects the color assignment for position 6 (color6), 
which would have its present color assignment changed to G 
(green).   All drawing components that have been assigned the 
position 6 color (as specified by the DISPGRPn command) will 
be changed to green.  Drawing components that have been 
assigned position 1, 2, 3, 4, 5, 7, and 8 colors on the 
palette will retain their current color assignments. 

There are three ways to assign colors:  enter the initial(s), 
the number, or the name. 

For example 

COLOR GREEN,WHITE,R,B,BLACK 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Components that are the same color as the background 
      will be indistinguishable, even though they are 
      displayed. 

Default color assignments are 

Assignment               Name 

Color 1                  Yellow 

Color 2                  Light-White 

Color 3                  Light-Red 

Color 4                  Blue 

Color 5                  Brown 

Color 6                  Light-Cyan 

Color 7                  Light-Green 

Color 8                  Magenta 

There are 16 colors available.  Specify colors by their 
initial(s), number, or name. 

Initial     Number     Name 

BL          0          Black 

B           1          Blue 

G           2          Green 

C           3          Cyan (blue-green - BG) 

R           4          Red 

M           5          Magenta 

Y           6          Yellow 

W           7          White 

GY          8          Grey 

LB          9          Light-Blue 

LG          10         Light-Green 

LC          11         Light-Cyan (light-blue-green - LBG) 

LR          12         Light-Red 

LM          13         Light-Magenta 

BR          14         Brown 

LW          15         Light-White 


Related Commands 

DISPGRPn

PALETTE


### CONNECT -  Enable/Disable Maintenance of Line Connections (Rubberbanding) 

Format 

CONNECT [ON | OFF]

Purpose 

Disable or enable the maintenance of line connections (rubber-
banding) when moving a symbol or area.

Remarks 

When a symbol or an area is moved with CONNECT enabled, its 
connections are maintained unless invalid line routing is 
created.  If this occurs, temporary lines are created.  Temporary
lines are dotted (they also change color on color systems), and 
overlay everything in their path.

Only /1 and /2 line types can be rubberbanded. 

When CONNECT is disabled and a symbol or area is moved, lines 
are broken at the symbol or area boundary. 

If CONNECT is enabled and a symbol is tagged and moved but not 
set, entering CONNECT will disable rubberbanding as well as 
cancel any rubberbanding in progress for the current symbol. 

When CONNECT is entered, a message appears indicating whether 
CONNECT is enabled or disabled. 

Enabled is the default setting. 

Related Commands 

PROFILE


### CONTEXT -  Restore Drawing Context 

Format 

CONTEXT

Purpose 

Restore the context of a drawing.

Remarks 

Context refers to the state of a drawing at the time it was 
last saved. Context includes cursor position, drawing screen
window, area definition, discrete zoom level (not dynamic zoom
level), font type, attribute type, and line type.  All are saved
with the drawing.  When the CONTEXT command is entered after a
Load Drawing command, the drawing is restored to the state it 
was in when last saved.  No command is necessary to store the 
information that CONTEXT restores.


### \<Ctrl>\<Backspace> Keys -  Erase Alphanumeric Data 

Keys 

\<Ctrl>\<Backspace>

Purpose 

While in alphanumeric mode, this command erases the data 
in the current alphanumeric field.

Remarks 

After erasing the data, the cursor is placed at the beginning 
of the field.

If alphanumeric mode is exited immediately after entering 

\<Ctrl>\<Backspace>

the field is deleted from the drawing.


### \<Ctrl>\<Home> or \<Ctrl>\<R7> Keys - Move Cursor to Beginning of Alphanumeric Field

\<Ctrl>\<End> or \<Ctrl>\<R13> Keys - Move Cursor to End of Alphanumeric Field  

Keys

PC               Sun 

\<Ctrl>\<Home>     \<Ctrl>\<R7>

\<Ctrl>\<End>      \<Ctrl>\<R13>

Purpose 

Move the cursor to the beginning or end of an alphanumeric 
field (when in alphanumeric mode) or the command line.

Remarks 

\<Ctrl>\<Home> or \<Ctrl>\<R7> moves the cursor to the beginning of
the current alphanumeric  field, or to the beginning of the
command line.

\<Ctrl>\<End> or \<Ctrl>\<R13> moves the cursor to the end of the 
current alphanumeric field, or to the end of the command line.

Moving the cursor with these commands does not affect the 
alphanumeric field. 


### CURSOR -  Set Cursor to Absolute Location 

Format 

CURSOR x,y [,xw,yw]

Purpose 

Specify up to two sets of coordinates:  one set to specify the 
graphics cursor location (x,y) and a second optional set to 
specify the location of the upper left corner of the display 
area (xw,yw).

Remarks 

The x,y are required; xw,yw are optional.

If both x,y and xw,yw are specified, the upper left corner of 
the window is placed at the intersection of the xw,yw 
coordinates and the cursor is placed at the intersection of 
the x,y coordinates.  If the x,y coordinates fall outside the 
window area specified by the xw,yw coordinates, the xw,yw 
coordinates are ignored and the window is centered around the 
x,y coordinates specified for the cursor. If the xw,yw 
coordinates are too close to the border to accommodate the 
entire window, then the location of the window is adjusted so 
that the upper left corner of the window is placed as near to 
the xw,yw coordinates as possible. 

This command is intended primarily for use in automatic 
command execution sequences. 


### #D -  Move Down in Design Hierarchy 

Format 

\#D[filename]

Keys 

PC | SUN 
---|----
\<Ctrl>\<PgDn> | \<Ctrl>\<R15>

Purpose 

Move down in the design hierarchy to the specified file or create
a new file with the specified name.

Remarks 

\#D filename loads the specified drawing file from a lower level
in the hierarchy or creates a new file with the specified name 
at a lower level in the design hierarchy.  It saves the current 
drawing, its status, and the graphics cursor location in a 
temporary file.  Drawings that are saved into temporary files when
\#D is entered are given unique filenames based on the root file-
name with a number for the extension.  If a file of the same name
exists, then the number will be incremented until the filename is
unique.  The drawings must be saved using the SAVEALL and SAVE 
commands before exiting FutureNet.

This command requires that a root drawing be loaded with the LOAD
command before \#D is entered.  If the root drawing is just being
created, then it must first be saved before \#D is entered.  If \#D
is entered before the root drawing is saved, the system displays 

          Drawing must be saved before moving down in the 
          drawing structure 

The optional parameter filename is available only with the \#D 
form of the command.  

\<Ctrl>\<PgDn> or \<Ctrl>\<R15> does not accept a filename.

\#D and \<Ctrl>\<PgDn> or \<Ctrl>\<R15> can be used to create a lower
level drawing without specifying a filename, if the cursor is 
located on an alphanumeric field that has been assigned the FILE
or FILN attribute.  These attributes (file name pointers) are 
assigned to the displayed name of a functional block in hier-
archical drawings.  A newly created file will use the field's 
display text as the name of the file.

If the graphics cursor is not on an alphanumeric field with 
attribute FILE or FILN and no filename is entered, the command 
has no effect. 

If the file named or located by the graphics cursor does not 
yet exist, the system displays 

          File not found.  OK to create new file (Y/N)? 

Y creates a new blank file with that name; N cancels the 
command. 

If the file named or located by the graphics cursor has 
already been viewed at a higher level in the hierarchy, and 
therefore is presently saved in a temporary file, the system 
cancels the command and displays 

          File already accessed in drawing structure 

If the drawing named or located by the graphics cursor exists, 
has been viewed in this session and left again, and is at a 
lower level in the design hierarchy, then it is loaded with 
its status and the graphics cursor as they were when it was 
left. 

\#D filename can be used to jump lower in the design hierarchy 
without accessing intermediate drawing levels by specifying 
the file to be opened. 

Related Commands 

\#U

\#L

\#R

SAVEALL

SAVE


### 'D -  Enable/Disable Attribute Display 

Format 

'D [ON | OFF]

Purpose 

Display alphanumeric field attribute assignments.

Remarks 

When ON, a reverse video field displaying the attribute number 
appears in place of the alphanumeric field(s) on the drawing.

When OFF, alphanumeric fields appear in their normal manner. 

Specifying 'D without any parameters toggles the setting. 

Related Commands 

'A

'CH A

'S


### .D -  Delete Pin Stub 

Format 

.D

Purpose 

Delete an existing pin stub.  

Remarks 

The graphics cursor must be positioned between the symbol cell 
and the outline to delete a stub.  This is the same location used
when the stub is created or replaced.  If the cursor is not in an
appropriate location, the command is ignored.

Only block symbols created with the .A, .B, or .F commands, 
and edited with the standard pin stub commands have the 
internal symbol definition instruction sequences that can be 
processed by this command. 

Related Commands 

.-

.-AI

.A

.B

.F


### .D -  Delete Target Line (Symbol Definition Mode) 

Format 

.D

Purpose 

Delete the symbol definition instruction currently in the 
target line.  

Remarks 

Not supported by FutureNet OEM products. 

When a symbol definition instruction is deleted, the next 
symbol definition instruction in sequence moves into the 
target line. 

Related Commands 

.I

.CLR


### .DCON -  Enable/Disable Display of Direct Connections 

Format 

.DCON [ON | OFF]

Purpose 

Displays direct connections across a symbol, instead of all 
other graphic data in the symbol.  

Remarks 

Use of the .DCON command is the only way to view direct connect 
lines.

This command updates the setting in the current user profile. 

If no parameter is specified, the setting in the current user 
profile is toggled. 

When turned on, .DCON turns off the display of all other items 
in a symbol and instead displays any direct connections drawn 
across the symbol. 

Related Commands 

/C

PROFILE


### .DEL -  Delete Symbol from Update Library 

Format 

.DEL symbolname 

Purpose 

Delete the specified symbol from the update symbol library.

Remarks 

.DEL only deletes symbols from the update library. The update 
library is specified using the .LIB command.

Related Commands 

.LIB

.DIR

.SAVE


### .DIR -  Display Symbol Library Directory  

Format 

.DIR

Purpose 

Display a list of library contents.

Remarks 

When the .DIR command is entered, the symbol directory dialog 
box is displayed.  

The Symbol Name Specification field filters the list of symbol 
names.  To change the filter, edit the field and press \<Enter>.
Wildcards are valid:  \* for any number of characters; ? for 
a single character.

The List of Symbol Libraries data box lists all open 
libraries, one of which is selected (highlighted).  The List 
of Symbols data box displays the symbols in the selected 
library that match the symbol name specification.  Select a 
different library by clicking on it with the mouse. 

Use the scroll bars in the List of Symbols data box to browse 
the various symbol names available.  To load a symbol, 
highlight it on the list and click on the LOAD action button.  
If there are no boundary conflicts, the symbol is loaded at 
the drawing cursor, tagged for moving. If a symbol or line is 
already at that location, the symbol is not loaded, and you 
receive the message: Symbol boundary conflict 

Exit the .DIR dialog box without selecting a symbol by 
clicking on the OK action button, by clicking on the CANCEL 
action button, by pressing the left mouse button outside of 
the dialog box, or by pressing \<Esc>.

Related Commands 

.L

.DEL

LIB

.LIB

.SAVE


### .DIRPR -  Print Symbol Library Directory 

Format 

.DIRPR

Purpose 

Print a list of the symbols in the libraries specified by the 
last .LIB command, the LIB command, and the system.sym library.

.DIRPR uses the relevant print options as set up in the 
PRINTOPT command. 

Related Commands 

.LIB

LIB

.DIR


### /D -  Insert/Delete Interconnect Dot 

Format 

/D

Purpose 

Insert or delete an interconnect dot at the graphics cursor 
location.

Remarks 

An interconnect dot is required when two lines which cross 
are connected.  Interconnect dots are allowed, but not required
at T intersections.

If there is an interconnect dot at the intersection located by 
the graphics cursor, /D deletes it.  The location of the 
graphics cursor is not changed. 

Interconnect dots cannot be placed within symbol cells.  They 
can only be placed on intersections of line types /1 and /2.  
No other line types will accept interconnect dots. 

Interconnect dots can be drawn and displayed in any zoom level 
less than or equal to full zoom (1.00). 

The graphics cursor must be located on a line segment when /D 
is entered; if not, the command is ignored. 

The /D command is equivalent to the middle mouse button when 
in FAST drawing mode. 


### [D -  Define Area 

Format 

[D

Purpose 

Define an area.

Remarks 

An area needs to be defined before it can be moved, copied, 
erased, or saved.

When [D is entered, area definition is initiated; the area 
definition point appears at the graphics cursor location and 
the MODE status field displays AREA.  The area definition 
point marks the fixed corner of the area to be defined.  
Define the area by moving left or right and up or down from 
that fixed point.  Re-enter [D or press the left mouse button 
to save the newly defined area.  The finished area boundary is 
a dotted rectangle. 

Area definition can be initiated anytime.  However, only one 
area at a time can be defined. 

When an area is already defined and the graphics cursor is 
outside of that area, entering [D cancels the currently 
defined area and begins a new area definition at the cursor 
location.  If this area is canceled before [D is re-entered, 
that is, if it is canceled while still in area definition 
mode, area definition is canceled and the previous area is 
restored. 

When an area is already defined and the cursor is inside that 
area, entering [D restarts area definition.  The corner of the 
area closest to the cursor ``snaps'' to the cursor location.  
This corner becomes the movable corner of the area rectangle 
and the corner opposite becomes the fixed corner.  Area 
definition can then proceed as described above. 

Area definition can be initiated from the menu or from the 
command line using the mouse or keyboard. 

The cursor movement commands, and Pan and Zoom can be entered 
from the keyboard without canceling area definition.  Entering 
any other command from the keyboard automatically cancels area 
definition and returns to the previous mode. 

Related Commands 

[C

[ERASE

[K

[LOAD

[M

[R

[RE

[SAVE


### DA -  Draw Arc in Display Units (Symbol Definition Mode) 

Format 

DA radius,center,eighths

Purpose 

Draw the specified arc in display units.

Remarks 

Not supported by FutureNet OEM products. 

The current x,y coordinates mark the beginning point of the 
arc. 

radius specifies the radius of the circle that the arc is 
taken from and is expressed in display units.  There are four 
dot units per display unit. 

center specifies the line on which the center of the arc will 
fall, relative to the current x,y position (see the figure 
in the FutureNet Command Reference).  The center will be radius
display units away from the start point of the arc.  center can
take values from 0 to 7. 

eighths specifies the length of the arc in eighths of a circle 
and the direction of travel:  clockwise or counterclockwise.  
Positive numbers from 1 to 8 draw the arc in a clockwise 
direction; negative numbers from -1 to -8 draw the arc in a 
counterclockwise direction.  1 draws 1/8 of a circle, or a 45 
degree arc; 8 draws an entire circle.  A value of 0 is not 
valid. 

The symbol editor coordinate marker moves to the end of the 
arc drawn. 

Related Commands 

DAD

DC


### DAD -  Draw Arc in Dot Units (Symbol Definition Mode) 

Format 

DAD radius,center,eighths

Purpose 

Draw the specified arc in dot units.

Remarks 

Not supported by FutureNet OEM products. 

The current x,y coordinates mark the beginning point of the 
arc. 

radius specifies the radius of the circle that the arc is 
taken from and is expressed in dot units.  There are four dot 
units per display unit. 

center specifies the line on which the center of the arc will 
fall (see the figure in the FutureNet Command Reference).  The 
center will be radius dot units away from the start point of
the arc. center can take values from 0 to 7. 

eighths specifies the length of the arc in eighths of a circle 
and the direction of travel:  clockwise or counterclockwise.  
Positive numbers from 1 to 8 draw the arc in a clockwise 
direction; negative numbers from -1 to -8 draw the arc in a 
counterclockwise direction.  1 draws 1/8 of a circle, or a 45 
degree arc; 8 draws an entire circle.  A value of 0 is not 
valid. 

The symbol editor coordinate marker moves to the end of the 
arc drawn. 

Related Commands 

DA

DCD


See fn. 


### DC -  Draw Circle in Display Units (Symbol Definition Mode) 

Format 

DC radius

Purpose 

Draw a circle with the radius specified in display units.

Remarks 

Not supported by FutureNet OEM products. 

radius is a required parameter and gives the radius of the 
circle in display units.  radius may have values from 0 to 
255. 

The center of the circle is at the current x,y coordinate 
location, which is not changed by this instruction. 

Related Commands 

DCD

DA


### DCD -  Draw Circle in Dot Units (Symbol Definition Mode) 

Format 

DCD radius

Purpose 

Draw a circle with the radius specified in dot units.

Remarks 

Not supported by FutureNet OEM products. 

radius is a required parameter and gives the radius of the 
circle in dot units.  radius may have values from 0 to 255. 

The center of the circle is at the current x,y coordinate 
location, which is not changed by this instruction. 

Related Commands 

DC

DAD


### DD -  Draw Dots (Symbol Definition Mode) 

Format 

DD a,b

Purpose 

Draw two dots.  

Remarks 

Not supported by FutureNet OEM products. 

The first dot is placed 1 dot unit from the current x,y 
coordinate, along the line indicated by parameter a, according 
to the figure in the FutureNet Command Reference.  The second 
dot is placed 1 dot unit from the first dot along the line 
indicated by parameter b, relative to the first dot. 

This instruction updates the x,y value to the location of the 
second dot drawn. 

The possible values of a and b are 0 to 7.  Relative to an x,y 
coordinate they are as follows: 




### DEL -   Delete File 

Format 

DEL [drive][path]filename

Purpose 

Delete the specified file from the current directory.

Remarks 

See your system documentation regarding the DEL command.  A 
drive, path, filename and extension can follow the DEL command.  
The global file characters \* and ? cannot be used.

Related Commands 

CD

DIR

RM


### DIR -  List Directory  

Format 

DIR command line arguments  (PC)

ls command line arguments  (SUN)

Purpose 

List all directory entries or only specified files.

Remarks 

When this command is entered, the directory dialog box is 
displayed.  The dialog box contains a list of the files in the
current directory that satisfy the file specification given as
an argument for the command.  This list is placed in a data 
window, which can be scrolled. 

The Directory Specification field filters the list of files.  
To change the filter, edit the field and press \<Enter>.
Wildcards are valid:  use \* for any number of symbols; ? for a 
single character.  Drawing, Area, Library, Command File and File 
commands can be performed by selecting the desired file, then 
clicking on the desired action button.

Use the scroll bars in the file data box to display the 
filenames available.  

Related Commands 

ls


### DISPGRPn -  Display Group Setup 

Format 

DISPGRP1 comp1color,comp2color,...comp7color

DISPGRP2 comp8color,comp9color,...comp15color

DISPGRP3 comp16color,comp17color,...comp23color

DISPGRP4 comp24color,comp25color,...comp31color

Purpose 

Change the color of the components that make up a drawing.

Remarks 

There are 31 drawing components arranged in one group of seven and
three groups of eight components.  Each component has an assigned
position within its group.  The number assigned to a position 
specifies the color from the palette (see COLOR) assigned to the
component associated with that position.  To change the color of
a component, assign a different color from the color palette to
that position.

DISPGRPn is used to specify the display group and the color 
from the color palette.  For example 

DISPGRP3 ,7 

will change the cursor, which is in position 2 of display 
group 3, to the color assigned to position 7 of the palette.  
Other colors are unchanged. 

Similarly, entering 

DISPGRP4 ,,1,8,, 

with the default color palette will cause direct connection 
lines to be yellow, and the pin cursor to be magenta. 

The following tables list the components in each display 
group, their position numbers, and the default color 
assignments.  The items that are marked with an asterisk (\*) 
are fixed colors and cannot be changed. 

DISPGRP1: 
Pos#    Component Name                  Comp#   Default Color 
------------------------------------------------------
 1      Border	                        1       4
 2      Status headings 	        2       4
 3      MODE status field value         3       3
 4      Other status field values       4       6
 5      Signal and bus lines            5       1 
 6      Symbol graphics                 6       1 
 7      Symbol boundary                 7       1 

DISPGRP2: 
Pos#    Component Name                  Comp#   Default Color 
------------------------------------------------------
 1      Alphanumeric text               8       1 
 2      Command line\*                   9       White
 3      Message line\*                   10      White
 4      Symbol reference number         11      4
 5      Unused                          12 
 6      Tagged object                   13      3
 7      Unused                 	        14  
 8      Menu text\*                      15      Black 

DISPGRP3: 
Pos#    Component Name                  Comp#   Default Color 
------------------------------------------------------
 1      Menu headings\*                  16      Black
 2      Cursor                          17      2
 3      Line target                     18      2
 4      Area target                     19      2
 5      Rubberbanded line               20      3
 6      Area definition outline         21      2
 7      Full scale window               22      4
 8      Preset WINDOWs                  23      1 

DISPGRP4: 
Pos#    Component Name                  Comp#   Default Color 
------------------------------------------------------
 1      Alphanumeric cursor 	        24      2
 2      Alpha mode insert cursor        25      3
 3      Direct connection line          26      3
 4      Pin cursor                      27      4
 5      Grid                            28      8
 6      Unused                          29
 7      Symbol edit instructions        30      2 
 8      Symbol edit target line         31      3 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: The background color is hard-coded to palette position 
      number 5 in the COLOR command. 

Related Commands 

COLOR

PALETTE


### DOS -  Enter DOS Command 

Format 

DOS [command]

Purpose 

To run a system command or enter a DOS shell without 
leaving FutureNet.

Remarks 

On the Sun, specify command with the DOS command; the results of
the specified command will be sent to the window from which 
FutureNet was run.  If no command is specified, the DOS command 
does nothing.

On the PC, the DOS command operates as follows: 

1. FutureNet will temporarily close and a DOS shell will be 
   opened.  Within the DOS shell, commands should function 
   normally with the following exceptions. 

\*     The Non-Extended Memory executable uses a "swapping" 
      mechanism which removes FutureNet from memory, and 
      writes it to a special file  created on the hard disk 
      named ZTCxxxxx.TMP (where xxxxx are unique characters).  
      This "swapping" provides more memory for the command to 
      be run.  However, at least 500KB of disk space is 
      required.  If insufficient disk space is available, the 
      DOS command will not run.  (No error message is given 
      when you are returned to FutureNet.) 

\*     Programs that permanently allocate memory cannot be run 
      in the DOS shell.  This includes all TSR (Terminate Stay 
      Resident) programs and DOS functions such as MODE, 
      PRINT, ASSIGN, GRAPHICS, SHARE, and GRAFTABL.  Using 
      these programs can cause FutureNet to fail (losing 
      changes done during the FutureNet session). 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: As a precaution, you may want to save your drawings 
      before performing the DOS command. 

\*     Using the SET command will only set environment 
      variables for the DOS shellit will not effect FutureNet. 

2. If a command is specified, the command will run in the DOS 
   shell, displaying its results on the screen.  When the 
   command has completed, the message 

       Press any key to continue

   will appear.  Pressing any key will return to FutureNet. 

3. If no command is specified when the DOS shell was entered, 
   the DOS prompt will appear.  The DOS prompt should include 
   a FutureNet string.  To return to FutureNet, use the DOS 
   EXIT command. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: If the DOS prompt does not include the string FutureNet 
      in its entirety, your system's environment space is too 
      small.  You may need to enlarge the environment space; 
      see your DOS manual. 

Do not run FutureNet from within a DOS shell.  This will cause 
loss of logging files and cause disk clusters to be lost. 

Related Commands 

!


### DR -  Draw Rectangle in Display Units (Symbol Definition Mode) 

Format 

DR width,length

Purpose 

Draw a rectangle of the specified length and width in display 
units at the current x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

width and length are specified in display units. 

width and length can take any value from +127 through -128.  

If the width is specified using positive numbers, the 
rectangle extends right from the x,y coordinates; if the width 
is specified with negative numbers, the rectangle extends 
left. 

If the length is specified using positive numbers, the 
rectangle extends down from the x,y coordinates; if the length 
is specified using negative numbers, the rectangle extends up. 

The symbol editor coordinate marker remains at the current x,y 
coordinates. 

Related Commands 

DRD


### DRD -  Draw Rectangle in Dot Units (Symbol Definition Mode) 

Format 

DRD width,length

Purpose 

Draw a rectangle of the specified length and width in dot 
units at the current x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

width and length are specified in dot units. 

width and length can take any value from +127 through -128.  

If the width is specified using positive numbers, the 
rectangle extends right from the x,y coordinates; if the width 
is specified with negative numbers, the rectangle extends 
left. 

If the length is specified using positive numbers, the 
rectangle extends down from the x,y coordinates; if the length 
is specified using negative numbers, the rectangle extends up. 

The symbol editor coordinate marker remains at the current x,y 
coordinates. 

Related Commands 

DR


### DX, DY -  Draw Line in Display Units (Symbol Definition Mode) 

Format 

DX length

DY length

Purpose 

Draw a line along the x or y axis, specifying 
the length in display units.

Remarks 

Not supported by FutureNet OEM products. 

length is in display units. 

length may take any value from +127 through -128.  If length 
is between +15 and -16, use the DXS and DYS (Draw Line Short) 
instructions to save memory. 

DX length draws a line of the specified length along the x 
axis.  If length is specified using negative numbers, the line 
is drawn left; if length is specified using positive numbers , 
the line is drawn right.  The y coordinate is not changed by 
the DX instruction. 

DY length draws a line of the specified length along the y 
axis.  If length is specified using negative numbers, the line 
is drawn up.  If length is specified using positive numbers, 
the line is drawn down.  The x coordinate is not changed by 
the DY instruction. 

After a DX or DY instruction, the symbol editor coordinate 
marker is located at the end of the line drawn. 

Related Commands 

DXD

DYD

DXY

DXS

DYS


### DXD, DYD -  Draw Line in Dot Units (Symbol Definition Mode) 

Format 

DXD length

DYD length

Purpose 

Draw a line along the x or y axis, specifying 
the length in dot units.

Remarks 

Not supported by FutureNet OEM products. 

length is in dot units 

length may take any value from +127 through -128. 

DXD length draws a line of the specified length along the x 
axis.  If length is specified using negative numbers, the line 
is drawn left; if length is specified using positive numbers , 
the line is drawn right.  The y coordinate is not changed by 
the DXD instruction. 

DYD length draws a line of the specified length along the y 
axis.  If length is specified using negative numbers, the line 
is drawn up.  If length is specified using positive numbers, 
the line is drawn down.  The x coordinate is not changed by 
the DYD instruction. 

After a DXD or DYD instruction, the symbol editor coordinate 
marker is located at the end of the line drawn. 

Related Commands 

DX

DY

DXYD

DXS

DYS


### DXL, DXR -  Draw x to Symbol Cell Boundary (Symbol Definition Mode) 

Format 

DXL

DXR

Purpose 

Draw a line along the current y coordinate to the 
left or right symbol cell boundary.  

Remarks 

Not supported by FutureNet OEM products. 

This command is particularly useful in drawing line stubs to 
the symbol cell boundary. 

DXL draws a line along the x coordinate to the left symbol 
cell boundary. 

DXR draws a line along the x coordinate to the right symbol 
cell boundary. 

The symbol editor coordinate marker location is not changed by 
these instructions. 

Related Commands 

BXL

BXR

DYU

DYL


### DXS, DYS -  Draw Line Short in Display Units (Symbol Definition Mode) 

Format 

DXS length

DYS length

Purpose 

Draw a short line (+15 through -16 display units in length) 
along the x or y axis.

Remarks 

Not supported by FutureNet OEM products. 

This instruction is used for lines between +15 and -16 display 
units in length. 

length is in display units. 

For lines between +15 and -16 display units in length, using 
this instruction will save memory.   For longer lines, use the 
DX or DY instruction. 

DXS length draws a line along the x axis beginning at the 
current x,y coordinates.  If length is negative, the line is 
drawn left; if length is positive, the line is drawn right. 

DYS length draws a line along the y axis beginning at the 
current x,y coordinates.  If length is negative, the line is 
drawn up; if length is positive, the line is drawn down in the 
symbol cell. 

After a DXS or DYS instruction, the symbol editor coordinate 
marker is located at the end of the line drawn. 

DXS and DYS are also used with Increment and Decrement modes, 
which are described under the SD and SI commands. 

Related Commands 

DX

DY

DXD

DYD

SD

SI


### DXY -  Draw Diagonal in Display Units  (Symbol Definition Mode)

Format 

DXY x,y

Purpose 

Draw a diagonal line, in display units, from the current 
x,y coordinates. 

Remarks 

Not supported by FutureNet OEM products. 

x and y are in display units, and can take any value from +127 
through -128. 

The DXY x,y instruction draws a line in any direction, 
beginning at the current x,y coordinates and ending the 
specified x and y distance from the original x,y coordinates. 

Positive numbers draw x to the right; negative numbers draw x 
to the left. 

Positive numbers draw y down; negative numbers draw y up. 

Positive and negative x,y value combinations will draw lines 
into the quadrants indicated: 

Following the DXY instruction, the symbol editor coordinate 
marker is located at the end of the line drawn. 

Note that this instruction can be used to draw lines along the 
current x or y axis. 

Related Commands 

DXYD

MXY


### DXYD -  Draw Diagonal in Dot Units (Symbol Definition Mode) 

Format 

DXYD x,y

Purpose 

Draw a diagonal line, in dot units, from the current x,y 
coordinates.

Remarks 

Not supported by FutureNet OEM products. 

x and y are in dot units, and can take any value from +127 
through -128. 

The DXYD x,y instruction permits a line to be drawn in any 
direction, beginning at the current x,y coordinates and ending 
the specified x and y distance from the original x,y 
coordinates. 

Positive numbers draw x to the right; negative numbers draw x 
to the left. 

Positive numbers draw y down; negative numbers draw y up. 

Positive and negative x,y value combinations will draw lines 
into the quadrants indicated: 

Following the DXYD instruction, the symbol editor coordinate 
marker is located at the end of the line drawn. 

Note that this instruction can be used to draw lines along the 
current x or y axis. 

Related Commands 

DXY

MXYD


### DYU, DYL -  Draw y to Symbol Cell Boundary  (Symbol Definition Mode) 

Format 

DYU

DYL

Purpose 

Draw a line along the current x coordinate to the 
upper or lower symbol cell boundary.  

Remarks 

Not supported by FutureNet OEM products. 

This command is primarily used to draw line stubs to the 
symbol cell boundaries. 

DYU draws a line along the y coordinate to the upper symbol 
cell boundary. 

DYL draws a line along the y coordinate to the lower symbol 
cell boundary. 

The symbol editor coordinate marker location is not changed by 
these instructions. 

Related Commands 

DXL

DXR

BYU

BYL


### 'E -  Erase Alphanumeric Field  

Format 

'E

Purpose 

Erase the current alphanumeric field.

Remarks 

An alphanumeric field can be erased by placing the graphics cursor
on the desired field and entering the 'E command or by entering 
the 'E command while a field is tagged, which erases the tagged 
field and cancels the tagging.

The entire field is erased. 

Related Commands 

'C

'M

'R

'I
\<Esc>


### .E -  Erase Symbol 

Format 

.E

Purpose 

Erase a symbol from the drawing.

Remarks 

A symbol does not have to be tagged to be erased, as long 
as the cursor is located somewhere within the symbol boundary.

The entire symbol cell, including the alphanumeric fields and 
reference number, is erased from the drawing.  Interconnects 
to the symbol are not changed. 

When no symbol is tagged or the cursor is not inside a symbol 
when this command is entered, the command is ignored and the 
system displays: Cursor not in symbol 

The graphics cursor location does not change when the symbol 
is erased. 

Related Commands 

.C

.M

.L


### /E -  Erase Line 

Format 

/E

Purpose 

Erase the line segment or the portion of a line segment overlapped 
by a new line segment.

Remarks 

The /E command initiates line erase from either MENU or FAST 
drawing.

/E works like /L except that it removes lines rather than 
draws them. 

/E erases any portions of a permanent line that are overlapped 
by a routing segment.  The line start point is relocated to 
the end of the routing segment when the command is entered, so 
additional connecting segments may be erased. 

The line being erased may be of any type, not necessarily the 
line type displayed in the LINE status field. 

Note that routing segments drawn by the /E command, whether 
drawn over existing line segments or not, can be made 
permanent by terminating the /E operation with the /L command. 
 That is, terminating /E with /L cancels the erase operation 
and makes all routing segments permanent lines.  

The mode status field shows LINE. 

Related Commands 

/L

/LE

/EL

/EN

/ES


### /EL -  Erase Line Segments 

Format 

/EL

Purpose 

Erase all interconnected line segments between two points 
of connection.

Remarks 

/EL traces a line and erases all segments until a connection 
with a pin, a junction, an interconnection, or another line is 
encountered.  A line interconnection is indicated by an inter-
connect dot where two lines meet or by a ``T'' connection.  The 
"T" may or may not have an interconnect dot.  The interconnect 
dot is not erased.

Temporary lines will also be deleted if encountered. 

The line being erased may be of any type, not necessarily the 
line type displayed in the LINE status field. 

The location of the graphics cursor does not change. 

Related Commands 

/E

/LE

/EN

/ES


### /EN -  Erase Line Network 

Format 

/EN

Purpose 

Erase a line and all interconnecting lines.

Remarks 

The graphics cursor is the starting point for the erase line 
network operation.

The line is traced and erased along with all interconnected 
lines.  Interconnect dots are erased. 

Lines are erased along any branch of the network and erasing 
stops when line ends are reached, including symbol cell 
boundaries and alphanumeric fields (signal names).  Lines 
emerging from interconnect dots and "T" intersections will be 
erased. 

Temporary lines and direct connect lines (see /C command) will 
also be deleted. 

If the cursor is on an electrically signficant line type (/1 
or /2), only electrically significant line types are traced 
and erased. 

If the cursor is on a line type that is not electrically 
significant, /EN will erase the line segment the cursor is 
located on, and any electrically significant lines networked 
to the segment.  The line type erased is not necessarily the 
line type displayed in the LINE status field. 

Related Commands 

/LE

/E

/EL

/ES


### /ES -  Erase Line Segment 

Format 

/ES

Purpose 

Erase the permanent or temporary line segment on which the 
graphics cursor is located.

Remarks 

The cursor may be located anywhere on the line segment to 
be erased.

The line segment is erased to an intersection, that is, a 
corner, symbol boundary, interconnect dot, or "T" connection. 

If the cursor is not on a line segment, the command is 
ignored.  If the cursor is at the intersection of two lines, 
the command is ignored and the system displays 

          Ambiguous cursor location 

Temporary lines resulting from rubberbanding will also be 
erased by the /ES command when the cursor is located on them.  
If the cursor is located on a temporary line which also 
overlays a permanent line segment, /ES erases only the 
temporary line. 

When temporary lines are touched, they will become dotted on 
monochrome systems or change color on color systems. 

The line being erased may be of any type, not necessarily the 
line type displayed in the LINE status field. 

The location of the graphics cursor does not change. 

The /ES command is equivalent to the right mouse button when 
using FAST drawing mode. 

Related Commands 

/E

/EL

/EN

/LE

/ET


### /ET -  Erase Temporary Lines 

Format 

/ET

Purpose 

Erase all temporary lines in the drawing.

Remarks 

The connection information maintained by the temporary lines 
is lost when this command is entered.  Therefore, permanent lines 
should be drawn to replace temporary lines before this command is 
entered.

/ET can be used at any time.  The temporary lines do not have 
to be rerouted and made into permanent legal connections 
before using this command. 

When entered, the system will prompt 

          OK to erase all temporary lines (Y/N)? 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: It is not necessary for the cursor to be on a temporary 
      line for this command to take effect. 

Related Commands 

/ES

/P


### [ERASE -  Erase Area 

Format 

[ERASE

Purpose 

Erase all objects completely contained within an area.

Remarks 

The area must already have been defined, though it does not 
have to be tagged; see [D.  The cursor may be located either
inside or outside the area when the [ERASE command is entered.

When the [ERASE command is entered, the system displays 

          OK to erase area (Y/N)? 

When the response is Y for yes, all objects within the area 
definition are erased and the area definition is lost. 

When the response is N for no, the area is maintained. 

[ERASE includes all symbols, alphanumeric fields, and lines 
completely inside the area boundary, as well as the area 
definition.  Any lines intersecting the area boundary are 
broken at the boundary and the segments inside the area are 
erased. 

If an area is erased during a MOVE/COPY/ERASE operation, 
control returns to the previous mode. 

Related Commands 

[D

See CLEAR. 


### Esc Key -  Enter/Exit Alphanumeric Mode 

Key 

\<Esc>

Purpose 

Toggle alphanumeric mode.

Remarks 

Pressing \<Esc> toggles alphanumeric mode.

ALPH appears in the MODE status field when alphanumeric mode 
is enabled. 

If the graphics cursor is located in an existing alphanumeric 
field when alphanumeric mode is entered, the cursor is 
positioned on the character at the left end of the field.  If 
the graphics cursor is not located in an existing field when 
alphanumeric mode is entered, a new alphanumeric field will be 
created. The graphics cursor marks the lower left corner of 
the first character position in the new field, and the 
alphanumeric cursor appears at this character location. 

Entering and immediately exiting alphanumeric mode will not 
affect an existing alphanumeric field nor create a new 
alphanumeric field. 

See the chapter "Mouse, Modes, and Cursors" in the FutureNet 
User Manual for more information on using alphanumeric mode. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Esc is also used to cancel the menus and dialog boxes. 


### EXEC -  Single-step Command Execution 

Format 

EXEC filename

Purpose 

Invoke single-step command file execution.

Remarks 

Before EXEC can be entered, a filename is required.  EXEC 
will search for command files with the default .cmd command file
extension unless a different file extension is specified.

This command can also be used in the Symbol Definition Mode. 

EXEC places each command on the command line and waits for a 
\<Enter> by the operator to enter it.  After the command is 
entered, the next command in the sequence is placed on the 
command line.

The command file named must be a standard ASCII text file 
created by a text editor and must follow the command file 
format.  That is, commands on the same line must be separated 
by semicolons (;), a line cannot exceed 256 characters, and 
each line must end with a complete command--they cannot wrap 
to the next line.  

EXEC can be interrupted momentarily at any line by typing a 
command on the command line and entering it.  After the 
user-initiated command(s) has been entered, the bypassed 
command file command is again placed on the command line. 

Single-step execution invoked by the EXEC command can be 
changed to automatic execution by typing AUTO without a 
command filename.  See the AUTO command for more information. 

A second command file can be initiated from within the first 
by including the EXEC filename command within the first 
command file.  Naming one command file within another command 
file stops the execution of the first command file and starts 
execution of the second command file.  It does not clear the 
current drawing work space, and there is no return to the 
first file. 

Any command following an EXEC filename command will not be 
entered. 

Related Commands 

AUTO

NOTE

PAUSE

STOP

VIEW


### Format 

EXPORT

Purpose 

Enter the post processing menus to create custom post processing 
scripts and generate reports.

Remarks 

The EXPORT menus can also be accessed by selecting EXPORT 
Generate Reports from the main menu.  

Complete information on operating the EXPORT menus is given in 
the FutureNet Post User Manual, and the FutureNet to EDIF 2 0 0 
Netlist Writer User Manual. 

The following reports can be generated through the EXPORT 
menus: 

Menu Selection           Report Generated 

Connect Design           Drawing Connectivity Model (.dcm) 

Check Design Rules       Design Rule Check Report (.drc) 

Generate Pinlist         Enhanced or compatible pinlist (.pv4 
                         or .pin) 

Generate Netlist         Enhanced or compatible netlist (.nv4 
                         or .net) 

Generate Parts List      Parts List (.prt) 

EDIF 2 0 0 Netlist       EDIF 2 0 0 netlist 


The settings in the EXPORT menu can be saved to the configuration
file fn.cfg, by choosing the EXPORT main menu item:

	Save Export Configuration

The EXPORT settings are saved under various headings.  For more 
information on the configuration file, see HELP FN.CFG.


### 'F -  Find Alphanumeric String 

Format 

'F ["]string["]

Purpose 

Search the drawing, starting at the graphics cursor location, 
for any alphanumeric field beginning with the specified character
string.

Remarks 

A search string must be specified. If one is not specified, the
system will prompt for one and position the command line cursor 
at the end of the command, ready to enter the search string.  To
include leading or trailing spaces in the string, use the double
quotations.  Otherwise FutureNet will search for the string from
the first non-blank character to the last non-blank character.

The drawing is searched from the current graphics cursor 
position, left to right and top to bottom, for an alphanumeric 
field starting with the specified character string.  If the 
string is not found below the cursor position, then the search 
will resume at the top of the drawing. 

Only fields starting with the specified string are located. 

Uppercase and lowercase occurrences of the specified string 
are located, as well as partial strings at the beginning of 
the text field. 

The display shifts to center on fields that are located 
outside the current drawing area. 

Since the original command remains on the command line, 
pressing \<Enter> causes the system to search for the next
occurrence of the string.

If the requested string is not found, the system displays 

          Not found 

To search and replace at the same time, use a command list of 
'F and either 'R or 'I as follows: 

'F ["]string["];'R ["]string["] 

searches for the 'F string and replaces it with the 'R string. 

'F ["]string["];'I ["]string["] 

searches for the 'F string and replaces it with the 'I string, 
incrementing the value specified by 'I. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: The entire field is replaced by the 'R or 'I string, not 
      just the string specified with 'F. 

For example, when the 'I string is a circuit designator, 
entering 

'F UNNN;'I U1 

replaces the first found occurrence of UNNN with U1, and 
increments the command on the command line to 'F UNNN;'I U2. 
Pressing \<Enter> runs this command, increments the number, and
so on.

'F is especially useful when looking for signal names causing 
errors in the Pinlist Generator or for finding circuit 
designator fields that need to be filled in. 

If a double quote (") or backward slash (\\) is to be included 
in the string, precede the character with a backward slash.  
For example, to find the string 

"status\\pin1" 

use the following 'F command: 

'F "\\"status\\\\pin1\\"" 

Related Commands 

'FA

'I

'R


### 'FA -  Find Attribute 

Format 

'FA name | number [["]string["]]

Purpose 

Search the drawing for alphanumeric fields having the specified 
attribute assignment (name or number) and, if included, beginning
with the specified string.

Remarks 

An attribute (name or number) must always be specified.  
Specifying a string is only required when the 'FA command is to
be used as part of a command list, otherwise it is optional.  To
include leading or trailing spaces in the string, use the double
quotations.  Otherwise FutureNet will use the string from the 
first non-blank character to the last non-blank character.

The drawing is searched from the current graphics cursor 
position, left to right and top to bottom, for an alphanumeric 
field with the specified attribute and, if included, the 
specified character string.  If the graphics cursor is within 
an alpha field, the search begins at the top of that field.  
If the attribute and/or string is not found below the cursor, 
then the search will resume at the top of the drawing. 

Uppercase and lowercase occurrences of string are located, as 
well as partial strings at the beginning of the text field. 

The FutureNet drawing screen shifts to center on fields with 
the attribute/string combination that are located outside the 
currently viewed drawing area. 

Since the original command remains on the command line, it can 
be entered as often as required. 

If the requested attribute is not found, the message Not found 
appears.  The same message appears when a string is specified 
and the system finds the specified attribute but not one with 
a string match. 

To search and replace at the same time, use a command list of 
'FA attribute string (note that in a command list, both the 
attribute and string are required parameters) and either 'R or 
'I to search and replace as follows: 

'FA attribute string;'R ["]string["] 

searches for the attribute and string specified in 'FA 
attribute string command and replaces the current string with 
the string specified in the 'R string command. 

'FA attribute string;'I ["]string["] 

searches for the 'FA attribute string and replaces it with the 
'I string, incrementing the value specified by 'I. 

For example, when the 'I string is a circuit designator, 
entering 

'FA LOC UNNN;'I U1 

will find the first occurrence of the text field UNNN, with 
attribute LOC, and replace it with U1.  At the same time, the 
'I command increments the string specified in 'I string 
command from 'FA LOC UNNN;'I U1 to 'FA LOC UNNN;'I U2.  
Entering the command list again locates the next occurrence of 
an alphanumeric field with the LOC attribute and UNNN text 
field and inserts the incremented U value, and so on. 

'FA is also useful when looking for signal names causing 
errors in the Pinlist Generator or for finding component value 
fields that need to be filled in. 

If a double quote (") or backward slash (\\) is to be included 
in the string, precede the character with a backward slash.  
For example, to find the string 

"status\\pin1" 

use the following string: 

"\\"status\\\\pin1\\"" 

Related Commands 

'CH A

'D

'F

'I

'R

'S


### .F -  Create Functional Block 

Format 

.F w,h[,ws[,hs]]

Purpose 

Create a functional block symbol with a double outline.

Remarks 

w is the width of the symbol rectangle expressed in 
display units; h is the height of the rectangle expressed in 
display units.

ws is the distance from the sides to the symbol cell.  If ws 
is not specified, the system default of 5 display units is 
used. 

hs is the distance from the top and bottom to the symbol cell.
If hs is not specified, the system default of 3 display unit is 
used. 

Specifying 0,0 for ws,hs causes the symbol boundary to overlap 
the symbol cell. This means that wires and buses can be drawn 
right to the symbol cell, eliminating the need for pin stubs.  
However, if you prefer using pin stubs, the pin stub commands 
work with the .F command. 

The combined numeric value of w and two times ws (one ws per 
side) cannot exceed the size of the X-coordinate of the drawing. 
Similarly, the combined numeric value of h and two times hs 
cannot exceed the size of the Y-coordinate of the drawing.  If 
the value exceeds either of the drawing dimensions the following
message will be displayed: 

          Symbol boundary conflict 

and the command will be ignored. 

Compatibility Note:

Modifications were made in FutureNet 6.1 to allow block symbols
larger than previous versions of FutureNet.  Although block 
symbols larger than allowed previously will be compatible with
previous versions of FutureNet, they will not be recognized as
block symbols in those versions.  For exact definitions of the
older limits, see the documentation for that version of FutureNet.

Negative values are not meaningful for any of the parameters and 
zero (0) is only meaningful for ws and hs.  The following message
will appear for negative and zero values which are not appropriate:

	Value out of range

The symbol is created with the top left corner of the symbol 
cell positioned at the graphics cursor location. 

The symbol is created with the top left corner of the symbol 
cell positioned at the graphics cursor location.  A new 
reference number is assigned to the block symbol. 

If another symbol, an alphanumeric field, a line, or the 
drawing edge is in the area to be allocated to the new symbol 
cell, a symbol boundary conflict will prevent the symbol from 
being placed.  The message 

          Symbol boundary conflict 

will be displayed.  The conflict must be resolved before the 
new symbol can be entered into the drawing.  

.F uses symbol definition instructions to generate a symbol. 
Symbol definition instructions are described in the chapters 
on symbol definition in the FutureNet User Manual.  The list 
of instructions used to define the block symbol can be viewed 
and edited by entering Symbol Definition Mode (.s command).  
However, once a symbol has been edited in Symbol Definition 
Mode, a bit is set that prevents it from being edited using 
drawing mode commands, such as the Add/Replace Pin Stub 
commands.  In order to use these commands on symbols that have 
been edited in Symbol Definition Mode, enter the .SBS command 
prior to exiting Symbol Definition Mode. 

Caution: Modifying the block symbol definition list can cause
the block symbol to be unaccessible to the pin stub commands.

Related Commands 

.-

.-AI

.A

.B 

.D

.SBS 


### Format 

FAST [ON|OFF]

Purpose 

To select FAST or MENU mode.

Remarks 

FAST ON selects FAST mode, which operates as described below.

FAST OFF selects MENU mode, which allows access to the menus 
with the right mouse button. 

FAST without parameters toggles between the MENU and FAST 
modes. 

The FAST mode makes the most frequently-used drawing functions 
accessible from the mouse. 

The mouse in FAST mode differs from MENU mode in the following 
ways only: 

Left button              No difference 

Middle                   Add/Remove Interconnect Dot.  When 
                         the graphics cursor is on a /1 or /2 
                         type line, the middle button toggles 
                         an interconnect dot (/D). 

Right                    Erase Line Segment.  The right button 
                         erases the line segment at the 
                         graphics cursor (/ES). 

All other modes are accessible from the FAST mode, allowing 
you to perform move, copy and erase operations, and enter 
commands on the command line. 

The FAST mode cursor is the graphics cursor. 

You can access the menus from FAST mode without exiting FAST 
mode by entering MENU. 

Related Commands 

MENU

/D

/ES


### FILE -  Display Filenames 

Format 

FILE[S]

Purpose 

Display the name of the current drawing file.

Remarks 

Filenames are displayed in the FILE dialog box: 

In a structured design, FILE displays the names of all files 
accessed in the current session and their status since the 
last CLEAR, SAVEALL, or LOAD command.  The files are displayed 
in the following order: 

\*     Current higher level files The current path of files is 
      displayed from the root drawing to the current file, 
      excluding any drawing levels that were bypassed with the 
      #D command. 

\*     Current file The current file. 

\*     Other files modified Any other files in the drawing 
      structure that have been changed, but not yet saved. 

To the left of each filename the word SAVE is displayed for 
any file that was modified in this session.  Files that have 
been viewed but not modified have no status next to their 
name. 

If there are no filenames to display, or if the loaded file 
has a .bak extension, the following message appears: 

          No current filename 

Exit the FILE dialog box by either selecting the OK button, by 
pressing the left mouse button outside of the dialog box, or 
by pressing \<Esc>.

Related Commands 

\#D

\#L

\#R

\#U

SAVE

SAVEALL

LOAD 


### fn -  Start FutureNet 

Format 

fn [drawing\_file]

fn [@][command file]

Purpose 

Start FutureNet and, optionally, load the specified drawing 
or run a command file.

Remarks 

This command is entered from the operating system command line.
  If filename extensions are not used, FutureNet first assumes 
a command file extension (.cmd).  If the file does not exist with
a .cmd extension, FutureNet assumes a drawing file extension 
(.dwg).  To load files with extensions other than .cmd or .dwg, 
the extension must be specified explicitly.

If the @ character is used, the specified command file runs in 
EXEC mode.  Otherwise, the command file runs in AUTO mode.  
See the EXEC and AUTO commands for more details. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: You cannot run fn from a DOS shell started from a 
      FutureNet drawing session; you can lose files and 
      autologging. 

Related Commands 

AUTO

EXEC

NOTE

PAUSE

STOP

VIEW


### FN.CFG - Configuration File

FutureNet 6.00 introduced the concept of "Configuration File" to 
save settings in the Export Menu.  This concept was expanded by 
FutureNet 6.10, to allow saving of certain drawing session 
settings.  The general type of settings that can be saved are:

        Printer Settings (see PRINTOPT)
        Color Settings (see PALETTE)
        Operating Settings, such as UNDO (see PROFILE for below)
        Display Settings, such as GRID   
        Symbol Libraries (Update, Reference and System)
        Help File
        Default Drawing Size
        Default Grid Spacing
        Default Function Key Mappings
        Export Menu Settings (see EXPORT)

The configuration file is always called fn.cfg.  When FutureNet
is initiated, the file is searched for and the settings are read
from the file, if the file is found.  The file is searched using
the following search order:

        1) Current directory
        2) Directory from which FutureNet was run
        3) The directories on the system path defined by the
           environment variable "PATH".

If the file is not found, then the above settings will be set to
the system defaults as defined by the various commands which set
those settings and an empty configuration file is created in the
current directory.

Settings can be saved to the configuration file by using the Save
button in PROFILE, PALETTE and PRINTOPT dialog boxes and the Save
Export Configuration menu item in the EXPORT menu.


### .G -  Change Symbol Type (Symbol Definition Mode) 

Format 

.G

Purpose 

Change a symbol from a circuit type symbol to a graphic type 
symbol, or vice versa.

Remarks 

Not supported by FutureNet OEM products. 

There are two types of symbols, circuit and graphic.  Circuit 
symbols are electrically significant, graphic symbols are not. 
 The symbol cell boundaries and reference numbers are 
displayed in the drawing for circuit symbols, but not for 
graphic symbols.  Both types of symbols are created and 
handled the same by FutureNet, but post processors use them 
differently.  Graphic symbols are ignored by post processors. 

.G toggles the setting for a symbol. 

While the setting may not be immediately obvious in Symbol 
Definition Mode, when you re-enter drawing mode, cell 
boundaries and reference numbers are affected. 


### GRID -  Display Grid/Toggle Grid Snap 

Format 

GRID [ON | OFF]

GRID x[,y]

NOGRID

GRID SNAP [ON | OFF]

Purpose 

Toggle the grid setting.

Remarks 

GRID is valid in both drawing and Symbol Definition modes.

A grid is a framework of horizontal and vertical tic marks.  
Entering GRID without specifying any parameters toggles the 
grid display on and off.  A default grid of 3 x 3 display 
units is used when no grid size has been previously specified. 

A grid size other than the default can be selected by 
specifying just the x value or both the x and y values.  For 
example 

GRID x 

or 

GRID x,y 

If only x is specified, y assumes the value assigned to x. 

The grid specification is saved with a drawing.  If on, the 
drawing will be saved with the grid on; if off, the drawing 
will be saved with the grid off. 

The grid is displayed in full and intermediate zoom levels.  
The grid can be printed by specifying R (reference parameter) 
in the PRINTOPT command or selecting the Print Non-Printable 
Attributes checkbox in the PRINTOPT dialog box. 

Entering NOGRID turns off the grid display. 

GRID SNAP without any parameters toggles the snap-to-grid 
setting.  GRID SNAP [ON | OFF] enables or disables grid snap 
as specified. 

The snap-to-grid feature will constrain line drawing, symbol 
placement, and text placement to the current grid. 

Related Commands 

PRINTOPT

PROFILE


### HELP -  Read Online Command Reference   

Format 

HELP [ [ " ]help\_string [ " ] ]

Key

\<F1>

Purpose 

Accesses online help.

Remarks 

The HELP command provides quick access to an online command 
reference.  The text for the online reference is similar to 
the text for the written FutureNet Command Reference.

The HELP command is available in MENU, FAST or Symbol 
Definition modes. 

The default command reference file is fn.hlp, which is 
distributed with the FutureNet product.  FutureNet accesses 
this file during initialization, searching the system using 
the following pattern: 

\*     Current directory 

\*     Directory containing the FutureNet executable 

\*     Directories on the system PATH 

The help file can also be set by the HELPFILE command.  Only 
one command reference file can be active at any time. 

To invoke the HELP dialog box, type either HELP optionally 
followed by a string on which help is needed, or \<F1>.

The string can be any FutureNet command as well as some phrases
which describe the command.  If no string is provided, the HELP
dialog box appears with help on the most common commands.

The HELP dialog box contains the following items: 

\*     Available Topics  This data window contains a list of 
      all topics which can be accessed using the HELP command. 
       The list contains all FutureNet commands plus some 
      equivalent phrases which describe the commands. 

      If a command is selected on the command line, it is 
      highlighted in the topics data window.  

      To select a topic, use the scroll bars to the right of 
      the data window to scroll the list and select one of the 
      topics by placing the mouse cursor on the desired item 
      and clicking the left mouse button.  The command 
      reference text for the selected command will appear in 
      the lower data window. 

      To use the keyboard to select the item, see the 
      FutureNet User Manual for information on keyboard 
      equivalents for the mouse. 

\*     Topic Description  The lower data window will contain 
      the actual reference text for the chosen topic.  Use the 
      scroll bars to the right of the data window to scroll 
      the reference material. 

\*     Cancel  The cancel button is used to leave the HELP 
      dialog box and return to previous mode. 

You can also exit the HELP dialog box by pressing \<Esc> or 
clicking the mouse cursor outside of the dialog box.  

Related Commands 

HELPFILE


### HELPFILE -  Open Online Command Reference File 

Format 

HELPFILE help\_filename

Purpose 

Specifies a help file for use by the FutureNet HELP command.

Remarks 

The HELPFILE command can be used to set the current command 
reference filename for the HELP command.  

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note:  Command reference files are not ASCII files.  Special 
       tools are required to create a command reference file.  
       These tools can be made available upon request. 

The HELPFILE command requires a file specification.  The file 
specification is searched for using the following pattern: 

\*     File specification as given 

\*     Directory containing the fn executable file 

\*     Directories on the system PATH 

Only one command reference file can be active at any time. 

Related Commands 

HELP


### HELPSAVE -  Save Custom Menu or Help Screen 

Format 

HELPSAVE name

Purpose 

Save the current drawing as a custom menu or help screen to 
the update library. 

Remarks 

name is a required parameter. 

Custom menus and help screens are saved into the update 
library which is specified with the .LIB command.  If update 
library has not been specified the following error message 
will appear: 

          Update library not specified 

The ? command is used to retrieve the custom menus.  When the 
? command is used without a parameter, then the symbol ?HELP 
is retrieved.  To save this default menu, use the following 
HELPSAVE command: 

HELPSAVE ?HELP 

Refer to the  FutureNet User Manual for more information on 
creating custom menus and help screens. 

Related Commands 

?

.LIB


### HOME -  Home Cursor 

Format 

HOME

Keys 

PC | SUN 
---|----
\<Home> | \<R7>

Purpose 

Center the work area around the cursor location.

Remarks 

The cursor location does not change with this command.  In full
or intermediate zoom,  the display shifts so that the cursor
location is centered on the screen.

When the cursor is at, or very close to, the edge of the 
drawing, the cursor cannot be centered on the screen when HOME 
is entered, because the system does not move the display 
beyond the edge of the drawing. 

The HOME command has no effect in fit zoom. 

HOME can be entered in command files. 

Related Commands 

CURSOR

WINDOW

PAN


### HOME - (Symbol Definition Mode) 

Format 

HOME

Keys 

PC | SUN 
---|----
\<Home> | \<R7>

Purpose 

Reposition the display window with the current symbol editor 
coordinate marker location as the center of the graphics portion
of the screen.

Remarks 

Not supported by FutureNet OEM products. 

HOME, and \<Home> or \<R7> are primarily used with symbols too 
large to display on one Symbol Definition screen.

### 'I -  Insert Alphanumeric String and Increment Numbering 

Format 

'I ["]string["]

Purpose 

Insert an alphanumeric string in the field at the cursor location
and automatically increase the numeric part of the string by one.

Remarks 

A string must be specified.  To include leading or trailing spaces
in the string, use the double quotations.  Otherwise FutureNet 
will use the string from the first non-blank character to the last
non-blank character.

'I replaces an existing alphanumeric field or creates a new 
one. 

The number must be at the end of the string, for example 

'I U1 

After the command is entered, the command and the incremented 
numeric value are displayed on the command line, for example 

'I U2 

Moving the cursor and entering the command again causes the 
system to insert the incremented string at the new cursor 
location and repeat the process. 

This command is especially useful when labeling pin fields, 
signal lines, etc. 

'I will not increment multiple instances of numerics in 
strings; only the last instance is found.  For example, 
incrementing U1a1 will result in U1a2, not U2a2. 

If a double quote (") or backward slash (\\) is to be included 
in the string, precede the character with a backward slash.  
For example, to insert the string 

"status\\pin1" 

use the following string: 

'I "\\"status\\\\pin1\\"" 

Related Commands 

'F

'FA

'R


### .I -  Enter/Exit Insert Mode (Symbol Definition Mode) 

Format 

.I

Purpose 

Toggle symbol definition insert mode.

Remarks 

Not supported by FutureNet OEM products. 

In insert mode, an instruction entered on the command line is 
inserted above the instruction currently on the target line.  
When not in insert mode, the current target line instruction 
moves up and the instruction entered on the command line 
becomes the new target line instruction. 

In insert mode, the horizontal line below the target line 
appears as a dotted line. 

Related Commands 

.D


### ID -  Insert Dot Matrix (Symbol Definition Mode) 

Format 

ID row1,row2,row3,row4

Purpose 

Insert the specified four-by-four dot matrix at the current 
x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

The matrix is a four-by-four square of dots, where each dot 
may be on or off. The parameters, row1 through row4, identify 
the rows of the matrix; row1 is the top row, row2 is the next, 
etc.  The matrix is placed at the current x,y coordinates, 
beginning at row 1 dot 1, and moving to the right and down. 

Parameters are specified in hexadecimal (0 through F), but are 
translated by the instruction into their 4-bit binary 
equivalents 0000 through 1111, where 0s indicate dots that are 
not illuminated and 1s indicate dots that are illuminated.  
Hexadecimal and binary equivalents are provided in the table 
accompanying this instruction. 

Hexadecimal     Binary 
-----------     ------
        0       0000
        1       0001
        2       0010
        3       0011
        4       0100
        5       0101
        6       0110
        7       0111
        8       1000
        9       1001
        A       1010
        B       1011
        C       1100
        D       1101
        E       1110
        F       1111 

For example, the following matrix has all dots off: 

        0000 0000 0000 0000 

In hexadecimal, the command would be coded as 

        ID 0,0,0,0 

The following matrix has all dots on: 

        1111 1111 1111 1111 

In hexadecimal it would be coded as 

        ID F,F,F,F 

Once the matrix has been defined, you can use it repeatedly 
during the current design session.  The command 

ISxx 10 

inserts a copy of the current matrix at the location given in 
the ISxx command.  See the ISxx command for more information.  
Using the IS command instead of repeated ID commands will save 
memory. 

The symbol editor coordinate marker location is not changed by 
this instruction. 

Related Commands 

ISxx


### IG -  Insert Graphics Element (Symbol Definition Mode) 

Format 

IG number

Purpose 

Insert the specified graphics element at the current x,y 
coordinates.

Remarks 

Not supported by FutureNet OEM products. 

Number is a required parameter.  Refer to the appendix in the 
FutureNet User Manual that lists the available graphics 
elements by index number. 

FutureNet includes a built-in table of commonly used graphic 
elements, such as AND gates, OR gates, etc.  They are 
referenced by an index number.  When the IG instruction is 
entered, the specified symbol is displayed at the cursor 
location.  

Graphic elements are placed at the cursor location beginning 
at the top of the element and moving down and to the right.  
The symbol editor coordinate marker location will not be 
changed by this instruction. 

Related Commands 

ISxx


### \<Ins> and \<R11> Keys -  Insert Alphanumeric Character 

Keys 

PC |SUN 
---|-----
\<Ins> | \<R11>

Purpose 

Toggle alphanumeric insert mode.

Remarks 

The default setting is Insert Off.

Pressing \<Ins> or \<R11> toggles the setting.

The alphanumeric cursor is a solid underscore for Insert Off, 
and a dotted underscore for Insert On.  

Characters are inserted at the alphanumeric cursor location as 
they are typed. They are inserted according to the 
justification set for the current alphanumeric field (left, 
right, or center). 

Once the field encounters another drawing element (symbol, 
alphanumeric field, etc.), or the edge of the drawing or 
screen, no more characters can be inserted.  Move the 
alphanumeric field to allow the additional characters to be 
inserted. 


### ISxx -  Insert Symbol Element (Symbol Definition Mode) 

Format 

ISxx number

Where xx is 

TC | TR | RC | BR | BC | BL | LC | TL | C 

Purpose 

Insert the specified symbol element at the current x,y 
coordinates.

Remarks 

Not supported by FutureNet OEM products.

Number is a required parameter.  Refer to the appendix in the 
FutureNet User Manual that lists the available graphics 
elements by index number. 

FutureNet includes a built-in table of commonly used 
dot-matrix symbol elements, mostly arrows of varying shapes 
and sizes.  They are referenced by an index number.  When the 
IS instruction is entered, the specified symbol is positioned 
at the current x,y coordinates according to the arguments 
specified with the IS instruction.  

ISTL                     x,y at top left of symbol 

ISTC                     x,y at top center of symbol 

ISTR                     x,y at top right of symbol 

ISRC                     x,y at right center of symbol 

ISBR                     x,y at bottom right of symbol 

ISBC                     x,y at bottom center of symbol 

ISBL                     x,y at bottom left of symbol 

ISLC                     x,y at left center of symbol 

ISC                      x,y at center of symbol 

See the Select Symbol Element Table (ST) instruction for 
information on selecting symbol element tables. 

The symbol editor coordinate marker location is not changed by 
the instruction. 

Related Commands 

ID

IG

ST

RS


### 'J -  Set Justification 

Format 

'J [L | R | C]

Purpose 

Set the default justification to be assigned to future alpha-
numeric fields.

Remarks 

This command sets the default justification that will be assigned
to new alphanumeric fields.  The justification for existing 
alphanumeric fields is not affected.  The minimum entry is 'J, 
which cycles through the options, from left to right to center.  
The justification is shown on the third line of the ALPH status 
field.

Left, L, enters type to the right of the initial cursor 
position.  Right, R, enters type to the left of the cursor 
position.  Center, C, spaces the type evenly on either side of 
the initial cursor position. 

Note that the 'B command displays the field boundary and the 
point of justification, which is the dot appearing at the 
lower left, center, or right of the field, depending on the 
justification set for the field. 

Related Commands 

'B

'CH J


### /J -  Draw Junction Segment 

Format 

/J [direction] [,[size] [,type ] ]

Purpose 

Draw a curved or diagonal line segment.  

Remarks 

The /J options must be specified in order with commas (,) used 
as delimiters.  Direction can be specified without any other 
options.  If you want to specify size, you must either specify 
direction or use a comma; if you want to specify type, you must
either specify the direction and size or use commas.

Used to join signal wires to buses, as an alternative to 
orthogonal (right angle) signal/bus connections. 

Direction is the direction the segment is to follow from the 
current cursor position.  The direction can be 

DR (or RD)               Down and Right 

UR (or RU)               Up and Right 

DL (or LD)               Down and Left 

UL (or LU)               Up and Left (default) 

When drawing diagonal segments, the order of the direction 
indicators doesn't matter; that is, DR and RD are equivalent. 

When drawing curved segments, the order is important because 
it indicates the segment's initial direction.  For example, DR 
causes the segment to be drawn down and then right, that is, 
counterclockwise; RD causes the segment to be drawn right, 
then down, that is, clockwise. 

Size is the segment's size in display units.  It can be from 1 
through 10. 

Type is the type of segment you want drawn.  D is diagonal; C 
is curved. 

Any unspecified parameters default to the values specified the 
last time /J was used during the current session.  If /J has 
not been used during the current session, the defaults are 

/J UL,3,D 

Junctions lines are actually graphic symbols.  They have 
undisplayed square boundaries in the size specified and direct 
connections between opposite corners.  Other objects, such as 
alphanumeric fields, symbols, and lines cannot be drawn within 
these boundaries.  You should use symbol commands to 
manipulate them, (for example, .E to erase one), rather than 
line commands.  However, the line erasing commands /ES, /EL, 
and /EN usually work to erase the segments. 

Related Commands 

.E

/ES

/EL

/EN


### 'K -  Cancel Alphanumeric Field Tag 

Format 

'K

Purpose 

Cancel an alphanumeric field's tag and return to the previous 
mode.

Remarks 

The 'K command can be performed anytime during tag and drag, 
and is equivalent to pressing the right mouse button.

The location of the graphics cursor will be where the tag 
cursor was located at the time 'K was entered.  The tag cursor 
will disappear and the graphics cursor will appear in its 
place.  

Related Commands 

'C

'M

'E


### .K -  Cancel Symbol Tag 

Format 

.K

Purpose 

Cancel a symbol tag and return to previous mode.

Remarks 

The .K command can be performed any time during tag or drag of 
a symbol.  The tag will be canceled and the drawing will remain 
unchanged.  The tag cursor will disappear and the graphics cursor
will appear in its place.

Related Commands 

.C

.E

.M


### /K -  Escape Line Drawing 

Format 

/K

Purpose 

Escape line drawing and return control to the previous mode.

Remarks 

When /K is entered, the line drawing cursor is replaced by 
the graphics cursor.  The cursor location does not change.

All routing segments disappear. 

When line drawing is terminated, the system returns to the 
mode that was in effect when line drawing was initiated. 

The /K command is equivalent to the right mouse button during 
line drawing. 


### [K -  Cancel Area Definition or Area Tag 

Format 

[K

Purpose 

Erase the area definition or cancel tag and drag, depending 
on the system mode.

Remarks 

The area must already have been defined; see [D.  If 
not, the command is ignored.

[K erases the existing area definition boundary. 

When [K is entered or the right mouse button is pressed while 
in MOVE/COPY/ERASE mode, the mode is canceled and the system 
is returned to MENU or FAST mode.  Entering [K again cancels 
the area definition. 

Since only one area can be defined at a time, the cursor can 
be located anywhere in the drawing when [K is entered. 

Related Commands 

[C

[D

[M


### KEY -  Assign a Function to a Function Key 

Format 

KEY \<key> command(s)

Purpose 

Assign a function to one of the "F" function keys (F1 - F10).

Remarks 

\<Key> is the name of the function key, for example, 
\<F2>.  Angle brackets are required.

Command(s) is the function the key is to perform; any commands 
that can be used in a command file or command list can be 
assigned to a function key.  Command(s) can be up to 127 
characters long. 

For example, 

KEY \<F1>  .=;down 3 

will allow the \<F1> key to add bus pin stubs to a functional 
block and move down 3 display units, ready to add the next bus 
pin stub. 

If command(s) is omitted, KEY prompts with the current key 
definition, leaving the cursor at the start of the command 
string. 

This command updates the current user profile. 

Function key assignments are for the current editing session 
only, unless they are entered into an fnpro.cmd file. 

Related Commands 

PROFILE


### !LB, !MB and !RB -  Emulate a Mouse Button Click 

Format 

!LB

!MB

!RB

Purpose 

Allow emulation of a mouse click from the keyboard.

Remarks 

!LB emulates the left mouse button click.

!MB emulates the middle mouse button click. 

!RB emulates the right mouse button click. 

The commands are all valid in MENU or FAST drawing modes.  !RB 
brings up the Symbol Definition menus in Symbol Definition 
Mode. 

These commands are not operational in dialog boxes, such as 
PROFILE, PALETTE or DIR.  Use the key bindings described in 
the FutureNet User Manual for operation of a dialog box from 
the keyboard. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Early versions of FutureNet on the Sun used the commands 
      LB, MB and RB, instead of !LB, !MB and !RB.  These 
      commands are treated as synonyms. 


### #L and #R -  Move Left or Right in Design Hierarchy 

Format 

\#L

\#R

Purpose 

Move between neighboring sheets in a drawing set.

Remarks 

\#L and #R only allow movement between adjacent drawings in the 
same functional block drawing set, and only after the following
sequence of events has happened:

1. The cursor is located on one of the alphanumeric fields 
   associated with the drawing set (these fields must be in a 
   functional block symbol at the drawing level immediately 
   above the drawing set and the alphanumeric fields must have 
   a FILE or FILN attribute assignment). 

2. One of the files in the functional block drawing set was 
   accessed by entering \<Ctrl>\<PgDn>, \<Ctrl>\<R15> or #D without
   a filename.  If the functional block drawing set was accessed
   using #D filename, #L and #R will not work. 

Once one of the drawings in the functional block drawing set 
has been accessed, #L and #R can be used to move between 
adjacent files in the set. 

If there is no adjacent file to move to, the higher level 
drawing is redisplayed with the graphics cursor at the first 
or last file name in the list. 

To move to a file in the functional block drawing set that is 
not adjacent to the current file, enter #L or #R until the 
desired file is reached or return to the next higher level 
using #U and specify the desired file. 

These commands are provided for moving between adjacent files 
in a multifile drawing set. 

Related Commands 

\#D

\#U


### 'L -  Create, Edit or View Layered Text  

Format 

'L

Purpose 

Create a new property sheet or display an existing one for a 
single alphanumeric field in order to add, edit, or view layered 
text, or view all layered text entries of the same class for an 
entire symbol. 

Remarks 

Not supported by FutureNet OEM products.

Layered text is used to add drawing information required by 
design tools and translators.  Layered text is stored on what 
are called property sheets that are attached to displayed 
alphanumeric fields. 

Layered text may be added to the text associated with a 
symbol.  When the symbol is saved into a symbol library the 
layered text is saved as well. 

The purpose, use of, and syntax for layered text are explained 
in the chapter ``Understanding FutureNet'' in the FutureNet 
User Manual. 

Exiting the layered text dialog box saves the edits made to 
the property sheet.  The drawing must be saved before ending 
the session or loading another drawing in order for the new 
property sheet to be saved to the drawing file. 

Pressing \<Esc> will cancel the layered text dialog box, ignoring
changes made during the editing session.

The Layered Text Dialog Box 

When the 'L command is entered, the layered text dialog 
box appears.

What appears in the dialog box depends upon the location of 
the cursor  at the time the 'L command is entered: 

\*     If the cursor is on a text field, the layered text for 
      the class of objects defined by the text attribute class 
      is displayed. 

\*     If the cursor is not on a text field, but the cursor  is 
      within a symbol boundary, then the object is assumed to 
      be SYMBOL  and all symbol-related layered text for the 
      current symbol is displayed. 

\*     If the cursor is not on a text field and is not in a 
      symbol, then the object is assumed to be DRAWING and all 
      drawing-related text is displayed. 

The operation of the dialog box is discussed below. 

Attribute Class 

Attributes are divided into five classes.  (See the chapter 
``Understanding FutureNet'' in the FutureNet User Manual for 
complete explanations of the classes).  Depending upon the 
location of the cursor, the layered text entries displayed belong
to one of these classes: 

Pin                      Lists layered text for all text with 
                         pin-related attributes that share the 
                         point of effect of the selected text 
                         and are unambiguously associated with 
                         that pin. 

Signal                   Similar to PIN except that the text 
                         has been assigned signal-related 
                         attributes. 

Symbol                   Lists layered text for all text with 
                         symbol-related attributes whose point 
                         of effect falls within the symbol 
                         boundary, according to OVERLAP rules 
                         (see OVERLAP command).  Note that 
                         text with attribute COM(0) that has 
                         its point of effect within the symbol 
                         boundary is considered to be symbol 
                         related. 

Drawing                  Lists layered text for all text with 
                         drawing-related attributes.  Note 
                         that text with attribute COM(0) that 
                         has its point of effect outside of 
                         symbol boundaries is considered to be 
                         drawing related. 

Circuit                  Lists layered text for all text in 
                         the drawing that has a circuit 
                         (design) related attribute.  Note 
                         that the property sheet does not list 
                         text from other sheets in the drawing 
                         set. 

Displayed Text 

Lists all displayed text that has attributes in the given 
attribute class.  If the cursor was in a text field when 'L was
entered, that text is highlighted in the data window. Otherwise,
the first displayed text in the list is highlighted. The syntax 
for the display is

          attribute\_name attribute\_number text\_string 

You may select (highlight) any entry in this list, and the 
associated text in the Layered Text Property Sheet is 
selected. 

Layered Text Property Sheet 

Lists all layered text associated with the listed displayed text.
Entries in this list are either headings or layered text.  
Headings (text in a row of dots) cannot be edited.  Unless 
changed, selected text is the first layered text associated with
the current Displayed Text.  When a line in this list is selected,
the associated text in the Displayed Text list is also selected.

Layered Text Entry Field 

The Layered Text Entry Field is used to enter new layered text 
or edit existing layered text. In insert mode, text is entered 
below the selected entry in the property sheet. In edit mode, the
text that is selected in the property sheet is copied to the entry
field for editing.

Editing Mode 

Editing modes that are available are Delete (delete text), Edit 
(edit text) and Insert (add new text).  Default setting is Insert.

To edit text, select the Edit mode button, then select the text
to edit.  The text appears in the entry field.  Edit the text
in the entry field, and press \<Enter> when finished.  The text
is updated on the list.

To insert new text, select the Insert mode button.  Select the 
layered text item after which the new text should be inserted. 
Enter the text in the entry field, and press \<Enter> when 
finished.

To delete text, select the Delete mode button, then select the 
layered text to be deleted.  The selected text is removed from 
the list. 

Syntax Checking 

Two-state mode button that allows you to adjust the sensitivity 
of syntax checking.  Strong does not allow incorrect syntax 
to be inserted into the Layered Text Property Sheet; Weak allows 
incorrect syntax, but warns you of the problem.

The Message Line and Syntax Errors 

The Message line gives you information on how to correct 
the layered text entry if there is a syntax error.  Some common
syntax errors are shown below:  

\*     Layered text syntax requires a colon after tool 
      specification if the tool is specified.  A colon is not 
      allowed anywhere else, with the exception of the value 
      field. 

          Misplaced colon.  Entry ignored.  (strong) Possible 
          misplaced colon.  (weak) 

\*     Syntax requires a property string. 

          Missing property.  Entry ignored.  (strong) Possible 
          missing property.  (weak) 

\*     Syntax requires an equal sign following the property 
      string. 

          Missing equal sign.  Entry ignored.  (strong) 
          Possible missing equal sign.  (weak) 

OK Button 

Accepts all changes and returns to the drawing.

Reset Button 

Abandons all changes made during the current layered text 
editing session.

Cancel Button 

The Cancel button cancels the layered text modifications 
and returns to the drawing.

To cancel the dialog box without saving the changes, use \<Esc> 
or click the left mouse button outside of the dialog box.

Related Commands 

'LE

'LR


### 'LE -  Erase Layered Text 

Format 

'LE ["]string["]

Purpose 

Erase the specified layered text entry in the current alpha-
numeric field or symbol.

Remarks 

Not supported by FutureNet OEM products.

The purpose, use of, and syntax for layered text are explained 
in the chapter "Understanding FutureNet" in the FutureNet User 
Manual. 

The graphics cursor must be on the alphanumeric field or 
symbol containing the target entry.  The 'LE string needs only 
the information necessary to uniquely identify the entry.  If 
the layered text entry to be erased is tool specific, begin 
the string with the tool, then add property and value 
information as necessary to uniquely identify the entry. If 
the layered text entry is not tool specific, start with the 
property information and add the value if needed to uniquely 
identify the entry. 

'LE searches for the specified string and erases it.  If no 
string is specified or the string is not found, no text will 
be erased.  To include leading or trailing spaces in the 
string, use the double quotations.  Otherwise FutureNet will 
use the string from the first non-blank character to the last 
non-blank character. 

If the cursor is on a symbol and is either on a symbol-related 
alphanumeric field or on no alphanumeric field, then all 
symbol-related text in the symbol will be searched.  
Otherwise, if the graphics cursor is on an alphanumeric field, 
then only layered text associated with that field will be 
searched. 

To use a double quote (") or backward slash (\\) in the string, 
precede the character with a backward slash.  For example, to 
erase the string 

"status\\pin1" 

use the following string: 

'LE "\\"status\\\\pin1\\"" 

This command will not affect the displayed text field information.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Be careful, especially when entering this command within 
      a symbol, since only partial strings are necessary to 
      allow a match. 

Related Commands 

'L

'LR


### 'LR -  Replace/Insert Layered Text 

Format 

'LR ["]string["]

Purpose 

Replace an existing value within a string in a layered text entry
or insert a new layered text entry consisting of the string in 
the current alphanumeric field or symbol.

Remarks 

Not supported by FutureNet OEM products.

The purpose, use of, and syntax for layered text is explained 
in the chapter ``Understanding FutureNet'' in the FutureNet 
User Manual. 

String must be in the form of one of the following syntaxes: 

tool:property=value tool:property#value property=value 
property#value 

In order for the value to be replaced, an exact match must be 
made up to and including the = or # characters: 

1. If the cursor is on a symbol and is either on a 
   symbol-related alphanumeric field or on no alphanumeric 
   field, then all symbol-related text in the symbol is 
   searched. 

2. If the graphics cursor is on an alphanumeric field, then 
   only layered text associated with that field will be 
   searched. 

If string (not including the value) is found, the current 
value is replaced with the value specified in the string.  For 
example, if the field you are searching has the string 
tool:property=xx and you enter the command 'LR 
tool:property=yy, the value xx will be replaced with yy. 

If string is not found, then the entire string is appended to 
the list of layered text entries, according to these 
guidelines: 

1. If the cursor is in a symbol but not on an alphanumeric 
   field, the string is appended to the last symbol-related 
   text field. 

2. If the cursor is on an alphanumeric field, string is 
   appended as the last entry for the alphanumeric field. 

To include leading or trailing spaces, use double quotation 
marks.  Otherwise, FutureNet uses the first non-blank 
character to the last non-blank character.  If a double quote 
(") or backward slash (\\) is to be included in the string, 
precede the character with a backward slash.  For example, to 
replace the string "status\\pin1" use the string 'LR 
"\\"status\\\\pin1\\"" 

Related Commands 

'L

'LE


### .L -  Load Symbol from Library (\*) 

Format 

.L symbolname [,[d:] filename]

\* symbolname [,[d:] filename]

Purpose 

Load the specified symbol at the graphics cursor location.

Remarks 

.L loads the symbol at the graphics cursor location, if there
is not a boundary conflict detected (see below for definition
of boundary conflict).

\* loads a symbol in tag and drag mode.  The symbol is not placed
in the drawing until the left mouse button is pressed or a .C or
.M command is run.  If a command changes the mode of the drawing
sessions, the symbol will be cancelled and will not load into 
the drawing.


When filename is omitted, the libraries are searched for 
symbolname in the following order: 

1. Update library specified in the last .LIB command. 

2. Reference libraries specified by a LIB command. 

3. System symbol library, system.sym. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: The system.sym library is loaded when FutureNet is 
      initialized.  This library is either in the directory 
      specified by the FNLIB environment variable or in the 
      current directory.  This library contains a large subset 
      of FutureNet symbols which are supplied with the 
      FutureNet products. 

When filename is specified as a symbol library, only that 
library is searched. 

The symbol is placed in the drawing so that the upper left 
corner of the symbol cell is located at the graphics cursor. 

If a inserted symbol overlaps other symbol cells, alphanumeric 
fields, lines, or the edge of the drawing the symbol will not 
be placed in the drawing and will be placed in tag and drag 
mode, similar to the \* command.  The following message will 
appear and the area must be clear before the symbol can be 
inserted into the drawing:

          Boundary conflict 

Either the new symbol location  must be moved, or the 
interfering drawing elements must be moved or erased. 

Symbols inserted into the drawing are automatically assigned a 
reference number.  This number is displayed in the upper left 
corner of the symbol cell.  The reference number is used to 
identify the symbol to the system and may be used to find the 
symbol on the display screen using the N command (Move to 
Symbol Using Reference Number). 

Related Commands 

.DIR

.LIB

LIB


### .L - Print the Symbol Definition List (Symbol Definition Mode) 

Format 

.L

Purpose 

Print the instructions for current symbol.

Remarks 

Not supported by FutureNet OEM products.

This command is entered while in Symbol Definition Mode to 
print the symbol instruction list.  It is not entered as part 
of the instruction list. 

.L uses the print relevant options as set up in the PRINTOPT 
command. 


### .LIB -  Update Symbol Library .NOLIB

Format 

.LIB [d:]filename

.NOLIB

Purpose 

Specify the update symbol library file into which new symbol 
definitions are to be saved.

Remarks 

The filename extension .sym is automatically added unless 
an extension is explicitly specified.

If the file already exists, it is assumed to be a library to 
which additional symbols will be added; otherwise, a new file 
is created. 

Entering the .LIB command supersedes use of a library 
previously specified by .LIB, if any. 

Once an Update Symbol Library has been specified, use the 
.SAVE command to place symbol definitions in the library. 

Entering .NOLIB terminates use of the update symbol library. 

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: (For PC Users) If a symbol library is specified on a 
      diskette using the .LIB command, the diskette must not 
      be removed unless access to the library is properly 
      terminated with the .NOLIB command.  If this is not 
      done, further access to the library may read random data 
      from a subsequent diskette that could damage the drawing 
      in the FutureNet memory workspace. 

Related Commands 

LIB

.DIR

DIR

.SAVE

.DEL

.L


### /L -  Draw Lines 

Format 

/L

Key 

Left mouse button in MENU and FAST modes.

Purpose 

Begin and end line drawing in MENU or FAST mode.

Remarks 

When line drawing is initiated, the graphics cursor is replaced 
by the line drawing cursor, a large cross hair.  The line drawing
cursor is used to route line segments.  Up to two routing segments
can be defined at one time, the first of which must be set before 
another can be started.  Routing segments are dynamic and, until 
set, can be rerouted or abandoned altogether.  Setting a line 
segment requires re-entering the /L command or pressing the left
mouse button.

Routing segments cannot start or end inside a symbol or 
alphanumeric field, nor pass through either drawing element.  
If the graphics cursor is located in an invalid line drawing 
location, inside a symbol or alphanumeric field, the /L 
command is ignored. 

If there are any boundary conflicts between routing segments 
and other drawing elements, the routing segments cannot be 
set. 

The MODE status field shows LINE. 

Lines can be drawn at any zoom level. 

The type of line that will be drawn is shown in the LINE 
status field. 

/L can be used to connect or extend existing lines, provided 
the line types are the same. 

/L is equivalent to the left mouse button in line drawing 
mode. 

If line drawing is initiated but no line is drawn and the 
cursor is not moved, entering /L again will cancel line 
drawing mode. 

Related Commands 

/V

/LE

/E


### /LE -  Draw/Erase Line 

Format 

/LE

Purpose 

Begin or end line drawing or erase those portions of existing 
line segments (up to two segments) that coincide with new routing
segments.

Remarks 

The Draw/Erase Line command can be used to draw new lines 
or erase existing line segments.  /LE operates like the /E 
command if the routing segment coincides with an existing line, 
but draws a new line like /L if the routing segment does not 
coincide with an existing line.

/LE is useful when modifying existing drawings because it 
allows lines to be drawn and erased with the same command. 

Related Commands 

/E

/EL

/L


### [LOAD -  Load Area 

Format 

[LOAD filename[.ara]

Purpose 

Insert a previously saved area into the drawing at the graphics 
cursor.

Remarks 

The area must first have been saved using the [SAVE 
command.

Filename specifies the area file to be loaded into the 
drawing.  The system adds the extension .ara unless an 
extension is explicitly specified. 

The area is inserted into the drawing so that the upper left 
corner is positioned at the graphics cursor. 

The area loaded is automatically defined (marked by an area 
boundary), canceling any previously defined areas. 

The [LOAD command completes only if the graphics cursor is 
located in a portion of the drawing that is at least the same 
size as the area to be loaded, that is, clear of symbol cells, 
alphanumeric fields, and lines. 

If the saved area cannot be loaded due to a boundary conflict, 
the command is ignored and the system displays 

          Boundary conflict 

The conflict must be resolved before the command can be 
entered. 

Area definition can be canceled using [K. Loading another area 
or creating a new one will also cancel the current area 
definition, as will saving an area as a file. 

Load Drawings as Areas 

A drawing file with a .dwg extension can also be specified 
for the [LOAD command.  However, the whole drawing in the file 
must fit into the area in the current drawing for this command to
work.

If a drawing is loaded, instead of an area file, then the 
system-assigned reference numbers will be from bottom up, 
rather than top down. 

If a drawing-sized area is loaded, the area definition 
boundary may be difficult to see because of its proximity to 
the drawing edge; nevertheless, areas are loaded with their 
area definition boundary displayed.  

Related Commands 

[D

[SAVE


### LIB -  Specify Symbol Library for Reference NOLIB
 

Format 

LIB [filename[,filename][...]]

NOLIB [filename]

Purpose 

Specify symbol libraries to be accessed in addition to system.sym.

Remarks 

LIB allows symbols from libraries other than system.sym 
to be loaded with the \* or .L command. 

Entering the LIB command without specifying a library causes 
the system to prompt for the name of the library. 

NOLIB can be used to unload the libraries loaded by the LIB command. 
FutureNet 6.1 introduced the ability to specify a filename to unload.
Also, the system library can be unloaded, although a confirmation is
required.

Entering the NOLIB command without specifying a library causes 
the unload of all reference libraries.  The system library will NOT
be effected by this command.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: FutureNet allows you to specify up to ten reference 
      libraries.  However, if you are going to specify more 
      than one symbol library simultaneously, an adequate 
      number of files must be specified in CONFIG.SYS first 
      with the FILES=n command.  See the DOS manual for 
      instructions on how to change the number of accessible 
      files. 

Related Commands 

.DIR

.LIB

.L


### LOAD -  Load Drawing 

Format 

LOAD filename 

Purpose 

Load the specified drawing file into the system workspace.

Remarks 

Filename specifies the name of the drawing file to 
be loaded.

FutureNet will automatically look for a file with the .dwg 
file extension unless a different file extension is given. 

When a drawing is loaded, an automatic CLEAR command is 
performed, and the drawing is displayed in fit zoom.  Various 
line, symbol, and area tags are reset and the graphics cursor 
is positioned at the upper left corner of the drawing. 

The size of the drawing is reset to the size stored with the 
drawing.  If a grid was displayed when the drawing was stored, 
the same grid appears when the drawing is reloaded.  Note that 
the grid does not display in fit zoom. 

If the workspace contains a drawing that has been modified and 
not saved when you load a new drawing, the system displays 

          OK to discard changes that have not already been 
          saved (Y/N)? 

Y clears the workspace and loads the drawing. N cancels the 
LOAD command. 

When the workspace contains a drawing that has not been 
changed since it was loaded, verification is bypassed. 

In a structured design, the LOAD command ends the previous 
session with a CLEAR command before loading the requested root 
drawing.  It displays the same message as the CLEAR command.  
This means that drawings modified during a structured design 
session can be lost if they are not saved before the LOAD 
command is entered. 

If a structured drawing is already loaded, you can use the #U, 
\#D, #L and #R commands to traverse the drawing structure 
represented by that root. 

If no file was previously loaded, or if LOAD is immediately 
preceded by SAVEALL or CLEAR, no message is displayed, and the 
file is loaded as requested. 

Related Commands 

SAVE

DEL

CLEAR


### ls -  List Directory 

See DIR. 


### 'M -  Move Alphanumeric Field 

Format 

'M

Purpose 

Move an alphanumeric field.

Remarks 

Place the cursor within the target field's boundary, and enter 'M
or press the left mouse button until the field is tagged.  When 
entered, the MODE status field changes from the current mode to 
MOVE and the graphics cursor becomes the tag cursor.

When the tag cursor is moved, a dotted boundary detaches from 
the field and can be moved around the drawing using the mouse 
or cursor movement commands.  The tagged field retains a 
highlighted boundary.  The ghosted boundary aids in locating 
the text field so that it can be positioned without causing a 
boundary conflict.  You can also use 'B to display boundaries 
of all alphanumeric fields.  On monochrome systems, BLINK 
causes the tagged field to blink so that it is easier to see.  
On color systems, the tagged field changes color. 

When 'M is entered again, the field is moved into the new 
cursor location, assuming there are no boundary conflicts with 
other symbol cells, alphanumeric fields, lines, or the edge of 
the drawing.  To resolve boundary conflicts, move the copy to 
a valid location, or move or erase conflicting objects. 

After the field is moved, tag and drag is canceled, control 
returns to the previous mode, and the graphics cursor is 
restored. 

Related Commands 

'B

'C

'K

BLINK


### .M -  Move Symbol 

Format 

.M

Purpose 

Move the tagged symbol to the current cursor location.

Remarks 

When the symbol to be moved is not tagged, first move the cursor
into the symbol and enter the .M command or press the left mouse
button as required to tag the symbol and enable MOVE/COPY/ERASE 
mode.

When the symbol to be moved is tagged and the cursor is moved, 
the boundary detaches from the symbol and moves around the 
drawing with the mouse or cursor movement commands.  The 
tagged symbol boundary and cursor blink on monochrome systems 
or change color on color systems so that the symbol and its 
new location are obvious.  Entering the .M command again or 
pressing the left mouse button moves the symbol to the current 
cursor location assuming there are no boundary conflicts. 

A symbol cannot be moved to a location where its cell overlaps 
another symbol, the edge of the drawing, an alphanumeric 
field, or a line.  If the move results in interference, the 
command is ignored and the system displays 

          Boundary conflict 

The boundary conflict must be resolved before the move can be 
successfully completed. 

The symbol reference number moves with the symbol. 

Whenever a symbol with connections is moved, the connections 
are maintained only if connection maintenance is enabled with 
the CONNECT command. If a two-segment line can be drawn 
connecting the points without conflicts, then it is drawn as a 
permanent line. Where there are conflicts, the connections are 
drawn as temporary lines.  

The area previously occupied by the symbol is left blank.  
When the move is complete, MOVE/COPY/ERASE mode is canceled, 
and control returns to the previous mode. 

Related Commands 

.C

.K

.E

.L

CONNECT


### [M -  Move Area 

Format 

[M

Purpose 

Move the tagged area.

Remarks 

The area must already have been defined; see [D.  

When [M is entered, the area is tagged and a boundary detaches 
from the area.  The boundary can be moved using the mouse or 
cursor movement commands to the desired location.  Entering [M 
again or pressing the left mouse button sets the area in its 
new location, assuming there are no boundary conflicts. 

Boundary conflicts are caused by other symbol cells, 
alphanumeric fields, lines, or the edge of the drawing. 

If the tagged area cannot be moved due to a boundary conflict, 
the command is ignored and the system displays 

          Boundary conflict 

Resolve the conflict by moving or erasing the conflicting 
drawing elements or repositioning the area. 

Connection Maintenance 

When an area is moved with the CONNECT command enabled, existing
signal lines that connect to elements outside the area boundary 
are maintained. Signal lines are maintained from the point at 
which they cross the area boundary to their point of connection
outside the area.  When the area is moved and set in place, 
connections that can be drawn without boundary conflicts are made
permanent; connections that conflict with other drawing elements
are drawn as temporary lines.

When an area is moved with the CONNECT command disabled, 
existing signal lines that connect to elements outside the 
area boundary are broken at the area boundary when the area is 
moved. 

Symbols and alphanumeric fields are treated as part of the 
tagged area only if they are completely within the area 
boundary.  Symbols and alphanumeric fields intersecting the 
area boundary are excluded from the move. 

Related Commands 

[C

[D

[K

CONNECT


### Format 

MENU

Purpose 

The MENU command brings up the menus.

Remarks 

The MENU command brings up the menus in MENU mode, FAST mode and
Symbol Definition Mode.  If you are in FAST mode, the MENU 
command brings up the menus without exiting FAST mode.

Related Commands 

FAST


### MOVEPOEE -  Enable/Disable Point of Effect Movement for Electrical Items  

Format 

MOVEPOEE [ON | OFF]

Purpose 

Move alphanumeric fields and their points of effect independently
of each other or keep them together.

Remarks 

This command applies to alphanumeric fields that have electrical 
significance, for example, pin and signal name fields.

MOVEPOEE applies when tagging and moving 

\*     an alphanumeric field 

\*     a symbol that an alphanumeric field's point of effect is 
      located in 

\*     an alphanumeric field or symbol as an area. 

If MOVEPOEE is ON, the point of effect returns to its default 
location within the alphanumeric field. 

If MOVEPOEE is OFF, the point of effect does not change, as 
long as the field is within 127 drawing units of the point of 
effect location.  If the alphanumeric field is more than 127 
drawing units from the point of effect, the point of effect 
will return to its default location. 

The default setting is ON. 

Entering MOVEPOEE without specifying a parameter toggles the 
setting. 

When moving a symbol, electrically significant fields located 
within the symbol whose points of effect are also located 
within the symbol move with the symbol regardless of the 
MOVEPOEE setting.  If a field's point of effect is outside the 
symbol, or the alphanumeric field is outside the symbol, the 
MOVEPOEE command works as described above.  If on, and either 
the field or symbol is moved, the point of effect returns to 
its default location within the alphanumeric field.  If off, 
the point of effect remains at its assigned location. 

For non-electrically significant items, see the MOVEPOEG 
command. 

Related Commands 

MOVEPOEG

'P

'PD

POEDISP

POER


### MOVEPOEG - Enable/Disable Point of Effect Movement for General Items  

Format 

MOVEPOEG [ON | OFF]

Purpose 

Move non-electrically significant alphanumeric fields and 
their points of effect independently or keep them together.

Remarks 

This command applies to alphanumeric fields that have non-
electrical significance, for example, comment and filename fields.

MOVEPOEG applies when tagging and moving 

\*     an alphanumeric field 

\*     a symbol that an alphanumeric field's point of effect is 
      located in 

\*     an alphanumeric field or symbol as an area. 

When MOVEPOEG is ON, the point of effect returns to its 
default location within the alphanumeric field. 

When MOVEPOEG is OFF, the point of effect does not change, as 
long as the field is within 127 drawing units of the point of 
effect location.  If the alphanumeric field is more than 127 
drawing units from the point of effect, the point of effect 
will return to its default location. 

The default setting is ON. 

Entering MOVEPOEG without specifying a parameter toggles the 
setting. 

When moving a symbol, non-electrically significant fields 
located within the symbol whose points of effect are also 
located within the symbol move with the symbol regardless of 
the MOVEPOEG setting.  If a field's point of effect is outside 
the symbol, or the alphanumeric field is outside the symbol, 
the MOVEPOEG command works as described above.  If on, and 
either the field or symbol is moved, the point of effect 
returns to its default location within the alphanumeric field. 
 If off, the point of effect remains at its assigned location. 

For electrically significant items, see the MOVEPOEE command. 

Related Commands 

MOVEPOEE

'P

'PD

POEDISP

POER


### MX, MY - Move Coordinate in Display Units (Symbol Definition Mode)

Format 

MX distance

MY distance

Purpose 

Change the cursor location by specifying the distance to 
move the x or y coordinates in display units.

Remarks 

Not supported by FutureNet OEM products. 

Distance is in display units. 

Subsequent instructions originate at the new cursor location. 

Distance may be assigned any value from +127 through -128.  If 
distance is between +15 and -16, use the MXS and MYS (Move 
Short instructions) to save memory. 

MX distance changes the x coordinate cursor value.  If distance
is negative, the x coordinate moves left.  If distance is 
positive, the x coordinate moves right.  The y coordinate is
not changed by the MX instruction. 

MY distance changes the y coordinate cursor value.  If distance
is negative, the y coordinate moves up.  If distance is 
positive, the y coordinate moves down.  The x coordinate is 
unchanged by the MY instruction. 

The symbol editor coordinate marker moves in the direction and 
the distance specified by the instruction. 

Related Commands 

MXS

MYS

MXD

MYD


### MXD, MYD - Move Coordinate in Dot Units (Symbol Definition Mode) 

Format 

MXD distance

MYD distance

Purpose 

Change the cursor location by specifying the distance to 
move the x or y coordinates in dot units.

Remarks 

Not supported by FutureNet OEM products. 

Distance is in dot units. 

Subsequent instructions originate at the new cursor location. 

Distance may be assigned any value from +127 through -128.  If 
distance is between +15 and -16, use the MXS and MYS (Move 
Short instructions) to save memory. 

MX distance changes the x coordinate cursor value.  If distance
is negative, the x coordinate moves left.  If distance is 
positive, the x coordinate moves right.  The y coordinate is not
changed by the MX instruction. 

MY distance changes the y coordinate cursor value.  If distance
is negative, the y coordinate moves up.  If distance is positive,
the y coordinate moves down. The x coordinate is unchanged by 
the MY instruction. 

The symbol editor coordinate marker moves in the direction and 
the distance specified by the instruction. 

Related Commands 

MX

MY

MXS

MYS

MXY


### MXS, MYS - Move Short in Display Units (Symbol Definition Mode) 

Format 

MXS distance

MYS distance

Purpose 

Change the cursor location by specifying the distance to move 
the x or y coordinate in display units (use for short moves, 
between +15 and -16 display units).

Remarks 

Not supported by FutureNet OEM products. 

Distance is in display units. 

If distance is between +15 and -16, use this instruction to 
save memory.  For larger distance values, use the MX and MY 
instructions. 

MXS distance changes the x coordinate cursor value.  If 
distance is negative, the x coordinate moves left.  If 
distance is positive, the x coordinate moves right.  The y 
coordinate is not changed by the MXS instruction. 

MYS distance changes the y coordinate cursor value.  If 
distance is negative, the y coordinate moves up.  If distance 
is positive, the y coordinate moves down. The x coordinate is 
unchanged by the MYS instruction. 

The symbol editor coordinate marker moves in the direction and 
the distance specified by the instruction. 

Related Commands 

MX

MY

MXD

MYD

MXY


### MXY -  Move to New Coordinates in Display Units  (Symbol Definition Mode) 

Format 

MXY x,y

Purpose 

Move the cursor in display units the specified distance from 
the x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

Both x and y must be specified. 

X and y are specified in display units.  They may be assigned 
any value from +127 through -128. 

MXY moves the x and y coordinates relative to the current x,y 
coordinates. 

Specifying x in negative numbers moves the x coordinate left; 
specifying x in positive numbers moves the x coordinate right. 

Specifying y in negative numbers moves the y coordinate up; 
specifying y in positive numbers moves the y coordinate down. 

The figure accompanying this instruction shows which direction 
the possible combinations of positive and negative numbers 
will move the x,y coordinates. 

The symbol editor coordinate marker moves in the direction and 
the distance specified by the instruction. 

Related Commands 

MXYD

MXYA


### MXYA - Move to Absolute x,y Coordinates (Symbol Definition Mode) 

Format 

MXYA x,y

Purpose 

Move the cursor to the specified x,y coordinates.

Remarks 

Not supported by FutureNet OEM products. 

X and y are specified in display units.  They may be assigned 
any value between +127 and -128. 

MYXA moves the cursor to the specified x,y coordinates.  It 
does not move relative to the current location, but the 
specified distance from the upper left corner of the editing 
area: x,y equal to 0,0. 

Specifying x in negative numbers moves the x coordinate left; 
specifying x in positive numbers moves the x coordinate right. 

Specifying y in negative numbers moves the y coordinate up; 
specifying y in positive numbers moves the y coordinate down. 

The figure accompanying this instruction shows which direction 
the possible combinations of positive and negative numbers 
will move the x,y coordinates. 

The symbol editor coordinate marker moves in the direction and 
the distance specified by the instruction. 

Related Commands 

MXY

MXYD


### MXYD -  Move to New Coordinates in Dot Units  (Symbol Definition Mode) 

Format 

MXYD x,y

Purpose  

Move the cursor in dot units to the specified x,y 
coordinates, starting from the current cursor location.

Remarks 

Not supported by FutureNet OEM products. 

Both x and y must be specified. 

X and y are specified in dot units.  They may be assigned any 
value from +127 through -128. 

MXYD moves the x and y coordinates relative to the current x,y 
coordinates. 

Specifying x in negative numbers moves the x coordinate left; 
specifying x in positive numbers moves the x coordinate right. 

Specifying y in negative numbers moves the y coordinate up; 
specifying y in positive numbers moves the y coordinate down. 

The figure accompanying this instruction shows which direction 
the possible combinations of positive and negative numbers 
will move the x,y coordinates. 

The symbol editor coordinate marker moves to the x,y location 
specified by the instruction. 

Related Commands 

MXY

MXYA

### 'NAME -  Assign a Name to an Unused Attribute

Format

'NAME number name

Purpose

Assign names to unassigned attributes.

Remarks

Both name and number are required parameters.

Note that this command is for users who are writing custom
translators. You cannot take an unassigned attribute and
define it to be a signal or pin attribute and have FutureNet
and the post processors treat it as such.  This command allows
entry of information that FutureNet and the post processors do
not use, but will pass on through to a custom post processor
via the pinlist.

For example, an unassigned attribute, such as a cost field,
can be defined by the user.  Any text entered in the field
would specify the cost for the item the field was associated
with.  A custom post processor would read these fields in the
pinlist and total the cost for the item represented by the
schematic.

Number is the number of the attribute you wish to assign a
name to.  number must be an unused attribute.  Attribute
numbers range from 0 to 255.  Unassigned attribute numbers
are:

\*     Unused signal-related attributes: 15 to 19

\*     Unused pin-related attributes: 30 to 49

\*     Unused unassigned attributes: 55 to 99, 108 to 113,
      116 to 127, 131, 133, 135, 140, 142, 143, 170 to 255

Name is the name (or mnemonic) you wish to assign the
specified  attribute number. A name can be 1 to 4 characters
long and cannot match any of the existing attribute names.
See the chapter "Understanding FutureNet" in the FutureNet
User Manual for more information on attributes.

Unless this command is set up in the fnpro.cmd file, the
setting is only active during the current editing session.

Related Commands

'A

'S

'CH A


### N -  Move to Symbol Using Reference Number

Format

N number

Purpose

Move the cursor to the specified symbol.

Remarks

Each symbol is automatically assigned a reference number
when it is loaded in the drawing. N number moves the
cursor to the upper left corner of the specified symbol.

In fit zoom, the cursor moves to the symbol.  In full and
intermediate zoom, if the symbol is located off the screen,
the display is updated so that the cursor at the symbol is
centered on the screen.

If there is no symbol with the specified reference number, the
cursor is not moved, and the message line displays

          Not Found

Note that only electrically significant symbols have reference
numbers.  Graphic Symbols, which can be created using the .G
symbol definition command, are not found with this command.

Related Commands

RENUM

.G


### NOTE -  Insert Prompt

Format

NOTE [ ["]text ["] ]

Purpose

Display a prompt or comment on the command line while a command
file is running (with either AUTO or EXEC).

Remarks

Command file only.

NOTE is used in command files to add user prompts or to
display comments, like information about the previous or next
command.

Comments are limited to 80 characters, including NOTE and any
spaces.  To include leading or trailing spaces in the text,
use the double quotations.  Otherwise FutureNet uses the
string from the first non-blank character to the last
non-blank character.

If a double quote (") or backward slash (\\) is to be included
in the string, precede the character with a backward slash.
For example, to display the string

"status\\pin1"

use the following string:

NOTE "\\"status\\\\pin1\\""

Related Commands

AUTO

EXEC


### 'O -  Set Default Orientation for Alphanumeric Fields

Format

'O [H | V]

Purpose

Set the default orientation, horizontal (H) or vertical (V),
for future alphanumeric fields.

Remarks

The orientation can be either horizontal or vertical.

Horizontal text reads from left to right.

Vertical text reads from bottom to top.

Entering 'O without specifying an orientation toggles the
current setting.

Related Commands

'CH O


### 'OVER -  Add/Delete Overbar to/from Alphanumeric Field (Signal Inversion)

Format

'OVER [ON | OFF]

Purpose

Add an overbar to the current alphanumeric field to indicate
signal inversion or delete an existing overbar.

Remarks

Entering 'OVER without a parameter adds an overbar to the current
alphanumeric field if one is not present and deletes one if it is
present.

Position the graphics cursor on the target text field and
enter the command. Use in base mode on existing alphanumeric
fields.

Spaces within the text field will not cause a break in the
overbar.

'OVER can only be entered on an existing field.

While in alphanumeric mode, an overbar can be added or deleted
by using the \<Ctrl>\<O> keystroke sequence.

When an existing field that already has an overbar is edited,
the overbar will be added or deleted as text is added or
deleted.

Related Commands

'UNDER


### OVERLAP - Allow Alphanumeric Fields to Overlap Symbol Boundaries

Format

OVERLAP [ON | OFF]

Purpose

When ON, alphanumeric fields may overlap symbol boundaries.

Remarks

Entering OVERLAP toggles the current setting; entering
OVERLAP ON or OVERLAP OFF sets the value directly.  OFF
is the default.

When overlap is ON, a symbol "owns" any alphanumeric field
whose point of effect is inside or on the border of the symbol
boundary, even when the alphanumeric field lies partially or
entirely outside the symbol boundary.  The following are also
true:

\*     Alphanumeric fields associated with the symbol are saved
      (.SAVE) or loaded (.L) with the symbol, and are moved
      (.M), copied (.C), rotated (.R), or reflected (.RE) with
      the symbol.

\*     The relationship of the alphanumeric field and the
      symbol remain constant during all operations involving
      the symbol, regardless of the MOVEPOEE/MOVEPOEG state.

\*     Alphanumeric fields are only permitted to overlap symbol
      boundaries, not lines or other alphanumeric fields.

\*     When symbols are tagged for any operation, boundaries
      are checked for any alphanumeric field that is outside
      the symbol boundary.

The only time that alphanumeric fields are not "owned" is when
one or more of the following are true:

\*     Both the point of effect and the text origin of the
      alphanumeric field are shared by two or more symbols.

\*     The point of effect resides in two or more symbols and
      the text origin resides in another.

\*     When the point of effect is shared by two symbols, and
      the alphanumeric field is outside both symbols, the
      field will be considered to be owned by the tagged
      symbol.

The above rules do not apply to AREA manipulations, even in
OVERLAP mode.


### 'P -  Set Point of Effect

Format

'P

Purpose

Move an alphanumeric field's point of effect in order to
associate it with an electric or graphic item.

Remarks

To move an alphanumeric field's point of effect, place the
graphics cursor on the target field and enter the 'P command.
The MODE status field will display PTR.  Move the point of effect
cursor using any method of cursor movement and re-enter the 'P
command to set the point of effect at the new location.

Related Commands

MOVEPOEE

MOVEPOEG

'PD

POER


### 'PD -  Reassign a Point of Effect to Its Default Location

Format

'PD

Purpose

Restore the current alphanumeric field's point of effect
to its default location.

Remarks

'PD will return the point of effect for the current
alphanumeric field to its default location.

The default location for the point of effect is determined by
the class of the attribute assigned to the text field, using
the following rules:

For Signal-related Attributes

If POER is enabled, then the point of effect defaults
to the nearest line within 10 display units.

If POER is disabled or no line is within 10 display units of
the text field, then the point of effect defaults to the
justification point of the field.

For Pin-related Attributes

If the alphanumeric field is within a symbol, then the point of
effect defaults to the side of the symbol which is closest to the
alphanumeric field, and is in the same orientation as the field.
For horizontal text, the appropriate sides are the left and right.
For vertical text, the appropriate sides are the top and bottom.

If the text field is outside of a symbol, then the default
point of effect location is the justification point of the
field.

For Other Attributes

The point of effect defaults to the justification point of
the field.

The graphics cursor must be on the alphanumeric field that
owns the point of effect that is to be returned to its default
location.

This command is probably most useful for returning the point
of effect to its alphanumeric field when the alphanumeric
field has been moved or when a graphic item has been placed
between an alphanumeric and its point of effect.

Related Commands

'CH J

'J

MOVEPOEE

MOVEPOEG

'P

POER


### 'PRINT -  Print All Text for Alphanumeric Field

Format

'PRINT [opt1][,opt2]...[,optn]

Purpose

The 'PRINT command allows you to output all the text for a
particular alphanumeric field, including layered text, to an
ASCII file.

Remarks

Move the graphics cursor onto the field to be printed and enter
the 'PRINT command.  This command uses the print options
specified in the PRINTOPT command except the Strip option.  You
can override any options by specifying them in the 'PRINT command.

See the PRINTOPT command for the options available.

Use the PRINTOPT command to check your current print defaults.

A minus sign (-) following any text field in the printout
indicates an overbar.

Related Commands

.PRINT

PRINT

PRINTOPT


### .PRINT -  Print All Text for Symbol

Format

.PRINT [opt1][,opt2]...[,optn]

Purpose

The .PRINT command allows you to output all the text for a
particular symbol, including layered text, to an ASCII file.

Remarks

Move the graphics cursor onto the symbol to be printed and enter
the .PRINT command.  This command uses the print options specified
in the PRINTOPT command except the Strip option.  You can override
any options by specifying them in the .PRINT command.

See the PRINTOPT command for the options available.

Use the PRINTOPT command to check your current print defaults.

A minus sign (-) following any text field in the printout
indicates an overbar.

Related Commands

'PRINT

PRINT

PRINTOPT


### /P -  Convert Temporary Lines to Permanent

Format

/P

Purpose

Convert the temporary line the cursor is on to a permanent line.

Remarks

The cursor must be on the temporary line that is to be converted
to a permanent line.  The /P command will trace and convert
up to three temporary segments, assuming no illegal line routing.

Related Commands

/ET


### PALETTE -  Assign FutureNet Colors

Format

PALETTE

Purpose

Select the colors for the FutureNet display; assign colors
to selected components of the FutureNet environment.

Remarks

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: If you use a monochrome monitor you have no choices to
      make.  FutureNet Group Color 5 and the background must
      be white.  All other choices must be black.

Entering the PALETTE command causes a dialog box to appear
that lets you choose 8 FutureNet colors from a palette of 16,
and then assign any one of those 8 colors to 28 components of
the FutureNet display.  The dialog box appears below.

The Palette area contains the range of available colors.

The Palette color boxes contain the colors that are used for
all FutureNet display items.  Up to 8 colors may be used.  To
assign a Palette color, click the left mouse button on the
color of your choice in the Available Colors for Palette.  The
cursor changes to a hollow box in that color.  Place the
cursor on the appropriate Palette box and press the left mouse
button again.  All items assigned to that Palette group change
color. Alternatively, you can press the left mouse button when
the pointing cursor is located inside of a Palette box.  The
box and its associated items cycle through the colors in the
Palette.  To return the cursor to the pointing cursor, select
the Reset Cursor button.

Display components may be assigned to a color group in two
ways.  The first method is to pick up a Palette color and
assign it to an item.  Do this by pressing the left mouse
button when the pointing cursor is in the appropriate Palette
box.  The cursor changes to solid block in that color.  Place
the cursor in the color box of the item to be assigned and
press the left mouse button again.  The item's color changes
to that of the new Group.  To return the cursor to the
pointing cursor, select the Reset Cursor button.  The second
method is to place the pointing cursor on the color box of the
item itself.  Press the left mouse button to cycle through the
colors available in the Palette.

If you attempt to place a color from an Available Colors box
in an item color box, the cursor changes to a stop sign.
Click on Reset cursor to return to the pointing cursor.

The Background box cannot be assigned a different Palette
color.  You can only change the background color by changing
the color of the fifth Palette box.

Action Button Choices

Reset Cursor - Returns any Palette cursor to the pointing
               cursor.

Reset Colors - Returns Palette colors to their previous
               assignments.

Reset Default - Returns Palette colors to the program default
                colors.

OK - Saves the changes made, exits the Palette dialog box,
     updates graphics if necessary, and returns to the drawing
     screen.  If your changes aren't reflected on the screen,
     refresh the screen by using the REFRESH command.

Save - Saves palette changes to the configuration file, fn.cfg.
       The palette is saved under the heading [SYSTEM.COLORS] and
       [SYSTEM.DISPGRPn] were n can be 1,2,3 or 4.

       See HELP FN.CFG for information on the configuration file.

Cancel - Leaves the Palette dialog box, cancelling any changes
         done.

You can also cancel the Palette dialog box and discard the
changes, by pressing the \<Esc> key or clicking the cursor outside
the dialog box.

Related Commands

DISPGRPn

COLOR


### PAN -  Pan to Window Locations

Format

PAN 1 | 2 | 3 | 4

Purpose

Move to the specified window.

Remarks

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Window locations must first be saved using the WINDOW
      command.

A number is required.

PAN rapidly displays a preset window location. Preset window
locations will appear in full zoom, regardless of the zoom
level in effect at the time PAN is entered.

If PAN is used and no window with the specified number exists,
the command is ignored and the message line displays

          Pan not set

Related Commands

WINDOW


### PAUSE -  Change from Automatic to Single-step Execution

Format

PAUSE

Purpose

Change command file execution from automatic (AUTO)
to single-step (EXEC).

Remarks

To enter, type PAUSE on the command line and press \<Enter> while
\<AUTO> execution is taking place.

The PAUSE command can also be entered in a command file.

The PAUSE command suspends AUTO execution.  The system goes
into single-step EXEC mode, displaying each command on the
command line.

Resume automatic file execution by entering the AUTO command
without specifying a filename.

Related Commands

AUTO

EXEC

STOP


### PgUp or R9 Keys - Move to Beginning of Command List PgDn or R15 Keys - Move to End of Command List (Symbol Definition Mode)

Keys

PC | SUN
---|----
\<PgUp> |\<R9>
\<PgDn> |\<R15>

Purpose

Move the beginning or end of the symbol definition instruction
list to the target line.

Remarks

Not supported by FutureNet OEM products.

\<PgUp> or \<R9> moves the first symbol definition instruction line
into the target line.

\<PgDn> or \<R15> makes the target line the one after the last
symbol definition instruction line.  When the line after the last
symbol definition instruction is positioned as the target line,
additional symbol definition instructions entered are added to
the end of the list.

Note that \<PgUp>, \<PgDn>, \<R9> and \<R15> have different functions
when not in Symbol Definition Mode.  See ZIN/ZOUT commands.


### PINSNAP -  Snap Line to Pin

Format

PINSNAP [ON | OFF]

Purpose

Enable or disable pinsnap.

Remarks

Pinsnap is the automatic connection of lines that come within
two display units of a pin-related text point of effect.

If no parameter is specified, the command toggles the current
condition.

Enabled is the default setting.

Related Commands

PROFILE


### POEDISP -  Display Points of Effect

Format

POEDISP [ON | OFF]

Purpose

Display points of effect.

Remarks

POEDISP displays the points of effect for all alphanumeric fields
in the drawing.  A line will connect the point of effect and the
field.

Points of effect can be moved while POEDISP is ON and the line
that associates a point of effect with its alphanumeric field
will track the point of effect as you move it.

The default setting is OFF.

Entering POEDISP without any parameters toggles the setting.

Related Commands

'P

'PD

MOVEPOEE

MOVEPOEG

POER

PROFILE


### POER -  Point of Effect Range

Format

POER [ON | OFF]

Purpose

Toggle the snap-to feature for signal-related alphanumeric
field points of effect.

Remarks

When ON, the points of effect for signal-related alphanumeric
fields will snap to a signal line from up to ten display units
away, or snap to the nearest of two signal lines.

When OFF, the signal-related alphanumeric field boundary must
be touching the signal line in order to affix the point of
effect to that line.

If no parameter is specified, the command toggles the current
condition.

This command only applies to alphanumeric fields which have
been assigned signal-related attributes.

Refer to the chapter "Understanding FutureNet" in the
FutureNet User Manual for a list of signal-related attributes.

In areas where signal lines are crowded together, it may be
necessary to use the 'P command to place point of effect in
the exact location desired.

Enabled is the default setting.

Related Commands

MOVEPOEE

MOVEPOEE

'P

'PD

PROFILE


### PRINT -  Print Drawing

Format

PRINT [opt1][,opt2]...[,optn]

NOPRINT

Purpose

Print the current drawing.

Remarks

Print the drawing currently in the work space.

PRINT uses the relevant print options as set up in the PRINTOPT
command, or options can be specified on the command line.
Options specified in this command temporarily override any
conflicting options in the PRINTOPT command.

Printing in progress can be canceled by pressing any key during
direct output to the printed, or by typing NOPRINT during spooled
printing.  In either case, FutureNet will ask for verification

          Halt printing (Y/N)?

See the PRINTOPT command for valid options.

Use the PRINTOPT command to see current settings.

Related Commands

'PRINT

.PRINT

PRINTOPT


### PRINTOPT -   Set Print Options

Format

PRINTOPT [opt1][,opt2]...[,optn]

Purpose

Select the printer, width and other print characteristics
to be used when you enter a PRINT command or \<Shift>\<PrtSc>.

Remarks

Options can be listed in any order and the use of commas as
delimiters is optional.  A conflicting option in a PRINT command
will override the one specified with PRINTOPT for that print job.
Entering the PRINTOPT command with no parameters brings up the
Printer Setup dialog box shown below.

The available print options are listed below.  The dialog box
selection is on the left, and the option syntax for the
PRINTOPT command line entry is given across from it on the
right.

Output Control

Device/File       (FILE=device/filename)

This option specifies the name of the device or file to which
the printer output  will be written.

A printer output file that has been created by direct print
can later be printed from DOS using the following command:

COPY filename port  /b

where port is the port connected to the printer (LPT1, COM2,
etc.) and /b (required) specifies binary mode for DOS.

Do not use the DOS PRINT command to print a direct print
output file.  The PRINT command acts as a filter, removing
essential escape sequence information.

Direct/Spooled Options               (F | S)

Direct print writes directly to the device or filename you
specify in the FILE= parameter at the end of the command.  The
default device is LPT1.  Spooled printing first writes to a
temporary file in the root directory (during this time you cannot
edit), then prints the temporary file (releasing the system so
that you can continue to edit while the printer is still
printing).

Print Screen Enabled                 (S$YES | S$NO)

This option allows you to enable the normal PC \<Shift>\<PrtSc>
function.

Print Non-printable Attributes  (R | RNO)

Prints all non-printing attributes, the symbol boundaries, the
symbol reference number, and the grid defined with the GRID
command. The default is do not print non-printing attributes.

Strip                           (ALL | 1  | 2  | 3 | 4  | 5)

        ALL     Prints all strips. If your drawing is wider than
                one strip, then multiple strips will be sent to
               the printer with each strip starting on a new
               page.

        1 - 5   Selects a specific horizontal strip of a drawing
               to be printed. The strip width is the paper width
               specified by the N, M, or W setting for dot-
               matrix printers or the paper width minus 0.5-
               inches (13 mm) for laser printers. 1 is the
               default.

Dot matrix Only

These options are used by dot-matrix printers only.

Paper Width              (N | M | W)

8.0 in./203 mm    Narrow (N).  Specifies printing on an
                 80-character, 8-inch (203 mm) wide printer.

11.0 in./279 mm   Medium (M).  Specifies printing on a
                  110-character, 11-inch (279 mm) wide printer.

13.5 in./343 mm   Wide (W).  Specifies printing on a
                 132-character, 13.5-inch (343 mm) wide printer.

Narrow is the default value.

Form Feed

        Before                     (PAGETOP | PAGETOPNO).

        Form feeds to a new page at the beginning of a print
        operation.

        After                   (PAGEBOT | PAGEBOTNO)

        Form feeds to a new page at the end of a print operation.

Laser Only Options

These options are used by laser printers only.

Paper Size

LETTER                   Denotes printer paper size of
                         8.5-inches by 11 inches.

LEGAL                    Denotes printer paper size of
                         8.5-inches by 14 inches.

A,B,C,D,E                Denotes English standard drawing
                         sizes.  See the SIZE command for size
                         of drawings.

A0,A1,A2,A3,A4,A5        Denotes metric standard drawing
                         sizes.  See the SIZE command for size
                         of drawings.

Compress to Fit            (FIT  | NOFIT)

Fits entire drawing on one page, if printer has the
capability (PostScript printers only).

Printer Selection        (PRINTER="name")

Sets the printer type with a name that uniquely identifies a
particular printer.  The name must be exactly as shown in the
list of printers in the PRINTOPT dialog box (printers are also
listed when you enter Help printer list on the FutureNet command
line).  All supported printers are listed in the PRINTOPT dialog
box.

To pick a printer from the Printer Selection list, position
the cursor over the printer of choice and click the left mouse
button to highlight it.

Print - This button prints the current drawing with the selected
        options.

OK - This button accepts the changes made in the dialog box.

Save - Saves printer settings to the configuration file, fn.cfg.
       The printer settings are saved under the heading, [SYSTEM.
       PRINTOPT].

       See HELP FN.CFG for information on the configuration file.

Cancel - This button exits the dialog box cancelling changes.
         You can also press \<Esc> or click outside the dialog box
         with the left mouse button.

See the FutureNet User Manual for information on how to use
dialog boxes.

Related Commands

'PRINT

.PRINT

PRINT

.L (Symbol Definition Mode)


### PROFILE -  Display Profile Options

Format

PROFILE

Purpose

Display system settings.

Remarks

The profile screen is a dialog box that, from one location, lets
you review or modify a selected set of FutureNet environment
parameters that would otherwise have to be reviewed or modified
individually.

See the FutureNet User Manual for basic operation in dialog
boxes.

Each section of the PROFILE screen is described below.
Capitalized words in parentheses, such as (CONNECT), identify
the command to which the profile selection is mapped.

Operating States

    Rubberbanding                  (CONNECT)

Default: ON.  Indicates whether or not connections should be
maintained by rubberbanding lines when moving areas or symbols.

    Fast Mode                      (FAST)

Default: OFF.  Indicates whether the drawing screen is in
MENU or FAST mode.

    Move Elec POE                  (MOVEPOEE)

Default: ON.  Indicates whether points of effect for electrical
fields (signals and pins) should return to default locations or
be left where they are, when the associated field is moved.  ON
indicates the point of effect moves with alphanumeric fields.

    Move Gen POE                   (MOVEPOEG)

Default: ON.  Indicates whether points of effect for non-
electrical fields (not signals or pins) should return to default
locations or be left where they are, when the associated field is
moved.  ON indicates the point of effect moves with alphanumeric
fields.

    Snap to Grid                   (GRID SNAP)

Default: OFF.  Indicates whether the cursor will snap to the
closest grid point in the left and upward direction when a grid
is displayed.  When ON, the cursor moves only between grid points.

    Snap to Pin                    (PINSNAP)

Default: ON.  Indicates whether a line that is being drawn
will snap to a symbol pin that is within two display units.  When
ON, lines that are being drawn snap to symbol pins.

    Snap Sig Poe                   (POER)

Default: ON.  Indicates whether the extended point of effect
range is active.  When ON, points of effect for alphanumeric
fields with signal attributes snap to the closest type /1
(signal) or /2 (bus) line that is within 10 display units.

    Text Overlap                   (OVERLAP)

Default: OFF. Indicates whether alphanumeric fields can overlap
symbol boundaries.  See OVERLAP for more information.

    Undo/Redo                      (UNDO and REDO)

Default: ON. Indicates whether Undo and Redo are active.  See
UNDO/REDO for more information.

    Autosave                       (AUTOSAVE)

Default: ON. Indicates if the current drawing session is autosaved. 
See AUTOSAVE for more information.

Display States

    Blink                          (BLINK)

The BLINK command controls blinking for the following:

 Zoom window (visible when in fit zoom)
 Alphanumeric fields at the graphics cursor
 Tagged alphanumeric fields, symbols and areas
 The direct-connection cursor (/C command)
 Temporary lines

Blinking is the default value on monochrome systems.  Non-
blinking is the default value on color systems.

    Direct Conn.                   (.DCON)

Default: OFF.  Indicates whether direct connections will be
displayed in place of other graphical elements.

    Text Boundary                  ('B)

Default: OFF.  Indicates whether the boundaries of all alpha-
numeric fields will be displayed.  When on, the dotted boundaries
and justification points for all alphanumeric fields are displayed.

    Attr Numbers                   ('D)

Default: OFF.  Indicates whether the attribute for each alpha-
numeric field will be displayed instead of the alphanumeric text.
When on, the attributes are displayed in reverse video.

    POEs                           (POEDISP)

Default: OFF. Indicates whether points of effect will be displayed
for all alphanumeric fields.  When ON, the points of effect for
all alphanumeric fields are displayed.  Points of effect are
connected to their corresponding alphanumeric field by a line in
order to show which point of effect belongs to which text field.

    Grid                           (GRID)

Default: OFF.  Selects whether a grid is displayed in the work-
space.  When ON, the grid is displayed.

Function Keys                      (KEY)

Displays the function currently assigned to each function key.
See the chapter "Using FutureNet" in the FutureNet User Manual
for information on setting up and using the function keys.

These fields can be edited by clicking the left mouse button on
the desired field.  You can display commands to the right using
\<Ctrl>\<right arrow>.  Complete editing instructions are given in
the FutureNet User Manual.

Drawing

Size                               (SIZE, SIZED, SIZEM)

Shows the size of the current drawing.  You cannot directly edit
this field.  It must be changed by command.

Grid                               (GRID)

Shows the grid defined in the xy plane.  The grid appears as dots
on the drawing screen.  You cannot directly edit this field.  It
must be changed with the GRID command.

Libraries

Update Library                     (.LIB and .NOLIB)

Displays the name of the Update Library, if one has been opened,
to which you can save symbols or from which you can load symbols.
You cannot directly edit these fields.  They must be changed by
command.

Reference Libraries                (LIB, NOLIB)

Displays the names of the reference libraries from which you can
read symbols.  Use the scroll bars to reveal long names, or to
view additional reference libraries.  You cannot directly edit this
field.  It must be changed by command.

Current Help File                  (HELPFILE)

Display path and filename of help file.  You cannot directly edit
this field.  It must be changed by command.


OK - This button saves the changes made in the dialog box.

Save - Saves profile changes to the configuration file, fn.cfg.

       See HELP FN.CFG for information on the configuration file.

Cancel - Quit the PROFILE screen without saving any changes.

Press \<Esc> or click the left mouse button outside of the dialog
box to cancel the PROFILE dialog box and discard any changes made.


### .Q -  Exit Symbol Definition Mode (Symbol Definition Mode)

Format

Q

QUIT

.Q

END

Purpose

Exit Symbol Definition Mode and return control to the drawing
screen.

Remarks

Not supported by FutureNet OEM products.

If Symbol Definition Mode was entered with the graphics cursor
located in an existing symbol, the edited symbol definition
replaces the original definition of that symbol in the
drawing.

If a new symbol was created, it is placed in the drawing with
the upper left corner of the symbol cell at the location of
the graphics cursor.

Related Commands

.S


### QUIT -  Quit Editing Session

Format

QUIT

Q

END

SYSTEM

Purpose

End the FutureNet editing session and return to the operating
system.  Or, if you are in Symbol Definition Mode, exit that mode
and return to the drawing screen.

Remarks

If the drawing in the work space or a drawing in a structured
design was modified and not saved, the system displays

          OK to discard changes that have not already been
          saved (Y/N)?

Y erases the contents of the system work space. N cancels the
command, allowing schematic editing to continue.  In a
structured design, you can use the FILE command to see the
status of all files accessed in this session and decide
whether or not to save them.  Use the SAVE command to save a
specific file.  Note that the drawing being saved must be in
the current workspace to be saved using the SAVE command.  Use
the SAVEALL command to save all changed files.

When quitting FutureNet with a drawing that has been viewed
but not changed, this verification is bypassed.  Commands that
affect only how the drawing is viewed do not change the
drawing.  These commands include cursor moves, Zoom In, Zoom
Out, the Home command, and setting default attributes,
character size, and line type.

If a drawing is being printed using spooling, the QUIT command
will issue the following warning:

          Halt printing (Y/N)?

If you answer Y for yes, printing stops.  If you answer N for
no, printing and schematic editing continue.


### 'R -  Replace Alphanumeric String

Format

'R ["]string["]

Purpose

Insert the specified alphanumeric string at the cursor location.

Remarks

String is a required parameter.  To include leading or trailing
spaces in the string, use the double quotations.  Otherwise
FutureNet will use the string from the first non-blank character
to the last non-blank character.

'R either replaces an existing alphanumeric field or creates a
new one.

The original command remains on the command line, making it
possible to move the cursor to a new location and enter the
command again.

If string replaces an existing alphanumeric string, it will
retain the font type, attribute, orientation, justification,
and printability values of the text it is replacing.  If it
creates a new field, it will assume the default font,
attribute, orientation, justification, and printability values
which are displayed in the status fields.

If a double quote (") or backward slash (\\) is to be included
in the string, precede the character with a backward slash.
For example, to replace the string

"status\\pin1"

use the following string:

'R "\\"status\\\\pin1\\""

Related Commands

'F

'FA

'I

\<Esc>


### .R -  Rotate a Symbol

Format

.R [ - | + ]

.R [90 | 180 | 270 | -90 | -180 | -270]

.R [0]

Purpose

Rotate a symbol in increments of 90 degrees around the upper left
corner of the symbol and tag the symbol so that it can be moved.

Remarks

.R and .R + without any parameters both rotate the symbol 90
degrees clockwise; .R - without any parameters rotates the symbol
90 degrees counterclockwise.  Note that there is a space between
the .R and the plus (+), minus (-) or number.  Positive numbers
rotate the symbol clockwise; negative numbers rotate the symbol
counterclockwise.  .R0 will return the symbol to non-rotated
position.

Move the cursor to the desired symbol and enter the .R command.
.R rotates the current symbol and also tags it so that it can be
moved using the mouse or arrow keys; MOVE appears in the MODE
status field.

If the symbol has already been tagged using .M, entering .R and
any optional parameters will rotate the symbol accordingly.  If
rotating or placing the symbol causes a boundary conflict, the
command is ignored and the system displays

          Boundary conflict

To complete the rotation operation, use the .M command or the
left mouse button.

Any connections to the symbol are broken at the symbol edge
when it is rotated.

Related Commands

.RE


### .RE -  Reflect a Symbol

Format

.RE [H | V]

Purpose

Reflect a symbol horizontally (H) or vertically (V) about
its axis, creating a mirror image of the symbol.

Remarks

Symbols can be reflected 180 degrees horizontally (H) or
vertically (V). If no direction is specified, the symbol is
reflected horizontally 180 degrees.

Symbols are tagged during reflection and MOVE appears in the
MODE status field. Until the symbol is set in place, it can be
moved using the mouse or arrow keys. If placing the reflected
symbol causes a boundary conflict, the command is ignored and
the system displays

    Boundary conflict

To complete the reflection operation, use the .M command or
the left mouse button.

Any connections to the symbol are broken at the symbol edge
when it is reflected.

Related Commands

.R


### /R -  Change Line Routing

Format

/R

Purpose

Switch line routing from horizontal/vertical to vertical/
horizontal.

Remarks

Normal line drawing draws horizontal routing segments first and
vertical routing segments second.  This order can be reversed
so that vertical segments are drawn first and horizontal segments
second by entering the /R command or using the middle mouse
button to change the order of the routing segments.  This command
only works in line drawing mode (line drawing cursor is present).
It can be entered before the routing segments have been drawn, or
after they have been drawn but before they have been set.  Once
line segments have been set, they cannot be changed using this
command.

The middle mouse button is equivalent to /R in MENU and FAST
drawing modes.

Related Commands

/E

/L

/LE

/V


### [R -  Rotate an Area

Format

[R [ - | + ]

[R [90 | 180 | 270 | -90 | -180 | -270]

Purpose

Rotate an area in increments of 90 degrees in the x,y plane
and tag it so it can be moved.

Remarks

The area must already have been defined; see [D.

[R and [R + without any parameters both rotate the area 90
degrees clockwise; [R - without any parameters rotates the
area 90 degrees counterclockwise.  Note that there is a space
between the [R and the plus (+), minus (-), or number.
Positive numbers rotate areas clockwise; negative numbers
rotate areas counterclockwise.

Areas are tagged during rotation and MOVE appears in the MODE
status field. Until the area is set in place by entering the
command again, it can be relocated using the mouse or arrow
keys, as well as rotated. If placing the rotated area causes a
boundary conflict, the command is ignored and the system
displays

          Boundary conflict

To complete the rotation operation, use the [M command or the
left mouse button.

Any connections to the area are broken at the area boundary
when it is rotated.

All text fields, whether inside or outside of a symbol, will
be oriented so that horizontal text is read from left to right
and vertical text is read from bottom to top, regardless of
the number of rotations.

Related Commands

[D

[M

[RE


### [RE -  Reflect an Area

Format

[RE [H | V]

Purpose

Reflect an area about its horizontal (H) or vertical (V)
axis, creating mirror image of the area.

Remarks

The area must already have been defined; see [D.

Areas can be reflected 180 degrees horizontally (H) or
vertically (V). If no direction is specified, the area is
reflected horizontally 180 degrees.

Reflecting an area horizontally means that it will pivot on a
vertical axis.  Reflecting an area vertically means that it
will pivot on a horizontal axis.

Areas are tagged during reflection and MOVE appears in the
MODE status field. Until the area is set in place, it can be
moved using the mouse or arrow keys. If placing the reflected
area causes a boundary conflict, the command is ignored and
the system displays

          Boundary conflict

To complete the reflection operation, use the [M command or
the left mouse button.

Any connections to the area are broken at the area edge when
it is reflected.

All text fields, whether inside or outside of a symbol, will
be oriented so that horizontal text is read from left to right
and vertical text is read from bottom to top.

Related Commands

[D

[M

[R


### REFRESH -  Refresh Screen

Format

REFRESH

Key

\<Ctrl>\<L>

Purpose

Repaint the current FutureNet screen.

Remarks

Entering REFRESH or pressing \<Ctrl>\<L> causes a repaint of the
screen, clearing any text from the message area.  The current
command on the command line is maintained.


### RENUM -  Resequence Symbol Reference Numbers

Format

RENUM

Purpose

Resequence symbol reference numbers in consecutive order
following changes to the drawing.

Remarks

Each symbol in a drawing is assigned a reference number as
it is loaded into the drawing.

This command numbers the symbols left to right, top to bottom
throughout the drawing.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: This command should be used with care because some
      translators and back-end tools use the symbol reference
      number to uniquely identify a symbol. Changing symbol
      reference numbers with RENUM can interfere with the
      operation of these tools.


### rm -   Remove File

Format

rm command line arguments

Purpose

Remove the specified file from the current directory (SUN
only).

Remarks

See your system documentation regarding the rm command.

Global file characters \* and ? can be used with the rm
command.

Related Commands

CD

DEL

ls


### RS -  Reset to Default Symbol Element Set (Symbol Definition Mode)

Format

RS

Purpose

Select the default symbol element table.

Remarks

Not supported by FutureNet OEM products.

RS resets the default symbol element table shown in an
appendix in the FutureNet User Manual.  The symbol element
table can be modified by the ST command.

The symbol editor coordinate marker is unaffected by this
instruction.

Related Commands

ISxx

ST


### 'S -  Set Attribute for Existing Field

Format

'S name | number

Purpose

Set the default attribute to be assigned to future alphanumeric
fields or change the attribute setting for the alphanumeric field
the cursor is located in.

Remarks

An attribute number or name must be specified.

To change the default attribute setting, move the graphics
cursor to an unoccupied area of the work space and enter the
'S command along with the new attribute setting. The new
default setting will appear in the ATTR status field.  This
mode of 'S is similar to the 'A command.

To change the attribute setting for an existing alphanumeric
field, move the graphics cursor to that field and enter the 'S
command along with the new attribute setting.  This mode of 'S
is similar to the 'CH A command.

The 'D command can be used to display all attributes for all
alphanumeric fields.  For a particular field, the attribute is
shown in ATTR status field when the graphics cursor is located
on the alphanumeric field.

See the chapter "Understanding FutureNet" in the FutureNet
User Manual for more information on attributes.

Related Commands

'A

'CH A

'D


### .S -  Enter Symbol Definition Mode (Symbol Definition Mode)

Format

.S

Purpose

Enter Symbol Definition Mode.

Remarks

Not supported by FutureNet OEM products.

The Symbol Definition Mode screen is shown below. The symbol
definition list appears in the column on the left, with the
definition area on the right.

The symbol editor coordinate marker will be positioned in the
upper left corner of the symbol definition area.

When creating a new symbol (the graphics cursor was not
located in a symbol cell when Symbol Definition Mode was
entered), the symbol definition list will be empty.  Before a
symbol cell is defined, the symbol cell is one display unit
high by one display unit wide.

When editing an existing symbol (the graphics cursor was
located in a symbol cell when Symbol Definition Mode was
entered), the symbol definition instruction list is copied
into the symbol editor and is displayed for viewing or
editing.  The symbol cell is displayed in the upper left
corner of the graphics area.

Related Commands

.Q


### .SAVE -  Save Symbol in Current Update Library

Format

.SAVE symbolname

Purpose

Save the specified symbol definition in the current update
library.

Remarks

.SAVE saves the symbol on which the graphics cursor is located.
The graphics cursor can be located anywhere within the symbol
boundary.

Symbolname specifies the name of the symbol and can include
any ASCII characters except commas and spaces.  Symbolname
should be no more than 16 characters in length.

The symbol and any alphanumeric data is stored in the current
update library (update libraries are specified using the .LIB
command).

If a the symbol name is already in use in the update library,
then the following confirmation will appear:

	Symbol already exists.  Overwrite (Y/N)?

Respond with a Y if the symbol is to be saved and the old symbol
overwritten.  Respond N to cancel the symbol save.

A successful symbol save will result in the following status 
message:

	Symbol '\<symbolname>' saved

Related Commands

.LIB

.DEL

.DIR

.DIRPR


### .SBS -  Set Block Symbol Bit (Symbol Definition Mode)

Format

.SBS

Purpose

Allow use of pin commands in drawing mode to edit block symbols.

Remarks

Not supported by FutureNet OEM products.

.SBS should only be used if Symbol Definition Mode was
inadvertantly entered when the cursor was on a block symbol.

.SBS must be entered prior to exiting Symbol Definition Mode,
if pin stub commands are to be used in the drawing mode.

Once a symbol has been edited in Symbol Definition Mode, a
flag is set that prevents it from being edited using the
symbol definition commands available outside Symbol Definition
Mode. Entering the .SBS command while in Symbol Definition
Mode resets the flag, permitting symbols to be edited with pin
stub commands.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: Symbol definition instructions are normally performed
      relative to the end point of the previous instruction,
      creating an instruction list in which the instruction
      graphics are drawn in the order of the instruction list.
       However, the instructions for the pin stub commands are
      inserted at the beginning of the existing instruction
      list.  So, if the MXYA instruction has not been entered
      before the first symbol definition instruction, all
      following instructions could be performed from different
      locations than expected, producing a symbol that does
      not appear as intended.

Related Commands

MXYA

.Q

.S


### [SAVE -  Save Area

Format

[SAVE filename

Purpose

Save the contents of the defined area in a file.

Remarks

The area must already have been defined; see [D.

[SAVE creates a file with the specified filename if one does
not already exist.  The filename extension .ara is
automatically added unless a different extension is explicitly
given.

If a file with the filename given already exists, the system
asks

    OK to save into existing file (Y/N)?

If the response is Y for yes, the file is overwritten with the
current information.

If the response is N for no, the [SAVE command is ignored.

Symbols and alphanumeric fields are treated as being within
the area to be saved only if they are completely within the
area boundary.  Drawing elements intersecting the area
boundary are not saved.  Lines are broken at the area boundary
and the segments inside are saved.

Related Commands

[D

[LOAD


### SAVE -   Save Drawing

Format

SAVE [filename]

Purpose

Write the current drawing to the specified file.

Remarks

If the file specified already exists when the Save Drawing
command is entered, the system displays

          OK to save into existing file (Y/N)?

Y overwrites the file.  N cancels the command.

When no file with the specified filename exists, one is
created. The filename extension .dwg is automatically added
unless an extension is explicitly given.  When filename is
omitted, the Save Drawing command writes the drawing to the
filename from the last Load Drawing command.  Use the FILE
command to check the name.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: If the loaded file has a .bak extension and you enter
      the SAVE command, the system will prompt you for a
      filename to avoid overwriting your backup file.

When filename is omitted and the drawing is a new one which
has not yet been saved, the system will prompt for filename.

When used without filename, SAVE copies the original file to a
file with a .bak extension and saves the drawing in the work
space to the old filename.

The Save Drawing command can be used during an editing session
to save working drawings in order to prevent significant loss
of work due to power failure or editing errors.

The drawing in memory is unchanged after the Save Drawing
command is completed so you can continue editing.

SAVE with a filename specification saves a copy of the file in
the name specified.  This form of the command can be useful in
saving copies of a file to different filenames while
continuing to make additional changes to the original file.

SAVE operates on one file only, not on a whole drawing
structure.

Related Commands

SAVEALL

AUTOSAVE


### SAVEALL -  Save All Changed Drawing Files

Format

SAVEALL

Purpose

Save all changes made in all drawing files accessed and modified
since the editing session began or since the last SAVEALL was
entered.

Remarks

As you move between files in a design hierarchy, the drawings and
any changes are saved in temporary drawing files.  These temporary
files include information as to whether the drawing was changed or
just viewed.  When the SAVEALL command is entered, it determines
which files have been modified and saves those temporary files.

The SAVEALL command can be entered from any drawing level of
the design.  It performs the save function on every modified
drawing in the hierarchy.  You can still move up and down in
the hierarchy after entering the SAVEALL command.

If you do not want to update all of the drawings accessed and
modified, move through the structure to the drawings you do
want to update and enter the SAVE command at each one.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: This command is SAVEALL, one word, with no space between
      SAVE and ALL.  If the command SAVE ALL is entered
      instead of SAVEALL, you will save the current file to a
      file named all.dwg and all other accessed drawings will
      remain in temporary files with potential for loss unless
      SAVEALL is entered properly prior to quitting the
      editing session.

Related Commands

SAVE


### SD -  Select Decrement Mode (Symbol Definition Mode)

Format

SD

Purpose

Decrement the opposite coordinate by one dot unit (x if y is
drawn short or y if x is drawn short) following any Draw Line
Short in Display Units (DXS or DYS) instruction.

Remarks

Not supported by FutureNet OEM products.

The SD assists in drawing curves by making it easier to draw a
stepped set of varying length line segments.

Once initiated, decrement mode remains in effect until turned
off by the SN or SI instruction described in this chapter.

In decrement mode, following entry of the DYS (Draw y Short)
instruction, the x coordinate is decremented (moved left) by
one dot unit while the y coordinate is extended by one dot
unit in the direction of the line segment drawn. Similarly, in
decrement mode, following entry of the DXS (Draw x Short)
instruction, the y coordinate is decremented (moved up) by one
dot unit while the x coordinate is extended by one dot in the
direction of the line segment drawn.

The DXS and DYS instructions are altered to interpret the
length of the line drawn in dots instead of display units.

The line drawn is one dot longer than the length specified in
the draw short instruction; that is, a line of length zero is
one dot long, while a line of length one is two dots long,
etc.

While in decrement mode, use the DX, DY, DXD, and DYD
instructions to draw a line of any length (-128 to +127),
including short lengths, without decrementing the opposite
coordinate.

Related Commands

DX

DY

DXD

DYD

DXS

DYS

SI

SN


### SI -  Select Increment Mode  (Symbol Definition Mode)

Format

SI

Purpose

Increment the opposite coordinate by one dot unit (x if y is
drawn short or y if x is drawn short) following any Draw Line
Short in Display Units (DXS or DYS) instruction.

Remarks

Not supported by FutureNet OEM products.

The SI instruction assists in drawing curves by making it
easier to draw a stepped set of varying length line segments.

Once initiated, increment mode remains in effect until turned
off by a Select No Increment/Decrement or Select Decrement
Mode instruction described in this chapter.

In increment mode, following entry of the DYS (Draw y Short)
instruction, the x coordinate is incremented (moved to the
right) by one dot unit while the y coordinate is extended by
one dot unit in the direction of the line segment drawn.
Similarly, in increment mode, following entry of the DXS (Draw
x Short) instruction, the y coordinate is incremented (moved
down) by one dot unit while the x coordinate is extended by
one dot in the direction of the line segment drawn.

The DXS and DYS instructions are altered to interpret the
length of the line in dot units instead of display units.

The line drawn is one dot longer than the length specified in
the draw short instruction; that is, a line of length zero is
one dot long, while a line of length one is two dots long,
etc.

While in increment mode, use the DX, DY, DXD, and DYD
instructions to draw a line of any length (-128 to +127),
including short lengths, without incrementing the opposite
coordinate.

Related Commands

DX

DY

DXD

DYD

DXS

DYS

SD

SN


### SIZE -  Set Drawing Size

Format

SIZE [A | B | C | D | E], [B | H | E | I | F | O | M | P | T]

SIZE width,height

SIZED width,height

SIZEM [A5 | A4 | A3 | A2 | A1 | A0]

SIZEM width,height

Purpose

Specify the drawing size.

Remarks

Drawings can be specified in standard drawing sizes A - E, metric
drawing sizes A5 - A0, inches, display units, and millimeters.
The SIZE status field always gives the drawing size in display
units; plus, if one of the standard sizes A - E or metric drawing
sizes A5 - A0 have been selected, those are also given.  Knowing
the drawing dimensions in display units is useful since the cursor
location is shown in display units in the XY status field and all
cursor movement is calculated in display units.

The default drawing size is B, which is 17 inches wide by 11
inches high.

There are approximately 20 display units per inch on both x
and y axes.

The printer parameter is provided to help compensate for
physical limitations of some printers.  For more information
on this parameter, see the discussion on printer compensation
below.

SIZE

Standard drawing sizes A - E, and their equivalent in inches and
display units are shown below.  The range for width in inches is
7.5 to 102.4. The range for height in inches is 3.8 to 102.4.
Indicate tenths of inches using a decimal point.

Following is a table of the standard drawing sizes and their
display unit equivalents. Note that this table assumes that no
printer option is specified in the SIZE command. This table
reflects true sizes without compensation for a particular
printer. See the discussion below on printer compensation for
more information.

Size    Inches      Display Units
         (wxh)              (wxh)
---------------------------------
A       11 x 8.5    220 x 170
B       17 x 11         340 x 220
C       22 x 17     440 x 340
D       34 x 22     680 x 440
E       44 x 34     880 x 680

SIZE is used to specify the width and height of a drawing in
standard drawing sizes A - E or in inches.

SIZED

SIZED is used to specify the width and height of a drawing in
display units.  The range for width in display units is 150 to
2048.  The range for height in display units is 75 to 2048.

SIZEM

Following is a table of the standard metric drawing sizes and
their display unit equivalents. Note that this table assumes that
no printer option is specified in the SIZE command. This table
reflects true sizes without compensation for a particular printer.
See the discussion below on printer compensation for more
information.

Size    Millimeters    Display Units
           (wxh)            (wxh)
----------------------------------------
A5      210 x 147       165 x 116
A4      297 x 210      234 x 165
A3      420 x 297      330 x 234
A2      594 x 420      467 x 330
A1      840 x 594      662 x 467
A0     1190 x 840     936 x 662

Metric drawing sizes A5 - A0 and their equivalents in
millimeters and display units are shown below.  The range for
width in millimeters is 191 to 2602.  The range for height in
inches is 96 to 2602.

Changing Drawing Size

To reduce the size of a drawing after drawing has started,
use the following method:

1. Save the drawing as an area using the Area Editing
   Commands.

2. Open a new drawing and select a size using any of the SIZE
   commands.

3. Load the area into the new drawing.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note:  This procedure will not scale a drawing down to a
       different size.  For example, if you have a D size
       drawing, which is 680 x 440 display units, and you want
       to put the drawing into a B size drawing, which is 340
       x 220 display units, you can only copy a B sized
       portion of the D drawing.

Printer Compensation

Most printers cannot print on an entire sheet of paper.
Compensation for this physical limitation is accomplished by
limiting the drawing area within FutureNet to match the physical
limitations of the printer. The following compensation factors
are provided using the printer parameter on the size commands:

Value                    Compensation Factors

B or H                   Reduces the drawing space by one inch
                         on each size, two inches in height
                         and two inches in width. Use when
                         drawing will be plotted with DPlot on
                         a plotter device.

E or I                   Reduces the drawing space by 10% on
                         the x axis and 25% on the y axis.
                         Use with printers similar to the
                         Epson MX family or Epson FX family in
                         IBM emulation.

F or O                   Reduces the drawing space by 10% on
                         both the x axis and the y axis. Use
                         with printers similar to the Epson FX
                         family or Okidata printers.

M                        Reduces the drawing space by 20% on
                         both the x axis and the y axis.
                         Specifically designed to create
                         Milspec drawings when scaled by 125%
                         in DPLOT.

T                        Reduces the drawing space by 10% on
                         the x axis. The y axis is not
                         affected by this option. Use with
                         printers similar to the C. Itoh 1550.

P                        No compensation for either axis. This
                         is the default setting.

Use the option that is most appropriate for your printer. Some
experimenting may be necessary to find the correct option for
your particular printer. The SIZED command can also be used to
specify exactly the size of the drawing area in display units.


### SL -  Select Broken Line Type (Symbol Definition Mode)

Format

SL 0 | 1 | 2 | 3 | 4 | ... | 14

Purpose

Select line type.

Remarks

Not supported by FutureNet OEM products.

Select the type of line that will be drawn by BX and BY
instructions.

The current line type is shown in the Line Status box.

This command has no effect on the symbol editor coordinate
marker.

Related Commands

BX

BY


### SN -  Select No Increment/Decrement Mode (Symbol Definition Mode)

Format

SN

Purpose

Restore the normal operation of the Draw Short instructions,
terminating the specialized operation of the SD and SI
instructions.

Related Commands

DXS

DYS

SI

SD


### ST -  Select Symbol Element Table (Symbol Definition Mode)

Format

ST 0 | 1 | 2 | 3 | 4 | 5 | 6

Purpose

Select the ASCII symbol element table and specify the font size
of the characters used as symbol elements when using the ISxx
command.

Remarks

Not supported by FutureNet OEM products.

The ASCII symbol element table (an appendix in the FutureNet
User Manual) is an alternative to the default symbol element
table (which is selected by entering the RS command).

This command allows addition of text in various fonts to a
symbol at a specific pixel location, rather than at a display
unit location.

Command      |   Selects ASCII Character Size
-------------|--------------------------------
  ST 0       |          '1
  ST 1       |          '2
  ST 2       |          '3
  ST 3       |          '4
  ST 4       |          '5
  ST 5       |          '6
  ST 6       |          '7

The ST command must precede the Insert Symbol Element (ISxx)
commands that are to be affected.

Once ST is selected, the Insert Symbol Element command uses
the selected character set until another ST command, or until
an RS command resets the symbol element table to the default
shapes.

Related Command

[RS](#rs----reset-to-default-symbol-element-set-symbol-definition-mode)

[ISxx](#isxx----insert-symbol-element-symbol-definition-mode)

### STOP -  Stop Command File Run

Format

STOP

NOAUTO

NOEXEC

Purpose

Stop automatic or single-step execution of a command file.

Remarks

STOP, NOAUTO, and NOEXEC can be entered anytime to stop AUTO
or EXEC command file execution.  NOAUTO and EXEC are synonyms
for STOP.

To enter, type STOP on the command line while the current
command file is being entered in either AUTO or EXEC mode.

Related Commands

[AUTO](#auto----automatic-command-execution)

[EXEC](#exec----single-step-command-execution)

[PAUSE](#pause----change-from-automatic-to-single-step-execution)

### SXY, RXY -  Save/Restore x and y Coordinates  (Symbol Definition Mode)

Format

SXY

RXY

Purpose

Save the current x,y values and then restore them after they have
been changed by other drawing instructions.

Remarks

Not supported by FutureNet OEM products.

SXY saves the current x,y coordinate values.  This instruction
does not affect the symbol editor coordinate marker location.

RXY restores the x,y coordinate values saved by the last SXY
instruction.  The symbol editor coordinate marker is restored
to the old x,y location.

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

Note: If an RXY command occurs in the symbol definition list,
      before an SXY command, then the default restored
      location is (0,0).


### Tab, Shift  Tab Keys -  Move Cursor Down/Up a Field

Keys

\<Tab>
\<Shift>\<Tab>

Purpose

When in alphanumeric mode, move the cursor to the beginning
of the next or previous alphanumeric field within a symbol cell.

Remarks

\<Tab> moves the cursor to the beginning of the next alphanumeric
field in the symbol cell.

\<Shift>\<Tab> moves the cursor to the beginning of the previous
alphanumeric field in the symbol cell.

\<Tab> fields are found moving left to right, top to bottom.

\<Shift>\<Tab> fields are found moving right to left, bottom to
top.

These keys work only within a symbol cell, and will not move
the cursor between symbol cells.

If the next alphanumeric field to be reached is not entirely
visible on the screen, the display shifts so that the cursor
is centered on the drawing screen.

Related Commands

[\<Esc>](#esc-key----enterexit-alphanumeric-mode)

### #U -  Move Up in Design Hierarchy

Format

\#U[filename]

Key

PC | SUN
---|----
\<Ctrl>\<PgUp> |\<Ctrl>\<R9>

Purpose

Move back up in the design hierarchy and show a previously
loaded drawing file.

Remarks

\#U[filename] saves the current drawing file to a temporary file
and reloads a previously loaded drawing file that is on the
current drawing path but at a higher level in the hierarchy.
The current drawing file is saved in a temporary file with a
unique filename based on the root filename with a numbered
extension.  If the filename exists, the numbered extension is
incremented until the filename is unique.

\<Ctrl>\<PgUp>, \<Ctrl>\<R9> or \<#U> without a filename reloads the
file above the current file in the hierarchy.

Pressing \<Ctrl>\<PgUp> or \<Ctrl>\<R9> repeatedly traverses the
drawing structure up to the root.  Once in the root drawing,
the Move Up in Design Hierarchy command has no effect.

Related Commands

[#D](#d----move-down-in-design-hierarchy)

[#L](#l-and-r----move-left-or-right-in-design-hierarchy)

[#R](#l-and-r----move-left-or-right-in-design-hierarchy)

[SAVEALL](#saveall----save-all-changed-drawing-files)

### 'UNDER -  Add/Delete Underscore to Alphanumeric Field

Format

'UNDER [ON | OFF]

Purpose

Adds or deletes a field underscore.

Remarks

Position the graphics cursor on the desired alphanumeric field
and enter the command.

If no parameter is specified, the command toggles the current
setting.

Spaces within the text field will not cause a break in the
underscore.

'UNDER can only be entered on an existing field.

While in alphanumeric mode, an underscore can be added or
deleted by using the \<Ctrl>\<U> keystoke sequence.

When an existing field that already has an underscore is edited,
the underscore will be added or deleted as text is added or
deleted.

Related Commands

['OVER](#over----adddelete-overbar-tofrom-alphanumeric-field-signal-inversion)

### UNDO/REDO -  Reverse/Restore Commands

Format

UNDO [ON | OFF]

REDO [ON | OFF]

Purpose

UNDO allows step-by-step reversal of actions done during
the current drawing session.  REDO reverses the UNDO feature,
redoing actions that have been undone.

Remarks

UNDO and REDO are available on most configurations supported by
FutureNet.  The following configurations are supported.

\*     All Sun systems

\*     PC systems with Extended Memory (using the Extended
      Memory executable)

\*     PC systems with Expanded Memory or Expanded Memory
      drivers (using the Non-Extended Memory executable)

UNDO/REDO works from a stack of actions that is built as a
drawing is edited.  UNDO is ON by default on systems that
can support UNDO/REDO.  Entering UNDO OFF or REDO OFF disables
this feature, and clears the stack.

You are able to move forward or backward in the stack at will.
The stack has the following limitations:

\*     Only actions which affect drawing graphics are placed on
      the stack.  For instance, accesses of the PROFILE screen
      or menus are ignored, as are file commands such as SAVE.

\*     Failed operations are not stacked.  For instance, if you
      attempt to draw a line that results in an ``Invalid line
      routing'' error and do not reroute the line correctly,
      the entire operation is ignored.

\*     Entering the LOAD, CLEAR, #L, #R, #U and #D commands
      clear the stack.

### /V -  Move Vertex

Format

/V

Purpose

Tag any point along a line or the intersection of two line
segments and move the line without moving the end points, or move
one end point of a line without moving the other end point.

Remarks

Position the graphics cursor at the desired location, either a
point on a line or the intersection to two line segments and
enter the /V command. A two display unit square will appear, with
its center at the tagged point, indicating that the point has
been tagged.  Move the tagged vertex to the desired location and
re-enter the /V command to complete the operation.

Lines moved using the /V command will be rubberbanded where
necessary to maintain connectivity.  Depending on where the
lines have been moved to, it may not be possible to reroute
them without creating temporary lines.

The left mouse button can also initiate this command.  Move
the graphics cursor to desired location on the line and press
the left mouse button twice.  The first press puts the system
in LINE mode and the second press puts the system in tag mode.
 Assuming the cursor coincides with a point on a line or a
vertex, the line will be tagged.  If other graphic elements
(symbols, text fields, or areas) are in close proximity, they
may be tagged first.  Continue pressing the left mouse button,
moving through the tagging hierarchy, until the line or vertex
is tagged.

This command works with permanent and temporary lines.  It
does not work with direct connect lines.

Related Commands

[/L](#l----draw-lines)

### VERSION -  Display Software Version

Format

VERSION

VER

Purpose

Display the version of FutureNet software presently installed
in the system.

Remarks

The version of software is also displayed in the message
area when FutureNet initializes.

This information may be helpful in any communication with a
Data I/O Customer Support office.


### VIEW -  View Drawing

Format

VIEW [ON | OFF]

Purpose

Toggles screen update ON/OFF.

Remarks

ON is the default setting.  VIEW without any parameters toggles
the setting.

When ON, the screen is updated as the commands are entered.

When OFF, screen updating is suspended.

VIEW can be entered on the command line or as part of a
command file.  It can be entered more than once, either on the
command line or in command files.  In command files, it can
appear anywhere in the list of commands.

Command files will run to completion faster with VIEW OFF
rather than ON because there will be no need to display
graphics. The lines, symbols, areas, and text fields are
created in the same manner whether VIEW is ON or OFF, but you
don't see them occurring when VIEW is OFF.

Type VIEW ON at any time to see the results of the commands
that were run since VIEW OFF was used.

Related Commands

[AUTO](#auto----automatic-command-execution)

[EXEC](#exec----single-step-command-execution)

### .w,h -  Define Symbol Cell Size

Format

.width,height

Purpose

Define or redefine the size of the symbol cell.

Remarks

Width and height are defined in display units.

This command is also available in the Symbol Definition Mode.

In drawing mode, the cursor must be inside a symbol cell
before this command can function.  Symbol size should be
specified before starting the symbol definition instruction
list.

An existing symbol cell can be made larger or smaller by
specifying different width and height dimensions.  .0,0 will
remove the boundary from the screen.

If the symbol cell defined does not fit in the drawing at the
graphics cursor location, the system displays

          Symbol boundary conflict

Before the symbol cell can be set to the desired size at the
current location, all boundary conflicts must be resolved by
moving or erasing the other drawing elements that interfere
with the new symbol.  The size of the symbol boundary is
restricted only by symbol boundary conflicts.

If no symbol cell size has been previously specified, a
default of w=1 and h=1 is provided for a newly created symbol.
 Symbols may extend beyond the limits of the symbol cell;
however, no boundary checks beyond the symbol cell size are
done.

Related Commands

[.S](#s----enter-symbol-definition-mode-symbol-definition-mode)

### WINDOW -  Set Window Locations

Format

WINDOW 1 | 2 | 3 | 4

WINDOW OFF

NOWIN

Purpose

Specify up to four window locations that can be searched
to using the PAN command.

Remarks

Assign a number from 1 through 4 to a window location.

The WINDOW command can be entered at any zoom level.

Fit (ZIN n) zoom is the best zoom mode in which to assign
window numbers because the entire drawing, including the
window position relative to other windows and elements, is
displayed.

The full-scale window (the rectangular box which appears at
fit zoom level) frames the area of the drawing that will be
assigned a window number.

Window locations saved with the WINDOW command are displayed
in fit zoom as dotted rectangles with the window number in the
upper left corner.

NOWIN or WINDOW OFF cancels all the saved window locations.

Related Commands

[PAN](#pan----pan-to-window-locations)

[ZIN](#zinzout----change-zoom-level)

[ZOUT](#zinzout----change-zoom-level)


### ZIN/ZOUT -  Change Zoom Level

Format

ZIN [0 | 1 | 2 | n]

ZOUT [0 | 1 | 2 | n]

Keys

PC | SUN
---|----
\<PgUp> |\<R9>   (ZIN)
\<PgDn> |\<R15>  (ZOUT)

Purpose

Select half, full, intermediate, or fit zoom.

Remarks

Depending on the current zoom level, repeatedly entering ZIN or
pressing \<PgUp> or \<R9> cycles from fit to half; ZOUT or \<PgDn>
or \<R15>, from half to fit.  To zoom to a specific level, use the
appropriate parameter.

The ZOOM status field displays the value of the zoom level where
full zoom (ZIN 1 or ZOUT 1) is considered as 1.

0                        Half (objects appear largest, roughly
                         twice actual size)

1                        Full (objects appear actual size)

2                        Intermediate (midway between full and
                         fit)

n                        Fit (entire drawing visible)

Full zoom is the default zoom level.

ZIN and ZOUT can be entered on the command line and in macros.

Half Zoom

Half zoom displays a small portion of the drawing, at roughly
twice actual size.  .70 is displayed in the ZOOM status field.
All drawing editing commands operate in full zoom.

All symbols, their alphanumeric text, and all connecting lines
that fit in the window are displayed.

Full Zoom

Full zoom displays a portion of the drawing, with all objects
appearing nearly actual size.  1.00 is displayed in the ZOOM
status field.  The full range of editing commands are available,
just as in half zoom.

Intermediate Zoom

Intermediate zoom displays the drawing in a scale that is the
average of the drawing area in fit and full zoom.  The ZOOM
status field displays a value midway between 1 and the zoom value
of fit zoom.

Outlines of the symbols and their connecting lines that fit on
the screen are displayed, but alphanumeric text is not.
Alphanumeric field boundaries display when the 'B command is
entered.

Fit Zoom

In fit zoom, the entire drawing is displayed in the scale
required for it to fit exactly on the screen. Outlines of the
symbols and their connecting lines are displayed, but not
alphanumeric text.  A rectangular boundary, called a full scale
window, outlines the portion of the drawing that would be
displayed in full zoom.  This window is centered around the
cursor and moves as it moves. Also shown are any saved windows
and their window numbers. Alphanumeric field boundaries appear
when the 'B command is entered.

In fit zoom, the HOME command has no effect on the cursor.  The
graphics cursor can be moved using the various move commands
(mouse, Move to Coordinates, arrows, etc.) to anywhere in the
drawing.

Lines can be drawn and symbols manipulated in fit zoom and the
full scale window can be relocated using the mouse and command
line.

Related Commands

[ZOOM](#zoom----dynamic-zoom-level)


### ZOOM -  Dynamic Zoom Level

Format

ZOOM

Purpose

Define and display an area at a custom zoom level.

Remarks

When the ZOOM command is entered, the zoom cursor appears at
the current cursor location, and ZOOM appears in the MODE status
field.  Using the mouse or with cursor control, a zoom box may
be stretched in any direction to enclose an area with one corner
at the original cursor location.  When the command is reentered,
the area within the zoom box grows to fill the entire window,
and the mode returns to the previous mode.

The ZOOM status field is updated to show the level of zoom
compared against full zoom=1.  The smallest zoom level
possible is .1.

When a zoom box is being defined, ZOOM mode may be cancelled
by pressing the right mouse button.

Use the ZIN/ZOUT commands or keys to move to a pre-defined
zoom level once you are finished with dynamic zoom.

Related Commands

[ZIN](#zinzout----change-zoom-level)

[ZOUT](#zinzout----change-zoom-level)
