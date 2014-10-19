#include <stdio.h>
#include "scan.h"

main(int argc,char *argv[])
  {
  if (argv[1])
    source_push(argv[1], 0);
  // compfile();
  pretty();
  }
