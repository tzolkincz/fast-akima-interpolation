/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   fast_akima.h
 * Author: v
 *
 * Created on 19. října 2016, 13:11
 */

#ifndef FAST_AKIMA_H
#define FAST_AKIMA_H

class FastAkima {
public:

	const int MINIMUM_NUMBER_POINTS = 5;
	const int SIMD_WIDTH = 4;


	FastAkima();

	/**
	 * Computes and returns interpolation coefficients of Akima Interpolation.
	 * Coefficients are stored in one array containing coefficients stored in
	 * column fashion. Eg. first coefficients are stored between 0 and count position (inclusive).
	 *
	 * @param count number of input nodes
	 * @param xvals x-vals of input pairs
	 * @param yvals y-vals of input pairs
	 * @return
	 */
	double* interpolate(int count, double* xvals, double* yvals);

private:
	void computeDiffsAndWeights(int count, double* xvals, double* yvals, double* differences, double* weights, double* ysInternalCopy);
	void computeFirstDerivates(int count, double* xvals, double* differences, double* weights, double* firstDerivatives);
	void computeHeadAndTailOfFirstDerivates(int count, double* xvals, double* yvals, double* firstDerivatives);
	void computePolynCoefs(int count, double* xvals, double* coefsOfPolynFunc);
	void computeFirstDerivatesWoTmpArr(int count, double* xvals, double* yvals, double* coefsOfPolynFunc);

};




#endif /* FAST_AKIMA_H */

