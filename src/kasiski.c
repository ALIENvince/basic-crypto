#include <err.h>
#include <math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int* frequence(char* s, int size) {
	char c;
	int occ_table_index = 0;
	int* occ_table = malloc(256 * sizeof(int));
	if(occ_table == NULL)
	{
		errx(EXIT_FAILURE, "can't allocate size for occ_table");
	}
	for(int i = 0; i < 256; i++) { occ_table[i] = 0;}
	for(int j = 0; j < size; j++)
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
	int* freq_table = frequence(subset, subsize);
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

int mutual_IC(char* s1,int s1_length, char* s2, int s2_length, int shift)
{
	int* f1 = frequence(s1, s1_length);
	int* f2 = frequence(s2, s2_length);
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

int ICM_offset(char* s1,int s1_len, char* s2, int s2_len)
{
	int shift_max;
	float ICM;
	float ICM_max = 0;
	for(int shift = 0; shift < 256; shift++)
	{
		ICM = mutual_IC(s1,s1_len,s2,s2_len,shift);
		if(ICM > ICM_max)
		{
			ICM_max = ICM;
			shift_max = shift;
		}
		ICM = 0;
	}
	return shift_max;
}

int get_subset_size(int keylen, int textlen, int row) {
	int tmp = textlen;
	int i = 0;
	while(tmp > 0) {
		tmp = tmp-keylen;
		i++;
	}

	if(row > keylen+tmp) {
		return i-1;
	}
	return i;
}

char** subset_alloc(int keylen, int textlen) {
	char** subset_array = malloc(keylen*sizeof(char*));

	if(subset_array == NULL)
		errx(EXIT_FAILURE, "malloc error in subset_alloc");

	for(int i = 0; i < keylen; i++) {
		char* subset = calloc(get_subset_size(keylen,textlen, i), sizeof(char));
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

void print_subsets(char **array, int keylen) {
	for(int i = 0; i < keylen; i++) {
		printf("%s", array[i]);
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

int find_key_length(char* cyphered_text, int text_length) {
	float average_table[10];
	float average_IC;
	float IC;
	int final_length = 0;
	char** subset_array;

	for(int key_length = 1; key_length <= 10; key_length++)
	{
		//alloc array
		subset_array = subset_alloc(key_length, text_length);

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
			IC = ic(subset_array[i], get_subset_size(key_length,text_length,i));
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

void print_possible_keys(char* offset_table, int keylen)
{
	char key_char;
	for(int i = 0; i < 255; i++)
	{
		for(int j = 0; j < keylen -1; j++)
		{
			key_char = (offset_table[j] + i) % 256;
			printf("%c", key_char);
		}
		printf("\n");
	}	    
}

char* build_offset_table(int keylen, int textlen, char** subset_table)
{
	int offset;
	char* offset_table = malloc(keylen*sizeof(char));
	for(int i = 0; i < keylen; i++)
	{
		offset = ICM_offset(subset_table[0],get_subset_size(keylen, textlen, 0), subset_table[i], get_subset_size(keylen, textlen, i));  
		offset_table[i] = offset;
	}
	printf("building offset: ");
	for(int i = 0; i < keylen; i++) {
		printf("%d", offset_table[i]);
	}
	printf("\n");
	return offset_table;
}


char **build_sub_array(int key_length, int text_length, char* ciphered_text) {
	char** subset_array = subset_alloc(key_length, text_length);

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

char* recompose_string(char **sub_array, int text_length, int key_length) {
	char* res = calloc(text_length+1, sizeof(char));
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
	char* path = argv[1];
	printf("path: %s\n", path);
	FILE* fd = fopen(path, "r");
	if(fd == NULL)
		err(EXIT_FAILURE, "fopen:");
	char c;


	int ind = 0;
	while((c=fgetc(fd)) != EOF) {
		ind++;
	}

	char* str = malloc(ind*sizeof(char));
	if(str==NULL)
		errx(EXIT_FAILURE, "malloc error");


	rewind(fd);
	ind = 0;
	while((c=fgetc(fd)) != EOF) {
		str[ind] = (char)c;
		ind++;
	}

	int keylen = find_key_length(str, ind);
	int textlen = ind;
	printf("Most probable key length: %d\n", keylen);

	char** sub_array = build_sub_array(keylen, textlen, str);
	print_subsets(sub_array,keylen);

	char* offtable = build_offset_table(keylen, textlen, sub_array);

	print_possible_keys(offtable, keylen);

	subset_free(sub_array, keylen);
	return 0;
}
