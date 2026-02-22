typedef struct{
	unsigned char* meta_data;
	unsigned char* data;
	unsigned int meta_data_size;
	unsigned int data_size;
}Compressed_File;

Compressed_File compress(unsigned char* input_file, unsigned int n);
