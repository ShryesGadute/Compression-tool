#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "huffman.h"
#include "../../bit_man.h"

#define DNE 0x8000

typedef unsigned char (*read_num_fn)(unsigned char* , unsigned int);
typedef void (*write_num_fn)(unsigned char* , unsigned int, unsigned char);


read_num_fn select_read_num_fn(unsigned char bit_length){
	switch(bit_length){
		case 1: return read_bit;
		case 2: return read_bat;
		case 4: return read_quad;
		case 8: return read_byte;
		default: return NULL;
	}
}

write_num_fn select_write_num_fn(unsigned char bit_length){
	switch(bit_length){
		case 1: return write_bit;
		case 2: return write_bat;
		case 4: return write_quad;
		case 8: return write_byte;
		default: return NULL;
	}
}


typedef struct{
	unsigned char code_length;
	unsigned char* bit_array;
}Symbol_Code;
	
void heapify_down(unsigned short* heap, unsigned short heap_size, unsigned short index, Huffman_Node* node_list){
	unsigned short left_child = 2*index+1;
	unsigned short right_child = 2*index+2;
	unsigned short min_index = index;
	unsigned short temp_store_i;

	if(left_child < heap_size && node_list[heap[left_child]].frequency < node_list[heap[min_index]].frequency) min_index = left_child;
	if(right_child < heap_size && node_list[heap[right_child]].frequency < node_list[heap[min_index]].frequency) min_index = right_child;

	if(min_index != index){
		//swap
		temp_store_i = heap[min_index];
		heap[min_index] = heap[index];
		heap[index] = temp_store_i;

		heapify_down(heap, heap_size, min_index, node_list);
	}
}

void heapify_up(unsigned short* heap, unsigned short index, Huffman_Node* node_list){
	if(index != 0){
	unsigned short child = index;
	unsigned short parent = (index-1)/2;
	unsigned short temp_store_i;
	if(node_list[heap[parent]].frequency > node_list[heap[child]].frequency){
		//swap
		temp_store_i = heap[parent];
		heap[parent] = heap[child];
		heap[child] = temp_store_i;
		heapify_up(heap, parent, node_list);
	}
	}
}

unsigned short pop_min(unsigned short* heap, Huffman_Node* node_list, unsigned short* heap_size){
	unsigned short min;
	min = heap[0];
	heap[0] = heap[(*heap_size)-1];
	(*heap_size)--;

	heapify_down(heap, *heap_size, 0, node_list);

	return min;
}

void make_min_heap(unsigned short* heap, unsigned short heap_size, Huffman_Node* node_list){
	short i;
	for(i=(heap_size-2)/2; i>=0; i--) heapify_down(heap, heap_size, i, node_list);
}

void add_to_heap(unsigned short* heap, unsigned short val, Huffman_Node* node_list, unsigned short* heap_size){
	heap[*heap_size] = val;
	(*heap_size)++;
	heapify_up(heap, (*heap_size)-1, node_list);
}


unsigned int* count_frequency(unsigned char* input, unsigned int n, unsigned char bit_length){
	unsigned int* frequencies = malloc((1u<<bit_length)*sizeof(unsigned int));
	memset(frequencies, 0, (1u<<bit_length)*sizeof(unsigned int));
	unsigned char (*read_num)(unsigned char*, unsigned int) = select_read_num_fn(bit_length);
	
	unsigned char num;
	for(unsigned int i=0; i<n; i++){
		num = read_num(input, i);
		frequencies[num]++;
	}

	return frequencies;
}


Huffman_Tree get_huffman_tree(unsigned int* frequency, unsigned int k){
	unsigned int i;
	unsigned short unique_chr=0;
	for(i=0; i<k; i++) if(frequency[i]>0) unique_chr++;

	Huffman_Tree ht;
	ht.node_list = malloc((2*unique_chr)*sizeof(Huffman_Node));
	unsigned short allocated = 0;

	unsigned short heap_size=0;
	unsigned short* heap = malloc(unique_chr*sizeof(unsigned short));

	for(i=0; i<k;i++){
		if(frequency[i] > 0){
			//create new node
			ht.node_list[allocated].symbol = i;
			ht.node_list[allocated].frequency = frequency[i];
			ht.node_list[allocated].left_child = DNE;
			ht.node_list[allocated].right_child = DNE;
			
			//put new node index in heap
			heap[heap_size] = allocated;
			heap_size++;
			allocated++;
		}
	}
	
	make_min_heap(heap, heap_size, ht.node_list);

	unsigned short min1_index;
	unsigned short min2_index;
	unsigned short new_node_index;
	while(heap_size != 1){
		min1_index = pop_min(heap, ht.node_list, &heap_size);
		min2_index = pop_min(heap, ht.node_list, &heap_size);
		
		//create new node and add it to heap
		new_node_index = allocated++;
		ht.node_list[new_node_index].frequency = ht.node_list[min1_index].frequency + ht.node_list[min2_index].frequency;
		ht.node_list[new_node_index].left_child = min1_index;
		ht.node_list[new_node_index].right_child = min2_index;

		add_to_heap(heap, new_node_index, ht.node_list, &heap_size);
	}
	
	ht.num_of_nodes = allocated;
	ht.root = ht.node_list + heap[0];
	return ht;
}

Symbol_Code* get_codes_table(Huffman_Tree ht, unsigned int bit_length){
	unsigned short stack[512];
unsigned short stack_length = 0;
	unsigned char code[32];
	unsigned char code_size;
	Symbol_Code* table = malloc((1<<bit_length)*sizeof(Symbol_Code));

	for(unsigned short i=0; i<(1<<bit_length); i++) table[i].code_length = 0;

	stack[2*stack_length] = ht.root->right_child + 32768;
	stack[2*stack_length+1] = 0;
	stack_length++;
	stack[2*stack_length] = ht.root->left_child;
	stack[2*stack_length+1] = 0;
	stack_length++;

	unsigned short current_node;
	while(stack_length != 0){
		stack_length--;
		current_node = stack[2*stack_length];
		code_size = stack[2*stack_length+1];
		if(current_node >= 32768){
			write_bit(code, code_size, 1);
			code_size++;
			current_node -= 32768;
		}
		else{
			write_bit(code, code_size, 0);
			code_size++;
		}

		if(ht.node_list[current_node].left_child != DNE){
			stack[2*stack_length] = ht.node_list[current_node].right_child + 32768;
			stack[2*stack_length+1] = code_size;
			stack_length++;
			stack[2*stack_length] = ht.node_list[current_node].left_child;
			stack[2*stack_length+1] = code_size;
			stack_length++;

		}

		else{
			table[ht.node_list[current_node].symbol].code_length = code_size;
			table[ht.node_list[current_node].symbol].bit_array = malloc(((code_size+7)/8)*sizeof(unsigned char));
			for(unsigned int i=0; i<(code_size+7)/8; i++) table[ht.node_list[current_node].symbol].bit_array[i] = code[i];
		}
	}
	return table;
}

Huffman_Tree huffman_encode(unsigned char* input, unsigned int n, unsigned int bit_length, unsigned char* output, long long* k){
	unsigned int* frequency = count_frequency(input, n, bit_length);
	Huffman_Tree ht = get_huffman_tree(frequency, (1<<bit_length));
	Symbol_Code* table = get_codes_table(ht, bit_length);
	unsigned char count;
	long long filled = 0;
	unsigned char curr;
	unsigned char (*read_num)(unsigned char*, unsigned int) = select_read_num_fn(bit_length);
	for(unsigned int i=0; i<n; i++){
		curr = read_num(input, i);	
		count = table[curr].code_length;
		for(unsigned char j=0; j<count; j++){
			if(read_bit(table[curr].bit_array, j)) write_bit(output, filled, 1);
			else write_bit(output, filled, 0);
			filled++;
		}
	}
	*k = filled;

	return ht;
}

unsigned int huffman_decode(unsigned char* input, unsigned char* output, long long n_bits, Huffman_Tree ht, unsigned char bit_length){
	Huffman_Node* current_node = ht.root;
	void (*write_num)(unsigned char*, unsigned int, unsigned char) = select_write_num_fn(bit_length);
	unsigned int i_1=0;
		
	for(long long i=0; i<n_bits; i++){
		if(current_node->left_child == DNE){
			write_num(output, i_1, current_node->symbol);
			i_1++;
			current_node = ht.root;
		}
		
		if(read_bit(input, i)) current_node = ht.node_list + current_node->right_child;
		else current_node = ht.node_list + current_node->left_child;
	}
	
	write_num(output, i_1, current_node->symbol);
	i_1++;

	return i_1;
	
}























