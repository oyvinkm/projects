/* Find, in parallel, those random integers that are divisible by two
   given numbers and print them to the screen. */

#include "transducers.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

void random_garbage(const void *arg, FILE *out) {
	int len = *(int*)arg;

	for (int i = 0; i < len; i++) {
		int x = rand();
		fwrite(&x, sizeof(int), 1, out);
	}
}

void divisible_by(const void *arg, FILE *out, FILE *in) {
	int d = *(int*)arg;

	int x;
	while (fread(&x, sizeof(int), 1, in) == 1) {
		int y = (x % d) == 0;
		fwrite(&y, sizeof(int), 1, out);
	}
}

void and_streams(const void *arg, FILE *out, FILE *in1, FILE *in2) {
	arg=arg; // Unused
	int x, y;
	while ((fread(&x, sizeof(int), 1, in1) == 1) &&
	       (fread(&y, sizeof(int), 1, in2) == 1)) {
		int z = x && y;
		fwrite(&z, sizeof(int), 1, out);
	}
}

void filter_streams(const void *arg, FILE *out, FILE *in1, FILE *in2) {
	arg=arg; // Unused
	int x, y;
	while ((fread(&x, sizeof(int), 1, in1) == 1) &&
	       (fread(&y, sizeof(int), 1, in2) == 1)) {
		if (x) {
			fwrite(&y, sizeof(int), 1, out);
		}
	}
}

void print_stream(void *arg, FILE *in) {
	arg=arg; // Unused
	int x;
	while (fread(&x, sizeof(int), 1, in) == 1) {
		printf("%d\n", x);
	}
}

int main() {
	stream* s[17];

	int len = 10;

	// Do not output stream to the same as input

	//Prepare streams
	assert(transducers_link_source(&s[0], random_garbage, &len) == 0);
	assert(transducers_dup(&s[2], &s[1], s[0]) == 0);
	assert(transducers_dup(&s[4], &s[3], s[1]) == 0);
	assert(transducers_dup(&s[6], &s[5], s[2]) == 0);
	assert(transducers_dup(&s[8], &s[7], s[3]) == 0);
	assert(transducers_dup(&s[10], &s[9], s[4]) == 0);
	assert(transducers_dup(&s[12], &s[11], s[5]) == 0);
	assert(transducers_dup(&s[14], &s[13], s[6]) == 0);
	assert(transducers_dup(&s[16], &s[15], s[7]) == 0);

	// Tests
	assert(transducers_dup(&s[9], &s[8], s[8]) == -1);
	assert(transducers_dup(&s[10], &s[11], s[10]) == -1);

	assert(transducers_link_1(&s[12], divisible_by, &len, s[12]) == -1);

	assert(transducers_link_2(&s[13], filter_streams, NULL, s[14], s[13]) == -1);
	assert(transducers_link_2(&s[16], filter_streams, NULL, s[16], s[15]) == -1);

	// Note the sizeof()-trick to determine the number of elements in
	// the array.  This *only* works for statically allocated arrays,
	// *not* ones created by malloc().
	for (int i = 0; i < (int)(sizeof(s)/sizeof(s[0])); i++) {
		transducers_free_stream(s[i]);
	}

	return 0;
}
