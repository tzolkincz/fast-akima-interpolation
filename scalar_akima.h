
#ifndef SCALAR_AKIMA_H
#define SCALAR_AKIMA_H

class ScalarAkima {
public:
	ScalarAkima();
	double* interpolate(int count, double* xvals, double* yvals);

private:
	double differentiateThreePointScalar(double* xvals, double* yvals,
			int indexOfDifferentiation,
			int indexOfFirstSample,
			int indexOfSecondsample,
			int indexOfThirdSample);

	double* interpolateHermiteScalar(int count, double* xvals, double* yvals, double* firstDerivatives);

};


#endif /* SCALAR_AKIMA_H */

