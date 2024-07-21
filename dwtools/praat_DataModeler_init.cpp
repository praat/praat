/* praat_DataModeler_init.cpp
 *
 * Copyright (C) 2014-2024 David Weenink
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
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "praatM.h"
#include "DataModeler.h"
#include "Formant_extensions.h"
#include "FormantModeler.h"
#include "OptimalCeilingTierEditor.h"
#include "Pitch.h"
#include "Table_extensions.h"
#include "TextGrid.h"

/* DataModeler */

FORM (CREATE_ONE__DataModeler_createSimple, U"Create simple DataModeler", nullptr) {
	WORD (name, U"Name", U"dm")
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"1.0")
	NATURAL (numberOfDataPoints, U"Number of data points", U"20")
	REALVECTOR (parameters, U"Parameters", WHITESPACE_SEPARATED_, U"0.0 1.0 1.0")
	POSITIVE (standardDeviation, U"Gaussian noise stdev", U"0.2")
	OPTIONMENU_ENUM (kDataModelerFunction, type, U"Basis functions", kDataModelerFunction::DEFAULT)		
	OK
DO
	CREATE_ONE
		autoDataModeler result = DataModeler_createSimple (xmin, xmax, numberOfDataPoints, parameters, standardDeviation, type);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_EACH__DataModeler_speckle, U"DataModeler: Speckle", U"DataModeler: Speckle...") {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"0.0")
	BOOLEAN (errorBars, U"Draw error bars", 1)
	REAL (barWidth_wc, U"Bar width (wc)", U"0.001")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (DataModeler)
		DataModeler_speckle (me, GRAPHICS, xmin, xmax, ymin, ymax, false, errorBars, barWidth_wc, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__DataModeler_drawModel, U"DataModeler: Draw model", U"DataModeler: Draw model...") {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"0.0")
	NATURAL (numberOfPoints, U"Number of points", U"1000")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (DataModeler)
		DataModeler_drawModel (me, GRAPHICS, xmin, xmax, ymin, ymax, numberOfPoints, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__DataModeler_drawEstimatedTrack, U"DataModeler: Draw estimated track", U"DataModeler: Draw estimated track...") {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (DataModeler)
		DataModeler_drawTrack (me, GRAPHICS, xmin, xmax, ymin, ymax, true, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__DataModeler_drawResiduals, U"DataModeler: Draw residuals", U"DataModeler: Draw residuals...") {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (DataModeler)
		DataModeler_drawResiduals (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfParameters) {
	QUERY_ONE_FOR_INTEGER (DataModeler)
		const integer result = my numberOfParameters;
	QUERY_ONE_FOR_INTEGER_END (U" (number of parameters)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfFixedParameters) {
	QUERY_ONE_FOR_INTEGER (DataModeler)
		const integer result = DataModeler_getNumberOfFixedParameters (me);
	QUERY_ONE_FOR_INTEGER_END (U" (number of fixed parameters)")
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getParameterValue, U"DataModeler: Get parameter value", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getParameterValue (me, parameterNumber);
	QUERY_ONE_FOR_REAL_END (U" (parameter ", parameterNumber, U")")
}

DIRECT (QUERY_ONE_FOR_REAL_VECTOR__DataModeler_listParameterValues) {
	QUERY_ONE_FOR_REAL_VECTOR (DataModeler)
		autoVEC result = DataModeler_listParameterValues (me);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_STRING__DataModeler_getParameterStatus, U"DataModeler: Get parameter status", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (DataModeler)
		const kDataModelerParameterStatus status = DataModeler_getParameterStatus (me, parameterNumber);
		conststring32 result = ( status == kDataModelerParameterStatus::FREE ? U"Free" :
			status == kDataModelerParameterStatus::FIXED_ ? U"Fixed" : U"Undefined" );
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getParameterStandardDeviation, U"DataModeler: Get parameter standard deviation", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getParameterStandardDeviation (me, parameterNumber);
	QUERY_ONE_FOR_REAL_END (U" (parameter ", parameterNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getVarianceOfParameters, U"DataModeler: Get variance of parameters", nullptr) {
	NATURAL (fromParameter, U"left Parameter range", U"1")
	INTEGER (toParameter, U"right Parameter range", U"0 (= all)")
	OK
DO
	integer nofp;
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getVarianceOfParameters (me, fromParameter, toParameter, &nofp);
	QUERY_ONE_FOR_REAL_END (U" (for ", nofp, U" free parameters)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfDataPoints) {
	QUERY_ONE_FOR_INTEGER (DataModeler)
		const integer result = my numberOfDataPoints;
	QUERY_ONE_FOR_INTEGER_END (U" (number of data points)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfInvalidDataPoints) {
	QUERY_ONE_FOR_INTEGER (DataModeler)
		const integer result = DataModeler_getNumberOfInvalidDataPoints (me);
	QUERY_ONE_FOR_INTEGER_END (U" (number of invalid data points)")
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getModelValueAtX, U"DataModeler: Get model value at x", nullptr) {
	REAL (x, U"X", U"0.1")
	OK
DO
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getModelValueAtX (me, x);
	QUERY_ONE_FOR_REAL_END (U"")
}

DIRECT (QUERY_ONE_FOR_REAL__DataModeler_getResidualSumOfSquares) {
	QUERY_ONE_FOR_REAL (DataModeler)
		integer n;
		const double result = DataModeler_getResidualSumOfSquares (me, & n);
	QUERY_ONE_FOR_REAL_END (U"  (for ", n, U" datapoints)")
}

DIRECT (QUERY_ONE_FOR_REAL__DataModeler_getResidualStandardDeviation) {
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getResidualStandardDeviation (me);
	QUERY_ONE_FOR_REAL_END (U"  (residual standard deviation)")
}

DIRECT (QUERY_ONE_FOR_REAL__DataModeler_getDataStandardDeviation) {
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getDataStandardDeviation (me);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getDataPointXValue, U"DataModeler: Get data point x value", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getDataPointXValue (me, index);
	QUERY_ONE_FOR_REAL_END (U" (value at point ", index, U")")
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getDataPointYValue, U"DataModeler: Get data point y value", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getDataPointYValue (me, index);
	QUERY_ONE_FOR_REAL_END (U" (value at point ", index, U")")
}

FORM (QUERY_ONE_FOR_REAL__DataModeler_getDataPointYSigma, U"DataModeler: Get data point y sigma", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getDataPointYSigma (me, index);
	QUERY_ONE_FOR_REAL_END (U" (sigma at point ", index, U")")
}

FORM (QUERY_ONE_FOR_STRING__DataModeler_getDataPointStatus, U"DataModeler: Get data point status", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (DataModeler)
		const kDataModelerData status = DataModeler_getDataPointStatus (me, index);
		conststring32 result = ( status == kDataModelerData::INVALID ? U"Invalid" : U"Valid" );
	QUERY_ONE_FOR_STRING_END
}

DIRECT (QUERY_ONE_FOR_REAL__DataModeler_getCoefficientOfDetermination) {
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getCoefficientOfDetermination (me, nullptr, nullptr);
	QUERY_ONE_FOR_REAL_END (U" (= R^2)");
}

DIRECT (INFO_ONE__DataModeler_reportChiSquared) {
	INFO_ONE (DataModeler)
		MelderInfo_open();
		DataModeler_reportChiSquared (me);
		MelderInfo_close();
	INFO_ONE_END
}

DIRECT (QUERY_ONE_FOR_MATRIX__DataModeler_getHessian) {
	QUERY_ONE_FOR_MATRIX (DataModeler)
		autoMAT result = DataModeler_getHessian (me);
	QUERY_ONE_FOR_MATRIX_END
}

DIRECT (QUERY_ONE_FOR_REAL__DataModeler_getDegreesOfFreedom) {
	QUERY_ONE_FOR_REAL (DataModeler)
		const double result = DataModeler_getDegreesOfFreedom (me);
	QUERY_ONE_FOR_REAL_END (U" degrees of freedom")
}

FORM (MODIFY_EACH__DataModeler_setDataWeighing, U"DataModeler: Set data weighing", nullptr) {
	OPTIONMENU_ENUM (kDataModelerWeights, weighDataType, U"Weigh data", kDataModelerWeights::DEFAULT)
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataWeighing (me, weighDataType);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setTolerance, U"DataModeler: Set tolerance", nullptr) {
	REAL (tolerance, U"Tolerance", U"1e-5")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setTolerance (me, tolerance);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setParameterName, U"DataModeler: Set parameter name", nullptr) {
	NATURAL (index, U"Index", U"1")
	SENTENCE (parameterName, U"Name", U"p [1]")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterName (me, index, parameterName);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setParameterValue, U"DataModeler: Set parameter value", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	REAL (value, U"Value", U"0.0")
	OPTIONMENU_ENUM (kDataModelerParameterStatus, parameterStatus, U"Status", kDataModelerParameterStatus::DEFAULT)
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterValue (me, parameterNumber, value, parameterStatus);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setParameterFree, U"DataModeler: Set parameter free", nullptr) {
	INTEGER (fromParameter, U"left Parameter range", U"0")
	INTEGER (toParameter, U"right Parameter range", U"0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParametersFree (me, fromParameter, toParameter);
	MODIFY_EACH_END
}


FORM (MODIFY_EACH__DataModeler_setParameterValuesToZero, U"DataModeler: Set parameter values to zero", nullptr) {
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterValuesToZero (me, numberOfSigmas);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setDataPointStatus, U"DataModeler: Set data point status", nullptr) {
	NATURAL (index, U"Index", U"1")
	OPTIONMENU (dataStatus, U"Status", 1)
		OPTION (U"valid")
		OPTION (U"invalid")
	OK
DO
	kDataModelerData status = dataStatus == 2 ? kDataModelerData::INVALID : kDataModelerData::VALID;
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointStatus (me, index, status);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setDataPointXValue, U"DataModeler: Set data point x value", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (x, U"X", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointXValue (me, index, x);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setDataPointYValue, U"DataModeler: Set data point y value", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointYValue (me, index, y);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setDataPointValues, U"DataModeler: Set data point values", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointValues (me, index, x, y);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__DataModeler_setDataPointYSigma, U"DataModeler: Set data point y sigma", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (sigma, U"Sigma", U"10.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointYSigma (me, index, sigma);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__DataModeler_fitModel) {
	MODIFY_EACH (DataModeler)
		DataModeler_fit (me);
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__DataModeler_to_Covariance_parameters) {
	CONVERT_EACH_TO_ONE (DataModeler)
		autoCovariance result = DataModeler_to_Covariance_parameters (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__DataModeler_to_Table_zscores) {
	CONVERT_EACH_TO_ONE (DataModeler)
		autoTable result = DataModeler_to_Table_zscores (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_z");
}

FORM (CONVERT_EACH_TO_ONE__Formant_to_FormantModeler, U"Formant: To FormantModeler", nullptr) {
	REAL (fromTime, U"left Start time", U"0.0")
	REAL (toTime, U"right End time", U"0.1")
	NATURAL (numberOfFormants, U"Number of formants", U"3")
	INTEGER (order, U"Order of polynomials", U"3")
	OK
DO
	Melder_require (order >= 0, 
		U"The order should be at least zero.");
	CONVERT_EACH_TO_ONE (Formant)
		autoFormantModeler result = Formant_to_FormantModeler (me, fromTime, toTime, numberOfFormants, order + 1);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_o", order);
}

FORM (COMBINE_ALL_TO_ONE__Formants_extractSmoothestPart, U"Formants: Extract smoothest part", U"Formants: Extract smoothest part") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	COMMENT (U"Use bandwidths to model the formant tracks:")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Zero parameter values whose range include zero:")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	COMBINE_ALL_TO_ONE (Formant)
		const integer index = Formants_getSmoothestInInterval (& list, fromTime, toTime, 
			numberOfFormantTracks, order + 1, weighFormants, 0, numberOfSigmas, power, 1.0, 1.0, 1.0, 1.0, 1.0
		);
		Formant him = list.at [index];
		autoFormant result = Formant_extractPart (him, fromTime, toTime);
	COMBINE_ALL_TO_ONE_END (his name.get(), U"_part")
}

FORM (COMBINE_ALL_TO_ONE__Formants_extractSmoothestPart_constrained, U"Formants: Extract smoothest part (constrained)", U"Formants: Extract smoothest part (constrained)...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	COMMENT (U"Use bandwidths to model the formant tracks:")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Zero parameter values whose range include zero:")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	COMMENT (U"The constraints on the formants:")
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1500.0")
	OK
DO
	COMBINE_ALL_TO_ONE (Formant)
		const integer index = Formants_getSmoothestInInterval (& list, fromTime, toTime, 
			numberOfFormantTracks, order + 1,
			weighFormants, 1, numberOfSigmas, power, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3
		);
		Formant him = list.at [index];
		autoFormant result = Formant_extractPart (him, fromTime, toTime);
	COMBINE_ALL_TO_ONE_END (his name.get(), U"_part")
}

/********************** FormantModeler ******************************/

FORM (GRAPHICS_EACH__FormantModeler_drawEstimatedTracks, U"FormantModeler: Draw estimated tracks", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"3")
	INTEGER (order, U"Order of polynomials for estimation", U"3")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	Melder_require (order >= 0,
		U"The order should be at least zero.");
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, 
			true, order + 1, Melder_BLACK, Melder_BLACK, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantModeler_drawTracks, U"FormantModeler: Draw tracks", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"3")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	integer order = 6;
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant,
			false, order + 1, Melder_BLACK, Melder_BLACK, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantModeler_speckle, U"FormantModeler: Speckle", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"3")
	BOOLEAN (errorBars, U"Draw error bars", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	integer order = 6;
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_speckle (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, 
			0, order + 1, errorBars, Melder_BLACK, Melder_BLACK, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantModeler_drawOutliersMarked, U"FormantModeler: Draw outliers marked", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"3")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"3.0")
	WORD (mark_string, U"Mark", U"o")
	POSITIVE (fontSize, U"Mark font size", U"12")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawOutliersMarked (
			me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, numberOfSigmas, mark_string,
			fontSize, Melder_BLACK, Melder_BLACK, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantModeler_drawVariancesOfShiftedTracks, U"FormantModeler: Draw variances of shifted tracks", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (fromVariance, U"left Variance range", U"0.0")
	REAL (toVariance, U"right Variance range", U"0.0")
	OPTIONMENU_ENUM (kFormantModelerTrackShift, shiftTracks, U"Shift tracks", kFormantModelerTrackShift::DEFAULT)
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"4")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawVariancesOfShiftedTracks (
			me, GRAPHICS, fromTime, toTime, fromVariance, toVariance, shiftTracks, fromFormant, toFormant, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantModeler_drawCumulativeChisqScores, U"FormantModeler: Draw cumulative chi scores", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (fromChisq, U"left Chisq range", U"0.0")
	REAL (toChisq, U"right Chisq range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawCumulativeChiScores (me, GRAPHICS, fromTime, toTime, fromChisq, toChisq, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_EACH__FormantModeler_normalProbabilityPlot, U"FormantModeler: Normal probability plot", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (numberOfSigmas, U"Number of sigmas", U"0.0")
	POSITIVE (fontSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_normalProbabilityPlot (
			me, GRAPHICS, formantNumber, numberOfQuantiles, numberOfSigmas, fontSize, label, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantModeler_drawBasisFunction, U"FormantModeler: Draw basis function", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (ymin, U"left Amplitude range (Hz)", U"0.0")
	REAL (ymax, U"right Amplitude range (Hz)", U"5500.0")
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (basisFunctionIndex, U"Basis function", U"2")
	BOOLEAN (scale, U"Scale function with parameter value", false)
	NATURAL (numberOfPoints, U"Number of points", U"200")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawBasisFunction (
			me, GRAPHICS, fromTime, toTime, ymin, ymax, formantNumber, basisFunctionIndex, scale, numberOfPoints, garnish
		);
	GRAPHICS_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getModelValueAtTime, U"", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getModelValueAtTime (me, formantNumber, time);
	QUERY_ONE_FOR_REAL_END (U"Hertz")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getDataPointValue, U"FormantModeler: Get data point value", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getDataPointValue (me, formantNumber, index);
	QUERY_ONE_FOR_REAL_END (U" (value of point ", index, U" in track F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getDataPointSigma, U"FormantModeler: Get data point sigma", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getDataPointSigma (me, formantNumber, index);
	QUERY_ONE_FOR_REAL_END (U" (sigma of point ", index, U" in track F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_STRING__FormantModeler_getDataPointStatus, U"FormantModeler: Get data point status", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (FormantModeler)
		const kDataModelerData status = FormantModeler_getDataPointStatus (me, formantNumber, index);
		conststring32 result = ( status == kDataModelerData::INVALID ? U"Invalid" : U"Valid" );
	QUERY_ONE_FOR_STRING_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfTracks) {
	QUERY_ONE_FOR_INTEGER (FormantModeler)
		const integer result = FormantModeler_getNumberOfTracks (me);
	QUERY_ONE_FOR_INTEGER_END (U" formants")
}

FORM (QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfParameters, U"FormantModeler: Get number of parameters", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (FormantModeler)
		const integer result = FormantModeler_getNumberOfParameters (me, formantNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of parameters for F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfFixedParameters, U"FormantModeler: Get number of fixed parameters", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (FormantModeler)
		const integer result = FormantModeler_getNumberOfFixedParameters (me, formantNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of fixed parameters for F", formantNumber, U")")
}

DIRECT (QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfDataPoints) {
	QUERY_ONE_FOR_INTEGER (FormantModeler)
		const integer result = FormantModeler_getNumberOfDataPoints (me);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfInvalidDataPoints, U"FormantModeler: Get number of invalid data points", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (FormantModeler)
		const integer result = FormantModeler_getNumberOfInvalidDataPoints (me, formantNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of invalid data points for F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getParameterValue, U"FormantModeler: Get parameter value", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getParameterValue (me, formantNumber, parameterNumber);
	QUERY_ONE_FOR_REAL_END (U" (parameter ", parameterNumber, U" for F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_STRING__FormantModeler_getParameterStatus, U"FormantModeler: Get parameter status", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (FormantModeler)
		const kDataModelerParameterStatus status = FormantModeler_getParameterStatus (me, formantNumber, parameterNumber);
		conststring32 result = Melder_cat (status == kDataModelerParameterStatus::FREE ? U"Free" : 
			status == kDataModelerParameterStatus::FIXED_ ? U"Fixed" : U"Undefined",
			U" (status of parameter ", parameterNumber, U" for F", formantNumber, U")"
		);
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getParameterStandardDeviation, U"FormantModeler: Get parameter standard deviatio", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getParameterStandardDeviation (me, formantNumber, parameterNumber);
	QUERY_ONE_FOR_REAL_END (U" (standard deviation of parameter ", parameterNumber, U" for F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getVarianceOfParameters, U"FormantModeler: Get variance of parameters", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	INTEGER (fromParameter, U"left Parameter range", U"0")
	INTEGER (toParameter, U"right Parameter range", U"0")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		integer numberOfFreeParameters;
		const double result = FormantModeler_getVarianceOfParameters (
			me, fromFormant, toFormant, fromParameter, toParameter, & numberOfFreeParameters
		);
	QUERY_ONE_FOR_REAL_END (U" (for ", numberOfFreeParameters, U" free parameters.)")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getCoefficientOfDetermination, U"FormantModeler: Get coefficient of determination", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getCoefficientOfDetermination (me, fromFormant, toFormant);
	QUERY_ONE_FOR_REAL_END (U" (= R^2)");
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getResidualSumOfSquares, U"FormantModeler: Get residual sum of squares", U"FormantModeler: Get residual sum of squares...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getResidualSumOfSquares (me, formantNumber, nullptr);
	QUERY_ONE_FOR_REAL_END (U" Hz^2 (residual sum of squares of F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getFormantStandardDeviation, U"FormantModeler: Get formant standard deviation", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getStandardDeviation (me, formantNumber);
	QUERY_ONE_FOR_REAL_END (U" Hz (standard deviation of F", formantNumber, U")")
}

DIRECT (INFO_ONE__FormantModeler_reportChiSquared) {
	INFO_ONE (FormantModeler)
		MelderInfo_open();
		FormantModeler_reportChiSquared (me);
		MelderInfo_close();
	INFO_ONE_END
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getDegreesOfFreedom, U"FormantModeler: Get degrees of freedom", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getDegreesOfFreedom (me, formantNumber);
	QUERY_ONE_FOR_REAL_END (U" (degrees of freedom of F", formantNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getStress, U"FormantModeler: Get stress", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	INTEGER (order, U"Order of polynomials", U"3")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getStress (me, fromFormant, toFormant, order, power);
	QUERY_ONE_FOR_REAL_END (U" (roughness)")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getAverageDistanceBetweenTracks, U"FormantModeler: Get average distance between tracks", nullptr) {
	NATURAL (track1, U"Track 1", U"2")
	NATURAL (track2, U"Track 2", U"3")
	OPTIONMENU (typeOfData, U"Type of data", 1)
		OPTION (U"data points")
		OPTION (U"modelled")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getAverageDistanceBetweenTracks (me, track1, track2, typeOfData - 1);
	QUERY_ONE_FOR_REAL_END (U" (average |F", track1, U" - F", track2, U"|)")
}

FORM (QUERY_ONE_FOR_REAL__FormantModeler_getFormantsConstraintsFactor, U"FormantModeler: Get formants constraints factor", nullptr) {
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1500.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantModeler)
		const double result = FormantModeler_getFormantsConstraintsFactor (me, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3);
	QUERY_ONE_FOR_REAL_END (U" (formants constraints factor)");
}

FORM (MODIFY_EACH__FormantModeler_setFormantWeighing, U"FormantModeler: Set data weighing", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh data", kFormantModelerWeights::DEFAULT)
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataWeighing (me, fromFormant, toFormant, weighFormants);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FormantModeler_setTolerance, U"FormantModeler: Set tolerance", nullptr) {
	REAL (tolerance, U"Tolerance", U"1e-5")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setTolerance (me, tolerance);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FormantModeler_setParameterValueFixed, U"FormantModeler: Set parameter value fixed", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	REAL (parameterValue, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParameterValueFixed (me, formantNumber, parameterNumber, parameterValue);
	MODIFY_EACH_END
}


FORM (MODIFY_EACH__FormantModeler_setParameterFree, U"FormantModeler: Set parameter free", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	INTEGER (fromParameter, U"left Parameter range", U"0")
	INTEGER (toParameter, U"right Parameter range", U"0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParametersFree (me, fromFormant, toFormant, fromParameter, toParameter);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FormantModeler_setParameterValuesToZero, U"FormantModeler: Set parameter values to zero", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParameterValuesToZero (me, fromFormant,  toFormant, numberOfSigmas);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FormantModeler_setDataPointValue, U"FormantModeler: Set data point value", nullptr) {
	NATURAL (formantNumber, U"Formant index", U"1")
	NATURAL (dataNumber, U"Data index", U"1")
	REAL (value, U"Value", U"1.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointValue (me, formantNumber,  dataNumber, value);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FormantModeler_setDataPointSigma, U"FormantModeler: Set data point sigma", nullptr) {
	NATURAL (formantNumber, U"Formant index", U"1")
	NATURAL (dataNumber, U"Data index", U"1")
	REAL (sigma, U"Sigma", U"10.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointSigma (me, formantNumber,  dataNumber, sigma);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FormantModeler_setDataPointStatus, U"FormantModeler: Set data point status", nullptr) {
	NATURAL (formantNumber, U"Formant index", U"1")
	NATURAL (dataNumber, U"Data index", U"1")
	OPTIONMENU_ENUM (kDataModelerData, status, U"Status", kDataModelerData::DEFAULT)
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointStatus (me, formantNumber,  dataNumber, status);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__FormantModeler_fitModel) {
	MODIFY_EACH (FormantModeler)
		FormantModeler_fit (me);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__FormantModeler_to_Covariance_parameters, U"", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantModeler)
		autoCovariance result = FormantModeler_to_Covariance_parameters (me, formantNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", formantNumber);
}

FORM (CONVERT_EACH_TO_ONE__FormantModeler_extractDataModeler, U"FormantModeler: Extract DataModeler", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantModeler)
		autoDataModeler result = FormantModeler_extractDataModeler (me, formantNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", formantNumber)
}

DIRECT (CONVERT_EACH_TO_ONE__FormantModeler_to_Table_zscores) {
	CONVERT_EACH_TO_ONE (FormantModeler)
		autoTable result = FormantModeler_to_Table_zscores (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_z")
}

FORM (CONVERT_EACH_TO_ONE__FormantModeler_to_FormantModeler_processOutliers, U"", nullptr) {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"3.0")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantModeler)
		autoFormantModeler result = FormantModeler_processOutliers (me, numberOfSigmas);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_outliers");
}

DIRECT (EDITOR_ONE_WITH_ONE_OptimalCeilingTier_edit) {
	EDITOR_ONE_WITH_ONE (an,OptimalCeilingTier, Sound)   // Sound may be null
		autoOptimalCeilingTierEditor editor = OptimalCeilingTierEditor_create (ID_AND_FULL_NAME, me, you);
	EDITOR_ONE_WITH_ONE_END
}

/*************************** PitchModeler *************************************/
/*
FORM (CONVERT_EACH_TO_ONE__Pitch_to_PitchModeler, U"Pitch: To PitchModeler", nullptr) {
	REAL (fromTime, U"left Start time (s)", U"0.0")
	REAL (toTime, U"right End time (s)", U"0.1")
	INTEGER (order, U"Order of polynomials", U"2")
	OK
DO
	CONVERT_EACH_TO_ONE (Pitch)
		autoPitchModeler result = Pitch_to_PitchModeler (me, fromTime, toTime, order + 1);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (GRAPHICS_EACH__PitchModeler_draw, U"PitchModeler: Draw", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"500.0")
	INTEGER (order, U"Order of polynomial for estimation", U"2")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PitchModeler)
		PitchModeler_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, order + 1,  garnish);
	GRAPHICS_EACH_END
}
*/

FORM (QUERY_ONE_FOR_REAL__Sound_getOptimalFormantCeiling, U"Sound: Get optimal formant ceiling", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.1")
	REAL (toTime, U"right Time range (s)", U"0.15")
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.0025")
	POSITIVE (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getOptimalFormantCeiling (
			me, fromTime, toTime, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, 
			preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power
		);
	QUERY_ONE_FOR_REAL_END (U" Hz");
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_interval, U"Sound: To Formant (interval)", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.1")
	REAL (toTime, U"right Time range (s)", U"0.15")
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.0025")
	POSITIVE (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval (
			me, fromTime, toTime, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps,
			preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas,
			power, 0, 1, 1, 1, 1, 1, & ceiling
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_fixed (ceiling, 0))
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_interval_constrained, U"Sound: To Formant (interval, constrained)", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.1")
	REAL (toTime, U"right Time range (s)", U"0.15")
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.0025")
	POSITIVE (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	COMMENT (U"Formant frequency constraints")
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval (
			me, fromTime, toTime, windowLength, timeStep, fromFrequency,  toFrequency, numberOfFrequencySteps,
			preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power, 1,
			minimumF1, maximumF1, minimumF2, maximumF2, minimumF3, & ceiling
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_fixed (ceiling, 0));
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_interval_constrained_robust, U"Sound: To Formant (interval, constrained, robust)", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.1")
	REAL (toTime, U"right Time range (s)", U"0.15")
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.0025")
	POSITIVE (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	COMMENT (U"Formant frequency constraints")
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval_robust (
			me, fromTime, toTime, windowLength, timeStep, fromFrequency, fromFrequency, numberOfFrequencySteps, 
			preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power, 1,
			minimumF1, maximumF1, minimumF2, minimumF2, minimumF3, & ceiling
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_fixed (ceiling, 0))
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_OptimalCeilingTier, U"", nullptr) {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.0025")
	POSITIVE (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	REAL (smoothingWindow_s, U"Formant smoothing window (s)", U"0.05")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGER (order, U"Order of polynomials", U"2")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	COMMENT (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoOptimalCeilingTier result = Sound_to_OptimalCeilingTier (
			me, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, 
			preEmphasisFrequency, smoothingWindow_s, numberOfFormantTracks, order + 1, 
			weighFormants, numberOfSigmas, power
		);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

FORM (CONVERT_EACH_TO_ONE__Table_to_DataModeler, U"", nullptr) {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0 (= auto)")
	WORD (xColumnName, U"Column with X data", U"")
	WORD (yColumnName, U"Column with Y data", U"")
	WORD (sigmaColumnName, U"Column with sigmas", U"")
	OPTIONMENU_ENUM (kDataModelerFunction, type, U"Basis functions", kDataModelerFunction::DEFAULT)		
	INTEGER (maximumOrder, U"Maximum order", U"3")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		Melder_require (type != kDataModelerFunction::LINEAR,
			U"No linear functions implemented. Choose another model.");
		const integer xColumnNumber     = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber     = Table_columnNameToNumber_e (me, yColumnName);
		const integer sigmaColumnNumber = Table_columnNameToNumber_0 (me, sigmaColumnName);
		autoDataModeler result = Table_to_DataModeler (me, xmin, xmax,
				xColumnNumber, yColumnNumber, sigmaColumnNumber, maximumOrder + 1, type);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

void praat_DataModeler_init ();
void praat_DataModeler_init () {
	Thing_recognizeClassesByName (classDataModeler, classFormantModeler, classOptimalCeilingTier, classOptimalCeilingTierEditor, nullptr);

	structOptimalCeilingTierArea :: f_preferences ();

	praat_addMenuCommand (U"Objects", U"New", U"Create simple DataModeler...", U"Create ISpline...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CREATE_ONE__DataModeler_createSimple);

	praat_addAction1 (classDataModeler, 0, U"Speckle...", 0, 0, 
			GRAPHICS_EACH__DataModeler_speckle);
	praat_addAction1 (classDataModeler, 0, U"Draw model...", 0, 0, 
			GRAPHICS_EACH__DataModeler_drawModel);
	praat_addAction1 (classDataModeler, 0, U"Draw estimated track...", 0, 0, 
			GRAPHICS_EACH__DataModeler_drawEstimatedTrack);
	praat_addAction1 (classDataModeler, 0, U"Draw residuals...", 0, 0, 
			GRAPHICS_EACH__DataModeler_drawResiduals);

	praat_addAction1 (classDataModeler, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, U"Get number of parameters", 0, 1,
				QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfParameters);
		praat_addAction1 (classDataModeler, 0, U"Get number of fixed parameters", 0, 1,
				QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfFixedParameters);
		praat_addAction1 (classDataModeler, 0, U"Get parameter value...", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getParameterValue);
		praat_addAction1 (classDataModeler, 0, U"List parameter values", 0, 1, 
				QUERY_ONE_FOR_REAL_VECTOR__DataModeler_listParameterValues);
		praat_addAction1 (classDataModeler, 0, U"Get parameter status...", 0, 1, 
				QUERY_ONE_FOR_STRING__DataModeler_getParameterStatus);
		praat_addAction1 (classDataModeler, 0, U"Get parameter standard deviation...", 0, 1,
				QUERY_ONE_FOR_REAL__DataModeler_getParameterStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get variance of parameters...", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getVarianceOfParameters);
		praat_addAction1 (classDataModeler, 1, U"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Get model value at x...", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getModelValueAtX);
		praat_addAction1 (classDataModeler, 0, U"Get number of data points", 0, 1, 
				QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfDataPoints);
		praat_addAction1 (classDataModeler, 0, U"Get number of invalid data points", 0, 1,
				QUERY_ONE_FOR_INTEGER__DataModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classDataModeler, 0, U"Get data point y value...", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getDataPointYValue);
		praat_addAction1 (classDataModeler, 0, U"Get data point x value...", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getDataPointXValue);
		praat_addAction1 (classDataModeler, 0, U"Get data point y sigma...", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getDataPointYSigma);
		praat_addAction1 (classDataModeler, 0, U"Get data point status...", 0, 1, 
				QUERY_ONE_FOR_STRING__DataModeler_getDataPointStatus);
		praat_addAction1 (classDataModeler, 1, U"-- get statistics info --", 0, 1, 0);
		
		praat_addAction1 (classDataModeler, 0, U"Get residual sum of squares", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getResidualSumOfSquares);
		praat_addAction1 (classDataModeler, 0, U"Get residual standard deviation", 0, 1, 
				QUERY_ONE_FOR_REAL__DataModeler_getResidualStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get data standard deviation", 0, 1,
				QUERY_ONE_FOR_REAL__DataModeler_getDataStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get coefficient of determination", 0, 1,
				QUERY_ONE_FOR_REAL__DataModeler_getCoefficientOfDetermination);
		praat_addAction1 (classDataModeler, 0, U"Report chi squared", 0, 1, 
				INFO_ONE__DataModeler_reportChiSquared);
		praat_addAction1 (classDataModeler, 0, U"Get degrees of freedom", 0, 1,
				QUERY_ONE_FOR_REAL__DataModeler_getDegreesOfFreedom);
		praat_addAction1 (classDataModeler, 0, U"Get hessian matrix", 0, 1,
				QUERY_ONE_FOR_MATRIX__DataModeler_getHessian);

		praat_addAction1 (classDataModeler, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, U"Set data weighing...", 0, 1, 
				MODIFY_EACH__DataModeler_setDataWeighing);
		praat_addAction1 (classDataModeler, 0, U"Set tolerance...", 0, 1, 
				MODIFY_EACH__DataModeler_setTolerance);
		praat_addAction1 (classDataModeler, 1, U"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Set parameter name...", 0, 1, 
				MODIFY_EACH__DataModeler_setParameterName);
		praat_addAction1 (classDataModeler, 0, U"Set parameter value...", 0, 1, 
				MODIFY_EACH__DataModeler_setParameterValue);
		praat_addAction1 (classDataModeler, 0, U"Set parameter free...", 0, 1,
				MODIFY_EACH__DataModeler_setParameterFree);
		praat_addAction1 (classDataModeler, 0, U"Set parameter values to zero...", 0, 1, 
				MODIFY_EACH__DataModeler_setParameterValuesToZero);
		praat_addAction1 (classDataModeler, 1, U"-- set data values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Set data point y value...", 0, 1, 
				MODIFY_EACH__DataModeler_setDataPointYValue);
		praat_addAction1 (classDataModeler, 0, U"Set data point x value...", 0, 1,
				MODIFY_EACH__DataModeler_setDataPointXValue);
		praat_addAction1 (classDataModeler, 0, U"Set data point y sigma...", 0, 1,
				MODIFY_EACH__DataModeler_setDataPointYSigma);
		praat_addAction1 (classDataModeler, 0, U"Set data point status...", 0, 1, 
				MODIFY_EACH__DataModeler_setDataPointStatus);
		
	praat_addAction1 (classDataModeler, 0, U"Fit model", 0, 0, 
			MODIFY_EACH__DataModeler_fitModel);
	
	praat_addAction1 (classDataModeler, 0, U"To Covariance (parameters)", 0, 0, 
			CONVERT_EACH_TO_ONE__DataModeler_to_Covariance_parameters);
	praat_addAction1 (classDataModeler, 0, U"To Table (z-scores)", 0, 0, 
			CONVERT_EACH_TO_ONE__DataModeler_to_Table_zscores);

	praat_addAction1 (classFormant, 0, U"To FormantModeler...", U"To LPC...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Formant_to_FormantModeler);
	praat_addAction1 (classFormant, 0, U"Extract smoothest part...", 0, GuiMenu_HIDDEN,
			COMBINE_ALL_TO_ONE__Formants_extractSmoothestPart);
	praat_addAction1 (classFormant, 0, U"Extract smoothest part (constrained)...", 0, GuiMenu_HIDDEN,
			COMBINE_ALL_TO_ONE__Formants_extractSmoothestPart_constrained);

	praat_addAction1 (classFormantModeler, 0, U"Draw -", 0, 0, 0);
	praat_addAction1 (classFormantModeler, 0, U"Speckle...", 0, 1, 
			GRAPHICS_EACH__FormantModeler_speckle);
	praat_addAction1 (classFormantModeler, 0, U"Draw tracks...", 0, 1, 
			GRAPHICS_EACH__FormantModeler_drawTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw estimated tracks...", 0, 1, 
			GRAPHICS_EACH__FormantModeler_drawEstimatedTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw variances of shifted tracks...", 0, GuiMenu_HIDDEN + GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FormantModeler_drawVariancesOfShiftedTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw outliers marked...", 0, 1, 
			GRAPHICS_EACH__FormantModeler_drawOutliersMarked);
	praat_addAction1 (classFormantModeler, 0, U"Draw cumulative chisq scores...", 0, 1,
			GRAPHICS_EACH__FormantModeler_drawCumulativeChisqScores);
	praat_addAction1 (classFormantModeler, 0, U"Normal probability plot...", 0, 1,
			GRAPHICS_EACH__FormantModeler_normalProbabilityPlot);
	praat_addAction1 (classFormantModeler, 0, U"Draw basis function...", 0, 1, 
			GRAPHICS_EACH__FormantModeler_drawBasisFunction);
	
	praat_addAction1 (classFormantModeler, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of tracks", 0, 1,
				QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfTracks);
		praat_addAction1 (classFormantModeler, 1, U"-- get parameter info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of parameters...", 0, 1, 
				QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfParameters);
		praat_addAction1 (classFormantModeler, 0, U"Get number of fixed parameters...", 0, 1,
				QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfFixedParameters);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter value...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getParameterValue);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter status...", 0, 1, 
				QUERY_ONE_FOR_STRING__FormantModeler_getParameterStatus);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter standard deviation...", 0, 1,
				QUERY_ONE_FOR_REAL__FormantModeler_getParameterStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get variance of parameters...", 0, 1,
				QUERY_ONE_FOR_REAL__FormantModeler_getVarianceOfParameters);
		praat_addAction1 (classFormantModeler, 1, U"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of data points", 0, 1, 
				QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfDataPoints);
		praat_addAction1 (classFormantModeler, 0, U"Get number of invalid data points...", 0, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				QUERY_ONE_FOR_INTEGER__FormantModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classFormantModeler, 0, U"Get model value at time...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getModelValueAtTime);
		praat_addAction1 (classFormantModeler, 0, U"Get data point value...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getDataPointValue);
		praat_addAction1 (classFormantModeler, 0, U"Get data point sigma...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, U"Get data point status...", 0, 1, 
				QUERY_ONE_FOR_STRING__FormantModeler_getDataPointStatus);

		praat_addAction1 (classFormantModeler, 1, U"-- get statistics info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get residual sum of squares...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getResidualSumOfSquares);
		praat_addAction1 (classFormantModeler, 0, U"Get formant standard deviation...", 0, 1,
				QUERY_ONE_FOR_REAL__FormantModeler_getFormantStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get coefficient of determination...", 0, 1,
				QUERY_ONE_FOR_REAL__FormantModeler_getCoefficientOfDetermination);
		praat_addAction1 (classFormantModeler, 0, U"Report chi squared", 0, 1, 
				INFO_ONE__FormantModeler_reportChiSquared);
		praat_addAction1 (classFormantModeler, 0, U"Get degrees of freedom...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getDegreesOfFreedom);
		praat_addAction1 (classFormantModeler, 0, U"Get stress...", 0, 1, 
				QUERY_ONE_FOR_REAL__FormantModeler_getStress);
		praat_addAction1 (classFormantModeler, 0, U"Get average distance between tracks...", 0, 1,
				QUERY_ONE_FOR_REAL__FormantModeler_getAverageDistanceBetweenTracks);
		praat_addAction1 (classFormantModeler, 0, U"Get formants constraints factor...", 0, 1,
				QUERY_ONE_FOR_REAL__FormantModeler_getFormantsConstraintsFactor);

	praat_addAction1 (classFormantModeler, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set data weighing...", 0, 1,
				MODIFY_EACH__FormantModeler_setFormantWeighing);
		praat_addAction1 (classFormantModeler, 0, U"Set tolerance...", 0, 1, 
				MODIFY_EACH__FormantModeler_setTolerance);
		praat_addAction1 (classFormantModeler, 1, U"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set parameter value fixed...", 0, 1,
				MODIFY_EACH__FormantModeler_setParameterValueFixed);
		praat_addAction1 (classFormantModeler, 0, U"Set parameter free...", 0, 1, 
				MODIFY_EACH__FormantModeler_setParameterFree);
		praat_addAction1 (classFormantModeler, 0, U"Set parameter values to zero...", 0, 1,
				MODIFY_EACH__FormantModeler_setParameterValuesToZero);
		praat_addAction1 (classFormantModeler, 1, U"-- set data points --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set data point value...", 0, 1, 
				MODIFY_EACH__FormantModeler_setDataPointValue);
		praat_addAction1 (classFormantModeler, 0, U"Set data point sigma...", 0, 1, 
				MODIFY_EACH__FormantModeler_setDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, U"Set data point status...", 0, 1, 
				MODIFY_EACH__FormantModeler_setDataPointStatus);
			
	praat_addAction1 (classFormantModeler, 0, U"Fit model", 0, 0, 
			MODIFY_EACH__FormantModeler_fitModel);
	
	
	praat_addAction1 (classFormantModeler, 0, U"To Covariance (parameters)...", 0, 0,
			CONVERT_EACH_TO_ONE__FormantModeler_to_Covariance_parameters);
	praat_addAction1 (classFormantModeler, 0, U"To Table (z-scores)", 0, 0, 
			CONVERT_EACH_TO_ONE__FormantModeler_to_Table_zscores);
	praat_addAction1 (classFormantModeler, 0, U"To FormantModeler (process outliers)...", 0, 0,
			CONVERT_EACH_TO_ONE__FormantModeler_to_FormantModeler_processOutliers);
	praat_addAction1 (classFormantModeler, 0, U"Extract DataModeler...", 0, 0, 
			CONVERT_EACH_TO_ONE__FormantModeler_extractDataModeler);

	praat_addAction1 (classOptimalCeilingTier, 1, U"View & Edit", 0, GuiMenu_ATTRACTIVE | GuiMenu_NO_API,
			EDITOR_ONE_WITH_ONE_OptimalCeilingTier_edit);
	
	//praat_addAction1 (classPitch, 0, U"To PitchModeler...", U"To PointProcess", GuiMenu_HIDDEN, CONVERT_EACH_TO_ONE__Pitch_to_PitchModeler);

	//praat_addAction1 (classPitchModeler, 0, U"Draw...", 0, 0, GRAPHICS_EACH__PitchModeler_draw);

	praat_addAction1 (classSound, 0, U"Get optimal formant ceiling...", U"Get intensity (dB)", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL__Sound_getOptimalFormantCeiling);
	praat_addAction1 (classSound, 0, U"To Formant (interval)...", U"To Formant (robust)...", GuiMenu_DEPTH_2 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_Formant_interval);
	praat_addAction1 (classSound, 0, U"To Formant (interval, constrained)...", U"To Formant (interval)...", GuiMenu_DEPTH_2 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_Formant_interval_constrained);

	praat_addAction1 (classSound, 0, U"To OptimalCeilingTier...", U"To Formant (interval, constrained)...", GuiMenu_DEPTH_2 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_OptimalCeilingTier);
	
	praat_addAction1 (classSound, 0, U"To Formant (interval, constrained, robust)...", U"To Formant (interval, constrained)...", GuiMenu_DEPTH_2 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_Formant_interval_constrained_robust);
	praat_addAction1 (classTable, 0, U"To DataModeler...", U"To logistic regression...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Table_to_DataModeler);
}

/* End of file praat_DataModeler_init.cpp 1566*/
