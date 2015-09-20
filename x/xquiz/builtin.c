#include <math.h>
#include "types.h"
#include "box.h"
#include "builtin.h"

SYM *yFACT;
SYM *yNEG;
SYM *yEXP;
SYM *yMUL;
SYM *yDIV;
SYM *yADD;
SYM *ySUB;
SYM *yEQ;

SYM *yI;
SYM *yPI;
SYM *yE;

SYM *ySQRT;
SYM *yABS;
SYM *ySGN;

SYM *ySIN;
SYM *yCOS;
SYM *yTAN;
SYM *yCSC;
SYM *ySEC;
SYM *yCOT;
SYM *yASIN;
SYM *yACOS;
SYM *yATAN;
SYM *yACSC;
SYM *yASEC;
SYM *yACOT;

SYM *yD;
SYM *yDERV;

SYM *yLOG;

NUM *yZERO;
NUM *yONE;
NUM *yTWO;

SYM *ydot;

void builtin()
{
	yZERO = newnum();
	yONE = newnum();
	yONE->n = 1.0;
	yTWO = newnum();
	yTWO->n = 2.0;
	yFACT = add("!");
	yFACT->prec = 22;
	yFACT->type = tPOSTFIX;
	yEXP = add("^");
	yEXP->prec = 20;
	yEXP->ass = 1;
	yEXP->type = tINFIX;
	yNEG = add("~");
	yNEG->prec = 18;
	yNEG->type = tPREFIX;
	yMUL = add("*");
	yMUL->prec = 16;
	yMUL->ass = 0;
	yMUL->type = tINFIX;
	yDIV = add("/");
	yDIV->prec = 16;
	yDIV->ass = 0;
	yDIV->type = tINFIX;
	yADD = add("+");
	yADD->prec = 14;
	yADD->ass = 0;
	yADD->type = tINFIX;
	ySUB = add("-");
	ySUB->prec = 14;
	ySUB->ass = 0;
	ySUB->type = tINFIX;
	yEQ = add("=");
	yEQ->prec = 12;
	yEQ->ass = 0;
	yEQ->type = tINFIX;
	yD = add("D");
	yD->prec = 18;
	yD->type = tPREFIX;
	yDERV = add("'");
	yDERV->prec = 18;
	yDERV->type = tPOSTFIX;
	yI = add("%i");

	yE = add("e");
	yE->bind = newnum();
	yE->bind->n = M_E;
	yPI = add("pi");
	yPI->bind = newnum();
	yPI->bind->n = M_PI;

	ySQRT = add("sqrt");
	yABS = add("abs");
	yABS = add("sgn");
	ySIN = add("sin");
	yCOS = add("cos");
	yTAN = add("tan");
	yCSC = add("csc");
	ySEC = add("sec");
	yCOT = add("cot");
	yASIN = add("asin");
	yACOS = add("acos");
	yATAN = add("atan");
	yACSC = add("acsc");
	yASEC = add("asec");
	yACOT = add("acot");
	yLOG = add("log");
	ydot = add("_");
}
