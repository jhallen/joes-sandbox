OBJS = scan.o parse.o main.o database.o tree.o simp.o chdr.o regex.o extract.o

vcsr: $(OBJS)
	gcc -g -o vcsr $(OBJS)

clean:
	rm -f $(OBJS) *~
