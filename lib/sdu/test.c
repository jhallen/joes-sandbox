/* Example usage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "meta.h"

main(int argc,char *argv[])
  {
  FILE *f=fopen(argv[1],"r");

  // Parse XML file
  struct foo *b=xml_parse(f,metadata);
  struct item *i;

  // Access values in foo
  printf("root->val = %d\n",b->val);
  printf("root->val_name = %s\n",b->val_name);
  printf("root->items->val = %d\n",b->items->val);
  printf("root->items->next->val = %d\n",b->items->next->val);
  printf("root->items->next->next->val = %d\n",b->items->next->next->val);

  // Print in various formats
  xml_print(stdout,0,b);

  lisp_print(stdout,0,b);

  lisp_print_untagged(stdout,0,b);

  indent_print(stdout,0,b);

  indent_print_untagged(stdout,0,b);

  json_print(stdout,0,b,0);

  // Create a database within C
  b=mk("foo");
  b->val=10;
  b->val_name=strdup("Hello");

  // Build list
  i=b->items=mk("item");
  i->val=7;
  i=i->next=mk("item");
  i->val=8;
  i=i->next=mk("item");
  i->val=9;

  // Print it
  xml_print(stdout,0,b);
  }
