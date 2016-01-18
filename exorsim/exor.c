/*	EXORcister simulator
 *	Copyright
 *		(C) 2011 Joseph H. Allen
 *
 * This is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 1, or (at your option) any later version.  
 *
 * It is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.  
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this software; see the file COPYING.  If not, write to the Free Software Foundation, 
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Exorciser / swtpc emulator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <signal.h>

#include "sim6800.h"
#include "exor.h"
#include "exorterm.h"

/* Options */

int swtpc = 0;
char *exbug_name; /*  = "exbug.bin"; */
int trace_disk = 0; /* Enable disk trace */
int lower = 0; /* Allow lower case */

/* Diskettes */

struct drive_info {
        char *name;
        FILE *f;
        int bytes; /* Bytes per sector */
        int tracks; /* Tracks per disk */
        int sects; /* Sectors per track */
} drive[4] =
{
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
};

/* Memory */
unsigned char mem[65536];

int pending_read_ahead = 1;
unsigned char read_ahead_c;

int count = 10;

int polling = 1; /* Allow ACIA polling */

int saved;

void getsect(int n, int addr, int sect, int len)
{
        if (trace_disk) printf("Read sector %d into %x, size=%d\n", sect, addr, len);
        if (drive[n].f) {
                fseek(drive[n].f, sect * drive[n].bytes, SEEK_SET);
                fread(mem + addr, len, 1, drive[n].f);
        } else {
                printf("Tried to read from non-existent disk %d\n", n);
                stop = 1;
        }
}

void getsect1(int n, unsigned char *addr, int ofst, int len)
{
        if (trace_disk) printf("Read sector %d\n", ofst / 256);
        fseek(drive[n].f, ofst, SEEK_SET);
        fread(addr, len, 1, drive[n].f);
}

void putsect(int n, int addr, int sect, int len)
{
        if (trace_disk) printf("Write sector %d into %x, size=%d\n", sect, addr, len);
        if (drive[n].f) {
                fseek(drive[n].f, sect * drive[n].bytes, SEEK_SET);
                fwrite(mem + addr, len, 1, drive[n].f);
                fflush(drive[n].f);
        } else {
                printf("Tried to write to non-existent disk %d\n", n);
                stop = 1;
        }
}

void putsect1(int n, unsigned char *addr, int ofst, int len)
{
        if (trace_disk) printf("Write sector %d\n", ofst / 256);
        fseek(drive[n].f, ofst, SEEK_SET);
        fwrite(addr, len, 1, drive[n].f);
        fflush(drive[n].f);
}

/* FD1771 emulation.. */

int cur_drive = 0;
int cur_sect = 1;
int cur_track = 0;
unsigned char cur_status = 0;
unsigned char cur_buf[256];
int cur_count;
unsigned char cur_data;
int cur_state; /* 0 = IDLE, 1 = read single, 2 = read multiple, 3 = write single */
int cur_dir = 1;

int count1;

/* All memory reads go through this function */

unsigned char mread(unsigned short addr)
{
        unsigned char c;
        int rtn;

        if (swtpc) {
                switch (addr) {
#if 0
                        case 0x8004: {
                                if (count--)
                                        return 0x00;
                                else {
                                        count = 10;
                                        return 0x03;
                                }
#endif
                        case 0x8004: { /* Check serial port status */
                                if (polling) {

                                        int flags;

                                        if (pending_read_ahead)
                                                return 0x03;

                                        flags = fcntl(fileno(stdin), F_GETFL);
                                        if (flags == -1) {
                                                printf("fcntl error\n");
                                                exit(-1);
                                        }
                                        fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);

                                        rtn = read(fileno(stdin), &read_ahead_c, 1);

                                        fcntl(fileno(stdin), F_SETFL, flags);

                                        if (rtn == 1) {
                                                count = 0;
                                                pending_read_ahead = 1;
                                                return 0x03;
                                        } else {
                                                skip:
                                                if (count == 1000)
                                                        poll(NULL, 0, 1); /* Don't hog CPU time */
                                                else
                                                        ++count;
                                                return 0x02;
                                        }
                                } else {
                                        /* No polling: return false then true */
                                        if (count--)
                                                return 0x00;
                                        else {
                                                count = 10;
                                                return 0x03;
                                        }
                                }
                        } case 0x8005: { /* Read from serial port */
                                if (polling) {
                                        c = read_ahead_c;
                                        pending_read_ahead = 0;
                                } else {
                                        int rtn = 0;
                                        int flags = fcntl(fileno(stdin), F_GETFL);
                                        c = '?';
                                        if (flags == -1) {
                                                printf("fcntl error\n");
                                                exit(-1);
                                        }
                                        while (rtn < 1 && !stop) {
                                                fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);
                                                rtn = read(fileno(stdin), &c, 1);
                                                fcntl(fileno(stdin), F_SETFL, flags);
                                                if (rtn < 1 && !stop) {
                                                        poll(NULL, 0, 8); /* Don't hog CPU time */
                                                }
                                        }
                                }
                                if (!lower && c >= 'a' && c <= 'z')
                                        c += 'A' - 'a';
                                if (swtpc) {
                                        if (c == 127)
                                                c = 8;
                                } else {
                                        if (c == 8)
                                                c = 127;
                                }
                                saved = c;
                                return c;
                        } case 0x8007: { /* Alias, used to test if this is really an ACIA */
                                return saved;
                        } case 0x8018: { /* status */
                                if (cur_state == 1) {
                                        if (count1) {
                                                if (!--count1) {
                                                        /* Not readying?  Give lost data. */
                                                        /* The code sometimes reads without taking data to verify a previous
                                                           write (by checking for CRC error). */
                                                        cur_state = 0;
                                                        cur_status = 0x04;
                                                }
                                        }
                                }
                                return cur_status;
                        } case 0x8019: { /* track */
                                return cur_track;
                        } case 0x801a: { /* sector */
                                return cur_sect;
                        } case 0x801b: { /* data */
                                /* printf("Read %d\n", cur_state); */
                                if (cur_state == 1) {
                                        count1 = 100;
                                        c = cur_buf[cur_count++];
                                        if (cur_count == drive[cur_drive].bytes) {
                                                /* printf("Sector %d done\n", cur_sect); */
                                                cur_state = 0;
                                                cur_status = 0;
                                        }
                                        return c;
                                } else if (cur_state == 2) {
                                        count1 = 100;
                                        c = cur_buf[cur_count++];
                                        if (cur_count == drive[cur_drive].bytes) {
                                                if (cur_sect == drive[cur_drive].sects) {
                                                        /* printf("Sector %d done, track done.\n", cur_sect); */
                                                        cur_status = 0; /* All done! */
                                                        cur_state = 0;
                                                } else {
                                                        /* printf("Sector %d done\n", cur_sect); */
                                                        ++cur_sect;
                                                        cur_count = 0;
                                                        getsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                                }
                                        }
                                        return c;
                                } else {
                                        return 0;
                                }
                        } default: {
                                return mem[addr];
                        }
                }
        } else {
                switch (addr) {
                        case 0xFCF9: {
                                return 0x80;
                        } case 0xFCF8: {
                                return 0xF;
                        } case 0xFCF4: {
                                if (count--)
                                        return 0x00;
                                else {
                                        count = 10;
                                        return 0x03;
                                }
#if 0
                        } case 0xFCF4: { /* Check serial port status */
                                if (polling) {

                                        int flags;

                                        if (pending_read_ahead)
                                                return 0x03;

                                        flags = fcntl(fileno(stdin), F_GETFL);
                                        if (flags == -1) {
                                                printf("fcntl error\n");
                                                exit(-1);
                                        }
                                        fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);

                                        rtn = read(fileno(stdin), &read_ahead_c, 1);

                                        fcntl(fileno(stdin), F_SETFL, flags);

                                        if (rtn == 1) {
                                                count = 0;
                                                pending_read_ahead = 1;
                                                return 0x03;
                                        } else {
                                                skip:
                                                if (count == 1000)
                                                        poll(NULL, 0, 1); /* Don't hog CPU time */
                                                else
                                                        ++count;
                                                return 0x02;
                                        }
                                } else {
                                        /* No polling: return false then true */
                                        if (count--)
                                                return 0x00;
                                        else {
                                                count = 10;
                                                return 0x03;
                                        }
                                }
                        } case 0xFCF5: { /* Read from serial port */
                                if (polling) {
                                        c = read_ahead_c;
                                        pending_read_ahead = 0;
                                } else {
                                        int rtn = 0;
                                        int flags = fcntl(fileno(stdin), F_GETFL);
                                        c = '?';
                                        if (flags == -1) {
                                                printf("fcntl error\n");
                                                exit(-1);
                                        }
                                        while (rtn < 1 && !stop) {
                                                fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);
                                                rtn = read(fileno(stdin), &c, 1);
                                                fcntl(fileno(stdin), F_SETFL, flags);
                                                if (rtn < 1 && !stop) {
                                                        poll(NULL, 0, 8); /* Don't hog CPU time */
                                                }
                                        }
                                }
                                if (!lower && c >= 'a' && c <= 'z')
                                        c += 'A' - 'a';
                                if (swtpc) {
                                        if (c == 127)
                                                c = 8;
                                } else {
                                        if (c == 8)
                                                c = 127;
                                }
                                return c;
#endif
                        } default: {
                                return mem[addr];
                        }
                }
        }
}

/* All memory writes go through this function */

void mwrite(unsigned short addr, unsigned char data)
{
        if (swtpc) {
                /* Do not write to ROM */
                if (addr >= 0xe000 && addr < 0xe400)
                        return;
                mem[addr] = data;
                switch (addr) {
                        case 0x8018: { /* Command */
                                switch (data & 0xF0) {
                                        case 0x00: { /* Restore */
                                                /* printf("FD1771 restore!\n"); */
                                                cur_track = 0;
                                                cur_sect = 1;
                                                cur_dir = 1;
                                                if (drive[cur_drive].f)
                                                        cur_status = 0x00;
                                                else
                                                        cur_status = 0xD0;
                                                break;
                                        } case 0x10: { /* Seek */
                                                /* printf("FD1771 seek to track %d\n", cur_data); */
                                                cur_track = cur_data;
                                                if (drive[cur_drive].f)
                                                        cur_status = 0x00;
                                                else
                                                        cur_status = 0xD0;
                                                break;
                                        } case 0x30: { /* Step */
                                                printf("FD1771 tried to step\n");
                                                if ((int)cur_track + cur_dir != -1)
                                                        cur_track += cur_dir;
                                                cur_state = 0x00;
                                                if (drive[cur_drive].f)
                                                        cur_status = 0x00;
                                                else
                                                        cur_status = 0xD0;
                                                break;
                                        } case 0x50: { /* Step in */
                                                printf("FD1771 tried to step in\n");
                                                ++cur_track;
                                                cur_dir = 1;
                                                if (drive[cur_drive].f)
                                                        cur_status = 0x00;
                                                else
                                                        cur_status = 0xD0;
                                                break;
                                        } case 0x70: { /* Step out */
                                                printf("FD1771 tried to step out\n");
                                                if (cur_track)
                                                        --cur_track;
                                                cur_dir = -1;
                                                if (drive[cur_drive].f)
                                                        cur_status = 0x00;
                                                else
                                                        cur_status = 0xD0;
                                                break;
                                        } case 0x80: { /* Read single */
                                                /* printf("FD1771 read single\n"); */
                                                if (drive[cur_drive].f) {
                                                        getsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                                        cur_state = 1;
                                                        cur_count = 0;
                                                        cur_status = 0x03; /* DRQ + BUSY */
                                                        count1 = 100;
                                                } else {
                                                        cur_status = 0x90;
                                                }
                                                break;
                                        } case 0x90: { /* Read multiple */
                                                /* printf("FD1771 read multiple\n"); */
                                                if (drive[cur_drive].f) {
                                                        getsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                                        cur_state = 2;
                                                        cur_count = 0;
                                                        cur_status = 0x03; /* DRQ + BUSY */
                                                } else {
                                                        cur_status = 0x90;
                                                }
                                                break;
                                        } case 0xA0: { /* Write single */
                                                if (drive[cur_drive].f) {
                                                        cur_state = 3;
                                                        cur_count = 0;
                                                        cur_status = 0x03; /* DQA + BUSY */
                                                } else {
                                                        cur_status = 0x90;
                                                }
                                                /* printf("FD1771 write single\n"); */
                                                break;
                                        } case 0xB0: { /* Write multiple */
                                                printf("FD1771 tried to write multiple\n");
                                                break;
                                        } case 0xC0: { /* Read track */
                                                printf("FD1771 tried to read track\n");
                                                break;
                                        } case 0xF0: { /* Write track */
                                                printf("FD1771 tried to write track\n");
                                                break;
                                        } case 0xD0: { /* Force interrupt */
                                                break;
                                        } default: {
                                                printf("Unknown FD1771 command %x\n", data);
                                                exit(-1);
                                        }
                                }
                                break;
                        } case 0x8014: { /* Set drive */
                                cur_drive = (data & 3);
                                /* printf("Set drive to %x\n", data); */
                                break;
                        } case 0x8019: { /* Track */
                                /* printf("Set track = %d\n", data); */
                                cur_track = data;
                                break;
                        } case 0x801a: { /* Sector */
                                /* printf("Set sector = %d\n", data); */
                                if (!data)
                                        data = 1;
                                cur_sect = data;
                                break;
                        } case 0x801b: { /* Data */
                                cur_data = data;
                                if (cur_state == 3) {
                                        /* printf("Write data %d\n", cur_count); */
                                        cur_buf[cur_count++] = data;
                                        if (cur_count == drive[cur_drive].bytes) {
                                                printf("Write done.\n");
                                                cur_state = 0;
                                                cur_status = 0;
                                                putsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                        }
                                }
                                break;
                        } case 0x8005: { /* Write to serial port */
                                putchar(data); fflush(stdout);
                                break;
                        }
                }
        } else {
                /* Do not write to ROM */
                if (addr >= 0xE800 && addr < 0xEC00 ||
                    addr >= 0xF000 && addr < 0xFC00 ||
                    addr >= 0xFCFC && addr < 0xFD00)
                        return;
                mem[addr] = data;
                switch (addr) {
                        case 0xFCF5: { /* Write to serial port */
                                term_out(data);
                                /* putchar(data); fflush(stdout); */
                        }
                }
        }
}

unsigned short pull2();

/* Addresses of floppy parameters */

#define CURDRV 0	/* Current drive: 0 -3 */
#define STRSCT 1	/* Starting sector (2 bytes) */
#define NUMSCT 3	/* Number of sectors (2 bytes) */
#define LSCTLN 5	/* Length of last sector (1 byte) */
#define CURADR 6	/* Transfer address (2 bytes) */
#define FDSTAT 8	/* Error status: 0x30 means no error */

#define SCTCNT 0x0B	/* Sector count (2 byts): (STRSCT + NUMSCT - SCTCNT - 1) is bad sector number */
#define SIDES 0x0D	/* bit 7 = 1 means single-sided, bit 7 = 0 means double-sided */

/* MDOS disk error codes for FDSTAT */

#define ER_NON '0'	/* No error */
#define ER_CRC '1'	/* Data CRC error */
#define ER_WRT '2'	/* Write protected disk */
#define ER_RDY '3'	/* Disk not ready */
#define ER_MRK '4'	/* Deleted data mark encountered */
#define ER_TIM '5'	/* Timeout */
#define ER_DAD '6'	/* Invalid disk address */
#define ER_SEK '7'	/* Seek error */
#define ER_DMA '8'	/* Data address mark error */
#define ER_ACR '9'	/* Address mark CRC error */

/* Check drive number */

int check_drive(int n)
{
        if (n >= 4) {
                // printf("\r\nFloppy error: attempt to access drive number %d >= 4\n", n);
                mem[FDSTAT] = ER_RDY;
                c_flag = 1;
                return -1;
        }
        if (!drive[n].f) {
                // printf("\r\nFloppy error: attempt to access non-existent disk %d\n", n);
                mem[FDSTAT] = ER_RDY;
                c_flag = 1;
                return -1;
        }
        return 0;
}

/* Check sector number */

int check_sect(int n, int sect)
{
        if (sect >= drive[n].sects * drive[n].tracks) {
                printf("\r\nFloppy error: attempt to access past end of disk %d, sector %d\n", n, sect);
                mem[FDSTAT] = ER_DAD;
                c_flag = 1;
                return -1;
        }
        return 0;
}

/* All jumps go through this function */

void jump(unsigned short addr)
{
        if (swtpc) {
                pc = addr;
                return;
        } else {
                switch (addr) {
/* Too many programs access ACIA directly */
#if 0
                        case 0xF9CF: case 0xF9DC: /* Output a character */ {
                                term_out(acca);
                                /* putchar(acca); fflush(stdout); */
                                c_flag = 0; /* Carry is error status */
                                break;
                        }
#endif
                        case 0xFA8B: /* Input a character */ {
                                acca = term_in();
                                if (!mem[0xFF53]) { /* Echo */
                                        term_out(acca);
                                        /* putchar(c);
                                        fflush(stdout); */
                                } else {
                                        mem[0xFF53] = 0;
                                }
                                c_flag = 0; /* No error */
                                break;
                        }
                        case 0xE800: /* OSLOAD (no modified parms) */ {
                                printf("\nOSLOAD...\n");
                                getsect(0, 0x0020, 23, 128);
                                getsect(0, 0x0020 + 0x0080, 24, 128);
                                pc = 0x0020;
                                sp = 0x00FF;
                                return;
                        } case 0xE822: /* FDINIT (no modified parms) */ {
                                c_flag = 0;
                                break;
                        } 
#if 0
                          case 0xF853: /* CHKERR */ {
                                break;
                        } case 0xE85A: /* PRNTER */ {
                                break;
                        } 
#endif
                          case 0xE869: /* READSC (read full sectors) */ {
                                mem[LSCTLN] = 128;
                        } case 0xE86D: /* READPS (read partial sectors) (FDSTAT, carry, sides) */ {
                                int x;
                                int n = mem[CURDRV];
                                int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
                                int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
                                int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
                                int last = mem[LSCTLN];
                                if (trace_disk) printf("Read sectors: drive=%d, first=%d, number=%d, addr=%x, size of last=%d\n", n, first, num,
                                       addr, last);
                                if (check_drive(n))
                                        break;
                                for (x = 0; x != num; ++x) {
                                        if (check_sect(n, first + x))
                                                goto oops;
                                        getsect(n, addr + 128 * x, first + x, ((x + 1 == num) ? mem[LSCTLN] : 128));
                                }
                                mem[FDSTAT] = ER_NON;
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                c_flag = 0;
                                oops: break;
                        } case 0xE86F: /* RDCRC */ {
                                if (trace_disk) printf("RDCRC\n");
                                int x;
                                int n = mem[CURDRV];
                                int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
                                int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
                                int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
                                int last = mem[LSCTLN];
                                if (trace_disk) printf("RDCRC: drive=%d, first=%d, number=%d, addr=%x, size of last=%d\n", n, first, num,
                                       addr, last);
                                if (check_drive(n))
                                        break;
                                for (x = 0; x != num; ++x) {
                                        if (check_sect(n, first + x))
                                                goto oops;
                                }
                                mem[FDSTAT] = ER_NON;
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                c_flag = 0;
                                break;
                        } case 0xE875: /* RESTOR */ {
                                int n = mem[CURDRV];
                                if (trace_disk) printf("RESTOR\n");
                                if (check_drive(n))
                                        break;
                                mem[FDSTAT] = ER_NON;
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                c_flag = 0;
                                break;
                        } case 0xE878: /* SEEK */ {
                                int n = mem[CURDRV];
                                int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
                                if (trace_disk) printf("SEEK\n");
                                if (check_drive(n))
                                        break;
                                if (check_sect(n, first))
                                        break;
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                c_flag = 0;
                                break;
                        } case 0xE872: /* RWTEST */ {
                                if (trace_disk) printf("RWTEST\n");
                        } case 0xE87B: /* WRTEST */ {
                                unsigned char buf[128];
                                int x;
                                int n = mem[CURDRV];
                                int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
                                int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
                                int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
                                int last = mem[LSCTLN];
                                if (trace_disk) printf("WRTEST\n");
                                if (check_drive(n))
                                        break;
                                for (x = 0; x != 128; x += 2) {
                                        buf[x] = mem[addr];
                                        buf[x + 1] = mem[addr + 1];
                                }
                                for(x=0; x != num; ++x) {
                                        if (check_sect(n, first + x))
                                                goto oops;
                                        if (trace_disk) printf("Wrtest sector %d drive %d\n", first + x, n);
                                        fseek(drive[n].f, (first + x) * 128, SEEK_SET);
                                        fwrite(buf, 128, 1, drive[n].f);
                                        fflush(drive[n].f);
                                }
                                c_flag = 0;
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                mem[FDSTAT] = ER_NON;
                                break;
                        } case 0xE87E: /* WRDDAM */ {
                                int n = mem[CURDRV];
                                printf("\r\nFloppy error: we do not support WRDDAM\n");
                                c_flag = 1;
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                mem[FDSTAT] = ER_WRT;
                                break;
                        } case 0xE884: /* WRITSC */ {
                                if (trace_disk) printf("WRITSC\n");
                        } case 0xE881: /* WRVERF */ {
                                int x;
                                int n = mem[CURDRV];
                                int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
                                int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
                                int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
                                int last = mem[LSCTLN];
                                if (trace_disk) printf("WRVERF: drive=%d, first=%d, number=%d, addr=%x, size of last=%d\n", n, first, num,
                                       addr, last);
                                if (check_drive(n))
                                        break;
                                for(x=0; x != num; ++x) {
                                        if (check_sect(n, first + x))
                                                goto oops;
                                        putsect(n, addr + 128 * x, first + x, 128);
                                }
                                if (drive[n].tracks == 77)
                                        mem[SIDES] = 0x80;
                                else
                                        mem[SIDES] = 0;
                                mem[FDSTAT] = ER_NON;
                                c_flag = 0;
                                break;
                        } case 0xE887: /* CLOCK */ {
                                printf("Floppy: Someone called CLOCK?\n");
                                c_flag = 0;
                                break;
                        } case 0xEBC0: /* LPINIT */ {
                                if (trace_disk) printf("LPINIT\n");
                                c_flag = 0;
                                break;
                        } case 0xEBCC: /* LIST */ {
                                if (trace_disk) printf("LIST\n");
                                term_out(acca);
                                /* putchar(acca); fflush(stdout); */
                                c_flag = 0;
                                break;
                        } case 0xEBE4: /* LDATA */ {
                                if (trace_disk)printf("LDATA\n");
                                while (mem[ix] != 4) {
                                        term_out(mem[ix]);
                                        /* putchar(mem[ix]); */
                                        ++ix;
                                }
                                term_out('\r');
                                term_out('\n');
                                /* printf("\n"); */
                                c_flag = 0;
                                break;
                        } case 0xEBF2: /* LDATA1 */ {
                                if (trace_disk) printf("LDATA1\n");
                                while (mem[ix] != 4) {
                                        /* putchar(mem[ix]); */
                                        term_out(mem[ix]);
                                        ++ix;
                                }
                                /* fflush(stdout); */
                                c_flag = 0;
                                break;
                        } default: {
                                pc = addr;
                                return;
                        }
                }
                simulated(addr);
                addr = pull2();
                jump(addr);
        }
}

int load_exbug()
{
        FILE *f = fopen(exbug_name, "r");
        if (!f) {
                fprintf(stderr, "Couldn't load '%s'\n", exbug_name);
                return -1;
        }
        if (1 != fread(mem, 64 * 1024, 1, f)) {
                fprintf(stderr, "Couldn't read '%s'\n", exbug_name);
                return -1;
        }
        printf("'%s' loaded.\n", exbug_name);
        fclose(f);
        return 0;
}

int load_drive(int n)
{
        FILE *f;
        long t;
        f = fopen(drive[n].name, "r+");
        if (!f) {
                fprintf(stderr, "Couldn't open '%s'\n", drive[n].name);
                return -1;
        }
        fseek(f, 0, SEEK_END);
        t = ftell(f);
        if (swtpc) {
                if (t == 256 * 10 * 35) {
                        drive[n].bytes = 256;
                        drive[n].sects = 10;
                        drive[n].tracks = 35;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[cur_drive].tracks, drive[cur_drive].sects);
                } else if (t == 256 * 10 * 40) {
                        drive[n].bytes = 256;
                        drive[n].sects = 10;
                        drive[n].tracks = 40;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 20 * 35) {
                        drive[n].bytes = 256;
                        drive[n].sects = 20;
                        drive[n].tracks = 35;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 20 * 40) {
                        drive[n].bytes = 256;
                        drive[n].sects = 20;
                        drive[n].tracks = 40;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 18 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 18;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 20 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 20;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 36 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 36;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 72 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 72;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else {
                        fclose(f);
                        printf("'%s' is not a valid disk\n", drive[n].name);
                        return -1;
                }
        } else {
                if (t == 128 * 26 * 77) {
                        printf("'%s' opened for drive %d (single sided)\n", drive[n].name, n);
                        drive[n].f = f;
                        drive[n].bytes = 128;
                        drive[n].tracks = 77;
                        drive[n].sects = 26;
                } else if (t == 128 * 26 * 77 * 2) {
                        printf("'%s' opened for drive %d (double sided)\n", drive[n].name, n);
                        drive[n].f = f;
                        drive[n].bytes = 128;
                        drive[n].tracks = 77 * 2;
                        drive[n].sects = 26;
                } else {
                        fclose(f);
                        printf("'%s' is not a valid disk: it's size must be %d or %d\n", drive[n].name, 128*26*77*2, 128*26*77);
                        return -1;
                }
        }
        return 0;
}

void ctrl_c()
{
        printf("Interrupt!\n");
        stop = 1;
}

int main(int argc, char *argv[])
{
        int x;
        int diskn = 0;
        int gotox = 0;
        mon_out = stdout;
        mon_in = stdin;
        char *facts_name = "facts";
        for (x = 1; x != argc; ++x) {
                if (argv[x][0] == '-') {
                        if (!strcmp(argv[x], "--facts") && x + 1 != argc) {
                                ++x;
                                facts_name = argv[x];
                        } else if (!strcmp(argv[x], "--trace")) {
                                trace = 1;
                        } else if (!strcmp(argv[x], "--swtpc")) {
                                swtpc = 1;
                        } else if (!strcmp(argv[x], "--dtrace")) {
                                trace_disk = 1;
                        } else if (!strcmp(argv[x], "--mon")) {
                                stop = 1;
                        } else if (!strcmp(argv[x], "--skip") && x + 1 != argc) {
                                ++x;
                                skip = atoi(argv[x]);
                        } else if (!strcmp(argv[x], "--exbug") && x + 1 != argc) {
                                exbug_name = argv[++x];
                        } else if (!strcmp(argv[x], "-x")) {
                                gotox = 1;
                        } else if (!strcmp(argv[x], "--lower")) {
                                lower = 1;
                        } else {
                                printf("EXORciser emulator\n");
                                printf("\n");
                                printf("exor [options] [disk0 [disk1 [disk2 [disk3]]]]\n");
                                printf("\n");
                                printf("  --trace	Produce instruction trace on stderr\n");
                                printf("  --dtrace	Produce disk access trace on stderr\n");
                                printf("  --skip nnn    Skip first nnn insns in trace\n");
                                printf("  --swtpc       Simulate SWTPC instead of EXORciser\n");
                                printf("  --exbug name	Give name for ROM if not 'exbug.bin'\n");
                                printf("  -x            Go into EXBUG/SWTBUG instead of MDOS/FLEX\n");
                                printf("  --facts file  Process facts files for commented disassembly\n");
                                printf("  --lower       Allow lowercase\n");
                                printf("  --mon         Start at monitor prompt\n");
                                printf("\n");
                                printf("Default disk0 is mdos.dsk/flex.dsk\n");
                                printf("\n");
                                printf("Hints:\n");
                                printf("  To load MDOS from EXBUG, type MAID followed by E800;G\n");
                                printf("\n");
                                printf("  To load FLEX2 from SWTBUG, type D\n");
                                printf("\n");
                                exit(-1);
                        }
                } else {
                        if (diskn == 4) {
                                printf("Only up to four disks allowed\n");
                                return -1;
                        } else {
                                drive[diskn++].name = argv[x];
                        }
                }
        }

        /* Default disk image name */
        if (!drive[0].name)
                if (swtpc)
                        drive[0].name = "flex.dsk";
                else
                        drive[0].name = "mdos.dsk";

        /* Default memory image name */
        if (!exbug_name) {
                if (swtpc) {
                        exbug_name = "swtbug.bin";
                } else {
                        exbug_name = "exbug.bin";
                }
        }

        /* Load facts file */
        if (facts_name) {
                FILE *f;
                printf("Load facts file '%s'\n", facts_name);
                f = fopen(facts_name, "r");
                if (f) {
                        parse_facts(f);
                        fclose(f);
                } else {
                        printf("Couldn't load '%s'\n", facts_name);
                }
        }

        /* Load initial memory image */
        if (load_exbug()) {
                /* Start halted if there is no ROM */
                stop = 1;
        }

        /* Mount drives */
        for (x = 0; x != 4; ++x) {
                if (drive[x].name)
                        load_drive(x);
        }

        if (!drive[0].f) {
                /* Do not boot DOS if there is no disk */
                gotox = 1;
        }

        /* Read starting address from reset vector */
        pc = ((mem[0xFFFE] << 8) + mem[0xFFFF]);

        /* ...but jump right to OS load unless gotox is set */
        if (!gotox) {
                if (swtpc) {
                        /* Jump right into flex */
                        sp = 0xA042;
                        pc = 0xE28F;
                } else {
                        /* Jump right into MDOS */
                        sp = 0xFF8A;
                        pc = 0xE800;
                }
        }

        /* system("stty cbreak -echo -icrnl"); */
        save_termios();
        sim_termios();

        signal(SIGINT, ctrl_c);

        printf("\nHit Ctrl-C for simulator command line.  Starting simulation...\n");
        izexorterm();

        sim();
        // echo test of terminal emulator
        // while (!stop) term_out(term_in());

        /* system("stty cooked echo icrnl"); */
        restore_termios();
        return 0;
}
