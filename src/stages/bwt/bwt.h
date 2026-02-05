typedef struct{
	unsigned char* file;	
	unsigned int sentinel_position;
}Bwt_Aggregate;

Bwt_Aggregate bwt(unsigned char* input_file, unsigned int n);
//input_file is the user given byte array whose burrows wheeler transform we want to find, its size is n.
//We store this output in a Bwt_Aggregator struct, which contains sentinel position and the bwt output file.

void reverse_bwt(Bwt_Aggregate input , unsigned char* output_file, unsigned int file_length);
