/* Example usage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "meta.h"

int main(int argc,char *argv[])
  {
  FILE *f=fopen(argv[1],"r");

  // Parse XML file
  struct foo *b=(struct foo *)xml_parse(f,"root",metadata,1);

  if (b) {

    // Print in various formats
    xml_print(stdout,"root",0,(struct base *)b);
    lisp_print_untagged(stdout,"root",0,(struct base *)b);
    lisp_print(stdout,"root",0,(struct base *)b);
    indent_print(stdout,"root",0,(struct base *)b);
    indent_print_untagged(stdout,"root",0,(struct base *)b);
    json_print(stdout,NULL,0,(struct base *)b,0);
  }

  return 0;
  }
