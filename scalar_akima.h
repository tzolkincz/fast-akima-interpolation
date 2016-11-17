
#ifndef SCALAR_AKIMA_H
#define SCALAR_AKIMA_H

#include "fast_akima.h"

class ScalarAkima {
public:

	ScalarAkima();

	/**
	 * Computes coefficients of Akima Interpolation.
	 * Coefficients are stored in one array containing coefficients stored in
	 * column fashion. Eg. first coefficients are stored between 0 and count position (inclusive).
	 *
	 * @param count # of elements
	 * @param xvals x-values
	 * @param yvals y-values
	 * @return
	 */
	//__attribute__((vector, nothrow)) --why not vector?
	__attribute__((nothrow)) AlignedCoefficients computeCoefficients(size_t count, std::vector<double> &xvals, std::vector<double> &yvals);

private:

	/**
	 * Compute differences within three points as needed by Akima interpolation
	 *
	 * @param xvals
	 * @param yvals
	 * @param indexOfDifferentiation
	 * @param indexOfFirstSample
	 * @param indexOfSecondsample
	 * @param indexOfThirdSample
	 * @return
	 */
	double differentiateThreePointScalar(std::vector<double> &xvals, std::vector<double> &yvals,
			size_t indexOfDifferentiation,
			size_t indexOfFirstSample,
			size_t indexOfSecondsample,
			size_t indexOfThirdSample);

	/**
	 * Interpolate on basis of Hermite's algorithm
	 *
	 * @param count # of elements
	 * @param xvals
	 * @param yvals
	 * @param firstDerivatives pointer to output structure with first derivates
	 * @return
	 */
	AlignedCoefficients interpolateHermiteScalar(size_t count, std::vector<double> &xvals,
			std::vector<double> &yvals, AlignedCoefficients &coefsOfPolynFunc);

};


#endif /* SCALAR_AKIMA_H */

