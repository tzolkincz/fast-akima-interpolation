

#include <stdio.h>
#include <immintrin.h>


void print(__m256d a) {
	double* res = (double*) &a;
	printf("%f %f %f %f\n",
			res[0], res[1], res[2], res[3]);
}


void printi(__m128i a) {
	int* res = (int*) &a;
	printf("%d %d %d %d\n",
			res[0], res[1], res[2], res[3]);
}
