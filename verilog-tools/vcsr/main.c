/* VCSR
 *
 * Joe Allen,  Sep 2004
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan.h"
#include "parse.h"
#include "database.h"

struct inst *design;

int do_scan;
int do_tree;
int do_regs;
int do_elab;
int do_chdr;

char *output_file_name;

int main(int argc,char *argv[])
  {
  int x;
  if(argc<2)
    {
    printf("\n\nVCSR - extract CSRs from Verilog\n\n");
    printf("  by: Joe Allen, Sep 2004\n\n");
    printf("Create C header register definition file from a Verilog project\n\n");
    printf("Syntax: vcsr [options] verilog-files\n");
    printf("  -i path           Add include file path\n");
    printf("  -t module_name    Set top-level module\n");
    printf("  -d macro 'body'   Create a `define macro\n");
    printf("  -o file           Set output file for C header file\n");
    printf("Output options:\n");
    printf("  -scan             Tokenize the Verilog files (no further parsing)\n");
    printf("  -tree             Show unelaborated design reference hierarchy\n");
    printf("  -elab             Show elaborated design hierarchy\n");
    printf("  -regs             Find programmable registers\n");
    printf("  -chdr             Generate C .h definition of programmable registers\n");
    return -1;
    }

  printf("Reading source files...\n");

  for(x=1;argv[x];++x)
    if(!strcmp(argv[x],"-i") && argv[x+1]) // Include path
      {
      ++x;
      if(argv[x])
        add_path(argv[x]);
      }
    else if(!strcmp(argv[x],"-t") && argv[x+1]) // Set top-level module
      {
      ++x;
      strcpy(select_top,argv[x]);
      }
    else if(!strcmp(argv[x],"-d") && argv[x+1] && argv[x+2]) // Define a macro
      {
      mk_macro(argv[x+1],argv[x+2]);
      x+=2;
      }
    else if(!strcmp(argv[x],"-o") && argv[x+1])
      {
      ++x;
      output_file_name=argv[x];
      }
    else if(!strcmp(argv[x],"-scan"))
      {
      do_scan=1;
      }
    else if(!strcmp(argv[x],"-tree"))
      {
      do_tree=1;
      }
    else if(!strcmp(argv[x],"-elab"))
      {
      do_elab=1;
      }
    else if(!strcmp(argv[x],"-regs"))
      {
      do_regs=1;
      }
    else if(!strcmp(argv[x],"-chdr"))
      {
      do_chdr=1;
      }
    else
      {
      source_push(argv[x]);
      if(do_scan)
        compfile();
      else
        parse();
      }

  printf("done.\n");

  if(do_scan)
    return;

  printf("Linking...\n");

  link();

  printf("done.\n");

  find_top();
  if(do_tree)
    {
    show_tree(0,top);
    return 0;
    }

  printf("Elaborating design...\n");
  design=elaborate(NULL,"top",NULL,NULL,top,NULL,NULL);
  printf("done.\n");

  if(do_elab)
    show_design(0,design);

  // Find all leaf nodes with 'ADDR' parameter
  if(do_regs)
    {
    printf("Programmable registers:\n");

    find_em(design);
    }

  if(do_chdr)
    {
    FILE *f;
    printf("Generating chdr file...\n");
    find_em_chdr(design);
    if(output_file_name)
      {
      f=fopen(output_file_name,"w");
      if(!f)
        {
        printf("Couldn't open output file\n");
        return -1;
        }
      else
        {
        gen_chdr(f,design->module);
        fclose(f);
        }
      }
    else
      {
      gen_chdr(stdout,design->module);
      }
    printf("done.\n");
    }

  return 0;
  }
