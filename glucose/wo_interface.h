
#include "../helpers.h"

#ifndef WO_INTERFACE_H
#define WO_INTERFACE_H

class GlucoseWoInterface {
private:
	TGlucoseLevelBounds *levelsBounds;
public:

	GlucoseWoInterface(TGlucoseLevelBounds *bounds) {
		levelsBounds = bounds;
	}

	//	inline size_t linearKnotSearch(double desired_time, size_t from, size_t to, double *times) {
	//
	//		for (size_t i = from; i <= to; i++) {
	//			if (times[i] > desired_time) {
	//				return i - 1;
	//			}
	//		}
	//		printf("%d %d %f\n", from, to, desired_time);
	//		throw ("Illegal state exception\n");
	//	}

	inline size_t guessKnot(size_t count, double desired_time, double *times) {
		int SIMD_WIDHT = 4;
		int INTERVALS = SIMD_WIDHT + 1;

		__m128i ONE_TO_FOUR = _mm_setr_epi32(1, 2, 3, 4);
		__m256d desired = _mm256_set1_pd(desired_time);


		int intervalSize = count / INTERVALS;
		__m128i intervalSizeVect = _mm_set1_epi32(intervalSize);
		__m128i intervalIndex = _mm_mullo_epi32(intervalSizeVect, ONE_TO_FOUR);

		int intervalStartIndex = 0;

		// 10 comes from test from some guy on some relatively new intel cpu
		// http://stackoverflow.com/a/39039018
		//call linear search (eg. appropriate interpolation function)
		//https://schani.wordpress.com/2010/04/30/linear-vs-binary-search/
		// 10 is not final value of linear search. Linear search will be resized INTERVALS * intervalSize
		// and searched for each item in vector (not yet vectorized). eg 10 -> ~200 (= 10*5*4)
		while (intervalSize > 10) {

			__m256d intervalGathered = _mm256_i32gather_pd(&times[0], intervalIndex, 8);
			__m256d cond = _mm256_cmp_pd(intervalGathered, desired, _CMP_LE_OQ);


			//here comes black magic
			__m128i possibleIndexStarts = _mm_sub_epi32(intervalIndex, _mm_set1_epi32(intervalSize));

			//set default index, this is last index ____|____|____|____|(____)
			//where ___ are items, | are comparsion points and (____) area we'll search in next iteration

			int si = 0;
			int m = _mm256_movemask_pd(cond);


			//@TODO check if compiled to CMOV instructions
			if (m == 0b1111) {
				si = _mm_extract_epi32(possibleIndexStarts, 3) + intervalSize;
			} else if (m == 0b0111) {
				si = _mm_extract_epi32(possibleIndexStarts, 3);
			} else if (m == 0b0011) {
				si = _mm_extract_epi32(possibleIndexStarts, 2);
			} else if (m == 0b0001) {
				si = _mm_extract_epi32(possibleIndexStarts, 1);
			} else {
				si = _mm_extract_epi32(possibleIndexStarts, 0);
			}


			intervalSize /= INTERVALS;
			__m128i indexShifts = _mm_mullo_epi32(_mm_set1_epi32(intervalSize), ONE_TO_FOUR);

			intervalIndex = _mm_add_epi32(indexShifts, _mm_set1_epi32(si));
			intervalStartIndex = _mm_extract_epi32(intervalIndex, 0) - intervalSize;
		}

		return intervalStartIndex; //guess
	}

	int GetLevels(floattype desired_time, floattype stepping, size_t cnt,
			floattype* levels, size_t* filled, size_t derivationorder, std::vector<double> &times,
			AlignedCoefficients &coefficients, size_t count) {

		if (stepping < 0) {
			throw ("stepping cannot be negative\n");
		}


		//perform vector binaray search for start knot of desired_time
		size_t knotIndex = guessKnot(count, desired_time, &times[0]);


		__m256d steppingVect = _mm256_mul_pd(_mm256_set1_pd(stepping), _mm256_setr_pd(0, 1, 2, 3));
		__m256d baseParams = _mm256_add_pd(_mm256_set1_pd(desired_time), steppingVect);
		__m256d shift = _mm256_set1_pd(stepping * 4);


		int i = 0;
		for (; i < cnt / 4; i += 4) {
			__m256d params = _mm256_fmadd_pd(_mm256_set1_pd(i), shift, baseParams);

			//potentially unsafe to access vector this way
			if (params[3] >= levelsBounds->MaxTime) {
				//vector would overflow
				break;
			}

			__m256d res = Interpolator::getValueAnyNextKnot(knotIndex, count, coefficients, times, params);

			//_mm256_stream_pd(&levels[i], res);
			//cant use stream instuction (_mm256_stream_pd) because there is no garantee, caller aligned levels
			_mm256_storeu_pd(&levels[i], res);
			(*filled) += 4;
		}

		//rest with scalar code
		for (; i < cnt && i * stepping + desired_time < levelsBounds->MaxTime; i++) {
			levels[i] = Interpolator::getInterpolationWithStartIndex(knotIndex, count, times,
					coefficients, i * stepping + desired_time);
			(*filled)++;
		}

		return 1;
	}

};


#endif /* WO_INTERFACE_H */

