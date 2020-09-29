OPT=-g -Wall -I./include
OBJ= obj/vigenere.o 

ALL:vigenere

vigenere: obj/vigenere.o 
	$(CC) -o $@ $^

obj/vigenere.o: src/vigenere.c include/vigenere.h
	$(CC) -o $@ -c $< $(OPT)

clean:
	rm -rf obj/*.o
	rm ./vigenere
