#include <stdlib.h>
#include <string.h>
#include "../stages/bwt/bwt.h"
#include "../stages/rle/rle.h"
#include "../stages/move-to-front/mtf.h"
#include "../stages/huffman-encoding/huffman.h"
#include "../bit_man.h"
#include "pipeline.h"

unsigned short DNE = 0x8000;

//pipeline connects all the stages and decides how the data flows from satge to stage and decides how the meta data is processed and stored.


typedef struct{
	unsigned char* tree;
	unsigned char*symbols;
	unsigned int num_of_nodes;
	unsigned int total_symbols;
}Flattened_Tree;

Flattened_Tree flatten_tree(Huffman_Tree ht){
	Flattened_Tree result;
	unsigned char* tree = malloc(((ht.num_of_nodes+7)/8)*sizeof(unsigned char));
	unsigned char* symbols = malloc(((ht.num_of_nodes+1)/2)*sizeof(unsigned char));
	unsigned short stack[256];
	unsigned short stack_length=0;
	stack[stack_length++] = ht.root-ht.node_list;
	unsigned int i_1;
	unsigned int i_2;
	i_1 = 0;
	i_2 = 0;
	unsigned short curr_node;

	while(stack_length != 0){
		curr_node = stack[--stack_length];
		if(ht.node_list[curr_node].left_child == DNE){
			write_bit(tree, i_1, 0);
			i_1++;
			symbols[i_2] = ht.node_list[curr_node].symbol;
			i_2++;
		}

		else{
			write_bit(tree, i_1, 1);
			i_1++;
			stack[stack_length++] = ht.node_list[curr_node].right_child;
			stack[stack_length++] = ht.node_list[curr_node].left_child;
		}	
	}

	result.tree = tree;
	result.symbols = symbols;
	result.num_of_nodes = ht.num_of_nodes;
	result.total_symbols = ht.root->frequency;

	return result;
}

Huffman_Tree unflatten_tree(Flattened_Tree ft){
	unsigned int i=0;
	unsigned int i_1=0;
	unsigned short stack[256];
	unsigned short stack_length = 1;
	unsigned short allocated = 1;
	stack[0] = 0;
	Huffman_Node* node_list = malloc(2*ft.num_of_nodes*sizeof(Huffman_Node));
	Huffman_Node root = node_list[0];
	root.left_child = DNE;
	root.right_child = DNE;
	for(i=1; i<ft.num_of_nodes; i++){
		Huffman_Node node = node_list[allocated++];
		node.left_child = DNE;
		node.right_child = DNE;
		
		Huffman_Node parent = node_list[stack[stack_length-1]];
		if(parent.left_child = DNE) parent.left_child = allocated-1;
		else{
			parent.right_child = allocated-1;
			stack_length--;
		}

		if(read_bit(ft.tree, i)) stack[stack_length++] = allocated-1;
		else node.symbol = ft.symbols[i_1++];
	}
	
	Huffman_Tree ht;
	ht.node_list = node_list;
	ht.root = node_list;
	ht.num_of_nodes = ft.num_of_nodes;

	return ht;
}

void copy_tree(Flattened_Tree ft, long long* num_of_bits, unsigned char* meta_data, unsigned int* total){
	unsigned char* new_addr = meta_data + (*total);
	memcpy(new_addr, num_of_bits, sizeof(long long));
	new_addr = new_addr + sizeof(long long);
	memcpy(new_addr, &(ft.num_of_nodes), sizeof(unsigned int));
	new_addr += sizeof(unsigned int);
	memcpy(new_addr, ft.symbols, (ft.num_of_nodes+1)/2);
	new_addr += ft.num_of_nodes;
	memcpy(new_addr, ft.tree, (ft.num_of_nodes+7)/8);

	*total += sizeof(long long) + sizeof(unsigned int) + ft.num_of_nodes + ((2*ft.num_of_nodes-1)+7)/8;
}


Compressed_File compress(unsigned char* input_file, unsigned int n){
	Bwt_Aggregate bwt_stage_data = bwt(input_file, n);
	
	unsigned char alphabet[256];
	for(unsigned int i=0; i<256; i++) alphabet[i] = i;

	move_to_front(bwt_stage_data.file, n, alphabet, 256);
	Rle_Aggregate rle_stage_data = run_length_encoding_i(bwt_stage_data.file, n);
	free(bwt_stage_data.file);


	unsigned char* huffman_array_1 = rle_stage_data.output+(rle_stage_data.array_1_n+7)/8;
	unsigned int huffman_buffer_1_n = (rle_stage_data.array_2_n+1)/2;
	unsigned char* huffman_buffer_1 = malloc(huffman_buffer_1_n*sizeof(unsigned char));
	
	
	long long num_of_bits_1 = 0;
	Huffman_Tree ht1 = huffman_encode(huffman_array_1, rle_stage_data.array_2_n, 4, huffman_buffer_1, &num_of_bits_1);
	
	unsigned char* huffman_array_2 = huffman_array_1 +(rle_stage_data.array_2_n+1)/2;
	unsigned int huffman_buffer_2_n= (rle_stage_data.array_3_n +3)/4;
	unsigned char* huffman_buffer_2 = malloc(huffman_buffer_2_n*sizeof(unsigned char));
	
	
	long long num_of_bits_2 = 0;
	Huffman_Tree ht2 = huffman_encode(huffman_array_2, rle_stage_data.array_3_n, 2, huffman_buffer_2, &num_of_bits_2);

	unsigned char* huffman_array_3 = huffman_array_2 + (rle_stage_data.array_3_n+3)/4;
	unsigned int huffman_buffer_3_n= rle_stage_data.array_1_n;
	unsigned char* huffman_buffer_3 = malloc(huffman_buffer_3_n*sizeof(unsigned char));
	
	
	long long num_of_bits_3 = 0;
	Huffman_Tree ht3 = huffman_encode(huffman_array_3, rle_stage_data.array_1_n, 8, huffman_buffer_3, &num_of_bits_3);
	
	Flattened_Tree ft1 = flatten_tree(ht1); 
	Flattened_Tree ft2 = flatten_tree(ht2); 
	Flattened_Tree ft3 = flatten_tree(ht3); 
	
	unsigned int total_meta_data_size = 7*sizeof(unsigned int) + 3*sizeof(long long) + (ft1.num_of_nodes+7)/8 + (ft2.num_of_nodes+7)/8 + (ft3.num_of_nodes+7)/8 + (ft1.num_of_nodes-1)/2 + (ft2.num_of_nodes-1)/2 + (ft3.num_of_nodes-1)/2;

	unsigned char* meta_data = malloc(total_meta_data_size*sizeof(unsigned char));
	unsigned int total = 0;

	copy_tree(ft1, &num_of_bits_1, meta_data, &total);
	copy_tree(ft2, &num_of_bits_2, meta_data, &total);
	copy_tree(ft3, &num_of_bits_3, meta_data, &total);

	memcpy(meta_data, &(bwt_stage_data.sentinel_position), sizeof(unsigned int));
	memcpy(meta_data, &(rle_stage_data.array_1_n), sizeof(unsigned int));
	memcpy(meta_data, &(rle_stage_data.array_2_n), sizeof(unsigned int));
	memcpy(meta_data, &(rle_stage_data.array_3_n), sizeof(unsigned int));
	
	unsigned int total_data_size = (rle_stage_data.array_1_n+7)/8 + (num_of_bits_1 + 7)/8 + (num_of_bits_2 + 7)/8 +(num_of_bits_3 + 7)/8;
	unsigned char* compressed_data = malloc(total_data_size*sizeof(unsigned char));
	unsigned char* temp_addr = compressed_data;

	memcpy(temp_addr, rle_stage_data.output, (rle_stage_data.array_1_n+7)/8);
	temp_addr += (rle_stage_data.array_1_n+7)/8;
	
	memcpy(temp_addr, huffman_buffer_1, (num_of_bits_1+7)/8);
	temp_addr += (num_of_bits_1+7)/8;
	memcpy(temp_addr, huffman_buffer_2, (num_of_bits_2+7)/8);
	temp_addr += (num_of_bits_2+7)/8;
	memcpy(temp_addr, huffman_buffer_3, (num_of_bits_3+7)/8);
	
	free(huffman_buffer_1);
	free(huffman_buffer_2);
	free(huffman_buffer_3);
	
	Compressed_File result;
	result.meta_data = meta_data;
	result.data = compressed_data;
	result.meta_data_size = total_meta_data_size;
	result.data_size = total_data_size;

	return result;

}
