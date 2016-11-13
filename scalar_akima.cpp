
#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include "scalar_akima.h"

ScalarAkima::ScalarAkima() {

}

double ScalarAkima::differentiateThreePointScalar(double* xvals, double* yvals,
		int indexOfDifferentiation, //0, 1, -2, -1
		int indexOfFirstSample, //0, 0, -3, -3
		int indexOfSecondsample, //1, 1, -2, -2
		int indexOfThirdSample) { //2, 2, -1, -1
	double x0 = yvals[indexOfFirstSample];
	double x1 = yvals[indexOfSecondsample];
	double x2 = yvals[indexOfThirdSample];

	double t = xvals[indexOfDifferentiation] - xvals[indexOfFirstSample];
	double t1 = xvals[indexOfSecondsample] - xvals[indexOfFirstSample];
	double t2 = xvals[indexOfThirdSample] - xvals[indexOfFirstSample];

	double a = (x2 - x0 - (t2 / t1 * (x1 - x0))) / (t2 * t2 - t1 * t2);
	double b = (x1 - x0 - a * t1 * t1) / t1;

	return (2 * a * t) +b;
}

double* ScalarAkima::interpolateHermiteScalar(int count, double* xvals, double* yvals, double* coefsOfPolynFunc) {

	double* firstDerivatives = &coefsOfPolynFunc[count];
	int numberOfDiffAndWeightElements = count - 1;

	int dimSize = count;
	for (int i = 0; i < numberOfDiffAndWeightElements; i++) {
		double w = xvals[i + 1] - xvals[i];
		double w2 = w * w;

		double yv = yvals[i];
		double yvP = yvals[i + 1];

		double fd = firstDerivatives[i];
		double fdP = firstDerivatives[i + 1];

		double divTmp = (yv - yvP) / w;

		coefsOfPolynFunc[2 * dimSize + i] = (-3 * divTmp - 2 * fd - fdP) / w;
		coefsOfPolynFunc[3 * dimSize + i] = (2 * divTmp + fd + fdP) / w2;
	}

	return coefsOfPolynFunc;
}

double* ScalarAkima::interpolate(int count, double* xvals, double* yvals) {

	double* coefsOfPolynFunc = (double*) malloc(sizeof (double) * 4 * count);
	double* firstDerivatives = &coefsOfPolynFunc[count];
//alglib mklib popř octave/matlab/wolfram gcc
//get levels rozhrani pro vektorovy instrukce (dvojice s konstantim offsetem a ty maji nekonstanti vzdalenost)

	coefsOfPolynFunc[0] = yvals[0];
	coefsOfPolynFunc[1] = yvals[1];

	double d1 = (yvals[1] - yvals[0]) / (xvals[1] - xvals[0]);
	double d2 = (yvals[2] - yvals[1]) / (xvals[2] - xvals[1]);

	double d3 = (yvals[3] - yvals[2]) / (xvals[3] - xvals[2]);
	double d4 = (yvals[4] - yvals[3]) / (xvals[4] - xvals[3]);


	double w1 = fabs(d1 - d2);
	double w2 = fabs(d2 - d3);
	double w3 = fabs(d3 - d4);

	double fd, fdPrev;


	auto computeFd = [&] (int i) {
		if (FP_ZERO == fpclassify(w3) && FP_ZERO == fpclassify(w1)) {
			double xv = xvals[i];		// no need to optimize this,
			double xvP = xvals[i + 1];	// expecting to be very rare case
			double xvM = xvals[i - 1];
			return (((xvP - xv) * d2) + ((xv - xvM) * d3)) / (xvP - xvM);
		} else {
			return ((w3 * d2) + (w1 * d3)) / (w3 + w1);
		}
	};

	auto compute3th4thCoefs = [&] (int i) {
		//copute 3. and 4. coeficients
			double w = xvals[i] - xvals[i - 1];
			double w_2 = w * w;

			double yv = yvals[i-1];
			double yvP = yvals[i];

			//saving one division
			double divTmp = (yv - yvP) / w;
			coefsOfPolynFunc[2 * count + i - 1] = (-3 * divTmp - 2 * fdPrev - fd) / w;
			coefsOfPolynFunc[3 * count + i - 1] = (2 * divTmp + fdPrev + fd) / w_2;
	};


	for (int i = 2; i < count - 2 - 1; i++) {
		coefsOfPolynFunc[i] = yvals[i];

		fd = computeFd(i);

		if(i!=2) { //is not first loop
			compute3th4thCoefs(i);
		}

		fdPrev = fd;
		firstDerivatives[i] = fd;

		d1 = d2;
		d2 = d3;
		d3 = d4;
		d4 = (yvals[3 + i] - yvals[2 + i]) / (xvals[3 + i] - xvals[2 + i]);

		w1 = w2;
		w2 = w3;
		w3 = fabs(d3 - d4);
	}

	int i  = count - 3;
	//last iteration
	coefsOfPolynFunc[i] = yvals[i];
	fd = computeFd(i);

	compute3th4thCoefs(i);

	fdPrev = fd;
	firstDerivatives[i] = fd;

	//compute last elements of 3. and 4. coefs
	i++;
	compute3th4thCoefs(i);

	coefsOfPolynFunc[count - 2] = yvals[count - 2];
	coefsOfPolynFunc[count - 1] = yvals[count - 1];

	firstDerivatives[0] = differentiateThreePointScalar(xvals, yvals, 0, 0, 1, 2);
	firstDerivatives[1] = differentiateThreePointScalar(xvals, yvals, 1, 0, 1, 2);
	firstDerivatives[count - 2] = differentiateThreePointScalar(xvals, yvals, count - 2,
			count - 3, count - 2, count - 1);
	firstDerivatives[count - 1] = differentiateThreePointScalar(xvals, yvals, count - 1,
			count - 3, count - 2, count - 1);

	return interpolateHermiteScalar(count, xvals, yvals, coefsOfPolynFunc);
}
