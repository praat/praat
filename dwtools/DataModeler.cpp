/* DataModeler.cpp
 *
 * Copyright (C) 2014-2016 David Weenink, 2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * ainteger with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20140217
*/

#include "DataModeler.h"
#include "NUM2.h"
#include "NUMmachar.h"
#include "SVD.h"
#include "Strings_extensions.h"
#include "Sound_and_LPC_robust.h"
#include "Table_extensions.h"
#include "tensor.h"

#include "oo_DESTROY.h"
#include "DataModeler_def.h"
#include "oo_COPY.h"
#include "DataModeler_def.h"
#include "oo_EQUAL.h"
#include "DataModeler_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "DataModeler_def.h"
#include "oo_WRITE_TEXT.h"
#include "DataModeler_def.h"
#include "oo_WRITE_BINARY.h"
#include "DataModeler_def.h"
#include "oo_READ_TEXT.h"
#include "DataModeler_def.h"
#include "oo_READ_BINARY.h"
#include "DataModeler_def.h"
#include "oo_DESCRIPTION.h"
#include "DataModeler_def.h"

extern machar_Table NUMfpp;

Thing_implement (DataModeler, Function, 0);

void structDataModeler :: v_info () {
	MelderInfo_writeLine (U"   Time domain:");
	MelderInfo_writeLine (U"      Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"      End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"      Total duration: ", xmax - xmin, U" seconds");
	double ndf, rSquared = DataModeler_getCoefficientOfDetermination (this, nullptr, nullptr);
	double probability, chisq = DataModeler_getChiSquaredQ (this, useSigmaY, &probability, &ndf);
	MelderInfo_writeLine (U"   Fit:");
	MelderInfo_writeLine (U"      Number of data points: ", numberOfDataPoints);
	MelderInfo_writeLine (U"      Number of parameters: ", numberOfParameters);
	MelderInfo_writeLine (U"      Each data point has ", useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? U" the same weight (estimated)." :
		useSigmaY == DataModeler_DATA_WEIGH_SIGMA ? U"a different weight (sigmaY)." : 
		useSigmaY == DataModeler_DATA_WEIGH_RELATIVE ? U"a different relative weight (Y_value/sigmaY)." :
		U"a different weight (SQRT(sigmaY)).");
	MelderInfo_writeLine (U"      Chi squared: ", chisq);
	MelderInfo_writeLine (U"      Number of degrees of freedom: ", ndf);
	MelderInfo_writeLine (U"      Probability: ", probability);
	MelderInfo_writeLine (U"      R-squared: ", rSquared);
	for (integer ipar = 1; ipar <= numberOfParameters; ipar ++) {
		double sigma = parameterStatus [ipar] == DataModeler_PARAMETER_FIXED ? 0 : sqrt (parameterCovariances -> data [ipar] [ipar]);
		MelderInfo_writeLine (U"      p [", ipar, U"] = ", parameter [ipar], U"; sigma = ", sigma);
	}
}

static double polynomial_evaluate (DataModeler me, double xin, double p [])
{
	double xpi = 1.0, result = p [1];
	// From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	double x = (2.0 * xin - my xmin - my xmax) / 2.0;
	for (integer i = 2; i <= my numberOfParameters; i ++) {
		xpi *= x;
		result += p [i] * xpi;
	}
	return result;
}

static void polynomial_evaluateBasisFunctions (DataModeler me, double xin, double term []) {
	term [1] = 1.0;
	// From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	double x = (2.0 * xin - my xmin - my xmax) / 2.0;
	for (integer i = 2; i <= my numberOfParameters; i ++) {
		term [i] = term [i-1] * x;
	}
}

static double legendre_evaluate (DataModeler me, double xin, double p []) {
	// From domain [xmin, xmax] to domain [-1, 1]
	double x = (2.0 * xin - my xmin - my xmax) / (my xmax - my xmin);
	double pti, ptim1, ptim2 = 1.0, result = p [1];
	if (my numberOfParameters > 1) {
		double twox = 2.0 * x, f2 = x, d = 1.0;
		result += p [2] * (ptim1 = x);
		for (integer i = 3; i <= my numberOfParameters; i ++) {
			double f1 = d ++;
			f2 += twox;
			result += p [i] * (pti = (f2 * ptim1 - f1 * ptim2) / d);
			ptim2 = ptim1;
			ptim1 = pti;
		}
	}
	return result;
}

static void legendre_evaluateBasisFunctions (DataModeler me, double xin, double term []) {
	term [1] = 1.0;
	/* transform x from domain [xmin, xmax] to domain [-1, 1] */
	double x = (2.0 * xin - my xmin - my xmax) / (my xmax - my xmin);
	if (my numberOfParameters > 1) {
		double twox = 2.0 * x, f2 = term [2] = x, d = 1.0;
		for (integer i = 3; i <= my numberOfParameters; i ++) {
			double f1 = d ++;
			f2 += twox;
			term [i] = (f2 * term [i-1] - f1 * term [i-2]) / d;
		}
	}
}

static void chisqFromZScores (double *zscores, integer numberOfZScores, double *p_chisq, integer *p_numberOfValidZScores) {
	integer numberOfValidZScores = numberOfZScores;
	double chisq = 0.0;
	for (integer i = 1; i <= numberOfZScores; i ++) {
		if (isdefined (zscores [i])) {
			chisq += zscores [i] * zscores [i];
		} else {
			numberOfValidZScores--;
		}
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_numberOfValidZScores) {
		*p_numberOfValidZScores = numberOfValidZScores;
	}
}

static double DataModeler_getDataPointInverseWeight (DataModeler me, integer iPoint, int useSigmaY ) {
	double iweight = 1.0;
	if (iPoint > 0 && iPoint <= my numberOfDataPoints && my dataPointStatus [iPoint] != DataModeler_DATA_INVALID) {
		if (useSigmaY == DataModeler_DATA_WEIGH_SIGMA) {
			iweight = my sigmaY [iPoint];
		} else if (useSigmaY == DataModeler_DATA_WEIGH_RELATIVE) {
			double q = my y [iPoint] / my sigmaY [iPoint];
			iweight = 500.0 / q; //
		} else if (useSigmaY == DataModeler_DATA_WEIGH_SQRT) {
			iweight = 7.071 * sqrt (my sigmaY [iPoint]); // .bw = 50 gives 50
		}
	}
	return iweight;
}

double DataModeler_getModelValueAtX (DataModeler me, double x) {
	double f = undefined;
	if (x >= my xmin && x <= my xmax) {
		f = my f_evaluate (me, x, my parameter);
	}
	return f;
}

double DataModeler_getModelValueAtIndex (DataModeler me, integer index) {
	double f = undefined;
	if (index > 0 && index <= my numberOfDataPoints) {
		f = my f_evaluate (me, my x [index], my parameter);
	}
	return f;
}

void DataModeler_getExtremaY (DataModeler me, double *p_ymin, double *p_ymax) {
	double ymin = 1e308, ymax = -ymin;
	for (integer i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
			if (my y [i] < ymin) {
				ymin = my y [i];
			}
			if (my y [i] > ymax) {
				ymax = my y [i];
			}
		}
	}
	if (p_ymin) {
		*p_ymin = ymin;
	}
	if (p_ymax) {
		*p_ymax = ymax;
	}
}

double DataModeler_getDataPointYValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfDataPoints && my dataPointStatus [index] != DataModeler_DATA_INVALID) {
		value = my y [index];
	}
	return value;
}

double DataModeler_getDataPointXValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfDataPoints && my dataPointStatus [index] != DataModeler_DATA_INVALID) {
		value = my x [index];
	}
	return value;
}

void DataModeler_setDataPointYValue (DataModeler me, integer index, double value) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my y [index] = value;
	}
}

void DataModeler_setDataPointXValue (DataModeler me, integer index, double value) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my x [index] = value;
	}
}

void DataModeler_setDataPointValues (DataModeler me, integer index, double xvalue, double yvalue) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my x [index] = xvalue;
		my y [index] = yvalue;
	}
}

void DataModeler_setDataPointYSigma (DataModeler me, integer index, double sigma) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my sigmaY [index] = sigma;
	}
}

double DataModeler_getDataPointYSigma (DataModeler me, integer index) {
	double sigma = undefined;
	if (index > 0 && index <= my numberOfDataPoints) {
		sigma = my sigmaY [index];
	}
	return sigma;
}

int DataModeler_getDataPointStatus (DataModeler me, integer index) {
	int value = DataModeler_DATA_INVALID;
	if (index > 0 && index <= my numberOfDataPoints) {
		value = my dataPointStatus [index];
	}
	return value;
}

void DataModeler_setDataPointStatus (DataModeler me, integer index, int status) {
	if (index > 0 && index <= my numberOfDataPoints) {
		if (status == DataModeler_DATA_VALID && isundef (my y [index])) {
			Melder_throw (U"Your data value is undefined. First set the value and then its status.");
		}
		my dataPointStatus [index] = status;
	}
}

static void DataModeler_setDataPointValueAndStatus (DataModeler me, integer index, double value, int dataStatus) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my y [index] = value;
		my dataPointStatus [index] = dataStatus;
	}
}

void DataModeler_setParameterValue (DataModeler me, integer index, double value, int status) {
	if (index > 0 && index <= my numberOfParameters) {
		my parameter [index] = value;
		my parameterStatus [index] = status;
	}
}

void DataModeler_setParameterValueFixed (DataModeler me, integer index, double value) {
	DataModeler_setParameterValue (me, index, value, DataModeler_PARAMETER_FIXED);
}

double DataModeler_getParameterValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfParameters) {
		value = my parameter [index];
	}
	return value;
}

int DataModeler_getParameterStatus (DataModeler me, integer index) {
	int status = DataModeler_PARAMETER_UNDEFINED;
	if (index > 0 && index <= my numberOfParameters) {
		status = my parameterStatus [index];
	}
	return status;
}

double DataModeler_getParameterStandardDeviation (DataModeler me, integer index) {
	double stdev = undefined;
	if (index > 0 && index <= my numberOfParameters) {
		stdev = sqrt (my parameterCovariances -> data [index] [index]);
	}
	return stdev;
}

double DataModeler_getVarianceOfParameters (DataModeler me, integer fromIndex, integer toIndex, integer *p_numberOfFreeParameters) {
	double variance = undefined;
	if (toIndex < fromIndex || (toIndex == 0 && fromIndex == 0)) {
		fromIndex = 1; toIndex = my numberOfParameters;
	}
	integer numberOfFreeParameters = 0;
	if (fromIndex <= toIndex && fromIndex > 0 && toIndex <= my numberOfParameters) {
		variance = 0;
		for (integer index = fromIndex; index <= toIndex; index ++) {
			if (my parameterStatus [index] != DataModeler_PARAMETER_FIXED) {
				variance += my parameterCovariances -> data [index] [index];
				numberOfFreeParameters ++;
			}
		}
	}
	if (p_numberOfFreeParameters) {
		*p_numberOfFreeParameters = numberOfFreeParameters;
	}
	return variance;
}

void DataModeler_setParametersFree (DataModeler me, integer fromIndex, integer toIndex) {
	if (toIndex < fromIndex || (toIndex == 0 && fromIndex == 0)) {
		fromIndex = 1; toIndex = my numberOfParameters;
	}
	if (fromIndex <= toIndex && fromIndex > 0 && toIndex <= my numberOfParameters) {
		for (integer index = fromIndex; index <= toIndex; index ++) {
			my parameterStatus [index] = DataModeler_PARAMETER_FREE;
		}
	}
}

void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas) {
	integer numberOfChangedParameters = 0;
	for (integer i = my numberOfParameters; i > 0; i --) {
		if (my parameterStatus [i] != DataModeler_PARAMETER_FIXED) {
			double value = my parameter [i];
			double sigmas = numberOfSigmas * DataModeler_getParameterStandardDeviation (me, i);
			if ((value - sigmas) * (value + sigmas) < 0) {
				DataModeler_setParameterValueFixed (me, i, 0.0);
				numberOfChangedParameters ++;
			}
		}
	}
}

static integer DataModeler_getNumberOfFreeParameters (DataModeler me) {
	integer numberOfFreeParameters = 0;
	for (integer i = 1; i <= my numberOfParameters; i ++) {
		if (my parameterStatus [i] == DataModeler_PARAMETER_FREE) {
			numberOfFreeParameters ++;
		}
	}
	return numberOfFreeParameters;
}

integer DataModeler_getNumberOfFixedParameters (DataModeler me) {
	return my numberOfParameters - DataModeler_getNumberOfFreeParameters (me);
}

static integer DataModeler_getNumberOfValidDataPoints (DataModeler me) {
	integer numberOfValidDataPoints = 0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
			numberOfValidDataPoints ++;
		}
	}
	return numberOfValidDataPoints;
}

integer DataModeler_getNumberOfInvalidDataPoints (DataModeler me) {
	return my numberOfDataPoints - DataModeler_getNumberOfValidDataPoints  (me);
}

void DataModeler_setTolerance (DataModeler me, double tolerance) {
	my tolerance = tolerance > 0.0 ? tolerance : my numberOfDataPoints * NUMfpp -> eps;
}

double DataModeler_getDegreesOfFreedom (DataModeler me) {
	integer numberOfDataPoints = 0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
			numberOfDataPoints ++;
		}
	}
	double ndf = numberOfDataPoints - DataModeler_getNumberOfFreeParameters (me);
	return ndf;
}

void DataModeler_getZScores (DataModeler me, int useSigmaY, double zscores []) {
	try {
		Melder_assert (zscores != nullptr);
		double estimatedSigmaY;
		if (useSigmaY == DataModeler_DATA_WEIGH_EQUAL) {
			integer numberOfValidDataPoints;
			double rss = DataModeler_getResidualSumOfSquares (me, & numberOfValidDataPoints);
			Melder_require (numberOfValidDataPoints > 1, U"Not enough data points to calculate sigma.");
			estimatedSigmaY = rss / (numberOfValidDataPoints - 1);
		}
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			double value = undefined;
			if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
				double estimate = my f_evaluate (me, my x [i], my parameter);
				double sigma = useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? estimatedSigmaY : DataModeler_getDataPointInverseWeight (me, i, useSigmaY);
				value = (my y [i] - estimate) / sigma;
			}
			zscores [i] = value;
		}
	} catch (MelderError) {
		Melder_throw (U"No z-scores calculated.");
	}
}

// chisq and zscores may be the same arrays!
static void DataModeler_getChisqScoresFromZScores (DataModeler me, double *zscores, bool substituteAverage, double *chisq) {
	Melder_assert (zscores != nullptr && chisq != nullptr);
	integer numberOfDefined = my numberOfDataPoints;
	double sumchisq = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (isdefined (zscores [i])) {
			chisq [i] = zscores [i] * zscores [i];
			sumchisq += chisq [i];
		} else {
			numberOfDefined --;
			chisq [i] = undefined;
		}
	}
	if (substituteAverage && numberOfDefined != my numberOfDataPoints && numberOfDefined > 0) {
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			if (isundef (chisq [i])) {
				chisq [i] = sumchisq / numberOfDefined;
			}
		}
	}
}

double DataModeler_getChiSquaredQ (DataModeler me, int useSigmaY, double *p_prob, double *p_df)
{
	double chisq;
	integer numberOfValidZScores;
	autoNUMvector<double> zscores (1, my numberOfDataPoints);
	DataModeler_getZScores (me, useSigmaY, zscores.peek());
	chisqFromZScores (zscores.peek(), my numberOfDataPoints, & chisq, & numberOfValidZScores);
	double df = useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? numberOfValidZScores - 1.0 : numberOfValidZScores; // we loose one df if sigma is estimated from the data
	if (p_prob) {
		*p_prob = NUMchiSquareQ (chisq, df);
	}
	if (p_df) {
		*p_df = df;
	}
	return chisq;
}

double DataModeler_getWeightedMean (DataModeler me) {
	double ysum = 0.0, wsum = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
			double s = DataModeler_getDataPointInverseWeight (me, i, my useSigmaY);
			double weight = 1.0 / (s * s);
			ysum += my y [i] * weight;
			wsum += weight;
		}
	}
	return ysum / wsum;
}

double DataModeler_getCoefficientOfDetermination (DataModeler me, double *p_ssreg, double *p_sstot) {

	/* We cannot use the standard expressions for ss_tot, and ss_reg because our data are weighted by 1 / sigma [i].
	 * We need the weighted mean and we need to weigh all sums-of-squares accordingly;
	 * if all sigma [i] terms are equal, the formulas reduce to the standard ones.
	 * Ref: A. Buse (1973): Goodness of Fit in Generalized Least Squares Estimation, The American Statician, vol 27, 106-108
	 */

	double ymean = DataModeler_getWeightedMean (me);
	double sstot = 0.0, ssreg = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
			double s = DataModeler_getDataPointInverseWeight (me, i, my useSigmaY);
			double diff = (my y [i] - ymean) / s;
			sstot += diff * diff; // total sum of squares
			double estimate = my f_evaluate (me, my x [i], my parameter);
			diff = (estimate - my y [i]) / s;
			ssreg += diff * diff; // regression sum of squares
		}
	}
	double rSquared = sstot > 0.0 ? 1.0 - ssreg / sstot : 1.0;
	if (p_ssreg) {
		*p_ssreg = sstot - ssreg;
	}
	if (p_sstot) {
		*p_sstot = sstot;
	}
	return rSquared;
}

static void DataModeler_drawBasisFunction_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, integer iterm, bool scale, integer numberOfPoints) {
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax;
	}
	autoNUMvector<double> x (1, numberOfPoints);
	autoNUMvector<double> y (1, numberOfPoints);
	autoNUMvector<double> term (1, my numberOfParameters);
	for (integer i = 1; i <= numberOfPoints; i ++) {
		x [i] = xmin + (i - 0.5) * (xmax - xmin) / numberOfPoints;
		my f_evaluateBasisFunctions (me, x [i], term.peek());
		y [i] = term [iterm];
		y [i] = scale ? y [i] * my parameter [iterm] : y [i];
	}
	if (ymax <= ymin) {
		ymin = 1e308; ymax = -ymin;
		for (integer i = 1; i <= numberOfPoints; i ++) {
			ymax = y [i] > ymax ? y [i] : ymax;
			ymin = y [i] < ymin ? y [i] : ymin;
		}
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (integer i = 2; i <= numberOfPoints; i ++) {
		Graphics_line (g, x [i-1], y [i-1], x [i], y [i]);
	}
}

static integer DataModeler_drawingSpecifiers_x (DataModeler me, double *xmin, double *xmax, integer *ixmin, integer *ixmax) {
	if (*xmax <= *xmin) {
		*xmin = my xmin; *xmax = my xmax;
	}
	*ixmin = 2;
	while (my x [*ixmin] < *xmin && *ixmin < my numberOfDataPoints) {
		(*ixmin) ++;
	}
	(*ixmin) --;

	*ixmax = my numberOfDataPoints - 1;
	while (my x [*ixmax] > *xmax && *ixmax > 1) {
		(*ixmax) --;
	}
	(*ixmax) ++;
	return *ixmax - *ixmin + 1;
}

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, int useSigmaY, char32 *mark, int marksFontSize, double horizontalOffset_mm) {
	integer ixmin, ixmax;
	if (DataModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) < 1) return;
	autoNUMvector<double> zscores (1, my numberOfDataPoints);
	DataModeler_getZScores (me, useSigmaY, zscores.peek());
	double horizontalOffset_wc = Graphics_dxMMtoWC (g, horizontalOffset_mm);
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setFontSize (g, marksFontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	int currentFontSize = Graphics_inqFontSize (g);
	for (integer idata = 1; idata <= my numberOfDataPoints; idata ++) {
		if (my dataPointStatus [idata] != DataModeler_DATA_INVALID) {
			double x = my x [idata], y = my y [idata];
			if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
				if (fabs (zscores [idata]) > numberOfSigmas) {
					Graphics_text (g, x + horizontalOffset_wc, y, mark);
				}
			}
		}
	}
	Graphics_setFontSize (g, currentFontSize);
}

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, int errorbars, int connectPoints, double barWidth_mm, double horizontalOffset_mm, int drawDots)
{
	if (xmax <= xmin) { 
		xmin = my xmin; xmax = my xmax; 
	}
	
	integer ixmin = 2;
	while (my x [ixmin] < xmin && ixmin < my numberOfDataPoints) { 
		ixmin ++;
	}
	ixmin --;
	
	integer ixmax = my numberOfDataPoints - 1;
	while (my x [ixmax] > xmax && ixmax > 1) {
		ixmax --;
	}
	ixmax ++;
	
	if (ixmin >= ixmax) {
		return; // nothing to draw
	}
	numberOfParameters = numberOfParameters > my numberOfParameters ? my numberOfParameters : numberOfParameters;
	autoNUMvector<double> parameter (1, my numberOfParameters);
	NUMvector_copyElements (my parameter, parameter.peek(), 1, numberOfParameters);
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double horizontalOffset_wc = Graphics_dxMMtoWC (g, horizontalOffset_mm);
	double barWidth_wc = barWidth_mm <= 0.0 ? 0.0 : Graphics_dxMMtoWC (g, barWidth_mm);
	double x1, y1, x2, y2;
	bool x1defined = false, x2defined = false;
	for (integer idata = ixmin; idata <= ixmax; idata ++) {
		if (my dataPointStatus [idata] != DataModeler_DATA_INVALID) {
			double x = my x [idata], y = my y [idata];
			if (! x1defined) {
				x1 = x;
				y1 = estimated ? my f_evaluate (me, x, parameter.peek()) : y;
				x1defined = true;
			} else {
				x2 = x;
				y2 = estimated ? my f_evaluate (me, x, parameter.peek()) : y;
				x2defined = true;
			}
			if (x1defined && drawDots) {
				if (y >= ymin && y <= ymax) {
					Graphics_speckle (g, x + horizontalOffset_wc, y);
				}
			}
			if (x2defined) { // if (x1defined && x2defined)
				if (connectPoints) {
					double xo1, yo1, xo2, yo2;
					if (NUMclipLineWithinRectangle (x1 + horizontalOffset_wc, y1, x2 + horizontalOffset_wc, y2,
						xmin, ymin, xmax, ymax, & xo1, & yo1, & xo2, & yo2)) {
						Graphics_line (g, xo1, yo1, xo2, yo2);
					}
					// Graphics_line (g, x1 + horizontalOffset_wc, y1, x2 + horizontalOffset_wc, y2);
				}
				x1 = x;
				y1 = y2;
			}
			if (x1defined && errorbars != 0) {
				double sigma = my sigmaY [idata]; // DataModeler_getDataPointInverseWeight ?
				double ym = y1;
				double yt = ym + 0.5 * sigma, yb = ym - 0.5 * sigma;
				if (estimated) {
					yt = (y - y1) > 0.0 ? y : y1;
					yb = (y - y1) > 0.0 ? y1 : y;
				}
				bool topOutside = yt > ymax, bottomOutside = yb < ymin;
				yt = topOutside ? ymax : yt;
				yb = bottomOutside ? ymin : yb;
				Graphics_line (g, x1 + horizontalOffset_wc, yb, x1 + horizontalOffset_wc, yt);
				if (barWidth_wc > 0.0 && ! estimated) {
					double xl = x1 - 0.5 * barWidth_wc + horizontalOffset_wc;
					double xr = xl + barWidth_wc;
					if (! topOutside) {
						Graphics_line (g, xl, yt, xr, yt);
					}
					if (! bottomOutside) {
						Graphics_line (g, xl, yb, xr, yb);
					}
				}
			}
		}
	}
}

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, double horizontalOffset_mm)
{
	int errorbars = 0, connectPoints = 1; double barWidth_mm = 0;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, horizontalOffset_mm, 0);
}

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, double horizontalOffset_mm, int garnish) {
	if (ymax <= ymin) {
		DataModeler_getExtremaY (me, &ymin, &ymax);
	}
	Graphics_setInner (g);
	DataModeler_drawTrack_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm)
{
	int connectPoints = 0;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, horizontalOffset_mm, 1);
}

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm, int garnish) {
	if (ymax <= ymin) {
		DataModeler_getExtremaY (me, &ymin, &ymax);
	}
	Graphics_setInner (g);
	DataModeler_speckle_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, barWidth_mm, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoTable DataModeler_to_Table_zscores (DataModeler me, int useSigmaY) {
	try {
		autoTable ztable = Table_createWithColumnNames (my numberOfDataPoints, U"x z");
		autoNUMvector<double> zscores (1, my numberOfDataPoints);
		DataModeler_getZScores (me, useSigmaY, zscores.peek());
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			Table_setNumericValue (ztable.get(), i, 1, my x [i]);
			Table_setNumericValue (ztable.get(), i, 2, zscores [i]);
		}
		return ztable;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

static void DataModeler_normalProbabilityPlot (DataModeler me, Graphics g, int useSigmaY, integer numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, bool garnish) {
	try {
		autoTable thee = DataModeler_to_Table_zscores (me, useSigmaY);
		Table_normalProbabilityPlot (thee.get(), g, 2, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	} catch (MelderError) {
		// ignore
	}
}

void DataModeler_setBasisFunctions (DataModeler me, int type) {
	if (type == DataModeler_TYPE_LEGENDRE) {
		my f_evaluate = legendre_evaluate;
		my f_evaluateBasisFunctions = legendre_evaluateBasisFunctions;
	} else {
		my f_evaluate = polynomial_evaluate;
		my f_evaluateBasisFunctions = polynomial_evaluateBasisFunctions;
	}
	my type = type;
}

void DataModeler_init (DataModeler me, double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, int type) {
	my xmin = xmin;
	my xmax = xmax;
	DataModeler_setBasisFunctions (me, type);
	my numberOfDataPoints = numberOfDataPoints;
	my x = NUMvector<double> (1, numberOfDataPoints);
	my y = NUMvector<double> (1, numberOfDataPoints);
	my sigmaY = NUMvector<double> (1, numberOfDataPoints);
	my dataPointStatus = NUMvector<int> (1, numberOfDataPoints);
	my numberOfParameters = numberOfParameters;
	
	Melder_require (numberOfParameters > 0, U"The number of parameters should be greater than zero.");
	Melder_require (numberOfParameters <= numberOfDataPoints, U"The number of parameters should not exceed the number of data points");
	
	my parameter = NUMvector<double> (1, numberOfParameters);
	my parameterStatus = NUMvector<int> (1, numberOfParameters);
	my parameterNames = Strings_createFixedLength (numberOfParameters);
	my parameterCovariances = Covariance_create (numberOfParameters);
}

autoDataModeler DataModeler_create (double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, int type) {
	try {
		autoDataModeler me = Thing_new (DataModeler);
		DataModeler_init (me.get(), xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		my xmin = xmin; my xmax = xmax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}
}

autoDataModeler DataModeler_createSimple (double xmin, double xmax, integer numberOfDataPoints, char32 *parameters, double gaussianNoiseStd, int type) {
	try {
		integer numberOfParameters;
		autoNUMvector <double> parameter (NUMstring_to_numbers (parameters, & numberOfParameters), 1);
		Melder_require (numberOfParameters > 0, U"At least one parameter should be defined.");
		Melder_require (xmin < xmax, U"The domain should be defined properly.");
		
		autoDataModeler me = DataModeler_create (xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		for (integer i = 1; i <= numberOfParameters; i ++) {
			my parameter [i] = parameter [i]; // parameter status ok
		}
		// generate the data that beinteger to the parameter values
		for (integer i = 1; i <= numberOfDataPoints; i ++) {
			my x [i] = xmin + (i - 0.5) * (xmax - xmin) / numberOfDataPoints;
			double modelY = my f_evaluate (me.get(), my x [i], my parameter);
			my y [i] = modelY + NUMrandomGauss (0.0, gaussianNoiseStd);
			my sigmaY [i] = undefined;
		}
		my useSigmaY = DataModeler_DATA_WEIGH_EQUAL;
		return me;
	} catch (MelderError) {
		Melder_throw (U"No simple DataModeler created.");
	}
}

void DataModeler_fit (DataModeler me)
{
	try {
		// Count the number of parameters to be fitted

		integer numberOfParameters = DataModeler_getNumberOfFreeParameters (me);
		if (numberOfParameters == 0) return;
		integer numberOfDataPoints = DataModeler_getNumberOfValidDataPoints (me);
		autoNUMvector<double> b (1, numberOfDataPoints);
		autoNUMvector<double> term (1, my numberOfParameters);
		autoNUMvector<double> parameter (1, my numberOfParameters);
		autoNUMmatrix<double> design (1, numberOfDataPoints, 1, numberOfParameters);

		// For function evaluation with only the FIXED parameters

		for (integer ipar = 1; ipar <= my numberOfParameters; ipar ++) {
			parameter [ipar] = my parameterStatus [ipar] == DataModeler_PARAMETER_FIXED ? my parameter [ipar] : 0.0;
		}

		// estimate sigma if we weigh all datapoint equally. 
		// This is necessary to get the parameter covariances right
		double sigmaY = ( my useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? DataModeler_estimateSigmaY (me) : undefined );
		integer idata = 0;
		// Accumulate coefficients of the design matrix
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
				// function evaluation with only the FIXED parameters
				double xi = my x [i], yi = my y [i];
				double yFixed = my f_evaluate (me, xi, parameter.peek());
				double si = ( my useSigmaY != DataModeler_DATA_WEIGH_EQUAL ? DataModeler_getDataPointInverseWeight (me, i, my useSigmaY) : sigmaY );

				// individual terms of the function

				my f_evaluateBasisFunctions (me, xi, term.peek());
				integer ipar = 0; ++ idata;
				for (integer j = 1; j <= my numberOfParameters; j ++) {
					if (my parameterStatus [j] != DataModeler_PARAMETER_FIXED) {
						design [idata] [++ ipar] = term [j] / si;
					}
				}

				// only 'residual variance' must be explained by the model

				b [idata] = (yi - yFixed) / si;
			}
		}
		
		// Singular value decomposition and evaluation of the singular values

		autoSVD thee = SVD_create_d (design.peek(), numberOfDataPoints, numberOfParameters);
		if (! NUMfpp) {
			NUMmachar ();
		}
		SVD_zeroSmallSingularValues (thee.get(), my tolerance > 0.0 ? my tolerance : numberOfDataPoints * NUMfpp -> eps);
		SVD_solve (thee.get(), b.peek(), parameter.peek()); // re-use parameter

		// Put the calculated parameters at the correct position in 'my p'
		Covariance cov = my parameterCovariances.get();
		integer ipar = 0;
		for (integer j = 1; j <= my numberOfParameters; j ++) {
			if (my parameterStatus [j] != DataModeler_PARAMETER_FIXED) {
				my parameter [j] = parameter [++ ipar];
			}
			cov -> centroid [j] = my parameter [j];
		}
		cov -> numberOfObservations = numberOfDataPoints;
		// estimate covariances between parameters
		if (numberOfParameters < my numberOfParameters) {
			autoNUMmatrix<double> covtmp (1, numberOfParameters, 1, numberOfParameters);
			SVD_getSquared (thee.get(), covtmp.peek(), true);
			// Set fixed parameters variances and covariances to zero.
			for (integer i = 1; i <= my numberOfParameters; i ++) {
				for (integer j = i; j <= my numberOfParameters; j ++) {
					cov -> data [i] [j] = cov -> data [j] [i] = 0.0;
				}
			}
			ipar = 0;
			for (integer i = 1; i <= my numberOfParameters; i ++) {
				if (my parameterStatus [i] != DataModeler_PARAMETER_FIXED) {
					integer jpar = 0;
					ipar ++;
					for (integer j = 1; j <= my numberOfParameters; j ++) {
						if (my parameterStatus [j] != DataModeler_PARAMETER_FIXED) {
							jpar++;
							cov -> data [i] [j] = covtmp [ipar] [jpar];
						}
					}
				}
			}
		} else {
			SVD_getSquared (thee.get(), cov -> data, true);
		}
	} catch (MelderError) {
		Melder_throw (U"DataModeler no fit.");
	}
}

void DataModeler_setDataWeighing (DataModeler me, int useSigmaY) {
	if (my useSigmaY != useSigmaY) {
		my useSigmaY = useSigmaY;
		DataModeler_fit (me); // because sigma has changed!
	}
}

autoCovariance DataModeler_to_Covariance_parameters (DataModeler me) {
	try {
		autoCovariance cov = Data_copy (my parameterCovariances.get());
		return cov;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
}

autoDataModeler Table_to_DataModeler (Table me, double xmin, double xmax, integer xcolumn, integer ycolumn, integer scolumn, integer numberOfParameters, int type) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, xcolumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, ycolumn);
		int useSigmaY = scolumn > 0;
		if (useSigmaY) {
			Table_checkSpecifiedColumnNumberWithinRange (me, scolumn);
		}
		integer numberOfRows = my rows.size, numberOfData = 0;
		autoNUMvector<double> x (1, numberOfRows), y (1, numberOfRows), sy (1, numberOfRows);
		for (integer i = 1; i <= numberOfRows; i ++) {
			double val = Table_getNumericValue_Assert (me, i, xcolumn);
			if (isdefined (val)) {
				x [++ numberOfData] = val;
				if (numberOfData > 1) {
					if (val < x [numberOfData - 1]) {
						Melder_throw (U"Data with x-values should be sorted.");
					} else if (val == x [numberOfData - 1]) {
						Melder_throw (U"All x-values should be different.");
					}
				}
				y [numberOfData] = Table_getNumericValue_Assert (me, i, ycolumn);
				sy [numberOfData] = useSigmaY ? Table_getNumericValue_Assert (me, i, scolumn) : 1;
			}
		}
		if (xmax <= xmin) {
			NUMvector_extrema<double> (x.peek(), 1, numberOfData, &xmin, &xmax);
		}
		Melder_require (xmin < xmax, U"The range of the x-values is too small.");
		
		integer numberOfDataPoints = 0, validData = 0;
		for (integer i = 1; i <= numberOfData; i ++) {
			if (x [i] >= xmin && x [i] <= xmax) {
				numberOfDataPoints ++;
			}
		}
		autoDataModeler thee = DataModeler_create (xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		numberOfDataPoints = 0;
		for (integer i = 1; i <= numberOfData; i ++) {
			if (x [i] >= xmin && x [i] <= xmax) {
				thy x [++ numberOfDataPoints] = x [i];
				thy dataPointStatus [numberOfDataPoints] = DataModeler_DATA_INVALID;
				if (isdefined (y [i])) {
					thy y [numberOfDataPoints] = y [i];
					thy sigmaY [numberOfDataPoints] = sy [i];
					thy dataPointStatus [numberOfDataPoints] = DataModeler_DATA_VALID;
					validData ++;
				}
			}
		}
		thy useSigmaY = useSigmaY;
		thy numberOfDataPoints = numberOfDataPoints;
		thy tolerance = 1e-5;
		Melder_require (validData >= numberOfParameters, U"The number of parameters should not exceed the number of data points.");
		
		DataModeler_setDataWeighing (thee.get(), DataModeler_DATA_WEIGH_SIGMA);
		DataModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Datamodeler not created from Table.");
	}
}

Thing_implement (FormantModeler, Function, 0);

void structFormantModeler :: v_info () {

	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	for (integer iformant = 1; iformant <= trackmodelers.size; iformant ++) {
		DataModeler ffi = trackmodelers.at [iformant];
		MelderInfo_writeLine (U"Formant ", iformant);
		ffi -> v_info();
	}
}

double DataModeler_getResidualSumOfSquares (DataModeler me, integer *numberOfDataPoints) {
	integer n = 0;
	double rss = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
				++ n;
				double dif = my y [i] - my f_evaluate (me, my x [i], my parameter);
				rss += dif * dif;
		}
	}
	if (numberOfDataPoints) {
		*numberOfDataPoints = n;
	}
	return ( n > 0 ? rss : undefined );
}

void DataModeler_reportChiSquared (DataModeler me, int weighDataType) {
	MelderInfo_writeLine (U"Chi squared test:");
	MelderInfo_writeLine (weighDataType == DataModeler_DATA_WEIGH_EQUAL ? U"Standard deviation is estimated from the data." :
		weighDataType == DataModeler_DATA_WEIGH_SIGMA ? U"Sigmas are used as estimate for local standard deviations." :
		weighDataType == DataModeler_DATA_WEIGH_RELATIVE ? U"1/Q's are used as estimate for local standard deviations." :
		U"Sqrt sigmas are used as estimate for local standard deviations.");
	double ndf, probability, chisq = DataModeler_getChiSquaredQ (me, weighDataType, &probability, &ndf);
	MelderInfo_writeLine (U"Chi squared = ", chisq);
	MelderInfo_writeLine (U"Probability = ", probability);
	MelderInfo_writeLine (U"Number of degrees of freedom = ", ndf);	
}

double DataModeler_estimateSigmaY (DataModeler me) {
	try {
		integer numberOfDataPoints = 0;
		autonumvec y (my numberOfDataPoints, kTensorInitializationType::RAW);
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			if (my dataPointStatus [i] != DataModeler_DATA_INVALID) {
				y [++ numberOfDataPoints] = my y [i];
			}
		}
		y.size = numberOfDataPoints;   // fake shrink
		return stdev_scalar (y.get());
	} catch (MelderError) {
		Melder_throw (U"Cannot estimate sigma.");
	}
}

double FormantModeler_getStandardDeviation (FormantModeler me, integer iformant) {
	double sigma = undefined;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		sigma = DataModeler_estimateSigmaY (ff);
	}
	return sigma;
}

double FormantModeler_getDataPointValue (FormantModeler me, integer iformant, integer index) {
	double value = undefined;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		value = DataModeler_getDataPointYValue (ff, index);
	}
	return value;
}

void FormantModeler_setDataPointValue (FormantModeler me, integer iformant, integer index, double value) {
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
 		DataModeler_setDataPointYValue (ff, index, value);
	}
}

double FormantModeler_getDataPointSigma (FormantModeler me, integer iformant, integer index) {
	double sigma = undefined;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = (DataModeler) my trackmodelers.at [iformant];
		sigma = DataModeler_getDataPointYSigma (ff, index);
	}
	return sigma;
}

void FormantModeler_setDataPointSigma (FormantModeler me, integer iformant, integer index, double sigma) {
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
 		DataModeler_setDataPointYSigma (ff, index, sigma);
	}
}

int FormantModeler_getDataPointStatus (FormantModeler me, integer iformant, integer index) {
	int value = DataModeler_DATA_INVALID;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		value = DataModeler_getDataPointStatus (ff, index);
	}
	return value;
}

void FormantModeler_setDataPointStatus (FormantModeler me, integer iformant, integer index, int status)
{
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		DataModeler_setDataPointStatus (ff, index, status);
	}
}

static void FormantModeler_setDataPointValueAndStatus (FormantModeler me, integer iformant, integer index, double value, int dataStatus)
{
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		DataModeler_setDataPointValueAndStatus (ff, index, value, dataStatus);
	}
}

void FormantModeler_setParameterValueFixed (FormantModeler me, integer iformant, integer index, double value) {
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ffi = my trackmodelers.at [iformant];
		DataModeler_setParameterValueFixed (ffi, index, value);
	}
}

void FormantModeler_setParametersFree (FormantModeler me, integer fromFormant, integer toFormant, integer fromIndex, integer toIndex) {
	integer numberOfFormants = my trackmodelers.size;
	if (toFormant < fromFormant || (fromFormant == toFormant && fromFormant == 0)) {
		fromFormant = 1;
		toFormant = numberOfFormants;
	}
	Melder_require (toFormant > 0 && toFormant <= numberOfFormants && fromFormant > 0 && fromFormant <= numberOfFormants && fromFormant <= toFormant, 
		U"Formant number(s) should be in the interval [1, ", numberOfFormants, U"].");
	
	for (integer iformant = fromFormant; iformant <= toFormant; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		DataModeler_setParametersFree (ffi, fromIndex, toIndex);
	}
}

void FormantModeler_setDataWeighing (FormantModeler me, integer fromFormant, integer toFormant, int useSigmaY) {
	integer numberOfFormants = my trackmodelers.size;
	if (toFormant < fromFormant || (fromFormant == toFormant && fromFormant == 0)) {
		fromFormant = 1; toFormant= numberOfFormants;
	}
	Melder_require (toFormant > 0 && toFormant <= numberOfFormants && fromFormant > 0 && fromFormant <= numberOfFormants && fromFormant <= toFormant, 
		U"Formant number(s) should be in the interval [1, ", numberOfFormants, U"].");
	
	for (integer iformant = fromFormant; iformant <= toFormant; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		DataModeler_setDataWeighing (ffi, useSigmaY);
	}
}

void FormantModeler_fit (FormantModeler me) {
	for (integer iformant = 1; iformant <= my trackmodelers.size; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		DataModeler_fit (ffi);
	}
}

void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax,
 	integer iformant, integer iterm, bool scaled, integer numberOfPoints, bool garnish)
{
	if (tmax <= tmin) {
		tmin = my xmin; tmax = my xmax; 
	}
	if (iformant < 1 || iformant > my trackmodelers.size) {
		return;
	}
	Graphics_setInner (g);
	DataModeler ffi = my trackmodelers.at [iformant];
	DataModeler_drawBasisFunction_inside (ffi, g, tmin, tmax, fmin, fmax, iterm, scaled, numberOfPoints);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_inqWindow (g, &tmin, &tmax, &fmin, &fmax);
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, (scaled ? U"Frequency (Hz)" : U"Amplitude"));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_markLeft (g, fmin, true, true, false, U"");
		Graphics_markLeft (g, fmax, true, true, false, U"");
	}
}

static integer FormantModeler_drawingSpecifiers_x (FormantModeler me, double *xmin, double *xmax, integer *ixmin, integer *ixmax) {
	Melder_assert (my trackmodelers.size > 0);
	DataModeler fm = my trackmodelers.at [1];
	return DataModeler_drawingSpecifiers_x (fm, xmin, xmax, ixmin, ixmax);
}

static void FormantModeler_getCumulativeChiScores (FormantModeler me, int useSigmaY, double chisq []) {
	try {
		integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		integer numberOfFormants = my trackmodelers.size;
		autoNUMvector<double> zscores (1, numberOfDataPoints);
		for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
			DataModeler fm = my trackmodelers.at [iformant];
			DataModeler_getZScores (fm, useSigmaY, zscores.peek());
			DataModeler_getChisqScoresFromZScores (fm, zscores.peek(), true, zscores.peek()); // undefined -> average
			for (integer i = 1; i <= numberOfDataPoints; i ++) {
				chisq [i] += zscores [i];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U"cannot determine cumulative chi squares.");
	}
}

static void FormantModeler_getVariancesBetweenTrackAndEstimatedTrack (FormantModeler me, integer iformant, integer estimatedFormant, double var []) {
	integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
	integer numberOfFormants = my trackmodelers.size;
	
	if (iformant < 1 || iformant > numberOfFormants || estimatedFormant < 1 || estimatedFormant > numberOfFormants) {
		return;
	}
	DataModeler fi = my trackmodelers.at [iformant];
	DataModeler fe = my trackmodelers.at [estimatedFormant];
	for (integer i = 1; i <= numberOfDataPoints; i ++) {
		var [i] = undefined;
		if (fi -> dataPointStatus [i] != DataModeler_DATA_INVALID) {
			double ye = fe -> f_evaluate (fe, fe -> x [i], fe -> parameter);
			double diff = ye - fi -> y [i];
			var [i] = diff * diff;
		}
	}
}

static void FormantModeler_getSumOfVariancesBetweenShiftedAndEstimatedTracks (FormantModeler me, int shiftDirection, integer *fromFormant, integer *toFormant, double var []) {
	try {
		integer numberOfFormants = my trackmodelers.size;
		if (*fromFormant < 1 || *fromFormant > numberOfFormants || *toFormant < 1 || *toFormant > numberOfFormants || *toFormant < *fromFormant) {
			*toFormant = 1; *fromFormant = numberOfFormants;
		}

		integer formantTrack = *fromFormant, estimatedFormantTrack = *fromFormant; // FormantModeler_NOSHIFT_TRACKS
		if (shiftDirection == FormantModeler_DOWNSHIFT_TRACKS) {
			estimatedFormantTrack = *fromFormant;
			formantTrack = *fromFormant + 1;
			*fromFormant = *fromFormant == 1 ? 2 : *fromFormant;
		} else if (shiftDirection == FormantModeler_UPSHIFT_TRACKS) {
			formantTrack = *fromFormant;
			estimatedFormantTrack = *fromFormant + 1;
			*toFormant = *toFormant == numberOfFormants ? numberOfFormants - 1 : *toFormant;
		}
		integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoNUMvector<double> vari (1, numberOfDataPoints);
		for (integer iformant = *fromFormant; iformant <= *toFormant; iformant ++) {
			FormantModeler_getVariancesBetweenTrackAndEstimatedTrack (me, formantTrack, estimatedFormantTrack, vari.peek());
			for (integer i = 1; i <= numberOfDataPoints; i ++) {
				if (isdefined (vari [i])) {
					var [i] += vari [i];
				}
			}
			formantTrack ++;
			estimatedFormantTrack ++;
		}
	} catch (MelderError) {
		Melder_throw (me, U" cannot get variances.");
	}
}

void FormantModeler_drawVariancesOfShiftedTracks (FormantModeler me, Graphics g, double xmin, double xmax,
	double ymin, double ymax, int shiftDirection, integer fromFormant, integer toFormant, bool garnish)
{
	try {
		integer ixmin, ixmax;
		Melder_require (FormantModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) > 0,
			U"The are not enough data points in the drawing range.");

		integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoNUMvector<double> var (1, numberOfDataPoints);
		autoNUMvector<double> varShifted (1, numberOfDataPoints);
		FormantModeler_getSumOfVariancesBetweenShiftedAndEstimatedTracks (me, shiftDirection, &fromFormant, &toFormant, varShifted.peek());
		FormantModeler_getSumOfVariancesBetweenShiftedAndEstimatedTracks (me, 0, &fromFormant, &toFormant, var.peek());
		for (integer i = ixmin + 1; i <= ixmax; i ++) {
			if (isdefined (varShifted [i]) && isdefined (var [i])) {
				var [i] -= varShifted [i];
			}
		}
		if (ymax <= ymin) {
			NUMvector_extrema<double> (var.peek(), ixmin, ixmax, &ymin, &ymax);
		}
		Graphics_setInner (g);
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		DataModeler thee = my trackmodelers.at [1];
		while (isundef (var [ixmin]) && ixmin <= ixmax) {
			ixmin++;
		}
		double xp = thy x [ixmin], yp = var [ixmin];
		for (integer i = ixmin + 1; i <= ixmax; i ++) {
			if (isdefined (var [i])) {
				Graphics_line (g, xp, yp, thy x [i], var [i]);
				xp = thy x [i];
				yp = var [i];
			}
		}
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_marksLeft (g, 2, true, true, false);
		}

	} catch (MelderError) {
		Melder_clearError ();
	}
}

void FormantModeler_drawCumulativeChiScores (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int useSigmaY, int garnish) {
	try {
		integer ixmin, ixmax;
		Melder_require (FormantModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) > 0,
			U"Not enough data points in drawing range.");

		integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoNUMvector<double> chisq (1, numberOfDataPoints);
		FormantModeler_getCumulativeChiScores (me, useSigmaY, chisq.peek());
		if (ymax <= ymin) {
			NUMvector_extrema<double> (chisq.peek(), ixmin, ixmax, & ymin, & ymax);
		}
		Graphics_setInner (g);
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		DataModeler thee = my trackmodelers.at [1];
		for (integer i = ixmin + 1; i <= ixmax; i ++) {
			Graphics_line (g, thy x [i - 1], chisq [i - 1], thy x [i], chisq [i]);
		}
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		//
	}
}

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack, double numberOfSigmas, int useSigmaY, char32 *mark, int marksFontSize, double horizontalOffset_mm, int garnish) {
	if (tmax <= tmin) { 
		tmin = my xmin; tmax = my xmax; 
	}
	integer maxTrack = my trackmodelers.size;
	if (toTrack == 0 && fromTrack == 0) {
		fromTrack = 1; toTrack = maxTrack;
	}
	if (fromTrack > maxTrack) return;
	if (toTrack > maxTrack) {
		toTrack = maxTrack;
	}
	Graphics_setInner (g);
	int currectFontSize = Graphics_inqFontSize (g);
	for (integer iformant = fromTrack; iformant <= toTrack; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		double xOffset_mm = ( iformant % 2 == 1 ? horizontalOffset_mm : -horizontalOffset_mm );
		DataModeler_drawOutliersMarked_inside (ffi, g, tmin, tmax, 0, fmax, numberOfSigmas, useSigmaY, mark, marksFontSize, xOffset_mm);
	}
	Graphics_setFontSize (g, currectFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, integer iformant, int useSigmaY, integer numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, bool garnish) {
	if (iformant > 0 || iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		DataModeler_normalProbabilityPlot (ff, g, useSigmaY, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	}
}

static void FormantModeler_drawTracks_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax,
	integer fromTrack, integer toTrack, int estimated, integer numberOfParameters, double horizontalOffset_mm) {
	for (integer iformant = fromTrack; iformant <= toTrack; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		double xOffset_mm = ( iformant % 2 == 1 ? horizontalOffset_mm : -horizontalOffset_mm );
		DataModeler_drawTrack_inside (ffi, g, xmin, xmax, 0, fmax, estimated, numberOfParameters, xOffset_mm);
	}
}

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, int estimated, integer numberOfParameters, double horizontalOffset_mm, int garnish) {
	if (tmax <= tmin) { 
		tmin = my xmin; tmax = my xmax; 
	}
	integer maxTrack = my trackmodelers.size;
	if (toTrack == 0 && fromTrack == 0) {
		fromTrack = 1; toTrack = maxTrack;
	}
	if (fromTrack > maxTrack) return;
	if (toTrack > maxTrack) {
		toTrack = maxTrack;
	}
	Graphics_setInner (g);
	FormantModeler_drawTracks_inside (me, g, tmin, tmax, fmax, fromTrack, toTrack, estimated, numberOfParameters, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

static void FormantModeler_speckle_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax,
	integer fromTrack, integer toTrack, int estimated, integer numberOfParameters, int errorBars, double barWidth_mm, double horizontalOffset_mm) {
	for (integer iformant = fromTrack; iformant <= toTrack; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		double xOffset_mm = ( iformant % 2 == 1 ? horizontalOffset_mm : -horizontalOffset_mm );
		DataModeler_speckle_inside (ffi, g, xmin, xmax, 0, fmax, estimated, numberOfParameters, errorBars, barWidth_mm, xOffset_mm);
	}
}

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, int estimated, integer numberOfParameters, int errorBars, double barWidth_mm, double horizontalOffset_mm, int garnish) {
	if (tmax <= tmin) { 
		tmin = my xmin; tmax = my xmax; 
	}
	integer maxTrack = my trackmodelers.size;
	if (toTrack == 0 && fromTrack == 0) {
		fromTrack = 1; toTrack = maxTrack;
	}
	if (fromTrack > maxTrack) return;
	if (toTrack > maxTrack) {
		toTrack = maxTrack;
	}
	Graphics_setInner (g);
	FormantModeler_speckle_inside (me, g, tmin, tmax, fmax, fromTrack, toTrack, estimated, numberOfParameters,  errorBars, barWidth_mm, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfFormants, integer numberOfDataPoints, integer numberOfParameters) {
	try {
		autoFormantModeler me = Thing_new (FormantModeler);
		my xmin = tmin; my xmax = tmax;
		for (integer itrack = 1; itrack <= numberOfFormants; itrack ++) {
			autoDataModeler ff = DataModeler_create (tmin, tmax, numberOfDataPoints, numberOfParameters,  DataModeler_TYPE_LEGENDRE);
			my trackmodelers. addItem_move (ff.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantModeler not created.");
	}
}

double FormantModeler_getModelValueAtTime (FormantModeler me, integer iformant, double time) {
	double f = undefined;
	if (iformant >= 1 && iformant <= my trackmodelers.size) {
		DataModeler thee = my trackmodelers.at [iformant];
		f = DataModeler_getModelValueAtX (thee, time);
	}
	return f;
}

double FormantModeler_getModelValueAtIndex (FormantModeler me, integer iformant, integer index) {
	double f = undefined;
	if (iformant >= 1 && iformant <= my trackmodelers.size) {
		DataModeler thee = my trackmodelers.at [iformant];
		f = DataModeler_getModelValueAtIndex (thee, index);
	}
	return f;
}

double FormantModeler_getWeightedMean (FormantModeler me, integer iformant) {
	double f = undefined;
	if (iformant >= 1 && iformant <= my trackmodelers.size) {
		DataModeler thee = my trackmodelers.at [iformant];
		f = DataModeler_getWeightedMean (thee);
	}
	return f;
	
}

integer FormantModeler_getNumberOfTracks (FormantModeler me) {
	return my trackmodelers.size;
}

integer FormantModeler_getNumberOfParameters (FormantModeler me, integer iformant) {
	integer numberOfParameters = 0;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		numberOfParameters = ff -> numberOfParameters;
	}
	return numberOfParameters;
}

integer FormantModeler_getNumberOfFixedParameters (FormantModeler me, integer iformant) {
	integer numberOfParameters = 0;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		numberOfParameters = ff -> numberOfParameters;
		numberOfParameters -= DataModeler_getNumberOfFreeParameters (ff);
	}
	return numberOfParameters;
}


integer FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, integer iformant) {
	integer numberOfInvalidDataPoints = 0;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		numberOfInvalidDataPoints = DataModeler_getNumberOfInvalidDataPoints (ff);
	}
	return numberOfInvalidDataPoints;
}

double FormantModeler_getParameterValue (FormantModeler me, integer iformant, integer iparameter) {
	double value = undefined;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		value = DataModeler_getParameterValue (ff, iparameter);
	}
	return value;
}

int FormantModeler_getParameterStatus (FormantModeler me, integer iformant, integer index) {
	int status = DataModeler_PARAMETER_UNDEFINED;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		status = DataModeler_getParameterStatus (ff, index);
	}
	return status;
}

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, integer iformant, integer index) {
	double stdev = undefined;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		stdev = DataModeler_getParameterStandardDeviation (ff, index);
	}
	return stdev;
}

double FormantModeler_getDegreesOfFreedom (FormantModeler me, integer iformant) {
	double dof = 0.0;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		dof = DataModeler_getDegreesOfFreedom (ff);
	}
	return dof;
}

double FormantModeler_getVarianceOfParameters (FormantModeler me, integer fromFormant, integer toFormant, integer fromIndex, integer toIndex, integer *numberOfFreeParameters) {
	double variance = undefined;
	integer numberOfFormants = my trackmodelers.size, numberOfParameters = 0, nofp;
	if (toFormant < fromFormant || (toFormant == 0 && fromFormant == 0)) {
		fromFormant = 1; toFormant = numberOfFormants;
	}
	if (fromFormant <= toFormant && fromFormant > 0 && toFormant <= numberOfFormants) {
		variance = 0.0;
		for (integer iformant = fromFormant; iformant <= toFormant; iformant ++) {
			DataModeler ff = my trackmodelers.at [iformant];
			variance += DataModeler_getVarianceOfParameters (ff, fromIndex, toIndex, &nofp);
			numberOfParameters += nofp;
		}
	}
	if (numberOfFreeParameters) {
		*numberOfFreeParameters = numberOfParameters;
	}
	return variance;
}

integer FormantModeler_getNumberOfDataPoints (FormantModeler me) {
	Melder_assert (my trackmodelers.size > 0);
	DataModeler thee = my trackmodelers.at [1];
	// all tracks have the same number of data points
	return thy numberOfDataPoints;
}

autoTable FormantModeler_to_Table_zscores (FormantModeler me, int useSigmaY) {
	try {
		integer icolt = 1, numberOfFormants = my trackmodelers.size;
		integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoNUMvector<double> zscores (1, numberOfDataPoints);
		autoTable ztable = Table_createWithoutColumnNames (numberOfDataPoints, numberOfFormants + 1);
		Table_setColumnLabel (ztable.get(), icolt, U"time");
		for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
			integer icolz = iformant + 1;
			Table_setColumnLabel (ztable.get(), icolz, Melder_cat (U"z", iformant));
			DataModeler ffi = my trackmodelers.at [iformant];
			if (iformant == 1) {
				for (integer i = 1; i <= numberOfDataPoints; i ++) { // only once all tracks have same x-values
				Table_setNumericValue (ztable.get(), i, icolt, ffi -> x [i]);
				}
			}
			DataModeler_getZScores (ffi, useSigmaY, zscores.peek());
			for (integer i = 1; i <= numberOfDataPoints; i ++) {
				Table_setNumericValue (ztable.get(), i, icolz, zscores [i]);
			}
		}
		return ztable;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

autoDataModeler FormantModeler_extractDataModeler (FormantModeler me, integer iformant) {
	try {
		Melder_require (iformant > 0 && iformant<= my trackmodelers.size, 
			U"The formant should be greater than zero and smaller than or equal to ", my trackmodelers.size);
		
		DataModeler ff = my trackmodelers.at [iformant];
		autoDataModeler thee = Data_copy (ff);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}	
}

autoCovariance FormantModeler_to_Covariance_parameters (FormantModeler me, integer iformant) {
	try {
		Melder_require (iformant > 0 && iformant<= my trackmodelers.size, 
			U"The formant should be greater than zero and smaller than or equal to ", my trackmodelers.size);
		DataModeler thee = my trackmodelers.at [iformant];
		autoCovariance cov = Data_copy (thy parameterCovariances.get());
		return cov;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
	
}

void FormantModeler_setTolerance (FormantModeler me, double tolerance) {
	for (integer iformant = 1; iformant <= my trackmodelers.size; iformant ++) {
		DataModeler ffi = my trackmodelers.at [iformant];
		DataModeler_setTolerance (ffi, tolerance);
	}
}

double FormantModeler_indexToTime (FormantModeler me, integer index) {
	Melder_assert (my trackmodelers.size > 0);
	DataModeler thee = my trackmodelers.at [1];
	return ( index > 0 && index <= thy numberOfDataPoints ? thy x [index] : undefined );
}

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, integer numberOfFormants, integer numberOfParametersPerTrack, int bandwidthEstimatesSigma) {
	try {
		integer ifmin, ifmax, posInCollection = 0;
		if (tmax <= tmin) {
			tmin = my xmin; tmax = my xmax;
		}
		integer numberOfDataPoints = Sampled_getWindowSamples (me, tmin, tmax, & ifmin, & ifmax);
		Melder_require (numberOfDataPoints >= numberOfParametersPerTrack, U"There are not enought data points, please extend the selection.");
		
		autoFormantModeler thee = FormantModeler_create (tmin, tmax, numberOfFormants, numberOfDataPoints, numberOfParametersPerTrack);
		for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
			posInCollection ++;
			DataModeler ffi = thy trackmodelers.at [posInCollection];
			integer idata = 0, validData = 0;
			for (integer iframe = ifmin; iframe <= ifmax; iframe ++) {
				Formant_Frame curFrame = & my d_frames [iframe];
				ffi -> x [++ idata] = Sampled_indexToX (me, iframe);
				ffi -> dataPointStatus [idata] = DataModeler_DATA_INVALID;
				if (iformant <= curFrame -> nFormants) {
					double frequency = curFrame -> formant [iformant]. frequency;
					if (isdefined (frequency)) {
						double bw = curFrame -> formant [iformant]. bandwidth;
						ffi -> y [idata] = curFrame -> formant [iformant]. frequency;
						ffi -> sigmaY [idata] = bw;
						ffi -> dataPointStatus [idata] = DataModeler_DATA_VALID;
						validData++;
					}
				}
			}
			ffi -> useSigmaY = bandwidthEstimatesSigma;
			ffi -> numberOfDataPoints = idata;
			ffi -> tolerance = 1e-5;
			if (validData < numberOfParametersPerTrack) {   // remove don't throw exception
				thy trackmodelers. removeItem (posInCollection);
				posInCollection --;
			}
		}
		if (posInCollection == 0) {
			Melder_throw (U"Not enough data points in all the formants.");
		}
		FormantModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FormantModeler not created.");
	}
}

autoFormant FormantModeler_to_Formant (FormantModeler me, int useEstimates, int estimateUndefineds) {
	try {
		integer numberOfFormants = my trackmodelers.size;
		DataModeler ff = my trackmodelers.at [1];
		integer numberOfFrames = ff -> numberOfDataPoints;
		double t1 = ff -> x [1], dt = ff -> x [2] - t1;
		autoFormant thee = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
		autoNUMvector<double> sigma (1, numberOfFormants);
		if (useEstimates || estimateUndefineds) {
			for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
				sigma [iformant] = FormantModeler_getStandardDeviation (me, iformant);
			}
		}
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			Formant_Frame thyFrame = & thy d_frames [iframe];
			thyFrame -> intensity = 1.0; //???
			thyFrame -> formant = NUMvector <structFormant_Formant> (1, numberOfFormants);
			
			for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
				DataModeler ffi = my trackmodelers.at [iformant];
				double f = undefined, b = f;
				if (ffi -> dataPointStatus [iframe] != DataModeler_DATA_INVALID) {
					f = ( useEstimates ? DataModeler_getModelValueAtX (ffi, ffi -> x [iframe]) : ffi -> y [iframe]);
					b = ff -> sigmaY [iframe]; // copy original value
				} else {
					if (estimateUndefineds) {
						f = FormantModeler_getModelValueAtTime (me, iformant, ffi -> x [iframe]);
						b = sigma [iformant];
					}
				}
				thyFrame -> formant [iformant]. frequency = f;
				thyFrame -> formant [iformant]. bandwidth = b;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Cannot create Formant from FormantModeler.");
	}
}

double FormantModeler_getChiSquaredQ (FormantModeler me, integer fromFormant, integer toFormant, int useSigmaY, double *probability, double *ndf) {
	double chisq = undefined, ndfTotal = 0.0;
	if (toFormant < fromFormant || (fromFormant == 0 && toFormant == 0)) {
		fromFormant = 1; toFormant = my trackmodelers.size;
	}
	if (fromFormant >= 1 && toFormant <= my trackmodelers.size) {
		chisq = 0.0;
		integer numberOfDefined = 0;
		for (integer iformant= fromFormant; iformant <= toFormant; iformant ++) {
			DataModeler ffi = my trackmodelers.at [iformant];
			double p, df, chisqi = DataModeler_getChiSquaredQ (ffi, useSigmaY, &p, &df);
			if (isdefined (chisqi)) {
				chisq += df * chisqi;
				ndfTotal += df;
				numberOfDefined ++;
			}
		}
		if (numberOfDefined == toFormant - fromFormant + 1) {   // chisq of all tracks defined
			chisq /= ndfTotal;
			if (ndf) {
				*ndf = ndfTotal;
			}
			if (probability) {
				*probability = NUMchiSquareQ (chisq, ndfTotal);
			}
		}
	}
	return chisq;
}

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, integer fromFormant, integer toFormant) {
	double rSquared = undefined;
	if (fromFormant == 0 && toFormant == 0) {
		fromFormant = 1;
		toFormant = my trackmodelers.size;
	}
	if (fromFormant >= 1 && toFormant <= my trackmodelers.size) {
		double ssreg = 0.0, sstot = 0.0;
		for (integer iformant= fromFormant; iformant <= toFormant; iformant ++) {
			DataModeler ffi = my trackmodelers.at [iformant];
			double ssregi, sstoti;
			DataModeler_getCoefficientOfDetermination (ffi, & ssregi, & sstoti);
			sstot += sstoti;
			ssreg += ssregi;
		}
		rSquared = ( sstot > 0.0 ? ssreg / sstot : 1.0 );
	}
	return rSquared;
}

double FormantModeler_getResidualSumOfSquares (FormantModeler me, integer iformant, integer *p_numberOfDataPoints) {
	double rss = undefined;
	integer numberOfDataPoints = -1;
	if (iformant > 0 && iformant <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [iformant];
		rss = DataModeler_getResidualSumOfSquares (ff, & numberOfDataPoints);
	}
	if (p_numberOfDataPoints) {
		*p_numberOfDataPoints = numberOfDataPoints;
	}
	return rss;
}

void FormantModeler_setParameterValuesToZero (FormantModeler me, integer fromFormant, integer toFormant, double numberOfSigmas) {
	if (fromFormant == 0 && toFormant == 0) {
		fromFormant = 1;
		toFormant = my trackmodelers.size;
	}
	if (fromFormant >= 1 && toFormant <= my trackmodelers.size) {
		for (integer iformant= fromFormant; iformant <= toFormant; iformant ++) {
			DataModeler ffi = my trackmodelers.at [iformant];
			DataModeler_setParameterValuesToZero (ffi, numberOfSigmas);
		}
	}
}

autoFormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas, int useSigmaY) {
	try {
		integer numberOfFormants = my trackmodelers.size;
		Melder_require (numberOfFormants > 2, U"We need at least three formants to process outliers.");
		
		integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoNUMvector<double> x (1, numberOfDataPoints); // also store x-values
		autoNUMmatrix<double> z (1, numberOfFormants, 1, numberOfDataPoints);
		// maybe some of the formants had NUMundefind's.

		// 1. calculate z-scores for each formant and sort them in descending order
		DataModeler ff = my trackmodelers.at [1];
		NUMvector_copyElements<double> (ff -> x, x.peek(), 1, numberOfDataPoints);
		for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
			DataModeler ffi = my trackmodelers.at [iformant];
			DataModeler_getZScores (ffi, useSigmaY, z [iformant]);
		}
		// 2. Do the manipulation in a copy
		autoFormantModeler thee = Data_copy (me);
		for (integer i = 1; i <= numberOfDataPoints; i ++) {
			// First the easy one: first formant missing: F1' = F2; F2' = F3
			if (isdefined (z [1] [i]) && isdefined (z [1] [i]) && isdefined (z [3] [i])) {
				if (z [1] [i] > numberOfSigmas && z [2] [i] > numberOfSigmas && z [3] [i] > numberOfSigmas) {
					// all deviations have the same sign:
					// probably F1 is missing
					// try if f2 <- F1 and f3 <- F2 reduces chisq
					double f2 = FormantModeler_getDataPointValue (me, 1, i); // F1
					double f3 = FormantModeler_getDataPointValue (me, 2, i); // F2
					FormantModeler_setDataPointStatus (thee.get(), 1, i, DataModeler_DATA_INVALID);
					FormantModeler_setDataPointValueAndStatus (thee.get(), 2, i, f2, FormantModeler_UPSHIFT_TRACKS);
					FormantModeler_setDataPointValueAndStatus (thee.get(), 3, i, f3, FormantModeler_UPSHIFT_TRACKS);
				}
			}
		}
		FormantModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Cannot calculate track discontinuities");
	}
}

double FormantModeler_getSmoothnessValue (FormantModeler me, integer fromFormant, integer toFormant, integer numberOfParametersPerTrack, double power) {
	double smoothness = undefined;
	if (toFormant < fromFormant || (toFormant == 0 && fromFormant == 0)) {
		fromFormant = 1;
		toFormant = my trackmodelers.size;
	}
	if (fromFormant > 0 && fromFormant <= toFormant && toFormant <= my trackmodelers.size) {
		integer nofp;
		double ndof, var = FormantModeler_getVarianceOfParameters (me, fromFormant, toFormant, 1, numberOfParametersPerTrack, &nofp);
		double chisq = FormantModeler_getChiSquaredQ (me, fromFormant, toFormant, true, nullptr, &ndof);
		if (isdefined (var) && isdefined (chisq) && nofp > 0) {
			smoothness = log10 (pow (var / nofp, power) * (chisq / ndof));
		}
	}
	return smoothness;
}

double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, integer track1, integer track2, int type) {
	double diff = undefined;
	if (track1 == track2) {
		return 0.0;
	}
	if (track1 <= my trackmodelers.size && track2 <= my trackmodelers.size) {
		DataModeler fi = my trackmodelers.at [track1];
		DataModeler fj = my trackmodelers.at [track2];
		// fi and fj have equal number of data points
		integer numberOfDataPoints = 0;
		diff = 0.0;
		for (integer i = 1; i <= fi -> numberOfDataPoints; i ++) {
			if (type != 0) {
				double fie = fi -> f_evaluate (fi, fi -> x [i], fi -> parameter);
				double fje = fj -> f_evaluate (fj, fj -> x [i], fj -> parameter);
				diff += fabs (fie - fje);
				numberOfDataPoints ++;
			} else if (fi -> dataPointStatus [i] != DataModeler_DATA_INVALID && fj -> dataPointStatus [i] != DataModeler_DATA_INVALID) {
				diff += fabs (fi -> y [i] - fj -> y [i]);
				numberOfDataPoints ++;
			}
		}
		diff /= numberOfDataPoints;
	}
	return diff;
}

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3) {
	double f1 = FormantModeler_getParameterValue (me, 1, 1); // trackmodelers -> item [1] -> parameter [1]
	double minF1Factor = f1 > minF1 ? 1 : sqrt (minF1 - f1 + 1.0);
	double maxF1Factor = f1 < maxF1 ? 1 : sqrt (f1 - maxF1 + 1.0);
	double f2 = FormantModeler_getParameterValue (me, 2, 1); // trackmodelers -> item [2] -> parameter [1]
	double minF2Factor = f2 > minF2 ? 1 : sqrt (minF2 - f2 + 1.0);
	double maxF2Factor = f2 < maxF2 ? 1 : sqrt (f2 - maxF2 + 1.0);
	double f3 = FormantModeler_getParameterValue (me, 3, 1); // trackmodelers -> item [3] -> parameter [1]
	double minF3Factor = f3 > minF3 ? 1 : sqrt (minF3 - f3 + 1.0);
	return minF1Factor * maxF1Factor * minF2Factor * maxF2Factor * minF3Factor;
}

void FormantModeler_reportChiSquared (FormantModeler me, int weighDataType) {
	integer numberOfFormants = my trackmodelers.size;
	double chisq = 0, ndf = 0, probability;
	MelderInfo_writeLine (U"Chi squared tests for individual models of each of ", numberOfFormants, U" formant track:");
	MelderInfo_writeLine (weighDataType == DataModeler_DATA_WEIGH_EQUAL ? U"Standard deviation is estimated from the data." :
		weighDataType == DataModeler_DATA_WEIGH_SIGMA ? U"\tBandwidths are used as estimate for local standard deviations." :
		weighDataType == DataModeler_DATA_WEIGH_RELATIVE ? U"\t1/Q's are used as estimate for local standard deviations." :
		U"\tSqrt bandwidths are used as estimate for local standard deviations.");
	for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
		chisq = FormantModeler_getChiSquaredQ (me, iformant, iformant, weighDataType, & probability, & ndf);
		MelderInfo_writeLine (U"Formant track ", iformant, U":");
		MelderInfo_writeLine (U"\tChi squared (F", iformant, U") = ", chisq);
		MelderInfo_writeLine (U"\tProbability (F", iformant, U") = ", probability);
		MelderInfo_writeLine (U"\tNumber of degrees of freedom (F", iformant, U") = ", ndf);
	}
	chisq = FormantModeler_getChiSquaredQ (me, 1, numberOfFormants, weighDataType, & probability, & ndf);
	MelderInfo_writeLine (U"Chi squared test for the complete model with ", numberOfFormants, U" formants:");
	MelderInfo_writeLine (U"\tChi squared = ", chisq);
	MelderInfo_writeLine (U"\tProbability = ", probability);
	MelderInfo_writeLine (U"\tNumber of degrees of freedom = ", ndf);
}

integer Formants_getSmoothestInInterval (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack,
	int useBandWidthsForTrackEstimation, int useConstraints, double numberOfSigmas, double power,
	double minF1, double maxF1, double minF2, double maxF2, double minF3)
{
	try {
		integer numberOfFormantObjects = my size, minNumberOfFormants = 1000000;
		if (numberOfFormantObjects == 1) {
			return 1;
		}
		autoNUMvector<integer> numberOfFormants (1, numberOfFormantObjects);
		autoNUMvector<int> invalid (1, numberOfFormantObjects);
		double tminf = 0.0, tmaxf = 0.0;
		for (integer iobject = 1; iobject <= numberOfFormantObjects; iobject ++) {
			// Check that all Formants have the same domain
			Formant fi = my at [iobject];
			if (tminf == tmaxf) {
				tminf = fi -> xmin; tmaxf = fi -> xmax;
			} else if (fi -> xmin != tminf || fi -> xmax != tmaxf) {
				Melder_throw (U"All Formant objects must have the same starting and finishing times.");
			}
			// Find the one that has least formant tracks
			numberOfFormants [iobject] = Formant_getMaxNumFormants (fi);
			if (numberOfFormants [iobject] < minNumberOfFormants) {
				minNumberOfFormants = numberOfFormants [iobject];
			}
		}
		if (numberOfFormantTracks == 0) { // default
			numberOfFormantTracks = minNumberOfFormants;
		}
		if (numberOfFormantTracks > minNumberOfFormants) {
			// make formants with not enough tracks invalid for the competition
			integer numberOfInvalids = 0;
			for (integer iobject = 1; iobject <= numberOfFormantObjects; iobject ++) {
				if (numberOfFormants [iobject] < numberOfFormantTracks) {
					invalid [iobject] = 1;
					numberOfInvalids ++;
				}
			}
			Melder_require (numberOfInvalids < numberOfFormantObjects, U"None of the Formants has enough formant tracks. Please, lower your upper formant number.");
			
		}
		if (tmax <= tmin) { // default
			tmin = tminf; tmax = tmaxf;
		}
		Melder_require (tmin >= tminf && tmax <= tmaxf, U"The selected interval should be within the Formant object's domain.");
		
		/* The chisq is not meaningfull as a the only test whether one model is better than the other because 
			if we have two models 1 & 2 with the same data points (x1 [i]=x2 [i] and y1 [i]= y2 [i] but if 
			sigma1 [i] < sigma2 [i] than chisq1 > chisq2.
			This is not what we want.
			We test therefore the variances of the parameters because if sigma1 [i] < sigma2 [i] than pvar1 < pvar2.
		 */
		double minChiVar = 1e308;
		integer index = 0;
		for (integer iobject = 1; iobject <= numberOfFormantObjects; iobject ++) {
			if (invalid [iobject] != 1) {
				Formant fi = my at [iobject];
				autoFormantModeler fs = Formant_to_FormantModeler (fi, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack, useBandWidthsForTrackEstimation);
				FormantModeler_setParameterValuesToZero (fs.get(), 1, numberOfFormantTracks, numberOfSigmas);
				double cf = useConstraints ? FormantModeler_getFormantsConstraintsFactor (fs.get(), minF1, maxF1, minF2, maxF2, minF3) : 1;
				double chiVar = FormantModeler_getSmoothnessValue (fs.get(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
				if (isdefined (chiVar) && cf * chiVar < minChiVar) {
					minChiVar = cf * chiVar;
					index = iobject;
				}
			}
		}
		return index;
	} catch (MelderError) {
		Melder_throw (U"No Formant object could be selected.");
	}
}

autoFormant Formant_extractPart (Formant me, double tmin, double tmax) {
	try {
		if (tmin >= tmax) {
			tmin = my xmin; tmax = my xmax;
		}
		Melder_require (tmin < my xmax && tmax > my xmin, U"Your start and end time should be between ", my xmin, U" and ", my xmax, U".");
		
		integer thyindex = 1, ifmin, ifmax;
		integer numberOfFrames = Sampled_getWindowSamples (me, tmin, tmax, & ifmin, & ifmax);
		double t1 = Sampled_indexToX (me, ifmin);
		autoFormant thee = Formant_create (tmin, tmax, numberOfFrames, my dx, t1, my maxnFormants);
		for (integer iframe = ifmin; iframe <= ifmax; iframe++, thyindex++) {
			Formant_Frame myFrame = & my d_frames [iframe];
			Formant_Frame thyFrame = & thy d_frames [thyindex];
			myFrame -> copy (thyFrame);
		}
		return thee;
		
	} catch (MelderError) {
		Melder_throw (U"Formant part could not be extracted.");
	}
}

autoFormant Formants_extractSmoothestPart (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, double numberOfSigmas, double power)
{
	try {
		integer index = Formants_getSmoothestInInterval (me, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,
			useBandWidthsForTrackEstimation, 0, numberOfSigmas, power, 1.0, 1.0, 1.0, 1.0, 1.0); // last five are just fillers
		Formant bestfit = my at [index];
		autoFormant thee = Formant_extractPart (bestfit, tmin, tmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Smoothest Formant part could not be extracted.");
	}
}


autoFormant Formants_extractSmoothestPart_withFormantsConstraints (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack,	int useBandWidthsForTrackEstimation, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3) {
	try {
		integer index = Formants_getSmoothestInInterval (me, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,
			useBandWidthsForTrackEstimation, 1, numberOfSigmas, power, minF1, maxF1, minF2, maxF2, minF3);
		Formant bestfit = my at [index];
		autoFormant thee = Formant_extractPart (bestfit, tmin, tmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Smoothest Formant part could not be extracted.");
	}
}

Thing_implement (PitchModeler, DataModeler, 0);

autoPitchModeler Pitch_to_PitchModeler (Pitch me, double tmin, double tmax, integer numberOfParameters) {
	try {
		integer ifmin, ifmax;
		if (tmax <= tmin) {
			tmin = my xmin; tmax = my xmax;
		}
		integer numberOfDataPoints = Sampled_getWindowSamples (me, tmin, tmax, & ifmin, & ifmax);
		Melder_require (numberOfParameters <= numberOfDataPoints, U"The number of parameters should not exceed the number of data points. Please, extend the selection.");
		
		autoPitchModeler thee = Thing_new (PitchModeler);
		DataModeler_init (thee.get(), tmin, tmax, numberOfDataPoints, numberOfParameters, DataModeler_TYPE_LEGENDRE);
		integer idata = 0, validData = 0;
		for (integer iframe = ifmin; iframe <= ifmax; iframe ++) {
			thy x [++ idata] = Sampled_indexToX (me, iframe);
			thy dataPointStatus [idata] = DataModeler_DATA_INVALID;
			if (Pitch_isVoiced_i (me, iframe)) {
				thy y [idata] = my frame [iframe]. candidate [1]. frequency;
				thy dataPointStatus [idata] = DataModeler_DATA_VALID;
				validData ++;
			}
		}
		thy numberOfDataPoints = idata;
		if (validData < numberOfParameters) { // remove don't throw exception
			Melder_throw (U"Not enough valid data in interval.");
		}
		DataModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No PitchModeler could be created.");
	}
}

void PitchModeler_draw (PitchModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax, integer numberOfParameters, int garnish) {
	Graphics_setInner (g);
	DataModeler_drawTrack_inside (me, g, tmin, tmax, fmin, fmax, 1, numberOfParameters, 0);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 100.0, true, true, true);
	}
}

double Sound_getOptimalFormantCeiling (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps, double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power) {
	double optimalCeiling;
	autoFormant thee = Sound_to_Formant_interval (me, startTime, endTime, windowLength, timeStep, minFreq, maxFreq,  numberOfFrequencySteps, preemphasisFrequency, numberOfFormantTracks, numberOfParametersPerTrack, weighData,  numberOfSigmas, power, false, 0.0, 5000.0, 0.0, 5000.0, 0.0, & optimalCeiling);
	return optimalCeiling;
}

autoFormant Sound_to_Formant_interval (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps, double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *p_optimalCeiling) {
	try {
		// parameter check
		if (endTime <= startTime) {
			startTime = my xmin; endTime = my xmax;
		}
		double nyquistFrequency = 0.5 / my dx;
		Melder_require (maxFreq <= nyquistFrequency, U"The upper value of the maximum frequency range should not exceed the Nyquist frequency of the sound.");
		
		double df = 0, mincriterium = 1e28;
		if (minFreq >= maxFreq) {
			numberOfFrequencySteps = 1;
		} else {
			df = (maxFreq - minFreq) / (numberOfFrequencySteps - 1);
		}
		double optimalCeiling = minFreq;
		integer i_best = 0;
		
		// extract part +- windowLength because of Gaussian windowing in the formant analysis
		// +timeStep/2 to have the analysis points maximally spread in the new domain.
		
		autoSound part = Sound_extractPart (me, startTime - windowLength + timeStep / 2.0, endTime + windowLength + timeStep / 2.0, kSound_windowShape::RECTANGULAR, 1, 1);

		// Resample to 2*maxFreq to reduce resampling load in Sound_to_Formant
		
		autoSound resampled = Sound_resample (part.get(), 2.0 * maxFreq, 50);
		OrderedOf<structFormant> formants;
		Melder_progressOff ();
		for (integer i = 1; i <= numberOfFrequencySteps; i ++) {
			double currentCeiling = minFreq + (i - 1) * df;
			autoFormant formant = Sound_to_Formant_burg (resampled.get(), timeStep, 5.0, currentCeiling, windowLength, preemphasisFrequency);
			autoFormantModeler fm = Formant_to_FormantModeler (formant.get(), startTime, endTime, numberOfFormantTracks, numberOfParametersPerTrack, weighData);
			FormantModeler_setParameterValuesToZero (fm.get(), 1, numberOfFormantTracks, numberOfSigmas);
			formants. addItem_move (formant.move());
			double cf = ( useConstraints ? FormantModeler_getFormantsConstraintsFactor (fm.get(), minF1, maxF1, minF2, maxF2, minF3) : 1 );
			double chiVar = FormantModeler_getSmoothnessValue (fm.get(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
			double criterium = chiVar * cf;
			if (isdefined (chiVar) && criterium < mincriterium) {
				mincriterium = criterium;
				optimalCeiling = currentCeiling;
				i_best = i;
			}
		}
		autoFormant thee = Formant_extractPart (formants.at [i_best], startTime, endTime);
		Melder_progressOn ();
		if (p_optimalCeiling) {
			*p_optimalCeiling = optimalCeiling;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Formant object created.");
	}
}

autoFormant Sound_to_Formant_interval_robust (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps, double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *p_optimalCeiling) {
	try {
		// parameter check
		if (endTime <= startTime) {
			startTime = my xmin; endTime = my xmax;
		}
		double nyquistFrequency = 0.5 / my dx;
		Melder_require (maxFreq <= nyquistFrequency, U"The upper value of the maximum frequency range should not exceed the Nyquist frequency of the sound.");
		double df = 0, mincriterium = 1e28;
		if (minFreq >= maxFreq) {
			numberOfFrequencySteps = 1;
		} else {
			df = (maxFreq - minFreq) / (numberOfFrequencySteps - 1);
		}
		integer i_best = 0;
		double optimalCeiling = minFreq;
		// extract part +- windowLength because of Gaussian windowing in the formant analysis
		// +timeStep/2 to have the analysis points maximally spread in the new domain.
		
		autoSound part = Sound_extractPart (me, startTime - windowLength + timeStep / 2, endTime + windowLength + timeStep / 2, kSound_windowShape::RECTANGULAR, 1, 1);

		// Resample to 2*maxFreq to reduce resampling load in Sound_to_Formant
		
		autoSound resampled = Sound_resample (part.get(), 2.0 * maxFreq, 50);
		OrderedOf<structFormant> formants;
		Melder_progressOff ();
		for (integer i = 1; i <= numberOfFrequencySteps; i ++) {
			double currentCeiling = minFreq + (i - 1) * df;
			autoFormant formant = Sound_to_Formant_robust (resampled.get(), timeStep, 5.0, currentCeiling, windowLength, preemphasisFrequency, 50.0, 1.5, 3, 0.0000001, 1);
			autoFormantModeler fm = Formant_to_FormantModeler (formant.get(), startTime, endTime, numberOfFormantTracks, numberOfParametersPerTrack, weighData);
			FormantModeler_setParameterValuesToZero (fm.get(), 1, numberOfFormantTracks, numberOfSigmas);
			formants. addItem_move (formant.move());
			double cf = ( useConstraints ? FormantModeler_getFormantsConstraintsFactor (fm.get(), minF1, maxF1, minF2, maxF2, minF3) : 1 );
			double chiVar = FormantModeler_getSmoothnessValue (fm.get(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
			double criterium = chiVar * cf;
			if (isdefined (chiVar) && criterium < mincriterium) {
				mincriterium = criterium;
				optimalCeiling = currentCeiling;
				i_best = i;
			}
		}
		autoFormant thee = Formant_extractPart (formants.at [i_best], startTime, endTime);
		Melder_progressOn ();
		if (p_optimalCeiling) {
			*p_optimalCeiling = optimalCeiling;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Formant object created.");
	}
}

#if 0
// If e.g. first formant is obviously "missing" then assign F1 as 
static void FormantModeler_correctFormantsProbablyIndexedFalsely (FormantModeler /* me */) {
	/* if shift down F1 ("correct" F1 missed)
	 * elsif shift down F2  ("correct" F2 missed)
	 * else if spurious formant before F1
	 * else if spurious formant between F1 and F2
	 * endif
	 * */
}
#endif

autoOptimalCeilingTier Sound_to_OptimalCeilingTier (Sound me, double windowLength, double timeStep, double minCeiling, double maxCeiling, integer numberOfFrequencySteps, double preemphasisFrequency, double smoothingWindow, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power) {
	try {
		OrderedOf<structFormant> formants;
		double frequencyStep = numberOfFrequencySteps > 1 ? (maxCeiling - minCeiling) / (numberOfFrequencySteps - 1) : 0;
		for (integer i = 1; i <= numberOfFrequencySteps; i ++) {
			double ceiling = minCeiling + (i - 1) * frequencyStep;
			autoFormant formant = Sound_to_Formant_burg (me, timeStep, 5, ceiling, windowLength, preemphasisFrequency);
			formants. addItem_move (formant.move());
		}
		integer numberOfFrames;
		double firstTime, modelingTimeStep = timeStep;
		autoOptimalCeilingTier octier = OptimalCeilingTier_create (my xmin, my xmax);
		Sampled_shortTermAnalysis (me, smoothingWindow, modelingTimeStep, & numberOfFrames, & firstTime);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			double time = firstTime + (iframe - 1) * modelingTimeStep;
			double tmin = time - smoothingWindow / 2.0;
			double tmax = tmin + smoothingWindow;
			integer index = Formants_getSmoothestInInterval (& formants, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,	weighData, 0, numberOfSigmas, power, 200.0, 1500.0, 300.0, 3000.0, 1000.0); // min/max values are not used
			double ceiling = minCeiling + (index - 1) * frequencyStep;
			RealTier_addPoint (octier.get(), time, ceiling);
		}
		return octier;
	} catch (MelderError) {
		Melder_throw (me, U" no OptimalCeilingTier calculated.");
	}
}

/* End of file DataModeler.cpp */
