/*-----------------------------------------------------------------------*\
 |  disassem.c  --  Z80 disassembler                                     |
 |                                                                       |
 |  Originally by T.J. Merritt but modified and debugged to run in the   |
 |  Z80 emulator instead of being standalone.                            |
 |                                                                       |
 |  Copyright 1986-1988 by Parag Patel.  All Rights Reserved.            |
 |  Copyright 1994-1995 by CodeGen, Inc.  All Rights Reserved.           |
\*-----------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include "defs.h"


#define bits(l,r) (((z80->mem[loc]) >> (r)) & mask[(l) - (r)])
#define put_cc(cc) put_str(cc_names[cc])

#define OPC_LD "LD    "
#define OPC_INC "INC   "
#define OPC_DEC "DEC   "
#define OPC_ADD "ADD   "
#define OPC_RET "RET   "
#define OPC_POP "POP   "
#define OPC_EXX "EXX   "
#define OPC_JP "JP    "
#define OPC_OUT "OUT   "
#define OPC_IN "IN    "
#define OPC_EX "EX    "
#define OPC_DI "DI    "
#define OPC_EI "EI    "
#define OPC_CALL "CALL  "
#define OPC_PUSH "PUSH  "
#define OPC_ILLEGAL "***"
#define OPC_RST "RST   "

static int
mask[] =
{
	0x001,
	0x003,
	0x007,
	0x00F,
	0x01F,
	0x03F,
	0x07F,
	0x0FF
};

static char *
rr_names[] =
{
	"B",
	"C",
	"D",
	"E",
	"H",
	"L",
	"(HL)",
	"A"
};

static char *
dd_names[] =
{
	"BC",
	"DE",
	"HL",
	"SP"
};

static char *
qq_names[] =
{
	"BC",
	"DE",
	"HL",
	"AF"
};

static char *
cc_names[] =
{
	"NZ",
	"Z",
	"NC",
	"C",
	"PO",
	"PE",
	"P",
	"M"
};

static char *
op_names[] =
{
	"ADD   A,",
	"ADC   A,",
	"SUB   ",
	"SBC   A,",
	"AND   ",
	"XOR   ",
	"OR    ",
	"CP    ",
};

static char *
jr_op_names[] =
{
	0,
	0,
	"DJNZ  ",
	"JR    ",
	"JR    NZ,",
	"JR    Z,",
	"JR    NC,",
	"JR    C,"
};

static char *
log_op_names[] =
{
	"RLCA",
	"RRCA",
	"RLA",
	"RRA",
	"DAA",
	"CPL",
	"SCF",
	"CCF"
};

static char *
shf_op_names[] =
{
	"RLC   ",
	"RRC   ",
	"RL    ",
	"RR    ",
	"SLA   ",
	"SRA   ",
	"SLL?  ",
	"SRL   "
};

static char *
bit_op_names[] =
{
	0,
	"BIT   ",
	"RES   ",
	"SET   ",
};

static char *
rep_op_names[] =
{
	"LDI",
	"CPI",
	"INI",
	"OUTI",
	"LDD",
	"CPD",
	"IND",
	"OUTD",
	"LDIR",
	"CPIR",
	"INIR",
	"OTIR",
	"LDDR",
	"CPDR",
	"INDR",
	"OTDR"
};

static char *index_reg;
static int str_length;
static FILE *fp;

static void
put_byte(byte b)
{
	fprintf(fp, "%02X", b & 0x0FF);
	str_length += 2;
}

static void
put_addr(z80info *z80, int loc)
{
	put_byte(z80->mem[loc]);
	put_byte(z80->mem[(loc - 1) & 0x0FFFF]);
}

static void
put_word(word w)
{
	fprintf(fp, "%04X", w & 0x0FFFF);
	str_length += 4;
}

static void
put_digit(byte digit)
{
	fprintf(fp, "%01X", digit & 0x00F);
	str_length += 1;
}

static void
put_char(char ch)
{
	putc(ch, fp);
	str_length += 1;
}

static void
put_str(char *str)
{
	fprintf(fp, "%s", str);
	str_length += strlen(str);
}

static void
put_reg(z80info *z80, int reg_num, int loc)
{
	int d;
	
	if (index_reg && (reg_num == 6))
	{
		put_str("(");
		put_str(index_reg);
		d = (signed char)z80->mem[loc + 1];

		if (d < 0)
		{
			put_str("-");
			put_byte(-d);
		}
		else
		{
			put_str("+");
			put_byte(d);
		}

		put_str(")");
	}
	else
	{
		put_str(rr_names[reg_num]);
	}
}

static void 
put_dd_reg(int reg_num)
{
	if (index_reg)
	{
		if (reg_num == 2)
			put_str(index_reg);
		else if (reg_num == 3)
			put_str("SP");
		else
			put_str(OPC_ILLEGAL);
	}
	else
		put_str(dd_names[reg_num]);
}

static void
put_qq_reg(int reg_num)
{
	if (index_reg)
	{
		if (reg_num == 2)
			put_str(index_reg);
		else
			put_str(OPC_ILLEGAL);
	}
	else
		put_str(qq_names[reg_num]);
}

int
disassemlen(z80info *z80)
{
	return str_length;
}

int
disassem(z80info *z80, word start, FILE *file)
{
	word loc;
	int byte_count;
	byte last_byte;

	loc = start;
	byte_count = 0;
	str_length = 0;

	if (file != NULL)
	{
		fp = file;
		index_reg = NULL;
	}

	switch (bits(7,6))
	{
	case 0:
		switch (bits(3,0))
		{
		case 0:
		case 8:
			if (bits(5,3) == 0)
			{
				put_str("NOP");
				break;
			}

			if (bits(5,3) == 1)
			{
				put_str("EX    AF,AF'");
				break;
			}

			put_str(jr_op_names[bits(5,3)]);
			loc++;
			byte_count++;
			put_word((signed char)z80->mem[loc] + (int)loc + 1);
			break;

		case 1:
			put_str(OPC_LD);
			put_dd_reg(bits(5,4));
			put_char(',');
			byte_count += 2;
			loc += 2;
			put_addr(z80, loc);
			break;

		case 2:
			last_byte = z80->mem[loc];
			put_str(OPC_LD);

			switch (bits(5,4))
			{
			case 0:
				put_str("(BC)");
				break;

			case 1:
				put_str("(DE)");
				break;

			case 2:
			case 3:
				put_char('(');
				byte_count += 2;
				loc += 2;
				put_addr(z80, loc);
				put_char(')');
				break;
			}

			if ((last_byte & 0x030) == 0x020)
			{
				put_char(',');

				if (index_reg)
					put_str(index_reg);
				else
					put_str("HL");
			}
			else
			{
				put_str(",A");
			}

			break;

		case 3:
			put_str(OPC_INC);
			put_dd_reg(bits(5,4));
			break;

		case 4:
		case 0x0C:
			put_str(OPC_INC);
			put_reg(z80, bits(5,3), loc);
			break;

		case 5:
		case 0x0D:
			put_str(OPC_DEC);
			put_reg(z80, bits(5,3), loc);
			break;

		case 6:
		case 0x0E:
			put_str(OPC_LD);
			put_reg(z80, bits(5,3), loc);
			put_char(',');

			if (index_reg && (bits(5,3) == 6))
			{
				loc++;
				byte_count++;
			}

			loc++;
			byte_count++;
			put_byte(z80->mem[loc]);
			break;

		case 7:
		case 0x0F:
			put_str(log_op_names[bits(5,3)]);
			break;

		case 9:
			put_str(OPC_ADD);
			put_str("HL,");
			put_dd_reg(bits(5,4));
			break;

		case 0x0A:
			put_str(OPC_LD);

			if (bits(5,4) == 2)
			{
				if (index_reg)
				{
					put_str(index_reg);
					put_str(",");
				}
				else
					put_str("HL,");
			}
			else
			{
				put_str("A,");
			}

			switch (bits(5,4))
			{
			case 0:
				put_str("(BC)");
				break;

			case 1:
				put_str("(DE)");
				break;

			case 2:
			case 3:
				put_char('(');
				byte_count += 2;
				loc += 2;
				put_addr(z80, loc);
				put_char(')');
				break;
			}

			break;

		case 0x0B:
			put_str(OPC_DEC);
			put_dd_reg(bits(5,4));
			break;
		}

		break;

	case 1:
		/* if ((bits(5,3) != 6) && (bits(2,0) != 6)) */
		if (z80->mem[loc] == 0x76)
		{
			put_str("HALT");
		}
		else
		{
			put_str(OPC_LD);
			put_reg(z80, bits(5,3), loc);
			put_char(',');
			put_reg(z80, bits(2,0), loc);
		}

		break;

	case 2:
		put_str(op_names[bits(5,3)]);
		put_reg(z80, bits(2,0), loc);
		break;

	case 3:
		if (z80->mem[loc] == 0xCB)
		{
			loc++;
			byte_count++;

			if (bits(7,6) == 0)
			{
				put_str(shf_op_names[bits(5,3)]);
			}
			else
			{
				put_str(bit_op_names[bits(7,6)]);
				put_digit(bits(5,3));
				put_char(',');
			}

			put_reg(z80, bits(2,0), loc);
			break;
		}

		/*if ((z80->mem[loc] & 0x0DD) == 0x0DD)*/
		if (z80->mem[loc] == 0xDD)
		{
			if (bits(5,5))
				index_reg = "IY";
			else
				index_reg = "IX";

			byte_count = disassem(z80, loc + 1, NULL);
			loc += byte_count;

			break;
		}

		if (z80->mem[loc] == 0xED)
		{
			loc++;
			byte_count++;

			switch (bits(7,6))
			{
			case 0:
				put_str(OPC_ILLEGAL);
				break;

			case 1:
				switch (bits(3,0))
				{
				case 0:
				case 8:
					put_str("IN    ");
					put_reg(z80, bits(5,3), loc);
					put_str(",[C]");
					break;

				case 1:
				case 9:
					put_str("OUT   [C],");
					put_reg(z80, bits(5,3), loc);
					break;

				case 2:
					put_str("SBC   HL,");
					put_dd_reg(bits(5,4));
					break;

				case 0xA:
					put_str("ADC   HL,");
					put_dd_reg(bits(5,4));
					break;

				case 3:
					put_str(OPC_LD);
					put_char('(');
					put_addr(z80, loc + 2);
					put_str("),");
					put_dd_reg(bits(5,4));
					byte_count += 2;
					loc += 2;
					break;

				case 0xB:
					put_str(OPC_LD);
					put_dd_reg(bits(5,4));
					put_str(",(");
					byte_count += 2;
					loc += 2;
					put_addr(z80, loc);
					put_char(')');
					break;

				case 4:
					if (bits(5,4) == 0)
						put_str("NEG");
					else
						put_str(OPC_ILLEGAL);

					break;

				case 0xC:
					put_str(OPC_ILLEGAL);
					break;

				case 5:
					if (bits(5,4) == 0)
						put_str("RETN");
					else
						put_str(OPC_ILLEGAL);

					break;

				case 0xD:
					if (bits(5,4) == 0)
						put_str("RETI");
					else
						put_str(OPC_ILLEGAL);

					break;

				case 6:
					if (bits(5,4) == 0)
						put_str("IM    0");
					else if (bits(5,4) == 1)
						put_str("IM    1");
					else
						put_str(OPC_ILLEGAL);

					break;

				case 0xE:
					if (bits(5,4) == 1)
						put_str("IM    2");
					else
						put_str(OPC_ILLEGAL);
					break;

				case 7:
					if (bits(5,4) == 0)
						put_str("LD    I,A");
					else if (bits(5,4) == 1)
						put_str("LD    A,I");
					else if (bits(5,4) == 2)
						put_str("RRD");
					else
						put_str(OPC_ILLEGAL);

					break;

				case 0xF:
					if (bits(5,4) == 0)
						put_str("LD    R,A");
					else if (bits(5,4) == 1)
						put_str("LD    A,R");
					else if (bits(5,4) == 2)
						put_str("RLD");
					else
						put_str(OPC_ILLEGAL);

					break;
				}

			    break;

			case 2:
			    if (bits(5,5) == 0)
			    {
				    put_str(OPC_ILLEGAL);
				    break;
			    }

			    if (bits(2,2) == 1)
			    {
				    put_str(OPC_ILLEGAL);
				    break;
			    }

			    put_str(rep_op_names[
				    ((z80->mem[loc] >> 1) & 0xC)
				    | (z80->mem[loc] & 3)
				    ]);
			    break;

			case 3:
			    put_str(OPC_ILLEGAL);
			    break;
			}

			break;
		}

		if ((z80->mem[loc] & 0x06) == 0x06)
		{
			put_str(op_names[bits(5,3)]);
			loc++;
			byte_count++;
			put_byte(z80->mem[loc]);
			break;
		}

		switch (bits(2,0))
		{
		case 0:
			put_str(OPC_RET);
			put_cc(bits(5,3));
			break;

		case 1:
			switch (bits(5,3))
			{
			case 0:
			case 2:
			case 4:
			case 6:
				put_str(OPC_POP);
				put_qq_reg(bits(5,4));
				break;

			case 1:
				put_str(OPC_RET);
				break;

			case 3:
				put_str(OPC_EXX);
				break;

			case 5:
				put_str(OPC_JP);

				if (index_reg)
				{
					put_str("(");
					put_str( index_reg);
					put_str(")");
				}
				else
					put_str("(HL)");

				break;

			case 7:
				put_str(OPC_LD);
				put_str("SP,");

				if (index_reg)
					put_str(index_reg);
				else
					put_str("HL");

				break;
			}

			break;

		case 2:
			put_str(OPC_JP);
			put_cc(bits(5,3));
			put_char(',');
			byte_count += 2;
			loc += 2;
			put_addr(z80, loc);
			break;

		case 3:
			switch (bits(5,3))
			{
			case 0:
				put_str(OPC_JP);
				byte_count += 2;
				loc += 2;
				put_addr(z80, loc);
				break;

			case 2:
				loc++;
				byte_count++;
				put_str(OPC_OUT);
				put_char('(');
				put_byte(z80->mem[loc]);
				put_str("),A");
				break;

			case 3:
				loc++;
				byte_count++;
				put_str(OPC_IN);
				put_str("A,(");
				put_byte(z80->mem[loc]);
				put_char(')');
				break;

			case 4:
				put_str(OPC_EX);
				put_str("(SP),");

				if (index_reg)
					put_str(index_reg);
				else
					put_str("HL");

				break;

			case 5:
				put_str(OPC_EX);
				put_str("HL,DE");
				break;

			case 6:
				put_str(OPC_DI);
				break;

			case 7:
				put_str(OPC_EI);
				break;
			}

			break;

		case 4:
			put_str(OPC_CALL);
			put_cc(bits(5,3));
			put_char(',');
			byte_count += 2;
			loc += 2;
			put_addr(z80, loc);
			break;

		case 5:
			if (bits(3,3))
			{
				put_str(OPC_CALL);
				byte_count += 2;
				loc += 2;
				put_addr(z80, loc);
			}
			else
			{
				put_str(OPC_PUSH);
				put_qq_reg(bits(5,4));
			}

			break;

		case 6:
			put_str(OPC_ILLEGAL);
			break;

		case 7:
			put_str(OPC_RST);
			put_byte(z80->mem[loc] & 0x038);
			break;
		}

		break;
	}

	loc++;
	byte_count++;
	return byte_count;
}
