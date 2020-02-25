/* Find, in parallel, those random integers that are divisible by two
   given numbers and print them to the screen. */

#include "transducers.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void empty(const void *arg, FILE *out) {
	arg=arg;
	out=out;
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
	stream* s[3];

	int len = 10;
	int two = 4;

	char *output = (char*)malloc(sizeof(char*));

	// Input NULL streams

	// Prepare
	assert(transducers_link_source(&s[0], empty, &len) == 0);
	assert(transducers_link_source(&s[1], empty, &len) == 0);

	// Test
	assert(transducers_dup(&s[0], &s[0], s[2]) == -1);
	assert(transducers_link_1(&s[0], link_1, &two, s[2]) == -1);
	assert(transducers_link_2(&s[0], link_2, NULL, s[2], s[0]) == -1);
	assert(transducers_link_2(&s[0], link_2, NULL, s[0], s[2]) == -1);
	assert(transducers_link_sink(sink, output, s[2]) == -1);

	// Note the sizeof()-trick to determine the number of elements in
	// the array.  This *only* works for statically allocated arrays,
	// *not* ones created by malloc().
	for (int i = 0; i < 2; i++) {
		transducers_free_stream(s[i]);
	}

	free(output);

	return 0;
}
