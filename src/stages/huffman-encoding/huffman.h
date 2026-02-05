typedef struct{
	unsigned int frequency;
	unsigned short left_child;
	unsigned short right_child;	
	unsigned char symbol;
}Huffman_Node;

typedef struct{
	Huffman_Node* node_list;
	Huffman_Node* root;
	unsigned int num_of_nodes;
}Huffman_Tree;


Huffman_Tree huffman_encode(unsigned char* input, unsigned int n, unsigned int bit_length, unsigned char* output, long long* k);

unsigned int huffman_decode(unsigned char* input, unsigned char* output, long long n_bits, Huffman_Tree ht, unsigned char bit_length);

