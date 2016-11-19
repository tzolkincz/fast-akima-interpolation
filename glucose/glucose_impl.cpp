

//#include "../lib/glucose/iface/ApproxIface.h"
//#include "../lib/glucose/rtl/LogicalClockImpl.h"
#include "../fast_akima.h"
#include "../interpolator.h"
#include "glucose_impl.h"
#include <immintrin.h>

HRESULT IfaceCalling GlucoseImplementation::Approximate(TApproximationParams* params) {

	//	CLogical_Clock::Signal_Clock();



	FastAkima fastAkimaImpl;
	coefficients = fastAkimaImpl.computeCoefficients(count, times, levels);

}

inline size_t linearKnotSearch(double desired_time, size_t from, size_t to, double *times) {

	for (size_t i = from; i < to; i++) {
		if (times[i] > desired_time) {
			return i - 1;
		}
	}
	throw ("Illegal state exception");
}

inline size_t searchKnot(size_t count, double desired_time, double *times) {
	int SIMD_WIDHT = 4;
	int INTERVALS = SIMD_WIDHT + 1;

	__m128i ONE_TO_FOUR = _mm_setr_epi32(1, 2, 3, 4);
	__m256d desired = _mm256_set1_pd(desired_time);


	int intervalSize = count / INTERVALS;
	__m128i intervalSizeVect = _mm_set1_epi32(intervalSize);
	__m128i intervalIndex = _mm_mul_epi32(intervalSizeVect, ONE_TO_FOUR);


	while (1) {
		__m256d intervalGathered = _mm256_i32gather_pd(&times[0], intervalIndex, 8);
		__m256d cond = _mm256_cmp_pd(intervalGathered, desired, _CMP_LE_OQ);


		//here comes black magic
		int tmpMaskIndexes[4];
		//stores indexes at tmpMaskIndexes

		//index is prev cos ve search for last time (x) value that is less than desired time
		__m128i prevIndex = _mm_sub_epi32(intervalIndex, _mm_set1_epi32(intervalSize));
		_mm_store_si128((__m128i*) tmpMaskIndexes, prevIndex);


		//set default index, this is last index ____|____|____|____|(____)
		//where ___ are items, | are comparsion points and (____) area we'll search in next iteration
		intervalIndex = _mm_set1_epi32(intervalSize * 4);

		//maskload garantee us not changed result on unsatisfied condition in condition mask
		//(because we've set current state as default)

		for (int position = 4; position > 0; position--) {
			intervalIndex = _mm256_mask_i64gather_epi32(intervalIndex, tmpMaskIndexes,
					_mm256_set1_epi64x(position), _mm256_cvttpd_epi32(cond), 4);
		}

		intervalSize /= INTERVALS;
		if (intervalSize > 100) {
			__m128i indexShifts = _mm_mul_epi32(_mm_set1_epi32(intervalSize), ONE_TO_FOUR);
			intervalIndex = _mm_add_epi32(indexShifts, intervalIndex);
		} else {
			//call linear search (eg. appropriate interpolation function)
			//https://schani.wordpress.com/2010/04/30/linear-vs-binary-search/

			size_t from = _mm_extract_epi32(intervalIndex, 0);
			return linearKnotSearch(desired_time, from, from + INTERVALS * intervalSize, times);
		}
	}
}

HRESULT IfaceCalling GlucoseImplementation::GetLevels(floattype desired_time, floattype stepping, size_t cnt,
		floattype* levels, size_t* filled, size_t derivationorder) {

	if (stepping < 0) {
		throw ("stepping cannot be negative");
	}



	//perform vector binaray search for start knot of desired_time
	size_t knotIndex = searchKnot(count, desired_time, &times[0]);

	__m256d steppingVect = _mm256_mul_pd(_mm256_set1_pd(stepping), _mm256_setr_pd(0, 1, 2, 3));
	__m256d baseParams = _mm256_add_pd(_mm256_set1_pd(desired_time), steppingVect);
	__m256d shift = _mm256_set1_pd(stepping * 4);

	int i = 0;
	//@TODO check bounds
	for (; i < cnt / 4; i += 4) {
		__m256d params = _mm256_fmadd_pd(_mm256_set1_pd(i), shift, baseParams);
		__m256d res = Interpolator::getValueAnyNextKnot(knotIndex, count, coefficients, times, params);
		_mm256_stream_pd(&levels[i], res);
	}

	//rest with scalar code
	for (; i < cnt; i++) {
		levels[i] = Interpolator::getInterpolation(count, times, coefficients, i * stepping + desired_time);
	}





	return 1;

}



//
//HRESULT IfaceCalling GlucoseImplementation::GetLevels(floattype* times, size_t count,
//		floattype *levels, size_t *filled) {
//	return 1;
//}
//
//HRESULT IfaceCalling GlucoseImplementation::GetBounds(TGlucoseLevelBounds *bounds) {
//	return 4;
//}

//GlucoseImplementation::~GlucoseImplementation() {}
