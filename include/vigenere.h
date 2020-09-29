#include <stdio.h>


int vigenere_encrypt(char *infile, char *outfile, char key[], int c_or_d);

int cypher(char c, char key[], int* keyindex, int c_or_d);

int find_key_length(char* ciphered_text);
