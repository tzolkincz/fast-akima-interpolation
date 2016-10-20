/*
 * File:   newsimpletest.cpp
 * Author: v
 *
 * Created on 19. října 2016, 12:51
 */

#include <stdlib.h>
#include <iostream>
#include <chrono>

#include "../fast_akima.h"
#include "../scalar_akima.h"


#define timeNow() std::chrono::high_resolution_clock::now()
#define durationMs(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()

void printCoefficients(int count, double* coefs);

/*
 * Simple C++ Test Suite
 */


void printCoefficients(int count, double* coefs) {
	for (int i = 0; i < count; i++) {
		printf("A: %f %f %f %f\n", coefs[i], coefs[count + i],
				coefs[2 * count + i], coefs[3 * count + i]);
	}
}

void simpleTest() {
	std::cout << "newsimpletest test 1" << std::endl;

	//init values
	int count = 13;

	double* x = (double*) malloc(sizeof (double) *count);
	double* y = (double*) malloc(sizeof (double) *count);


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
	y[i++] = 3.0;
	y[i++] = 1.0;
	y[i++] = 2.0;
	y[i++] = 3.0;


	FastAkima fastAkimaImpl;
	double* coefsOfFastImpl = fastAkimaImpl.interpolate(count, x, y);


	ScalarAkima scalarImpl;
	double* coefsOfScalar = scalarImpl.interpolate(count, x, y);



	//check diffs:
	bool equals = true;

	for (int i = 0; i < count * 4; i++) {
		if (coefsOfFastImpl[i] - coefsOfScalar[i] > 0.000000001 && i % count != count - 1) {
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

	free(x);
	free(y);
	free(coefsOfFastImpl);
	free(coefsOfScalar);
}



int main(int argc, char** argv) {
	std::cout << "%SUITE_STARTING% newsimpletest" << std::endl;
	std::cout << "%SUITE_STARTED%" << std::endl;

	std::cout << "%TEST_STARTED% test1 (newsimpletest)" << std::endl;
	simpleTest();
	std::cout << "%TEST_FINISHED% time=0 test1 (newsimpletest)" << std::endl;

	std::cout << "%TEST_STARTED% test2 (newsimpletest)\n" << std::endl;
//	simplePerfTest();
	std::cout << "%TEST_FINISHED% time=0 test2 (newsimpletest)" << std::endl;

	std::cout << "%SUITE_FINISHED% time=0" << std::endl;

	return (EXIT_SUCCESS);
}

