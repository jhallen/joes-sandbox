/*-----------------------------------------------------------------------*\
 |  Originally by Kevin Kayes, but he refused any responsibility for it. |
 |                                                                       |
 |  Copyright 1986-1988 by Parag Patel.  All Rights Reserved.            |
 |  Copyright 1994 by CodeGen, Inc.  All Rights Reserved.                |
\*-----------------------------------------------------------------------*/


#define SECTORSIZE	128
#define SECTORSPERTRACK	26
#define TRACKSPERDISC	77
#define SECTOROFFSET	1
#define TRACKOFFSET	0
#define RESERVEDTRACKS	2
#define SECTORSPERBLOCK	8
#define SECTORSPEREXTENT 128
#define EXTENTSIZE	32
#define TOTALEXTENTS	64

#define EXTENTSPERSECTOR (SECTORSIZE / EXTENTSIZE)
#define TRACKSIZE	(long)(SECTORSIZE * SECTORSPERTRACK)
#define DISCSIZE	(long)(TRACKSIZE * TRACKSPERDISC)

unsigned char	sectorxlat[] = {
			1, 7, 13, 19,
			25, 5, 11, 17,
			23, 3, 9, 15,
			21, 2, 8, 14,
			20, 26, 6, 12,
			18, 24, 4, 10,
			16, 22
			};

