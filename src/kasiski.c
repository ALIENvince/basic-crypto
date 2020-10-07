#include "vigenere.h"

#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int* frequence(char* s) {
	char c;
	int occ_table_index = 0;
	int s_length = strlen(s);
	int* occ_table = malloc(256 * sizeof(int));
	if(occ_table == NULL)
	{
		errx(EXIT_FAILURE, "can't allocate size for occ_table");
	}
	for(int i = 0; i < 256; i++) { occ_table[i] = 0;}
	for(int j = 0; j < s_length; j++)
	{
		c = s[j];
		occ_table_index = (int)c;
		occ_table[occ_table_index]++;
	}
	return occ_table;
}

void print_table(int* occ_table) {
	for(int i = 0; i < 256; i++) {
		if(occ_table[i] != 0)
			printf("char: %c -> %d\n", i,occ_table[i]);
	}
}

float* ics_alloc(int size) {
	float* array = malloc(size * sizeof(float));
	if(array == NULL)
		errx(EXIT_FAILURE, "malloc error in ics_alloc");
	return array;
}

float ic(char* subset, int subsize) {
	int* freq_table = frequence(subset);
	float sum = 0;
	int tmp;
	for(int i = 0; i < 256; i++) {
		tmp = freq_table[i];
		sum += (tmp *(tmp-1));
	}
	//print_table(freq_table);
	free(freq_table);
	return sum/(subsize*(subsize-1));
}

char** subset_alloc(int rows, int columns) {
	char** subset_array = malloc(rows*sizeof(char*));

	if(subset_array == NULL)
		errx(EXIT_FAILURE, "malloc error in subset_alloc");

	for(int i = 0; i < rows; i++) {
		char* subset = calloc(columns, sizeof(char));
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

void print_subsets(char **array, int rows, int columns) {
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			printf("%c",array[i][j]);
		}
		printf("\n");
	}
}

int max_array(float* array, int size) {
	float max = array[0];
	int index = 0;
	for(int i = 0; i < size; i++) {
		if(max < array[i]) {
			max = array[i];
			index = i;
		}
	}
	return index;
}

int find_key_length(char* cyphered_text) {
	float average_table[10];
	float average_IC;
	float IC;
	int text_length = strlen(cyphered_text);
	int final_length = 0;
	char** subset_array;
	int rows;
	int columns;

	for(int key_length = 1; key_length <= 10; key_length++)
	{
		//alloc array
		rows = key_length;
		columns = ceil(text_length/key_length)+1;
		subset_array = subset_alloc(rows, columns);

		//init values
		int r;
		int c;
		for(int i = 0; i < text_length ; i++) {
			r = i % key_length;
			c = i / key_length;
			subset_array[r][c] = cyphered_text[i];
		}

		//print_subsets(subset_array, rows, columns);
		//calculate IC for each subset
		float* ics = ics_alloc(key_length);

		for(int i = 0; i < key_length-1; i++) {
			IC = ic(subset_array[i], columns);
			ics[i] = IC;
		}

		//calculate average IC
		average_IC = 0;
		for(int i = 0; i < key_length-1; i++) {
			average_IC += ics[i];
		}
		average_IC = average_IC / key_length;
		free(ics);

		//put this IC in array
		average_table[key_length-1] = average_IC;

		//free subset_array
		subset_free(subset_array, key_length);
	}
	//TODO find maximum in average_table
	for(int i=0; i<10;i++)
	final_length = max_array(average_table, 10)+1;
	return final_length;
}

int main(int argc, char* argv[]) {
	char* string_test = "coucou c'est nous on est la tavu, toujours debout!";
	int keylen = find_key_length(string_test);
	printf("Most probable key length: %d\n", keylen);

	return 0;
}
