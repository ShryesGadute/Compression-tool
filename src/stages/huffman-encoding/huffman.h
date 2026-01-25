typedef struct{
	unsigned char symbol;
	unsigned int frequency;
	unsigned short left_child;
	unsigned short right_child;	
}Huffman_Node;

typedef struct{
	Huffman_Node* node_list;
	Huffman_Node* root;
}Huffman_Tree;

void print_codes(unsigned char* input, unsigned int n);

Huffman_Tree huffman_encode(unsigned char* input, unsigned int n, unsigned char* ouput, unsigned int* k);
