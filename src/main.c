#include <stdio.h>
#include <stdlib.h>
#include "pipeline/pipeline.h"


int main(){
	size_t capacity = 1024;
	size_t size = 0;
	unsigned char *buffer = malloc(capacity);

	if(!buffer) return 1;

	int ch;
	while((ch=fgetc(stdin)) != EOF){
		if(size >= capacity){
			capacity *= 2;
			buffer = realloc(buffer, capacity);
			if(!buffer) return 1;
		}
		buffer[size++] = (unsigned char)ch;
	}

	Compressed_File compressed = compress(buffer, size);
	printf("\noriginal   size = %d", size);
	printf("\ncompressed size = %d", (compressed.meta_data_size+compressed.data_size));
	printf("\ncompressed %    = %f\n", (float)((size-(compressed.data_size+compressed.meta_data_size))*100/(float)size));

	FILE *out = fopen("output.comp", "wb");
	fwrite(&(compressed.meta_data_size), sizeof(compressed.meta_data_size), 1, out);
	fwrite(compressed.meta_data, 1, compressed.meta_data_size, out);
	fwrite(&(compressed.data_size), sizeof(compressed.data_size), 1, out);
	fwrite(compressed.data, 1, compressed.data_size, out);

	free(buffer);
	return 0;	
}
