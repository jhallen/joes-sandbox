OBJS=re_comp.o re_dfa.o re_exec.o
CFLAGS = -DDEBUG -g

re : ${OBJS}
	cc ${CFLAGS} -o re ${OBJS}

clean :
	rm -f ${OBJS}
