

#include <stdio.h>
#include <immintrin.h>

__m256d avx_abs_pd(__m256d x) {
	return _mm256_max_pd(_mm256_sub_pd(_mm256_setzero_pd(), x), x);
}

void print(__m256d a) {
	double* res = (double*) &a;
	printf("%f %f %f %f\n",
			res[0], res[1], res[2], res[3]);
}
