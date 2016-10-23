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


void simplePerfTest() {
	std::cout << "simple perf test 1" << std::endl;

	//init values
	int count = 10*1000*1000;

	double* x = (double*) aligned_alloc(64, sizeof (double) *count);
	double* y = (double*) aligned_alloc(64, sizeof (double) *count);

	srand(time(NULL));
	for (int i = 0; i < count; i++) {
		x[i] = (double) i;
		y[i] = rand() * 0.0001;
	}



	auto t1 = timeNow();
	FastAkima fastAkimaImpl;
	double* coefsOfFastImpl = fastAkimaImpl.interpolate(count, x, y);

	std::cout << "Fast impl took: " << durationMs(timeNow() - t1) << " ms\n";

	printf("%f %f\n", coefsOfFastImpl[100], coefsOfFastImpl[100000]);
	free(coefsOfFastImpl);



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
//        double* coefsOfFastScalar = scalarImpl.interpolateFastestYet(count, x, y);
//
//        std::cout << "Scalar fast-impl took: " << durationMs(timeNow() - t1) << " ms\n";
//        printf("%f %f\n", coefsOfFastScalar[100], coefsOfFastScalar[100000]);
//
//        free(coefsOfFastScalar);


	free(x);
	free(y);
}


/*
 *
 */
int main(int argc, char** argv) {


	simplePerfTest();

	return 0;
}

