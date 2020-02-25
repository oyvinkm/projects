/* Find, in parallel, those random integers that are divisible by two
   given numbers and print them to the screen. */

#include "transducers.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void random_garbage(const void *arg, FILE *out) {
	int len = *(int*)arg;

	for (int i = 0; i < len; i++) {
		int x = rand();
		fwrite(&x, sizeof(int), 1, out);
	}
}


void link_1(const void *arg, FILE *out, FILE *in) {
	arg=arg; // Unused
	in=in; // Unused
	out=out; // Unused
}

void link_2(const void *arg, FILE *out, FILE *in1, FILE *in2) {
	arg=arg; // Unused
	in1=in1; // Unused
	in2=in2; // Unused
	out=out; //Unused
}

void sink(void *arg, FILE *in) {
	arg=arg;
	in=in;
}

int main() {
	stream* s[8];

	int len = 10;
	int two = 4;

	char *output = (char*)malloc(sizeof(char*));

	// Do not read from stream already read

	// Prepares
	assert(transducers_link_source(&s[0], random_garbage, &len) == 0);
	assert(transducers_dup(&s[1], &s[2], s[0]) == 0);

	// Test
	assert(transducers_dup(&s[3], &s[4], s[0]) == -1);
	assert(transducers_link_1(&s[5], link_1, &two, s[0]) == -1);
	assert(transducers_link_2(&s[6], link_2, NULL, s[5], s[0]) == -1);
	assert(transducers_link_2(&s[7], link_2, NULL, s[0], s[6]) == -1);
	assert(transducers_link_sink(sink, output, s[0]) == -1);

	// Note the sizeof()-trick to determine the number of elements in
	// the array.  This *only* works for statically allocated arrays,
	// *not* ones created by malloc().
	for (int i = 0; i < 3; i++) {
		transducers_free_stream(s[i]);
	}

	free(output);

	return 0;
}
