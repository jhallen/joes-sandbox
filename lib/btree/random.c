/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * This is derived from the Berkeley source:
 *	@(#)random.c	5.5 (Berkeley) 7/6/88
 * It was reworked for the GNU C Library by Roland McGrath.
 *
 * Fixed to be portable to MSDOS by jhallen (changed ints to longs)
 *
 * jhallen- Made sure functions work if long is larger than 32 bits- it does
 *          Long is required to be at least 32 bits
 *          Also made saving and restoring state information far simpler
 */

#include "random.h"

struct state state=
 {
  {
  0x9a319039, 0x32d9c024, 0x9b663182, 0x5da1f342,
  0xde3b81e0, 0xdf0a6fb5, 0xf103bc02, 0x48f340fb,
  0x7449e56b, 0xbeb1dbb0, 0xab5c5918, 0x946554fd,
  0x8c2e680f, 0xeb3d799f, 0xb11ee0b7, 0x2d436b86,
  0xda672e2a, 0x1588ca88, 0xe369735d, 0x904f35f7,
  0xd7158fd6, 0x6fa6f051, 0x616e6b96, 0xac94efdc,
  0x36413f93, 0xc622c298, 0xf5a42ab8, 0x8a88d77b,
  0xf5ad9d0e, 0x8999220b, 0x27fb47b9
  },
  &state.state[SEP],
  &state.state[0]
 };

void srandom(x)
 {
 register long i;
 state.state[0]=x;
 for(i=1;i!=DEG;++i) state.state[i]=(1103515145*state.state[i-1])+12345;
 state.fptr= &state.state[SEP];
 state.rptr= &state.state[0];
 for(i=0;i!=10*DEG;++i) random();
 }

long random()
 {
 long i=((*state.fptr+=*state.rptr)>>1)&0x7FFFFFFF;
 if(++state.fptr==&state.state[DEG]) state.fptr=state.state, ++state.rptr;
 else if(++state.rptr==&state.state[DEG]) state.rptr=state.state;
 return i;
 }
