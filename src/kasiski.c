#include "vigenere.h"

#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int* frequence(char* s)
{
    char c;
    int occ_table_index = 0;
    int s_length = strlen(s);
    int* occ_table = malloc(256 * sizeof(int));
    if(occ_table == NULL)
    {
	errx(EXIT_FAILURE, "can't allocate size for occ_table");
    }
    for(int i = 0; i < 256; i++){ occ_table[i] = 0;}
    for(int j = 0; j < s_length; j++)
    {
	    c = s[j];
	    occ_table_index = (int)c;
	    occ_table[occ_table_index]++;
    }
    return occ_table;
}

void print_table(int* occ_table)
{
    for(int i = 0; i < 256; i++) {
		printf("%d",occ_table[i]);
    }
}

char** subset_alloc(int key_length, int text_length) {
	char** subset_array = malloc(key_length*sizeof(char*));
	if(subset_array == NULL)
		errx(EXIT_FAILURE, "malloc error in subset_alloc");
	for(int i = 0; i < key_length; i++)
	{
	    char* subset = malloc(ceil(text_length/key_length)*sizeof(char));
		if(subset == NULL)
			errx(EXIT_FAILURE, "malloc error in subset_alloc");
		subset_array[i] = subset;
	}
	return subset_array;
}

void subset_free(char** subset_array, int size) {
	for(int i = 0; i < size; i++) {
		free(subset_array[i]);
	}
	free(subset_array);
}

float* ics_alloc(int size) {
	float* array = malloc(size * sizeof(float));
	if(array == NULL)
		errx(EXIT_FAILURE, "malloc error in ics_alloc");
	return array;
}

int find_key_length(char* cyphered_text)
{
	//float average_table[10];
    int text_length = strlen(cyphered_text);
    int final_length = 0;
    for(int key_length = 1; key_length < 10; key_length++)
    {
		//alloc array
		char** subset_array = subset_alloc(key_length, text_length);

		//init values
		int r;
		int c;
		for(int i = 0; i < text_length ; i++) {
			r = i % key_length;
			c = i / key_length;
			subset_array[r][c] = cyphered_text[i];
		}
		//calculate IC for each subset
		//
		//calculate average IC
		//put this IC in array
		//
		//free subset_array
		subset_free(subset_array, key_length);
    }
    return final_length;
}

int main(int argc, char* argv[])
{
    char* string_test = "coucou";
    if(*argv[1] == 'o')
    {
	int* occ_table;
	occ_table = frequence(string_test);
	print_table(occ_table);
	free(occ_table);
    }

}
