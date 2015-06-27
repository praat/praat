/* DataModeler.cpp
 *
 * Copyright (C) 2014 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
	double ndf, rSquared = DataModeler_getCoefficientOfDetermination (this, NULL, NULL);
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
	for (long ipar = 1; ipar <= numberOfParameters; ipar++) {
		double sigma = parameterStatus[ipar] == DataModeler_PARAMETER_FIXED ? 0 : sqrt (parameterCovariances -> data[ipar][ipar]);
		MelderInfo_writeLine (U"      p[", ipar, U"] = ", parameter[ipar], U"; sigma = ", sigma);
	}
}

static double polynome_evaluate (DataModeler me, double x, double p[])
{
	double xpi = 1, result = p[1];
	for (long i = 2; i <= my numberOfParameters; i++) {
		xpi *= x;
		result += p[i] * xpi;
	}
	return result;
}

static void polynome_evaluateBasisFunctions (DataModeler me, double xin, double term[])
{
	term[1] = 1;
	// From domain [xmin, xmax] to domain [-1, 1]
	double x = (2 * xin - my xmin - my xmax) / (my xmax - my xmin);
	for (long i = 2; i <= my numberOfParameters; i++) {
		term[i] = term[i-1] * x;
	}
}

static double legendre_evaluate (DataModeler me, double xin, double p[])
{
	// From domain [xmin, xmax] to domain [-1, 1]
	double x = (2 * xin - my xmin - my xmax) / (my xmax - my xmin);
	double pti, ptim1, ptim2 = 1, result = p[1];
	if (my numberOfParameters > 1) {
		double twox = 2 * x, f2 = x, d = 1.0;
		result += p[2] * (ptim1 = x);
		for (long i = 3; i <= my numberOfParameters; i++) {
			double f1 = d++;
			f2 += twox;
			result += p[i] * (pti = (f2 * ptim1 - f1 * ptim2) / d);
			ptim2 = ptim1; ptim1 = pti;
		}
	}
	return result;
}

static void legendre_evaluateBasisFunctions (DataModeler me, double xin, double term[])
{
	term[1] = 1;
	/* transform x from domain [xmin, xmax] to domain [-1, 1] */
	double x = ( 2 * xin - my xmin - my xmax) / (my xmax - my xmin);
	if (my numberOfParameters > 1) {
		double twox = 2 * x, f2 = term[2] = x, d = 1.0;
		for (long i = 3; i <= my numberOfParameters; i++) {
			double f1 = d++;
			f2 += twox;
			term[i] = (f2 * term[i-1] - f1 * term[i-2]) / d;
		}
	}
}

double DataModeler_getDataPointWeight (DataModeler me, long iPoint, int useSigmaY ) {
	double weight = 1;
	if (iPoint > 0 && iPoint <= my numberOfDataPoints && my dataPointStatus[iPoint] != DataModeler_DATA_INVALID) {
		if (useSigmaY == DataModeler_DATA_WEIGH_SIGMA) {
			weight = my sigmaY[iPoint];
		} else if (useSigmaY == DataModeler_DATA_WEIGH_RELATIVE) {
			double q = my y[iPoint] / my sigmaY[iPoint];
			weight = 500 / q; //
		} else if (useSigmaY == DataModeler_DATA_WEIGH_SQRT) {
			weight = 7.071 * sqrt (my sigmaY[iPoint]); // .bw = 50 gives 50
		}
	}
	return weight;
}

double DataModeler_getModelValueAtX (DataModeler me, double x) {
	double f = NUMundefined;
	if (x >= my xmin && x <= my xmax) {
		f = my f_evaluate (me, x, my parameter);
	}
	return f;
}

void DataModeler_getExtremaY (DataModeler me, double *ymin, double *ymax) {
	double min = 1e38, max = -min;
	for (long i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
			if (my y[i] < min) {
				min = my y[i];
			}
			if (my y[i] > max) {
				max = my y[i];
			}
		}
	}
	if (ymin != NULL) {
		*ymin = min;
	}
	if (ymax != NULL) {
		*ymax = max;
	}
}

double DataModeler_getDataPointValue (DataModeler me, long index) {
	double value = NUMundefined;
	if (index > 0 && index <= my numberOfDataPoints && my dataPointStatus[index] != DataModeler_DATA_INVALID) {
		value = my y[index];
	}
	return value;
}

void DataModeler_setDataPointValue (DataModeler me, long index, double value) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my y[index] = value;
	}
}

void DataModeler_setDataPointSigma (DataModeler me, long index, double sigma) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my sigmaY[index] = sigma;
	}
}

double DataModeler_getDataPointSigma (DataModeler me, long index) {
	double sigma = NUMundefined;
	if (index > 0 && index <= my numberOfDataPoints) {
		sigma = my sigmaY[index];
	}
	return sigma;
}

int DataModeler_getDataPointStatus (DataModeler me, long index) {
	int value = DataModeler_DATA_INVALID;
	if (index > 0 && index <= my numberOfDataPoints) {
		value = my dataPointStatus[index];
	}
	return value;
}

void DataModeler_setDataPointStatus (DataModeler me, long index, int status) {
	if (index > 0 && index <= my numberOfDataPoints) {
		if (status == DataModeler_DATA_VALID && ! NUMdefined (my y[index])) {
			Melder_throw (U"Your data value is undefined. First set the value and then its status.");
		}
		my dataPointStatus[index] = status;
	}
}

void DataModeler_setDataPointValueAndStatus (DataModeler me, long index, double value, int dataStatus) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my y[index] = value;
		my dataPointStatus[index] = dataStatus;
	}
}

void DataModeler_setParameterValueFixed (DataModeler me, long index, double value) {
	if (index > 0 && index <= my numberOfParameters) {
		my parameter[index] = value;
		my parameterStatus[index] = DataModeler_PARAMETER_FIXED;
	}
}

double DataModeler_getParameterValue (DataModeler me, long index) {
	double value = NUMundefined;
	if (index > 0 && index <= my numberOfParameters) {
		value = my parameter[index];
	}
	return value;
}

int DataModeler_getParameterStatus (DataModeler me, long index) {
	int status = DataModeler_PARAMETER_UNDEFINED;
	if (index > 0 && index <= my numberOfParameters) {
		status = my parameterStatus[index];
	}
	return status;
}

double DataModeler_getParameterStandardDeviation (DataModeler me, long index) {
	double stdev = NUMundefined;
	if (index > 0 && index <= my numberOfParameters) {
		stdev = sqrt (my parameterCovariances -> data[index][index]);
	}
	return stdev;
}

double DataModeler_getVarianceOfParameters (DataModeler me, long fromIndex, long toIndex, long *numberOfFreeParameters) {
	double variance = NUMundefined;
	if (toIndex < fromIndex || (toIndex == 0 && fromIndex == 0)) {
		fromIndex = 1; toIndex = my numberOfParameters;
	}
	long numberOfParameters = 0;
	if (fromIndex <= toIndex && fromIndex > 0 && toIndex <= my numberOfParameters) {
		variance = 0;
		for (long index = fromIndex; index <= toIndex; index++) {
			if (my parameterStatus[index] != DataModeler_PARAMETER_FIXED) {
				variance += my parameterCovariances -> data[index][index];
				numberOfParameters++;
			}
		}
	}
	if (numberOfFreeParameters != NULL) {
		*numberOfFreeParameters = numberOfParameters;
	}
	return variance;
}

void DataModeler_setParametersFree (DataModeler me, long fromIndex, long toIndex) {
	if (toIndex < fromIndex || (toIndex == 0 && fromIndex == 0)) {
		fromIndex = 1; toIndex = my numberOfParameters;
	}
	if (fromIndex <= toIndex && fromIndex > 0 && toIndex <= my numberOfParameters) {
		for (long index = fromIndex; index <= toIndex; index++) {
			my parameterStatus[index] = DataModeler_PARAMETER_FREE;
		}
	}
}

void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas) {
	long numberOfChangedParameters = 0;
	for (long i = my numberOfParameters; i > 0; i--) {
		if (my parameterStatus[i] != DataModeler_PARAMETER_FIXED) {
			double value = my parameter[i];
			double sigmas = numberOfSigmas * DataModeler_getParameterStandardDeviation (me, i);
			if ((value - sigmas) * (value + sigmas) < 0) {
				DataModeler_setParameterValueFixed (me, i, 0);
				numberOfChangedParameters++;
			}
		}
	}
}

static long DataModeler_getNumberOfFreeParameters (DataModeler me) {
	long numberOfFreeParameters = 0;
	for (long i = 1; i <= my numberOfParameters; i++) {
		if (my parameterStatus[i] == DataModeler_PARAMETER_FREE) numberOfFreeParameters++;
	}
	return numberOfFreeParameters;
}

long DataModeler_getNumberOfFixedParameters (DataModeler me) {
	return my numberOfParameters - DataModeler_getNumberOfFreeParameters (me);
}

static long DataModeler_getNumberOfValidDataPoints (DataModeler me) {
	long numberOfValidDataPoints = 0;
	for (long i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
			numberOfValidDataPoints++;
		}
	}
	return numberOfValidDataPoints;
}

long DataModeler_getNumberOfInvalidDataPoints (DataModeler me) {
	return my numberOfDataPoints - DataModeler_getNumberOfValidDataPoints  (me);
}

void DataModeler_setTolerance (DataModeler me, double tolerance) {
	my tolerance = tolerance > 0 ? tolerance : my numberOfDataPoints * NUMfpp -> eps;
}

double DataModeler_getDegreesOfFreedom (DataModeler me) {
	long numberOfDataPoints = 0;
	for (long i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
			numberOfDataPoints++;
		}
	}
	double ndf = numberOfDataPoints - DataModeler_getNumberOfFreeParameters (me);
	return ndf;
}

double *DataModeler_getZScores (DataModeler me, int useSigmaY) {
	try {
		double estimatedSigmaY;
		if (useSigmaY == DataModeler_DATA_WEIGH_EQUAL) {
			estimatedSigmaY = DataModeler_estimateSigmaY (me);
			if (! NUMdefined (estimatedSigmaY)) {
				Melder_throw (U"Not enough data points to calculate sigma.");
			}
		}
		autoNUMvector<double> zscores (1, my numberOfDataPoints);
		for (long i = 1; i <= my numberOfDataPoints; i++) {
			double value = NUMundefined;
			if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
				double estimate = my f_evaluate (me, my x[i], my parameter);
				double sigma = useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? estimatedSigmaY : DataModeler_getDataPointWeight (me, i, useSigmaY);
				value = (my y[i] - estimate) / sigma;
			}
			zscores[i] = value;
		}
		return zscores.transfer();
	} catch (MelderError) {
		Melder_throw (U"No z-scores calculated.");
	}
}

double DataModeler_getChiSquaredQ (DataModeler me, int useSigmaY, double *probability, double *ndf)
{
	double chisq = 0;
	autoNUMvector<double> zscores (DataModeler_getZScores (me, useSigmaY), 1);
	for (long idata = 1; idata <= my numberOfDataPoints; idata++)
	{
		if (NUMdefined (zscores[idata])) {
			chisq += zscores[idata] * zscores[idata];
		}
	}
	double dof =  DataModeler_getDegreesOfFreedom (me);
	dof = useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? dof - 1 : dof; // we loose one dof if sigma is estimated from the data
	if (probability != NULL) {
		*probability = NUMchiSquareQ (chisq, dof);
	}
	if (ndf != NULL) {
		*ndf = dof;
	}
	return chisq;
}

double DataModeler_getWeightedMean (DataModeler me) {
	double ysum = 0, wsum = 0;
	for (long i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
			double s = DataModeler_getDataPointWeight (me, i, my useSigmaY);
			double weight =  1 / (s * s);
			ysum += my y[i] * weight;
			wsum += weight;
		}
	}
	return ysum / wsum;
}

double DataModeler_getCoefficientOfDetermination (DataModeler me, double *ssreg, double *sstot) {

	/* We cannot use the standard expressions for ss_tot, and ss_reg because our data are weighted by 1 / sigma[i].
	 * We need the weighted mean and we need to weigh all sums-of-squares accordingly;
	 * if all sigma[i] terms are equal, the formulas reduce to the standard ones.
	 * Ref: A. Buse (1973): Goodness of Fit in Generalized Least Squares Estimatio, The American Statician, vol 27, 106-108
	 */

	double ymean = DataModeler_getWeightedMean (me);
	double ss_tot = 0, ss_reg = 0;
	for (long i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
			double s = DataModeler_getDataPointWeight (me, i, my useSigmaY);
			double diff = (my y[i] - ymean) / s;
			ss_tot += diff * diff; // total sum of squares
			double estimate = my f_evaluate (me, my x[i], my parameter);
			diff = (estimate - my y[i]) / s;
			ss_reg += diff * diff; // regression sum of squares
		}
	}
	double rSquared = ss_tot > 0 ? 1 - ss_reg / ss_tot : 1;
	if (ssreg != NULL) {
		*ssreg = ss_tot - ss_reg;
	}
	if (sstot != NULL) {
		*sstot = ss_tot;
	}
	return rSquared;
}

void DataModeler_drawBasisFunction_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
 	long iterm, bool scale, long numberOfPoints) {
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax;
	}
	autoNUMvector<double> x (1, numberOfPoints);
	autoNUMvector<double> y (1, numberOfPoints);
	autoNUMvector<double> term (1, my numberOfParameters);
	for (long i = 1; i <= numberOfPoints; i++) {
		x[i] = xmin + (i - 0.5) * (xmax - xmin) / numberOfPoints;
		my f_evaluateBasisFunctions (me, x[i], term.peek());
		y[i] = term[iterm];
		y[i] = scale ? y[i] * my parameter[iterm] : y[i];
	}
	if (ymax <= ymin) {
		ymin = 1e38;ymax = -ymin;
		for (long i = 1; i <= numberOfPoints; i++) {
			ymax = y[i] > ymax ? y[i] : ymax;
			ymin = y[i] < ymin ? y[i] : ymin;
		}
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (long i = 2; i <= numberOfPoints; i++) {
		Graphics_line (g, x[i-1], y[i-1], x[i], y[i]);
	}
}

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int useSigmaY, double numberOfSigmas, char32 *mark, int marksFontSize, double horizontalOffset_mm) {
	if (xmax <= xmin) { 
		xmin = my xmin; xmax = my xmax;
	}
	long ixmin = 2;
	while (my x[ixmin] < xmin && ixmin < my numberOfDataPoints) { 
		ixmin++;
	}
	
	ixmin--;
	long ixmax = my numberOfDataPoints - 1;
	while (my x[ixmax] > xmax && ixmax > 1) {
		ixmax--;
	}
	ixmax++;
	if (ixmin >= ixmax) {
		return; // nothing to draw
	}
	
	autoNUMvector<double> zscores (DataModeler_getZScores (me, useSigmaY), 1);
	double horizontalOffset_wc = Graphics_dxMMtoWC (g, horizontalOffset_mm);
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setFontSize (g, marksFontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	int currentFontSize = Graphics_inqFontSize (g);
	for (long idata = 1; idata <= my numberOfDataPoints; idata++) {
		if (my dataPointStatus[idata] != DataModeler_DATA_INVALID) {
			double x = my x[idata], y = my y[idata];
			if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
				if (fabs (zscores[idata]) > numberOfSigmas) {
					Graphics_text (g, x + horizontalOffset_wc, y, mark);
				}
			}
		}
	}
	Graphics_setFontSize (g, currentFontSize);
}

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, int errorbars, int connectPoints, double barWidth_mm, double horizontalOffset_mm, int drawDots)
{
	if (xmax <= xmin) { 
		xmin = my xmin; xmax = my xmax; 
	}
	
	long ixmin = 2;
	while (my x[ixmin] < xmin && ixmin < my numberOfDataPoints) { 
		ixmin++;
	}
	ixmin--;
	
	long ixmax = my numberOfDataPoints - 1;
	while (my x[ixmax] > xmax && ixmax > 1) {
		ixmax--;
	}
	ixmax++;
	
	if (ixmin >= ixmax) {
		return; // nothing to draw
	}
	numberOfParameters = numberOfParameters > my numberOfParameters ? my numberOfParameters : numberOfParameters;
	autoNUMvector<double> parameter (1, my numberOfParameters);
	NUMvector_copyElements (my parameter, parameter.peek(), 1, numberOfParameters);
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double horizontalOffset_wc = Graphics_dxMMtoWC (g, horizontalOffset_mm);
	double barWidth_wc = barWidth_mm <= 0 ? 0 : Graphics_dxMMtoWC (g, barWidth_mm);
	bool x1defined = false, x2defined = false;
	for (long idata = ixmin; idata <= ixmax; idata++) {
		if (my dataPointStatus[idata] != DataModeler_DATA_INVALID) {
			double x1, y1, x2, y2, x = my x[idata], y = my y[idata];
			if (! x1defined) {
				x1 = x;
				y1 = estimated ? my f_evaluate (me, x, parameter.peek()) : y;
				y1 = y1 < ymin ? ymin : y1 > ymax ? ymax : y1;
				x1defined = true;
			} else {
				x2 = x;
				y2 = estimated ? my f_evaluate (me, x, parameter.peek()) : y;
				y2 = y2 < ymin ? ymin : y2 > ymax ? ymax : y2;
				x2defined = true;
			}
			if (x1defined && drawDots) {
				Graphics_speckle (g, x + horizontalOffset_wc, y);
			}
			if (x1defined && x2defined) {
				if (connectPoints) {
					Graphics_line (g, x1 + horizontalOffset_wc, y1, x2 + horizontalOffset_wc, y2);
				}
				x1 = x;
				y1 = y2;
			}
			if (x1defined && errorbars != 0) {
				double sigma = my sigmaY[idata]; // DataModeler_getDataPointWeight ?
				double ym = y1;
				double yt = ym + 0.5 * sigma, yb = ym - 0.5 * sigma;
				if (estimated) {
					yt = (y - y1) > 0 ? y : y1;
					yb = (y - y1) > 0 ? y1 : y;
				}
				int topOutside = yt > ymax, bottomOutside = yb < ymin;
				yt = topOutside ? ymax : yt;
				yb = bottomOutside ? ymin : yb;
				Graphics_line (g, x1 + horizontalOffset_wc, yb, x1 + horizontalOffset_wc, yt);
				if (barWidth_wc > 0 && ! estimated) {
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

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, double horizontalOffset_mm)
{
	int errorbars = 0, connectPoints = 1; double barWidth_mm = 0;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, horizontalOffset_mm, 0);
}

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, double horizontalOffset_mm, int garnish) {
	if (ymax <= ymin) {
		DataModeler_getExtremaY (me, &ymin, &ymax);
	}
	Graphics_setInner (g);
	DataModeler_drawTrack_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm)
{
	int connectPoints = 0;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, horizontalOffset_mm, 1);
}

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm, int garnish) {
	if (ymax <= ymin) {
		DataModeler_getExtremaY (me, &ymin, &ymax);
	}
	Graphics_setInner (g);
	DataModeler_speckle_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, barWidth_mm, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

Table DataModeler_to_Table_zscores (DataModeler me, int useSigmaY) {
	try {
		autoTable ztable = Table_createWithColumnNames (my numberOfDataPoints, U"x z");
		autoNUMvector<double> zscores (DataModeler_getZScores (me, useSigmaY), 1);
		for (long i = 1; i <= my numberOfDataPoints; i++) {
			Table_setNumericValue (ztable.peek(), i, 1, my x[i]);
			Table_setNumericValue (ztable.peek(), i, 2, zscores[i]);
		}
		return ztable.transfer();
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

void DataModeler_normalProbabilityPlot (DataModeler me, Graphics g, int useSigmaY, long numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, int garnish) {
	try {
		autoTable thee = DataModeler_to_Table_zscores (me, useSigmaY);
		Table_normalProbabilityPlot (thee.peek(), g, 2, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	} catch (MelderError) {
		// ignore
	}
}

void DataModeler_setBasisFunctions (DataModeler me, int type) {
	if (type == DataModeler_TYPE_LEGENDRE) {
		my f_evaluate = legendre_evaluate;
		my f_evaluateBasisFunctions = legendre_evaluateBasisFunctions;
	} else {
		my f_evaluate = polynome_evaluate;
		my f_evaluateBasisFunctions = polynome_evaluateBasisFunctions;
	}
	my type = type;
}

void  DataModeler_init (DataModeler me, double xmin, double xmax, long numberOfDataPoints, long numberOfParameters, int type) {
	my xmin = xmin; my xmax = xmax;
	DataModeler_setBasisFunctions (me, type);
	my numberOfDataPoints = numberOfDataPoints;
	my x = NUMvector<double> (1, numberOfDataPoints);
	my y = NUMvector<double> (1, numberOfDataPoints);
	my sigmaY = NUMvector<double> (1, numberOfDataPoints);
	my dataPointStatus = NUMvector<int> (1, numberOfDataPoints);
	my numberOfParameters = numberOfParameters;
	if (numberOfParameters <= 0) {
		Melder_throw (U"The number of parameters must be greater than zero.");
	}
	if (numberOfParameters > numberOfDataPoints) {
		Melder_throw (U"The number of parameters must be smaller than the number of data points");
	}
	my parameter = NUMvector<double> (1, numberOfParameters);
	my parameterStatus = NUMvector<int> (1, numberOfParameters);
	my parameterNames = Strings_createFixedLength (numberOfParameters);
	my parameterCovariances = Covariance_create (numberOfParameters);
}

DataModeler DataModeler_create (double xmin, double xmax, long numberOfDataPoints, long numberOfParameters, int type) {
	try {
		autoDataModeler me = Thing_new (DataModeler);
		DataModeler_init (me.peek(), xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		my xmin = xmin; my xmax = xmax;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}
}

void DataModeler_fit (DataModeler me)
{
	try {
		// Count the number of parameters to be fitted

		long numberOfParameters = DataModeler_getNumberOfFreeParameters (me);
		if (numberOfParameters == 0) return;
		long numberOfDataPoints = DataModeler_getNumberOfValidDataPoints (me);
		autoNUMvector<double> b (1, numberOfDataPoints);
		autoNUMvector<double> term (1, my numberOfParameters);
		autoNUMvector<double> parameter (1, my numberOfParameters);
		autoNUMmatrix<double> design (1, numberOfDataPoints, 1, numberOfParameters);

		// For function evaluation with only the FIXED parameters

		for (long ipar = 1; ipar <= my numberOfParameters; ipar++) {
			parameter[ipar] = my parameterStatus[ipar] == DataModeler_PARAMETER_FIXED ? my parameter[ipar] : 0;
		}

		// estimate sigma if we weigh all datapoint equally. 
		// This is necessary to get the parameter covariances right
		double sigmaY = my useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? DataModeler_estimateSigmaY (me) : NUMundefined;
		long idata = 0;
		// Accumulate coefficients of the design matrix
		for (long i = 1; i <= my numberOfDataPoints; i++) {
			if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
				// function evaluation with only the FIXED parameters
				double xi = my x[i], yi = my y[i];
				double yFixed = my f_evaluate (me, xi, parameter.peek());
				double si = my useSigmaY != DataModeler_DATA_WEIGH_EQUAL ? DataModeler_getDataPointWeight (me, i, my useSigmaY) : sigmaY;

				// individual terms of the function

				my f_evaluateBasisFunctions (me, xi, term.peek());
				long ipar = 0; ++idata;
				for (long j = 1; j <= my numberOfParameters; j++) {
					if (my parameterStatus[j] != DataModeler_PARAMETER_FIXED) {
						design[idata][++ipar] = term[j] / si;
					}
				}

				// only 'residual variance' must be explained by the model

				b[idata] = (yi - yFixed) / si;
			}
		}
		
		// Singular value decomposition and evaluation of the singular values

		autoSVD thee = SVD_create_d (design.peek(), numberOfDataPoints, numberOfParameters);
		if (! NUMfpp) {
			NUMmachar ();
		}
		SVD_zeroSmallSingularValues (thee.peek(), my tolerance > 0 ? my tolerance : numberOfDataPoints * NUMfpp -> eps);
		SVD_solve (thee.peek(), b.peek(), parameter.peek()); // re-use parameter

		// Put the calculated parameters at the correct position in 'my p'
		Covariance cov = my parameterCovariances;
		long ipar = 0;
		for (long j = 1; j <= my numberOfParameters; j++) {
			if (my parameterStatus[j] != DataModeler_PARAMETER_FIXED) {
				my parameter[j] = parameter[++ipar];
			}
			cov -> centroid[j] = my parameter[j];
		}
		cov -> numberOfObservations = numberOfDataPoints;
		// estimate covariances between parameters
		if (numberOfParameters < my numberOfParameters) {
			autoNUMmatrix<double> covtmp (1, numberOfParameters, 1, numberOfParameters);
			SVD_getSquared (thee.peek(), covtmp.peek(), true);
			// Set fixed parameters variances and covariances to zero.
			for (long i = 1; i <= my numberOfParameters; i++) {
				for (long j = i; j <= my numberOfParameters; j++) {
					cov -> data[i][j] = cov -> data[j][i] = 0;
				}
			}
			long ipar = 0, jpar;
			for (long i = 1; i <= my numberOfParameters; i++) {
				if (my parameterStatus[i] != DataModeler_PARAMETER_FIXED) {
					jpar = 0; ipar++;
					for (long j = 1; j <= my numberOfParameters; j++) {
						if (my parameterStatus[j] != DataModeler_PARAMETER_FIXED) {
							jpar++;
							cov -> data[i][j] = covtmp[ipar][jpar];
						}
					}
				}
			}
		} else {
			SVD_getSquared (thee.peek(), cov -> data, true);
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

Covariance DataModeler_to_Covariance_parameters (DataModeler me) {
	try {
		autoCovariance cov = (Covariance) Data_copy (my parameterCovariances);
		return cov.transfer();
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
}

DataModeler Table_to_DataModeler (Table me, double xmin, double xmax, long xcolumn, long ycolumn, long scolumn, long numberOfParameters, int type) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, xcolumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, ycolumn);
		int useSigmaY = scolumn > 0;
		if (useSigmaY) {
			Table_checkSpecifiedColumnNumberWithinRange (me, scolumn);
		}
		long numberOfRows = my rows -> size, numberOfData = 0;
		autoNUMvector<double> x (1, numberOfRows), y (1, numberOfRows), sy (1, numberOfRows);
		for (long i = 1; i <= numberOfRows; i++) {
			double val = Table_getNumericValue_Assert (me, i, xcolumn);
			if (NUMdefined (val)) {
				numberOfData++; x[numberOfData] = val;
				if (numberOfData > 1) {
					if (val < x[numberOfData - 1]) {
						Melder_throw (U"Data with x-values must be sorted.");
					} else if (val == x[numberOfData - 1]) {
						Melder_throw (U"All x-values must be different.");
					}
				}
				y[numberOfData] = Table_getNumericValue_Assert (me, i, ycolumn);
				sy[numberOfData] = useSigmaY ? Table_getNumericValue_Assert (me, i, scolumn) : 1;
			}
		}
		if (xmax <= xmin) {
			NUMvector_extrema<double> (x.peek(), 1, numberOfData, &xmin, &xmax);
		}
		if (xmin >= xmax) {
			Melder_throw (U"Range of x-values too small.");
		}
		long numberOfDataPoints = 0, validData = 0;
		for (long i = 1; i <= numberOfData; i++) {
			if (x[i] >= xmin && x[i] <= xmax) {
				numberOfDataPoints++;
			}
		}
		autoDataModeler thee = DataModeler_create (xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		numberOfDataPoints = 0;
		for (long i = 1; i <= numberOfData; i++) {
			if (x[i] >= xmin && x[i] <= xmax) {
				thy x[++numberOfDataPoints] = x[i];
				thy dataPointStatus[numberOfDataPoints] = DataModeler_DATA_INVALID;
				if (NUMdefined (y[i])) {
					thy y[numberOfDataPoints] = y[i];
					thy sigmaY[numberOfDataPoints] = sy[i];
					thy dataPointStatus[numberOfDataPoints] = DataModeler_DATA_VALID;
					validData++;
				}
			}
		}
		thy useSigmaY = useSigmaY;
		thy numberOfDataPoints = numberOfDataPoints;
		thy tolerance = 1e-5;
		if (validData < numberOfParameters) {
			Melder_throw (U"The number of parameters must not exceed the number of data points.");
		}
		DataModeler_setDataWeighing (thee.peek(), DataModeler_DATA_WEIGH_SIGMA);
		DataModeler_fit (thee.peek());
		return thee.transfer();
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
	for (long iformant = 1; iformant <= datamodelers -> size; iformant++) {
		DataModeler ffi = (DataModeler) datamodelers -> item[iformant];
		MelderInfo_writeLine (U"Formant ", iformant);
		ffi -> v_info();
	}
}

double DataModeler_getResidualSumOfSquares (DataModeler me, long *numberOfDataPoints) {
	long n = 0;
	double rss = 0;
	for (long i = 1; i <= my numberOfDataPoints; i++) {
		if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
				++n;
				double dif = my y[i] - my f_evaluate (me, my x[i], my parameter);
				rss += dif * dif;
		}
	}
	if (numberOfDataPoints) {
		*numberOfDataPoints = n;
	}
	return rss;
}

double DataModeler_estimateSigmaY (DataModeler me) {
	try {
		long numberOfDataPoints = 0;
		autoNUMvector<double> y (1, my numberOfDataPoints);
		for (long i = 1; i <= my numberOfDataPoints; i++) {
			if (my dataPointStatus[i] != DataModeler_DATA_INVALID) {
				y[++numberOfDataPoints] = my y[i];
			}
		}
		double variance;
		NUMvector_avevar (y.peek(), numberOfDataPoints, NULL, &variance);
		double sigma = NUMdefined (variance) ? sqrt (variance / (numberOfDataPoints - 1)) : NUMundefined;
		return sigma;
	} catch (MelderError) {
		Melder_throw (U"Cannot estimate sigma.");
	}
}

double FormantModeler_getStandardDeviation (FormantModeler me, long iformant) {
	double sigma = NUMundefined;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		sigma = DataModeler_estimateSigmaY (ff);
	}
	return sigma;
}

double FormantModeler_getDataPointValue (FormantModeler me, long iformant, long index) {
	double value = NUMundefined;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		value = DataModeler_getDataPointValue (ff, index);
	}
	return value;
}

void FormantModeler_setDataPointValue (FormantModeler me, long iformant, long index, double value) {
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
 		DataModeler_setDataPointValue (ff, index, value);
	}
}

double FormantModeler_getDataPointSigma (FormantModeler me, long iformant, long index) {
	double sigma = NUMundefined;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		sigma = DataModeler_getDataPointSigma (ff, index);
	}
	return sigma;
}

void FormantModeler_setDataPointSigma (FormantModeler me, long iformant, long index, double sigma) {
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
 		DataModeler_setDataPointSigma (ff, index, sigma);
	}
}

int FormantModeler_getDataPointStatus (FormantModeler me, long iformant, long index) {
	int value = DataModeler_DATA_INVALID;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		value = DataModeler_getDataPointStatus (ff, index);
	}
	return value;
}

void FormantModeler_setDataPointStatus (FormantModeler me, long iformant, long index, int status)
{
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_setDataPointStatus (ff, index, status);
	}
}

void FormantModeler_setDataPointValueAndStatus (FormantModeler me, long iformant, long index, double value, int dataStatus)
{
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_setDataPointValueAndStatus (ff, index, value, dataStatus);
	}
}

void FormantModeler_setParameterValueFixed (FormantModeler me, long iformant, long index, double value) {
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_setParameterValueFixed (ffi, index, value);
	}
}

void FormantModeler_setParametersFree (FormantModeler me, long fromFormant, long toFormant, long fromIndex, long toIndex) {
	long numberOfFormants = my datamodelers -> size;
	if (toFormant < fromFormant || (fromFormant == toFormant && fromFormant == 0)) {
		fromFormant = 1; toFormant= numberOfFormants;
	}
	if (! (toFormant >= 1 && toFormant <= numberOfFormants && fromFormant >= 1 && fromFormant <= numberOfFormants &&
		fromFormant <= toFormant)) {
		Melder_throw (U"Formant number(s) must be in the interval [1, ", numberOfFormants, U"].");
	}
	for (long iformant = fromFormant; iformant <= toFormant; iformant++) {
		DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_setParametersFree (ffi, fromIndex, toIndex);
	}
}

void FormantModeler_setDataWeighing (FormantModeler me, long fromFormant, long toFormant, int useSigmaY) {
	long numberOfFormants = my datamodelers -> size;
	if (toFormant < fromFormant || (fromFormant == toFormant && fromFormant == 0)) {
		fromFormant = 1; toFormant= numberOfFormants;
	}
	if (! (toFormant >= 1 && toFormant <= numberOfFormants && fromFormant >= 1 && fromFormant <= numberOfFormants &&
		fromFormant <= toFormant)) {
		Melder_throw (U"Formant number(s) must be in the interval [1, ", numberOfFormants, U"].");
	}
	for (long iformant = fromFormant; iformant <= toFormant; iformant++) {
		DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_setDataWeighing (ffi, useSigmaY);
	}
}

void FormantModeler_fit (FormantModeler me) {
	for (long iformant = 1; iformant <= my datamodelers -> size; iformant++) {
		DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_fit (ffi);
	}
}

void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax,
 	long iformant, long iterm, bool scaled, long numberOfPoints, int garnish) {
	if (tmax <= tmin) {
		tmin = my xmin; tmax = my xmax; 
	}
	if (iformant < 1 || iformant > my datamodelers -> size) {
		return;
	}
	Graphics_setInner (g);
	DataModeler ffi =  (DataModeler) my datamodelers -> item[iformant];
	DataModeler_drawBasisFunction_inside (ffi, g, tmin, tmax, fmin, fmax, iterm, scaled, numberOfPoints);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_inqWindow (g, &tmin, &tmax, &fmin, &fmax);
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, U"Time (s)");
		Graphics_textLeft (g, 1, (scaled ? U"Frequency (Hz)" : U"Amplitude"));
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_markLeft (g, fmin, 1, 1, 0, U"");
		Graphics_markLeft (g, fmax, 1, 1, 0, U"");
	}
}

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, long fromTrack, long toTrack, double numberOfSigmas, int useSigmaY, char32 *mark, int marksFontSize, double horizontalOffset_mm, int garnish) {
	if (tmax <= tmin) { 
		tmin = my xmin; tmax = my xmax; 
	}
	long maxTrack = my datamodelers -> size;
	if (toTrack == 0 && fromTrack == 0) {
		fromTrack = 1; toTrack = maxTrack;
	}
	if (fromTrack > maxTrack) return;
	if (toTrack > maxTrack) {
		toTrack = maxTrack;
	}
	Graphics_setInner (g);
	int currectFontSize = Graphics_inqFontSize (g);
	for (long iformant = fromTrack; iformant <= toTrack; iformant++) {
		DataModeler ffi =  (DataModeler) my datamodelers -> item[iformant];
		double xOffset_mm = (iformant % 2 == 1) ? horizontalOffset_mm : -horizontalOffset_mm;
		DataModeler_drawOutliersMarked_inside (ffi, g, tmin, tmax, 0, fmax, useSigmaY, numberOfSigmas, mark, marksFontSize, xOffset_mm);
	}
	Graphics_setFontSize (g, currectFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, U"Time (s)");
		Graphics_textLeft (g, 1, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, 1, 1, 1);
	}
}

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, long iformant, int useSigmaY, long numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, int garnish) {
	if (iformant > 0 || iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers ->item[iformant];
		DataModeler_normalProbabilityPlot (ff, g, useSigmaY, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	}
}

void FormantModeler_drawTracks_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax,
	long fromTrack, long toTrack, int estimated, long numberOfParameters, double horizontalOffset_mm) {
	for (long iformant = fromTrack; iformant <= toTrack; iformant++) {
		DataModeler ffi =  (DataModeler) my datamodelers -> item[iformant];
		double xOffset_mm = (iformant % 2 == 1) ? horizontalOffset_mm : -horizontalOffset_mm;
		DataModeler_drawTrack_inside (ffi, g, xmin, xmax, 0, fmax, estimated, numberOfParameters, xOffset_mm);
	}
}

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	long fromTrack, long toTrack, int estimated, long numberOfParameters, double horizontalOffset_mm, int garnish) {
	if (tmax <= tmin) { 
		tmin = my xmin; tmax = my xmax; 
	}
	long maxTrack = my datamodelers -> size;
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
		Graphics_textBottom (g, 1, U"Time (s)");
		Graphics_textLeft (g, 1, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, 1, 1, 1);
	}
}

void FormantModeler_speckle_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax,
	long fromTrack, long toTrack, int estimated, long numberOfParameters, int errorBars, double barWidth_mm, double horizontalOffset_mm) {
	for (long iformant = fromTrack; iformant <= toTrack; iformant++) {
		DataModeler ffi =  (DataModeler) my datamodelers -> item[iformant];
		double xOffset_mm = (iformant % 2 == 1) ? horizontalOffset_mm : -horizontalOffset_mm;
		DataModeler_speckle_inside (ffi, g, xmin, xmax, 0, fmax, estimated, numberOfParameters, errorBars, barWidth_mm, xOffset_mm);
	}
}

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	long fromTrack, long toTrack, int estimated, long numberOfParameters, int errorBars, double barWidth_mm, double horizontalOffset_mm, int garnish) {
	if (tmax <= tmin) { 
		tmin = my xmin; tmax = my xmax; 
	}
	long maxTrack = my datamodelers -> size;
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
		Graphics_textBottom (g, 1, U"Time (s)");
		Graphics_textLeft (g, 1, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, 1, 1, 1);
	}
}

FormantModeler FormantModeler_create (double tmin, double tmax, long numberOfFormants, long numberOfDataPoints, long numberOfParameters) {
	try {
		autoFormantModeler me = Thing_new (FormantModeler);
		my xmin = tmin; my xmax = tmax;
		my datamodelers = Ordered_create ();
		for (long itrack = 1; itrack <= numberOfFormants; itrack++) {
			autoDataModeler ff = DataModeler_create (tmin, tmax, numberOfDataPoints, numberOfParameters,  DataModeler_TYPE_LEGENDRE);
			Collection_addItem (my datamodelers, ff.transfer());
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"FormantModeler not created.");
	}
}

double FormantModeler_getModelValueAtTime (FormantModeler me, long iformant, double time) {
	double f = NUMundefined;
	if (iformant >= 1 && iformant <= my datamodelers -> size) {
		DataModeler thee =  (DataModeler) my datamodelers -> item[iformant];
		f =  DataModeler_getModelValueAtX (thee, time);
	}
	return f;
}

double FormantModeler_getWeightedMean (FormantModeler me, long iformant) {
	double f = NUMundefined;
	if (iformant >= 1 && iformant <= my datamodelers -> size) {
		DataModeler thee =  (DataModeler) my datamodelers -> item[iformant];
		f =  DataModeler_getWeightedMean (thee);
	}
	return f;
	
}

long FormantModeler_getMaximumNumberOfParameters (FormantModeler me) {
	long maxnum = 1;
	for (long i = 1; i <= my datamodelers -> size; i++) {
		DataModeler ffi = (DataModeler) my datamodelers -> item[i];
		if (ffi -> numberOfParameters > maxnum) {
			maxnum = ffi -> numberOfParameters;
		}
	}
	return maxnum;
}

long FormantModeler_getNumberOfTracks (FormantModeler me) {
	return my datamodelers -> size;
}

long FormantModeler_getNumberOfParameters (FormantModeler me, long iformant) {
	long numberOfParameters = 0;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		numberOfParameters = ff -> numberOfParameters;
	}
	return numberOfParameters;
}

long FormantModeler_getNumberOfFixedParameters (FormantModeler me, long iformant) {
	long numberOfParameters = 0;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		numberOfParameters = ff -> numberOfParameters;
		numberOfParameters -= DataModeler_getNumberOfFreeParameters (ff);
	}
	return numberOfParameters;
}

long FormantModeler_getNumberOfDataPoints (FormantModeler me, long iformant) {
	long numberOfDataPoints = 0;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		numberOfDataPoints = ff -> numberOfDataPoints;
	}
	return numberOfDataPoints;
}


long FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, long iformant) {
	long numberOfInvalidDataPoints = 0;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		numberOfInvalidDataPoints = DataModeler_getNumberOfInvalidDataPoints (ff);
	}
	return numberOfInvalidDataPoints;
}

double FormantModeler_getParameterValue (FormantModeler me, long iformant, long iparameter) {
	double value = NUMundefined;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		value = DataModeler_getParameterValue (ff, iparameter);
	}
	return value;
}

int FormantModeler_getParameterStatus (FormantModeler me, long iformant, long index) {
	int status = DataModeler_PARAMETER_UNDEFINED;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		status = DataModeler_getParameterStatus (ff, index);
	}
	return status;
}

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, long iformant, long index) {
	double stdev = NUMundefined;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		stdev = DataModeler_getParameterStandardDeviation (ff, index);
	}
	return stdev;
}

double FormantModeler_getDegreesOfFreedom (FormantModeler me, long iformant) {
	double dof = 0;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		dof = DataModeler_getDegreesOfFreedom (ff);
	}
	return dof;
}

double FormantModeler_getVarianceOfParameters (FormantModeler me, long fromFormant, long toFormant, long fromIndex, long toIndex, long *numberOfFreeParameters) {
	double variance = NUMundefined;
	long numberOfFormants = my datamodelers -> size, numberOfParameters = 0, nofp;
	if (toFormant < fromFormant || (toFormant == 0 && fromFormant == 0)) {
		fromFormant = 1; toFormant = numberOfFormants;
	}
	if (fromFormant <= toFormant && fromFormant > 0 && toFormant <= numberOfFormants) {
		variance = 0;
		for (long iformant = fromFormant; iformant <= toFormant; iformant++) {
			DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
			variance += DataModeler_getVarianceOfParameters (ff, fromIndex, toIndex, &nofp);
			numberOfParameters += nofp;
		}
	}
	if (numberOfFreeParameters != NULL) {
		*numberOfFreeParameters = numberOfParameters;
	}
	return variance;
}

long FormantModeler_getNumberOfDataPoints (FormantModeler me) {
	DataModeler thee = (DataModeler) my datamodelers -> item[1];
	return thy numberOfDataPoints;
}

Table FormantModeler_to_Table_zscores (FormantModeler me, int useSigmaY) {
	try {
		long icolt = 1, numberOfFormants = my datamodelers -> size;
		long numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoTable ztable = Table_createWithoutColumnNames (numberOfDataPoints, numberOfFormants + 1);
		Table_setColumnLabel (ztable.peek(), icolt, U"time");
		for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
			long icolz = iformant + 1;
			Table_setColumnLabel (ztable.peek(), icolz, Melder_cat (U"z", iformant));
			DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
			if (iformant == 1) {
				for (long i = 1; i <= numberOfDataPoints; i++) { // only once all tracks have same x-values
				Table_setNumericValue (ztable.peek(), i, icolt, ffi -> x[i]);
				}
			}
			autoNUMvector<double> zscores (DataModeler_getZScores (ffi, useSigmaY), 1);
			for (long i = 1; i <= numberOfDataPoints; i++) {
				Table_setNumericValue (ztable.peek(), i, icolz, zscores[i]);
			}
		}
		return ztable.transfer();
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

DataModeler FormantModeler_extractDataModeler (FormantModeler me, long iformant) {
	try {
		if (! (iformant > 0 && iformant <= my datamodelers -> size)) {
			Melder_throw (U"");
		}
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		autoDataModeler thee = (DataModeler) Data_copy (ff);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}	
}

Covariance FormantModeler_to_Covariance_parameters (FormantModeler me, long iformant) {
	try {
		if (iformant < 1 || iformant > my datamodelers -> size) {
			Melder_throw (U"The formant should be greater than zero and smaller than or equal to ", 
				  my datamodelers -> size);
		}
		DataModeler thee = (DataModeler) my datamodelers -> item[iformant];
		autoCovariance cov = (Covariance) Data_copy (thy parameterCovariances);
		return cov.transfer();
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
	
}

void FormantModeler_setTolerance (FormantModeler me, double tolerance) {
	for (long iformant = 1; iformant <= my datamodelers -> size; iformant++) {
		DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
		DataModeler_setTolerance (ffi, tolerance);
	}
}

FormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, long numberOfFormants, long numberOfParametersPerTrack, int bandwidthEstimatesSigma) {
	try {
		long ifmin, ifmax, posInCollection = 0;
		if (tmax <= tmin) {
			tmin = my xmin; tmax = my xmax;
		}
		long numberOfDataPoints = Sampled_getWindowSamples (me, tmin, tmax, &ifmin, &ifmax);
		if (numberOfDataPoints < numberOfParametersPerTrack) {
			Melder_throw (U"Not enought data points, extend the selection.");
		}
		autoFormantModeler thee = FormantModeler_create (tmin, tmax, numberOfFormants, numberOfDataPoints, numberOfParametersPerTrack);
		for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
			posInCollection++;
			DataModeler ffi = (DataModeler) thy datamodelers -> item[posInCollection];
			long idata = 0, validData = 0;
			for (long iframe = ifmin; iframe <= ifmax; iframe++) {
				Formant_Frame curFrame = & my d_frames[iframe];
				ffi -> x[++idata] = Sampled_indexToX (me, iframe);
				ffi -> dataPointStatus[idata] = DataModeler_DATA_INVALID;
				if (iformant <= curFrame -> nFormants) {
					double frequency = curFrame -> formant[iformant].frequency;
					if (NUMdefined (frequency)) {
						double bw = curFrame -> formant[iformant].bandwidth;
						ffi -> y[idata] = curFrame -> formant[iformant].frequency;
						ffi -> sigmaY[idata] = bw;
						ffi -> dataPointStatus[idata] = DataModeler_DATA_VALID;
						validData++;
					}
				}
			}
			ffi -> useSigmaY = bandwidthEstimatesSigma;
			ffi -> numberOfDataPoints = idata;
			ffi -> tolerance = 1e-5;
			if (validData < numberOfParametersPerTrack) { // remove don't throw exception
				Collection_removeItem (thy datamodelers, posInCollection);
				posInCollection--;
			}
		}
		if (posInCollection == 0) {
			Melder_throw (U"Not enought data points in all the formants!");
		}
		FormantModeler_fit (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"FormantModeler not created.");
	}
}

Formant FormantModeler_to_Formant (FormantModeler me, int useEstimates, int estimateUndefineds) {
	try {
		long numberOfFormants = my datamodelers -> size;
		DataModeler ff = (DataModeler) my datamodelers -> item[1];
		long numberOfFrames = ff -> numberOfDataPoints;
		double t1 = ff -> x[1], dt = ff -> x[2] -ff -> x[1];
		autoFormant thee = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
		autoNUMvector<double> sigma (1, numberOfFormants);
		if (useEstimates || estimateUndefineds) {
			for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
				sigma[iformant] = FormantModeler_getStandardDeviation (me, iformant);
			}
		}
		for (long iframe = 1; iframe <= numberOfFrames; iframe++) {
			Formant_Frame thyFrame = & thy d_frames [iframe];
			thyFrame -> intensity = 1; //???
			thyFrame -> formant = NUMvector <structFormant_Formant> (1, numberOfFormants);
			
			for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
				DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
				double f = NUMundefined, b = f;
				if (ffi -> dataPointStatus[iframe] != DataModeler_DATA_INVALID) {
					f = useEstimates ? DataModeler_getModelValueAtX (ffi, ffi -> x[iframe]) : ffi -> y[iframe];
					b = ff -> sigmaY[iframe]; // copy original value
				} else {
					if (estimateUndefineds) {
						f = FormantModeler_getModelValueAtTime (me, iformant, ffi -> x[iframe]);
						b = sigma[iformant];
					}
				}
				thyFrame -> formant[iformant].frequency = f;
				thyFrame -> formant[iformant].bandwidth = b;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"Cannot create Formant from FormantModeler.");
	}
}

double FormantModeler_getChiSquaredQ (FormantModeler me, long fromFormant, long toFormant, int useSigmaY, double *probability, double *ndf) {
	double chisq = NUMundefined, ndfTotal = 0;
	if (fromFormant == 0 && toFormant == 0) {
		fromFormant = 1; toFormant = my datamodelers -> size;
	}
	if (fromFormant >= 1 && toFormant <= my datamodelers -> size) {
		chisq = 0;
		long numberOfDefined = 0;
		for (long iformant= fromFormant; iformant <= toFormant; iformant++) {
			DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
			double p, df, chisqi = DataModeler_getChiSquaredQ (ffi, useSigmaY, &p, &df);
			if (NUMdefined (chisqi)) {
				chisq += df * chisqi;
				ndfTotal += df; numberOfDefined++;
			}
		}
		if (numberOfDefined > 0) {
			chisq *= numberOfDefined / ndfTotal;
			if (ndf != NULL) { *ndf = ndfTotal; }
			if (probability != NULL) { *probability = NUMchiSquareQ (chisq, ndfTotal); }
		}
	}
	return chisq;
}

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, long fromFormant, long toFormant) {
	double rSquared = NUMundefined;
	if (fromFormant == 0 && toFormant == 0) {
		fromFormant = 1; toFormant = my datamodelers -> size;
	}
	if (fromFormant >= 1 && toFormant <= my datamodelers -> size) {
		double ssreg = 0, sstot = 0, ssregi, sstoti;
		for (long iformant= fromFormant; iformant <= toFormant; iformant++) {
			DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
			DataModeler_getCoefficientOfDetermination (ffi, &ssregi, &sstoti);
			sstot += sstoti; ssreg += ssregi;
		}
		rSquared = sstot > 0 ? ssreg / sstot : 1;
	}
	return rSquared;
}

double FormantModeler_getResidualSumOfSquares (FormantModeler me, long iformant, long *numberOfDataPoints) {
	double rss = NUMundefined;
	if (iformant > 0 && iformant <= my datamodelers -> size) {
		DataModeler ff = (DataModeler) my datamodelers -> item[iformant];
		rss = DataModeler_getResidualSumOfSquares (ff, numberOfDataPoints);
	}
	return rss;
}

void FormantModeler_setParameterValuesToZero (FormantModeler me, long fromFormant, long toFormant, double numberOfSigmas) {
	if (fromFormant == 0 && toFormant == 0) {
		fromFormant = 1; toFormant = my datamodelers -> size;
	}
	if (fromFormant >= 1 && toFormant <= my datamodelers -> size) {
		for (long iformant= fromFormant; iformant <= toFormant; iformant++) {
			DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
			DataModeler_setParameterValuesToZero (ffi, numberOfSigmas);
		}
	}
}

FormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas, int useSigmaY) {
	try {
		long numberOfFormants = my datamodelers -> size;
		if (numberOfFormants < 3) {
			Melder_throw (U"We need at least three formants to process outliers.");
		}
		long numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoNUMvector<double> x (1, numberOfDataPoints); // also store x-values
		autoNUMmatrix<double> z (1, numberOfFormants, 1, numberOfDataPoints);
		// maybe some of the formants had NUMundefind's.

		// 1. calculate z-scores for each formant and sort them in descending order
		DataModeler ff = (DataModeler) my datamodelers -> item[1];
		NUMvector_copyElements<double> (ff -> x, x.peek(), 1, numberOfDataPoints);
		for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
			DataModeler ffi = (DataModeler) my datamodelers -> item[iformant];
			autoNUMvector<double> zscores (DataModeler_getZScores (ffi, useSigmaY), 1);
			NUMvector_copyElements<double> (zscores.peek(), z[iformant], 1, numberOfDataPoints);
		}
		// 2. Do the manipulation in a copy
		autoFormantModeler thee = (FormantModeler) Data_copy (me);
		for (long i = 1; i <= numberOfDataPoints; i++) {
			// First the easy one: first formant missing: F1' = F2; F2' = F3
			if (NUMdefined (z[1][i]) && NUMdefined (z[1][i]) && NUMdefined (z[3][i])) {
				if (z[1][i] > numberOfSigmas && z[2][i] > numberOfSigmas && z[3][i] > numberOfSigmas) {
					// all deviations have the same sign:
					// probably F1 is missing
					// try if f2 <- F1 and f3 <- F2 reduces chisq
					double f2 = FormantModeler_getDataPointValue (me, 1, i); // F1
					double f3 = FormantModeler_getDataPointValue (me, 2, i); // F2
					FormantModeler_setDataPointStatus (thee.peek(), 1, i, DataModeler_DATA_INVALID);
					FormantModeler_setDataPointValueAndStatus (thee.peek(), 2, i, f2, FormantModeler_DATA_FROM_LOWER);
					FormantModeler_setDataPointValueAndStatus (thee.peek(), 3, i, f3, FormantModeler_DATA_FROM_LOWER);
				}
			}
		}
		FormantModeler_fit (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"Cannot calculate track discontinuities");
	}
}

double FormantModeler_getSmoothnessValue (FormantModeler me, long fromFormant, long toFormant, long numberOfParametersPerTrack, double power) {
	double smoothness = NUMundefined;
	if (toFormant >= fromFormant) {
		fromFormant = 1; toFormant = my datamodelers -> size;
	}
	if (fromFormant > 0 && fromFormant <= toFormant && toFormant <= my datamodelers -> size) {
		long nofp;
		double ndof, var = FormantModeler_getVarianceOfParameters (me, fromFormant, toFormant, 1, numberOfParametersPerTrack, &nofp);
		double chisq = FormantModeler_getChiSquaredQ (me, fromFormant, toFormant, TRUE, NULL, &ndof);
		if (NUMdefined (var) && NUMdefined (chisq) && nofp > 0) {
			smoothness = log10 (pow (var / nofp, power) * (chisq / ndof));
		}
	}
	return smoothness;
}

double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, long track1, long track2, int type) {
	double diff = NUMundefined;
	if (track1 == track2) {
		return 0;
	}
	if (track1 <= my datamodelers -> size && track2 <= my datamodelers -> size) {
		DataModeler fi = (DataModeler) my datamodelers -> item[track1];
		DataModeler fj = (DataModeler) my datamodelers -> item[track2];
		// fi and fj have equal number of data points
		long numberOfDataPoints = 0; diff = 0;
		for (long i = 1; i <= fi -> numberOfDataPoints; i++) {
			if (type != 0) {
				double fie = fi -> f_evaluate (fi, fi -> x[i], fi -> parameter);
				double fje = fj -> f_evaluate (fj, fj -> x[i], fj -> parameter);
				diff += fabs (fie - fje);
				numberOfDataPoints++;
			} else if (fi -> dataPointStatus[i] != DataModeler_DATA_INVALID && fj -> dataPointStatus[i] != DataModeler_DATA_INVALID) {
				diff += fabs (fi -> y[i] - fj -> y[i]);
				numberOfDataPoints++;
			}
		}
		diff /= numberOfDataPoints;
	}
	return diff;
}

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3) {
	double f1 = FormantModeler_getParameterValue (me, 1, 1); // datamodelers -> item[1] -> parameter[1]
	double minF1Factor = f1 > minF1 ? 1 : sqrt (minF1 - f1 + 1);
	double maxF1Factor = f1 < maxF1 ? 1 : sqrt (f1 - maxF1 + 1);
	double f2 = FormantModeler_getParameterValue (me, 2, 1); // datamodelers -> item[2] -> parameter[1]
	double minF2Factor = f2 > minF2 ? 1 : sqrt (minF2 - f2 + 1);
	double maxF2Factor = f2 < maxF2 ? 1 : sqrt (f2 - maxF2 + 1);
	double f3 = FormantModeler_getParameterValue (me, 3, 1); // datamodelers -> item[3] -> parameter[1]
	double minF3Factor = f3 > minF3 ? 1 : sqrt (minF3 - f3 + 1);
	return minF1Factor * maxF1Factor * minF2Factor * maxF2Factor * minF3Factor;
}

long Formants_getSmoothestInInterval (Collection me, double tmin, double tmax, long numberOfFormantTracks, long numberOfParametersPerTrack,
	int useBandWidthsForTrackEstimation, int useConstraints, double numberOfSigmas, double power,
	double minF1, double maxF1, double minF2, double maxF2, double minF3)
{
	try {
		long numberOfFormantObjects = my size, minNumberOfFormants = 1000000;
		if (numberOfFormantObjects == 1) {
			return 1;
		}
		autoNUMvector<long> numberOfFormants (1, numberOfFormantObjects);
		autoNUMvector<int> invalid (1, numberOfFormantObjects);
		double tminf = 0, tmaxf = 0;
		for (long iobject = 1; iobject <= numberOfFormantObjects; iobject++) {
			// Check that all Formants have the same domain
			Formant fi = (Formant) my item[iobject];
			if (tminf == tmaxf) {
				tminf = fi -> xmin; tmaxf = fi -> xmax;
			} else if (fi -> xmin != tminf || fi -> xmax != tmaxf) {
				Melder_throw (U"All Formant objects must have the same starting and finishing times.");
			}
			// Find the one that has least formant tracks
			numberOfFormants[iobject] = Formant_getMaxNumFormants (fi);
			if (numberOfFormants[iobject] < minNumberOfFormants) {
				minNumberOfFormants = numberOfFormants[iobject];
			}
		}
		if (numberOfFormantTracks == 0) { // default
			numberOfFormantTracks = minNumberOfFormants;
		}
		if (numberOfFormantTracks > minNumberOfFormants) {
			// make formants with not enough tracks invalid for the competition
			long numberOfInvalids = 0;
			for (long iobject = 1; iobject <= numberOfFormantObjects; iobject++) {
				if (numberOfFormants[iobject] < numberOfFormantTracks) {
					invalid[iobject] = 1;
					numberOfInvalids++;
				}
			}
			if (numberOfInvalids == numberOfFormantObjects) {
				Melder_throw (U"None of the Formants has enough formant tracks. Lower your upper formant number.");
			}
		}
		if (tmax <= tmin) { // default
			tmin = tminf; tmax = tmaxf;
		}
		if (! (tmin >= tminf && tmax <= tmaxf)) {
			Melder_throw (U"The selected interval needs to be within the Formant object's domain.");
		}
		/* The chisq is not meaningfull as a the only test whether one model is better than the other because if we have two models 
		 * 1 & 2 with the same data points (x1[i]=x2[i] and y1[i]= y2[i] but if sigma1[i] < sigma2[i] than chisq1 > chisq2.
		 * This is not what we want.
		 * We test therefore the variances of the parameters because if sigma1[i] < sigma2[i] than pvar1 < pvar2.
		 */
		double minChiVar = 1e38;
		long index = 0;
		for (long iobject = 1; iobject <= numberOfFormantObjects; iobject++) {
			if (invalid[iobject] != 1) {
				Formant fi = (Formant) my item[iobject];
				autoFormantModeler fs = Formant_to_FormantModeler (fi, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack, useBandWidthsForTrackEstimation);
				FormantModeler_setParameterValuesToZero (fs.peek(), 1, numberOfFormantTracks, numberOfSigmas);
				double cf = useConstraints ? FormantModeler_getFormantsConstraintsFactor (fs.peek(), minF1, maxF1, minF2, maxF2, minF3) : 1;
				double chiVar = FormantModeler_getSmoothnessValue (fs.peek(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
				if (NUMdefined (chiVar) && cf * chiVar < minChiVar) {
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

Formant Formant_extractPart (Formant me, double tmin, double tmax) {
	try {
		if (tmin >= tmax) {
			tmin = my xmin; tmax = my xmax;
		}
		if (tmin >= my xmax || tmax <= my xmin) {
			Melder_throw (U"Your start and end time should be between ", my xmin, U" and ", my xmax, U".");
		}
		long thyindex = 1, ifmin, ifmax;
		long numberOfFrames = Sampled_getWindowSamples (me, tmin, tmax, &ifmin, &ifmax);
		double t1 = Sampled_indexToX (me, ifmin);
		autoFormant thee = Formant_create (tmin, tmax, numberOfFrames, my dx, t1, my maxnFormants);
		for (long iframe = ifmin; iframe <= ifmax; iframe++, thyindex++) {
			Formant_Frame myFrame = & my d_frames [iframe];
			Formant_Frame thyFrame = & thy d_frames [thyindex];
			myFrame -> copy (thyFrame);
		}
		return thee.transfer();
		
	} catch (MelderError) {
		Melder_throw (U"Formant part could not be extracted.");
	}
}

Formant Formants_extractSmoothestPart (Collection me, double tmin, double tmax, long numberOfFormantTracks, long numberOfParametersPerTrack,
	int useBandWidthsForTrackEstimation, double numberOfSigmas, double power)
{
	try {
		long index = Formants_getSmoothestInInterval (me, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,
			useBandWidthsForTrackEstimation, 0, numberOfSigmas, power, 1, 1, 1, 1, 1); // last four are just fillers
		Formant bestfit = (Formant) my item[index];
		autoFormant thee = Formant_extractPart (bestfit, tmin, tmax);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"Smoothest Formant part could not be extracted.");
	}
}


Formant Formants_extractSmoothestPart_withFormantsConstraints (Collection me, double tmin, double tmax, long numberOfFormantTracks, long numberOfParametersPerTrack,
	int useBandWidthsForTrackEstimation, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3)
{
	try {
		long index = Formants_getSmoothestInInterval (me, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,
			useBandWidthsForTrackEstimation, 1, numberOfSigmas, power, minF1, maxF1, minF2, maxF2, minF3);
		Formant bestfit = (Formant) my item[index];
		autoFormant thee = Formant_extractPart (bestfit, tmin, tmax);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"Smoothest Formant part could not be extracted.");
	}
}

Thing_implement (PitchModeler, DataModeler, 0);

PitchModeler Pitch_to_PitchModeler (Pitch me, double tmin, double tmax, long numberOfParameters) {
	try {
		long ifmin, ifmax;
		if (tmax <= tmin) {
			tmin = my xmin; tmax = my xmax;
		}
		long numberOfDataPoints = Sampled_getWindowSamples (me, tmin, tmax, &ifmin, &ifmax);
		if (numberOfDataPoints < numberOfParameters) {
			Melder_throw (U"Not enough data points, extend the selection.");
		}
		autoPitchModeler thee = Thing_new (PitchModeler);
		DataModeler_init (thee.peek(), tmin, tmax, numberOfDataPoints, numberOfParameters, DataModeler_TYPE_LEGENDRE);
		long idata = 0, validData = 0;
		for (long iframe = ifmin; iframe <= ifmax; iframe++) {
			thy x[++idata] = Sampled_indexToX (me, iframe);
			thy dataPointStatus[idata] = DataModeler_DATA_INVALID;
			if (Pitch_isVoiced_i (me, iframe)) {
				thy y[idata] = my frame [iframe]. candidate [1]. frequency;
				thy dataPointStatus[idata] = DataModeler_DATA_VALID;
				validData++;
			}
		}
		thy numberOfDataPoints = idata;
		if (validData < numberOfParameters) { // remove don't throw exception
			Melder_throw (U"Not enough valid data in interval.");
		}
		DataModeler_fit (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"No PitchModeler could be created.");
	}
}

void PitchModeler_draw (PitchModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax, long numberOfParameters, int garnish) {
	Graphics_setInner (g);
	DataModeler_drawTrack_inside (me, g, tmin, tmax, fmin, fmax, 1, numberOfParameters, 0);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, U"Time (s)");
		Graphics_textLeft (g, 1, U"Frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1.0, 100.0, 1, 1, 1);
	}
}

double Sound_getOptimalFormantCeiling (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, long numberOfFrequencySteps, double preemphasisFrequency, long numberOfFormantTracks, long numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power) {
	double optimalCeiling;
	autoFormant thee = Sound_to_Formant_interval (me, startTime, endTime, windowLength, timeStep, minFreq, maxFreq,  numberOfFrequencySteps, preemphasisFrequency, numberOfFormantTracks, numberOfParametersPerTrack, weighData,  numberOfSigmas, power, false, 0, 5000, 0, 5000, 0, &optimalCeiling);
	return optimalCeiling;
}

Formant Sound_to_Formant_interval (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, long numberOfFrequencySteps, double preemphasisFrequency, long numberOfFormantTracks, long numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *optimalCeiling) {
	try {
		// parameter check
		if (endTime <= startTime) {
			startTime = my xmin; endTime = my xmax;
		}
		double nyquistFrequency = 0.5 / my dx;
		if (maxFreq > nyquistFrequency) {
			Melder_throw (U"The upper value of the maximum frequency range is higher than the Nyquist frequency of the sound.");
		}
		double df = 0, ceiling_best, mincriterium = 1e28;
		if (minFreq >= maxFreq) {
			numberOfFrequencySteps = 1;
		} else {
			df = (maxFreq - minFreq) / (numberOfFrequencySteps - 1);
		}
		long i_best = 0;
		
		// extract part +- windowLength because of Gaussian windowing in the formant analysis
		// +timeStep/2 to have the analysis points maximally spread in the new domain.
		
		autoSound part = Sound_extractPart (me, startTime - windowLength + timeStep / 2, endTime + windowLength + timeStep / 2, kSound_windowShape_RECTANGULAR, 1, 1);

		// Resample to 2*maxFreq to reduce resampling load in Sound_to_Formant
		
		autoSound resampled = Sound_resample (part.peek(), 2 * maxFreq, 50);
		autoOrdered formants = Ordered_create ();
		Melder_progressOff ();
		for (long i = 1; i <= numberOfFrequencySteps; i++) {
			double currentCeiling = minFreq + (i - 1) * df;
			autoFormant formant = Sound_to_Formant_burg (resampled.peek(), timeStep, 5, currentCeiling, windowLength, preemphasisFrequency);
			autoFormantModeler fm = Formant_to_FormantModeler (formant.peek(), startTime, endTime, numberOfFormantTracks, numberOfParametersPerTrack, weighData);
			FormantModeler_setParameterValuesToZero (fm.peek(), 1, numberOfFormantTracks, numberOfSigmas);
			Collection_addItem (formants.peek(), formant.transfer());
			double cf = useConstraints ? FormantModeler_getFormantsConstraintsFactor (fm.peek(), minF1, maxF1, minF2, maxF2, minF3) : 1;
			double chiVar = FormantModeler_getSmoothnessValue (fm.peek(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
			double criterium = chiVar * cf;
			if (NUMdefined (chiVar) && criterium < mincriterium) {
				mincriterium = criterium;
				ceiling_best= currentCeiling;
				i_best = i;
			}
		}
		autoFormant thee = Formant_extractPart ((Formant) formants -> item[i_best], startTime, endTime);
		Melder_progressOn ();
		if (optimalCeiling) {
			*optimalCeiling = ceiling_best;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"No Formant object created.");
	}
}

Formant Sound_to_Formant_interval_robust (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, long numberOfFrequencySteps, double preemphasisFrequency, long numberOfFormantTracks, long numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *optimalCeiling) {
	try {
		// parameter check
		if (endTime <= startTime) {
			startTime = my xmin; endTime = my xmax;
		}
		double nyquistFrequency = 0.5 / my dx;
		if (maxFreq > nyquistFrequency) {
			Melder_throw (U"The upper value of the maximum frequency range is higher than the Nyquist frequency of the sound.");
		}
		double df = 0, ceiling_best, mincriterium = 1e28;
		if (minFreq >= maxFreq) {
			numberOfFrequencySteps = 1;
		} else {
			df = (maxFreq - minFreq) / (numberOfFrequencySteps - 1);
		}
		long i_best = 0;
		
		// extract part +- windowLength because of Gaussian windowing in the formant analysis
		// +timeStep/2 to have the analysis points maximally spread in the new domain.
		
		autoSound part = Sound_extractPart (me, startTime - windowLength + timeStep / 2, endTime + windowLength + timeStep / 2, kSound_windowShape_RECTANGULAR, 1, 1);

		// Resample to 2*maxFreq to reduce resampling load in Sound_to_Formant
		
		autoSound resampled = Sound_resample (part.peek(), 2 * maxFreq, 50);
		autoOrdered formants = Ordered_create ();
		Melder_progressOff ();
		for (long i = 1; i <= numberOfFrequencySteps; i++) {
			double currentCeiling = minFreq + (i - 1) * df;
			autoFormant formant = Sound_to_Formant_robust (resampled.peek(), timeStep, 5, currentCeiling, windowLength, preemphasisFrequency, 50, 1.5, 3, 0.0000001, 1);
			autoFormantModeler fm = Formant_to_FormantModeler (formant.peek(), startTime, endTime, numberOfFormantTracks, numberOfParametersPerTrack, weighData);
			FormantModeler_setParameterValuesToZero (fm.peek(), 1, numberOfFormantTracks, numberOfSigmas);
			Collection_addItem (formants.peek(), formant.transfer());
			double cf = useConstraints ? FormantModeler_getFormantsConstraintsFactor (fm.peek(), minF1, maxF1, minF2, maxF2, minF3) : 1;
			double chiVar = FormantModeler_getSmoothnessValue (fm.peek(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
			double criterium = chiVar * cf;
			if (NUMdefined (chiVar) && criterium < mincriterium) {
				mincriterium = criterium;
				ceiling_best= currentCeiling;
				i_best = i;
			}
		}
		autoFormant thee = Formant_extractPart ((Formant) formants -> item[i_best], startTime, endTime);
		Melder_progressOn ();
		if (optimalCeiling) {
			*optimalCeiling = ceiling_best;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"No Formant object created.");
	}
}

/* End of file DataModeler.cpp */
