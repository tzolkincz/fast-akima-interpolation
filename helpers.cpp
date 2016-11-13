

#include <stdio.h>
#include <immintrin.h>


void print(__m256d a) {
	double* res = (double*) &a;
	printf("%f %f %f %f\n",
			res[0], res[1], res[2], res[3]);
}

//
// Vector helpers
//

inline __m256d _mm256_rotate_left_pd(__m256d a) {
	return _mm256_permute4x64_pd(a, 0b00111001);
}

inline __m256d _mm256_rotate_right_pd(__m256d a) {
	return _mm256_permute4x64_pd(a, 0b10010011);
}

inline __m256d _mm256_reverse_pd(__m256d a) {
	return _mm256_permute4x64_pd(a, 0b00011011);
}

inline __m256d _mm256_shiftl_and_load_next_pd(__m256d toShift, __m256d toLoadNext) {
	return _mm256_blend_pd(_mm256_rotate_left_pd(toShift), _mm256_reverse_pd(toLoadNext), 0b1000);
}

inline __m256d _mm256_shiftr_and_load_next_pd(__m256d toLoadNext, __m256d toShift) {
	return _mm256_blend_pd(_mm256_reverse_pd(toLoadNext), _mm256_rotate_right_pd(toShift), 0b1110);
}
