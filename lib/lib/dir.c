/* Directory package for older UNIXs
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

struct direct
 {
 short d_ino;
 char d_name[14];
 };

void *opendir(name)
char *name;
 {
 return fopen(name,"r");
 }

struct direct *readdir(f)
void *f;
 {
 static struct direct direct;
 while(1==fread(&direct,sizeof(struct direct),1,(FILE *)f))
  if(direct.d_ino) return &direct;
 return 0;
 }

void closedir(f)
FILE *f;
 {
 fclose(f);
 }

int mkdir(s)
char *s;
 {
 char *y=0;
 int rtval;
 y=vsncpy(sv(y),sc("/bin/mkdir "));
 y=vsncpy(sv(y),sz(s));
 y=vsncpy(sv(y),sc(" 2>/dev/null"));
 tickoff();
 rtval=system(y);
 tickon();
 vsrm(y);
 return rtval;
 }
