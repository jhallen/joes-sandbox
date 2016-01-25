/*-----------------------------------------------------------------------*\
 |  z80.c  --  z80 emulator                                              |
 |                                                                       |
 |  Copyright 1986-1988 by Parag Patel.  All Rights Reserved.            |
 |  Copyright 1994-1995 by CodeGen, Inc.  All Rights Reserved.           |
\*-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

int nobdos;
int strace;
int bdos_return = -1;

/* All the following macros assume access to a parameter named "z80" */


/* entry in "REGPAIRXY[]" to get to the IX or IY registers */
#define XYPAIR 2

#define REG		z80->reg
#define REGPAIRAF	z80->regpairaf
#define REGPAIRSP	z80->regpairsp
#define REGPAIRXY	z80->regpairxy
#define REGIXY		z80->regixy
#define REGIR		z80->regir


/* bit masks for jump/call/return group instructions */
static const byte flagmask[] =
{
	ZERO, CARRY, PARITY, SIGN
};

/* bit masks for bit-twiddling instructions */
static const byte bitmask[] =
{
	BIT0, BIT1, BIT2, BIT3,
	BIT4, BIT5, BIT6, BIT7
};

/* parity setting array - initialized in init_z80info() below */
static int parityarr[0x100];
static boolean parity_inited = FALSE;



/* handy defines for playing with the F(lag) register */

#define flagon(flag) (F |= (flag))
#define flagoff(flag) (F &= ~(flag))
#define setflag(flag,val) ((val) ? flagon(flag) : flagoff(flag))
#define resetflag(flag,val) ((val) ? flagoff(flag) : flagon(flag))

#define setsign() setflag(SIGN, A & BIT7)
#define setzero() setflag(ZERO, !A)



/* The following macros are supposed to look just like function calls.
   To that effect they use a bunch of temporary vars that must be
   allocated somewhere.  They're generally named "t*" or "r*".
*/


/* macros for swapping various popular entities */

#define swapw(reg1,reg2) (tt = reg1, reg1 = reg2, reg2 = tt)
#define swapb(reg1,reg2) (t = reg1, reg1 = reg2, reg2 = t)



/* set the parity flag based on the value specified */

#define setparity(val)	setflag(PARITY, parityarr[val & 0xFF])



/* set the flags for most bit-twiddling instructions */

#define flags(val) \
{\
	v = val;\
	setflag(SIGN, v & BIT7);\
	setflag(ZERO, !v);\
	flagoff(HALF);\
	flagoff(NEGATIVE);\
	setparity(v);\
}



/* for generic 8-bit arithmetic instructions */

#define arith8(val, carry, sub) \
{\
	vv = val;\
	s = sub;\
	if (s)\
	{\
		flagon(NEGATIVE);\
		setflag(HALF, ((A & MASK4) - (vv & MASK4)) & BIT4);\
		tt = A - vv;\
		if ((carry) && (F & CARRY)) tt -= 1;\
	}\
	else\
	{\
		flagoff(NEGATIVE);\
		setflag(HALF, ((A & MASK4) + (vv & MASK4)) & BIT4);\
		tt = A + vv;\
		if ((carry) && (F & CARRY)) tt += 1;\
	}\
	setflag(SIGN, tt & BIT7);\
	setflag(OVERFLOW, ((A & BIT7) == (vv & BIT7)) &&\
			((A & BIT7) != (tt & BIT7)));\
	setflag(CARRY, tt & BIT8);\
	v = tt;\
	setflag(ZERO, !v);\
}



/* set flags for most logical (AND, OR, ...) instructions */

#define logical(hval) \
{\
	h = hval;\
	setflag(SIGN, A & BIT7);\
	setflag(ZERO, !A);\
	setflag(HALF, h);\
	setparity(A);\
	flagoff(NEGATIVE);\
	flagoff(CARRY);\
}



/* for incrementing/decrementing of a register */

#define increment(reg, neg) \
{\
	i = reg;\
	n = neg;\
	tt = i;\
	if (n)\
	{\
		setflag(HALF, !(tt-- & MASK4));\
		setflag(OVERFLOW, (i & BIT7) && !(tt & BIT7));\
	}\
	else\
	{\
		setflag(HALF, !(++tt & MASK4));\
		setflag(OVERFLOW, !(i & BIT7) && (tt & BIT7));\
	}\
	setflag(SIGN, tt & BIT7);\
	setflag(ZERO, !(tt & MASK8));\
	setflag(NEGATIVE, n);\
}




/*-----------------------------------------------------------------------*\
 |  z80  --  emulate a z80  --  labels & gotos are used here (if you
 |  don't like 'em, tough!)
\*-----------------------------------------------------------------------*/

boolean
z80_emulator(z80info *z80, int count)
{
	byte t = 0, t1, t2, cy, v, *r = NULL;
	word tt, tt2, vv, *rr;
	longword ttt;
	int i, j, h, n, s;

	/* main loop  --  all "goto"s eventually end up here */
infloop:

	/* only execute "count" instructions at one whack */
	if (count-- <= 0)
		return TRUE;

	/* see if the z80 is to be interrupted for any reason */
	if (EVENT)
	{
		EVENT = FALSE;

		/* HALT execution if desired - this is for tracing & such */
		if (HALT)
			haltcpu(z80);

		/* "i" is used to see if we need to get the next opcode or not*/
		i = TRUE;

		/* get the interrupt type  --  use a sequence of "if" statements
		   instead of a "switch" since the order is important  --  each
		   interrupt resets itself only so that another one will run */

		if (RESET)		/* RESET "line" has been "pulled" */
		{
			IFF = 0;
			IFF2 = 0;
			I = 0;
			R = 0;
			PC = 0;
			IMODE = 0;
			RESET = FALSE;
			if (NMI || INTR)	/* catch these the next time */
				EVENT = TRUE;
		}
		else if (NMI)			/* non-maskable interrupt */
		{
			--SP;
			SETMEM(SP, PC >> 8);
			--SP;
			SETMEM(SP, PC & MASK8);
			PC = 0x66;
			IFF = 0;
			NMI = FALSE;
			if (INTR)		/* catch this the next time */
				EVENT = TRUE;
		}
		else if (INTR && IFF)	/* normal masked interrupt */
		{
			/* we have three interrupt modes in the z80 */
			switch (IMODE)
			{
				case 0:			/* 8080-mode -- this is NOT correct */
					/* get the next instruction from the interrupting
					   device - this may be more than one byte but we
					   cannot handle that yet */
					i = FALSE;
					t = INTR;
					break;
				case 1:			/* like a "rst" to 0x38 */
				default:
					--SP;
					SETMEM(SP, PC >> 8);
					--SP;
					SETMEM(SP, PC & MASK8);
					PC = 0x38;
					break;
				case 2:	/* most powerful/flexible mode */
					--SP;
					SETMEM(SP, PC >> 8);
					--SP;
					SETMEM(SP, PC & MASK8);
					tt = (I << 8) | (INTR & 0xFF);
					PC = MEM(tt);
					tt++;
					PC |= MEM(tt) << 8;
					break;
			}
			IFF = IFF2 = 0;
			INTR = 0;
		}
		else if (INTR)			/* try again the next time around */
			EVENT = TRUE;

		/* get the next opcode to execute if we do not have it yet */
		if (i)
		{
			t = MEM(PC);
			PC++;
		}
	}
	else
	{
		/* just get the next opcode */
		t = MEM(PC);
		PC++;
	}


	/* main "switch" for initial opcode */
	switch (t)
	{
	/* go to other switch statements for the multi-byte opcodes */
	case 0xDD:
	case 0xFD:			/* index-register instructions */
		goto ireginstr;
		break;
	case 0xED:			/* extended instructions */
		goto extinstr;
		break;
	case 0xCB:			/* bit-twiddling instructions */
		goto bitinstr;
		break;


	/* 8-bit load group */

	case 0x40:					/* ld b,b */
	case 0x41:					/* ld b,c */
	case 0x42:					/* ld b,d */
	case 0x43:					/* ld b,e */
	case 0x44:					/* ld b,h */
	case 0x45:					/* ld b,l */
	case 0x47:					/* ld b,a */
	case 0x48:					/* ld c,b */
	case 0x49:					/* ld c,c */
	case 0x4A:					/* ld c,d */
	case 0x4B:					/* ld c,e */
	case 0x4C:					/* ld c,h */
	case 0x4D:					/* ld c,l */
	case 0x4F:					/* ld c,a */
	case 0x50:					/* ld d,b */
	case 0x51:					/* ld d,c */
	case 0x52:					/* ld d,d */
	case 0x53:					/* ld d,e */
	case 0x54:					/* ld d,h */
	case 0x55:					/* ld d,l */
	case 0x57:					/* ld d,a */
	case 0x58:					/* ld e,b */
	case 0x59:					/* ld e,c */
	case 0x5A:					/* ld e,d */
	case 0x5B:					/* ld e,e */
	case 0x5C:					/* ld e,h */
	case 0x5D:					/* ld e,l */
	case 0x5F:					/* ld e,a */
	case 0x60:					/* ld h,b */
	case 0x61:					/* ld h,c */
	case 0x62:					/* ld h,d */
	case 0x63:					/* ld h,e */
	case 0x64:					/* ld h,h */
	case 0x65:					/* ld h,l */
	case 0x67:					/* ld h,a */
	case 0x68:					/* ld l,b */
	case 0x69:					/* ld l,c */
	case 0x6A:					/* ld l,d */
	case 0x6B:					/* ld l,e */
	case 0x6C:					/* ld l,h */
	case 0x6D:					/* ld l,l */
	case 0x6F:					/* ld l,a */
	case 0x78:					/* ld a,b */
	case 0x79:					/* ld a,c */
	case 0x7A:					/* ld a,d */
	case 0x7B:					/* ld a,e */
	case 0x7C:					/* ld a,h */
	case 0x7D:					/* ld a,l */
	case 0x7F:					/* ld a,a */
		*REG[(t >> 3) & MASK3] = *REG[t & MASK3];
		break;

	case 0x46:					/* ld b,(hl) */
	case 0x4E:					/* ld c,(hl) */
	case 0x56:					/* ld d,(hl) */
	case 0x5E:					/* ld e,(hl) */
	case 0x66:					/* ld h,(hl) */
	case 0x6E:					/* ld l,(hl) */
	case 0x7E:					/* ld a,(hl) */
		*REG[(t >> 3) & MASK3] = MEM(HL);
		break;

	case 0x70:					/* ld (hl),b */
	case 0x71:					/* ld (hl),c */
	case 0x72:					/* ld (hl),d */
	case 0x73:					/* ld (hl),e */
	case 0x74:					/* ld (hl),h */
	case 0x75:					/* ld (hl),l */
	case 0x77:					/* ld (hl),a */
		SETMEM(HL, *REG[t & MASK3]);
		break;

	case 0x06:					/* ld b,n */
	case 0x0E:					/* ld c,n */
	case 0x16:					/* ld d,n */
	case 0x1E:					/* ld e,n */
	case 0x26:					/* ld h,n */
	case 0x2E:					/* ld l,n */
	case 0x3E:					/* ld a,n */
		*REG[(t >> 3) & MASK3] = MEM(PC);
		PC++;
		break;
	case 0x36:					/* ld (hl),nn */
		t1 = MEM(PC);
		PC++;
		SETMEM(HL, t1);
		break;

	case 0x0A:					/* ld a,(bc) */
	case 0x1A:					/* ld a,(de) */
		A = MEM(*REGPAIRAF[t >> 4]);
		break;

	case 0x02:					/* ld (bc),a */
	case 0x12:					/* ld (de),a */
		SETMEM(*REGPAIRAF[t >> 4], A);
		break;

	case 0x3A:					/* ld a,(nn) */
		t = MEM(PC);
		PC++;
		t1 = MEM(PC);
		A = MEM((t1 << 8) | t);
		PC++;
		break;
	case 0x32:					/* ld (nn),a */
		t = MEM(PC);
		PC++;
		t1 = MEM(PC);
		PC++;
		SETMEM((t1 << 8) | t, A);
		break;


	/* 16-bit load group */

	case 0x01:					/* ld bc,nn */
	case 0x11:					/* ld de,nn */
	case 0x21:					/* ld hl,nn */
	case 0x31:					/* ld sp,nn */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		*REGPAIRSP[(t >> 4) & MASK2] = tt;
		break;

	case 0x2A:					/* ld hl,(nn) */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		L = MEM(tt);
		tt++;
		H = MEM(tt);
		break;

	case 0x22:					/* ld (nn),hl */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		SETMEM(tt, L);
		tt++;
		SETMEM(tt, H);
		break;

	case 0xF9:					/* ld sp,hl */
		SP = HL;
		break;

	case 0xC5:					/* push bc */
	case 0xD5:					/* push de */
	case 0xE5:					/* push hl */
	case 0xF5:					/* push af */
		tt = *REGPAIRAF[(t >> 4) & MASK2];
		--SP;
		SETMEM(SP, tt >> 8);
		--SP;
		SETMEM(SP, tt & MASK8);
		break;

	case 0xC1:					/* pop bc */
	case 0xD1:					/* pop de */
	case 0xE1:					/* pop hl */
	case 0xF1:					/* pop af */
		rr = REGPAIRAF[(t >> 4) & MASK2];
		*rr = MEM(SP);
		SP++;
		*rr |= MEM(SP) << 8;
		SP++;
		break;


	/* exchange group and block transfer & search group */

	case 0x08:					/* ex af,af2 */
		swapw(AF, AF2);
		break;
	case 0xEB:					/* ex de,hl */
		swapw(DE, HL);
		break;
	case 0xD9:					/* exx */
		swapw(BC, BC2);
		swapw(DE, DE2);
		swapw(HL, HL2);
		break;
	case 0xE3:					/* ex (sp),hl */
		t1 = L;
		L = MEM(SP);
		SETMEM(SP, t1);
		t1 = H;
		H = MEM((SP + 1) & MASK16);
		SETMEM((SP + 1) & MASK16, t1);
		break;


	/* 8-bit arithmetic & logical group */

	case 0x80:					/* add a,b */
	case 0x81:					/* add a,c */
	case 0x82:					/* add a,d */
	case 0x83:					/* add a,e */
	case 0x84:					/* add a,h */
	case 0x85:					/* add a,l */
	case 0x87:					/* add a,a */
	case 0x88:					/* adc a,b */
	case 0x89:					/* adc a,c */
	case 0x8A:					/* adc a,d */
	case 0x8B:					/* adc a,e */
	case 0x8C:					/* adc a,h */
	case 0x8D:					/* adc a,l */
	case 0x8F:					/* adc a,a */
	case 0x90:					/* sub b */
	case 0x91:					/* sub c */
	case 0x92:					/* sub d */
	case 0x93:					/* sub e */
	case 0x94:					/* sub h */
	case 0x95:					/* sub l */
	case 0x97:					/* sub a */
	case 0x98:					/* sbc a,b */
	case 0x99:					/* sbc a,c */
	case 0x9A:					/* sbc a,d */
	case 0x9B:					/* sbc a,e */
	case 0x9C:					/* sbc a,h */
	case 0x9D:					/* sbc a,l */
	case 0x9F:					/* sbc a,a */
		arith8(*REG[t & MASK3], t & BIT3, t & BIT4);
		A = v;
		break;
	case 0x86:					/* add a,(hl) */
	case 0x8E:					/* adc a,(hl) */
	case 0x96:					/* sub (hl) */
	case 0x9E:					/* sbc a,(hl) */
		arith8(MEM(HL), t & BIT3, t & BIT4);
		A = v;
		break;
	case 0xC6:					/* add a,n */
	case 0xCE:					/* adc a,n */
	case 0xD6:					/* sub n */
	case 0xDE:					/* sbc a,n */
		arith8(MEM(PC), t & BIT3, t & BIT4);
		PC++;
		A = v;
		break;

	case 0xA0:					/* and b */
	case 0xA1:					/* and c */
	case 0xA2:					/* and d */
	case 0xA3:					/* and e */
	case 0xA4:					/* and h */
	case 0xA5:					/* and l */
	case 0xA7:					/* and a */
		A &= *REG[t & MASK3];
		logical(1);
		break;
	case 0xA6:					/* and (hl) */
		A &= MEM(HL);
		logical(1);
		break;
	case 0xE6:					/* and n */
		A &= MEM(PC);
		PC++;
		logical(1);
		break;

	case 0xA8:					/* xor b */
	case 0xA9:					/* xor c */
	case 0xAA:					/* xor d */
	case 0xAB:					/* xor e */
	case 0xAC:					/* xor h */
	case 0xAD:					/* xor l */
	case 0xAF:					/* xor a */
		A ^= *REG[t & MASK3];
		logical(0);
		break;
	case 0xAE:					/* xor (hl) */
		A ^= MEM(HL);
		logical(0);
		break;
	case 0xEE:					/* xor n */
		A ^= MEM(PC);
		PC++;
		logical(0);
		break;

	case 0xB0:					/* or b */
	case 0xB1:					/* or c */
	case 0xB2:					/* or d */
	case 0xB3:					/* or e */
	case 0xB4:					/* or h */
	case 0xB5:					/* or l */
	case 0xB7:					/* or a */
		A |= *REG[t & MASK3];
		logical(0);
		break;
	case 0xB6:					/* or (hl) */
		A |= MEM(HL);
		logical(0);
		break;
	case 0xF6:					/* or n */
		A |= MEM(PC);
		PC++;
		logical(0);
		break;

	case 0xB8:					/* cp b */
	case 0xB9:					/* cp c */
	case 0xBA:					/* cp d */
	case 0xBB:					/* cp e */
	case 0xBC:					/* cp h */
	case 0xBD:					/* cp l */
	case 0xBF:					/* cp a */
		arith8(*REG[t & MASK3], 0, 1);
		break;
	case 0xBE:					/* cp (hl) */
		arith8(MEM(HL), 0, 1);
		break;
	case 0xFE:					/* cp n */
		arith8(MEM(PC), 0, 1);
		PC++;
		break;

#ifdef NO_LARGE_SWITCH
	/* this is for compilers that cannot handle a large switch statement */
	/* neat, eh? */

	default:
		goto contsw;
	}

	goto infloop;

contsw:
	switch (t)
	{
#endif /* NO_LARGE_SWITCH */

	/* still the 8-bit arithmetic & logical group */

	case 0x04:					/* inc b */
	case 0x05:					/* dec b */
	case 0x0C:					/* inc c */
	case 0x0D:					/* dec c */
	case 0x14:					/* inc d */
	case 0x15:					/* dec d */
	case 0x1C:					/* inc e */
	case 0x1D:					/* dec e */
	case 0x24:					/* inc h */
	case 0x25:					/* dec h */
	case 0x2C:					/* inc l */
	case 0x2D:					/* dec l */
	case 0x3C:					/* inc a */
	case 0x3D:					/* dec a */
		r = REG[(t >> 3) & MASK3];
		increment(*r, t & BIT0);
		*r = tt;
		break;
	case 0x34:					/* inc (hl) */
	case 0x35:					/* dec (hl) */
		increment(MEM(HL), t & BIT0);
		SETMEM(HL, tt);
		break;


	/* general purpose arithmetic & CPU control groups */

	case 0x27:					/* daa - this is REALLY messy */
		t = 0x00;
		if (F & NEGATIVE)
		{
			if (F & CARRY)
			{
				if (F & HALF)
				{
					if ((A & MASKU4) >= 0x60 && (A & MASK4) >= 0x06)
						t = 0x9A;
				}
				else		/* no HALF carry */
				{
					if ((A & MASKU4) >= 0x70 && (A & MASK4) <= 0x09)
						t = 0xA0;
				}
			}
			else			/* no CARRY */
			{
				if (F & HALF)
				{
					if ((A & MASKU4) <= 0x80 && (A & MASK4) >= 0x06)
						t = 0xFA;
				}
			}
		}
		else				/* not NEGATIVE */
		{
			if (F & CARRY)
			{
				if (F & HALF)
				{
					if ((A & MASKU4) <= 0x30 && (A & MASK4) <= 0x03)
						t = 0x66;
				}
				else		/* no HALF */
				{
					if ((A & MASKU4) <= 0x20)
					{
						if ((A & MASK4) <= 0x09)
						{
							t = 0x60;
						}
						else
						{
							t = 0x66;
						}
					}
				}
			}
			else			/* no CARRY */
			{
				if (F & HALF)
				{
					if ((A & MASK4) <= 0x03)
					{
						if ((A & MASKU4) <= 0x90)
						{
							t = 0x06;
						}
						else
						{
							t = 0x66;
						}
					}
				}
				else		/* no HALF */
				{
					if ((A & MASK4) <= 0x09)
					{
						if ((A & MASKU4) >= 0xA0)
							t = 0x60;
					}
					else
					{
						if ((A & MASKU4) <= 0x80)
						{
							t = 0x06;
						}
						else
						{
							t = 0x66;
						}
					}
				}
			}
		}
		i = F & NEGATIVE;
		arith8(t, 0, 0);
		F |= i;
		A = v;
		setparity(A);
		break;

	case 0x2F:					/* cpl */
		A = ~A;
		flagon(HALF);
		flagon(NEGATIVE);
		break;

	case 0x3F:					/* ccf */
		setflag(CARRY, !(F & CARRY));
		flagoff(NEGATIVE);
		break;
	case 0x37:					/* scf */
		flagon(CARRY);
		flagoff(HALF);
		flagoff(NEGATIVE);
		break;

	case 0x00:					/* nop */
		break;
	case 0x76:					/* HALT */
		/*while (!EVENT)
			sleep(1);*/
		EVENT = HALT = TRUE;
		break;

	case 0xF3:					/* di */
		IFF = IFF2 = 0;
		break;
	case 0xFB:					/* ei */
		IFF = IFF2 = 1;
		break;


	/* 16-bit arithmetic group */

	case 0x09:					/* add hl,bc */
	case 0x19:					/* add hl,de */
	case 0x29:					/* add hl,hl */
	case 0x39:					/* add hl,sp */
		ttt = HL + *REGPAIRSP[(t >> 4) & MASK2];
		flagoff(NEGATIVE);
		setflag(CARRY, ttt & BIT16);
		HL = ttt;
		break;

	case 0x03:					/* inc bc */
	case 0x13:					/* inc de */
	case 0x23:					/* inc hl */
	case 0x33:					/* inc sp */
	case 0x0B:					/* dec bc */
	case 0x1B:					/* dec de */
	case 0x2B:					/* dec hl */
	case 0x3B:					/* dec sp */
		*REGPAIRSP[(t >> 4) & MASK2] += (t & BIT3) ? -1 : 1;
		break;


	/* rotate & shift group */

	case 0x07:					/* rlca */
	case 0x17:					/* rla */
	case 0x0F:					/* rrca */
	case 0x1F:					/* rra */
		t1 = F & CARRY;
		if (t & BIT3)
		{
			setflag(CARRY, A & BIT0);
			A >>= 1;
			t2 = BIT7;
		}
		else
		{
			setflag(CARRY, A & BIT7);
			A <<= 1;
			t2 = BIT0;
		}
		if (t & BIT4)
		{
			if (t1)
				A |= t2;
		}
		else
		{
			if (F & CARRY)
				A |= t2;
		}
		flagoff(HALF);
		flagoff(NEGATIVE);
		break;


	/* jump group */

	case 0xC3:					/* jp nn */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC = tt;
		break;
	case 0xC2:					/* jp nz,nn */
	case 0xD2:					/* jp nc,nn */
	case 0xE2:					/* jp po,nn */
	case 0xF2:					/* jp p,nn */
		if (F & flagmask[(t >> 4) & MASK2])
			PC += 2;
		else
		{
			tt = MEM(PC);
			PC++;
			tt |= MEM(PC) << 8;
			PC = tt;
		}
		break;
	case 0xCA:					/* jp z,nn */
	case 0xDA:					/* jp c,nn */
	case 0xEA:					/* jp p,nn */
	case 0xFA:					/* jp m,nn */
		if (F & flagmask[(t >> 4) & MASK2])
		{
			tt = MEM(PC);
			PC++;
			tt |= MEM(PC) << 8;
			PC = tt;
		}
		else
			PC += 2;
		break;

	case 0x18:					/* jr e */
		PC += ((signed char)MEM(PC)) + 1;
		break;
	case 0x20:					/* jr nz,e */
	case 0x30:					/* jr nc,e */
		if (!(F & flagmask[(t >> 4) & MASK1]))
			PC += ((signed char)MEM(PC)) + 1;
		else
			PC += 1;
		break;
	case 0x28:					/* jr z,e */
	case 0x38:					/* jr c,e */
		if (F & flagmask[(t >> 4) & MASK1])
			PC += ((signed char)MEM(PC)) + 1;
		else
			PC += 1;
		break;

	case 0xE9:					/* jp (hl) */
		PC = HL;
		break;
	case 0x10:					/* djnz e */
		if (--B)
			PC += ((signed char)MEM(PC)) + 1;
		else
			PC += 1;
		break;


	/* call & return group */

	case 0xCD:					/* call nn */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		--SP;
		SETMEM(SP, PC >> 8);
		--SP;
		SETMEM(SP, PC & MASK8);
		PC = tt;
		break;
	case 0xC4:					/* call nz,nn */
	case 0xD4:					/* call nc,nn */
	case 0xE4:					/* call po,nn */
	case 0xF4:					/* call p,nn */
		if (F & flagmask[(t >> 4) & MASK2])
			PC += 2;
		else
		{
			tt = MEM(PC);
			PC++;
			tt |= MEM(PC) << 8;
			PC++;
			--SP;
			SETMEM(SP, PC >> 8);
			--SP;
			SETMEM(SP, PC & MASK8);
			PC = tt;
		}
		break;
	case 0xCC:					/* call z,nn */
	case 0xDC:					/* call c,nn */
	case 0xEC:					/* call pe,nn */
	case 0xFC:					/* call m,nn */
		if (F & flagmask[(t >> 4) & MASK2])
		{
			tt = MEM(PC);
			PC++;
			tt |= MEM(PC) << 8;
			PC++;
			--SP;
			SETMEM(SP, PC >> 8);
			--SP;
			SETMEM(SP, PC & MASK8);
			PC = tt;
		}
		else
			PC += 2;
		break;

	case 0xC9:					/* ret */
		PC = MEM(SP);
		SP++;
		PC |= MEM(SP) << 8;
		SP++;
		break;
	case 0xC0:					/* ret nz */
	case 0xD0:					/* ret nc */
	case 0xE0:					/* ret po */
	case 0xF0:					/* ret p */
		if (!(F & flagmask[(t >> 4) & MASK2]))
		{
			PC = MEM(SP);
			SP++;
			PC |= MEM(SP) << 8;
			SP++;
		}
		break;
	case 0xC8:					/* ret z */
	case 0xD8:					/* ret c */
	case 0xE8:					/* ret pe */
	case 0xF8:					/* ret m */
		if (F & flagmask[(t >> 4) & MASK2])
		{
			PC = MEM(SP);
			SP++;
			PC |= MEM(SP) << 8;
			SP++;
		}
		break;

	case 0xC7:					/* rst 0 */
	case 0xCF:					/* rst 8 */
	case 0xD7:					/* rst 16 */
	case 0xDF:					/* rst 24 */
	case 0xE7:					/* rst 32 */
	case 0xEF:					/* rst 40 */
	case 0xF7:					/* rst 48 */
	case 0xFF:					/* rst 56 */
		--SP;
		SETMEM(SP, PC >> 8);
		--SP;
		SETMEM(SP, PC & MASK8);
		PC = t & 0x38;
		break;


	/* input & output group */

	case 0xDB:					/* in a,n */
		if (!input(z80, A, MEM(PC), &t1))
			return FALSE;

		A = t1;
		PC++;
		break;
	case 0xD3:					/* out a,n */
		output(z80, A, MEM(PC), A);
		PC++;
		break;


	default: 
		undefinstr(z80, t);
		break;
	}					/* end of main "switch" */

	/* Trace system calls */
	if (strace && PC == BDOS_HOOK)
	{
	        printf("\r\nbdos call %d %s (AF=%04x BC=%04x DE=%04x HL =%04x SP=%04x STACK=", C, bdos_decode(C), AF, BC, DE, HL, SP);
		for (i = 0; i < 8; ++i)
		    printf(" %4x", z80->mem[SP + 2*i]
			   + 256 * z80->mem[SP + 2*i + 1]);
		printf(")\r\n");
		bdos_return = SP + 2;
		if (bdos_fcb(C))
			bdos_fcb_dump(z80);
	}

	if (SP == bdos_return)
	{
	        printf("\r\nbdos return %d %s (AF=%04x BC=%04x DE=%04x HL =%04x SP=%04x STACK=", C, bdos_decode(C), AF, BC, DE, HL, SP);
		for (i = 0; i < 8; ++i)
		    printf(" %4x", z80->mem[SP + 2*i]
			   + 256 * z80->mem[SP + 2*i + 1]);
		printf(")\r\n");
		bdos_return = -1;
		if (bdos_fcb(C))
			bdos_fcb_dump(z80);
	}

	if (!nobdos && PC == BDOS_HOOK)
	{
		check_BDOS_hook(z80);
	}

	goto infloop;



	/* bit-twiddling instructions */
bitinstr:
	t = MEM(PC);
	PC++;

	switch (t)
	{
	/* rotate & shift group */

	case 0x00:					/* rlc b */
	case 0x01:					/* rlc c */
	case 0x02:					/* rlc d */
	case 0x03:					/* rlc e */
	case 0x04:					/* rlc h */
	case 0x05:					/* rlc l */
	case 0x07:					/* rlc a */
	case 0x08:					/* rrc b */
	case 0x09:					/* rrc c */
	case 0x0A:					/* rrc d */
	case 0x0B:					/* rrc e */
	case 0x0C:					/* rrc h */
	case 0x0D:					/* rrc l */
	case 0x0F:					/* rrc a */
	case 0x10:					/* rl b */
	case 0x11:					/* rl c */
	case 0x12:					/* rl d */
	case 0x13:					/* rl e */
	case 0x14:					/* rl h */
	case 0x15:					/* rl l */
	case 0x17:					/* rl a */
	case 0x18:					/* rr b */
	case 0x19:					/* rr c */
	case 0x1A:					/* rr d */
	case 0x1B:					/* rr e */
	case 0x1C:					/* rr h */
	case 0x1D:					/* rr l */
	case 0x1F:					/* rr a */
	case 0x20:					/* sla b */
	case 0x21:					/* sla c */
	case 0x22:					/* sla d */
	case 0x23:					/* sla e */
	case 0x24:					/* sla h */
	case 0x25:					/* sla l */
	case 0x27:					/* sla a */
	case 0x28:					/* sra b */
	case 0x29:					/* sra c */
	case 0x2A:					/* sra d */
	case 0x2B:					/* sra e */
	case 0x2C:					/* sra h */
	case 0x2D:					/* sra l */
	case 0x2F:					/* sra a */
	case 0x38:					/* srl b */
	case 0x39:					/* srl c */
	case 0x3A:					/* srl d */
	case 0x3B:					/* srl e */
	case 0x3C:					/* srl h */
	case 0x3D:					/* srl l */
	case 0x3F:					/* srl a */
		r = REG[t & MASK3];
		cy = F & CARRY;
		if (t & BIT3)
		{
			setflag(CARRY, *r & BIT0);
			*r >>= 1;
			t2 = BIT7;
		}
		else
		{
			setflag(CARRY, *r & BIT7);
			*r <<= 1;
			t2 = BIT0;
		}
		if (t & BIT5)
		{
			if (t2 == BIT7 && !(t & BIT4))
				if (*r & BIT6)
					*r |= BIT7;
		}
		else
		{
			if (t & BIT4)
			{
				if (cy)
					*r |= t2;
			}
			else
			{
				if (F & CARRY)
					*r |= t2;
			}
		}
		flags(*r);
		break;

	case 0x06:					/* rlc (hl) */
	case 0x0E:					/* rrc (hl) */
	case 0x16:					/* rl (hl) */
	case 0x1E:					/* rr (hl) */
	case 0x26:					/* sla (hl) */
	case 0x2E:					/* sra (hl) */
	case 0x3E:					/* srl (hl) */
		cy = F & CARRY;
		t1 = MEM(HL);
		if (t & BIT3)
		{
			setflag(CARRY, t1 & BIT0);
			t1 >>= 1;
			t2 = BIT7;
		}
		else
		{
			setflag(CARRY, t1 & BIT7);
			t1 <<= 1;
			t2 = BIT0;
		}
		if (t & BIT5)
		{
			if (t2 == BIT7 && !(t & BIT4))
				if (t1 & BIT6)
					t1 |= BIT7;
		}
		else
		{
			if (t & BIT4)
			{
				if (cy)
					t1 |= t2;
			}
			else
			{
				if (F & CARRY)
					t1 |= t2;
			}
		}
		SETMEM(HL, t1);
		flags(t1);
		break;


	/* bit set, reset, and test group */

	case 0x40:					/* bit 0,b */
	case 0x41:					/* bit 0,c */
	case 0x42:					/* bit 0,d */
	case 0x43:					/* bit 0,e */
	case 0x44:					/* bit 0,h */
	case 0x45:					/* bit 0,l */
	case 0x47:					/* bit 0,a */
	case 0x48:					/* bit 1,b */
	case 0x49:					/* bit 1,c */
	case 0x4A:					/* bit 1,d */
	case 0x4B:					/* bit 1,e */
	case 0x4C:					/* bit 1,h */
	case 0x4D:					/* bit 1,l */
	case 0x4F:					/* bit 1,a */
	case 0x50:					/* bit 2,b */
	case 0x51:					/* bit 2,c */
	case 0x52:					/* bit 2,d */
	case 0x53:					/* bit 2,e */
	case 0x54:					/* bit 2,h */
	case 0x55:					/* bit 2,l */
	case 0x57:					/* bit 2,a */
	case 0x58:					/* bit 3,b */
	case 0x59:					/* bit 3,c */
	case 0x5A:					/* bit 3,d */
	case 0x5B:					/* bit 3,e */
	case 0x5C:					/* bit 3,h */
	case 0x5D:					/* bit 3,l */
	case 0x5F:					/* bit 3,a */
	case 0x60:					/* bit 4,b */
	case 0x61:					/* bit 4,c */
	case 0x62:					/* bit 4,d */
	case 0x63:					/* bit 4,e */
	case 0x64:					/* bit 4,h */
	case 0x65:					/* bit 4,l */
	case 0x67:					/* bit 4,a */
	case 0x68:					/* bit 5,b */
	case 0x69:					/* bit 5,c */
	case 0x6A:					/* bit 5,d */
	case 0x6B:					/* bit 5,e */
	case 0x6C:					/* bit 5,h */
	case 0x6D:					/* bit 5,l */
	case 0x6F:					/* bit 5,a */
	case 0x70:					/* bit 6,b */
	case 0x71:					/* bit 6,c */
	case 0x72:					/* bit 6,d */
	case 0x73:					/* bit 6,e */
	case 0x74:					/* bit 6,h */
	case 0x75:					/* bit 6,l */
	case 0x77:					/* bit 6,a */
	case 0x78:					/* bit 7,b */
	case 0x79:					/* bit 7,c */
	case 0x7A:					/* bit 7,d */
	case 0x7B:					/* bit 7,e */
	case 0x7C:					/* bit 7,h */
	case 0x7D:					/* bit 7,l */
	case 0x7F:					/* bit 7,a */
		r = REG[t & MASK3];
		resetflag(ZERO, *r & bitmask[(t >> 3) & MASK3]);
		flagon(HALF);
		flagoff(NEGATIVE);
		break;
	case 0x46:					/* bit 0,(hl) */
	case 0x4E:					/* bit 1,(hl) */
	case 0x56:					/* bit 2,(hl) */
	case 0x5E:					/* bit 3,(hl) */
	case 0x66:					/* bit 4,(hl) */
	case 0x6E:					/* bit 5,(hl) */
	case 0x76:					/* bit 6,(hl) */
	case 0x7E:					/* bit 7,(hl) */
		resetflag(ZERO, MEM(HL) & bitmask[(t >> 3) & MASK3]);
		flagon(HALF);
		flagoff(NEGATIVE);
		break;

	case 0x80:					/* res 0,b */
	case 0x81:					/* res 0,c */
	case 0x82:					/* res 0,d */
	case 0x83:					/* res 0,e */
	case 0x84:					/* res 0,h */
	case 0x85:					/* res 0,l */
	case 0x87:					/* res 0,a */
	case 0x88:					/* res 1,b */
	case 0x89:					/* res 1,c */
	case 0x8A:					/* res 1,d */
	case 0x8B:					/* res 1,e */
	case 0x8C:					/* res 1,h */
	case 0x8D:					/* res 1,l */
	case 0x8F:					/* res 1,a */
	case 0x90:					/* res 2,b */
	case 0x91:					/* res 2,c */
	case 0x92:					/* res 2,d */
	case 0x93:					/* res 2,e */
	case 0x94:					/* res 2,h */
	case 0x95:					/* res 2,l */
	case 0x97:					/* res 2,a */
	case 0x98:					/* res 3,b */
	case 0x99:					/* res 3,c */
	case 0x9A:					/* res 3,d */
	case 0x9B:					/* res 3,e */
	case 0x9C:					/* res 3,h */
	case 0x9D:					/* res 3,l */
	case 0x9F:					/* res 3,a */
	case 0xA0:					/* res 4,b */
	case 0xA1:					/* res 4,c */
	case 0xA2:					/* res 4,d */
	case 0xA3:					/* res 4,e */
	case 0xA4:					/* res 4,h */
	case 0xA5:					/* res 4,l */
	case 0xA7:					/* res 4,a */
	case 0xA8:					/* res 5,b */
	case 0xA9:					/* res 5,c */
	case 0xAA:					/* res 5,d */
	case 0xAB:					/* res 5,e */
	case 0xAC:					/* res 5,h */
	case 0xAD:					/* res 5,l */
	case 0xAF:					/* res 5,a */
	case 0xB0:					/* res 6,b */
	case 0xB1:					/* res 6,c */
	case 0xB2:					/* res 6,d */
	case 0xB3:					/* res 6,e */
	case 0xB4:					/* res 6,h */
	case 0xB5:					/* res 6,l */
	case 0xB7:					/* res 6,a */
	case 0xB8:					/* res 7,b */
	case 0xB9:					/* res 7,c */
	case 0xBA:					/* res 7,d */
	case 0xBB:					/* res 7,e */
	case 0xBC:					/* res 7,h */
	case 0xBD:					/* res 7,l */
	case 0xBF:					/* res 7,a */
		*REG[t & MASK3] &= ~bitmask[(t >> 3) & MASK3];
		break;
	case 0x86:					/* res 0,(hl) */
	case 0x8E:					/* res 1,(hl) */
	case 0x96:					/* res 2,(hl) */
	case 0x9E:					/* res 3,(hl) */
	case 0xA6:					/* res 4,(hl) */
	case 0xAE:					/* res 5,(hl) */
	case 0xB6:					/* res 6,(hl) */
	case 0xBE:					/* res 7,(hl) */
		t1 = MEM(HL) & ~bitmask[(t >> 3) & MASK3];
		SETMEM(HL, t1);
		break;


	case 0xC0:					/* set 0,b */
	case 0xC1:					/* set 0,c */
	case 0xC2:					/* set 0,d */
	case 0xC3:					/* set 0,e */
	case 0xC4:					/* set 0,h */
	case 0xC5:					/* set 0,l */
	case 0xC7:					/* set 0,a */
	case 0xC8:					/* set 1,b */
	case 0xC9:					/* set 1,c */
	case 0xCA:					/* set 1,d */
	case 0xCB:					/* set 1,e */
	case 0xCC:					/* set 1,h */
	case 0xCD:					/* set 1,l */
	case 0xCF:					/* set 1,a */
	case 0xD0:					/* set 2,b */
	case 0xD1:					/* set 2,c */
	case 0xD2:					/* set 2,d */
	case 0xD3:					/* set 2,e */
	case 0xD4:					/* set 2,h */
	case 0xD5:					/* set 2,l */
	case 0xD7:					/* set 2,a */
	case 0xD8:					/* set 3,b */
	case 0xD9:					/* set 3,c */
	case 0xDA:					/* set 3,d */
	case 0xDB:					/* set 3,e */
	case 0xDC:					/* set 3,h */
	case 0xDD:					/* set 3,l */
	case 0xDF:					/* set 3,a */
	case 0xE0:					/* set 4,b */
	case 0xE1:					/* set 4,c */
	case 0xE2:					/* set 4,d */
	case 0xE3:					/* set 4,e */
	case 0xE4:					/* set 4,h */
	case 0xE5:					/* set 4,l */
	case 0xE7:					/* set 4,a */
	case 0xE8:					/* set 5,b */
	case 0xE9:					/* set 5,c */
	case 0xEA:					/* set 5,d */
	case 0xEB:					/* set 5,e */
	case 0xEC:					/* set 5,h */
	case 0xED:					/* set 5,l */
	case 0xEF:					/* set 5,a */
	case 0xF0:					/* set 6,b */
	case 0xF1:					/* set 6,c */
	case 0xF2:					/* set 6,d */
	case 0xF3:					/* set 6,e */
	case 0xF4:					/* set 6,h */
	case 0xF5:					/* set 6,l */
	case 0xF7:					/* set 6,a */
	case 0xF8:					/* set 7,b */
	case 0xF9:					/* set 7,c */
	case 0xFA:					/* set 7,d */
	case 0xFB:					/* set 7,e */
	case 0xFC:					/* set 7,h */
	case 0xFD:					/* set 7,l */
	case 0xFF:					/* set 7,a */
		*REG[t & MASK3] |= bitmask[(t >> 3) & MASK3];
		break;

	case 0xC6:					/* set 0,(hl) */
	case 0xCE:					/* set 1,(hl) */
	case 0xD6:					/* set 2,(hl) */
	case 0xDE:					/* set 3,(hl) */
	case 0xE6:					/* set 4,(hl) */
	case 0xEE:					/* set 5,(hl) */
	case 0xF6:					/* set 6,(hl) */
	case 0xFE:					/* set 7,(hl) */
		t1 = MEM(HL) | bitmask[(t >> 3) & MASK3];
		SETMEM(HL, t1);
		break;

	default: 
		undefinstr(z80, t);
		break;
	}	/* end of "bitinstr" "switch" */

	goto infloop;



	/* index-register instructions */
ireginstr:

	/* pointer to either the IX or the IY register */
	rr = REGIXY[(t >> 5) & MASK1];
	t = MEM(PC);
	PC++;

	/* note: in comments below, "ir" is either "ix" or "iy" */
	switch (t)
	{
	case 0xCB:		/* index-register bit-twiddling instructions */
		goto iregbitinstr;
		break;


	/* 8-bit load group */

	case 0x46:					/* ld b,(ir+d) */
	case 0x4E:					/* ld c,(ir+d) */
	case 0x56:					/* ld d,(ir+d) */
	case 0x5E:					/* ld e,(ir+d) */
	case 0x66:					/* ld h,(ir+d) */
	case 0x6E:					/* ld l,(ir+d) */
	case 0x7E:					/* ld a,(ir+d) */
		i = (t >> 3) & MASK3;
		j = (int)((signed char)MEM(PC));
		PC++;
		*REG[i] = MEM(((int)*rr + j) & MASK16);
		break;

	case 0x70:					/* ld (ir+d),b */
	case 0x71:					/* ld (ir+d),c */
	case 0x72:					/* ld (ir+d),d */
	case 0x73:					/* ld (ir+d),e */
	case 0x74:					/* ld (ir+d),h */
	case 0x75:					/* ld (ir+d),l */
	case 0x77:					/* ld (ir+d),a */
		t1 = MEM(PC);
		PC++;
		SETMEM(((int)*rr + ((signed char)t1)) & MASK16, *REG[t &MASK3]);
		break;


	/* 16-bit load group */

	case 0x36:					/* ld (ir+d),n */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		t1 = MEM(PC);
		PC++;
		SETMEM(tt, t1);
		break;

	case 0x21:					/* ld ir,nn */
		*rr = MEM(PC);
		PC++;
		*rr |= MEM(PC) << 8;
		PC++;
		break;

	case 0x2A:					/* ld ir,(nn) */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		*rr = MEM(tt);
		tt++;
		*rr |= MEM(tt) << 8;
		break;

	case 0x22:					/* ld (nn),ir */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		SETMEM(tt, *rr & MASK8);
		tt++;
		SETMEM(tt, *rr >> 8);
		break;

	case 0xF9:					/* ld sp,ir */
		SP = *rr;
		break;

	case 0xE5:					/* push ir */
		--SP;
		SETMEM(SP, *rr >> 8);
		--SP;
		SETMEM(SP, *rr & MASK8);
		break;

	case 0xE1:					/* pop ir */
		*rr = MEM(SP);
		SP++;
		*rr |= MEM(SP) << 8;
		SP++;
		break;


	/* exchange group */

	case 0xE3:					/* ex sp,ir */
		tt = MEM(SP);
		tt |= MEM(SP + 1) << 8;
		SETMEM(SP, *rr & MASK8);
		SETMEM(SP + 1, *rr >> 8);
		*rr = tt;
		break;


	/* 8-bit arithmetic group */

	case 0x86:					/* add a,(ir+d) */
	case 0x8E:					/* adc a,(ir+d) */
	case 0x96:					/* sub (ir+d) */
	case 0x9E:					/* sbc a,(ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		arith8(MEM(tt), t & BIT3, t & BIT4);
		A = v;
		break;

	case 0x34:					/* inc (ir+d) */
	case 0x35:					/* dec (ir+d) */
		tt2 = (int)*rr + ((signed char)MEM(PC));
		PC++;
		increment(MEM(tt2), t & BIT0);
		SETMEM(tt2, tt);
		break;

	case 0xA6:					/* and (ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		A &= MEM(tt);
		logical(1);
		break;
	case 0xAE:					/* xor (ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		A ^= MEM(tt);
		logical(0);
		break;
	case 0xB6:					/* or (ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		A |= MEM(tt);
		logical(0);
		break;
	case 0xBE:					/* cp (ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		arith8(MEM(tt), 0, 1);
		break;


	/* 16-bit arithmetic group */

	case 0x09:					/* add ir,bc */
	case 0x19:					/* add ir,de */
	case 0x29:					/* add ir,rr */
	case 0x39:					/* add ir,sp */
		REGPAIRXY[XYPAIR] = rr;
		i = *rr;
		j = *REGPAIRXY[(t >> 4) & MASK2];
		ttt = i + j;
		flagoff(NEGATIVE);
		setflag(CARRY, ttt & BIT16);
		*rr = ttt;
		break;

	case 0x23:					/* inc ir */
	case 0x2B:					/* dec ir */
		*rr += (t & BIT3) ? -1 : 1;
		break;


	/* jump group */

	case 0xE9:					/* jp (ir) */
		PC = *rr;
		break;


	default:
		undefinstr(z80, t);
		break;
	}	/* end of "ireginstr" "switch" */

	goto infloop;



	/* extended instructions */
extinstr: 
	t = MEM(PC);
	PC++;
	switch (t)
	{
	/* 8-bit load group */

	case 0x57:					/* ld a,i */
	case 0x5F:					/* ld a,r */
		A = *REGIR[(t >> 3) & MASK1];
		setsign();
		setzero();
		flagoff(HALF);
		setflag(PARITY, IFF2);
		flagoff(NEGATIVE);
		break;

	case 0x47:					/* ld i,a */
	case 0x4F:					/* ld r,a */
		*REGIR[(t >> 3) & MASK1] = A;
		break;


	/* 16-bit load group */

	case 0x4B:					/* ld bc,(nn) */
	case 0x5B:					/* ld de,(nn) */
	case 0x6B:					/* ld hl,(nn) */
	case 0x7B:					/* ld sp,(nn) */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		tt2 = MEM(tt);
		tt++;
		tt2 |= MEM(tt) << 8;
		*REGPAIRSP[(t >> 4) & MASK2] = tt2;
		break;

	case 0x43:					/* ld (nn),bc */
	case 0x53:					/* ld (nn),de */
	case 0x63:					/* ld (nn),hl */
	case 0x73:					/* ld (nn),sp */
		tt = MEM(PC);
		PC++;
		tt |= MEM(PC) << 8;
		PC++;
		tt2 = *REGPAIRSP[(t >> 4) & MASK2];
		SETMEM(tt, tt2 & MASK8);
		tt++;
		SETMEM(tt, tt2 >> 8);
		break;


	/* block transfer and search group */

	case 0xA0:					/* ldi */
	case 0xA8:					/* ldd */
	case 0xB0:					/* ldir */
	case 0xB8:					/* lddr */
		if (t & BIT3)
		{
			t1 = MEM(HL);
			HL--;
			SETMEM(DE, t1);
			DE--;
		}
		else
		{
			t1 = MEM(HL);
			HL++;
			SETMEM(DE, t1);
			DE++;
		}

		setflag(OVERFLOW, --BC);

		if ((t & BIT4) && BC)
			PC -= 2;

		flagoff(HALF);
		flagoff(NEGATIVE);
		break;

	case 0xA1:					/* cpi */
	case 0xA9:					/* cpd */
	case 0xB1:					/* cpir */
	case 0xB9:					/* cpdr */
		t1 = MEM(HL);

		if (t & BIT3)
		    HL--;
		else
		    HL++;

		t2 = A - t1;
		setflag(SIGN, t2 & BIT7);
		setflag(ZERO, !t2);
		setflag(HALF, (A & MASK4) < (t1 & MASK4));
		setflag(OVERFLOW, --BC);
		flagon(NEGATIVE);
		if ((t & BIT4) && t2 && BC)
			PC -= 2;
		break;


	/* general purpose arithmetic and cpu control groups */

	case 0x44:					/* neg */
		A = ~A;
		arith8(1, 0, 0);
		A = v;
		/* flagon(HALF); */
		flagon(NEGATIVE);
		break;

	case 0x46:					/* im 0 */
		IMODE = 0;
		break;
	case 0x56:					/* im 1 */
		IMODE = 1;
		break;
	case 0x5E:					/* im 2 */
		IMODE = 2;
		break;


	/* 16-bit arithmetic group */

	case 0x4A:					/* adc hl,bc */
	case 0x5A:					/* adc hl,de */
	case 0x6A:					/* adc hl,hl */
	case 0x7A:					/* adc hl,sp */
	case 0x42:					/* sbc hl,bc */
	case 0x52:					/* sbc hl,de */
	case 0x62:					/* sbc hl,hl */
	case 0x72:					/* sbc hl,sp */
		vv = *REGPAIRSP[(t >> 4) & MASK2];
		n = !(t & BIT3);
		if (n)
		{
			ttt = (int)HL - (int)vv - ((F & CARRY) ? 1 : 0);
			setflag(OVERFLOW, (HL & BIT15) != (vv & BIT15) &&
					(vv & BIT15) == (ttt & BIT15));
		}
		else
		{
			ttt = (int)HL + (int)vv + ((F & CARRY) ? 1 : 0);
			setflag(OVERFLOW, (HL & BIT15) == (vv & BIT15) &&
					(vv & BIT15) != (ttt & BIT15));
		}
		setflag(SIGN, ttt & BIT15);
		setflag(ZERO, !ttt);
		setflag(NEGATIVE, n);
		setflag(CARRY, ttt & BIT16);
		HL = ttt;
		break;


	/* rotate & shift group */

	case 0x67:				/* rrd */
	case 0x6F:				/* rld */
		t1 = MEM(HL);
		if (t & BIT3)
		{
			SETMEM(HL, (t1 << 4) | (A & MASK4));
			A = (A & MASKU4) | (t1 >> 4);
		}
		else
		{
			SETMEM(HL, (t1 >> 4) | (A << 4));
			A = (A & MASKU4) | (t1 & MASK4);
		}
		flags(A);
		break;


	/* call & return group */

	case 0x45:					/* retn */
		IFF = IFF2;
		PC = MEM(SP);
		SP++;
		PC |= MEM(SP) << 8;
		SP++;
		break;
	case 0x4D:					/* reti */
		PC = MEM(SP);
		SP++;
		PC |= MEM(SP) << 8;
		SP++;
		break;


	/* input & output group */

	case 0x40:					/* in b,c */
	case 0x48:					/* in c,c */
	case 0x50:					/* in d,c */
	case 0x58:					/* in e,c */
	case 0x60:					/* in h,c */
	case 0x68:					/* in l,c */
	case 0x70:					/* in ?,c */
	case 0x78:					/* in a,c */
		if (!input(z80, B, C, &t1))
			return FALSE;

		v = *REG[(t >> 3) & MASK3] = t1;
		setflag(SIGN, v & BIT7);
		resetflag(ZERO, v);
		flagoff(HALF);
		setparity(v);
		flagoff(NEGATIVE);
		break;

	case 0x49:					/* out c,c */
	case 0x51:					/* out d,c */
	case 0x59:					/* out e,c */
	case 0x61:					/* out h,c */
	case 0x69:					/* out l,c */
	case 0x79:					/* out a,c */
	case 0x41:					/* out b,c */
		output(z80, B, C, *REG[(t >> 3) & MASK3]);
		break;

	case 0xA2:					/* ini */
	case 0xAA:					/* ind */
	case 0xB2:					/* inir */
	case 0xBA:					/* indr */
		if (!input(z80, B, C, &t1))
			return FALSE;

		SETMEM(HL, t1);

		if (t & BIT3)
			HL--;
		else
			HL++;

		resetflag(ZERO, --B);
		flagon(NEGATIVE);

		if ((t & BIT4) && B)
			PC -= 2;

		break;

	case 0xA3:					/* outi */
	case 0xAB:					/* outd */
	case 0xB3:					/* otir */
	case 0xBB:					/* otdr */
		resetflag(ZERO, --B);
		output(z80, B, C, MEM(HL));

		if (t & BIT3)
			HL--;
		else
			HL++;

		flagon(NEGATIVE);

		if ((t & BIT4) && B)
			PC -= 2;

		break;


	default:
		undefinstr(z80, t);
		break;
	}	/* end of "extinstr" "switch" */

	goto infloop;


	/* index-register bit-twiddling instuctions */
iregbitinstr:

	/* note: we have to look ahead 1 byte for the opcode  -- the PC is
	   bumped later after the "switch" */
	switch (t = MEM((PC + 1) & 0xFFFF))
	{

	/* rotate & shift group */

	case 0x06:					/* rlc (ir+d) */
	case 0x0E:					/* rrc (ir+d) */
	case 0x16:					/* rl (ir+d) */
	case 0x1E:					/* rr (ir+d) */
	case 0x26:					/* sla (ir+d) */
	case 0x2E:					/* sra (ir+d) */
	case 0x3E:					/* srl (ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		t1 = MEM(tt);
		cy = F & CARRY;
		if (t & BIT3)
		{
			setflag(CARRY, t1 & BIT0);
			t1 >>= 1;
			t2 = BIT7;
		}
		else
		{
			setflag(CARRY, t1 & BIT7);
			t1 <<= 1;
			t2 = BIT0;
		}
		if (t & BIT5)
		{
			if (t2 == BIT7 && !(t & BIT4))
				if (t1 & BIT6)
					t1 |= BIT7;
		}
		else
		{
			if (t & BIT4)
			{
				if (cy)
					t1 |= t2;
			}
			else
			{
				if (F & CARRY)
					t1 |= t2;
			}
		}
		SETMEM(tt, t1);
		flags(t1);
		break;


	/* bit set, reset, & test group */

	case 0x46:					/* bit 0,(ir+d) */
	case 0x4E:					/* bit 1,(ir+d) */
	case 0x56:					/* bit 2,(ir+d) */
	case 0x5E:					/* bit 3,(ir+d) */
	case 0x66:					/* bit 4,(ir+d) */
	case 0x6E:					/* bit 5,(ir+d) */
	case 0x76:					/* bit 6,(ir+d) */
	case 0x7E:					/* bit 7,(ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		resetflag(ZERO, MEM(tt) & bitmask[(t >> 3) & MASK3]);
		flagon(HALF);
		flagoff(NEGATIVE);
		break;

	case 0x86:					/* res 0,(ir+d) */
	case 0x8E:					/* res 1,(ir+d) */
	case 0x96:					/* res 2,(ir+d) */
	case 0x9E:					/* res 3,(ir+d) */
	case 0xA6:					/* res 4,(ir+d) */
	case 0xAE:					/* res 5,(ir+d) */
	case 0xB6:					/* res 6,(ir+d) */
	case 0xBE:					/* res 7,(ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		t1 = MEM(tt) & ~bitmask[(t >> 3) & MASK3];
		SETMEM(tt, t1);
		break;

	case 0xC6:					/* set 0,(ir+d) */
	case 0xCE:					/* set 1,(ir+d) */
	case 0xD6:					/* set 2,(ir+d) */
	case 0xDE:					/* set 3,(ir+d) */
	case 0xE6:					/* set 4,(ir+d) */
	case 0xEE:					/* set 5,(ir+d) */
	case 0xF6:					/* set 6,(ir+d) */
	case 0xFE:					/* set 7,(ir+d) */
		tt = (int)*rr + ((signed char)MEM(PC));
		PC++;
		t1 = MEM(tt) | bitmask[(t >> 3) & MASK3];
		SETMEM(tt, t1);
		break;


	default: 
		undefinstr(z80, t);
		break;
	}	/* end of "iregbitinstr" "switch" */

	PC++;	/* bump the PC here instead */
	goto infloop;

}		/* end of "z80_emulator()" */



/* initialize the z80 struct with sane stuff */
z80info *
init_z80info(z80info *z80)
{
	int i;

	/* clear it the easy way */
	memset(z80, 0, sizeof *z80);

	/* could allocate memory if need be here - check for NULL malloc */
	/* z80->mem = malloc(0x10000L); */

	/* initialize the generic byte registers */
	REG[0] = &B;
	REG[1] = &C;
	REG[2] = &D;
	REG[3] = &E;
	REG[4] = &H;
	REG[5] = &L;
	/* REG[6] = NULL; */
	REG[7] = &A;

	/* initialize the register pairs with AF */
	REGPAIRAF[0] = &BC;
	REGPAIRAF[1] = &DE;
	REGPAIRAF[2] = &HL;
	REGPAIRAF[3] = &AF;

	/* register pairs with SP instead of AF */
	REGPAIRSP[0] = &BC;
	REGPAIRSP[1] = &DE;
	REGPAIRSP[2] = &HL;
	REGPAIRSP[3] = &SP;

	/* register pairs with IX/IY in place of HL - it is set to either the
	   address of IX or IY depending on the opcode during runtime */
	REGPAIRXY[0] = &BC;
	REGPAIRXY[1] = &DE;
	/* REGPAIRXY[2] = NULL; */
	REGPAIRXY[3] = &SP;

	/* index registers */
	REGIXY[0] = &IX;
	REGIXY[1] = &IY;

	/* interrupt vector & memory refresh registers */
	REGIR[0] = &I;
	REGIR[1] = &R;

	/* initialize the other misc stuff */
	z80->trace = FALSE;
	z80->step = FALSE;
	z80->sig = 0;
	z80->syscall = FALSE;

	/* initialize the CP/M BIOS data */
	z80->drive = 0;
	z80->dma = 0x80;
	z80->track = 0;
	z80->sector = 1;

	/* initialize the global parity array if necessary */
	if (!parity_inited)
	{
		for (i = 0; i <= 0xFF; i++)
		{
			word tt = 1;
			byte t;

			for (t = 1; t; t <<= 1)
				if (i & t)
					tt++;

			parityarr[i] = tt & BIT0;
		}

		parity_inited = TRUE;
	}

	return z80;
}

z80info *
destroy_z80info(z80info *z80)
{
	/* free the mem array if allocated above */
	/* free(z80->mem); */
	return z80;
}

z80info *
new_z80info(void)
{
	z80info *z80 = (z80info*)malloc(sizeof *z80);

	if (z80 == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for a z80 object\n");
		return NULL;
	}

	return init_z80info(z80);
}

void
delete_z80info(z80info *z80)
{
	if (z80 == NULL)
		return;

	destroy_z80info(z80);
	free(z80);
}
