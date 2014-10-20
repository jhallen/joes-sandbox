#include "config.h"
#include "blocks.h"
#include "heap.h"
#include "object.h"
#include "disp.h"
#include "sc.h"
#include "gedit.h"

ttsig()
{
}

main(argc,argv)
char *argv[];
{
if(argc!=2)
 {
 printf("draft name\n");
 exit(1);
 }
strcpy(name,argv[1]);
load();
loadfont();

ttopen();
graph();

mainw=mkw(1,1,width-2,height-2);
upd=1;

edit();
text();
ttclose();
}
