// N pass calculator

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>


#define US (unsigned char *)

int fail;

unsigned char *merr;

struct var {
	unsigned char *name;
	int set;
	double val;
	struct var *next;
} *vars = NULL;

static struct var *get(unsigned char *str)
{
	struct var *v;

	for (v = vars; v; v = v->next) {
		if (!strcmp((char *)v->name, (char *)str)) {
			return v;
		}
	}
	v = (struct var *) malloc(sizeof(struct var));

	v->set = 0;
	v->next = vars;
	vars = v;
	v->name = (unsigned char *)strdup((char *)str);
	return v;
}

unsigned char *ptr;
struct var *dumb;

static double expr(int prec, struct var **rtv)
{
	double x = 0.0;
	struct var *v = NULL;

	while (*ptr == ' ' || *ptr == '\t') {
		++ptr;
	}
	if ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z')
	    || *ptr == '_') {
		unsigned char *s = ptr, c;

		while ((*ptr >= 'a' && *ptr <= 'z')
		       || (*ptr >= 'A' && *ptr <= 'Z')
		       || *ptr == '_' || (*ptr >= '0' && *ptr <= '9')) {
			++ptr;
		}
		c = *ptr;
		*ptr = 0;
		v = get(s);
		if(!v->set)
		  {
		  merr=US "Unknown variable";
		  }
		x = v->val;
		*ptr = c;
	} else if ((*ptr >= '0' && *ptr <= '9') || *ptr == '.') {
		sscanf((char *)ptr, "%lf", &x);
		while ((*ptr >= '0' && *ptr <= '9') || *ptr == '.' || *ptr == 'e' || *ptr == 'E')
			++ptr;
	} else if (*ptr == '(') {
		++ptr;
		x = expr(0, &v);
		if (*ptr == ')')
			++ptr;
		else {
			if (!merr)
				merr = US "Missing )";
		}
	} else if (*ptr == '-') {
		++ptr;
		x = -expr(10, &dumb);
	}
      loop:
	while (*ptr == ' ' || *ptr == '\t')
		++ptr;
	if (*ptr == '*' && 5 > prec) {
		++ptr;
		x *= expr(5, &dumb);
		goto loop;
	} else if (*ptr == '/' && 5 > prec) {
		++ptr;
		x /= expr(5, &dumb);
		goto loop;
	} else if (*ptr == '+' && 4 > prec) {
		++ptr;
		x += expr(4, &dumb);
		goto loop;
	} else if (*ptr == '-' && 4 > prec) {
		++ptr;
		x -= expr(4, &dumb);
		goto loop;
	} else if (*ptr == '=' && 2 >= prec) {
		++ptr;
		x = expr(2, &dumb);
		if (v) {
			v->val = x;
			v->set = 1;
		} else {
			if (!merr)
				merr = US "Left side of = is not an l-value";
		}
		goto loop;
	}
	*rtv = v;
	return x;
}

double eval(unsigned char *s)
{
	double result;

	ptr = s;
	merr = 0;
      up:
	result = expr(0, &dumb);
	if (!merr) {
		while (*ptr == ' ' || *ptr == '\t') {
			++ptr;
		}
		if (*ptr == ';') {
			++ptr;
			while (*ptr == ' ' || *ptr == '\t') {
				++ptr;
			}
			if (*ptr) {
				goto up;
			}
		} else if (*ptr!='}') {
			merr = US "Extra junk after end of expr";
		}
	}
	return result;
}

void parse(char *s,int flg)
  {
  char buf[50];
  char *t;
  while(*s)
    if(s[0]=='\\' && s[1]=='{')
      {
      double ans;
      /* int isvar=0;
      int x; */
      char *start=s;
      s+=2;
      // Look for variable name
      /*
      t=s;
      if(*t>='a' && *t<='z' || *t>='A' && *t<='Z' || *t=='_')
        {
        x=0;
        while(*t>='a' && *t<='z' || *t>='A' && *t<='Z' || *t=='_' || *t>='0' && *t<='9')
          buf[x++]= *t++;
        if(*t==':')
          {
          isvar=1;
          buf[x]=0;
          s=t+1;
          }
        }
      */
      // Evaluate expression
      if((ans=eval((unsigned char *)s)), !merr)
        {
        // if(isvar) set(buf,ans);
        if(flg)
          {
          sprintf(buf,"%lg",ans);
          t=buf;
          while(*t)
            {
            putchar(*t);
            ++t;
            }
          }
        while(*s && *s!='}')
          ++s;
        if(*s=='}') ++s;
        }
      else
        {
        fail=1;
        s=start;
        if(flg) putchar(*s);
        ++s;
        }
      }
    else
      {
      if(flg) putchar(*s);
      ++s;
      }
  }

void pass(int flg)
  {
  char buf[32768];
  rewind(stdin);
  while(gets(buf))
    {
    parse(buf,flg);
    if(flg)
      putchar('\n');
    }
  }

int main(int argc, char *argv[])
  {
  int count;

  // N passes to set all variables.

  for(count=10;count;count--)
    {
    fail=0;
    pass(0);
    if(!fail) break;
    }

  if(fail)
    {
    fprintf(stderr,"Couldn't evaluate all equations in N passes");
    }

  // Final pass
  pass(1);

  return fail;
  }
