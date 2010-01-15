/* praat_David_init.c
 *
 * Copyright (C) 1993-2010 David Weenink
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
*/

#include "praat.h"
#include "NUM2.h"
#include "NUMlapack.h"
#include "NUMmachar.h"
extern machar_Table NUMfpp;

#include "Activation.h"
#include "Categories.h"
#include "CategoriesEditor.h"
#include "ClassificationTable.h"
#include "Collection_extensions.h"
#include "Confusion.h"
#include "Discriminant.h"
#include "Editor.h"
#include "Eigen_and_Matrix.h"
#include "Eigen_and_Procrustes.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "Excitations.h"
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
#include "Minimizers.h"
#include "Pattern.h"
#include "PCA.h"
#include "PitchTierEditor.h"
#include "Polygon_extensions.h"
#include "Polynomial.h"
#include "Sound_extensions.h"
#include "Spectrum_extensions.h"
#include "Spectrogram.h"
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

static wchar_t *QUERY_BUTTON   = L"Query -                ";
static wchar_t *DRAW_BUTTON    = L"Draw -                 ";
static wchar_t *MODIFY_BUTTON  = L"Modify -               ";
static wchar_t *EXTRACT_BUTTON = L"Extract -";

extern void praat_TimeFunction_query_init (void *klas);
extern void praat_TimeFrameSampled_query_init (void *klas);
extern void praat_TableOfReal_init (void *klas);
void praat_TableOfReal_init2  (void *klas);

void praat_CC_init (void *klas);
void DTW_constraints_addCommonFields (void *dia);
void DTW_constraints_getCommonFields (void *dia, int *begin, int *end, int *slope);
void praat_Matrixft_query_init (void *klas);
int praat_Fon_formula (UiForm dia, Interpreter interpreter);

#undef INCLUDE_DTW_SLOPES

static int pr_LongSounds_appendToExistingSoundFile (MelderFile file)
{
	int IOBJECT, status = 0;
	Ordered me = Ordered_create ();
	if (me == NULL) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) goto end;

	status = LongSounds_appendToExistingSoundFile (me, file);
end:
	my size = 0; forget (me);
	return status;
}

static int pr_LongSounds_writeToStereoAudioFile (MelderFile file, int audioFileType)
{
	int IOBJECT;
	LongSound me = NULL, thee = NULL;
	WHERE (SELECTED)
	{
		if (me) thee = OBJECT;
		else me = OBJECT;
	}
	return LongSounds_writeToStereoAudioFile16 (me, thee, audioFileType, file);
}

/********************** Activation *******************************************/

FORM (Activation_formula, L"Activation: Formula", 0)
	LABEL (L"label", L"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }}")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	if (! praat_Fon_formula (dia, interpreter)) return 0;
END

DIRECT (Activation_to_Matrix)
	EVERY_TO (Activation_to_Matrix (OBJECT))
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
	EVERY_DRAW (FilterBank_drawTimeSlice (OBJECT, GRAPHICS,
		GET_REAL (L"Time"), GET_REAL (L"left Frequency range"),
		GET_REAL (L"right Frequency range"), GET_REAL (L"left Amplitude range"),
		GET_REAL (L"right Amplitude range"), L"Barks", GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (BarkFilter_drawSekeyHansonFilterFunctions (OBJECT, GRAPHICS,
		GET_INTEGER (L"Frequency scale"),
		GET_INTEGER (L"left Filter range"), GET_INTEGER (L"right Filter range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_INTEGER (L"Amplitude scale in dB"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Garnish")))
END
/********************** Categories  ****************************************/

FORM (Categories_append, L"Categories: Append 1 category", L"Categories: Append 1 category...")
	SENTENCE (L"Category", L"")
	OK
DO
	if (! OrderedOfString_append (ONLY_OBJECT, GET_STRING (L"Category")))
		return 0;
END

DIRECT (Categories_edit)
	if (theCurrentPraatApplication -> batch)
		return Melder_error1 (L"Cannot edit a Categories from batch.");
	else
		WHERE (SELECTED) if (! praat_installEditor (CategoriesEditor_create (theCurrentPraatApplication -> topShell,
			FULL_NAME, OBJECT), IOBJECT)) return 0;
END

DIRECT (Categories_getNumberOfDifferences)
	Categories c1 = NULL, c2 = NULL;
	long NumberOfDifferences;
	WHERE (SELECTED && CLASS == classCategories)
	{
   		if (c1) c2 = OBJECT; else c1 = OBJECT;
	}
	NumberOfDifferences = OrderedOfString_getNumberOfDifferences (c1, c2);
	if (NumberOfDifferences< 0) Melder_information1 (L"-1 (undefined: number of elements differ!)");
	else
		Melder_information2 (Melder_integer (NumberOfDifferences), L" differences");
END

DIRECT (Categories_getFractionDifferent)
	Categories c1 = NULL, c2 = NULL;
	WHERE (SELECTED && CLASS == classCategories)
	{
   		if (c1) c2 = OBJECT; else c1 = OBJECT;
	}
	Melder_information1 (Melder_double (OrderedOfString_getFractionDifferent (c1,c2)));
END

DIRECT (Categories_difference)
	Categories l1 = NULL, l2 = NULL;
	double fraction;
	long n;
	WHERE (SELECTED && CLASS == classCategories)
	{
   		if (l1) l2 = OBJECT; else l1 = OBJECT;
	}
	if (! OrderedOfString_difference (l1, l2, &n, &fraction)) return 0;
	Melder_information2 (Melder_integer (n), L" differences");
END

DIRECT (Categories_selectUniqueItems)
	EVERY_TO ((Categories) Categories_selectUniqueItems (OBJECT, 1))
END

DIRECT (Categories_to_Confusion)
	Categories c1 = NULL, c2 = NULL;
	int i1 = 0, i2 = 0;
	WHERE (SELECTED)
	{
		if (c1)
		{
			c2 = OBJECT; i2 = IOBJECT;
		}
		else
		{
			c1 = OBJECT; i1 = IOBJECT;
		}
	}
	Melder_assert (c1 && c2);
	if (! praat_new3 (Categories_to_Confusion (c1, c2), Thing_getName(c1), L"_", Thing_getName(c2))) return 0;
END

DIRECT (Categories_to_Strings)
	EVERY_TO (Categories_to_Strings (OBJECT))
END

DIRECT (Categories_join)
	Data l1 = NULL, l2 = NULL;
	WHERE (SELECTED && CLASS == classCategories)
	{
		if (l1) l2 = OBJECT; else l1 = OBJECT;
	}
	NEW (OrderedOfString_joinItems (l1, l2))
END

DIRECT (Categories_permuteItems)
	Collection c = NULL;
	WHERE (SELECTED && CLASS == classCategories) c = OBJECT;
	NEW (Collection_permuteItems (c));
END

/***************** CC ****************************************/
FORM (CC_getValue, L"CC: Get value", L"CC: Get value...")
	REAL (L"Time (s)", L"0.1")
	NATURAL (L"Index", L"1")
	OK
DO
	Melder_informationReal (CC_getValue (ONLY_OBJECT, GET_REAL (L"Time"), GET_INTEGER (L"Index")), NULL);
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
	EVERY_DRAW (CC_paint (OBJECT, GRAPHICS, GET_REAL (L"left Time range"),
		GET_REAL (L"right Time range"), GET_INTEGER (L"From coefficient"),
		GET_INTEGER (L"To coefficient"), GET_REAL (L"Minimum"),
		GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish")))
END

FORM (CC_drawC0, L"CC: Draw c0", L"CC: Draw c0...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Amplitude range", L"0.0")
	REAL (L"right Amplitude range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (CC_drawC0 (OBJECT, GRAPHICS, GET_REAL (L"left Time range"),
		GET_REAL (L"right Time range"), GET_REAL (L"left Amplitude range"),
		GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Garnish")))
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
	CC c1 = NULL, c2 = NULL;
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);

	WHERE (SELECTED && Thing_member (OBJECT, classCC))
	{
		if (c1) c2 = OBJECT; else c1 = OBJECT;
	}
	NEW (CCs_to_DTW (c1, c2, GET_REAL (L"Cepstral weight"),
		GET_REAL (L"Log energy weight"), GET_REAL (L"Regression weight"),
		GET_REAL (L"Regression weight log energy"),
		GET_REAL (L"Regression coefficients window"),begin, end, slope))

END

DIRECT (CC_to_Matrix)
	EVERY_TO (CC_to_Matrix (OBJECT))
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
	EVERY_DRAW (CCA_drawEigenvector (OBJECT, GRAPHICS, GET_INTEGER (L"X or Y"),
		GET_INTEGER (L"Eigenvector number"),
		GET_INTEGER (L"left Element range"), GET_INTEGER (L"right Element range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
		GET_INTEGER (L"Component loadings"), GET_REAL (L"Mark size"),
		GET_STRING (L"Mark string"), GET_INTEGER (L"Connect points"),
		GET_INTEGER (L"Garnish")))
END

DIRECT (CCA_getNumberOfCorrelations)
	CCA cca = ONLY(classCCA);
	Melder_information1 (Melder_double (cca->numberOfCoefficients));
END

FORM (CCA_getCorrelationCoefficient, L"CCA: Get canonical correlation coefficient", L"CCA: Get canonical correlation coefficient")
	NATURAL (L"Coefficient number", L"1")
	OK
DO
	Melder_information1 (Melder_double (CCA_getCorrelationCoefficient (ONLY (classCCA),
		GET_INTEGER (L"Coefficient number"))));
END

FORM (CCA_getEigenvectorElement, L"CCA: Get eigenvector element", L"Eigen: Get eigenvector element...")
	OPTIONMENU (L"X or Y", 1)
	OPTION (L"y")
	OPTION (L"x")
	NATURAL (L"Eigenvector number", L"1")
	NATURAL (L"Element number", L"1")
	OK
DO
	Melder_information1 (Melder_double (CCA_getEigenvectorElement (ONLY (classCCA),
		GET_INTEGER (L"X or Y"), GET_INTEGER (L"Eigenvector number"),
		GET_INTEGER (L"Element number"))));
END

FORM (CCA_getZeroCorrelationProbability, L"CCA: Get zero correlation probability", L"CCA: Get zero correlation probability...")
	NATURAL (L"Coefficient number", L"1")
	OK
DO
	double p, chisq; long ndf;
	CCA_getZeroCorrelationProbability (ONLY (classCCA), GET_INTEGER (L"Coefficient number"),
		&chisq, &ndf, &p);
	Melder_information6 (Melder_double (p), L" (=probability for chisq = ", Melder_double (chisq), L" and ndf = ", Melder_integer (ndf), L")");
END

DIRECT (CCA_and_Correlation_factorLoadings)
	CCA cca = ONLY (classCCA);
	if (! praat_new2 (CCA_and_Correlation_factorLoadings (cca,
		ONLY (classCorrelation)), Thing_getName (cca), L"_loadings")) return 0;
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
	int x_or_y = GET_INTEGER (L"X or Y");
	int cv_from = GET_INTEGER (L"left Canonical variate range");
	int cv_to = GET_INTEGER (L"right Canonical variate range");
	Melder_information7 (Melder_double (CCA_and_Correlation_getVarianceFraction (ONLY (classCCA),
		ONLY (classCorrelation), x_or_y, cv_from, cv_to)), L" (fraction variance from ",
		(x_or_y == 1 ? L"y" : L"x"), L", extracted by canonical variates ", Melder_integer (cv_from), L" to ",
		 Melder_integer (cv_to));
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
	int x_or_y = GET_INTEGER (L"X or Y");
	int cv_from = GET_INTEGER (L"left Canonical variate range");
	int cv_to = GET_INTEGER (L"right Canonical variate range");
	Melder_information7 (Melder_double (CCA_and_Correlation_getRedundancy_sl (ONLY (classCCA), ONLY (classCorrelation),
		x_or_y, cv_from, cv_to)), L" (redundancy from ", (x_or_y == 1 ? L"y" : L"x"), L" extracted by canonical variates ",
		Melder_integer (cv_from), L" to ", Melder_integer (cv_to));
END


DIRECT (CCA_and_TableOfReal_factorLoadings)
	CCA cca = ONLY (classCCA);
	if (! praat_new2 (CCA_and_TableOfReal_factorLoadings (cca,
		ONLY (classTableOfReal)), Thing_getName (cca), L"_loadings")) return 0;
END

FORM (CCA_and_TableOfReal_scores, L"CCA & TableOfReal: To TableOfReal (scores)", L"CCA & TableOfReal: To TableOfReal (scores)...")
	INTEGER (L"Number of canonical correlations", L"0 (=all)")
	OK
DO
	CCA cca = ONLY (classCCA);
	if (! praat_new2 (CCA_and_TableOfReal_scores (cca, ONLY (classTableOfReal),
		GET_INTEGER (L"Number of canonical correlations")),
		Thing_getName (cca), L"_scores")) return 0;
END

FORM (CCA_and_TableOfReal_predict, L"CCA & TableOfReal: Predict", L"CCA & TableOfReal: Predict...")
	LABEL (L"", L"The data set from which to predict starts at...")
	INTEGER (L"Column number", L"1")
	OK
DO
	NEW (CCA_and_TableOfReal_predict (ONLY (classCCA), ONLY(classTableOfReal),
		GET_INTEGER (L"Column number")))
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
	if (! praat_new1 (ChebyshevSeries_createFromString (xmin, xmax,
		GET_STRING (L"Coefficients")), GET_STRING (L"Name"))) return 0;
END

DIRECT (ChebyshevSeries_to_Polynomial)
	EVERY_TO (ChebyshevSeries_to_Polynomial (OBJECT))
END

/***************** ClassificationTable ****************************************/

DIRECT (ClassificationTable_help)
	Melder_help (L"ClassificationTable");
END

DIRECT (ClassificationTable_to_Confusion)
	EVERY_TO (ClassificationTable_to_Confusion (OBJECT))
END

DIRECT (ClassificationTable_to_Strings_maximumProbability)
	EVERY_TO (ClassificationTable_to_Strings_maximumProbability (OBJECT))
END

/********************** Confusion *******************************************/

DIRECT (Confusion_help)
	Melder_help (L"Confusion");
END

DIRECT (Confusion_to_TableOfReal_marginals)
	EVERY_TO (Confusion_to_TableOfReal_marginals (OBJECT))
END

DIRECT (Confusion_difference)
	Confusion c1 = NULL, c2 = NULL;
	WHERE (SELECTED && CLASS == classConfusion) { if (c1) c2 = OBJECT; else c1 = OBJECT; }
    NEW (Confusion_difference (c1, c2))
END

FORM (Confusion_condense, L"Confusion: Condense", L"Confusion: Condense...")
	SENTENCE (L"Search", L"a")
	SENTENCE (L"Replace", L"a")
	INTEGER (L"Replace limit", L"0 (=unlimited)")
	RADIO (L"Search and replace are", 1)
	RADIOBUTTON (L"Literals")
	RADIOBUTTON (L"Regular Expressions")
	OK
DO
	EVERY_TO (Confusion_condense (OBJECT, GET_STRING (L"Search"),
		GET_STRING (L"Replace"), GET_INTEGER (L"Replace limit"),
		GET_INTEGER (L"Search and replace are") - 1))
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
	EVERY_DRAW (Confusion_drawAsNumbers (OBJECT, GRAPHICS,
		GET_INTEGER (L"Draw marginals"),
		GET_INTEGER (L"Format"), GET_INTEGER (L"Precision")))
END

DIRECT (Confusion_getFractionCorrect)
	double f; long n;
	Confusion_getFractionCorrect (ONLY (classConfusion), &f, &n);
	Melder_information2 (Melder_double (f), L" (fraction correct)");
END

/******************* Confusion & Matrix *************************************/

FORM (Confusion_Matrix_draw, L"Confusion & Matrix: Draw confusions with arrows", 0)
    INTEGER (L"Category position", L"0 (=all)")
    REAL (L"lower level(%)", L"0")
    REAL (L"left Horizontal range", L"0.0")
    REAL (L"right Horizontal range", L"0.0")
    REAL (L"left Vertical range", L"0.0")
    REAL (L"right Vertical range", L"0.0")
    BOOLEAN (L"Garnish", 1)
	OK
DO
	long categoryPosition = GET_INTEGER (L"Category position");
	REQUIRE (categoryPosition >= 0, L"Category position must be >= 0")
	EVERY_DRAW (Confusion_Matrix_draw(ONLY(classConfusion), ONLY(classMatrix), GRAPHICS,
		categoryPosition, GET_REAL (L"lower level(%)"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_TO (Correlation_confidenceIntervals (OBJECT, cl, numberOfTests,
		GET_INTEGER (L"Approximation")))
END

FORM (Correlation_testDiagonality_bartlett, L"Correlation: Get diagonality (bartlett)", L"SSCP: Get diagonality (bartlett)...")
	NATURAL (L"Number of contraints", L"1")
	OK
DO
	double chisq, p;
	long nc = GET_INTEGER (L"Number of contraints");
	Correlation me = ONLY_OBJECT;
	Correlation_testDiagonality_bartlett (me, nc, &chisq, &p);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (my numberOfRows * (my numberOfRows - 1) / 2));
END

DIRECT (Correlation_to_PCA)
	EVERY_TO (SSCP_to_PCA (OBJECT))
END

/**********************Covariance *******************************************/

DIRECT (Covariance_help)
	Melder_help (L"Covariance");
END

FORM (Covariance_getSignificanceOfOneMean, L"Covariance: Get significance of one mean", L"Covariance: Get significance of one mean...")
	LABEL (L"", L"Get probability that the mean with")
	NATURAL (L"Index", L"1")
	LABEL (L"", L"differs from")
	REAL (L"Value", L"0.0")
	LABEL (L"", L"(Null hypothesis: the observed difference is due to chance.)")
	OK
DO
	double t, p; double ndf;
	Covariance_getSignificanceOfOneMean (ONLY_OBJECT, GET_INTEGER (L"Index"),
		GET_REAL (L"Value"), &p, &t , &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on t = ",
		Melder_double (t), L" and ndf = ", Melder_integer (ndf));
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
	double t, p; double ndf;
	Covariance_getSignificanceOfMeansDifference (ONLY_OBJECT,
		GET_INTEGER (L"Index1"), GET_INTEGER (L"Index2"),
		GET_REAL (L"Value"), GET_INTEGER (L"Paired"),
		GET_INTEGER (L"Equal variances"), &p, &t , &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on t = ",
		Melder_double (t), L"and ndf = ", Melder_integer (ndf));
END

FORM (Covariance_getSignificanceOfOneVariance, L"Covariance: Get significance of one variance", L"Covariance: Get significance of one variance...")
	LABEL (L"", L"Get probability that the variance with")
	NATURAL (L"Index", L"1")
	LABEL (L"", L"differs from")
	REAL (L"Value", L"0.0")
	LABEL (L"", L"(Null hypothesis: the observed difference is due to chance.)")
	OK
DO
	double chisq, p; long ndf;
	Covariance_getSignificanceOfOneVariance (ONLY_OBJECT, GET_INTEGER (L"Index"),
		GET_REAL (L"Value"), &p, &chisq , &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

FORM (Covariance_getSignificanceOfVariancesRatio, L"Covariance: Get significance of variances ratio", L"Covariance: Get significance of variances ratio...")
	NATURAL (L"Index1", L"1")
	NATURAL (L"Index2", L"2")
	REAL (L"Hypothesized ratio", L"1.0")
	OK
DO
	double f, p; long ndf;
	Covariance_getSignificanceOfVariancesRatio (ONLY_OBJECT,
		GET_INTEGER (L"Index1"), GET_INTEGER (L"Index2"),
		GET_REAL (L"Hypothesized ratio"), &p, &f , &ndf);
	Melder_information7 (Melder_double (p), L" (=probability, based on F = ",
		Melder_double (f), L"and ndf1 = ", Melder_integer (ndf),
		L" and ndf2 = ", Melder_integer (ndf));
END

FORM (Covariance_getFractionVariance, L"Covariance: Get fraction variance", L"Covariance: Get fraction variance...")
	NATURAL (L"From dimension", L"1")
	NATURAL (L"To dimension", L"1")
	OK
DO
	Melder_information1 (Melder_double (SSCP_getFractionVariation (ONLY_OBJECT,
		GET_INTEGER (L"From dimension"), GET_INTEGER (L"To dimension"))));
END

FORM (Covariances_reportMultivariateMeanDifference, L"Covariances: Report multivariate mean difference",
	L"Covariances: Report multivariate mean difference...")
	BOOLEAN (L"Covariances are equal", 1)
	OK
DO
	Covariance c1 = NULL, c2 = NULL;
	double prob, fisher, df1, df2, difference;
	int equalCovariances = GET_INTEGER (L"Covariances are equal");
	WHERE (SELECTED && CLASS == classCovariance)
	{
		if (c1) c2 = OBJECT; else c1 = OBJECT;
	}
	MelderInfo_open ();
	difference = Covariances_getMultivariateCentroidDifference (c1, c2, equalCovariances, &prob, &fisher, &df1, &df2);
	MelderInfo_writeLine3 (L"Under the assumption that the two covariances are", (equalCovariances ? L" " : L" not "),L"equal:");
	MelderInfo_writeLine2 (L"Difference between multivariate means = ", Melder_double (difference));
	MelderInfo_writeLine2 (L"Fisher's F = ", Melder_double (fisher));
	MelderInfo_writeLine2 (L"Significance from zero = ", Melder_double (prob));
	MelderInfo_writeLine4 (L"Degrees of freedom = ", Melder_double (df1), L", ", Melder_double (df2));
	MelderInfo_writeLine4 (L"(Number of observations = ", Melder_integer (c1->numberOfObservations), L", ",
		Melder_integer (c2->numberOfObservations));
	MelderInfo_writeLine3 (L"Dimension of covariance matrices = ", Melder_integer (c1-> numberOfRows), L")");
	MelderInfo_close ();
END

FORM (Covariance_to_TableOfReal_randomSampling, L"Covariance: To TableOfReal (random sampling)", L"Covariance: To TableOfReal (random sampling)...")
	INTEGER (L"Number of data points", L"0")
	OK
DO
	EVERY_TO (Covariance_to_TableOfReal_randomSampling (OBJECT,
		GET_INTEGER (L"Number of data points")))
END

DIRECT (Covariances_reportEquality)
	Ordered covars = Ordered_create ();
	if (covars == NULL) return 0;
	WHERE (SELECTED)
	{
		if (! Collection_addItem (covars, OBJECT))
		{
			covars -> size = 0; forget (covars); return 0;
		}
	}
	MelderInfo_open ();
	{
		double chisq, p, df;
		Covariances_equality (covars, 1, &p, &chisq, &df);
		MelderInfo_writeLine1 (L"Difference between covariance matrices:");
		MelderInfo_writeLine2 (L"Significance of difference (bartlett) = ", Melder_double (p));
		MelderInfo_writeLine2 (L"Chi-squared = ", Melder_double (chisq));
		MelderInfo_writeLine2 (L"Degrees of freedom = ", Melder_double (df));
		Covariances_equality (covars, 2, &p, &chisq, &df);
		MelderInfo_writeLine2 (L"Significance of difference (wald) = ", Melder_double (p));
		MelderInfo_writeLine2 (L"Chi-squared = ", Melder_double (chisq));
		MelderInfo_writeLine2 (L"Degrees of freedom = ", Melder_double (df));
	}
	MelderInfo_close ();
	covars -> size = 0; forget (covars);
END

DIRECT (Covariance_to_Correlation)
	EVERY_TO (SSCP_to_Correlation (OBJECT))
END

DIRECT (Covariance_to_PCA)
	EVERY_TO (SSCP_to_PCA (OBJECT))
END

FORM (Covariance_and_TableOfReal_mahalanobis, L"Covariance & TableOfReal: To TableOfReal (mahalanobis)", L"Covariance & TableOfReal: To TableOfReal (mahalanobis)...")
	BOOLEAN (L"Centroid from table", 0)
	OK
DO
	NEW (Covariance_and_TableOfReal_mahalanobis (ONLY (classCovariance), ONLY (classTableOfReal),
		GET_INTEGER (L"Centroid from table")))
END

/********************** Discriminant **********************************/

DIRECT (Discriminant_help)
	Melder_help (L"Discriminant");
END

DIRECT (Discriminant_setGroupLabels)
	if (! Discriminant_setGroupLabels (ONLY(classDiscriminant),
		ONLY (classStrings))) return 0;
END

FORM (Discriminant_and_Pattern_to_Categories, L"Discriminant & Pattern: To Categories", L"Discriminant & Pattern: To Categories...")
	BOOLEAN (L"Pool covariance matrices", 1)
	BOOLEAN (L"Use apriori probabilities", 1)
	OK
DO
	NEW (Discriminant_and_Pattern_to_Categories
		(ONLY (classDiscriminant), ONLY_GENERIC (classPattern),
		GET_INTEGER (L"Pool covariance matrices"),
		GET_INTEGER (L"Use apriori probabilities")))
END

FORM (Discriminant_and_TableOfReal_to_Configuration, L"Discriminant & TableOfReal: To Configuration", L"Discriminant & TableOfReal: To Configuration...")
	INTEGER (L"Number of dimensions", L"0")
	OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	REQUIRE (dimension >= 0, L"Number of dimensions must be greater equal zero.")
	NEW (Discriminant_and_TableOfReal_to_Configuration
		(ONLY (classDiscriminant), ONLY_GENERIC (classTableOfReal),
			dimension))
END

DIRECT (hint_Discriminant_and_TableOfReal_to_ClassificationTable)
	Melder_information1 (L"You can use the Discriminant as a classifier by \nselecting a Discriminant and a TableOfReal object together.");
END

FORM (Discriminant_and_TableOfReal_to_ClassificationTable, L"Discriminant & TableOfReal: To ClassificationTable", L"Discriminant & TableOfReal: To ClassificationTable...")
	BOOLEAN (L"Pool covariance matrices", 1)
	BOOLEAN (L"Use apriori probabilities", 1)
	OK
DO
	Discriminant d = ONLY (classDiscriminant);
	TableOfReal t = ONLY_GENERIC (classTableOfReal);
	if (! praat_new3 (Discriminant_and_TableOfReal_to_ClassificationTable
		(d,	t, GET_INTEGER (L"Pool covariance matrices"),
		GET_INTEGER (L"Use apriori probabilities")),
		Thing_getName (d), L"_", Thing_getName (t))) return 0;
END

FORM (Discriminant_and_TableOfReal_mahalanobis, L"Discriminant & TableOfReal: To TableOfReal (mahalanobis)", L"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...")
	SENTENCE (L"Group label", L"")
	BOOLEAN (L"Pool covariance matrices", 0)
	OK
DO
	Discriminant d = ONLY (classDiscriminant);
	long group = Discriminant_groupLabelToIndex (d, GET_STRING (L"Group label"));
	REQUIRE (group > 0, L"Group label does not exist.")
	NEW (Discriminant_and_TableOfReal_mahalanobis (d, ONLY(classTableOfReal), group,
		GET_INTEGER (L"Pool covariance matrices")))
END

FORM (Discriminant_getWilksLambda, L"Discriminant: Get Wilks' lambda", L"Discriminant: Get Wilks' lambda...")
	LABEL (L"", L"Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	INTEGER (L"From", L"1")
	OK
DO
	long from = GET_INTEGER (L"From");
	REQUIRE (from >= 1, L"Number must be greater than or equal to one.")
	Melder_information1 (Melder_double (Discriminant_getWilksLambda (ONLY_OBJECT, from)));
END

FORM (Discriminant_getCumulativeContributionOfComponents, L"Discriminant: Get cumulative contribution of components", L"Eigen: Get cumulative contribution of components...")
	NATURAL (L"From component", L"1")
	NATURAL (L"To component", L"1")
	OK
DO
	Melder_information1 (Melder_double (Eigen_getCumulativeContributionOfComponents
		(ONLY_OBJECT, GET_INTEGER (L"From component"),
		GET_INTEGER (L"To component"))));
END


FORM (Discriminant_getPartialDiscriminationProbability, L"Discriminant: Get partial discrimination probability", L"Discriminant: Get partial discrimination probability...")
	INTEGER (L"Number of dimensions", L"1")
	OK
DO
	long ndf, n = GET_INTEGER (L"Number of dimensions");
	double chisq, p;
	REQUIRE (n >= 0, L"Number of dimensions must be greater than or equal to zero.")
	Discriminant_getPartialDiscriminationProbability (ONLY_OBJECT, n, &p, &chisq, &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ", Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

DIRECT (Discriminant_getHomegeneityOfCovariances_box)
	Discriminant thee = ONLY_OBJECT;
	double chisq, p;
	long ndf;
	SSCPs_getHomegeneityOfCovariances_box (thy groups, &p, &chisq, &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

DIRECT (Discriminant_reportEqualityOfCovariances_wald)
	Discriminant thee = ONLY_OBJECT;
	double chisq, prob, df;
	MelderInfo_open ();
	Covariances_equality ((Ordered)(thy groups), 2, &prob, &chisq, &df);
	MelderInfo_writeLine1 (L"Wald test for equality of covariance matrices:");
	MelderInfo_writeLine2 (L"Chi squared: ", Melder_double (chisq));
	MelderInfo_writeLine2 (L"Significance: ", Melder_double (prob));
	MelderInfo_writeLine2 (L"Degrees of freedom: ", Melder_double (df));
	MelderInfo_writeLine2 (L"Number of matrices: ", Melder_integer (thy groups -> size));
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
	Discriminant d = ONLY_OBJECT;
	long group = Discriminant_groupLabelToIndex (d, GET_STRING (L"Group label"));
	REQUIRE (group > 0, L"Group label does not exist.")
	Melder_information1 (Melder_double (Discriminant_getConcentrationEllipseArea(d, group,
		GET_REAL (L"Number of sigmas"), 0, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"))));
END

FORM (Discriminant_getConfidenceEllipseArea, L"Discriminant: Get confidence ellipse area", L"Discriminant: Get confidence ellipse area...")
	SENTENCE (L"Group label", L"")
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	BOOLEAN (L"Discriminant plane", 1)
	INTEGER (L"X-dimension", L"1")
	INTEGER (L"Y-dimension", L"2")
	OK
DO
	Discriminant d = ONLY_OBJECT;
	long group = Discriminant_groupLabelToIndex (d, GET_STRING (L"Group label"));
	REQUIRE (group > 0, L"Group label does not exist.")
	Melder_information1 (Melder_double (Discriminant_getConcentrationEllipseArea(d, group,
		GET_REAL (L"Confidence level"), 1, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"))));
END

FORM (Discriminant_getLnDeterminant_group, L"Discriminant: Get determinant (group)", L"Discriminant: Get determinant (group)...")
	SENTENCE (L"Group label", L"")
	OK
DO
	Discriminant d = ONLY_OBJECT;
	long group = Discriminant_groupLabelToIndex (d, GET_STRING (L"Group label"));
	REQUIRE (group > 0, L"Group label does not exist.")
	Melder_information1 (Melder_double (Discriminant_getLnDeterminant_group (d, group)));
END

DIRECT (Discriminant_getLnDeterminant_total)
	Melder_information1 (Melder_double (Discriminant_getLnDeterminant_total (ONLY_OBJECT)));
END

FORM (Discriminant_invertEigenvector, L"Discriminant: Invert eigenvector", 0)
	NATURAL (L"Index of eigenvector", L"1")
	OK
DO
	EVERY (Eigen_invertEigenvector (OBJECT,
		GET_INTEGER (L"Index of eigenvector")))
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
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Number of sigmas"), 0, NULL, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Number of sigmas"), 0, GET_STRING (L"Label"),
		GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Confidence level"), 1, NULL, GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Confidence level"), 1, GET_STRING (L"Label"), GET_INTEGER (L"Discriminant plane"),
		GET_INTEGER (L"X-dimension"), GET_INTEGER (L"Y-dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
END

DIRECT (Discriminant_extractBetweenGroupsSSCP)
	EVERY_TO (Discriminant_extractBetweenGroupsSSCP (OBJECT))
END
/*
FORM (Discriminant_extractCoefficients, "Extract coefficients...", 0)
	RADIO (L"Function coefficients", 1)
	RADIOBUTTON (L"Raw")
	RADIOBUTTON (L"Unstandardized")
	RADIOBUTTON (L"Standardized")
	OK
DO
	EVERY_TO (Discriminant_extractCoefficients (OBJECT, GET_INTEGER (L"Function coefficients")))
END
*/
DIRECT (Discriminant_extractGroupCentroids)
	EVERY_TO (Discriminant_extractGroupCentroids (OBJECT))
END

DIRECT (Discriminant_extractGroupStandardDeviations)
	EVERY_TO (Discriminant_extractGroupStandardDeviations (OBJECT))
END

DIRECT (Discriminant_extractGroupLabels)
	EVERY_TO (Discriminant_extractGroupLabels (OBJECT))
END

DIRECT (Discriminant_extractPooledWithinGroupsSSCP)
	EVERY_TO (Discriminant_extractPooledWithinGroupsSSCP (OBJECT))
END

FORM (Discriminant_extractWithinGroupSSCP, L"Discriminant: Extract within-group SSCP", L"Discriminant: Extract within-group SSCP...")
	NATURAL (L"Group index", L"1")
	OK
DO
	EVERY_TO (Discriminant_extractWithinGroupSSCP (OBJECT,
		GET_INTEGER (L"Group index")))
END

DIRECT (Discriminant_getNumberOfFunctions)
	Melder_information1 (Melder_integer (Discriminant_getNumberOfFunctions (ONLY_OBJECT)));
END

DIRECT (Discriminant_getDimensionOfFunctions)
	Melder_information1 (Melder_integer (Eigen_getDimensionOfComponents (ONLY_OBJECT)));
END

DIRECT (Discriminant_getNumberOfGroups)
	Melder_information1 (Melder_integer (Discriminant_getNumberOfGroups (ONLY_OBJECT)));
END

FORM (Discriminant_getNumberOfObservations, L"Discriminant: Get number of observations", L"Discriminant: Get number of observations...")
	INTEGER (L"Group", L"0 (=total)")
	OK
DO
	Melder_information1 (Melder_integer (Discriminant_getNumberOfObservations (ONLY_OBJECT, GET_INTEGER (L"Group"))));
END


/********************** DTW *******************************************/

FORM (DTW_and_Sounds_draw, L"DTW & Sounds: Draw", L"DTW & Sounds: Draw...")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED && CLASS == classSound)
	{
		if (s1) s2 = OBJECT; else s1 = OBJECT;
	}
	praat_picture_open ();
	DTW_and_Sounds_draw (ONLY (classDTW), s2, s1, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish"));
	praat_picture_close ();
	return 1;
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
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED && CLASS == classSound)
	{
		if (s1) s2 = OBJECT; else s1 = OBJECT;
	}
	praat_picture_open ();
	DTW_and_Sounds_drawWarpX (ONLY (classDTW), s2, s1, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Time"), GET_INTEGER (L"Garnish"));
	praat_picture_close ();
	return 1;
END

void DTW_constraints_addCommonFields (void *dia)
{
	Any radio;
	LABEL (L"", L"Boundary conditions")
	BOOLEAN (L"Match begin positions", 0)
	BOOLEAN (L"Match end positions", 0)
	RADIO (L"Slope constraints", 1)
	RADIOBUTTON (L"no restriction")
	RADIOBUTTON (L"1/3 < slope < 3")
	RADIOBUTTON (L"1/2 < slope < 2")
	RADIOBUTTON (L"2/3 < slope < 3/2")
}

void DTW_constraints_getCommonFields (void *dia, int *begin, int *end, int *slope)
{
	*begin = GET_INTEGER (L"Match begin positions");
	*end = GET_INTEGER (L"Match end positions");
	*slope = GET_INTEGER (L"Slope constraints");
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
    EVERY_DRAW (DTW_drawPath (OBJECT, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish")))
END

FORM (DTW_drawDistancesAlongPath, L"DTW: Draw distances along path", 0)
    REAL (L"left Horizontal range", L"0.0")
    REAL (L"right Horizontal range", L"0.0")
    REAL (L"left Vertical range", L"0.0")
    REAL (L"right Vertical range", L"0.0")
    BOOLEAN (L"Garnish", 0);
    OK
DO
    EVERY_DRAW (DTW_drawDistancesAlongPath (OBJECT, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (DTW_paintDistances (OBJECT, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Minimum"), GET_REAL (L"Maximum"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (DTW_drawWarpX (OBJECT, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Time"), GET_INTEGER (L"Garnish")))
END

FORM (DTW_getPathY, L"DTW: Get time along path", L"DTW: Get time along path...")
	REAL (L"Time (s)", L"0.0")
	OK
DO
	Melder_information1 (Melder_double (DTW_getPathY (ONLY_OBJECT, GET_REAL (L"Time"))));
END

FORM (DTW_getYTime, L"DTW: Get y time", L"DTW: Get y time...")
	REAL (L"Time at x (s)", L"0.0")
	OK
DO
	Melder_information1 (Melder_double (DTW_getYTime (ONLY_OBJECT, GET_REAL (L"Time at x"))));
END

FORM (DTW_getXTime, L"DTW: Get x time", L"DTW: Get x time...")
	REAL (L"Time at y (s)", L"0.0")
	OK
DO
	Melder_information1 (Melder_double (DTW_getXTime (ONLY_OBJECT, GET_REAL (L"Time at y"))));
END

FORM (DTW_getMaximumConsecutiveSteps, L"DTW: Get maximum consecutive steps", L"DTW: Get maximum consecutive steps...")
	OPTIONMENU (L"Direction", 1)
	OPTION (L"X")
	OPTION (L"Y")
	OPTION (L"Diagonaal")
	OK
DO
	int direction[] = {DTW_START, DTW_X, DTW_Y, DTW_XANDY};
	wchar_t *string[] = {L"", L"x", L"y", L"diagonal"};
	int d = GET_INTEGER (L"Direction");
	Melder_information4 (Melder_integer (DTW_getMaximumConsecutiveSteps (ONLY_OBJECT, direction[d])),
		L" (=maximum number of consecutive steps in ", string[d], L" direction");
END

DIRECT (DTW_getWeightedDistance)
	DTW me = ONLY_OBJECT;
	Melder_information1 (Melder_double (my weightedDistance));
END

FORM (DTW_findPath, L"DTW: Find path", 0)
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	EVERY (DTW_findPath (OBJECT, begin, end, slope))
END

#ifdef INCLUDE_DTW_SLOPES
FORM (DTW_pathFinder_slopes, L"DTW: Find path (slopes)", L"DTW: Find path (slopes)...")
	LABEL (L"", L"Slope constraints:")
	LABEL (L"", L"This number of")
	INTEGER (L"Non-diagonal steps", L"1")
	LABEL (L"", L"must be followed by at least this number of")
	INTEGER (L"Diagonal steps", L"0 (=no constraints)")
	LABEL (L"", L"Directional weights")
	REAL (L"X weight", L"1.0")
	REAL (L"Y weight", L"1.0")
	REAL (L"Diagonal weight", L"2.0")
	OK
DO
	EVERY_CHECK (DTW_pathFinder_slopes (OBJECT, GET_INTEGER (L"Non-diagonal steps"), GET_INTEGER (L"Diagonal steps"),
		GET_REAL (L"X weight"), GET_REAL (L"Y weight"), GET_REAL (L"Diagonal weight")))
END
#endif

FORM (DTW_pathFinder_band, L"DTW: Find path (Sakoe-Chiba band)", L"DTW: Find path (band)...")
	REAL (L"Adjustment window duration (s)", L"0.1")
	BOOLEAN (L"Adjustment window includes end", 0)
	LABEL (L"", L"Directional weights")
	REAL (L"X weight", L"1.0")
	REAL (L"Y weight", L"1.0")
	REAL (L"Diagonal weight", L"2.0")
	OK
DO
	EVERY_CHECK (DTW_pathFinder_band (OBJECT, GET_REAL (L"Adjustment window duration"),
		GET_INTEGER (L"Adjustment window includes end"),
		GET_REAL (L"X weight"), GET_REAL (L"Y weight"), GET_REAL (L"Diagonal weight")))
END

FORM (DTW_to_Polygon_slopes, L"DTW: To Polygon (slopes)", L"DTW: To Polygon (slopes)...")
	LABEL (L"", L"Slope constraints:")
	LABEL (L"", L"This number of")
	INTEGER (L"Non-diagonal steps", L"1")
	LABEL (L"", L"must be followed by at least this number of")
	INTEGER (L"Diagonal steps", L"0 (=no constraints)")
	OK
DO
	EVERY_TO (DTW_to_Polygon_slopes (OBJECT, GET_INTEGER (L"Non-diagonal steps"), GET_INTEGER (L"Diagonal steps")))
END

FORM (DTW_to_Polygon_band, L"DTW: To Polygon (band)", L"DTW: To Polygon (band)...")
	REAL (L"Adjustment window duration (s)", L"0.1")
	BOOLEAN (L"Adjustment window includes end", 0)
	OK
DO
	EVERY_TO (DTW_to_Polygon_band (OBJECT, GET_REAL (L"Adjustment window duration"), GET_INTEGER (L"Adjustment window includes end")))
END

DIRECT (DTW_distancesToMatrix)
	EVERY_TO (DTW_distancesToMatrix (OBJECT))
END

DIRECT (DTW_swapAxes)
	EVERY_TO (DTW_swapAxes (OBJECT))
END

DIRECT (DTW_and_TextGrid_to_TextGrid)
	NEW (DTW_and_TextGrid_to_TextGrid (ONLY (classDTW), ONLY (classTextGrid)))
END
/******************** Eigen ********************************************/

DIRECT (Eigen_drawEigenvalues_scree)
	Melder_warning1 (L"The command \"Draw eigenvalues (scree)...\" has been "
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
	EVERY_DRAW (Eigen_drawEigenvalues (OBJECT, GRAPHICS,
		GET_INTEGER (L"left Eigenvalue range"), GET_INTEGER (L"right Eigenvalue range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
		GET_INTEGER (L"Fraction of eigenvalues summed"), GET_INTEGER (L"Cumulative"),
		GET_REAL (L"Mark size"),
		GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Eigen_drawEigenvector (OBJECT, GRAPHICS,
		GET_INTEGER (L"Eigenvector number"),
		GET_INTEGER (L"left Element range"), GET_INTEGER (L"right Element range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
		GET_INTEGER (L"Component loadings"), GET_REAL (L"Mark size"),
		GET_STRING (L"Mark string"), GET_INTEGER (L"Connect points"), NULL,
		GET_INTEGER (L"Garnish")))
END

DIRECT (Eigen_getNumberOfEigenvalues)
	Eigen me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfEigenvalues));
END

DIRECT (Eigen_getDimension)
	Eigen me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my dimension));
END

FORM (Eigen_getEigenvalue, L"Eigen: Get eigenvalue", L"Eigen: Get eigenvalue...")
	NATURAL (L"Eigenvalue number", L"1")
	OK
DO
	Eigen me = ONLY_OBJECT;
	long number = GET_INTEGER (L"Eigenvalue number");
	if (number > my numberOfEigenvalues) return Melder_error2 (L"DO_Eigen_getEigenvalue: Eigenvalue number must be smaller than ", Melder_integer(my numberOfEigenvalues + 1));
	Melder_information1 (Melder_double (my eigenvalues[number]));
END

FORM (Eigen_getSumOfEigenvalues, L"Eigen:Get sum of eigenvalues", L"Eigen: Get sum of eigenvalues...")
	INTEGER (L"left Eigenvalue range",  L"0")
	INTEGER (L"right Eigenvalue range", L"0")
	OK
DO
	Melder_information1 (Melder_double (Eigen_getSumOfEigenvalues(ONLY_OBJECT,
		GET_INTEGER (L"left Eigenvalue range"), GET_INTEGER (L"right Eigenvalue range"))));
END

FORM (Eigen_getEigenvectorElement, L"Eigen: Get eigenvector element", L"Eigen: Get eigenvector element...")
	NATURAL (L"Eigenvector number", L"1")
	NATURAL (L"Element number", L"1")
	OK
DO
	Melder_information1 (Melder_double (Eigen_getEigenvectorElement (ONLY_OBJECT,
		GET_INTEGER (L"Eigenvector number"), GET_INTEGER (L"Element number"))));
END

DIRECT (Eigens_alignEigenvectors)
	Ordered c = Ordered_create ();
	int status = 0;
	if (! c) return 0;
	WHERE (SELECTED)
	{
		if (! Collection_addItem (c, OBJECT)) goto end;
	}
	status = Eigens_alignEigenvectors (c);
end:
	c -> size = 0;
	forget (c);
	if (status == 0) return 0;
END

FORM (Eigen_and_Matrix_project, L"Eigen & Matrix: Project", L"Eigen & Matrix: Project...")
	INTEGER (L"Number of dimensions", L"0")
	OK
DO
	NEW (Eigen_and_Matrix_project (ONLY_GENERIC (classEigen),
		ONLY_GENERIC (classMatrix), GET_INTEGER (L"Number of dimensions")))
END

DIRECT (Eigen_and_SSCP_project)
	NEW (Eigen_and_SSCP_project (ONLY_GENERIC (classEigen),
		ONLY (classSSCP)))
END

DIRECT (Eigen_and_Covariance_project)
	NEW (Eigen_and_Covariance_project (ONLY_GENERIC (classEigen),
		ONLY (classCovariance)))
END

/******************** Index ********************************************/

DIRECT (Index_help)
	Melder_help (L"Index");
END

DIRECT (Index_getNumberOfClasses)
	Index thee = ONLY_OBJECT;
	Melder_information1 (Melder_integer (thy classes -> size));
END

FORM (StringsIndex_getClassLabel, L"StringsIndex: Get class label", L"StringsIndex: Get class label...")
	NATURAL (L"Class index", L"1")
	OK
DO
	StringsIndex thee = ONLY_OBJECT;
	long klas = GET_INTEGER (L"Class index");
	long numberOfClasses = thy classes -> size;
	SimpleString ss;
	if (klas > numberOfClasses) return Melder_error3 (L"Element index must be less than or equal ", Melder_integer (numberOfClasses), L".");
	ss = thy classes -> item[klas];
	Melder_information1 (ss -> string);
END

FORM (StringsIndex_getLabel, L"StringsIndex: Get label", L"StringsIndex: Get label...")
	NATURAL (L"Element index", L"1")
	OK
DO
	StringsIndex thee = ONLY_OBJECT;
	long klas, index = GET_INTEGER (L"Element index");
	SimpleString ss;
	if (index > thy numberOfElements) return Melder_error3 (L"Element index must be less than or equal ", Melder_integer (thy numberOfElements), L".");
	klas = thy classIndex[index];
	ss = thy classes -> item [klas];
	Melder_information1 (ss -> string);
END

FORM (Index_getIndex, L"Index: Get index", L"Index: Get index...")
	NATURAL (L"Element index", L"1")
	OK
DO
	Index thee = ONLY_OBJECT;
	long index = GET_INTEGER (L"Element index");
	if (index > thy numberOfElements) return Melder_error3 (L"Element index must be less than or equal ", Melder_integer (thy numberOfElements), L".");
	Melder_information1 (Melder_integer (thy classIndex[index]));
END

FORM (StringsIndex_getClassIndex, L"StringsIndex: Get class index", L"StringsIndex: Get class index...")
	WORD (L"Class label", L"label")
	OK
DO
	StringsIndex thee = ONLY_OBJECT;
	wchar_t *klasLabel = GET_STRING (L"Class label");
	long index = StringsIndex_getClass (thee, klasLabel);
	Melder_information1 (Melder_integer (index));
END

FORM (Index_extractPart, L"Index: Extract part", L"Index: Extract part...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	OK
DO
	Index thee = ONLY_OBJECT;
	if (! praat_new2 (Index_extractPart (thee, GET_INTEGER (L"left Range"), GET_INTEGER (L"right Range")),
		Thing_getName (thee), L"_part")) return 0;
END

FORM (Index_to_Permutation, L"Index: To Permutation", L"Index: To Permutation...")
	BOOLEAN (L"Permute within classes", 1)
	OK
DO
	EVERY_TO (Index_to_Permutation_permuteRandomly (OBJECT, GET_INTEGER (L"Permute within classes")))
END

DIRECT (StringsIndex_to_Strings)
	EVERY_TO (StringsIndex_to_Strings (OBJECT))
END

/******************** Excitation ********************************************/

DIRECT (Excitation_to_Excitations)
	Excitations e = Excitations_create (100);
	if (! e) return 0;
	WHERE_DOWN (SELECTED)
	{
		(void) Collection_addItem (e, Data_copy (OBJECT));
	}
	praat_show();
	NEW (e)
END

/******************** Excitations ********************************************/

FORM (Excitations_formula,L"Excitations: Formula", 0)
	LABEL (L"label", L"for all objects in Excitations do { for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	WHERE (SELECTED && Thing_member (OBJECT, classExcitations))
	{
		Ordered ms = OBJECT;
		int j;
		for (j = 1; j <= ms -> size; j++)
			if (! Matrix_formula (ms->item[j], GET_STRING (L"formula"), interpreter, NULL)) break;
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

DIRECT (Excitations_addItem)
	Excitations e = NULL;
	WHERE (SELECTED && CLASS == classExcitations) e = OBJECT;
	WHERE_DOWN (SELECTED && CLASS == classExcitation)
	{
		(void) Collection_addItem (e, Data_copy (OBJECT));
		praat_show();
	}
	praat_show();
END

FORM (Excitations_getItem, L"Excitations: Get item", 0)
	NATURAL (L"Item number", L"1")
	OK
DO
	WHERE (SELECTED && CLASS == classExcitations)
	{
		Excitation me = Excitations_getItem (OBJECT, GET_INTEGER (L"Item number"));
		if (me == NULL || ! praat_new1 (me, Thing_getName (me))) return 0;
	}
END

DIRECT (Excitations_append)
   Data e1 = NULL, e2 = NULL;
   WHERE (SELECTED && CLASS == classExcitations) { if (e1) e2 = OBJECT; else e1 = OBJECT; }
   NEW (Collections_merge (e1, e2))
END

FORM (Excitations_to_Pattern,L"Excitations: To Pattern", 0)
	NATURAL (L"Join", L"1")
	OK
DO
    EVERY_TO (Excitations_to_Pattern (OBJECT, GET_INTEGER (L"Join")))
END

DIRECT (Excitations_to_TableOfReal)
	EVERY_TO (Excitations_to_TableOfReal (OBJECT))
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
	EVERY_DRAW (Matrix_drawRows (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range")))
END

FORM (FilterBank_drawOneContour, L"FilterBank: Draw one contour", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Frequency range", L"0.0")
	REAL (L"right Frequency range", L"0.0")
	REAL (L"Height (dB)", L"40.0")
	OK
DO
	EVERY_DRAW (Matrix_drawOneContour (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"Height")))
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
	EVERY_DRAW (Matrix_drawContours (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range")))
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
	EVERY_DRAW (FilterBank_drawFrequencyScales (OBJECT, GRAPHICS,
		GET_INTEGER (L"Horizontal frequency scale"),
		GET_REAL (L"left Horizontal frequency range"),
		GET_REAL (L"right Horizontal frequency range"),
		GET_INTEGER (L"Vertical frequency scale"),
		GET_REAL (L"left Vertical frequency range"),
		GET_REAL (L"right Vertical frequency range"),GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Matrix_paintImage (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range")))
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
	EVERY_DRAW (Matrix_paintContours (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range")))
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
	EVERY_DRAW (Matrix_paintCells (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range")))
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
	EVERY_DRAW (Matrix_paintSurface (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
		30, 45))
END

FORM (FilterBank_getFrequencyInHertz, L"FilterBank: Get frequency in Hertz", L"FilterBank: Get frequency in Hertz...")
	REAL (L"Frequency", L"10.0")
	RADIO (L"Unit", 2)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	OK
DO
	double f = FilterBank_getFrequencyInHertz (ONLY_OBJECT,
		GET_REAL (L"Frequency"), GET_INTEGER (L"Unit"));
	Melder_informationReal (f, L"Hertz");
END

FORM (FilterBank_getFrequencyInBark, L"FilterBank: Get frequency in Bark", L"FilterBank: Get frequency in Bark...")
	REAL (L"Frequency", L"93.17")
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	OK
DO
	Melder_informationReal (FilterBank_getFrequencyInBark (ONLY_OBJECT, GET_REAL (L"Frequency"),
		GET_INTEGER (L"Unit")), L"Bark");
END

FORM (FilterBank_getFrequencyInMel, L"FilterBank: Get frequency in mel", L"FilterBank: Get frequency in mel...")
	REAL (L"Frequency", L"1000.0")
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIOBUTTON (L"mel")
	OK
DO
	double f = FilterBank_getFrequencyInMel (ONLY_OBJECT,
		GET_REAL (L"Frequency"), GET_INTEGER (L"Unit"));
	Melder_informationReal (f, L"mel");
END

FORM (FilterBank_equalizeIntensities, L"FilterBank: Equalize intensities", L"")
	REAL (L"Intensity (dB)", L"80.0")
	OK
DO
	EVERY (FilterBank_equalizeIntensities (OBJECT, GET_REAL (L"Intensity")))
END

DIRECT (FilterBank_to_Matrix)
	EVERY_TO (FilterBank_to_Matrix (OBJECT))
END

DIRECT (FilterBank_to_Intensity)
	EVERY_TO (FilterBank_to_Intensity (OBJECT))
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
	EVERY_DRAW (FormantFilter_drawFilterFunctions (OBJECT, GRAPHICS,
		GET_REAL (L"Bandwidth"), GET_INTEGER (L"Frequency scale"),
		GET_INTEGER (L"left Filter range"), GET_INTEGER (L"right Filter range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_INTEGER (L"Amplitude scale in dB"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (FilterBank_drawTimeSlice (OBJECT, GRAPHICS,
		GET_REAL (L"Time"), GET_REAL (L"left Frequency range"),
		GET_REAL (L"right Frequency range"), GET_REAL (L"left Amplitude range"),
		GET_REAL (L"right Amplitude range"), L"Hz", GET_INTEGER (L"Garnish")))
END

/****************** FormantGrid  *********************************/

FORM (FormantGrid_draw, L"FormantGrid: Draw", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (=all)")
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (=auto)")
	BOOLEAN (L"Bandwidths", false)
	BOOLEAN (L"Garnish", true)
	OK
DO
	EVERY_DRAW (FormantGrid_draw (OBJECT, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_INTEGER (L"Bandwidths"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (FunctionTerms_draw (OBJECT, GRAPHICS, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Extrapolate"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (FunctionTerms_drawBasisFunction (OBJECT, GRAPHICS,
		GET_INTEGER (L"Index"),
		GET_REAL (L"Xmin"), GET_REAL (L"Xmax"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Extrapolate"), GET_INTEGER (L"Garnish")))
END

FORM (FunctionTerms_evaluate, L"FunctionTerms: Evaluate", 0)
	REAL (L"X", L"0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	Melder_information1 (Melder_double (FunctionTerms_evaluate (f, GET_REAL (L"X"))));
END

DIRECT (FunctionTerms_getNumberOfCoefficients)
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	Melder_information1 (Melder_integer (f -> numberOfCoefficients));
END

FORM (FunctionTerms_getCoefficient, L"FunctionTerms: Get coefficient", 0)
	LABEL (L"", L"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	REQUIRE (index <= f -> numberOfCoefficients, L"Index too large.")
	Melder_information1 (Melder_double (f -> coefficients[index]));
END

DIRECT (FunctionTerms_getDegree)
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	Melder_information1 (Melder_integer (FunctionTerms_getDegree (f)));
END

FORM (FunctionTerms_getMaximum, L"FunctionTerms: Get maximum", L"Polynomial: Get maximum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getMaximum (f, GET_REAL (L"Xmin"),
		GET_REAL (L"Xmax"));
	Melder_information1 (Melder_double (x));
END

FORM (FunctionTerms_getMinimum, L"FunctionTerms: Get minimum", L"Polynomial: Get minimum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getMinimum (f, GET_REAL (L"Xmin"),
		GET_REAL (L"Xmax"));
	Melder_information1 (Melder_double (x));
END

FORM (FunctionTerms_getXOfMaximum, L"FunctionTerms: Get x of maximum", L"Polynomial: Get x of maximum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getXOfMaximum (f, GET_REAL (L"Xmin"),
		GET_REAL (L"Xmax"));
	Melder_information1 (Melder_double (x));
END

FORM (FunctionTerms_getXOfMinimum, L"FunctionTerms: Get x of minimum", L"Polynomial: Get x of minimum...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getXOfMinimum (f, GET_REAL (L"Xmin"),
		GET_REAL (L"Xmax"));
	Melder_information1 (Melder_double (x));
END

FORM (FunctionTerms_setCoefficient, L"FunctionTerms: Set coefficient", 0)
	LABEL (L"", L"p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	LABEL (L"", L"F[k] is of degree k")
	NATURAL (L"Index", L"1")
	REAL (L"Value", L"0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	if (! FunctionTerms_setCoefficient (f, GET_INTEGER (L"Index"),
		GET_REAL (L"Value"))) return 0;
END

FORM (FunctionTerms_setDomain, L"FunctionTerms: Set domain", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"2.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	REQUIRE (xmin < xmax, L"Xmax must be larger than Xmin.")
	FunctionTerms_setDomain (f, xmin, xmax);
END

/***************** Intensity ***************************************************/

FORM (Intensity_to_TextGrid_detectSilences, L"Intensity: To TextGrid (silences)", L"Intensity: To TextGrid (silences)...")
	REAL (L"Silence threshold (dB)", L"-25.0")
	POSITIVE (L"Minimum silent interval duration (s)", L"0.1")
	POSITIVE (L"Minimum sounding interval duration (s)", L"0.1")
	WORD (L"Silent interval label", L"silent")
	WORD (L"Sounding interval label", L"sounding")
	OK
DO
	EVERY_TO (Intensity_to_TextGrid_detectSilences (OBJECT, GET_REAL (L"Silence threshold"),
		GET_REAL (L"Minimum silent interval duration"), GET_REAL (L"Minimum sounding interval duration"),
		GET_STRING (L"Silent interval label"), GET_STRING (L"Sounding interval label")))
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
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	if (! praat_new1 (ISpline_createFromStrings (xmin, xmax, degree,
		GET_STRING (L"Coefficients"), GET_STRING (L"Interior knots")),
		GET_STRING (L"Name"))) return 0;
END

/******************* KlattTable  *********************************/

DIRECT (KlattTable_help) Melder_help (L"KlattTable"); END

DIRECT (KlattTable_createExample)
	if (! praat_new1 (KlattTable_createExample (), L"example")) return 0;
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
	REQUIRE (flutter >= 0 && flutter <= 100, L"Flutter must be between 0 and 100%")
	EVERY_TO (KlattTable_to_Sound (OBJECT, GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Synthesis model"),
		GET_INTEGER (L"Number of formants"), GET_REAL (L"Frame duration"), GET_INTEGER (L"Voicing source"),
		GET_REAL (L"Flutter percentage"), outputType))
END

FORM (KlattTable_to_KlattGrid, L"KlattTable: To KlattGrid", 0)
	POSITIVE (L"Frame duration (s)", L"0.002")
	OK
DO
	EVERY_TO (KlattTable_to_KlattGrid (OBJECT, GET_REAL (L"Frame duration")))
END

DIRECT (KlattTable_to_Table)
	EVERY_TO (KlattTable_to_Table (OBJECT))
END

DIRECT (Table_to_KlattTable)
	EVERY_TO (Table_to_KlattTable (OBJECT))
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
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	if (! praat_new1 (LegendreSeries_createFromString (xmin, xmax,
		GET_STRING (L"Coefficients")), GET_STRING (L"Name"))) return 0;
END

DIRECT (LegendreSeries_help) Melder_help (L"LegendreSeries"); END

DIRECT (LegendreSeries_to_Polynomial)
	EVERY_TO (LegendreSeries_to_Polynomial (OBJECT))
END
/********************* LongSound **************************************/

FORM_READ (LongSounds_appendToExistingSoundFile, L"LongSound: Append to existing sound file", 0, false)
	if (! pr_LongSounds_appendToExistingSoundFile (file)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoAiffFile, L"LongSound: Write to AIFF file", 0, L"aiff")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_AIFF)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoAifcFile, L"LongSound: Write to AIFC file", 0, L"aifc")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_AIFC)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoWavFile, L"LongSound: Write to WAV file", 0, L"wav")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_WAV)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoNextSunFile, L"LongSound: Write to NeXT/Sun file", 0, L"au")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoNistFile, L"LongSound: Write to NIST file", 0, L"nist")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_NIST)) return 0;
END

/******************* Matrix **************************************************/

FORM (Matrix_drawAsSquares,L"Matrix: Draw as squares", L"Matrix: Draw as squares...")
    REAL (L"left Horizontal range", L"0.0")
    REAL (L"right Horizontal range", L"0.0")
    REAL (L"left Vertical range", L"0.0")
    REAL (L"right Vertical range", L"0.0")
    BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Matrix_drawAsSquares (OBJECT, GRAPHICS,
    	GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
    	GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Matrix_drawDistribution (OBJECT, GRAPHICS,
    	GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
    	GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
    	GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"),
    	GET_INTEGER (L"Number of bins"),
    	GET_REAL (L"Minimum frequency"), GET_REAL (L"Maximum frequency"), 0,
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Matrix_drawDistribution (OBJECT, GRAPHICS,
    	GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
    	GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
    	GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"),
    	GET_INTEGER (L"Number of bins"),
    	GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), 1,
		GET_INTEGER (L"Garnish")))
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
	REQUIRE (scale > 0 && scale < 4, L"Illegal value for scale.")
	EVERY (Matrix_scale (OBJECT, scale))
END

DIRECT (Matrix_transpose)
	EVERY_TO (Matrix_transpose (OBJECT))
END

FORM (Matrix_solveEquation, L"Matrix: Solve equation", L"Matrix: Solve equation...")
	REAL (L"Tolerance", L"1.19e-7")
	OK
DO
	WHERE (SELECTED)
	{
		if (! praat_new2 (Matrix_solveEquation (OBJECT, GET_REAL (L"Tolerance")), NAMEW, L"_solution")) return 0;
	}
END

DIRECT (Matrix_Categories_to_TableOfReal)
	NEW (Matrix_and_Categories_to_TableOfReal (ONLY_GENERIC (classMatrix),
		ONLY (classCategories)))
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
    REQUIRE (x != 0 && y != 0, L"X and Y component must differ from 0.")
    EVERY_DRAW (Matrix_scatterPlot (OBJECT, GRAPHICS, x, y,
    	GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
    	GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
    	GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
		GET_INTEGER (L"Garnish")))
END

DIRECT (Matrix_to_Activation)
	EVERY_TO (Matrix_to_Activation (OBJECT))
END

FORM (Matrices_to_DTW, L"Matrices: To DTW", L"Matrix: To DTW...")
	LABEL (L"", L"Distance  between cepstral coefficients")
	REAL (L"Distance metric", L"2.0")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	Matrix m1 = NULL, m2 = NULL;
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	WHERE (SELECTED && Thing_member (OBJECT, classMatrix))
	{
		if (m1) m2 = OBJECT; else m1 = OBJECT;
	}
	NEW (Matrices_to_DTW (m1, m2, begin, end, slope, GET_REAL (L"Distance metric")))

END

FORM (Matrix_to_Pattern, L"Matrix: To Pattern", 0)
	NATURAL (L"Join", L"1")
	OK
DO
	EVERY_TO (Matrix_to_Pattern (OBJECT, GET_INTEGER (L"Join")))
END

/***** MATRIXFT *************/

DIRECT (Matrixft_getHighestFrequency)
	Matrix me = ONLY_OBJECT;
	Melder_information1 (Melder_double (my ymax));
END

DIRECT (Matrixft_getLowestFrequency)
	Matrix me = ONLY_OBJECT;
	Melder_information1 (Melder_double (my ymin));
END

DIRECT (Matrixft_getNumberOfFrequencies)
	Matrix me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my ny));
END

DIRECT (Matrixft_getFrequencyDistance)
	Matrix me = ONLY_OBJECT;
	Melder_information1 (Melder_double (my dy));
END

FORM (Matrixft_getFrequencyOfRow, L"Get frequency of row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	Melder_information1 (Melder_double (Matrix_rowToY (ONLY_OBJECT, GET_INTEGER (L"Row number"))));
END

FORM (Matrixft_getXofColumn, L"Get time of column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	Melder_information1 (Melder_double (Matrix_columnToX (ONLY_OBJECT, GET_INTEGER (L"Column number"))));
END

FORM (Matrixft_getValueInCell, L"Get value in cell", 0)
	POSITIVE (L"Time (s)", L"0.5")
	POSITIVE (L"Frequency", L"1")
	OK
DO
	Matrix me = ONLY_OBJECT;
	long row, col;
	double ta, t = GET_REAL (L"Time");
	double fa, f = GET_REAL (L"Frequency");
	REQUIRE (f>= my xmin && f <= my ymax, L"Frequency out of range.")
	REQUIRE (t>= my xmin && t <= my xmax, L"Time out of range.")
	col = Matrix_xToNearestColumn (me, t);
	if (col < 1) col = 1;
	if (col > my nx) col = my nx;
	row = Matrix_yToNearestRow (me, f);
	if (row < 1) row = 1;
	if (row > my ny) row = my ny;
	ta = Matrix_columnToX (me, col);
	fa = Matrix_rowToY (me, row);
	Melder_information6 (Melder_single (my z[row][col]), L" (delta t: ", Melder_double (ta - t), L" f: ",
		Melder_double (fa - f), L")");
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
	EVERY_DRAW (MelFilter_drawFilterFunctions (OBJECT, GRAPHICS,
		GET_INTEGER (L"Frequency scale"),
		GET_INTEGER (L"left Filter range"), GET_INTEGER (L"right Filter range"),
		GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
		GET_INTEGER (L"Amplitude scale in dB"),
		GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (FilterBank_drawTimeSlice (OBJECT, GRAPHICS,
		GET_REAL (L"Time"), GET_REAL (L"left Frequency range"),
		GET_REAL (L"right Frequency range"), GET_REAL (L"left Amplitude range"),
		GET_REAL (L"right Amplitude range"), L"Mels", GET_INTEGER (L"Garnish")))
END

FORM (MelFilter_to_MFCC, L"MelFilter: To MFCC", L"MelFilter: To MFCC...")
	NATURAL (L"Number of coefficients", L"12")
	OK
DO
	EVERY_TO (MelFilter_to_MFCC (OBJECT,
		GET_INTEGER (L"Number of coefficients")))
END

/**************** MFCC *******************************************/

DIRECT (MFCC_help)
	Melder_help (L"MFCC");
END

FORM (MFCC_to_MelFilter, L"MFCC: To MelFilter", L"MFCC: To MelFilter...")
	INTEGER (L"From coefficient", L"0")
	INTEGER (L"To coefficient", L"0")
	POSITIVE (L"Position of first filter (mel)", L"100.0")
	POSITIVE (L"Distance between filters (mel)", L"100.0")
	OK
DO
	EVERY_TO (MFCC_to_MelFilter (OBJECT, GET_INTEGER (L"From coefficient"),
		GET_INTEGER (L"To coefficient"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Distance between filters")))
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
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	if (! praat_new1 (MSpline_createFromStrings (xmin, xmax, degree,
		GET_STRING (L"Coefficients"), GET_STRING (L"Interior knots")),
		GET_STRING (L"Name"))) return 0;
END

DIRECT (MSpline_help) Melder_help (L"MSpline"); END

/********************** Pattern *******************************************/

DIRECT (Pattern_and_Categories_to_Discriminant)
	Pattern p = ONLY (classPattern);
	Categories c = ONLY (classCategories);
	if (! praat_new3 (Pattern_and_Categories_to_Discriminant (p, c),
		Thing_getName (p), L"_", Thing_getName (c))) return 0;
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
	EVERY_DRAW (Pattern_draw (OBJECT, GRAPHICS, GET_INTEGER (L"Pattern number"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish")))
END

FORM (Pattern_formula, L"Pattern: Formula", 0)
	LABEL (L"label", L"        y := 1; for row := 1 to nrow do { x := 1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + 1 } "
		"y := y + 1 }}")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	if (! praat_Fon_formula (dia, interpreter)) return 0;
END

FORM (Pattern_setValue, L"Pattern: Set value", L"Pattern: Set value...")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	REAL (L"New value", L"0.0")
	OK
DO
	WHERE (SELECTED) {
		Pattern me = OBJECT;
		long row = GET_INTEGER (L"Row number"), column = GET_INTEGER (L"Column number");
		REQUIRE (row <= my ny, L"Row number must not be greater than number of rows.")
		REQUIRE (column <= my nx, L"Column number must not be greater than number of columns.")
		my z [row] [column] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

DIRECT (Pattern_to_Matrix)
	NEW (Pattern_to_Matrix (ONLY (classPattern)))
END

/******************* PCA ******************************/

DIRECT (PCA_help)
	Melder_help (L"PCA");
END

DIRECT (hint_PCA_and_TableOfReal_to_Configuration)
	Melder_information1 (L"You can get principal components by selecting a PCA and a TableOfReal\n"
		"together and choosing \"To Configuration...\".");
END

DIRECT (hint_PCA_and_Covariance_Project)
	Melder_information1 (L"You can get a new Covariance object rotated to the directions of the direction vectors\n"
		" in the PCA object by selecting a PCA and a Covariance object together.");
END

DIRECT (hint_PCA_and_Configuration_to_TableOfReal_reconstruct)
	Melder_information1 (L"You can reconstruct the original TableOfReal as well as possible from\n"
		" the principal components in the Configuration and the direction vectors in the PCA object.");
END

FORM (PCA_and_TableOfReal_getFractionVariance,L"PCA & TableOfReal: Get fraction variance", L"PCA & TableOfReal: Get fraction variance...")
	NATURAL (L"left Principal component range", L"1")
	NATURAL (L"right Principal component range", L"1")
	OK
DO
	Melder_information1 (Melder_double (PCA_and_TableOfReal_getFractionVariance
		(ONLY (classPCA), ONLY_GENERIC (classTableOfReal),
		GET_INTEGER (L"left Principal component range"),
		GET_INTEGER (L"right Principal component range"))));
END

DIRECT (PCA_and_Configuration_to_TableOfReal_reconstruct)
	NEW (PCA_and_Configuration_to_TableOfReal_reconstruct (ONLY (classPCA),
		ONLY (classConfiguration)))
END

FORM (PCA_and_TableOfReal_to_Configuration, L"PCA & TableOfReal: To Configuration", L"PCA & TableOfReal: To Configuration...")
	INTEGER (L"Number of dimensions", L"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	REQUIRE (dimension >= 0, L"Number of dimensions must be greater equal zero.")
	NEW (PCA_and_TableOfReal_to_Configuration (ONLY (classPCA),
		ONLY_GENERIC (classTableOfReal), dimension))
END

FORM (PCA_getEqualityOfEigenvalues, L"PCA: Get equality of eigenvalues", L"PCA: Get equality of eigenvalues...")
	INTEGER (L"left Eigenvalue range", L"0")
	INTEGER (L"right Eigenvalue range", L"0")
	BOOLEAN (L"Conservative test", 0)
	OK
DO
	long ndf; double p, chisq;
	PCA_getEqualityOfEigenvalues (ONLY_OBJECT, GET_INTEGER (L"left Eigenvalue range"),
		GET_INTEGER (L"right Eigenvalue range"), GET_INTEGER (L"Conservative test"),
		&p, &chisq, &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

FORM (PCA_getNumberOfComponentsVAF, L"PCA: Get number of components (VAF)", L"PCA: Get number of components (VAF)...")
	POSITIVE (L"Variance fraction (0-1)", L"0.95")
	OK
DO
	double f = GET_REAL (L"Variance fraction");
	REQUIRE (f > 0 && f <= 1, L"The variance fraction must be in interval (0-1).")
	Melder_information1 (Melder_integer (Eigen_getDimensionOfFraction (ONLY_OBJECT, f)));
END

FORM (PCA_getFractionVAF, L"PCA: Get fraction variance accounted for", L"PCA: Get fraction variance accounted for...")
	NATURAL (L"left Principal component range", L"1")
	NATURAL (L"right Principal component range", L"1")
	OK
DO
	long from = GET_INTEGER (L"left Principal component range");
	long to = GET_INTEGER (L"right Principal component range");
	REQUIRE (from <= to, L"The second component must be greater than or equal to the first component.")
	Melder_information1 (Melder_double (Eigen_getCumulativeContributionOfComponents	(ONLY_OBJECT, from, to)));
END

FORM (PCA_invertEigenvector, L"PCA: Invert eigenvector", 0)
	NATURAL (L"Eigenvector number", L"1")
	OK
DO
	EVERY (Eigen_invertEigenvector (OBJECT, GET_INTEGER (L"Eigenvector number")))
END

FORM (PCA_to_TableOfReal_reconstruct1, L"PCA: To TableOfReal (reconstruct)", L"PCA: To TableOfReal (reconstruct 1)...")
	SENTENCE (L"Coefficients", L"1.0 1.0")
	OK
DO
	EVERY_TO (PCA_to_TableOfReal_reconstruct1 (OBJECT, GET_STRING (L"Coefficients")))
END

FORM (PCAs_to_Procrustes, L"PCA & PCA: To Procrustes", L"PCA & PCA: To Procrustes...")
	NATURAL (L"left Eigenvector range", L"1")
	NATURAL (L"right Eigenvector range", L"2")
	OK
DO
	long from = GET_INTEGER (L"left Eigenvector range");
	long to = GET_INTEGER (L"right Eigenvector range");
	PCA me = NULL, thee = NULL;
	WHERE (SELECTED) { if (me) thee = OBJECT; else me = OBJECT; }
	if (! praat_new3 (Eigens_to_Procrustes (me, thee, from, to), Thing_getName (me), L"_",
		Thing_getName (thee))) return 0;
END


DIRECT (PCAs_getAngleBetweenPc1Pc2Plane_degrees)
	PCA me = NULL, thee = NULL;
	WHERE (SELECTED) { if (me) thee = OBJECT; else me = OBJECT; }
	Melder_information2 (Melder_double (Eigens_getAngleBetweenEigenplanes_degrees (me, thee)),
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
	if (! identity && ! Permutation_permuteRandomly_inline (p, 0, 0))
	{
		forget (p); return 0;
	}
	if (! praat_new1 (p, GET_STRING (L"Name"))) return 0;
END

DIRECT (Permutation_getNumberOfElements)
	Permutation p = ONLY_OBJECT;
	Melder_information1 (Melder_integer (p -> numberOfElements));
END

FORM (Permutation_getValueAtIndex, L"Permutation: Get value", L"Permutation: Get value...")
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	Melder_information4 (Melder_integer (Permutation_getValueAtIndex (ONLY_OBJECT, index)), L" (value, at index = ",
		Melder_integer (index), L")");
END

FORM (Permutation_getIndexAtValue, L"Permutation: Get index", L"Permutation: Get index...")
	NATURAL (L"Value", L"1")
	OK
DO
	long value = GET_INTEGER (L"Value");
	Melder_information4 (Melder_integer (Permutation_getIndexAtValue (ONLY_OBJECT, value)), L" (index, at value = ",
		Melder_integer (value), L")");
END

DIRECT (Permutation_sort)
	Permutation_sort (ONLY_OBJECT);
	praat_dataChanged (ONLY_OBJECT);
END

FORM (Permutation_swapBlocks, L"Permutation: Swap blocks", L"Permutation: Swap blocks...")
	NATURAL (L"From", L"1")
	NATURAL (L"To", L"2")
	NATURAL (L"Block size", L"1")
	OK
DO
	if (! Permutation_swapBlocks (ONLY_OBJECT, GET_INTEGER (L"From"), GET_INTEGER (L"To"), GET_INTEGER (L"Block size"))) return 0;
	praat_dataChanged (ONLY_OBJECT);
END


FORM (Permutation_swapOneFromRange, L"Permutation: Swap one from range", L"Permutation: Swap one from range...")
	LABEL (L"", L"A randomly chosen element from ")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	LABEL (L"", L"is swapped with the element at")
	NATURAL (L"Index", L"1")
	BOOLEAN (L"Forbid same", 1)
	OK
DO
	if (! Permutation_swapOneFromRange (ONLY_OBJECT, GET_INTEGER (L"left Range"), GET_INTEGER (L"right Range"),
		GET_INTEGER (L"Index"), GET_INTEGER (L"Forbid same"))) return 0;
	praat_dataChanged (ONLY_OBJECT);
END

FORM (Permutation_permuteRandomly, L"Permutation: Permute randomly", L"Permutation: Permute randomly...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	OK
DO
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_permuteRandomly (p, GET_INTEGER (L"left Range"),
		GET_INTEGER (L"right Range")), Thing_getName (p), L"_randomly")) return 0;
END

FORM (Permutation_rotate, L"Permutation: Rotate", L"Permutation: Rotate...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	INTEGER (L"Step size", L"1")
	OK
DO
	Permutation p = ONLY_OBJECT;
	long step = GET_INTEGER (L"Step size");
	if (! praat_new3 (Permutation_rotate (p, GET_INTEGER (L"left Range"), GET_INTEGER (L"right Range"), step), Thing_getName (p), L"_rotate", Melder_integer (step))) return 0;
END

FORM (Permutation_reverse, L"Permutation: Reverse", L"Permutation: Reverse...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	OK
DO
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_reverse (p, GET_INTEGER (L"left Range"), GET_INTEGER (L"right Range")),
		Thing_getName (p), L"_reverse")) return 0;
END

FORM (Permutation_permuteBlocksRandomly, L"Permutation: Permute blocks randomly", L"Permutation: Permute randomly (blocks)...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	NATURAL (L"Block size", L"12")
	BOOLEAN (L"Permute within blocks", 1)
	BOOLEAN (L"No doublets", 0)
	OK
DO
	Permutation p = ONLY_OBJECT;
	long blocksize = GET_INTEGER (L"Block size");
	if (! praat_new3 (Permutation_permuteBlocksRandomly (p, GET_INTEGER (L"left Range"),
		GET_INTEGER (L"right Range"), blocksize, GET_INTEGER (L"Permute within blocks"), GET_INTEGER (L"No doublets")),
		Thing_getName (p), L"_blocks", Melder_integer(blocksize))) return 0;
END

FORM (Permutation_interleave, L"Permutation: Interleave", L"Permutation: Interleave...")
	INTEGER (L"left Range", L"0")
	INTEGER (L"right Range", L"0")
	NATURAL (L"Block size", L"12")
	INTEGER (L"Offset", L"0")
	OK
DO
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_interleave (ONLY_OBJECT, GET_INTEGER (L"left Range"), GET_INTEGER (L"right Range"),
		GET_INTEGER (L"Block size"), GET_INTEGER (L"Offset")), Thing_getName (p), L"_interleave")) return 0;
END

DIRECT (Permutation_invert)
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_invert (p), Thing_getName (p), L"_inverse")) return 0;
END

DIRECT (Permutations_multiply)
	long np = 0, n = 0;
	Permutation buf = NULL, thee = NULL;

	WHERE (SELECTED)
	{
		Permutation me = OBJECT;
		if (n == 0)
		{
			n = my numberOfElements;
		}
		else if (my numberOfElements != n)
		{
			return Melder_error1 (L"To apply a number of permutations they all must have the same number of elements.");
		}
		np += 1;
	}
	WHERE (SELECTED)
	{
		Permutation p = OBJECT;
		if (thee == NULL)
		{
			thee = Data_copy (p);
			if (thee == NULL) return 0;
		}
		else
		{
			long i;
			buf = Data_copy (thee);
			if (buf == NULL) goto end;
			for (i = 1; i <= n; i++)
			{
				thy p[i] = buf -> p[p -> p[i]];
			}
			forget (buf);
		}
	}
end:
	if (Melder_hasError ())
	{
		forget (thee); return 0;
	}
	if (! praat_new2 (thee, L"multiply", Melder_integer(np))) return 0;
END

FORM (Pitches_to_DTW, L"Pitches: To DTW", L"Pitches: To DTW...")
	REAL (L"Voiced-unvoiced costs", L"24.0")
	REAL (L"Time costs weight", L"10.0")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	Pitch p1 = NULL, p2 = NULL;
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	WHERE (SELECTED) { if (p1) p2 = OBJECT; else p1 = OBJECT; }
	if (! praat_new4 (Pitches_to_DTW (p1, p2, GET_REAL (L"Voiced-unvoiced costs"), GET_REAL (L"Time costs weight"), begin, end, slope), L"dtw_", p1 -> name, L"_", p2 -> name)) return 0;

END

FORM (PitchTier_to_Pitch, L"PitchTier: To Pitch", L"PitchTier: To Pitch...")
	POSITIVE (L"Step size", L"0.02")
	POSITIVE (L"Pitch floor", L"60.0")
	POSITIVE (L"Pitch ceiling", L"400.0")
	OK
DO
	EVERY_TO (PitchTier_to_Pitch (ONLY(classPitchTier), GET_REAL (L"Step size"),
		GET_REAL (L"Pitch floor"),GET_REAL (L"Pitch ceiling")))
END

/******************* Polygon & Categories *************************************/

FORM (Polygon_translate, L"Polygon: Translate", 0)
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	OK
DO
	Polygon_translate (ONLY(classPolygon), GET_REAL (L"X"), GET_REAL (L"Y"));
END

FORM (Polygon_rotate, L"Polygon: Rotate", 0)
	REAL (L"Angle (degrees)", L"0.0")
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	OK
DO
	Polygon_rotate (ONLY(classPolygon), GET_REAL (L"Angle"), GET_REAL (L"X"), GET_REAL (L"Y"));
END

FORM (Polygon_scale, L"Polygon: Scale polygon", 0)
	REAL (L"X", L"0.0")
	REAL (L"Y", L"0.0")
	OK
DO
	Polygon_scale (ONLY(classPolygon), GET_REAL (L"X"), GET_REAL (L"Y"));
END

FORM (Polygon_Categories_draw, L"Polygon & Categories: Draw", 0)
    REAL (L"left Horizontal range", L"0.0")
    REAL (L"right Horizontal range", L"0.0")
    REAL (L"left Vertical range", L"0.0")
    REAL (L"right Vertical range", L"0.0")
    BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Polygon_Categories_draw (ONLY(classPolygon),
		ONLY(classCategories),
		GRAPHICS, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish")))
END

DIRECT (Polygon_reverseX)
	Polygon_reverseX (ONLY(classPolygon));
END

DIRECT (Polygon_reverseY)
	Polygon_reverseY (ONLY(classPolygon));
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
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	if (! praat_new1 (Polynomial_createFromString (xmin, xmax,
		GET_STRING (L"Coefficients")), GET_STRING (L"Name"))) return 0;
END

FORM (Polynomial_getArea, L"Polynomial: Get area", L"Polynomial: Get area...")
	LABEL (L"", L"Interval")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	OK
DO
	double area = Polynomial_getArea (ONLY (classPolynomial),
		GET_REAL (L"Xmin"), GET_REAL (L"Xmax"));
	Melder_information1 (Melder_double (area));
END

DIRECT (Polynomial_getDerivative)
	EVERY_TO (Polynomial_getDerivative (OBJECT))
END

DIRECT (Polynomial_getPrimitive)
	EVERY_TO (Polynomial_getPrimitive (OBJECT))
END

FORM (Polynomial_scaleX, L"Polynomial: Scale x", L"Polynomial: Scale x...")
	LABEL (L"", L"New domain")
	REAL (L"Xmin", L"-1.0")
	REAL (L"Xmax", L"1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	EVERY_TO (Polynomial_scaleX (OBJECT, xmin, xmax))
END

DIRECT (Polynomial_scaleCoefficients_monic)
	EVERY (Polynomial_scaleCoefficients_monic (OBJECT))
END

DIRECT (Polynomial_to_Roots)
	EVERY_TO (Polynomial_to_Roots (OBJECT))
END

FORM (Polynomial_evaluate_z, L"Polynomial: Get value (complex)", L"Polynomial: Get value (complex)...")
	REAL (L"Real part", L"0.0")
	REAL (L"Imaginary part", L"0.0")
	OK
DO
	dcomplex p, z = dcomplex_create (GET_REAL (L"Real part"), GET_REAL (L"Imaginary part"));
	Polynomial_evaluate_z (ONLY_OBJECT, &z, &p);
	Melder_information4 (Melder_double (p.re), L" + ", Melder_double (p.im), L" i");
END


FORM (Polynomial_to_Spectrum, L"Polynomial: To Spectrum", L"Polynomial: To Spectrum...")
	POSITIVE (L"Nyquist frequency (Hz)", L"5000.0")
	NATURAL (L"Number of frequencies (>1)", L"1025")
	OK
DO
	long n = GET_INTEGER (L"Number of frequencies");
	REQUIRE (n > 1, L"\"Number of frequencies\" must be greater than 1.")
	EVERY_TO (Polynomial_to_Spectrum (OBJECT, GET_REAL (L"Nyquist frequency"),
		n, 1.0))
END

DIRECT (Polynomials_multiply)
	Polynomial p1 = NULL, p2 = NULL;
	WHERE (SELECTED) { if (p1) p2 = OBJECT; else p1 = OBJECT; }
	if (! praat_new4 (Polynomials_multiply (p1, p2), p1->name, L"_", p2->name, L"_mul")) return 0;
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
	Polynomial p1 = NULL, p2 = NULL, s, r, q;
	int wantq = GET_INTEGER (L"Want quotient");
	int wantr = GET_INTEGER (L"Want remainder");
	REQUIRE (wantq || wantr, L"Either \'Want quotient\' or \'Want remainder\' must be chosen")
	WHERE (SELECTED) { if (p1) p2 = OBJECT; else p1 = OBJECT; }
	if (! wantq) q = NULL;
	if (! wantr) r = NULL;
	s = Polynomial_create (0,1,1);
	forget (s);
	if (! Polynomials_divide (p1, p2, &q, &r)) return 0;
	if (wantq && ! praat_new2 (q, p1 -> name, L"_q")) return 0;
	if (wantr && ! praat_new2 (r, p1 -> name, L"_r")) return 0;
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
	EVERY_DRAW (Roots_draw (OBJECT, GRAPHICS,
		GET_REAL (L"Minimum of real axis"), GET_REAL (L"Maximum of real axis"),
		GET_REAL (L"Minimum of imaginary axis"),
		GET_REAL (L"Maximum of imaginary axis"),
		GET_STRING (L"Mark string"), GET_INTEGER (L"Mark size"),
		GET_INTEGER (L"Garnish")))
END

DIRECT (Roots_getNumberOfRoots)
	Melder_information1 (Melder_integer (Roots_getNumberOfRoots (ONLY (classRoots))));
END

FORM (Roots_getRoot, L"Roots: Get root", 0)
	NATURAL (L"Root number", L"1")
	OK
DO
	dcomplex z = Roots_getRoot (ONLY (classRoots), GET_INTEGER (L"Root number"));
	Melder_information4 (Melder_double (z.re), (z.im < 0 ? L" - " : L" + "), Melder_double (fabs(z.im)), L" i");
END

FORM (Roots_getRealPartOfRoot, L"Roots: Get real part", 0)
	NATURAL (L"Root number", L"1")
	OK
DO
	dcomplex z = Roots_getRoot (ONLY (classRoots), GET_INTEGER (L"Root number"));
	Melder_information1 (Melder_double (z.re));
END

FORM (Roots_getImaginaryPartOfRoot, L"Roots: Get imaginary part", 0)
	NATURAL (L"Root number", L"1")
	OK
DO
	dcomplex z = Roots_getRoot (ONLY (classRoots), GET_INTEGER (L"Root number"));
	Melder_information1 (Melder_double (z.im));
END

FORM (Roots_setRoot, L"Roots: Set root", 0)
	NATURAL (L"Root number", L"1")
	REAL (L"Real part", L"1.0/sqrt(2)")
	REAL (L"Imaginary part", L"1.0/sqrt(2)")
	OK
DO
	if (! Roots_setRoot (ONLY_OBJECT, GET_INTEGER (L"Root number"),
		GET_REAL (L"Real part"), GET_REAL (L"Imaginary part"))) return 0;
END

FORM (Roots_to_Spectrum, L"Roots: To Spectrum", L"Roots: To Spectrum...")
	POSITIVE (L"Nyquist frequency (Hz)", L"5000.0")
	NATURAL (L"Number of frequencies (>1)", L"1025")
	OK
DO
	long n = GET_INTEGER (L"Number of frequencies");
	REQUIRE (n > 1, L"\"Number of frequencies\" must be greater than 1.")
	EVERY_TO (Roots_to_Spectrum (OBJECT, GET_REAL (L"Nyquist frequency"),
		n, 1.0))
END

DIRECT (Roots_and_Polynomial_polish)
	 Roots_and_Polynomial_polish (ONLY(classRoots), ONLY(classPolynomial));
END

/*****************************************************************************/

DIRECT (Praat_ReportFloatingPointProperties)
	if (! NUMfpp) NUMmachar ();
	MelderInfo_open ();
	MelderInfo_writeLine1 (L"Double precision floating point properties of this machine,");
	MelderInfo_writeLine1 (L"as calculated by algorithms from the Binary Linear Algebra System (BLAS)");
	MelderInfo_writeLine2 (L"Radix: ", Melder_double (NUMfpp -> base));
	MelderInfo_writeLine2 (L"Number of digits in mantissa: ", Melder_double (NUMfpp -> t));
	MelderInfo_writeLine2 (L"Smallest exponent before (gradual) underflow (expmin): ", Melder_integer (NUMfpp -> emin));
	MelderInfo_writeLine2 (L"Largest exponent before overflow (expmax): ", Melder_integer (NUMfpp -> emax));
	MelderInfo_writeLine2 (L"Does rounding occur in addition: ", (NUMfpp -> rnd == 1 ? L"yes" : L"no"));
	MelderInfo_writeLine2 (L"Quantization step (d): ", Melder_double (NUMfpp -> prec));
	MelderInfo_writeLine2 (L"Quantization error (eps = d/2): ", Melder_double (NUMfpp -> eps));
	MelderInfo_writeLine2 (L"Underflow threshold (= radix ^ (expmin - 1)): ", Melder_double (NUMfpp -> rmin));
	MelderInfo_writeLine2 (L"Safe minimum (such that its inverse does not overflow): ", Melder_double (NUMfpp -> sfmin));
	MelderInfo_writeLine2 (L"Overflow threshold (= (1 - eps) * radix ^ expmax): ", Melder_double (NUMfpp -> rmax));
	MelderInfo_close ();
END

/******************** Sound ****************************************/

static void Sound_create_addCommonFields (void *dia)
{
	REAL (L"Starting time (s)", L"0.0")
	REAL (L"Finishing time (s)", L"0.1")
	POSITIVE (L"Sampling frequency (Hz)", L"22050.0")
}

static int Sound_create_checkCommonFields (void *dia, double *startingTime, double *finishingTime,
	double *samplingFrequency)
{
	double numberOfSamples_real;
	*startingTime = GET_REAL (L"Starting time");
	*finishingTime = GET_REAL (L"Finishing time");
	*samplingFrequency = GET_REAL (L"Sampling frequency");
	numberOfSamples_real = floor ((*finishingTime - *startingTime) * *samplingFrequency + 0.5);
	if (*finishingTime <= *startingTime)
	{
		if (*finishingTime == *startingTime)
			(void) Melder_error1 (L"A Sound cannot have a duration of zero.\n");
		else
			(void) Melder_error1 (L"A Sound cannot have a duration less than zero.\n");
		if (*startingTime == 0.0)
			return Melder_error1 (L"Please set the finishing time to something greater than 0 seconds.");
		else
			return Melder_error1 (L"Please lower the starting time or raise the finishing time.");
	}
	if (*samplingFrequency <= 0.0)
	{
		(void) Melder_error1 (L"A Sound cannot have a negative sampling frequency.\n");
		return Melder_error1 (L"Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");
	}
	if (numberOfSamples_real < 1.0)
	{
		(void) Melder_error1 (L"A Sound cannot have zero samples.\n");
		if (*startingTime == 0.0)
			return Melder_error1 (L"Please raise the finishing time.");
		else
			return Melder_error1 (L"Please lower the starting time or raise the finishing time.");
	}
	if (numberOfSamples_real > LONG_MAX)
	{
		(void) Melder_error5 (L"A Sound cannot have ", Melder_bigInteger (numberOfSamples_real), L" samples; the maximum is ", Melder_bigInteger (LONG_MAX), L" samples.\n");
		if (*startingTime == 0.0)
			return Melder_error1 (L"Please lower the finishing time or the sampling frequency.");
		else
			return Melder_error1 (L"Please raise the starting time, lower the finishing time, or lower the sampling frequency.");
	}
	return 1;
}

static int Sound_create_check (Sound me, double startingTime, double finishingTime, double samplingFrequency)
{
	if (me != NULL) return 1;

	if (wcsstr (Melder_getError (), L"memory"))
	{
		double numberOfSamples_real = floor ((finishingTime - startingTime) * samplingFrequency + 0.5);
		Melder_clearError ();
		(void) Melder_error3 (L"There is not enough memory to create a Sound that contains ", Melder_bigInteger (numberOfSamples_real), L" samples.\n");
		if (startingTime == 0.0)
			return Melder_error1 (L"You could lower the finishing time or the sampling frequency and try again.");
		else
			return Melder_error1 (L"You could raise the starting time or lower the finishing time or the sampling frequency, and try again.");
	}

	return 0;
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
	 if (! praat_new1 (Sound_and_Pitch_to_FormantFilter (ONLY(classSound),
	 	ONLY(classPitch), GET_REAL (L"Analysis window duration"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters"),
		GET_REAL (L"Relative bandwidth")), NULL)) return 0;
END

FORM (Sound_and_Pitch_changeGender, L"Sound & Pitch: Change gender", L"Sound & Pitch: Change gender...")
	POSITIVE (L"Formant shift ratio", L"1.2")
	REAL (L"New pitch median (Hz)", L"0.0 (=no change)")
	POSITIVE (L"Pitch range factor", L"1.0 (=no change)")
	POSITIVE (L"Duration factor", L"1.0")
	OK
DO
	if (! praat_new1 (Sound_and_Pitch_changeGender_old (ONLY(classSound), ONLY(classPitch),
		GET_REAL (L"Formant shift ratio"), GET_REAL (L"New pitch median"),
		GET_REAL (L"Pitch range factor"), GET_REAL (L"Duration factor")), NULL)) return 0;
END

FORM (Sound_and_Pitch_changeSpeaker, L"Sound & Pitch: Change speaker", L"Sound & Pitch: Change speaker...")
	POSITIVE (L"Multiply formants by", L"1.1 (male->female)")
	POSITIVE (L"Multiply pitch by", L"1.8 (male->female")
	REAL (L"Multiply pitch range by", L"1.0 (=no change)")
	POSITIVE (L"Multiply duration", L"1.0")
	OK
DO
	if (! praat_new1 (Sound_and_Pitch_changeSpeaker (ONLY(classSound), ONLY(classPitch),
		GET_REAL (L"Multiply formants by"), GET_REAL (L"Multiply pitch by"),
		GET_REAL (L"Multiply pitch range by"), GET_REAL (L"Multiply duration")), NULL)) return 0;
END

FORM (Sound_createFromGammaTone, L"Create a gamma-tone", L"Create Sound from gamma-tone...")
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
	Sound sound = NULL;
	double startingTime, finishingTime, samplingFrequency;
	long gamma = GET_INTEGER (L"Gamma");
	double bandwidth = GET_REAL (L"Bandwidth");
	double f = GET_REAL (L"Frequency");

	if (! Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency))
		return 0;
	if (f >= samplingFrequency / 2) return Melder_error2
		(L"Frequency cannot be larger than half the sampling frequency.\n"
			"Please use a frequency smaller than ", Melder_double (samplingFrequency / 2));
	if (gamma < 0) return Melder_error1 (L"Gamma cannot be negative.\nPlease use a positive or zero gamma.");
	if (bandwidth < 0) return Melder_error1 (L"Bandwidth cannot be negative.\nPlease use a positive or zero bandwidth.");
	sound = Sound_createGammaTone (startingTime, finishingTime, samplingFrequency, gamma, f,
		bandwidth, GET_REAL (L"Initial phase"), GET_REAL (L"Addition factor"),
		GET_INTEGER (L"Scale amplitudes"));
	if (! Sound_create_check (sound, startingTime, finishingTime, samplingFrequency) ||
		! praat_new1 (sound, GET_STRING (L"Name"))) return 0;
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
	Sound sound = NULL;
	double startingTime, finishingTime, samplingFrequency;
	double amplitudeRange = GET_REAL (L"Amplitude range");
	double octaveShiftFraction = GET_REAL (L"Octave shift fraction");
	if (! Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency)) return 0;
	if (amplitudeRange < 0) return Melder_error1 (L"Amplitude range cannot be negative.\nPlease use a positive or zero amplitude range.");
	sound = Sound_createShepardToneComplex (startingTime, finishingTime, samplingFrequency,
		GET_REAL (L"Lowest frequency"), GET_INTEGER (L"Number of components"),
		GET_REAL (L"Frequency change"), GET_REAL (L"Amplitude range"), octaveShiftFraction);

	if (! Sound_create_check (sound, startingTime, finishingTime, samplingFrequency) ||
		! praat_new1 (sound, GET_STRING (L"Name"))) return 0;
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
	EVERY_DRAW (Sound_drawWhere (OBJECT, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"),
	GET_STRING (L"Drawing method"), numberOfBisections, GET_STRING (L"Formula"), interpreter))
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
	EVERY_TO (Sound_to_TextGrid_detectSilences (OBJECT, GET_REAL (L"Minimum pitch"), GET_REAL (L"Time step"),
		GET_REAL (L"Silence threshold"), GET_REAL (L"Minimum silent interval duration"),
		GET_REAL (L"Minimum sounding interval duration"), GET_STRING (L"Silent interval label"),
		GET_STRING (L"Sounding interval label")))

END

FORM (Sound_to_BarkFilter, L"Sound: To BarkFilter", L"Sound: To BarkFilter...")
	POSITIVE (L"Analysis window duration (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (bark)", L"1.0")
	POSITIVE (L"Distance between filters (bark)", L"1.0")
	REAL (L"Maximum frequency (bark)", L"0");
	OK
DO
	EVERY_TO (Sound_to_BarkFilter (OBJECT, GET_REAL (L"Analysis window duration"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters")))
END

FORM (Sound_to_FormantFilter, L"Sound: To FormantFilter", L"Sound: To FormantFilter...")
	POSITIVE (L"Analysis window duration (s)", L"0.015")
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
	EVERY_TO (Sound_to_FormantFilter (OBJECT, GET_REAL (L"Analysis window duration"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters"),
		GET_REAL (L"Relative bandwidth"), GET_REAL (L"Minimum pitch"),
		GET_REAL (L"Maximum pitch")))
END

FORM (Sound_to_MelFilter, L"Sound: To MelFilter", L"Sound: To MelFilter...")
	POSITIVE (L"Analysis window duration (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (mel)", L"100.0")
	POSITIVE (L"Distance between filters (mel)", L"100.0")
	REAL (L"Maximum frequency (mel)", L"0.0");
	OK
DO
	EVERY_TO (Sound_to_MelFilter (OBJECT, GET_REAL (L"Analysis window duration"),
		GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters")))
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
	REQUIRE (minimumPitch < ceiling, L"Minimum pitch should be smaller than ceiling.")
	REQUIRE (ceiling <= fmax, L"Maximum frequency must be greater than or equal to ceiling.")
	EVERY_TO (Sound_to_Pitch_shs (OBJECT, GET_REAL (L"Time step"),
		minimumPitch, fmax, ceiling,
		GET_INTEGER (L"Max. number of subharmonics"),
		GET_INTEGER (L"Max. number of candidates"),
		GET_REAL (L"Compression factor"),
		GET_INTEGER (L"Number of points per octave")))
END

FORM (Sound_fadeIn, L"Sound: Fade in", L"Sound: Fade in...")
	OPTIONMENU (L"Channel", 1)
	OPTION (L"All")
	OPTION (L"Left")
	OPTION (L"Right")
	REAL (L"Time (s)", L"-10000.0")
	REAL (L"Fade time (s)", L"0.005")
	BOOLEAN (L"Silent from start", 0)
	OK
DO
	long channel = GET_INTEGER (L"Channel") - 1;
	WHERE (SELECTED)
	{
		Sound_fade (OBJECT, channel, GET_REAL (L"Time"), GET_REAL (L"Fade time"), -1, GET_INTEGER (L"Silent from start"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_fadeOut, L"Sound: Fade out", L"Sound: Fade out...")
	OPTIONMENU (L"Channel", 1)
	OPTION (L"All")
	OPTION (L"Left")
	OPTION (L"Right")
	REAL (L"Time (s)", L"10000.0")
	REAL (L"Fade time (s)", L"-0.005")
	BOOLEAN (L"Silent to end", 0)
	OK
DO
	long channel = GET_INTEGER (L"Channel") - 1;
	WHERE (SELECTED)
	{
		Sound_fade (OBJECT, channel, GET_REAL (L"Time"), GET_REAL (L"Fade time"), 1, GET_INTEGER (L"Silent to end"));
		praat_dataChanged (OBJECT);
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
	WHERE (SELECTED)
	{
		if (! praat_new1 (Sound_to_KlattGrid_simple (OBJECT, GET_REAL (L"Time step"),
			GET_INTEGER (L"Max. number of formants"), GET_REAL (L"Maximum formant"),
			GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis from"),
			GET_REAL (L"Pitch floor"), GET_REAL (L"Pitch ceiling"),
			GET_REAL (L"Minimum pitch"), GET_INTEGER (L"Subtract mean")), FULL_NAME)) return 0;
	}
END

FORM (Sound_to_Pitch_SPINET, L"Sound: To SPINET", L"Sound: To SPINET...")
	POSITIVE (L"Time step (s)", L"0.005")
	POSITIVE (L"Analysis window duration (s)", L"0.040")
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
	REQUIRE (fmax > fmin, L"Maximum frequency must be larger than minimum frequency.")
	EVERY_TO (Sound_to_Pitch_SPINET (OBJECT, GET_REAL (L"Time step"),
		GET_REAL (L"Analysis window duration"),
		fmin, fmax, GET_INTEGER (L"Number of filters"),
		GET_REAL (L"Ceiling"), GET_INTEGER (L"Max. number of candidates")))
END

FORM (Sound_filterByGammaToneFilter4, L"Sound: Filter (gammatone)", L"Sound: Filter (gammatone)...")
	POSITIVE (L"Centre frequency (Hz)", L"1000.0")
	POSITIVE (L"Bandwidth (Hz)", L"150.0")
	OK
DO
	EVERY_TO (Sound_filterByGammaToneFilter4 (OBJECT,
		GET_REAL (L"Centre frequency"), GET_REAL (L"Bandwidth")))
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
	REQUIRE (minimumPitch < maximumPitch, L"Maximum pitch should be greater than minimum pitch.")
	EVERY_TO (Sound_changeSpeaker (OBJECT, minimumPitch, maximumPitch,
		GET_REAL (L"Multiply formants by"), GET_REAL (L"Multiply pitch by"),
		GET_REAL (L"Multiply pitch range by"), GET_REAL (L"Multiply duration by")))
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
	REQUIRE (minimumPitch < maximumPitch, L"Maximum pitch should be greater than minimum pitch.")
	REQUIRE (pitchrf >= 0, L"Pitch range factor may not be negative")
	EVERY_TO (Sound_changeGender_old (OBJECT, minimumPitch, maximumPitch,
		GET_REAL (L"Formant shift ratio"), GET_REAL (L"New pitch median"),
		pitchrf, GET_REAL (L"Duration factor")))
END

FORM_READ (Sound_readFromRawFileLE, L"Read Sound from raw Little Endian file", 0, true)
	if (! praat_new1 (Sound_readFromRawFile (file, NULL, 16, 1, 0, 0,
		16000), MelderFile_name (file))) return 0;
END

FORM_READ (Sound_readFromRawFileBE, L"Read Sound from raw 16-bit Little Endian file", 0, true)
	if (! praat_new1 (Sound_readFromRawFile (file, NULL, 16, 0, 0, 0,
		16000), MelderFile_name (file))) return 0;
END

FORM_READ (KlattTable_readFromRawTextFile, L"KlattTable_readFromRawTextFile", 0, true)
	if (! praat_new1 (KlattTable_readFromRawTextFile (file), MelderFile_name (file))) return 0;
END

FORM_WRITE (Sound_writeToRawFileBE, L"Sound: Write to raw 16-bit Big Endian file", 0, L"raw")
	if (! Sound_writeToRawFile (ONLY_OBJECT, file, 0, 0, 16, 0)) return 0;
END

FORM_WRITE (Sound_writeToRawFileLE, L"Sound: Write to raw 16-bit Little Endian file", 0, L"raw")
	if (! Sound_writeToRawFile (ONLY_OBJECT, file, 0, 1, 16, 0)) return 0;
END

/************ Spectrograms *********************************************/

FORM (Spectrograms_to_DTW, L"Spectrograms: To DTW", 0)
	DTW_constraints_addCommonFields (dia);
	OK
DO
	Spectrogram s1 = NULL, s2 = NULL;
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	WHERE (SELECTED && CLASS == classSpectrogram)
	{
		if (s1) s2 = OBJECT; else s1 = OBJECT;
	}
	NEW (Spectrograms_to_DTW (s1, s2, begin, end, slope, 1))
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
	EVERY_DRAW (Spectrum_drawPhases (OBJECT, GRAPHICS,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		GET_REAL (L"Minimum phase"), GET_REAL (L"Maximum phase"),
		GET_INTEGER (L"Unwrap"), GET_INTEGER (L"Garnish")))
END

DIRECT (Spectrum_conjugate)
	WHERE (SELECTED)
	{
		Spectrum_conjugate (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Spectra_multiply)
	Spectrum s1 = NULL, s2 = NULL;
	WHERE (SELECTED && CLASS == classSpectrum)
	{
		if (s1) s2 = OBJECT; else s1 = OBJECT;
	}
	NEW (Spectra_multiply (s1, s2))
END

DIRECT (Spectrum_unwrap)
	EVERY_TO (Spectrum_unwrap (OBJECT))
END

DIRECT (Spectrum_to_Cepstrum)
	EVERY_TO (Spectrum_to_Cepstrum (OBJECT))
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
	EVERY_DRAW (Spline_drawKnots (OBJECT, GRAPHICS,
		GET_REAL (L"Xmin"), GET_REAL (L"Xmax"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Garnish")))
END

DIRECT (Spline_getOrder)
	Melder_information1 (Melder_integer (Spline_getOrder (ONLY_OBJECT)));
END

FORM (Spline_scaleX, L"Spline: Scale x", L"Spline: Scale x...")
	LABEL (L"", L"New domain")
	REAL (L"Xmin", L"-1.0")
	REAL (L"Xmax", L"1.0")
	OK
DO
	double xmin = GET_REAL (L"Xmin"), xmax = GET_REAL (L"Xmax");
	REQUIRE (xmin < xmax, L"Xmin must be smaller than Xmax.")
	EVERY_TO (Spline_scaleX (OBJECT, xmin, xmax))
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
	EVERY_DRAW (SSCP_drawConcentrationEllipse (OBJECT, GRAPHICS,
		GET_REAL (L"Confidence level"), 1,
		GET_INTEGER (L"Index for X-axis"), GET_INTEGER (L"Index for Y-axis"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
		GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (SSCP_drawConcentrationEllipse (OBJECT, GRAPHICS,
		GET_REAL (L"Number of sigmas"), 0,
		GET_INTEGER (L"Index for X-axis"), GET_INTEGER (L"Index for Y-axis"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish")))
END

DIRECT (SSCP_extractCentroid)
	EVERY_CHECK (praat_new2 (SSCP_extractCentroid (OBJECT),
		Thing_getName (OBJECT), L"_centroid"))
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
	Melder_information1 (Melder_double (SSCP_getConcentrationEllipseArea (ONLY_OBJECT, conf, 1, d1, d2)));
END

FORM (SSCP_getFractionVariation, L"SSCP: Get fraction variation", L"SSCP: Get fraction variation...")
	NATURAL (L"From dimension", L"1")
	NATURAL (L"To dimension", L"1")
	OK
DO
	Melder_information1 (Melder_double (SSCP_getFractionVariation (ONLY_OBJECT,
		GET_INTEGER (L"From dimension"), GET_INTEGER (L"To dimension"))));
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
	Melder_information1 (Melder_double (SSCP_getConcentrationEllipseArea (ONLY_OBJECT,
		nsigmas, 0, d1, d2)));
END

DIRECT (SSCP_getDegreesOfFreedom)
	Melder_information1 (Melder_double (SSCP_getDegreesOfFreedom (ONLY_OBJECT)));
END

DIRECT (SSCP_getNumberOfObservations)
	SSCP me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfObservations));
END

DIRECT (SSCP_getTotalVariance)
	Melder_information1 (Melder_double (SSCP_getTotalVariance (ONLY_OBJECT)));
END

FORM (SSCP_getCentroidElement, L"SSCP: Get centroid element", L"SSCP: Get centroid element")
	NATURAL (L"Number", L"1")
	OK
DO
	SSCP me = ONLY_OBJECT;
	long number = GET_INTEGER (L"Number");
	if (number < 1 || number > my numberOfColumns)
	{
		return Melder_error3 (L"SSCP_getCentroidElement: \"Number\" must be smaller than ", Melder_integer (my numberOfColumns + 1), L".");
	}
	Melder_information1 (Melder_double (my centroid[number]));
END

DIRECT (SSCP_getLnDeterminant)
	Melder_information1 (Melder_double (SSCP_getLnDeterminant (ONLY_OBJECT)));
END

FORM (SSCP_testDiagonality_bartlett, L"SSCP: Get diagonality (bartlett)", L"SSCP: Get diagonality (bartlett)...")
	NATURAL (L"Number of contraints", L"1")
	OK
DO
	double chisq, p;
	long nc = GET_INTEGER (L"Number of contraints");
	SSCP me = ONLY_OBJECT;
	SSCP_testDiagonality_bartlett (me, nc, &chisq, &p);
	Melder_information6 (Melder_double (p), L" (=probability for chisq = ", Melder_double (chisq), L" and ndf = ",
		Melder_integer (my numberOfRows * (my numberOfRows - 1) / 2), L")");
END

DIRECT (SSCP_to_Correlation)
	EVERY_TO (SSCP_to_Correlation (OBJECT))
END

FORM (SSCP_to_Covariance, L"SSCP: To Covariance", L"SSCP: To Covariance...")
	NATURAL (L"Number of constraints", L"1")
	OK
DO
	long noc = GET_INTEGER (L"Number of constraints");
	EVERY_TO (SSCP_to_Covariance (OBJECT, noc))
END

DIRECT (SSCP_to_PCA)
	EVERY_TO (SSCP_to_PCA (OBJECT))
END

/******************* Strings ****************************/

DIRECT (Strings_append)
	int status = 0;
	Ordered me = Ordered_create ();
	if (! me) return 0;
	WHERE (SELECTED)
	{
		if (! Collection_addItem (me, OBJECT)) goto Strings_append_end;
	}
	if (praat_new1 (Strings_append (me), L"appended")) status = 1;
Strings_append_end:
	my size = 0; forget (me);
	return status;
END

DIRECT (Strings_to_Categories)
	EVERY_TO (Strings_to_Categories (OBJECT))
END

FORM (Strings_setString, L"Strings: Set string", L"Strings: Set string...")
	NATURAL (L"Index", L"1")
	SENTENCE (L"String", L"")
	OK
DO
	if (! Strings_setString (ONLY (classStrings), GET_STRING (L"String"),
		GET_INTEGER (L"Index"))) return 0;
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
	EVERY_TO (Strings_change (OBJECT, GET_STRING (L"Search"),
		GET_STRING (L"Replace"), GET_INTEGER (L"Replace limit"), &nmatches,
		&nstringmatches, GET_INTEGER (L"Search and replace are") - 1))
END

FORM (Strings_extractPart, L"Strings: Extract part", L"")
	NATURAL (L"From index", L"1")
	NATURAL (L"To index", L"1")
	OK
DO
	EVERY_TO (Strings_extractPart (OBJECT, GET_INTEGER (L"From index"), GET_INTEGER (L"To index")))
END

FORM (Strings_to_Permutation, L"Strings: To Permutation", L"Strings: To Permutation...")
	BOOLEAN (L"Sort", 1)
	OK
DO
	EVERY_TO (Strings_to_Permutation (OBJECT, GET_INTEGER (L"Sort")))
END

DIRECT (Strings_and_Permutation_permuteStrings)
	NEW (Strings_and_Permutation_permuteStrings (ONLY (classStrings), ONLY (classPermutation)))
END

FORM (SVD_to_TableOfReal, L"SVD: To TableOfReal", L"SVD: To TableOfReal...")
	NATURAL (L"First component", L"1")
	INTEGER (L"Last component", L"0 (=all)")
	OK
DO
	EVERY_TO (SVD_to_TableOfReal (OBJECT, GET_INTEGER (L"First component"),
		GET_INTEGER (L"Last component")))
END

DIRECT (SVD_extractLeftSingularVectors)
	SVD svd = ONLY (classSVD);
	if (! praat_new2 (SVD_extractLeftSingularVectors (svd), Thing_getName (svd), L"_lsv")) return 0;
END

DIRECT (SVD_extractRightSingularVectors)
	SVD svd = ONLY (classSVD);
	if (! praat_new2 (SVD_extractRightSingularVectors (svd), Thing_getName (svd), L"_rsv")) return 0;
END

DIRECT (SVD_extractSingularValues)
	SVD svd = ONLY (classSVD);
	if (! praat_new2 (SVD_extractSingularValues (svd), Thing_getName (svd), L"_sv")) return 0;
END

/******************* Table ****************************/

DIRECT (Table_createFromPetersonBarneyData)
	if (! praat_new1 (Table_createFromPetersonBarneyData (), L"pb")) return 0;
END

DIRECT (Table_createFromPolsVanNieropData)
	if (! praat_new1 (Table_createFromPolsVanNieropData (), L"pvn")) return 0;
END

DIRECT (Table_createFromWeeninkData)
	if (! praat_new1 (Table_createFromWeeninkData (), L"m10w10c10")) return 0;
END

FORM (Table_drawScatterPlotWithConfidenceIntervals, L"Table: Scatter plot (confidence intervals)", L"")
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
	Table_drawScatterPlotWithConfidenceIntervals (ONLY (classTable), GRAPHICS,
		GET_INTEGER (L"Horizontal axis column"), GET_INTEGER (L"Vertical axis column"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"left Horizontal confidence interval column"), GET_INTEGER (L"right Horizontal confidence interval column"),
		GET_INTEGER (L"left Vertical confidence interval column"), GET_INTEGER (L"right Vertical confidence interval column"),
		GET_REAL (L"Bar size"), GET_INTEGER (L"Garnish"));
END

/******************* TableOfReal ****************************/

FORM (TableOfReal_reportMultivariateNormality, L"TableOfReal: Report multivariate normality (Henze-Zirkler)", L"TableOfReal: Report multivariate normality (HZ)...")
	REAL (L"Beta", L"0.0")
	OK
DO
	TableOfReal t = ONLY (classTableOfReal);
	long n = t -> numberOfRows, p = t -> numberOfColumns;
	double prob, wnb, lnmu, lnvar;
	double beta = GET_REAL (L"Beta");
	MelderInfo_open ();
	prob = NUMnormalityTest_HenzeZirkler (t -> data, n, p, &beta, &wnb, &lnmu, &lnvar);
	MelderInfo_writeLine1 (L"Henze-Zirkler normality test:");
	MelderInfo_writeLine2 (L"Henze-Zirkler statistic: ", Melder_double (wnb));
	MelderInfo_writeLine2 (L"Significance of normality: ", Melder_double (prob));
	MelderInfo_writeLine2 (L"Lognormal mean: ", Melder_double (lnmu));
	MelderInfo_writeLine2 (L"Lognormal variance: ", Melder_double (lnvar));
	MelderInfo_writeLine2 (L"Smoothing beta: ", Melder_double (beta));
	MelderInfo_writeLine2 (L"Sample size: ", Melder_integer (n));
	MelderInfo_writeLine2 (L"Number of variables: ", Melder_integer (p));
	MelderInfo_close ();
END

DIRECT (TableOfReal_and_Permutation_permuteRows)
	TableOfReal t = ONLY (classTableOfReal);
	Permutation p = ONLY (classPermutation);
	if (! praat_new3 (TableOfReal_and_Permutation_permuteRows (t, p),
		Thing_getName (t), L"_", Thing_getName (p))) return 0;
END

DIRECT (TableOfReal_to_Permutation_sortRowlabels)
	EVERY_TO (TableOfReal_to_Permutation_sortRowLabels (OBJECT))
END

DIRECT (TableOfReal_appendColumns)
	Collection me = Collection_create (classTableOfReal, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT))
		{
			my size = 0; forget (me); return 0;
		}
	if (! praat_new1 (TableOfReal_appendColumnsMany (me), L"columns_appended"))
	{
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (TableOfReal_createFromPolsData_50males, L"Create TableOfReal (Pols 1973)", L"Create TableOfReal (Pols 1973)...")
	BOOLEAN (L"Include formant levels", 0)
	OK
DO
	if (! praat_new1 (TableOfReal_createFromPolsData_50males
		(GET_INTEGER (L"Include formant levels")), L"pols_50males")) return 0;
END

FORM (TableOfReal_createFromVanNieropData_25females, L"Create TableOfReal (Van Nierop 1973)...", L"Create TableOfReal (Van Nierop 1973)...")
	BOOLEAN (L"Include formant levels", 0)
	OK
DO
	if (! praat_new1 (TableOfReal_createFromVanNieropData_25females
		(GET_INTEGER (L"Include formant levels")), L"vannierop_25females")) return 0;
END

FORM (TableOfReal_createFromWeeninkData, L"Create TableOfReal (Weenink 1985)...", L"Create TableOfReal (Weenink 1985)...")
	RADIO (L"Speakers group", 1)
	RADIOBUTTON (L"Men")
	RADIOBUTTON (L"Women")
	RADIOBUTTON (L"Children")
	OK
DO
	int type = GET_INTEGER (L"Speakers group");
	if (! praat_new1 (TableOfReal_createFromWeeninkData (type),
		(type == 1 ? L"m10" : type == 2 ? L"w10" : L"c10"))) return 0;
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
	EVERY_DRAW (TableOfReal_drawScatterPlot (OBJECT, GRAPHICS,
			GET_INTEGER (L"Horizontal axis column number"),
			GET_INTEGER (L"Vertical axis column number"),
			GET_INTEGER (L"left Row number range"), GET_INTEGER (L"right Row number range"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Label size"), GET_INTEGER (L"Use row labels"),
			GET_STRING (L"Label"), GET_INTEGER (L"Garnish")))
END

FORM (TableOfReal_drawScatterPlotMatrix, L"TableOfReal: Draw scatter plots matrix", 0)
	INTEGER (L"From column", L"0")
	INTEGER (L"To column", L"0")
	POSITIVE (L"Fraction white", L"0.1")
	OK
DO
	EVERY_DRAW (TableOfReal_drawScatterPlotMatrix (OBJECT, GRAPHICS,
		GET_INTEGER (L"From column"), GET_INTEGER (L"To column"),
		GET_REAL (L"Fraction white")))
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
	EVERY_DRAW (TableOfReal_drawBiplot (OBJECT, GRAPHICS,
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_REAL (L"Split factor"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (TableOfReal_drawVectors (OBJECT, GRAPHICS,
		GET_INTEGER (L"left From columns"), GET_INTEGER (L"right From columns"),
		GET_INTEGER (L"left To columns"), GET_INTEGER (L"right To columns"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Vector type"), GET_INTEGER (L"Label size"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (TableOfReal_drawRowsAsHistogram (OBJECT, GRAPHICS, GET_STRING (L"Row number"),
		GET_INTEGER (L"left Column range"), GET_INTEGER (L"right Column range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Horizontal offset"), 0,
		GET_REAL (L"Distance between bars"), GET_STRING (L"Grey value"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (TableOfReal_drawRowsAsHistogram (OBJECT, GRAPHICS,
		GET_STRING (L"Row numbers"),
		GET_INTEGER (L"left Column range"), GET_INTEGER (L"right Column range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_REAL (L"Horizontal offset"), GET_REAL (L"Distance between bars"),
		GET_REAL (L"Distance between bar groups"), GET_STRING (L"Grey values"),
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (TableOfReal_drawBoxPlots (OBJECT, GRAPHICS,
		GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
		GET_INTEGER (L"From column"), GET_INTEGER (L"To column"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish")))
END

FORM (TableOfReal_drawColumnAsDistribution, L"TableOfReal: Draw column as distribution", L"TableOfReal: Draw column as distribution...")
	NATURAL (L"Column", L"1")
    REAL (L"Minimum value", L"0.0")
    REAL (L"Maximum value", L"0.0")
    LABEL (L"", L"Display of the distribution")
    NATURAL (L"Number of bins", L"10")
    REAL (L"Minimum frequency", L"0.0")
    REAL (L"Maximum frequency", L"0.0")
    BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawColumnAsDistribution (OBJECT, GRAPHICS, GET_INTEGER (L"Column"),
		GET_REAL (L"Minimum value"), GET_REAL (L"Maximum value"),GET_INTEGER (L"Number of bins"),
		GET_REAL (L"Minimum frequency"), GET_REAL (L"Maximum frequency"), 0, GET_INTEGER (L"Garnish")))
END

FORM (TableOfReal_to_Configuration_lda, L"TableOfReal: To Configuration (lda)", L"TableOfReal: To Configuration (lda)...")
	INTEGER (L"Number of dimensions", L"0")
	OK
DO
	long dimension = GET_INTEGER (L"Number of dimensions");
	REQUIRE (dimension >= 0, L"Number of dimensions must be greater equal zero.")
	EVERY_TO (TableOfReal_to_Configuration_lda (OBJECT, dimension))
END

FORM (TableOfReal_to_CCA, L"TableOfReal: To CCA", L"TableOfReal: To CCA...")
	NATURAL (L"Dimension of dependent variate", L"2")
	OK
DO
	EVERY_TO (TableOfReal_to_CCA (OBJECT, GET_INTEGER (L"Dimension of dependent variate")))
END

FORM (TableOfReal_to_Configuration_pca, L"TableOfReal: To Configuration (pca)", L"TableOfReal: To Configuration (pca)...")
	NATURAL (L"Number of dimensions", L"2")
	OK
DO
	EVERY_TO (TableOfReal_to_Configuration_pca (OBJECT,
		GET_INTEGER (L"Number of dimensions")))
END

DIRECT (TableOfReal_to_Discriminant)
	EVERY_TO (TableOfReal_to_Discriminant (OBJECT))
END

DIRECT (TableOfReal_to_PCA)
	EVERY_TO (TableOfReal_to_PCA (OBJECT))
END

FORM (TableOfReal_to_SSCP, L"TableOfReal: To SSCP", L"TableOfReal: To SSCP...")
	INTEGER (L"Begin row", L"0")
	INTEGER (L"End row", L"0")
	INTEGER (L"Begin column", L"0")
	INTEGER (L"End column", L"0")
	OK
DO
	EVERY_TO (TableOfReal_to_SSCP (OBJECT, GET_INTEGER (L"Begin row"), GET_INTEGER (L"End row"),
		GET_INTEGER (L"Begin column"), GET_INTEGER (L"End column")))
END

/* For the inheritors */
DIRECT (TableOfReal_to_TableOfReal)
	EVERY_TO (TableOfReal_to_TableOfReal (OBJECT))
END

DIRECT (TableOfReal_to_Correlation)
	EVERY_TO (TableOfReal_to_Correlation (OBJECT))
END
DIRECT (TableOfReal_to_Correlation_rank)
	EVERY_TO (TableOfReal_to_Correlation_rank (OBJECT))
END

DIRECT (TableOfReal_to_Covariance)
	EVERY_TO (TableOfReal_to_Covariance (OBJECT))
END

DIRECT (TableOfReal_to_SVD)
	EVERY_TO (TableOfReal_to_SVD (OBJECT))
END

DIRECT (TablesOfReal_to_Eigen_gsvd)
	TableOfReal me = NULL, thee = NULL;
	WHERE (SELECTED)
	{
		if (me) thee = OBJECT;
		else me = OBJECT;
	}
	NEW (TablesOfReal_to_Eigen_gsvd (me, thee))
END

DIRECT (TablesOfReal_to_GSVD)
	TableOfReal me = NULL, thee = NULL;
	WHERE (SELECTED)
	{
		if (me) thee = OBJECT;
		else me = OBJECT;
	}
	NEW (TablesOfReal_to_GSVD (me, thee))
END

FORM (TableOfReal_choleskyDecomposition, L"TableOfReal: Cholesky decomposition", 0)
	BOOLEAN (L"Upper (else L)", 0)
	BOOLEAN (L"Inverse", 0)
	OK
DO
	EVERY_TO (TableOfReal_choleskyDecomposition (OBJECT, GET_INTEGER (L"Upper"), GET_INTEGER (L"Inverse")))
END

FORM (TableOfReal_to_Pattern_and_Categories, L"TableOfReal: To Pattern and Categories", L"TableOfReal: To Pattern and Categories...")
	INTEGER (L"left Row range", L"0")
	INTEGER (L"right Row range", L"0 (=all)")
	INTEGER (L"left Column range", L"0")
	INTEGER (L"right Column range", L"0 (=all)")
	OK
DO
	Pattern p; Categories c; TableOfReal t = ONLY_OBJECT;
	if (TableOfReal_to_Pattern_and_Categories (t, GET_INTEGER (L"left Row range"),
		GET_INTEGER (L"right Row range"), GET_INTEGER (L"left Column range"),
		GET_INTEGER (L"right Column range"), &p, &c))
	{
		wchar_t *name = Thing_getName (t);
		praat_new1 (p, name);
		praat_new1 (c, name);
	}
END

FORM (TableOfReal_getColumnSum, L"TableOfReal: Get column sum", L"")
	INTEGER (L"Column", L"1")
	OK
DO
	Melder_information1 (Melder_double (TableOfReal_getColumnSum (ONLY_GENERIC(classTableOfReal), GET_INTEGER (L"Column"))));
END

FORM (TableOfReal_getRowSum, L"TableOfReal: Get row sum", L"")
	INTEGER (L"Row", L"1")
	OK
DO
	Melder_information1 (Melder_double (TableOfReal_getRowSum (ONLY_GENERIC(classTableOfReal), GET_INTEGER (L"Row"))));
END

DIRECT (TableOfReal_getGrandSum)
	Melder_information1 (Melder_double (TableOfReal_getGrandSum (ONLY_GENERIC(classTableOfReal))));
END

FORM (TableOfReal_meansByRowLabels, L"TableOfReal: Means by row labels", L"TableOfReal: To TableOfReal (means by row labels)...")
    BOOLEAN (L"Expand", 0)
	OK
DO
	EVERY_CHECK(praat_new2 (TableOfReal_meansByRowLabels (OBJECT, GET_INTEGER (L"Expand"), 0), NAMEW, L"_byrowlabels"))
END

FORM (TableOfReal_mediansByRowLabels, L"TableOfReal: Medians by row labels", L"TableOfReal: To TableOfReal (medians by row labels)...")
    BOOLEAN (L"Expand", 0)
	OK
DO
	EVERY_CHECK(praat_new2 (TableOfReal_meansByRowLabels (OBJECT, GET_INTEGER (L"Expand"), 1), NAMEW, L"_byrowlabels"))
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
	WHERE (SELECTED)
	{
		TextGrid_extendTime (OBJECT, GET_REAL (L"Extend domain by"), GET_INTEGER (L"At") - 1);
		praat_dataChanged (OBJECT);
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
	int regexp = GET_INTEGER (L"Search and replace strings are") - 1;
	wchar_t *search = GET_STRING (L"Search");
	long nmatches, nstringmatches;

	WHERE (SELECTED)
	{
		if (! TextGrid_changeLabels (OBJECT, GET_INTEGER (L"Tier number"), from, to, search, GET_STRING (L"Replace"),
			regexp, &nmatches, &nstringmatches)) return 0;
		praat_dataChanged (OBJECT);
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
	long nmatches, nstringmatches;
	WHERE (SELECTED)
	{
		if (! TextGrid_changeLabels (OBJECT, GET_INTEGER (L"Tier number"), from, to, GET_STRING (L"Search"), GET_STRING (L"Replace"),
			GET_INTEGER (L"Search and replace strings are")-1, &nmatches, &nstringmatches)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setTierName, L"TextGrid: Set tier name", L"TextGrid: Set tier name...")
	NATURAL (L"Tier number:", L"1")
	SENTENCE (L"Name", L"");
	OK
DO
	if (! TextGrid_setTierName (ONLY_OBJECT, GET_INTEGER (L"Tier number"),
		GET_STRING (L"Name"))) return 0;
		praat_dataChanged (OBJECT);
END

static VowelEditor vowelEditor = NULL;
DIRECT (VowelEditor_create)
	if (theCurrentPraatApplication -> batch) return Melder_error1 (L"Cannot edit from batch.");
	vowelEditor = VowelEditor_create (theCurrentPraatApplication -> topShell, L"VowelEditor", NULL);
	if (vowelEditor == NULL) return 0;
END

static Any cmuAudioFileRecognizer (int nread, const char *header, MelderFile fs)
{
	return nread < 12 || header [0] != 6 || header [1] != 0 ?
	 NULL : Sound_readFromCmuAudioFile (fs);
}

void praat_CC_init (void *klas)
{
	praat_addAction1 (klas, 1, L"Paint...", 0, 1, DO_CC_paint);
	praat_addAction1 (klas, 1, L"Draw...", 0, 1, DO_CC_drawC0);
	praat_addAction1 (klas, 1, QUERY_BUTTON, 0, 0, 0);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, L"Get value...", 0, 1, DO_CC_getValue);
	praat_addAction1 (klas, 0, L"To Matrix", 0, 0, DO_CC_to_Matrix);
	praat_addAction1 (klas, 2, L"To DTW...", 0, 0, DO_CCs_to_DTW);
}

static void praat_Eigen_Matrix_project (void *klase, void *klasm);
static void praat_Eigen_Matrix_project (void *klase, void *klasm)
{
	praat_addAction2 (klase, 1, klasm, 1, L"Project...", 0, 0, DO_Eigen_and_Matrix_project);
}

static void praat_Eigen_query_init (void *klas)
{
	praat_addAction1 (klas, 1, L"Get eigenvalue...", 0, 1, DO_Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, L"Get sum of eigenvalues...", 0, 1, DO_Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, L"Get number of eigenvectors", 0, 1, DO_Eigen_getNumberOfEigenvalues);
	praat_addAction1 (klas, 1, L"Get eigenvector dimension", 0, 1, DO_Eigen_getDimension);
	praat_addAction1 (klas, 1, L"Get eigenvector element...", 0, 1, DO_Eigen_getEigenvectorElement);
}

static void praat_Eigen_draw_init (void *klas)
{
	praat_addAction1 (klas, 0, L"Draw eigenvalues...", 0, 1, DO_Eigen_drawEigenvalues);
	praat_addAction1 (klas, 0, L"Draw eigenvalues (scree)...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_Eigen_drawEigenvalues_scree);
	praat_addAction1 (klas, 0, L"Draw eigenvector...", 0, 1, DO_Eigen_drawEigenvector);
}

static void praat_Index_init (void *klas)
{
    praat_addAction1 (klas, 1, L"Get number of classes", 0, 0, DO_Index_getNumberOfClasses);
    praat_addAction1 (klas, 1, L"To Permutation...", 0, 0, DO_Index_to_Permutation);
    praat_addAction1 (klas, 1, L"Extract part...", 0, 0, DO_Index_extractPart);
}

static void praat_FilterBank_query_init (void *klas);
static void praat_FilterBank_query_init (void *klas)
{
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
	praat_Matrixft_query_init (klas);
	praat_addAction1 (klas, 0, L"-- frequency scales --", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get frequency in Hertz...", 0, 1, DO_FilterBank_getFrequencyInHertz);
	praat_addAction1 (klas, 1, L"Get frequency in Bark...", 0, 1, DO_FilterBank_getFrequencyInBark);
	praat_addAction1 (klas, 1, L"Get frequency in mel...", 0, 1, DO_FilterBank_getFrequencyInMel);
}

static void praat_FilterBank_modify_init (void *klas);
static void praat_FilterBank_modify_init (void *klas)
{
	praat_addAction1 (klas, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, L"Equalize intensities...", 0, 1, DO_FilterBank_equalizeIntensities);
}

static void praat_FilterBank_draw_init (void *klas);
static void praat_FilterBank_draw_init (void *klas)
{
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

static void praat_FilterBank_all_init (void *klas);
static void praat_FilterBank_all_init (void *klas)
{
	praat_FilterBank_draw_init (klas);
	praat_FilterBank_query_init (klas);
	praat_FilterBank_modify_init (klas);
	praat_addAction1 (klas, 0, L"To Intensity", 0, 0, DO_FilterBank_to_Intensity);
	praat_addAction1 (klas, 0, L"To Matrix", 0, 0, DO_FilterBank_to_Matrix);
}

static void praat_FunctionTerms_init (void *klas)
{
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
	praat_addAction1 (klas, 0, L"Modify -               ", 0, 0, 0);
		praat_addAction1 (klas, 1, L"Set domain...", 0, 1, DO_FunctionTerms_setDomain);
		praat_addAction1 (klas, 1, L"Set coefficient...", 0, 1, DO_FunctionTerms_setCoefficient);
	praat_addAction1 (klas, 0, L"Analyse", 0, 0, 0);
}

/* Query buttons for frame-based frequency x time subclasses of matrix. */

void praat_Matrixft_query_init (void *klas)
{
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

static void praat_Spline_init (void *klas)
{
	praat_FunctionTerms_init (klas);
	praat_addAction1 (klas, 0, L"Draw knots...", L"Draw basis function...", 1, DO_Spline_drawKnots);
	praat_addAction1 (klas, 1, L"Get order", L"Get degree", 1, DO_Spline_getOrder);
	praat_addAction1 (klas, 1, L"Scale x...", L"Analyse",	0, DO_Spline_scaleX);

}

static void praat_SSCP_query_init (void *klas)
{
	praat_addAction1 (klas, 1, L"-- statistics --", L"Get value...", 1, 0);
	praat_addAction1 (klas, 1, L"Get number of observations", L"-- statistics --", 1, DO_SSCP_getNumberOfObservations);
	praat_addAction1 (klas, 1, L"Get degrees of freedom", L"Get number of observations", 1, DO_SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, L"Get centroid element...", L"Get degrees of freedom",1, DO_SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, L"Get ln(determinant)", L"Get centroid element...", 1, DO_SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (void *klas)
{
	praat_addAction1 (klas, 1, L"Extract centroid", EXTRACT_BUTTON, 1, DO_SSCP_extractCentroid);
}

void praat_TableOfReal_init2  (void *klas)
{
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, L"To TableOfReal", L"To Matrix", 1, DO_TableOfReal_to_TableOfReal);
}

/* Query buttons for frame-based time-based subclasses of Sampled.
void praat_TimeFrameSampled_query_init (void *klas)
{
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, L"Get number of frames", 0, 1,
		DO_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, L"Get frame length", 0, 1,
		DO_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, L"Get time from frame...", 0, 1,
		DO_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, L"Get frame from time...", 0, 1,
		DO_TimeFrameSampled_getFrameFromTime);
}
*/
void praat_uvafon_David_init (void);
void praat_uvafon_David_init (void)
{
	Data_recognizeFileType (TextGrid_TIMITLabelFileRecognizer);
	Data_recognizeFileType (cmuAudioFileRecognizer);

    Thing_recognizeClassesByName (classActivation, classBarkFilter,
		classCategories, classCepstrum, classCCA,
		classChebyshevSeries,classClassificationTable, classConfusion,
    	classCorrelation, classCovariance, classDiscriminant, classDTW,
		classEigen, classExcitations, classFormantFilter, classIndex, classKlattTable,
		classPermutation,
		classISpline, classLegendreSeries,
		classMelFilter,
		classMSpline, classPattern, classPCA, classPolynomial, classRoots,
		classSimpleString, classStringsIndex, classSPINET, classSSCP, classSVD, NULL);

    praat_addMenuCommand (L"Objects", L"Goodies", L"Report floating point properties", 0, 0, DO_Praat_ReportFloatingPointProperties);

 praat_addMenuCommand (L"Objects", L"New", L"Create Permutation...", 0, 0, DO_Permutation_create);
    praat_addMenuCommand (L"Objects", L"New", L"Polynomial", 0, 0, 0);
    	praat_addMenuCommand (L"Objects", L"New", L"Create Polynomial...", 0, 1, DO_Polynomial_create);
    	praat_addMenuCommand (L"Objects", L"New", L"Create LegendreSeries...", 0, 1, DO_LegendreSeries_create);
    	praat_addMenuCommand (L"Objects", L"New", L"Create ChebyshevSeries...", 0, 1, DO_ChebyshevSeries_create);
    	praat_addMenuCommand (L"Objects", L"New", L"Create MSpline...", 0, 1, DO_MSpline_create);
    	praat_addMenuCommand (L"Objects", L"New", L"Create ISpline...", 0, 1, DO_ISpline_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from gamma-tone...", L"Create Sound from tone complex...", 1, DO_Sound_createFromGammaTone);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from Shepard tone...", L"Create Sound from gamma-tone...", 1, DO_Sound_createFromShepardTone);
	praat_addMenuCommand (L"Objects", L"New", L"Create Sound from VowelEditor...", L"Create Sound from Shepard tone...", praat_DEPTH_1, DO_VowelEditor_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create formant table (Pols & Van Nierop 1973)", L"Create Table...", 1, DO_Table_createFromPolsVanNieropData);
	praat_addMenuCommand (L"Objects", L"New", L"Create formant table (Peterson & Barney 1952)", L"Create Table...", 1, DO_Table_createFromPetersonBarneyData);
	praat_addMenuCommand (L"Objects", L"New", L"Create formant table (Weenink 1985)", L"Create formant table (Peterson & Barney 1952)",1, DO_Table_createFromWeeninkData);
	praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal (Pols 1973)...", L"Create TableOfReal...", 1, DO_TableOfReal_createFromPolsData_50males);
	praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal (Van Nierop 1973)...", L"Create TableOfReal (Pols 1973)...", 1, DO_TableOfReal_createFromVanNieropData_25females);
	praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal (Weenink 1985)...", L"Create TableOfReal (Van Nierop 1973)...", 1, DO_TableOfReal_createFromWeeninkData);
	praat_addMenuCommand (L"Objects", L"New", L"Create KlattTable example", L"Create TableOfReal (Weenink 1985)...", praat_DEPTH_1+praat_HIDDEN, DO_KlattTable_createExample);

	praat_addMenuCommand (L"Objects", L"Read", L"Read Sound from raw 16-bit Little Endian file...", L"Read from special sound file", 1,
		 DO_Sound_readFromRawFileLE);
	praat_addMenuCommand (L"Objects", L"Read", L"Read Sound from raw 16-bit Big Endian file...", L"Read Sound from raw 16-bit Little Endian file...", 1, DO_Sound_readFromRawFileBE);
	praat_addMenuCommand (L"Objects", L"Read", L"Read KlattTable from raw text file...", L"Read Matrix from raw text file...", praat_HIDDEN, DO_KlattTable_readFromRawTextFile);

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
	praat_addAction1 (classConfusion, 0, L"Draw as numbers...", L"Draw -                 ", 1, DO_Confusion_drawAsNumbers);
	praat_addAction1 (classConfusion, 0, L"-- confusion statistics --", L"Get value...", 1, 0);
	praat_addAction1 (classConfusion, 1, L"Get fraction correct", L"-- confusion statistics --", 1, DO_Confusion_getFractionCorrect);
	praat_addAction1 (classConfusion, 1, L"Get row sum...", L"Get fraction correct", 1, DO_TableOfReal_getRowSum);
 	praat_addAction1 (classConfusion, 1, L"Get column sum...", L"Get row sum...", 1, DO_TableOfReal_getColumnSum);
	praat_addAction1 (classConfusion, 1, L"Get grand sum", L"Get column sum...", 1, DO_TableOfReal_getGrandSum);
	praat_addAction1 (classConfusion, 0, L"To TableOfReal (marginals)", L"To TableOfReal", 0, DO_Confusion_to_TableOfReal_marginals);
	praat_addAction1 (classConfusion, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classConfusion, 0, L"Condense...", 0, 0,
		DO_Confusion_condense);
    praat_addAction1 (classConfusion, 2, L"To difference matrix", 0, 0,
		DO_Confusion_difference);

    praat_addAction2 (classConfusion, 1, classMatrix, 1, L"Draw", 0, 0, 0);
    praat_addAction2 (classConfusion, 1, classMatrix, 1, L"Draw confusion...",
		0, 0, DO_Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, L"Covariance help", 0, 0,
		DO_Covariance_help);
    praat_TableOfReal_init2 (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
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
	praat_addAction1 (classDTW, 0, L"Draw", 0, 0, 0);
    praat_addAction1 (classDTW, 0, L"Draw path...", 0, 0, DO_DTW_drawPath);
    praat_addAction1 (classDTW, 0, L"Paint distances...", 0, 0, DO_DTW_paintDistances);
    praat_addAction1 (classDTW, 0, L"Draw warp (x)...", 0, 0, DO_DTW_drawWarpX);
    praat_addAction1 (classDTW, 0, QUERY_BUTTON, 0, 0, 0);
    praat_addAction1 (classDTW, 1, L"Get distance (weighted)", 0, 1, DO_DTW_getWeightedDistance);
	praat_addAction1 (classDTW, 1, L"Get maximum consecutive steps...", 0, 1, DO_DTW_getMaximumConsecutiveSteps);
    praat_addAction1 (classDTW, 1, L"Get time along path...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_DTW_getPathY);
    praat_addAction1 (classDTW, 1, L"Get y time...", 0, 1, DO_DTW_getYTime);
    praat_addAction1 (classDTW, 1, L"Get x time...", 0, 1, DO_DTW_getXTime);


    praat_addAction1 (classDTW, 0, L"Analyse", 0, 0, 0);
    praat_addAction1 (classDTW, 0, L"Find path...", 0, 0, DO_DTW_findPath);
	praat_addAction1 (classDTW, 0, L"Find path (band)...", 0, 0, DO_DTW_pathFinder_band);
#ifdef INCLUDE_DTW_SLOPES
	praat_addAction1 (classDTW, 0, L"Find path (slopes)...", 0, 0, DO_DTW_pathFinder_slopes);
#endif
	praat_addAction1 (classDTW, 0, L"To Polygon (band)...", 0, 0, DO_DTW_to_Polygon_band);
	praat_addAction1 (classDTW, 0, L"To Polygon (slopes)...", 0, 0, DO_DTW_to_Polygon_slopes);
    praat_addAction1 (classDTW, 0, L"To Matrix (distances)", 0, 0, DO_DTW_distancesToMatrix);
	praat_addAction1 (classDTW, 0, L"Swap axes", 0, 0, DO_DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classTextGrid, 1, L"To TextGrid (warp times)", 0, 0, DO_DTW_and_TextGrid_to_TextGrid);

	praat_addAction2 (classDTW, 1, classSound, 2, L"Draw...", 0, 0, DO_DTW_and_Sounds_draw);
	praat_addAction2 (classDTW, 1, classSound, 2, L"Draw warp (x)...", 0, 0, DO_DTW_and_Sounds_drawWarpX);

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


	praat_addAction1 (classFormantFilter, 0, L"FormantFilter help", 0, 0, DO_FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, L"Draw spectrum (slice)...", L"Draw filters...", 1, DO_FormantFilter_drawSpectrum);
	praat_addAction1 (classFormantFilter, 0, L"Draw filter functions...", L"Draw filters...", 1, DO_FormantFilter_drawFilterFunctions);
	praat_addAction1 (classFormantGrid, 0, L"Draw...", L"Edit", 1, DO_FormantGrid_draw);

	praat_addAction1 (classIntensity, 0, L"To TextGrid (silences)...", L"To IntensityTier (valleys)", 0, DO_Intensity_to_TextGrid_detectSilences);

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

	praat_addAction1 (classLongSound, 2, L"Write to stereo AIFF file...", L"Write to NIST file...", 1, DO_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo AIFC file...", L"Write to stereo AIFF file...", 1, DO_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo WAV file...", L"Write to stereo AIFC file...", 1, DO_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo NeXt/Sun file...", L"Write to stereo WAV file...", 1, DO_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, L"Write to stereo NIST file...", L"Write to stereo NeXt/Sun file...", 1, DO_LongSounds_writeToStereoNistFile);

	praat_addAction1 (classMatrix, 0, L"Scatter plot...", L"Paint cells...", 1, DO_Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, L"Draw as squares...", L"Scatter plot...", 1, DO_Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, L"Draw distribution...", L"Draw as squares...", 1, DO_Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, L"Draw cumulative distribution...", L"Draw distribution...", 1, DO_Matrix_drawCumulativeDistribution);
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
	praat_addAction1 (classPermutation, 1, L"Swap one from range...", 0, 1, DO_Permutation_swapOneFromRange);
	praat_addAction1 (classPermutation, 1, L"Permute randomly...", 0, 0, DO_Permutation_permuteRandomly);
	praat_addAction1 (classPermutation, 1, L"Permute randomly (blocks)...", 0, 0, DO_Permutation_permuteBlocksRandomly);
	praat_addAction1 (classPermutation, 1, L"Interleave...", 0, 0, DO_Permutation_interleave);
	praat_addAction1 (classPermutation, 1, L"Rotate...", 0, 0, DO_Permutation_rotate);
	praat_addAction1 (classPermutation, 1, L"Reverse...", 0, 0, DO_Permutation_reverse);
	praat_addAction1 (classPermutation, 1, L"Invert", 0, 0, DO_Permutation_invert);
	praat_addAction1 (classPermutation, 0, L"Multiply", 0, 0, DO_Permutations_multiply);

	praat_addAction1 (classPitch, 2, L"To DTW...", L"To PointProcess", praat_HIDDEN, DO_Pitches_to_DTW);

	praat_addAction1 (classPitchTier, 0, L"To Pitch...", L"To Sound (sine)...", 1, DO_PitchTier_to_Pitch);
	praat_addAction1 (classPolygon, 0, L"Translate...", L"Modify", 0, DO_Polygon_translate);
	praat_addAction1 (classPolygon, 0, L"Rotate...", L"Translate...", 0, DO_Polygon_rotate);
	praat_addAction1 (classPolygon, 0, L"Scale...", L"Rotate...", 0, DO_Polygon_scale);
	praat_addAction1 (classPolygon, 0, L"Reverse X", L"Scale...", 0, DO_Polygon_reverseX);
	praat_addAction1 (classPolygon, 0, L"Reverse Y", L"Reverse X", 0, DO_Polygon_reverseY);

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

	praat_addAction2 (classRoots, 1, classPolynomial, 1,L"Polish roots", 0, 0, DO_Roots_and_Polynomial_polish);

	praat_addAction1 (classSound, 1, L"Write to raw 16-bit Big Endian file...", 0, 0, DO_Sound_writeToRawFileBE);
	praat_addAction1 (classSound, 1, L"Write to raw 16-bit Little Endian file...", 0, 0, DO_Sound_writeToRawFileLE);

	praat_addAction1 (classSound, 0, L"To TextGrid (silences)...", L"To IntervalTier", 1, DO_Sound_to_TextGrid_detectSilences);

	praat_addAction1 (classSound, 0, L"Draw where...", L"Draw...", 1, DO_Sound_drawWhere);

	praat_addAction1 (classSound, 0, L"To Pitch (shs)...", L"To Pitch (cc)...", 1, DO_Sound_to_Pitch_shs);
	praat_addAction1 (classSound, 0, L"Fade in...", L"Multiply by window...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_fadeIn);
	praat_addAction1 (classSound, 0, L"Fade out...", L"Fade in...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_fadeOut);
	praat_addAction1 (classSound, 0, L"To Pitch (SPINET)...", L"To Pitch (cc)...", 1, DO_Sound_to_Pitch_SPINET);

	praat_addAction1 (classSound, 0, L"To FormantFilter...", L"To Cochleagram (edb)...", 1, DO_Sound_to_FormantFilter);

	praat_addAction1 (classSound, 0, L"To BarkFilter...", L"To FormantFilter...", 1, DO_Sound_to_BarkFilter);

	praat_addAction1 (classSound, 0, L"To MelFilter...", L"To BarkFilter...", 1, DO_Sound_to_MelFilter);

	praat_addAction1 (classSound, 0, L"Filter (gammatone)...", L"Filter (formula)...", 1, DO_Sound_filterByGammaToneFilter4);

	praat_addAction1 (classSound, 0, L"Change gender...", L"Deepen band modulation...", 1, DO_Sound_changeGender);

	praat_addAction1 (classSound, 0, L"Change speaker...", L"Deepen band modulation...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_changeSpeaker);
	praat_addAction1 (classSound, 0, L"To KlattGrid (simple)...", L"To Manipulation...", 1, DO_Sound_to_KlattGrid_simple);

	praat_addAction2 (classSound, 1, classPitch, 1, L"To FormantFilter...", 0, 0, DO_Sound_and_Pitch_to_FormantFilter);

	praat_addAction2 (classSound, 1, classPitch, 1, L"Change gender...", 0, 0, DO_Sound_and_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, L"Change speaker...", 0, praat_HIDDEN, DO_Sound_and_Pitch_changeSpeaker);

    praat_addAction1 (classSpectrogram, 2, L"To DTW...", L"To Spectrum (slice)...", 0, DO_Spectrograms_to_DTW);

	praat_addAction1 (classSpectrum, 0, L"Draw phases...", L"Draw (log freq)...", 1, DO_Spectrum_drawPhases);
	praat_addAction1 (classSpectrum, 0, L"Conjugate", L"Formula...", praat_HIDDEN | praat_DEPTH_1, DO_Spectrum_conjugate);
	praat_addAction1 (classSpectrum, 2, L"Multiply", L"To Sound (fft)", praat_HIDDEN, DO_Spectra_multiply);
	praat_addAction1 (classSpectrum, 0, L"To Matrix (unwrap)", L"To Matrix", 0, DO_Spectrum_unwrap);
	praat_addAction1 (classSpectrum, 0, L"To Cepstrum", L"To Spectrogram", 0, DO_Spectrum_to_Cepstrum);

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
	praat_addAction1 (classStrings, 1, L"Set string...", L"Genericize", 0, DO_Strings_setString);
	praat_addAction1 (classStrings, 0, L"Change...", L"Set string...", 0, DO_Strings_change);
	praat_addAction1 (classStrings, 0, L"Extract part...", L"Change...", 0, DO_Strings_extractPart);
	praat_addAction1 (classStrings, 0, L"To Permutation...", L"To Distributions", 0, DO_Strings_to_Permutation);

	praat_addAction1 (classSVD, 0, L"To TableOfReal...", 0, 0, DO_SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, L"Extract left singular vectors", 0, 0, DO_SVD_extractLeftSingularVectors);
	praat_addAction1 (classSVD, 0, L"Extract right singular vectors", 0, 0, DO_SVD_extractRightSingularVectors);
	praat_addAction1 (classSVD, 0, L"Extract singular values", 0, 0, DO_SVD_extractSingularValues);

	praat_addAction1 (classTable, 0, L"Scatter plot (ci)...", 0, praat_DEPTH_1|praat_HIDDEN, DO_Table_drawScatterPlotWithConfidenceIntervals);
	praat_addAction1 (classTable, 0, L"To KlattTable", 0, praat_HIDDEN, DO_Table_to_KlattTable);

	praat_addAction1 (classTableOfReal, 1, L"Report multivariate normality...", L"Get column stdev (label)...",
		praat_DEPTH_1|praat_HIDDEN, DO_TableOfReal_reportMultivariateNormality);
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
	praat_addAction1 (classTableOfReal, 1, L"Draw column as distribution...", L"Draw rows as histogram...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawColumnAsDistribution);

	praat_addAction2 (classStrings, 1, classPermutation, 1, L"Permute strings", 0, 0, DO_Strings_and_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, L"Permute rows",	0, 0, DO_TableOfReal_and_Permutation_permuteRows);

	praat_addAction1 (classTextGrid, 0, L"Extend time...", L"Scale times...", 2, DO_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, L"Set tier name...", L"Remove tier...", 1, DO_TextGrid_setTierName);
			praat_addAction1 (classTextGrid, 0, L"Replace interval text...", L"Set interval text...", 2, DO_TextGrid_replaceIntervalTexts);
			praat_addAction1 (classTextGrid, 0, L"Replace point text...", L"Set point text...", 2, DO_TextGrid_replacePointTexts);

    INCLUDE_LIBRARY (praat_uvafon_MDS_init)
	INCLUDE_LIBRARY (praat_KlattGrid_init)
	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)
}

/* End of file praat_David.c */
