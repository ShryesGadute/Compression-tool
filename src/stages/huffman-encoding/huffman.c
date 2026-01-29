#include <stdlib.h>
#include <stdbool.h>
#include "huffman.h"
#include "../../bit_man.h"

#define DNE 0x8000
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

Huffman_Tree get_huffman_tree(unsigned char* input, unsigned int n){
	unsigned int i;
	unsigned int frequency[256] = {0};

	for(i=0; i<n; i++) frequency[input[i]]++;
	
	unsigned short unique_chr=0;
	for(i=0; i<256; i++){
		if(frequency[i]>0) unique_chr++;
	}

	Huffman_Tree ht;
	ht.node_list = malloc((2*unique_chr)*sizeof(Huffman_Node));
	unsigned short allocated = 0;

	unsigned short heap_size=0;
	unsigned short* heap = malloc(unique_chr*sizeof(unsigned short));

	for(i=0; i<256; i++){
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
	
	ht.root = ht.node_list + heap[0];
	return ht;
}

Symbol_Code* get_codes_table(Huffman_Tree ht){
	unsigned short stack[512];
	unsigned short stack_length = 0;
	unsigned char code[32];
	unsigned char code_size;
	Symbol_Code* table = malloc(256*sizeof(Symbol_Code));

	for(unsigned short i=0; i<256; i++) table[i].code_length = 0;

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
			reset_bit(code, code_size);
			code_size++;
			current_node -= 32768;
		}
		else{
			set_bit(code, code_size);
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

Symbol_Code* get_code_table(Huffman_Tree ht){
	unsigned short stack[256];
	unsigned short stack_length = 0;
	unsigned char code[32];
	unsigned char code_size=0;
	Symbol_Code* table = malloc(256*sizeof(Symbol_Code));
	stack[stack_length++] = (ht.root-ht.node_list);

	unsigned short current_node;
	while(stack_length != 0){
		current_node = stack[--stack_length]; //pop stack
		if(current_node >= 32768){
			code_size = stack_length;+1;
			reset_bit(code, code_size-1);
			current_node -= 32768;
		}
	
		if(ht.node_list[current_node].right_child == DNE){
			//save
			table[ht.node_list[current_node].symbol].code_length = code_size;
			table[ht.node_list[current_node].symbol].bit_array = malloc(((code_size+7)/8)*sizeof(unsigned char));
			for(unsigned int i=0; i<(code_size+7)/8; i++) table[ht.node_list[current_node].symbol].bit_array[i] = code[i];
		}
		
		else{
			set_bit(code, code_size);
			code_size++;
			stack[stack_length++] = ht.node_list[current_node].right_child + 32768;
			stack[stack_length++] = ht.node_list[current_node].left_child;
		}
	}

	return table;
}

void travel(Huffman_Node* node_list, unsigned short index){
	if(node_list[index].left_child == DNE) ("%c", node_list[index].symbol);
	("%d ", node_list[index].frequency);

	if(node_list[index].left_child != DNE){
		travel(node_list, node_list[index].left_child);
		travel(node_list, node_list[index].right_child);
	}
}

float bit_per_character(unsigned short root, Huffman_Node* node_list, unsigned char depth, unsigned int n){
	if(node_list[root].left_child == DNE){
		return ((float)depth * node_list[root].frequency)/n;
	}
	else{
		float b1 = bit_per_character(node_list[root].left_child, node_list, depth+1, n);
		float b2 = bit_per_character(node_list[root].right_child, node_list, depth+1, n);
		return b1+b2;
	}
}

void print_codes(unsigned char* input ,unsigned int n){
	Huffman_Tree ht = get_huffman_tree(input, n);
	travel(ht.node_list, ht.root-ht.node_list);
	("\n");
	Symbol_Code* table = get_codes_table(ht);
	float bits_per_char = bit_per_character(ht.root-ht.node_list, ht.node_list, 0, n);
	for(unsigned int i=0; i<256; i++){
		if(table[i].code_length > 0){
			("%c\t", i);
			for(unsigned int j=0; j<table[i].code_length; j++){
				bool bit = get_bit(table[i].bit_array, j);
				("%d", bit);
			}
			("\n");
		}
	}
}

Huffman_Tree huffman_encode(unsigned char* input, unsigned int n, unsigned char* output, unsigned int* k){
	Huffman_Tree ht = get_huffman_tree(input, n);
	Symbol_Code* table = get_codes_table(ht);
	unsigned char count;
	long filled = 0;

	for(unsigned int i=0; i<n; i++){
		count = table[input[i]].code_length;
		for(unsigned char j=0; j<count; j++){
			if(get_bit(table[input[i]].bit_array, j)) set_bit(output, filled);
			else reset_bit(output, filled);
			filled++;
		}
	}
	*k = (filled+7)/8;

	return ht;
}





