//output_length is the address of the variable that holds then length of the transformed input after rle
typedef struct{
	unsigned char* output;
	unsigned int output_length;
	unsigned char* block_map;
	unsigned int block_map_length;
}rle_block;

rle_block run_length_encoding(unsigned char* input, unsigned int n);

