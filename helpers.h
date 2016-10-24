
/*
 * File:   helpers.h
 * Author: v
 *
 * Created on 19. října 2016, 12:59
 */

#include <immintrin.h>

#ifndef HELPERS_H
#define HELPERS_H


/**
 * Compute abs value of double intrinsic vector
 * @param x
 * @return
 */
inline __m256d _mm256_abs_pd(__m256d x) {
	return _mm256_max_pd(_mm256_sub_pd(_mm256_setzero_pd(), x), x);
}

/**
 * Prints double intrinsics vector
 * @param a
 */
void print(__m256d a);

#endif /* HELPERS_H */

