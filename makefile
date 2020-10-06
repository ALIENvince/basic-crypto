OPT=-g -Wall -I./include

ALL:kasiski #vigenere 

%: obj/%.o 
	$(CC) -o $@ $^

obj/%.o: src/%.c include/%.h
	$(CC) -o $@ -c $< $(OPT)

clean:
	rm -rf obj/*.o
	rm ./vigenere
	rm ./kasiski
