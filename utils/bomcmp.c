/* Compare two hierarchical CSV BOMs */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 65000
#define MAXFIELDS 100

/* Known colums */

char *col_pn = "P/N"; /* Part number */
char *col_indent_level = "Indent Level";
char *col_qty = "Qty"; /* Quantity */
char *col_title = "Title";
char *col_parent_assy = "Parent Assy";
char *col_line = "Line";
char *col_rev = "Rev";

int fields(char *buf,char **ptrs)
 {
 int n=0;
 char *start;
 int flg = 0;

 loop:
 if(*buf=='"')
   {
   ++buf;
   flg = 1;
   }
 start=buf;
 loop1: while(*buf && *buf!='"' && !(!flg && *buf==',')) ++buf;
 if (buf[0]=='"' && buf[1]=='"')
   {
   buf+=2;
   goto loop1;
   }
 ptrs[n++]=start;
 if(*buf==',')
   {
   *buf++=0;
   goto loop;
   }
 else if(*buf=='"')
   {
   *buf++=0;
   if(*buf==',')
     {
     ++buf;
     goto loop;
     }
   }
 return n;
 }

void show_line(char **ptrs, int n)
  {
  int x;
  printf("%d: ",n);
  for (x = 0; x != n; ++x)
    printf("\"%s\" ",ptrs[x]);
  printf("\n");
  }

struct field
  {
  struct field *next;
  char *name;
  char *value;
  int mark;
  };

struct item
  {
  struct item *next;
  struct field *fields;
  };

struct assy
  {
  struct assy *next;
  struct item *header;
  struct assy *items;
  int mark;
  };

void clr_fields(struct item *i)
  {
  struct field *f;
  for (f = i->fields; f; f = f->next)
    f->mark = 0;
  }

void clr_assy(struct assy *a)
  {
  while(a)
    {
    a->mark = 0;
    a = a->next;
    }
  }

void show_item(struct item *i)
  {
  struct field *f;
  printf("Item:\n");
  for (f = i->fields; f; f = f->next)
    printf("  %s = %s\n", f->name, f->value);
  }

void quote(char *s)
  {
  while(*s)
    {
/* should not be needed
    if (*s=='"')
      printf("\"");
*/
    printf("%c",*s++);
    }
  }

/* Query functions */

char *get_field(struct item *i, char  *name)
  {
  struct field *f;
  for (f = i->fields; f; f=f->next)
    if (!strcmp(f->name, name))
      break;
  if (f)
    return f->value;
  else
    return 0;
  }

void show_item_rev1(struct field *f)
  {
  if (f->next)
    {
    show_item_rev1(f->next);
    printf(",\"");
    quote(f->value);
    printf("\"");
    }
  else
    { /* First */
    printf("\"");
    quote(f->value);
    printf("\"");
    }
  }

void show_item_rev_diff1(struct field *a,struct item *b)
  {
  if (a->next)
    {
    char *s;
    show_item_rev_diff1(a->next,b);
    s = get_field(b, a->name);
    if (!s) s="";
    if (strcmp(a->value, s))
      {
      printf(",\"");
      quote(a->value);
      printf("\"");
      }
    else
      printf(",");
    }
  else
    { /* First */
    char *s;
    s = get_field(b, a->name);
    if (!s) s="";
    if (strcmp(a->value, s))
      {
      printf("\"");
      quote(a->value);
      printf("\"");
      }
    }
  }

void show_item_rev(struct item *i, int nw)
  {
  if(nw)
    printf("new BOM,");
  else
    printf("old BOM,");
  show_item_rev1(i->fields);
  printf("\n");
  }

void show_item_rev_diff(struct item *a, struct item *b)
  {
  printf("new BOM,");
  show_item_rev_diff1(a->fields,b);
  printf("\n\n");
  }

void set_field(struct item *i, char *name)
  {
  struct field *f;
  for (f = i->fields; f; f=f->next)
    if (!strcmp(f->name, name))
      {
      f->mark = 1;
      break;
      }
  }

struct assy *get_assy(struct assy *a, char *name)
  {
  while (a)
    {
    if (!strcmp(name, get_field(a->header, col_pn)))
      break;
    a=a->next;
    }
  return a;
  }

struct assy *mark_assy(struct assy *a, char *name)
  {
  while (a)
    {
    if (!strcmp(name, get_field(a->header, col_pn)))
      {
      a->mark = 1;
      break;
      }
    a=a->next;
    }
  return a;
  }

/* Show database */

void indent(int x)
  {
  int z;
  for(z = 0; z != x; ++z)
    printf(" ");
  }

void show_assy(struct assy *a, int ind)
  {
  printf("%2.2s:", get_field(a->header, col_indent_level));
  indent(ind); printf("%s %s %s\n", get_field(a->header, col_pn), get_field(a->header, col_qty), get_field(a->header, col_title));
  for (a = a->items; a; a = a->next)
    show_assy(a, ind + 2);
  }

/* Load a single item */

struct item *load_item(FILE *f, char **hptrs, int nh)
  {
  struct item *i;
  struct field *z;
  int x;
  char buf[MAXLINE];
  char *ptrs[MAXFIELDS];
  int n;
  if (!fgets(buf, MAXLINE, f))
    return 0;
  n = fields(buf, ptrs);
  /* show_line(ptrs, n); */
  i = (struct item *)malloc(sizeof(struct item));
  i->next = 0;
  i->fields = 0;
  for (x = 0; x != n && x != nh; ++x)
    {
    z = (struct field *)malloc(sizeof(struct field));
    z->name = strdup(hptrs[x]);
    z->value = strdup(ptrs[x]);
    z->next = i->fields;
    i->fields = z;
    }
  /* Add fields from header */
  for(; x != nh; ++x)
    {
    z = (struct field *)malloc(sizeof(struct field));
    z->name = strdup(hptrs[x]);
    z->value = strdup("");
    z->next = i->fields;
    i->fields = z;
    }
  return i;
  }

/* Load an assembly */

struct assy *load_assy(FILE *f, char **hptrs, int nh, struct item *i, struct item **ra)
  {
  struct assy *a = (struct assy *)malloc(sizeof(struct assy));
  struct assy *b;
  char *part = 0;
  a->next = 0;
  a->header = i;
  a->items = 0;
  if (i)
    {
    part = get_field(i, col_pn);
    /* printf("Loading part %s\n",part); */
    }
  i = load_item(f, hptrs, nh);
  if (i && !part) /* For first one */
    {
    part = get_field(i, col_parent_assy);
    /* printf("Loading part %s\n",part); */
    a->header = (struct item *)malloc(sizeof(struct item));
    a->header->next = 0;
    a->header->fields = (struct field *)malloc(sizeof(struct field));
    a->header->fields->next = 0;
    a->header->fields->name = strdup(col_pn);
    a->header->fields->value = strdup(part);
    }
  /* show_item(i); */
  while (i)
    {
    char *ip = get_field(i, col_parent_assy);
    /* Part of this assy? */
    if (strcmp(ip, part))
      break;
    b = load_assy(f, hptrs, nh, i, &i);
    b->next = a->items;
    a->items = b;
    }
  *ra = i;
  return a;
  }

/* Load a database */

struct assy *load(char *name,int e)
  {
  struct assy *list;
  struct item *i;
  char hbuf[MAXLINE]; /* Headers */
  char *hptrs[MAXFIELDS];
  int nh; /* Number of headers */
  FILE *f;
  if (!name)
    return 0;
  f = fopen(name,"r");
  if (!f)
    {
    fprintf(stderr,"Couldn't open %s\n",name);
    exit(-1);
    }
  /* Get headers */
  if(!fgets(hbuf,MAXLINE,f))
    {
    fprintf(stderr,"Headers line is missing from %s\n",name);
    exit(-1);
    }
  if(e)
    printf("new BOM,%s\n\n",hbuf);
  nh = fields(hbuf, hptrs);

  /* show_line(hptrs, nh); */

  /* Load assemblies */
  /* i = load_item(f, hptrs, nh); */
  /* show_item(i); */

  list = load_assy(f, hptrs, nh, NULL, &i);

  fclose(f);
  
  return list;
  }

struct list
  {
  struct list *next;
  struct assy *a;
  int full;
  };

struct list *alist;
struct list *dlist;

void add(struct assy *x,int full)
  {
  struct list *l=(struct list *)malloc(sizeof(struct list));
  l->next = alist;
  alist = l;
  l->a = x;
  l->full = full;
  }

void del(struct assy *x,int full)
  {
  struct list *l=(struct list *)malloc(sizeof(struct list));
  l->next = dlist;
  dlist = l;
  l->a = x;
  l->full = full;
  }

void cmp_assy(struct assy *a_top,struct assy *a,struct assy *b_top,struct assy *b,int ind, char *path)
  {
  struct field *f;
  struct assy *x;
  int che=0;
  char *part;
  char *title;
  char newpath[65000];
  part=get_field(a->header,col_pn);
  title=get_field(a->header,col_title);
  if(part && part[0])
    if(path[0])
      sprintf(newpath,"%s (\"%s\") in %s",part,title,path);
    else
      sprintf(newpath,"%s (\"%s\")",part,title);
  else
    newpath[0] = 0;

  /* Compare headers */
  /*
  indent(ind);
  printf("Comparing part %s (\"%s\") in %s...\n",part=get_field(a->header,col_pn),get_field(a->header,col_title),get_field(a->header,col_parent_assy));
  */
  clr_fields(b->header);
  for (f = a->header->fields; f; f = f->next)
    {
    char *y;
    y = get_field(b->header, f->name);
    set_field(b->header, f->name);
    if (y)
      {
      if (strcmp(f->value, y) && strcmp(f->name, col_line)) /* Ignore line number changes */
        {
/*
        if (!header)
          {
          header = 1;
          indent(ind);
          printf("%s...\n",newpath);
          }
        indent(ind);
        printf("  *** Field \"%s\" changed.  New = \"%s\", Old = \"%s\"\n",f->name, f->value, y);
*/
        che=1;
        }
      }
    else
      {
      if (strcmp(f->name, col_rev)) /* Ignore new rev field */
        {
/*
        if (!header)
          {
          header = 1;
          indent(ind);
          printf("%s...\n",newpath);
          }
        indent(ind);
        printf("  *** Field \"%s\" added.  New = \"%s\"\n", f->name, f->value);
*/
        che=1;
        }
      }
    }
  for (f = b->header->fields; f; f = f->next)
    if (!f->mark)
      {
/*
      if (!header)
        {
        header = 1;
        indent(ind);
        printf("%s...\n",newpath);
        }
      indent(ind);
      printf("  *** Field \"%s\" deleted.\n", f->name);
*/
      che=1;
      }

  if (che)
    {
    show_item_rev(b->header,0);
    show_item_rev_diff(a->header, b->header);
    }
#if 0
  if (che)
    {
    add(a,0); del(b,0); /* Header change only */
    }
#endif

  /* Compare contents */
  clr_assy(b->items);
  for (x = a->items; x; x = x->next)
    {
    struct assy *y;
    char *item_name = get_field(x->header, col_pn);
    y = get_assy(b->items, item_name);
    mark_assy(b->items, item_name);
    if (y)
      {
      cmp_assy(a_top, x, b_top, y, ind + 2,newpath);
      }
    else
      {
/*
      if (!header)
        {
        header = 1;
        indent(ind);
        printf("%s...\n",newpath);
        }
      indent(ind);
      printf("  *** Part \"%s\" (\"%s\") has been added to \"%s\"\n", item_name, get_field(x->header, col_title), part);
*/
      add(x,1); /* Show full */
      }
    }
  for (x = b->items; x; x = x->next)
    {
    if (!x->mark)
      {
/*
      if (!header)
        {
        header = 1;
        indent(ind);
        printf("%s...\n",newpath);
        }
      indent(ind);
      printf("  *** Part \"%s\" (\"%s\") has been deleted from \"%s\"\n", get_field(x->header, col_pn), get_field(x->header, col_title), part);
*/
      del(x,1); /* Show full */
      }
    }
  }

void show_rev(struct assy *i,int nw)
  {
  if (i->next)
    show_rev(i->next, nw);
  show_item_rev(i->header, nw);
  if (i->items) show_rev(i->items, nw); /* Recurse? */
  }

void show_list(struct list *l, int nw)
  {
  while(l)
    {
    show_item_rev(l->a->header, nw);
    if (l->full)
      { /* Show contents */
      if (l->a->items) show_rev(l->a->items, nw);
      }
    l=l->next;
    }
  }

int main(int argc, char *argv[])
  {
  if (argc != 3 && argc != 2)
    {
    fprintf(stderr,"bomcmp a [b]\n");
    return -1;
    }

  /* Load them */
  struct assy *a = load(argv[1],1);
  struct assy *b = load(argv[2],0);

  if (!b)
    {
    /* Just list */
    if (a)
      {
      show_assy(a, 0);
      return 0;
      }
    else
      {
      fprintf(stderr,"No BOM loaded\n");
      return -1;
      }
    }

  /* Compare each assy */
  printf("Changed parts\n");
  cmp_assy(a,a,b,b,0,"");

  printf("\nAdded parts\n");
  show_list(alist,1);

  printf("\nDeleted parts\n");
  show_list(dlist,0);

  return 0;
  }
