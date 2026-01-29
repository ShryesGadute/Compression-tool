//output_length is the address of the variable that holds then length of the transformed input after rle
typedef struct{
	unsigned char* output;
	unsigned int output_length;
	unsigned char* block_map;
	unsigned int block_map_length;
}rle_block;

typedef struct{
	unsigned char* output;
	unsigned int total_n;
	unsigned int array_1_n;
	unsigned int array_2_n;
	unsigned int array_3_n;
}Rle_Aggregate;

rle_block run_length_encoding(unsigned char* input, unsigned int n);
Rle_Aggregate run_length_encoding_i(unsigned char* input, unsigned int n);
unsigned char* reverse_run_length_encoding_i(Rle_Aggregate input, unsigned int* n);
