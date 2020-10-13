/* praat_DataModeler_init.cpp
 *
 * Copyright (C) 2014-2020 David Weenink
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

#include "praatP.h"
#include "DataModeler.h"
#include "Formant_extensions.h"
#include "FormantModeler.h"
#include "OptimalCeilingTierEditor.h"
#include "Pitch.h"
#include "Table_extensions.h"
#include "TextGrid.h"

#undef iam
#define iam iam_LOOP

/* DataModeler */

FORM (NEW1_DataModeler_createSimple, U"Create simple DataModeler", nullptr) {
	WORD (name, U"Name", U"dm")
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"1.0")
	NATURAL (numberOfDataPoints, U"Number of data points", U"20")
	SENTENCE (parameter_string, U"Parameters", U"0.0 1.0 1.0")
	POSITIVE (standardDeviation, U"Gaussian noise stdev", U"0.2")
	OPTIONMENU_ENUM (kDataModelerFunction, type, U"Basis functions", kDataModelerFunction::DEFAULT)		
	OK
DO
	CREATE_ONE
		autoDataModeler result = DataModeler_createSimple (xmin, xmax, numberOfDataPoints, parameter_string, standardDeviation, type);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_DataModeler_speckle, U"DataModeler: Speckle", nullptr) {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"0.0")
	BOOLEAN (errorBars, U"Draw error bars", 1)
	REAL (barWidth_wc, U"Bar width (wc)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	integer order = 6;
	GRAPHICS_EACH (DataModeler)
		DataModeler_speckle (me, GRAPHICS, xmin, xmax,ymin, ymax, 0, order + 1, errorBars, barWidth_wc, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_DataModeler_drawEstimatedTrack, U"DataModeler: Draw estimated track", nullptr) {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"0.0")
	INTEGER (order, U"Order of polynomials for estimation", U"3")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	Melder_require (order >= 0, U"The order should be at least zero.");
	GRAPHICS_EACH (DataModeler)
		DataModeler_drawTrack (me, GRAPHICS, xmin, xmax, ymin, ymax, 1, order + 1, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_DataModeler_getNumberOfParameters) {
	INTEGER_ONE (DataModeler)
		integer result = my numberOfParameters;
	INTEGER_ONE_END (U" (= number of parameters)")
}

DIRECT (INTEGER_DataModeler_getNumberOfFixedParameters) {
	INTEGER_ONE (DataModeler)
		integer result = DataModeler_getNumberOfFixedParameters (me);
	INTEGER_ONE_END (U" (= number of fixed parameters)")
}

FORM (REAL_DataModeler_getParameterValue, U"DataModeler: Get parameter value", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getParameterValue (me, parameterNumber);
	NUMBER_ONE_END (U" (= parameter[", parameterNumber, U"])")
}

FORM (INFO_DataModeler_getParameterStatus, U"DataModeler: Get parameter status", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	STRING_ONE (DataModeler)
		kDataModelerParameter status = DataModeler_getParameterStatus (me, parameterNumber);
		conststring32 result = ( status == kDataModelerParameter::FREE ? U"Free" :
			status == kDataModelerParameter::FIXED_ ? U"Fixed" : U"Undefined" );
	STRING_ONE_END
}

FORM (REAL_DataModeler_getParameterStandardDeviation, U"DataModeler: Get parameter standard deviation", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getParameterStandardDeviation (me, parameterNumber);
	NUMBER_ONE_END (U" (= parameter[", parameterNumber, U"])")
}

FORM (REAL_DataModeler_getVarianceOfParameters, U"DataModeler: Get variance of parameters", nullptr) {
	NATURAL (fromParameter, U"left Parameter range", U"1")
	INTEGER (toParameter, U"right Parameter range", U"0 (=all)")
	OK
DO
	integer nofp;
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getVarianceOfParameters (me, fromParameter, toParameter, &nofp);
	NUMBER_ONE_END (U" (for ", nofp, U" free parameters)")
}

DIRECT (INTEGER_DataModeler_getNumberOfDataPoints) {
	INTEGER_ONE (DataModeler)
		integer result = my numberOfDataPoints;
	INTEGER_ONE_END (U" (= number of data points)")
}

DIRECT (INTEGER_DataModeler_getNumberOfInvalidDataPoints) {
	INTEGER_ONE (DataModeler)
		integer result = DataModeler_getNumberOfInvalidDataPoints (me);
	INTEGER_ONE_END (U" (= number of invalid data points)")
}

FORM (REAL_DataModeler_getModelValueAtX, U"DataModeler: Get model value at x", nullptr) {
	REAL (x, U"X", U"0.1")
	OK
DO
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getModelValueAtX (me, x);
	NUMBER_ONE_END (U"")
}



DIRECT (REAL_DataModeler_getResidualSumOfSquares) {
	NUMBER_ONE (DataModeler)
		integer n;
		double result = DataModeler_getResidualSumOfSquares (me, &n);
	NUMBER_ONE_END (U"  (for ", n, U" datapoints)")
}

DIRECT (REAL_DataModeler_getDataStandardDeviation) {
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getDataStandardDeviation (me);
	NUMBER_ONE_END (U"")
}

FORM (REAL_DataModeler_getDataPointXValue, U"DataModeler: Get data point x value", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getDataPointXValue (me, index);
	NUMBER_ONE_END (U" (= value at point ", index, U")")
}

FORM (REAL_DataModeler_getDataPointYValue, U"DataModeler: Get data point y value", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getDataPointYValue (me, index);
	NUMBER_ONE_END (U" (= value at point ", index, U")")
}

FORM (REAL_DataModeler_getDataPointYSigma, U"DataModeler: Get data point y sigma", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getDataPointYSigma (me, index);
	NUMBER_ONE_END (U" (= sigma at point ", index, U")")
}

FORM (INFO_DataModeler_getDataPointStatus, U"DataModeler: Get data point status", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	STRING_ONE (DataModeler)
		kDataModelerData status = DataModeler_getDataPointStatus (me, index);
		conststring32 result = ( status == kDataModelerData::INVALID ? U"Invalid" : U"Valid" );
	STRING_ONE_END
}

DIRECT (REAL_DataModeler_getCoefficientOfDetermination) {
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getCoefficientOfDetermination (me, nullptr, nullptr);
	NUMBER_ONE_END (U" (= R^2)");
}


DIRECT (INFO_DataModeler_reportChiSquared) {
	INFO_ONE (DataModeler)
		MelderInfo_open();
		DataModeler_reportChiSquared (me);
		MelderInfo_close();
	INFO_ONE_END
}

DIRECT (REAL_DataModeler_getDegreesOfFreedom) {
	NUMBER_ONE (DataModeler)
		double result = DataModeler_getDegreesOfFreedom (me);
	NUMBER_ONE_END (U" (= degrees of freedom)")
}


FORM (MODIFY_DataModeler_setDataWeighing, U"DataModeler: Set data weighing", nullptr) {
	OPTIONMENU_ENUM (kDataModelerWeights, weighDataType, U"Weigh data", kDataModelerWeights::DEFAULT)
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataWeighing (me, weighDataType);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setTolerance, U"DataModeler: Set tolerance", nullptr) {
	REAL (tolerance, U"Tolerance", U"1e-5")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setTolerance (me, tolerance);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setParameterValue, U"DataModeler: Set parameter value", nullptr) {
	NATURAL (parameterNumber, U"Parameter number", U"1")
	REAL (value, U"Value", U"0.0")
	OPTIONMENU_ENUM (kDataModelerParameter, parameterStatus, U"Status", kDataModelerParameter::DEFAULT)
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterValue (me, parameterNumber, value, parameterStatus);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setParameterFree, U"DataModeler: Set parameter free", nullptr) {
	INTEGER (fromParameter, U"left Parameter range", U"0")
	INTEGER (toParameter, U"right Parameter range", U"0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParametersFree (me, fromParameter, toParameter);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setParameterValuesToZero, U"DataModeler: Set parameter values to zero", nullptr) {
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterValuesToZero (me, numberOfSigmas);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointStatus, U"DataModeler: Set data point status", nullptr) {
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

FORM (MODIFY_DataModeler_setDataPointXValue, U"DataModeler: Set data point x value", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (x, U"X", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointXValue (me, index, x);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointYValue, U"DataModeler: Set data point y value", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointYValue (me, index, y);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointValues, U"DataModeler: Set data point values", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointValues (me, index, x, y);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointYSigma, U"DataModeler: Set data point y sigma", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (sigma, U"Sigma", U"10.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointYSigma (me, index, sigma);
	MODIFY_EACH_END
}

DIRECT (MODIFY_DataModeler_fitModel) {
	MODIFY_EACH (DataModeler)
		DataModeler_fit (me);
	MODIFY_EACH_END
}

DIRECT (NEW_DataModeler_to_Covariance_parameters) {
	CONVERT_EACH (DataModeler)
		autoCovariance result = DataModeler_to_Covariance_parameters (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_DataModeler_to_Table_zscores) {
	CONVERT_EACH (DataModeler)
		autoTable result = DataModeler_to_Table_zscores (me);
	CONVERT_EACH_END (my name.get(), U"_z");
}

FORM (NEW_Formant_to_FormantModeler, U"Formant: To FormantModeler", nullptr) {
//double tmin, double tmax, integer numberOfFormants, integer numberOfParametersPerTrack
	REAL (fromTime, U"left Start time", U"0.0")
	REAL (toTime, U"right End time", U"0.1")
	NATURAL (numberOfFormants, U"Number of formants", U"3")
	INTEGER (order, U"Order of polynomials", U"3")
	OK
DO
	Melder_require (order >= 0, U"The order should be at least zero.");
	CONVERT_EACH (Formant)
		autoFormantModeler result = Formant_to_FormantModeler (me, fromTime, toTime, numberOfFormants, order + 1);
	CONVERT_EACH_END (my name.get(), U"_o", order);
}

FORM (NEW1_Formants_extractSmoothestPart, U"Formants: Extract smoothest part", U"Formants: Extract smoothest part") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	LABEL (U"Use bandwidths to model the formant tracks:")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	LABEL (U"Zero parameter values whose range include zero:")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	OrderedOf<structFormant> formants;
	LOOP {
		iam (Formant);
		formants. addItem_ref (me);
	}
	integer index = Formants_getSmoothestInInterval (& formants, fromTime, toTime, numberOfFormantTracks, order + 1, weighFormants, 0, numberOfSigmas, power, 1.0, 1.0, 1.0, 1.0, 1.0);
	// next code is necessary to get the Formant at postion index selected and to get its name
	integer iselected = 0;
	Formant him = nullptr;
	LOOP {
		iselected ++;
		if (iselected == index) {
			him = static_cast<Formant> (OBJECT);
		}
	}
	Melder_assert (him);
	autoFormant result = Formant_extractPart (him, fromTime, toTime);
	praat_new (result.move(), his name.get(), U"_part");
END }

FORM (NEW1_Formants_extractSmoothestPart_constrained, U"Formants: Extract smoothest part (constrained)", U"Formants: Extract smoothest part (constrained)...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	NATURAL (numberOfFormantTracks, U"Number of formant tracks", U"4")
	INTEGER (order, U"Order of polynomials", U"3")
	LABEL (U"Use bandwidths to model the formant tracks:")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh formants", kFormantModelerWeights::DEFAULT)
	LABEL (U"Zero parameter values whose range include zero:")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	LABEL (U"The constraints on the formants:")
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1500.0")
	OK
DO
	OrderedOf<structFormant> formants;
	LOOP {
		iam (Formant);
		formants. addItem_ref (me);
	}
	integer index = Formants_getSmoothestInInterval (& formants, fromTime, toTime, numberOfFormantTracks, order + 1, weighFormants, 1, numberOfSigmas, power, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3);
	// next code is necessary to get the Formant at postion index selected and to get its name
	integer iselected = 0;
	Formant him = nullptr;
	LOOP {
		iselected ++;
		if (iselected == index) {
			him = static_cast<Formant> (OBJECT);
		}
	}
	Melder_assert (him);
	autoFormant result = Formant_extractPart (him, fromTime, toTime);
	praat_new (result.move(), his name.get(), U"_part");
END }

/********************** FormantModeler ******************************/

FORM (GRAPHICS_FormantModeler_drawEstimatedTracks, U"FormantModeler: Draw estimated tracks", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"3")
	INTEGER (order, U"Order of polynomials for estimation", U"3")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	Melder_require (order >= 0, U"The order should be at least zero.");
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, true, order + 1, Melder_BLACK, Melder_BLACK, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawTracks, U"FormantModeler: Draw tracks", nullptr) {
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
		FormantModeler_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, false, order + 1, Melder_BLACK, Melder_BLACK, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_speckle, U"FormantModeler: Speckle", nullptr) {
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
		FormantModeler_speckle (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, 0, order + 1, errorBars, Melder_BLACK, Melder_BLACK, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawOutliersMarked, U"FormantModeler: Draw outliers marked", nullptr) {
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
		FormantModeler_drawOutliersMarked (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, numberOfSigmas, mark_string, fontSize, Melder_BLACK, Melder_BLACK, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawVariancesOfShiftedTracks, U"FormantModeler: Draw variances of shifted tracks", nullptr) {
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
		FormantModeler_drawVariancesOfShiftedTracks (me, GRAPHICS, fromTime, toTime, fromVariance, toVariance, shiftTracks, fromFormant, toFormant, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawCumulativeChisqScores, U"FormantModeler: Draw cumulative chi scores", nullptr) {
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


FORM (GRAPHICS_FormantModeler_normalProbabilityPlot, U"FormantModeler: Normal probability plot", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (numberOfSigmas, U"Number of sigmas", U"0.0")
	POSITIVE (fontSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_normalProbabilityPlot (me, GRAPHICS, formantNumber, numberOfQuantiles, numberOfSigmas, fontSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawBasisFunction, U"FormantModeler: Draw basis function", nullptr) {
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
		FormantModeler_drawBasisFunction (me, GRAPHICS, fromTime, toTime, ymin, ymax, formantNumber, basisFunctionIndex, scale, numberOfPoints, garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_FormantModeler_getModelValueAtTime, U"", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getModelValueAtTime (me, formantNumber, time);
	NUMBER_ONE_END (U"Hertz")
}


FORM (REAL_FormantModeler_getDataPointValue, U"FormantModeler: Get data point value", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getDataPointValue (me, formantNumber, index);
	NUMBER_ONE_END (U" (= value of point ", index, U" in track F", formantNumber, U")")
}

FORM (REAL_FormantModeler_getDataPointSigma, U"FormantModeler: Get data point sigma", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getDataPointSigma (me, formantNumber, index);
	NUMBER_ONE_END (U" (= sigma of point ", index, U" in track F", formantNumber, U")")
}


FORM (INFO_FormantModeler_getDataPointStatus, U"FormantModeler: Get data point status", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		kDataModelerData status = FormantModeler_getDataPointStatus (me, formantNumber, index);
		conststring32 result = ( status == kDataModelerData::INVALID ? U"Invalid" : U"Valid" );
	INTEGER_ONE_END (U"")
}

DIRECT (INTEGER_FormantModeler_getNumberOfTracks) {
	INTEGER_ONE (FormantModeler)
		integer result = FormantModeler_getNumberOfTracks (me);
	INTEGER_ONE_END (U" (= number of formants)")
}


FORM (INTEGER_FormantModeler_getNumberOfParameters, U"FormantModeler: Get number of parameters", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		integer result = FormantModeler_getNumberOfParameters (me, formantNumber);
	INTEGER_ONE_END (U" (= number of parameters for F", formantNumber, U")")
}


FORM (INTEGER_FormantModeler_getNumberOfFixedParameters, U"FormantModeler: Get number of fixed parameters", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		integer result = FormantModeler_getNumberOfFixedParameters (me, formantNumber);
	INTEGER_ONE_END (U" (= number of fixed parameters for F", formantNumber, U")")
}

DIRECT (INTEGER_FormantModeler_getNumberOfDataPoints) {
	INTEGER_ONE (FormantModeler)
		integer result = FormantModeler_getNumberOfDataPoints (me);
	INTEGER_ONE_END (U"")
}

FORM (INTEGER_FormantModeler_getNumberOfInvalidDataPoints, U"FormantModeler: Get number of invalid data points", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		integer result = FormantModeler_getNumberOfInvalidDataPoints (me, formantNumber);
	INTEGER_ONE_END (U" (= number of invalid data points for F", formantNumber, U")")
}

FORM (REAL_FormantModeler_getParameterValue, U"FormantModeler: Get parameter value", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getParameterValue (me, formantNumber, parameterNumber);
	NUMBER_ONE_END (U" (= parameter ", parameterNumber, U" for F", formantNumber, U")")
}

FORM (INFO_FormantModeler_getParameterStatus, U"FormantModeler: Get parameter status", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	STRING_ONE (FormantModeler)
		kDataModelerParameter status = FormantModeler_getParameterStatus (me, formantNumber, parameterNumber);
		conststring32 result = Melder_cat (
			status == kDataModelerParameter::FREE ? U"Free" : status == kDataModelerParameter::FIXED_ ? U"Fixed" : U"Undefined",
			U" (= status of parameter ", parameterNumber, U" for F", formantNumber, U")"
		);
	STRING_ONE_END
}

FORM (REAL_FormantModeler_getParameterStandardDeviation, U"FormantModeler: Get parameter standard deviatio", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getParameterStandardDeviation (me, formantNumber, parameterNumber);
	NUMBER_ONE_END (U" (= standard deviation of parameter ", parameterNumber, U" for F", formantNumber, U")")
}


FORM (REAL_FormantModeler_getVarianceOfParameters, U"FormantModeler: Get variance of parameters", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	INTEGER (fromParameter, U"left Parameter range", U"0")
	INTEGER (toParameter, U"right Parameter range", U"0")
	OK
DO
	integer numberOfFreeParameters;
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getVarianceOfParameters (me, fromFormant, toFormant, fromParameter, toParameter, & numberOfFreeParameters);
	NUMBER_ONE_END (U" (for ", numberOfFreeParameters, U" free parameters.)")
}


FORM (REAL_FormantModeler_getCoefficientOfDetermination, U"FormantModeler: Get coefficient of determination", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getCoefficientOfDetermination (me, fromFormant, toFormant);
	NUMBER_ONE_END (U" (= R^2)");
}


FORM (REAL_FormantModeler_getResidualSumOfSquares, U"FormantModeler: Get residual sum of squares", U"FormantModeler: Get residual sum of squares...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getResidualSumOfSquares (me, formantNumber, nullptr);
	NUMBER_ONE_END (U" Hz^2,  (= residual sum of squares of F", formantNumber, U")")
}

FORM (REAL_FormantModeler_getFormantStandardDeviation, U"FormantModeler: Get formant standard deviation", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getStandardDeviation (me, formantNumber);
	NUMBER_ONE_END (U" Hz (= standard deviation of F", formantNumber, U")")
}

DIRECT (INFO_FormantModeler_reportChiSquared) {
	INFO_ONE (FormantModeler)
		MelderInfo_open();
		FormantModeler_reportChiSquared (me);
		MelderInfo_close();
	INFO_ONE_END
}

FORM (REAL_FormantModeler_getDegreesOfFreedom, U"FormantModeler: Get degrees of freedom", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getDegreesOfFreedom (me, formantNumber);
	NUMBER_ONE_END (U" (= degrees of freedom of F", formantNumber, U")")
}

FORM (REAL_FormantModeler_getStress, U"FormantModeler: Get stress", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	INTEGER (order, U"Order of polynomials", U"3")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getStress (me, fromFormant, toFormant, order, power);
	NUMBER_ONE_END (U" (= roughness)")
}

FORM (REAL_FormantModeler_getAverageDistanceBetweenTracks, U"FormantModeler: Get average distance between tracks", nullptr) {
	NATURAL (track1, U"Track 1", U"2")
	NATURAL (track2, U"Track 2", U"3")
	OPTIONMENU (typeOfData, U"Type of data", 1)
		OPTION (U"data points")
		OPTION (U"modelled")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getAverageDistanceBetweenTracks (me, track1, track2, typeOfData - 1);
	NUMBER_ONE_END (U" (= average |F", track1, U" - F", track2, U"|)")
}

FORM (REAL_FormantModeler_getFormantsConstraintsFactor, U"FormantModeler: Get formants constraints factor", nullptr) {
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1500.0")
	OK
DO
	NUMBER_ONE (FormantModeler)
		double result = FormantModeler_getFormantsConstraintsFactor (me, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3);
	NUMBER_ONE_END (U" (= formants constraints factor)");
}

FORM (MODIFY_FormantModeler_setFormantWeighing, U"FormantModeler: Set data weighing", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	OPTIONMENU_ENUM (kFormantModelerWeights, weighFormants, U"Weigh data", kFormantModelerWeights::DEFAULT)
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataWeighing (me, fromFormant, toFormant, weighFormants);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setTolerance, U"FormantModeler: Set tolerance", nullptr) {
	REAL (tolerance, U"Tolerance", U"1e-5")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setTolerance (me, tolerance);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setParameterValueFixed, U"FormantModeler: Set parameter value fixed", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	NATURAL (parameterNumber, U"Parameter number", U"1")
	REAL (parameterValue, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParameterValueFixed (me, formantNumber, parameterNumber, parameterValue);
	MODIFY_EACH_END
}


FORM (MODIFY_FormantModeler_setParameterFree, U"FormantModeler: Set parameter free", nullptr) {
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

FORM (MODIFY_FormantModeler_setParameterValuesToZero, U"FormantModeler: Set parameter values to zero", nullptr) {
	INTEGER (fromFormant, U"left Formant range", U"0")
	INTEGER (toFormant, U"right Formant range", U"0")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParameterValuesToZero (me, fromFormant,  toFormant, numberOfSigmas);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setDataPointValue, U"FormantModeler: Set data point value", nullptr) {
	NATURAL (formantNumber, U"Formant index", U"1")
	NATURAL (dataNumber, U"Data index", U"1")
	REAL (value, U"Value", U"1.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointValue (me, formantNumber,  dataNumber, value);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setDataPointSigma, U"FormantModeler: Set data point sigma", nullptr) {
	NATURAL (formantNumber, U"Formant index", U"1")
	NATURAL (dataNumber, U"Data index", U"1")
	REAL (sigma, U"Sigma", U"10.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointSigma (me, formantNumber,  dataNumber, sigma);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setDataPointStatus, U"FormantModeler: Set data point status", nullptr) {
	NATURAL (formantNumber, U"Formant index", U"1")
	NATURAL (dataNumber, U"Data index", U"1")
	OPTIONMENU_ENUM (kDataModelerData, status, U"Status", kDataModelerData::DEFAULT)
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointStatus (me, formantNumber,  dataNumber, status);
	MODIFY_EACH_END
}

DIRECT (MODIFY_FormantModeler_fitModel) {
	MODIFY_EACH (FormantModeler)
		FormantModeler_fit (me);
	MODIFY_EACH_END
}

FORM (NEW_FormantModeler_to_Covariance_parameters, U"", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoCovariance result = FormantModeler_to_Covariance_parameters (me, formantNumber);
	CONVERT_EACH_END (my name.get(), U"_", formantNumber);
}

FORM (NEW_FormantModeler_extractDataModeler, U"FormantModeler: Extract DataModeler", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoDataModeler result = FormantModeler_extractDataModeler (me, formantNumber);
	CONVERT_EACH_END (my name.get(), U"_", formantNumber)
}

DIRECT (NEW_FormantModeler_to_Table_zscores) {
	CONVERT_EACH (FormantModeler)
		autoTable result = FormantModeler_to_Table_zscores (me);
	CONVERT_EACH_END (my name.get(), U"_z")
}

FORM (NEW_FormantModeler_to_FormantModeler_processOutliers, U"", nullptr) {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"3.0")
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoFormantModeler result = FormantModeler_processOutliers (me, numberOfSigmas);
	CONVERT_EACH_END (my name.get(), U"_outliers");
}


DIRECT (WINDOW_OptimalCeilingTier_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an OptimalCeilingTier from batch.");
	FIND_TWO_WITH_IOBJECT (OptimalCeilingTier, Sound)   // Sound may be null
		autoOptimalCeilingTierEditor editor = OptimalCeilingTierEditor_create (ID_AND_FULL_NAME, me, you, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}


/*************************** PitchModeler *************************************/
/*
FORM (NEW_Pitch_to_PitchModeler, U"Pitch: To PitchModeler", nullptr) {
	REAL (fromTime, U"left Start time (s)", U"0.0")
	REAL (toTime, U"right End time (s)", U"0.1")
	INTEGER (order, U"Order of polynomials", U"2")
	OK
DO
	CONVERT_EACH (Pitch)
		autoPitchModeler result = Pitch_to_PitchModeler (me, fromTime, toTime, order + 1);
	CONVERT_EACH_END (my name.get())
}

FORM (GRAPHICS_PitchModeler_draw, U"PitchModeler: Draw", nullptr) {
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

FORM (REAL_Sound_getOptimalFormantCeiling, U"Sound: Get optimal formant ceiling", nullptr) {
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
	LABEL (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	NUMBER_ONE (Sound)
		double result = Sound_getOptimalFormantCeiling (me, fromTime, toTime, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power);
	NUMBER_ONE_END (U" Hz");
}

FORM (NEW_Sound_to_Formant_interval, U"Sound: To Formant (interval)", nullptr) {
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
	LABEL (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	CONVERT_EACH (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval (me, fromTime, toTime, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power, 0, 1, 1, 1, 1, 1, &ceiling);
	CONVERT_EACH_END (my name.get(), U"_", Melder_fixed (ceiling, 0))
}

FORM (NEW_Sound_to_Formant_interval_constrained, U"Sound: To Formant (interval, constrained)", nullptr) {
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
	LABEL (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	LABEL (U"Formant frequency constraints")
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval (me, fromTime, toTime, windowLength, timeStep, fromFrequency,  toFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power, 1, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3, & ceiling);
	CONVERT_EACH_END (my name.get(), U"_", Melder_fixed (ceiling, 0));
}

FORM (NEW_Sound_to_Formant_interval_constrained_robust, U"Sound: To Formant (interval, constrained, robust)", nullptr) {
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
	LABEL (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	LABEL (U"Formant frequency constraints")
	REAL (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REAL (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REAL (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (minimumF3, U"Minimum F3 (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval_robust (me, fromTime, toTime, windowLength, timeStep, fromFrequency, fromFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power, 1, minimumF1, maximumF1, minimumF2, minimumF2, minimumF3, &ceiling);
	CONVERT_EACH_END (my name.get(), U"_", Melder_fixed (ceiling, 0))
}

FORM (NEW_Sound_to_OptimalCeilingTier, U"", nullptr) {
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
	LABEL (U"Make parameters that include zero in their confidence region zero")
	REAL (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (power, U"Parameter variance power", U"1.5")
	OK
DO
	CONVERT_EACH (Sound)
		autoOptimalCeilingTier result = Sound_to_OptimalCeilingTier (me, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, preEmphasisFrequency, smoothingWindow_s, numberOfFormantTracks, order + 1, weighFormants, numberOfSigmas, power);
	CONVERT_EACH_END (my name.get());
}

FORM (NEW_Table_to_DataModeler, U"", nullptr) {
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"0.0 (= auto)")
	WORD (columnWithX_string, U"Column with X data", U"")
	WORD (columnWithY_string, U"Column with Y data", U"")
	WORD (columnEithSigma_string, U"Column with sigmas", U"")
	OPTIONMENU_ENUM (kDataModelerFunction, type, U"Basis functions", kDataModelerFunction::DEFAULT)		
	INTEGER (maximumOrder, U"Maximum order", U"3")
	OK
DO
	CONVERT_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, columnWithX_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, columnWithY_string);
		integer scolumn = Table_findColumnIndexFromColumnLabel (me, columnEithSigma_string);
		autoDataModeler result = Table_to_DataModeler (me, xmin, xmax, xcolumn, ycolumn, scolumn, maximumOrder + 1, type);
	CONVERT_EACH_END (my name.get())
}

void praat_DataModeler_init ();
void praat_DataModeler_init () {
	Thing_recognizeClassesByName (classDataModeler, classFormantModeler, classOptimalCeilingTier, classOptimalCeilingTierEditor, nullptr);
	
	praat_addMenuCommand (U"Objects", U"New", U"Create simple DataModeler...", U"Create ISpline...", praat_HIDDEN + praat_DEPTH_1, NEW1_DataModeler_createSimple);

	praat_addAction1 (classDataModeler, 0, U"Speckle...", 0, 0, GRAPHICS_DataModeler_speckle);
	praat_addAction1 (classDataModeler, 0, U"Draw estimated track...", 0, 0, GRAPHICS_DataModeler_drawEstimatedTrack);

	praat_addAction1 (classDataModeler, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, U"Get number of parameters", 0, 1, INTEGER_DataModeler_getNumberOfParameters);
		praat_addAction1 (classDataModeler, 0, U"Get number of fixed parameters", 0, 1, INTEGER_DataModeler_getNumberOfFixedParameters);
		praat_addAction1 (classDataModeler, 0, U"Get parameter value...", 0, 1, REAL_DataModeler_getParameterValue);
		praat_addAction1 (classDataModeler, 0, U"Get parameter status...", 0, 1, INFO_DataModeler_getParameterStatus);
		praat_addAction1 (classDataModeler, 0, U"Get parameter standard deviation...", 0, 1, REAL_DataModeler_getParameterStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get variance of parameters...", 0, 1, REAL_DataModeler_getVarianceOfParameters);
		praat_addAction1 (classDataModeler, 1, U"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Get model value at x...", 0, 1, REAL_DataModeler_getModelValueAtX);
		praat_addAction1 (classDataModeler, 0, U"Get number of data points", 0, 1, INTEGER_DataModeler_getNumberOfDataPoints);
		praat_addAction1 (classDataModeler, 0, U"Get number of invalid data points", 0, 1, INTEGER_DataModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classDataModeler, 0, U"Get data point y value...", 0, 1, REAL_DataModeler_getDataPointYValue);
		praat_addAction1 (classDataModeler, 0, U"Get data point x value...", 0, 1, REAL_DataModeler_getDataPointXValue);
		praat_addAction1 (classDataModeler, 0, U"Get data point y sigma...", 0, 1, REAL_DataModeler_getDataPointYSigma);
		praat_addAction1 (classDataModeler, 0, U"Get data point status...", 0, 1, INFO_DataModeler_getDataPointStatus);
		praat_addAction1 (classDataModeler, 1, U"-- get statistics info --", 0, 1, 0);
		
		praat_addAction1 (classDataModeler, 0, U"Get residual sum of squares", 0, 1, REAL_DataModeler_getResidualSumOfSquares);
		praat_addAction1 (classDataModeler, 0, U"Get data standard deviation", 0, 1, REAL_DataModeler_getDataStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get coefficient of determination", 0, 1, REAL_DataModeler_getCoefficientOfDetermination);
		praat_addAction1 (classDataModeler, 0, U"Report chi squared", 0, 1, INFO_DataModeler_reportChiSquared);
		praat_addAction1 (classDataModeler, 0, U"Get degrees of freedom", 0, 1, REAL_DataModeler_getDegreesOfFreedom);

		praat_addAction1 (classDataModeler, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, U"Set data weighing...", 0, 1, MODIFY_DataModeler_setDataWeighing);
		praat_addAction1 (classDataModeler, 0, U"Set tolerance...", 0, 1, MODIFY_DataModeler_setTolerance);
		praat_addAction1 (classDataModeler, 1, U"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Set parameter value...", 0, 1, MODIFY_DataModeler_setParameterValue);
		praat_addAction1 (classDataModeler, 0, U"Set parameter free...", 0, 1, MODIFY_DataModeler_setParameterFree);
		praat_addAction1 (classDataModeler, 0, U"Set parameter values to zero...", 0, 1, MODIFY_DataModeler_setParameterValuesToZero);
		praat_addAction1 (classDataModeler, 1, U"-- set data values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Set data point y value...", 0, 1, MODIFY_DataModeler_setDataPointYValue);
		praat_addAction1 (classDataModeler, 0, U"Set data point x value...", 0, 1, MODIFY_DataModeler_setDataPointXValue);
		praat_addAction1 (classDataModeler, 0, U"Set data point y sigma...", 0, 1, MODIFY_DataModeler_setDataPointYSigma);
		praat_addAction1 (classDataModeler, 0, U"Set data point status...", 0, 1, MODIFY_DataModeler_setDataPointStatus);
		
	praat_addAction1 (classDataModeler, 0, U"Fit model", 0, 0, MODIFY_DataModeler_fitModel);
	
	praat_addAction1 (classDataModeler, 0, U"To Covariance (parameters)", 0, 0, NEW_DataModeler_to_Covariance_parameters);
	praat_addAction1 (classDataModeler, 0, U"To Table (z-scores)", 0, 0, NEW_DataModeler_to_Table_zscores);

	praat_addAction1 (classFormant, 0, U"To FormantModeler...", U"To LPC...", praat_HIDDEN, NEW_Formant_to_FormantModeler);
	praat_addAction1 (classFormant, 0, U"Extract smoothest part...", 0, praat_HIDDEN, NEW1_Formants_extractSmoothestPart);
	praat_addAction1 (classFormant, 0, U"Extract smoothest part (constrained)...", 0, praat_HIDDEN, NEW1_Formants_extractSmoothestPart_constrained);

	praat_addAction1 (classFormantModeler, 0, U"Draw -", 0, 0, 0);
	praat_addAction1 (classFormantModeler, 0, U"Speckle...", 0, 1, GRAPHICS_FormantModeler_speckle);
	praat_addAction1 (classFormantModeler, 0, U"Draw tracks...", 0, 1, GRAPHICS_FormantModeler_drawTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw estimated tracks...", 0, 1, GRAPHICS_FormantModeler_drawEstimatedTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw variances of shifted tracks...", 0, 1, GRAPHICS_FormantModeler_drawVariancesOfShiftedTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw outliers marked...", 0, 1, GRAPHICS_FormantModeler_drawOutliersMarked);
	praat_addAction1 (classFormantModeler, 0, U"Draw cumulative chisq scores...", 0, 1, GRAPHICS_FormantModeler_drawCumulativeChisqScores);
	praat_addAction1 (classFormantModeler, 0, U"Normal probability plot...", 0, 1, GRAPHICS_FormantModeler_normalProbabilityPlot);
	praat_addAction1 (classFormantModeler, 0, U"Draw basis function...", 0, 1, GRAPHICS_FormantModeler_drawBasisFunction);
	
	praat_addAction1 (classFormantModeler, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of tracks", 0, 1, INTEGER_FormantModeler_getNumberOfTracks);
		praat_addAction1 (classFormantModeler, 1, U"-- get parameter info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of parameters...", 0, 1, INTEGER_FormantModeler_getNumberOfParameters);
		praat_addAction1 (classFormantModeler, 0, U"Get number of fixed parameters...", 0, 1, INTEGER_FormantModeler_getNumberOfFixedParameters);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter value...", 0, 1, REAL_FormantModeler_getParameterValue);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter status...", 0, 1, INFO_FormantModeler_getParameterStatus);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter standard deviation...", 0, 1, REAL_FormantModeler_getParameterStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get variance of parameters...", 0, 1, REAL_FormantModeler_getVarianceOfParameters);
		praat_addAction1 (classFormantModeler, 1, U"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of data points", 0, 1, INTEGER_FormantModeler_getNumberOfDataPoints);
		praat_addAction1 (classFormantModeler, 0, U"Get number of invalid data points...", 0, praat_DEPTH_1 + praat_HIDDEN, INTEGER_FormantModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classFormantModeler, 0, U"Get model value at time...", 0, 1, REAL_FormantModeler_getModelValueAtTime);
		praat_addAction1 (classFormantModeler, 0, U"Get data point value...", 0, 1, REAL_FormantModeler_getDataPointValue);
		praat_addAction1 (classFormantModeler, 0, U"Get data point sigma...", 0, 1, REAL_FormantModeler_getDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, U"Get data point status...", 0, 1, INFO_FormantModeler_getDataPointStatus);

		praat_addAction1 (classFormantModeler, 1, U"-- get statistics info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get residual sum of squares...", 0, 1, REAL_FormantModeler_getResidualSumOfSquares);
		praat_addAction1 (classFormantModeler, 0, U"Get formant standard deviation...", 0, 1, REAL_FormantModeler_getFormantStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get coefficient of determination...", 0, 1, REAL_FormantModeler_getCoefficientOfDetermination);
		praat_addAction1 (classFormantModeler, 0, U"Report chi squared", 0, 1, INFO_FormantModeler_reportChiSquared);
		praat_addAction1 (classFormantModeler, 0, U"Get degrees of freedom...", 0, 1, REAL_FormantModeler_getDegreesOfFreedom);
		praat_addAction1 (classFormantModeler, 0, U"Get stress...", 0, 1, REAL_FormantModeler_getStress);
		praat_addAction1 (classFormantModeler, 0, U"Get average distance between tracks...", 0, 1, REAL_FormantModeler_getAverageDistanceBetweenTracks);
		praat_addAction1 (classFormantModeler, 0, U"Get formants constraints factor...", 0, 1, REAL_FormantModeler_getFormantsConstraintsFactor);

	praat_addAction1 (classFormantModeler, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set data weighing...", 0, 1, MODIFY_FormantModeler_setFormantWeighing);
		praat_addAction1 (classFormantModeler, 0, U"Set tolerance...", 0, 1, MODIFY_FormantModeler_setTolerance);
		praat_addAction1 (classFormantModeler, 1, U"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set parameter value fixed...", 0, 1, MODIFY_FormantModeler_setParameterValueFixed);
		praat_addAction1 (classFormantModeler, 0, U"Set parameter free...", 0, 1, MODIFY_FormantModeler_setParameterFree);
		praat_addAction1 (classFormantModeler, 0, U"Set parameter values to zero...", 0, 1, MODIFY_FormantModeler_setParameterValuesToZero);
		praat_addAction1 (classFormantModeler, 1, U"-- set data points --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set data point value...", 0, 1, MODIFY_FormantModeler_setDataPointValue);
		praat_addAction1 (classFormantModeler, 0, U"Set data point sigma...", 0, 1, MODIFY_FormantModeler_setDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, U"Set data point status...", 0, 1, MODIFY_FormantModeler_setDataPointStatus);
			
	praat_addAction1 (classFormantModeler, 0, U"Fit model", 0, 0, MODIFY_FormantModeler_fitModel);
	
	
	praat_addAction1 (classFormantModeler, 0, U"To Covariance (parameters)...", 0, 0, NEW_FormantModeler_to_Covariance_parameters);
	praat_addAction1 (classFormantModeler, 0, U"To Table (z-scores)", 0, 0, NEW_FormantModeler_to_Table_zscores);
	praat_addAction1 (classFormantModeler, 0, U"To FormantModeler (process outliers)...", 0, 0, NEW_FormantModeler_to_FormantModeler_processOutliers);
	praat_addAction1 (classFormantModeler, 0, U"Extract DataModeler...", 0, 0, NEW_FormantModeler_extractDataModeler);

	praat_addAction1 (classOptimalCeilingTier, 1, U"View & Edit", 0, praat_ATTRACTIVE | praat_NO_API, WINDOW_OptimalCeilingTier_edit);
	
	//praat_addAction1 (classPitch, 0, U"To PitchModeler...", U"To PointProcess", praat_HIDDEN, NEW_Pitch_to_PitchModeler);

	//praat_addAction1 (classPitchModeler, 0, U"Draw...", 0, 0, GRAPHICS_PitchModeler_draw);

	praat_addAction1 (classSound, 0, U"Get optimal formant ceiling...", U"Get intensity (dB)", praat_DEPTH_1 | praat_HIDDEN, REAL_Sound_getOptimalFormantCeiling);
	praat_addAction1 (classSound, 0, U"To Formant (interval)...", U"To Formant (robust)...", praat_DEPTH_2 | praat_HIDDEN, NEW_Sound_to_Formant_interval);
	praat_addAction1 (classSound, 0, U"To Formant (interval, constrained)...", U"To Formant (interval)...",
		praat_DEPTH_2 | praat_HIDDEN, NEW_Sound_to_Formant_interval_constrained);

	praat_addAction1 (classSound, 0, U"To OptimalCeilingTier...", U"To Formant (interval, constrained)...", praat_DEPTH_2 | praat_HIDDEN, NEW_Sound_to_OptimalCeilingTier);
	
	praat_addAction1 (classSound, 0, U"To Formant (interval, constrained, robust)...", U"To Formant (interval, constrained)...", 
		praat_DEPTH_2 | praat_HIDDEN, NEW_Sound_to_Formant_interval_constrained_robust);
	praat_addAction1 (classTable, 0, U"To DataModeler...", U"To logistic regression...", praat_DEPTH_1 + praat_HIDDEN, NEW_Table_to_DataModeler);
}

/* End of file praat_DataModeler_init.cpp 1566*/
