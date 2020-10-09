#include "vigenere.h"

#include <err.h>
#include <math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int* frequence(unsigned char* s) {
	unsigned char c;
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

float ic(unsigned char* subset, int subsize) {
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

int mutual_IC(unsigned char* s1, unsigned char* s2, int shift)
{
	int* f1 = frequence(s1);
	int* f2 = frequence(s2);
	int s1_length = strlen(s1);
	int s2_length = strlen(s2);
	float mutual_freq;
	float ICM;
	for(int i = 0; i < 256; i++)
	{
		if(f1[i] != 0 && f2[(i+shift) % 256] != 0)
		{
			mutual_freq += f1[i]*f2[(i+shift) % 256];
		}
	}
	ICM = mutual_freq/(s1_length*s2_length);
	return ICM;
}

int ICM_offset(unsigned char* s1, unsigned char* s2)
{
	int shift_max;
	float ICM;
	float ICM_max = 0;
	for(int shift = 0; shift < 256; shift++)
	{
		ICM = mutual_IC(s1,s2,shift);
		if(ICM > ICM_max)
		{
			ICM_max = ICM;
			shift_max = shift;
		}
		ICM = 0;
	}
	return shift_max;
}

unsigned char** subset_alloc(int rows, int columns) {
	unsigned char** subset_array = malloc(rows*sizeof(unsigned char*));

	if(subset_array == NULL)
		errx(EXIT_FAILURE, "malloc error in subset_alloc");

	for(int i = 0; i < rows; i++) {
		unsigned char* subset = calloc(columns, sizeof(unsigned char));
		if(subset == NULL)
			errx(EXIT_FAILURE, "malloc error in subset_alloc");
		subset_array[i] = subset;
	}
	return subset_array;
}

void subset_free(unsigned char** subset_array, int size) {
	for(int i = 0; i < size; i++) {
		free(subset_array[i]);
	}
	free(subset_array);
}

void print_subsets(unsigned char **array, int rows, int columns) {
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

int max_frequency(int* array) {
	int max = array[0];
	int index = 0;
	int size = 256;
	for(int i = 0; i < size; i++) {
		if(max < array[i]) {
			max = array[i];
			index = i;
		}
	}
	return index;
}

int find_key_length(unsigned char* cyphered_text) {
	float average_table[10];
	float average_IC;
	float IC;
	int text_length = strlen(cyphered_text);
	int final_length = 0;
	unsigned char** subset_array;
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
	for(int i=0; i<10;i++)
		final_length = max_array(average_table, 10)+1;
	return final_length;
}

void print_possible_keys(unsigned char* offset_table)
{
    char key_char;
    for(int i = 0; i < 255; i++)
    {
	for(int j = 0; j < table_length -1; j++)
	{
	    key_char = (offset_table[j] + i) % 256;
	    printf("%c", key_char);
	}
	printf("\n");
    }	    
}

unsigned char* build_offset_table(int keylen, char** subset_table)
{
	int offset;
	int subset_length;
	unsigned char* offset_table = malloc(keylen*sizeof(unsigned char));
	for(int i = 0; i < keylen; i++)
	{
		offset = ICM_offset(subset_table[0], subset_table[i]);  
		offset_table[i] = offset;
	}
	return offset_table;
}

unsigned char **build_sub_array(int key_length, int text_length, unsigned char* ciphered_text) {
	int rows = key_length;
	int columns = ceil(text_length/key_length)+1;
	unsigned char** subset_array = subset_alloc(rows, columns);

	//init values
	int r;
	int c;
	for(int i = 0; i < text_length ; i++) {
		r = i % key_length;
		c = i / key_length;
		subset_array[r][c] = ciphered_text[i];
	}
	return subset_array;
}

unsigned char* recompose_string(unsigned char **sub_array, int text_length, int key_length) {
	unsigned char* res = calloc(text_length+1, sizeof(unsigned char));
	int r;
	int c;
	for(int i = 0; i < text_length ; i++) {
		r = i % key_length;
		c = i / key_length;
		res[i] = sub_array[r][c];
	}
	return res;
}

int main(int argc, char* argv[]) {
	FILE* fd = fopen("test.txt", "r");
	unsigned char c;
	size_t size = 0;
	struct stat st;
	if(stat("test.txt",&st)==0)
		 size = st.st_size;

	unsigned char* str = malloc(size);

	int ind = 0;
	while((c=fgetc(fd)) != EOF) {
		str[ind] = c;
		ind++;
	}

	int keylen = find_key_length(str);
	int textlen = strlen(str);
	printf("Most probable key length: %d\n", keylen);

	unsigned char** sub_array = build_sub_array(keylen, textlen, str);
	shift_all_subset(keylen, sub_array);

	unsigned char* string_res = recompose_string(sub_array, textlen, keylen);
	int* occ_table = frequence(string_res);

	int max_occ = max_frequency(occ_table);

	subset_free(sub_array, keylen);
	return 0;
}
