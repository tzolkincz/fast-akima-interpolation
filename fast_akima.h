
#include <immintrin.h>

#ifndef FAST_AKIMA_H
#define FAST_AKIMA_H

class FastAkima {
public:

	const int MINIMUM_NUMBER_POINTS = 5;
	const int SIMD_WIDTH = 4;

	FastAkima();

	/**
	 * Computes and returns interpolation coefficients of Akima Interpolation.
	 * Coefficients are stored in one array containing coefficients stored in
	 * column fashion. Eg. first coefficients are stored between 0 and count position (inclusive).
	 *
	 * @param count number of input nodes
	 * @param xvals x-vals of input pairs
	 * @param yvals y-vals of input pairs
	 * @return
	 */
	double* computeCoefficients(int count, double* xvals, double* yvals);

private:
	void computeDiffsAndWeights(int count, double* xvals, double* yvals, double* differences, double* weights, double* ysInternalCopy);
	void computeFirstDerivates(int count, double* xvals, double* differences, double* weights, double* firstDerivatives);
	void computeHeadAndTailOfFirstDerivates(int count, double* xvals, double* yvals, double* firstDerivatives);
	void computePolynCoefs(int count, double* xvals, double* coefsOfPolynFunc);
	void computeFirstDerivatesWoTmpArr(int count, double* xvals, double* yvals, double* coefsOfPolynFunc);
	void computeThirdAndFourthCoef(int count, int i, __m256d fd, __m256d fdNext, __m256d xv, __m256d xvNext,
			__m256d yv, __m256d yvNext, double* coefsOfPolynFunc);
	__m256d storeFirstDerivats(int fdStoreIndex, double* firstDerivatives, __m256d d0, __m256d d1,
			__m256d x0, __m256d x1, __m256d w1, __m256d w2);
	void computeRestCoefsScalar(int count, int fdStoreIndex, double* coefsOfPolynFunc, double* xvals, double* yvals);
};




#endif /* FAST_AKIMA_H */

