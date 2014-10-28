/* Example usage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "sdu.h"
#include "schema.h"

struct schema myschema=
  {
    NULL,
    {
#include "meta.h"
#include "schema.h"
      { 0, 0 }
    }
  };

int main(int argc,char *argv[])
  {
  FILE *f=fopen(argv[1],"r");

  // Parse XML file
  struct foo *b=(struct foo *)xml_parse(f,"root",&myschema,metafind(&myschema, "foo"),1);
  struct item *i;

  // Access values in foo
  printf("root->val = %d\n",b->val);
  printf("root->val_name = %s\n",b->val_name);
  printf("root->items->val = %d\n",b->items->val);
  printf("root->items->next->val = %d\n",b->items->next->val);
  printf("root->items->next->next->val = %d\n",b->items->next->next->val);

  // Print in various formats
  xml_print(stdout,"root",0,(struct base *)b);

  lisp_print(stdout,"root",0,(struct base *)b);

  lisp_print_untagged(stdout,"root",0,(struct base *)b);

  indent_print(stdout,"root",0,(struct base *)b);

  indent_print_untagged(stdout,"root",0,(struct base *)b);

  json_print(stdout,NULL,0,(struct base *)b,0);

  // Create a database within C
  b=mk(&myschema, "foo");
  b->val=10;
  b->val_name=strdup("Hello");

  // Build list
  i=b->items=mk(&myschema, "item");
  i->val=7;
  i=i->next=mk(&myschema, "item");
  i->val=8;
  i=i->next=mk(&myschema, "item");
  i->val=9;

  // Print it
  xml_print(stdout,"root",0,(struct base *)b);

  return 0;
  }
