/*
 * File:   newsimpletest.cpp
 * Author: v
 *
 * Created on 19. října 2016, 12:51
 */

#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <iomanip>

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


double EPSILON = 0.000000001;

/*
 hustý:
 * I observed an interesting phenomenon when executing 256-bit vector instructions on the Skylake. There is a warm-up period of approximately 14 µs before it can execute 256-bit vector instructions at full speed. Apparently, the upper 128-bit half of the execution units and data buses is turned off in order to save power when it is not used. As soon as the processor sees a 256-bit instruction it starts to power up the upper half. It can still execute 256-bit instructions during the warm-up period, but it does so by using the lower 128-bit units twice for every 256-bit vector. The result is that the throughput for 256-bit vectors is 4-5 times slower during this warm-up period. If you know in advance that you will need to use 256-bit instructions soon, then you can start the warm-up process by placing a dummy 256-bit instruction at a strategic place in the code. My measurements showed that the upper half of the units is shut down again after 675 µs of inactivity.
 * http://www.agner.org/optimize/blog/read.php?i=415
 *
 *
 myšlenka skývání latence: (dokonce i přímo u hornera)
 * https://www.yeppp.info/resources/ppam-presentation.pdf
 */

/**
 * Horner's evaluation method
 *
 * @param knotIndex
 * @param size
 * @param coefs
 * @param argument
 * @return
 */
double getValue(int knotIndex, int size, double* coefs, double argument) {

	//@TODO use FMA instructions
	double res = coefs[size * 3 + knotIndex];

	res = argument * res + coefs[size * 2 + knotIndex];
	res = argument * res + coefs[size * 1 + knotIndex];
	res = argument * res + coefs[size * 0 + knotIndex];

	//	for (int j = 2; j >= 0; j--) {
	//		res = argument * res + coefs[size * j  + knotIndex];
	//	}

	return res;
}

double getInterpolation(int size, double* xvals, double * coefs, double argument) {

	int knotIndex = -1;
	for (int i = 0; i < size; i++) {
		if (xvals[i] > argument) {
			knotIndex = i - 1;
			break;
		}
	}


	return getValue(knotIndex, size, coefs, argument - xvals[knotIndex]);
}

void simpleTest() {
	std::cout << "newsimpletest test 1" << std::endl;




	//init values
	const int count = 24;


	//precomputed, verified values
	double correctInterpol[count * 4] = {1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.25, 3.5, 3.75, 4,
		3.625, 2.5, 1.375, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.2734375, 3.5625, 3.8203125, 4,
		4.0703125, 4.0625, 4.0234375, 4, 4, 4, 4, 4, 3.484375, 2.375, 1.328125, 1, 1.25, 1.5, 1.75, 2,
		2.25, 2.5, 2.75, 3, 3.37109375, 3.90625, 4.48828125, 5, 5.390625, 5.6875, 5.890625, 6, 5.74609375,
		5.09375, 4.39453125, 4, 4.1234375, 4.5375, 4.9328125, 5, 4.5984375, 3.9625, 3.3453125, 3,
		2.9484375, 2.9875, 3.0328125, 3, 2.7640625, 2.3875, 2.0671875, 2, 2.3125, 2.875, 3.5, 4, 4.3671875,
		4.6875, 4.9140625, 5, 4.734375, 4.125, 3.453125, 3, 2.75, 2.5, 2.25, 598, 610.3125, 622.75, 635.3125};

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
	//	y[i++] = 2.0;

	FastAkima fastAkimaImpl;
	double* coefsOfFastImpl = fastAkimaImpl.interpolate(count, x, y);


	ScalarAkima scalarImpl;
	double* coefsOfScalar = scalarImpl.interpolate(count, x, y);



	//check diffs:
	bool equals = true;

	for (int i = 0; i < count * 4; i++) {
		if (coefsOfFastImpl[i] - coefsOfScalar[i] > EPSILON && i % count != count - 1) {
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
	for (int i = 0; i < count; i++) {
		for (double j = 0.0; j < 1; j += 0.25) {
			if (correctInterpol[k] - getInterpolation(count, x, coefsOfFastImpl, i + j) > EPSILON) {
				std::cout << correctInterpol[k] << " " << correctInterpol[k] - getInterpolation(count, x, coefsOfFastImpl, i + j) << "\n";
				std::cout << i + j << "," << getInterpolation(count, x, coefsOfFastImpl, i + j) << "\n";
				std::cout << "%TEST_FAILED% time=0 testname=simpleTest (newsimpletest) message="
						"results of interpolation are not correct " << std::endl;
			}

			k++;
			//std::cout << std::setprecision(10) << getInterpolation(count, x, coefsOfFastImpl, i + j) << ", ";
			//std::cout << i + j << "," << getInterpolation(count, x, coefsOfFastImpl, i + j) << "\n";
		}
	}

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

