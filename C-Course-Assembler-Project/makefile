
TARGET = my_assembler
SRCS_C = my_assembler.c data_structures.c utils.c parsing.c first_pass.c second_pass.c
CC = gcc
CFLAGS = -Wall -g -ansi -pedantic 
LIBS = -lm
OBJ= $(SRCS_C:.c=.o)


$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)  $(LIBS)

$(OBJ) : defs.h	utils.h
data_structures.o my_assembler.o first_pass.o  second_pass.o  :  data_structures.h 
parsing.o first_pass.o second_pass.o  		:  parsing.h	
my_assembler.o first_pass.o						:  first_pass.h
my_assembler.o second_pass.o 	:second_pass.h


.PHONY : clean
clean : 
	rm $(TARGET) *.o