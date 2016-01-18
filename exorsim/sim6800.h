/* Controls */

extern int skip;
extern int trace;
extern int stop;
extern int reset;
extern int abrt;
extern int sp_stop;

extern int brk;
extern unsigned short brk_addr;

/* CPU registers */

extern unsigned char acca;
extern unsigned char accb;
extern unsigned short ix;
extern unsigned short pc;
extern unsigned short sp;
extern unsigned char c_flag;
extern unsigned char v_flag;
extern unsigned char z_flag;
extern unsigned char n_flag;
extern unsigned char i_flag; /* 1=masked, 0=enabled */
extern unsigned char h_flag;

unsigned char read_flags();
void write_flags(unsigned char f);

/* Simulate */

void sim(void);

/* Dump trace buffer */
void show_traces(int n);

/* Provided externally */

void jump(unsigned short addr);
unsigned char mread(unsigned short addr);
void mwrite(unsigned short addr, unsigned char data);
void monitor(void);
