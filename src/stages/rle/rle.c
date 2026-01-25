#include <stdlib.h>
#include "rle.h"
unsigned char EIGHT = 1;
#define set_bit(addr, i) addr[i/8] = (addr[i/8] | (EIGHT >> (i%8)))
#define reset_bit(addr, i) addr[i/8] = (addr[i/8] & (~(EIGHT >> (i%8))))

//we define a block in our input as the consecutive range of eqaul elements. we consider the input array array of blocks
//a block can have maximum length of 256 elements, after which a new block must start, even if the new block has same elements as that of the previous one.
//rle_block_map is a bit array, where each bit tells whether the block at that index in array of blocks has length greater than 1 or not, 1bit for yes and 0bit for no.
//in the output each block is compressed as its representative character and length of block if it is greater than 1.
//the length of block in the output doesn't consider the representative character, so something like Q 230 means, Q followed by 230 Q's, so a total of 231 consecutive Qs.
//the rle_block_map is a different array and not the part of the output array, it is meta data.
 
rle_block run_length_encoding(unsigned char* input, unsigned int n){
	rle_block ans; 
	unsigned char* output = malloc(n*sizeof(unsigned char));//n bytes is enough, output array will never occupy more space than the input array.
	unsigned char* rle_block_map = malloc(((n/8)+1)*sizeof(unsigned char));
	unsigned int next_block_index = 0;
	unsigned char current_character;
	unsigned char previous_character = input[0];
	unsigned int count = 0;
	output[0] = previous_character;
	unsigned int output_length = 1; // set to 1 because already placed 1st input character in output
	unsigned int i=1;
	for(unsigned int j=0; j<n/8+1; j++) rle_block_map[j]=0;

	while(i<n){
		current_character = input[i];
		if(current_character == previous_character && count < 255) count++;
		
		else if(current_character == previous_character && count == 255){
			//store
			output[output_length] = count;
			output_length++;
			count = 0;
			set_bit(rle_block_map, next_block_index);
			next_block_index++;
			output[output_length] = current_character;
			output_length++;
		}

		else{
			//store
			if(count > 0){
				output[output_length]=count;
				output_length++;
				count = 0;
				set_bit(rle_block_map, next_block_index);
			}
			else reset_bit(rle_block_map, next_block_index);	
			
			next_block_index++;
			output[output_length]=current_character;
			output_length++;
		}
		previous_character = current_character;
		i++;
	}

	if(count)output[output_length] = count;
	output_length++;

	//create the return struct
	ans.output = output;
	ans.block_map = rle_block_map;
	ans.output_length = output_length;
	ans.block_map_length = next_block_index;

	return ans;
}

