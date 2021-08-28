C_FILES = main.c pass1.c pass2.c OutputToFiles.c SymbolTable.c code.c data.c  
H_FILES = definitions.h pass1.h pass2.h SymbolTable.h OutputToFiles.h data.h code.h
O_FILES = $(C_FILES:.c=.o)


all: assembler
assembler: $(O_FILES) 
	gcc -Wall -ansi -pedantic $(O_FILES) -o assembler 
%.o: %.c $(H_FILES)
	gcc -Wall -ansi -pedantic -c -o $@ $<
clean:
	rm -f *.o 
