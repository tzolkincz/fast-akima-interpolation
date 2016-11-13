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


#define timeNow() std::chrono::high_resolution_clock::now()
#define durationMs(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()



#include <immintrin.h>
#include "helpers.h"

inline __m256d getValueGathers(int knotBase, int knotVindex, int size, double * coefs, __m256d args) {
	//__m256i vindex = _mm256_setr_epi64x(0, size, 2* size, 3 * size);
	//__m256d coefs0 = _mm256_i64gather_pd(&coefs[knotBase], vindex, 8); //gathery nebudou potřeba, asi
	int SIMD_WIDTH = 4;

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
	int count = 1 * 1000 * 1000;

	double* x = (double*) aligned_alloc(64, sizeof (double) *count);
	double* y = (double*) aligned_alloc(64, sizeof (double) *count);

	srand(time(NULL));
	for (int i = 0; i < count; i++) {
		x[i] = (double) i;
		y[i] = rand() * 0.0001;
	}


	for (int i = 0; i < 10; i++) {

		auto t1 = timeNow();
		FastAkima fastAkimaImpl;
		double* coefsOfFastImpl = fastAkimaImpl.computeCoefficients(count, x, y);

		std::cout << "Fast impl took: " << durationMs(timeNow() - t1) << " ms\n";

		printf("%f %f\n", coefsOfFastImpl[100], coefsOfFastImpl[100000]);

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
		std::cout << "pyču kozy: " << anti_optimalizator << " \n";


		free(coefsOfFastImpl);
	}


	//	t1 = timeNow();
	//	ScalarAkima scalarImpl;
	//	double* coefsOfScalar = scalarImpl.interpolate(count, x, y);
	//
	//	std::cout << "Scalar impl took: " << durationMs(timeNow() - t1) << " ms\n";
	//	printf("%f %f\n", coefsOfScalar[100], coefsOfScalar[100000]);
	//
	//	free(coefsOfScalar);
	//
	//
	//	t1 = timeNow();
	//	double* coefsOfFastScalar = scalarImpl.interpolateFastestYet(count, x, y);
	//
	//	std::cout << "Scalar fast-impl took: " << durationMs(timeNow() - t1) << " ms\n";
	//	printf("%f %f\n", coefsOfFastScalar[100], coefsOfFastScalar[100000]);
	//
	//	free(coefsOfFastScalar);


	free(x);
	free(y);
}

/*
 *
 */
int main1(int argc, char** argv) {


	simplePerfTest();

	return 0;
}

