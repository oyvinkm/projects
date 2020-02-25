#include "transducers.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void string_stream(const void *arg, FILE *out) {
	fputs((const char*) arg, out);
}

void and_streams(const void *arg, FILE *out, FILE *in1, FILE *in2) {
	arg=arg; // Unused
	char x, y;
	while ((fread(&x, sizeof(char), 1, in1) == 1) &&
	       (fread(&y, sizeof(char), 1, in2) == 1)) {
		char z = x;
		fwrite(&z, sizeof(char), 1, out);
	}
}

void save_stream(void *arg, FILE *in) {
	/* We will be writing bytes to this location. */
	unsigned char *d = arg;

	while (fread(d, sizeof(unsigned char), 1, in) == 1) {
		d++; /* Move location ahead by one byte. */
	}
}

int main() {
	stream* s[4];

	char *input = "Hello, World!";
	char *output = malloc(strlen(input)+1);
	output[strlen(input)] = '\0'; /* Ensure terminating NULL. */

	assert(transducers_link_source(&s[0], string_stream, input) == 0);
	assert(transducers_dup(&s[1], &s[2], s[0]) == 0);
	assert(transducers_link_2(&s[3], and_streams, NULL, s[1], s[2]) == 0);
	assert(transducers_link_sink(save_stream, output, s[3]) == 0);

	/* We cannot use the '==' operator for comparing strings, as strings
	   in C are just pointers.  Using '==' would compare the _addresses_
	   of the two strings, which is not what we want. */
	assert(strcmp("Hello, World!",output) == 0);

	/* Note the sizeof()-trick to determine the number of elements in
	   the array.  This *only* works for statically allocated arrays,
	 * not* ones created by malloc(). */
	for (int i = 0; i < (int)(sizeof(s)/sizeof(s[0])); i++) {
		transducers_free_stream(s[i]);
	}

	free(output);

	return 0;
}
