#include <setjmp.h>

int alloc1(int level);

int alloc(int level)
{
    char space[1024];
    jmp_buf env;
    setjmp(env);
    printf("\n");
    printf("%x\n", env->__jmpbuf[0]);
    printf("%x\n", env->__jmpbuf[1]);
    printf("%x\n", env->__jmpbuf[2]);
    printf("%x  SP\n", env->__jmpbuf[3]);
    printf("%x\n", env->__jmpbuf[4]);
    printf("%x  PC\n", env->__jmpbuf[5]);
    if (level==7) return;
    else
      alloc1(level+1);
}

int alloc1(int level)
{
    char space[1024];
    jmp_buf env;
    setjmp(env);
    printf("\n");
    printf("%x\n", env->__jmpbuf[0]);
    printf("%x\n", env->__jmpbuf[1]);
    printf("%x\n", env->__jmpbuf[2]);
    printf("%x  SP\n", env->__jmpbuf[3]);
    printf("%x\n", env->__jmpbuf[4]);
    printf("%x  PC\n", env->__jmpbuf[5]);
    if (level==7) return;
    else
      alloc(level+1);
}

main()
{
  alloc(0);
}
