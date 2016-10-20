/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   scalar_akima.h
 * Author: v
 *
 * Created on 20. října 2016, 9:57
 */

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

