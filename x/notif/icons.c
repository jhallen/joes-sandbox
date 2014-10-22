/* Scalable icons 
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

#include "notif.h"

int picuparw[]=
 {
 picline(5000,625,1250,4375),
 picline(1250,4375,3750,4375),
 picline(3750,4375,3750,8750),
 picline(3750,8750,6250,8750),
 picline(6250,8750,6250,4375),
 picline(6250,4375,8750,4375),
 picline(8750,4375,5000,625),
 picfill,
 picuser,
 picdone
 };

int picdnarw[]=
 {
 picline(5000,8750,1250,5000),
 picline(1250,5000,3750,5000),
 picline(3750,5000,3750,625),
 picline(3750,625,6250,625),
 picline(6250,625,6250,5000),
 picline(6250,5000,8750,5000),
 picline(8750,5000,5000,8750),
 picfill,
 picuser,
 picdone
 };

int picltarw[]=
 {
 picline(625,5000,4375,1250),
 picline(4375,1250,4375,3750),
 picline(4375,3750,8750,3750),
 picline(8750,3750,8750,6250),
 picline(8750,6250,4375,6250),
 picline(4375,6250,4375,8750),
 picline(4375,8750,625,5000),
 picfill,
 picuser,
 picdone
 };

int picrtarw[]=
 {
 picline(8750,5000,5000,1250),
 picline(5000,1250,5000,3750),
 picline(5000,3750,625,3750),
 picline(625,3750,625,6250),
 picline(625,6250,5000,6250),
 picline(5000,6250,5000,8750),
 picline(5000,8750,8750,5000),
 picfill,
 picuser,
 picdone
 };

int picx[]=
 {
 picline(625,1875,3750,5000),
 picline(3750,5000,625,8125),
 picline(625,8125,1875,9375),
 picline(1875,9375,5000,6250),
 picline(5000,6250,8125,9375),
 picline(8125,9375,9375,8125),
 picline(9375,8125,6250,5000),
 picline(6250,5000,9375,1875),
 picline(9375,1875,8125,625),
 picline(8125,625,5000,3750),
 picline(5000,3750,1875,625),
 picline(1875,625,625,1875),
 picfill,
 picuser,
 picdone
 };

int piccheck[]=
 {
 picline(1875,5000,4375,7500),
 picline(4375,7500,7500,1875),
 picline(7500,1875,6875,1250),
 picline(6875,1250,4375,6875),
 picline(4375,6875,2500,4375),
 picline(2500,4375,1875,5000),
 picfill,
 picuser,
 picdone
 };

int picarc[]=
 {
 picline(1000,1000,9000,1000),
 picbezier(1000,1000,2000,9000,8000,9000,9000,1000),
 picfill,
 picuser,
 picdone
 };
