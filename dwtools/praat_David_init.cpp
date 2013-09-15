/* praat_David_init.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
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
 djmw 20030701 Added Strings_setString.
 djmw 20031020 Changed Matrix_solveEquation.
 djmw 20031023 Added Spectra_multiply, Spectrum_conjugate and modified interface for CCA_and_TableOfReal_scores.
 djmw 20031030 Added TableOfReal_appendColumns.
 djmw 20031107 Added TablesOfReal_to_GSVD.
 djmw 20040303 Latest modification
 djmw 20040305 Added hints for PCA.
 djmw 20040323 Added hint for Discriminant.
 djmw 20040324 Added PCA_and_TableOfReal_getFractionVariance.
 djmw 20040331 Modified Eigen_drawEigenvalues interface.
 djmw 20040406 Extensive checks for creation of Sounds.
 djmw 20040414 Forms texts.
 djmw 20040523 Discriminant_and_TableOfReal_to_ClassificationTable: give new object a name.
 djmw 20040623 Added ClassificationTable_to_Strings_maximumProbability.
 djmw 20040704 BarkFilter... in Thing_recognizeClassesByName.
 djmw 20041020 MelderFile -> structMelderFile.
 djmw 20041105 TableOfReal_createFromVanNieropData_25females.
 djmw 20041108 FormantFilter_drawSpectrum bug correted (wrong field name).
 djmw 20050308 Find path (slopes), Find path (band)... and others.
 djmw 20050404 TableOfReal_appendColumns -> TableOfReal_appendColumnsMany
 djmw 20050406 Procrustus -> Prorustes
 djmw 20050407 MelFilter_drawFilterFunctions error in field names crashed praat
 djmw 20050706 Eigen_getSumOfEigenvalues
 djmw 20051012 Robust LPC analysis test
 djmw 20051116 TableOfReal_drawScatterPlot horizontal and vertical axes indices must be positive numbers
 djmw SVD extract lef/right singular vectors
 djmw 20060111 TextGrid: Extend time moved from depth 1 to depth 2.
 djmw 20060308 Thing_recognizeClassesByName: StringsIndex, CCA
 djmw 20070206 Sound_changeGender: pitch range factor must be >= 0
 djmw 20070304 Latest modification.
 djmw 20070903 Melder_new<1...>
 djmw 20071011 REQUIRE requires L"".
 djmw 20071202 Melder_warning<n>
 djmw 20080521 Confusion_drawAsnumbers
 djmw 20090109 KlattGrid formulas for formant
 djmw 20090708 KlattTable <-> Table
 djmw 20090822 Thing_recognizeClassesByName: added classCepstrum, classIndex, classKlattTable
 djmw 20090914 Excitation to Excitations crashed because of NULL reference
 djmw 20090927 TableOfReal_drawRow(s)asHistogram
 djmw 20091023 Sound_draw_selectedIntervals
 djmw 20091230 Covariance_and_TableOfReal_mahalanobis
 djmw 20100212 Standardize on Window length
 djmw 20100511 Categories_getNumberOfCategories
 djmw 20120813 Latest modification.
*/

#include "praat.h"
#include "NUM2.h"
#include "NUMlapack.h"
#include "NUMmachar.h"

#include "Activation.h"
#include "Categories.h"
#include "CategoriesEditor.h"
#include "ClassificationTable.h"
#include "Collection_extensions.h"
#include "Confusion.h"
#include "Discriminant.h"
#include "EditDistanceTable.h"
#include "Editor.h"
#include "EditDistanceTable.h"
#include "Eigen_and_Matrix.h"
#include "Eigen_and_Procrustes.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "Excitations.h"
#include "espeakdata_FileInMemory.h"
#include "FileInMemory.h"
#include "Formula.h"
#include "FormantGridEditor.h"
#include "FormantGrid_extensions.h"
#include "Intensity_extensions.h"
#include "IntensityTierEditor.h"
#include "Matrix_Categories.h"
#include "Matrix_extensions.h"
#include "LongSound_extensions.h"
#include "KlattGridEditors.h"
#include "KlattTable.h"
#include "Ltas_extensions.h"
#include "Minimizers.h"
#include "Pattern.h"
#include "PCA.h"
#include "PitchTierEditor.h"
#include "Polygon_extensions.h"
#include "Polynomial.h"
#include "Sound_extensions.h"
#include "Sounds_to_DTW.h"
#include "Spectrum_extensions.h"
#include "Spectrogram.h"
#include "SpeechSynthesizer.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "SSCP.h"
#include "Strings_extensions.h"
#include "SVD.h"
#include "Table_extensions.h"
#include "TableOfReal_and_Permutation.h"
#include "TextGrid_extensions.h"

#include "Categories_and_Strings.h"
#include "CCA_and_Correlation.h"
#include "Cepstrum_and_Spectrum.h"
#include "CCs_to_DTW.h"
#include "Discriminant_Pattern_Categories.h"
#include "DTW_and_TextGrid.h"
#include "MelFilter_and_MFCC.h"
#include "Permutation_and_Index.h"
#include "Pitch_extensions.h"
#include "Sound_and_FilterBank.h"
#include "Sound_to_Pitch2.h"
#include "Sound_to_SPINET.h"
#include "TableOfReal_and_SVD.h"
#include "VowelEditor.h"

#undef iam
#define iam iam_LOOP

static const wchar_t *QUERY_BUTTON   = L"Query -";
static const wchar_t *DRAW_BUTTON    = L"Draw -";
static const wchar_t *MODIFY_BUTTON  = L"Modify -";
static const wchar_t *EXTRACT_BUTTON = L"Extract -";

void praat_TimeFunction_query_init (ClassInfo klas);
void praat_TimeFrameSampled_query_init (ClassInfo klas);
void praat_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init2 (ClassInfo klas);
void praat_SSCP_as_TableOfReal_init (ClassInfo klas);

void praat_CC_init (ClassInfo klas);
void DTW_constraints_addCommonFields (void *dia);
void DTW_constraints_getCommonFields (void *dia, int *begin, int *end, int *slope);
void praat_Matrixft_query_init (ClassInfo klas);
int praat_Fon_formula (UiForm dia, Interpreter interpreter);
void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas);

#undef INCLUDE_DTW_SLOPES

/********************** Activation *******************************************/

FORM (Activation_formula, L"Activation: Formula", 0)
	LABEL (L"label", L"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }}")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	praat_Fon_formula (dia, interpreter);
END

DIRECT (Activation_to_Matrix)
	LOOP {
		iam (Activation);
		praat_new (Activation_to_Matrix (me), my name);
	}
END

/********************** BarkFilter *******************************************/

DIRECT (BarkFilter_help)
	Melder_help (L"BarkFilter");
END

FORM (BarkFilter_drawSpectrum, L"BarkFilter: Draw spectrum (slice)", L"FilterBank: Draw spectrum (slice)...")
	POSITIVE (L"Time (s)", L"0.1")
	REAL (L"left Frequency range (Bark)", L"0.0")
	REAL (L"right Frequency range (Bark)", L"0.0")
	REAL (L"left Amplitude range (dB)", L"0.0")
	REAL (L"right Amplitude range (dB)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BarkFilter);
		FilterBank_drawTimeSlice (me, GRAPHICS, GET_REAL (L"Time"), GET_REAL (L"left Frequency range"),
			GET_REAL (L"right Frequency range"), GET_REAL (L"left Amplitude range"),
			GET_REAL (L"right Amplitude range"), L"Barks", GET_INTEGER (L"Garnish"));
	}
END

FORM (BarkFilter_drawSekeyHansonFilterFunctions, L"BarkFilter: Draw filter functions", L"FilterBank: Draw filter functions...")
	INTEGER (L"left Filter range", L"0")
	INTEGER (L"right Filter range", L"0")
	RADIO (L"Frequency scale", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	BOOLEAN (L"Amplitude scale in dB", 1)
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BarkFilter);
		BarkFilter_drawSekeyHansonFilterFunctions (me, GRAPHICS, GET_INTEGER (L"Frequency scale"),
			GET_INTEGER (L"left Filter range"), GET_INTEGER (L"right Filter range"),
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_INTEGER (L"Amplitude scale in dB"), GET_REAL (L"left Amplitude range"),
			GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Garnish"));
	}
END
/********************** Categories  ****************************************/

FORM (Categories_append, L"Categories: Append 1 category", L"Categories: Append 1 category...")
	SENTENCE (L"Category", L"")
	OK
DO
	LOOP {
		iam (Categories);
		OrderedOfString_append (me, GET_STRING (L"Category"));
	}
END

DIRECT (Categories_edit)
	if (theCurrentPraatApplication -> batch) {
		Melder_throw ("Cannot edit a Categories from batch.");
	} else {
		LOOP {
			iam (Categories);
			praat_installEditor (CategoriesEditor_create (
				my name, me), IOBJECT);
		}
	}
END

DIRECT (Categories_getNumberOfCategories)
	LOOP {
		iam (Categories);
		Melder_information (Melder_integer (my size), L" categories");
	}
END

DIRECT (Categories_getNumberOfDifferences)
	Categories c1 = 0, c2 = 0;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);

	long NumberOfDifferences = OrderedOfString_getNumberOfDifferences (c1, c2);
	if (NumberOfDifferences < 0) {
		Melder_information (L"-1 (undefined: number of elements differ!)");
	} else {
		Melder_information (Melder_integer (NumberOfDifferences), L" differences");
	}
END

DIRECT (Categories_getFractionDifferent)
	Categories c1 = 0, c2 = 0;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	Melder_information (Melder_double (OrderedOfString_getFractionDifferent (c1, c2)));
END

DIRECT (Categories_difference)
	Categories c1 = 0, c2 = 0;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	double fraction; long n;
	OrderedOfString_difference (c1, c2, &n, &fraction);
	Melder_information (Melder_integer (n), L" differences");
END

DIRECT (Categories_selectUniqueItems)
	LOOP {
		iam (Categories);
		praat_new (Categories_selectUniqueItems (me, 1), my name, L"_uniq");
	}
END

DIRECT (Categories_to_Confusion)
	Categories c1 = 0, c2 = 0;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	praat_new (Categories_to_Confusion (c1, c2), Thing_getName (c1), L"_", Thing_getName (c2));
END

DIRECT (Categories_to_Strings)
	LOOP {
		iam (Categories);
		praat_new (Categories_to_Strings (me), my name);
	}
END

DIRECT (Categories_join)
	Categories c1 = 0, c2 = 0;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	praat_new (OrderedOfString_joinItems (c1, c2), 0);
END

DIRECT (Categories_permuteItems)
	LOOP {
		iam (Collection);
		praat_new (Collection_permuteItems (me), my name, L"_perm");
	}
END

/***************** CC ****************************************/

FORM (CC_getValue, L"CC: Get value", L"CC: Get value...")
	REAL (L"Time (s)", L"0.1")
	NATURAL (L"Index", L"1")
	OK
DO
	LOOP {
		iam (CC); // ?? generic
		Melder_informationReal (CC_getValue (me, GET_REAL (L"Time"), GET_INTEGER (L"Index")), 0);
	}
END

FORM (CC_paint, L"CC: Paint", L"CC: Paint...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	INTEGER (L"From coefficient", L"0")
	INTEGER (L"To coefficient", L"0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (CC);
		CC_paint (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"From coefficient"), GET_INTEGER (L"To coefficient"),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

FORM (CC_drawC0, L"CC: Draw c0", L"CC: Draw c0...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (CC);
		CC_drawC0 (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (CCs_to_DTW, L"CC: To DTW", L"CC: To DTW...")
	LABEL (L"", L"Distance  between cepstral coefficients")
	REAL (L"Cepstral weight", L"1.0")
	REAL (L"Log energy weight", L"0.0")
	REAL (L"Regression weight", L"0.0")
	REAL (L"Regression weight log energy", L"0.0")
	REAL (L"Regression coefficients window (s)", L"0.056")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	CC c1 = 0, c2 = 0;
	LOOP {
		iam (CC);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
    autoDTW thee = CCs_to_DTW (c1, c2, GET_REAL (L"Cepstral weight"), GET_REAL (L"Log energy weight"),
        GET_REAL (L"Regression weight"), GET_REAL (L"Regression weight log energy"),
        GET_REAL (L"Regression coefficients window"));
    DTW_findPath (thee.peek(), begin, end, slope);
	praat_new (thee.transfer(), 0);
END

DIRECT (CC_to_Matrix)
	LOOP {
		iam (CC);
		praat_new (CC_to_Matrix (me), my name);
	}
END

/******************* class CCA ********************************/

FORM (CCA_drawEigenvector, L"CCA: Draw eigenvector", L"Eigen: Draw eigenvector...")
	OPTIONMENU (L"X or Y", 1)
	OPTION (L"y")
	OPTION (L"x")
	INTEGER (L"Eigenvector number", L"1")
	LABEL (L"", L"Multiply by eigenvalue?")
	BOOLEAN (L"Component loadings", 0)
	LABEL (L"", L"Select part of the eigenvector:")
	INTEGER (L"left Element range", L"0")
	INTEGER (L"right Element range", L"0")
	REAL (L"left Amplitude range", L"-1.0")
	REAL (L"right Amplitude range", L"1.0")
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Connect points", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (CCA);
		CCA_drawEigenvector (me, GRAPHICS, GET_INTEGER (L"X or Y"), GET_INTEGER (L"Eigenvector number"),
			GET_INTEGER (L"left Element range"), GET_INTEGER (L"right Element range"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
			GET_INTEGER (L"Component loadings"), GET_REAL (L"Mark size"),
			GET_STRING (L"Mark string"), GET_INTEGER (L"Connect points"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (CCA_getNumberOfCorrelations)
	LOOP {
		iam (CCA);
		Melder_information (Melder_double (my numberOfCoefficients));
	}
END

FORM (CCA_getCorrelationCoefficient, L"CCA: Get canonical correlation coefficient", L"CCA: Get canonical correlation coefficient")
	NATURAL (L"Coefficient number", L"1")
	OK
DO
	LOOP {
		iam (CCA);
		Melder_information (Melder_double (CCA_getCorrelationCoefficient (me, GET_INTEGER (L"Coefficient number"))));
	}
END

FORM (CCA_getEigenvectorElement, L"CCA: Get eigenvector element", L"Eigen: Get eigenvector element...")
	OPTIONMENU (L"X or Y", 1)
	OPTION (L"y")
	OPTION (L"x")
	NATURAL (L"Eigenvector number", L"1")
	NATURAL (L"Element number", L"1")
	OK
DO
	LOOP {
		iam (CCA);
		Melder_information (Melder_double (CCA_getEigenvectorElement (me, GET_INTEGER (L"X or Y"),
		GET_INTEGER (L"Eigenvector number"), GET_INTEGER (L"Element number"))));
	}
END

FORM (CCA_getZeroCorrelationProbability, L"CCA: Get zero correlation probability", L"CCA: Get zero correlation probability...")
	NATURAL (L"Coefficient number", L"1")
	OK
DO
	LOOP {
		iam (CCA);
		double p, chisq; long ndf;
		CCA_getZeroCorrelationProbability (me, GET_INTEGER (L"Coefficient number"), &chisq, &ndf, &p);
		Melder_information (Melder_double (p), L" (=probability for chisq = ", Melder_double (chisq),
		L" and ndf = ", Melder_integer (ndf), L")");
	}
END

DIRECT (CCA_and_Correlation_factorLoadings)
	CCA cca = FIRST (CCA);
	Correlation c = FIRST (Correlation);
	praat_new (CCA_and_Correlation_factorLoadings (cca, c), Thing_getName (cca), L"_loadings");
END

FORM (CCA_and_Correlation_getVarianceFraction, L"CCA & Correlation: Get variance fraction", L"CCA & Correlation: Get variance fraction...")
	LABEL (L"", L"Get the fraction of variance from the data in set...")
	OPTIONMENU (L"X or Y", 1)
	OPTION (L"y")
	OPTION (L"x")
	LABEL (L"", L"extracted by...")
	NATURAL (L"left Canonical variate range", L"1")
	NATURAL (L"right Canonical variate range", L"1")
	OK
DO
	CCA cca = FIRST (CCA);
	Correlation c = FIRST (Correlation);
	int x_or_y = GET_INTEGER (L"X or Y");
	int cv_from = GET_INTEGER (L"left Canonical variate range");
	int cv_to = GET_INTEGER (L"right Canonical variate range");
	Melder_information (Melder_double (CCA_and_Correlation_getVarianceFraction (cca, c, x_or_y, cv_from, cv_to)),
		L" (fraction variance from ", (x_or_y == 1 ? L"y" : L"x"), L", extracted by canonical variates ",
		Melder_integer (cv_from), L" to ", Melder_integer (cv_to), L")");
END

FORM (CCA_and_Correlation_getRedundancy_sl, L"CCA & Correlation: Get Stewart-Love redundancy", L"CCA & Correlation: Get redundancy (sl)...")
	LABEL (L"", L"Get the redundancy of the data in set...")
	OPTIONMENU (L"X or Y", 1)
	OPTION (L"y")
	OPTION (L"x")
	LABEL (L"", L"extracted by...")
	NATURAL (L"left Canonical variate range", L"1")
	NATURAL (L"right Canonical variate range", L"1")
	LABEL (L"", L"...given the availability of the data in the other set.")
	OK
DO
	CCA cca = FIRST (CCA);
	Correlation c = FIRST (Correlation);
	int x_or_y = GET_INTEGER (L"X or Y");
	int cv_from = GET_INTEGER (L"left Canonical variate range");
	int cv_to = GET_INTEGER (L"right Canonical variate range");
	Melder_information (Melder_double (CCA_and_Correlation_getRedundancy_sl (cca, c, x_or_y, cv_from, cv_to)),
		L" (redundancy from ", (x_or_y == 1 ? L"y" : L"x"), L" extracted by canonical variates ",
		Melder_integer (cv_from), L" to ", Melder_integer (cv_to), L")");
END

DIRECT (CCA_and_TableOfReal_factorLoadings)
	CCA cca = FIRST (CCA);
	TableOfReal tr = FIRST (TableOfReal);
	praat_new (CCA_and_TableOfReal_factorLoadings (cca, tr), Thing_getName (cca), L"_loadings");
END

FORM (CCA_and_TableOfReal_scores, L"CCA & TableOfReal: To TableOfReal (scores)", L"CCA & TableOfReal: To TableOfReal (scores)...")
	INTEGER (L"Number of canonical correlations", L"0 (=all)")
	OK
DO
	CCA cca = FIRST (CCA);
	TableOfReal tr = FIRST (TableOfReal);
	praat_new (CCA_and_TableOfReal_scores (cca, tr, GET_INTEGER (L"Number of canonical correlations")),
		Thing_getName (cca), L"_scores");
END

FORM (CCA_and_TableOfReal_predict, L"CCA & TableOfReal: Predict", L"CCA & TableOfReal: Predict...")
	LABEL (L"", L"The data set from which to predict starts at...")
	INTEGER (L"Column number", L"1")
	OK
DO
	CCA cca = FIRST (CCA);
	TableOfReal tr = FIRST (TableOfReal);
	praat_new (CCA_and_TableOfReal_predict (cca, tr, GET_INTEGER (L"Column number")), tr->name, L"_", cca->name);
END

/***************** ChebyshevSeries ****************************************/

DIRECT (ChebyshevSeries_help)
	Melder_help (L"ChebyshevSeries");
END

FORM (ChebyshevSeries_create, L"Create ChebyshevSeries", L"Create ChebyshevSeries...")
	WORD (L"Name", L"cs")
	LABEL (L"", L"Domain")
	REAL (L"Xmin", L"-1")
	REAL (L"Xmax", L"1")
	LABEL (L"", L"ChebyshevSeries(x) = c[1] T[0](x) + c[2] T[1](x) + ... c[n+1] T[n](x)")
	LABEL (L"", L"T[k] is a Chebyshev polynomial of degree k")
	SENTENCE (L"Coefficients (c[k])", L"0 0 1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	praat_new (ChebyshevSeries_createFromString (xmin, xmax, GET_STRING (L"Coefficients")), GET_STRING (L"Name"));
END

DIRECT (ChebyshevSeries_to_Polynomial)
	LOOP {
		iam (ChebyshevSeries);
		praat_new (ChebyshevSeries_to_Polynomial (me), my name);
	}
END

/***************** ClassificationTable ****************************************/

DIRECT (ClassificationTable_help)
	Melder_help (L"ClassificationTable");
END

DIRECT (ClassificationTable_to_Confusion)
	LOOP {
		iam (ClassificationTable);
		praat_new (ClassificationTable_to_Confusion (me), 0);
	}
END

DIRECT (ClassificationTable_to_Correlation_columns)
	LOOP {
		iam (ClassificationTable);
		praat_new (ClassificationTable_to_Correlation_columns (me), my name, L"_col");
	}
END

DIRECT (ClassificationTable_to_Strings_maximumProbability)
	LOOP {
		iam (ClassificationTable);
		praat_new (ClassificationTable_to_Strings_maximumProbability (me), my name);
	}
END

/********************** Confusion *******************************************/

DIRECT (Confusion_help)
	Melder_help (L"Confusion");
END

FORM (Confusion_createSimple, L"Create simple Confusion", L"Create simple Confusion...")
	WORD (L"Name", L"simple")
	SENTENCE (L"Labels", L"u i a")
	OK
DO
	praat_new (Confusion_createSimple (GET_STRING (L"Labels")), GET_STRING (L"Name"));
END

FORM (Confusion_increase, L"Confusion: Increase", L"Confusion: Increase...")
	WORD (L"Stimulus", L"u")
	WORD (L"Response", L"i")
	OK
DO
	LOOP {
		iam (Confusion);
		Confusion_increase (me, GET_STRING (L"Stimulus"), GET_STRING (L"Response"));
		praat_dataChanged (me);
	}
END

FORM (Confusion_getValue, L"Confusion: Get value", 0)
	WORD (L"Stimulus", L"u")
	WORD (L"Response", L"i")
	OK
DO
	wchar_t *stim = GET_STRING (L"Stimulus");
	wchar_t *resp = GET_STRING (L"Response");
	LOOP {
		iam (Confusion);
		Melder_information (Melder_double (Confusion_getValue (me, stim, resp)),
		L" ( [\"", stim, L"\", \"",  resp, L"\"] )");
	}
END

FORM (Confusion_getResponseSum, L"Confusion: Get response sum", L"Confusion: Get response sum...")
	WORD (L"Response", L"u")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_double (TableOfReal_getColumnSumByLabel (me, GET_STRING (L"Response"))));
	}
END

FORM (Confusion_getStimulusSum, L"Confusion: Get stimulus sum", L"Confusion: Get stimulus sum...")
	WORD (L"Stimulus", L"u")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_double (TableOfReal_getRowSumByLabel (me, GET_STRING (L"Stimulus"))));
	}
END

DIRECT (Confusion_to_TableOfReal_marginals)
	LOOP {
		iam (TableOfReal);
		praat_new (Confusion_to_TableOfReal_marginals (me), my name);
	}
END

DIRECT (Confusion_difference)
	Confusion c1 = 0, c2 = 0;
	LOOP {
		iam (Confusion);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	praat_new (Confusion_difference (c1, c2), L"diffs");
END

FORM (Confusion_condense, L"Confusion: Condense", L"Confusion: Condense...")
	SENTENCE (L"Search", L"^(u|i)$")
	SENTENCE (L"Replace", L"high")
	INTEGER (L"Replace limit", L"0 (=unlimited)")
	RADIO (L"Search and replace are", 2)
	RADIOBUTTON (L"Literals")
	RADIOBUTTON (L"Regular Expressions")
	OK
DO
	LOOP {
		iam (Confusion);
		praat_new (Confusion_condense (me, GET_STRING (L"Search"), GET_STRING (L"Replace"),
		GET_INTEGER (L"Replace limit"), GET_INTEGER (L"Search and replace are") - 1), my name, L"_cnd");
	}
END

FORM (Confusion_group, L"Confusion: Group stimuli & responses", L"Confusion: Group...")
	SENTENCE (L"Stimuli & Responses", L"u i")
	SENTENCE (L"New label", L"high")
	INTEGER (L"New label position", L"0 (=at start)")
	OK
DO
	const wchar_t *newlabel = GET_STRING (L"New label");
	LOOP {
		iam (Confusion);
		praat_new (Confusion_group (me, GET_STRING (L"Stimuli & Responses"), newlabel,
		GET_INTEGER (L"New label position")), Thing_getName (me), L"_sr", newlabel);
	}
END

FORM (Confusion_groupStimuli, L"Confusion: Group stimuli", L"Confusion: Group stimuli...")
	SENTENCE (L"Stimuli", L"u i")
	SENTENCE (L"New label", L"high")
	INTEGER (L"New label position", L"0")
	OK
DO
	const wchar_t *newlabel = GET_STRING (L"New label");
	LOOP {
		iam (Confusion);
		praat_new (Confusion_groupStimuli (me, GET_STRING (L"Stimuli"), newlabel,
		GET_INTEGER (L"New label position")), Thing_getName (me), L"_s", newlabel);
	}
END

FORM (Confusion_groupResponses, L"Confusion: Group responses", L"Confusion: Group responses...")
	SENTENCE (L"Responses", L"a i")
	SENTENCE (L"New label", L"front")
	INTEGER (L"New label position", L"0")
	OK
DO
	const wchar_t *newlabel = GET_STRING (L"New label");
	LOOP {
		iam (Confusion);
		praat_new (Confusion_groupResponses (me, GET_STRING (L"Responses"), newlabel,
		GET_INTEGER (L"New label position")), Thing_getName (me), L"_s", newlabel);
	}
END

FORM (Confusion_drawAsNumbers, L"", L"")
	BOOLEAN (L"Draw marginals", 1)
	RADIO (L"Format", 3)
	RADIOBUTTON (L"decimal")
	RADIOBUTTON (L"exponential")
	RADIOBUTTON (L"free")
	RADIOBUTTON (L"rational")
	NATURAL (L"Precision", L"5")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Confusion);
		Confusion_drawAsNumbers (me, GRAPHICS, GET_INTEGER (L"Draw marginals"),
		GET_INTEGER (L"Format"), GET_INTEGER (L"Precision"));
	}
END

DIRECT (Confusion_getFractionCorrect)
	LOOP {
		iam (Confusion);
		double f; long n;
		Confusion_getFractionCorrect (me, &f, &n);
		Melder_information (Melder_double (f), L" (fraction correct)");
	}
END

/******************* Confusion & Matrix *************************************/

FORM (Confusion_Matrix_draw, L"Confusion & Matrix: Draw confusions with arrows", 0)
	INTEGER (L"Category position", L"0 (=all)")
	REAL (L"Lower level (%)", L"0")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	long categoryPosition = GET_INTEGER (L"Category position");
	REQUIRE (categoryPosition >= 0, L"Category position must be >= 0")
	Confusion conf = FIRST (Confusion);
	Matrix mat = FIRST (Matrix);
	Confusion_Matrix_draw (conf, mat, GRAPHICS, categoryPosition, GET_REAL (L"Lower level"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
END

/**********************Correlation *******************************************/

DIRECT (Correlation_help)
	Melder_help (L"Correlation");
END

FORM (Correlation_confidenceIntervals, L"Correlation: Confidence intervals...", L"Correlation: Confidence intervals...")
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	INTEGER (L"Number of tests (Bonferroni correction)", L"0")
	RADIO (L"Approximation", 1)
	RADIOBUTTON (L"Ruben")
	RADIOBUTTON (L"Fisher")
	OK
DO
	double cl = GET_REAL (L"Confidence level");
	double numberOfTests = GET_INTEGER (L"Number of tests");
	LOOP {
		iam (Correlation);
		praat_new (Correlation_confidenceIntervals (me, cl, numberOfTests, GET_INTEGER (L"Approximation")), L"conf_intervals");
	}
END

FORM (Correlation_testDiagonality_bartlett, L"Correlation: Get diagonality (bartlett)", L"SSCP: Get diagonality (bartlett)...")
	NATURAL (L"Number of contraints", L"1")
	OK
DO
	double chisq, p;
	long nc = GET_INTEGER (L"Number of contraints");
	LOOP {
		iam (Correlation);
		Correlation_testDiagonality_bartlett (me, nc, &chisq, &p);
		Melder_information (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (my numberOfRows * (my numberOfRows - 1) / 2));
	}
END

DIRECT (Correlation_to_PCA)
	LOOP {
		iam (Correlation);
		praat_new (SSCP_to_PCA (me), my name);
	}
END

/**********************Covariance *******************************************/

DIRECT (Covariance_help)
	Melder_help (L"Covariance");
END

FORM (Covariance_createSimple, L"Create simple Covariance", L"Create simple Covariance...")
	WORD (L"Name", L"c")
	SENTENCE (L"Covariances", L"1.0 0.0 1.0")
	SENTENCE (L"Centroid", L"0.0 0.0")
	POSITIVE (L"Number of observations", L"100.0")
	OK
DO
	praat_new (Covariance_createSimple (GET_STRING (L"Covariances"), GET_STRING (L"Centroid"),
		GET_REAL (L"Number of observations")), GET_STRING (L"Name"));
END

FORM (Covariance_getProbabilityAtPosition, L"Covariance: Get probability at position", 0)
	SENTENCE (L"Position", L"10.0 20.0")
	OK
DO
	wchar_t *position = GET_STRING (L"Position");
	LOOP {
		iam (Covariance);
		double p = Covariance_getProbabilityAtPosition_string (me, position);
		Melder_information (Melder_double (p), L" (= probability at position ", position, L")");
	}
END

FORM (Covariance_getSignificanceOfOneMean, L"Covariance: Get significance of one mean", L"Covariance: Get significance of one mean...")
	LABEL (L"", L"Get probability that the mean with")
	NATURAL (L"Index", L"1")
	LABEL (L"", L"differs from")
	REAL (L"Value", L"0.0")
	LABEL (L"", L"(Null hypothesis: the observed difference is due to chance.)")
	OK
DO
	LOOP {
		iam (Covariance);
		double t, p; double ndf;
		Covariance_getSignificanceOfOneMean (me, GET_INTEGER (L"Index"), GET_REAL (L"Value"), &p, &t , &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on t = ", Melder_double (t), L" and ndf = ", Melder_integer (ndf));
	}
END

FORM (Covariance_getSignificanceOfMeansDifference, L"Covariance: Get significance of means difference", L"Covariance: Get significance of means difference...")
	LABEL (L"", L"Get probability that the difference between means")
	NATURAL (L"Index1", L"1")
	NATURAL (L"Index2", L"2")
	LABEL (L"", L"differs from")
	REAL (L"Value", L"0.0")
	LABEL (L"", L"when the means are")
	BOOLEAN (L"Paired", 1)
	LABEL (L"", L"and have")
	BOOLEAN (L"Equal variances", 1)
	OK
DO
	LOOP {
		iam (Covariance);
		double t, p; double ndf;
		Covariance_getSignificanceOfMeansDifference (me, GET_INTEGER (L"Index1"), GET_INTEGER (L"Index2"),
		GET_REAL (L"Value"), GET_INTEGER (L"Paired"), GET_INTEGER (L"Equal variances"), &p, &t , &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on t = ",
		Melder_double (t), L"and ndf = ", Melder_integer (ndf), L")");
	}
END

FORM (Covariance_getSignificanceOfOneVariance, L"Covariance: Get significance of one variance", L"Covariance: Get significance of one variance...")
	LABEL (L"", L"Get probability that the variance with")
	NATURAL (L"Index", L"1")
	LABEL (L"", L"differs from")
	REAL (L"Value", L"0.0")
	LABEL (L"", L"(Null hypothesis: the observed difference is due to chance.)")
	OK
DO
	LOOP {
		iam (Covariance);
		double chisq, p; long ndf;
		Covariance_getSignificanceOfOneVariance (me, GET_INTEGER (L"Index"), GET_REAL (L"Value"), &p, &chisq , &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on chisq = ", Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
	}
END

FORM (Covariance_getSignificanceOfVariancesRatio, L"Covariance: Get significance of variances ratio", L"Covariance: Get significance of variances ratio...")
	NATURAL (L"Index1", L"1")
	NATURAL (L"Index2", L"2")
	REAL (L"Hypothesized ratio", L"1.0")
	OK
DO
	LOOP {
		iam (Covariance);
		double f, p; long ndf;
		Covariance_getSignificanceOfVariancesRatio (me, GET_INTEGER (L"Index1"), GET_INTEGER (L"Index2"),
		GET_REAL (L"Hypothesized ratio"), &p, &f , &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on F = ", Melder_double (f),
		L"and ndf1 = ", Melder_integer (ndf), L" and ndf2 = ", Melder_integer (ndf));
	}
END

FORM (Covariance_getFractionVariance, L"Covariance: Get fraction variance", L"Covariance: Get fraction variance...")
	NATURAL (L"From dimension", L"1")
	NATURAL (L"To dimension", L"1")
	OK
DO
	LOOP {
		iam (Covariance);
		Melder_information (Melder_double (SSCP_getFractionVariation (me, GET_INTEGER (L"From dimension"), GET_INTEGER (L"To dimension"))));
	}
END

FORM (Covariances_reportMultivariateMeanDifference, L"Covariances: Report multivariate mean difference",
      L"Covariances: Report multivariate mean difference...")
	BOOLEAN (L"Covariances are equal", 1)
	OK
DO
	Covariance c1 = 0, c2 = 0;
	LOOP {
		iam (Covariance);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	double prob, fisher, df1, df2, difference;
	int equalCovariances = GET_INTEGER (L"Covariances are equal");
	MelderInfo_open ();
	difference = Covariances_getMultivariateCentroidDifference (c1, c2, equalCovariances, &prob, &fisher, &df1, &df2);
	MelderInfo_writeLine (L"Under the assumption that the two covariances are", (equalCovariances ? L" " : L" not "), L"equal:");
	MelderInfo_writeLine (L"Difference between multivariate means = ", Melder_double (difference));
	MelderInfo_writeLine (L"Fisher's F = ", Melder_double (fisher));
	MelderInfo_writeLine (L"Significance from zero = ", Melder_double (prob));
	MelderInfo_writeLine (L"Degrees of freedom = ", Melder_double (df1), L", ", Melder_double (df2));
	MelderInfo_writeLine (L"(Number of observations = ", Melder_integer (c1->numberOfObservations), L", ",
		Melder_integer (c2->numberOfObservations));
	MelderInfo_writeLine (L"Dimension of covariance matrices = ", Melder_integer (c1-> numberOfRows), L")");
	MelderInfo_close ();
END

FORM (Covariance_to_TableOfReal_randomSampling, L"Covariance: To TableOfReal (random sampling)", L"Covariance: To TableOfReal (random sampling)...")
	INTEGER (L"Number of data points", L"0")
	OK
DO
	LOOP {
		iam (Covariance);
		praat_new (Covariance_to_TableOfReal_randomSampling (me, GET_INTEGER (L"Number of data points")), my name);
	}
END

DIRECT (Covariances_reportEquality)
	autoCollection set = praat_getSelectedObjects ();
	MelderInfo_open ();
	{
		double chisq, p, df;
		Covariances_equality (set.peek(), 1, &p, &chisq, &df);
		MelderInfo_writeLine (L"Difference between covariance matrices:");
		MelderInfo_writeLine (L"Significance of difference (bartlett) = ", Melder_double (p));
		MelderInfo_writeLine (L"Chi-squared = ", Melder_double (chisq));
		MelderInfo_writeLine (L"Degrees of freedom = ", Melder_double (df));
		Covariances_equality (set.peek(), 2, &p, &chisq, &df);
		MelderInfo_writeLine (L"Significance of difference (wald) = ", Melder_double (p));
		MelderInfo_writeLine (L"Chi-squared = ", Melder_double (chisq));
		MelderInfo_writeLine (L"Degrees of freedom = ", Melder_double (df));
	}
	MelderInfo_close ();
END

DIRECT (Covariance_to_Correlation)
	LOOP {
		iam (Covariance);
		praat_new (SSCP_to_Correlation (me), my name);
	}
END

DIRECT (Covariance_to_PCA)
	LOOP {
		iam (Covariance);
		praat_new (SSCP_to_PCA (me), my name);
	}
END

FORM (Covariance_and_TableOfReal_mahalanobis, L"Covariance & TableOfReal: To TableOfReal (mahalanobis)", L"Covariance & TableOfReal: To TableOfReal (mahalanobis)...")
	BOOLEAN (L"Centroid from table", 0)
	OK
DO
	Covariance cov = FIRST (Covariance);
	TableOfReal tr = FIRST (TableOfReal);
	praat_new (Covariance_and_TableOfReal_mahalanobis (cov, tr, GET_INTEGER (L"Centroid from table")), L"mahalanobis");
END

/********************** Discriminant **********************************/

DIRECT (Discriminant_help)
	Melder_help (L"Discriminant");
END

DIRECT (Discriminant_setGroupLabels)
	Discriminant me = FIRST (Discriminant);
	Strings ss = FIRST (Strings);
	Discriminant_setGroupLabels (me, ss);
	praat_dataChanged (me);
END

FORM (Discriminant_and_Pattern_to_Categories, L"Discriminant & Pattern: To Categories", L"Discriminant & Pattern: To Categories...")
	BOOLEAN (L"Pool covariance matrices", 1)
	BOOLEAN (L"Use apriori probabilities", 1)
	OK
DO
	Discriminant me = FIRST (Discriminant);
	Pattern pat = FIRST (Pattern);
	praat_new (Discriminant_and_Pattern_to_Categories (me, pat, GET_INTEGER (L"Pool covariance matrices"),
		GET_INTEGER (L"Use apriori probabilities")), my name, L"_", pat->name);
END

FORM (Discriminant_and_TableOfReal_to_Configuration, L"Discriminant & TableOfReal: To Configuration", L"Discriminant & TableOfReal: To Configuration...")
		INTEGER (L"Number of dimensions", L"0")
		OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	REQUIRE (dimension >= 0, L"Number of dimensions must be greater equal zero.")
	Discriminant me = FIRST (Discriminant);
	TableOfReal tr = FIRST_GENERIC (TableOfReal);
	praat_new (Discriminant_and_TableOfReal_to_Configuration (me, tr, dimension), my name, L"_", tr->name);
END

DIRECT (hint_Discriminant_and_TableOfReal_to_ClassificationTable)
	Melder_information (L"You can use the Discriminant as a classifier by \nselecting a Discriminant and a TableOfReal object together.");
END

FORM (Discriminant_and_TableOfReal_to_ClassificationTable, L"Discriminant & TableOfReal: To ClassificationTable", L"Discriminant & TableOfReal: To ClassificationTable...")
	BOOLEAN (L"Pool covariance matrices", 1)
	BOOLEAN (L"Use apriori probabilities", 1)
	OK
DO
	Discriminant me = FIRST (Discriminant);
	TableOfReal tr = FIRST_GENERIC (TableOfReal);
	praat_new (Discriminant_and_TableOfReal_to_ClassificationTable (me, tr,
		GET_INTEGER (L"Pool covariance matrices"), GET_INTEGER (L"Use apriori probabilities")),
		my name, L"_", tr->name);
END

FORM (Discriminant_and_TableOfReal_mahalanobis, L"Discriminant & TableOfReal: To TableOfReal (mahalanobis)", L"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...")
	SENTENCE (L"Group label", L"")
	BOOLEAN (L"Pool covariance matrices", 0)
	OK
DO
	Discriminant me = FIRST (Discriminant);
	TableOfReal tr = FIRST (TableOfReal);
	long group = Discriminant_groupLabelToIndex (me, GET_STRING (L"Group label"));
	REQUIRE (group > 0, L"Group label does not exist.")
	praat_new (Discriminant_and_TableOfReal_mahalanobis (me, tr, group, GET_INTEGER (L"Pool covariance matrices")), L"mahalanobis");
END

FORM (Discriminant_getWilksLambda, L"Discriminant: Get Wilks' lambda", L"Discriminant: Get Wilks' lambda...")
	LABEL (L"", L"Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	INTEGER (L"From", L"1")
	OK
DO
	long from = GET_INTEGER (L"From");
	REQUIRE (from >= 1, L"Number must be greater than or equal to one.")
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_double (Discriminant_getWilksLambda (me, from)));
	}
END

FORM (Discriminant_getCumulativeContributionOfComponents, L"Discriminant: Get cumulative contribution of components", L"Eigen: Get cumulative contribution of components...")
	NATURAL (L"From component", L"1")
	NATURAL (L"To component", L"1")
	OK
DO
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_double (Eigen_getCumulativeContributionOfComponents (me,
		GET_INTEGER (L"From component"), GET_INTEGER (L"To component"))));
	}
END


FORM (Discriminant_getPartialDiscriminationProbability, L"Discriminant: Get partial discrimination probability", L"Discriminant: Get partial discrimination probability...")
	INTEGER (L"Number of dimensions", L"1")
	OK
DO
	long ndf, n = GET_INTEGER (L"Number of dimensions");
	double chisq, p;
	REQUIRE (n >= 0, L"Number of dimensions must be greater than or equal to zero.")
	LOOP {
		iam (Discriminant);
		Discriminant_getPartialDiscriminationProbability (me, n, &p, &chisq, &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on chisq = ", Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
	}
END

DIRECT (Discriminant_getHomegeneityOfCovariances_box)
	LOOP {
		iam (Discriminant);
		double chisq, p; long ndf;
		SSCPs_getHomegeneityOfCovariances_box ( (SSCPs) my groups, &p, &chisq, &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
	}
END

DIRECT (Discriminant_reportEqualityOfCovariances_wald)
	MelderInfo_open ();
	LOOP {
		iam (Discriminant);
		double chisq, prob, df;
		Covariances_equality ( (Collection) my groups, 2, &prob, &chisq, &df);
		MelderInfo_writeLine (L"Wald test for equality of covariance matrices:");
		MelderInfo_writeLine (L"Chi squared: ", Melder_double (chisq));
		MelderInfo_writeLine (L"Significance: ", Melder_double (prob));
		MelderInfo_writeLine (L"Degrees of freedom: ", Melder_double (df));
		MelderInfo_writeLine (L"Number of matrices: ", Melder_integer (my groups -> size));
	}
	MelderInfo_close ();
END

FORM (Discriminant_getConcentrationEllipseArea, L"Discriminant: Get concentration ellipse area", L"Discriminant: Get concentration ellipse area...")
	SENTENCE (L"Group label", L"")
	POSITIVE (L"Number of sigmas", L"1.0")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	OK
DO
	LOOP {
		iam (Discriminant);
		long group = Discriminant_groupLabelToIndex (me, GET_STRING (L"Group label"));
		REQUIRE (group > 0, L"Group label does not exist.")
		Melder_information (Melder_double (Discriminant_getConcentrationEllipseArea (me, group,
		GET_REAL (L"Number of sigmas"), 0, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"))));
	}
END

FORM (Discriminant_getConfidenceEllipseArea, L"Discriminant: Get confidence ellipse area", L"Discriminant: Get confidence ellipse area...")
	SENTENCE (L"Group label", L"")
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	OK
DO
	LOOP {
		iam (Discriminant);
		long group = Discriminant_groupLabelToIndex (me, GET_STRING (L"Group label"));
		REQUIRE (group > 0, L"Group label does not exist.")
		Melder_information (Melder_double (Discriminant_getConcentrationEllipseArea (me, group,
		GET_REAL (L"Confidence level"), 1, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"))));
	}
END

FORM (Discriminant_getLnDeterminant_group, L"Discriminant: Get determinant (group)", L"Discriminant: Get determinant (group)...")
	SENTENCE (L"Group label", L"")
	OK
DO
	LOOP {
		iam (Discriminant);
		long group = Discriminant_groupLabelToIndex (me, GET_STRING (L"Group label"));
		REQUIRE (group > 0, L"Group label does not exist.")
		Melder_information (Melder_double (Discriminant_getLnDeterminant_group (me, group)));
	}
END

DIRECT (Discriminant_getLnDeterminant_total)
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_double (Discriminant_getLnDeterminant_total (me)));
	}
END

FORM (Discriminant_invertEigenvector, L"Discriminant: Invert eigenvector", 0)
	NATURAL (L"Index of eigenvector", L"1")
	OK
DO
	LOOP {
		iam (Discriminant);
		Eigen_invertEigenvector (me, GET_INTEGER (L"Index of eigenvector"));
		praat_dataChanged (me);
	}
END

FORM (Discriminant_drawSigmaEllipses, L"Discriminant: Draw sigma ellipses", L"Discriminant: Draw sigma ellipses...")
	POSITIVE (L"Number of sigmas", L"1.0")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, GET_REAL (L"Number of sigmas"), 0, 0,
		GET_INTEGER (L"Discriminant plane"), GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Discriminant_drawOneSigmaEllipse, L"Discriminant: Draw one sigma ellipse", L"Discriminant: Draw one sigma ellipse...")
	SENTENCE (L"Label", L"")
	POSITIVE (L"Number of sigmas", L"1.0")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, GET_REAL (L"Number of sigmas"), 0,
		GET_STRING (L"Label"), GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Discriminant_drawConfidenceEllipses, L"Discriminant: Draw confidence ellipses", 0)
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS,
		GET_REAL (L"Confidence level"), 1, NULL, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END


FORM (Discriminant_drawOneConfidenceEllipse, L"Discriminant: Draw one confidence ellipse", 0)
	SENTENCE (L"Label", L"")
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS,
		GET_REAL (L"Confidence level"), 1, GET_STRING (L"Label"), GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Discriminant_extractBetweenGroupsSSCP)
	LOOP {
		iam (Discriminant);
		praat_new (Discriminant_extractBetweenGroupsSSCP (me), 0);
	}
END

DIRECT (Discriminant_extractGroupCentroids)
	LOOP {
		iam (Discriminant);
		praat_new (Discriminant_extractGroupCentroids (me), L"centroids");
	}
END

DIRECT (Discriminant_extractGroupStandardDeviations)
	LOOP {
		iam (Discriminant);
		praat_new (Discriminant_extractGroupStandardDeviations (me), L"group_stddevs");
	}
END

DIRECT (Discriminant_extractGroupLabels)
	LOOP {
		iam (Discriminant);
		praat_new (Discriminant_extractGroupLabels (me), L"group_labels");
	}
END

DIRECT (Discriminant_extractPooledWithinGroupsSSCP)
	LOOP {
		iam (Discriminant);
		praat_new (Discriminant_extractPooledWithinGroupsSSCP (me), L"pooled_within");
	}
END

FORM (Discriminant_extractWithinGroupSSCP, L"Discriminant: Extract within-group SSCP", L"Discriminant: Extract within-group SSCP...")
	NATURAL (L"Group index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Group index");
	LOOP {
		iam (Discriminant);
		praat_new (Discriminant_extractWithinGroupSSCP (me, index), my name, L"_g", Melder_integer (index));
	}
END

DIRECT (Discriminant_getNumberOfFunctions)
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_integer (Discriminant_getNumberOfFunctions (me)));
	}
END

DIRECT (Discriminant_getDimensionOfFunctions)
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_integer (Eigen_getDimensionOfComponents (me)));
	}
END

DIRECT (Discriminant_getNumberOfGroups)
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_integer (Discriminant_getNumberOfGroups (me)));
	}
END

FORM (Discriminant_getNumberOfObservations, L"Discriminant: Get number of observations", L"Discriminant: Get number of observations...")
	INTEGER (L"Group", L"0 (=total)")
	OK
DO
	LOOP {
		iam (Discriminant);
		Melder_information (Melder_integer (Discriminant_getNumberOfObservations (me, GET_INTEGER (L"Group"))));
	}
END


/********************** DTW *******************************************/

FORM (DTW_and_Polygon_findPathInside, L"DTW & Polygon: Find path inside", 0)
    RADIO (L"Slope constraint", 1)
    RADIOBUTTON (L"no restriction")
    RADIOBUTTON (L"1/3 < slope < 3")
    RADIOBUTTON (L"1/2 < slope < 2")
    RADIOBUTTON (L"2/3 < slope < 3/2")
    OK
DO
    int localSlope = GET_INTEGER (L"Slope constraint");
    DTW me = FIRST (DTW);
    Polygon thee = FIRST (Polygon);
    DTW_and_Polygon_findPathInside (me, thee, localSlope, 0);

END

FORM (DTW_and_Polygon_to_Matrix_cummulativeDistances, L"DTW & Polygon: To Matrix (cumm. distances)", 0)
    RADIO (L"Slope constraint", 1)
    RADIOBUTTON (L"no restriction")
    RADIOBUTTON (L"1/3 < slope < 3")
    RADIOBUTTON (L"1/2 < slope < 2")
    RADIOBUTTON (L"2/3 < slope < 3/2")
    OK
DO
    int localSlope = GET_INTEGER (L"Slope constraint");
    DTW me = FIRST (DTW);
    Polygon thee = FIRST (Polygon);
    autoMatrix him = DTW_and_Polygon_to_Matrix_cummulativeDistances (me, thee, localSlope);
    praat_new (him.transfer(), my name, L"_", Melder_integer (localSlope));
END

FORM (DTW_and_Sounds_draw, L"DTW & Sounds: Draw", L"DTW & Sounds: Draw...")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Sound s1 = 0, s2 = 0; DTW dtw = 0;
	LOOP {
		iam (Data);
		if (CLASS == classSound) {
			(s1 ? s2 : s1) = (Sound) me;
		} else if (CLASS == classDTW) {
			dtw = (DTW) me;
		}
	}
	Melder_assert (s1 && s2 && dtw);
	autoPraatPicture picture;
	DTW_and_Sounds_draw (dtw, s2, s1, GRAPHICS, GET_REAL (L"left Horizontal range"),
		GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish"));
END

FORM (DTW_and_Sounds_drawWarpX, L"DTW & Sounds: Draw warp (x)", L"DTW & Sounds: Draw warp (x)...")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	REAL (L"Time (s)", L"0.1")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Sound s1 = 0, s2 = 0;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	DTW dtw = FIRST (DTW);
	autoPraatPicture picture;
	DTW_and_Sounds_drawWarpX (dtw, s2, s1, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Time"), GET_INTEGER (L"Garnish"));
END

void DTW_constraints_addCommonFields (void *dia) {
	Any radio;
	LABEL (L"", L"Boundary conditions")
	BOOLEAN (L"Match begin positions", 0)
	BOOLEAN (L"Match end positions", 0)
	RADIO (L"Slope constraint", 1)
	RADIOBUTTON (L"no restriction")
	RADIOBUTTON (L"1/3 < slope < 3")
	RADIOBUTTON (L"1/2 < slope < 2")
	RADIOBUTTON (L"2/3 < slope < 3/2")
}

void DTW_constraints_getCommonFields (void *dia, int *begin, int *end, int *slope) {
	*begin = GET_INTEGER (L"Match begin positions");
	*end = GET_INTEGER (L"Match end positions");
	*slope = GET_INTEGER (L"Slope constraint");
}

DIRECT (DTW_help) Melder_help (L"DTW"); END

FORM (DTW_drawPath, L"DTW: Draw path", 0)
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 0);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawPath (me, GRAPHICS, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (DTW_drawDistancesAlongPath, L"DTW: Draw distances along path", 0)
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 0);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawDistancesAlongPath (me, GRAPHICS, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (DTW_paintDistances, L"DTW: Paint distances", 0)
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	BOOLEAN (L"Garnish", 0);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_paintDistances (me, GRAPHICS, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

FORM (DTW_drawWarpX, L"DTW: Draw warp (x)", L"DTW: Draw warp (x)...")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	REAL (L"Time (s)", L"0.1")
	BOOLEAN (L"Garnish", 0);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawWarpX (me, GRAPHICS, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Time"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (DTW_getStartTimeX)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my xmin), L" s (= start time along x)");
	}
END

DIRECT (DTW_getEndTimeX)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my xmax), L" s (= end time along x)");
	}
END

DIRECT (DTW_getTotalDurationX)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my xmax - my xmin), L" s (= total duration along x)");
	}
END

DIRECT (DTW_getStartTimeY)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my ymin), L" s (= start time along y)");
	}
END

DIRECT (DTW_getEndTimeY)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my ymax), L" s (= end time along y)");
	}
END

DIRECT (DTW_getTotalDurationY)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my ymax - my ymin), L" s (= total duration along y)");
	}
END

DIRECT (DTW_getNumberOfFramesX)
	LOOP {
		iam (DTW);
		Melder_information (Melder_integer (my nx), L" (= number of frames along x)");
	}
END

DIRECT (DTW_getTimeStepX)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my dx), L" s (= time step along x)");
	}
END

FORM (DTW_getTimeFromFrameNumberX, L"DTW: Get time from frame number (x)", 0)
	NATURAL (L"Frame number (x)", L"1")
	OK
DO
	double column = GET_INTEGER (L"Frame number");
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (Matrix_columnToX (me, column)), L" s (= y time at x frame ", Melder_integer (column), L")");
	}
END

FORM (DTW_getFrameNumberFromTimeX, L"DTW: Get frame number from time (x)", 0)
	REAL (L"Time along x (s)", L"0.1")
	OK
DO
	double time = GET_REAL (L"Time along x");
	LOOP {
		iam (DTW);
		if (time < my xmin || time > my xmax) {
			Melder_throw (me, "Time outside x domain.");
		}
		long iframe = floor (Matrix_xToColumn (me, time) + 0.5);
		Melder_information (Melder_integer (iframe), L" (= x frame at y time ", Melder_double (time), L")");
	}
END

DIRECT (DTW_getNumberOfFramesY)
	LOOP {
		iam (DTW);
		Melder_information (Melder_integer (my ny), L" (= number of frames along y)");
	}
END

DIRECT (DTW_getTimeStepY)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my dy), L" s (= time step along y)");
	}
END

FORM (DTW_getTimeFromFrameNumberY, L"DTW: Get time from frame number (y)", 0)
	NATURAL (L"Frame number (y)", L"1")
	OK
DO
	double row = GET_INTEGER (L"Frame number");
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (Matrix_rowToY (me, row)), L" s (= x time at y frame ", Melder_integer (row), L")");
	}
END

FORM (DTW_getFrameNumberFromTimeY, L"DTW: Get frame number from time (y)", 0)
	REAL (L"Time along y (s)", L"0.1")
	OK
DO
	double time = GET_REAL (L"Time along y");
	LOOP {
		iam (DTW);
		if (time < my ymin || time > my ymax) {
			Melder_throw (me, "Time outside y domain.");
		}
		long iframe = floor (Matrix_yToRow (me, time) + 0.5);
		Melder_information (Melder_integer (iframe), L" (= y frame at x time ", Melder_double (time), L")");
	}
END


FORM (DTW_getPathY, L"DTW: Get time along path", L"DTW: Get time along path...")
	REAL (L"Time (s)", L"0.0")
	OK
DO
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (DTW_getPathY (me, GET_REAL (L"Time"))));
	}
END

FORM (DTW_getYTimeFromXTime, L"DTW: Get y time from x time", L"DTW: Get y time from x time...")
	REAL (L"Time at x (s)", L"0.0")
	OK
DO
	double time = GET_REAL (L"Time at x");
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (DTW_getYTimeFromXTime (me, time)), L" s (= y time at z time ", Melder_double (time), L")");
	}
END

FORM (DTW_getXTimeFromYTime, L"DTW: Get x time from y time", L"DTW: Get x time from y time...")
	REAL (L"Time at y (s)", L"0.0")
	OK
DO
	double time = GET_REAL (L"Time at y");
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (DTW_getXTimeFromYTime (me, time)), L" s (= x time at y time ", Melder_double (time), L")");
	}
END

FORM (DTW_getMaximumConsecutiveSteps, L"DTW: Get maximum consecutive steps", L"DTW: Get maximum consecutive steps...")
	OPTIONMENU (L"Direction", 1)
	OPTION (L"X")
	OPTION (L"Y")
	OPTION (L"Diagonaal")
	OK
DO
	int direction[] = {DTW_START, DTW_X, DTW_Y, DTW_XANDY};
	const wchar_t *string[] = {L"", L"x", L"y", L"diagonal"};
	int d = GET_INTEGER (L"Direction");
	LOOP {
		iam (DTW);
		Melder_information (Melder_integer (DTW_getMaximumConsecutiveSteps (me, direction[d])),
		L" (= maximum number of consecutive steps in ", string[d], L" direction)");
	}
END

DIRECT (DTW_getWeightedDistance)
	LOOP {
		iam (DTW);
		Melder_information (Melder_double (my weightedDistance));
	}
END

FORM (DTW_getDistanceValue, L"DTW: Get distance value", 0)
	REAL (L"Time at x (s)", L"0.1")
	REAL (L"Time at y (s)", L"0.1")
	OK
DO
	double xtime = GET_REAL (L"Time at x");
	double ytime = GET_REAL (L"Time at y");
	double dist;
	LOOP {
		iam (DTW);
		if (xtime < my xmin || xtime > my xmax || ytime < my ymin || ytime > my ymax) {
			dist = NUMundefined;
		} else
		{
			long irow = Matrix_yToNearestRow (me, ytime);
			long icol = Matrix_xToNearestColumn (me, xtime);
			dist = my z[irow][icol];
		}
		Melder_information (Melder_double (dist), L" (= distance at (", Melder_double (xtime), L", ", Melder_double (ytime), L"))");
	}
END

DIRECT (DTW_getMinimumDistance)
	LOOP {
		iam (DTW);
		double minimum = NUMundefined, maximum = NUMundefined;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		Melder_informationReal (minimum, 0);
	}
END

DIRECT (DTW_getMaximumDistance)
	LOOP {
		iam (DTW);
		double minimum = NUMundefined, maximum = NUMundefined;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		Melder_informationReal (maximum, 0);
	}
END

FORM (DTW_formulaDistances, L"DTW: Formula (distances)", 0)
	LABEL (L"label", L"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	LOOP {
		iam (DTW);
		autoMatrix cp = DTW_to_Matrix_distances (me);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (L"formula"), interpreter, 0);
			double minimum, maximum;
			Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
			if (minimum < 0) {
				DTW_and_Matrix_replace (me, cp.peek()); // restore original
				Melder_throw ("Execution of the formula has made some distance(s) negative which is not allowed.");
			}
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (DTW_setDistanceValue, L"DTW: Set distance value", 0)
	REAL (L"Time at x (s)", L"0.1")
	REAL (L"Time at y (s)", L"0.1")
	REAL (L"New value", L"0.0")
	OK
DO
	double xtime = GET_REAL (L"Time at x");
	double ytime = GET_REAL (L"Time at y");
	double val = GET_REAL (L"New value");
	if (val < 0) {
		Melder_throw ("Distances cannot be negative.");
	}
	LOOP {
		iam (DTW);
		if (xtime < my xmin || xtime > my xmax) {
			Melder_throw ("Time at x outside domain.");
		}
		if (ytime < my ymin || ytime > my ymax) {
			Melder_throw ("Time at y outside domain.");
		}
		long irow = Matrix_yToNearestRow (me, ytime);
		long icol = Matrix_xToNearestColumn (me, xtime);
		my z[irow][icol] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

FORM (DTW_findPath, L"DTW: Find path", 0)
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	LOOP {
		iam (DTW);
		DTW_findPath (me, begin, end, slope);
	}
END

FORM (DTW_findPath_bandAndSlope, L"DTW: find path (band & slope)", 0)
    REAL (L"Sakoe-Chiba band (s)", L"0.05")
    RADIO (L"Slope constraint", 1)
    RADIOBUTTON (L"no restriction")
    RADIOBUTTON (L"1/3 < slope < 3")
    RADIOBUTTON (L"1/2 < slope < 2")
    RADIOBUTTON (L"2/3 < slope < 3/2")
    OK
DO
    double band = GET_REAL (L"Sakoe-Chiba band");
    int slope = GET_INTEGER (L"Slope constraint");
    LOOP {
        iam (DTW);
        DTW_findPath_bandAndSlope (me, band, slope, 0);
    }
END

FORM (DTW_to_Matrix_cummulativeDistances, L"DTW: To Matrix", 0)
    REAL (L"Sakoe-Chiba band (s)", L"0.05")
    RADIO (L"Slope constraint", 1)
    RADIOBUTTON (L"no restriction")
    RADIOBUTTON (L"1/3 < slope < 3")
    RADIOBUTTON (L"1/2 < slope < 2")
    RADIOBUTTON (L"2/3 < slope < 3/2")
    OK
DO
    double band = GET_REAL (L"Sakoe-Chiba band");
    int slope = GET_INTEGER (L"Slope constraint");
    LOOP {
        iam (DTW);
        autoMatrix thee = DTW_to_Matrix_cummulativeDistances (me, band, slope);
        praat_new (thee.transfer(), my name, L"_cd");
    }
END

FORM (DTW_to_Polygon, L"DTW: To Polygon...", 0)
    REAL (L"Sakoe-Chiba band (s)", L"0.1")
    RADIO (L"Slope constraint", 1)
    RADIOBUTTON (L"no restriction")
    RADIOBUTTON (L"1/3 < slope < 3")
    RADIOBUTTON (L"1/2 < slope < 2")
    RADIOBUTTON (L"2/3 < slope < 3/2")
    OK
DO
    double band = GET_REAL (L"Sakoe-Chiba band");
    int slope = GET_INTEGER (L"Slope constraint");
    LOOP {
        iam (DTW);
        autoPolygon thee = DTW_to_Polygon (me, band, slope);
        praat_new (thee.transfer(), my name);
    }
END

DIRECT (DTW_to_Matrix_distances)
	LOOP {
		iam (DTW);
		praat_new (DTW_to_Matrix_distances (me), my name);
	}
END

DIRECT (DTW_swapAxes)
	LOOP {
		iam (DTW);
		praat_new (DTW_swapAxes (me), my name, L"_axesSwapped");
	}
END

DIRECT (DTW_and_Matrix_replace)
	DTW me = FIRST (DTW);
	Matrix m = FIRST (Matrix);
	DTW_and_Matrix_replace (me, m);
	praat_dataChanged (me);
END

DIRECT (DTW_and_TextGrid_to_TextGrid)
	DTW me = FIRST (DTW);
	TextGrid tg = FIRST (TextGrid);
	praat_new (DTW_and_TextGrid_to_TextGrid (me, tg, 0), 0);
END

DIRECT (DTW_and_IntervalTier_to_Table)
	DTW me = FIRST (DTW);
	IntervalTier ti = FIRST (IntervalTier);
	praat_new (DTW_and_IntervalTier_to_Table (me, ti, 1.0/44100), my name);
END

/******************** EditDistanceTable & EditCostsTable ********************************************/

DIRECT (EditDistanceTable_help)
	Melder_help (L"EditDistanceTable");
END

DIRECT (EditDistanceTable_to_TableOfReal_directions)
	LOOP {
		iam (EditDistanceTable);
		praat_new (EditDistanceTable_to_TableOfReal_directions (me), my name);
	}
END

DIRECT (EditDistanceTable_setEditCosts)
	EditDistanceTable me = FIRST (EditDistanceTable);
	EditCostsTable thee = FIRST(EditCostsTable);
	EditDistanceTable_setEditCosts (me, thee);
END

FORM (EditDistanceTable_setDefaultCosts, L"", 0)
	REAL (L"Insertion costs", L"1.0")
	REAL (L"Deletion costs", L"1.0")
	REAL (L"Substitution costs", L"2.0")
	OK
DO
	double insertionCosts = GET_REAL (L"Insertion costs");
	if (insertionCosts < 0) {
		Melder_throw ("Insertion costs cannot be negative.");
	}
	double deletionCosts = GET_REAL (L"Deletion costs");
	if (deletionCosts < 0) {
		Melder_throw ("Deletion costs cannot be negative.");
	}
	double substitutionCosts = GET_REAL (L"Substitution costs");
	if (substitutionCosts < 0) {
		Melder_throw ("Substitution costs cannot be negative.");
	}
	LOOP {
		iam (EditDistanceTable);
		EditDistanceTable_setDefaultCosts (me, insertionCosts, deletionCosts, substitutionCosts);
	}
END

FORM (EditDistanceTable_draw, L"EditDistanceTable_draw", 0)
	RADIO (L"Format", 3)
		RADIOBUTTON (L"decimal")
		RADIOBUTTON (L"exponential")
		RADIOBUTTON (L"free")
		RADIOBUTTON (L"rational")
	NATURAL (L"Precision", L"1")
	REAL (L"Rotate source labels by (degrees)", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (EditDistanceTable);
		EditDistanceTable_draw (me, GRAPHICS, GET_INTEGER (L"Format"), GET_INTEGER (L"Precision"), GET_REAL (L"Rotate source labels by"));
	}
END

DIRECT (EditDistanceTable_drawEditOperations)
	autoPraatPicture picture;
	LOOP {
		iam(EditDistanceTable);
		EditDistanceTable_drawEditOperations (me, GRAPHICS);
	}
END

DIRECT (EditCostsTable_help)
	Melder_help (L"EditCostsTable");
END

FORM (EditCostsTable_getTargetIndex, L"EditCostsTable: Get target index", 0)
	SENTENCE (L"Target", L"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getTargetIndex (me, GET_STRING (L"Target")), NULL);
	}
END

FORM (EditCostsTable_getSourceIndex, L"EditCostsTable: Get source index", 0)
	SENTENCE (L"Source", L"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getSourceIndex (me, GET_STRING (L"Source")), NULL);
	}
END

FORM (EditCostsTable_getInsertionCost, L"EditCostsTable: Get insertion cost", 0)
	SENTENCE (L"Target", L"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getInsertionCost (me, GET_STRING (L"Target")), NULL);
	}
END

FORM (EditCostsTable_getDeletionCost, L"EditCostsTable: Get deletion cost", 0)
	SENTENCE (L"Source", L"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getDeletionCost (me, GET_STRING (L"Source")), NULL);
	}
END

FORM (EditCostsTable_getSubstitutionCost, L"EditCostsTable: Get substitution cost", 0)
	SENTENCE (L"Target", L"")
	SENTENCE (L"Source", L"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getSubstitutionCost (me, GET_STRING (L"Target"), GET_STRING (L"Source")), NULL);
	}
END

FORM (EditCostsTable_getOthersCost, L"EditCostsTable: Get cost (others)", 0)
	RADIO (L"Others cost type", 1)
	RADIOBUTTON (L"Insertion")
	RADIOBUTTON (L"Deletion")
	RADIOBUTTON (L"Equality")
	RADIOBUTTON (L"Inequality")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getOthersCost (me, GET_INTEGER (L"Others cost type")), NULL);
	}
END

FORM (EditCostsTable_setTargetSymbol_index, L"EditCostsTable: Set target symbol (index)", 0)
	NATURAL (L"Index", L"1")
	SENTENCE (L"Target", L"a")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setRowLabel (me, GET_INTEGER (L"Index"), GET_STRING (L"Target"));
	}
END

FORM (EditCostsTable_setSourceSymbol_index, L"EditCostsTable: Set source symbol (index)", 0)
	NATURAL (L"Index", L"1")
	SENTENCE (L"Source", L"a")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setColumnLabel (me, GET_INTEGER (L"Index"), GET_STRING (L"Source"));
	}
END

FORM (EditCostsTable_setInsertionCosts, L"EditCostsTable: Set insertion costs", 0)
	SENTENCE (L"Targets", L"")
	REAL (L"Cost", L"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setInsertionCosts (me, GET_STRING (L"Targets"), GET_REAL (L"Cost"));
	}
END

FORM (EditCostsTable_setDeletionCosts, L"EditCostsTable: Set deletion costs", 0)
	SENTENCE (L"Sources", L"")
	REAL (L"Cost", L"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setDeletionCosts (me, GET_STRING (L"Sources"), GET_REAL (L"Cost"));
	}
END

FORM (EditCostsTable_setSubstitutionCosts, L"EditCostsTable: Set substitution costs", 0)
	SENTENCE (L"Targets", L"a i u")
	SENTENCE (L"Sources", L"a i u")
	REAL (L"Cost", L"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setSubstitutionCosts (me, GET_STRING (L"Targets"), GET_STRING (L"Sources"), GET_REAL (L"Cost"));
	}
END

FORM (EditCostsTable_setOthersCosts, L"EditCostsTable: Set costs (others)", 0)
	LABEL (L"", L"Others costs")
	REAL (L"Insertion", L"1.0")
	REAL (L"Deletion", L"1.0")
	LABEL (L"", L"Substitution costs")
	REAL (L"Equality", L"0.0")
	REAL (L"Inequality", L"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setOthersCosts (me, GET_REAL (L"Insertion"), GET_REAL (L"Deletion"), GET_REAL (L"Equality"), GET_REAL (L"Inequality"));
	}
END

DIRECT (EditCostsTable_to_TableOfReal)
	LOOP {
		iam (EditCostsTable);
		praat_new (EditCostsTable_to_TableOfReal (me), my name);
	}
END

FORM (EditCostsTable_createEmpty, L"Create empty EditCostsTable", L"Create empty EditCostsTable...")
	SENTENCE (L"Name", L"editCosts")
	INTEGER (L"Number of target symbols", L"0")
	INTEGER (L"Number of source symbols", L"0")
	OK
DO
	long numberOfTargetSymbols = GET_INTEGER (L"Number of target symbols");
	numberOfTargetSymbols = numberOfTargetSymbols < 0 ? 0 : numberOfTargetSymbols;
	long numberOfSourceSymbols = GET_INTEGER (L"Number of source symbols");
	numberOfSourceSymbols = numberOfSourceSymbols < 0 ? 0 : numberOfSourceSymbols;
	praat_new (EditCostsTable_create (numberOfTargetSymbols, numberOfSourceSymbols), GET_STRING (L"Name"));
END

/******************** Eigen ********************************************/

DIRECT (Eigen_drawEigenvalues_scree)
	Melder_warning (L"The command \"Draw eigenvalues (scree)...\" has been "
		"removed.\n To get a scree plot use \"Draw eigenvalues...\" with the "
		"arguments\n 'Fraction of eigenvalues summed' and 'Cumulative' unchecked.");
END

FORM (Eigen_drawEigenvalues, L"Eigen: Draw eigenvalues", L"Eigen: Draw eigenvalues...")
	INTEGER (L"left Eigenvalue range", L"0")
	INTEGER (L"right Eigenvalue range", L"0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	BOOLEAN (L"Fraction of eigenvalues summed", 0)
	BOOLEAN (L"Cumulative", 0)
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Eigen);
		Eigen_drawEigenvalues (me, GRAPHICS, GET_INTEGER (L"left Eigenvalue range"),
		GET_INTEGER (L"right Eigenvalue range"), GET_REAL (L"left Amplitude range"),
		GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Fraction of eigenvalues summed"),
		GET_INTEGER (L"Cumulative"), GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
		GET_INTEGER (L"Garnish"));
	}
END

FORM (Eigen_drawEigenvector, L"Eigen: Draw eigenvector", L"Eigen: Draw eigenvector...")
	INTEGER (L"Eigenvector number", L"1")
	BOOLEAN (L"Component loadings", 0)
	INTEGER (L"left Element range", L"0")
	INTEGER (L"right Element range", L"0")
	REAL (L"left Amplitude range", L"-1.0")
	REAL (L"right Amplitude range", L"1.0")
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Connect points", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Eigen);
		Eigen_drawEigenvector (me, GRAPHICS, GET_INTEGER (L"Eigenvector number"),
		GET_INTEGER (L"left Element range"), GET_INTEGER (L"right Element range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
		GET_INTEGER (L"Component loadings"), GET_REAL (L"Mark size"),
		GET_STRING (L"Mark string"), GET_INTEGER (L"Connect points"), 0, GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Eigen_getNumberOfEigenvalues)
	LOOP {
		iam (Eigen);
		Melder_information (Melder_integer (my numberOfEigenvalues));
	}
END

DIRECT (Eigen_getDimension)
	LOOP {
		iam (Eigen);
		Melder_information (Melder_integer (my dimension));
	}
END

FORM (Eigen_getEigenvalue, L"Eigen: Get eigenvalue", L"Eigen: Get eigenvalue...")
	NATURAL (L"Eigenvalue number", L"1")
	OK
DO
	LOOP {
		iam (Eigen);
		long number = GET_INTEGER (L"Eigenvalue number");
		if (number > my numberOfEigenvalues) {
			Melder_throw ("Eigenvalue number must be smaller than ", my numberOfEigenvalues + 1);
		}
		Melder_information (Melder_double (my eigenvalues[number]));
	}
END

FORM (Eigen_getSumOfEigenvalues, L"Eigen:Get sum of eigenvalues", L"Eigen: Get sum of eigenvalues...")
	INTEGER (L"left Eigenvalue range",  L"0")
	INTEGER (L"right Eigenvalue range", L"0")
	OK
DO
	LOOP {
		iam (Eigen);
		Melder_information (Melder_double (Eigen_getSumOfEigenvalues (me, GET_INTEGER (L"left Eigenvalue range"), GET_INTEGER (L"right Eigenvalue range"))));
	}
END

FORM (Eigen_getEigenvectorElement, L"Eigen: Get eigenvector element", L"Eigen: Get eigenvector element...")
	NATURAL (L"Eigenvector number", L"1")
	NATURAL (L"Element number", L"1")
	OK
DO
	LOOP {
		iam (Eigen);
		Melder_information (Melder_double (Eigen_getEigenvectorElement (me, GET_INTEGER (L"Eigenvector number"), GET_INTEGER (L"Element number"))));
	}
END

DIRECT (Eigens_alignEigenvectors)
	autoCollection set = praat_getSelectedObjects ();
	Eigens_alignEigenvectors (set.peek());
END

FORM (Eigen_and_Matrix_project, L"Eigen & Matrix: Project", L"Eigen & Matrix: Project...")
	INTEGER (L"Number of dimensions", L"0")
	OK
DO
	Eigen me = FIRST_GENERIC (Eigen);
	Matrix mat = FIRST_GENERIC (Matrix);
	praat_new (Eigen_and_Matrix_project (me, mat, GET_INTEGER (L"Number of dimensions")), my name, L"_", mat->name);
END

DIRECT (Eigen_and_SSCP_project)
	Eigen me = FIRST_GENERIC (Eigen);
	SSCP cp = FIRST (SSCP);
	praat_new (Eigen_and_SSCP_project (me, cp), my name, L"_", cp->name);
END

DIRECT (Eigen_and_Covariance_project)
	Eigen me = FIRST_GENERIC (Eigen);
	Covariance cv = FIRST (Covariance);
	praat_new (Eigen_and_Covariance_project (me, cv), my name, L"_", cv->name);
END

/******************** Index ********************************************/

DIRECT (Index_help)
	Melder_help (L"Index");
END

DIRECT (Index_getNumberOfClasses)
	LOOP {
		iam (Index);
		Melder_information (Melder_integer (my classes -> size));
	}
END

FORM (StringsIndex_getClassLabel, L"StringsIndex: Get class label", L"StringsIndex: Get class label...")
	NATURAL (L"Class index", L"1")
	OK
DO
	long klas = GET_INTEGER (L"Class index");
	LOOP {
		iam (StringsIndex);
		long numberOfClasses = my classes -> size;
		if (klas > numberOfClasses) {
			Melder_throw ("Index must be less than or equal ", numberOfClasses, L".");
		}
		SimpleString ss = (SimpleString) my classes -> item[klas];
		Melder_information (ss -> string);
	}
END

FORM (StringsIndex_getLabel, L"StringsIndex: Get label", L"StringsIndex: Get label...")
	NATURAL (L"Element index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Element index");
	LOOP {
		iam (StringsIndex);
		if (index > my numberOfElements) {
			Melder_throw ("Index must be less than or equal ", my numberOfElements, L".");
		}
		long klas = my classIndex[index];
		SimpleString ss = (SimpleString) my classes -> item [klas];
		Melder_information (ss -> string);
	}
END

FORM (Index_getIndex, L"Index: Get index", L"Index: Get index...")
	NATURAL (L"Element index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Element index");
	LOOP {
		iam (Index);
		if (index > my numberOfElements) {
			Melder_throw ("Index must be less than or equal ", my numberOfElements, L".");
		}
		Melder_information (Melder_integer (my classIndex[index]));
	}
END

FORM (StringsIndex_getClassIndex, L"StringsIndex: Get class index", L"StringsIndex: Get class index...")
	WORD (L"Class label", L"label")
	OK
DO
	wchar_t *klasLabel = GET_STRING (L"Class label");
	LOOP {
		iam (StringsIndex);
		long index = StringsIndex_getClass (me, klasLabel);
		Melder_information (Melder_integer (index));
	}
END

FORM (Index_extractPart, L"Index: Extract part", L"Index: Extract part...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	OK
DO
	LOOP {
		iam (Index);
		praat_new (Index_extractPart (me, GET_INTEGER (L"left Range"), GET_INTEGER (L"right Range")),
		Thing_getName (me), L"_part");
	}
END

FORM (Index_to_Permutation, L"Index: To Permutation", L"Index: To Permutation...")
	BOOLEAN (L"Permute within classes", 1)
	OK
DO
	LOOP {
		iam (Index);
		praat_new (Index_to_Permutation_permuteRandomly (me, GET_INTEGER (L"Permute within classes")), my name);
	}
END

DIRECT (StringsIndex_to_Strings)
	LOOP {
		iam (StringsIndex);
		praat_new (StringsIndex_to_Strings (me), my name);
	}
END

/******************** Excitation ********************************************/

DIRECT (Excitation_to_Excitations)
	autoExcitations e = Excitations_create (100);
	LOOP {
		iam (Excitation);
		autoExcitation thee = Data_copy (me);
		Collection_addItem (e.peek(), thee.transfer());
	}
	praat_new (e.transfer(), L"appended");
END

/******************** Excitations ********************************************/

FORM (Excitations_formula, L"Excitations: Formula", 0)
	LABEL (L"label", L"for all objects in Excitations do { for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	LOOP {
		iam (Excitations);
		for (long j = 1; j <= my size; j++) {
			Matrix_formula ( (Matrix) my item[j], GET_STRING (L"formula"), interpreter, 0);
		}
		praat_dataChanged (me);
	}
END

DIRECT (Excitations_addItem)
	Excitations e = FIRST (Excitations);
	WHERE_DOWN (SELECTED && CLASS == classExcitation) {
		iam (Excitation);
		autoExcitation thee = Data_copy (me);
		Collection_addItem (e, thee.transfer());
	}
END

FORM (Excitations_getItem, L"Excitations: Get item", 0)
	NATURAL (L"Item number", L"1")
	OK
DO
	LOOP {
		iam (Excitations);
		praat_new (Excitations_getItem (me, GET_INTEGER (L"Item number")), my name, L"_item");
	}
END

DIRECT (Excitations_append)
	Excitations e1 = 0, e2 = 0;
	LOOP {
		iam (Excitations);
		(e1 ? e2 : e1) = me;
	}
	Melder_assert (e1 && e2);
	praat_new ( (Excitations) Collections_merge (e1, e2), L"appended");
END

FORM (Excitations_to_Pattern, L"Excitations: To Pattern", 0)
	NATURAL (L"Join", L"1")
	OK
DO
	LOOP {
		iam (Excitations);
		praat_new (Excitations_to_Pattern (me, GET_INTEGER (L"Join")), my name);
	}
END

DIRECT (Excitations_to_TableOfReal)
	LOOP {
		iam (Excitations);
		praat_new (Excitations_to_TableOfReal (me), my name);
	}
END


/************************* FileInMemory ***********************************/


FORM_READ (FileInMemory_create, L"Create file in memory", 0, true)
	autoFileInMemory me = FileInMemory_create (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

FORM (FileInMemory_setId, L"FileInMemory: Set id", 0)
	SENTENCE (L"New id", L"New id")
	OK
DO
	LOOP {
		iam (FileInMemory);
		FileInMemory_setId (me, GET_STRING (L"New id"));
		praat_dataChanged (me);
	}
END

FORM (FileInMemory_showAsCode, L"FileInMemory: Show as code", 0)
	WORD (L"Name", L"example")
	INTEGER (L"Number of bytes per line", L"20")
	OK
DO
	const wchar_t *name = GET_STRING (L"Name");
	LOOP {
		iam (FileInMemory);
		MelderInfo_open ();
		FileInMemory_showAsCode (me, name, GET_INTEGER (L"Number of bytes per line"));
		MelderInfo_close ();
	}
END

/************************* FilesInMemory ***********************************/

FORM (FilesInMemory_createFromDirectoryContents, L"Create files in memory from directory contents", 0)
	SENTENCE (L"Name", L"list")
	LABEL (L"", L"Directory:")
	TEXTFIELD (L"Directory", L"/home/david/praat/src/espeak-work/espeak-1.46.13/espeak-data")
	WORD (L"Only files that match pattern", L"*.txt")
	OK
DO
	autoFilesInMemory me = FilesInMemory_createFromDirectoryContents (GET_STRING (L"Directory"), GET_STRING (L"Only files that match pattern"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (FilesInMemory_createCopyFromFilesInMemory, L"", 0)
	OPTIONMENU (L"Espeakdata", 5)
	OPTION (L"phons")
	OPTION (L"dicts")
	OPTION (L"voices")
	OPTION (L"variants")
	OPTION (L"voices_names")
	OPTION (L"variants_names")
	OK
DO
	long choice = GET_INTEGER (L"Espeakdata");
	if (choice == 1) {
		autoFilesInMemory f = (FilesInMemory) Data_copy (espeakdata_phons);
		praat_new (f.transfer(), L"espeakdata_phons");
	}
	else if (choice == 2) {
		autoFilesInMemory f = (FilesInMemory) Data_copy (espeakdata_dicts);
		praat_new (f.transfer(), L"espeakdata_dicts");
	}
	else if (choice == 3) {
		autoFilesInMemory f = (FilesInMemory) Data_copy (espeakdata_voices);
		praat_new (f.transfer(), L"espeakdata_voices");
	}
	else if (choice == 4) {
		autoFilesInMemory f = (FilesInMemory) Data_copy (espeakdata_variants);
		praat_new (f.transfer(), L"espeakdata_variants");
	}
	else if (choice == 5) {
		autoStrings s = (Strings) Data_copy (espeakdata_voices_names);
		praat_new (s.transfer(), L"espeakdata_voices_names");
	}
	else if (choice == 6) {
		autoStrings s = (Strings) Data_copy (espeakdata_variants_names);
		praat_new (s.transfer(), L"espeakdata_variants_names");
	}
END

FORM (FilesInMemory_showAsCode, L"FilesInMemory: Show as code", 0)
	WORD (L"Name", L"example")
	INTEGER (L"Number of bytes per line", L"20")
	OK
DO
	LOOP {
		iam (FilesInMemory);
		MelderInfo_open ();
		FilesInMemory_showAsCode (me, GET_STRING (L"Name"), GET_INTEGER (L"Number of bytes per line"));
		MelderInfo_close ();
	}
END

FORM (FilesInMemory_showOneFileAsCode, L"FilesInMemory: Show one file as code", 0)
	NATURAL (L"Index", L"1")
	WORD (L"Name", L"example")
	INTEGER (L"Number of bytes per line", L"20")
	OK
DO
	LOOP {
		iam (FilesInMemory);
		MelderInfo_open ();
		FilesInMemory_showOneFileAsCode (me, GET_INTEGER (L"Index"), GET_STRING (L"Name"), GET_INTEGER (L"Number of bytes per line"));
		MelderInfo_close ();
	}
END

DIRECT (FileInMemory_to_FilesInMemory)
	autoFilesInMemory thee = FilesInMemory_create ();
	LOOP {
		iam (FileInMemory);
		FileInMemory him = Data_copy (me);
		Collection_addItem (thee.peek(), him);
	}
	praat_new (thee.transfer(), L"files");
END

DIRECT (FilesInMemory_addItems)
	FilesInMemory thee = FIRST (FilesInMemory);
	LOOP {
		iam (Data);
		if (CLASS == classFileInMemory) {
			FileInMemory t1 = (FileInMemory) Data_copy (me);
			Collection_addItem (thee, t1);
		}
	}
END

DIRECT (FilesInMemory_merge)
	FilesInMemory f1 = 0, f2 = 0;
	LOOP { iam (FilesInMemory); (f1 ? f2 : f1) = me; }
	Melder_assert (f1 != 0 && f2 != 0);
	autoFilesInMemory fim = (FilesInMemory) Collections_merge (f1, f2);
	praat_new (fim.transfer(), f1 -> name, L"_", f2 -> name);
END

DIRECT (FilesInMemory_to_Strings_id)
	LOOP {
		iam (FilesInMemory);
		praat_new (FilesInMemory_to_Strings_id (me), my name);
	}
END

/************************* FilterBank ***********************************/

FORM (FilterBank_drawFilters, L"FilterBank: Draw filters", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawRows (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"));
	}
END

FORM (FilterBank_drawOneContour, L"FilterBank: Draw one contour", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"Height (dB)", L"40.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawOneContour (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"), GET_REAL (L"Height"));
	}
END

FORM (FilterBank_drawContours, L"FilterBank: Draw contours", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawContours (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"));
	}
END

FORM (FilterBank_drawFrequencyScales, L"FilterBank: Draw frequency scales", L"FilterBank: Draw frequency scales...")
	RADIO (L"Horizontal frequency scale", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	REAL (L"left Horizontal frequency range", L"0.0")
	REAL (L"right Horizontal frequency range", L"0.0")
	RADIO (L"Vertical frequency scale", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	REAL (L"left Vertical frequency range", L"0.0")
	REAL (L"right Vertical frequency range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FilterBank);
		FilterBank_drawFrequencyScales (me, GRAPHICS, GET_INTEGER (L"Horizontal frequency scale"),
		GET_REAL (L"left Horizontal frequency range"), GET_REAL (L"right Horizontal frequency range"),
		GET_INTEGER (L"Vertical frequency scale"), GET_REAL (L"left Vertical frequency range"),
		GET_REAL (L"right Vertical frequency range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FilterBank_paintImage, L"FilterBank: Paint image", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintImage (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"));
	}
END

FORM (FilterBank_paintContours, L"FilterBank: Paint contours", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintContours (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"));
	}
END


FORM (FilterBank_paintCells, L"FilterBank: Paint cells", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintCells (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"));
	}
END

FORM (FilterBank_paintSurface, L"FilterBank: Paint surface", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintSurface (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"), 30, 45);
	}
END

FORM (FilterBank_getFrequencyInHertz, L"FilterBank: Get frequency in Hertz", L"FilterBank: Get frequency in Hertz...")
	REAL (L"Frequency", L"10.0")
	RADIO (L"Unit", 2)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	OK
DO
	LOOP {
		iam (FilterBank);
		double f = FilterBank_getFrequencyInHertz (me, GET_REAL (L"Frequency"), GET_INTEGER (L"Unit"));
		Melder_informationReal (f, L"Hertz");
	}
END

FORM (FilterBank_getFrequencyInBark, L"FilterBank: Get frequency in Bark", L"FilterBank: Get frequency in Bark...")
	REAL (L"Frequency", L"93.17")
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	OK
DO
	LOOP {
		iam (FilterBank);
		double f = FilterBank_getFrequencyInBark (me, GET_REAL (L"Frequency"), GET_INTEGER (L"Unit"));
		Melder_informationReal (f, L"Bark");
	}
END

FORM (FilterBank_getFrequencyInMel, L"FilterBank: Get frequency in mel", L"FilterBank: Get frequency in mel...")
	REAL (L"Frequency", L"1000.0")
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	OK
DO
	LOOP {
		iam (FilterBank);
		double f = FilterBank_getFrequencyInMel (me, GET_REAL (L"Frequency"), GET_INTEGER (L"Unit"));
		Melder_informationReal (f, L"mel");
	}
END

FORM (FilterBank_equalizeIntensities, L"FilterBank: Equalize intensities", L"")
	REAL (L"Intensity (dB)", L"80.0")
	OK
DO
	LOOP {
		iam (FilterBank);
		FilterBank_equalizeIntensities (me, GET_REAL (L"Intensity"));
		praat_dataChanged (me);
	}
END

DIRECT (FilterBank_to_Matrix)
	LOOP {
		iam (FilterBank);
		praat_new (FilterBank_to_Matrix (me), my name);
	}
END

FORM (FilterBanks_crossCorrelate, L"FilterBanks: Cross-correlate", 0)
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	FilterBank f1 = 0, f2 = 0;
	LOOP { iam (FilterBank); (f1 ? f2 : f1) = me; }
	Melder_assert (f1 != 0 && f2 != 0);
	praat_new (FilterBanks_crossCorrelate (f1, f2, GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is...")),
		f1 -> name, L"_", f2 -> name);
END

FORM (FilterBanks_convolve, L"FilterBanks: Convolve", 0)
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	FilterBank f1 = 0, f2 = 0;
	LOOP { iam (FilterBank); (f1 ? f2 : f1) = me; }
	Melder_assert (f1 != 0 && f2 != 0);
	praat_new (FilterBanks_convolve (f1, f2, GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is...")),
		f1 -> name, L"_", f2 -> name);
END

DIRECT (FilterBank_to_Intensity)
	LOOP {
		iam (FilterBank);
		praat_new (FilterBank_to_Intensity (me), my name);
	}
END

/*********** FormantFilter *******************************************/

DIRECT (FormantFilter_help)
	Melder_help (L"FormantFilter");
END

FORM (FormantFilter_drawFilterFunctions, L"FormantFilter: Draw filter functions", L"FilterBank: Draw filter functions...")
	INTEGER (L"left Filter range", L"0")
	INTEGER (L"right Filter range", L"0")
	POSITIVE (L"Bandwidth (Hz)", L"100.0")
	RADIO (L"Frequency scale", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	BOOLEAN (L"Amplitude scale in dB", 1)
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantFilter);
		FormantFilter_drawFilterFunctions (me, GRAPHICS, GET_REAL (L"Bandwidth"), GET_INTEGER (L"Frequency scale"),
			GET_INTEGER (L"left Filter range"), GET_INTEGER (L"right Filter range"),
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_INTEGER (L"Amplitude scale in dB"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FormantFilter_drawSpectrum, L"FormantFilter: Draw spectrum (slice)", L"FilterBank: Draw spectrum (slice)...")
	POSITIVE (L"Time (s)", L"0.1")
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0")
	REAL (L"left Amplitude range (dB)", L"0.0")
	REAL (L"right Amplitude range (dB)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FilterBank);
		FilterBank_drawTimeSlice (me, GRAPHICS, GET_REAL (L"Time"), GET_REAL (L"left Frequency range"),
			GET_REAL (L"right Frequency range"), GET_REAL (L"left Amplitude range"),
			GET_REAL (L"right Amplitude range"), L"Hz", GET_INTEGER (L"Garnish"));
	}
END

/****************** FormantGrid  *********************************/

FORM (old_FormantGrid_draw, L"FormantGrid: Draw", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (=all)")
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (=auto)")
	BOOLEAN (L"Bandwidths", false)
	BOOLEAN (L"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantGrid);
		FormantGrid_draw (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_INTEGER (L"Bandwidths"), GET_INTEGER (L"Garnish"), L"lines and speckles");
	}
END

FORM (FormantGrid_draw, L"FormantGrid: Draw", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (=all)")
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (=auto)")
	BOOLEAN (L"Bandwidths", false)
	BOOLEAN (L"Garnish", true)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 1)
	OPTION (L"lines")
	OPTION (L"speckles")
	OPTION (L"lines and speckles")
	OK
DO_ALTERNATIVE (old_FormantGrid_draw)
	autoPraatPicture picture;
	LOOP {
		iam (FormantGrid);
		FormantGrid_draw (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_INTEGER (L"Bandwidths"), GET_INTEGER (L"Garnish"), GET_STRING (L"Drawing method"));
	}
END

/****************** FunctionTerms  *********************************/

FORM (FunctionTerms_draw, L"FunctionTerms: Draw", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Extrapolate", 0)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FunctionTerms);
		FunctionTerms_draw (me, GRAPHICS, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Extrapolate"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FunctionTerms_drawBasisFunction, L"FunctionTerms: Draw basis function", 0)
	NATURAL (L"Index", L"1")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Extrapolate", 0)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FunctionTerms);
		FunctionTerms_drawBasisFunction (me, GRAPHICS, GET_INTEGER (L"Index"), GET_REAL (L"Xmin"),
			GET_REAL (L"Xmax"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Extrapolate"), GET_INTEGER (L"Garnish"));
	}
END

FORM (FunctionTerms_evaluate, L"FunctionTerms: Evaluate", 0)
	REAL (L"X", L"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		Melder_information (Melder_double (FunctionTerms_evaluate (me, GET_REAL (L"X"))));
	}
END

DIRECT (FunctionTerms_getNumberOfCoefficients)
	LOOP {
		iam (FunctionTerms);
		Melder_information (Melder_integer (my numberOfCoefficients));
	}
END

FORM (FunctionTerms_getCoefficient, L"FunctionTerms: Get coefficient", 0)
	LABEL (L"", L"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (FunctionTerms);
		if (index > my numberOfCoefficients) {
			Melder_throw ("Index too large.");
		}
		Melder_information (Melder_double (my coefficients[index]));
	}
END

DIRECT (FunctionTerms_getDegree)
	LOOP {
		iam (FunctionTerms);
		Melder_information (Melder_integer (FunctionTerms_getDegree (me)));
	}
END

FORM (FunctionTerms_getMaximum, L"FunctionTerms: Get maximum", L"Polynomial: Get maximum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getMaximum (me, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"));
		Melder_information (Melder_double (x));
	}
END

FORM (FunctionTerms_getMinimum, L"FunctionTerms: Get minimum", L"Polynomial: Get minimum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getMinimum (me, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"));
		Melder_information (Melder_double (x));
	}
END

FORM (FunctionTerms_getXOfMaximum, L"FunctionTerms: Get x of maximum", L"Polynomial: Get x of maximum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getXOfMaximum (me, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"));
		Melder_information (Melder_double (x));
	}
END

FORM (FunctionTerms_getXOfMinimum, L"FunctionTerms: Get x of minimum", L"Polynomial: Get x of minimum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getXOfMinimum (me, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"));
		Melder_information (Melder_double (x));
	}
END

FORM (FunctionTerms_setCoefficient, L"FunctionTerms: Set coefficient", 0)
	LABEL (L"", L"p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	LABEL (L"", L"F[k] is of degree k")
	NATURAL (L"Index", L"1")
	REAL (L"Value", L"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		FunctionTerms_setCoefficient (me, GET_INTEGER (L"Index"), GET_REAL (L"Value"));
	}
END

FORM (FunctionTerms_setDomain, L"FunctionTerms: Set domain", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"2.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	LOOP {
		iam (FunctionTerms);
		if (xmax <= xmin) {
			Melder_throw ("Xmax must be larger than Xmin.");
		}
		FunctionTerms_setDomain (me, xmin, xmax);
	}
END

/***************** Intensity ***************************************************/

FORM (Intensity_to_TextGrid_detectSilences, L"Intensity: To TextGrid (silences)", L"Intensity: To TextGrid (silences)...")
	REAL (L"Silence threshold (dB)", L"-25.0")
	POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
	POSITIVE (L"Minimum sounding interval duration (s)", L"0.05")
	WORD (L"Silent interval label", L"silent")
	WORD (L"Sounding interval label", L"sounding")
	OK
DO
	LOOP {
		iam (Intensity);
		praat_new (Intensity_to_TextGrid_detectSilences (me, GET_REAL (L"Silence threshold"),
			GET_REAL (L"Minimum silent interval duration"), GET_REAL (L"Minimum sounding interval duration"),
			GET_STRING (L"Silent interval label"), GET_STRING (L"Sounding interval label")), my name);
	}
END

/***************** IntensityTier ***************************************************/

FORM (IntensityTier_to_TextGrid_detectSilences, L"IntensityTier: To TextGrid (silences)", L"Intensity: To TextGrid (silences)...")
	REAL (L"Silence threshold (dB)", L"-25.0")
	POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
	POSITIVE (L"Minimum sounding interval duration (s)", L"0.05")
	WORD (L"Silent interval label", L"silent")
	WORD (L"Sounding interval label", L"sounding")
	POSITIVE (L"Time step (s)", L"0.001")
	OK
DO
	LOOP {
		iam (IntensityTier);
		praat_new (IntensityTier_to_TextGrid_detectSilences (me, GET_REAL (L"Time step"), GET_REAL (L"Silence threshold"),
			GET_REAL (L"Minimum silent interval duration"), GET_REAL (L"Minimum sounding interval duration"),
			GET_STRING (L"Silent interval label"), GET_STRING (L"Sounding interval label")), my name);
	}
END

FORM (IntensityTier_to_Intensity, L"", 0)
	POSITIVE (L"Time step (s)", L"0.001")
	OK
DO
	LOOP {
		iam (IntensityTier);
		autoIntensity thee = IntensityTier_to_Intensity (me, GET_REAL (L"Time step"));
		praat_new (thee.transfer(), my name);
	}
END

/***************** ISpline ***************************************************/

DIRECT (ISpline_help) Melder_help (L"ISpline"); END

FORM (ISpline_create, L"Create ISpline", L"Create ISpline...")
	WORD (L"Name", L"ispline")
	LABEL (L"", L"Domain")
	REAL (L"Xmin", L"0")
	REAL (L"Xmax", L"1")
	LABEL (L"", L"ISpline(x) = c[1] I[1](x) + c[2] I[1](x) + ... c[n] I[n](x)")
	LABEL (L"", L"all I[k] are polynomials of degree \"Degree\"")
	LABEL (L"", L"Relation: numberOfCoefficients == numberOfInteriorKnots + degree")
	INTEGER (L"Degree", L"3")
	SENTENCE (L"Coefficients (c[k])", L"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE (L"Interior knots" , L"0.3 0.5 0.6")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	long degree = GET_INTEGER (L"Degree");
	if (xmax <= xmin) {
		Melder_throw ("Xmin must be smaller than Xmax.");
	}
	praat_new (ISpline_createFromStrings (xmin, xmax, degree,
		GET_STRING (L"Coefficients"), GET_STRING (L"Interior knots")), GET_STRING (L"Name"));
END

/******************* KlattTable  *********************************/

DIRECT (KlattTable_help) Melder_help (L"KlattTable"); END

DIRECT (KlattTable_createExample)
	praat_new (KlattTable_createExample (), L"example");
END

FORM (KlattTable_to_Sound, L"KlattTable: To Sound", L"KlattTable: To Sound...")
	POSITIVE (L"Sampling frequency", L"16000")
	RADIO (L"Synthesis model", 1)
	RADIOBUTTON (L"Cascade")
	RADIOBUTTON (L"Parallel")
	NATURAL (L"Number of formants", L"5")
	POSITIVE (L"Frame duration (s)", L"0.005")
	REAL (L"Flutter percentage (%)", L"0.0")
	OPTIONMENU (L"Voicing source", 1)
	OPTION (L"Impulsive")
	OPTION (L"Natural")
	OPTIONMENU (L"Output type", 1)
	OPTION (L"Sound")
	OPTION (L"Voicing")
	OPTION (L"Aspiration")
	OPTION (L"Frication")
	OPTION (L"Cascade-glottal-output")
	OPTION (L"Parallel-glottal-output")
	OPTION (L"Bypass-output")
	OPTION (L"All-excitations")
	OK
DO
	double flutter = GET_REAL (L"Flutter percentage");
	int outputType = GET_INTEGER (L"Output type") - 1;
	if (flutter < 0 || flutter > 100) {
		Melder_throw ("Flutter must be between 0 and 100%.");
	}
	LOOP {
		iam (KlattTable);
		praat_new (KlattTable_to_Sound (me, GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Synthesis model"),
			GET_INTEGER (L"Number of formants"), GET_REAL (L"Frame duration"), GET_INTEGER (L"Voicing source"),
			GET_REAL (L"Flutter percentage"), outputType), my name);
	}
END

FORM (KlattTable_to_KlattGrid, L"KlattTable: To KlattGrid", 0)
	POSITIVE (L"Frame duration (s)", L"0.002")
	OK
DO
	LOOP {
		iam (KlattTable);
		praat_new (KlattTable_to_KlattGrid (me, GET_REAL (L"Frame duration")), my name);
	}
END

DIRECT (KlattTable_to_Table)
	LOOP {
		iam (KlattTable);
		praat_new (KlattTable_to_Table (me), my name);
	}
END

DIRECT (Table_to_KlattTable)
	LOOP {
		iam (Table);
		praat_new (Table_to_KlattTable (me), my name);
	}
END

FORM (Table_getMedianAbsoluteDeviation, L"Table: Get median absolute deviation", L"Table: Get median absolute deviation...")
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		double mad = Table_getMedianAbsoluteDeviation (me, icol);
		Melder_information (Melder_double (mad));
	}
END

static void print_means (Table me);
static void print_means (Table me) {
	wchar_t s[200];
	Table_numericize_Assert (me, 2);
	Table_numericize_Assert (me, 3);
	if (my numberOfColumns < 3) {
		MelderInfo_writeLine (L"Table has not the right format.");
		return;
	}
	swprintf (s, 199, L"%15ls\t%15ls\t%15ls", my columnHeaders[1].label, my columnHeaders[2].label, my columnHeaders[3].label);
	MelderInfo_writeLine (s);
	for (long irow = 1; irow <= my rows -> size; irow++) {
		TableRow row = (TableRow) my rows -> item [irow];
		swprintf (s, 199, L"%15ls\t%15g\t%15g", row -> cells[1].string, row -> cells[2].number, row -> cells[3].number);
		MelderInfo_writeLine (s);
	}
}

FORM (Table_reportOneWayAnova, L"Table: Report one-way anova",  L"Table: Report one-way anova...")
	SENTENCE (L"Column with data", L"F0")
	SENTENCE (L"Factor", L"Vowel")
	BOOLEAN (L"Table with means", 0);
	BOOLEAN (L"Table with differences between means", 0)
	BOOLEAN (L"Table with Tukey's post-hoc HSD test", 0)
	OK
DO
	wchar_t *factor = GET_STRING (L"Factor");
	wchar_t *dataLabel = GET_STRING (L"Column with data");
	bool getMeans = GET_INTEGER (L"Table with means");
	bool getMeansDiff = GET_INTEGER (L"Table with differences between means");
	bool getMeansDiffProbabilities = GET_INTEGER (L"Table with Tukey's post-hoc HSD test");
	LOOP {
		iam (Table);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factor);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataLabel);
		Table tmeans = 0, tmeansDiff = 0, tmeansDiffProbabilities = 0;
		autoTable anova = Table_getOneWayAnalysisOfVarianceF (me, dataColumn, factorColumn, &tmeans,
			&tmeansDiff, & tmeansDiffProbabilities);
		autoTable means = tmeans, meansDiff = tmeansDiff, meansDiffProbabilities = tmeansDiffProbabilities;
		MelderInfo_open ();
		MelderInfo_writeLine (L"One-way analysis of \"", dataLabel, L"\" by \"", factor, L"\".\n");
		Table_printAsAnovaTable (anova.peek());
		MelderInfo_writeLine (L"\nMeans:\n");
		print_means (means.peek());
		MelderInfo_close ();
		if (getMeans) {
			praat_new (means.transfer(), my name, L"_groupMeans");
		}
		if (getMeansDiff) {
			praat_new (meansDiff.transfer(), my name, L"_meansDiff");
		}
		if (getMeansDiffProbabilities) {
			praat_new (meansDiffProbabilities.transfer(), my name, L"_meansDiffP");
		}
	}
END

FORM (Table_reportTwoWayAnova, L"Table: Report two-way anova", L"Table: Report two-way anova...")
	SENTENCE (L"Column with data", L"Data")
	SENTENCE (L"First factor", L"A")
	SENTENCE (L"Second factor", L"B")
	BOOLEAN (L"Table with means", 0);
	OK
DO
	wchar_t *factorA = GET_STRING (L"First factor");
	wchar_t *factorB = GET_STRING (L"Second factor");
	wchar_t *dataLabel = GET_STRING (L"Column with data");
	bool getMeans = GET_INTEGER (L"Table with means");
	LOOP {
		iam (Table);
		long factorColumnA = Table_getColumnIndexFromColumnLabel (me, factorA);
		long factorColumnB = Table_getColumnIndexFromColumnLabel (me, factorB);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataLabel);
		Table tmeans = 0, tsizes = 0;
		autoTable anova = Table_getTwoWayAnalysisOfVarianceF (me, dataColumn, factorColumnA, factorColumnB, &tmeans, &tsizes);
		autoTable means = tmeans, sizes = tsizes;
		MelderInfo_open ();
		MelderInfo_writeLine (L"Two-way analysis of \"", dataLabel, L"\" by \"", factorA, L"\" and \"", factorB, L".\n");
		Table_printAsAnovaTable (anova.peek());
		MelderInfo_writeLine (L"\nMeans:\n");
		Table_printAsMeansTable (means.peek());
		MelderInfo_writeLine (L"\nCell sizes:\n");
		Table_printAsMeansTable (sizes.peek());
		MelderInfo_close ();
		if (getMeans) {
			praat_new (means.transfer(), my name, L"_groupMeans");
		}
	}
END

FORM (Table_reportOneWayKruskalWallis, L"Table: Report one-way Kruskal-Wallis", L"Table: Report one-way Kruskal-Wallis...")
	SENTENCE (L"Column with data", L"Data")
	SENTENCE (L"Factor", L"Group")
	OK
DO
	wchar_t *factor = GET_STRING (L"Factor");
	wchar_t *dataLabel = GET_STRING (L"Column with data");
	LOOP {
		iam (Table);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factor);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataLabel);
		double degreesOfFreedom, kruskalWallis, probability;
		autoTable thee = Table_getOneWayKruskalWallis (me, dataColumn, factorColumn, &degreesOfFreedom, &kruskalWallis, &probability);
		MelderInfo_open ();
		MelderInfo_writeLine (L"One-way Kruskal-Wallis of \"", dataLabel, L"\" by \"", factor, L"\".\n");
		MelderInfo_writeLine (L"Chi squared: ", Melder_double (kruskalWallis));
		MelderInfo_writeLine (L"Degrees of freedom: ", Melder_double (degreesOfFreedom));
		MelderInfo_writeLine (L"Probability: ", Melder_double (probability));
		MelderInfo_writeLine (L"\nMeans:\n");
		print_means (thee.peek());
		MelderInfo_close ();
		//praat_new (thee.transfer(), my name, L"_groupMeans");
	}
END

FORM (Table_to_StringsIndex_column, L"Table: To StringsIndex (column)", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	wchar *columnLabel = GET_STRING (L"Column label");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoStringsIndex thee = Table_to_StringsIndex_column (me, icol);
		praat_new (thee.transfer(), my name, L"_", columnLabel);
	}
END

/******************* LegendreSeries *********************************/

FORM (LegendreSeries_create, L"Create LegendreSeries", L"Create LegendreSeries...")
	WORD (L"Name", L"ls")
	LABEL (L"", L"Domain")
	REAL (L"Xmin", L"-1")
	REAL (L"Xmax", L"1")
	LABEL (L"", L"LegendreSeries(x) = c[1] P[0](x) + c[2] P[1](x) + ... c[n+1] P[n](x)")
	LABEL (L"", L"P[k] is a Legendre polynomial of degree k")
	SENTENCE (L"Coefficients", L"0 0 1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	if (xmin >= xmax) {
		Melder_throw ("Xmin must be smaller than Xmax.");
	}
	praat_new (LegendreSeries_createFromString (xmin, xmax, GET_STRING (L"Coefficients")), GET_STRING (L"Name"));
END

DIRECT (LegendreSeries_help) Melder_help (L"LegendreSeries"); END

DIRECT (LegendreSeries_to_Polynomial)
	LOOP {
		iam (LegendreSeries);
		praat_new (LegendreSeries_to_Polynomial (me), my name);
	}
END
/********************* LongSound **************************************/

FORM_READ (LongSounds_appendToExistingSoundFile, L"LongSound: Append to existing sound file", 0, false)
	autoCollection set = praat_getSelectedObjects ();
	LongSounds_appendToExistingSoundFile (set.peek(), file);
END

FORM_WRITE (LongSounds_writeToStereoAiffFile, L"LongSound: Save as AIFF file", 0, L"aiff")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_AIFF, file);
END

FORM_WRITE (LongSounds_writeToStereoAifcFile, L"LongSound: Save as AIFC file", 0, L"aifc")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_AIFC, file);
END

FORM_WRITE (LongSounds_writeToStereoWavFile, L"LongSound: Save as WAV file", 0, L"wav")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_WAV, file);
END

FORM_WRITE (LongSounds_writeToStereoNextSunFile, L"LongSound: Save as NeXT/Sun file", 0, L"au")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_NEXT_SUN, file);
END

FORM_WRITE (LongSounds_writeToStereoNistFile, L"LongSound: Save as NIST file", 0, L"nist")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_NIST, file);
END

/******************* Matrix **************************************************/

FORM (Matrix_drawAsSquares, L"Matrix: Draw as squares", L"Matrix: Draw as squares...")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawAsSquares (me, GRAPHICS, GET_REAL (L"left Horizontal range"),
			GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
			GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Matrix_drawDistribution, L"Matrix: Draw distribution", L"Matrix: Draw distribution...")
	LABEL (L"", L"Selection of (part of) Matrix")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	LABEL (L"", L"Selection of Matrix values")
	REAL (L"Minimum value", L"0.0")
	REAL (L"Maximum value", L"0.0")
	LABEL (L"", L"Display of the distribution")
	NATURAL (L"Number of bins", L"10")
	REAL (L"Minimum frequency", L"0.0")
	REAL (L"Maximum frequency", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawDistribution (me, GRAPHICS, GET_REAL (L"left Horizontal range"),
			GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
			GET_REAL (L"right Vertical range"), GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"),
			GET_INTEGER (L"Number of bins"), GET_REAL (L"Minimum frequency"), GET_REAL (L"Maximum frequency"), 0,
			GET_INTEGER (L"Garnish"));
	}
END

FORM (Matrix_drawCumulativeDistribution, L"Matrix: Draw cumulative distribution", L"")
	LABEL (L"", L"Selection of (part of) Matrix")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	LABEL (L"", L"Selection of Matrix values")
	REAL (L"Minimum value", L"0.0")
	REAL (L"Maximum value", L"0.0")
	LABEL (L"", L"Display of the distribution")
	NATURAL (L"Number of bins", L"10")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawDistribution (me, GRAPHICS, GET_REAL (L"left Horizontal range"),
			GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"), GET_INTEGER (L"Number of bins"),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), 1, GET_INTEGER (L"Garnish"));
	}
END

FORM (Matrix_getMean, L"Matrix: Get mean", 0)
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double mean = Matrix_getMean (me, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"));
	Melder_informationReal (mean, NULL);
END

FORM (Matrix_getStandardDeviation, L"Matrix: Get standard deviation", 0)
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double stdev = Matrix_getStandardDeviation (me, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"));
	Melder_informationReal (stdev, NULL);
END

FORM (Matrix_scale, L"Matrix: Scale", 0)
	LABEL (L"", L"self[row, col] := self[row, col] / `Scale factor'")
	RADIO (L"Scale factor", 1)
	RADIOBUTTON (L"Extremum in matrix")
	RADIOBUTTON (L"Extremum in each row")
	RADIOBUTTON (L"Extremum in each column")
	OK
DO
	int scale = GET_INTEGER (L"Scale factor");
	if (scale < 1 || scale > 4) {
		Melder_throw ("Scale must be in (0,4) interval.");
	}
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_scale (me, scale);
		praat_dataChanged (me);
	}
END

DIRECT (Matrix_transpose)
	LOOP {
		iam (Matrix);
		praat_new (Matrix_transpose (me), my name, L"_transposed");
	}
END

FORM (Matrix_solveEquation, L"Matrix: Solve equation", L"Matrix: Solve equation...")
	REAL (L"Tolerance", L"1.19e-7")
	OK
DO
	LOOP {
		iam (Matrix);
		praat_new (Matrix_solveEquation (me, GET_REAL (L"Tolerance")), Thing_getName (me), L"_solution");
	}
END

DIRECT (Matrix_Categories_to_TableOfReal)
	Matrix me = FIRST (Matrix);
	Categories cat = FIRST (Categories);
	praat_new (Matrix_and_Categories_to_TableOfReal (me, cat), my name, L"_", cat->name);
END

FORM (Matrix_scatterPlot, L"Matrix: Scatter plot", 0)
	NATURAL (L"Column for X-axis", L"1")
	NATURAL (L"Column for Y-axis", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	long x = GET_INTEGER (L"Column for X-axis");
	long y = GET_INTEGER (L"Column for Y-axis");
	if (x == 0 || y == 0) {
		Melder_throw ("X and Y component must differ from 0.");
	}
	LOOP {
		iam (Matrix);
		Matrix_scatterPlot (me, GRAPHICS, x, y, GET_REAL (L"left Horizontal range"),
			GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
			GET_REAL (L"right Vertical range"), GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
			GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Matrix_to_Activation)
	LOOP {
		iam (Matrix);
		praat_new (Matrix_to_Activation (me), my name);
	}
END

FORM (Matrices_to_DTW, L"Matrices: To DTW", L"Matrix: To DTW...")
	LABEL (L"", L"Distance  between cepstral coefficients")
	REAL (L"Distance metric", L"2.0")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	Matrix m1 = 0, m2 = 0;
	LOOP {
		iam (Matrix);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	praat_new (Matrices_to_DTW (m1, m2, begin, end, slope, GET_REAL (L"Distance metric")), m1->name, L"_", m2->name);
END

FORM (Matrix_to_Pattern, L"Matrix: To Pattern", 0)
	NATURAL (L"Join", L"1")
	OK
DO
	LOOP {
		iam (Matrix);
		praat_new (Matrix_to_Pattern (me, GET_INTEGER (L"Join")), my name);
	}
END

/***** MATRIXFT *************/

DIRECT (Matrixft_getHighestFrequency)
	LOOP {
		iam (Matrix);
		Melder_information (Melder_double (my ymax));
	}
END

DIRECT (Matrixft_getLowestFrequency)
	LOOP {
		iam (Matrix);
		Melder_information (Melder_double (my ymin));
	}
END

DIRECT (Matrixft_getNumberOfFrequencies)
	LOOP {
		iam (Matrix);
		Melder_information (Melder_double (my ny));
	}
END

DIRECT (Matrixft_getFrequencyDistance)
	LOOP {
		iam (Matrix);
		Melder_information (Melder_double (my dy));
	}
END

FORM (Matrixft_getFrequencyOfRow, L"Get frequency of row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	LOOP {
		iam (Matrix);
		Melder_information (Melder_double (Matrix_rowToY (me, GET_INTEGER (L"Row number"))));
	}
END

FORM (Matrixft_getXofColumn, L"Get time of column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (Matrix);
		Melder_information (Melder_double (Matrix_columnToX (me, GET_INTEGER (L"Column number"))));
	}
END

FORM (Matrixft_getValueInCell, L"Get value in cell", 0)
	POSITIVE (L"Time (s)", L"0.5")
	POSITIVE (L"Frequency", L"1")
	OK
DO
	double t = GET_REAL (L"Time");
	double f = GET_REAL (L"Frequency");
	LOOP {
		iam (Matrix);
		if (f < my ymin || f > my ymax) {
			Melder_throw ("Frequency out of range.");
		}
		if (t < my xmin || t > my xmax) {
			Melder_throw ("Time out of range.");
		}
		long col = Matrix_xToNearestColumn (me, t);
		if (col < 1) {
			col = 1;
		}
		if (col > my nx) {
			col = my nx;
		}
		long row = Matrix_yToNearestRow (me, f);
		if (row < 1) {
			row = 1;
		}
		if (row > my ny) {
			row = my ny;
		}
		double ta = Matrix_columnToX (me, col);
		double fa = Matrix_rowToY (me, row);
		Melder_information (Melder_single (my z[row][col]), L" (delta t: ", Melder_double (ta - t), L" f: ",
		Melder_double (fa - f), L")");
	}
END

/**************** MelFilter *******************************************/

DIRECT (MelFilter_help)
	Melder_help (L"MelFilter");
END

FORM (MelFilter_drawFilterFunctions, L"MelFilter: Draw filter functions", L"FilterBank: Draw filter functions...")
	INTEGER (L"left Filter range", L"0")
	INTEGER (L"right Filter range", L"0")
	RADIO (L"Frequency scale", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"Mel")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	BOOLEAN (L"Amplitude scale in dB", 0)
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (MelFilter);
		MelFilter_drawFilterFunctions (me, GRAPHICS, GET_INTEGER (L"Frequency scale"),
			GET_INTEGER (L"left Filter range"), GET_INTEGER (L"right Filter range"),
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_INTEGER (L"Amplitude scale in dB"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
			GET_INTEGER (L"Garnish"));
	}
END

FORM (MelFilter_drawSpectrum, L"MelFilter: Draw spectrum (slice)", L"FilterBank: Draw spectrum (slice)...")
	POSITIVE (L"Time (s)", L"0.1")
	REAL (L"left Frequency range (mel)", L"0.0")
	REAL (L"right Frequency range (mel)", L"0.0")
	REAL (L"left Amplitude range (dB)", L"0.0")
	REAL (L"right Amplitude range (dB)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FilterBank);
		FilterBank_drawTimeSlice (me, GRAPHICS, GET_REAL (L"Time"), GET_REAL (L"left Frequency range"),
			GET_REAL (L"right Frequency range"), GET_REAL (L"left Amplitude range"),
			GET_REAL (L"right Amplitude range"), L"Mels", GET_INTEGER (L"Garnish"));
	}
END

FORM (MelFilter_to_MFCC, L"MelFilter: To MFCC", L"MelFilter: To MFCC...")
	NATURAL (L"Number of coefficients", L"12")
	OK
DO
	LOOP {
		iam (MelFilter);
		praat_new (MelFilter_to_MFCC (me, GET_INTEGER (L"Number of coefficients")), my name);
	}
END

/**************** Ltas *******************************************/

#include "UnicodeData.h"
FORM (Ltas_reportSpectralTilt, L"Ltas: Report spectral tilt", 0)
	POSITIVE (L"left Frequency range (Hz)", L"100.0")
	POSITIVE (L"right Frequency range (Hz)", L"5000.0")
	OPTIONMENU (L"Frequency scale", 1)
	OPTION (L"Linear")
	OPTION (L"Logarithmic")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	bool logScale = GET_INTEGER (L"Frequency scale") == 2;
	LOOP {
		iam (Ltas);
		double a, b;
		Ltas_fitTiltLine (me, GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			logScale, GET_INTEGER (L"Fit method"), &a, &b);
		MelderInfo_open ();
		MelderInfo_writeLine (L"Spectral model: amplitude_dB(frequency_Hz) " UNITEXT_ALMOST_EQUAL_TO " ", logScale ? L"offset + slope * log (frequency_Hz)" : L"offset + slope * frequency_Hz");
		MelderInfo_writeLine (L"Slope: ", Melder_double (a), logScale ? L" dB/decade" : L" dB/Hz");
		MelderInfo_writeLine (L"Offset: ", Melder_double (b), L" dB");
		MelderInfo_close ();
	}
END


/**************** MFCC *******************************************/

DIRECT (MFCC_help)
	Melder_help (L"MFCC");
END

FORM (MFCC_to_MelFilter, L"MFCC: To MelFilter", L"MFCC: To MelFilter...")
	INTEGER (L"From coefficient", L"0")
	INTEGER (L"To coefficient", L"0")
	OK
DO
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_MelFilter (me, GET_INTEGER (L"From coefficient"), GET_INTEGER (L"To coefficient")), my name);
	}
END

FORM (MFCC_to_TableOfReal, L"MFCC: To TableOfReal", L"MFCC: To TableOfReal...")
	BOOLEAN (L"Include energy", 0)
	OK
DO
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_TableOfReal (me, GET_INTEGER (L"Include energy")), my name);
	}
END

FORM (MFCC_to_Matrix_features, L"MFCC: To Matrix (features)", L"")
	POSITIVE (L"Window length (s)", L"0.025")
	BOOLEAN (L"Include energy", 0)
	OK
DO
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_Matrix_features (me, GET_REAL (L"Window length"), GET_INTEGER (L"Include energy")), my name);
	}
END

FORM (MFCCs_crossCorrelate, L"MFCC & MFCC: Cross-correlate", 0)
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	MFCC m1 = 0, m2 = 0;
	LOOP {
		iam (MFCC);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	praat_new (MFCCs_crossCorrelate (m1, m2,
		GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is...")),
		m1 -> name, L"_", m2 -> name);
END

FORM (MFCCs_convolve, L"MFCC & MFCC: Convolve", 0)
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	MFCC m1 = 0, m2 = 0;
	LOOP {
		iam (MFCC);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	praat_new (MFCCs_convolve (m1, m2,
		GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is...")),
		m1 -> name, L"_", m2 -> name);
END

DIRECT (MFCC_to_Sound)
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_Sound (me), my name);
	}
END

/**************** MSpline *******************************************/

FORM (MSpline_create, L"Create MSpline", L"Create MSpline...")
	WORD (L"Name", L"mspline")
	LABEL (L"", L"Domain")
	REAL (L"Xmin", L"0")
	REAL (L"Xmax", L"1")
	LABEL (L"", L"MSpline(x) = c[1] M[1](x) + c[2] M[1](x) + ... c[n] M[n](x)")
	LABEL (L"", L"all M[k] are polynomials of degree \"Degree\"")
	LABEL (L"", L"Relation: numberOfCoefficients == numberOfInteriorKnots + degree + 1")
	INTEGER (L"Degree", L"2")
	SENTENCE (L"Coefficients (c[k])", L"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE (L"Interior knots" , L"0.3 0.5 0.6")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	long degree = GET_INTEGER (L"Degree");
	if (xmin >= xmax) {
		Melder_throw ("Xmin must be smaller than Xmax.");
	}
	praat_new (MSpline_createFromStrings (xmin, xmax, degree,
		GET_STRING (L"Coefficients"), GET_STRING (L"Interior knots")), GET_STRING (L"Name"));
END

DIRECT (MSpline_help) Melder_help (L"MSpline"); END

/********************** Pattern *******************************************/

DIRECT (Pattern_and_Categories_to_Discriminant)
	Pattern me = FIRST (Pattern);
	Categories cat = FIRST (Categories);
	praat_new (Pattern_and_Categories_to_Discriminant (me, cat), Thing_getName (me), L"_", Thing_getName (cat));
	END

	FORM (Pattern_draw, L"Pattern: Draw", 0)
	NATURAL (L"Pattern number", L"1")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Pattern);
		Pattern_draw (me, GRAPHICS, GET_INTEGER (L"Pattern number"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Pattern_formula, L"Pattern: Formula", 0)
	LABEL (L"label", L"        y := 1; for row := 1 to nrow do { x := 1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + 1 } "
		"y := y + 1 }}")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	praat_Fon_formula (dia, interpreter);
END

FORM (Pattern_setValue, L"Pattern: Set value", L"Pattern: Set value...")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	REAL (L"New value", L"0.0")
	OK
DO
	LOOP {
		iam (Pattern);
		long row = GET_INTEGER (L"Row number"), column = GET_INTEGER (L"Column number");
		if (row > my ny) {
			Melder_throw ("Row number must not be greater than number of rows.");
		}
		if (column > my nx) {
			Melder_throw ("Column number must not be greater than number of columns.");
		}
		my z [row] [column] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

DIRECT (Pattern_to_Matrix)
	LOOP {
		iam (Pattern);
		praat_new (Pattern_to_Matrix (me), my name);
	}
END

/******************* PCA ******************************/

DIRECT (PCA_help)
	Melder_help (L"PCA");
END

DIRECT (hint_PCA_and_TableOfReal_to_Configuration)
	Melder_information (L"You can get principal components by selecting a PCA and a TableOfReal\n"
		"together and choosing \"To Configuration...\".");
END

DIRECT (hint_PCA_and_Covariance_Project)
	Melder_information (L"You can get a new Covariance object rotated to the directions of the direction vectors\n"
		" in the PCA object by selecting a PCA and a Covariance object together.");
END

DIRECT (hint_PCA_and_Configuration_to_TableOfReal_reconstruct)
	Melder_information (L"You can reconstruct the original TableOfReal as well as possible from\n"
		" the principal components in the Configuration and the direction vectors in the PCA object.");
END

FORM (PCA_and_TableOfReal_getFractionVariance, L"PCA & TableOfReal: Get fraction variance", L"PCA & TableOfReal: Get fraction variance...")
	NATURAL (L"left Principal component range", L"1")
	NATURAL (L"right Principal component range", L"1")
	OK
DO
	PCA me = FIRST (PCA);
	TableOfReal tab = FIRST (TableOfReal);
	Melder_information (Melder_double (PCA_and_TableOfReal_getFractionVariance (me, tab,
		GET_INTEGER (L"left Principal component range"), GET_INTEGER (L"right Principal component range"))));
END

DIRECT (PCA_and_Configuration_to_TableOfReal_reconstruct)
	PCA me = FIRST (PCA);
	Configuration conf = FIRST (Configuration);
	praat_new (PCA_and_Configuration_to_TableOfReal_reconstruct (me, conf), my name, L"_", conf->name);
END

FORM (PCA_and_TableOfReal_to_Configuration, L"PCA & TableOfReal: To Configuration", L"PCA & TableOfReal: To Configuration...")
	INTEGER (L"Number of dimensions", L"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	if (dimension < 0) {
		Melder_throw ("Number of dimensions must be greater equal zero.");
	}
	PCA me = FIRST (PCA);
	TableOfReal tab = FIRST_GENERIC (TableOfReal);
	praat_new (PCA_and_TableOfReal_to_Configuration (me, tab, dimension), my name, L"_", tab->name);
END

FORM (PCA_and_TableOfReal_to_TableOfReal_zscores, L"PCA & TableOfReal: To TableOfReal (z-scores)", L"PCA & TableOfReal: To TableOfReal (z-scores)...")
	INTEGER (L"Number of dimensions", L"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	if (dimension < 0) {
		Melder_throw ("Number of dimensions must be greater than or equal to zero.");
	}
	PCA me = FIRST (PCA);
	TableOfReal thee = FIRST_GENERIC (TableOfReal);
	praat_new (PCA_and_TableOfReal_to_TableOfReal_zscores (me, thee, dimension), my name, L"_", thy name, L"_zscores");
END

FORM (PCA_getCentroidElement, L"PCA: Get centroid element...", 0)
	NATURAL (L"Number", L"1")
	OK
DO
	long number = GET_INTEGER (L"Number");
	LOOP {
		iam (PCA);
		if (number > my dimension) {
			Melder_throw ("Number may not be larger than ", my dimension, ".");
		}
		Melder_information (Melder_double (my centroid[number]), L" (element ", Melder_integer (number), L")");
	}
END

FORM (PCA_getEqualityOfEigenvalues, L"PCA: Get equality of eigenvalues", L"PCA: Get equality of eigenvalues...")
	INTEGER (L"left Eigenvalue range", L"0")
	INTEGER (L"right Eigenvalue range", L"0")
	BOOLEAN (L"Conservative test", 0)
	OK
DO
	LOOP {
		iam (PCA);
		long ndf; double p, chisq;
		PCA_getEqualityOfEigenvalues (me, GET_INTEGER (L"left Eigenvalue range"),
		GET_INTEGER (L"right Eigenvalue range"), GET_INTEGER (L"Conservative test"), &p, &chisq, &ndf);
		Melder_information (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
	}
END

FORM (PCA_getNumberOfComponentsVAF, L"PCA: Get number of components (VAF)", L"PCA: Get number of components (VAF)...")
	POSITIVE (L"Variance fraction (0-1)", L"0.95")
	OK
DO
	double f = GET_REAL (L"Variance fraction");
	LOOP {
		iam (Eigen);
		if (f <= 0 || f > 1) {
			Melder_throw ("The variance fraction must be in interval (0-1).");
		}
		Melder_information (Melder_integer (Eigen_getDimensionOfFraction (me, f)));
	}
END

FORM (PCA_getFractionVAF, L"PCA: Get fraction variance accounted for", L"PCA: Get fraction variance accounted for...")
	NATURAL (L"left Principal component range", L"1")
	NATURAL (L"right Principal component range", L"1")
	OK
DO
	long from = GET_INTEGER (L"left Principal component range");
	long to = GET_INTEGER (L"right Principal component range");
	if (from > to) {
		Melder_throw ("The second component must be greater than or equal to the first component.");
	}
	LOOP {
		iam (Eigen);
		if (from > to) {
			Melder_throw ("The second component must be greater than or equal to the first component.");
		}
		Melder_information (Melder_double (Eigen_getCumulativeContributionOfComponents (me, from, to)));
	}
END

FORM (PCA_invertEigenvector, L"PCA: Invert eigenvector", 0)
	NATURAL (L"Eigenvector number", L"1")
	OK
DO
	LOOP {
		iam (Eigen);
		Eigen_invertEigenvector (me, GET_INTEGER (L"Eigenvector number"));
		praat_dataChanged (me);
	}
END

FORM (PCA_to_TableOfReal_reconstruct1, L"PCA: To TableOfReal (reconstruct)", L"PCA: To TableOfReal (reconstruct 1)...")
	SENTENCE (L"Coefficients", L"1.0 1.0")
	OK
DO
	LOOP {
		iam (PCA);
		praat_new (PCA_to_TableOfReal_reconstruct1 (me, GET_STRING (L"Coefficients")), my name, L"_reconstructed");
	}
END

FORM (PCAs_to_Procrustes, L"PCA & PCA: To Procrustes", L"PCA & PCA: To Procrustes...")
	NATURAL (L"left Eigenvector range", L"1")
	NATURAL (L"right Eigenvector range", L"2")
	OK
DO
	long from = GET_INTEGER (L"left Eigenvector range");
	long to = GET_INTEGER (L"right Eigenvector range");
	PCA p1 = 0, p2 = 0;
	LOOP {
		iam (PCA);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	praat_new (Eigens_to_Procrustes (p1, p2, from, to), Thing_getName (p1), L"_", Thing_getName (p2));
END


DIRECT (PCAs_getAngleBetweenPc1Pc2Plane_degrees)
	PCA p1 = 0, p2 = 0;
	LOOP {
		iam (PCA);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	Melder_information (Melder_double (Eigens_getAngleBetweenEigenplanes_degrees (p1, p2)),
		L" degrees (=angle of intersection between the two pc1-pc2 eigenplanes)");
END

/******************* Permutation **************************************/

DIRECT (Permutation_help)
	Melder_help (L"Permutation");
END

FORM (Permutation_create, L"Create Permutation", L"Create Permutation...")
	WORD (L"Name", L"p")
	NATURAL (L"Number of elements", L"10")
	BOOLEAN (L"Identity Permutation", 1)
	OK
DO
	Permutation p = Permutation_create (GET_INTEGER (L"Number of elements"));
	int identity = GET_INTEGER (L"Identity Permutation");
	if (! identity) {
		Permutation_permuteRandomly_inline (p, 0, 0);
	}
	praat_new (p, GET_STRING (L"Name"));
END

DIRECT (Permutation_getNumberOfElements)
	LOOP {
		iam (Permutation);
		Melder_information (Melder_integer (my numberOfElements));
	}
END

FORM (Permutation_getValueAtIndex, L"Permutation: Get value", L"Permutation: Get value...")
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (Permutation);
		Melder_information (Melder_integer (Permutation_getValueAtIndex (me, index)), L" (value, at index = ",
			Melder_integer (index), L")");
	}
END

FORM (Permutation_getIndexAtValue, L"Permutation: Get index", L"Permutation: Get index...")
	NATURAL (L"Value", L"1")
	OK
DO
	long value = GET_INTEGER (L"Value");
	LOOP {
		iam (Permutation);
		Melder_information (Melder_integer (Permutation_getIndexAtValue (me, value)), L" (index, at value = ",
			Melder_integer (value), L")");
	}
END

DIRECT (Permutation_sort)
	LOOP {
		iam (Permutation);
		Permutation_sort (me);
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapBlocks, L"Permutation: Swap blocks", L"Permutation: Swap blocks...")
	NATURAL (L"From index", L"1")
	NATURAL (L"To index", L"2")
	NATURAL (L"Block size", L"1")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapBlocks (me, GET_INTEGER (L"From index"), GET_INTEGER (L"To index"), GET_INTEGER (L"Block size"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapPositions, L"Permutation: Swap positions", L"Permutation: Swap positions...")
	NATURAL (L"First index", L"1")
	NATURAL (L"Second index", L"2")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapPositions (me, GET_INTEGER (L"First index"), GET_INTEGER (L"Second index"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapNumbers, L"Permutation: Swap numbers", L"Permutation: Swap numbers...")
	NATURAL (L"First number", L"1")
	NATURAL (L"Second number", L"2")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapNumbers (me, GET_INTEGER (L"First number"), GET_INTEGER (L"Second number"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapOneFromRange, L"Permutation: Swap one from range", L"Permutation: Swap one from range...")
	LABEL (L"", L"A randomly chosen element from ")
	INTEGER (L"left Index range", L"0")
	INTEGER (L"right Index range", L"0")
	LABEL (L"", L"is swapped with the element at")
	NATURAL (L"Index", L"1")
	BOOLEAN (L"Forbid same", 1)
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapOneFromRange (me, GET_INTEGER (L"left Index range"), GET_INTEGER (L"right Index range"),
			GET_INTEGER (L"Index"), GET_INTEGER (L"Forbid same"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_permuteRandomly, L"Permutation: Permute randomly", L"Permutation: Permute randomly...")
	INTEGER (L"left Index range", L"0")
	INTEGER (L"right Index range", L"0")
	OK
DO
	LOOP {
		iam (Permutation);
		praat_new (Permutation_permuteRandomly (me, GET_INTEGER (L"left Index range"),
		GET_INTEGER (L"right Index range")), Thing_getName (me), L"_rdm");
	}
END

FORM (Permutation_rotate, L"Permutation: Rotate", L"Permutation: Rotate...")
	INTEGER (L"left Index range", L"0")
	INTEGER (L"right Index range", L"0")
	INTEGER (L"Step size", L"1")
	OK
DO
	long step = GET_INTEGER (L"Step size");
	LOOP {
		iam (Permutation);
		praat_new (Permutation_rotate (me, GET_INTEGER (L"left Index range"), GET_INTEGER (L"right Index range"), step),
			Thing_getName (me), L"_rot", Melder_integer (step));
	}
END

FORM (Permutation_reverse, L"Permutation: Reverse", L"Permutation: Reverse...")
	INTEGER (L"left Index range", L"0")
	INTEGER (L"right Index range", L"0")
	OK
DO
	LOOP {
		iam (Permutation);
		praat_new (Permutation_reverse (me, GET_INTEGER (L"left Index range"), GET_INTEGER (L"right Index range")),
			Thing_getName (me), L"_rev");
	}
END

FORM (Permutation_permuteBlocksRandomly, L"Permutation: Permute blocks randomly", L"Permutation: Permute randomly (blocks)...")
	INTEGER (L"left Index range", L"0")
	INTEGER (L"right Index range", L"0")
	NATURAL (L"Block size", L"12")
	BOOLEAN (L"Permute within blocks", 1)
	BOOLEAN (L"No doublets", 0)
	OK
DO
	long blocksize = GET_INTEGER (L"Block size");
	LOOP {
		iam (Permutation);
		praat_new (Permutation_permuteBlocksRandomly (me, GET_INTEGER (L"left Index range"),
			GET_INTEGER (L"right Index range"), blocksize, GET_INTEGER (L"Permute within blocks"),
			GET_INTEGER (L"No doublets")), Thing_getName (me), L"_pbr", Melder_integer (blocksize));
	}
END

FORM (Permutation_interleave, L"Permutation: Interleave", L"Permutation: Interleave...")
	INTEGER (L"left Index range", L"0")
	INTEGER (L"right Index range", L"0")
	NATURAL (L"Block size", L"12")
	INTEGER (L"Offset", L"0")
	OK
DO
	LOOP {
		iam (Permutation);
		praat_new (Permutation_interleave (me, GET_INTEGER (L"left Index range"), GET_INTEGER (L"right Index range"),
			GET_INTEGER (L"Block size"), GET_INTEGER (L"Offset")), Thing_getName (me), L"_itl");
	}
END

DIRECT (Permutation_invert)
	LOOP {
		iam (Permutation);
		praat_new (Permutation_invert (me), Thing_getName (me), L"_inv");
	}
END
DIRECT (Permutations_multiply)
	autoCollection set = praat_getSelectedObjects ();
	praat_new (Permutations_multiply (set.peek()), L"mul_", Melder_integer (set -> size));
END

DIRECT (Permutations_next)
	LOOP {
		iam (Permutation);
		Permutation_next_inline (me);
		praat_dataChanged (me);
	}
END

DIRECT (Permutations_previous)
	LOOP {
		iam (Permutation);
		Permutation_previous_inline (me);
		praat_dataChanged (me);
	}
END

FORM (Pitches_to_DTW, L"Pitches: To DTW", L"Pitches: To DTW...")
	REAL (L"Voiced-unvoiced costs", L"24.0")
	REAL (L"Time costs weight", L"10.0")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	Pitch p1 = 0, p2 = 0;
	LOOP {
		iam (Pitch);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	praat_new (Pitches_to_DTW (p1, p2, GET_REAL (L"Voiced-unvoiced costs"), GET_REAL (L"Time costs weight"), begin, end, slope), L"dtw_", Thing_getName (p1), L"_", Thing_getName (p2));
END

FORM (PitchTier_to_Pitch, L"PitchTier: To Pitch", L"PitchTier: To Pitch...")
	POSITIVE (L"Step size", L"0.02")
	POSITIVE (L"Pitch floor", L"60.0")
	POSITIVE (L"Pitch ceiling", L"400.0")
	OK
DO
	LOOP {
		iam (PitchTier);
		praat_new (PitchTier_to_Pitch (me, GET_REAL (L"Step size"),
		GET_REAL (L"Pitch floor"), GET_REAL (L"Pitch ceiling")), my name);
	}
END

/******************* Polygon & Categories *************************************/

FORM (Polygon_createSimple, L"Create simple Polygon", L"Create simple Polygon...")
	WORD (L"Name", L"p")
	SENTENCE (L"Vertices as X-Y pairs", L"0.0 0.0  0.0 1.0  1.0 0.0")
	OK
DO
	praat_new (Polygon_createSimple (GET_STRING (L"Vertices as X-Y pairs")), GET_STRING (L"Name"));
END

FORM (Polygon_createFromRandomVertices, L"", 0)
	WORD (L"Name", L"p")
	NATURAL (L"Number of vertices", L"10")
	REAL (L"left X range", L"0.0")
	REAL (L"right X range", L"1.0")
	REAL (L"left Y range", L"0.0")
	REAL (L"right Y range", L"1.0")
	OK
DO
	praat_new (Polygon_createFromRandomVertices (GET_INTEGER (L"Number of vertices"),
		GET_REAL (L"left X range"), GET_REAL (L"right X range"),
		GET_REAL (L"left Y range"), GET_REAL (L"right Y range")), GET_STRING (L"Name"));
END

DIRECT (Polygon_getNumberOfPoints)
	LOOP {
		iam (Polygon);
		Melder_information (Melder_integer (my numberOfPoints));
	}
END

FORM (Polygon_getPointX, L"Polygon: Get point (x)", 0)
	NATURAL (L"Point number", L"1")
	OK
DO
	long point = GET_INTEGER (L"Point number");
	LOOP {
		iam (Polygon);
		if (point > my numberOfPoints) {
			Melder_throw ("Point cannot be larger than ", my numberOfPoints, ".");
		}
		Melder_information (Melder_double (my x[point]));
	}
END

FORM (Polygon_getPointY, L"Polygon: Get point (y)", 0)
	NATURAL (L"Point number", L"1")
	OK
DO
	long point = GET_INTEGER (L"Point number");
	LOOP {
		iam (Polygon);
		if (point > my numberOfPoints) {
			Melder_throw ("Vertex cannot be larger than ", my numberOfPoints, ".");
		}
		Melder_information (Melder_double (my y[point]));
	}
END

FORM (Polygon_getLocationOfPoint, L"Get location of point", L"Polygon: Get location of point...")
	LABEL (L"", L"Point is (I)n, (O)ut, (E)dge or (V)ertex?")
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	REAL (L"Precision", L"1.64e-15")
	OK
DO
	double eps = GET_REAL (L"Precision");
	REQUIRE (eps >= 0, L"The precision cannot be negative.")
	LOOP {
		iam (Polygon);
		int loc = Polygon_getLocationOfPoint (me, GET_REAL (L"X"), GET_REAL (L"Y"), eps);
		Melder_information (loc == Polygon_INSIDE ? L"I" : loc == Polygon_OUTSIDE ? L"O" :
		loc == Polygon_EDGE ? L"E" : L"V");
	}
END

FORM (Polygon_circularPermutation, L"Polygon: Circular permutation", 0)
	INTEGER (L"Shift", L"1")
	OK
DO
	long shift = GET_INTEGER (L"Shift");
	LOOP {
		iam (Polygon);
		praat_new (Polygon_circularPermutation (me, shift), my name, L"_", Melder_integer (shift));
	}
END

DIRECT (Polygon_simplify)
	LOOP {
		iam (Polygon);
		praat_new (Polygon_simplify (me), my name, L"_s");
	}
END

FORM (Polygon_translate, L"Polygon: Translate", L"Polygon: Translate...")
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		Polygon_translate (me, GET_REAL (L"X"), GET_REAL (L"Y"));
	}
END

FORM (Polygon_rotate, L"Polygon: Rotate", L"Polygon: Rotate...")
	LABEL (L"", L"Rotate counterclockwise over the")
	REAL (L"Angle (degrees)", L"0.0")
	LABEL (L"", L"With respect to the point")
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		Polygon_rotate (me, GET_REAL (L"Angle"), GET_REAL (L"X"), GET_REAL (L"Y"));
		praat_dataChanged (me);
	}
END

FORM (Polygon_scale, L"Polygon: Scale polygon", 0)
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		Polygon_scale (me, GET_REAL (L"X"), GET_REAL (L"Y"));
		praat_dataChanged (me);
	}
END

FORM (Polygon_Categories_draw, L"Polygon & Categories: Draw", 0)
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Polygon me = FIRST (Polygon);
	Categories cat = FIRST (Categories);
	Polygon_Categories_draw (me, cat, GRAPHICS, GET_REAL (L"left Horizontal range"),
		GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish"));
END

DIRECT (Polygon_reverseX)
	LOOP {
		iam (Polygon);
		Polygon_reverseX (me);
		praat_dataChanged (me);
	}
END

DIRECT (Polygon_reverseY)
	LOOP {
		iam (Polygon);
		Polygon_reverseY (me);
		praat_dataChanged (me);
	}
END

/***************** Polynomial *******************/

DIRECT (Polynomial_help) Melder_help (L"Polynomial"); END

FORM (Polynomial_create, L"Create Polynomial", L"Create Polynomial...")
	WORD (L"Name", L"p")
	LABEL (L"", L"Domain of polynomial")
	REAL (L"Xmin", L"-3")
	REAL (L"Xmax", L"4")
	LABEL (L"", L"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	SENTENCE (L"Coefficients", L"2.0 -1.0 -2.0 1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	if (xmin >= xmax) {
		Melder_throw ("Xmin must be smaller than Xmax.");
	}
	praat_new (Polynomial_createFromString (xmin, xmax, GET_STRING (L"Coefficients")), GET_STRING (L"Name"));
END

FORM (Polynomial_getArea, L"Polynomial: Get area", L"Polynomial: Get area...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	LOOP {
		iam (Polynomial);
		double area = Polynomial_getArea (me, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"));
		Melder_information (Melder_double (area));
	}
END

DIRECT (Polynomial_getDerivative)
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_getDerivative (me), my name, L"_derivative");
	}
END

DIRECT (Polynomial_getPrimitive)
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_getPrimitive (me), my name, L"_primitive");
	}
END

FORM (Polynomial_scaleX, L"Polynomial: Scale x", L"Polynomial: Scale x...")
	LABEL (L"", L"New domain")
	REAL (L"Xmin", L"-1.0")
	REAL (L"Xmax", L"1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	if (xmin >= xmax) {
		Melder_throw ("Xmin must be smaller than Xmax.");
	}
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_scaleX (me, xmin, xmax), my name, L"_scaleX");
	}
END

DIRECT (Polynomial_scaleCoefficients_monic)
	LOOP {
		iam (Polynomial);
		Polynomial_scaleCoefficients_monic (me);
		praat_dataChanged (me);
	}
END

DIRECT (Polynomial_to_Roots)
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_to_Roots (me), my name);
	}
END

FORM (Polynomial_evaluate_z, L"Polynomial: Get value (complex)", L"Polynomial: Get value (complex)...")
	REAL (L"Real part", L"0.0")
	REAL (L"Imaginary part", L"0.0")
	OK
DO
	dcomplex p, z = dcomplex_create (GET_REAL (L"Real part"), GET_REAL (L"Imaginary part"));
	LOOP {
		iam (Polynomial);
		Polynomial_evaluate_z (me, &z, &p);
		Melder_information (Melder_double (p.re), L" + ", Melder_double (p.im), L" i");
	}
END


FORM (Polynomial_to_Spectrum, L"Polynomial: To Spectrum", L"Polynomial: To Spectrum...")
	POSITIVE (L"Nyquist frequency (Hz)", L"5000.0")
	NATURAL (L"Number of frequencies (>1)", L"1025")
	OK
DO
	long n = GET_INTEGER (L"Number of frequencies");
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_to_Spectrum (me, GET_REAL (L"Nyquist frequency"), n, 1.0), my name);
	}
END

DIRECT (Polynomials_multiply)
	Polynomial p1 = 0, p2 = 0;
	LOOP {
		iam (Polynomial);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	praat_new (Polynomials_multiply (p1, p2), Thing_getName (p1), L"_x_", Thing_getName (p2));
END

FORM (Polynomials_divide, L"Polynomials: Divide", L"Polynomials: Divide...")
	BOOLEAN (L"Want quotient", 1)
	BOOLEAN (L"Want remainder", 1)
	OK
DO
/* With gcc (GCC) 3.2.2 20030217 (Red Hat Linux 8.0 3.2.2-2)
	The following line initiates pq = NULL and I don't know why
Polynomial p1 = NULL, p2 = NULL, pq, pr;
*/

	bool wantq = GET_INTEGER (L"Want quotient");
	bool wantr = GET_INTEGER (L"Want remainder");
	if (! wantq && ! wantr) {
		Melder_throw ("Either \'Want quotient\' or \'Want remainder\' must be chosen");
	}
	Polynomial p1 = 0, p2 = 0;
	LOOP {
		iam (Polynomial);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	Polynomial q, r;
	if (! wantq) {
		q = 0;
	}
	if (! wantr) {
		r = 0;
	}
	Polynomials_divide (p1, p2, &q, &r);
	autoPolynomial aq = q, ar = r;
	if (wantq) {
		praat_new (aq.transfer(), Thing_getName (p1), L"_q");
	}
	if (wantr) {
		praat_new (ar.transfer(), Thing_getName (p1), L"_r");
	}
END

/********************* Roots ******************************/

DIRECT (Roots_help) Melder_help (L"Roots"); END

FORM (Roots_draw, L"Roots: Draw", 0)
	REAL (L"Minimum of real axis", L"0.0")
	REAL (L"Maximum of real axis", L"0.0")
	REAL (L"Minimum of imaginary axis", L"0.0")
	REAL (L"Maximum of imaginary axis", L"0.0")
	SENTENCE (L"Mark string (+x0...)", L"o")
	NATURAL (L"Mark size", L"12")
	BOOLEAN (L"Garnish", 0)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Roots);
		Roots_draw (me, GRAPHICS, GET_REAL (L"Minimum of real axis"), GET_REAL (L"Maximum of real axis"),
		GET_REAL (L"Minimum of imaginary axis"), GET_REAL (L"Maximum of imaginary axis"),
		GET_STRING (L"Mark string"), GET_INTEGER (L"Mark size"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Roots_getNumberOfRoots)
	LOOP {
		iam (Roots);
		Melder_information (Melder_integer (Roots_getNumberOfRoots (me)));
	}
END

FORM (Roots_getRoot, L"Roots: Get root", 0)
	NATURAL (L"Root number", L"1")
	OK
DO
	LOOP {
		iam (Roots);
		dcomplex z = Roots_getRoot (me, GET_INTEGER (L"Root number"));
		Melder_information (Melder_double (z.re), (z.im < 0 ? L" - " : L" + "), Melder_double (fabs (z.im)), L" i");
	}
END

FORM (Roots_getRealPartOfRoot, L"Roots: Get real part", 0)
	NATURAL (L"Root number", L"1")
	OK
DO
	LOOP {
		iam (Roots);
		dcomplex z = Roots_getRoot (me, GET_INTEGER (L"Root number"));
		Melder_information (Melder_double (z.re));
	}
END

FORM (Roots_getImaginaryPartOfRoot, L"Roots: Get imaginary part", 0)
	NATURAL (L"Root number", L"1")
	OK
DO
	LOOP {
		iam (Roots);
		dcomplex z = Roots_getRoot (me, GET_INTEGER (L"Root number"));
		Melder_information (Melder_double (z.im));
	}
END

FORM (Roots_setRoot, L"Roots: Set root", 0)
	NATURAL (L"Root number", L"1")
	REAL (L"Real part", L"1.0/sqrt(2)")
	REAL (L"Imaginary part", L"1.0/sqrt(2)")
	OK
DO
	LOOP {
		iam (Roots);
		Roots_setRoot (me, GET_INTEGER (L"Root number"), GET_REAL (L"Real part"), GET_REAL (L"Imaginary part"));
		praat_dataChanged (me);
	}
END

FORM (Roots_to_Spectrum, L"Roots: To Spectrum", L"Roots: To Spectrum...")
	POSITIVE (L"Nyquist frequency (Hz)", L"5000.0")
	NATURAL (L"Number of frequencies (>1)", L"1025")
	OK
DO
	long n = GET_INTEGER (L"Number of frequencies");
	LOOP {
		iam (Roots);
		praat_new (Roots_to_Spectrum (me, GET_REAL (L"Nyquist frequency"), n, 1.0), my name);
	}
END

DIRECT (Roots_and_Polynomial_polish)
	Roots me = FIRST (Roots);
	Polynomial pol = FIRST (Polynomial);
	Roots_and_Polynomial_polish (me, pol);
	praat_dataChanged (me);
END

/*****************************************************************************/

DIRECT (Praat_ReportFloatingPointProperties)
	if (! NUMfpp) {
		NUMmachar ();
	}
	MelderInfo_open ();
	MelderInfo_writeLine (L"Double precision floating point properties of this machine,");
	MelderInfo_writeLine (L"as calculated by algorithms from the Binary Linear Algebra System (BLAS)");
	MelderInfo_writeLine (L"Radix: ", Melder_double (NUMfpp -> base));
	MelderInfo_writeLine (L"Number of digits in mantissa: ", Melder_double (NUMfpp -> t));
	MelderInfo_writeLine (L"Smallest exponent before (gradual) underflow (expmin): ", Melder_integer (NUMfpp -> emin));
	MelderInfo_writeLine (L"Largest exponent before overflow (expmax): ", Melder_integer (NUMfpp -> emax));
	MelderInfo_writeLine (L"Does rounding occur in addition: ", (NUMfpp -> rnd == 1 ? L"yes" : L"no"));
	MelderInfo_writeLine (L"Quantization step (d): ", Melder_double (NUMfpp -> prec));
	MelderInfo_writeLine (L"Quantization error (eps = d/2): ", Melder_double (NUMfpp -> eps));
	MelderInfo_writeLine (L"Underflow threshold (= radix ^ (expmin - 1)): ", Melder_double (NUMfpp -> rmin));
	MelderInfo_writeLine (L"Safe minimum (such that its inverse does not overflow): ", Melder_double (NUMfpp -> sfmin));
	MelderInfo_writeLine (L"Overflow threshold (= (1 - eps) * radix ^ expmax): ", Melder_double (NUMfpp -> rmax));
	MelderInfo_close ();
END

FORM (Praat_getTukeyQ, L"Get TukeyQ", 0)
	REAL (L"Critical value", L"2.0")
	NATURAL (L"Number of means", L"3")
	POSITIVE (L"Degrees of freedom", L"10.0")
	NATURAL (L"Number of rows", L"1")
	OK
DO
	double q = GET_REAL (L"Critical value");
	REQUIRE (q > 0 , L"Critical value must be > 0.")
	double val = NUMtukeyQ (q, GET_INTEGER (L"Number of means"), GET_REAL (L"Degrees of freedom"), GET_INTEGER (L"Number of rows") );
	Melder_informationReal (val, NULL);
END

FORM (Praat_getInvTukeyQ, L"Get invTukeyQ", 0)
	REAL (L"Probability", L"0.05")
	NATURAL (L"Number of means", L"3")
	POSITIVE (L"Degrees of freedom", L"10.0")
	NATURAL (L"Number of rows", L"1")
	OK
DO
	double p = GET_REAL (L"Probability");
	REQUIRE (p >= 0 && p <= 1, L"Probability must be in (0,1).")
	double val = NUMinvTukeyQ (p, GET_INTEGER (L"Number of means"), GET_REAL (L"Degrees of freedom"), GET_INTEGER (L"Number of rows"));
	Melder_informationReal (val, NULL);
END

/******************** Sound ****************************************/

static void Sound_create_addCommonFields (void *dia) {
	REAL (L"Starting time (s)", L"0.0")
	REAL (L"Finishing time (s)", L"0.1")
	POSITIVE (L"Sampling frequency (Hz)", L"44100.0")
}

static void Sound_create_checkCommonFields (void *dia, double *startingTime, double *finishingTime,
        double *samplingFrequency) {
	double numberOfSamples_real;
	*startingTime = GET_REAL (L"Starting time");
	*finishingTime = GET_REAL (L"Finishing time");
	*samplingFrequency = GET_REAL (L"Sampling frequency");
	numberOfSamples_real = floor ( (*finishingTime - *startingTime) * *samplingFrequency + 0.5);
	if (*finishingTime <= *startingTime) {
		if (*finishingTime == *startingTime) {
			Melder_throw ("A Sound cannot have a duration of zero.");
		} else {
			Melder_throw ("A Sound cannot have a duration less than zero.");
		}
		if (*startingTime == 0.0) {
			Melder_throw ("Please set the finishing time to something greater than 0 seconds.");
		} else {
			Melder_throw ("Please lower the starting time or raise the finishing time.");
		}
	}
	if (*samplingFrequency <= 0.0)
		Melder_throw ("A Sound cannot have a negative sampling frequency.\n"
		              "Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");

	if (numberOfSamples_real < 1.0) {
		Melder_error_ ("A Sound cannot have zero samples.\n");
		if (*startingTime == 0.0) {
			Melder_throw ("Please raise the finishing time.");
		} else {
			Melder_throw ("Please lower the starting time or raise the finishing time.");
		}
	}
	if (numberOfSamples_real > LONG_MAX) {
		Melder_throw ("A Sound cannot have ", Melder_bigInteger (numberOfSamples_real), " samples; the maximum is ", Melder_bigInteger (LONG_MAX), " samples.\n");
#if 0
		if (*startingTime == 0.0) {
			Melder_throw (L"Please lower the finishing time or the sampling frequency.");
		} else {
			Melder_throw (L"Please raise the starting time, lower the finishing time, or lower the sampling frequency.");
		}
#endif
	}
}

FORM (Sound_and_Pitch_to_FormantFilter, L"Sound & Pitch: To FormantFilter", L"Sound & Pitch: To FormantFilter...")
	POSITIVE (L"Analysis window duration (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (Hz)", L"100.0")
	POSITIVE (L"Distance between filters (Hz)", L"50.0")
	REAL (L"Maximum frequency", L"0");
	POSITIVE (L"Relative bandwidth", L"1.1")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch p = FIRST (Pitch);
	praat_new (Sound_and_Pitch_to_FormantFilter (me, p, GET_REAL (L"Analysis window duration"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters"),
		GET_REAL (L"Relative bandwidth")), my name, L"_", p->name);
END

FORM (Sound_and_Pitch_changeGender, L"Sound & Pitch: Change gender", L"Sound & Pitch: Change gender...")
	POSITIVE (L"Formant shift ratio", L"1.2")
	REAL (L"New pitch median (Hz)", L"0.0 (=no change)")
	POSITIVE (L"Pitch range factor", L"1.0 (=no change)")
	POSITIVE (L"Duration factor", L"1.0")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch p = FIRST (Pitch);
	praat_new (Sound_and_Pitch_changeGender_old (me, p, GET_REAL (L"Formant shift ratio"),
			GET_REAL (L"New pitch median"), GET_REAL (L"Pitch range factor"), GET_REAL (L"Duration factor")),
			my name, L"_", p->name);
END

FORM (Sound_and_Pitch_changeSpeaker, L"Sound & Pitch: Change speaker", L"Sound & Pitch: Change speaker...")
	POSITIVE (L"Multiply formants by", L"1.1 (male->female)")
	POSITIVE (L"Multiply pitch by", L"1.8 (male->female")
	REAL (L"Multiply pitch range by", L"1.0 (=no change)")
	POSITIVE (L"Multiply duration", L"1.0")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch p = FIRST (Pitch);
	praat_new (Sound_and_Pitch_changeSpeaker (me, p, GET_REAL (L"Multiply formants by"),
		GET_REAL (L"Multiply pitch by"), GET_REAL (L"Multiply pitch range by"), GET_REAL (L"Multiply duration")), my name, L"_", p->name);
END

FORM (Sound_and_IntervalTier_cutPartsMatchingLabel, L"Sound & IntervalTier: Cut parts matching label", 0)
	SENTENCE (L"Label", L"cut")
	OK
DO
	const wchar_t *label = GET_STRING (L"Label");
	Sound me = FIRST (Sound);
	IntervalTier thee = FIRST (IntervalTier);
	praat_new (Sound_and_IntervalTier_cutPartsMatchingLabel (me, thee, label), my name, L"_cut");
END

FORM (Sound_createFromGammaTone, L"Create a gammatone", L"Create Sound from gammatone...")
	WORD (L"Name", L"gammatone")
	Sound_create_addCommonFields (dia);
	INTEGER (L"Gamma", L"4")
	POSITIVE (L"Frequency (Hz)", L"1000.0")
	REAL (L"Bandwidth (Hz)", L"150.0")
	REAL (L"Initial phase (radians)", L"0.0")
	REAL (L"Addition factor", L"0.0")
	BOOLEAN (L"Scale amplitudes", 1)
	OK
DO
	double startingTime, finishingTime, samplingFrequency;
	long gamma = GET_INTEGER (L"Gamma");
	double bandwidth = GET_REAL (L"Bandwidth");
	double f = GET_REAL (L"Frequency");

	Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency);
	if (f >= samplingFrequency / 2) Melder_throw ("Frequency cannot be larger than half the sampling frequency.\n"
				"Please use a frequency smaller than ", samplingFrequency / 2);
	if (gamma < 0) {
		Melder_throw ("Gamma cannot be negative.\nPlease use a positive or zero gamma.");
	}
	if (bandwidth < 0) {
		Melder_throw ("Bandwidth cannot be negative.\nPlease use a positive or zero bandwidth.");
	}
	autoSound sound = Sound_createGammaTone (startingTime, finishingTime, samplingFrequency, gamma, f, bandwidth,
					GET_REAL (L"Initial phase"), GET_REAL (L"Addition factor"), GET_INTEGER (L"Scale amplitudes"));
	//Sound_create_check (sound.peek(), startingTime, finishingTime, samplingFrequency);//TODO
	praat_new (sound.transfer(), GET_STRING (L"Name"));
END

FORM (Sound_createFromShepardTone, L"Create a Shepard tone", L"Create Sound from Shepard tone...")
	WORD (L"Name", L"shepardTone")
	Sound_create_addCommonFields (dia);
	POSITIVE (L"Lowest frequency (Hz)", L"4.863")
	NATURAL (L"Number of components", L"10")
	REAL (L"Frequency change (semitones/s)", L"4.0")
	REAL (L"Amplitude range (dB)", L"30.0")
	REAL (L"Octave shift fraction ([0,1))", L"0.0")
	OK
DO
	double startingTime, finishingTime, samplingFrequency;
	double amplitudeRange = GET_REAL (L"Amplitude range");
	double octaveShiftFraction = GET_REAL (L"Octave shift fraction");
	Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency);
	if (amplitudeRange < 0) {
		Melder_throw ("Amplitude range cannot be negative.\nPlease use a positive or zero amplitude range.");
	}
	autoSound sound = Sound_createShepardToneComplex (startingTime, finishingTime, samplingFrequency,
					GET_REAL (L"Lowest frequency"), GET_INTEGER (L"Number of components"),
					GET_REAL (L"Frequency change"), GET_REAL (L"Amplitude range"), octaveShiftFraction);
	// Sound_create_check (sound, startingTime, finishingTime, samplingFrequency); //TODO
	praat_new (sound.transfer(), GET_STRING (L"Name"));
END

FORM (Sound_drawWhere, L"Sound: Draw where", L"Sound: Draw where...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range", L"0.0 (= all)")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 1)
	OPTION (L"Curve")
	OPTION (L"Bars")
	OPTION (L"Poles")
	OPTION (L"Speckles")
	LABEL (L"", L"Draw only those parts where the following condition holds:")
	TEXTFIELD (L"Formula", L"x < xmin + (xmax - xmin) / 2; first half")
	OK
DO
	long numberOfBisections = 10;
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_drawWhere (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"),
		GET_STRING (L"Drawing method"), numberOfBisections, GET_STRING (L"Formula"), interpreter);
	}
END

FORM (Sound_playOneChannel, L"Sound: Play one channel", 0)
    NATURAL (L"Channel", L"1")
    OK
DO
    long ichannel = GET_INTEGER (L"Channel");
    LOOP {
        iam (Sound);
        if (ichannel > my ny) {
            Melder_throw (me, " there is no channel 6. Sound in channel not played.");
        }
        autoSound thee = Sound_extractChannel (me, ichannel);
        Sound_play (thee.peek(), 0, 0);
    }
END

FORM (Sound_playAsFrequencyShifted, L"Sound: Play as frequency shifted", L"Sound: Play as frequency shifted...")
	REAL (L"Shift by (Hz)", L"1000.0")
	POSITIVE (L"New sampling frequency (Hz)", L"44100.0")
	NATURAL (L"Precision (samples)", L"50")
	OK
DO
	double shiftBy = GET_REAL (L"Shift by");
	double newSamplingFrequency = GET_REAL (L"New sampling frequency");
	long precision = GET_INTEGER (L"Precision");
	LOOP {
		iam (Sound);
		Sound_playAsFrequencyShifted (me, shiftBy, newSamplingFrequency, precision);
	}
END

FORM (Sounds_to_DTW, L"Sounds: To DTW", 0)
    POSITIVE (L"Window length (s)", L"0.015")
    POSITIVE (L"Time step (s)", L"0.005")
    LABEL (L"", L"")
    REAL (L"Sakoe-Chiba band (s)", L"0.1")
    RADIO (L"Slope constraint", 1)
    RADIOBUTTON (L"no restriction")
    RADIOBUTTON (L"1/3 < slope < 3")
    RADIOBUTTON (L"1/2 < slope < 2")
    RADIOBUTTON (L"2/3 < slope < 3/2")
    OK
DO
    double analysisWidth = GET_REAL (L"Window length");
    double dt = GET_REAL (L"Time step");
    double band = GET_REAL (L"Sakoe-Chiba band");
    int slope = GET_INTEGER (L"Slope constraint");
    Sound s1 = 0, s2 = 0;
    LOOP {
        iam (Sound);
        (s1 ? s2 : s1) = me;
    }
    Melder_assert (s1 && s2);
    praat_new (Sounds_to_DTW (s1, s2, analysisWidth, dt, band, slope), s1 -> name, L"_", s2 -> name);
END

FORM (Sound_to_TextGrid_detectSilences, L"Sound: To TextGrid (silences)", L"Sound: To TextGrid (silences)...")
	LABEL (L"", L"Parameters for the intensity analysis")
	POSITIVE (L"Minimum pitch (Hz)", L"100")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	LABEL (L"", L"Silent intervals detection")
	REAL (L"Silence threshold (dB)", L"-25.0")
	POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
	POSITIVE (L"Minimum sounding interval duration (s)", L"0.1")
	WORD (L"Silent interval label", L"silent")
	WORD (L"Sounding interval label", L"sounding")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_TextGrid_detectSilences (me, GET_REAL (L"Minimum pitch"), GET_REAL (L"Time step"),
		GET_REAL (L"Silence threshold"), GET_REAL (L"Minimum silent interval duration"),
		GET_REAL (L"Minimum sounding interval duration"), GET_STRING (L"Silent interval label"),
		GET_STRING (L"Sounding interval label")), my name);
	}
END

FORM (Sound_copyChannelRanges, L"Sound: Copy channel ranges", 0)
	LABEL (L"", L"Create a new Sound from the following channels:")
	TEXTFIELD (L"Ranges", L"1:64")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_copyChannelRanges (me, GET_STRING (L"Ranges"));
		praat_new (thee.transfer(), my name, L"_channels");
	}
END

FORM (Sound_trimSilences, L"Sound: Trim silences", L"Sound: Trim silences...")
    REAL (L"Trim duration (s)", L"0.08")
	BOOLEAN (L"Only at start and end", 1);
	LABEL (L"", L"Parameters for the intensity analysis")
	POSITIVE (L"Minimum pitch (Hz)", L"100")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	LABEL (L"", L"Silent intervals detection")
	REAL (L"Silence threshold (dB)", L"-35.0")
	POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
	POSITIVE (L"Minimum sounding interval duration (s)", L"0.05")
    BOOLEAN (L"Save trimming info as TextGrid", 0)
    WORD (L"Trim label", L"trimmed")
	OK
DO
    double trimDuration = GET_REAL (L"Trim duration");
    if (trimDuration < 0) {
        trimDuration = 0;
    }
	bool onlyAtStartAndEnd = GET_INTEGER (L"Only at start and end");
	double minPitch = GET_REAL (L"Minimum pitch");
	double timeStep = GET_REAL (L"Time step");
	double silenceThreshold = GET_REAL (L"Silence threshold");
	double minSilenceDuration = GET_REAL (L"Minimum silent interval duration");
	double minSoundingDuration = GET_REAL (L"Minimum sounding interval duration");
    bool saveTextGrid = GET_INTEGER (L"Save trimming info as TextGrid");
    const wchar_t *trimlabel = GET_STRING (L"Trim label");
	LOOP {
		iam (Sound);
        TextGrid tg = NULL;
		autoSound thee = Sound_trimSilences (me, trimDuration, onlyAtStartAndEnd, minPitch, timeStep, silenceThreshold,
			minSilenceDuration, minSoundingDuration, (saveTextGrid ? &tg : 0), trimlabel);
        autoTextGrid atg = tg;
        if (saveTextGrid) {
            praat_new (atg.transfer(), my name, L"_trimmed");
        }
		praat_new (thee.transfer(), my name, L"_trimmed");
	}
END

FORM (Sound_to_BarkFilter, L"Sound: To BarkFilter", L"Sound: To BarkFilter...")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (bark)", L"1.0")
	POSITIVE (L"Distance between filters (bark)", L"1.0")
	REAL (L"Maximum frequency (bark)", L"0");
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_BarkFilter (me, GET_REAL (L"Window length"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters")), my name);
	}
END

FORM (Sound_to_FormantFilter, L"Sound: To FormantFilter", L"Sound: To FormantFilter...")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (Hz)", L"100.0")
	POSITIVE (L"Distance between filters (Hz)", L"50.0")
	REAL (L"Maximum frequency", L"0");
	POSITIVE (L"Relative bandwidth", L"1.1")
	LABEL (L"", L"Pitch analysis")
	REAL (L"Minimum pitch (Hz)", L"75.0")
	REAL (L"Maximum pitch (Hz)", L"600.0")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_FormantFilter (me, GET_REAL (L"Window length"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters"),
		GET_REAL (L"Relative bandwidth"), GET_REAL (L"Minimum pitch"),
		GET_REAL (L"Maximum pitch")), my name);
	}
END

FORM (Sound_to_MelFilter, L"Sound: To MelFilter", L"Sound: To MelFilter...")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (mel)", L"100.0")
	POSITIVE (L"Distance between filters (mel)", L"100.0")
	REAL (L"Maximum frequency (mel)", L"0.0");
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_MelFilter (me, GET_REAL (L"Window length"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters")), my name);
	}
END

FORM (Sound_to_Pitch_shs, L"Sound: To Pitch (shs)", L"Sound: To Pitch (shs)...")
	POSITIVE (L"Time step (s)", L"0.01")
	POSITIVE (L"Minimum pitch (Hz)", L"50.0")
	NATURAL (L"Max. number of candidates (Hz)", L"15")
	LABEL (L"", L"Algorithm parameters")
	POSITIVE (L"Maximum frequency component (Hz)", L"1250.0")
	NATURAL (L"Max. number of subharmonics", L"15")
	POSITIVE (L"Compression factor (<=1)", L"0.84")
	POSITIVE (L"Ceiling (Hz)", L"600.0")
	NATURAL (L"Number of points per octave", L"48");
	OK
DO
	double minimumPitch = GET_REAL (L"Minimum pitch");
	double fmax = GET_REAL (L"Maximum frequency component");
	double ceiling = GET_REAL (L"Ceiling");
	if (minimumPitch >= ceiling) {
		Melder_throw ("Minimum pitch should be smaller than ceiling.");
	}
	if (ceiling >= fmax) {
		Melder_throw ("Maximum frequency must be greater than or equal to ceiling.");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Pitch_shs (me, GET_REAL (L"Time step"), minimumPitch, fmax, ceiling,
		GET_INTEGER (L"Max. number of subharmonics"), GET_INTEGER (L"Max. number of candidates"),
		GET_REAL (L"Compression factor"), GET_INTEGER (L"Number of points per octave")), my name);
	}
END

FORM (Sound_fadeIn, L"Sound: Fade in", L"Sound: Fade in...")
	CHANNEL (L"Channel (number, 0 = (all))", L"1")
	REAL (L"Time (s)", L"-10000.0")
	REAL (L"Fade time (s)", L"0.005")
	BOOLEAN (L"Silent from start", 0)
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (Sound);
		Sound_fade (me, channel, GET_REAL (L"Time"), GET_REAL (L"Fade time"), -1, GET_INTEGER (L"Silent from start"));
		praat_dataChanged (me);
	}
END

FORM (Sound_fadeOut, L"Sound: Fade out", L"Sound: Fade out...")
	CHANNEL (L"Channel (number, 0 = (all))", L"1")
	REAL (L"Time (s)", L"10000.0")
	REAL (L"Fade time (s)", L"-0.005")
	BOOLEAN (L"Silent to end", 0)
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (Sound);
		Sound_fade (me, channel, GET_REAL (L"Time"), GET_REAL (L"Fade time"), 1, GET_INTEGER (L"Silent to end"));
		praat_dataChanged (me);
	}
END

FORM (Sound_to_KlattGrid_simple, L"Sound: To KlattGrid (simple)", L"Sound: To KlattGrid (simple)...")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Formant determination")
	NATURAL (L"Max. number of formants", L"5")
	POSITIVE (L"Maximum formant (Hz)", L"5500 (=adult female)")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50.0")
	LABEL (L"", L"Pitch determination")
	POSITIVE (L"Pitch floor (Hz)", L"60.0")
	POSITIVE (L"Pitch ceiling (Hz)", L"600.0")
	LABEL (L"", L"Intensity determination")
	POSITIVE (L"Minimum pitch (Hz)", L"100.0")
	BOOLEAN (L"Subtract mean", 1)
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_KlattGrid_simple (me, GET_REAL (L"Time step"),
		GET_INTEGER (L"Max. number of formants"), GET_REAL (L"Maximum formant"),
		GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis from"),
		GET_REAL (L"Pitch floor"), GET_REAL (L"Pitch ceiling"),
		GET_REAL (L"Minimum pitch"), GET_INTEGER (L"Subtract mean")), my name);
	}
END

FORM (Sound_to_Pitch_SPINET, L"Sound: To SPINET", L"Sound: To SPINET...")
	POSITIVE (L"Time step (s)", L"0.005")
	POSITIVE (L"Window length (s)", L"0.040")
	LABEL (L"", L"Gammatone filter bank")
	POSITIVE (L"Minimum filter frequency (Hz)", L"70.0")
	POSITIVE (L"Maximum filter frequency (Hz)", L"5000.0")
	NATURAL (L"Number of filters", L"250");
	POSITIVE (L"Ceiling (Hz)", L"500.0")
	NATURAL (L"Max. number of candidates", L"15")
	OK
DO
	double fmin = GET_REAL (L"Minimum filter frequency");
	double fmax = GET_REAL (L"Maximum filter frequency");
	if (fmax <= fmin) {
		Melder_throw ("Maximum frequency must be larger than minimum frequency.");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Pitch_SPINET (me, GET_REAL (L"Time step"), GET_REAL (L"Window length"),
		fmin, fmax, GET_INTEGER (L"Number of filters"),
		GET_REAL (L"Ceiling"), GET_INTEGER (L"Max. number of candidates")), my name);
	}
END

FORM (Sound_to_Polygon, L"Sound: To Polygon", L"Sound: To Polygon...")
	CHANNEL (L"Channel (number, Left, or Right)", L"1")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	REAL (L"Connection y-value", L"0.0")
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (Sound);
		if (channel > my ny) {
			channel = 1;
		}
		praat_new (Sound_to_Polygon (me, channel, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_REAL (L"Connection y-value")), my name);
	}
END

FORM (Sounds_to_Polygon_enclosed, L"Sounds: To Polygon (enclosed)", L"Sounds: To Polygon (enclosed)...")
	CHANNEL (L"Channel (number, Left, or Right)", L"1")
	OPTION (L"Left")
	OPTION (L"Right")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	Sound s1 = 0, s2 = 0;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	praat_new (Sounds_to_Polygon_enclosed (s1, s2, channel, GET_REAL (L"left Time range"),
		GET_REAL (L"right Time range"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range")), s1->name, L"_", s2->name);
END

FORM (Sound_filterByGammaToneFilter4, L"Sound: Filter (gammatone)", L"Sound: Filter (gammatone)...")
	POSITIVE (L"Centre frequency (Hz)", L"1000.0")
	POSITIVE (L"Bandwidth (Hz)", L"150.0")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_filterByGammaToneFilter4 (me, GET_REAL (L"Centre frequency"), GET_REAL (L"Bandwidth")), my name, L"_filtered");
	}
END

FORM (Sound_removeNoise, L"Sound: Remove noise", L"Sound: Remove noise...")
	REAL (L"left Noise time range (s)", L"0.0")
	REAL (L"right Noise time range (s)", L"0.0")
	POSITIVE (L"Window length (s)", L"0.025")
	LABEL (L"", L"Filter")
	REAL (L"left Filter frequency range (Hz)", L"80.0")
	REAL (L"right Filter frequency range (Hz)", L"10000.0")
	POSITIVE (L"Smoothing (Hz)", L"40.0")
	OPTIONMENU (L"Noise reduction method", 1)
	OPTION (L"Spectral subtraction")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_removeNoise (me, GET_REAL (L"left Noise time range"), GET_REAL (L"right Noise time range"),
			GET_REAL (L"Window length"), GET_REAL (L"left Filter frequency range"),
			GET_REAL (L"right Filter frequency range"), GET_REAL (L"Smoothing"), GET_INTEGER (L"Noise reduction method"));
		praat_new (thee.transfer(), my name, L"_denoised");
	}
END

FORM (Sound_changeSpeaker, L"Sound: Change speaker", L"Sound: Change speaker...")
	LABEL (L"", L"Pitch measurement parameters")
	POSITIVE (L"Pitch floor (Hz)", L"75.0")
	POSITIVE (L"Pitch ceiling (Hz)", L"600.0")
	LABEL (L"", L"Modification parameters")
	POSITIVE (L"Multiply formants by", L"1.2")
	POSITIVE (L"Multiply pitch by", L"1.0")
	REAL (L"Multiply pitch range by", L"1.0 (=no change)")
	POSITIVE (L"Multiply duration by", L"1.0")
	OK
DO
	double minimumPitch = GET_REAL (L"Pitch floor");
	double maximumPitch = GET_REAL (L"Pitch ceiling");
	if (minimumPitch >= maximumPitch) {
		Melder_throw ("Maximum pitch should be greater than minimum pitch.");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_changeSpeaker (me, minimumPitch, maximumPitch,
		GET_REAL (L"Multiply formants by"), GET_REAL (L"Multiply pitch by"),
		GET_REAL (L"Multiply pitch range by"), GET_REAL (L"Multiply duration by")), my name, L"_changeSpeaker");
	}
END

FORM (Sound_changeGender, L"Sound: Change gender", L"Sound: Change gender...")
	LABEL (L"", L"Pitch measurement parameters")
	POSITIVE (L"Pitch floor (Hz)", L"75.0")
	POSITIVE (L"Pitch ceiling (Hz)", L"600.0")
	LABEL (L"", L"Modification parameters")
	POSITIVE (L"Formant shift ratio", L"1.2")
	REAL (L"New pitch median (Hz)", L"0.0 (=no change)")
	REAL (L"Pitch range factor", L"1.0 (=no change)")
	POSITIVE (L"Duration factor", L"1.0")
	OK
DO
	double minimumPitch = GET_REAL (L"Pitch floor");
	double maximumPitch = GET_REAL (L"Pitch ceiling");
	double pitchrf = GET_REAL (L"Pitch range factor");
	if (minimumPitch >= maximumPitch) {
		Melder_throw ("Maximum pitch should be greater than minimum pitch.");
	}
	if (pitchrf < 0) {
		Melder_throw ("Pitch range factor may not be negative");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_changeGender_old (me, minimumPitch, maximumPitch,
		GET_REAL (L"Formant shift ratio"), GET_REAL (L"New pitch median"),
		pitchrf, GET_REAL (L"Duration factor")), my name, L"_changeGender");
	}
END

FORM (Sound_paintWhere, L"Sound paint where", L"Sound: Paint where...")
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	REAL (L"Fill from level", L"0.0")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"Paint only those parts where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; always")
	OK
DO
	long numberOfBisections = 10;
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_paintWhere (me, GRAPHICS, GET_COLOUR (L"Colour"), GET_REAL (L"left Time range"),
		GET_REAL (L"right Time range"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Fill from level"), GET_INTEGER (L"Garnish"), numberOfBisections, GET_STRING (L"Formula"),
		interpreter);
	}
END

FORM (Sounds_paintEnclosed, L"Sounds paint enclosed", L"Sounds: Paint enclosed...")
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Sound s1 = 0, s2 = 0;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	autoPraatPicture picture;
	Sounds_paintEnclosed (s1, s2, GRAPHICS, GET_COLOUR (L"Colour"),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
END

FORM_READ (Sound_readFromRawFileLE, L"Read Sound from raw Little Endian file", 0, true)
	praat_new (Sound_readFromRawFile (file, NULL, 16, 1, 0, 0, 16000), MelderFile_name (file));
END

FORM_READ (Sound_readFromRawFileBE, L"Read Sound from raw 16-bit Little Endian file", 0, true)
	praat_new (Sound_readFromRawFile (file, NULL, 16, 0, 0, 0, 16000), MelderFile_name (file));
END

FORM_READ (KlattTable_readFromRawTextFile, L"KlattTable_readFromRawTextFile", 0, true)
	praat_new (KlattTable_readFromRawTextFile (file), MelderFile_name (file));
END

/************ Spectrograms *********************************************/

FORM (Spectrograms_to_DTW, L"Spectrograms: To DTW", 0)
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	Spectrogram s1 = 0, s2 = 0;
	LOOP {
		iam (Spectrogram);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	praat_new (Spectrograms_to_DTW (s1, s2, begin, end, slope, 1), s1->name, L"_", s2->name);
END

/**************** Spectrum *******************************************/

FORM (Spectrum_drawPhases, L"Spectrum: Draw phases", L"Spectrum: Draw phases...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0")
	REAL (L"Minimum phase (dB/Hz)", L"0.0 (= auto)")
	REAL (L"Maximum phase (dB/Hz)", L"0.0 (= auto)")
	BOOLEAN (L"Unwrap", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Spectrum);
		Spectrum_drawPhases (me, GRAPHICS, GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		GET_REAL (L"Minimum phase"), GET_REAL (L"Maximum phase"),
		GET_INTEGER (L"Unwrap"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Spectrum_conjugate)
	LOOP {
		iam (Spectrum);
		Spectrum_conjugate (me);
		praat_dataChanged (me);
	}
END

FORM (Spectrum_shiftFrequencies, L"Spectrum: Shift frequencies", L"Spectrum: Shift frequencies...")
	REAL (L"Shift by (Hz)", L"1000.0")
	POSITIVE (L"New maximum frequency (Hz)", L"22050")
	NATURAL (L"Precision", L"50")
	OK
DO
	double shiftBy = GET_REAL (L"Shift by");
	double newMaximumFrequency = GET_REAL (L"New maximum frequency");
	long precision = GET_INTEGER (L"Precision");
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_shiftFrequencies (me, shiftBy, newMaximumFrequency, precision);
		praat_new (thee.transfer(), my name, (shiftBy < 0 ? L"_m" : L"_"), Melder_integer (shiftBy));
	}
END

DIRECT (Spectra_multiply)
	Spectrum s1 = 0, s2 = 0;
	LOOP {
		iam (Spectrum);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	praat_new (Spectra_multiply (s1, s2), Thing_getName (s1), L"_x_", Thing_getName (s2));
END

FORM (Spectrum_resample, L"Spectrum: Resample", 0)
	NATURAL (L"New number of frequencies", L"256")
	OK
DO
	long numberOfFrequencies = GET_INTEGER (L"New number of frequencies");
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_resample (me, numberOfFrequencies);
		praat_new (thee.transfer(), my name, L"_", Melder_integer (numberOfFrequencies));
	}
END

FORM (Spectrum_compressFrequencyDomain, L"Spectrum: Compress frequency domain", 0)
	POSITIVE (L"Maximum frequency (Hz)", L"5000.0")
	INTEGER (L"Interpolation depth", L"50")
	RADIO (L"Interpolation scale", 1)
	RADIOBUTTON (L"Linear")
	RADIOBUTTON (L"Logarithmic")
	OK
DO
	double maximumFrequency = GET_REAL (L"Maximum frequency");
	long interpolationDepth = GET_INTEGER (L"Interpolation depth");
	int freqScale = GET_INTEGER (L"Interpolation scale");
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_compressFrequencyDomain (me, maximumFrequency, interpolationDepth, freqScale, 1);
		praat_new (thee.transfer(), my name, L"_", Melder_integer (maximumFrequency));
	}
END

DIRECT (Spectrum_unwrap)
	LOOP {
		iam (Spectrum);
		praat_new (Spectrum_unwrap (me), my name);
	}
END

DIRECT (Spectrum_to_PowerCepstrum)
	LOOP {
		iam (Spectrum);
		praat_new (Spectrum_to_PowerCepstrum (me), my name);
	}
END

DIRECT (Spectrum_to_Cepstrum)
	LOOP {
		iam (Spectrum);
		praat_new (Spectrum_to_Cepstrum (me), my name);
	}
END

/************* SpeechSynthesizer *************************************************/

DIRECT (SpeechSynthesizer_help)
	Melder_help (L"SpeechSynthesizer");
END

FORM (SpeechSynthesizer_create, L"Create SpeechSynthesizer", L"Create SpeechSynthesizer...")
	long prefVoice = Strings_findString (espeakdata_voices_names, L"English");
	if (prefVoice == 0) {
		prefVoice = 1;
	}
	LIST (L"Language", espeakdata_voices_names -> numberOfStrings, (const wchar_t **) espeakdata_voices_names -> strings, prefVoice)
	long prefVariant = Strings_findString (espeakdata_variants_names, L"default");
	LIST (L"Voice variant", espeakdata_variants_names -> numberOfStrings,
		(const wchar_t **) espeakdata_variants_names -> strings, prefVariant)
	OK
DO
	long voiceIndex = GET_INTEGER (L"Language");
	long variantIndex = GET_INTEGER (L"Voice variant"); // default is not in the list!
	autoSpeechSynthesizer me = SpeechSynthesizer_create (espeakdata_voices_names -> strings[voiceIndex],
		espeakdata_variants_names -> strings[variantIndex]);
    praat_new (me.transfer(),  espeakdata_voices_names -> strings[voiceIndex], L"_",
        espeakdata_variants_names -> strings[variantIndex]);
END

FORM (SpeechSynthesizer_playText, L"SpeechSynthesizer: Play text", L"SpeechSynthesizer: Play text...")
	TEXTFIELD (L"Text", L"This is some text.")
	OK
DO
	const wchar_t *text = GET_STRING (L"Text");
	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_playText (me, text);
	}
END

FORM (SpeechSynthesizer_to_Sound, L"SpeechSynthesizer: To Sound", L"SpeechSynthesizer: To Sound...")
	TEXTFIELD (L"Text", L"This is some text.")
	BOOLEAN (L"Create TextGrid with annotations", 0);
	OK
DO
	const wchar_t *text = GET_STRING (L"Text");
	bool createTextGrid = GET_INTEGER (L"Create TextGrid with annotations");
	LOOP {
		iam (SpeechSynthesizer);
		TextGrid tg = 0; Table t = 0;
		autoSound thee = SpeechSynthesizer_to_Sound (me, text, (createTextGrid ? &tg : NULL), (Melder_debug == -2 ? &t : NULL));
		autoTextGrid atg = tg; autoTable atr = t;
		praat_new (thee.transfer(), my name);
		if (createTextGrid) {
			praat_new (atg.transfer(), my name);
		}
		if (Melder_debug == -2) {
			praat_new (atr.transfer(), my name);
		}
	}
END

DIRECT (SpeechSynthesizer_getVoiceName)
	LOOP {
		iam (SpeechSynthesizer);
		Melder_information (my d_voiceLanguageName);
	}
END

DIRECT (SpeechSynthesizer_getVoiceVariant)
	LOOP {
		iam (SpeechSynthesizer);
		Melder_information (my d_voiceVariantName);
	}
END

FORM (SpeechSynthesizer_setTextInputSettings, L"SpeechSynthesizer: Set text input settings", L"SpeechSynthesizer: Set text input settings...")
	OPTIONMENU (L"Input text format is", 1)
	OPTION (L"Text only")
	OPTION (L"Phoneme codes only")
	OPTION (L"Mixed with tags")
	OPTIONMENU (L"Input phoneme codes are", 1)
	OPTION (L"Kirshenbaum_espeak")
	OK
DO
	int inputTextFormat = GET_INTEGER (L"Input text format is");
	int inputPhonemeCoding = SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM; //
	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_setTextInputSettings (me, inputTextFormat, inputPhonemeCoding);
	}
END

FORM (SpeechSynthesizer_setSpeechOutputSettings, L"SpeechSynthesizer: Set speech output settings", L"SpeechSynthesizer: Set speech output settings...")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	REAL (L"Gap between words (s)", L"0.01")
	INTEGER (L"Pitch adjustment (0-99)", L"50")
	INTEGER (L"Pitch range (0-99)", L"50");
	NATURAL (L"Words per minute (80-450)", L"175");
	BOOLEAN (L"Estimate rate from data", 1);
	OPTIONMENU (L"Output phoneme codes are", 2)
	OPTION (L"Kirshenbaum_espeak")
	OPTION (L"IPA")
	OK
DO
	double samplingFrequency = GET_REAL (L"Sampling frequency");
	double wordgap = GET_REAL (L"Gap between words");
	if (wordgap < 0) wordgap = 0;
	double pitchAdjustment = GET_INTEGER (L"Pitch adjustment");
	if (pitchAdjustment < 0) pitchAdjustment = 0;
	if (pitchAdjustment > 99) pitchAdjustment = 99;
	double pitchRange = GET_INTEGER (L"Pitch range");
	if (pitchRange < 0) pitchRange = 0;
	if (pitchRange > 99) pitchRange = 99;
	double wordsPerMinute = GET_INTEGER (L"Words per minute");
	bool estimateWordsPerMinute = GET_INTEGER (L"Estimate rate from data");
	int outputPhonemeCodes = GET_INTEGER (L"Output phoneme codes are");

	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_setSpeechOutputSettings (me, samplingFrequency, wordgap, pitchAdjustment, pitchRange, wordsPerMinute, estimateWordsPerMinute, outputPhonemeCodes);
	}
END

/************* SpeechSynthesizer and TextGrid ************************/

FORM (SpeechSynthesizer_and_TextGrid_to_Sound, L"SpeechSynthesizer & TextGrid: To Sound", 0)
	NATURAL (L"Tier number", L"1")
	NATURAL (L"Interval number", L"1")
	BOOLEAN (L"Create TextGrid with annotations", 0);
	OK
DO
	bool createTextGrid = GET_INTEGER (L"Create TextGrid with annotations");
	SpeechSynthesizer me = FIRST (SpeechSynthesizer);
	TextGrid thee = FIRST (TextGrid), tg = 0;
	autoSound him = SpeechSynthesizer_and_TextGrid_to_Sound (me, thee, GET_INTEGER (L"Tier number"),
		GET_INTEGER (L"Interval number"), (createTextGrid ? &tg : NULL));
	autoTextGrid atg = tg;
	praat_new (him.transfer(), my name);
	if (createTextGrid) {
		praat_new (atg.transfer(), my name);
	}
END

FORM (SpeechSynthesizer_and_Sound_and_TextGrid_align, L"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align)", 0)
	NATURAL (L"Tier number", L"1")
	NATURAL (L"From interval number", L"1")
	NATURAL (L"To interval number", L"1")
	REAL (L"Silence threshold (dB)", L"-35.0")
	POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
	POSITIVE (L"Minimum sounding interval duration (s)", L"0.1")
	OK
DO
	double silenceThreshold = GET_REAL (L"Silence threshold");
	double minSilenceDuration = GET_REAL (L"Minimum silent interval duration");
	double minSoundingDuration = GET_REAL (L"Minimum sounding interval duration");
	SpeechSynthesizer synth = FIRST (SpeechSynthesizer);
	Sound s = FIRST (Sound);
	TextGrid tg = FIRST (TextGrid);
	autoTextGrid thee = SpeechSynthesizer_and_Sound_and_TextGrid_align (synth, s, tg,
		GET_INTEGER (L"Tier number"), GET_INTEGER (L"From interval number"),
		GET_INTEGER (L"To interval number"), silenceThreshold, minSilenceDuration, minSoundingDuration);
	praat_new (thee.transfer(), s -> name, L"_aligned");
END

FORM (SpeechSynthesizer_and_Sound_and_TextGrid_align2, L"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align, trim)", 0)
    NATURAL (L"Tier number", L"1")
    NATURAL (L"From interval number", L"1")
    NATURAL (L"To interval number", L"1")
    REAL (L"Silence threshold (dB)", L"-35.0")
    POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
    POSITIVE (L"Minimum sounding interval duration (s)", L"0.1")
    REAL (L"Silence trim duration (s)", L"0.08")
    OK
DO
    double silenceThreshold = GET_REAL (L"Silence threshold");
    double minSilenceDuration = GET_REAL (L"Minimum silent interval duration");
    double minSoundingDuration = GET_REAL (L"Minimum sounding interval duration");
    double trimDuration = GET_REAL (L"Silence trim duration");
    if (trimDuration < 0) {
        trimDuration = 0;
    }
    SpeechSynthesizer synth = FIRST (SpeechSynthesizer);
    Sound s = FIRST (Sound);
    TextGrid tg = FIRST (TextGrid);
    autoTextGrid thee = SpeechSynthesizer_and_Sound_and_TextGrid_align2 (synth, s, tg,
        GET_INTEGER (L"Tier number"), GET_INTEGER (L"From interval number"),
        GET_INTEGER (L"To interval number"), silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration);
    praat_new (thee.transfer(), s -> name, L"_aligned");
END

/************* Spline *************************************************/

FORM (Spline_drawKnots, L"Spline: Draw knots", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Spline);
		Spline_drawKnots (me, GRAPHICS, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Spline_getOrder)
	LOOP {
		iam (Spline);
		Melder_information (Melder_integer (Spline_getOrder (me)));
	}
END

FORM (Spline_scaleX, L"Spline: Scale x", L"Spline: Scale x...")
	LABEL (L"", L"New domain")
	REAL (L"Xmin", L"-1.0")
	REAL (L"Xmax", L"1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	if (xmin >= xmax) {
		Melder_throw ("Xmin must be smaller than Xmax.");
	}
	LOOP {
		iam (Spline);
		praat_new (Spline_scaleX (me, xmin, xmax), my name, L"_scaleX");
	}
END

/************ SSCP ***************************************************/

DIRECT (SSCP_help) Melder_help (L"SSCP"); END

FORM (SSCP_drawConfidenceEllipse, L"SSCP: Draw confidence ellipse", 0)
	POSITIVE (L"Confidence level", L"0.95")
	NATURAL (L"Index for X-axis", L"1")
	NATURAL (L"Index for Y-axis", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (SSCP);
		SSCP_drawConcentrationEllipse (me, GRAPHICS, GET_REAL (L"Confidence level"), 1,
		GET_INTEGER (L"Index for X-axis"), GET_INTEGER (L"Index for Y-axis"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (SSCP_drawSigmaEllipse, L"SSCP: Draw sigma ellipse", L"SSCP: Draw sigma ellipse...")
	POSITIVE (L"Number of sigmas", L"1.0")
	NATURAL (L"Index for X-axis", L"1")
	NATURAL (L"Index for Y-axis", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (SSCP);
		SSCP_drawConcentrationEllipse (me, GRAPHICS, GET_REAL (L"Number of sigmas"), 0,
		GET_INTEGER (L"Index for X-axis"), GET_INTEGER (L"Index for Y-axis"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (SSCP_extractCentroid)
	LOOP {
		iam (SSCP);
		praat_new (SSCP_extractCentroid (me), my name, L"_centroid");
	}
END

FORM (SSCP_getConfidenceEllipseArea, L"SSCP: Get confidence ellipse area", L"SSCP: Get confidence ellipse area...")
	POSITIVE (L"Confidence level", L"0.95")
	NATURAL (L"Index for X-axis", L"1")
	NATURAL (L"Index for Y-axis", L"2")
	OK
DO
	double conf = GET_REAL (L"Confidence level");
	long d1 = GET_INTEGER (L"Index for X-axis");
	long d2 = GET_INTEGER (L"Index for Y-axis");
	LOOP {
		iam (SSCP);
		Melder_information (Melder_double (SSCP_getConcentrationEllipseArea (me, conf, 1, d1, d2)));
	}
END

FORM (SSCP_getFractionVariation, L"SSCP: Get fraction variation", L"SSCP: Get fraction variation...")
	NATURAL (L"From dimension", L"1")
	NATURAL (L"To dimension", L"1")
	OK
DO
	LOOP {
		iam (SSCP);
		Melder_information (Melder_double (SSCP_getFractionVariation (me,
		GET_INTEGER (L"From dimension"), GET_INTEGER (L"To dimension"))));
	}
END


FORM (SSCP_getConcentrationEllipseArea, L"SSCP: Get sigma ellipse area", L"SSCP: Get sigma ellipse area...")
	POSITIVE (L"Number of sigmas", L"1.0")
	NATURAL (L"Index for X-axis", L"1")
	NATURAL (L"Index for Y-axis", L"2")
	OK
DO
	double nsigmas = GET_REAL (L"Number of sigmas");
	long d1 = GET_INTEGER (L"Index for X-axis");
	long d2 = GET_INTEGER (L"Index for Y-axis");
	LOOP {
		iam (SSCP);
		Melder_information (Melder_double (SSCP_getConcentrationEllipseArea (me, nsigmas, 0, d1, d2)));
}
END

DIRECT (SSCP_getDegreesOfFreedom)
	LOOP {
		iam (SSCP);
		Melder_information (Melder_double (SSCP_getDegreesOfFreedom (me)));
	}
END

DIRECT (SSCP_getNumberOfObservations)
	LOOP {
		iam (SSCP);
		Melder_information (Melder_integer (my numberOfObservations));
	}
END

DIRECT (SSCP_getTotalVariance)
	LOOP {
		iam (SSCP);
		Melder_information (Melder_double (SSCP_getTotalVariance (me)));
	}
END

FORM (SSCP_getCentroidElement, L"SSCP: Get centroid element", L"SSCP: Get centroid element")
	NATURAL (L"Number", L"1")
	OK
DO
	long number = GET_INTEGER (L"Number");
	LOOP {
		iam (SSCP);
		if (number < 1 || number > my numberOfColumns) {
			Melder_throw ("\"Number\" must be smaller than ", my numberOfColumns + 1, ".");
		}
		Melder_information (Melder_double (my centroid[number]));
	}
END

DIRECT (SSCP_getLnDeterminant)
	LOOP {
		iam (SSCP);
		Melder_information (Melder_double (SSCP_getLnDeterminant (me)));
	}
END

FORM (SSCP_testDiagonality_bartlett, L"SSCP: Get diagonality (bartlett)", L"SSCP: Get diagonality (bartlett)...")
	NATURAL (L"Number of contraints", L"1")
	OK
DO
	double chisq, p;
	long nc = GET_INTEGER (L"Number of contraints");
	LOOP {
		iam (SSCP);
		SSCP_testDiagonality_bartlett (me, nc, &chisq, &p);
		Melder_information (Melder_double (p), L" (=probability for chisq = ", Melder_double (chisq),
		L" and ndf = ", Melder_integer (my numberOfRows * (my numberOfRows - 1) / 2), L")");
	}
END

DIRECT (SSCP_to_Correlation)
	LOOP {
		iam (SSCP);
		praat_new (SSCP_to_Correlation (me), 0);
	}
END

FORM (SSCP_to_Covariance, L"SSCP: To Covariance", L"SSCP: To Covariance...")
	NATURAL (L"Number of constraints", L"1")
	OK
DO
	long noc = GET_INTEGER (L"Number of constraints");
	LOOP {
		iam (SSCP);
		praat_new (SSCP_to_Covariance (me, noc), 0);
	}
END

DIRECT (SSCP_to_PCA)
	LOOP {
		iam (SSCP);
		praat_new (SSCP_to_PCA (me), 0);
	}
END

/******************* Strings ****************************/
DIRECT (Strings_createFromEspeakVoices)
	praat_new (NULL, L"voices");
END

FORM (Strings_createAsCharacters, L"Strings: Create as characters", 0)
	SENTENCE (L"Text", L"intention")
	OK
DO
	praat_new (Strings_createAsCharacters (GET_STRING (L"Text")), 0);
END

FORM (Strings_createAsTokens, L"Strings: Create as tokens", 0)
	SENTENCE (L"Text", L"There are seven tokens in this text")
	OK
DO
	praat_new (Strings_createAsTokens (GET_STRING (L"Text")), 0);
END

DIRECT (Strings_append)
	autoCollection set = praat_getSelectedObjects ();
	praat_new (Strings_append (set.transfer()), L"appended");
END

DIRECT (Strings_to_Categories)
	LOOP {
		iam (Strings);
		praat_new (Strings_to_Categories (me), 0);
	}
END

FORM (Strings_change, L"Strings: Change", L"Strings: Change")
	SENTENCE (L"Search", L"a")
	SENTENCE (L"Replace", L"a")
	INTEGER (L"Replace limit", L"0 (=unlimited)")
	RADIO (L"Search and replace are:", 1)
	RADIOBUTTON (L"Literals")
	RADIOBUTTON (L"Regular Expressions")
	OK
DO
	long nmatches, nstringmatches;
	LOOP {
		iam (Strings);
		praat_new (Strings_change (me, GET_STRING (L"Search"), GET_STRING (L"Replace"),
		GET_INTEGER (L"Replace limit"), &nmatches, &nstringmatches, GET_INTEGER (L"Search and replace are") - 1), 0);
	}
END

FORM (Strings_extractPart, L"Strings: Extract part", L"")
	NATURAL (L"From index", L"1")
	NATURAL (L"To index", L"1")
	OK
DO
	LOOP {
		iam (Strings);
		praat_new (Strings_extractPart (me, GET_INTEGER (L"From index"), GET_INTEGER (L"To index")), my name, L"_part");
	}
END

DIRECT (Strings_to_EditDistanceTable)
	Strings s1 = NULL, s2 = NULL;
	LOOP {
		iam(Strings);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 != NULL && s2 != NULL);
	autoEditDistanceTable table = EditDistanceTable_create (s1, s2);
	praat_new (table.transfer(), s1 -> name, L"_", s2 -> name);
END

FORM (Strings_to_Permutation, L"Strings: To Permutation", L"Strings: To Permutation...")
	BOOLEAN (L"Sort", 1)
	OK
DO
	LOOP {
		iam (Strings);
		praat_new (Strings_to_Permutation (me, GET_INTEGER (L"Sort")), my name);
	}
END

DIRECT (Strings_and_Permutation_permuteStrings)
	Strings me = FIRST (Strings);
	Permutation p = FIRST (Permutation);
	praat_new (Strings_and_Permutation_permuteStrings (me, p), my name, L"_", p->name);
END

FORM (SVD_to_TableOfReal, L"SVD: To TableOfReal", L"SVD: To TableOfReal...")
	NATURAL (L"First component", L"1")
	INTEGER (L"Last component", L"0 (=all)")
	OK
DO
	LOOP {
		iam (SVD);
		praat_new (SVD_to_TableOfReal (me, GET_INTEGER (L"First component"), GET_INTEGER (L"Last component")), my name);
	}
END

DIRECT (SVD_extractLeftSingularVectors)
	LOOP {
		iam (SVD);
		praat_new (SVD_extractLeftSingularVectors (me), Thing_getName (me), L"_lsv");
	}
END

DIRECT (SVD_extractRightSingularVectors)
	LOOP {
		iam (SVD);
		praat_new (SVD_extractRightSingularVectors (me), Thing_getName (me), L"_rsv");
	}
END

DIRECT (SVD_extractSingularValues)
	LOOP {
		iam (SVD);
		praat_new (SVD_extractSingularValues (me), Thing_getName (me), L"_sv");
	}
END

/******************* Table ****************************/

DIRECT (Table_createFromPetersonBarneyData)
	praat_new (Table_createFromPetersonBarneyData (), L"pb");
END

DIRECT (Table_createFromPolsVanNieropData)
	praat_new (Table_createFromPolsVanNieropData (), L"pvn");
END

DIRECT (Table_createFromWeeninkData)
	praat_new (Table_createFromWeeninkData (), L"m10w10c10");
END

FORM (Table_scatterPlotWhere, L"Table: Scatter plot where", 0)
	WORD (L"Horizontal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	WORD (L"Column with marks", L"")
	NATURAL (L"Font size", L"12")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"Use only data from rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		long markColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with marks"));
		autoTable thee = Table_extractRowsWhere (me,  GET_STRING (L"Formula"), interpreter);
		Table_scatterPlot (thee.peek(), GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			markColumn, GET_INTEGER (L"Font size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_scatterPlotMarkWhere, L"Scatter plot where (marks)", 0)
	WORD (L"Horizontal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	POSITIVE (L"Mark size (mm)", L"1.0")
	BOOLEAN (L"Garnish", 1)
	SENTENCE (L"Mark string (+xo.)", L"+")
	LABEL (L"", L"Use only data from rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		autoTable thee = Table_extractRowsWhere (me,  GET_STRING (L"Formula"), interpreter);
		Table_scatterPlot_mark (thee.peek(), GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_barPlotWhere, L"Table: Bar plot where", L"Table: Bar plot where...")
	SENTENCE (L"Vertical column(s)", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	SENTENCE (L"Column with labels", L"")
	LABEL (L"", L"Distances are in units of 'bar width'")
	REAL (L"Distance of first bar from border", L"1.0")
	REAL (L"Distance between bar groups", L"1.0")
	REAL (L"Distance between bars within group", L"0.0")
	SENTENCE (L"Colours", L"Grey")
	REAL (L"Label text angle (degrees)", L"0.0");
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"Use only data from rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"row >= 1 and row <= 8")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		Table_barPlotWhere (me, GRAPHICS, GET_STRING (L"Vertical column"), GET_REAL (L"left Vertical range"), 
			GET_REAL (L"right Vertical range"), GET_STRING (L"Column with labels"),
			GET_REAL (L"Distance of first bar from border"), GET_REAL (L"Distance between bars within group"), GET_REAL (L"Distance between bar groups"),
			GET_STRING (L"Colours"),GET_REAL (L"Label text angle"), GET_INTEGER (L"Garnish"), GET_STRING (L"Formula"), interpreter);
	}
END

FORM (Table_LineGraphWhere, L"Table: Line graph where", L"Table: Line graph where...")
	SENTENCE (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	SENTENCE (L"Horizonal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Text", L"+")
	REAL (L"Label text angle (degrees)", L"0.0");
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"Use only data from rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; (= everything)")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		long xcolumn = Table_findColumnIndexFromColumnLabel (me, GET_STRING (L"Horizonal column"));
		Table_lineGraphWhere (me, GRAPHICS, xcolumn, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			ycolumn, GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), 
			GET_STRING (L"Text"), GET_REAL (L"Label text angle"),
		  	GET_INTEGER (L"Garnish"), GET_STRING (L"Formula"), interpreter);
	}
END

FORM (Table_boxPlots, L"Table: Box plots", 0)
	WORD (L"Data column", L"")
	WORD (L"Factor column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	double ymin = GET_REAL (L"left Vertical range");
	double ymax = GET_REAL (L"right Vertical range");
	int garnish = GET_INTEGER (L"Garnish");
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Data column"));
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Factor column"));
		Table_boxPlots (me, GRAPHICS, dataColumn, factorColumn, ymin, ymax, garnish);
	}
END

FORM (Table_boxPlotsWhere, L"Table: Box plots where", 0)
	WORD (L"Data column", L"")
	WORD (L"Factor column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1);
	LABEL (L"", L"Use only data in rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	double ymin = GET_REAL (L"left Vertical range");
	double ymax = GET_REAL (L"right Vertical range");
	int garnish = GET_INTEGER (L"Garnish");
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Data column"));
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Factor column"));
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (L"Formula"), interpreter);
		Table_boxPlots (thee.peek(), GRAPHICS, dataColumn, factorColumn, ymin, ymax, garnish);
	}
END

FORM (Table_drawEllipseWhere, L"Draw ellipse (standard deviation)", 0)
	WORD (L"Horizontal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	POSITIVE (L"Number of sigmas", L"2.0")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"Use only data in rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (L"Formula"), interpreter);
		Table_drawEllipse_e (thee.peek(), GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Number of sigmas"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_normalProbabilityPlot, L"Table: Normal probability plot", L"Table: Normal probability plot...")
	WORD (L"Column", L"")
	NATURAL (L"Number of quantiles", L"100")
	REAL (L"Number of sigmas", L"0.0")
	NATURAL (L"Label size", L"12")
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column"));
		Table_normalProbabilityPlot (me, GRAPHICS, column,
			GET_INTEGER (L"Number of quantiles"), GET_REAL (L"Number of sigmas"),
			GET_INTEGER (L"Label size"), GET_STRING (L"Label"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_normalProbabilityPlotWhere, L"Table: Normal probability plot where", L"Table: Normal probability plot...")
	WORD (L"Column", L"")
	NATURAL (L"Number of quantiles", L"100")
	REAL (L"Number of sigmas", L"0.0")
	NATURAL (L"Label size", L"12")
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1);
	LABEL (L"", L"Use only data in rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column"));
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (L"Formula"), interpreter);
		Table_normalProbabilityPlot (thee.peek(), GRAPHICS, column,
			GET_INTEGER (L"Number of quantiles"), GET_REAL (L"Number of sigmas"),
			GET_INTEGER (L"Label size"), GET_STRING (L"Label"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_quantileQuantilePlot, L"Table: Quantile-quantile plot", L"Table: Quantile-quantile plot...")
	WORD (L"Horizontal axis column", L"")
	WORD (L"Vertical axis column", L"")
	NATURAL (L"Number of quantiles", L"100")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	NATURAL (L"Label size", L"12")
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal axis column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical axis column"));
		Table_quantileQuantilePlot (me, GRAPHICS, xcolumn, ycolumn, GET_INTEGER (L"Number of quantiles"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_STRING (L"Label"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_quantileQuantilePlot_betweenLevels, L"Table: Quantile-quantile plot (between levels)", L"Table: Quantile-quantile plot...")
	WORD (L"Data column", L"")
	WORD (L"Factor column", L"")
	WORD (L"Horizontal factor level", L"")
	WORD (L"Vertical factor level", L"")
	NATURAL (L"Number of quantiles", L"100")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	NATURAL (L"Label size", L"12")
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Data column"));
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Factor column"));
		wchar_t *xLevel = GET_STRING (L"Horizontal factor level");
		wchar_t *yLevel = GET_STRING (L"Vertical factor level");
		Table_quantileQuantilePlot_betweenLevels (me, GRAPHICS, dataColumn, factorColumn, xLevel, yLevel,
			GET_INTEGER (L"Number of quantiles"), GET_REAL (L"left Horizontal range"),
			GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
			GET_REAL (L"right Vertical range"), GET_INTEGER (L"Label size"), GET_STRING (L"Label"),
			GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_distributionPlot, L"Table: Distribution plot", 0)
	WORD (L"Data column", L"data")
	REAL (L"Minimum value", L"0.0")
	REAL (L"Maximum value", L"0.0")
	LABEL (L"", L"Display of the distribution")
	NATURAL (L"Number of bins", L"10")
	REAL (L"Minimum frequency", L"0.0")
	REAL (L"Maximum frequency", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Data column"));
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"),
			GET_INTEGER (L"Number of bins"), GET_REAL (L"Minimum frequency"), GET_REAL (L"Maximum frequency"), GET_INTEGER (L"Garnish"),
			L"1", interpreter);
	}
END

FORM (Table_distributionPlotWhere, L"Table: Distribution plot where", 0)
	WORD (L"Data column", L"data")
	REAL (L"Minimum value", L"0.0")
	REAL (L"Maximum value", L"0.0")
	LABEL (L"", L"Display of the distribution")
	NATURAL (L"Number of bins", L"10")
	REAL (L"Minimum frequency", L"0.0")
	REAL (L"Maximum frequency", L"0.0")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"Use only data in rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Data column"));
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"),
			GET_INTEGER (L"Number of bins"), GET_REAL (L"Minimum frequency"), GET_REAL (L"Maximum frequency"), GET_INTEGER (L"Garnish"),
			GET_STRING (L"Formula"), interpreter);
	}
END

FORM (Table_scatterPlotWithConfidenceIntervals, L"Table: Scatter plot (confidence intervals)", L"")
	NATURAL (L"Horizontal axis column", L"1")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	INTEGER (L"left Horizontal confidence interval column", L"3")
	INTEGER (L"right Horizontal confidence interval column", L"4")
	NATURAL (L"Vertical axis column", L"2")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	INTEGER (L"left Vertical confidence interval column", L"5")
	INTEGER (L"right Vertical confidence interval column", L"6")
	REAL (L"Bar size (mm)", L"1.0")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		Table_scatterPlotWithConfidenceIntervals (me, GRAPHICS,
		GET_INTEGER (L"Horizontal axis column"), GET_INTEGER (L"Vertical axis column"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"left Horizontal confidence interval column"), GET_INTEGER (L"right Horizontal confidence interval column"),
		GET_INTEGER (L"left Vertical confidence interval column"), GET_INTEGER (L"right Vertical confidence interval column"),
		GET_REAL (L"Bar size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_extractRowsWhere, L"Table: Extract rows where", 0)
	LABEL (L"", L"Extractm rows where the following condition holds:")
	TEXTFIELD (L"Formula", L"1; self$[\"gender\"]=\"male\"")
	OK
DO
	LOOP {
		iam (Table);
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (L"Formula"), interpreter);
		praat_new (thee.transfer(), my name, L"_formula");
	}
END

/******************* TableOfReal ****************************/

DIRECT (New_CreateIrisDataset)
	praat_new (TableOfReal_createIrisDataset (), 0);
END

FORM (TableOfReal_reportMultivariateNormality, L"TableOfReal: Report multivariate normality (BHEP)", L"TableOfReal: Report multivariate normality (BHEP)...")
	REAL (L"Smoothing parameter", L"0.0")
	OK
DO
	double h = GET_REAL (L"Smoothing parameter");
	MelderInfo_open ();
	LOOP {
		iam (TableOfReal);
		double tnb, lnmu, lnvar;
		double prob = TableOfReal_normalityTest_BHEP (me, &h, &tnb, &lnmu, &lnvar);
		MelderInfo_open ();
		MelderInfo_writeLine (L"Baringhaus–Henze–Epps–Pulley normality test:");
		MelderInfo_writeLine (L"Significance of normality: ", Melder_double (prob));
		MelderInfo_writeLine (L"BHEP statistic: ", Melder_double (tnb));
		MelderInfo_writeLine (L"Lognormal mean: ", Melder_double (lnmu));
		MelderInfo_writeLine (L"Lognormal variance: ", Melder_double (lnvar));
		MelderInfo_writeLine (L"Smoothing: ", Melder_double (h));
		MelderInfo_writeLine (L"Sample size: ", Melder_integer (my numberOfRows));
		MelderInfo_writeLine (L"Number of variables: ", Melder_integer (my numberOfColumns));
	}
	MelderInfo_close ();
END

DIRECT (TableOfReal_and_Permutation_permuteRows)
	TableOfReal me = FIRST (TableOfReal);
	Permutation p = FIRST (Permutation);
	praat_new (TableOfReal_and_Permutation_permuteRows (me, p),
		my name, L"_", p->name);
END

DIRECT (TableOfReal_to_Permutation_sortRowlabels)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Permutation_sortRowLabels (me), my name);
	}
END

DIRECT (TableOfReal_appendColumns)
	autoCollection set = praat_getSelectedObjects ();
	praat_new (TableOfReal_appendColumnsMany (set.peek()), L"columns_appended");
END

FORM (TableOfReal_createFromPolsData_50males, L"Create TableOfReal (Pols 1973)", L"Create TableOfReal (Pols 1973)...")
	BOOLEAN (L"Include formant levels", 0)
	OK
DO
	praat_new (TableOfReal_createFromPolsData_50males (GET_INTEGER (L"Include formant levels")), L"pols_50males");
END

DIRECT (Table_createFromEspositoData)
	praat_new (Table_createFromEspositoData (), L"h1_h2");
END

DIRECT (Table_createFromGanongData)
	praat_new (Table_createFromGanongData (), L"ganong");
END

FORM (TableOfReal_createFromVanNieropData_25females, L"Create TableOfReal (Van Nierop 1973)...", L"Create TableOfReal (Van Nierop 1973)...")
	BOOLEAN (L"Include formant levels", 0)
	OK
DO
	praat_new (TableOfReal_createFromVanNieropData_25females (GET_INTEGER (L"Include formant levels")), L"vannierop_25females");
END

FORM (TableOfReal_createFromWeeninkData, L"Create TableOfReal (Weenink 1985)...", L"Create TableOfReal (Weenink 1985)...")
	RADIO (L"Speakers group", 1)
	RADIOBUTTON (L"Men")
	RADIOBUTTON (L"Women")
	RADIOBUTTON (L"Children")
	OK
DO
	int type = GET_INTEGER (L"Speakers group");
	praat_new (TableOfReal_createFromWeeninkData (type),
		(type == 1 ? L"m10" : type == 2 ? L"w10" : L"c10"));
END

FORM (TableOfReal_drawScatterPlot, L"TableOfReal: Draw scatter plot", L"TableOfReal: Draw scatter plot...")
	LABEL (L"", L"Select the part of the table")
	NATURAL (L"Horizontal axis column number", L"1")
	NATURAL (L"Vertical axis column number", L"2")
	INTEGER (L"left Row number range", L"0")
	INTEGER (L"right Row number range", L"0")
	LABEL (L"", L"Select the drawing area limits")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	NATURAL (L"Label size", L"12")
	BOOLEAN (L"Use row labels", 0)
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		TableOfReal_drawScatterPlot (me, GRAPHICS,
		GET_INTEGER (L"Horizontal axis column number"),
		GET_INTEGER (L"Vertical axis column number"),
		GET_INTEGER (L"left Row number range"), GET_INTEGER (L"right Row number range"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Use row labels"),
		GET_STRING (L"Label"), GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_drawScatterPlotMatrix, L"TableOfReal: Draw scatter plots matrix", 0)
	INTEGER (L"From column", L"0")
	INTEGER (L"To column", L"0")
	POSITIVE (L"Fraction white", L"0.1")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawScatterPlotMatrix (me, GRAPHICS, GET_INTEGER (L"From column"), GET_INTEGER (L"To column"),
		GET_REAL (L"Fraction white"));
	}
END

FORM (TableOfReal_drawBiplot, L"TableOfReal: Draw biplot", L"TableOfReal: Draw biplot...")
	LABEL (L"", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	REAL (L"Split factor", L"0.5")
	INTEGER (L"Label size", L"10")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawBiplot (me, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_REAL (L"Split factor"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_drawVectors, L"Draw vectors", L"TableOfReal: Draw vectors...")
	LABEL (L"", L"From (x1, y1) to (x2, y2)")
	NATURAL (L"left From columns (x1, y1)", L"1")
	NATURAL (L"right From columns (x1, y1)", L"2")
	NATURAL (L"left To columns (x2, y2)", L"3")
	NATURAL (L"right To columns (x2, y2)", L"4")
	LABEL (L"", L"Select the drawing area")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	RADIO (L"Vector type", 1)
	RADIOBUTTON (L"Arrow")
	RADIOBUTTON (L"Double arrow")
	RADIOBUTTON (L"Line")
	INTEGER (L"Label size", L"10")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawVectors (me, GRAPHICS,
		GET_INTEGER (L"left From columns"), GET_INTEGER (L"right From columns"),
		GET_INTEGER (L"left To columns"), GET_INTEGER (L"right To columns"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Vector type"), GET_INTEGER (L"Label size"),
		GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_drawRowAsHistogram, L"Draw row as histogram", L"TableOfReal: Draw rows as histogram...")
	LABEL (L"", L"Select from the table")
	WORD (L"Row number", L"1")
	INTEGER (L"left Column range", L"0")
	INTEGER (L"right Column range", L"0")
	LABEL (L"", L"Vertical drawing range")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	LABEL (L"", L"Offset and distance in units of 'bar width'")
	REAL (L"Horizontal offset", L"0.5")
	REAL (L"Distance between bars", L"1.0")
	WORD (L"Grey value (1=white)", L"0.7")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, GET_STRING (L"Row number"),
		GET_INTEGER (L"left Column range"), GET_INTEGER (L"right Column range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Horizontal offset"), 0,
		GET_REAL (L"Distance between bars"), GET_STRING (L"Grey value"),
		GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_drawRowsAsHistogram, L"Draw rows as histogram", L"TableOfReal: Draw rows as histogram...")
	LABEL (L"", L"Select from the table")
	SENTENCE (L"Row numbers", L"1 2")
	INTEGER (L"left Column range", L"0")
	INTEGER (L"right Column range", L"0")
	LABEL (L"", L"Vertical drawing range")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	LABEL (L"", L"Offset and distance in units of 'bar width'")
	REAL (L"Horizontal offset", L"1.0")
	REAL (L"Distance between bar groups", L"1.0")
	REAL (L"Distance between bars", L"0.0")
	SENTENCE (L"Grey values (1=white)", L"1 1")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS,
		GET_STRING (L"Row numbers"),
		GET_INTEGER (L"left Column range"), GET_INTEGER (L"right Column range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Horizontal offset"), GET_REAL (L"Distance between bars"),
		GET_REAL (L"Distance between bar groups"), GET_STRING (L"Grey values"),
		GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_drawBoxPlots, L"TableOfReal: Draw box plots", L"TableOfReal: Draw box plots...")
	INTEGER (L"From row", L"0")
	INTEGER (L"To row", L"0")
	INTEGER (L"From column", L"0")
	INTEGER (L"To column", L"0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawBoxPlots (me, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
		GET_INTEGER (L"From column"), GET_INTEGER (L"To column"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_drawColumnAsDistribution, L"TableOfReal: Draw column as distribution", L"TableOfReal: Draw column as distribution...")
	NATURAL (L"Column number", L"1")
	REAL (L"left Value range", L"0.0")
	REAL (L"right Value range", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right frequency range", L"0.0")
	NATURAL (L"Number of bins", L"10")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawColumnAsDistribution (me, GRAPHICS, GET_INTEGER (L"Column number"),
		GET_REAL (L"left Value range"), GET_REAL (L"right Value range"), GET_INTEGER (L"Number of bins"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right frequency range"), 0, GET_INTEGER (L"Garnish"));
	}
END

FORM (TableOfReal_to_Configuration_lda, L"TableOfReal: To Configuration (lda)", L"TableOfReal: To Configuration (lda)...")
	INTEGER (L"Number of dimensions", L"0 (= all)")
	OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	if (dimension < 0) {
		Melder_throw ("Number of dimensions must be greater equal zero.");
	}
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Configuration_lda (me, dimension), my name, L"_lda");
	}
END

FORM (TableOfReal_to_CCA, L"TableOfReal: To CCA", L"TableOfReal: To CCA...")
	NATURAL (L"Dimension of dependent variate", L"2")
	OK
DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_CCA (me, GET_INTEGER (L"Dimension of dependent variate")), my name);
	}
END

FORM (TableOfReal_to_Configuration_pca, L"TableOfReal: To Configuration (pca)", L"TableOfReal: To Configuration (pca)...")
	NATURAL (L"Number of dimensions", L"2")
	OK
DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Configuration_pca (me, GET_INTEGER (L"Number of dimensions")), my name, L"_pca");
	}
END

DIRECT (TableOfReal_to_Discriminant)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Discriminant (me), my name);
	}
END

DIRECT (TableOfReal_to_PCA)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_PCA (me), my name);
	}
END

FORM (TableOfReal_to_SSCP, L"TableOfReal: To SSCP", L"TableOfReal: To SSCP...")
	INTEGER (L"Begin row", L"0")
	INTEGER (L"End row", L"0")
	INTEGER (L"Begin column", L"0")
	INTEGER (L"End column", L"0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_SSCP (me, GET_INTEGER (L"Begin row"), GET_INTEGER (L"End row"),
		GET_INTEGER (L"Begin column"), GET_INTEGER (L"End column")), my name);
	}
END

/* For the inheritors */
DIRECT (TableOfReal_to_TableOfReal)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_TableOfReal (me), my name);
	}
END

DIRECT (TableOfReal_to_Correlation)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Correlation (me), my name);
	}
END

DIRECT (TableOfReal_to_Correlation_rank)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Correlation_rank (me), my name);
	}
END

DIRECT (TableOfReal_to_Covariance)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Covariance (me), my name);
	}
END

DIRECT (TableOfReal_to_SVD)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_SVD (me), my name);
	}
END

DIRECT (TablesOfReal_to_Eigen_gsvd)
	TableOfReal t1 = 0, t2 = 0;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 && t2);
	praat_new (TablesOfReal_to_Eigen_gsvd (t1, t2), 0);
END

FORM (TableOfReal_and_TableOfReal_crossCorrelations, L"TableOfReal & TableOfReal: Cross-correlations", 0)
	OPTIONMENU (L"Correlations between", 1)
	OPTION (L"Rows")
	OPTION (L"Columns")
	BOOLEAN (L"Center", 0)
	BOOLEAN (L"Normalize", 0)
	OK
DO
	int by_columns = GET_INTEGER (L"Correlations between") - 1;
	TableOfReal t1 = 0, t2 = 0;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 && t2);
	praat_new (TableOfReal_and_TableOfReal_crossCorrelations (t1, t2, by_columns,
			GET_INTEGER (L"Center"), GET_INTEGER (L"Normalize")), (by_columns ? L"by_columns" : L"by_rows"), L"cc");
END

DIRECT (TablesOfReal_to_GSVD)
	TableOfReal t1 = 0, t2 = 0;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 && t2);
	praat_new (TablesOfReal_to_GSVD (t1, t2), 0);
END

FORM (TableOfReal_choleskyDecomposition, L"TableOfReal: Cholesky decomposition", 0)
	BOOLEAN (L"Upper (else L)", 0)
	BOOLEAN (L"Inverse", 0)
	OK
	DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_choleskyDecomposition (me, GET_INTEGER (L"Upper"), GET_INTEGER (L"Inverse")), my name);
	}
END

FORM (TableOfReal_to_Pattern_and_Categories, L"TableOfReal: To Pattern and Categories", L"TableOfReal: To Pattern and Categories...")
	INTEGER (L"left Row range", L"0")
	INTEGER (L"right Row range", L"0 (=all)")
	INTEGER (L"left Column range", L"0")
	INTEGER (L"right Column range", L"0 (=all)")
	OK
	DO
	LOOP {
		iam (TableOfReal);
		Pattern p = 0; Categories c = 0;
		TableOfReal_to_Pattern_and_Categories (me, GET_INTEGER (L"left Row range"),
		GET_INTEGER (L"right Row range"), GET_INTEGER (L"left Column range"),
		GET_INTEGER (L"right Column range"), &p, &c);
		autoPattern ap = p; autoCategories ac = c;
		praat_new (ap.transfer(), Thing_getName (me));
		praat_new (ac.transfer(), Thing_getName (me));
	}
END

FORM (TableOfReal_getColumnSum, L"TableOfReal: Get column sum", L"")
	INTEGER (L"Column", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_double (TableOfReal_getColumnSum (me, GET_INTEGER (L"Column"))));
	}
END

FORM (TableOfReal_getRowSum, L"TableOfReal: Get row sum", L"")
	INTEGER (L"Row", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_double (TableOfReal_getRowSum (me, GET_INTEGER (L"Row"))));
	}
END

DIRECT (TableOfReal_getGrandSum)
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_double (TableOfReal_getGrandSum (me)));
	}
END

FORM (TableOfReal_meansByRowLabels, L"TableOfReal: Means by row labels", L"TableOfReal: To TableOfReal (means by row labels)...")
	BOOLEAN (L"Expand", 0)
	OK
	DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_meansByRowLabels (me, GET_INTEGER (L"Expand"), 0), NAME, L"_byrowlabels");
	}
END

FORM (TableOfReal_mediansByRowLabels, L"TableOfReal: Medians by row labels", L"TableOfReal: To TableOfReal (medians by row labels)...")
	BOOLEAN (L"Expand", 0)
	OK
DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_meansByRowLabels (me, GET_INTEGER (L"Expand"), 1), Thing_getName (me), L"_byrowlabels");
	}
END

/***** TableOfReal and FilterBank  *****/

FORM (TextGrid_extendTime, L"TextGrid: Extend time", L"TextGrid: Extend time...")
	LABEL (L"", L"")
	POSITIVE (L"Extend domain by (s)", L"1.0")
	RADIO (L"At", 1)
	RADIOBUTTON (L"End")
	RADIOBUTTON (L"Start")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_extendTime (me, GET_REAL (L"Extend domain by"), GET_INTEGER (L"At") - 1);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_replaceIntervalTexts, L"TextGrid: Replace interval text", L"TextGrid: Replace interval text...")
	LABEL (L"", L"")
	NATURAL (L"Tier number", L"1")
	INTEGER (L"left Interval range", L"0")
	INTEGER (L"right Interval range", L"0")
	SENTENCE (L"Search", L"a")
	SENTENCE (L"Replace", L"a")
	RADIO (L"Search and replace strings are:", 1)
	RADIOBUTTON (L"Literals")
	RADIOBUTTON (L"Regular Expressions")
	OK
DO
	long from = GET_INTEGER (L"left Interval range");
	long to = GET_INTEGER (L"right Interval range");
	int isregexp = GET_INTEGER (L"Search and replace strings are") - 1;
	wchar_t *search = GET_STRING (L"Search");
	LOOP {
		iam (TextGrid);
		long nmatches, nstringmatches;
		TextGrid_changeLabels (me, GET_INTEGER (L"Tier number"), from, to, search, GET_STRING (L"Replace"),
		isregexp, &nmatches, &nstringmatches);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_replacePointTexts, L"TextGrid: Replace point text", L"TextGrid: Replace point text...")
	LABEL (L"", L"")
	NATURAL (L"Tier number", L"1")
	INTEGER (L"left Interval range", L"0")
	INTEGER (L"right Interval range", L"0")
	SENTENCE (L"Search", L"a")
	SENTENCE (L"Replace", L"a")
	RADIO (L"Search and replace strings are:", 1)
	RADIOBUTTON (L"Literals")
	RADIOBUTTON (L"Regular Expressions")
	OK
DO
	long from = GET_INTEGER (L"left Interval range");
	long to = GET_INTEGER (L"right Interval range");
	LOOP {
		iam (TextGrid);
		long nmatches, nstringmatches;
		TextGrid_changeLabels (me, GET_INTEGER (L"Tier number"), from, to, GET_STRING (L"Search"), GET_STRING (L"Replace"),
		GET_INTEGER (L"Search and replace strings are") - 1, &nmatches, &nstringmatches);
		praat_dataChanged (me);
	}
END

FORM (TextGrids_to_Table_textAlignmentment, L"TextGrids: To Table (text alignment)", 0)
	NATURAL (L"Target tier", L"1")
	NATURAL (L"Source tier", L"1")
	OK
DO
	TextGrid tg1 = 0, tg2 = 0;
    LOOP {
        iam (TextGrid);
        (tg1 ? tg2 : tg1) = me;
    }
    Melder_assert (tg1 && tg2);
	praat_new (TextGrids_to_Table_textAlignmentment (tg1, GET_INTEGER (L"Target tier"), tg2, GET_INTEGER (L"Source tier"), 0), tg1 -> name, L"_", tg2 -> name);
END

FORM (TextGrids_and_EditCostsTable_to_Table_textAlignmentment, L"TextGrids & EditCostsTable: To Table(text alignmentment)", 0)
	NATURAL (L"Target tier", L"1")
	NATURAL (L"Source tier", L"1")
	OK
DO
	TextGrid tg1 = 0, tg2 = 0; EditCostsTable ect = 0;
    LOOP {
        if (CLASS == classTextGrid) {
        	(tg1 ? tg2 : tg1) = (TextGrid) OBJECT;
		} else {
			ect = (EditCostsTable) OBJECT;
		}
    }
 	Melder_assert (tg1 && tg2 && ect);
	praat_new (TextGrids_to_Table_textAlignmentment (tg1, GET_INTEGER (L"Target tier"), tg2, GET_INTEGER (L"Source tier"), ect), tg1 -> name, L"_", tg2 -> name);
END

FORM (TextGrid_setTierName, L"TextGrid: Set tier name", L"TextGrid: Set tier name...")
	NATURAL (L"Tier number:", L"1")
	SENTENCE (L"Name", L"");
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_setTierName (me, GET_INTEGER (L"Tier number"), GET_STRING (L"Name"));
		praat_dataChanged (me);
	}
END

DIRECT (VowelEditor_create)
	if (theCurrentPraatApplication -> batch) {
		Melder_throw ("Cannot edit from batch.");
	}
	autoVowelEditor vowelEditor = VowelEditor_create (L"VowelEditor", NULL);
	vowelEditor.transfer(); // user becomes the owner
END

static Any cmuAudioFileRecognizer (int nread, const char *header, MelderFile fs) {
	return nread < 12 || header [0] != 6 || header [1] != 0 ?
	       NULL : Sound_readFromCmuAudioFile (fs);
}

void praat_CC_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"Paint...", 0, 1, DO_CC_paint);
	praat_addAction1 (klas, 1, L"Draw...", 0, 1, DO_CC_drawC0);
	praat_addAction1 (klas, 1, QUERY_BUTTON, 0, 0, 0);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, L"Get value...", 0, 1, DO_CC_getValue);
	praat_addAction1 (klas, 0, L"To Matrix", 0, 0, DO_CC_to_Matrix);
	praat_addAction1 (klas, 2, L"To DTW...", 0, 0, DO_CCs_to_DTW);
}

static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm);
static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, L"Project...", 0, 0, DO_Eigen_and_Matrix_project);
}

static void praat_Eigen_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"Get eigenvalue...", 0, 1, DO_Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, L"Get sum of eigenvalues...", 0, 1, DO_Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, L"Get number of eigenvectors", 0, 1, DO_Eigen_getNumberOfEigenvalues);
	praat_addAction1 (klas, 1, L"Get eigenvector dimension", 0, 1, DO_Eigen_getDimension);
	praat_addAction1 (klas, 1, L"Get eigenvector element...", 0, 1, DO_Eigen_getEigenvectorElement);
}

static void praat_Eigen_draw_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, L"Draw eigenvalues...", 0, 1, DO_Eigen_drawEigenvalues);
	praat_addAction1 (klas, 0, L"Draw eigenvalues (scree)...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_Eigen_drawEigenvalues_scree);
	praat_addAction1 (klas, 0, L"Draw eigenvector...", 0, 1, DO_Eigen_drawEigenvector);
}

static void praat_Index_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"Get number of classes", 0, 0, DO_Index_getNumberOfClasses);
	praat_addAction1 (klas, 1, L"To Permutation...", 0, 0, DO_Index_to_Permutation);
	praat_addAction1 (klas, 1, L"Extract part...", 0, 0, DO_Index_extractPart);
}

static void praat_FilterBank_query_init (ClassInfo klas);
static void praat_FilterBank_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
	praat_Matrixft_query_init (klas);
	praat_addAction1 (klas, 0, L"-- frequency scales --", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get frequency in Hertz...", 0, 1, DO_FilterBank_getFrequencyInHertz);
	praat_addAction1 (klas, 1, L"Get frequency in Bark...", 0, 1, DO_FilterBank_getFrequencyInBark);
	praat_addAction1 (klas, 1, L"Get frequency in mel...", 0, 1, DO_FilterBank_getFrequencyInMel);
}

static void praat_FilterBank_modify_init (ClassInfo klas);
static void praat_FilterBank_modify_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, L"Equalize intensities...", 0, 1, DO_FilterBank_equalizeIntensities);
}

static void praat_FilterBank_draw_init (ClassInfo klas);
static void praat_FilterBank_draw_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, L"Draw filters...", 0, 1, DO_FilterBank_drawFilters);
	praat_addAction1 (klas, 0, L"Draw one contour...", 0, 1, DO_FilterBank_drawOneContour);
	praat_addAction1 (klas, 0, L"Draw contours...", 0, 1, DO_FilterBank_drawContours);
	praat_addAction1 (klas, 0, L"Paint image...", 0, 1, DO_FilterBank_paintImage);
	praat_addAction1 (klas, 0, L"Paint contours...", 0, 1, DO_FilterBank_paintContours);
	praat_addAction1 (klas, 0, L"Paint cells...", 0, 1, DO_FilterBank_paintCells);
	praat_addAction1 (klas, 0, L"Paint surface...", 0, 1, DO_FilterBank_paintSurface);
	praat_addAction1 (klas, 0, L"-- frequency scales --", 0, 1, 0);
	praat_addAction1 (klas, 0, L"Draw frequency scales...", 0, 1, DO_FilterBank_drawFrequencyScales);

}

static void praat_FilterBank_all_init (ClassInfo klas);
static void praat_FilterBank_all_init (ClassInfo klas) {
	praat_FilterBank_draw_init (klas);
	praat_FilterBank_query_init (klas);
	praat_FilterBank_modify_init (klas);
	praat_addAction1 (klas, 0, L"To Intensity", 0, 0, DO_FilterBank_to_Intensity);
	praat_addAction1 (klas, 0, L"To Matrix", 0, 0, DO_FilterBank_to_Matrix);
	praat_addAction1 (klas, 2, L"Cross-correlate...", 0, 0, DO_FilterBanks_crossCorrelate);
	praat_addAction1 (klas, 2, L"Convolve...", 0, 0, DO_FilterBanks_convolve);
}

static void praat_FunctionTerms_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, L"Draw...", 0, 1, DO_FunctionTerms_draw);
	praat_addAction1 (klas, 0, L"Draw basis function...", 0, 1, DO_FunctionTerms_drawBasisFunction);
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 1, L"Get number of coefficients", 0, 1, DO_FunctionTerms_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, L"Get coefficient...", 0, 1, DO_FunctionTerms_getCoefficient);
	praat_addAction1 (klas, 1, L"Get degree", 0, 1, DO_FunctionTerms_getDegree);
	praat_addAction1 (klas, 0, L"-- function specifics --", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get value...", 0, 1, DO_FunctionTerms_evaluate);
	praat_addAction1 (klas, 1, L"Get minimum...", 0, 1, DO_FunctionTerms_getMinimum);
	praat_addAction1 (klas, 1, L"Get x of minimum...", 0, 1, DO_FunctionTerms_getXOfMinimum);
	praat_addAction1 (klas, 1, L"Get maximum...", 0, 1, DO_FunctionTerms_getMaximum);
	praat_addAction1 (klas, 1, L"Get x of maximum...", 0, 1, DO_FunctionTerms_getXOfMaximum);
	praat_addAction1 (klas, 0, L"Modify -", 0, 0, 0);
	praat_addAction1 (klas, 1, L"Set domain...", 0, 1, DO_FunctionTerms_setDomain);
	praat_addAction1 (klas, 1, L"Set coefficient...", 0, 1, DO_FunctionTerms_setCoefficient);
	praat_addAction1 (klas, 0, L"Analyse", 0, 0, 0);
}

/* Query buttons for frame-based frequency x time subclasses of matrix. */

void praat_Matrixft_query_init (ClassInfo klas) {
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, L"Get time from column...", 0, 1, DO_Matrixft_getXofColumn);
	praat_addAction1 (klas, 1, L"-- frequencies --", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get lowest frequency", 0, 1, DO_Matrixft_getLowestFrequency);
	praat_addAction1 (klas, 1, L"Get highest frequency", 0, 1, DO_Matrixft_getHighestFrequency);
	praat_addAction1 (klas, 1, L"Get number of frequencies", 0, 1, DO_Matrixft_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, L"Get frequency distance", 0, 1, DO_Matrixft_getFrequencyDistance);
	praat_addAction1 (klas, 1, L"Get frequency from row...", 0, 1, DO_Matrixft_getFrequencyOfRow);
	praat_addAction1 (klas, 1, L"-- get value --", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get value in cell...", 0, 1, DO_Matrixft_getValueInCell);
}

static void praat_Spline_init (ClassInfo klas) {
	praat_FunctionTerms_init (klas);
	praat_addAction1 (klas, 0, L"Draw knots...", L"Draw basis function...", 1, DO_Spline_drawKnots);
	praat_addAction1 (klas, 1, L"Get order", L"Get degree", 1, DO_Spline_getOrder);
	praat_addAction1 (klas, 1, L"Scale x...", L"Analyse",	0, DO_Spline_scaleX);

}

static void praat_SSCP_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"-- statistics --", L"Get value...", 1, 0);
	praat_addAction1 (klas, 1, L"Get number of observations", L"-- statistics --", 1, DO_SSCP_getNumberOfObservations);
	praat_addAction1 (klas, 1, L"Get degrees of freedom", L"Get number of observations", 1, DO_SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, L"Get centroid element...", L"Get degrees of freedom", 1, DO_SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, L"Get ln(determinant)", L"Get centroid element...", 1, DO_SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"Extract centroid", EXTRACT_BUTTON, 1, DO_SSCP_extractCentroid);
}

FORM (SSCP_setValue, L"Covariance: Set value", L"Covariance: Set value...")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	REAL (L"New value", L"1.0")
	OK
DO
	LOOP {
		iam (SSCP);
		SSCP_setValue (me, GET_INTEGER (L"Row number"), GET_INTEGER (L"Column number"), GET_REAL (L"New value"));
	}
END

FORM (SSCP_setCentroid, L"", 0)
	NATURAL (L"Element number", L"1")
	REAL (L"New value", L"1.0")
	OK
DO
	LOOP {
		iam (SSCP);
		SSCP_setCentroid (me, GET_INTEGER (L"Element number"), GET_REAL (L"New value"));
	}
END

void praat_SSCP_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_removeAction (klas, NULL, NULL, L"Set value...");
	praat_addAction1 (klas, 1, L"Set centroid...", L"Formula...", 1, DO_SSCP_setCentroid);
	praat_addAction1 (klas, 1, L"Set value...", L"Formula...", 1, DO_SSCP_setValue);
	praat_addAction1 (klas, 0, L"To TableOfReal", L"To Matrix", 1, DO_TableOfReal_to_TableOfReal);

}

void praat_TableOfReal_init2 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, L"To TableOfReal", L"To Matrix", 1, DO_TableOfReal_to_TableOfReal);
}

void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, L"Set default costs...", L"Formula...", 1, DO_EditDistanceTable_setDefaultCosts);
	praat_removeAction (klas, NULL, NULL, L"Draw as numbers...");
	praat_addAction1 (klas, 0, L"Draw...", L"Draw -", 1, DO_EditDistanceTable_draw);
	praat_addAction1 (klas, 0, L"Draw edit operations", L"Draw...", 1, DO_EditDistanceTable_drawEditOperations);
	praat_removeAction (klas, NULL, NULL, L"Draw as numbers if...");
	praat_removeAction (klas, NULL, NULL, L"Draw as squares...");
	praat_removeAction (klas, NULL, NULL, L"Draw vertical lines...");
	praat_removeAction (klas, NULL, NULL, L"Draw horizontal lines...");
	praat_removeAction (klas, NULL, NULL, L"Draw left and right lines...");
	praat_removeAction (klas, NULL, NULL, L"Draw top and bottom lines...");
	praat_removeAction (klas, NULL, NULL, L"-- draw lines --");
}

void praat_uvafon_David_init ();
void praat_uvafon_David_init () {
	Data_recognizeFileType (TextGrid_TIMITLabelFileRecognizer);
	Data_recognizeFileType (cmuAudioFileRecognizer);

	Thing_recognizeClassesByName (classActivation, classBarkFilter,
		classCategories, classCepstrum, classCCA,
		classChebyshevSeries, classClassificationTable, classConfusion,
		classCorrelation, classCovariance, classDiscriminant, classDTW,
		classEigen, classExcitations, classEditCostsTable, classEditDistanceTable,
		classFileInMemory, classFilesInMemory, classFormantFilter,
		classIndex, classKlattTable,
		classPermutation, classISpline, classLegendreSeries,
		classMelFilter, classMSpline, classPattern, classPCA, classPolynomial, classRoots,
		classSimpleString, classStringsIndex, classSpeechSynthesizer, classSPINET, classSSCP,
		classSVD, NULL);

	VowelEditor_prefs ();

	espeakdata_praat_init ();

	praat_addMenuCommand (L"Objects", L"Technical", L"Report floating point properties", L"Report integer properties", 0, DO_Praat_ReportFloatingPointProperties);
	praat_addMenuCommand (L"Objects", L"Goodies", L"Get TukeyQ...", 0, praat_HIDDEN, DO_Praat_getTukeyQ);
	praat_addMenuCommand (L"Objects", L"Goodies", L"Get invTukeyQ...", 0, praat_HIDDEN, DO_Praat_getInvTukeyQ);
	praat_addMenuCommand (L"Objects", L"New", L"Create Strings from espeak voices", L"Create Strings as directory list...", praat_DEPTH_1 + praat_HIDDEN, DO_Strings_createFromEspeakVoices);
	praat_addMenuCommand (L"Objects", L"New", L"Create iris data set", L"Create TableOfReal...", 1, DO_New_CreateIrisDataset);
	praat_addMenuCommand (L"Objects", L"New", L"Create Permutation...", 0, 0, DO_Permutation_create);
	praat_addMenuCommand (L"Objects", L"New", L"Polynomial", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Create Polynomial...", 0, 1, DO_Polynomial_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create LegendreSeries...", 0, 1, DO_LegendreSeries_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create ChebyshevSeries...", 0, 1, DO_ChebyshevSeries_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create MSpline...", 0, 1, DO_MSpline_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create ISpline...", 0, 1, DO_ISpline_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from gammatone...", L"Create Sound from tone complex...", 1, DO_Sound_createFromGammaTone);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from gamma-tone...", L"Create Sound from tone complex...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_createFromGammaTone);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from Shepard tone...", L"Create Sound from gammatone...", 1, DO_Sound_createFromShepardTone);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from VowelEditor...", L"Create Sound from Shepard tone...", praat_DEPTH_1, DO_VowelEditor_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create SpeechSynthesizer...", L"Create Sound from VowelEditor...", praat_DEPTH_1, DO_SpeechSynthesizer_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create formant table (Pols & Van Nierop 1973)", L"Create Table...", 1, DO_Table_createFromPolsVanNieropData);
	praat_addMenuCommand (L"Objects", L"New", L"Create formant table (Peterson & Barney 1952)", L"Create Table...", 1, DO_Table_createFromPetersonBarneyData);
	praat_addMenuCommand (L"Objects", L"New", L"Create formant table (Weenink 1985)", L"Create formant table (Peterson & Barney 1952)", 1, DO_Table_createFromWeeninkData);
	praat_addMenuCommand (L"Objects", L"New", L"Create H1H2 table (Esposito 2006)", L"Create formant table (Weenink 1985)", praat_DEPTH_1+ praat_HIDDEN, DO_Table_createFromEspositoData);
	praat_addMenuCommand (L"Objects", L"New", L"Create Table (Ganong 1980)", L"Create H1H2 table (Esposito 2006)", praat_DEPTH_1+ praat_HIDDEN, DO_Table_createFromGanongData);
	praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal (Pols 1973)...", L"Create TableOfReal...", 1, DO_TableOfReal_createFromPolsData_50males);
	praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal (Van Nierop 1973)...", L"Create TableOfReal (Pols 1973)...", 1, DO_TableOfReal_createFromVanNieropData_25females);
	praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal (Weenink 1985)...", L"Create TableOfReal (Van Nierop 1973)...", 1, DO_TableOfReal_createFromWeeninkData);
	praat_addMenuCommand (L"Objects", L"New", L"Create simple Confusion...", L"Create TableOfReal (Weenink 1985)...", 1, DO_Confusion_createSimple);
	praat_addMenuCommand (L"Objects", L"New", L"Create simple Covariance...", L"Create simple Confusion...", 1, DO_Covariance_createSimple);
	praat_addMenuCommand (L"Objects", L"New", L"Create empty EditCostsTable...", L"Create simple Covariance...", 1, DO_EditCostsTable_createEmpty);

	praat_addMenuCommand (L"Objects", L"New", L"Create KlattTable example", L"Create TableOfReal (Weenink 1985)...", praat_DEPTH_1 + praat_HIDDEN, DO_KlattTable_createExample);
	praat_addMenuCommand (L"Objects", L"New", L"Create Strings as characters...", L"Create TextGrid...", praat_HIDDEN, DO_Strings_createAsCharacters);
	praat_addMenuCommand (L"Objects", L"New", L"Create Strings as tokens...", L"Create TextGrid...", praat_HIDDEN, DO_Strings_createAsTokens);

	praat_addMenuCommand (L"Objects", L"New", L"Create simple Polygon...", 0, praat_HIDDEN, DO_Polygon_createSimple);
	praat_addMenuCommand (L"Objects", L"New", L"Create Polygon (random vertices)...", 0, praat_HIDDEN, DO_Polygon_createFromRandomVertices);
	praat_addMenuCommand (L"Objects", L"New", L"FilesInMemory", 0, praat_HIDDEN, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create FileInMemory...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_FileInMemory_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create copy from FilesInMemory...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_FilesInMemory_createCopyFromFilesInMemory);
		praat_addMenuCommand (L"Objects", L"New", L"Create FilesInMemory from directory contents...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_FilesInMemory_createFromDirectoryContents);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Sound from raw 16-bit Little Endian file...", L"Read from special sound file", 1,
	                      DO_Sound_readFromRawFileLE);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Sound from raw 16-bit Big Endian file...", L"Read Sound from raw 16-bit Little Endian file...", 1, DO_Sound_readFromRawFileBE);
	praat_addMenuCommand (L"Objects", L"Open", L"Read KlattTable from raw text file...", L"Read Matrix from raw text file...", praat_HIDDEN, DO_KlattTable_readFromRawTextFile);

	praat_addAction1 (classActivation, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classActivation, 0, L"Formula...", 0, 0,
	                  DO_Activation_formula);
	praat_addAction1 (classActivation, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classActivation, 0, L"To Matrix", 0, 0,
	                  DO_Activation_to_Matrix);

	praat_addAction2 (classActivation, 1, classCategories, 1, L"To TableOfReal", 0, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classBarkFilter, 0, L"BarkFilter help", 0, 0, DO_BarkFilter_help);
	praat_FilterBank_all_init (classBarkFilter);
	praat_addAction1 (classBarkFilter, 0, L"Draw spectrum (slice)...", L"Draw filters...", 1, DO_BarkFilter_drawSpectrum);
	praat_addAction1 (classBarkFilter, 1, L"Draw filter functions...", L"Draw filters...", 1, DO_BarkFilter_drawSekeyHansonFilterFunctions);

	praat_addAction1 (classCategories, 0, L"Edit", 0, 0, DO_Categories_edit);
	praat_addAction1 (classCategories, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classCategories, 1, L"Get number of categories", QUERY_BUTTON, 1, DO_Categories_getNumberOfCategories);
	praat_addAction1 (classCategories, 2, L"Get difference", QUERY_BUTTON, praat_HIDDEN | praat_DEPTH_1, DO_Categories_difference);
	praat_addAction1 (classCategories, 2, L"Get number of differences", QUERY_BUTTON, 1, DO_Categories_getNumberOfDifferences);
	praat_addAction1 (classCategories, 2, L"Get fraction different", QUERY_BUTTON, 1, DO_Categories_getFractionDifferent);
	praat_addAction1 (classCategories, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classCategories, 1, L"Append 1 category...", MODIFY_BUTTON,
	                  1, DO_Categories_append);
	praat_addAction1 (classCategories, 0, L"Extract", 0, 0, 0);
	praat_addAction1 (classCategories, 0, L"To unique Categories", 0, 0,
	                  DO_Categories_selectUniqueItems);
	praat_addAction1 (classCategories, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classCategories, 2, L"To Confusion", 0, 0,
	                  DO_Categories_to_Confusion);
	praat_addAction1 (classCategories, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classCategories, 2, L"Join", 0, 0, DO_Categories_join);
	praat_addAction1 (classCategories, 0, L"Permute items", 0, 0, DO_Categories_permuteItems);
	praat_addAction1 (classCategories, 0, L"To Strings", 0, 0,
	                  DO_Categories_to_Strings);

	praat_addAction1 (classChebyshevSeries, 0, L"ChebyshevSeries help", 0, 0,
	                  DO_ChebyshevSeries_help);
	praat_FunctionTerms_init (classChebyshevSeries);
	praat_addAction1 (classChebyshevSeries, 0, L"To Polynomial", L"Analyse",
	                  0, DO_ChebyshevSeries_to_Polynomial);

	praat_addAction1 (classCCA, 1, L"Draw eigenvector...", 0, 0,
	                  DO_CCA_drawEigenvector);
	praat_addAction1 (classCCA, 1, L"Get number of correlations", 0, 0,
	                  DO_CCA_getNumberOfCorrelations);
	praat_addAction1 (classCCA, 1, L"Get correlation...", 0, 0,
	                  DO_CCA_getCorrelationCoefficient);
	praat_addAction1 (classCCA, 1, L"Get eigenvector element...", 0, 0, DO_CCA_getEigenvectorElement);
	praat_addAction1 (classCCA, 1, L"Get zero correlation probability...", 0, 0, DO_CCA_getZeroCorrelationProbability);

	praat_addAction2 (classCCA, 1, classTableOfReal, 1, L"To TableOfReal (scores)...",
	                  0, 0, DO_CCA_and_TableOfReal_scores);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, L"To TableOfReal (loadings)",
	                  0, 0, DO_CCA_and_TableOfReal_factorLoadings);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, L"Predict...", 0, 0,
	                  DO_CCA_and_TableOfReal_predict);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, L"To TableOfReal (loadings)",
	                  0, 0, DO_CCA_and_Correlation_factorLoadings);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, L"Get variance fraction...",
	                  0, 0, DO_CCA_and_Correlation_getVarianceFraction);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, L"Get redundancy (sl)...",
	                  0, 0, DO_CCA_and_Correlation_getRedundancy_sl);

	praat_addAction1 (classConfusion, 0, L"Confusion help", 0, 0,
	                  DO_Confusion_help);
	praat_TableOfReal_init2 (classConfusion);
	praat_removeAction (classConfusion, NULL, NULL, L"Draw as numbers...");
	praat_removeAction (classConfusion, NULL, NULL, L"Sort by label...");
	praat_removeAction (classConfusion, NULL, NULL, L"Sort by column...");
	praat_addAction1 (classConfusion, 0, L"Draw as numbers...", L"Draw -", 1, DO_Confusion_drawAsNumbers);
	praat_addAction1 (classConfusion, 1, L"Get value (labels)...", L"Get value...", 1, DO_Confusion_getValue);
	praat_addAction1 (classConfusion, 0, L"-- confusion statistics --", L"Get value (labels)...", 1, 0);
	praat_addAction1 (classConfusion, 1, L"Get fraction correct", L"-- confusion statistics --", 1, DO_Confusion_getFractionCorrect);
	praat_addAction1 (classConfusion, 1, L"Get stimulus sum...", L"Get fraction correct", 1, DO_Confusion_getStimulusSum);
	praat_addAction1 (classConfusion, 1, L"Get row sum...", L"Get fraction correct", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_getRowSum);
	praat_addAction1 (classConfusion, 1, L"Get response sum...", L"Get stimulus sum...", 1, DO_Confusion_getResponseSum);
	praat_addAction1 (classConfusion, 1, L"Get column sum...", L"Get row sum...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_getColumnSum);
	praat_addAction1 (classConfusion, 1, L"Get grand sum", L"Get response sum...", 1, DO_TableOfReal_getGrandSum);
	praat_addAction1 (classConfusion, 0, L"Increase...", L"Formula...", 1, DO_Confusion_increase);
	praat_addAction1 (classConfusion, 0, L"To TableOfReal (marginals)", L"To TableOfReal", 0, DO_Confusion_to_TableOfReal_marginals);
	praat_addAction1 (classConfusion, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classConfusion, 0, L"Condense...", 0, praat_HIDDEN, DO_Confusion_condense);
	praat_addAction1 (classConfusion, 0, L"Group...", 0, 0, DO_Confusion_group);
	praat_addAction1 (classConfusion, 0, L"Group stimuli...", 0, 0, DO_Confusion_groupStimuli);
	praat_addAction1 (classConfusion, 0, L"Group responses...", 0, 0, DO_Confusion_groupResponses);
	praat_addAction1 (classConfusion, 2, L"To difference matrix", 0, 0,
	                  DO_Confusion_difference);

	praat_addAction2 (classConfusion, 1, classMatrix, 1, L"Draw", 0, 0, 0);
	praat_addAction2 (classConfusion, 1, classMatrix, 1, L"Draw confusion...",
	                  0, 0, DO_Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, L"Covariance help", 0, 0,
	                  DO_Covariance_help);
	praat_SSCP_as_TableOfReal_init (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
	praat_addAction1 (classCovariance, 1, L"Get probability at position...", L"Get value...", 1, DO_Covariance_getProbabilityAtPosition);
	praat_addAction1 (classCovariance, 1, L"Get diagonality (bartlett)...", L"Get ln(determinant)", 1, DO_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classCovariance, 1, L"Get significance of one mean...", L"Get diagonality (bartlett)...", 1, DO_Covariance_getSignificanceOfOneMean);
	praat_addAction1 (classCovariance, 1, L"Get significance of means difference...", L"Get significance of one mean...", 1, DO_Covariance_getSignificanceOfMeansDifference);
	praat_addAction1 (classCovariance, 1, L"Get significance of one variance...", L"Get significance of means difference...", 1, DO_Covariance_getSignificanceOfOneVariance);
	praat_addAction1 (classCovariance, 1, L"Get significance of variances ratio...", L"Get significance of one variance...", 1, DO_Covariance_getSignificanceOfVariancesRatio);
	praat_addAction1 (classCovariance, 1, L"Get fraction variance...", L"Get significance of variances ratio...", 1, DO_Covariance_getFractionVariance);
	praat_addAction1 (classCovariance, 2, L"Report multivariate mean difference...", L"Get fraction variance...", 1, DO_Covariances_reportMultivariateMeanDifference);
	praat_addAction1 (classCovariance, 2, L"Difference", L"Report multivariate mean difference...", praat_DEPTH_1 | praat_HIDDEN, DO_Covariances_reportEquality);
	praat_addAction1 (classCovariance, 0, L"Report equality of covariances", L"Report multivariate mean difference...", praat_DEPTH_1 | praat_HIDDEN, DO_Covariances_reportEquality);

	praat_addAction1 (classCovariance, 0, L"To TableOfReal (random sampling)...", 0, 0, DO_Covariance_to_TableOfReal_randomSampling);

	praat_addAction1 (classCovariance, 0, L"To Correlation", 0, 0, DO_Covariance_to_Correlation);
	praat_addAction1 (classCovariance, 0, L"To PCA", 0, 0, DO_Covariance_to_PCA);

	praat_addAction2 (classCovariance, 1, classTableOfReal, 1, L"To TableOfReal (mahalanobis)...", 0, 0, DO_Covariance_and_TableOfReal_mahalanobis);

	praat_addAction1 (classClassificationTable, 0, L"ClassificationTable help", 0, 0, DO_ClassificationTable_help);
	praat_TableOfReal_init (classClassificationTable);
	praat_addAction1 (classClassificationTable, 0, L"To Confusion", 0, 0, DO_ClassificationTable_to_Confusion);
	praat_addAction1 (classClassificationTable, 0, L"To Correlation (columns)", 0, 0, DO_ClassificationTable_to_Correlation_columns);
	praat_addAction1 (classClassificationTable, 0, L"To Strings (max. prob.)", 0, 0, DO_ClassificationTable_to_Strings_maximumProbability);

	praat_addAction1 (classCorrelation, 0, L"Correlation help", 0, 0, DO_Correlation_help);
	praat_TableOfReal_init2 (classCorrelation);
	praat_SSCP_query_init (classCorrelation);
	praat_SSCP_extract_init (classCorrelation);
	praat_addAction1 (classCorrelation, 1, L"Get diagonality (bartlett)...", L"Get ln(determinant)", 1, DO_Correlation_testDiagonality_bartlett);
	praat_addAction1 (classCorrelation, 0, L"Confidence intervals...", 0, 0, DO_Correlation_confidenceIntervals);
	praat_addAction1 (classCorrelation, 0, L"To PCA", 0, 0, DO_Correlation_to_PCA);

	praat_addAction1 (classDiscriminant, 0, L"Discriminant help", 0, 0, DO_Discriminant_help);
	praat_addAction1 (classDiscriminant, 0, DRAW_BUTTON, 0, 0, 0);
	praat_Eigen_draw_init (classDiscriminant);
	praat_addAction1 (classDiscriminant, 0, L"-- sscps --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 0, L"Draw sigma ellipses...", 0, 1, DO_Discriminant_drawSigmaEllipses);
	praat_addAction1 (classDiscriminant, 0, L"Draw one sigma ellipse...", 0, 1, DO_Discriminant_drawOneSigmaEllipse);
	praat_addAction1 (classDiscriminant, 0, L"Draw confidence ellipses...", 0, 1, DO_Discriminant_drawConfidenceEllipses);

	praat_addAction1 (classDiscriminant, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classDiscriminant, 1, L"-- eigen structure --", 0, 1, 0);
	praat_Eigen_query_init (classDiscriminant);
	praat_addAction1 (classDiscriminant, 1, L"-- discriminant --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, L"Get number of functions", 0, 1, DO_Discriminant_getNumberOfFunctions);
	praat_addAction1 (classDiscriminant, 1, L"Get dimension of functions", 0, 1, DO_Discriminant_getDimensionOfFunctions);
	praat_addAction1 (classDiscriminant, 1, L"Get number of groups", 0, 1, DO_Discriminant_getNumberOfGroups);
	praat_addAction1 (classDiscriminant, 1, L"Get number of observations...", 0, 1, DO_Discriminant_getNumberOfObservations);
	praat_addAction1 (classDiscriminant, 1, L"-- tests --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, L"Get Wilks lambda...", 0, 1, DO_Discriminant_getWilksLambda);
	praat_addAction1 (classDiscriminant, 1, L"Get cumulative contribution of components...", 0, 1, DO_Discriminant_getCumulativeContributionOfComponents);
	praat_addAction1 (classDiscriminant, 1, L"Get partial discrimination probability...", 0, 1,
	                  DO_Discriminant_getPartialDiscriminationProbability);
	praat_addAction1 (classDiscriminant, 1, L"Get homogeneity of covariances (box)", 0, praat_DEPTH_1 | praat_HIDDEN,
	                  DO_Discriminant_getHomegeneityOfCovariances_box);
	praat_addAction1 (classDiscriminant, 1, L"Report equality of covariance matrices", 0, 1,
	                  DO_Discriminant_reportEqualityOfCovariances_wald);
	praat_addAction1 (classDiscriminant, 1, L"-- ellipses --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, L"Get sigma ellipse area...", 0, 1, DO_Discriminant_getConcentrationEllipseArea);
	praat_addAction1 (classDiscriminant, 1, L"Get confidence ellipse area...", 0, 1, DO_Discriminant_getConfidenceEllipseArea);
	praat_addAction1 (classDiscriminant, 1, L"Get ln(determinant_group)...", 0, 1, DO_Discriminant_getLnDeterminant_group);
	praat_addAction1 (classDiscriminant, 1, L"Get ln(determinant_total)", 0, 1, DO_Discriminant_getLnDeterminant_total);

	praat_addAction1 (classDiscriminant, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classDiscriminant, 1, L"Invert eigenvector...", 0, 1, DO_Discriminant_invertEigenvector);
	praat_addAction1 (classDiscriminant, 0, L"Align eigenvectors", 0, 1, DO_Eigens_alignEigenvectors);

	praat_addAction1 (classDiscriminant, 0, EXTRACT_BUTTON, 0, 0, 0);
	praat_addAction1 (classDiscriminant, 1, L"Extract pooled within-groups SSCP", 0, 1,
	                  DO_Discriminant_extractPooledWithinGroupsSSCP);
	praat_addAction1 (classDiscriminant, 1, L"Extract within-group SSCP...", 0, 1, DO_Discriminant_extractWithinGroupSSCP);
	praat_addAction1 (classDiscriminant, 1, L"Extract between-groups SSCP", 0, 1, DO_Discriminant_extractBetweenGroupsSSCP);
	praat_addAction1 (classDiscriminant, 1, L"Extract group centroids", 0, 1, DO_Discriminant_extractGroupCentroids);
	praat_addAction1 (classDiscriminant, 1, L"Extract group standard deviations", 0, 1, DO_Discriminant_extractGroupStandardDeviations);
	praat_addAction1 (classDiscriminant, 1, L"Extract group labels", 0, 1, DO_Discriminant_extractGroupLabels);

	praat_addAction1 (classDiscriminant , 0, L"& TableOfReal: To ClassificationTable?", 0, 0, DO_hint_Discriminant_and_TableOfReal_to_ClassificationTable);

	/*		praat_addAction1 (classDiscriminant, 1, L"Extract coefficients...", 0, 1, DO_Discriminant_extractCoefficients);*/



	praat_Eigen_Matrix_project (classDiscriminant, classFormantFilter);
	praat_Eigen_Matrix_project (classDiscriminant, classBarkFilter);
	praat_Eigen_Matrix_project (classDiscriminant, classMelFilter);

	praat_addAction2 (classDiscriminant, 1, classPattern, 1, L"To Categories...", 0, 0, DO_Discriminant_and_Pattern_to_Categories);
	praat_addAction2 (classDiscriminant, 1, classSSCP, 1, L"Project", 0, 0, DO_Eigen_and_SSCP_project);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, L"Modify Discriminant", 0, 0, 0);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, L"Set group labels", 0, 0, DO_Discriminant_setGroupLabels);

	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, L"To Configuration...", 0, 0, DO_Discriminant_and_TableOfReal_to_Configuration);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, L"To ClassificationTable...", 0, 0,
	                  DO_Discriminant_and_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, L"To TableOfReal (mahalanobis)...", 0, 0, DO_Discriminant_and_TableOfReal_mahalanobis);


	praat_addAction1 (classDTW, 0, L"DTW help", 0, 0, DO_DTW_help);
	praat_addAction1 (classDTW, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classDTW, 0, L"Draw path...", 0, 1, DO_DTW_drawPath);
	praat_addAction1 (classDTW, 0, L"Paint distances...", 0, 1, DO_DTW_paintDistances);
	praat_addAction1 (classDTW, 0, L"Draw warp (x)...", 0, 1, DO_DTW_drawWarpX);
	praat_addAction1 (classDTW, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classDTW, 1, L"Query time domains", 0, 1, 0);
	praat_addAction1 (classDTW, 1, L"Get start time (x)", 0, 2, DO_DTW_getStartTimeX);
	praat_addAction1 (classDTW, 1, L"Get end time (x)", 0, 2, DO_DTW_getEndTimeX);
	praat_addAction1 (classDTW, 1, L"Get total duration (x)", 0, 2, DO_DTW_getTotalDurationX);
	praat_addAction1 (classDTW, 1, L"-- time domain x from y separator --", 0, 2, 0);
	praat_addAction1 (classDTW, 1, L"Get start time (y)", 0, 2, DO_DTW_getStartTimeY);
	praat_addAction1 (classDTW, 1, L"Get end time (y)", 0, 2, DO_DTW_getEndTimeY);
	praat_addAction1 (classDTW, 1, L"Get total duration (y)", 0, 2, DO_DTW_getTotalDurationY);
	praat_addAction1 (classDTW, 1, L"Query time samplings", 0, 1, 0);
	praat_addAction1 (classDTW, 1, L"Get number of frames (x)", 0, 2, DO_DTW_getNumberOfFramesX);
	praat_addAction1 (classDTW, 1, L"Get time step (x)", 0, 2, DO_DTW_getTimeStepX);
	praat_addAction1 (classDTW, 1, L"Get time from frame number (x)...", 0, 2, DO_DTW_getTimeFromFrameNumberX);
	praat_addAction1 (classDTW, 1, L"Get frame number from time (x)...", 0, 2, DO_DTW_getFrameNumberFromTimeX);
	praat_addAction1 (classDTW, 1, L"-- time sampling x from y separator --", 0, 2, 0);
	praat_addAction1 (classDTW, 1, L"Get number of frames (y)", 0, 2, DO_DTW_getNumberOfFramesY);
	praat_addAction1 (classDTW, 1, L"Get time step (y)", 0, 2, DO_DTW_getTimeStepY);
	praat_addAction1 (classDTW, 1, L"Get time from frame number (y)...", 0, 2, DO_DTW_getTimeFromFrameNumberY);
	praat_addAction1 (classDTW, 1, L"Get frame number from time (y)...", 0, 2, DO_DTW_getFrameNumberFromTimeY);

	praat_addAction1 (classDTW, 1, L"Get y time from x time...", 0, 1, DO_DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, L"Get x time from y time...", 0, 1, DO_DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, L"Get y time...", 0, praat_HIDDEN + praat_DEPTH_1, DO_DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, L"Get x time...", 0, praat_HIDDEN + praat_DEPTH_1, DO_DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, L"Get maximum consecutive steps...", 0, 1, DO_DTW_getMaximumConsecutiveSteps);
	praat_addAction1 (classDTW, 1, L"Get time along path...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_DTW_getPathY);
	praat_addAction1 (classDTW, 1, L"-- distance queries --", 0, 1, 0);
	praat_addAction1 (classDTW, 1, L"Get distance value...", 0, 1, DO_DTW_getDistanceValue);
	praat_addAction1 (classDTW, 1, L"Get minimum distance", 0, 1, DO_DTW_getMinimumDistance);
	praat_addAction1 (classDTW, 1, L"Get maximum distance", 0, 1, DO_DTW_getMaximumDistance);
	praat_addAction1 (classDTW, 1, L"Get distance (weighted)", 0, 1, DO_DTW_getWeightedDistance);
	praat_addAction1 (classDTW, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classDTW, 0, L"Formula (distances)...", 0, 1, DO_DTW_formulaDistances);
	praat_addAction1 (classDTW, 0, L"Set distance value...", 0, 1, DO_DTW_setDistanceValue);

	praat_addAction1 (classDTW, 0, L"Analyse", 0, 0, 0);
    praat_addAction1 (classDTW, 0, L"Find path...", 0, praat_HIDDEN, DO_DTW_findPath);
    praat_addAction1 (classDTW, 0, L"Find path (band & slope)...", 0, 0, DO_DTW_findPath_bandAndSlope);
    praat_addAction1 (classDTW, 0, L"To Polygon...", 0, 1, DO_DTW_to_Polygon);
	praat_addAction1 (classDTW, 0, L"To Matrix (distances)", 0, 0, DO_DTW_to_Matrix_distances);
    praat_addAction1 (classDTW, 0, L"To Matrix (cumm. distances)...", 0, 0, DO_DTW_to_Matrix_cummulativeDistances);
	praat_addAction1 (classDTW, 0, L"Swap axes", 0, 0, DO_DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classMatrix, 1, L"Replace matrix", 0, 0, DO_DTW_and_Matrix_replace);
	praat_addAction2 (classDTW, 1, classTextGrid, 1, L"To TextGrid (warp times)", 0, 0, DO_DTW_and_TextGrid_to_TextGrid);
	praat_addAction2 (classDTW, 1, classIntervalTier, 1, L"To Table (distances)", 0, 0, DO_DTW_and_IntervalTier_to_Table);

    praat_addAction2 (classDTW, 1, classPolygon, 1, L"Find path inside...", 0, 0, DO_DTW_and_Polygon_findPathInside);
    praat_addAction2 (classDTW, 1, classPolygon, 1, L"To Matrix (cumm. distances)...", 0, 0, DO_DTW_and_Polygon_to_Matrix_cummulativeDistances);
	praat_addAction2 (classDTW, 1, classSound, 2, L"Draw...", 0, 0, DO_DTW_and_Sounds_draw);
	praat_addAction2 (classDTW, 1, classSound, 2, L"Draw warp (x)...", 0, 0, DO_DTW_and_Sounds_drawWarpX);

	praat_addAction1 (classEditDistanceTable, 1, L"EditDistanceTable help", 0, 0, DO_EditDistanceTable_help);
	praat_EditDistanceTable_as_TableOfReal_init (classEditDistanceTable);
	praat_addAction1 (classEditDistanceTable, 1, L"To TableOfReal (directions)...", 0, praat_HIDDEN, DO_EditDistanceTable_to_TableOfReal_directions);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, L"Set new edit costs", 0, 0, DO_EditDistanceTable_setEditCosts);

	praat_addAction1 (classEditCostsTable, 1, L"EditCostsTable help", 0, 0, DO_EditCostsTable_help);
	praat_addAction1 (classEditCostsTable, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, L"Get target index...", 0, 1, DO_EditCostsTable_getTargetIndex);
	praat_addAction1 (classEditCostsTable, 1, L"Get source index...", 0, 1, DO_EditCostsTable_getSourceIndex);
	praat_addAction1 (classEditCostsTable, 1, L"Get insertion cost...", 0, 1, DO_EditCostsTable_getInsertionCost);
	praat_addAction1 (classEditCostsTable, 1, L"Get deletion cost...", 0, 1, DO_EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, L"Get substitution cost...", 0, 1, DO_EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, L"Get cost (others)...", 0, 1, DO_EditCostsTable_getOthersCost);
	praat_addAction1 (classEditCostsTable, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, L"Set target symbol (index)...", 0, 1, DO_EditCostsTable_setTargetSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, L"Set source symbol (index)...", 0, 1, DO_EditCostsTable_setSourceSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, L"Set insertion costs...", 0, 1, DO_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, L"Set deletion costs...", 0, 1, DO_EditCostsTable_setDeletionCosts);
	praat_addAction1 (classEditCostsTable, 1, L"Set substitution costs...", 0, 1, DO_EditCostsTable_setSubstitutionCosts);
	praat_addAction1 (classEditCostsTable, 1, L"Set costs (others)...", 0, 1, DO_EditCostsTable_setOthersCosts);
	praat_addAction1 (classEditCostsTable, 1, L"To TableOfReal", 0, 0, DO_EditCostsTable_to_TableOfReal);

	praat_Index_init (classStringsIndex);
	praat_addAction1 (classIndex, 0, L"Index help", 0, 0, DO_Index_help);
	praat_addAction1 (classStringsIndex, 1, L"Get class label...", 0, 0, DO_StringsIndex_getClassLabel);
	praat_addAction1 (classStringsIndex, 1, L"Get class index...", 0, 0, DO_StringsIndex_getClassIndex);
	praat_addAction1 (classStringsIndex, 1, L"Get label...", 0, 0, DO_StringsIndex_getLabel);
	praat_addAction1 (classIndex, 1, L"Get index...", 0, 0, DO_Index_getIndex);
	praat_addAction1 (classStringsIndex, 1, L"To Strings", 0, 0, DO_StringsIndex_to_Strings);

	praat_addAction1 (classExcitation, 0, L"Synthesize", L"To Formant...", 0, 0);
	praat_addAction1 (classExcitation, 0, L"To Excitations", L"Synthesize", 0, DO_Excitation_to_Excitations);

	praat_addAction1 (classExcitations, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classExcitations, 0, L"Formula...", 0, 0, DO_Excitations_formula);
	praat_addAction1 (classExcitations, 0, L"Extract", 0, 0, 0);
	praat_addAction1 (classExcitations, 0, L"Extract Excitation...", 0, 0, DO_Excitations_getItem);
	praat_addAction1 (classExcitations, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classExcitations, 0, L"Append", 0, 0, DO_Excitations_append);
	praat_addAction1 (classExcitations, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classExcitations, 0, L"To Pattern...", 0, 0, DO_Excitations_to_Pattern);
	praat_addAction1 (classExcitations, 0, L"To TableOfReal", 0, 0, DO_Excitations_to_TableOfReal);

	praat_addAction2 (classExcitations, 1, classExcitation, 0, L"Add to Excitations", 0, 0, DO_Excitations_addItem);

	praat_addAction1 (classFileInMemory, 1, L"Show as code...", 0, 0, DO_FileInMemory_showAsCode);
	praat_addAction1 (classFileInMemory, 1, L"Set id...", 0, 0, DO_FileInMemory_setId);
	praat_addAction1 (classFileInMemory, 0, L"To FilesInMemory", 0, 0, DO_FileInMemory_to_FilesInMemory);

	praat_addAction1 (classFilesInMemory, 1, L"Show as code...", 0, 0, DO_FilesInMemory_showAsCode);
	praat_addAction1 (classFilesInMemory, 1, L"Show one file as code...", 0, 0, DO_FilesInMemory_showOneFileAsCode);
	praat_addAction1 (classFilesInMemory, 0, L"Merge", 0, 0, DO_FilesInMemory_merge);
	praat_addAction1 (classFilesInMemory, 0, L"To Strings (id)", 0, 0, DO_FilesInMemory_to_Strings_id);

	praat_addAction2 (classFilesInMemory, 1, classFileInMemory, 0, L"Add items to Collection", 0, 0, DO_FilesInMemory_addItems);

	praat_addAction1 (classFormantFilter, 0, L"FormantFilter help", 0, 0, DO_FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, L"Draw spectrum (slice)...", L"Draw filters...", 1, DO_FormantFilter_drawSpectrum);
	praat_addAction1 (classFormantFilter, 0, L"Draw filter functions...", L"Draw filters...", 1, DO_FormantFilter_drawFilterFunctions);
	praat_addAction1 (classFormantGrid, 0, L"Draw...", L"Edit", 1, DO_FormantGrid_draw);


	praat_addAction1 (classIntensity, 0, L"To TextGrid (silences)...", L"To IntensityTier (valleys)", 0, DO_Intensity_to_TextGrid_detectSilences);
	praat_addAction1 (classIntensityTier, 0, L"To TextGrid (silences)...", 0, 0, DO_IntensityTier_to_TextGrid_detectSilences);
	praat_addAction1 (classIntensityTier, 0, L"To Intensity...", 0, praat_HIDDEN, DO_IntensityTier_to_Intensity);

	praat_addAction1 (classISpline, 0, L"ISpline help", 0, 0, DO_ISpline_help);
	praat_Spline_init (classISpline);

	praat_addAction1 (classKlattTable, 0, L"KlattTable help", 0, 0, DO_KlattTable_help);
	praat_addAction1 (classKlattTable, 0, L"To Sound...", 0, 0, DO_KlattTable_to_Sound);
	praat_addAction1 (classKlattTable, 0, L"To KlattGrid...", 0, 0, DO_KlattTable_to_KlattGrid);
	praat_addAction1 (classKlattTable, 0, L"To Table", 0, 0, DO_KlattTable_to_Table);

	praat_addAction1 (classLegendreSeries, 0, L"LegendreSeries help", 0, 0, DO_LegendreSeries_help);
	praat_FunctionTerms_init (classLegendreSeries);
	praat_addAction1 (classLegendreSeries, 0, L"To Polynomial", L"Analyse", 0, DO_LegendreSeries_to_Polynomial);

	praat_addAction1 (classLongSound, 0, L"Append to existing sound file...", 0, 0, DO_LongSounds_appendToExistingSoundFile);
	praat_addAction1 (classSound, 0, L"Append to existing sound file...", 0, 0, DO_LongSounds_appendToExistingSoundFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Append to existing sound file...", 0, 0, DO_LongSounds_appendToExistingSoundFile);

	praat_addAction1 (classLongSound, 2, L"Save as stereo AIFF file...", L"Save as NIST file...", 1, DO_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo AIFF file...", L"Write to NIST file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, L"Save as stereo AIFC file...", L"Save as stereo AIFF file...", 1, DO_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo AIFC file...", L"Write to stereo AIFF file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, L"Save as stereo WAV file...", L"Save as stereo AIFC file...", 1, DO_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo WAV file...", L"Write to stereo AIFC file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, L"Save as stereo NeXt/Sun file...", L"Save as stereo WAV file...", 1, DO_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo NeXt/Sun file...", L"Write to stereo WAV file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, L"Save as stereo NIST file...", L"Save as stereo NeXt/Sun file...", 1, DO_LongSounds_writeToStereoNistFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo NIST file...", L"Write to stereo NeXt/Sun file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoNistFile);

	praat_addAction1 (classLtas, 0, L"Report spectral tilt...", L"Get slope...", 1, DO_Ltas_reportSpectralTilt);

	praat_addAction1 (classMatrix, 0, L"Scatter plot...", L"Paint cells...", 1, DO_Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, L"Draw as squares...", L"Scatter plot...", 1, DO_Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, L"Draw distribution...", L"Draw as squares...", 1, DO_Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, L"Draw cumulative distribution...", L"Draw distribution...", 1, DO_Matrix_drawCumulativeDistribution);
	praat_addAction1 (classMatrix, 0, L"Get mean...", L"Get sum", 1, DO_Matrix_getMean);
	praat_addAction1 (classMatrix, 0, L"Get standard deviation...", L"Get mean...", 1, DO_Matrix_getStandardDeviation);
	praat_addAction1 (classMatrix, 0, L"Transpose", L"Synthesize", 0, DO_Matrix_transpose);
	praat_addAction1 (classMatrix, 0, L"Solve equation...", L"Analyse", 0, DO_Matrix_solveEquation);
	praat_addAction1 (classMatrix, 0, L"To Pattern...", L"To VocalTract", 1, DO_Matrix_to_Pattern);
	praat_addAction1 (classMatrix, 0, L"To Activation", L"To Pattern...", 1, DO_Matrix_to_Activation);
	praat_addAction1 (classMatrix, 2, L"To DTW...", L"To ParamCurve", 1, DO_Matrices_to_DTW);

	praat_addAction2 (classMatrix, 1, classCategories, 1, L"To TableOfReal", 0, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classMelFilter, 0, L"MelFilter help", 0, 0, DO_MelFilter_help);
	praat_FilterBank_all_init (classMelFilter);
	praat_addAction1 (classMelFilter, 0, L"Draw spectrum (slice)...", L"Draw filters...", 1, DO_MelFilter_drawSpectrum);
	praat_addAction1 (classMelFilter, 0, L"Draw filter functions...", L"Draw filters...", 1, DO_MelFilter_drawFilterFunctions);
	praat_addAction1 (classMelFilter, 0, L"To MFCC...", 0, 0, DO_MelFilter_to_MFCC);

	praat_addAction1 (classMFCC, 0, L"MFCC help", 0, 0, DO_MFCC_help);
	praat_CC_init (classMFCC);
	praat_addAction1 (classMFCC, 0, L"To MelFilter...", 0, 0, DO_MFCC_to_MelFilter);
	praat_addAction1 (classMFCC, 0, L"To TableOfReal...", 0, 0, DO_MFCC_to_TableOfReal);
	praat_addAction1 (classMFCC, 0, L"To Matrix (features)...", 0, praat_HIDDEN, DO_MFCC_to_Matrix_features);
	praat_addAction1 (classMFCC, 0, L"To Sound", 0, praat_HIDDEN, DO_MFCC_to_Sound);
	praat_addAction1 (classMFCC, 2, L"Cross-correlate...", 0, 0, DO_MFCCs_crossCorrelate);
	praat_addAction1 (classMFCC, 2, L"Convolve...", 0, 0, DO_MFCCs_convolve);

	praat_addAction1 (classMSpline, 0, L"MSpline help", 0, 0, DO_MSpline_help);
	praat_Spline_init (classMSpline);

	praat_addAction1 (classPattern, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classPattern, 0, L"Draw...", 0, 0, DO_Pattern_draw);
	praat_addAction1 (classPattern, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPattern, 0, L"Formula...", 0, 1, DO_Pattern_formula);
	praat_addAction1 (classPattern, 0, L"Set value...", 0, 1, DO_Pattern_setValue);
	praat_addAction1 (classPattern, 0, L"To Matrix", 0, 0, DO_Pattern_to_Matrix);

	praat_addAction2 (classPattern, 1, classCategories, 1, L"To TableOfReal", 0, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction2 (classPattern, 1, classCategories, 1, L"To Discriminant", 0, 0, DO_Pattern_and_Categories_to_Discriminant);

	praat_addAction1 (classPCA, 0, L"PCA help", 0, 0, DO_PCA_help);
	praat_addAction1 (classPCA, 0, DRAW_BUTTON, 0, 0, 0);
	praat_Eigen_draw_init (classPCA);
	praat_addAction1 (classPCA, 0, QUERY_BUTTON, 0, 0, 0);
	praat_Eigen_query_init (classPCA);
	praat_addAction1 (classPCA, 1, L"-- pca --", 0, 1, 0);
	praat_addAction1 (classPCA, 1, L"Get centroid element...", 0, 1, DO_PCA_getCentroidElement);
	praat_addAction1 (classPCA, 1, L"Get equality of eigenvalues...", 0, 1, DO_PCA_getEqualityOfEigenvalues);
	praat_addAction1 (classPCA, 1, L"Get fraction variance accounted for...", 0, 1, DO_PCA_getFractionVAF);
	praat_addAction1 (classPCA, 1, L"Get number of components (VAF)...", 0, 1, DO_PCA_getNumberOfComponentsVAF);
	praat_addAction1 (classPCA, 2, L"Get angle between pc1-pc2 planes", 0, 1, DO_PCAs_getAngleBetweenPc1Pc2Plane_degrees);
	praat_addAction1 (classPCA, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPCA, 1, L"Invert eigenvector...", 0, 1, DO_PCA_invertEigenvector);
	praat_addAction1 (classPCA, 0, L"Align eigenvectors", 0, 1, DO_Eigens_alignEigenvectors);
	praat_addAction1 (classPCA, 2, L"To Procrustes...", 0, 0, DO_PCAs_to_Procrustes);
	praat_addAction1 (classPCA, 0, L"To TableOfReal (reconstruct 1)...", 0, 0, DO_PCA_to_TableOfReal_reconstruct1);
	praat_addAction1 (classPCA, 0, L"& TableOfReal: To Configuration?", 0, 0, DO_hint_PCA_and_TableOfReal_to_Configuration);
	praat_addAction1 (classPCA, 0, L"& Configuration (reconstruct)?", 0, 0, DO_hint_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction1 (classPCA, 0, L"& Covariance: Project?", 0, 0, DO_hint_PCA_and_Covariance_Project);
	praat_addAction2 (classPCA, 1, classConfiguration, 1, L"To TableOfReal (reconstruct)", 0, 0, DO_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction2 (classPCA, 1, classSSCP, 1, L"Project", 0, 0, DO_Eigen_and_SSCP_project);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, L"To Configuration...", 0, 0, DO_PCA_and_TableOfReal_to_Configuration);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, L"To TableOfReal (z-scores)...", 0, 0, DO_PCA_and_TableOfReal_to_TableOfReal_zscores);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, L"Get fraction variance...", 0, 0, DO_PCA_and_TableOfReal_getFractionVariance);
	praat_addAction2 (classPCA, 1, classCovariance, 1, L"Project", 0, 0, DO_Eigen_and_Covariance_project);

	praat_Eigen_Matrix_project (classPCA, classFormantFilter);
	praat_Eigen_Matrix_project (classPCA, classBarkFilter);
	praat_Eigen_Matrix_project (classPCA, classMelFilter);

	praat_addAction1 (classPermutation, 0, L"Permutation help", 0, 0, DO_Permutation_help);
	praat_addAction1 (classPermutation, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, L"Get number of elements", 0, 1, DO_Permutation_getNumberOfElements);
	praat_addAction1 (classPermutation, 1, L"Get value...", 0, 1, DO_Permutation_getValueAtIndex);
	praat_addAction1 (classPermutation, 1, L"Get index...", 0, 1, DO_Permutation_getIndexAtValue);
	praat_addAction1 (classPermutation, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, L"Sort", 0, 1, DO_Permutation_sort);
	praat_addAction1 (classPermutation, 1, L"Swap blocks...", 0, 1, DO_Permutation_swapBlocks);
	praat_addAction1 (classPermutation, 1, L"Swap numbers...", 0, 1, DO_Permutation_swapNumbers);
	praat_addAction1 (classPermutation, 1, L"Swap positions...", 0, 1, DO_Permutation_swapPositions);
	praat_addAction1 (classPermutation, 1, L"Swap one from range...", 0, 1, DO_Permutation_swapOneFromRange);
	praat_addAction1 (classPermutation, 0, L"-- sequential permutations --", 0, 1, 0);
	praat_addAction1 (classPermutation, 0, L"Next", 0, 1, DO_Permutations_next);
	praat_addAction1 (classPermutation, 0, L"Previous", 0, 1, DO_Permutations_previous);
	praat_addAction1 (classPermutation, 1, L"Permute randomly...", 0, 0, DO_Permutation_permuteRandomly);
	praat_addAction1 (classPermutation, 1, L"Permute randomly (blocks)...", 0, 0, DO_Permutation_permuteBlocksRandomly);
	praat_addAction1 (classPermutation, 1, L"Interleave...", 0, 0, DO_Permutation_interleave);
	praat_addAction1 (classPermutation, 1, L"Rotate...", 0, 0, DO_Permutation_rotate);
	praat_addAction1 (classPermutation, 1, L"Reverse...", 0, 0, DO_Permutation_reverse);
	praat_addAction1 (classPermutation, 1, L"Invert", 0, 0, DO_Permutation_invert);
	praat_addAction1 (classPermutation, 0, L"Multiply", 0, 0, DO_Permutations_multiply);

	praat_addAction1 (classPitch, 2, L"To DTW...", L"To PointProcess", praat_HIDDEN, DO_Pitches_to_DTW);

	praat_addAction1 (classPitchTier, 0, L"To Pitch...", L"To Sound (sine)...", 1, DO_PitchTier_to_Pitch);
	praat_addAction1 (classPolygon, 0, QUERY_BUTTON, L"Paint circles...", 0, 0);
	praat_addAction1 (classPolygon, 0, L"Get number of points", QUERY_BUTTON, 1, DO_Polygon_getNumberOfPoints);
	praat_addAction1 (classPolygon, 0, L"Get point (x)...", L"Get number of points", 1, DO_Polygon_getPointX);
	praat_addAction1 (classPolygon, 0, L"Get point (y)...",  L"Get point (x)...", 1, DO_Polygon_getPointY);
	praat_addAction1 (classPolygon, 0, L"-- other queries --",  L"Get point (y)...", 1, 0);
	praat_addAction1 (classPolygon, 0, L"Get location of point...", L"-- other queries --", 1, DO_Polygon_getLocationOfPoint);
	praat_addAction1 (classPolygon, 0, L"Translate...", MODIFY_BUTTON, 1, DO_Polygon_translate);
	praat_addAction1 (classPolygon, 0, L"Rotate...", L"Translate...", 1, DO_Polygon_rotate);
	praat_addAction1 (classPolygon, 0, L"Scale...", L"Rotate...", 1, DO_Polygon_scale);
	praat_addAction1 (classPolygon, 0, L"Reverse X", L"Scale...", 1, DO_Polygon_reverseX);
	praat_addAction1 (classPolygon, 0, L"Reverse Y", L"Reverse X", 1, DO_Polygon_reverseY);
	praat_addAction1 (classPolygon, 0, L"Simplify", 0, praat_HIDDEN, DO_Polygon_simplify);
	praat_addAction1 (classPolygon, 0, L"Circular permutation...", 0, praat_HIDDEN, DO_Polygon_circularPermutation);

	praat_addAction2 (classPolygon, 1, classCategories, 1, L"Draw...", 0, 0, DO_Polygon_Categories_draw);

	praat_addAction1 (classPolynomial, 0, L"Polynomial help", 0, 0, DO_Polynomial_help);
	praat_FunctionTerms_init (classPolynomial);
	praat_addAction1 (classPolynomial, 0, L"-- area --", L"Get x of maximum...", 1, 0);
	praat_addAction1 (classPolynomial, 1, L"Get area...", L"-- area --", 1, DO_Polynomial_getArea);
	praat_addAction1 (classPolynomial, 0, L"-- monic --", L"Set coefficient...", 1, 0);
	praat_addAction1 (classPolynomial, 0, L"Scale coefficients (monic)", L"-- monic --", 1, DO_Polynomial_scaleCoefficients_monic);
	praat_addAction1 (classPolynomial, 1, L"Get value (complex)...", L"Get value...", 1, DO_Polynomial_evaluate_z);
	praat_addAction1 (classPolynomial, 0, L"To Spectrum...", L"Analyse", 0, DO_Polynomial_to_Spectrum);
	praat_addAction1 (classPolynomial, 0, L"To Roots", 0, 0, DO_Polynomial_to_Roots);
	praat_addAction1 (classPolynomial, 0, L"To Polynomial (derivative)", 0, 0, DO_Polynomial_getDerivative);
	praat_addAction1 (classPolynomial, 0, L"To Polynomial (primitive)", 0, 0, DO_Polynomial_getPrimitive);
	praat_addAction1 (classPolynomial, 0, L"Scale x...", 0, 0, DO_Polynomial_scaleX);
	praat_addAction1 (classPolynomial, 2, L"Multiply", 0, 0, DO_Polynomials_multiply);
	praat_addAction1 (classPolynomial, 2, L"Divide...", 0, 0, DO_Polynomials_divide);

	praat_addAction1 (classRoots, 1, L"Roots help", 0, 0, DO_Roots_help);
	praat_addAction1 (classRoots, 1, L"Draw...", 0, 0, DO_Roots_draw);
	praat_addAction1 (classRoots, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classRoots, 1, L"Get number of roots", 0, 1, DO_Roots_getNumberOfRoots);
	praat_addAction1 (classRoots, 1, L"-- roots --", 0, 1, 0);
	praat_addAction1 (classRoots, 1, L"Get root...", 0, 1, DO_Roots_getRoot);
	praat_addAction1 (classRoots, 1, L"Get real part of root...", 0, 1, DO_Roots_getRealPartOfRoot);
	praat_addAction1 (classRoots, 1, L"Get imaginary part of root...", 0, 1, DO_Roots_getImaginaryPartOfRoot);
	praat_addAction1 (classRoots, 1, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classRoots, 1, L"Set root...", 0, 1, DO_Roots_setRoot);
	praat_addAction1 (classRoots, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classRoots, 0, L"To Spectrum...", 0, 0, DO_Roots_to_Spectrum);

	praat_addAction2 (classRoots, 1, classPolynomial, 1, L"Polish roots", 0, 0, DO_Roots_and_Polynomial_polish);

	praat_addAction1 (classSound, 0, L"To TextGrid (silences)...", L"To IntervalTier", 1, DO_Sound_to_TextGrid_detectSilences);
    praat_addAction1 (classSound, 0, L"Play one channel...", L"Play", praat_HIDDEN, DO_Sound_playOneChannel);
    praat_addAction1 (classSound, 0, L"Play as frequency shifted...", L"Play", praat_HIDDEN, DO_Sound_playAsFrequencyShifted);
	praat_addAction1 (classSound, 0, L"Draw where...", L"Draw...", 1, DO_Sound_drawWhere);
	//	praat_addAction1 (classSound, 0, L"Paint where...", L"Draw where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_paintWhere);
	praat_addAction1 (classSound, 0, L"Paint where...", L"Draw where...", 1, DO_Sound_paintWhere);
	//	praat_addAction1 (classSound, 2, L"Paint enclosed...", L"Paint where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sounds_paintEnclosed);
	praat_addAction1 (classSound, 2, L"Paint enclosed...", L"Paint where...", 1, DO_Sounds_paintEnclosed);

	praat_addAction1 (classSound, 0, L"To Pitch (shs)...", L"To Pitch (cc)...", 1, DO_Sound_to_Pitch_shs);
	praat_addAction1 (classSound, 0, L"Fade in...", L"Multiply by window...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_fadeIn);
	praat_addAction1 (classSound, 0, L"Fade out...", L"Fade in...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_fadeOut);
	praat_addAction1 (classSound, 0, L"To Pitch (SPINET)...", L"To Pitch (cc)...", 1, DO_Sound_to_Pitch_SPINET);

	praat_addAction1 (classSound, 0, L"To FormantFilter...", L"To Cochleagram (edb)...", 1, DO_Sound_to_FormantFilter);

	praat_addAction1 (classSound, 0, L"To BarkFilter...", L"To FormantFilter...", 1, DO_Sound_to_BarkFilter);

	praat_addAction1 (classSound, 0, L"To MelFilter...", L"To BarkFilter...", 1, DO_Sound_to_MelFilter);

	praat_addAction1 (classSound, 0, L"To Polygon...", L"Down to Matrix", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_to_Polygon);
    praat_addAction1 (classSound, 2, L"To Polygon (enclosed)...", L"Cross-correlate...", praat_DEPTH_1 | praat_HIDDEN, DO_Sounds_to_Polygon_enclosed);
    praat_addAction1 (classSound, 2, L"To DTW...", L"Cross-correlate...", praat_DEPTH_1, DO_Sounds_to_DTW);

	praat_addAction1 (classSound, 1, L"Filter (gammatone)...", L"Filter (de-emphasis)...", 1, DO_Sound_filterByGammaToneFilter4);
	praat_addAction1 (classSound, 0, L"Remove noise...", L"Filter (formula)...", 1, DO_Sound_removeNoise);

	praat_addAction1 (classSound, 0, L"Change gender...", L"Deepen band modulation...", 1, DO_Sound_changeGender);

	praat_addAction1 (classSound, 0, L"Change speaker...", L"Deepen band modulation...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_changeSpeaker);
	praat_addAction1 (classSound, 0, L"Copy channel ranges...", L"Extract all channels", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_copyChannelRanges);
	praat_addAction1 (classSound, 0, L"Trim silences...", L"Resample...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_trimSilences);
	praat_addAction1 (classSound, 0, L"To KlattGrid (simple)...", L"To Manipulation...", 1, DO_Sound_to_KlattGrid_simple);
	praat_addAction2 (classSound, 1, classPitch, 1, L"To FormantFilter...", 0, 0, DO_Sound_and_Pitch_to_FormantFilter);

	praat_addAction2 (classSound, 1, classPitch, 1, L"Change gender...", 0, 0, DO_Sound_and_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, L"Change speaker...", 0, praat_HIDDEN, DO_Sound_and_Pitch_changeSpeaker);
	praat_addAction2 (classSound, 1, classIntervalTier, 1, L"Cut parts matching label...", 0, 0, DO_Sound_and_IntervalTier_cutPartsMatchingLabel);
	praat_addAction1 (classSpectrogram, 2, L"To DTW...", L"To Spectrum (slice)...", 0, DO_Spectrograms_to_DTW);
	praat_addAction1 (classSpectrum, 0, L"Draw phases...", L"Draw (log freq)...", praat_DEPTH_1 | praat_HIDDEN, DO_Spectrum_drawPhases);
	praat_addAction1 (classSpectrum, 0, L"Conjugate", L"Formula...", praat_HIDDEN | praat_DEPTH_1, DO_Spectrum_conjugate);
	praat_addAction1 (classSpectrum, 2, L"Multiply", L"To Sound (fft)", praat_HIDDEN, DO_Spectra_multiply);
	praat_addAction1 (classSpectrum, 0, L"To Matrix (unwrap)", L"To Matrix", praat_HIDDEN, DO_Spectrum_unwrap);
	praat_addAction1 (classSpectrum, 0, L"Shift frequencies...", L"To Matrix", praat_HIDDEN, DO_Spectrum_shiftFrequencies);
	praat_addAction1 (classSpectrum, 0, L"Compress frequency domain...", L"Shift frequencies...", praat_HIDDEN, DO_Spectrum_compressFrequencyDomain);
	praat_addAction1 (classSpectrum, 0, L"Resample...", L"Compress frequency domain...", praat_HIDDEN, DO_Spectrum_resample);
	praat_addAction1 (classSpectrum, 0, L"To Cepstrum", L"To Spectrogram", 1, DO_Spectrum_to_Cepstrum);
	praat_addAction1 (classSpectrum, 0, L"To PowerCepstrum", L"To Cepstrum", 1, DO_Spectrum_to_PowerCepstrum);

	praat_addAction1 (classSpeechSynthesizer, 0, L"SpeechSynthesizer help", 0, 0, DO_SpeechSynthesizer_help);
	praat_addAction1 (classSpeechSynthesizer, 0, L"Play text...", 0, 0, DO_SpeechSynthesizer_playText);
	praat_addAction1 (classSpeechSynthesizer, 0, L"To Sound...", 0, 0, DO_SpeechSynthesizer_to_Sound);
	praat_addAction1 (classSpeechSynthesizer, 0, QUERY_BUTTON, 0, 0, 0);
		praat_addAction1 (classSpeechSynthesizer, 1, L"Get voice name", 0, 1, DO_SpeechSynthesizer_getVoiceName);
		praat_addAction1 (classSpeechSynthesizer, 1, L"Get voice variant", 0, 1, DO_SpeechSynthesizer_getVoiceVariant);
	praat_addAction1 (classSpeechSynthesizer, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_addAction1 (classSpeechSynthesizer, 0, L"Set text input settings...", 0, 1, DO_SpeechSynthesizer_setTextInputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, L"Set speech output settings...", 0, 1, DO_SpeechSynthesizer_setSpeechOutputSettings);
	praat_addAction2 (classSpeechSynthesizer, 1, classTextGrid, 1, L"To Sound...", 0, 0, DO_SpeechSynthesizer_and_TextGrid_to_Sound);

	praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, L"To TextGrid (align)...", 0, 0, DO_SpeechSynthesizer_and_Sound_and_TextGrid_align);
    praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, L"To TextGrid (align,trim)...", 0, 0, DO_SpeechSynthesizer_and_Sound_and_TextGrid_align2);

	praat_addAction1 (classSSCP, 0, L"SSCP help", 0, 0, DO_SSCP_help);
	praat_TableOfReal_init2 (classSSCP);
	praat_removeAction (classSSCP, NULL, NULL, L"Append");
	praat_addAction1 (classSSCP, 0, L"Draw sigma ellipse...", DRAW_BUTTON, 1, DO_SSCP_drawSigmaEllipse);
	praat_addAction1 (classSSCP, 0, L"Draw confidence ellipse...", DRAW_BUTTON, 1, DO_SSCP_drawConfidenceEllipse);
	praat_SSCP_query_init (classSSCP);
	praat_addAction1 (classSSCP, 1, L"Get diagonality (bartlett)...", L"Get ln(determinant)", 1, DO_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classSSCP, 1, L"Get total variance", L"Get diagonality (bartlett)...", 1, DO_SSCP_getTotalVariance);
	praat_addAction1 (classSSCP, 1, L"Get sigma ellipse area...", L"Get total variance", 1, DO_SSCP_getConcentrationEllipseArea);
	praat_addAction1 (classSSCP, 1, L"Get confidence ellipse area...", L"Get sigma ellipse area...", 1, DO_SSCP_getConfidenceEllipseArea);
	praat_addAction1 (classSSCP, 1, L"Get fraction variation...", L"Get confidence ellipse area...", 1, DO_SSCP_getFractionVariation);
	praat_SSCP_extract_init (classSSCP);
	praat_addAction1 (classSSCP, 0, L"To PCA", 0, 0, DO_SSCP_to_PCA);
	praat_addAction1 (classSSCP, 0, L"To Correlation", 0, 0, DO_SSCP_to_Correlation);
	praat_addAction1 (classSSCP, 0, L"To Covariance...", 0, 0, DO_SSCP_to_Covariance);

	praat_addAction1 (classStrings, 0, L"To Categories", 0, 0, DO_Strings_to_Categories);
	praat_addAction1 (classStrings, 0, L"Append", 0, 0, DO_Strings_append);
	praat_addAction1 (classStrings, 0, L"Change...", L"Replace all...", praat_HIDDEN, DO_Strings_change);
	praat_addAction1 (classStrings, 0, L"Extract part...", L"Replace all...", 0, DO_Strings_extractPart);
	praat_addAction1 (classStrings, 0, L"To Permutation...", L"To Distributions", 0, DO_Strings_to_Permutation);
	praat_addAction1 (classStrings, 2, L"To EditDistanceTable", L"To Distributions", 0, DO_Strings_to_EditDistanceTable);

	praat_addAction1 (classSVD, 0, L"To TableOfReal...", 0, 0, DO_SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, L"Extract left singular vectors", 0, 0, DO_SVD_extractLeftSingularVectors);
	praat_addAction1 (classSVD, 0, L"Extract right singular vectors", 0, 0, DO_SVD_extractRightSingularVectors);
	praat_addAction1 (classSVD, 0, L"Extract singular values", 0, 0, DO_SVD_extractSingularValues);

		praat_addAction1 (classTable, 0, L"Box plots...", L"Draw ellipse (standard deviation)...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_boxPlots);
		praat_addAction1 (classTable, 0, L"Normal probability plot...", L"Box plots...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_normalProbabilityPlot);
		praat_addAction1 (classTable, 0, L"Quantile-quantile plot...", L"Normal probability plot...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_quantileQuantilePlot);
		praat_addAction1 (classTable, 0, L"Quantile-quantile plot (between levels)...", L"Quantile-quantile plot...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_quantileQuantilePlot_betweenLevels);
		praat_addAction1 (classTable, 0, L"Scatter plot (ci)...", 0, praat_DEPTH_1, DO_Table_scatterPlotWithConfidenceIntervals);
		praat_addAction1 (classTable, 0, L"Distribution plot...", L"Quantile-quantile plot...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_distributionPlot);
		praat_addAction1 (classTable, 1, L"Draw where -",  L"Quantile-quantile plot (between levels)...", 1 , 0);
			praat_addAction1 (classTable, 0, L"Scatter plot where...", L"Draw where -", 2, DO_Table_scatterPlotWhere);
			praat_addAction1 (classTable, 0, L"Scatter plot where (mark)...", L"Scatter plot where...", 2, DO_Table_scatterPlotMarkWhere);
			praat_addAction1 (classTable, 0, L"Distribution plot where...", L"Scatter plot where (mark)...", 2, DO_Table_distributionPlotWhere);
			praat_addAction1 (classTable, 0, L"Draw ellipse where (standard deviation)...", L"Distribution plot where...", 2, DO_Table_drawEllipseWhere);
			praat_addAction1 (classTable, 0, L"Box plots where...", L"Draw ellipse where (standard deviation)...", 2, DO_Table_boxPlotsWhere);
			praat_addAction1 (classTable, 0, L"Normal probability plot where...", L"Box plots where...", 2, DO_Table_normalProbabilityPlotWhere);
			praat_addAction1 (classTable, 0, L"Bar plot where...", L"Normal probability plot where...", 2, DO_Table_barPlotWhere);
			praat_addAction1 (classTable, 0, L"Line graph where...", L"Bar plot where...", 2, DO_Table_LineGraphWhere);

	praat_addAction1 (classTable, 1, L"Report one-way anova...", L"Report group difference (Wilcoxon rank sum)...", praat_DEPTH_1 | praat_HIDDEN,	DO_Table_reportOneWayAnova);
	praat_addAction1 (classTable, 1, L"Report one-way Kruskal-Wallis...", L"Report one-way anova...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_reportOneWayKruskalWallis);
	praat_addAction1 (classTable, 1, L"Report two-way anova...", L"Report one-way Kruskal-Wallis...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_reportTwoWayAnova);
	praat_addAction1 (classTable, 1, L"Extract rows where...", L"Extract rows where column (text)...", praat_DEPTH_1, DO_Table_extractRowsWhere);

	praat_addAction1 (classTable, 0, L"To KlattTable", 0, praat_HIDDEN, DO_Table_to_KlattTable);
	praat_addAction1 (classTable, 1, L"Get median absolute deviation...", L"Get standard deviation...", 1, DO_Table_getMedianAbsoluteDeviation);
	praat_addAction1 (classTable, 0, L"To StringsIndex (column)...", 0, praat_HIDDEN, DO_Table_to_StringsIndex_column);

	praat_addAction1 (classTableOfReal, 1, L"Report multivariate normality...", L"Get column stdev (label)...",
		praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_reportMultivariateNormality);
	praat_addAction1 (classTableOfReal, 0, L"Append columns", L"Append", 1, DO_TableOfReal_appendColumns);
	praat_addAction1 (classTableOfReal, 0, L"Multivariate statistics -", 0, 0, 0);
	praat_addAction1 (classTableOfReal, 0, L"To Discriminant", 0, 1, DO_TableOfReal_to_Discriminant);
	praat_addAction1 (classTableOfReal, 0, L"To PCA", 0, 1, DO_TableOfReal_to_PCA);
	praat_addAction1 (classTableOfReal, 0, L"To SSCP...", 0, 1, DO_TableOfReal_to_SSCP);
	praat_addAction1 (classTableOfReal, 0, L"To Covariance", 0, 1, DO_TableOfReal_to_Covariance);
	praat_addAction1 (classTableOfReal, 0, L"To Correlation", 0, 1, DO_TableOfReal_to_Correlation);
	praat_addAction1 (classTableOfReal, 0, L"To Correlation (rank)", 0, 1, DO_TableOfReal_to_Correlation_rank);
	praat_addAction1 (classTableOfReal, 0, L"To CCA...", 0, 1, DO_TableOfReal_to_CCA);
	praat_addAction1 (classTableOfReal, 0, L"To TableOfReal (means by row labels)...", 0, 1, DO_TableOfReal_meansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, L"To TableOfReal (medians by row labels)...", 0, 1, DO_TableOfReal_mediansByRowLabels);

	praat_addAction1 (classTableOfReal, 0, L"-- configurations --", 0, 1, 0);
	praat_addAction1 (classTableOfReal, 0, L"To Configuration (pca)...",	0, 1, DO_TableOfReal_to_Configuration_pca);
	praat_addAction1 (classTableOfReal, 0, L"To Configuration (lda)...", 0, 1, DO_TableOfReal_to_Configuration_lda);
	praat_addAction1 (classTableOfReal, 2, L"-- between tables --", L"To Configuration (lda)...", 1, 0);
	praat_addAction1 (classTableOfReal, 2, L"To TableOfReal (cross-correlations)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_TableOfReal_and_TableOfReal_crossCorrelations);


	praat_addAction1 (classTableOfReal, 1, L"To Pattern and Categories...", L"To Matrix", 1, DO_TableOfReal_to_Pattern_and_Categories);
	praat_addAction1 (classTableOfReal, 1, L"Split into Pattern and Categories...", L"To Pattern and Categories...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_to_Pattern_and_Categories);
	praat_addAction1 (classTableOfReal, 0, L"To Permutation (sort row labels)", L"To Matrix", 1, DO_TableOfReal_to_Permutation_sortRowlabels);

	praat_addAction1 (classTableOfReal, 1, L"To SVD", 0, praat_HIDDEN, DO_TableOfReal_to_SVD);
	praat_addAction1 (classTableOfReal, 2, L"To GSVD", 0, praat_HIDDEN, DO_TablesOfReal_to_GSVD);
	praat_addAction1 (classTableOfReal, 2, L"To Eigen (gsvd)", 0, praat_HIDDEN, DO_TablesOfReal_to_Eigen_gsvd);

	praat_addAction1 (classTableOfReal, 0, L"To TableOfReal (cholesky)...", 0, praat_HIDDEN, DO_TableOfReal_choleskyDecomposition);

	praat_addAction1 (classTableOfReal, 0, L"-- scatter plots --", L"Draw top and bottom lines...", 1, 0);
	praat_addAction1 (classTableOfReal, 0, L"Draw scatter plot...", L"-- scatter plots --", 1, DO_TableOfReal_drawScatterPlot);
	praat_addAction1 (classTableOfReal, 0, L"Draw scatter plot matrix...", L"Draw scatter plot...", 1, DO_TableOfReal_drawScatterPlotMatrix);
	praat_addAction1 (classTableOfReal, 0, L"Draw box plots...", L"Draw scatter plot matrix...", 1, DO_TableOfReal_drawBoxPlots);
	praat_addAction1 (classTableOfReal, 0, L"Draw biplot...", L"Draw box plots...", 1, DO_TableOfReal_drawBiplot);
	praat_addAction1 (classTableOfReal, 0, L"Draw vectors...", L"Draw box plots...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawVectors);
	praat_addAction1 (classTableOfReal, 1, L"Draw row as histogram...", L"Draw biplot...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawRowAsHistogram);
	praat_addAction1 (classTableOfReal, 1, L"Draw rows as histogram...", L"Draw row as histogram...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawRowsAsHistogram);
	praat_addAction1 (classTableOfReal, 1, L"Draw column as distribution...", L"Draw rows as histogram...", praat_DEPTH_1, DO_TableOfReal_drawColumnAsDistribution);

	praat_addAction2 (classStrings, 1, classPermutation, 1, L"Permute strings", 0, 0, DO_Strings_and_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, L"Permute rows",	0, 0, DO_TableOfReal_and_Permutation_permuteRows);

	praat_addAction1 (classTextGrid, 0, L"Extend time...", L"Scale times...", 2, DO_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, L"Set tier name...", L"Remove tier...", 1, DO_TextGrid_setTierName);
	praat_addAction1 (classTextGrid, 0, L"Replace interval text...", L"Set interval text...", 2, DO_TextGrid_replaceIntervalTexts);
	praat_addAction1 (classTextGrid, 0, L"Replace point text...", L"Set point text...", 2, DO_TextGrid_replacePointTexts);
	praat_addAction1 (classTextGrid, 2, L"To Table (text alignment)...", L"Extract part...", 0, DO_TextGrids_to_Table_textAlignmentment);
	praat_addAction2 (classTextGrid, 2, classEditCostsTable, 1, L"To Table (text alignment)...", 0, 0, DO_TextGrids_and_EditCostsTable_to_Table_textAlignmentment);

	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)

	INCLUDE_LIBRARY (praat_uvafon_MDS_init)
	INCLUDE_LIBRARY (praat_KlattGrid_init)
	INCLUDE_LIBRARY (praat_HMM_init)
	INCLUDE_LIBRARY (praat_BSS_init)
}

/* End of file praat_David.cpp */
