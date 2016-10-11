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

FORM3 (NEW_DataModeler_createSimple, U"Create simple DataModeler", nullptr) {
	WORD (U"Name", U"dm")
	REAL (U"left X range", U"0.0")
	REAL (U"right X range", U"1.0")
	NATURAL (U"Number of data points", U"20")
	SENTENCE (U"Parameters", U"0.0 1.0 1.0")
	POSITIVE (U"Gaussian noise stdev", U"0.2")
	OPTIONMENU (U"Basis functions", 2)
		OPTION (U"Polynomial")
		OPTION (U"Legendre")
		
	OK2
DO
	autoDataModeler thee = DataModeler_createSimple (GET_REAL (U"left X range"), GET_REAL (U"right X range"),
		GET_INTEGER (U"Number of data points"), GET_STRING (U"Parameters"), GET_REAL (U"Gaussian noise stdev"),
		GET_INTEGER (U"Basis functions") - 1);
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

FORM3 (GRAPHICS_DataModeler_speckle, U"DataModeler: Speckle", nullptr) {
	REAL (U"left X range", U"0.0")
	REAL (U"right X range", U"0.0")
	REAL (U"left Y range", U"0.0")
	REAL (U"right Y range", U"0.0")
	BOOLEAN (U"Draw error bars", 1)
	REAL (U"Bar width (mm)", U"1.0")
	REAL (U"Horizontal offset (mm)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	long order = 6;
	LOOP {
		iam (DataModeler);
		DataModeler_speckle (me, GRAPHICS, GET_REAL (U"left X range"), GET_REAL (U"right X range"),
			GET_REAL (U"left Y range"), GET_REAL (U"right Y range"),
			0, order + 1, GET_INTEGER (U"Draw error bars"), GET_REAL (U"Bar width"), GET_REAL (U"Horizontal offset"),
			GET_INTEGER (U"Garnish"));
	}
END2 }


FORM3 (GRAPHICS_DataModeler_drawEstimatedTrack, U"DataModeler: Draw estimated track", nullptr) {
	REAL (U"left X range", U"0.0")
	REAL (U"right X range", U"0.0")
	REAL (U"left Y range", U"0.0")
	REAL (U"right Y range", U"0.0")
	INTEGER (U"Order of polynomials for estimation", U"3")
	REAL (U"Horizontal offset (mm)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	long order = GET_INTEGER (U"Order of polynomials for estimation");
	REQUIRE (order >= 0, U"The order must be greater than or equal to zero.")
	LOOP {
		iam (DataModeler);
		DataModeler_drawTrack (me, GRAPHICS, GET_REAL (U"left X range"), GET_REAL (U"right X range"),
			GET_REAL (U"left Y range"), GET_REAL (U"right Y range"), 1, order + 1, GET_REAL (U"Horizontal offset"), GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT3 (INTEGER_DataModeler_getNumberOfParameters) {
	LOOP {
		iam (DataModeler);
		Melder_information (my numberOfParameters, U" (= number of parameters)");
	}
END2 }

DIRECT3 (INTEGER_DataModeler_getNumberOfFixedParameters) {
	LOOP {
		iam (DataModeler);
		Melder_information (DataModeler_getNumberOfFixedParameters (me), U" (= number of parameters)");
	}
END2 }

FORM3 (REAL_DataModeler_getParameterValue, U"DataModeler: Get parameter value", nullptr) {
	NATURAL (U"Parameter number", U"1")
	OK2
DO
	long iparameter = GET_INTEGER (U"Parameter number");
	LOOP {
		iam (DataModeler);
		double parameter = DataModeler_getParameterValue (me, iparameter);
		Melder_information (parameter, U" (= parameter[", iparameter, U"])");
	}
END2 }

FORM3 (INTEGER_DataModeler_getParameterStatus, U"DataModeler: Get parameter status", nullptr) {
	NATURAL (U"Parameter number", U"1")
	OK2
DO
	long iparameter = GET_INTEGER (U"Parameter number");
	LOOP {
		iam (DataModeler);
		int status = DataModeler_getParameterStatus (me, iparameter);
		Melder_information (status == DataModeler_PARAMETER_FREE ? U"Free" : (status == DataModeler_PARAMETER_FIXED ? U"Fixed" :
			U"Undefined"), U" (= parameter[", iparameter, U"])");
	}
END2 }

FORM3 (REAL_DataModeler_getParameterStandardDeviation, U"DataModeler: Get parameter standard deviation", nullptr) {
	NATURAL (U"Parameter number", U"1")
	OK2
DO
	long iparameter = GET_INTEGER (U"Parameter number");
	LOOP {
		iam (DataModeler);
		double sigma = DataModeler_getParameterStandardDeviation (me, iparameter);
		Melder_information (sigma, U" (= parameter[", iparameter, U"])");
	}
END2 }

FORM3 (REAL_DataModeler_getVarianceOfParameters, U"DataModeler: Get variance of parameters", nullptr) {
	INTEGER (U"left Parameter range", U"0")
	INTEGER (U"right Parameter range", U"0")
	OK2
DO
	long nofp;
	LOOP {
		iam (DataModeler);
		double var = DataModeler_getVarianceOfParameters (me, GET_INTEGER (U"left Parameter range"), GET_INTEGER (U"right Parameter range"), &nofp);
		Melder_information (var, U" (for ", nofp, U" free parameters)");
	}
END2 }

DIRECT3 (INTEGER_DataModeler_getNumberOfDataPoints) {
	LOOP {
		iam (DataModeler);
		Melder_information (my numberOfDataPoints, U" (= number of data points)");
	}
END2 }

DIRECT3 (INTEGER_DataModeler_getNumberOfInvalidDataPoints) {
	LOOP {
		iam (DataModeler);
		Melder_information (DataModeler_getNumberOfInvalidDataPoints (me), U" (= number of invalid data points)");
	}
END2 }

FORM3 (REAL_DataModeler_getModelValueAtX, U"DataModeler: Get model value at x", nullptr) {
	REAL (U"X", U"0.1")
	OK2
DO
	LOOP {
		iam (DataModeler);
		double y = DataModeler_getModelValueAtX (me, GET_REAL (U"X"));
		Melder_informationReal (y, nullptr);
	}
END2 }


DIRECT3 (REAL_DataModeler_getResidualSumOfSquares) {
	LOOP {
		long n;
		iam (DataModeler);
		double rss = DataModeler_getResidualSumOfSquares (me, &n);
		Melder_information (rss, U"  (for ", n, U" datapoints)");
	}
END2 }

DIRECT3 (REAL_DataModeler_getStandardDeviation) {
	LOOP {
		iam (DataModeler);
		double sigma = DataModeler_estimateSigmaY (me);
		Melder_information (sigma);
	}
END2 }

FORM3 (REAL_DataModeler_getDataPointValue, U"DataModeler: Get data point value", nullptr) {
	NATURAL (U"Index", U"1")
	OK2
DO
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (DataModeler);
		double value = DataModeler_getDataPointValue (me, index);
		Melder_information (value, U" (= value at point ", index, U")");
	}
END2 }

FORM3 (REAL_DataModeler_getDataPointSigma, U"DataModeler: Get data point sigma", nullptr) {
	NATURAL (U"Index", U"1")
	OK2
DO
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (DataModeler);
		double sigma = DataModeler_getDataPointSigma (me, index);
		Melder_information (sigma, U" (= sigma at point ", index, U")");
	}
END2 }

FORM3 (INTEGER_DataModeler_getDataPointStatus, U"DataModeler: Get data point status", nullptr) {
	NATURAL (U"Index", U"1")
	OK2
DO
	LOOP {
		iam (DataModeler);
		int status = DataModeler_getDataPointStatus (me, GET_INTEGER (U"Index"));
		Melder_information (status == DataModeler_DATA_INVALID ? U"Invalid" : U"Valid");
	}
END2 }

DIRECT3 (REAL_DataModeler_getCoefficientOfDetermination) {
	LOOP {
		iam (DataModeler);
		double rSquared = DataModeler_getCoefficientOfDetermination (me, nullptr, nullptr);
		Melder_informationReal (rSquared, U" (= R^2)");
	}
END2 }

FORM3 (REAL_DataModeler_reportChiSquared, U"DataModeler: Report chi squared", nullptr) {
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Sigma")
		OPTION (U"Relative")
		OPTION (U"Sqrt sigma")
	OK2
DO
	LOOP {
		iam (DataModeler);
		int useSigmaY = GET_INTEGER (U"Weigh data") - 1;
		MelderInfo_open ();
		MelderInfo_writeLine (U"Chi squared test:");
		MelderInfo_writeLine (useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? U"Standard deviation is estimated from the data." :
			useSigmaY == DataModeler_DATA_WEIGH_SIGMA ? U"Sigmas are used as estimate for local standard deviations." : 
			useSigmaY == DataModeler_DATA_WEIGH_RELATIVE ? U"1/Q's are used as estimate for local standard deviations." :
			U"Sqrt sigmas are used as estimate for local standard deviations.");
		double ndf, probability, chisq = DataModeler_getChiSquaredQ (me, useSigmaY, &probability, &ndf);
		MelderInfo_writeLine (U"Chi squared = ", chisq);
		MelderInfo_writeLine (U"Probability = ", probability);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", ndf);
		MelderInfo_close ();
	}
END2 }

DIRECT3 (REAL_DataModeler_getDegreesOfFreedom) {
	LOOP {
		iam (DataModeler);
		double dof = DataModeler_getDegreesOfFreedom (me);
		Melder_informationReal (dof, U" (= degrees of freedom)");
	}
END2 }

FORM3 (MODIFY_DataModeler_setDataWeighing, U"DataModeler: Set data weighing", nullptr) {
	OPTIONMENU (U"Weigh data", 1)
		OPTION (U"Equally")
		OPTION (U"Sigma")
		OPTION (U"Relative")
		OPTION (U"Sqrt sigma")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setDataWeighing (me, GET_INTEGER (U"Weigh data") - 1);
	}
END2 }

FORM3 (MODIFY_DataModeler_setTolerance, U"DataModeler: Set tolerance", nullptr) {
	REAL (U"Tolerance", U"1e-5")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setTolerance (me, GET_REAL (U"Tolerance"));
	}
END2 }

FORM3 (MODIFY_DataModeler_setParameterValue, U"DataModeler: Set parameter value", nullptr) {
	NATURAL (U"Parameter number", U"1")
	REAL (U"Value", U"0.0")
	OPTIONMENU (U"Status", 1)
		OPTION (U"Free")
		OPTION (U"Fixed")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setParameterValue (me, GET_INTEGER (U"Parameter number"), GET_REAL (U"Value"), GET_INTEGER (U"Status") - 1);
	}
END2 }

FORM3 (MODIFY_DataModeler_setParameterFree, U"DataModeler: Set parameter free", nullptr) {
	INTEGER (U"left Parameter range", U"0")
	INTEGER (U"right Parameter range", U"0")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setParametersFree (me, GET_INTEGER (U"left Parameter range"), GET_INTEGER (U"right Parameter range"));
	}
END2 }

FORM3 (MODIFY_DataModeler_setParameterValuesToZero, U"DataModeler: Set parameter values to zero", nullptr) {
	REAL (U"Number of sigmas", U"1.0")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setParameterValuesToZero (me, GET_REAL (U"Number of sigmas"));
	}
END2 }

FORM3 (MODIFY_DataModeler_setDataPointStatus, U"DataModeler: Set data point status", nullptr) {
	NATURAL (U"Index", U"1")
	OPTIONMENU (U"Status", 1)
		OPTION (U"Valid")
		OPTION (U"Invalid")
	OK2
DO
	int menustatus = GET_INTEGER (U"Status");
	int status = menustatus == 2 ? DataModeler_DATA_INVALID : DataModeler_DATA_VALID;
	LOOP {
		iam (DataModeler);
		DataModeler_setDataPointStatus (me, GET_INTEGER (U"Index"), status);
	}
END2 }

FORM3 (MODIFY_DataModeler_setDataPointValue, U"DataModeler: Set data point value", nullptr) {
	NATURAL (U"Index", U"1")
	REAL (U"Value", U"0.0")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setDataPointValue (me, GET_INTEGER (U"Index"), GET_REAL (U"Value"));
	}
END2 }

FORM3 (MODIFY_DataModeler_setDataPointSigma, U"DataModeler: Set data point sigma", nullptr) {
	NATURAL (U"Index", U"1")
	REAL (U"Sigma", U"10.0")
	OK2
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setDataPointSigma (me, GET_INTEGER (U"Index"), GET_REAL (U"Sigma"));
	}
END2 }

DIRECT3 (MODIFY_DataModeler_fitModel) {
	LOOP {
		iam (DataModeler);
		DataModeler_fit (me);
	}
END2 }

DIRECT3 (NEW_DataModeler_to_Covariance_parameters) {
	LOOP {
		iam (DataModeler);
		autoCovariance thee = DataModeler_to_Covariance_parameters (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM3 (NEW_DataModeler_to_Table_zscores, U"DataModeler: To Table (z-scores)", nullptr) {
	BOOLEAN (U"Use sigmas on y-values", 1)
	OK2
DO
	LOOP {
		iam (DataModeler);
		autoTable thee = DataModeler_to_Table_zscores (me, GET_INTEGER (U"Use sigmas on y-values"));
		praat_new (thee.move(), my name, U"_z");
	}
END2 }

FORM3 (NEW_Formant_to_FormantModeler, U"Formant: To FormantModeler", nullptr) {
//double tmin, double tmax, long numberOfFormants, long numberOfParametersPerTrack
	REAL (U"left Start time", U"0.0")
	REAL (U"right End time", U"0.1")
	NATURAL (U"Number of formants", U"3")
	INTEGER (U"Order of polynomials", U"3")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	OK2
DO
	long order = GET_INTEGER (U"Order of polynomials");
	REQUIRE (order >= 0, U"The order must be greater than or equal to zero.")
	LOOP {
		iam (Formant);
		autoFormantModeler thee = Formant_to_FormantModeler (me, GET_REAL (U"left Start time"), 
			GET_REAL (U"right End time"), GET_INTEGER (U"Number of formants"), order + 1, GET_INTEGER (U"Weigh data") - 1);
		praat_new (thee.move(), my name, U"_o", order);
	}
END2 }

FORM3 (NEW1_Formants_extractSmoothestPart, U"Formants: Extract smoothest part", U"Formants: Extract smoothest part") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	NATURAL (U"Number of formant tracks", U"4")
	INTEGER (U"Order of polynomials", U"3")
	LABEL (U"", U"Use bandwidths to model the formant tracks:")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Zero parameter values whose range include zero:")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	OK2
DO
	OrderedOf<structFormant> formants;
	LOOP {
		iam (Formant);
		formants. addItem_ref (me);
	}
	double tmin = GET_REAL (U"left Time range"), tmax = GET_REAL (U"right Time range");
	long index = Formants_getSmoothestInInterval (& formants, tmin, tmax, GET_INTEGER (U"Number of formant tracks"), GET_INTEGER (U"Order of polynomials") + 1,
		GET_INTEGER (U"Weigh data") - 1, 0, GET_REAL (U"Number of sigmas"), GET_REAL (U"Parameter variance power"), 1.0, 1.0, 1.0, 1.0, 1.0);
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
	autoFormant thee = Formant_extractPart (him, tmin, tmax);
	praat_new (thee.move(), his name, U"_part");
END2 }

FORM3 (NEW1_Formants_extractSmoothestPart_constrained, U"Formants: Extract smoothest part (constrained)", U"Formants: Extract smoothest part (constrained)...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	NATURAL (U"Number of formant tracks", U"4")
	INTEGER (U"Order of polynomials", U"3")
	LABEL (U"", U"Use bandwidths to model the formant tracks:")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Zero parameter values whose range include zero:")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	LABEL (U"", U"The constraints on the formants:")
	REAL (U"Minimum F1 (Hz)", U"100.0")
	REAL (U"Maximum F1 (Hz)", U"1200.0")
	REAL (U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (U"Minimum F3 (Hz)", U"1500.0")
	OK2
DO
	OrderedOf<structFormant> formants;
	LOOP {
		iam (Formant);
		formants. addItem_ref (me);
	}
	double tmin = GET_REAL (U"left Time range"), tmax = GET_REAL (U"right Time range");
	long index = Formants_getSmoothestInInterval (& formants, tmin, tmax, GET_INTEGER (U"Number of formant tracks"), GET_INTEGER (U"Order of polynomials") + 1,
		GET_INTEGER (U"Weigh data") - 1, 1, GET_REAL (U"Number of sigmas"), GET_REAL (U"Parameter variance power"),
		GET_REAL (U"Minimum F1"), GET_REAL (U"Maximum F1"), GET_REAL (U"Minimum F2"), GET_REAL (U"Maximum F2"), GET_REAL (U"Minimum F3"));
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
	autoFormant thee = Formant_extractPart (him, tmin, tmax);
	praat_new (thee.move(), his name, U"_part");
END2 }

/********************** FormantModeler ******************************/

FORM3 (GRAPHICS_FormantModeler_drawEstimatedTracks, U"FormantModeler: Draw estimated tracks", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (U"left Formant range", U"1")
	NATURAL (U"right Formant range", U"3")
	INTEGER (U"Order of polynomials for estimation", U"3")
	REAL (U"Horizontal offset (mm)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	long order = GET_INTEGER (U"Order of polynomials for estimation");
	REQUIRE (order >= 0, U"The order must be greater than or equal to zero.")
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawTracks (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 
			GET_REAL (U"Maximum frequency"), GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"), 1, order + 1, 
			GET_REAL (U"Horizontal offset"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (GRAPHICS_FormantModeler_drawTracks, U"FormantModeler: Draw tracks", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (U"left Formant range", U"1")
	NATURAL (U"right Formant range", U"3")
	REAL (U"Horizontal offset (mm)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	long order = 6;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawTracks (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 
			GET_REAL (U"Maximum frequency"), GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"),
			0, order + 1, GET_REAL (U"Horizontal offset"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (GRAPHICS_FormantModeler_speckle, U"FormantModeler: Speckle", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (U"left Formant range", U"1")
	NATURAL (U"right Formant range", U"3")
	BOOLEAN (U"Draw error bars", true)
	REAL (U"Bar width (mm)", U"1.0")
	REAL (U"Horizontal offset (mm)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	long order = 6;
	LOOP {
		iam (FormantModeler);
		FormantModeler_speckle (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 
			GET_REAL (U"Maximum frequency"), GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"),
			0, order + 1, GET_INTEGER (U"Draw error bars"), GET_REAL (U"Bar width"), GET_REAL (U"Horizontal offset"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (GRAPHICS_FormantModeler_drawOutliersMarked, U"FormantModeler: Draw outliers marked", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"Maximum frequency (Hz)", U"5500.0")
	NATURAL (U"left Formant range", U"1")
	NATURAL (U"right Formant range", U"3")
	POSITIVE (U"Number of sigmas", U"3.0")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	WORD (U"Mark", U"o")
	NATURAL (U"Mark font size", U"12")
	REAL (U"Horizontal offset (mm)", U"0.0")
	BOOLEAN (U"Garnish", false)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawOutliersMarked (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Maximum frequency"), GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"),
			GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Weigh data") - 1, GET_STRING (U"Mark"),
			GET_INTEGER (U"Mark font size"), GET_REAL (U"Horizontal offset"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (GRAPHICS_FormantModeler_drawVariancesOfShiftedTracks, U"FormantModeler: Draw variances of shifted tracks", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range", U"0.0")
	REAL (U"left Variance range", U"0.0")
	REAL (U"right Variance range", U"0.0")
	OPTIONMENU (U"Shift tracks", 1)
		OPTION (U"No")
		OPTION (U"Up")
		OPTION (U"Down")
	NATURAL (U"left Formant range", U"1")
	NATURAL (U"right Formant range", U"4")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawVariancesOfShiftedTracks (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Variance range"), GET_REAL (U"right Variance range"), GET_INTEGER (U"Shift tracks"), 
			GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (GRAPHICS_FormantModeler_drawCumulativeChiScores, U"FormantModeler: Draw cumulative chi scores", nullptr) {
	REAL (U"left Time range", U"0.0")
	REAL (U"right Time range", U"0.0")
	REAL (U"left Chisq range", U"0.0")
	REAL (U"right Chisq range", U"0.0")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawCumulativeChiScores (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 
			GET_REAL (U"left Chisq range"), GET_REAL (U"right Chisq range"), GET_INTEGER (U"Weigh data"), GET_INTEGER (U"Garnish"));
	}
END2 }


FORM3 (GRAPHICS_FormantModeler_normalProbabilityPlot, U"FormantModeler: Normal probability plot", nullptr) {
	NATURAL (U"Formant number", U"1")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	NATURAL (U"Number of quantiles", U"100")
	REAL (U"Number of sigmas", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", true);
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_normalProbabilityPlot (me, GRAPHICS, GET_INTEGER (U"Formant number"), GET_INTEGER (U"Weigh data") - 1,
			GET_INTEGER (U"Number of quantiles"),
			GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Label size"), GET_STRING (U"Label"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (GRAPHICS_FormantModeler_drawBasisFunction, U"FormantModeler: Draw basis function", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Amplitude range (Hz)", U"0.0")
	REAL (U"right Amplitude range (Hz)", U"5500.0")
	//long iterm, bool scaled, long numberOfPoints, int garnish
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Basis function", U"2")
	BOOLEAN (U"Scale function with parameter value", false)
	NATURAL (U"Number of points", U"200")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawBasisFunction (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),GET_INTEGER (U"Formant number"),
 			GET_INTEGER (U"Basis function"), GET_INTEGER (U"Scale function with parameter value"),
			GET_INTEGER (U"Number of points"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (REAL_FormantModeler_getModelValueAtTime, U"", nullptr) {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.1")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		double y = FormantModeler_getModelValueAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"));
		Melder_informationReal (y, U"Hertz");
	}
END2 }

FORM3 (REAL_FormantModeler_getDataPointValue, U"FormantModeler: Get data point value", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Index", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (FormantModeler);
		double value = FormantModeler_getDataPointValue (me, iformant, index);
		Melder_information (value, U" (= value of point ", index, U" in track F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_getDataPointSigma, U"FormantModeler: Get data point sigma", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Index", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (FormantModeler);
		double value = FormantModeler_getDataPointSigma (me, iformant, index);
		Melder_information (value, U" (= sigma of point ", index, U" in track F", iformant, U")");
	}
END2 }

FORM3 (INTEGER_FormantModeler_getDataPointStatus, U"FormantModeler: Get data point status", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Index", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (FormantModeler);
		int status = FormantModeler_getDataPointStatus (me, iformant, index);
		Melder_information (status == DataModeler_DATA_INVALID ? U"Invalid" : U"Valid");
	}
END2 }

DIRECT3 (INTEGER_FormantModeler_getNumberOfTracks) {
	LOOP {
		iam (FormantModeler);
		long nop = FormantModeler_getNumberOfTracks (me);
		Melder_information (nop, U" (= number of formants)");
	}
END2 }

FORM3 (INTEGER_FormantModeler_getNumberOfParameters, U"FormantModeler: Get number of parameters", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		long nop = FormantModeler_getNumberOfParameters (me, iformant);
		Melder_information (nop, U" (= number of parameters for F", iformant, U")");
	}
END2 }

FORM3 (INTEGER_FormantModeler_getNumberOfFixedParameters, U"FormantModeler: Get number of fixed parameters", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		long nop = FormantModeler_getNumberOfFixedParameters (me, iformant);
		Melder_information (nop, U" (= number of fixed parameters for F", iformant, U")");
	}
END2 }

DIRECT3 (INTEGER_FormantModeler_getNumberOfDataPoints) {
	LOOP {
		iam (FormantModeler);
		long numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		Melder_information (numberOfDataPoints);
	}
END2 }

FORM3 (INTEGER_FormantModeler_getNumberOfInvalidDataPoints, U"FormantModeler: Get number of invalid data points", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		long numberOfInvalidDataPoints = FormantModeler_getNumberOfInvalidDataPoints (me, iformant);
		Melder_information (numberOfInvalidDataPoints, U" (= number of invalid data points for F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_getParameterValue, U"FormantModeler: Get parameter value", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Parameter number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number"), iparameter = GET_INTEGER (U"Parameter number");
	LOOP {
		iam (FormantModeler);
		double parameter = FormantModeler_getParameterValue (me, iformant, iparameter);
		Melder_information (parameter, U" (= parameter[", iparameter, U"] for F", iformant, U")");
	}
END2 }

FORM3 (INTEGER_FormantModeler_getParameterStatus, U"FormantModeler: Get parameter status", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Parameter number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number"), iparameter = GET_INTEGER (U"Parameter number");
	LOOP {
		iam (FormantModeler);
		int status = FormantModeler_getParameterStatus (me, iformant, iparameter);
		Melder_information (status == DataModeler_PARAMETER_FREE ? U"Free" : (status == DataModeler_PARAMETER_FIXED ? U"Fixed" :
			U"Undefined"), U" (= parameter[", iparameter, U"] for F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_getParameterStandardDeviation, U"FormantModeler: Get parameter standard deviatio", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Parameter number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number"), iparameter = GET_INTEGER (U"Parameter number");
	LOOP {
		iam (FormantModeler);
		double sigma = FormantModeler_getParameterStandardDeviation (me, iformant, iparameter);
		Melder_information (sigma, U" (= parameter[", iparameter, U"] for F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_getVarianceOfParameters, U"FormantModeler: Get variance of parameters", nullptr) {
	INTEGER (U"left Formant range", U"0")
	INTEGER (U"right Formant range", U"0")
	INTEGER (U"left Parameter range", U"0")
	INTEGER (U"right Parameter range", U"0")
	OK2
DO
	long nofp;
	LOOP {
		iam (FormantModeler);
		double var = FormantModeler_getVarianceOfParameters (me, GET_INTEGER (U"left Formant range"), 
			GET_INTEGER (U"right Formant range"), GET_INTEGER (U"left Parameter range"), GET_INTEGER (U"right Parameter range"), &nofp);
		Melder_information (var, U" (for ", nofp, U" free parameters.)");
	}
END2 }

FORM3 (REAL_FormantModeler_getCoefficientOfDetermination, U"FormantModeler: Get coefficient of determination", nullptr) {
	INTEGER (U"left Formant range", U"0")
	INTEGER (U"right Formant range", U"0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		double rSquared = FormantModeler_getCoefficientOfDetermination (me, GET_INTEGER (U"left Formant range"), 
			GET_INTEGER (U"right Formant range"));
		Melder_informationReal (rSquared, U" (= R^2)");
	}
END2 }

FORM3 (REAL_FormantModeler_getResidualSumOfSquares, U"FormantModeler: Get residual sum of squares", U"FormantModeler: Get residual sum of squares...") {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long n, iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		double rss = FormantModeler_getResidualSumOfSquares (me, iformant, &n);
		Melder_information (rss, U" Hz^2,  (= RSS of F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_getStandardDeviation, U"FormantModeler: Get formant standard deviation", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		double sigma = FormantModeler_getStandardDeviation (me, iformant);
		Melder_information (sigma, U" Hz (= std. dev. of F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_reportChiSquared, U"FormantModeler: Report chi squared", nullptr) {
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		long numberOfFormants = my trackmodelers.size;
		int useSigmaY = GET_INTEGER (U"Weigh data") - 1;
		double chisq = 0, ndf = 0, probability;
		MelderInfo_open ();
		MelderInfo_writeLine (U"Chi squared tests for individual models of each of ", numberOfFormants, U" formant track:");
		MelderInfo_writeLine (useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? U"Standard deviation is estimated from the data." :
			useSigmaY == DataModeler_DATA_WEIGH_SIGMA ? U"\tBandwidths are used as estimate for local standard deviations." : 
			useSigmaY == DataModeler_DATA_WEIGH_RELATIVE ? U"\t1/Q's are used as estimate for local standard deviations." :
			U"\tSqrt bandwidths are used as estimate for local standard deviations.");
		for (long iformant = 1; iformant <= numberOfFormants; iformant ++) {
			chisq = FormantModeler_getChiSquaredQ (me, iformant, iformant, useSigmaY, &probability, &ndf);
			MelderInfo_writeLine (U"Formant track ", iformant, U":");
			MelderInfo_writeLine (U"\tChi squared (F", iformant, U") = ", chisq);
			MelderInfo_writeLine (U"\tProbability (F", iformant, U") = ", probability);
			MelderInfo_writeLine (U"\tNumber of degrees of freedom (F", iformant, U") = ", ndf);
		}
		chisq = FormantModeler_getChiSquaredQ (me, 1, numberOfFormants, useSigmaY, & probability, & ndf);
		MelderInfo_writeLine (U"Chi squared test for the complete model with ", numberOfFormants, U" formants:");
		MelderInfo_writeLine (U"\tChi squared = ", chisq);
		MelderInfo_writeLine (U"\tProbability = ", probability);
		MelderInfo_writeLine (U"\tNumber of degrees of freedom = ", ndf);
		MelderInfo_close ();
	}
END2 }

FORM3 (REAL_FormantModeler_getDegreesOfFreedom, U"FormantModeler: Get degrees of freedom", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		double sigma = FormantModeler_getDegreesOfFreedom (me, iformant);
		Melder_information (sigma, U" (= degrees of freedom of F", iformant, U")");
	}
END2 }

FORM3 (REAL_FormantModeler_getSmoothnessValue, U"FormantModeler: Get smoothness value", nullptr) {
	INTEGER (U"left Formant range", U"0")
	INTEGER (U"right Formant range", U"0")
	INTEGER (U"Order of polynomials", U"3")
	REAL (U"Parameter variance power", U"1.5")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		double smoothness = FormantModeler_getSmoothnessValue (me, GET_INTEGER (U"left Formant range"), 
			GET_INTEGER (U"right Formant range"), GET_INTEGER (U"Order of polynomials"), GET_REAL (U"Parameter variance power"));
		Melder_information (smoothness, U" (= smoothness)");
	}
END2 }

FORM3 (REAL_FormantModeler_getAverageDistanceBetweenTracks, U"FormantModeler: Get average distance between tracks", nullptr) {
	NATURAL (U"Track 1", U"2")
	NATURAL (U"Track 2", U"3")
	OPTIONMENU (U"Type of data", 1)
		OPTION (U"Data points")
		OPTION (U"Modeled")
	OK2
DO
	long track1 = GET_INTEGER (U"Track 1"), track2 = GET_INTEGER (U"Track 2");
	LOOP {
		iam (FormantModeler);
		double distance = FormantModeler_getAverageDistanceBetweenTracks (me, track1, track2, GET_INTEGER (U"Type of data") - 1);
		Melder_information (distance, U" (= average |F", track1, U" - F", track2, U"|)");
	}
END2 }

FORM3 (REAL_FormantModeler_getFormantsConstraintsFactor, U"FormantModeler: Get formants constraints factor", nullptr) {
	REAL (U"Minimum F1 (Hz)", U"100.0")
	REAL (U"Maximum F1 (Hz)", U"1200.0")
	REAL (U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (U"Minimum F3 (Hz)", U"1500.0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		double fc = FormantModeler_getFormantsConstraintsFactor (me, GET_REAL (U"Minimum F1"), GET_REAL (U"Maximum F1"),
		GET_REAL (U"Minimum F2"), GET_REAL (U"Maximum F2"), GET_REAL (U"Minimum F3"));
		Melder_information (fc, U" (= formants constraints factor)");
	}
END2 }

FORM3 (MODIFY_FormantModeler_setDataWeighing, U"FormantModeler: Set data weighing", nullptr) {
	INTEGER (U"left Formant range", U"0")
	INTEGER (U"right Formant range", U"0")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataWeighing (me, GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"), 
			GET_INTEGER (U"Weigh data") - 1);
	}
END2 }

FORM3 (MODIFY_FormantModeler_setTolerance, U"FormantModeler: Set tolerance", nullptr) {
	REAL (U"Tolerance", U"1e-5")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setTolerance (me, GET_REAL (U"Tolerance"));
	}
END2 }

FORM3 (MODIFY_FormantModeler_setParameterValueFixed, U"FormantModeler: Set parameter value fixed", nullptr) {
	NATURAL (U"Formant number", U"1")
	NATURAL (U"Parameter number", U"1")
	REAL (U"Value", U"0.0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setParameterValueFixed (me, GET_INTEGER (U"Formant number"), GET_INTEGER (U"Parameter number"), GET_REAL (U"Value"));
	}
END2 }


FORM3 (MODIFY_FormantModeler_setParameterFree, U"FormantModeler: Set parameter free", nullptr) {
	INTEGER (U"left Formant range", U"0")
	INTEGER (U"right Formant range", U"0")
	INTEGER (U"left Parameter range", U"0")
	INTEGER (U"right Parameter range", U"0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setParametersFree (me, GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"),
			GET_INTEGER (U"left Parameter range"), GET_INTEGER (U"right Parameter range"));
	}
END2 }

FORM3 (MODIFY_FormantModeler_setParameterValuesToZero, U"FormantModeler: Set parameter values to zero", nullptr) {
	INTEGER (U"left Formant range", U"0")
	INTEGER (U"right Formant range", U"0")
	REAL (U"Number of sigmas", U"1.0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setParameterValuesToZero (me, GET_INTEGER (U"left Formant range"),  GET_INTEGER (U"right Formant range"),
			 GET_REAL (U"Number of sigmas"));
	}
END2 }

FORM3 (MODIFY_FormantModeler_setDataPointValue, U"FormantModeler: Set data point value", nullptr) {
	NATURAL (U"Formant index", U"1")
	NATURAL (U"Data index", U"1")
	REAL (U"Value", U"1.0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataPointValue (me, GET_INTEGER (U"Formant index"),  GET_INTEGER (U"Data index"),
			 GET_REAL (U"Value"));
	}
END2 }

FORM3 (MODIFY_FormantModeler_setDataPointSigma, U"FormantModeler: Set data point sigma", nullptr) {
	NATURAL (U"Formant index", U"1")
	NATURAL (U"Data index", U"1")
	REAL (U"Sigma", U"10.0")
	OK2
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataPointSigma (me, GET_INTEGER (U"Formant index"),  GET_INTEGER (U"Data index"),
			 GET_REAL (U"Sigma"));
	}
END2 }

FORM3 (MODIFY_FormantModeler_setDataPointStatus, U"FormantModeler: Set data point status", nullptr) {
	NATURAL (U"Formant index", U"1")
	NATURAL (U"Data index", U"1")
	OPTIONMENU (U"Status", 1)
		OPTION (U"Valid")
		OPTION (U"Invalid")
	OK2
DO
	int menustatus = GET_INTEGER (U"Status");
	int status = menustatus == 2 ? DataModeler_DATA_INVALID : DataModeler_DATA_VALID;
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataPointStatus (me, GET_INTEGER (U"Formant index"),  GET_INTEGER (U"Data index"), status);
	}
END2 }

DIRECT3 (MODIFY_FormantModeler_fitModel) {
	LOOP {
		iam (FormantModeler);
		FormantModeler_fit (me);
	}
END2 }

FORM3 (NEW_FormantModeler_to_Covariance_parameters, U"", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		autoCovariance thee = FormantModeler_to_Covariance_parameters (me, iformant);
		praat_new (thee.move(), my name, U"_", iformant);
	}
END2 }

FORM3 (NEW_FormantModeler_extractDataModeler, U"FormantModeler: Extract DataModeler", nullptr) {
	NATURAL (U"Formant number", U"1")
	OK2
DO
	long iformant = GET_INTEGER (U"Formant number");
	LOOP {
		iam (FormantModeler);
		autoDataModeler thee = FormantModeler_extractDataModeler (me, iformant);
		praat_new (thee.move(), my name, U"_", iformant);
	}
END2 }

FORM3 (NEW_FormantModeler_to_Table_zscores, U"", nullptr) {
	BOOLEAN (U"Bandwidths as standard deviation", true)
	OK2
DO
	LOOP {
		iam (FormantModeler);
		autoTable thee = FormantModeler_to_Table_zscores (me, GET_INTEGER (U"Bandwidths as standard deviation"));
		praat_new (thee.move(), my name, U"_z");
	}
END2 }

FORM3 (NEW_FormantModeler_processOutliers, U"", nullptr) {
	POSITIVE (U"Number of sigmas", U"3.0")
	BOOLEAN (U"Bandwidths as standard deviation", true)
	OK2
DO
	LOOP {
		iam (FormantModeler);
		autoFormantModeler thee = FormantModeler_processOutliers (me, GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Bandwidths as standard deviation"));
		praat_new (thee.move(), my name, U"_outliers");
	}
END2 }


DIRECT3 (WINDOW_OptimalCeilingTier_edit) {
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
END2 }


/*************************** PitchModeler *************************************/

FORM3 (NEW_Pitch_to_PitchModeler, U"Pitch: To PitchModeler", nullptr) {
	REAL (U"left Start time (s)", U"0.0")
	REAL (U"right End time (s)", U"0.1")
	INTEGER (U"Order of polynomials", U"2")
	OK2
DO
	LOOP {
		iam (Pitch);
		autoPitchModeler thee = Pitch_to_PitchModeler (me, GET_REAL (U"left Start time"), GET_REAL (U"right End time"), 
			GET_INTEGER (U"Order of polynomials") + 1);
		praat_new (thee.move(), my name);
	}
END2 }

FORM3 (GRAPHICS_PitchModeler_draw, U"PitchModeler: Draw", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"500.0")
	INTEGER (U"Order of polynomial for estimation", U"2")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (PitchModeler);
		PitchModeler_draw (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"), GET_INTEGER (U"Order of polynomial for estimation") + 1,  GET_INTEGER (U"Garnish"));
	}
END2 }

FORM3 (REAL_Sound_getOptimalFormantCeiling, U"Sound: Get optimal formant ceiling", nullptr) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range (s)", U"0.15")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.0025")
	POSITIVE (U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (U"Number of frequency steps", U"11")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (U"Number of formant tracks in model", U"4")
	INTEGER (U"Order of polynomials", U"3")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	OK2
DO
	LOOP {
		iam (Sound);
		double ceiling = Sound_getOptimalFormantCeiling (me, 
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Window length"), GET_REAL (U"Time step"), GET_REAL (U"left Maximum frequency range"), 
			GET_REAL (U"right Maximum frequency range"), GET_INTEGER (U"Number of frequency steps"), 
			GET_REAL (U"Pre-emphasis from"), 
			GET_INTEGER (U"Number of formant tracks in model"), GET_INTEGER (U"Order of polynomials") + 1,
			GET_INTEGER (U"Weigh data") - 1, GET_REAL (U"Number of sigmas"), GET_REAL (U"Parameter variance power"));
	Melder_informationReal (ceiling, U" Hz");
	}
END2 }

FORM3 (NEW_Sound_to_Formant_interval, U"Sound: To Formant (interval)", nullptr) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range (s)", U"0.15")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.0025")
	POSITIVE (U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (U"Number of frequency steps", U"11")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (U"Number of formant tracks in model", U"4")
	INTEGER (U"Order of polynomials", U"3")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	OK2
DO
	LOOP {
		iam (Sound);
		double ceiling;
		autoFormant formant = Sound_to_Formant_interval (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Window length"), GET_REAL (U"Time step"), GET_REAL (U"left Maximum frequency range"), 
			GET_REAL (U"right Maximum frequency range"), GET_INTEGER (U"Number of frequency steps"), 
			GET_REAL (U"Pre-emphasis from"), 
			GET_INTEGER (U"Number of formant tracks in model"), GET_INTEGER (U"Order of polynomials") + 1,
			GET_INTEGER (U"Weigh data") - 1, GET_REAL (U"Number of sigmas"), GET_REAL (U"Parameter variance power"),
			0, 1, 1, 1, 1, 1, &ceiling);
		praat_new (formant.move(), my name, U"_", Melder_fixed (ceiling, 0));
	}
END2 }

FORM3 (NEW_Sound_to_Formant_interval_constrained, U"Sound: To Formant (interval, constrained)", nullptr) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range (s)", U"0.15")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.0025")
	POSITIVE (U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (U"Number of frequency steps", U"11")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (U"Number of formant tracks in model", U"4")
	INTEGER (U"Order of polynomials", U"3")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	LABEL (U"", U"Formant frequency constraints")
	REAL (U"Minimum F1 (Hz)", U"100.0")
	REAL (U"Maximum F1 (Hz)", U"1200.0")
	REAL (U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (U"Minimum F3 (Hz)", U"1000.0")
	OK2
DO
	LOOP {
		iam (Sound);
		double ceiling;
		autoFormant formant = Sound_to_Formant_interval (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Window length"), GET_REAL (U"Time step"), GET_REAL (U"left Maximum frequency range"), 
			GET_REAL (U"right Maximum frequency range"), GET_INTEGER (U"Number of frequency steps"),
			GET_REAL (U"Pre-emphasis from"), GET_INTEGER (U"Number of formant tracks in model"), 
			GET_INTEGER (U"Order of polynomials") + 1, GET_INTEGER (U"Weigh data") - 1, GET_REAL (U"Number of sigmas"),
			GET_REAL (U"Parameter variance power"), 1,
			GET_REAL (U"Minimum F1"), GET_REAL (U"Maximum F1"), GET_REAL (U"Minimum F2"), 
			GET_REAL (U"Maximum F2"), GET_REAL (U"Minimum F3"), &ceiling);
		praat_new (formant.move(), my name, U"_", Melder_fixed (ceiling, 0));
	}
END2 }

FORM3 (NEW_Sound_to_Formant_interval_constrained_robust, U"Sound: To Formant (interval, constrained, robust)", nullptr) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range (s)", U"0.15")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.0025")
	POSITIVE (U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (U"Number of frequency steps", U"11")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	NATURAL (U"Number of formant tracks in model", U"4")
	INTEGER (U"Order of polynomials", U"3")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	LABEL (U"", U"Formant frequency constraints")
	REAL (U"Minimum F1 (Hz)", U"100.0")
	REAL (U"Maximum F1 (Hz)", U"1200.0")
	REAL (U"Minimum F2 (Hz)", U"0.0")
	POSITIVE (U"Maximum F2 (Hz)", U"5000.0")
	POSITIVE (U"Minimum F3 (Hz)", U"1000.0")
	OK2
DO
	LOOP {
		iam (Sound);
		double ceiling;
		autoFormant formant = Sound_to_Formant_interval_robust (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Window length"), GET_REAL (U"Time step"), GET_REAL (U"left Maximum frequency range"), 
			GET_REAL (U"right Maximum frequency range"), GET_INTEGER (U"Number of frequency steps"),
			GET_REAL (U"Pre-emphasis from"), GET_INTEGER (U"Number of formant tracks in model"), 
			GET_INTEGER (U"Order of polynomials") + 1, GET_INTEGER (U"Weigh data") - 1, GET_REAL (U"Number of sigmas"),
			GET_REAL (U"Parameter variance power"), 1,
			GET_REAL (U"Minimum F1"), GET_REAL (U"Maximum F1"), GET_REAL (U"Minimum F2"), 
			GET_REAL (U"Maximum F2"), GET_REAL (U"Minimum F3"), &ceiling);
		praat_new (formant.move(), my name, U"_", Melder_fixed (ceiling, 0));
	}
END2 }

FORM3 (NEW_Sound_to_OptimalCeilingTier, U"", nullptr) {
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.0025")
	POSITIVE (U"left Maximum frequency range (Hz)", U"4500.0")
	POSITIVE (U"right Maximum frequency range (Hz)", U"6500.0")
	NATURAL (U"Number of frequency steps", U"11")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	REAL (U"Formant smoothing window (s)", U"0.05")
	NATURAL (U"Number of formant tracks in model", U"4")
	INTEGER (U"Order of polynomials", U"2")
	OPTIONMENU (U"Weigh data", 2)
		OPTION (U"Equally")
		OPTION (U"Bandwidth")
		OPTION (U"Bandwidth / frequency")
		OPTION (U"Sqrt bandwidth")
	LABEL (U"", U"Make parameters that include zero in their confidence region zero")
	REAL (U"Number of sigmas", U"1.0")
	REAL (U"Parameter variance power", U"1.5")
	OK2
DO
	LOOP {
		iam (Sound);
		autoOptimalCeilingTier octier = Sound_to_OptimalCeilingTier (me, GET_REAL (U"Window length"), GET_REAL (U"Time step"), 
			GET_REAL (U"left Maximum frequency range"), GET_REAL (U"right Maximum frequency range"), GET_INTEGER (U"Number of frequency steps"),
			GET_REAL (U"Pre-emphasis from"), GET_REAL (U"Formant smoothing window"), GET_INTEGER (U"Number of formant tracks in model"),
			GET_INTEGER (U"Order of polynomials") + 1, GET_INTEGER (U"Weigh data") - 1, GET_REAL (U"Number of sigmas"), GET_REAL (U"Parameter variance power"));
		praat_new (octier.move(), my name);
	}
END2 }

FORM3 (NEW_Table_to_DataModeler, U"", nullptr) {
	REAL (U"left X range", U"0.0")
	REAL (U"right X range", U"0.0 (= auto)")
	WORD (U"Column with X data", U"")
	WORD (U"Column with Y data", U"")
	WORD (U"Column with sigmas", U"")
	OPTIONMENU (U"Model functions", 1)
	OPTION (U"Legendre polynomials")
	INTEGER (U"Maximum order", U"3")
	OK2
DO
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column with X data"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column with Y data"));
		long scolumn = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Column with sigmas"));
		autoDataModeler thee = Table_to_DataModeler (me, GET_REAL (U"left X range"), GET_REAL (U"right X range"),
			xcolumn, ycolumn, scolumn, GET_INTEGER (U"Maximum order") + 1, GET_INTEGER (U"Model functions"));
		praat_new (thee.move(), my name);
	}
END2 }

void praat_DataModeler_init ();
void praat_DataModeler_init () {
	Thing_recognizeClassesByName (classDataModeler, classFormantModeler, classOptimalCeilingTier, classOptimalCeilingTierEditor, classPitchModeler, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Create simple DataModeler...", U"Create ISpline...", praat_HIDDEN + praat_DEPTH_1, NEW_DataModeler_createSimple);

	praat_addAction1 (classDataModeler, 0, U"Speckle...", 0, 0, GRAPHICS_DataModeler_speckle);
	praat_addAction1 (classDataModeler, 0, U"Draw estimated track...", 0, 0, GRAPHICS_DataModeler_drawEstimatedTrack);

	praat_addAction1 (classDataModeler, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, U"Get number of parameters", 0, 1, INTEGER_DataModeler_getNumberOfParameters);
		praat_addAction1 (classDataModeler, 0, U"Get number of fixed parameters", 0, 1, INTEGER_DataModeler_getNumberOfFixedParameters);
		praat_addAction1 (classDataModeler, 0, U"Get parameter value...", 0, 1, REAL_DataModeler_getParameterValue);
		praat_addAction1 (classDataModeler, 0, U"Get parameter status...", 0, 1, INTEGER_DataModeler_getParameterStatus);
		praat_addAction1 (classDataModeler, 0, U"Get parameter standard deviation...", 0, 1, REAL_DataModeler_getParameterStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get variance of parameters...", 0, 1, REAL_DataModeler_getVarianceOfParameters);
		praat_addAction1 (classDataModeler, 1, U"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Get model value at x...", 0, 1, REAL_DataModeler_getModelValueAtX);
		praat_addAction1 (classDataModeler, 0, U"Get number of data points", 0, 1, INTEGER_DataModeler_getNumberOfDataPoints);
		praat_addAction1 (classDataModeler, 0, U"Get number of invalid data points", 0, 1, INTEGER_DataModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classDataModeler, 0, U"Get data point value...", 0, 1, REAL_DataModeler_getDataPointValue);
		praat_addAction1 (classDataModeler, 0, U"Get data point sigma...", 0, 1, REAL_DataModeler_getDataPointSigma);
		praat_addAction1 (classDataModeler, 0, U"Get data point status...", 0, 1, INTEGER_DataModeler_getDataPointStatus);
		praat_addAction1 (classDataModeler, 1, U"-- get statistics info --", 0, 1, 0);
		
		praat_addAction1 (classDataModeler, 0, U"Get residual sum of squares", 0, 1, REAL_DataModeler_getResidualSumOfSquares);
		praat_addAction1 (classDataModeler, 0, U"Get data standard deviation", 0, 1, REAL_DataModeler_getStandardDeviation);
		praat_addAction1 (classDataModeler, 0, U"Get coefficient of determination", 0, 1, REAL_DataModeler_getCoefficientOfDetermination);
		praat_addAction1 (classDataModeler, 0, U"Report chi squared...", 0, 1, REAL_DataModeler_reportChiSquared);
		praat_addAction1 (classDataModeler, 0, U"Get degrees of freedom", 0, 1, REAL_DataModeler_getDegreesOfFreedom);

	praat_addAction1 (classDataModeler, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, U"Set data weighing...", 0, 1, MODIFY_DataModeler_setDataWeighing);
		praat_addAction1 (classDataModeler, 0, U"Set tolerance...", 0, 1, MODIFY_DataModeler_setTolerance);
		praat_addAction1 (classDataModeler, 1, U"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Set parameter value...", 0, 1, MODIFY_DataModeler_setParameterValue);
		praat_addAction1 (classDataModeler, 0, U"Set parameter free...", 0, 1, MODIFY_DataModeler_setParameterFree);
		praat_addAction1 (classDataModeler, 0, U"Set parameter values to zero...", 0, 1, MODIFY_DataModeler_setParameterValuesToZero);
		praat_addAction1 (classDataModeler, 1, U"-- set data values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, U"Set data point status...", 0, 1, MODIFY_DataModeler_setDataPointStatus);
		praat_addAction1 (classDataModeler, 0, U"Set data point value...", 0, 1, MODIFY_DataModeler_setDataPointValue);
		praat_addAction1 (classDataModeler, 0, U"Set data point sigma...", 0, 1, MODIFY_DataModeler_setDataPointSigma);
		
	praat_addAction1 (classDataModeler, 0, U"Fit model", 0, 0, MODIFY_DataModeler_fitModel);
	
	praat_addAction1 (classDataModeler, 0, U"To Covariance (parameters)...", 0, 0, NEW_DataModeler_to_Covariance_parameters);
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
		praat_addAction1 (classFormantModeler, 0, U"Get parameter status...", 0, 1, INTEGER_FormantModeler_getParameterStatus);
		praat_addAction1 (classFormantModeler, 0, U"Get parameter standard deviation...", 0, 1, REAL_FormantModeler_getParameterStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get variance of parameters...", 0, 1, REAL_FormantModeler_getVarianceOfParameters);
		praat_addAction1 (classFormantModeler, 1, U"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get number of data points", 0, 1, INTEGER_FormantModeler_getNumberOfDataPoints);
		praat_addAction1 (classFormantModeler, 0, U"Get number of invalid data points...", 0, praat_DEPTH_1 + praat_HIDDEN, INTEGER_FormantModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classFormantModeler, 0, U"Get model value at time...", 0, 1, REAL_FormantModeler_getModelValueAtTime);
		praat_addAction1 (classFormantModeler, 0, U"Get data point value...", 0, 1, REAL_FormantModeler_getDataPointValue);
		praat_addAction1 (classFormantModeler, 0, U"Get data point sigma...", 0, 1, REAL_FormantModeler_getDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, U"Get data point status...", 0, 1, INTEGER_FormantModeler_getDataPointStatus);

		praat_addAction1 (classFormantModeler, 1, U"-- get statistics info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, U"Get residual sum of squares...", 0, 1, REAL_FormantModeler_getResidualSumOfSquares);
		praat_addAction1 (classFormantModeler, 0, U"Get formant standard deviation...", 0, 1, REAL_FormantModeler_getStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, U"Get coefficient of determination...", 0, 1, REAL_FormantModeler_getCoefficientOfDetermination);
		praat_addAction1 (classFormantModeler, 0, U"Report chi squared...", 0, 1, REAL_FormantModeler_reportChiSquared);
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

/* End of file praat_DataModeler_init.cpp */
