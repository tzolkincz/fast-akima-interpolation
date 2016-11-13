
#ifndef SCALAR_AKIMA_H
#define SCALAR_AKIMA_H

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
	double* computeCoefficients(int count, double* xvals, double* yvals);

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
	double differentiateThreePointScalar(double* xvals, double* yvals,
			int indexOfDifferentiation,
			int indexOfFirstSample,
			int indexOfSecondsample,
			int indexOfThirdSample);

	/**
	 * Interpolate on basis of Hermite's algorithm
	 *
	 * @param count # of elements
	 * @param xvals
	 * @param yvals
	 * @param firstDerivatives pointer to output structure with first derivates
	 * @return
	 */
	double* interpolateHermiteScalar(int count, double* xvals, double* yvals, double* firstDerivatives);

};


#endif /* SCALAR_AKIMA_H */

