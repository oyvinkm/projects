#include "transducers.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void binary_stream(const void *arg, FILE *out) {
	FILE* source = fopen(arg, "rb");
	int numr;
	char* buffer = (char*)malloc(sizeof(char));
	while (feof(source) == 0)
	{
		numr = fread(buffer, sizeof(char), 1, source);
		fwrite(buffer, sizeof(char), numr, out);
	}
	free(buffer);
	fclose(source);
}

void save_binary(void *arg, FILE *in) {
	FILE* target = fopen(arg, "wb");
	int numr;
	char* buffer = (char*)malloc(sizeof(char));
	while (feof(in) == 0)
	{
		numr = fread(buffer, sizeof(char), 1, in);
		fwrite(buffer, sizeof(char),numr, target);
	}
	free(buffer);
	fclose(target);
}

int compare_binaries(char* source, char* target) {
	FILE* source_file = fopen(source, "rb");
	FILE* target_file = fopen(target, "rb");
	int ch1, ch2;
	do {
		ch1 = fgetc(source_file);
		ch2 = fgetc(target_file);

		if (ch1 != ch2) {
			return -1;
		}

	} while (ch1 != EOF || ch2 != EOF);
	fclose(target_file);
	fclose(source_file);
	return 0;
}

int main() {
	stream* s[1];
	char* source = "test3";
	char* target = "test3out";

	// Stream binary

	assert(transducers_link_source(&s[0], binary_stream, source) == 0);
	assert(transducers_link_sink(save_binary, target, s[0]) == 0);
	assert(compare_binaries(source, target) == 0);

	/* We cannot use the '==' operator for comparing strings, as strings
	   in C are just pointers.  Using '==' would compare the _addresses_
	   of the two strings, which is not what we want. */


	/* Note the sizeof()-trick to determine the number of elements in
	   the array.  This *only* works for statically allocated arrays,
	 * not* ones created by malloc(). */
	for (int i = 0; i < (int)(sizeof(s)/sizeof(s[0])); i++) {
		transducers_free_stream(s[i]);
	}
	return 0;
}
