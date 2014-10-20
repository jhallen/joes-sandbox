#ifndef _Idate
#define _Idate 1

/* Date function which use YYYYMMDD\0 ASCII date format. */

void today(char *date);	/* Get today's date in YYYYMMDD\0 format */

void printdate1(char *buf,const char *date);
			/* Print date: Sat Feb 14 */

void printdate1w(char *buf,const char *date);
			/* Print date: Sat Feb 14 wk3 */

void printweek(char *buf,const char *date);
			/* Print date: 3         (for week no. 3) */

void printdate2(char *buf,const char *date);
			/* Print date: 05/07/81 */

void printdate3(char *buf,const char *date);
			/* Print date: Jan 14, 1980 */

void printdate4(char *buf,const char *date);
			/* Print date: Jan 14, 1980 w17 */

void printdate5(char *buf,const char *date);
			/* Print date: Sat Jan 14, 1980 (week 17) */

int parsedate(char *date,const char *input);
			/* Parse user entered date.  Returns 0 for success,
			   or -1 for error (date[0] will be set to 0 for this
			   case) */

/* week may be entered with: w14.  When this is done, the day of week is monday */

#endif
