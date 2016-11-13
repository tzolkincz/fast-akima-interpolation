
#include <immintrin.h>

#ifndef HELPERS_H
#define HELPERS_H


/**
 * Prints double intrinsics vector
 * @param a
 */
void print(__m256d a);

/**
 * Perform left rotation of packed double values vector
 *
 * @param a
 * @return
 */
inline __m256d _mm256_rotate_left_pd(__m256d a) {
	return _mm256_permute4x64_pd(a, 0b00111001);
}

/**
 * Perform right rotation of packed double values vector
 *
 * @param a
 * @return
 */
inline __m256d _mm256_rotate_right_pd(__m256d a) {
	return _mm256_permute4x64_pd(a, 0b10010011);
}

/**
 * Returns vector with reversed items
 *
 * @param a
 * @return
 */
inline __m256d _mm256_reverse_pd(__m256d a) {
	return _mm256_permute4x64_pd(a, 0b00011011);
}

/**
 * Perform left shift and load next element from second vector
 *
 * @param toShift vector to be shifter
 * @param toLoadNext defines new values which will be put on empty space after shift
 * @return
 */
inline __m256d _mm256_shiftl_and_load_next_pd(__m256d toShift, __m256d toLoadNext) {
	return _mm256_blend_pd(_mm256_rotate_left_pd(toShift), _mm256_reverse_pd(toLoadNext), 0b1000);
}

/**
 * Perform right shift and load next element from second vector
 *
 * @param toLoadNext defines new values which will be put on empty space after shift
 * @param toShift vector to be shifter
 * @return
 */
inline __m256d _mm256_shiftr_and_load_next_pd(__m256d toLoadNext, __m256d toShift) {
	return _mm256_blend_pd(_mm256_reverse_pd(toLoadNext), _mm256_rotate_right_pd(toShift), 0b1110);
}

/**
 * Compute abs value of double intrinsic vector
 * @param x
 * @return
 */
inline __m256d _mm256_abs_pd(__m256d x) {
	return _mm256_max_pd(_mm256_sub_pd(_mm256_setzero_pd(), x), x);
}

#endif /* HELPERS_H */

