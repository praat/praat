/* praat_DataModeler_init.cpp
 *
 * Copyright (C) 2014,2016 David Weenink
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
#include "OptimalCeilingTierEditor.h"
#include "Pitch.h"
#include "Table_extensions.h"
#include <math.h>

#undef iam
#define iam iam_LOOP

/* DataModeler */

#define INFO_ONE(klas)  FIND_ONE (klas)  MelderInfo_open ();
#define INFO_ONE_END  MelderInfo_close (); END

FORM (NEW1_DataModeler_createSimple, U"Create simple DataModeler", nullptr) {
	WORDVAR (name, U"Name", U"dm")
	REALVAR (xmin, U"left X range", U"0.0")
	REALVAR (xmax, U"right X range", U"1.0")
	NATURALVAR (numberOfDataPoints, U"Number of data points", U"20")
	SENTENCEVAR (parameter_string, U"Parameters", U"0.0 1.0 1.0")
	POSITIVEVAR (standardDeviation, U"Gaussian noise stdev", U"0.2")
	OPTIONMENUVAR (functionType, U"Basis functions", 2)
		OPTION (U"Polynomial")
		OPTION (U"Legendre")
		
	OK
DO
	CREATE_ONE
		autoDataModeler result = DataModeler_createSimple (xmin, xmax, numberOfDataPoints, parameter_string, standardDeviation, functionType - 1);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_DataModeler_speckle, U"DataModeler: Speckle", nullptr) {
	REALVAR (xmin, U"left X range", U"0.0")
	REALVAR (xmax, U"right X range", U"0.0")
	REALVAR (ymin, U"left Y range", U"0.0")
	REALVAR (ymax, U"right Y range", U"0.0")
	BOOLEANVAR (errorBars, U"Draw error bars", 1)
	REALVAR (barWidth_mm, U"Bar width (mm)", U"1.0")
	REALVAR (xOffset_mm, U"Horizontal offset (mm)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	long order = 6;
	GRAPHICS_EACH (DataModeler)
		DataModeler_speckle (me, GRAPHICS, xmin, xmax,ymin, ymax, 0, order + 1, errorBars, barWidth_mm, xOffset_mm, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_DataModeler_drawEstimatedTrack, U"DataModeler: Draw estimated track", nullptr) {
	REALVAR (xmin, U"left X range", U"0.0")
	REALVAR (xmax, U"right X range", U"0.0")
	REALVAR (ymin, U"left Y range", U"0.0")
	REALVAR (ymax, U"right Y range", U"0.0")
	INTEGERVAR (order, U"Order of polynomials for estimation", U"3")
	REALVAR (xOffset, U"Horizontal offset (mm)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	REQUIRE (order >= 0, U"The order must be greater than or equal to zero.")
	GRAPHICS_EACH (DataModeler)
		DataModeler_drawTrack (me, GRAPHICS, xmin, xmax, ymin, ymax, 1, order + 1, xOffset, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_DataModeler_getNumberOfParameters) {
	INTEGER_ONE (DataModeler)
		long result = my numberOfParameters;
	INTEGER_ONE_END (U" (= number of parameters)")
}

DIRECT (INTEGER_DataModeler_getNumberOfFixedParameters) {
	INTEGER_ONE (DataModeler)
		long result = DataModeler_getNumberOfFixedParameters (me);
	INTEGER_ONE_END (U"(= number of parameters)")
}

FORM (REAL_DataModeler_getParameterValue, U"DataModeler: Get parameter value", nullptr) {
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	OK
DO
	REAL_ONE (DataModeler)
		double result = DataModeler_getParameterValue (me, parameterNumber);
	REAL_ONE_END (Melder_cat (U" (= parameter[", parameterNumber, U"])"))
}

FORM (INFO_DataModeler_getParameterStatus, U"DataModeler: Get parameter status", nullptr) {
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	OK
DO
	STRING_ONE (DataModeler)
		int status = DataModeler_getParameterStatus (me, parameterNumber);
		const char32 *result = status == DataModeler_PARAMETER_FREE ? U"Free" : (status == DataModeler_PARAMETER_FIXED ? U"Fixed" :
		U"Undefined");
	STRING_ONE_END
}

FORM (REAL_DataModeler_getParameterStandardDeviation, U"DataModeler: Get parameter standard deviation", nullptr) {
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	OK
DO
	REAL_ONE (DataModeler)
		double result = DataModeler_getParameterStandardDeviation (me, parameterNumber);
	REAL_ONE_END (Melder_cat (U" (= parameter[", parameterNumber, U"])"));
}

FORM (REAL_DataModeler_getVarianceOfParameters, U"DataModeler: Get variance of parameters", nullptr) {
	INTEGERVAR (fromParameter, U"left Parameter range", U"0")
	INTEGERVAR (toParameter, U"right Parameter range", U"0")
	OK
DO
	long nofp;
	REAL_ONE (DataModeler)
		double result = DataModeler_getVarianceOfParameters (me, fromParameter, toParameter, &nofp);
	REAL_ONE_END (Melder_cat (U" (for ", nofp, U" free parameters)"))
}

DIRECT (INTEGER_DataModeler_getNumberOfDataPoints) {
	INTEGER_ONE (DataModeler)
		long result = my numberOfDataPoints;
	INTEGER_ONE_END (U" (= number of data points)")
}

DIRECT (INTEGER_DataModeler_getNumberOfInvalidDataPoints) {
	INTEGER_ONE (DataModeler)
		long result = DataModeler_getNumberOfInvalidDataPoints (me);
	INTEGER_ONE_END (U" (= number of invalid data points)")
}

FORM (REAL_DataModeler_getModelValueAtX, U"DataModeler: Get model value at x", nullptr) {
	REALVAR (x, U"X", U"0.1")
	OK
DO
	REAL_ONE (DataModeler)
		double result = DataModeler_getModelValueAtX (me, x);
	REAL_ONE_END (U"")
}



DIRECT (REAL_DataModeler_getResidualSumOfSquares) {
	REAL_ONE (DataModeler)
		long n;
		double result = DataModeler_getResidualSumOfSquares (me, &n);
	REAL_ONE_END (Melder_cat (U"  (for ", n, U" datapoints)"))
}

DIRECT (REAL_DataModeler_getStandardDeviation) {
	REAL_ONE (DataModeler)
		double result = DataModeler_estimateSigmaY (me);
	REAL_ONE_END (U"")
}

FORM (REAL_DataModeler_getDataPointXValue, U"DataModeler: Get data point x value", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	REAL_ONE (DataModeler)
		double result = DataModeler_getDataPointXValue (me, index);
	REAL_ONE_END (Melder_cat (U" (= value at point ", index, U")"))
}

FORM (REAL_DataModeler_getDataPointYValue, U"DataModeler: Get data point y value", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	REAL_ONE (DataModeler)
		double result = DataModeler_getDataPointYValue (me, index);
	REAL_ONE_END (Melder_cat (U" (= value at point ", index, U")"))
}

FORM (REAL_DataModeler_getDataPointYSigma, U"DataModeler: Get data point y sigma", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	REAL_ONE (DataModeler)
		double result = DataModeler_getDataPointYSigma (me, index);
	REAL_ONE_END (Melder_cat (U" (= sigma at point ", index, U")"))
}

FORM (INFO_DataModeler_getDataPointStatus, U"DataModeler: Get data point status", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	STRING_ONE (DataModeler)
		int status = DataModeler_getDataPointStatus (me, index);
		const char32 *result = status == DataModeler_DATA_INVALID ? U"Invalid" : U"Valid";
	STRING_ONE_END
}

DIRECT (REAL_DataModeler_getCoefficientOfDetermination) {
	REAL_ONE (DataModeler)
		double result = DataModeler_getCoefficientOfDetermination (me, nullptr, nullptr);
	REAL_ONE_END (U" (= R^2)");
}


FORM (INFO_DataModeler_reportChiSquared, U"DataModeler: Report chi squared", nullptr) {
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Sigma")
		OPTION (U"Relative")
		OPTION (U"Sqrt sigma")
	OK
DO
	INFO_ONE (DataModeler)
		DataModeler_reportChiSquared (me, weighDataType);
	INFO_ONE_END
}

DIRECT (REAL_DataModeler_getDegreesOfFreedom) {
	REAL_ONE (DataModeler)
		double result = DataModeler_getDegreesOfFreedom (me);
	REAL_ONE_END (U" (= degrees of freedom)")
}


FORM (MODIFY_DataModeler_setDataWeighing, U"DataModeler: Set data weighing", nullptr) {
	OPTIONMENUVAR (weighDataType, U"Weigh data", 1)
		OPTION (U"Equally")
		OPTION (U"Sigma")
		OPTION (U"Relative")
		OPTION (U"Sqrt sigma")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataWeighing (me, weighDataType - 1);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setTolerance, U"DataModeler: Set tolerance", nullptr) {
	REALVAR (tolerance, U"Tolerance", U"1e-5")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setTolerance (me, tolerance);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setParameterValue, U"DataModeler: Set parameter value", nullptr) {
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	REALVAR (value, U"Value", U"0.0")
	OPTIONMENUVAR (parameterStatus, U"Status", 1)
		OPTION (U"Free")
		OPTION (U"Fixed")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterValue (me, parameterNumber, value, parameterStatus - 1);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setParameterFree, U"DataModeler: Set parameter free", nullptr) {
	INTEGERVAR (fromParameter, U"left Parameter range", U"0")
	INTEGERVAR (toParameter, U"right Parameter range", U"0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParametersFree (me, fromParameter, toParameter);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setParameterValuesToZero, U"DataModeler: Set parameter values to zero", nullptr) {
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setParameterValuesToZero (me, numberOfSigmas);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointStatus, U"DataModeler: Set data point status", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OPTIONMENUVAR (dataStatus, U"Status", 1)
		OPTION (U"Valid")
		OPTION (U"Invalid")
	OK
DO
	int status = dataStatus == 2 ? DataModeler_DATA_INVALID : DataModeler_DATA_VALID;
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointStatus (me, index, status);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointXValue, U"DataModeler: Set data point x value", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	REALVAR (x, U"X", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointXValue (me, index, x);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointYValue, U"DataModeler: Set data point y value", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	REALVAR (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointYValue (me, index, y);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointValues, U"DataModeler: Set data point values", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	REALVAR (x, U"X", U"0.0")
	REALVAR (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (DataModeler)
		DataModeler_setDataPointValues (me, index, x, y);
	MODIFY_EACH_END
}

FORM (MODIFY_DataModeler_setDataPointYSigma, U"DataModeler: Set data point y sigma", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	REALVAR (sigma, U"Sigma", U"10.0")
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
	CONVERT_EACH_END (my name)
}

FORM (NEW_DataModeler_to_Table_zscores, U"DataModeler: To Table (z-scores)", nullptr) {
	BOOLEANVAR (useSigmaY, U"Use sigmas on y-values", 1)
	OK
DO
	CONVERT_EACH (DataModeler)
		autoTable result = DataModeler_to_Table_zscores (me, useSigmaY);
	CONVERT_EACH_END (my name, U"_z");
}

FORM (NEW_Formant_to_FormantModeler, U"Formant: To FormantModeler", nullptr) {
//double tmin, double tmax, long numberOfFormants, long numberOfParametersPerTrack
	REALVAR (fromTime, U"left Start time", U"0.0")
	REALVAR (toTime, U"right End time", U"0.1")
	NATURALVAR (numberOfFormants, U"Number of formants", U"3")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	OK
DO
	REQUIRE (order >= 0, U"The order must be greater than or equal to zero.")
	CONVERT_EACH (Formant)
		autoFormantModeler result = Formant_to_FormantModeler (me, fromTime, toTime, numberOfFormants, order + 1, weighDataType - 1);
	CONVERT_EACH_END (my name, U"_o", order);
}

FORM (NEW1_Formants_extractSmoothestPart, U"Formants: Extract smoothest part", U"Formants: Extract smoothest part") {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	LABEL (U"", U"Use bandwidths to model the formant tracks:")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Zero parameter values whose range include zero:")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	OK
DO
	OrderedOf<structFormant> formants;
	LOOP {
		iam (Formant);
		formants. addItem_ref (me);
	}
	long index = Formants_getSmoothestInInterval (& formants, fromTime, toTime, numberOfFormantTracks, order + 1, weighDataType - 1, 0, numberOfSigmas, power, 1.0, 1.0, 1.0, 1.0, 1.0);
	// next code is necessary to get the Formant at postion index selected and to get its name
	long iselected = 0;
	Formant him = nullptr;
	LOOP {
		iselected ++;
		if (iselected == index) {
			him = static_cast<Formant> (OBJECT);
		}
	}
	Melder_assert (him);
	autoFormant thee = Formant_extractPart (him, fromTime, toTime);
	praat_new (thee.move(), his name, U"_part");
END }

FORM (NEW1_Formants_extractSmoothestPart_constrained, U"Formants: Extract smoothest part (constrained)", U"Formants: Extract smoothest part (constrained)...") {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	LABEL (U"", U"Use bandwidths to model the formant tracks:")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Zero parameter values whose range include zero:")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	LABEL (U"", U"The constraints on the formants:")
	REALVAR (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REALVAR (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REALVAR (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVEVAR (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVEVAR (minimumF3, U"Minimum F3 (Hz)", U"1500.0")
	OK
DO
	OrderedOf<structFormant> formants;
	LOOP {
		iam (Formant);
		formants. addItem_ref (me);
	}
	long index = Formants_getSmoothestInInterval (& formants, fromTime, toTime, numberOfFormantTracks, order + 1, weighDataType - 1, 1, numberOfSigmas, power, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3);
	// next code is necessary to get the Formant at postion index selected and to get its name
	long iselected = 0;
	Formant him = nullptr;
	LOOP {
		iselected ++;
		if (iselected == index) {
			him = static_cast<Formant> (OBJECT);
		}
	}
	Melder_assert (him);
	autoFormant thee = Formant_extractPart (him, fromTime, toTime);
	praat_new (thee.move(), his name, U"_part");
END }

/********************** FormantModeler ******************************/

FORM (GRAPHICS_FormantModeler_drawEstimatedTracks, U"FormantModeler: Draw estimated tracks", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURALVAR (fromFormant, U"left Formant range", U"1")
	NATURALVAR (toFormant, U"right Formant range", U"3")
	INTEGERVAR (order, U"Order of polynomials for estimation", U"3")
	REALVAR (xOffset_mm, U"Horizontal offset (mm)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	REQUIRE (order >= 0, U"The order must be greater than or equal to zero.")
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, 1, order + 1, xOffset_mm, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawTracks, U"FormantModeler: Draw tracks", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURALVAR (fromFormant, U"left Formant range", U"1")
	NATURALVAR (toFormant, U"right Formant range", U"3")
	REALVAR (xOffset_mm, U"Horizontal offset (mm)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	long order = 6;
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, 0, order + 1, xOffset_mm, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_speckle, U"FormantModeler: Speckle", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURALVAR (fromFormant, U"left Formant range", U"1")
	NATURALVAR (toFormant, U"right Formant range", U"3")
	BOOLEANVAR (errorBars, U"Draw error bars", true)
	REALVAR (barWidth_mm, U"Bar width (mm)", U"1.0")
	REALVAR (xOffset_mm, U"Horizontal offset (mm)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	long order = 6;
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_speckle (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, 0, order + 1, errorBars, barWidth_mm, xOffset_mm, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawOutliersMarked, U"FormantModeler: Draw outliers marked", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	NATURALVAR (fromFormant, U"left Formant range", U"1")
	NATURALVAR (toFormant, U"right Formant range", U"3")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"3.0")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	WORDVAR (mark_string, U"Mark", U"o")
	NATURALVAR (fontSize, U"Mark font size", U"12")
	REALVAR (xOffset_mm, U"Horizontal offset (mm)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawOutliersMarked (me, GRAPHICS, fromTime, toTime, maximumFrequency, fromFormant, toFormant, numberOfSigmas, weighDataType - 1, mark_string, fontSize, xOffset_mm, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawVariancesOfShiftedTracks, U"FormantModeler: Draw variances of shifted tracks", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (fromVariance, U"left Variance range", U"0.0")
	REALVAR (toVariance, U"right Variance range", U"0.0")
	OPTIONMENUVAR (shiftTracks, U"Shift tracks", 1)
		OPTION (U"No")
		OPTION (U"Up")
		OPTION (U"Down")
	NATURALVAR (fromFormant, U"left Formant range", U"1")
	NATURALVAR (toFormant, U"right Formant range", U"4")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawVariancesOfShiftedTracks (me, GRAPHICS, fromTime, toTime, fromVariance, toVariance, shiftTracks, fromFormant, toFormant, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawCumulativeChiScores, U"FormantModeler: Draw cumulative chi scores", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (fromChisq, U"left Chisq range", U"0.0")
	REALVAR (toChisq, U"right Chisq range", U"0.0")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawCumulativeChiScores (me, GRAPHICS, fromTime, toTime, fromChisq, toChisq, weighDataType, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_FormantModeler_normalProbabilityPlot, U"FormantModeler: Normal probability plot", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	NATURALVAR (numberOfQuantiles, U"Number of quantiles", U"100")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURALVAR (fontSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_normalProbabilityPlot (me, GRAPHICS, formantNumber, weighDataType - 1, numberOfQuantiles, numberOfSigmas, fontSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantModeler_drawBasisFunction, U"FormantModeler: Draw basis function", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (ymin, U"left Amplitude range (Hz)", U"0.0")
	REALVAR (ymax, U"right Amplitude range (Hz)", U"5500.0")
	//long iterm, bool scaled, long numberOfPoints, int garnish
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (basisFunctionIndex, U"Basis function", U"2")
	BOOLEANVAR (scale, U"Scale function with parameter value", false)
	NATURALVAR (numberOfPoints, U"Number of points", U"200")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantModeler)
		FormantModeler_drawBasisFunction (me, GRAPHICS, fromTime, toTime, ymin, ymax, formantNumber, basisFunctionIndex, scale, numberOfPoints, garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_FormantModeler_getModelValueAtTime, U"", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	REALVAR (time, U"Time (s)", U"0.1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getModelValueAtTime (me, formantNumber, time);
	REAL_ONE_END (U"Hertz")
}


FORM (REAL_FormantModeler_getDataPointValue, U"FormantModeler: Get data point value", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getDataPointValue (me, formantNumber, index);
	REAL_ONE_END (Melder_cat (U" (= value of point ", index, U" in track F", formantNumber, U")"))
}

FORM (REAL_FormantModeler_getDataPointSigma, U"FormantModeler: Get data point sigma", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getDataPointSigma (me, formantNumber, index);
	REAL_ONE_END (Melder_cat (U" (= sigma of point ", index, U" in track F", formantNumber, U")"))
}


FORM (INFO_FormantModeler_getDataPointStatus, U"FormantModeler: Get data point status", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		int status = FormantModeler_getDataPointStatus (me, formantNumber, index);
		const char32 *result = status == DataModeler_DATA_INVALID ? U"Invalid" : U"Valid";
	INTEGER_ONE_END (U"")
}

DIRECT (INTEGER_FormantModeler_getNumberOfTracks) {
	INTEGER_ONE (FormantModeler)
		long result = FormantModeler_getNumberOfTracks (me);
	INTEGER_ONE_END (U" (= number of formants)")
}


FORM (INTEGER_FormantModeler_getNumberOfParameters, U"FormantModeler: Get number of parameters", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		long result = FormantModeler_getNumberOfParameters (me, formantNumber);
	INTEGER_ONE_END (U" (= number of parameters for F", formantNumber, U")")
}


FORM (INTEGER_FormantModeler_getNumberOfFixedParameters, U"FormantModeler: Get number of fixed parameters", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		long result = FormantModeler_getNumberOfFixedParameters (me, formantNumber);
	INTEGER_ONE_END (U" (= number of fixed parameters for F", formantNumber, U")")
}

DIRECT (INTEGER_FormantModeler_getNumberOfDataPoints) {
	INTEGER_ONE (FormantModeler)
		long result = FormantModeler_getNumberOfDataPoints (me);
	INTEGER_ONE_END (U"")
}

FORM (INTEGER_FormantModeler_getNumberOfInvalidDataPoints, U"FormantModeler: Get number of invalid data points", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	INTEGER_ONE (FormantModeler)
		long result = FormantModeler_getNumberOfInvalidDataPoints (me, formantNumber);
	INTEGER_ONE_END (U" (= number of invalid data points for F", formantNumber, U")")
}

FORM (REAL_FormantModeler_getParameterValue, U"FormantModeler: Get parameter value", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getParameterValue (me, formantNumber, parameterNumber);
	REAL_ONE_END (Melder_cat (U" (= parameter[", parameterNumber, U"] for F", formantNumber, U")"))
}

FORM (INFO_FormantModeler_getParameterStatus, U"FormantModeler: Get parameter status", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	OK
DO
	STRING_ONE (FormantModeler)
		int status = FormantModeler_getParameterStatus (me, formantNumber, parameterNumber);
		const char32 *result = Melder_cat (status == DataModeler_PARAMETER_FREE ? U"Free" : (status == DataModeler_PARAMETER_FIXED ? U"Fixed" :
		U"Undefined"), U" (= parameter[", parameterNumber, U"] for F", formantNumber, U")");
	STRING_ONE_END
}

FORM (REAL_FormantModeler_getParameterStandardDeviation, U"FormantModeler: Get parameter standard deviatio", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getParameterStandardDeviation (me, formantNumber, parameterNumber);
	REAL_ONE_END (Melder_cat (U" (= parameter[", parameterNumber, U"] for F", formantNumber, U")"))
}


FORM (REAL_FormantModeler_getVarianceOfParameters, U"FormantModeler: Get variance of parameters", nullptr) {
	INTEGERVAR (fromFormant, U"left Formant range", U"0")
	INTEGERVAR (toFormant, U"right Formant range", U"0")
	INTEGERVAR (fromParameter, U"left Parameter range", U"0")
	INTEGERVAR (toParameter, U"right Parameter range", U"0")
	OK
DO
	long nofp;
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getVarianceOfParameters (me, fromFormant, toFormant, fromParameter, toParameter, &nofp);
	REAL_ONE_END (Melder_cat (U" (for ", nofp, U" free parameters.)"))
}


FORM (REAL_FormantModeler_getCoefficientOfDetermination, U"FormantModeler: Get coefficient of determination", nullptr) {
	INTEGERVAR (fromFormant, U"left Formant range", U"0")
	INTEGERVAR (toFormant, U"right Formant range", U"0")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getCoefficientOfDetermination (me, fromFormant, toFormant);
	REAL_ONE_END (U" (= R^2)");
}


FORM (REAL_FormantModeler_getResidualSumOfSquares, U"FormantModeler: Get residual sum of squares", U"FormantModeler: Get residual sum of squares...") {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getResidualSumOfSquares (me, formantNumber, nullptr);
	REAL_ONE_END (Melder_cat (U" Hz^2,  (= RSS of F", formantNumber, U")"))
}

FORM (REAL_FormantModeler_getStandardDeviation, U"FormantModeler: Get formant standard deviation", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getStandardDeviation (me, formantNumber);
	REAL_ONE_END (Melder_cat (U" Hz (= std. dev. of F", formantNumber, U")"))
}

FORM (INFO_FormantModeler_reportChiSquared, U"FormantModeler: Report chi squared", nullptr) {
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	OK
DO
	INFO_ONE (FormantModeler)
		FormantModeler_reportChiSquared (me, weighDataType);
	INFO_ONE_END
}

FORM (REAL_FormantModeler_getDegreesOfFreedom, U"FormantModeler: Get degrees of freedom", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getDegreesOfFreedom (me, formantNumber);
	REAL_ONE_END (Melder_cat (U" (= degrees of freedom of F", formantNumber, U")"))
}

FORM (REAL_FormantModeler_getSmoothnessValue, U"FormantModeler: Get smoothness value", nullptr) {
	INTEGERVAR (fromFormant, U"left Formant range", U"0")
	INTEGERVAR (toFormant, U"right Formant range", U"0")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	REALVAR (power, U"Parameter variance power", U"1.5")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getSmoothnessValue (me, fromFormant, toFormant, order, power);
	REAL_ONE_END (U" (= smoothness)")
}

FORM (REAL_FormantModeler_getAverageDistanceBetweenTracks, U"FormantModeler: Get average distance between tracks", nullptr) {
	NATURALVAR (track1, U"Track 1", U"2")
	NATURALVAR (track2, U"Track 2", U"3")
	OPTIONMENUVAR (typeOfData, U"Type of data", 1)
		OPTION (U"Data points")
		OPTION (U"Modeled")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getAverageDistanceBetweenTracks (me, track1, track2, typeOfData - 1);
	REAL_ONE_END (Melder_cat (U" (= average |F", track1, U" - F", track2, U"|)"))
}

FORM (REAL_FormantModeler_getFormantsConstraintsFactor, U"FormantModeler: Get formants constraints factor", nullptr) {
	REALVAR (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REALVAR (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REALVAR (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVEVAR (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVEVAR (minimumF3, U"Minimum F3 (Hz)", U"1500.0")
	OK
DO
	REAL_ONE (FormantModeler)
		double result = FormantModeler_getFormantsConstraintsFactor (me, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3);
	REAL_ONE_END (U" (= formants constraints factor)");
}

FORM (MODIFY_FormantModeler_setDataWeighing, U"FormantModeler: Set data weighing", nullptr) {
	INTEGERVAR (fromFormant, U"left Formant range", U"0")
	INTEGERVAR (toFormant, U"right Formant range", U"0")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataWeighing (me, fromFormant, toFormant, weighDataType - 1);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setTolerance, U"FormantModeler: Set tolerance", nullptr) {
	REALVAR (tolerance, U"Tolerance", U"1e-5")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setTolerance (me, tolerance);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setParameterValueFixed, U"FormantModeler: Set parameter value fixed", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	NATURALVAR (parameterNumber, U"Parameter number", U"1")
	REALVAR (parameterValue, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParameterValueFixed (me, formantNumber, parameterNumber, parameterValue);
	MODIFY_EACH_END
}


FORM (MODIFY_FormantModeler_setParameterFree, U"FormantModeler: Set parameter free", nullptr) {
	INTEGERVAR (fromFormant, U"left Formant range", U"0")
	INTEGERVAR (toFormant, U"right Formant range", U"0")
	INTEGERVAR (fromParameter, U"left Parameter range", U"0")
	INTEGERVAR (toParameter, U"right Parameter range", U"0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParametersFree (me, fromFormant, toFormant, fromParameter, toParameter);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setParameterValuesToZero, U"FormantModeler: Set parameter values to zero", nullptr) {
	INTEGERVAR (fromFormant, U"left Formant range", U"0")
	INTEGERVAR (toFormant, U"right Formant range", U"0")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setParameterValuesToZero (me, fromFormant,  toFormant, numberOfSigmas);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setDataPointValue, U"FormantModeler: Set data point value", nullptr) {
	NATURALVAR (formantNumber, U"Formant index", U"1")
	NATURALVAR (dataNumber, U"Data index", U"1")
	REALVAR (value, U"Value", U"1.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointValue (me, formantNumber,  dataNumber, value);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setDataPointSigma, U"FormantModeler: Set data point sigma", nullptr) {
	NATURALVAR (formantNumber, U"Formant index", U"1")
	NATURALVAR (dataNumber, U"Data index", U"1")
	REALVAR (sigma, U"Sigma", U"10.0")
	OK
DO
	MODIFY_EACH (FormantModeler)
		FormantModeler_setDataPointSigma (me, formantNumber,  dataNumber, sigma);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantModeler_setDataPointStatus, U"FormantModeler: Set data point status", nullptr) {
	NATURALVAR (formantNumber, U"Formant index", U"1")
	NATURALVAR (dataNumber, U"Data index", U"1")
	OPTIONMENUVAR (dataStatus, U"Status", 1)
		OPTION (U"Valid")
		OPTION (U"Invalid")
	OK
DO
	int status = dataStatus == 2 ? DataModeler_DATA_INVALID : DataModeler_DATA_VALID;
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
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoCovariance result = FormantModeler_to_Covariance_parameters (me, formantNumber);
	CONVERT_EACH_END (my name, U"_", formantNumber);
}

FORM (NEW_FormantModeler_extractDataModeler, U"FormantModeler: Extract DataModeler", nullptr) {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoDataModeler result = FormantModeler_extractDataModeler (me, formantNumber);
	CONVERT_EACH_END (my name, U"_", formantNumber)
}

FORM (NEW_FormantModeler_to_Table_zscores, U"", nullptr) {
	BOOLEANVAR (useBandwidth, U"Bandwidths as standard deviation", true)
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoTable result = FormantModeler_to_Table_zscores (me, useBandwidth);
	CONVERT_EACH_END (my name, U"_z")
}

FORM (NEW_FormantModeler_processOutliers, U"", nullptr) {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"3.0")
	BOOLEANVAR (useBandwidth, U"Bandwidths as standard deviation", true)
	OK
DO
	CONVERT_EACH (FormantModeler)
		autoFormantModeler result = FormantModeler_processOutliers (me, numberOfSigmas, useBandwidth);
	CONVERT_EACH_END (my name, U"_outliers");
}


DIRECT (WINDOW_OptimalCeilingTier_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an OptimalCeilingTier from batch.");
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSound) {
			sound = (Sound) OBJECT;   // may stay nullptr
		}
	}
	LOOP if (CLASS == classOptimalCeilingTier) {
		iam (OptimalCeilingTier);
		autoOptimalCeilingTierEditor editor = OptimalCeilingTierEditor_create (ID_AND_FULL_NAME, me, sound, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }


/*************************** PitchModeler *************************************/

FORM (NEW_Pitch_to_PitchModeler, U"Pitch: To PitchModeler", nullptr) {
	REALVAR (fromTime, U"left Start time (s)", U"0.0")
	REALVAR (toTime, U"right End time (s)", U"0.1")
	INTEGERVAR (order, U"Order of polynomials", U"2")
	OK
DO
	CONVERT_EACH (Pitch)
		autoPitchModeler result = Pitch_to_PitchModeler (me, fromTime, toTime, order + 1);
	CONVERT_EACH_END (my name)
}

FORM (GRAPHICS_PitchModeler_draw, U"PitchModeler: Draw", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0")
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"500.0")
	INTEGERVAR (order, U"Order of polynomial for estimation", U"2")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PitchModeler)
		PitchModeler_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, order + 1,  garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_Sound_getOptimalFormantCeiling, U"Sound: Get optimal formant ceiling", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.1")
	REALVAR (toTime, U"right Time range (s)", U"0.15")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.0025")
	POSITIVEVAR (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVEVAR (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURALVAR (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	OK
DO
	REAL_ONE (Sound)
		double result = Sound_getOptimalFormantCeiling (me, fromTime, toTime, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighDataType - 1, numberOfSigmas, power);
	REAL_ONE_END (U" Hz");
}

FORM (NEW_Sound_to_Formant_interval, U"Sound: To Formant (interval)", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.1")
	REALVAR (toTime, U"right Time range (s)", U"0.15")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.0025")
	POSITIVEVAR (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVEVAR (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURALVAR (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	OK
DO
	CONVERT_EACH (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval (me, fromTime, toTime, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighDataType - 1, numberOfSigmas, power, 0, 1, 1, 1, 1, 1, &ceiling);
	CONVERT_EACH_END (my name, U"_", Melder_fixed (ceiling, 0))
}

FORM (NEW_Sound_to_Formant_interval_constrained, U"Sound: To Formant (interval, constrained)", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.1")
	REALVAR (toTime, U"right Time range (s)", U"0.15")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.0025")
	POSITIVEVAR (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVEVAR (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURALVAR (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	LABEL (U"", U"Formant frequency constraints")
	REALVAR (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REALVAR (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REALVAR (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVEVAR (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVEVAR (minimumF3, U"Minimum F3 (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval (me, fromTime, toTime, windowLength, timeStep, fromFrequency,  toFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighDataType - 1, numberOfSigmas, power, 1, minimumF1, maximumF1, minimumF2, maximumF2, minimumF3, & ceiling);
	CONVERT_EACH_END (my name, U"_", Melder_fixed (ceiling, 0));
}

FORM (NEW_Sound_to_Formant_interval_constrained_robust, U"Sound: To Formant (interval, constrained, robust)", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.1")
	REALVAR (toTime, U"right Time range (s)", U"0.15")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.0025")
	POSITIVEVAR (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVEVAR (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURALVAR (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"3")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	LABEL (U"", U"Formant frequency constraints")
	REALVAR (minimumF1, U"Minimum F1 (Hz)", U"100.0")
	REALVAR (maximumF1, U"Maximum F1 (Hz)", U"1200.0")
	REALVAR (minimumF2, U"Minimum F2 (Hz)", U"0.0")
	POSITIVEVAR (maximumF2, U"Maximum F2 (Hz)", U"5000.0")
	POSITIVEVAR (minimumF3, U"Minimum F3 (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH (Sound)
		double ceiling;
		autoFormant result = Sound_to_Formant_interval_robust (me, fromTime, toTime, windowLength, timeStep, fromFrequency, fromFrequency, numberOfFrequencySteps, preEmphasisFrequency, numberOfFormantTracks, order + 1, weighDataType - 1, numberOfSigmas, power, 1, minimumF1, maximumF1, minimumF2, minimumF2, minimumF3, &ceiling);
	CONVERT_EACH_END (my name, U"_", Melder_fixed (ceiling, 0))
}

FORM (NEW_Sound_to_OptimalCeilingTier, U"", nullptr) {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.0025")
	POSITIVEVAR (fromFrequency, U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVEVAR (toFrequency, U"right Maximum frequency range (Hz)", U"6500.0")
	NATURALVAR (numberOfFrequencySteps, U"Number of frequency steps", U"11")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	REALVAR (smoothingWindow_s, U"Formant smoothing window (s)", U"0.05")
	NATURALVAR (numberOfFormantTracks, U"Number of formant tracks in model", U"4")
	INTEGERVAR (order, U"Order of polynomials", U"2")
	OPTIONMENUVAR (weighDataType, U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	REALVAR (power, U"Parameter variance power", U"1.5")
	OK
DO
	CONVERT_EACH (Sound)
		autoOptimalCeilingTier result = Sound_to_OptimalCeilingTier (me, windowLength, timeStep, fromFrequency, toFrequency, numberOfFrequencySteps, preEmphasisFrequency, smoothingWindow_s, numberOfFormantTracks, order + 1, weighDataType - 1, numberOfSigmas, power);
	CONVERT_EACH_END (my name);
}

FORM (NEW_Table_to_DataModeler, U"", nullptr) {
	REALVAR (xmin, U"left X range", U"0.0")
	REALVAR (xmax, U"right X range", U"0.0 (= auto)")
	WORDVAR (columnWithX_string, U"Column with X data", U"")
	WORDVAR (columnWithY_string, U"Column with Y data", U"")
	WORDVAR (columnEithSigma_string, U"Column with sigmas", U"")
	OPTIONMENUVAR (functionType, U"Model functions", 1)
		OPTION (U"Legendre polynomials")
	INTEGERVAR (maximumOrder, U"Maximum order", U"3")
	OK
DO
	CONVERT_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, columnWithX_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, columnWithY_string);
		long scolumn = Table_findColumnIndexFromColumnLabel (me, columnEithSigma_string);
		autoDataModeler result = Table_to_DataModeler (me, xmin, xmax, xcolumn, ycolumn, scolumn, maximumOrder + 1, functionType);
	CONVERT_EACH_END (my name)
}

void praat_DataModeler_init ();
void praat_DataModeler_init () {
	Thing_recognizeClassesByName (classDataModeler, classFormantModeler, classOptimalCeilingTier, classOptimalCeilingTierEditor, classPitchModeler, nullptr);

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
		praat_addAction1 (classDataModeler, 0, U"Get data standard deviation", 0, 1, REAL_DataModeler_getStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get coefficient of determination", 0, 1, REAL_DataModeler_getCoefficientOfDetermination);
		praat_addAction1 (classDataModeler, 0, U"Report chi squared...", 0, 1, INFO_DataModeler_reportChiSquared);
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
	praat_addAction1 (classDataModeler, 0, U"To Table (z-scores)...", 0, 0, NEW_DataModeler_to_Table_zscores);

	praat_addAction1 (classFormant, 0, U"To FormantModeler...", U"To LPC...", praat_HIDDEN, NEW_Formant_to_FormantModeler);
	praat_addAction1 (classFormant, 0, U"Extract smoothest part...", 0, praat_HIDDEN, NEW1_Formants_extractSmoothestPart);
	praat_addAction1 (classFormant, 0, U"Extract smoothest part (constrained)...", 0, praat_HIDDEN, NEW1_Formants_extractSmoothestPart_constrained);

	praat_addAction1 (classFormantModeler, 0, U"Draw -", 0, 0, 0);
	praat_addAction1 (classFormantModeler, 0, U"Speckle...", 0, 1, GRAPHICS_FormantModeler_speckle);
	praat_addAction1 (classFormantModeler, 0, U"Draw tracks...", 0, 1, GRAPHICS_FormantModeler_drawTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw estimated tracks...", 0, 1, GRAPHICS_FormantModeler_drawEstimatedTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw variances of shifted tracks...", 0, 1, GRAPHICS_FormantModeler_drawVariancesOfShiftedTracks);
	praat_addAction1 (classFormantModeler, 0, U"Draw outliers marked...", 0, 1, GRAPHICS_FormantModeler_drawOutliersMarked);
	praat_addAction1 (classFormantModeler, 0, U"Draw cumulative chisq scores...", 0, 1, GRAPHICS_FormantModeler_drawCumulativeChiScores);
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
		praat_addAction1 (classFormantModeler, 0, U"Get formant standard deviation...", 0, 1, REAL_FormantModeler_getStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get coefficient of determination...", 0, 1, REAL_FormantModeler_getCoefficientOfDetermination);
		praat_addAction1 (classFormantModeler, 0, U"Report chi squared...", 0, 1, INFO_FormantModeler_reportChiSquared);
		praat_addAction1 (classFormantModeler, 0, U"Get degrees of freedom...", 0, 1, REAL_FormantModeler_getDegreesOfFreedom);
		praat_addAction1 (classFormantModeler, 0, U"Get smoothness value...", 0, 1, REAL_FormantModeler_getSmoothnessValue);
		praat_addAction1 (classFormantModeler, 0, U"Get average distance between tracks...", 0, 1, REAL_FormantModeler_getAverageDistanceBetweenTracks);
		praat_addAction1 (classFormantModeler, 0, U"Get formants constraints factor...", 0, 1, REAL_FormantModeler_getFormantsConstraintsFactor);

	praat_addAction1 (classFormantModeler, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, U"Set data weighing...", 0, 1, MODIFY_FormantModeler_setDataWeighing);
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
	praat_addAction1 (classFormantModeler, 0, U"To Table (z-scores)...", 0, 0, NEW_FormantModeler_to_Table_zscores);
	praat_addAction1 (classFormantModeler, 0, U"To FormantModeler (process outliers)...", 0, 0, NEW_FormantModeler_processOutliers);
	praat_addAction1 (classFormantModeler, 0, U"Extract DataModeler...", 0, 0, NEW_FormantModeler_extractDataModeler);

	praat_addAction1 (classOptimalCeilingTier, 1, U"View & Edit", 0, praat_ATTRACTIVE | praat_NO_API, WINDOW_OptimalCeilingTier_edit);
	
	praat_addAction1 (classPitch, 0, U"To PitchModeler...", U"To PointProcess", praat_HIDDEN, NEW_Pitch_to_PitchModeler);

	praat_addAction1 (classPitchModeler, 0, U"Draw...", 0, 0, GRAPHICS_PitchModeler_draw);

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
