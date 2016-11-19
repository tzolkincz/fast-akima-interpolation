
#include <immintrin.h>

#include "fast_akima.h"
#include "helpers.h"

#ifndef INTERPOLATE_H
#define INTERPOLATE_H

class Interpolator {
public:

	const int MINIMUM_NUMBER_POINTS = 5;
	const int SIMD_WIDTH = 4;

	Interpolator();

	/**
	 * Evaluates interpolation within one knot
	 *
	 * @param knotBase - knot index
	 * @param size # of elements
	 * @param coefs of cubic equation stored by columns
	 * @param xvals
	 * @param args values to be interpolated
	 * @return
	 */
	inline static __m256d getValueWithinOneKnot(size_t knotBase, size_t size, AlignedCoefficients &coefs,
			std::vector<double> &xvals, __m256d args) {
		__m256d xx = _mm256_set1_pd(xvals[knotBase]);
		args = _mm256_sub_pd(args, xx);

		__m256d coefs3 = _mm256_set1_pd(coefs[knotBase + 3 * size]);
		__m256d coefs2 = _mm256_set1_pd(coefs[knotBase + 2 * size]);
		__m256d coefs1 = _mm256_set1_pd(coefs[knotBase + 1 * size]);
		__m256d coefs0 = _mm256_set1_pd(coefs[knotBase + 0 * size]);

		return getValueVector(args, coefs0, coefs1, coefs2, coefs3);
	}

	/**
	 * Evaluates interpolation within known knots. Eg caller knows shift of next knots
	 *
	 * @param knotBase knot index
	 * @param knotSteps steps in knots for next elements
	 * @param size # of elements
	 * @param coefs of cubic equation stored by columns
	 * @param xvals
	 * @param args values to be interpolated
	 * @return
	 */
	inline static __m256d getValueKnownKnots(size_t knotBase, __m128i knotSteps, size_t size, AlignedCoefficients &coefs,
			std::vector<double> &xvals, __m256d args) {

		__m256d xx = _mm256_i32gather_pd(&xvals[knotBase], knotSteps, 8);
		args = _mm256_sub_pd(args, xx);

		__m256d coefs3 = _mm256_i32gather_pd(&coefs[knotBase + 3 * size], knotSteps, 8);
		__m256d coefs2 = _mm256_i32gather_pd(&coefs[knotBase + 2 * size], knotSteps, 8);
		__m256d coefs1 = _mm256_i32gather_pd(&coefs[knotBase + 1 * size], knotSteps, 8);
		__m256d coefs0 = _mm256_i32gather_pd(&coefs[knotBase + 0 * size], knotSteps, 8);

		return getValueVector(args, coefs0, coefs1, coefs2, coefs3);
	}

	/**
	 *  Evaluates interpolation within known knots. Eg caller knows only minimal
	 *  knot index, rest is computed here
	 *
	 * @param knotStart minimal knot issued by caller
	 * @param size # of elements
	 * @param coefs of cubic equation stored by columns
	 * @param xvals
	 * @param args values to be interpolated
	 * @return
	 */
	inline static __m256d getValueAnyNextKnot(size_t knotStart, size_t size, AlignedCoefficients &coefs,
			std::vector<double> &xvals, __m256d args) {

		double argsArr[4];
		_mm256_storeu_pd(argsArr, args);

		int knotStepsArr[4] = {-1, -1, -1, -1};
		//for each item in vector, find knot step
		for (int i = 0; i < 4; i++) {

			for (int j = knotStart; j < size; j++) {
				if (xvals[j] > argsArr[i]) {
					//					knotStepsArr[i] = j - 1;
					knotStepsArr[i] = j - knotStart - 1;
					break;
				}
			}
			if (knotStepsArr[i] == -1) {
				throw "Value out of range.\n";
			}
		}
		__m128i knotSteps = _mm_setr_epi32(knotStepsArr[0], knotStepsArr[1], knotStepsArr[2], knotStepsArr[3]);
		return getValueKnownKnots(knotStart, knotSteps, size, coefs, xvals, args);
	}

	/**
	 * Scalar implementation for evaluationg interpolation.
	 * Warning: inefficient linear search for knot from start
	 * FOR TEST ONLY PURPOSES
	 *
	 * @param size # of elments
	 * @param xvals
	 * @param coefs
	 * @param argument
	 * @return
	 */
	inline static double getInterpolation(size_t size, std::vector<double> &xvals,
			AlignedCoefficients &coefs, double argument) {

		int knotIndex = -1;
		for (int i = 0; i < size; i++) {
			if (xvals[i] > argument) {
				knotIndex = i - 1;
				break;
			}
		}

		if (knotIndex < 0) {
			throw "Value out of range.\n";
		}
		return getValue(knotIndex, size, coefs, argument - xvals[knotIndex]);
	}

	/**
	 * Scalar implementation for evaluationg interpolation. Use when some safe guess of start knot is known.
	 *
	 * @param size # of elments
	 * @param xvals
	 * @param coefs
	 * @param argument
	 * @return
	 */
	inline static double getInterpolationWithStartIndex(size_t startKnot, size_t size,
			std::vector<double> &xvals, AlignedCoefficients &coefs, double argument) {

		int knotIndex = -1;
		for (int i = startKnot; i < size; i++) {
			if (xvals[i] > argument) {
				knotIndex = i - 1;
				break;
			}
		}

		if (knotIndex < 0) {
			throw "Value out of range.\n";
		}
		return getValue(knotIndex, size, coefs, argument - xvals[knotIndex]);
	}

	/**
	 * Horner's evaluation method, vector implementation. This is more like internal mehod,
	 * caller have to know coefficients. For easy way of interpolating @see getValueKnownKnots.
	 *
	 * @param args
	 * @param coefs0
	 * @param coefs1
	 * @param coefs2
	 * @param coefs3
	 * @return
	 */
	inline static __m256d getValueVector(__m256d args, __m256d coefs0, __m256d coefs1,
			__m256d coefs2, __m256d coefs3) {
		__m256d res = _mm256_fmadd_pd(args, coefs3, coefs2);
		res = _mm256_fmadd_pd(args, res, coefs1);
		return _mm256_fmadd_pd(args, res, coefs0);
	}

	/**
	 * Horner's evaluation method, scalar implementation
	 *
	 * @param knotIndex
	 * @param size
	 * @param coefs
	 * @param argument
	 * @return
	 */
	inline static double getValue(size_t knotIndex, size_t size, AlignedCoefficients &coefs, double argument) {

		double res = coefs[size * 3 + knotIndex];

		res = argument * res + coefs[size * 2 + knotIndex];
		res = argument * res + coefs[size * 1 + knotIndex];
		res = argument * res + coefs[size * 0 + knotIndex];

		return res;
	}

};



#endif /* INTERPOLATE_H */

