/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   perfTest.cpp
 * Author: v
 *
 * Created on 20. října 2016, 17:28
 */

#include <cstdlib>
#include <chrono>
#include <stdio.h>
#include <iostream>
#include "fast_akima.h"
#include "scalar_akima.h"


#include "interpolator.h"
#include "lib/glucose/GlucoseLevels.h"
// #include "glucose/glucose_impl.h"
#include "glucose/wo_interface.h"


#define timeNow() std::chrono::high_resolution_clock::now()
#define durationMs(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()



#include <immintrin.h>
#include "helpers.h"

inline __m256d getValueGathers(int knotBase, int knotVindex, int size, AlignedCoefficients &coefs, __m256d args) {

	__m256d coefs3 = _mm256_set1_pd(coefs[knotBase + 3 * size]);
	__m256d coefs2 = _mm256_set1_pd(coefs[knotBase + 2 * size]);
	__m256d coefs1 = _mm256_set1_pd(coefs[knotBase + 1 * size]);
	__m256d coefs0 = _mm256_set1_pd(coefs[knotBase + 0 * size]);

	__m256d res = _mm256_fmadd_pd(args, coefs3, coefs2);
	res = _mm256_fmadd_pd(args, res, coefs1);
	res = _mm256_fmadd_pd(args, res, coefs0);

	return res;
}

void simplePerfTest() {
	std::cout << "simple perf test 1" << std::endl;

	//init values
//	size_t count = 1 * 1000 * 1000;
	size_t count = 100* 1000;

	std::vector<double> x(count);
	std::vector<double> y(count);

	srand(time(NULL));
	for (int i = 0; i < count; i++) {
		x[i] = (double) i;
		y[i] = rand() * 0.0001;
	}


	for (int i = 0; i < 10; i++) {

		auto t1 = timeNow();
		FastAkima fastAkimaImpl;
		auto coefsOfFastImpl = fastAkimaImpl.computeCoefficients(count, x, y);

		std::cout << "Fast impl took: " << durationMs(timeNow() - t1) << " ms\n";

		printf("%f %f\n", coefsOfFastImpl[100], coefsOfFastImpl[10000]);

		t1 = timeNow();



		__m256d interPoints = _mm256_setr_pd(0, 0.25, 0.5, 0.75);

		double anti_optimalizator = 0;
		for (int i = 0; i < count - 1; i++) {
			__m256d arg = _mm256_add_pd(_mm256_set1_pd(i), interPoints);
			__m256d xx = _mm256_set1_pd(x[i]);
			__m256d res = getValueGathers(i, 0, count, coefsOfFastImpl, _mm256_sub_pd(arg, xx));
			anti_optimalizator += res[2];
		}

		std::cout << "interpol took: " << durationMs(timeNow() - t1) << " ms\n";
		std::cout << "anti optimizer: " << anti_optimalizator << " \n";


		// test glucose interface perf
		t1 = timeNow();



		//		std::vector<double> levels(count);
		//		CGlucoseLevels gl =



		std::cout << "interpol took: " << durationMs(timeNow() - t1) << " ms\n";
		std::cout << "anti optimizer: " << anti_optimalizator << " \n";


	}

	for (int i = 0; i < 10; i++) {
		auto t1 = timeNow();
		ScalarAkima scalarImpl;
		auto coefsOfScalar = scalarImpl.computeCoefficients(count, x, y);

		std::cout << "Scalar impl took: " << durationMs(timeNow() - t1) << " ms\n";
		printf("%f %f\n", coefsOfScalar[100], coefsOfScalar[10000]);
	}

}


void glucoseTest4() {

	size_t cnt = 1  *1000* 1000;



	printf("vals:\n");
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


		TGlucoseLevelBounds *gb = new TGlucoseLevelBounds();
		gb->MaxTime = times[cnt - 1];

		GlucoseWoInterface *m = new GlucoseWoInterface(gb);





		size_t desired_levels_count = 10;
		size_t filled;
		double* levels = (double*) malloc(sizeof (double) * desired_levels_count);

		double anti_opt = 0.0;

		auto t1 = timeNow();
		for (int j = 0; j < 1* 1000* 1000; j++) {
			double dt = (rand() % (cnt -10)) + 0.1;
			m->GetLevels(dt, 2.2, desired_levels_count, levels, &filled, 0, times, coefficients, cnt);
			anti_opt += levels[1];

		}
		std::cout << "pentarysearch and interpol took: " << durationMs(timeNow() - t1) << " ms\n";
		printf("anti_opt %f\n", anti_opt);

		//		printf("with glucose interface %d\n", filled);
		//		for (int i = 0; i < desired_levels_count; i++) {
		//			printf("%f  %f\n", levels[i], Interpolator::getInterpolation(cnt, times, coefficients, i*0.2 + 0.32));
		//		}


		free(levels);

	} catch (char const* msg) {
		std::cout << msg;
	}

}

/*
 *
 */
int main(int argc, char** argv) {


	simplePerfTest();

	glucoseTest4();

	return 0;
}

