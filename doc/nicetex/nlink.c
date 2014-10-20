// Link equations

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **s;	// Array of lines.
int nlines;	// Total number of lines

// Get an equation from a line

void get_eqn(int n,int field,int col,char *buf,int *xx)
  {
  char *l=s[n];
  int fld=field;
  // Move to start of specified field
  while(field--)
    {
    while(*l && *l!='|')
      ++l;
    if(*l)
      ++l;
    }
  // Move to first equation or end of line
  while(*l && !(l[0]=='\\' && l[1]=='{'))
    ++l;
  // Move forwards or backwards by equation
  while(col>0)
    {
    if(*l) ++l;
    while(*l && !(l[0]=='\\' && l[1]=='{')) ++l;
    --col;
    }
  while(col<0)
    {
    if(l!=s[n]) --l;
    while(l!=s[n] && !(l[0]=='\\' && l[1]=='{')) --l;
    ++col;
    }
  // Copy equation if we have it
  if(l[0]=='\\' && l[1]=='{')
    {
    int x= *xx;
    l+=2;
    while(*l && *l!='}')
      {
      if(*l=='r' || *l=='l' || *l=='R' || *l=='L' ||
         *l=='u' || *l=='d' || *l=='U' || *l=='D')
        {
        int row=0;
        int field=0;
        int col=0;
        // Build relative reference
      loop:
        if(*l=='r') { ++col; ++l; }
        else if(*l=='R') { ++field; ++l; }
        else if(*l=='l') { --col; ++l; }
        else if(*l=='L') { --field; ++l; }
        else if(*l=='u') { --row; ++l; }
        else if(*l=='U') { --row; ++l; }
        else if(*l=='d') { ++row; ++l; }
        else if(*l=='D') { ++row; ++l; }
        else
          goto done;
        goto loop;
      done:
        buf[x++]='(';
        get_eqn(row+n,fld+field,col,buf,&x);
        buf[x++]=')';
        }
      else if((*l>='a' && *l<='z') ||
         (*l>='A' && *l<='Z') ||
         *l=='_')
        {
        while((*l>='a' && *l<='z') ||
              (*l>='A' && *l<='Z') ||
              (*l>='0' && *l<='9') ||
              *l=='_')
          buf[x++] = *l++;
        }
      else
        buf[x++]= *l++;
      }
    *xx= x;
    }
  else
    { /* Equation not found error */
    fprintf(stderr,"Link error\n");
    }
  }

// Link a line

void link(int n)
  {
  char buf[262144];
  char *l=s[n];
  int fld=0;
  int x=0;
  while(*l)
    if(l[0]=='\\' && l[1]=='{')
      {
      buf[x++]='\\';
      buf[x++]='{';
      l+=2;
      while(*l && *l!='}')
        {
        if(*l=='r' || *l=='l' || *l=='R' || *l=='L' ||
           *l=='u' || *l=='d' || *l=='U' || *l=='D')
          {
          int row=0;
          int field=0;
          int col=0;
          // Build relative reference
        loop:
          if(*l=='r') { ++col; ++l; }
          else if(*l=='R') { ++field; ++l; }
          else if(*l=='l') { --col; ++l; }
          else if(*l=='L') { --field; ++l; }
          else if(*l=='u') { --row; ++l; }
          else if(*l=='U') { --row; ++l; }
          else if(*l=='d') { ++row; ++l; }
          else if(*l=='D') { ++row; ++l; }
          else
            goto done;
          goto loop;
        done:
          buf[x++]='(';
          get_eqn(row+n,field+fld,col,buf,&x);
          buf[x++]=')';
          }
        else if((*l>='a' && *l<='z') ||
           (*l>='A' && *l<='Z') ||
           *l=='_')
          {
          while((*l>='a' && *l<='z') ||
                (*l>='A' && *l<='Z') ||
                (*l>='0' && *l<='9') ||
                *l=='_')
            buf[x++] = *l++;
          }
        else
          buf[x++]= *l++;
        }
      if(*l=='}')
        {
        buf[x++]= *l++;
        }
      }
    else
      {
      if(*l=='|') ++fld;
      buf[x++]= *l++;
      }
  buf[x]=0;
  s[n]=strdup(buf);
  }

int main(int argc, char *argv[])
  {
  char buf[32768];
  int n;

  // Read file: count lines
  for(nlines=0;gets(buf);++nlines);
  rewind(stdin);

  // Allocate array
  s=(char **)malloc(nlines*sizeof(char *));

  // Read lines
  for(n=0;n!=nlines;++n)
    {
    gets(buf);
    s[n]=strdup(buf);
    }

  // Link
  for(n=0;n!=nlines;++n)
    link(n);

  // Emit
  for(n=0;n!=nlines;++n)
    puts(s[n]);

  return 0;
  }
