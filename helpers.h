
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
__m256d avx_abs_pd(__m256d x);


/**
 * Prints double intrinsics vector
 * @param a
 */
void print(__m256d a);

#endif /* HELPERS_H */

