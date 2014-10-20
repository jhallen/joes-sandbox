/* Printer interface
   Copyright (C) 1991 Joseph H. Allen

JTERM - Joe's pass-through printing terminal emulator

JTERM is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.

JTERM is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License
along with JTERM; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <dos.h>
#include "lp.h"

void pout(int port,unsigned char c)
{
outportb(port,c);
outportb(port+2,5);
outportb(port+2,4);
}
