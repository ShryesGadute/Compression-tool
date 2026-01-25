#include <stdlib.h>
#include "suffix_array.h"
#include "bwt.h"


Bwt_Aggregate bwt(unsigned char* input_file, unsigned int file_length){
	unsigned int* SA = (unsigned int*)malloc(file_length*sizeof(unsigned int));
	saca_k(input_file, SA, 256, file_length, file_length, 0);
	unsigned char* output_file = malloc(file_length*sizeof(unsigned char));

	output_file[0] = input_file[file_length-1];// put last character left of virtual sentinel into the output
	unsigned int i = 0;
	unsigned int j = 1;

	Bwt_Aggregate out;

	while(SA[i] != 0){
		output_file[j] = input_file[SA[i]-1];
		i++;
		j++;
	}

	out.sentinel_position = ++i;

	while(i<file_length){
		output_file[j] = input_file[SA[i]-1];
		i++;
		j++;

  }
	free(SA);
	out.file = output_file;

  return out;
}

void reverse_bwt(Bwt_Aggregate input, unsigned char* output_file, unsigned int file_length){
//The function below uses the LF mapping property of bwt.
	unsigned int char_bkt[256] = {0};
	unsigned int* rank = malloc(file_length*sizeof(unsigned int));
	unsigned int i;
	unsigned char* input_file_data = input.file;
	
	//calculate the start of each character bucket;
	for(i=0; i<256; i++) char_bkt[i] = 0;
	for(i=0; i<file_length; i++){
		rank[i] = char_bkt[input_file_data[i]]++;
	}
	
	//calculate starts of the buckets
	unsigned int next_bkt = 1;
	for(i=0; i<256; i++){
		next_bkt += char_bkt[i];
		char_bkt[i] = next_bkt - char_bkt[i];
	}

	unsigned int sentinel_position = input.sentinel_position;
	unsigned int prev_position = 0;
	unsigned int characters_recovered = 0;
	unsigned char current_character;
	while(characters_recovered < file_length){
		current_character = input_file_data[prev_position];
		output_file[file_length-1-characters_recovered] = current_character;
		prev_position = char_bkt[current_character] + rank[prev_position];
		prev_position -= (prev_position > sentinel_position ? 1:0);	
		characters_recovered++;
	}
	free(rank);
}




