OBJS = lex.o main.o y.tab.o
CFLAGS = -g

all : y.tab.c lex

y.tab.c : gram.y
	yacc -d gram.y

lex : $(OBJS)
	$(CC) $(OPTS) -o lex $(OBJS)

clean:
	rm -f $(OBJS) y.tab.c y.tab.h
