

#include <cstdlib>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>


#include "helpers.h"
#include "fast_akima.h"

FastAkima::FastAkima() {

}

//static __inline void __attribute__((__always_inline__, __nodebug__))
//_mm256_storeu2_m128d(double const *addr_hi, double const *addr_lo, __m256d a)
//{
//  __m128d v128;
//
//  v128 = _mm256_castpd256_pd128(a);
//  __builtin_ia32_storeupd(addr_lo, v128);
//  v128 = _mm256_extractf128_pd(a, 1);
//  __builtin_ia32_storeupd(addr_hi, v128);
//}

double* FastAkima::interpolate(int count, double* xvals, double* yvals) {
	if (count < MINIMUM_NUMBER_POINTS) {
		return (double*) - 1;
	}

	double* differences = (double*) malloc(sizeof (double) * count);
	double* weights = (double*) malloc(sizeof (double) * count);

	double* coefsOfPolynFunc = (double*) malloc(sizeof (double) * 4 * count);
	double* firstDerivatives = &coefsOfPolynFunc[1 * count];

	//@TODO misto pole pro prvni derivaci rovnou pouzit koeficientni pole
	//@TODO zkusit jesete neco vyhnojit rovnou v prvni smycce - treba prvni koef.


	computeDiffsAndWeights(count, xvals, yvals, differences, weights, &coefsOfPolynFunc[0]);

	// Prepare Hermite interpolation scheme.

	computeFirstDerivates(count, xvals, differences, weights, firstDerivatives);

	free(differences);
	free(weights);

	computeHeadAndTailOfFirstDerivates(count, xvals, yvals, firstDerivatives);

	//interpolate hermite sorted
	computePolynCoefs(count, xvals, coefsOfPolynFunc);

	return coefsOfPolynFunc;
}

void FastAkima::computeDiffsAndWeights(int count, double* xvals, double* yvals, double* differences, double* weights, double* ysInternalCopy) {

	int numberOfDiffAndWeightElements = count - 1;

	//ysInternalCopy are first arguments of result polynoms

	//first iteration
	__m256d yvs = _mm256_load_pd(&yvals[0]);
	__m256d dPrev = _mm256_div_pd(
			_mm256_sub_pd(_mm256_load_pd(&yvals[1]), yvs),
			_mm256_sub_pd(_mm256_load_pd(&xvals[1]), _mm256_load_pd(&xvals[0])));

	_mm256_store_pd(&differences[0], dPrev);
	_mm256_store_pd(&ysInternalCopy[0], yvs);

	//next interations
	int i = SIMD_WIDTH;
	int weightIndex = 1;
	for (; i <= numberOfDiffAndWeightElements - SIMD_WIDTH; i += SIMD_WIDTH) {
		__m256d yvs = _mm256_load_pd(&yvals[i]);
		__m256d d = _mm256_div_pd(
				_mm256_sub_pd(_mm256_load_pd(&yvals[i + 1]), yvs),
				_mm256_sub_pd(_mm256_load_pd(&xvals[i + 1]), _mm256_load_pd(&xvals[i])));

		_mm256_store_pd(&differences[i], d);
		_mm256_store_pd(&ysInternalCopy[i], yvs);

		__m256d w = _mm256_load_pd(&differences[weightIndex]);
		_mm256_store_pd(&weights[weightIndex], avx_abs_pd(_mm256_sub_pd(w, dPrev)));

		dPrev = d;
		weightIndex += SIMD_WIDTH;
	}

	//process rest with scalar code
	for (i -= SIMD_WIDTH; i < numberOfDiffAndWeightElements; i++) {
		differences[i] = (yvals[i + 1] - yvals[i]) / (xvals[i + 1] - xvals[i]);
		ysInternalCopy[i] = yvals[i]; //yvs var at vector code
	}
	//set lastInternal copy
	ysInternalCopy[numberOfDiffAndWeightElements] = yvals[numberOfDiffAndWeightElements];

	for (; weightIndex < numberOfDiffAndWeightElements; weightIndex++) {
		weights[weightIndex] = fabs(differences[weightIndex] - differences[weightIndex - 1]);
	}
}

void FastAkima::computeFirstDerivates(int count, double* xvals, double* differences, double* weights, double* firstDerivatives) {
	__m256d zero = _mm256_setzero_pd();
	__m256i intTrue = _mm256_set1_epi32(0xFFFFFFFF);

	int i = 2;
	for (; i <= count - 2 - SIMD_WIDTH; i += SIMD_WIDTH) {
		__m256d wP = _mm256_load_pd(&weights[i + 1]);
		__m256d wM = _mm256_load_pd(&weights[i - 1]);

		__m256d dPrev = _mm256_load_pd(&differences[i - 1]);
		__m256d dCurr = _mm256_load_pd(&differences[i]);

		__m256d cond1 = _mm256_cmp_pd(wP, zero, _CMP_EQ_OQ);
		__m256d cond2 = _mm256_cmp_pd(wM, zero, _CMP_EQ_OQ);

		__m256d condAnd = _mm256_and_pd(cond1, cond2);
		if (_mm256_testz_pd(cond1, cond2)) {
			__m256d d = _mm256_add_pd(_mm256_mul_pd(wP, dPrev), _mm256_mul_pd(wM, dCurr));
			d = _mm256_div_pd(d, _mm256_add_pd(wP, wM));
			_mm256_store_pd(&firstDerivatives[i], d);
		} else {
			//compute special cases
			__m256d xv = _mm256_load_pd(&xvals[i]);
			__m256d xvP = _mm256_load_pd(&xvals[i + SIMD_WIDTH]);
			__m256d xvM = _mm256_load_pd(&xvals[i - SIMD_WIDTH]);

			__m256d d = _mm256_mul_pd(_mm256_sub_pd(xvP, xv), dPrev);
			d = _mm256_add_pd(d, _mm256_mul_pd(_mm256_sub_pd(xv, xvM), dCurr));
			d = _mm256_div_pd(d, _mm256_sub_pd(xvP, xvM));

			__m256i storeMask = _mm256_castsi128_si256(_mm256_cvtpd_epi32(condAnd));
			_mm256_maskstore_pd(&firstDerivatives[i], storeMask, d);

			//compute ok cases
			d = _mm256_add_pd(_mm256_mul_pd(wP, dPrev), _mm256_mul_pd(wM, dCurr));
			d = _mm256_div_pd(d, _mm256_add_pd(wP, wM));
			_mm256_maskstore_pd(&firstDerivatives[i], _mm256_andnot_si256(storeMask, intTrue), d);
		}
	}

	//process rest with scalar code
	for (i = std::max(2, i - SIMD_WIDTH); i < count - 2; i++) {
		double wP = weights[i + 1];
		double wM = weights[i - 1];
		if (FP_ZERO == fpclassify(wP) && FP_ZERO == fpclassify(wM)) {
			double xv = xvals[i];
			double xvP = xvals[i + 1];
			double xvM = xvals[i - 1];
			firstDerivatives[i] = (((xvP - xv) * differences[i - 1]) + ((xv - xvM) * differences[i])) / (xvP - xvM);
		} else {
			firstDerivatives[i] = ((wP * differences[i - 1]) + (wM * differences[i])) / (wP + wM);
		}
	}
}

void FastAkima::computeHeadAndTailOfFirstDerivates(int count, double* xvals, double* yvals, double* firstDerivatives) {
	// differentiateThreePoint
	__m256d x0 = _mm256_setr_pd(yvals[0], yvals[0], yvals[count - 3], yvals[count - 3]);
	__m256d x1 = _mm256_setr_pd(yvals[1], yvals[1], yvals[count - 2], yvals[count - 2]);
	__m256d x2 = _mm256_setr_pd(yvals[2], yvals[2], yvals[count - 1], yvals[count - 1]);

	__m256d firstSample = _mm256_setr_pd(xvals[0], xvals[0], xvals[count - 3], xvals[count - 3]);
	__m256d t0 = _mm256_setr_pd(xvals[0], xvals[1], xvals[count - 2], xvals[count - 1]);
	t0 = _mm256_sub_pd(t0, firstSample);

	__m256d t1 = _mm256_setr_pd(xvals[1], xvals[1], xvals[count - 2], xvals[count - 2]);
	t1 = _mm256_sub_pd(t1, firstSample);

	__m256d t2 = _mm256_setr_pd(xvals[2], xvals[2], xvals[count - 1], xvals[count - 1]);
	t2 = _mm256_sub_pd(t2, firstSample);

	__m256d x1MinusX0 = _mm256_sub_pd(x1, x0);
	__m256d x2MinusX0 = _mm256_sub_pd(x2, x0);
	__m256d t1Pow2 = _mm256_mul_pd(t1, t1);
	__m256d t2Pow2 = _mm256_mul_pd(t2, t2);
	__m256d t1MulT2 = _mm256_mul_pd(t1, t2);
	__m256d t2DivT1 = _mm256_div_pd(t2, t1);

	__m256d a = _mm256_sub_pd(x2MinusX0, _mm256_mul_pd(t2DivT1, x1MinusX0));
	a = _mm256_div_pd(a, _mm256_sub_pd(t2Pow2, t1MulT2));

	__m256d b = _mm256_sub_pd(x1MinusX0, _mm256_mul_pd(a, t1Pow2));
	b = _mm256_div_pd(b, t1);

	__m256d res = _mm256_mul_pd(_mm256_add_pd(a, a), t0);
	res = _mm256_add_pd(res, b);
	_mm256_storeu2_m128d(&firstDerivatives[count - 2], &firstDerivatives[0], res);
}

inline void FastAkima::computePolynCoefs(int count, double* xvals, double* coefsOfPolynFunc) {

	double* firstDerivatives = &coefsOfPolynFunc[1 * count];
	double* yvals = &coefsOfPolynFunc[0];


	int numberOfDiffAndWeightElements = count - 1;
	int dimSize = count;

	__m256d three = _mm256_set1_pd(3);

	int i;
	for (i = 0; i <= numberOfDiffAndWeightElements - SIMD_WIDTH; i += SIMD_WIDTH) {
		__m256d w = _mm256_sub_pd(_mm256_load_pd(&xvals[i + 1]), _mm256_load_pd(&xvals[i]));
		__m256d w2 = _mm256_mul_pd(w, w);

		__m256d yv = _mm256_load_pd(&yvals[i]); // equals to first coef
		__m256d yvNext = _mm256_load_pd(&yvals[i + 1]);

		__m256d fd = _mm256_load_pd(&firstDerivatives[i]); // equals to second coef
		__m256d fdNext = _mm256_load_pd(&firstDerivatives[i + 1]);

		__m256d yvMinusYvNext = _mm256_sub_pd(yv, yvNext);
		__m256d yvNextMinuxYv = _mm256_sub_pd(yvNext, yv);
		__m256d fdPlusFdNext = _mm256_add_pd(fd, fdNext);

		__m256d coef3 = _mm256_div_pd(_mm256_mul_pd(three, yvNextMinuxYv), w);
		coef3 = _mm256_sub_pd(coef3, _mm256_add_pd(fd, fd));
		coef3 = _mm256_div_pd(_mm256_sub_pd(coef3, fdNext), w);

		__m256d coef4 = _mm256_div_pd(_mm256_add_pd(yvMinusYvNext, yvMinusYvNext), w);
		coef4 = _mm256_add_pd(coef4, fdPlusFdNext);
		coef4 = _mm256_div_pd(coef4, w2);

		_mm256_store_pd(&coefsOfPolynFunc[2 * dimSize + i], coef3);
		_mm256_store_pd(&coefsOfPolynFunc[3 * dimSize + i], coef4);
	}

	for (i -= SIMD_WIDTH; i < numberOfDiffAndWeightElements; i++) {
		double w = xvals[i + 1] - xvals[i];
		double w2 = w * w;

		double yv = yvals[i];
		double yvP = yvals[i + 1];

		double fd = firstDerivatives[i];
		double fdP = firstDerivatives[i + 1];

		coefsOfPolynFunc[2 * dimSize + i] = (3 * (yvP - yv) / w - 2 * fd - fdP) / w;
		coefsOfPolynFunc[3 * dimSize + i] = (2 * (yv - yvP) / w + fd + fdP) / w2;
	}
}

