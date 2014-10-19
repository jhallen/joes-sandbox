/* Main Notif header file
   Copyright (C) 1999 Joseph H. Allen

This file is part of Notif

Notif is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

Notif is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
Notif; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#ifndef _Inotif
#define _Inotif 1

#ifndef _Ixincs
#define _Ixincs 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#endif

#include <stdio.h>

/* Utilities */

#include "int.h"
#include "link.h"
#include "list.h"
#include "hash.h"
#include "fn.h"
#include "kbd.h"
#include "io.h"

/* Types */

class Widget;
struct Pmgr;
class Lith;
class Wind;
class Shadow;
class Iborder;
class Eborder;
class Mborder;
class Tborder;
class Help;
class Text;
class Icon;
class Button;
class Mbutton;
class Cascade;
class Mcascade;
class Mbcascade;
class Toggle;
class Mtoggle;
class Radio;
class Edit;
class Border;
class Titled;
class Slider;
class Vslider;
class Hslider;
class Bar;
class Vbar;
class Hbar;
class Listwidget;
class Image;
class Scope;
class Index;
class Vlist;
class Listbar;
class Menu;
class Scroll;
class Option;
class Drag;
class Gsep;
class Tedit;
class Titlebar;

/* Core notif */
#include "event.h"
#include "lith.h"
#include "wind.h"
#include "border.h"
#include "widget.h"
#include "draw.h"
#include "help.h"

/* Widgets */
#include "text.h"
#include "icons.h"
#include "icon.h"
#include "button.h"
#include "menu.h"
#include "cascade.h"
#include "gsep.h"

#include "toggle.h"
#include "radio.h"

#include "slider.h"
#include "bar.h"
#include "scroll.h"

#include "edit.h"

//#include "dragdrop.h"
//#include "list.h"
//#include "image.h"
#include "scope.h"
//#include "option.h"
#include "index.h"
#include "tedit.h"

#endif
