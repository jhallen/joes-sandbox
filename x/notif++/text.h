/* Text widget 
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

class Text : public Widget
  {
  int xpos;
  int ypos;
  int state;
  char *text;

  public:

  virtual void st(char *s);	// Set text string
  virtual char *gt();		// Get text string

  // Overrides

  virtual int gtheight();	// If size has not been set, make window
  virtual int gtwidth();	// just big enough to hold text.

  // Event handlers

  virtual int select();
  virtual int unselect();
  virtual int activate();
  virtual int deactivate();
  virtual int expose(XEvent *ev);

  Text();
  };
