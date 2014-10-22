/* Error printing

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  


IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _Ierror
#define _Ierror 1

typedef struct error_printer Error_printer;

struct error_printer {
	int error_flag;		/* Set if there's an error */
	char error_msg[192];	/* Error message */
	void *error_obj;	/* 'this' pointer for error printer */
	void (*error_print)(void *error_obj, char *msg);
				/* Error printing function */
};

#define error_0(p,msg) do { \
	(p)->error_flag = 1; \
	snprintf((p)->error_msg,sizeof((p)->error_msg),(msg)); \
	if ((p)->error_print) (p)->error_print((p)->error_obj,(p)->error_msg); \
} while(0)

#define error_1(p,msg,a) do { \
	(p)->error_flag = 1; \
	snprintf((p)->error_msg,sizeof((p)->error_msg),(msg),(a)); \
	if ((p)->error_print) (p)->error_print((p)->error_obj,(p)->error_msg); \
} while(0)

#define error_2(p,msg,a,b) do { \
	(p)->error_flag = 1; \
	snprintf((p)->error_msg,sizeof((p)->error_msg),(msg),(a),(b)); \
	if ((p)->error_print) (p)->error_print((p)->error_obj,(p)->error_msg); \
} while(0)

#define error_3(p,msg,a,b,c) do { \
	(p)->error_flag = 1; \
	snprintf((p)->error_msg,sizeof((p)->error_msg),(msg),(a),(b),(c)); \
	if ((p)->error_print) (p)->error_print((p)->error_obj,(p)->error_msg); \
} while(0)

#define error_4(p,msg,a,b,c,d) do { \
	(p)->error_flag = 1; \
	snprintf((p)->error_msg,sizeof((p)->error_msg),(msg),(a),(b),(c),(d)); \
	if ((p)->error_print) (p)->error_print((p)->error_obj,(p)->error_msg); \
} while(0)

#endif
