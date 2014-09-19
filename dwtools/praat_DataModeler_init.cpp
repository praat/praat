/* praat_DataModeler_init.cpp
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

#include <math.h>
#include "praatP.h"
#include "DataModeler.h"
#include "Formant_extensions.h"
#include "Pitch.h"
#include "Table_extensions.h"

#undef iam
#define iam iam_LOOP

FORM (DataModeler_speckle, L"DataModeler: Speckle", 0)
	REAL (L"left X range", L"0.0")
	REAL (L"right X range", L"0.0")
	REAL (L"left Y range", L"0.0")
	REAL (L"right Y range", L"0.0")
	BOOLEAN (L"Draw error bars", 1)
	REAL (L"Bar width (mm)", L"1.0")
	REAL (L"Horizontal offset (mm)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	long order = 6;
	LOOP {
		iam (DataModeler);
		DataModeler_speckle (me, GRAPHICS, GET_REAL (L"left X range"), GET_REAL (L"right X range"),
			GET_REAL (L"left Y range"), GET_REAL (L"right Y range"),
			0, order + 1, GET_INTEGER (L"Draw error bars"), GET_REAL (L"Bar width"), GET_REAL (L"Horizontal offset"),
			GET_INTEGER (L"Garnish"));
	}
END


FORM (DataModeler_drawEstimatedTrack, L"DataModeler: Draw estimated track", 0)
	REAL (L"left X range", L"0.0")
	REAL (L"right X range", L"0.0")
	REAL (L"left Y range", L"0.0")
	REAL (L"right Y range", L"0.0")
	INTEGER (L"Order of polynomials for estimation", L"3")
	REAL (L"Horizontal offset (mm)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	long order = GET_INTEGER (L"Order of polynomials for estimation");
	REQUIRE (order >= 0, L"The order must be greater than or equal to zero.")
	LOOP {
		iam (DataModeler);
		DataModeler_drawTrack (me, GRAPHICS, GET_REAL (L"left X range"), GET_REAL (L"right X range"),
			GET_REAL (L"left Y range"), GET_REAL (L"right Y range"), 1, order + 1, GET_REAL (L"Horizontal offset"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (DataModeler_getNumberOfParameters)
	LOOP {
		iam (DataModeler);
		Melder_information (Melder_integer (my numberOfParameters), L" (= number of parameters)");
	}
END

DIRECT (DataModeler_getNumberOfFixedParameters)
	LOOP {
		iam (DataModeler);
		Melder_information (Melder_integer (DataModeler_getNumberOfFixedParameters (me)), L" (= number of parameters)");
	}
END

FORM (DataModeler_getParameterValue, L"DataModeler: Get parameter value", 0)
	NATURAL (L"Parameter number", L"1")
	OK
DO
	long iparameter = GET_INTEGER (L"Parameter number");
	LOOP {
		iam (DataModeler);
		double parameter = DataModeler_getParameterValue (me, iparameter);
		Melder_information (Melder_double (parameter), L" (= parameter[", Melder_integer (iparameter), L"])");
	}
END

FORM (DataModeler_getParameterStatus, L"DataModeler: Get parameter status", 0)
	NATURAL (L"Parameter number", L"1")
	OK
DO
	long iparameter = GET_INTEGER (L"Parameter number");
	LOOP {
		iam (DataModeler);
		int status = DataModeler_getParameterStatus (me, iparameter);
		Melder_information (status == DataModeler_PARAMETER_FREE ? L"Free" : (status == DataModeler_PARAMETER_FIXED ? L"Fixed" :
			L"Undefined"), L" (= parameter[", Melder_integer (iparameter), L"])");
	}
END

FORM (DataModeler_getParameterStandardDeviation, L"DataModeler: Get parameter standard deviatio", 0)
	NATURAL (L"Parameter number", L"1")
	OK
DO
	long iparameter = GET_INTEGER (L"Parameter number");
	LOOP {
		iam (DataModeler);
		double sigma = DataModeler_getParameterStandardDeviation (me, iparameter);
		Melder_information (Melder_double (sigma), L" (= parameter[", Melder_integer (iparameter), L"])");
	}
END

FORM (DataModeler_getVarianceOfParameters, L"DataModeler: Get variance of parameters", 0)
	INTEGER (L"left Parameter range", L"0")
	INTEGER (L"right Parameter range", L"0")
	OK
DO
	long nofp;
	LOOP {
		iam (DataModeler);
		double var = DataModeler_getVarianceOfParameters (me, GET_INTEGER (L"left Parameter range"), GET_INTEGER (L"right Parameter range"), &nofp);
		Melder_information (Melder_double (var), L" (for ", Melder_integer(nofp), L" free parameters.)");
	}
END

DIRECT (DataModeler_getNumberOfDataPoints)
	LOOP {
		iam (DataModeler);
		Melder_information (Melder_integer (my numberOfDataPoints), L" (= number of data points)");
	}
END

DIRECT (DataModeler_getNumberOfInvalidDataPoints)
	LOOP {
		iam (DataModeler);
		Melder_information (Melder_integer (DataModeler_getNumberOfInvalidDataPoints (me)), L" (= number of invalid data points)");
	}
END

FORM (DataModeler_getModelValueAtX, L"DataModeler: Get model value at x", 0)
	REAL (L"X", L"0.1")
	OK
DO
	LOOP {
		iam (DataModeler);
		double y = DataModeler_getModelValueAtX (me, GET_REAL (L"X"));
		Melder_informationReal (y, NULL);
	}
END


DIRECT (DataModeler_getResidualSumOfSquares)
	LOOP {
		long n;
		iam (DataModeler);
		double rss = DataModeler_getResidualSumOfSquares (me, &n);
		Melder_information (Melder_double (rss), L"  (for ", Melder_integer (n), L" datapoints)");
	}
END

DIRECT (DataModeler_getStandardDeviation)
	LOOP {
		iam (DataModeler);
		double sigma = DataModeler_estimateSigmaY (me);
		Melder_information (Melder_double (sigma), NULL);
	}
END

FORM (DataModeler_getDataPointValue, L"DataModeler: Get data point value", 0)
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (DataModeler);
		double value = DataModeler_getDataPointValue (me, index);
		Melder_information (Melder_double (value), L" (= value at point ", Melder_integer (index), L")");
	}
END

FORM (DataModeler_getDataPointSigma, L"DataModeler: Get data point sigma", 0)
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (DataModeler);
		double sigma = DataModeler_getDataPointSigma (me, index);
		Melder_information (Melder_double (sigma), L" (= sigma at point ", Melder_integer (index), L")");
	}
END

FORM (DataModeler_getDataPointStatus, L"DataModeler: Get data point status", 0)
	NATURAL (L"Index", L"1")
	OK
DO
	LOOP {
		iam (DataModeler);
		int status = DataModeler_getDataPointStatus (me, GET_INTEGER (L"Index"));
		Melder_information (status == DataModeler_DATA_INVALID ? L"Invalid" : L"Valid");
	}
END

DIRECT (DataModeler_getCoefficientOfDetermination)
	LOOP {
		iam (DataModeler);
		double rSquared = DataModeler_getCoefficientOfDetermination (me, NULL, NULL);
		Melder_informationReal (rSquared, L" (= R^2)");
	}
END

FORM (DataModeler_reportChiSquared, L"DataModeler: Report chi squared", 0)
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Sigma")
		OPTION (L"Relative")
		OPTION (L"Sqrt sigma")
	OK
DO
	LOOP {
		iam (DataModeler);
		int useSigmaY = GET_INTEGER (L"Weigh data") - 1;
		MelderInfo_open ();
		MelderInfo_writeLine (L"Chi squared test:");
		MelderInfo_writeLine (useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? L"Standard deviation is estimated from the data." :
			useSigmaY == DataModeler_DATA_WEIGH_SIGMA ? L"Sigmas are used as estimate for local standard deviations." : 
			useSigmaY == DataModeler_DATA_WEIGH_RELATIVE ? L"1/Q's are used as estimate for local standard deviations." :
			L"Sqrt sigmas are used as estimate for local standard deviations.");
		double  ndf, probability, chisq = DataModeler_getChiSquaredQ (me, useSigmaY, &probability, &ndf);
		MelderInfo_writeLine (L"Chi squared = ", Melder_double (chisq));
		MelderInfo_writeLine (L"Probability = ", Melder_double (probability));
		MelderInfo_writeLine (L"Number of degrees of freedom = ", Melder_double (ndf));
		MelderInfo_close ();
	}
END

DIRECT (DataModeler_getDegreesOfFreedom)
	LOOP {
		iam (DataModeler);
		double dof = DataModeler_getDegreesOfFreedom (me);
		Melder_informationReal (dof, L" (= degrees of freedom)");
	}
END

FORM (DataModeler_setDataWeighing, L"DataModeler: Set data weighing", 0)
	OPTIONMENU (L"Weigh data", 1)
		OPTION (L"Equally")
		OPTION (L"Sigma")
		OPTION (L"Relative")
		OPTION (L"Sqrt sigma")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setDataWeighing (me, GET_INTEGER (L"Weigh data") - 1);
	}
END

FORM (DataModeler_setTolerance, L"DataModeler: Set tolerance", 0)
	REAL (L"Tolerance", L"1e-5")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setTolerance (me, GET_REAL (L"Tolerance"));
	}
END

FORM (DataModeler_setParameterValueFixed, L"DataModeler: Set parameter value fixed", 0)
	NATURAL (L"Parameter number", L"1")
	REAL (L"Value", L"0.0")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setParameterValueFixed (me, GET_INTEGER (L"Parameter number"), GET_REAL (L"Value"));
	}
END

FORM (DataModeler_setParameterFree, L"DataModeler: Set parameter free", 0)
	INTEGER (L"left Parameter range", L"0")
	INTEGER (L"right Parameter range", L"0")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setParametersFree (me, GET_INTEGER (L"left Parameter range"), GET_INTEGER (L"right Parameter range"));
	}
END

FORM (DataModeler_setParameterValuesToZero, L"DataModeler: Set parameter values to zero", 0)
	REAL (L"Number of sigmas", L"1.0")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setParameterValuesToZero (me, GET_REAL (L"Number of sigmas"));
	}
END

FORM (DataModeler_setDataPointStatus, L"DataModeler: Set data point status", 0)
	NATURAL (L"Index", L"1")
	OPTIONMENU (L"Status", 1)
		OPTION (L"Valid")
		OPTION (L"Invalid")
	OK
DO
	int menustatus = GET_INTEGER (L"Status");
	int status = menustatus == 2 ? DataModeler_DATA_INVALID : DataModeler_DATA_VALID;
	LOOP {
		iam (DataModeler);
		DataModeler_setDataPointStatus (me, GET_INTEGER (L"Index"), status);
	}
END

FORM (DataModeler_setDataPointValue, L"DataModeler: Set data point value", 0)
	NATURAL (L"Index", L"1")
	REAL (L"Value", L"0.0")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setDataPointValue (me, GET_INTEGER (L"Index"), GET_REAL (L"Value"));
	}
END

FORM (DataModeler_setDataPointSigma, L"DataModeler: Set data point sigma", 0)
	NATURAL (L"Index", L"1")
	REAL (L"Sigma", L"10.0")
	OK
DO
	LOOP {
		iam (DataModeler);
		DataModeler_setDataPointSigma (me, GET_INTEGER (L"Index"), GET_REAL (L"Sigma"));
	}
END

DIRECT (DataModeler_fitModel)
	LOOP {
		iam (DataModeler);
		DataModeler_fit (me);
	}
END

DIRECT (DataModeler_to_Covariance_parameters)
	LOOP {
		iam (DataModeler);
		autoCovariance thee = DataModeler_to_Covariance_parameters (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (DataModeler_to_Table_zscores, L"DataModeler: To Table (z-scores)", 0)
	BOOLEAN (L"Use sigmas on y-values", 1)
	OK
DO
	LOOP {
		iam (DataModeler);
		autoTable thee = DataModeler_to_Table_zscores (me, GET_INTEGER (L"Use sigmas on y-values"));
		praat_new (thee.transfer(), my name, L"_z");
	}
END

FORM (Formant_to_FormantModeler, L"Formant: To FormantModeler", 0)
//double tmin, double tmax, long numberOfFormants, long numberOfParametersPerTrack
	REAL (L"left Start time", L"0.0")
	REAL (L"right End time", L"0.1")
	NATURAL (L"Number of formants", L"3")
	INTEGER (L"Order of polynomials", L"3")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	OK
DO
	long order = GET_INTEGER (L"Order of polynomials");
	REQUIRE (order >= 0, L"The order must be greater than or equal to zero.")
	LOOP {
		iam (Formant);
		autoFormantModeler thee = Formant_to_FormantModeler (me, GET_REAL (L"left Start time"), GET_REAL (L"right End time"),
			GET_INTEGER (L"Number of formants"), order + 1, GET_INTEGER (L"Weigh data") - 1);
		praat_new (thee.transfer(), my name, L"_o", Melder_integer (order));
	}
END

FORM (Formants_extractSmoothestPart, L"Formants: Extract smoothest part", L"Formants: Extract smoothest part")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	NATURAL (L"Number of formant tracks", L"4")
	INTEGER (L"Order of polynomials", L"3")
	LABEL (L"", L"Use bandwidths to model the formant tracks")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	LABEL (L"", L"Zero parameter values whose range include zero.")
	REAL (L"Number of sigmas", L"1.0")
	POSITIVE (L"Parameter variance power", L"1.5")
	OK
DO
	autoCollection set = praat_getSelectedObjects ();
	double tmin = GET_REAL (L"left Time range"), tmax = GET_REAL (L"right Time range");
	long index = Formants_getSmoothestInInterval (set.peek(), tmin, tmax, GET_INTEGER (L"Number of formant tracks"),
		GET_INTEGER (L"Order of polynomials") + 1,
		GET_INTEGER (L"Weigh data") - 1, GET_REAL (L"Number of sigmas"), GET_REAL (L"Parameter variance power"), 0, 1, 1, 1, 1, 1);
	// next code is necessary to get the Formant at postion index selected and to get its name
	long iselected = 0;
	Formant him = NULL;
	LOOP {
		iselected ++;
		if (iselected != index) {
			praat_deselect (IOBJECT);
		} else {
			him = static_cast<Formant> (OBJECT);
		}
	}
	Melder_assert (him != NULL);
	autoFormant thee = Formant_extractPart (him, tmin, tmax);
	praat_new (thee.transfer(), his name, L"_part");
END


FORM (Formants_extractSmoothestPart_constrained, L"Formants: Extract smoothest part (constrained)", L"Formants: Extract smoothest part (constrained)...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	NATURAL (L"Number of formant tracks", L"4")
	INTEGER (L"Order of polynomials", L"3")
	LABEL (L"", L"Use bandwidths to model the formant tracks")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	LABEL (L"", L"Zero parameter values whose range include zero.")
	REAL (L"Number of sigmas", L"1.0")
	POSITIVE (L"Parameter variance power", L"1.5")
	LABEL (L"", L"The constraints on the formants")
	REAL (L"Minimum F1 (Hz)", L"100.0")
	REAL (L"Maximum F1 (Hz)", L"1200.0")
	REAL (L"Minimum F2 (Hz)", L"0.0")
	POSITIVE (L"Maximum F2 (Hz)", L"5000.0")
	POSITIVE (L"Minimum F3 (Hz)", L"1500.0")
	OK
DO
	autoCollection set = praat_getSelectedObjects ();
	double tmin = GET_REAL (L"left Time range"), tmax = GET_REAL (L"right Time range");
	long index = Formants_getSmoothestInInterval (set.peek(), tmin, tmax, 
		GET_INTEGER (L"Number of formant tracks"), 
	 	GET_INTEGER (L"Order of polynomials") + 1, GET_INTEGER (L"Weigh data") - 1, 
		GET_REAL (L"Number of sigmas"), GET_REAL (L"Parameter variance power"), 1, GET_REAL (L"Minimum F1"), GET_REAL (L"Maximum F1"),
		GET_REAL (L"Minimum F2"), GET_REAL (L"Maximum F2"), GET_REAL (L"Minimum F3"));
	// next code is necessary to get the Formant at postion index selected and to get its name
	long iselected = 0;
	Formant him = NULL;
	LOOP {
		iselected ++;
		if (iselected != index) {
			praat_deselect (IOBJECT);
		} else {
			him = static_cast<Formant> (OBJECT);
		}
	}
	Melder_assert (him != NULL);
	autoFormant thee = Formant_extractPart (him, tmin, tmax);
	praat_new (thee.transfer(), his name, L"_part");
END

/********************** FormantModeler ******************************/

FORM (FormantModeler_drawEstimatedTracks, L"FormantModeler: Draw estimated tracks", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"Maximum frequency (Hz)", L"5500.0")
	NATURAL (L"left Formant range", L"1")
	NATURAL (L"right Formant range", L"3")
	INTEGER (L"Order of polynomials for estimation", L"3")
	REAL (L"Horizontal offset (mm)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	long order = GET_INTEGER (L"Order of polynomials for estimation");
	REQUIRE (order >= 0, L"The order must be greater than or equal to zero.")
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawTracks (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 
			GET_REAL (L"Maximum frequency"), GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"), 1, order + 1, 
			GET_REAL (L"Horizontal offset"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FormantModeler_drawTracks, L"FormantModeler: Draw tracks", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"Maximum frequency (Hz)", L"5500.0")
	NATURAL (L"left Formant range", L"1")
	NATURAL (L"right Formant range", L"3")
	REAL (L"Horizontal offset (mm)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	long order = 6;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawTracks (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 
			GET_REAL (L"Maximum frequency"), GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"),
			0, order + 1, GET_REAL (L"Horizontal offset"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FormantModeler_speckle, L"FormantModeler: Speckle", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"Maximum frequency (Hz)", L"5500.0")
	NATURAL (L"left Formant range", L"1")
	NATURAL (L"right Formant range", L"3")
	BOOLEAN (L"Draw error bars", 1)
	REAL (L"Bar width (mm)", L"1.0")
	REAL (L"Horizontal offset (mm)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	long order = 6;
	LOOP {
		iam (FormantModeler);
		FormantModeler_speckle (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 
			GET_REAL (L"Maximum frequency"), GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"),
			0, order + 1, GET_INTEGER (L"Draw error bars"), GET_REAL (L"Bar width"), GET_REAL (L"Horizontal offset"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FormantModeler_drawOutliersMarked, L"FormantModeler: Draw outliers marked", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"Maximum frequency (Hz)", L"5500.0")
	NATURAL (L"left Formant range", L"1")
	NATURAL (L"right Formant range", L"3")
	POSITIVE (L"Number of sigmas", L"3.0")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	WORD (L"Mark", L"o")
	NATURAL (L"Mark font size", L"12")
	REAL (L"Horizontal offset (mm)", L"0.0")
	BOOLEAN (L"Garnish", 0)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawOutliersMarked (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Maximum frequency"), GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"),
			GET_REAL (L"Number of sigmas"), GET_INTEGER (L"Weigh data") - 1, GET_STRING (L"Mark"),
			GET_INTEGER (L"Mark font size"), GET_REAL (L"Horizontal offset"), GET_INTEGER (L"Garnish"));
	}
END

//(FormantModeler me, Graphics g, long iformant, long numberOfQuantiles, double numberOfSigmas, int labelSize, const wchar_t *label, int garnish)
FORM (FormantModeler_normalProbabilityPlot, L"FormantModeler: Normal probability plot", 0)
	NATURAL (L"Formant number", L"1")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	NATURAL (L"Number of quantiles", L"100")
	REAL (L"Number of sigmas", L"0.0")
	NATURAL (L"Label size", L"12")
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_normalProbabilityPlot (me, GRAPHICS, GET_INTEGER (L"Formant number"), GET_INTEGER (L"Weigh data") - 1,
			GET_INTEGER (L"Number of quantiles"),
			GET_REAL (L"Number of sigmas"), GET_INTEGER (L"Label size"), GET_STRING (L"Label"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FormantModeler_drawBasisFunction, L"FormantModeler: Draw basis function", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Amplitude range (Hz)", L"0.0")
	REAL (L"right Amplitude range (Hz)", L"5500.0")
	//long iterm, bool scaled, long numberOfPoints, int garnish
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Basis function", L"2")
	BOOLEAN (L"Scale function with parameter value", 0)
	NATURAL (L"Number of points", L"200")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantModeler);
		FormantModeler_drawBasisFunction (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),GET_INTEGER (L"Formant number"),
 			GET_INTEGER (L"Basis function"), GET_INTEGER (L"Scale function with parameter value"),
			GET_INTEGER (L"Number of points"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FormantModeler_getModelValueAtTime, L"", 0)
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.1")
	OK
DO
	LOOP {
		iam (FormantModeler);
		double y = FormantModeler_getModelValueAtTime (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"));
		Melder_informationReal (y, L"Hertz");
	}
END

FORM (FormantModeler_getDataPointValue, L"FormantModeler: Get data point value", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Index", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (FormantModeler);
		double value = FormantModeler_getDataPointValue (me, iformant, index);
		Melder_information (Melder_double (value), L" (= value of point ", Melder_integer (index), L" in track F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getDataPointSigma, L"FormantModeler: Get data point sigma", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Index", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (FormantModeler);
		double value = FormantModeler_getDataPointSigma (me, iformant, index);
		Melder_information (Melder_double (value), L" (= sigma of point ", Melder_integer (index), L" in track F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getDataPointStatus, L"FormantModeler: Get data point status", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Index", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (FormantModeler);
		int status = FormantModeler_getDataPointStatus (me, iformant, index);
		Melder_information (status == DataModeler_DATA_INVALID ? L"Invalid" : L"Valid");
	}
END

DIRECT (FormantModeler_getNumberOfTracks)
	LOOP {
		iam (FormantModeler);
		double nop = FormantModeler_getNumberOfTracks (me);
		Melder_information (Melder_integer (nop), L" (= number of formants)");
	}
END

FORM (FormantModeler_getNumberOfParameters, L"FormantModeler: Get number of parameters", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double nop = FormantModeler_getNumberOfParameters (me, iformant);
		Melder_information (Melder_integer (nop), L" (= number of parameters for F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getNumberOfFixedParameters, L"FormantModeler: Get number of fixed parameters", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double nop = FormantModeler_getNumberOfFixedParameters (me, iformant);
		Melder_information (Melder_integer (nop), L" (= number of fixed parameters for F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getNumberOfDataPoints, L"FormantModeler: Get number of data points", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me, iformant);
		Melder_information (Melder_integer (numberOfDataPoints), L" (= number of data points for F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getNumberOfInvalidDataPoints, L"FormantModeler: Get number of invalid data points", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double numberOfDataPoints = FormantModeler_getNumberOfInvalidDataPoints (me, iformant);
		Melder_information (Melder_integer (numberOfDataPoints), L" (= number of invalid data points for F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getParameterValue, L"FormantModeler: Get parameter value", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Parameter number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number"), iparameter = GET_INTEGER (L"Parameter number");
	LOOP {
		iam (FormantModeler);
		double parameter = FormantModeler_getParameterValue (me, iformant, iparameter);
		Melder_information (Melder_double (parameter), L" (= parameter[", Melder_integer (iparameter), L"] for F", 
			Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getParameterStatus, L"FormantModeler: Get parameter status", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Parameter number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number"), iparameter = GET_INTEGER (L"Parameter number");
	LOOP {
		iam (FormantModeler);
		int status = FormantModeler_getParameterStatus (me, iformant, iparameter);
		Melder_information (status == DataModeler_PARAMETER_FREE ? L"Free" : (status == DataModeler_PARAMETER_FIXED ? L"Fixed" :
			L"Undefined"), L" (= parameter[", Melder_integer (iparameter), L"] for F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getParameterStandardDeviation, L"FormantModeler: Get parameter standard deviatio", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Parameter number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number"), iparameter = GET_INTEGER (L"Parameter number");
	LOOP {
		iam (FormantModeler);
		double sigma = FormantModeler_getParameterStandardDeviation (me, iformant, iparameter);
		Melder_information (Melder_double (sigma), L" (= parameter[", Melder_integer (iparameter), L"] for F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getVarianceOfParameters, L"FormantModeler: Get variance of parameters", 0)
	INTEGER (L"left Formant range", L"0")
	INTEGER (L"right Formant range", L"0")
	INTEGER (L"left Parameter range", L"0")
	INTEGER (L"right Parameter range", L"0")
	OK
DO
	long nofp;
	LOOP {
		iam (FormantModeler);
		double var = FormantModeler_getVarianceOfParameters (me, GET_INTEGER (L"left Formant range"), 
			GET_INTEGER (L"right Formant range"), GET_INTEGER (L"left Parameter range"), GET_INTEGER (L"right Parameter range"), &nofp);
		Melder_information (Melder_double (var), L" (for ", Melder_integer(nofp), L" free parameters.)");
	}
END

FORM (FormantModeler_getCoefficientOfDetermination, L"FormantModeler: Get coefficient of determination", 0)
	INTEGER (L"left Formant range", L"0")
	INTEGER (L"right Formant range", L"0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		double rSquared = FormantModeler_getCoefficientOfDetermination (me, GET_INTEGER (L"left Formant range"), 
			GET_INTEGER (L"right Formant range"));
		Melder_informationReal (rSquared, L" (= R^2)");
	}
END

FORM (FormantModeler_getResidualSumOfSquares, L"FormantModeler: Get residual sum of squares", L"FormantModeler: Get residual sum of squares...")
	NATURAL (L"Formant number", L"1")
	OK
DO
	long n, iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double rss = FormantModeler_getResidualSumOfSquares (me, iformant, &n);
		Melder_information (Melder_double (rss), L" Hz^2,  (= RSS of F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getStandardDeviation, L"FormantModeler: Get formant standard deviation", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double sigma = FormantModeler_getStandardDeviation (me, iformant);
		Melder_information (Melder_double (sigma), L" Hz (= std. dev. of F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_reportChiSquared, L"FormantModeler: Report chi squared", 0)
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	OK
DO
	LOOP {
		iam (FormantModeler);
		long numberOfFormants = my datamodelers -> size;
		int useSigmaY = GET_INTEGER (L"Weigh data") - 1;
		double chisq = 0, ndf = 0, probability;
		MelderInfo_open ();
		MelderInfo_writeLine (L"Chi squared tests for individual models of each ", Melder_integer (numberOfFormants), L" formant track:");
		MelderInfo_writeLine (useSigmaY == DataModeler_DATA_WEIGH_EQUAL ? L"Standard deviation is estimated from the data." :
			useSigmaY == DataModeler_DATA_WEIGH_SIGMA ? L"\tBandwidths are used as estimate for local standard deviations." : 
			useSigmaY == DataModeler_DATA_WEIGH_RELATIVE ? L"\t1/Q's are used as estimate for local standard deviations." :
			L"\tSqrt bandwidths are used as estimate for local standard deviations.");
		for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
			chisq = FormantModeler_getChiSquaredQ (me, iformant, iformant, useSigmaY, &probability, &ndf);
			MelderInfo_writeLine (L"Formant track ", Melder_integer (iformant), L":");
			MelderInfo_writeLine (L"\tChi squared (F", Melder_integer (iformant), L") = ", Melder_double (chisq));
			MelderInfo_writeLine (L"\tProbability (F", Melder_integer (iformant), L") = ", Melder_double (probability));
			MelderInfo_writeLine (L"\tNumber of degrees of freedom (F", Melder_integer (iformant), L") = ", Melder_double (ndf));
		}
		chisq = FormantModeler_getChiSquaredQ (me, 1, numberOfFormants, useSigmaY, &probability, &ndf);
		MelderInfo_writeLine (L"Chi squared test for the complete model with ", Melder_integer (numberOfFormants), L" formants:");
		MelderInfo_writeLine (L"\tChi squared = ", Melder_double (chisq));
		MelderInfo_writeLine (L"\tProbability = ", Melder_double (probability));
		MelderInfo_writeLine (L"\tNumber of degrees of freedom = ", Melder_double (ndf));
		MelderInfo_close ();
	}
END

FORM (FormantModeler_getDegreesOfFreedom, L"FormantModeler: Get degrees of freedom", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		double sigma = FormantModeler_getDegreesOfFreedom (me, iformant);
		Melder_information (Melder_double (sigma), L" (= degrees of freedom of F", Melder_integer (iformant), L")");
	}
END

FORM (FormantModeler_getSmoothnessValue, L"FormantModeler: Get smoothness value", 0)
	INTEGER (L"left Formant range", L"0")
	INTEGER (L"right Formant range", L"0")
	INTEGER (L"Order of polynomials", L"3")
	POSITIVE (L"Parameter variance power", L"1.5")
	OK
DO
	LOOP {
		iam (FormantModeler);
		double smoothness = FormantModeler_getSmoothnessValue (me, GET_INTEGER (L"left Formant range"), 
			GET_INTEGER (L"right Formant range"), GET_INTEGER (L"Order of polynomials"), GET_REAL (L"Parameter variance power"));
		Melder_information (Melder_double (smoothness), L" (= smoothness)");
	}
END

FORM (FormantModeler_getAverageDistanceBetweenTracks, L"FormantModeler: Get average distance between tracks", 0)
	NATURAL (L"Track 1", L"2")
	NATURAL (L"Track 2", L"3")
	OPTIONMENU (L"Type of data", 1)
		OPTION (L"Data points")
		OPTION (L"Modeled")
	OK
DO
	long track1 = GET_INTEGER (L"Track 1"), track2 = GET_INTEGER (L"Track 2");
	LOOP {
		iam (FormantModeler);
		double distance = FormantModeler_getAverageDistanceBetweenTracks (me, track1, track2, GET_INTEGER (L"Type of data") - 1);
		Melder_information (Melder_double (distance), L" (= average |F", Melder_integer(track1), L" - F", Melder_integer (track2), L"|)");
	}
END

FORM (FormantModeler_getFormantsConstraintsFactor, L"FormantModeler: Get formants constraints factor", 0)
	REAL (L"Minimum F1 (Hz)", L"100.0")
	REAL (L"Maximum F1 (Hz)", L"1200.0")
	REAL (L"Minimum F2 (Hz)", L"0.0")
	POSITIVE (L"Maximum F2 (Hz)", L"5000.0")
	POSITIVE (L"Minimum F3 (Hz)", L"1500.0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		double fc = FormantModeler_getFormantsConstraintsFactor (me, GET_REAL (L"Minimum F1"), GET_REAL (L"Maximum F1"),
		GET_REAL (L"Minimum F2"), GET_REAL (L"Maximum F2"), GET_REAL (L"Minimum F3"));
		Melder_information (Melder_double (fc), L" (= formants constraints factor)");
	}
END

FORM (FormantModeler_setDataWeighing, L"FormantModeler: Set data weighing", 0)
	INTEGER (L"left Formant range", L"0")
	INTEGER (L"right Formant range", L"0")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataWeighing (me, GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"), 
			GET_INTEGER (L"Weigh data") - 1);
	}
END

FORM (FormantModeler_setTolerance, L"FormantModeler: Set tolerance", 0)
	REAL (L"Tolerance", L"1e-5")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setTolerance (me, GET_REAL (L"Tolerance"));
	}
END

FORM (FormantModeler_setParameterValueFixed, L"FormantModeler: Set parameter value fixed", 0)
	NATURAL (L"Formant number", L"1")
	NATURAL (L"Parameter number", L"1")
	REAL (L"Value", L"0.0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setParameterValueFixed (me, GET_INTEGER (L"Formant number"), GET_INTEGER (L"Parameter number"), GET_REAL (L"Value"));
	}
END


FORM (FormantModeler_setParameterFree, L"FormantModeler: Set parameter free", 0)
	INTEGER (L"left Formant range", L"0")
	INTEGER (L"right Formant range", L"0")
	INTEGER (L"left Parameter range", L"0")
	INTEGER (L"right Parameter range", L"0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setParametersFree (me, GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"),
			GET_INTEGER (L"left Parameter range"), GET_INTEGER (L"right Parameter range"));
	}
END

FORM (FormantModeler_setParameterValuesToZero, L"FormantModeler: Set parameter values to zero", 0)
	INTEGER (L"left Formant range", L"0")
	INTEGER (L"right Formant range", L"0")
	REAL (L"Number of sigmas", L"1.0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setParameterValuesToZero (me, GET_INTEGER (L"left Formant range"),  GET_INTEGER (L"right Formant range"),
			 GET_REAL (L"Number of sigmas"));
	}
END

FORM (FormantModeler_setDataPointValue, L"FormantModeler: Set data point value", 0)
	NATURAL (L"Formant index", L"1")
	NATURAL (L"Data index", L"1")
	REAL (L"Value", L"1.0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataPointValue (me, GET_INTEGER (L"Formant index"),  GET_INTEGER (L"Data index"),
			 GET_REAL (L"Value"));
	}
END

FORM (FormantModeler_setDataPointSigma, L"FormantModeler: Set data point sigma", 0)
	NATURAL (L"Formant index", L"1")
	NATURAL (L"Data index", L"1")
	REAL (L"Sigma", L"10.0")
	OK
DO
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataPointSigma (me, GET_INTEGER (L"Formant index"),  GET_INTEGER (L"Data index"),
			 GET_REAL (L"Sigma"));
	}
END

FORM (FormantModeler_setDataPointStatus, L"FormantModeler: Set data point status", 0)
	NATURAL (L"Formant index", L"1")
	NATURAL (L"Data index", L"1")
	OPTIONMENU (L"Status", 1)
		OPTION (L"Valid")
		OPTION (L"Invalid")
	OK
DO
	int menustatus = GET_INTEGER (L"Status");
	int status = menustatus == 2 ? DataModeler_DATA_INVALID : DataModeler_DATA_VALID;
	LOOP {
		iam (FormantModeler);
		FormantModeler_setDataPointStatus (me, GET_INTEGER (L"Formant index"),  GET_INTEGER (L"Data index"), status);
	}
END

DIRECT (FormantModeler_fitModel)
	LOOP {
		iam (FormantModeler);
		FormantModeler_fit (me);
	}
END

FORM (FormantModeler_to_Covariance_parameters, L"", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		autoCovariance thee = FormantModeler_to_Covariance_parameters (me, iformant);
		praat_new (thee.transfer(), my name, L"_", Melder_integer (iformant));
	}
END

FORM (FormantModeler_extractDataModeler, L"FormantModeler: Extract DataModeler", 0)
	NATURAL (L"Formant number", L"1")
	OK
DO
	long iformant = GET_INTEGER (L"Formant number");
	LOOP {
		iam (FormantModeler);
		autoDataModeler thee = FormantModeler_extractDataModeler (me, iformant);
		praat_new (thee.transfer(), my name, L"_", Melder_integer (iformant));
	}
END

FORM (FormantModeler_to_Table_zscores, L"", 0)
	BOOLEAN (L"Bandwidths as standard deviation", 1)
	OK
DO
	LOOP {
		iam (FormantModeler);
		autoTable thee = FormantModeler_to_Table_zscores (me, GET_INTEGER (L"Bandwidths as standard deviation"));
		praat_new (thee.transfer(), my name, L"_z");
	}
END

FORM (FormantModeler_processOutliers, L"", 0)
	POSITIVE (L"Number of sigmas", L"3.0")
	BOOLEAN (L"Bandwidths as standard deviation", 1)
	OK
DO
	LOOP {
		iam (FormantModeler);
		autoFormantModeler thee = FormantModeler_processOutliers (me, GET_REAL (L"Number of sigmas"), GET_INTEGER (L"Bandwidths as standard deviation"));
		praat_new (thee.transfer(), my name, L"_outliers");
	}
END

/*************************** PitchModeler *************************************/

FORM (Pitch_to_PitchModeler, L"Pitch: To PitchModeler", 0)
	REAL (L"left Start time (s)", L"0.0")
	REAL (L"right End time (s)", L"0.1")
	INTEGER (L"Order of polynomials", L"2")
	OK
DO
	LOOP {
		iam (Pitch);
		autoPitchModeler thee = Pitch_to_PitchModeler (me, GET_REAL (L"left Start time"), GET_REAL (L"right End time"), 
			GET_INTEGER (L"Order of polynomials") + 1);
		praat_new (thee.transfer(), my name);
	}
END

FORM (PitchModeler_draw, L"PitchModeler: Draw", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"500.0")
	INTEGER (L"Order of polynomial for estimation", L"2")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PitchModeler);
		PitchModeler_draw (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"), GET_INTEGER (L"Order of polynomial for estimation") + 1,  GET_INTEGER (L"Garnish"));
	}
END

FORM (Sound_to_Formant_interval, L"Sound: To Formant (interval)", 0)
	REAL (L"left Time range (s)", L"0.1")
	REAL (L"right Time range (s)", L"0.15")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.0025")
	POSITIVE (L"left Maximum frequency range (Hz)", L"4500.0")
	POSITIVE (L"right Maximum frequency range (Hz)", L"6500.0")
	NATURAL (L"Number of frequency steps", L"11")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50.0")
	NATURAL (L"Number of formant tracks in model", L"4")
	INTEGER (L"Order of polynomials", L"3")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	LABEL (L"", L"Make parameters that include zero in their confidence region zero")
	REAL (L"Number of sigmas", L"1.0")
	POSITIVE (L"Parameter variance power", L"1.5")
	OK
DO
	LOOP {
		iam (Sound);
		double ceiling;
		autoFormant formant = Sound_to_Formant_interval (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Window length"), GET_REAL (L"Time step"), GET_REAL (L"left Maximum frequency range"), 
			GET_REAL (L"right Maximum frequency range"), GET_INTEGER (L"Number of frequency steps"), 
			GET_REAL (L"Pre-emphasis from"), 
			GET_INTEGER (L"Number of formant tracks in model"), GET_INTEGER (L"Order of polynomials") + 1,
			GET_INTEGER (L"Weigh data") - 1, GET_REAL (L"Number of sigmas"), GET_REAL (L"Parameter variance power"),
			0, 1, 1, 1, 1, 1, &ceiling);
		praat_new (formant.transfer(), my name, L"_", Melder_fixed (ceiling, 0));
	}
END

FORM (Sound_to_Formant_interval_constrained, L"Sound: To Formant (interval, constrained)", 0)
	REAL (L"left Time range (s)", L"0.1")
	REAL (L"right Time range (s)", L"0.15")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.0025")
	POSITIVE (L"left Maximum frequency range (Hz)", L"4500.0")
	POSITIVE (L"right Maximum frequency range (Hz)", L"6500.0")
	NATURAL (L"Number of frequency steps", L"11")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50.0")
	NATURAL (L"Number of formant tracks in model", L"4")
	INTEGER (L"Order of polynomials", L"3")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	LABEL (L"", L"Make parameters that include zero in their confidence region zero")
	REAL (L"Number of sigmas", L"1.0")
	POSITIVE (L"Parameter variance power", L"1.5")
	LABEL (L"", L"Formant frequency constraints")
	REAL (L"Minimum F1 (Hz)", L"100.0")
	REAL (L"Maximum F1 (Hz)", L"1200.0")
	REAL (L"Minimum F2 (Hz)", L"0.0")
	POSITIVE (L"Maximum F2 (Hz)", L"5000.0")
	POSITIVE (L"Minimum F3 (Hz)", L"1000.0")
	OK
DO
	LOOP {
		iam (Sound);
		double ceiling;
		autoFormant formant = Sound_to_Formant_interval (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Window length"), GET_REAL (L"Time step"), GET_REAL (L"left Maximum frequency range"), 
			GET_REAL (L"right Maximum frequency range"), GET_INTEGER (L"Number of frequency steps"),
			GET_REAL (L"Pre-emphasis from"), GET_INTEGER (L"Number of formant tracks in model"), 
			GET_INTEGER (L"Order of polynomials") + 1, GET_INTEGER (L"Weigh data") - 1, GET_REAL (L"Number of sigmas"),
			GET_REAL (L"Parameter variance power"), 1,
			GET_REAL (L"Minimum F1"), GET_REAL (L"Maximum F1"), GET_REAL (L"Minimum F2"), 
			GET_REAL (L"Maximum F2"), GET_REAL (L"Minimum F3"), &ceiling);
		praat_new (formant.transfer(), my name, L"_", Melder_fixed (ceiling, 0));
	}
END

FORM (Sound_to_Formant_interval_constrained_robust, L"Sound: To Formant (interval, constrained, robust)", 0)
	REAL (L"left Time range (s)", L"0.1")
	REAL (L"right Time range (s)", L"0.15")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.0025")
	POSITIVE (L"left Maximum frequency range (Hz)", L"4500.0")
	POSITIVE (L"right Maximum frequency range (Hz)", L"6500.0")
	NATURAL (L"Number of frequency steps", L"11")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50.0")
	NATURAL (L"Number of formant tracks in model", L"4")
	INTEGER (L"Order of polynomials", L"3")
	OPTIONMENU (L"Weigh data", 2)
		OPTION (L"Equally")
		OPTION (L"Bandwidth")
		OPTION (L"Bandwidth / frequency")
		OPTION (L"Sqrt bandwidth")
	LABEL (L"", L"Make parameters that include zero in their confidence region zero")
	REAL (L"Number of sigmas", L"1.0")
	POSITIVE (L"Parameter variance power", L"1.5")
	LABEL (L"", L"Formant frequency constraints")
	REAL (L"Minimum F1 (Hz)", L"100.0")
	REAL (L"Maximum F1 (Hz)", L"1200.0")
	REAL (L"Minimum F2 (Hz)", L"0.0")
	POSITIVE (L"Maximum F2 (Hz)", L"5000.0")
	POSITIVE (L"Minimum F3 (Hz)", L"1000.0")
	OK
DO
	LOOP {
		iam (Sound);
		double ceiling;
		autoFormant formant = Sound_to_Formant_interval_robust (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Window length"), GET_REAL (L"Time step"), GET_REAL (L"left Maximum frequency range"), 
			GET_REAL (L"right Maximum frequency range"), GET_INTEGER (L"Number of frequency steps"),
			GET_REAL (L"Pre-emphasis from"), GET_INTEGER (L"Number of formant tracks in model"), 
			GET_INTEGER (L"Order of polynomials") + 1, GET_INTEGER (L"Weigh data") - 1, GET_REAL (L"Number of sigmas"),
			GET_REAL (L"Parameter variance power"), 1,
			GET_REAL (L"Minimum F1"), GET_REAL (L"Maximum F1"), GET_REAL (L"Minimum F2"), 
			GET_REAL (L"Maximum F2"), GET_REAL (L"Minimum F3"), &ceiling);
		praat_new (formant.transfer(), my name, L"_", Melder_fixed (ceiling, 0));
	}
END

FORM (Table_to_DataModeler, L"", 0)
	REAL (L"left X range", L"0.0")
	REAL (L"right X range", L"0.0 (=auto)")
	WORD (L"Column with X data", L"")
	WORD (L"Column with Y data", L"")
	WORD (L"Column with sigmas", L"")
	OPTIONMENU (L"Model functions", 1)
	OPTION (L"Legendre polynomials")
	INTEGER (L"Maximum order", L"3")
	OK
DO
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with X data"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with Y data"));
		long scolumn = Table_findColumnIndexFromColumnLabel (me, GET_STRING (L"Column with sigmas"));
		autoDataModeler thee = Table_to_DataModeler (me, GET_REAL (L"left X range"), GET_REAL (L"right X range"),
			xcolumn, ycolumn, scolumn, GET_INTEGER (L"Maximum order") + 1, GET_INTEGER (L"Model functions"));
		praat_new (thee.transfer(), my name);
	}
END

void praat_DataModeler_init (void);
void praat_DataModeler_init (void) {
	Thing_recognizeClassesByName (classDataModeler, classFormantModeler, classPitchModeler, NULL);
	
	praat_addAction1 (classDataModeler, 0, L"Speckle...", 0, 0, DO_DataModeler_speckle);
	praat_addAction1 (classDataModeler, 0, L"Draw estimated track...", 0, 0, DO_DataModeler_drawEstimatedTrack);

	praat_addAction1 (classDataModeler, 1, L"Query -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, L"Get number of parameters", 0, 1, DO_DataModeler_getNumberOfParameters);
		praat_addAction1 (classDataModeler, 0, L"Get number of fixed parameters", 0, 1, DO_DataModeler_getNumberOfFixedParameters);
		praat_addAction1 (classDataModeler, 0, L"Get parameter value...", 0, 1, DO_DataModeler_getParameterValue);
		praat_addAction1 (classDataModeler, 0, L"Get parameter status...", 0, 1, DO_DataModeler_getParameterStatus);
		praat_addAction1 (classDataModeler, 0, L"Get parameter standard deviation...", 0, 1, DO_DataModeler_getParameterStandardDeviation);
		praat_addAction1 (classDataModeler, 0, L"Get variance of parameters...", 0, 1, DO_DataModeler_getVarianceOfParameters);
		praat_addAction1 (classDataModeler, 1, L"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, L"Get model value at x...", 0, 1, DO_DataModeler_getModelValueAtX);
		praat_addAction1 (classDataModeler, 0, L"Get number of data points...", 0, 1, DO_DataModeler_getNumberOfDataPoints);
		praat_addAction1 (classDataModeler, 0, L"Get number of invalid data points...", 0, 1, DO_DataModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classDataModeler, 0, L"Get data point value...", 0, 1, DO_DataModeler_getDataPointValue);
		praat_addAction1 (classDataModeler, 0, L"Get data point sigma...", 0, 1, DO_DataModeler_getDataPointSigma);
		praat_addAction1 (classDataModeler, 0, L"Get data point status...", 0, 1, DO_DataModeler_getDataPointStatus);
		praat_addAction1 (classDataModeler, 1, L"-- get statistics info --", 0, 1, 0);
		
		praat_addAction1 (classDataModeler, 0, L"Get residual sum of squares", 0, 1, DO_DataModeler_getResidualSumOfSquares);
		praat_addAction1 (classDataModeler, 0, L"Get data standard deviation...", 0, 1, DO_DataModeler_getStandardDeviation);
		praat_addAction1 (classDataModeler, 0, L"Get coefficient of determination", 0, 1, DO_DataModeler_getCoefficientOfDetermination);
		praat_addAction1 (classDataModeler, 0, L"Report chi squared...", 0, 1, DO_DataModeler_reportChiSquared);
		praat_addAction1 (classDataModeler, 0, L"Get degrees of freedom...", 0, 1, DO_DataModeler_getDegreesOfFreedom);

	praat_addAction1 (classDataModeler, 1, L"Modify -", 0, 0, 0);
		praat_addAction1 (classDataModeler, 0, L"Set data weighing...", 0, 1, DO_DataModeler_setDataWeighing);
		praat_addAction1 (classDataModeler, 0, L"Set tolerance...", 0, 1, DO_DataModeler_setTolerance);
		praat_addAction1 (classDataModeler, 1, L"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, L"Set parameter value fixed...", 0, 1, DO_DataModeler_setParameterValueFixed);
		praat_addAction1 (classDataModeler, 0, L"Set parameter free...", 0, 1, DO_DataModeler_setParameterFree);
		praat_addAction1 (classDataModeler, 0, L"Set parameter values to zero...", 0, 1, DO_DataModeler_setParameterValuesToZero);
		praat_addAction1 (classDataModeler, 1, L"-- set data values --", 0, 1, 0);
		praat_addAction1 (classDataModeler, 0, L"Set data point status...", 0, 1, DO_DataModeler_setDataPointStatus);
		praat_addAction1 (classDataModeler, 0, L"Set data point value...", 0, 1, DO_DataModeler_setDataPointValue);
		praat_addAction1 (classDataModeler, 0, L"Set data point sigma...", 0, 1, DO_DataModeler_setDataPointSigma);
		
	praat_addAction1 (classDataModeler, 0, L"Fit model", 0, 0, DO_DataModeler_fitModel);
	
	praat_addAction1 (classDataModeler, 0, L"To Covariance (parameters)...", 0, 0, DO_DataModeler_to_Covariance_parameters);
	praat_addAction1 (classDataModeler, 0, L"To Table (z-scores)...", 0, 0, DO_DataModeler_to_Table_zscores);

	praat_addAction1 (classFormant, 0, L"To FormantModeler...", L"To LPC...", 0, DO_Formant_to_FormantModeler);
	praat_addAction1 (classFormant, 0, L"Extract smoothest part...", 0, 0, DO_Formants_extractSmoothestPart);
	praat_addAction1 (classFormant, 0, L"Extract smoothest part (constrained)...", 0, 0, DO_Formants_extractSmoothestPart_constrained);

	praat_addAction1 (classFormantModeler, 0, L"Speckle...", 0, 0, DO_FormantModeler_speckle);
	praat_addAction1 (classFormantModeler, 0, L"Draw tracks...", 0, 0, DO_FormantModeler_drawTracks);
	praat_addAction1 (classFormantModeler, 0, L"Draw estimated tracks...", 0, 0, DO_FormantModeler_drawEstimatedTracks);
	praat_addAction1 (classFormantModeler, 0, L"Draw outliers marked...", 0, 0, DO_FormantModeler_drawOutliersMarked);
	praat_addAction1 (classFormantModeler, 0, L"Normal probability plot...", 0, 0, DO_FormantModeler_normalProbabilityPlot);
	praat_addAction1 (classFormantModeler, 0, L"Draw basis function...", 0, 0, DO_FormantModeler_drawBasisFunction);
	
	praat_addAction1 (classFormantModeler, 1, L"Query -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, L"Get number of tracks...", 0, 1, DO_FormantModeler_getNumberOfTracks);
		praat_addAction1 (classFormantModeler, 1, L"-- get parameter info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, L"Get number of parameters...", 0, 1, DO_FormantModeler_getNumberOfParameters);
		praat_addAction1 (classFormantModeler, 0, L"Get number of fixed parameters...", 0, 1, DO_FormantModeler_getNumberOfFixedParameters);
		praat_addAction1 (classFormantModeler, 0, L"Get parameter value...", 0, 1, DO_FormantModeler_getParameterValue);
		praat_addAction1 (classFormantModeler, 0, L"Get parameter status...", 0, 1, DO_FormantModeler_getParameterStatus);
		praat_addAction1 (classFormantModeler, 0, L"Get parameter standard deviation...", 0, 1, DO_FormantModeler_getParameterStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, L"Get variance of parameters...", 0, 1, DO_FormantModeler_getVarianceOfParameters);
		praat_addAction1 (classFormantModeler, 1, L"-- get data points info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, L"Get number of data points...", 0, 1, DO_FormantModeler_getNumberOfDataPoints);
		praat_addAction1 (classFormantModeler, 0, L"Get number of invalid data points...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_FormantModeler_getNumberOfInvalidDataPoints);
		praat_addAction1 (classFormantModeler, 0, L"Get model value at time...", 0, 1, DO_FormantModeler_getModelValueAtTime);
		praat_addAction1 (classFormantModeler, 0, L"Get data point value...", 0, 1, DO_FormantModeler_getDataPointValue);
		praat_addAction1 (classFormantModeler, 0, L"Get data point sigma...", 0, 1, DO_FormantModeler_getDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, L"Get data point status...", 0, 1, DO_FormantModeler_getDataPointStatus);

		praat_addAction1 (classFormantModeler, 1, L"-- get statistics info --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, L"Get residual sum of squares...", 0, 1, DO_FormantModeler_getResidualSumOfSquares);
		praat_addAction1 (classFormantModeler, 0, L"Get formant standard deviation...", 0, 1, DO_FormantModeler_getStandardDeviation);
		praat_addAction1 (classFormantModeler, 0, L"Get coefficient of determination...", 0, 1, DO_FormantModeler_getCoefficientOfDetermination);
		praat_addAction1 (classFormantModeler, 0, L"Report chi squared...", 0, 1, DO_FormantModeler_reportChiSquared);
		praat_addAction1 (classFormantModeler, 0, L"Get degrees of freedom...", 0, 1, DO_FormantModeler_getDegreesOfFreedom);
		praat_addAction1 (classFormantModeler, 0, L"Get smoothness value...", 0, 1, DO_FormantModeler_getSmoothnessValue);
		praat_addAction1 (classFormantModeler, 0, L"Get average distance between tracks...", 0, 1, DO_FormantModeler_getAverageDistanceBetweenTracks);
		praat_addAction1 (classFormantModeler, 0, L"Get formants constraints factor...", 0, 1, DO_FormantModeler_getFormantsConstraintsFactor);

	praat_addAction1 (classFormantModeler, 1, L"Modify -", 0, 0, 0);
		praat_addAction1 (classFormantModeler, 0, L"Set data weighing...", 0, 1, DO_FormantModeler_setDataWeighing);
		praat_addAction1 (classFormantModeler, 0, L"Set tolerance...", 0, 1, DO_FormantModeler_setTolerance);
		praat_addAction1 (classFormantModeler, 1, L"-- set parameter values --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, L"Set parameter value fixed...", 0, 1, DO_FormantModeler_setParameterValueFixed);
		praat_addAction1 (classFormantModeler, 0, L"Set parameter free...", 0, 1, DO_FormantModeler_setParameterFree);
		praat_addAction1 (classFormantModeler, 0, L"Set parameter values to zero...", 0, 1, DO_FormantModeler_setParameterValuesToZero);
		praat_addAction1 (classFormantModeler, 1, L"-- set data points --", 0, 1, 0);
		praat_addAction1 (classFormantModeler, 0, L"Set data point value...", 0, 1, DO_FormantModeler_setDataPointValue);
		praat_addAction1 (classFormantModeler, 0, L"Set data point sigma...", 0, 1, DO_FormantModeler_setDataPointSigma);
		praat_addAction1 (classFormantModeler, 0, L"Set data point status...", 0, 1, DO_FormantModeler_setDataPointStatus);
			
	praat_addAction1 (classFormantModeler, 0, L"Fit model", 0, 0, DO_FormantModeler_fitModel);
	
	praat_addAction1 (classFormantModeler, 0, L"To Covariance (parameters)...", 0, 0, DO_FormantModeler_to_Covariance_parameters);
	praat_addAction1 (classFormantModeler, 0, L"To Table (z-scores)...", 0, 0, DO_FormantModeler_to_Table_zscores);
	praat_addAction1 (classFormantModeler, 0, L"To FormantModeler (process outliers)...", 0, 0, DO_FormantModeler_processOutliers);
	praat_addAction1 (classFormantModeler, 0, L"Extract DataModeler...", 0, 0, DO_FormantModeler_extractDataModeler);
	
	praat_addAction1 (classPitch, 0, L"To PitchModeler...", L"To PointProcess", praat_HIDDEN, DO_Pitch_to_PitchModeler);

	praat_addAction1 (classPitchModeler, 0, L"Draw...", 0, 0, DO_PitchModeler_draw);

	praat_addAction1 (classSound, 0, L"To Formant (interval)...", L"To Formant (robust)...", praat_DEPTH_2 | praat_HIDDEN, DO_Sound_to_Formant_interval);
	praat_addAction1 (classSound, 0, L"To Formant (interval, constrained)...", L"To Formant (interval)...",
		praat_DEPTH_2 | praat_HIDDEN, DO_Sound_to_Formant_interval_constrained);
	praat_addAction1 (classSound, 0, L"To Formant (interval, constrained, robust)...", L"To Formant (interval, constrained)...", 
		praat_DEPTH_2 | praat_HIDDEN, DO_Sound_to_Formant_interval_constrained_robust);
	praat_addAction1 (classTable, 0, L"To DataModeler...", L"To logistic regression...", praat_DEPTH_1 + praat_HIDDEN, DO_Table_to_DataModeler);
}

/* End of file praat_DataModeler_init.c */
