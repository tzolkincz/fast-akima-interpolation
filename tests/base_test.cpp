
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <math.h>

#include "../fast_akima.h"
#include "../scalar_akima.h"
#include "../interpolator.h"
#include "../lib/glucose/GlucoseLevels.h"
#include "../glucose/glucose_impl.h"
#include "../glucose/wo_interface.h"


#define timeNow() std::chrono::high_resolution_clock::now()
#define durationMs(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()

void printCoefficients(size_t count, AlignedCoefficients coefs);

/*
 * Simple C++ Test Suite
 */

/*
 simple helper function
 */
void printCoefficients(size_t count, AlignedCoefficients coefs) {
	for (int i = 0; i < count; i++) {
		printf("A: %f %f %f %f\n", coefs[i], coefs[count + i],
				coefs[2 * count + i], coefs[3 * count + i]);
	}
}


const double EPSILON = 0.000000001;

/*
 hustý:
 * I observed an interesting phenomenon when executing 256-bit vector instructions on the Skylake. There is a warm-up period of approximately 14 µs before it can execute 256-bit vector instructions at full speed. Apparently, the upper 128-bit half of the execution units and data buses is turned off in order to save power when it is not used. As soon as the processor sees a 256-bit instruction it starts to power up the upper half. It can still execute 256-bit instructions during the warm-up period, but it does so by using the lower 128-bit units twice for every 256-bit vector. The result is that the throughput for 256-bit vectors is 4-5 times slower during this warm-up period. If you know in advance that you will need to use 256-bit instructions soon, then you can start the warm-up process by placing a dummy 256-bit instruction at a strategic place in the code. My measurements showed that the upper half of the units is shut down again after 675 µs of inactivity.
 * http://www.agner.org/optimize/blog/read.php?i=415
 *
 *
 myšlenka skývání latence: (dokonce i přímo u hornera)
 * https://www.yeppp.info/resources/ppam-presentation.pdf
 */



void simpleTest() {
	std::cout << "newsimpletest test 1" << std::endl;

	//init values
	const size_t count = 24;


	//precomputed, verified values
	double correctInterpol[count * 4] = {1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.25, 3.5, 3.75, 4,
		3.625, 2.5, 1.375, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.2734375, 3.5625, 3.8203125, 4,
		4.0703125, 4.0625, 4.0234375, 4, 4, 4, 4, 4, 3.484375, 2.375, 1.328125, 1, 1.25, 1.5, 1.75, 2,
		2.25, 2.5, 2.75, 3, 3.37109375, 3.90625, 4.48828125, 5, 5.390625, 5.6875, 5.890625, 6, 5.74609375,
		5.09375, 4.39453125, 4, 4.1234375, 4.5375, 4.9328125, 5, 4.5984375, 3.9625, 3.3453125, 3,
		2.9484375, 2.9875, 3.0328125, 3, 2.7640625, 2.3875, 2.0671875, 2, 2.3125, 2.875, 3.5, 4, 4.3671875,
		4.6875, 4.9140625, 5, 4.734375, 4.125, 3.453125, 3, 2.75, 2.5, 2.25, 598, 610.3125, 622.75, 635.3125};

	std::vector<double> x(count);
	std::vector<double> y(count);


	for (int i = 0; i < count; i++) {
		x[i] = (double) i;
	}

	int i = 0;
	y[i++] = 1.0;
	y[i++] = 2.0;
	y[i++] = 3.0;
	y[i++] = 4.0;
	y[i++] = 1.0;

	y[i++] = 2.0;
	y[i++] = 3.0;
	y[i++] = 4.0;
	y[i++] = 4.0;
	y[i++] = 4.0;

	y[i++] = 1.0;
	y[i++] = 2.0;
	y[i++] = 3.0;
	y[i++] = 5.0;
	y[i++] = 6.0;

	y[i++] = 4.0;
	y[i++] = 5.0;
	y[i++] = 3.0;
	y[i++] = 3.0;
	y[i++] = 2.0;

	y[i++] = 4.0;
	y[i++] = 5.0;
	y[i++] = 3.0;
	y[i++] = 3.0;

	FastAkima fastAkimaImpl;
	auto coefsOfFastImpl = fastAkimaImpl.computeCoefficients(count, x, y);


	ScalarAkima scalarImpl;
	auto coefsOfScalar = scalarImpl.computeCoefficients(count, x, y);



	//check diffs:
	bool equals = true;

	for (size_t i = 0; i < count * 4; i++) {
		if (fabs(coefsOfFastImpl[i] - coefsOfScalar[i]) > EPSILON && i % count != count - 1) {
			printf("ERR! fast: %f scalar: %f diff: %f %d \n",
					coefsOfFastImpl[i], coefsOfScalar[i], coefsOfFastImpl[i] - coefsOfScalar[i], i);
			equals = false;
		}
	}

	if (!equals) {
		std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
				"Results of scalar implementation are not equal to "
				"results of Fast Implementation" << std::endl;
	}

	printCoefficients(count, coefsOfFastImpl);

	std::cout << "\n \n";
	printCoefficients(count, coefsOfScalar);


	std::cout << "\n \n";
	int k = 0;
	for (size_t i = 0; i < count - 5; i++) {
		for (double j = 0.0; j < 1; j += 0.25) {
			if (fabs(correctInterpol[k] - Interpolator::getInterpolation(count, x, coefsOfFastImpl, i + j)) > EPSILON) {
				std::cout << correctInterpol[k] << " " << correctInterpol[k]
						- Interpolator::getInterpolation(count, x, coefsOfFastImpl, i + j) << "\n";
				std::cout << i + j << "," << Interpolator::getInterpolation(count, x, coefsOfFastImpl, i + j) << "\n";
				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
						"results of interpolation are not correct " << std::endl;
			}

			k++;
		}
	}

	for (size_t i = 0; i < count - 5; i++) {

		__m256d arg = _mm256_setr_pd(i, i + 0.25, i + 0.5, i + 0.75);
		__m256d res = Interpolator::getValueWithinOneKnot(i, count, coefsOfFastImpl, x, arg);

		int k = 0;
		for (double j = 0.0; j < 1; j += 0.25) {
			double in = Interpolator::getInterpolation(count, x, coefsOfFastImpl, i + j);
			if (fabs(in - res[k++]) > EPSILON) {
				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
						"vector interpolation failed on " << k << " th item" << std::endl;
			}
			//			std::cout << std::setprecision(10) << getInterpolation(count, x, coefsOfFastImpl, i + j) << ", ";
		}
	}


	//test getValueKnownKnots function
	{
		__m256d arg = _mm256_setr_pd(1, 2.25, 3.5, 4.75);
		__m128i knotSteps = _mm_setr_epi32(1, 2, 3, 4);
		__m256d resVector = Interpolator::getValueKnownKnots(0, knotSteps, count, coefsOfFastImpl, x, arg);

		__m256d resScalar = _mm256_setr_pd(
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 1),
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 2.25),
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 3.5),
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 4.75)
				);

		__m256d diffForCmp = _mm256_sub_pd(resScalar, resVector);
		__m256d cmp = _mm256_cmp_pd(diffForCmp, _mm256_set1_pd(EPSILON), _CMP_GT_OQ);
		if (!_mm256_testz_pd(cmp, cmp)) {
			std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
					"test of function getValueKnownKnots failed" << std::endl;
		}
	}

	//test getValueAnyNextKnot function
	{
		__m256d arg = _mm256_setr_pd(1, 2.25, 3.5, 4.75);
		__m256d resVector = Interpolator::getValueAnyNextKnot(0, count, coefsOfFastImpl, x, arg);

		__m256d resScalar = _mm256_setr_pd(
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 1),
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 2.25),
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 3.5),
				Interpolator::getInterpolation(count, x, coefsOfFastImpl, 4.75)
				);

		__m256d diffForCmp = _mm256_sub_pd(resScalar, resVector);
		__m256d cmp = _mm256_cmp_pd(diffForCmp, _mm256_set1_pd(EPSILON), _CMP_GT_OQ);
		if (!_mm256_testz_pd(cmp, cmp)) {
			std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
					"test of function getValueAnyNextKnot failed" << std::endl;
		}
	}


}

void glucoseTest() {

	size_t cnt = 100;

	std::vector<TGlucoseLevel> tl(cnt);
	std::vector<double> times(cnt);
	std::vector<double> vals(cnt);
	for (size_t i = 0; i < cnt; i++) {
		TGlucoseLevel l;
		l.datetime = 0.1 * i;
		l.level = rand() * 0.0001;

		times[i] = l.datetime;
		vals[i] = l.level;
		tl[i] = l;
	}

	try {
		FastAkima fastAkimaImpl;
		auto coefficients = fastAkimaImpl.computeCoefficients(cnt, times, vals);

		TGlucoseLevelBounds *gb = new TGlucoseLevelBounds();
		gb->MaxTime = times[cnt - 1];

		GlucoseWoInterface *m = new GlucoseWoInterface(gb);


		size_t desired_levels_count = 10;
		size_t filled;
		double* levels = (double*) malloc(sizeof (double) * desired_levels_count);
		m->GetLevels(0.12, 1, desired_levels_count, levels, &filled, 0, times, coefficients, cnt);

		for (int i = 0; i < desired_levels_count; i++) {
			if (fabs(Interpolator::getInterpolation(cnt, times, coefficients, i * 1 + 0.12) - levels[i])
					> EPSILON) {
				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
						"test glucose interface get levels" << std::endl;
			}
		}

		free(levels);
	} catch (char const* msg) {
		std::cout << msg;
	}

}

void glucoseInterfaceTest() {
	size_t cnt = 100;

	std::vector<TGlucoseLevel> tl(cnt);
	std::vector<double> times(cnt);
	std::vector<double> vals(cnt);
	for (size_t i = 0; i < cnt; i++) {
		TGlucoseLevel l;
		l.datetime = 0.1 * i;
		l.level = rand() * 0.0001;

		times[i] = l.datetime;
		vals[i] = l.level;
		tl[i] = l;
	}

	try {
		FastAkima fastAkimaImpl;
		auto coefficients = fastAkimaImpl.computeCoefficients(cnt, times, vals);


		CGlucoseLevels *gl = new CGlucoseLevels();
		gl->SetLevelsCount(cnt);
		gl->SetLevels(tl);

		GlucoseImplementation *m = new GlucoseImplementation(gl);
		m->Approximate(new TApproximationParams);

		size_t desired_levels_count = 6;
		size_t filled;
		double* levels = (double*) malloc(sizeof (double) * desired_levels_count);
		m->GetLevels(0.12, 1, desired_levels_count, levels, &filled, 0);
		for (int i = 0; i < desired_levels_count; i++) {
			if (fabs(Interpolator::getInterpolation(cnt, times, coefficients, i * 1 + 0.12) - levels[i])
					> EPSILON) {
				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
						"test glucose interface get levels" << std::endl;
			}
		}

		free(levels);
	} catch (char const* msg) {
		std::cout << msg;
	}

}

void glucoseInterfaceTestGlucoseArray() {
	size_t cnt = 100;

	std::vector<TGlucoseLevel> tl(cnt);
	std::vector<double> times(cnt);
	std::vector<double> vals(cnt);
	for (size_t i = 0; i < cnt; i++) {
		TGlucoseLevel l;
		l.datetime = 0.1 * i;
		l.level = rand() * 0.0001;

		times[i] = l.datetime;
		vals[i] = l.level;
		tl[i] = l;
	}

	try {
		FastAkima fastAkimaImpl;
		auto coefficients = fastAkimaImpl.computeCoefficients(cnt, times, vals);


		CGlucoseLevels *gl = new CGlucoseLevels();
		gl->SetLevelsCount(cnt);
		gl->SetLevels(tl);

		GlucoseImplementation *m = new GlucoseImplementation(gl);
		m->Approximate(new TApproximationParams);


		const size_t desired_levels_count = 6;
		double desired_array[desired_levels_count] = {0.12, 1.12, 2.12, 3.12, 4.12, 5.12};
		size_t filled;
		double* levels = (double*) malloc(sizeof (double) * desired_levels_count);
		m->GetLevels(desired_array, desired_levels_count, levels, &filled);

		for (int i = 0; i < desired_levels_count; i++) {
			if (fabs(Interpolator::getInterpolation(cnt, times, coefficients, i * 1 + 0.12) - levels[i])
					> EPSILON) {
				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
						"test glucose interface get levels as array" << std::endl;
			}
		}

		free(levels);
	} catch (char const* msg) {
		std::cout << msg;
	}

}

void glucoseInterfaceTestGlucoseArrayPerf1() {
	size_t cnt = 10 * 1000 * 1000;

	std::vector<TGlucoseLevel> tl(cnt);
	std::vector<double> times(cnt);
	std::vector<double> vals(cnt);
	for (size_t i = 0; i < cnt; i++) {
		TGlucoseLevel l;
		l.datetime = i;
		l.level = rand() * 0.0001;

		times[i] = l.datetime;
		vals[i] = l.level;
		tl[i] = l;
	}

	try {
		FastAkima fastAkimaImpl;
		auto coefficients = fastAkimaImpl.computeCoefficients(cnt, times, vals);

		CGlucoseLevels *gl = new CGlucoseLevels();
		gl->SetLevelsCount(cnt);
		gl->SetLevels(tl);

		GlucoseImplementation *m = new GlucoseImplementation(gl);
		m->Approximate(new TApproximationParams);

		const size_t desired_levels_count = 1 * 1000 * 1000;
		double *desired_array = (double*) malloc(sizeof (double) *desired_levels_count);
		//fill desired array
		desired_array[0] = 0.1;
		for (int i = 1; i < desired_levels_count; i++) {
			desired_array[i] = desired_array[i - 1] + rand() * 0.0000000001;
		}


		size_t filled;
		double* levels = (double*) malloc(sizeof (double) * desired_levels_count);

		auto t1 = timeNow();
		m->GetLevels(desired_array, desired_levels_count, levels, &filled);
		std::cout << "get levels as array and interpol took: " << durationMs(timeNow() - t1) << " ms\n";

		//commented out checks
		//		for (int i = 0; i < desired_levels_count; i++) {
		//			if (fabs(Interpolator::getInterpolation(cnt, times, coefficients, desired_array[i]) - levels[i])
		//					> EPSILON) {
		//				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
		//						"test glucose interface get levels as array" << std::endl;
		//			}
		//		}

		free(levels);
	} catch (char const* msg) {
		std::cout << msg;
	}

}

int main(int argc, char** argv) {
	std::cout << "%SUITE_STARTING% newsimpletest" << std::endl;
	std::cout << "%SUITE_STARTED%" << std::endl;

	std::cout << "%TEST_STARTED% test1 (newsimpletest)" << std::endl;
	simpleTest();
	std::cout << "%TEST_FINISHED% time=0 test1 (newsimpletest)" << std::endl;

	std::cout << "%TEST_STARTED% test2 (newsimpletest)\n" << std::endl;
	glucoseTest();
	std::cout << "%TEST_FINISHED% time=0 test2 (newsimpletest)" << std::endl;

	std::cout << "%TEST_STARTED% test3 (newsimpletest)\n" << std::endl;
	glucoseInterfaceTest();
	std::cout << "%TEST_FINISHED% time=0 test3 (newsimpletest)" << std::endl;

	std::cout << "%TEST_STARTED% test3 (newsimpletest)\n" << std::endl;
	glucoseInterfaceTestGlucoseArray();
	std::cout << "%TEST_FINISHED% time=0 test3 (newsimpletest)" << std::endl;

	std::cout << "%TEST_STARTED% test3 (newsimpletest)\n" << std::endl;
	glucoseInterfaceTestGlucoseArrayPerf1();
	std::cout << "%TEST_FINISHED% time=0 test3 (newsimpletest)" << std::endl;


	std::cout << "%SUITE_FINISHED% time=0" << std::endl;

	return (EXIT_SUCCESS);
}

