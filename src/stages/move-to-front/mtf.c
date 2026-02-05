#include <stdlib.h>
#include <stdio.h>
#include "mtf.h"

//k <= 256 -> this condition should always hold
unsigned char* make_linked_list(unsigned char* initial_alphabet, unsigned int k){
	unsigned char* alphabet = malloc(257*sizeof(unsigned char)); // +1 is to keep track of the staring node of the linked list
	unsigned char curr_node, next_node;
	curr_node = initial_alphabet[0];
	alphabet[256] = curr_node; //last byte stores the index(or address) of the first element of the linked list
	for(unsigned int i=0; i<k-1; i++){
		next_node = initial_alphabet[i+1];
		alphabet[curr_node] = next_node;
		curr_node = next_node;
	}

	alphabet[curr_node] = curr_node; //last node of the linked list points to itself

	return alphabet;
}

//finds the index of character and moves that node to the front
unsigned char find_and_move(unsigned char* alphabet, unsigned char character){
	unsigned char index = 0;
	unsigned char curr_character, prev_character;
	curr_character = alphabet[256];

	while(character != curr_character){
		prev_character = curr_character;
		curr_character = alphabet[curr_character];
		index++;
	}
	
	if(index != 0){
		alphabet[prev_character] = alphabet[curr_character];
		alphabet[curr_character] = alphabet[256];
		alphabet[256] = curr_character;
	}

	return index;
}

//
unsigned char jump_and_move(unsigned char* alphabet, unsigned char index){
	unsigned char jump = 0;
	unsigned char curr_character, prev_character;
	curr_character = alphabet[256];

	while(jump != index){
		prev_character = curr_character;
		curr_character = alphabet[curr_character];
		jump++;
	}
	
	if(index != 0){
		alphabet[prev_character] = alphabet[curr_character];
		alphabet[curr_character] = alphabet[256];
		alphabet[256] = curr_character;
	}

	return curr_character;
}


void print_list(unsigned char* ll, unsigned int k){
	unsigned char start = ll[256];
	for(unsigned int i=0; i<k; i++){
		printf("%c", start);
		start = ll[start];
	}
}

void move_to_front(unsigned char* input, unsigned int n, unsigned char* initial_alphabet, unsigned int k){
	unsigned char* alphabet = make_linked_list(initial_alphabet, k);
	unsigned int i=0;
	unsigned char index;

	for(i=0; i<n;i++){
		index = find_and_move(alphabet, input[i]);
		input[i] = index;
	}
	
	free(alphabet);
}

void reverse_move_to_front(unsigned char* mtf_input, unsigned int n, unsigned char* initial_alphabet, unsigned int k){
	unsigned char* alphabet = make_linked_list(initial_alphabet, k);
	unsigned int i=0;
	unsigned char character;

	for(i=0; i<n; i++){
		character = jump_and_move(alphabet, mtf_input[i]);
		mtf_input[i] = character;
	}
}


