# OPT=-O6
CFLAGS=-I. -Wall -pedantic -ansi -g $(OPT) $(POOL)
OBJ=lex.yy.o y.tab.o edfelement.o
LIB=libedf.a

targets: edfparser file $(LIB)

edfparser: main.o $(LIB)
	cc -g main.o -o edfparser -L. -ledf

file: file.o $(LIB)
	cc -g file.o -o file -DFILE_TEST -L. -ledf

test: edfparser tests test.pl
	perl test.pl < tests

libedf.a: $(OBJ)
	ar rc libedf.a $(OBJ)
	ranlib libedf.a

y.tab.c y.tab.h: edf.y edf.h
	bison -vd -p edf_p edf.y -o y.tab.c

lex.yy.c: edf.l edf.h
	flex -Pedf_p -olex.yy.c edf.l

clean:
	rm -f y.tab.[ch] lex.yy.c *~ *.o edfparser edfparser.exe file.exe test.exe *.output *.a *.so file

main.o: main.c y.tab.h
