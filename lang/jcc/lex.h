// Tokens

enum {
        tEOF = -1, tNUM = -2, tFP = -3, tSTRING = -4, tWORD = -5, tAUTO = -6, tBREAK = -7,
        tCASE = -8, tCHAR = -9, tCONST = -10, tCONTINUE = -11, tDEFAULT = -12, tDO = -13,
        tDOUBLE = -14, tELSE = -15, tENUM = -16, tEXTERN = -17, tFLOAT = -18, tFOR = -19,
        tGOTO = -20, tIF = -21, tINT = -22, tLONG = -23, tREGISTER = -24, tRETURN = -25,
        tSHORT = -26, tSIGNED = -27, tSIZEOF = -28, tSTATIC = -29, tSTRUCT = -30,
        tSWITCH = -31, tTYPEDEF = -32, tUNION = -33, tUNSIGNED = -34, tVOID = -35,
        tVOLATILE = -36, tWHILE = -37,
        pDEFINE = -38, pINCLUDE = -39, pIFDEF = -40, pIFNDEF = -41, pPRAGMA = -42,
        pELIF = -43, pENDIF = -44, pELSE = -45, pIF = -46, pUNDEF = -47, pERROR = -48,
        pLINE = -49, tDEFINED = -50, pINCLUDE_NEXT = -51, pWARNING = -52
};

int get_tok(); // Read next token

// Data associated with most recently read token
extern char *word_buffer;
extern int word_buffer_len;

struct num {
        unsigned long long num;
        int is_unsigned;
        int is_long_long;
};

extern struct num num;
extern double float_val;

void unget_tok(int token); // Unget token
void show_tok(int token); // Print token

// Information about source file

extern int line;
extern char *file_name;
