

#include <cstdlib>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>


#include "helpers.h"
#include "fast_akima.h"


const __m256d zero = _mm256_setzero_pd();
const __m256i intTrue = _mm256_set1_epi32(0xFFFFFFFF);
const __m256d MINUS_THREE_PD = _mm256_set1_pd(-3);
const __m256d TWO_PD = _mm256_set1_pd(2);

FastAkima::FastAkima() {

}

void FastAkima::computeRestCoefsScalar(int count, int fdStoreIndex, double* coefsOfPolynFunc, double* xvals, double* yvals) {

	fdStoreIndex--;
	int quantity = count - fdStoreIndex; //count of elements processed by scalar code
	int offset = fdStoreIndex; //from where starts scalar code

	// array of max 15 elements - allocate on stack
	double differences[quantity];
	double weights[quantity];

	double* firstDerivatives = &coefsOfPolynFunc[count];

	for (int i = fdStoreIndex; i < count - 1; i++) {
		differences[i - offset] = (yvals[i + 1] - yvals[i]) / (xvals[i + 1] - xvals[i]);
	}

	for (int i = fdStoreIndex; i < count - 1; i++) {
		weights[i - offset] = fabs(differences[i - offset] - differences[i - offset - 1]);
		coefsOfPolynFunc[i] = yvals[i];
	}

	// Prepare Hermite interpolation scheme.
	for (int i = fdStoreIndex; i < count - 2; i++) {
		double wP = weights[i - offset + 1];
		double wM = weights[i - offset - 1];

		if (FP_ZERO == fpclassify(wP) && FP_ZERO == fpclassify(wM)) {
			double xv = xvals[i];
			double xvP = xvals[i + 1];
			double xvM = xvals[i - 1];
			firstDerivatives[i] = (((xvP - xv) * differences[i - offset - 1])
					+ ((xv - xvM) * differences[i - offset])) / (xvP - xvM);
		} else {
			firstDerivatives[i] = ((wP * differences[i - offset - 1])
					+ (wM * differences[i - offset])) / (wP + wM);
		}
	}

	int dimSize = count;
	for (int i = fdStoreIndex; i < count - 2; i++) {
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

inline void FastAkima::computeThirdAndFourthCoef(int count, int i, __m256d fd, __m256d fdNext, __m256d xv, __m256d xvNext,
		__m256d yv, __m256d yvNext, double* coefsOfPolynFunc) {

	//@TODO diff of xNext and x is allready computed (two scopes before)
	__m256d w = _mm256_sub_pd(xvNext, xv);
	__m256d yvMinusYvNext = _mm256_sub_pd(yv, yvNext);

	//optimaize one division
	__m256d tmpDiv = _mm256_div_pd(yvMinusYvNext, w);

	//FMA instruction: -3 * tmpDiv - 2*fd
	__m256d coef3 = _mm256_fmsub_pd(MINUS_THREE_PD, tmpDiv, _mm256_mul_pd(TWO_PD, fd));
	coef3 = _mm256_div_pd(_mm256_sub_pd(coef3, fdNext), w);
	_mm256_stream_pd(&coefsOfPolynFunc[2 * count + i * SIMD_WIDTH], coef3);

	__m256d fdPlusFdNext = _mm256_add_pd(fd, fdNext);
	__m256d w2 = _mm256_mul_pd(w, w);

	__m256d coef4 = _mm256_fmadd_pd(TWO_PD, tmpDiv, fdPlusFdNext);
	coef4 = _mm256_div_pd(coef4, w2);
	_mm256_stream_pd(&coefsOfPolynFunc[3 * count + i * SIMD_WIDTH], coef4);
}

inline __m256d FastAkima::storeFirstDerivats(int fdStoreIndex, double* firstDerivatives, __m256d d0, __m256d d1,
		__m256d x0, __m256d x1, __m256d w1, __m256d w2) {
	//wP = wnn
	//wM = w1

	//creates vector [W1_2, W1_3, W2_0, W2_1]
	__m256d wnn = _mm256_permute2f128_pd(w1, w2, 0b00100001);

	__m256d cond1 = _mm256_cmp_pd(wnn, zero, _CMP_EQ_OQ);
	__m256d cond2 = _mm256_cmp_pd(w1, zero, _CMP_EQ_OQ);

	__m256d dPrev = _mm256_shiftl_and_load_next_pd(d0, d1);
	//creates vector [d0_2, d0_3, d1_0, d1_1]
	__m256d dCurr = _mm256_permute2f128_pd(d0, d1, 0b00100001);

	__m256d condAnd = _mm256_and_pd(cond1, cond2);
	__m256d fd; //result - first derivates

	if (_mm256_testz_pd(cond1, cond2)) {
		fd = _mm256_add_pd(_mm256_mul_pd(wnn, dPrev), _mm256_mul_pd(w1, dCurr));
		fd = _mm256_div_pd(fd, _mm256_add_pd(wnn, w1));
		_mm256_store_pd(&firstDerivatives[fdStoreIndex], fd);
	} else {
		//compute special cases
		//creates vector [x0_2, x0_3, x1_0, x1_1]
		__m256d xv = _mm256_permute2f128_pd(x0, x1, 0b00100001);
		__m256d xvP = _mm256_shiftr_and_load_next_pd(x0, x1);
		__m256d xvM = _mm256_shiftl_and_load_next_pd(x0, x1);

		//compute special cases - store in fd1
		__m256d fd1 = _mm256_mul_pd(_mm256_sub_pd(xvP, xv), dPrev);
		fd1 = _mm256_add_pd(fd1, _mm256_mul_pd(_mm256_sub_pd(xv, xvM), dCurr));
		fd1 = _mm256_div_pd(fd1, _mm256_sub_pd(xvP, xvM));

		//compute ok cases - store in fd2
		__m256d fd2 = _mm256_add_pd(_mm256_mul_pd(wnn, dPrev), _mm256_mul_pd(w1, dCurr));
		fd2 = _mm256_div_pd(fd2, _mm256_add_pd(wnn, w1));

		//combine ok and special cases
		__m256i combineMask = _mm256_castpd_si256(condAnd);
		//will it blend? watch?v=lAl28d6tbko
		fd = _mm256_blendv_pd(fd1, fd2, _mm256_castsi256_pd(_mm256_andnot_si256(combineMask, intTrue)));
		_mm256_store_pd(&firstDerivatives[fdStoreIndex], fd); //avoid masked store
	}

	return fd;
}

void FastAkima::computeFirstDerivatesWoTmpArr(int count, double* xvals, double* yvals, double* coefsOfPolynFunc) {

	double* firstDerivatives = &coefsOfPolynFunc[count];

	// cannot use array - array of vectors causes L1 access
	__m256d x0, x1, x2, x3;
	__m256d y0, y1, y2, y3;
	__m256d d0, d1, d2;

	x0 = _mm256_loadu_pd(&xvals[0]);
	y0 = _mm256_loadu_pd(&yvals[0]);

	auto initStartVars = [&](__m256d* xPrev, __m256d* yPrev, __m256d* x, __m256d* y, __m256d* d, int i) {
		*x = _mm256_loadu_pd(&xvals[i * SIMD_WIDTH]);
		*y = _mm256_loadu_pd(&yvals[i * SIMD_WIDTH]);
		__m256d xn = _mm256_shiftl_and_load_next_pd(*xPrev, *x);
		__m256d yn = _mm256_shiftl_and_load_next_pd(*yPrev, *y);
		__m256d dx = _mm256_sub_pd(*xPrev, xn);
		__m256d dy = _mm256_sub_pd(*yPrev, yn);
		*d = _mm256_div_pd(dy, dx);
	};

	int initIndex = 1;
	initStartVars(&x0, &y0, &x1, &y1, &d0, initIndex++);
	initStartVars(&x1, &y1, &x2, &y2, &d1, initIndex++);
	initStartVars(&x2, &y2, &x3, &y3, &d2, initIndex++);

	__m256d d1n = _mm256_shiftl_and_load_next_pd(d0, d1);
	__m256d w1 = _mm256_abs_pd(_mm256_sub_pd(d0, d1n));

	__m256d d2n = _mm256_shiftl_and_load_next_pd(d1, d2);
	__m256d w2 = _mm256_abs_pd(_mm256_sub_pd(d1, d2n));


	//store fist coefs
	_mm256_stream_pd(&coefsOfPolynFunc[0 * SIMD_WIDTH], y0);
	_mm256_stream_pd(&coefsOfPolynFunc[1 * SIMD_WIDTH], y1);
	_mm256_stream_pd(&coefsOfPolynFunc[2 * SIMD_WIDTH], y2);
	_mm256_stream_pd(&coefsOfPolynFunc[3 * SIMD_WIDTH], y3);

	//first two deriavates are allready computed, hence fd_0 and fd_1 are valid
	__m256d fdPrev = _mm256_load_pd(&coefsOfPolynFunc[count + 0 * SIMD_WIDTH]);
	__m256d fd; //normalka se shiftem

	int i = 3;
	for (; i <= (count - 2) / SIMD_WIDTH - 1; i++) {

		int fdStoreIndex = (i - 3) * SIMD_WIDTH + 2;

		fd = storeFirstDerivats(fdStoreIndex, firstDerivatives, d0, d1, x0, x1, w1, w2);

		//creates vector [fdPrev_0, fdPrev_1, fd_0, fd_1]
		__m256d fd0 = _mm256_permute2f128_pd(fdPrev, fd, 0b00100000);

		//fd1 = [fd0_1, fd0_2, fd0_3, fd_2]
		__m256d fd1 = _mm256_blend_pd(_mm256_rotate_left_pd(fd0), _mm256_rotate_right_pd(fd), 0b1110);

		__m256d xn0 = _mm256_shiftl_and_load_next_pd(x0, x1);
		__m256d yn0 = _mm256_shiftl_and_load_next_pd(y0, y1);
		computeThirdAndFourthCoef(count, i - 3, fd0, fd1, x0, xn0, y0, yn0, coefsOfPolynFunc);

		fdPrev = fd;

		x0 = x1;
		x1 = x2;
		x2 = x3;
		x3 = _mm256_loadu_pd(&xvals[(i + 1) * SIMD_WIDTH]);

		y0 = y1;
		y1 = y2;
		y2 = y3;
		y3 = _mm256_loadu_pd(&yvals[(i + 1) * SIMD_WIDTH]);

		//stream store - hint to cpu not to use tmp (L1) buffer
		//break dependecy to place it here - after assign
		_mm256_stream_pd(&coefsOfPolynFunc[(i + 1) * SIMD_WIDTH], y3);

		d0 = d1;
		d1 = d2;
		__m256d yn = _mm256_shiftl_and_load_next_pd(y2, y3);
		__m256d xn = _mm256_shiftl_and_load_next_pd(x2, x3);

		__m256d dy = _mm256_sub_pd(y2, yn);
		__m256d dx = _mm256_sub_pd(x2, xn);
		d2 = _mm256_div_pd(dy, dx);


		w1 = w2;
		__m256d d2n = _mm256_shiftl_and_load_next_pd(d1, d2);
		w2 = _mm256_abs_pd(_mm256_sub_pd(d1, d2n));
	}


	int fdStoreIndex = (i - 3) * SIMD_WIDTH + 2;
	storeFirstDerivats(fdStoreIndex, firstDerivatives, d0, d1, x0, x1, w1, w2);

	computeRestCoefsScalar(count, fdStoreIndex, coefsOfPolynFunc, xvals, yvals);
}

double* FastAkima::computeCoefficients(int count, double* xvals, double* yvals) {
	if (count < MINIMUM_NUMBER_POINTS) {
		throw "Insufficient data exception.";
	}

	double* coefsOfPolynFunc = (double*) aligned_alloc(64, sizeof (double) * 4 * count);
	double* firstDerivatives = &coefsOfPolynFunc[1 * count];

	computeHeadAndTailOfFirstDerivates(count, xvals, yvals, firstDerivatives);
	computeFirstDerivatesWoTmpArr(count, xvals, yvals, coefsOfPolynFunc);

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
		_mm256_store_pd(&weights[weightIndex], _mm256_abs_pd(_mm256_sub_pd(w, dPrev)));

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

	//visualc do not have this intrinsic
	//_mm256_storeu2_m128d(&firstDerivatives[count - 2], &firstDerivatives[0], res);
	double tmpRes[4];
	_mm256_storeu_pd(tmpRes, res);
	firstDerivatives[0] = tmpRes[1];
	firstDerivatives[1] = tmpRes[0];
	firstDerivatives[count - 1] = tmpRes[3];
	firstDerivatives[count - 2] = tmpRes[2];
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

