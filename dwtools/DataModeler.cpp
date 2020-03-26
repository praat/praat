/* DataModeler.cpp
 *
 * Copyright (C) 2014-2020 David Weenink, 2017 Paul Boersma
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
 * TODO:
	20200325 Draw variances crashed, 
	info FormantModeler:Datamodeler crahed
	DataModeler_getDataPointInverseWeight is inconsistent

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

#include "enums_getText.h"
#include "DataModeler_enums.h"
#include "enums_getValue.h"
#include "DataModeler_enums.h"

extern machar_Table NUMfpp;

Thing_implement (DataModeler, Function, 1);

void structDataModeler :: v_info () {
	MelderInfo_writeLine (U"   Time domain:");
	MelderInfo_writeLine (U"      Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"      End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"      Total duration: ", xmax - xmin, U" seconds");
	double ndf, rSquared = DataModeler_getCoefficientOfDetermination (this, nullptr, nullptr);
	double probability, chisq = DataModeler_getChiSquaredQ (this, weighData, & probability, & ndf);
	MelderInfo_writeLine (U"   Fit:");
	MelderInfo_writeLine (U"      Number of data points: ", numberOfDataPoints);
	MelderInfo_writeLine (U"      Number of parameters: ", numberOfParameters);
	MelderInfo_writeLine (U"      Each data point has ",  (weighData == kDataModelerWeights::EqualWeights ? U" the same weight (estimated)." :
		( weighData == kDataModelerWeights::OneOverSigma ? U"a different weight (sigmaY)." : 
		( weighData == kDataModelerWeights::Relative ? U"a different relative weight (Y_value/sigmaY)." :
		U"a different weight (SQRT(sigmaY))." ) ) ));
	MelderInfo_writeLine (U"      Chi squared: ", chisq);
	MelderInfo_writeLine (U"      Number of degrees of freedom: ", ndf);
	MelderInfo_writeLine (U"      Probability: ", probability);
	MelderInfo_writeLine (U"      R-squared: ", rSquared);
	for (integer ipar = 1; ipar <= numberOfParameters; ipar ++) {
		double sigma = ( parameters [ipar] .status == kDataModelerParameter::Fixed ? 0 : sqrt (parameterCovariances -> data [ipar] [ipar]) );
		MelderInfo_writeLine (U"      p [", ipar, U"] = ", parameters [ipar] .value, U"; sigma = ", sigma);
	}
}

static double polynomial_evaluate (DataModeler me, double xin, vector<structDataModelerParameter> p) {
	Melder_assert (p.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const double x = (2.0 * xin - my xmin - my xmax) / 2.0;
	double xpi = 1.0, result = p [1] .value;
	for (integer i = 2; i <= my numberOfParameters; i ++) {
		xpi *= x;
		result += p [i] .value * xpi;
	}
	return result;
}

static void polynomial_evaluateBasisFunctions (DataModeler me, double xin, VEC term) {
	Melder_assert (term.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const double x = (2.0 * xin - my xmin - my xmax) / 2.0;
	term [1] = 1.0;
	for (integer i = 2; i <= my numberOfParameters; i ++)
		term [i] = term [i - 1] * x;
}

static double legendre_evaluate (DataModeler me, double xin, vector<structDataModelerParameter> p) {
	Melder_assert (p.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-1, 1]
	*/
	const double x = (2.0 * xin - my xmin - my xmax) / (my xmax - my xmin);
	double pti, ptim1, ptim2 = 1.0, result = p [1] .value;
	if (my numberOfParameters > 1) {
		const double twox = 2.0 * x;
		double f2 = x, d = 1.0;
		result += p [2] .value * (ptim1 = x);
		for (integer i = 3; i <= my numberOfParameters; i ++) {
			const double f1 = d ++;
			f2 += twox;
			result += p [i] .value * (pti = (f2 * ptim1 - f1 * ptim2) / d);
			ptim2 = ptim1;
			ptim1 = pti;
		}
	}
	return result;
}

static void legendre_evaluateBasisFunctions (DataModeler me, double xin, VEC term) {
	Melder_assert (term.size == my numberOfParameters);
	term [1] = 1.0;
	/*
		transform x from domain [xmin, xmax] to domain [-1, 1]
	*/
	const double x = (2.0 * xin - my xmin - my xmax) / (my xmax - my xmin);
	if (my numberOfParameters > 1) {
		const double twox = 2.0 * x;
		double f2 = term [2] = x, d = 1.0;
		for (integer i = 3; i <= my numberOfParameters; i ++) {
			const double f1 = d ++;
			f2 += twox;
			term [i] = (f2 * term [i-1] - f1 * term [i-2]) / d;
		}
	}
}

static void chisqFromZScores (VEC zscores, double *out_chisq, integer *out_numberOfValidZScores) {
	integer numberOfValidZScores = zscores.size;
	double chisq = 0.0;
	for (integer i = 1; i <= zscores.size; i ++) {
		if (isdefined (zscores [i]))
			chisq += zscores [i] * zscores [i];
		else 
			numberOfValidZScores --;
	}
	if (out_chisq)
		*out_chisq = chisq;
	if (out_numberOfValidZScores)
		*out_numberOfValidZScores = numberOfValidZScores;
}

static double DataModeler_getDataPointInverseWeight (DataModeler me, integer iPoint, kDataModelerWeights weighData ) {
	double inverseWeight = 1.0;
	if (iPoint > 0 && iPoint <= my numberOfDataPoints && my data [iPoint] .status != kDataModelerData::Invalid) {
		if (weighData == kDataModelerWeights::OneOverSigma) {
			inverseWeight = my data [iPoint] .sigmaY;
		} else if (weighData == kDataModelerWeights::Relative) {
			double q = my data [iPoint] .y / my data [iPoint] .sigmaY;
			inverseWeight = 500.0 / q; //
		} else if (weighData == kDataModelerWeights::OneOverSqrtSigma) {
			inverseWeight = 7.071 * sqrt (my data [iPoint] .sigmaY); // .bw = 50 gives 50
		}
	}
	return inverseWeight;
}

double DataModeler_getModelValueAtX (DataModeler me, double x) {
	double f = undefined;
	if (x >= my xmin && x <= my xmax)
		f = my f_evaluate (me, x, my parameters.get());
	return f;
}

double DataModeler_getModelValueAtIndex (DataModeler me, integer index) {
	double f = undefined;
	if (index > 0 && index <= my numberOfDataPoints)
		f = my f_evaluate (me, my data [index] .x, my parameters.get());
	return f;
}

void DataModeler_getExtremaY (DataModeler me, double *out_ymin, double *out_ymax) {
	MelderExtremaWithInit extrema;
	for (integer i = 1; i <= my numberOfDataPoints; i++)
		if (my data [i] .status != kDataModelerData::Invalid)
			extrema.update (my data [i] .y);

	if (out_ymin)
		*out_ymin = extrema.min;
	if (out_ymax)
		*out_ymax = extrema.max;
}

double DataModeler_getDataPointYValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfDataPoints && my data [index] .status != kDataModelerData::Invalid)
		value = my data [index] .y;
	return value;
}

double DataModeler_getDataPointXValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfDataPoints && my data [index] .status != kDataModelerData::Invalid)
		value = my data [index] .x;
	return value;
}

void DataModeler_setDataPointYValue (DataModeler me, integer index, double value) {
	if (index > 0 && index <= my numberOfDataPoints)
		my data [index] .y = value;
}

void DataModeler_setDataPointXValue (DataModeler me, integer index, double value) {
	if (index > 0 && index <= my numberOfDataPoints)
		my data [index] .x = value;
}

void DataModeler_setDataPointValues (DataModeler me, integer index, double xvalue, double yvalue) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my data [index] .x = xvalue;
		my data [index] .y = yvalue;
	}
}

void DataModeler_setDataPointYSigma (DataModeler me, integer index, double sigma) {
	if (index > 0 && index <= my numberOfDataPoints)
		my  data [index] .sigmaY = sigma;
}

double DataModeler_getDataPointYSigma (DataModeler me, integer index) {
	double sigma = undefined;
	if (index > 0 && index <= my numberOfDataPoints)
		sigma = my data [index] .sigmaY;
	return sigma;
}

kDataModelerData DataModeler_getDataPointStatus (DataModeler me, integer index) {
	kDataModelerData value = kDataModelerData::Invalid;
	if (index > 0 && index <= my numberOfDataPoints)
		value = my data [index] .status;
	return value;
}

void DataModeler_setDataPointStatus (DataModeler me, integer index, kDataModelerData status) {
	if (index > 0 && index <= my numberOfDataPoints) {
		if (status == kDataModelerData::Valid && isundef (my data [index] .y))
			Melder_throw (U"Your data value is undefined. First set the value and then its status.");
		my data [index] .status = status;
	}
}

void DataModeler_setDataPointValueAndStatus (DataModeler me, integer index, double value, kDataModelerData dataStatus) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my data [index] .y = value;
		my data [index] .status = dataStatus;
	}
}

void DataModeler_setParameterValue (DataModeler me, integer index, double value, kDataModelerParameter status) {
	if (index > 0 && index <= my numberOfParameters) {
		my parameters [index] .value = value;
		my parameters [index] .status = status;
	}
}

void DataModeler_setParameterValueFixed (DataModeler me, integer index, double value) {
	DataModeler_setParameterValue (me, index, value, kDataModelerParameter::Fixed);
}

double DataModeler_getParameterValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfParameters)
		value = my parameters [index] .value;
	return value;
}

kDataModelerParameter DataModeler_getParameterStatus (DataModeler me, integer index) {
	kDataModelerParameter status = kDataModelerParameter::Undefined;
	if (index > 0 && index <= my numberOfParameters)
		status = my parameters [index] .status;
	return status;
}

double DataModeler_getParameterStandardDeviation (DataModeler me, integer index) {
	double stdev = undefined;
	if (index > 0 && index <= my numberOfParameters)
		stdev = sqrt (my parameterCovariances -> data [index] [index]);
	return stdev;
}

double DataModeler_getVarianceOfParameters (DataModeler me, integer fromIndex, integer toIndex, integer *out_numberOfFreeParameters) {
	double variance = undefined;
	if (toIndex < fromIndex || (toIndex == 0 && fromIndex == 0)) {
		fromIndex = 1;
		toIndex = my numberOfParameters;
	}
	integer numberOfFreeParameters = 0;
	if (fromIndex <= toIndex && fromIndex > 0 && toIndex <= my numberOfParameters) {
		variance = 0;
		for (integer index = fromIndex; index <= toIndex; index ++) {
			if (my parameters [index] .status != kDataModelerParameter::Fixed) {
				variance += my parameterCovariances -> data [index] [index];
				numberOfFreeParameters ++;
			}
		}
	}
	
	if (out_numberOfFreeParameters)
		*out_numberOfFreeParameters = numberOfFreeParameters;
	return variance;
}

void DataModeler_setParametersFree (DataModeler me, integer fromIndex, integer toIndex) {
	if (toIndex < fromIndex || (toIndex == 0 && fromIndex == 0)) {
		fromIndex = 1;
		toIndex = my numberOfParameters;
	}
	if (fromIndex <= toIndex && fromIndex > 0 && toIndex <= my numberOfParameters) {
		for (integer index = fromIndex; index <= toIndex; index ++)
			my parameters [index] .status = kDataModelerParameter::Free;
	}
}

void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas) {
	integer numberOfChangedParameters = 0;
	for (integer i = my numberOfParameters; i > 0; i --) {
		if (my parameters [i] .status != kDataModelerParameter::Fixed) {
			const double value = my parameters [i] .value;
			double sigmas = numberOfSigmas * DataModeler_getParameterStandardDeviation (me, i);
			if ((value - sigmas) * (value + sigmas) < 0) {
				DataModeler_setParameterValueFixed (me, i, 0.0);
				numberOfChangedParameters ++;
			}
		}
	}
}

integer DataModeler_getNumberOfFreeParameters (DataModeler me) {
	integer numberOfFreeParameters = 0;
	for (integer i = 1; i <= my numberOfParameters; i ++) {
		if (my parameters [i] .status == kDataModelerParameter::Free)
			numberOfFreeParameters ++;
	}
	return numberOfFreeParameters;
}

integer DataModeler_getNumberOfFixedParameters (DataModeler me) {
	return my numberOfParameters - DataModeler_getNumberOfFreeParameters (me);
}

static integer DataModeler_getNumberOfValidDataPoints (DataModeler me) {
	integer numberOfValidDataPoints = 0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my data [i] .status != kDataModelerData::Invalid)
			numberOfValidDataPoints ++;
	}
	return numberOfValidDataPoints;
}

integer DataModeler_getNumberOfInvalidDataPoints (DataModeler me) {
	return my numberOfDataPoints - DataModeler_getNumberOfValidDataPoints  (me);
}

void DataModeler_setTolerance (DataModeler me, double tolerance) {
	my tolerance = ( tolerance > 0.0 ? tolerance : my numberOfDataPoints * NUMfpp -> eps );
}

double DataModeler_getDegreesOfFreedom (DataModeler me) {
	integer numberOfDataPoints = 0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my data [i] .status != kDataModelerData::Invalid)
			numberOfDataPoints ++;
	}
	const double ndf = numberOfDataPoints - DataModeler_getNumberOfFreeParameters (me);
	return ndf;
}

autoVEC DataModeler_getZScores (DataModeler me, kDataModelerWeights weighData) {
	try {
		autoVEC zscores = newVECraw (my numberOfDataPoints);
		double estimatedSigmaY;
		if (weighData == kDataModelerWeights::EqualWeights) {
			integer numberOfValidDataPoints;
			const double rss = DataModeler_getResidualSumOfSquares (me, & numberOfValidDataPoints);
			Melder_require (numberOfValidDataPoints > 1,
				U"Not enough data points to calculate sigma.");
			estimatedSigmaY = rss / (numberOfValidDataPoints - 1);
		}
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			double value = undefined;
			if (my data [i] .status != kDataModelerData::Invalid) {
				const double estimate = my f_evaluate (me, my data [i] .x, my parameters.get());
				const double sigma = ( weighData == kDataModelerWeights::EqualWeights ? estimatedSigmaY : DataModeler_getDataPointInverseWeight (me, i, weighData) );
				value = (my data [i] .y - estimate) / sigma;
			}
			zscores [i] = value;
		}
		return zscores;
	} catch (MelderError) {
		Melder_throw (U"No z-scores calculated.");
	}
}

// chisq and zscores may be the same arrays!
autoVEC DataModeler_getChisqScoresFromZScores (DataModeler me, constVEC zscores, bool substituteAverage) {
	Melder_assert (zscores.size == my numberOfDataPoints);
	autoVEC chisq = newVECraw (zscores.size);
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
			if (isundef (chisq [i]))
				chisq [i] = sumchisq / numberOfDefined;
		}
	}
	return chisq;
}

double DataModeler_getChiSquaredQ (DataModeler me, kDataModelerWeights weighData, double *out_prob, double *out_df) {
	double chisq;
	integer numberOfValidZScores;
	autoVEC zscores = DataModeler_getZScores (me, weighData);
	chisqFromZScores (zscores.get(), & chisq, & numberOfValidZScores);
	const double df = ( weighData == kDataModelerWeights::EqualWeights ? numberOfValidZScores - 1.0 : numberOfValidZScores );   // we lose one df if sigma is estimated from the data
	
	if (out_prob)
		*out_prob = NUMchiSquareQ (chisq, df);
	if (out_df)
		*out_df = df;
	return chisq;
}

double DataModeler_getWeightedMean (DataModeler me) {
	double ysum = 0.0, wsum = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my data [i] .status != kDataModelerData::Invalid) {
			const double s = DataModeler_getDataPointInverseWeight (me, i, my weighData);
			const double weight = 1.0 / (s * s);
			ysum += my data [i] .y * weight;
			wsum += weight;
		}
	}
	return ysum / wsum;
}

double DataModeler_getCoefficientOfDetermination (DataModeler me, double *out_ssreg, double *out_sstot) {

	/*
		We cannot use the standard expressions for ss_tot, and ss_reg because our data are weighted by 1 / sigma [i].
		We need the weighted mean and we need to weigh all sums-of-squares accordingly;
		if all sigma [i] terms are equal, the formulas reduce to the standard ones.
		Ref: A. Buse (1973): Goodness of Fit in Generalized Least Squares Estimation, The American Statician, vol 27, 106-108
	 */

	const double ymean = DataModeler_getWeightedMean (me);
	double sstot = 0.0, ssreg = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my data [i] .status != kDataModelerData::Invalid) {
			const double s = DataModeler_getDataPointInverseWeight (me, i, my weighData);
			double diff = (my data [i] .y - ymean) / s;
			sstot += diff * diff; // total sum of squares
			const double estimate = my f_evaluate (me, my data [i] .x, my parameters.get());
			diff = (estimate - my data [i] .y) / s;
			ssreg += diff * diff; // regression sum of squares
		}
	}
	const double rSquared = ( sstot > 0.0 ? 1.0 - ssreg / sstot : 1.0 );
	
	if (out_ssreg)
		*out_ssreg = sstot - ssreg;
	if (out_sstot)
		*out_sstot = sstot;
	return rSquared;
}

void DataModeler_drawBasisFunction_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	integer iterm, bool scale, integer numberOfPoints)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	autoVEC x = newVECraw (numberOfPoints);
	autoVEC y = newVECraw (numberOfPoints);
	autoVEC term = newVECraw (my numberOfParameters);
	for (integer i = 1; i <= numberOfPoints; i ++) {
		x [i] = xmin + (i - 0.5) * (xmax - xmin) / numberOfPoints;
		my f_evaluateBasisFunctions (me, x [i], term.get());
		y [i] = term [iterm];
		y [i] = ( scale ? y [i] * my parameters [iterm] .value : y [i] );
	}
	if (ymax <= ymin) {
		MelderExtremaWithInit extrema;
		for (integer i = 1; i <= numberOfPoints; i ++)
			extrema.update (y [i]);
		ymax = extrema.max;
		ymin = extrema.min;
		
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (integer i = 2; i <= numberOfPoints; i ++)
		Graphics_line (g, x [i-1], y [i-1], x [i], y [i]);
}

integer DataModeler_drawingSpecifiers_x (DataModeler me, double *xmin, double *xmax, integer *ixmin, integer *ixmax) {
	if (*xmax <= *xmin) {
		*xmin = my xmin;
		*xmax = my xmax;
	}
	*ixmin = 2;
	while (my data [*ixmin] .x < *xmin && *ixmin < my numberOfDataPoints)
		(*ixmin) ++;
	(*ixmin) --;
	*ixmax = my numberOfDataPoints - 1;
	while (my data [*ixmax] .x > *xmax && *ixmax > 1)
		(*ixmax) --;
	(*ixmax) ++;
	return *ixmax - *ixmin + 1;
}

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double numberOfSigmas, kDataModelerWeights weighData, conststring32 mark, double marksFontSize, double horizontalOffset_mm)
{
	integer ixmin, ixmax;
	if (DataModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) < 1) return;
	autoVEC zscores = DataModeler_getZScores (me, weighData);
	const double horizontalOffset_wc = Graphics_dxMMtoWC (g, horizontalOffset_mm);
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setFontSize (g, marksFontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	const double currentFontSize = Graphics_inqFontSize (g);
	for (integer idata = 1; idata <= my numberOfDataPoints; idata ++) {
		if (my data [idata] .status != kDataModelerData::Invalid) {
			const double x = my data [idata] .x, y = my data [idata] .y;
			if (x >= xmin && x <= xmax && y >= ymin && y <= ymax)
				if (fabs (zscores [idata]) > numberOfSigmas)
					Graphics_text (g, x + horizontalOffset_wc, y, mark);
		}
	}
	Graphics_setFontSize (g, currentFontSize);
}

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool errorbars, bool connectPoints, double barWidth_mm, double horizontalOffset_mm, bool drawDots)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer ixmin = 2;
	while (my data [ixmin] .x < xmin && ixmin < my numberOfDataPoints)
		ixmin ++;
	ixmin --;
	integer ixmax = my numberOfDataPoints - 1;
	while (my  data [ixmax] .x > xmax && ixmax > 1)
		ixmax --;
	ixmax ++;
	if (ixmin >= ixmax)
		return; // nothing to draw
	numberOfParameters = ( numberOfParameters > my numberOfParameters ? my numberOfParameters : numberOfParameters );
	autovector<structDataModelerParameter> parameters = newvectorcopy (my parameters.all());
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	const double horizontalOffset_wc = Graphics_dxMMtoWC (g, horizontalOffset_mm);
	const double barWidth_wc = ( barWidth_mm <= 0.0 ? 0.0 : Graphics_dxMMtoWC (g, barWidth_mm) );
	double x1, y1, x2, y2;
	bool x1defined = false, x2defined = false;
	for (integer idata = ixmin; idata <= ixmax; idata ++) {
		if (my data [idata] .status != kDataModelerData::Invalid) {
			const double x = my  data [idata] .x, y = my data [idata].y;
			if (! x1defined) {
				x1 = x;
				y1 = ( estimated ? my f_evaluate (me, x, parameters.get()) : y );
				x1defined = true;
			} else {
				x2 = x;
				y2 = ( estimated ? my f_evaluate (me, x, parameters.get()) : y );
				x2defined = true;
			}
			if (x1defined && drawDots) {
				if (y >= ymin && y <= ymax)
					Graphics_speckle (g, x + horizontalOffset_wc, y);
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
				const double sigma = my data [idata] .sigmaY; // DataModeler_getDataPointInverseWeight ?
				const double ym = y1;
				double yt = ym + 0.5 * sigma, yb = ym - 0.5 * sigma;
				if (estimated) {
					yt = ( (y - y1) > 0.0 ? y : y1 );
					yb = ( (y - y1) > 0.0 ? y1 : y );
				}
				bool topOutside = yt > ymax, bottomOutside = yb < ymin;
				yt = ( topOutside ? ymax : yt );
				yb = ( bottomOutside ? ymin : yb );
				Graphics_line (g, x1 + horizontalOffset_wc, yb, x1 + horizontalOffset_wc, yt);
				if (barWidth_wc > 0.0 && ! estimated) {
					double xl = x1 - 0.5 * barWidth_wc + horizontalOffset_wc;
					double xr = xl + barWidth_wc;
					if (! topOutside)
						Graphics_line (g, xl, yt, xr, yt);
					if (! bottomOutside)
						Graphics_line (g, xl, yb, xr, yb);
				}
			}
		}
	}
}

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool estimated, integer numberOfParameters, double horizontalOffset_mm)
{
	const bool errorbars = false, connectPoints = true;
	const double barWidth_mm = 0;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, horizontalOffset_mm, 0);
}

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, double horizontalOffset_mm, bool garnish) {
	if (ymax <= ymin)
		DataModeler_getExtremaY (me, & ymin, & ymax);
	Graphics_setInner (g);
	DataModeler_drawTrack_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool errorbars, double barWidth_mm, double horizontalOffset_mm) {
	bool connectPoints = false;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, horizontalOffset_mm, 1);
}

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool errorbars, double barWidth_mm, double horizontalOffset_mm, bool garnish)
{
	if (ymax <= ymin)
		DataModeler_getExtremaY (me, & ymin, & ymax);
	Graphics_setInner (g);
	DataModeler_speckle_inside (me, g, xmin, xmax, ymin, ymax,
		estimated, numberOfParameters, errorbars, barWidth_mm, horizontalOffset_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoTable DataModeler_to_Table_zscores (DataModeler me, kDataModelerWeights weighData) {
	try {
		autoTable ztable = Table_createWithColumnNames (my numberOfDataPoints, U"x z");
		autoVEC zscores = DataModeler_getZScores (me, weighData);
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			Table_setNumericValue (ztable.get(), i, 1, my  data [i] .x);
			Table_setNumericValue (ztable.get(), i, 2, zscores [i]);
		}
		return ztable;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

void DataModeler_normalProbabilityPlot (DataModeler me, Graphics g,	kDataModelerWeights weighData, integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish) {
	try {
		autoTable thee = DataModeler_to_Table_zscores (me, weighData);
		Table_normalProbabilityPlot (thee.get(), g, 2, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void DataModeler_setBasisFunctions (DataModeler me, kDataModelerFunction type) {
	if (type == kDataModelerFunction::Legendre) {
		my f_evaluate = legendre_evaluate;
		my f_evaluateBasisFunctions = legendre_evaluateBasisFunctions;
	} else {
		my f_evaluate = polynomial_evaluate;
		my f_evaluateBasisFunctions = polynomial_evaluateBasisFunctions;
	}
	my type = type;
}

void DataModeler_init (DataModeler me, double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, kDataModelerFunction type) {
	my xmin = xmin;
	my xmax = xmax;
	DataModeler_setBasisFunctions (me, type);
	my numberOfDataPoints = numberOfDataPoints;
	my data = newvectorzero<structDataModelerData> (numberOfDataPoints);
	my numberOfParameters = numberOfParameters;
	
	Melder_require (numberOfParameters > 0,
		U"The number of parameters should be greater than zero.");
	Melder_require (numberOfParameters <= numberOfDataPoints,
		U"The number of parameters should not exceed the number of data points");
	
	my parameters = newvectorzero<structDataModelerParameter> (numberOfParameters);
	my parameterNames = Strings_createFixedLength (numberOfParameters);
	my parameterCovariances = Covariance_create (numberOfParameters);
	my type = type;
}

autoDataModeler DataModeler_create (double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, kDataModelerFunction type) {
	try {
		autoDataModeler me = Thing_new (DataModeler);
		DataModeler_init (me.get(), xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		return me;
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}
}

autoDataModeler DataModeler_createSimple (double xmin, double xmax,
	integer numberOfDataPoints, conststring32 parameters, double gaussianNoiseStd, kDataModelerFunction type)
{
	try {
		autoVEC parameterValues = VEC_createFromString (parameters);
		Melder_require (xmin < xmax,
			U"The domain should be defined properly.");
		
		autoDataModeler me = DataModeler_create (xmin, xmax, numberOfDataPoints, parameterValues.size, type);
		for (integer i = 1; i <= parameterValues.size; i ++)
			my parameters [i] .value = parameterValues [i];   // parameters status ok
		// generate the data that beinteger to the parameter values
		for (integer i = 1; i <= numberOfDataPoints; i ++) {
			my  data [i] .x = xmin + (i - 0.5) * (xmax - xmin) / numberOfDataPoints;
			const double modelY = my f_evaluate (me.get(), my data [i] .x, my parameters.get());
			my data [i] .y = modelY + NUMrandomGauss (0.0, gaussianNoiseStd);
			my data [i] .sigmaY = undefined;
		}
		my weighData = kDataModelerWeights::EqualWeights;
		return me;
	} catch (MelderError) {
		Melder_throw (U"No simple DataModeler created.");
	}
}

void DataModeler_fit (DataModeler me) {
	try {
		// Count the number of parameters to be fitted

		const integer numberOfParameters = DataModeler_getNumberOfFreeParameters (me);
		if (numberOfParameters == 0) return;
		const integer numberOfDataPoints = DataModeler_getNumberOfValidDataPoints (me);
		autoVEC b = newVECzero (numberOfDataPoints);
		autoVEC term = newVECzero (my numberOfParameters);
		autovector<structDataModelerParameter> parameters = newvectorcopy (my parameters.all());
		autoMAT design = newMATzero (numberOfDataPoints, numberOfParameters);

		// For function evaluation with only the FIXED parameters

		for (integer ipar = 1; ipar <= my numberOfParameters; ipar ++)
			parameters [ipar] .value = ( my parameters [ipar] .status == kDataModelerParameter::Fixed ? my parameters [ipar] .value : 0.0 );

		// estimate sigma if we weigh all datapoint equally. 
		// This is necessary to get the parameter covariances right
		double sigmaY = ( my weighData == kDataModelerWeights::EqualWeights ? DataModeler_estimateSigmaY (me) : undefined );
		integer idata = 0;
		// Accumulate coefficients of the design matrix
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			if (my data [i] .status != kDataModelerData::Invalid) {
				++ idata;
				// function evaluation with only the FIXED parameters
				const double xi = my  data [i] .x, yi = my data [i] .y;
				const double yFixed = my f_evaluate (me, xi, parameters.get());
				const double si = ( my weighData != kDataModelerWeights::EqualWeights ? DataModeler_getDataPointInverseWeight (me, i, my weighData) : sigmaY );

				// individual terms of the function

				my f_evaluateBasisFunctions (me, xi, term.get());
				integer ipar = 0;
				for (integer j = 1; j <= my numberOfParameters; j ++)
					if (my parameters [j] .status!= kDataModelerParameter::Fixed)
						design [idata] [++ ipar] = term [j] / si;

				// only 'residual variance' must be explained by the model

				b [idata] = (yi - yFixed) / si;
			}
		}
		
		// Singular value decomposition and evaluation of the singular values

		autoSVD thee = SVD_createFromGeneralMatrix (design.get());
		if (! NUMfpp)
			NUMmachar ();
		SVD_zeroSmallSingularValues (thee.get(), ( my tolerance > 0.0 ? my tolerance : numberOfDataPoints * NUMfpp -> eps ));
		autoVEC result = SVD_solve (thee.get(), b.get());

		// Put the calculated parameters at the correct position in 'my p'
		Covariance cov = my parameterCovariances.get();
		integer ipar = 0;
		for (integer j = 1; j <= my numberOfParameters; j ++) {
			if (my parameters [j] .status != kDataModelerParameter::Fixed)
				my parameters [j] .value = parameters [++ ipar] .value;
			cov -> centroid [j] = my parameters [j] .value;
		}
		cov -> numberOfObservations = numberOfDataPoints;
		// estimate covariances between parameters
		if (numberOfParameters < my numberOfParameters) {
			autoMAT covtmp = SVD_getSquared (thee.get(), true);
			// Set fixed parameters variances and covariances to zero.
			cov -> data.all() <<= 0.0;
			
			ipar = 0;
			for (integer i = 1; i <= my numberOfParameters; i ++) {
				if (my parameters [i] .status != kDataModelerParameter::Fixed) {
					++ ipar;
					integer jpar = 0;
					for (integer j = 1; j <= my numberOfParameters; j ++) {
						if (my parameters [j] .status != kDataModelerParameter::Fixed)
							cov -> data [i] [j] = covtmp [ipar] [++ jpar];
					}
				}
			}
		} else {
			SVD_getSquared_preallocated (thee.get(), true, cov -> data.get());
		}
	} catch (MelderError) {
		Melder_throw (U"DataModeler no fit.");
	}
}

void DataModeler_setDataWeighing (DataModeler me, kDataModelerWeights weighData) {
	if (my weighData != weighData) {
		my weighData = weighData;
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

autoDataModeler Table_to_DataModeler (Table me, double xmin, double xmax, integer xcolumn, integer ycolumn, integer sigmacolumn, integer numberOfParameters,  kDataModelerFunction type) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, xcolumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, ycolumn);
		const bool hasSigmaColumn = ( sigmacolumn > 0 );
		if (hasSigmaColumn)
			Table_checkSpecifiedColumnNumberWithinRange (me, sigmacolumn);
		const integer numberOfRows = my rows.size;
		integer numberOfData = 0;
		autoVEC x = newVECraw (numberOfRows);
		autoVEC y = newVECraw (numberOfRows);
		autoVEC sy = newVECraw (numberOfRows);
		for (integer i = 1; i <= numberOfRows; i ++) {
			const double val = Table_getNumericValue_Assert (me, i, xcolumn);
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
				sy [numberOfData] = ( hasSigmaColumn ? Table_getNumericValue_Assert (me, i, sigmacolumn) : 1.0 );
			}
		}
		if (xmax <= xmin)
			NUMextrema (x.part (1, numberOfData), & xmin, & xmax);
		Melder_require (xmin < xmax,
			U"The range of the x-values is too small.");
		
		integer numberOfDataPoints = 0, validData = 0;
		for (integer i = 1; i <= numberOfData; i ++) {
			if (x [i] >= xmin && x [i] <= xmax)
				numberOfDataPoints ++;
		}
		autoDataModeler thee = DataModeler_create (xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		numberOfDataPoints = 0;
		for (integer i = 1; i <= numberOfData; i ++) {
			if (x [i] >= xmin && x [i] <= xmax) {
				thy data [++ numberOfDataPoints] .x = x [i];
				thy data [numberOfDataPoints] .status = kDataModelerData::Invalid;
				if (isdefined (y [i])) {
					thy data [numberOfDataPoints] .y = y [i];
					thy data [numberOfDataPoints] .sigmaY = sy [i];
					thy data [numberOfDataPoints] .status = kDataModelerData::Valid;
					validData ++;
				}
			}
		}
		thy numberOfDataPoints = numberOfDataPoints;
		thy tolerance = 1e-5;
		thy weighData = ( hasSigmaColumn ? kDataModelerWeights::OneOverSigma : kDataModelerWeights::EqualWeights);
		Melder_require (validData >= numberOfParameters,
			U"The number of parameters should not exceed the number of data points.");
		
		DataModeler_setDataWeighing (thee.get(), kDataModelerWeights::OneOverSigma);
		DataModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Datamodeler not created from Table.");
	}
}

double DataModeler_getResidualSumOfSquares (DataModeler me, integer *numberOfDataPoints) {
	integer n = 0;
	longdouble rss = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my data [i] .status != kDataModelerData::Invalid) {
			++ n;
			const double dif = my data [i] .y - my f_evaluate (me, my data [i] .x, my parameters.get());
			rss += dif * dif;
		}
	}
	if (numberOfDataPoints)
		*numberOfDataPoints = n;
	return ( n > 0 ? (double) rss : undefined );
}

void DataModeler_reportChiSquared (DataModeler me, kDataModelerWeights weighData) {
	MelderInfo_writeLine (U"Chi squared test:");
	MelderInfo_writeLine (( weighData == kDataModelerWeights::EqualWeights ? U"Standard deviation is estimated from the data." :
		( weighData == kDataModelerWeights::OneOverSigma ? U"Sigmas are used as estimate for local standard deviations." :
		( weighData == kDataModelerWeights::Relative ? U"1/Q's are used as estimate for local standard deviations." :
		U"Sqrt sigmas are used as estimate for local standard deviations." ) ) ));
	double ndf, probability;
	const double chisq = DataModeler_getChiSquaredQ (me, weighData, & probability, & ndf);
	MelderInfo_writeLine (U"Chi squared = ", chisq);
	MelderInfo_writeLine (U"Probability = ", probability);
	MelderInfo_writeLine (U"Number of degrees of freedom = ", ndf);	
}

double DataModeler_estimateSigmaY (DataModeler me) {
	try {
		integer numberOfDataPoints = 0;
		autoVEC y = newVECraw (my numberOfDataPoints);
		for (integer i = 1; i <= my numberOfDataPoints; i ++) {
			if (my data [i] .status != kDataModelerData::Invalid)
				y [++ numberOfDataPoints] = my data [i] .y;
		}
		y.size = numberOfDataPoints;   // fake shrink
		return NUMstdev (y.get());
	} catch (MelderError) {
		Melder_throw (U"Cannot estimate sigma.");
	}
}

/* End of file DataModeler.cpp */
