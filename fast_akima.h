
#include <vector>
#include <immintrin.h>
#include "lib/AlignmentAllocator.h"

#ifndef FAST_AKIMA_H
#define FAST_AKIMA_H

typedef AlignmentAllocator<double, AVX2Alignment> AA;
typedef std::vector<double, AA> AlignedCoefficients;

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
	//const std::vector<double> &computeCoefficients(int count, const std::vector<double> &xvals, const std::vector<double> &yvals);
	AlignedCoefficients computeCoefficients(size_t count,
			const std::vector<double> &xvals, const std::vector<double> &yvals);

private:
	/**
	 * Private method for computing first two and last two elements of first derivates
	 *
	 * @param count # of elements
	 * @param xvals
	 * @param yvals
	 * @param firstDerivatives  this reference can be directly first output coefficients (eg. no
	 * need for extra array)
	 */
	void computeHeadAndTailOfFirstDerivates(size_t count, const std::vector<double> &xvals,
			const std::vector<double> &yvals, double* firstDerivatives);

	/**
	 * Compute fist derivates with constant memory footprint
	 *
	 * @param count # of elements
	 * @param xvals
	 * @param yvals
	 * @param coefsOfPolynFunc output array for storing results
	 */
	void computeWoTmpArr(size_t count, const std::vector<double> &xvals,
			const std::vector<double> &yvals, AlignedCoefficients &coefsOfPolynFunc);

	/**
	 * Compute third and fourth element of output coefficients
	 *
	 * @param count # of elements
	 * @param i start index
	 * @param fd current first derivates
	 * @param fdNext next first derivates
	 * @param xv current x values
	 * @param xvNext next x values
	 * @param yv current y values
	 * @param yvNext next y values
	 * @param coefsOfPolynFunc output array for storing results
	 */
	void computeThirdAndFourthCoef(size_t count, size_t i, __m256d fd, __m256d fdNext, __m256d xv, __m256d xvNext,
			__m256d yv, __m256d yvNext, AlignedCoefficients &coefsOfPolynFunc);

	/**
	 * Computes and stores first derivates
	 *
	 * @param fdStoreIndex index within output array (fistDerivates param)
	 * @param firstDerivatives output array
	 * @param d0 zeroth derivates
	 * @param d1 fists derivates
	 * @param x0 zeroth x vals
	 * @param x1 first x vals
	 * @param w1 first weights
	 * @param w2 second weights
	 * @return Returns result - can be used in next iteration as prev. derivation
	 */
	__m256d storeFirstDerivates(size_t fdStoreIndex, double* firstDerivatives,
			__m256d d0, __m256d d1, __m256d x0, __m256d x1, __m256d w1, __m256d w2);

	/**
	 * Finish computation with scalar code. Rest that can't be computed with vector alg.
	 *
	 * @param count # of elements
	 * @param fdStoreIndex start store index
	 * @param coefsOfPolynFunc output array
	 * @param xvals
	 * @param yvals
	 */
	void computeRestCoefsScalar(size_t count, size_t fdStoreIndex, AlignedCoefficients &coefsOfPolynFunc,
			const std::vector<double> &xvals, const std::vector<double> &yvals);
};




#endif /* FAST_AKIMA_H */

