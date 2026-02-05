#include <stdlib.h>
#include <string.h>
#include "rle.h"
#include "../../bit_man.h"

Rle_Aggregate calculate_memory(unsigned char* input, unsigned int n){
	unsigned int block_size=0;
	unsigned int l1=0; //number of number_of_bats, also represents number of blocks in input array
	unsigned int l2=0; //number of quads, number of blocks whose length is greater than one
	unsigned int l3=0; //number of bTA required to store all non 1 lengths
	unsigned char curr;
	unsigned char prev = ~input[0];	
	unsigned char number_of_bats;

	for(unsigned int i=0; i<n; i++){
		curr = input[i];
		if(curr == prev) block_size++;
		else if(block_size == 1) l1++;

		else if(block_size > 1){
			l1++;
			l2++;
			number_of_bats = 0;
			while(block_size != 0){
				block_size >>= 2;
				number_of_bats++;
			}
			l3 += number_of_bats;
			block_size = 1;

		}
		else block_size = 1;
		prev = curr;
		
	}

	if(block_size == 1) l1++;
	else if(block_size > 1){
		l1++;
		l2++;
		number_of_bats = 0;
		while(block_size != 0){
			block_size >>= 2;
			number_of_bats++;	
		}

		l3 += number_of_bats;
	}
	
	unsigned char* array_required = malloc(((l1+7)/8 + (l2+1)/2 + (l3+1)/2 + l1)*sizeof(unsigned char)); // plus 8 is for memory safety, so we don't access memory that doesn't belong to us.
	Rle_Aggregate result;
	result.output = array_required;
	result.array_1_n = l1;
	result.array_2_n = l2;
	result.array_3_n = l3;
	result.total_n = n;
	return result;
}

Rle_Aggregate run_length_encoding_i(unsigned char* input, unsigned int n){
	Rle_Aggregate result = calculate_memory(input, n);
	unsigned char* array_1 = result.output;
	unsigned char* array_2 = array_1 + (result.array_1_n+7)/8;
	unsigned char* array_3 = array_2 + (result.array_2_n+1)/2;
	unsigned char* array_4 = array_3 + (result.array_3_n+3)/4;

	unsigned int i_1=0; //iterator for array_1 
	unsigned int i_2=0; //iterator for array_2
	unsigned int i_3=0; //iterator for array_3
	
	unsigned char current_char;
	unsigned char previous_char;
	
	char number_of_bats;
	unsigned int block_size; 
	unsigned int temp_size;
	unsigned char temp;	

	previous_char = ~input[0];
	block_size = 0;

	memset(result.output, 0, ((result.array_1_n+7)/8 + (result.array_2_n+1)/2 + (result.array_3_n+3)/4));

	for(unsigned int i=0; i<n; i++){
		current_char = input[i];
		if(current_char == previous_char) block_size++;

		else if(block_size == 1){
			//reset_bit(array_1, i_1); we don't need to do this because that bit is 0 because of out initialization
			array_4[i_1] = previous_char;
			i_1++;
		}

		else if(block_size > 1){
			write_bit(array_1, i_1, 1);
			array_4[i_1] = previous_char;
			i_1++;

			number_of_bats = -1;
			temp_size=block_size;
			while(temp_size != 0){
				temp_size >>= 2;
				number_of_bats++;
			}
			write_quad(array_2, i_2, number_of_bats);
			i_2++;
			i_3 += number_of_bats;
			for(unsigned char k=0; k<number_of_bats+1; k++){
				temp = (block_size & 3);
				block_size >>= 2;
				write_bat(array_3, i_3-k, temp);
			}
			i_3++;
			block_size = 1;		
			
		}

		else block_size = 1;
		
		previous_char = current_char;
	}

	if(block_size == 1) array_4[i_1++] = previous_char;

	else if(block_size > 1){
		write_bit(array_1, i_1, 1);
		array_4[i_1] = previous_char;
		i_1++;

		number_of_bats = -1;
		temp_size=block_size;
		while(temp_size != 0){
			temp_size >>= 2;
			number_of_bats++;
		}
		write_quad(array_2, i_2, number_of_bats);
		i_2++;
		i_3 += (number_of_bats);
		for(unsigned char k=0; k<number_of_bats+1; k++){
			temp = (block_size & 3);
			block_size >>= 2;
			write_bat(array_3, i_3-k, temp);
		}			
		i_3++;
		block_size = 1;			
	}

	return result;
}


unsigned char* reverse_run_length_encoding_i(Rle_Aggregate input){
	unsigned char* result = malloc(input.total_n*sizeof(unsigned char));
	unsigned char* array_1 = input.output;
	unsigned char* array_2 = array_1 + (input.array_1_n+7)/8;
	unsigned char* array_3 = array_2 + (input.array_2_n+1)/2;
	unsigned char* array_4 = array_3 + (input.array_3_n+3)/4;

	unsigned int i_1=0; //iterator for array_1 
	unsigned int i_2=0; //iterator for array_2
	unsigned int i_3=0; //iterator for array_3
	unsigned int filled =0; //how much the output array has been filled
	
	unsigned int count;
	unsigned char count_length;
	unsigned char current_character;

	for(unsigned int i=0; i<input.array_1_n; i++){
		current_character = array_4[i];
		if(read_bit(array_1, i)){
			count_length = read_quad(array_2, i_2);
			count_length += 1;
			i_2++;
			
			count = 0;
			while(count_length != 0){
			count <<= 2;
			count += read_bat(array_3, i_3);
			i_3++;
			count_length--;
			}
			

		}
		
		else count = 1;

		while(count != 0){
			result[filled++] = current_character;
			count--;
		}
	}
	
	return result;
}










