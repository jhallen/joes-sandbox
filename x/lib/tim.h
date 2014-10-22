/* Time string functions */

#ifndef _Itim
#define _Itim 1

#include "config.h"

char *dntos1(char *,long n);	/* secs -> Sat Feb 14       */

char *dntos2(char *,long n);	/* secs -> 05/07/81         */

char *dntos3(char *,long n);	/* secs -> Jan 14, 1980     */

char *dntos4(char *,long n);	/* secs -> Sat Jan 14, 1980 */

long dmyton(int day,
            int month,
            int year);	/* Day,month,year -> secs */

long dston(char *s);	/* Convert string to seconds or -1 if error */

long ston(char *s);		/* Same as atoi() */

char *ntos(char *,long n);

#endif
