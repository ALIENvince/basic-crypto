OPT=-g -Wall -I./include

ALL:kasiski vigenere 

%: %.o 
	$(CC) -o $@ $^

%.o: src/%.c
	$(CC) -o $@ -c $< $(OPT)

clean:
	rm -rf obj/*.o
	rm ./vigenere
	rm ./kasiski
