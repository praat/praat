/* praat_David_init.c
 * 
 * Copyright (C) 1993-2007 David Weenink
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
#include "Intensity_extensions.h"
#include "Matrix_Categories.h"
#include "Matrix_extensions.h"
#include "LongSound_extensions.h"
#include "Minimizers.h"
#include "Pattern.h"
#include "PCA.h"
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
#include "TableOfReal_and_SVD.h"

static char *QUERY_BUTTON   = "Query -                ";
static char *DRAW_BUTTON    = "Draw -                 ";
static char *MODIFY_BUTTON  = "Modify -               ";
static char *EXTRACT_BUTTON = "Extract -";

extern void praat_TimeFunction_query_init (void *klas);
extern void praat_TimeFrameSampled_query_init (void *klas);
extern void praat_TableOfReal_init (void *klas);
void praat_TableOfReal_init2  (void *klas);
 
void praat_CC_init (void *klas);
void DTW_constraints_addCommonFields (void *dia);
void DTW_constraints_getCommonFields (void *dia, int *begin, int *end, int *slope);
void praat_Matrixft_query_init (void *klas);
int praat_Fon_formula (Any dia);

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

FORM (Activation_formula,"Activation: Formula", 0)
	LABEL ("label", "for col := 1 to ncol do { self [row, col] := `formula' ;"
		" x := x + dx } y := y + dy }}")
	TEXTFIELD ("formula", "self")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END	

DIRECT (Activation_to_Matrix)
	EVERY_TO (Activation_to_Matrix (OBJECT))
END

/********************** BarkFilter *******************************************/

DIRECT (BarkFilter_help)
	Melder_help (L"BarkFilter");
END

FORM (BarkFilter_drawSpectrum, "BarkFilter: Draw spectrum (slice)", "FilterBank: Draw spectrum (slice)...")
	POSITIVE ("Time (s)", "0.1")
	REAL ("left Frequency range (Bark)", "0.0")
	REAL ("right Frequency range (Bark)", "0.0")
	REAL ("left Amplitude range (dB)", "0.0")
	REAL ("right Amplitude range (dB)", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO	
	EVERY_DRAW (FilterBank_drawTimeSlice (OBJECT, GRAPHICS,
		GET_REAL ("Time"), GET_REAL ("left Frequency range"),
		GET_REAL ("right Frequency range"), GET_REAL ("left Amplitude range"),
		GET_REAL ("right Amplitude range"), L"Barks", GET_INTEGER ("Garnish")))
END

FORM (BarkFilter_drawSekeyHansonFilterFunctions, "BarkFilter: Draw filter functions", 
	"FilterBank: Draw filter functions...")
	INTEGER ("left Filter range", "0")
	INTEGER ("right Filter range", "0")
	RADIO ("Frequency scale", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	BOOLEAN ("Amplitude scale in dB", 1)
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (BarkFilter_drawSekeyHansonFilterFunctions (OBJECT, GRAPHICS,
		GET_INTEGER ("Frequency scale"),
		GET_INTEGER ("left Filter range"), GET_INTEGER ("right Filter range"),
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_INTEGER ("Amplitude scale in dB"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"), GET_INTEGER ("Garnish")))
END
/********************** Categories  ****************************************/

FORM (Categories_append, "Categories: Append 1 category", "Categories: Append 1 category...")
	SENTENCE("Category", "")
	OK
DO
	if (! OrderedOfString_append (ONLY_OBJECT, GET_STRINGW (L"Category")))
		return 0;
END

DIRECT (Categories_edit)
	if (theCurrentPraat -> batch)
		return Melder_error ("Cannot edit a Categories from batch.");
	else
		WHERE (SELECTED) if (! praat_installEditor (CategoriesEditor_create (theCurrentPraat -> topShell,
			FULL_NAMEW, OBJECT), IOBJECT)) return 0;
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
	wchar_t name [200];
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
	(void) swprintf (name, 200, L"%ls_%ls", wcschr (theCurrentPraat -> list[i1].name, ' ') + 1,
		wcschr (theCurrentPraat -> list[i2].name, ' ') + 1);
	if (! praat_new1 (Categories_to_Confusion (c1, c2), name)) return 0;
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

FORM (CC_paint, "CC: Paint", "CC: Paint...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	INTEGER ("From coefficient", "0")
	INTEGER ("To coefficient", "0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (CC_paint (OBJECT, GRAPHICS, GET_REAL ("left Time range"),
		GET_REAL ("right Time range"), GET_INTEGER ("From coefficient"),
		GET_INTEGER ("To coefficient"), GET_REAL ("Minimum"),
		GET_REAL ("Maximum"), GET_INTEGER ("Garnish")))
END

FORM (CC_drawC0, "CC: Draw c0", "CC: Draw c0...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (CC_drawC0 (OBJECT, GRAPHICS, GET_REAL ("left Time range"),
		GET_REAL ("right Time range"), GET_REAL ("left Amplitude range"),
		GET_REAL ("right Amplitude range"), GET_INTEGER ("Garnish")))
END

FORM (CCs_to_DTW, "CC: To DTW", "CC: To DTW...")
	LABEL ("", "Distance  between cepstral coefficients")
	REAL ("Cepstral weight", "1.0")
	REAL ("Log energy weight", "0.0")
	REAL ("Regression weight", "0.0")
	REAL ("Regression weight log energy", "0.0")
	REAL ("Regression coefficients window (s)", "0.056")
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
	NEW (CCs_to_DTW (c1, c2, GET_REAL ("Cepstral weight"),
		GET_REAL ("Log energy weight"), GET_REAL ("Regression weight"),
		GET_REAL ("Regression weight log energy"),
		GET_REAL ("Regression coefficients window"),begin, end, slope))

END

DIRECT (CC_to_Matrix)
	EVERY_TO (CC_to_Matrix (OBJECT))
END

/******************* class CCA ********************************/

FORM (CCA_drawEigenvector, "CCA: Draw eigenvector", "Eigen: Draw eigenvector...")
	OPTIONMENU ("X or Y",1)
	OPTION ("y")
	OPTION ("x")
	INTEGER ("Eigenvector number", "1")
	LABEL ("", "Multiply by eigenvalue?")
	BOOLEAN ("Component loadings", 0)
	LABEL ("", "Select part of the eigenvector:")
	INTEGER ("left Element range", "0")
	INTEGER ("right Element range", "0")
	REAL ("left Amplitude range", "-1.0")
	REAL ("right Amplitude range", "1.0")
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
	BOOLEAN ("Connect points", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (CCA_drawEigenvector (OBJECT, GRAPHICS, GET_INTEGER ("X or Y"),
		GET_INTEGER ("Eigenvector number"),
		GET_INTEGER ("left Element range"), GET_INTEGER ("right Element range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"), 
		GET_INTEGER ("Component loadings"), GET_REAL ("Mark size"),
		GET_STRINGW (L"Mark string"), GET_INTEGER ("Connect points"),
		GET_INTEGER ("Garnish")))
END

DIRECT (CCA_getNumberOfCorrelations)
	CCA cca = ONLY(classCCA);
	Melder_information1 (Melder_double (cca->numberOfCoefficients));
END

FORM (CCA_getCorrelationCoefficient,"CCA: Get canonical correlation coefficient", 
	"CCA: Get canonical correlation coefficient")
	NATURAL ("Coefficient number", "1")
	OK
DO
	Melder_information1 (Melder_double (CCA_getCorrelationCoefficient (ONLY (classCCA),
		GET_INTEGER ("Coefficient number"))));
END

FORM (CCA_getEigenvectorElement, "CCA: Get eigenvector element", 
	"Eigen: Get eigenvector element...")
	OPTIONMENU ("X or Y",1)
	OPTION ("y")
	OPTION ("x")
	NATURAL ("Eigenvector number", "1")
	NATURAL ("Element number", "1")
	OK
DO
	Melder_information1 (Melder_double (CCA_getEigenvectorElement (ONLY (classCCA), 
		GET_INTEGER ("X or Y"), GET_INTEGER ("Eigenvector number"),
		GET_INTEGER ("Element number"))));
END

FORM (CCA_getZeroCorrelationProbability, "CCA: Get zero correlation probability", 
	"CCA: Get zero correlation probability...")
	NATURAL ("Coefficient number", "1")
	OK
DO
	double p, chisq; long ndf;
	CCA_getZeroCorrelationProbability (ONLY (classCCA), GET_INTEGER ("Coefficient number"),
		&chisq, &ndf, &p);
	Melder_information6 (Melder_double (p), L" (=probability for chisq = ", Melder_double (chisq), L" and ndf = ", Melder_integer (ndf), L")");
END

DIRECT (CCA_and_Correlation_factorLoadings)
	CCA cca = ONLY (classCCA);
	if (! praat_new2 (CCA_and_Correlation_factorLoadings (cca, 
		ONLY (classCorrelation)), Thing_getNameW (cca), L"_loadings")) return 0;
END

FORM (CCA_and_Correlation_getVarianceFraction, "CCA & Correlation: Get variance fraction",
	"CCA & Correlation: Get variance fraction...")
	LABEL ("", "Get the fraction of variance from the data in set...")
	OPTIONMENU ("X or Y", 1)
	OPTION ("y")
	OPTION ("x")
	LABEL ("", "extracted by...")
	NATURAL ("left Canonical variate range", "1")
	NATURAL ("right Canonical variate range", "1")
	OK
DO
	int x_or_y = GET_INTEGER ("X or Y");
	int cv_from = GET_INTEGER ("left Canonical variate range");
	int cv_to = GET_INTEGER ("right Canonical variate range");
	Melder_information7 (Melder_double (CCA_and_Correlation_getVarianceFraction (ONLY (classCCA), 
		ONLY (classCorrelation), x_or_y, cv_from, cv_to)), L" (fraction variance from ",
		(x_or_y == 1 ? L"y" : L"x"), L", extracted by canonical variates ", Melder_integer (cv_from), L" to ",
		 Melder_integer (cv_to));
END

FORM (CCA_and_Correlation_getRedundancy_sl, "CCA & Correlation: Get Stewart-Love redundancy",
	"CCA & Correlation: Get redundancy (sl)...")
	LABEL ("", "Get the redundancy of the data in set...")
	OPTIONMENU ("X or Y", 1)
	OPTION ("y")
	OPTION ("x")
	LABEL ("", "extracted by...")
	NATURAL ("left Canonical variate range", "1")
	NATURAL ("right Canonical variate range", "1")
	LABEL ("", "...given the availability of the data in the other set.")
	OK
DO
	int x_or_y = GET_INTEGER ("X or Y");
	int cv_from = GET_INTEGER ("left Canonical variate range");
	int cv_to = GET_INTEGER ("right Canonical variate range");
	Melder_information7 (Melder_double (CCA_and_Correlation_getRedundancy_sl (ONLY (classCCA), ONLY (classCorrelation),
		x_or_y, cv_from, cv_to)), L" (redundancy from ", (x_or_y == 1 ? L"y" : L"x"), L" extracted by canonical variates ",
		Melder_integer (cv_from), L" to ", Melder_integer (cv_to));
END


DIRECT (CCA_and_TableOfReal_factorLoadings)
	CCA cca = ONLY (classCCA);
	if (! praat_new2 (CCA_and_TableOfReal_factorLoadings (cca, 
		ONLY (classTableOfReal)), Thing_getNameW (cca), L"_loadings")) return 0;
END

FORM (CCA_and_TableOfReal_scores, "CCA & TableOfReal: To TableOfReal (scores)",
	"CCA & TableOfReal: To TableOfReal (scores)...")
	INTEGER ("Number of canonical correlations", "0 (=all)")
	OK
DO
	CCA cca = ONLY (classCCA);
	if (! praat_new2 (CCA_and_TableOfReal_scores (cca, ONLY (classTableOfReal),
		GET_INTEGER ("Number of canonical correlations")),
		Thing_getNameW (cca), L"_scores")) return 0;
END

FORM (CCA_and_TableOfReal_predict, "CCA & TableOfReal: Predict", 
	"CCA & TableOfReal: Predict...")
	LABEL ("", "The data set from which to predict starts at...")
	INTEGER ("Column number", "1")
	OK
DO
	NEW (CCA_and_TableOfReal_predict (ONLY (classCCA), ONLY(classTableOfReal),
		GET_INTEGER ("Column number")))
END

/***************** ChebyshevSeries ****************************************/

DIRECT (ChebyshevSeries_help)
	Melder_help (L"ChebyshevSeries");
END

FORM (ChebyshevSeries_create, "Create ChebyshevSeries",
	"Create ChebyshevSeries...")
	WORD ("Name", "cs")
	LABEL ("", "Domain")
	REAL ("Xmin", "-1")
	REAL ("Xmax", "1")
	LABEL ("", "ChebyshevSeries(x) = c[1] T[0](x) + c[2] T[1](x) + "
		"... c[n+1] T[n](x)")
	LABEL ("", "T[k] is a Chebyshev polynomial of degree k")
	SENTENCE ("Coefficients (c[k])", "0 0 1.0")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	if (! praat_new1 (ChebyshevSeries_createFromString (xmin, xmax,
		GET_STRINGW (L"Coefficients")), GET_STRINGW (L"Name"))) return 0;
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

FORM (Confusion_condense, "Confusion: Condense", 
	"Confusion: Condense...")
	SENTENCE ("Search", "a")
	SENTENCE ("Replace", "a")
	INTEGER ("Replace limit", "0 (=unlimited)")
	RADIO ("Search and replace are", 1)
	RADIOBUTTON ("Literals")
	RADIOBUTTON ("Regular Expressions")
	OK
DO
	EVERY_TO (Confusion_condense (OBJECT, GET_STRINGW (L"Search"),
		GET_STRINGW (L"Replace"), GET_INTEGER ("Replace limit"),
		GET_INTEGER ("Search and replace are") - 1))
END

/******************* Confusion & Matrix *************************************/

DIRECT (Confusion_getFractionCorrect)
	double f; long n;
	Confusion_getFractionCorrect (ONLY (classConfusion), &f, &n);
	Melder_information2 (Melder_double (f), L" (fraction correct)");
END

FORM (Confusion_Matrix_draw, "Confusion & Matrix: Draw confusions with arrows", 0)
    INTEGER ("Category position", "0 (=all)")
    REAL ("lower level(%)", "0")
    REAL ("left Horizontal range", "0.0")
    REAL ("right Horizontal range", "0.0")
    REAL ("left Vertical range", "0.0")
    REAL ("right Vertical range", "0.0")
    BOOLEAN ("Garnish", 1)
	OK
DO
	long categoryPosition = GET_INTEGER ("Category position");
	REQUIRE (categoryPosition >= 0, "Category position must be >= 0")
	EVERY_DRAW (Confusion_Matrix_draw(ONLY(classConfusion), ONLY(classMatrix), GRAPHICS,
		categoryPosition, GET_REAL ("lower level(%)"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER("Garnish")))
END

/**********************Correlation *******************************************/

DIRECT (Correlation_help)
	Melder_help (L"Correlation");
END

FORM (Correlation_confidenceIntervals, "Correlation: Confidence intervals...", 
	"Correlation: Confidence intervals...")
	POSITIVE ("Confidence level (0-1)", "0.95")
	INTEGER ("Number of tests (Bonferroni correction)", "0")
	RADIO ("Approximation", 1)
	RADIOBUTTON ("Ruben")
	RADIOBUTTON ("Fisher")
	OK
DO
	double cl = GET_REAL ("Confidence level");
	double numberOfTests = GET_INTEGER ("Number of tests");
	EVERY_TO (Correlation_confidenceIntervals (OBJECT, cl, numberOfTests,
		GET_INTEGER ("Approximation")))
END

FORM (Correlation_testDiagonality_bartlett, "Correlation: Get diagonality (bartlett)", 
	"SSCP: Get diagonality (bartlett)...")
	NATURAL ("Number of contraints", "1")
	OK
DO
	double chisq, p;
	long nc = GET_INTEGER ("Number of contraints");
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

FORM (Covariance_getSignificanceOfOneMean, "Covariance: Get significance of one mean",
	"Covariance: Get significance of one mean...")
	LABEL ("","Get probability that the mean with")
	NATURAL ("Index", "1")
	LABEL ("", "differs from")
	REAL ("Value", "0.0")
	LABEL ("","(Null hypothesis: the observed difference is due to chance.)")
	OK
DO
	double t, p; double ndf;
	Covariance_getSignificanceOfOneMean (ONLY_OBJECT, GET_INTEGER ("Index"),
		GET_REAL ("Value"), &p, &t , &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on t = ",
		Melder_double (t), L" and ndf = ", Melder_integer (ndf));
END

FORM (Covariance_getSignificanceOfMeansDifference, 
	"Covariance: Get significance of means difference",
	"Covariance: Get significance of means difference...")
	LABEL ("", "Get probability that the difference between means")
	NATURAL ("Index1", "1")
	NATURAL ("Index2", "2")
	LABEL ("", "differs from")
	REAL ("Value", "0.0")
	LABEL ("", "when the means are")
	BOOLEAN ("Paired", 1)
	LABEL ("", "and have")
	BOOLEAN ("Equal variances", 1)
	OK
DO
	double t, p; double ndf;
	Covariance_getSignificanceOfMeansDifference (ONLY_OBJECT, 
		GET_INTEGER ("Index1"), GET_INTEGER ("Index2"),
		GET_REAL ("Value"), GET_INTEGER ("Paired"),
		GET_INTEGER ("Equal variances"), &p, &t , &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on t = ",
		Melder_double (t), L"and ndf = ", Melder_integer (ndf));
END

FORM (Covariance_getSignificanceOfOneVariance, "Covariance: Get significance of one variance",
	"Covariance: Get significance of one variance...")
	LABEL ("", "Get probability that the variance with")
	NATURAL ("Index", "1")
	LABEL ("", "differs from")
	REAL ("Value", "0.0")
	LABEL ("","(Null hypothesis: the observed difference is due to chance.)")
	OK
DO
	double chisq, p; long ndf;
	Covariance_getSignificanceOfOneVariance (ONLY_OBJECT, GET_INTEGER ("Index"),
		GET_REAL ("Value"), &p, &chisq , &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

FORM (Covariance_getSignificanceOfVariancesRatio, 
	"Covariance: Get significance of variances ratio",
	"Covariance: Get significance of variances ratio...")
	NATURAL ("Index1", "1")
	NATURAL ("Index2", "2")
	REAL ("Hypothesized ratio", "1.0")
	OK
DO
	double f, p; long ndf;
	Covariance_getSignificanceOfVariancesRatio (ONLY_OBJECT, 
		GET_INTEGER ("Index1"), GET_INTEGER ("Index2"),
		GET_REAL ("Hypothesized ratio"), &p, &f , &ndf);
	Melder_information7 (Melder_double (p), L" (=probability, based on F = ",
		Melder_double (f), L"and ndf1 = ", Melder_integer (ndf), 
		L" and ndf2 = ", Melder_integer (ndf));
END

FORM (Covariance_getFractionVariance, "Covariance: Get fraction variance", 
	"Covariance: Get fraction variance...")
	NATURAL ("From dimension", "1")
	NATURAL ("To dimension", "1")
	OK
DO
	Melder_information1 (Melder_double (SSCP_getFractionVariation (ONLY_OBJECT,
		GET_INTEGER ("From dimension"), GET_INTEGER ("To dimension"))));
END

FORM (Covariance_to_TableOfReal_randomSampling, "Covariance: To TableOfReal (random "
	"sampling)", "Covariance: To TableOfReal (random sampling)...")
	INTEGER ("Number of data points", "0")
	OK
DO
	EVERY_TO (Covariance_to_TableOfReal_randomSampling (OBJECT, 
		GET_INTEGER ("Number of data points")))
END

DIRECT (Covariance_difference)
	Covariance c1 = NULL, c2 = NULL;
	double chisq, p; long ndf;
	WHERE (SELECTED && CLASS == classCovariance)
	{
		if (c1) c2 = OBJECT; else c1 = OBJECT;
	}
	if (! Covariance_difference (c1, c2, &p, &chisq, &ndf)) return 0;
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

DIRECT (Covariance_to_Correlation)
	EVERY_TO (SSCP_to_Correlation (OBJECT))
END

DIRECT (Covariance_to_PCA)
	EVERY_TO (SSCP_to_PCA (OBJECT))
END

/********************** Discriminant **********************************/

DIRECT (Discriminant_help)
	Melder_help (L"Discriminant");
END

DIRECT (Discriminant_setGroupLabels)
	if (! Discriminant_setGroupLabels (ONLY(classDiscriminant), 
		ONLY (classStrings))) return 0;
END

FORM (Discriminant_and_Pattern_to_Categories,
	"Discriminant & Pattern: To Categories",
	"Discriminant & Pattern: To Categories...")
	BOOLEAN ("Pool covariance matrices", 1)
	BOOLEAN ("Use apriori probabilities", 1)
	OK
DO
	NEW (Discriminant_and_Pattern_to_Categories
		(ONLY (classDiscriminant), ONLY_GENERIC (classPattern),
		GET_INTEGER ("Pool covariance matrices"),
		GET_INTEGER ("Use apriori probabilities")))
END

FORM (Discriminant_and_TableOfReal_to_Configuration, "Discriminant & TableOfReal: To Configuration",
	"Discriminant & TableOfReal: To Configuration...")
	INTEGER ("Number of dimensions", "0")
	OK
DO
	long dimension = GET_INTEGER ("Number of dimensions");
	REQUIRE (dimension >= 0, "Number of dimensions must be greater equal zero.")
	NEW (Discriminant_and_TableOfReal_to_Configuration
		(ONLY (classDiscriminant), ONLY_GENERIC (classTableOfReal),
			dimension))
END

DIRECT (hint_Discriminant_and_TableOfReal_to_ClassificationTable)
	Melder_information1 (L"You can use the Discriminant as a classifier by \n"
		"selecting a Discriminant and a TableOfReal object together.");
END

FORM (Discriminant_and_TableOfReal_to_ClassificationTable,
	"Discriminant & TableOfReal: To ClassificationTable",
	"Discriminant & TableOfReal: To ClassificationTable...")
	BOOLEAN ("Pool covariance matrices", 1)
	BOOLEAN ("Use apriori probabilities", 1)
	OK
DO
	Discriminant d = ONLY (classDiscriminant);
	TableOfReal t = ONLY_GENERIC (classTableOfReal);
	if (! praat_new3 (Discriminant_and_TableOfReal_to_ClassificationTable
		(d,	t, GET_INTEGER ("Pool covariance matrices"),
		GET_INTEGER ("Use apriori probabilities")),
		Thing_getNameW (d), L"_", Thing_getNameW (t))) return 0;
END

FORM (Discriminant_getWilksLambda, "Discriminant: Get Wilks' lambda",
	"Discriminant: Get Wilks' lambda...")
	LABEL ("", "Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	INTEGER ("From", "1")
	OK
DO
	long from = GET_INTEGER ("From");
	REQUIRE (from >= 1, "Number must be greater than or equal to one.")
	Melder_information1 (Melder_double (Discriminant_getWilksLambda (ONLY_OBJECT, from)));
END

FORM (Discriminant_getCumulativeContributionOfComponents,
	"Discriminant: Get cumulative contribution of components",
	"Eigen: Get cumulative contribution of components...")
	NATURAL ("From component", "1")
	NATURAL ("To component", "1")
	OK
DO
	Melder_information1 (Melder_double (Eigen_getCumulativeContributionOfComponents
		(ONLY_OBJECT, GET_INTEGER ("From component"),
		GET_INTEGER ("To component"))));
END


FORM (Discriminant_getPartialDiscriminationProbability,
	"Discriminant: Get partial discrimination probability",
	"Discriminant: Get partial discrimination probability...")
	INTEGER ("Number of dimensions", "1")
	OK
DO
	long ndf, n = GET_INTEGER ("Number of dimensions");
	double chisq, p;
	REQUIRE (n >= 0, "Number of dimensions must be greater than or equal "
		"to zero.")
	Discriminant_getPartialDiscriminationProbability (ONLY_OBJECT, n,
		&p, &chisq, &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

DIRECT (Discriminant_getHomegeneityOfCovariances_box)
	double chisq, p; long ndf; Discriminant thee = ONLY_OBJECT;
	SSCPs_getHomegeneityOfCovariances_box (thy groups, &p, &chisq, &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

FORM (Discriminant_getConcentrationEllipseArea,
	"Discriminant: Get concentration ellipse area",
	"Discriminant: Get concentration ellipse area...")
	SENTENCE ("Group label", "")
	POSITIVE ("Number of sigmas", "1.0")
	BOOLEAN ("Discriminant plane", 1)
	INTEGER ("X-dimension", "1")
	INTEGER ("Y-dimension", "2")
	OK
DO
	Discriminant d = ONLY_OBJECT;
	long group = Discriminant_groupLabelToIndex (d, GET_STRINGW (L"Group label"));
	REQUIRE (group > 0, "Group label does not exist.")
	Melder_information1 (Melder_double (Discriminant_getConcentrationEllipseArea(d, group,
		GET_REAL ("Number of sigmas"), 0, GET_INTEGER ("Discriminant plane"),
		GET_INTEGER ("X-dimension"), GET_INTEGER ("Y-dimension"))));
END

FORM (Discriminant_getConfidenceEllipseArea, "Discriminant: Get confidence ellipse area",
	"Discriminant: Get confidence ellipse area...")
	SENTENCE ("Group label", "")
	POSITIVE ("Confidence level (0-1)", "0.95")
	BOOLEAN ("Discriminant plane", 1)
	INTEGER ("X-dimension", "1")
	INTEGER ("Y-dimension", "2")
	OK
DO
	Discriminant d = ONLY_OBJECT;
	long group = Discriminant_groupLabelToIndex (d, GET_STRINGW (L"Group label"));
	REQUIRE (group > 0, "Group label does not exist.")
	Melder_information1 (Melder_double (Discriminant_getConcentrationEllipseArea(d, group,
		GET_REAL ("Confidence level"), 1, GET_INTEGER ("Discriminant plane"),
		GET_INTEGER ("X-dimension"), GET_INTEGER ("Y-dimension"))));
END

FORM (Discriminant_getLnDeterminant_group,
	"Discriminant: Get determinant (group)",
	"Discriminant: Get determinant (group)...")
	SENTENCE ("Group label", "")
	OK
DO
	Discriminant d = ONLY_OBJECT;
	long group = Discriminant_groupLabelToIndex (d, GET_STRINGW (L"Group label"));
	REQUIRE (group > 0, "Group label does not exist.")
	Melder_information1 (Melder_double (Discriminant_getLnDeterminant_group (d, group)));
END

DIRECT (Discriminant_getLnDeterminant_total)
	Melder_information1 (Melder_double (Discriminant_getLnDeterminant_total (ONLY_OBJECT)));
END

FORM (Discriminant_invertEigenvector, "Discriminant: Invert eigenvector", 0)
	NATURAL ("Index of eigenvector", "1")
	OK
DO
	EVERY (Eigen_invertEigenvector (OBJECT,
		GET_INTEGER ("Index of eigenvector")))
END

FORM (Discriminant_drawSigmaEllipses, "Discriminant: Draw sigma ellipses",
	"Discriminant: Draw sigma ellipses...")
	POSITIVE ("Number of sigmas", "1.0")
	BOOLEAN ("Discriminant plane", 1)
	INTEGER ("X-dimension", "1")
	INTEGER ("Y-dimension", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Number of sigmas"), 0, NULL, GET_INTEGER ("Discriminant plane"),
		GET_INTEGER ("X-dimension"), GET_INTEGER ("Y-dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

FORM (Discriminant_drawOneSigmaEllipse, "Discriminant: Draw one sigma ellipse",
	"Discriminant: Draw one sigma ellipse...")
	SENTENCE ("Label", "")
	POSITIVE ("Number of sigmas", "1.0")
	BOOLEAN ("Discriminant plane", 1)
	INTEGER ("X-dimension", "1")
	INTEGER ("Y-dimension", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Number of sigmas"), 0, GET_STRINGW (L"Label"),
		GET_INTEGER ("Discriminant plane"),
		GET_INTEGER ("X-dimension"), GET_INTEGER ("Y-dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

FORM (Discriminant_drawConfidenceEllipses, "Discriminant: Draw confidence ellipses", 0)
	POSITIVE ("Confidence level (0-1)", "0.95")
	BOOLEAN ("Discriminant plane", 1)
	INTEGER ("X-dimension", "1")
	INTEGER ("Y-dimension", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Confidence level"), 1, NULL, GET_INTEGER ("Discriminant plane"),
		GET_INTEGER ("X-dimension"), GET_INTEGER ("Y-dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END


FORM (Discriminant_drawOneConfidenceEllipse, "Discriminant: Draw one confidence ellipse", 0)
	SENTENCE ("Label", "")
	POSITIVE ("Confidence level (0-1)", "0.95")
	BOOLEAN ("Discriminant plane", 1)
	INTEGER ("X-dimension", "1")
	INTEGER ("Y-dimension", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Discriminant_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Confidence level"), 1, GET_STRINGW (L"Label"), GET_INTEGER ("Discriminant plane"),
		GET_INTEGER ("X-dimension"), GET_INTEGER ("Y-dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

DIRECT (Discriminant_extractBetweenGroupsSSCP)
	EVERY_TO (Discriminant_extractBetweenGroupsSSCP (OBJECT))
END
/*
FORM (Discriminant_extractCoefficients, "Extract coefficients...", 0)
	RADIO ("Function coefficients", 1)
	RADIOBUTTON ("Raw")
	RADIOBUTTON ("Unstandardized")
	RADIOBUTTON ("Standardized")
	OK
DO
	EVERY_TO (Discriminant_extractCoefficients (OBJECT, GET_INTEGER ("Function coefficients")))
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

FORM (Discriminant_extractWithinGroupSSCP, "Discriminant: Extract within-group SSCP",
	"Discriminant: Extract within-group SSCP...")
	NATURAL ("Group index", "1")
	OK
DO
	EVERY_TO (Discriminant_extractWithinGroupSSCP (OBJECT,
		GET_INTEGER ("Group index")))
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

FORM (Discriminant_getNumberOfObservations, "Discriminant: Get number of observations", 
	"Discriminant: Get number of observations...")
	INTEGER ("Group", "0 (=total)")
	OK
DO
	Melder_information1 (Melder_integer (Discriminant_getNumberOfObservations (ONLY_OBJECT, GET_INTEGER ("Group"))));
END

	
/********************** DTW *******************************************/

void DTW_constraints_addCommonFields (void *dia)
{
	Any radio;
	LABEL("", "Boundary conditions")
	BOOLEAN ("Match begin positions", 0)
	BOOLEAN ("Match end positions", 0)
	RADIO ("Slope constraints", 1)
	RADIOBUTTON ("no restriction")
	RADIOBUTTON ("1/3 < slope < 3")
	RADIOBUTTON ("1/2 < slope < 2")
	RADIOBUTTON ("2/3 < slope < 3/2")
}

void DTW_constraints_getCommonFields (void *dia, int *begin, int *end, int *slope)
{
	*begin = GET_INTEGER ("Match begin positions");
	*end = GET_INTEGER ("Match end positions");
	*slope = GET_INTEGER ("Slope constraints");
}

DIRECT (DTW_help) Melder_help (L"DTW"); END

FORM (DTW_drawPath, "DTW: Draw path", 0)
    REAL ("left Horizontal range", "0.0")
    REAL ("right Horizontal range", "0.0")
    REAL ("left Vertical range", "0.0")
    REAL ("right Vertical range", "0.0")
    BOOLEAN ("Garnish", 0);
    OK
DO
    EVERY_DRAW (DTW_drawPath (OBJECT, GRAPHICS,
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER("Garnish")))
END

FORM (DTW_drawDistancesAlongPath, "DTW: Draw distances along path", 0)
    REAL ("left Horizontal range", "0.0")
    REAL ("right Horizontal range", "0.0")
    REAL ("left Vertical range", "0.0")
    REAL ("right Vertical range", "0.0")
    BOOLEAN ("Garnish", 0);
    OK
DO
    EVERY_DRAW (DTW_drawDistancesAlongPath (OBJECT, GRAPHICS,
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER("Garnish")))
END

FORM (DTW_paintDistances, "DTW: Paint distances", 0)
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
    BOOLEAN ("Garnish", 0);
	OK
DO
	EVERY_DRAW (DTW_paintDistances (OBJECT, GRAPHICS,
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_REAL ("Minimum"), GET_REAL ("Maximum"),
		GET_INTEGER("Garnish")))
END

FORM (DTW_getPathY, "DTW: Get time along path", "DTW: Get time along path...")
	REAL ("Time (s)", "0.0")
	OK
DO
	Melder_information1 (Melder_double (DTW_getPathY (ONLY_OBJECT, GET_REAL ("Time"))));	
END

FORM (DTW_getYTime, "DTW: Get y time", "DTW: Get y time...")
	REAL ("Time at x (s)", "0.0")
	OK
DO
	Melder_information1 (Melder_double (DTW_getYTime (ONLY_OBJECT, GET_REAL ("Time at x"))));	
END

FORM (DTW_getXTime, "DTW: Get x time", "DTW: Get x time...")
	REAL ("Time at y (s)", "0.0")
	OK
DO
	Melder_information1 (Melder_double (DTW_getXTime (ONLY_OBJECT, GET_REAL ("Time at y"))));	
END

FORM (DTW_getMaximumConsecutiveSteps, "DTW: Get maximum consecutive steps", "DTW: Get maximum consecutive steps...")
	OPTIONMENU ("Direction", 1)
	OPTION ("X")
	OPTION ("Y")
	OPTION ("Diagonaal")
	OK
DO
	int direction[] = {DTW_START, DTW_X, DTW_Y, DTW_XANDY};
	wchar_t *string[] = {L"", L"x", L"y", L"diagonal"};
	int d = GET_INTEGER ("Direction");
	Melder_information4 (Melder_integer (DTW_getMaximumConsecutiveSteps (ONLY_OBJECT, direction[d])), 
		L" (=maximum number of consecutive steps in ", string[d], L" direction");
END

DIRECT (DTW_getWeightedDistance)
	DTW me = ONLY_OBJECT;
	Melder_information1 (Melder_double (my weightedDistance)); 
END

FORM (DTW_findPath, "DTW: Find path", 0)
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	EVERY (DTW_findPath (OBJECT, begin, end, slope))
END

#ifdef INCLUDE_DTW_SLOPES
FORM (DTW_pathFinder_slopes, "DTW: Find path (slopes)", "DTW: Find path (slopes)...")
	LABEL ("", "Slope constraints:")
	LABEL ("", "This number of")
	INTEGER ("Non-diagonal steps", "1")
	LABEL ("", "must be followed by at least this number of")
	INTEGER ("Diagonal steps", "0 (=no constraints)")
	LABEL ("", "Directional weights")
	REAL ("X weight", "1.0")
	REAL ("Y weight", "1.0")
	REAL ("Diagonal weight", "2.0")
	OK
DO
	EVERY_CHECK (DTW_pathFinder_slopes (OBJECT, GET_INTEGER ("Non-diagonal steps"), GET_INTEGER ("Diagonal steps"),
		GET_REAL ("X weight"), GET_REAL ("Y weight"), GET_REAL ("Diagonal weight")))
END
#endif

FORM (DTW_pathFinder_band, "DTW: Find path (Sakoe-Chiba band)", "DTW: Find path (band)...")
	REAL ("Adjustment window duration (s)", "0.1")
	BOOLEAN ("Adjustment window includes end", 0)
	LABEL ("", "Directional weights")
	REAL ("X weight", "1.0")
	REAL ("Y weight", "1.0")
	REAL ("Diagonal weight", "2.0")
	OK
DO
	EVERY_CHECK (DTW_pathFinder_band (OBJECT, GET_REAL ("Adjustment window duration"),
		GET_INTEGER ("Adjustment window includes end"),
		GET_REAL ("X weight"), GET_REAL ("Y weight"), GET_REAL ("Diagonal weight")))
END

FORM (DTW_to_Polygon_slopes, "DTW: To Polygon (slopes)", "DTW: To Polygon (slopes)...")
	LABEL ("", "Slope constraints:")
	LABEL ("", "This number of")
	INTEGER ("Non-diagonal steps", "1")
	LABEL ("", "must be followed by at least this number of")
	INTEGER ("Diagonal steps", "0 (=no constraints)")
	OK
DO
	EVERY_TO (DTW_to_Polygon_slopes (OBJECT, GET_INTEGER ("Non-diagonal steps"), GET_INTEGER ("Diagonal steps")))
END

FORM (DTW_to_Polygon_band, "DTW: To Polygon (band)", "DTW: To Polygon (band)...")
	REAL ("Adjustment window duration (s)", "0.1")
	BOOLEAN ("Adjustment window includes end", 0)
	OK
DO
	EVERY_TO (DTW_to_Polygon_band (OBJECT, GET_REAL ("Adjustment window duration"), GET_INTEGER ("Adjustment window includes end")))
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
	Melder_warning ("The command \"Draw eigenvalues (scree)...\" has been "
		"removed.\n To get a scree plot use \"Draw eigenvalues...\" with the "
		"arguments\n 'Fraction of eigenvalues summed' and 'Cumulative' unchecked.");
END

FORM (Eigen_drawEigenvalues, "Eigen: Draw eigenvalues", "Eigen: Draw eigenvalues...")
	INTEGER ("left Eigenvalue range", "0")
	INTEGER ("right Eigenvalue range", "0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	BOOLEAN ("Fraction of eigenvalues summed", 0)
	BOOLEAN ("Cumulative", 0)
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Eigen_drawEigenvalues (OBJECT, GRAPHICS, 
		GET_INTEGER ("left Eigenvalue range"), GET_INTEGER ("right Eigenvalue range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"), 
		GET_INTEGER ("Fraction of eigenvalues summed"), GET_INTEGER ("Cumulative"),
		GET_REAL ("Mark size"),
		GET_STRINGW (L"Mark string"), GET_INTEGER ("Garnish")))
END

FORM (Eigen_drawEigenvector, "Eigen: Draw eigenvector", "Eigen: Draw eigenvector...")
	INTEGER ("Eigenvector number", "1")
	BOOLEAN ("Component loadings", 0)
	INTEGER ("left Element range", "0")
	INTEGER ("right Element range", "0")
	REAL ("left Amplitude range", "-1.0")
	REAL ("right Amplitude range", "1.0")
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
	BOOLEAN ("Connect points", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Eigen_drawEigenvector (OBJECT, GRAPHICS, 
		GET_INTEGER ("Eigenvector number"),
		GET_INTEGER ("left Element range"), GET_INTEGER ("right Element range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"), 
		GET_INTEGER ("Component loadings"), GET_REAL ("Mark size"),
		GET_STRINGW (L"Mark string"), GET_INTEGER ("Connect points"), NULL,
		GET_INTEGER ("Garnish")))
END

DIRECT (Eigen_getNumberOfEigenvalues)
	Eigen me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfEigenvalues));
END

DIRECT (Eigen_getDimension)
	Eigen me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my dimension));
END

FORM (Eigen_getEigenvalue, "Eigen: Get eigenvalue", "Eigen: Get eigenvalue...")
	NATURAL ("Eigenvalue number", "1")
	OK
DO
	Eigen me = ONLY_OBJECT;
	long number = GET_INTEGER ("Eigenvalue number");
	if (number > my numberOfEigenvalues) return Melder_error
		("DO_Eigen_getEigenvalue: Eigenvalue number must be smaller than %ld",
		my numberOfEigenvalues + 1);
	Melder_information1 (Melder_double (my eigenvalues[number]));
END

FORM (Eigen_getSumOfEigenvalues, "Eigen:Get sum of eigenvalues", "Eigen: Get sum of eigenvalues...")
	INTEGER ("left Eigenvalue range",  "0")
	INTEGER ("right Eigenvalue range", "0")
	OK
DO
	Melder_information1 (Melder_double (Eigen_getSumOfEigenvalues(ONLY_OBJECT,
		GET_INTEGER ("left Eigenvalue range"), GET_INTEGER ("right Eigenvalue range"))));
END

FORM (Eigen_getEigenvectorElement, "Eigen: Get eigenvector element",
	"Eigen: Get eigenvector element...")
	NATURAL ("Eigenvector number", "1")
	NATURAL ("Element number", "1")
	OK
DO
	Melder_information1 (Melder_double (Eigen_getEigenvectorElement (ONLY_OBJECT, 
		GET_INTEGER ("Eigenvector number"), GET_INTEGER ("Element number"))));
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

FORM (Eigen_and_Matrix_project, "Eigen & Matrix: Project", "Eigen & Matrix: "
	"Project...")
	INTEGER ("Number of dimensions", "0")
	OK
DO
	NEW (Eigen_and_Matrix_project (ONLY_GENERIC (classEigen), 
		ONLY_GENERIC (classMatrix), GET_INTEGER ("Number of dimensions")))
END 

DIRECT (Eigen_and_SSCP_project)
	NEW(Eigen_and_SSCP_project (ONLY_GENERIC (classEigen),
		ONLY (classSSCP)))
END

DIRECT (Eigen_and_Covariance_project)
	NEW(Eigen_and_Covariance_project (ONLY_GENERIC (classEigen),
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

FORM (StringsIndex_getClassLabel, "StringsIndex: Get class label", "StringsIndex: Get class label...")
	NATURAL ("Class index", "1")
	OK
DO
	StringsIndex thee = ONLY_OBJECT;
	long klas = GET_INTEGER ("Class index");
	long numberOfClasses = thy classes -> size;
	SimpleString ss;
	if (klas > numberOfClasses) return Melder_error
		("Element index must be less than or equal %ld.", numberOfClasses);
	ss = thy classes -> item[klas];
	Melder_information1 (ss -> string);
END

FORM (StringsIndex_getLabel, "StringsIndex: Get label", "StringsIndex: Get label...")
	NATURAL ("Element index", "1")
	OK
DO
	StringsIndex thee = ONLY_OBJECT;
	long klas, index = GET_INTEGER ("Element index");
	SimpleString ss;
	if (index > thy numberOfElements) return Melder_error
		("Element index must be less than or equal %ld.", thy numberOfElements);
	klas = thy classIndex[index];
	ss = thy classes -> item [klas];	
	Melder_information1 (ss -> string);
END

FORM (Index_getIndex, "Index: Get index", "Index: Get index...")
	NATURAL ("Element index", "1")
	OK
DO
	Index thee = ONLY_OBJECT;
	long index = GET_INTEGER ("Element index");
	if (index > thy numberOfElements) return Melder_error
		("Element index must be less than or equal %ld.", thy numberOfElements);
	Melder_information1 (Melder_integer (thy classIndex[index]));
END

FORM (StringsIndex_getClassIndex, "StringsIndex: Get class index", "StringsIndex: Get class index...")
	WORD ("Class label", "label")
	OK
DO
	StringsIndex thee = ONLY_OBJECT;
	wchar_t *klasLabel = GET_STRINGW (L"Class label");
	long index = StringsIndex_getClass (thee, klasLabel);
	Melder_information1 (Melder_integer (index));
END

FORM (Index_extractPart, "Index: Extract part", "Index: Extract part...")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	OK
DO
	Index thee = ONLY_OBJECT;
	if (! praat_new2 (Index_extractPart (thee, GET_INTEGER ("left Range"), GET_INTEGER ("right Range")),
		Thing_getNameW (thee), L"_part")) return 0;
END

FORM (Index_to_Permutation, "Index: To Permutation", "Index: To Permutation...")
	BOOLEAN ("Permute within classes", 1)
	OK
DO
	EVERY_TO (Index_to_Permutation_permuteRandomly (OBJECT, GET_INTEGER ("Permute within classes")))
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
		(void) Collection_addItem (e, OBJECT);
		OBJECT = NULL; praat_removeObject (IOBJECT);
	}
	praat_show();
	NEW (e)
END

/******************** Excitations ********************************************/

FORM (Excitations_formula,"Excitations: Formula", 0)
	LABEL ("label", "for all objects in Excitations do { " 
		"for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	TEXTFIELD ("formula", "self")
	OK
DO
	WHERE (SELECTED && Thing_member (OBJECT, classExcitations))
	{
		Ordered ms = OBJECT;
		int j;
		for (j = 1; j <= ms -> size; j++)
			if (! Matrix_formula (ms->item[j], GET_STRINGW (L"formula"), NULL)) break;
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

DIRECT (Excitations_addItem)
	Excitations e = NULL;
	WHERE (SELECTED && CLASS == classExcitations) e = OBJECT;
	WHERE_DOWN (SELECTED && CLASS == classExcitation)
	{ 
		(void) Collection_addItem (e, OBJECT);
		OBJECT = NULL; praat_removeObject (IOBJECT); praat_show();
	}
	praat_show();
END

FORM (Excitations_getItem, "Excitations: Get item", 0)
	NATURAL ("Item number", "1")
	OK
DO
	WHERE (SELECTED && CLASS == classExcitations)
	{
		Excitation me = Excitations_getItem (OBJECT, GET_INTEGER("Item number"));
		if (! praat_new1 (me, Thing_getNameW (me))) return 0;
	}
END

DIRECT (Excitations_append)
   Data e1 = NULL, e2 = NULL;
   WHERE (SELECTED && CLASS == classExcitations) { if (e1) e2 = OBJECT; else e1 = OBJECT; }
   NEW (Collections_merge (e1, e2))
END

FORM (Excitations_to_Pattern,"Excitations: To Pattern", 0)
	NATURAL ("Join", "1")
	OK
DO
    EVERY_TO (Excitations_to_Pattern (OBJECT, GET_INTEGER ("Join")))
END

DIRECT (Excitations_to_TableOfReal) 
	EVERY_TO (Excitations_to_TableOfReal (OBJECT))
END

/************************* FilterBank ***********************************/

FORM (FilterBank_drawFilters, "FilterBank: Draw filters", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_drawRows (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range")))
END

FORM (FilterBank_drawOneContour, "FilterBank: Draw one contour", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("Height (dB)", "40.0")
	OK
DO
	EVERY_DRAW (Matrix_drawOneContour (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("Height")))
END

FORM (FilterBank_drawContours, "FilterBank: Draw contours", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_drawContours (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range")))
END

FORM (FilterBank_drawFrequencyScales, "FilterBank: Draw frequency scales", 
	"FilterBank: Draw frequency scales...")
	RADIO ("Horizontal frequency scale", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	REAL ("left Horizontal frequency range", "0.0")
	REAL ("right Horizontal frequency range", "0.0")
	RADIO ("Vertical frequency scale", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	REAL ("left Vertical frequency range", "0.0")
	REAL ("right Vertical frequency range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (FilterBank_drawFrequencyScales (OBJECT, GRAPHICS,
		GET_INTEGER ("Horizontal frequency scale"), 
		GET_REAL ("left Horizontal frequency range"),
		GET_REAL ("right Horizontal frequency range"),
		GET_INTEGER ("Vertical frequency scale"), 
		GET_REAL ("left Vertical frequency range"),
		GET_REAL ("right Vertical frequency range"),GET_INTEGER ("Garnish")))
END

FORM (FilterBank_paintImage, "FilterBank: Paint image", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintImage (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range")))
END

FORM (FilterBank_paintContours, "FilterBank: Paint contours", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintContours (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range")))
END


FORM (FilterBank_paintCells, "FilterBank: Paint cells", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintCells (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range")))
END

FORM (FilterBank_paintSurface, "FilterBank: Paint surface", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintSurface (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"),
		30, 45))
END

FORM (FilterBank_getFrequencyInHertz, "FilterBank: Get frequency in Hertz", 
	"FilterBank: Get frequency in Hertz...")
	REAL ("Frequency", "10.0")
	RADIO ("Unit", 2)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	OK
DO
	double f = FilterBank_getFrequencyInHertz (ONLY_OBJECT, 
		GET_REAL ("Frequency"), GET_INTEGER ("Unit"));
	Melder_informationReal (f, L"Hertz");
END

FORM (FilterBank_getFrequencyInBark, "FilterBank: Get frequency in Bark", 
	"FilterBank: Get frequency in Bark...")
	REAL ("Frequency", "93.17")
	RADIO ("Unit", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	OK
DO
	Melder_informationReal (FilterBank_getFrequencyInBark (ONLY_OBJECT, GET_REAL ("Frequency"), 
		GET_INTEGER ("Unit")), L"Bark");
END

FORM (FilterBank_getFrequencyInMel, "FilterBank: Get frequency in mel", 
	"FilterBank: Get frequency in mel...")
	REAL ("Frequency", "1000.0")
	RADIO ("Unit", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	OK
DO
	double f = FilterBank_getFrequencyInMel (ONLY_OBJECT, 
		GET_REAL ("Frequency"), GET_INTEGER ("Unit"));
	Melder_informationReal (f, L"mel");
END
	
FORM (FilterBank_equalizeIntensities, "FilterBank: Equalize intensities", "")
	REAL ("Intensity (dB)", "80.0")
	OK
DO
	EVERY (FilterBank_equalizeIntensities (OBJECT, GET_REAL ("Intensity")))	
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

FORM (FormantFilter_drawFilterFunctions, "FormantFilter: Draw filter functions", 
	"FilterBank: Draw filter functions...")
	INTEGER ("left Filter range", "0")
	INTEGER ("right Filter range", "0")
	POSITIVE ("Bandwidth (Hz)", "100.0")
	RADIO ("Frequency scale", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("mel")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	BOOLEAN ("Amplitude scale in dB", 1)
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (FormantFilter_drawFilterFunctions (OBJECT, GRAPHICS,
		GET_REAL ("Bandwidth"), GET_INTEGER ("Frequency scale"),
		GET_INTEGER ("left Filter range"), GET_INTEGER ("right Filter range"),
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_INTEGER ("Amplitude scale in dB"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"), GET_INTEGER ("Garnish")))
END

FORM (FormantFilter_drawSpectrum, "FormantFilter: Draw spectrum (slice)", "FilterBank: Draw spectrum (slice)...")
	POSITIVE ("Time (s)", "0.1")
	REAL ("left Frequency range (Hz)", "0.0")
	REAL ("right Frequency range (Hz)", "0.0")
	REAL ("left Amplitude range (dB)", "0.0")
	REAL ("right Amplitude range (dB)", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO	
	EVERY_DRAW (FilterBank_drawTimeSlice (OBJECT, GRAPHICS, 
		GET_REAL ("Time"), GET_REAL ("left Frequency range"),
		GET_REAL ("right Frequency range"), GET_REAL ("left Amplitude range"),
		GET_REAL ("right Amplitude range"), L"Hz", GET_INTEGER ("Garnish")))
END

/****************** FunctionTerms  *********************************/

FORM (FunctionTerms_draw, "FunctionTerms: Draw", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Extrapolate", 0)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (FunctionTerms_draw (OBJECT, GRAPHICS, GET_REAL ("Xmin"), GET_REAL ("Xmax"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Extrapolate"), GET_INTEGER ("Garnish")))
END

FORM (FunctionTerms_drawBasisFunction, "FunctionTerms: Draw basis function", 0)
	NATURAL ("Index", "1")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Extrapolate", 0)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (FunctionTerms_drawBasisFunction (OBJECT, GRAPHICS,
		GET_INTEGER ("Index"),
		GET_REAL ("Xmin"), GET_REAL ("Xmax"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Extrapolate"), GET_INTEGER ("Garnish")))
END

FORM (FunctionTerms_evaluate, "FunctionTerms: Evaluate", 0)
	REAL ("X", "0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	Melder_information1 (Melder_double (FunctionTerms_evaluate (f, GET_REAL ("X"))));
END

DIRECT (FunctionTerms_getNumberOfCoefficients)
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	Melder_information1 (Melder_integer (f -> numberOfCoefficients));
END

FORM (FunctionTerms_getCoefficient, "FunctionTerms: Get coefficient", 0)
	LABEL ("", "p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURAL ("Index", "1")
	OK
DO
	long index = GET_INTEGER ("Index");
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	REQUIRE (index <= f -> numberOfCoefficients, "Index too large.")
	Melder_information1 (Melder_double (f -> coefficients[index]));
END
	
DIRECT (FunctionTerms_getDegree)
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	Melder_information1 (Melder_integer (FunctionTerms_getDegree (f)));
END

FORM (FunctionTerms_getMaximum, "FunctionTerms: Get maximum", "Polynomial: Get maximum...")
	LABEL ("", "Interval")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getMaximum (f, GET_REAL ("Xmin"),
		GET_REAL ("Xmax"));
	Melder_information1 (Melder_double (x)); 
END

FORM (FunctionTerms_getMinimum, "FunctionTerms: Get minimum", "Polynomial: Get minimum...")
	LABEL ("", "Interval")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getMinimum (f, GET_REAL ("Xmin"),
		GET_REAL ("Xmax"));
	Melder_information1 (Melder_double (x)); 
END

FORM (FunctionTerms_getXOfMaximum, "FunctionTerms: Get x of maximum",
	"Polynomial: Get x of maximum...")
	LABEL ("", "Interval")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getXOfMaximum (f, GET_REAL ("Xmin"),
		GET_REAL ("Xmax"));
	Melder_information1 (Melder_double (x)); 
END

FORM (FunctionTerms_getXOfMinimum, "FunctionTerms: Get x of minimum",
	"Polynomial: Get x of minimum...")
	LABEL ("", "Interval")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double x = FunctionTerms_getXOfMinimum (f, GET_REAL ("Xmin"),
		GET_REAL ("Xmax"));
	Melder_information1 (Melder_double (x)); 
END

FORM (FunctionTerms_setCoefficient, "FunctionTerms: Set coefficient", 0)
	LABEL ("", "p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	LABEL ("", "F[k] is of degree k")
	NATURAL ("Index", "1")
	REAL ("Value", "0.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	if (! FunctionTerms_setCoefficient (f, GET_INTEGER ("Index"),
		GET_REAL ("Value"))) return 0;
END

FORM (FunctionTerms_setDomain, "FunctionTerms: Set domain", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "2.0")
	OK
DO
	FunctionTerms f = ONLY_GENERIC (classFunctionTerms);
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	REQUIRE (xmin < xmax, "Xmax must be larger than Xmin.")
	FunctionTerms_setDomain (f, xmin, xmax);
END

/***************** Intensity ***************************************************/

FORM (Intensity_to_TextGrid_detectSilences, "Intensity: To TextGrid (silences)", "Intensity: To TextGrid (silences)...")
	REAL ("Silence threshold (dB)", "-25.0")
	POSITIVE ("Minimum silent interval duration (s)", "0.1")
	POSITIVE ("Minimum sounding interval duration (s)", "0.1")
	WORD ("Silent interval label", "silent")
	WORD ("Sounding interval label", "sounding")
	OK
DO
	EVERY_TO (Intensity_to_TextGrid_detectSilences (OBJECT, GET_REAL ("Silence threshold"),
		GET_REAL ("Minimum silent interval duration"), GET_REAL ("Minimum sounding interval duration"),
		GET_STRINGW (L"Silent interval label"), GET_STRINGW (L"Sounding interval label")))
END

/***************** ISpline ***************************************************/

DIRECT (ISpline_help) Melder_help (L"ISpline"); END

FORM (ISpline_create, "Create ISpline", "Create ISpline...")
	WORD ("Name", "ispline")
	LABEL ("", "Domain")
	REAL ("Xmin", "0")
	REAL ("Xmax", "1")
	LABEL ("", "ISpline(x) = c[1] I[1](x) + c[2] I[1](x) + ... c[n] I[n](x)")
	LABEL ("", "all I[k] are polynomials of degree \"Degree\"")
	LABEL ("", "Relation: numberOfCoefficients == numberOfInteriorKnots + "
		"degree")
	INTEGER ("Degree", "3")
	SENTENCE ("Coefficients (c[k])", "1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE ("Interior knots" , "0.3 0.5 0.6")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	long degree = GET_INTEGER ("Degree");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	if (! praat_new1 (ISpline_createFromStrings (xmin, xmax, degree,
		GET_STRINGW (L"Coefficients"), GET_STRINGW (L"Interior knots")),
		GET_STRINGW (L"Name"))) return 0;
END

/******************* LegendreSeries *********************************/

FORM (LegendreSeries_create, "Create LegendreSeries",
	"Create LegendreSeries...")
	WORD ("Name", "ls")
	LABEL ("", "Domain")
	REAL ("Xmin", "-1")
	REAL ("Xmax", "1")
	LABEL ("", "LegendreSeries(x) = c[1] P[0](x) + c[2] P[1](x) + "
		"... c[n+1] P[n](x)")
	LABEL ("", "P[k] is a Legendre polynomial of degree k")
	SENTENCE ("Coefficients", "0 0 1.0")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	if (! praat_new1 (LegendreSeries_createFromString (xmin, xmax,
		GET_STRINGW (L"Coefficients")), GET_STRINGW (L"Name"))) return 0;
END

DIRECT (LegendreSeries_help) Melder_help (L"LegendreSeries"); END

DIRECT (LegendreSeries_to_Polynomial)
	EVERY_TO (LegendreSeries_to_Polynomial (OBJECT))
END
/********************* LongSound **************************************/

FORM_READ (LongSounds_appendToExistingSoundFile, "LongSound: Append to existing sound file", 0)
	if (! pr_LongSounds_appendToExistingSoundFile (file)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoAiffFile, "LongSound: Write to AIFF file", 0, "aiff")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_AIFF)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoAifcFile, "LongSound: Write to AIFC file", 0, "aifc")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_AIFC)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoWavFile, "LongSound: Write to WAV file", 0, "wav")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_WAV)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoNextSunFile, "LongSound: Write to NeXT/Sun file", 0, "au")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (LongSounds_writeToStereoNistFile, "LongSound: Write to NIST file", 0, "nist")
	if (! pr_LongSounds_writeToStereoAudioFile (file, Melder_NIST)) return 0;
END
	   
/******************* Matrix **************************************************/

FORM (Matrix_drawAsSquares,"Matrix: Draw as squares", "Matrix: Draw as squares...")
    REAL("left Horizontal range", "0.0")
    REAL("right Horizontal range", "0.0")
    REAL("left Vertical range", "0.0")
    REAL("right Vertical range", "0.0")
    BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Matrix_drawAsSquares (OBJECT, GRAPHICS,
    	GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
    	GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER("Garnish")))
END

FORM (Matrix_drawDistribution, "Matrix: Draw distribution", "Matrix: Draw distribution...")
	LABEL ("", "Selection of (part of) Matrix")
    REAL ("left Horizontal range", "0.0")
    REAL ("right Horizontal range", "0.0")
    REAL ("left Vertical range", "0.0")
    REAL ("right Vertical range", "0.0")
    LABEL ("", "Selection of Matrix values")
    REAL ("Minimum value", "0.0")
    REAL ("Maximum value", "0.0")
    LABEL ("", "Display of the distribution")
    NATURAL ("Number of bins", "10")
    REAL ("Minimum frequency", "0.0")
    REAL ("Maximum frequency", "0.0")    
    BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Matrix_drawDistribution (OBJECT, GRAPHICS,
    	GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
    	GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
    	GET_REAL ("Minimum value"), GET_REAL ("Maximum value"),
    	GET_INTEGER ("Number of bins"),
    	GET_REAL ("Minimum frequency"), GET_REAL ("Maximum frequency"), 0,
		GET_INTEGER("Garnish")))
END

FORM (Matrix_drawCumulativeDistribution, 
	"Matrix: Draw cumulative distribution", 0)
	LABEL ("", "Selection of (part of) Matrix")
    REAL ("left Horizontal range", "0.0")
    REAL ("right Horizontal range", "0.0")
    REAL ("left Vertical range", "0.0")
    REAL ("right Vertical range", "0.0")
    LABEL ("", "Selection of Matrix values")
    REAL ("Minimum value", "0.0")
    REAL ("Maximum value", "0.0")
    LABEL ("", "Display of the distribution")
    NATURAL ("Number of bins", "10")
    REAL ("Minimum", "0.0")
    REAL ("Maximum", "0.0")    
    BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Matrix_drawDistribution (OBJECT, GRAPHICS,
    	GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
    	GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
    	GET_REAL ("Minimum value"), GET_REAL ("Maximum value"),
    	GET_INTEGER ("Number of bins"),
    	GET_REAL ("Minimum"), GET_REAL ("Maximum"), 1,
		GET_INTEGER("Garnish")))
END

FORM (Matrix_scale, "Matrix: Scale", 0)
	LABEL ("", "self[row, col] := self[row, col] / `Scale factor'")
	RADIO ("Scale factor", 1)
	RADIOBUTTON ("Extremum in matrix")
	RADIOBUTTON ("Extremum in each row")
	RADIOBUTTON ("Extremum in each column")
	OK
DO
	int scale = GET_INTEGER ("Scale factor");
	REQUIRE (scale > 0 && scale < 4, "illegal value for scale.") 
	EVERY (Matrix_scale (OBJECT, scale))
END

DIRECT (Matrix_transpose)
	EVERY_TO (Matrix_transpose (OBJECT))
END

FORM (Matrix_solveEquation, "Matrix: Solve equation",
	"Matrix: Solve equation...")
	REAL ("Tolerance", "1.19e-7")
	OK
DO
	WHERE (SELECTED)
	{
		if (! praat_new2 (Matrix_solveEquation (OBJECT, GET_REAL ("Tolerance")), NAMEW, L"_solution")) return 0;
	}
END

DIRECT (Matrix_Categories_to_TableOfReal)
	NEW (Matrix_and_Categories_to_TableOfReal (ONLY_GENERIC (classMatrix),
		ONLY (classCategories)))
END



FORM (Matrix_scatterPlot, "Matrix: Scatter plot", 0)
    NATURAL ("Column for X-axis", "1")
    NATURAL ("Column for Y-axis", "2")
    REAL("left Horizontal range", "0.0")
    REAL("right Horizontal range", "0.0")
    REAL("left Vertical range", "0.0")
    REAL("right Vertical range", "0.0")
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
    BOOLEAN ("Garnish", 1)
	OK
DO
    long x = GET_INTEGER ("Column for X-axis");
	long y = GET_INTEGER ("Column for Y-axis");
    REQUIRE (x != 0 && y != 0, "X and Y component must differ from 0.")
    EVERY_DRAW (Matrix_scatterPlot (OBJECT, GRAPHICS, x, y,
    	GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
    	GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
    	GET_REAL ("Mark size"), GET_STRINGW (L"Mark string"),
		GET_INTEGER("Garnish")))
END

DIRECT (Matrix_to_Activation)
	EVERY_TO (Matrix_to_Activation (OBJECT))
END

FORM (Matrices_to_DTW, "Matrices: To DTW", "Matrix: To DTW...")
	LABEL ("", "Distance  between cepstral coefficients")
	REAL ("Distance metric", "2.0")
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
	NEW (Matrices_to_DTW (m1, m2, begin, end, slope, GET_REAL ("Distance metric")))

END

FORM (Matrix_to_Pattern, "Matrix: To Pattern", 0)
	NATURAL ("Join", "1")
	OK
DO
	EVERY_TO (Matrix_to_Pattern (OBJECT, GET_INTEGER ("Join")))
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

FORM (Matrixft_getFrequencyOfRow, "Get frequency of row", 0)
	NATURAL ("Row number", "1")
	OK
DO
	Melder_information1 (Melder_double (Matrix_rowToY (ONLY_OBJECT, GET_INTEGER ("Row number"))));
END

FORM (Matrixft_getXofColumn, "Get time of column", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Melder_information1 (Melder_double (Matrix_columnToX (ONLY_OBJECT, GET_INTEGER ("Column number"))));
END

FORM (Matrixft_getValueInCell, "Get value in cell", 0)
	POSITIVE ("Time (s)", "0.5")
	POSITIVE ("Frequency", "1")
	OK
DO
	Matrix me = ONLY_OBJECT;
	long row, col;
	double ta, t = GET_REAL ("Time");
	double fa, f = GET_REAL ("Frequency");
	REQUIRE (f>= my xmin && f <= my ymax, "Frequency out of range.")
	REQUIRE (t>= my xmin && t <= my xmax, "Time out of range.")
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

FORM (MelFilter_drawFilterFunctions, "MelFilter: Draw filter functions",
	"FilterBank: Draw filter functions...")
	INTEGER ("left Filter range", "0")
	INTEGER ("right Filter range", "0")
	RADIO ("Frequency scale", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIOBUTTON ("Mel")
	REAL ("left Frequency range", "0.0")
	REAL ("right Frequency range", "0.0")
	BOOLEAN ("Amplitude scale in dB", 0)
	REAL ("left Amplitude range", "0.0")
	REAL ("right Amplitude range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (MelFilter_drawFilterFunctions (OBJECT, GRAPHICS,
		GET_INTEGER ("Frequency scale"),
		GET_INTEGER ("left Filter range"), GET_INTEGER ("right Filter range"),
		GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_INTEGER ("Amplitude scale in dB"),
		GET_REAL ("left Amplitude range"), GET_REAL ("right Amplitude range"),
		GET_INTEGER ("Garnish")))
END

FORM (MelFilter_drawSpectrum, "MelFilter: Draw spectrum (slice)", "FilterBank: Draw spectrum (slice)...")
	POSITIVE ("Time (s)", "0.1")
	REAL ("left Frequency range (mel)", "0.0")
	REAL ("right Frequency range (mel)", "0.0")
	REAL ("left Amplitude range (dB)", "0.0")
	REAL ("right Amplitude range (dB)", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO	
	EVERY_DRAW (FilterBank_drawTimeSlice (OBJECT, GRAPHICS, 
		GET_REAL ("Time"), GET_REAL ("left Frequency range"),
		GET_REAL ("right Frequency range"), GET_REAL ("left Amplitude range"),
		GET_REAL ("right Amplitude range"), L"Mels", GET_INTEGER ("Garnish")))
END

FORM (MelFilter_to_MFCC, "MelFilter: To MFCC",
	"MelFilter: To MFCC...")
	NATURAL ("Number of coefficients", "12")
	OK
DO
	EVERY_TO (MelFilter_to_MFCC (OBJECT,
		GET_INTEGER ("Number of coefficients")))
END

/**************** MFCC *******************************************/

DIRECT (MFCC_help)
	Melder_help (L"MFCC");
END

FORM (MFCC_to_MelFilter, "MFCC: To MelFilter", "MFCC: To MelFilter...")
	INTEGER ("From coefficient", "0")
	INTEGER ("To coefficient", "0")
	POSITIVE ("Position of first filter (mel)", "100.0")
	POSITIVE ("Distance between filters (mel)", "100.0")
	OK
DO
	EVERY_TO (MFCC_to_MelFilter (OBJECT, GET_INTEGER ("From coefficient"),
		GET_INTEGER ("To coefficient"), GET_REAL ("Position of first filter"),
		GET_REAL ("Distance between filters")))
END

/**************** MSpline *******************************************/

FORM (MSpline_create, "Create MSpline", "Create MSpline...")
	WORD ("Name", "mspline")
	LABEL ("", "Domain")
	REAL ("Xmin", "0")
	REAL ("Xmax", "1")
	LABEL ("", "MSpline(x) = c[1] M[1](x) + c[2] M[1](x) + ... c[n] M[n](x)")
	LABEL ("", "all M[k] are polynomials of degree \"Degree\"")
	LABEL ("", "Relation: numberOfCoefficients == numberOfInteriorKnots + "
		"degree + 1")
	INTEGER ("Degree", "2")
	SENTENCE ("Coefficients (c[k])", "1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE ("Interior knots" , "0.3 0.5 0.6")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	long degree = GET_INTEGER ("Degree");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	if (! praat_new1 (MSpline_createFromStrings (xmin, xmax, degree,
		GET_STRINGW (L"Coefficients"), GET_STRINGW (L"Interior knots")),
		GET_STRINGW (L"Name"))) return 0;
END

DIRECT (MSpline_help) Melder_help (L"MSpline"); END

/********************** Pattern *******************************************/

DIRECT (Pattern_and_Categories_to_Discriminant)
	Pattern p = ONLY (classPattern);
	Categories c = ONLY (classCategories);
	if (! praat_new3 (Pattern_and_Categories_to_Discriminant (p, c), 
		Thing_getNameW (p), L"_", Thing_getNameW (c))) return 0;
END

FORM (Pattern_draw, "Pattern: Draw", 0)
	NATURAL ("Pattern number", "1")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Pattern_draw (OBJECT, GRAPHICS, GET_INTEGER ("Pattern number"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"), GET_INTEGER ("Garnish")))
END

FORM (Pattern_formula,"Pattern: Formula", 0)
	LABEL ("label", "        y := 1; for row := 1 to nrow do { x := 1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + 1 } "
		"y := y + 1 }}")
	TEXTFIELD ("formula", "self")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORM (Pattern_setValue, "Pattern: Set value", "Pattern: Set value...")
	NATURAL ("Row number", "1")
	NATURAL ("Column number", "1")
	REAL ("New value", "0.0")
	OK
DO
	WHERE (SELECTED) {
		Pattern me = OBJECT;
		long row = GET_INTEGER ("Row number"), column = GET_INTEGER ("Column number");
		REQUIRE (row <= my ny, "Row number must not be greater than number of rows.")
		REQUIRE (column <= my nx, "Column number must not be greater than number of columns.")
		my z [row] [column] = GET_REAL ("New value");
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

FORM (PCA_and_TableOfReal_getFractionVariance,"PCA & TableOfReal: Get fraction variance",
	"PCA & TableOfReal: Get fraction variance...")
	NATURAL ("left Principal component range", "1")
	NATURAL ("right Principal component range", "1")
	OK
DO
	Melder_information1 (Melder_double (PCA_and_TableOfReal_getFractionVariance 
		(ONLY (classPCA), ONLY_GENERIC (classTableOfReal),
		GET_INTEGER ("left Principal component range"), 
		GET_INTEGER ("right Principal component range"))));	
END

DIRECT (PCA_and_Configuration_to_TableOfReal_reconstruct)
	NEW (PCA_and_Configuration_to_TableOfReal_reconstruct (ONLY (classPCA), 
		ONLY (classConfiguration)))
END

FORM (PCA_and_TableOfReal_to_Configuration,
	"PCA & TableOfReal: To Configuration",
	"PCA & TableOfReal: To Configuration...")
	INTEGER ("Number of dimensions", "0 (=all)")
	OK
DO
	long dimension = GET_INTEGER ("Number of dimensions");
	REQUIRE (dimension >= 0, "Number of dimensions must be greater equal zero.")
	NEW (PCA_and_TableOfReal_to_Configuration (ONLY (classPCA),
		ONLY_GENERIC (classTableOfReal), dimension))
END

FORM (PCA_getEqualityOfEigenvalues, "PCA: Get equality of eigenvalues",
	"PCA: Get equality of eigenvalues...")
	INTEGER ("left Eigenvalue range", "0")
	INTEGER ("right Eigenvalue range", "0")
	BOOLEAN ("Conservative test", 0)
	OK
DO
	long ndf; double p, chisq;
	PCA_getEqualityOfEigenvalues (ONLY_OBJECT, GET_INTEGER ("left Eigenvalue range"),
		GET_INTEGER ("right Eigenvalue range"), GET_INTEGER ("Conservative test"),
		&p, &chisq, &ndf);
	Melder_information5 (Melder_double (p), L" (=probability, based on chisq = ",
		Melder_double (chisq), L"and ndf = ", Melder_integer (ndf));
END

FORM (PCA_getNumberOfComponentsVAF, "PCA: Get number of components (VAF)",
	"PCA: Get number of components (VAF)...")
	POSITIVE ("Variance fraction (0-1)", "0.95")
	OK
DO
	double f = GET_REAL ("Variance fraction");
	REQUIRE (f > 0 && f <= 1, "The variance fraction must be in interval (0-1).")
	Melder_information1 (Melder_integer (Eigen_getDimensionOfFraction (ONLY_OBJECT, f)));
END

FORM (PCA_getFractionVAF, "PCA: Get fraction variance accounted for",
	"PCA: Get fraction variance accounted for...")
	NATURAL ("left Principal component range", "1")
	NATURAL ("right Principal component range", "1")
	OK
DO
	long from = GET_INTEGER ("left Principal component range");
	long to = GET_INTEGER ("right Principal component range");
	REQUIRE (from <= to, "The second component must be greater than or equal "
		"to the first component.")
	Melder_information1 (Melder_double (Eigen_getCumulativeContributionOfComponents
		(ONLY_OBJECT, from, to)));
END

FORM (PCA_invertEigenvector, "PCA: Invert eigenvector", 0)
	NATURAL ("Eigenvector number", "1")
	OK
DO
	EVERY (Eigen_invertEigenvector (OBJECT, GET_INTEGER ("Eigenvector number")))
END

FORM (PCA_to_TableOfReal_reconstruct1, "PCA: To TableOfReal (reconstruct)", 
	"PCA: To TableOfReal (reconstruct 1)...")
	SENTENCE ("Coefficients", "1.0 1.0")
	OK
DO
	EVERY_TO (PCA_to_TableOfReal_reconstruct1 (OBJECT, GET_STRINGW (L"Coefficients")))
END

FORM (PCAs_to_Procrustes, "PCA & PCA: To Procrustes", "PCA & PCA: To Procrustes...")
	NATURAL ("left Eigenvector range", "1")
	NATURAL ("right Eigenvector range", "2")
	OK
DO
	long from = GET_INTEGER ("left Eigenvector range");
	long to = GET_INTEGER ("right Eigenvector range");
	PCA me = NULL, thee = NULL;
	WHERE (SELECTED) { if (me) thee = OBJECT; else me = OBJECT; }
	if (! praat_new3 (Eigens_to_Procrustes (me, thee, from, to), Thing_getNameW (me), L"_", 
		Thing_getNameW (thee))) return 0;
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

FORM (Permutation_create, "Create Permutation", "Create Permutation...")
	WORD ("Name", "p")
	NATURAL ("Number of elements", "10")
	BOOLEAN ("Identity Permutation", 1)
	OK
DO
	Permutation p = Permutation_create (GET_INTEGER ("Number of elements"));
	int identity = GET_INTEGER ("Identity Permutation");
	if (! identity && ! Permutation_permuteRandomly_inline (p, 0, 0))
	{
		forget (p); return 0;
	}
	if (! praat_new1 (p, GET_STRINGW (L"Name"))) return 0;
END

DIRECT (Permutation_getNumberOfElements)
	Permutation p = ONLY_OBJECT;
	Melder_information1 (Melder_integer (p -> numberOfElements));
END

FORM (Permutation_getValueAtIndex, "Permutation: Get value", "Permutation: Get value...")
	NATURAL ("Index", "1")
	OK
DO
	long index = GET_INTEGER ("Index");
	Melder_information4 (Melder_integer (Permutation_getValueAtIndex (ONLY_OBJECT, index)), L" (value, at index = ", 
		Melder_integer (index), L")");
END

FORM (Permutation_getIndexAtValue, "Permutation: Get index", "Permutation: Get index...")
	NATURAL ("Value", "1")
	OK
DO
	long value = GET_INTEGER ("Value");
	Melder_information4 (Melder_integer (Permutation_getIndexAtValue (ONLY_OBJECT, value)), L" (index, at value = ",
		Melder_integer (value), L")");
END

DIRECT (Permutation_sort)
	Permutation_sort (ONLY_OBJECT);
	praat_dataChanged (ONLY_OBJECT);
END

FORM (Permutation_swapBlocks, "Permutation: Swap blocks", "Permutation: Swap blocks...")
	NATURAL ("From", "1")
	NATURAL ("To", "2")
	NATURAL ("Block size", "1")
	OK
DO
	if (! Permutation_swapBlocks (ONLY_OBJECT, GET_INTEGER ("From"), GET_INTEGER ("To"), GET_INTEGER ("Block size"))) return 0;
	praat_dataChanged (ONLY_OBJECT);
END


FORM (Permutation_swapOneFromRange, "Permutation: Swap one from range", "Permutation: Swap one from range...")
	LABEL ("", "A randomly chosen element from ")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	LABEL ("", "is swapped with the element at")
	NATURAL ("Index", "1")
	BOOLEAN ("Forbid same", 1)
	OK
DO
	if (! Permutation_swapOneFromRange (ONLY_OBJECT, GET_INTEGER ("left Range"), GET_INTEGER ("right Range"),
		GET_INTEGER ("Index"), GET_INTEGER ("Forbid same"))) return 0;
	praat_dataChanged (ONLY_OBJECT);
END

FORM (Permutation_permuteRandomly, "Permutation: Permute randomly", "Permutation: Permute randomly...")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	OK
DO
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_permuteRandomly (p, GET_INTEGER ("left Range"), 
		GET_INTEGER ("right Range")), Thing_getNameW (p), L"_randomly")) return 0;
END

FORM (Permutation_rotate, "Permutation: Rotate", "Permutation: Rotate...")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	INTEGER ("Step size", "1")
	OK
DO
	Permutation p = ONLY_OBJECT;
	long step = GET_INTEGER ("Step size");
	if (! praat_new3 (Permutation_rotate (p, GET_INTEGER ("left Range"), GET_INTEGER ("right Range"), step), Thing_getNameW (p), L"_rotate", Melder_integer (step))) return 0;
END

FORM (Permutation_reverse, "Permutation: Reverse", "Permutation: Reverse...")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	OK
DO
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_reverse (p, GET_INTEGER ("left Range"), GET_INTEGER ("right Range")),
		Thing_getNameW (p), L"_reverse")) return 0;
END

FORM (Permutation_permuteBlocksRandomly, "Permutation: Permute blocks randomly", "Permutation: Permute randomly (blocks)...")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	NATURAL ("Block size", "12")
	BOOLEAN ("Permute within blocks", 1)
	BOOLEAN ("No doublets", 0)
	OK
DO
	Permutation p = ONLY_OBJECT;
	long blocksize = GET_INTEGER ("Block size");
	if (! praat_new3 (Permutation_permuteBlocksRandomly (p, GET_INTEGER ("left Range"), 
		GET_INTEGER ("right Range"), blocksize, GET_INTEGER ("Permute within blocks"), GET_INTEGER ("No doublets")),
		Thing_getNameW (p), L"_blocks", Melder_integer(blocksize))) return 0;
END

FORM (Permutation_interleave, "Permutation: Interleave", "Permutation: Interleave...")
	INTEGER ("left Range", "0")
	INTEGER ("right Range", "0")
	NATURAL ("Block size", "12")
	INTEGER ("Offset", "0")
	OK
DO
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_interleave (ONLY_OBJECT, GET_INTEGER ("left Range"), GET_INTEGER ("right Range"),
		GET_INTEGER ("Block size"), GET_INTEGER ("Offset")), Thing_getNameW (p), L"_interleave")) return 0;
END

DIRECT (Permutation_invert)
	Permutation p = ONLY_OBJECT;
	if (! praat_new2 (Permutation_invert (p), Thing_getNameW (p), L"_inverse")) return 0;
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
			return Melder_error ("To apply a number of permutations they all must have the same number of elements.");	
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

FORM (Pitches_to_DTW, "Pitches: To DTW", "Pitches: To DTW...")
	REAL ("Voiced-unvoiced costs", "24.0")
	REAL ("Time costs weight", "10.0")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	Pitch p1 = NULL, p2 = NULL;
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
	WHERE (SELECTED) { if (p1) p2 = OBJECT; else p1 = OBJECT; }
	if (! praat_new4 (Pitches_to_DTW (p1, p2, GET_REAL ("Voiced-unvoiced costs"), GET_REAL ("Time costs weight"), begin, end, slope), L"dtw_", p1 -> nameW, L"_", p2 -> nameW)) return 0;

END

FORM (PitchTier_to_Pitch, "PitchTier: To Pitch", "PitchTier: To Pitch...")
	POSITIVE ("Step size", "0.02")
	POSITIVE ("Pitch floor", "60.0")	
	POSITIVE ("Pitch ceiling", "400.0")
	OK
DO
	EVERY_TO (PitchTier_to_Pitch (ONLY(classPitchTier), GET_REAL ("Step size"), 
		GET_REAL ("Pitch floor"),GET_REAL ("Pitch ceiling")))
END

/******************* Polygon & Categories *************************************/

FORM (Polygon_translate, "Polygon: Translate", 0)
	REAL("X", "0.0")
	REAL("Y", "0.0")
	OK
DO
	Polygon_translate (ONLY(classPolygon), GET_REAL("X"), GET_REAL("Y"));
END

FORM (Polygon_rotate, "Polygon: Rotate", 0)
	REAL("Angle (degrees)", "0.0")
	REAL("X", "0.0")
	REAL("Y", "0.0")
	OK
DO
	Polygon_rotate (ONLY(classPolygon), GET_REAL("Angle"), GET_REAL("X"), GET_REAL("Y"));
END

FORM (Polygon_scale, "Polygon: Scale polygon", 0)
	REAL("X", "0.0")
	REAL("Y", "0.0")
	OK
DO
	Polygon_scale (ONLY(classPolygon), GET_REAL("X"), GET_REAL("Y"));
END

FORM (Polygon_Categories_draw, "Polygon & Categories: Draw", 0) 
    REAL("left Horizontal range", "0.0")
    REAL("right Horizontal range", "0.0")
    REAL("left Vertical range", "0.0")
    REAL("right Vertical range", "0.0")
    BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Polygon_Categories_draw (ONLY(classPolygon),
		ONLY(classCategories),
		GRAPHICS, GET_REAL("left Horizontal range"), GET_REAL("right Horizontal range"),
		GET_REAL("left Vertical range"), GET_REAL("right Vertical range"),
		GET_INTEGER("Garnish")))
END

DIRECT (Polygon_reverseX)
	Polygon_reverseX (ONLY(classPolygon));
END

DIRECT (Polygon_reverseY)
	Polygon_reverseY (ONLY(classPolygon));
END

/***************** Polynomial *******************/

DIRECT (Polynomial_help) Melder_help (L"Polynomial"); END

FORM (Polynomial_create, "Create Polynomial", "Create Polynomial...")
	WORD ("Name", "p")
	LABEL ("", "Domain of polynomial")
	REAL ("Xmin", "-3")
	REAL ("Xmax", "4")
	LABEL ("", "p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	SENTENCE ("Coefficients", "2.0 -1.0 -2.0 1.0")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	if (! praat_new1 (Polynomial_createFromString (xmin, xmax,
		GET_STRINGW (L"Coefficients")), GET_STRINGW (L"Name"))) return 0;
END

FORM (Polynomial_getArea, "Polynomial: Get area", "Polynomial: Get area...")
	LABEL ("", "Interval")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	OK
DO
	double area = Polynomial_getArea (ONLY (classPolynomial),
		GET_REAL ("Xmin"), GET_REAL ("Xmax"));
	Melder_information1 (Melder_double (area)); 
END

DIRECT (Polynomial_getDerivative)
	EVERY_TO (Polynomial_getDerivative (OBJECT))
END

DIRECT (Polynomial_getPrimitive)
	EVERY_TO (Polynomial_getPrimitive (OBJECT))
END

FORM (Polynomial_scaleX, "Polynomial: Scale x", "Polynomial: Scale x...")
	LABEL ("", "New domain")
	REAL ("Xmin", "-1.0") 
	REAL ("Xmax", "1.0")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	EVERY_TO (Polynomial_scaleX (OBJECT, xmin, xmax))
END

DIRECT (Polynomial_scaleCoefficients_monic)
	EVERY (Polynomial_scaleCoefficients_monic (OBJECT))
END

DIRECT (Polynomial_to_Roots)
	EVERY_TO (Polynomial_to_Roots (OBJECT))
END

FORM (Polynomial_evaluate_z, "Polynomial: Get value (complex)",
	"Polynomial: Get value (complex)...")
	REAL ("Real part", "0.0")
	REAL ("Imaginary part", "0.0")
	OK
DO
	dcomplex p, z = dcomplex_create (GET_REAL ("Real part"), GET_REAL ("Imaginary part"));
	Polynomial_evaluate_z (ONLY_OBJECT, &z, &p);
	Melder_information4 (Melder_double (p.re), L" + ", Melder_double (p.im), L" i");
END


FORM (Polynomial_to_Spectrum, "Polynomial: To Spectrum",
	"Polynomial: To Spectrum...")
	POSITIVE ("Nyquist frequency (Hz)", "5000.0")
	NATURAL ("Number of frequencies (>1)", "1025")
	OK
DO
	long n = GET_INTEGER ("Number of frequencies");
	REQUIRE (n > 1, "\"Number of frequencies\" must be greater than 2.")
	EVERY_TO (Polynomial_to_Spectrum (OBJECT, GET_REAL ("Nyquist frequency"),
		n, 1.0))
END

DIRECT (Polynomials_multiply)
	Polynomial p1 = NULL, p2 = NULL;
	WHERE (SELECTED) { if (p1) p2 = OBJECT; else p1 = OBJECT; }
	if (! praat_new4 (Polynomials_multiply (p1, p2), p1->nameW, L"_", p2->nameW, L"_mul")) return 0;
END

FORM (Polynomials_divide, "Polynomials: Divide", "Polynomials: Divide...")
	BOOLEAN ("Want quotient", 1)
	BOOLEAN ("Want remainder", 1)
	OK
DO
	/* With gcc (GCC) 3.2.2 20030217 (Red Hat Linux 8.0 3.2.2-2)
		The following line initiates pq = NULL and I don't know why
	Polynomial p1 = NULL, p2 = NULL, pq, pr;
	*/
	Polynomial p1 = NULL, p2 = NULL, s, r, q;
	int wantq = GET_INTEGER ("Want quotient");
	int wantr = GET_INTEGER ("Want remainder");
	REQUIRE (wantq || wantr, "Either \'Want quotient\' or \'Want remainder\' must be chosen")
	WHERE (SELECTED) { if (p1) p2 = OBJECT; else p1 = OBJECT; }
	if (! wantq) q = NULL;
	if (! wantr) r = NULL;
	s = Polynomial_create (0,1,1);
	forget (s);
	if (! Polynomials_divide (p1, p2, &q, &r)) return 0;
	if (wantq && ! praat_new2 (q, p1 -> nameW, L"_q")) return 0;
	if (wantr && ! praat_new2 (r, p1 -> nameW, L"_r")) return 0;
END

/********************* Roots ******************************/

DIRECT (Roots_help) Melder_help (L"Roots"); END

FORM (Roots_draw, "Roots: Draw", 0)
	REAL ("Minimum of real axis", "0.0")
	REAL ("Maximum of real axis", "0.0")
	REAL ("Minimum of imaginary axis", "0.0")
	REAL ("Maximum of imaginary axis", "0.0")
	SENTENCE ("Mark string (+x0...)", "o")
	NATURAL ("Mark size", "12")
	BOOLEAN ("Garnish", 0)
	OK
DO
	EVERY_DRAW (Roots_draw (OBJECT, GRAPHICS,
		GET_REAL ("Minimum of real axis"), GET_REAL ("Maximum of real axis"), 
		GET_REAL ("Minimum of imaginary axis"),
		GET_REAL ("Maximum of imaginary axis"), 
		GET_STRINGW (L"Mark string"), GET_INTEGER ("Mark size"),
		GET_INTEGER ("Garnish")))
END

DIRECT (Roots_getNumberOfRoots)
	Melder_information1 (Melder_integer (Roots_getNumberOfRoots (ONLY (classRoots))));
END

FORM (Roots_getRoot, "Roots: Get root", 0)
	NATURAL ("Root number", "1")
	OK
DO
	dcomplex z = Roots_getRoot (ONLY (classRoots), GET_INTEGER ("Root number"));
	Melder_information4 (Melder_double (z.re), (z.im < 0 ? L" - " : L" + "), Melder_double (fabs(z.im)), L" i");
END

FORM (Roots_getRealPartOfRoot, "Roots: Get real part", 0)
	NATURAL ("Root number", "1")
	OK
DO
	dcomplex z = Roots_getRoot (ONLY (classRoots), GET_INTEGER ("Root number"));
	Melder_information1 (Melder_double (z.re));
END

FORM (Roots_getImaginaryPartOfRoot, "Roots: Get imaginary part", 0)
	NATURAL ("Root number", "1")
	OK
DO
	dcomplex z = Roots_getRoot (ONLY (classRoots), GET_INTEGER ("Root number"));
	Melder_information1 (Melder_double (z.im));
END

FORM (Roots_setRoot, "Roots: Set root", 0)
	NATURAL ("Root number", "1")
	REAL ("Real part", "1.0/sqrt(2)")
	REAL ("Imaginary part", "1.0/sqrt(2)")
	OK
DO
	if (! Roots_setRoot (ONLY_OBJECT, GET_INTEGER ("Root number"),
		GET_REAL ("Real part"), GET_REAL ("Imaginary part"))) return 0;
END

FORM (Roots_to_Spectrum, "Roots: To Spectrum", "Roots: To Spectrum...")
	POSITIVE ("Nyquist frequency (Hz)", "5000.0")
	NATURAL ("Number of frequencies (>1)", "1025")
	OK
DO
	long n = GET_INTEGER ("Number of frequencies");
	REQUIRE (n > 1, "\"Number of frequencies\" must be greater than 2.")
	EVERY_TO (Roots_to_Spectrum (OBJECT, GET_REAL ("Nyquist frequency"),
		n, 1.0))
END

DIRECT (Roots_and_Polynomial_polish)
	 Roots_and_Polynomial_polish (ONLY(classRoots), ONLY(classPolynomial));
END

/*****************************************************************************/

DIRECT (Praat_ReportFloatingPointProperties)
	if (! NUMfpp) NUMmachar ();
	Melder_information1 (L"Double precision floating point properties of this machine,");
	Melder_information1 (L"as calculated by algorithms from the Binary Linear Algebra System (BLAS)");
	Melder_information2 (L"Radix: ", Melder_double (NUMfpp -> base));
	Melder_information2 (L"Number of digits in mantissa: ", Melder_double (NUMfpp -> t));
	Melder_information2 (L"Smallest exponent before (gradual) underflow (expmin): ", Melder_integer (NUMfpp -> emin));
	Melder_information2 (L"Largest exponent before overflow (expmax): ", Melder_integer (NUMfpp -> emax));
	Melder_information2 (L"Does rounding occur in addition: ", (NUMfpp -> rnd == 1 ? L"yes" : L"no"));
	Melder_information2 (L"Quantization step (d): ", Melder_double (NUMfpp -> prec));
	Melder_information2 (L"Quantization error (eps = d/2): ", Melder_double (NUMfpp -> eps));
	Melder_information2 (L"Underflow threshold (= radix ^ (expmin - 1)): ", Melder_double (NUMfpp -> rmin));
	Melder_information2 (L"Safe minimum (such that its inverse does not overflow): ", Melder_double (NUMfpp -> sfmin));
	Melder_information2 (L"Overflow threshold (= (1 - eps) * radix ^ expmax): ", Melder_double (NUMfpp -> rmax));
END

/******************** Sound ****************************************/

static void Sound_create_addCommonFields (void *dia)
{
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "0.1")
	POSITIVE ("Sampling frequency (Hz)", "22050.0")
}

static int Sound_create_checkCommonFields (void *dia, double *startingTime, double *finishingTime,
	double *samplingFrequency)
{
	double numberOfSamples_real;
	*startingTime = GET_REAL ("Starting time");
	*finishingTime = GET_REAL ("Finishing time");
	*samplingFrequency = GET_REAL ("Sampling frequency");
	numberOfSamples_real = floor ((*finishingTime - *startingTime) * *samplingFrequency + 0.5);
	if (*finishingTime <= *startingTime)
	{
		if (*finishingTime == *startingTime)
			(void) Melder_error ("A Sound cannot have a duration of zero.\n");
		else
			(void) Melder_error ("A Sound cannot have a duration less than zero.\n");
		if (*startingTime == 0.0)
			return Melder_error ("Please set the finishing time to something greater than 0 seconds.");
		else
			return Melder_error ("Please lower the starting time or raise the finishing time.");
	}
	if (*samplingFrequency <= 0.0)
	{
		(void) Melder_error ("A Sound cannot have a negative sampling frequency.\n");
		return Melder_error ("Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");
	}
	if (numberOfSamples_real < 1.0)
	{
		(void) Melder_error ("A Sound cannot have zero samples.\n");
		if (*startingTime == 0.0)
			return Melder_error ("Please raise the finishing time.");
		else
			return Melder_error ("Please lower the starting time or raise the finishing time.");
	}
	if (numberOfSamples_real > LONG_MAX)
	{
		(void) Melder_error ("A Sound cannot have %ls samples; the maximum is %ls samples.\n", Melder_bigInteger (numberOfSamples_real), Melder_bigInteger (LONG_MAX));
		if (*startingTime == 0.0)
			return Melder_error ("Please lower the finishing time or the sampling frequency.");
		else
			return Melder_error ("Please raise the starting time, lower the finishing time, or lower the sampling frequency.");
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
		(void) Melder_error ("There is not enough memory to create a Sound that contains %ls samples.\n", Melder_bigInteger (numberOfSamples_real));
		if (startingTime == 0.0)
			return Melder_error ("You could lower the finishing time or the sampling frequency and try again.");
		else
			return Melder_error ("You could raise the starting time or lower the finishing time or the sampling frequency, and try again.");
	}
	
	return 0;
}

FORM (Sound_and_Pitch_to_FormantFilter, "Sound & Pitch: To FormantFilter",
	"Sound & Pitch: To FormantFilter...")
	POSITIVE ("Analysis window duration (s)", "0.015")
	POSITIVE ("Time step (s)", "0.005")
	LABEL("","Filter bank parameters")
	POSITIVE ("Position of first filter (Hz)", "100.0")
	POSITIVE ("Distance between filters (Hz)", "50.0")
	REAL ("Maximum frequency", "0");
	POSITIVE ("Relative bandwidth", "1.1")
	OK
DO
	 if (! praat_new (Sound_and_Pitch_to_FormantFilter (ONLY(classSound),
	 	ONLY(classPitch), GET_REAL ("Analysis window duration"),
		GET_REAL ("Time step"), GET_REAL ("Position of first filter"), 
		GET_REAL ("Maximum frequency"), GET_REAL ("Distance between filters"),
		GET_REAL ("Relative bandwidth")), NULL)) return 0;
END

FORM (Sound_and_Pitch_changeGender, "Sound & Pitch: Change gender", "Sound & Pitch: Change gender...")
	POSITIVE ("Formant shift ratio", "1.2")
	REAL ("New pitch median (Hz)", "0.0 (=no change)")
	POSITIVE ("Pitch range factor", "1.0 (=no change)")
	POSITIVE ("Duration factor", "1.0")
	OK
DO
	if (! praat_new (Sound_and_Pitch_changeGender_old (ONLY(classSound), ONLY(classPitch),
		GET_REAL ("Formant shift ratio"), GET_REAL ("New pitch median"), 
		GET_REAL ("Pitch range factor"), GET_REAL ("Duration factor")), NULL)) return 0;
END

FORM (Sound_and_Pitch_changeSpeaker, "Sound & Pitch: Change speaker", "Sound & Pitch: Change speaker...")
	POSITIVE ("Multiply formants by", "1.1 (male->female)")
	POSITIVE ("Multiply pitch by", "1.8 (male->female")
	REAL ("Multiply pitch range by", "1.0 (=no change)")
	POSITIVE ("Multiply duration", "1.0")
	OK
DO
	if (! praat_new (Sound_and_Pitch_changeSpeaker (ONLY(classSound), ONLY(classPitch),
		GET_REAL ("Multiply formants by"), GET_REAL ("Multiply pitch by"), 
		GET_REAL ("Multiply pitch range by"), GET_REAL ("Multiply duration")), NULL)) return 0;
END

FORM (Sound_createFromGammaTone, "Create a gamma-tone",
	"Create Sound from gamma-tone...")
	WORD ("Name", "gammatone")
	Sound_create_addCommonFields (dia);
	INTEGER ("Gamma", "4")
	POSITIVE ("Frequency (Hz)", "1000.0")
	REAL ("Bandwidth (Hz)", "150.0")
	REAL ("Initial phase (radians)", "0.0")
	REAL ("Addition factor", "0.0")
	BOOLEAN ("Scale amplitudes", 1)
	OK
DO
	Sound sound = NULL;
	double startingTime, finishingTime, samplingFrequency;
	long gamma = GET_INTEGER ("Gamma");
	double bandwidth = GET_REAL ("Bandwidth");
	double f = GET_REAL ("Frequency");

	if (! Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency))
		return 0;
	if (f >= samplingFrequency / 2)
	{
		Melder_error ( "Frequency cannot be larger than half the sampling frequency.\n");
		return Melder_error ( "Please use a frequency smaller than %ls.", 
			Melder_double (samplingFrequency / 2));
	}
	if (gamma < 0)
	{
		(void) Melder_error ("Gamma can not be negative.\n");
		return Melder_error ("Please use a positive or zero gamma.");
	}
	if (bandwidth < 0)
	{
		(void) Melder_error ("Bandwidth can not be negative.\n");
		return Melder_error ("Please use a positive or zero bandwidth.");
	}
	sound = Sound_createGammaTone (startingTime, finishingTime, samplingFrequency, gamma, f,
		bandwidth, GET_REAL ("Initial phase"), GET_REAL ("Addition factor"),
		GET_INTEGER ("Scale amplitudes"));
	if (! Sound_create_check (sound, startingTime, finishingTime, samplingFrequency) || 
		! praat_new1 (sound, GET_STRINGW (L"Name"))) return 0;
END

FORM (Sound_createFromShepardTone, "Create a Shepard tone",
	"Create Sound from Shepard tone...")
	WORD ("Name", "shepardTone")
	Sound_create_addCommonFields (dia);
	POSITIVE ("Lowest frequency (Hz)", "4.863")
	NATURAL ("Number of components", "10")
	REAL ("Frequency change (semitones/s)", "4.0")
	REAL ("Amplitude range (dB)", "30.0")
	REAL ("Octave shift fraction ([0,1))", "0.0")
	OK
DO
	Sound sound = NULL;
	double startingTime, finishingTime, samplingFrequency;
	double amplitudeRange = GET_REAL ("Amplitude range");
	double octaveShiftFraction = GET_REAL ("Octave shift fraction");
	if (! Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency)) return 0;
	if (amplitudeRange < 0)
	{
		(void) Melder_error ("Amplitude range can not be negative.\n");
		return Melder_error ("Please use a positive or zero amplitude range.");
	}
	sound = Sound_createShepardToneComplex (startingTime, finishingTime, samplingFrequency,
		GET_REAL ("Lowest frequency"), GET_INTEGER("Number of components"),
		GET_REAL ("Frequency change"), GET_REAL ("Amplitude range"), octaveShiftFraction);
	
	if (! Sound_create_check (sound, startingTime, finishingTime, samplingFrequency) ||
		! praat_new1 (sound, GET_STRINGW (L"Name"))) return 0; 
END
/* old
FORM (Sound_createFromShepardTone, "Create a Shepard tone",
	"Create Sound from Shepard tone...")
	WORD ("Name", "shepardTone")
	Sound_create_addCommonFields (dia);
	POSITIVE ("Lowest frequency (Hz)", "4.863")
	NATURAL ("Number of components", "10")
	REAL ("Frequency change (semitones/s)", "4.0")
	REAL ("Amplitude range (dB)", "30.0")
	OK
DO
	Sound sound = NULL;
	double startingTime, finishingTime, samplingFrequency;
	double amplitudeRange = GET_REAL ("Amplitude range");
	
	if (! Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency)) return 0;
	if (amplitudeRange < 0)
	{
		(void) Melder_error ("Amplitude range can not be negative.\n");
		return Melder_error ("Please use a positive or zero amplitude range.");
	}
	sound = Sound_createShepardTone (startingTime, finishingTime, samplingFrequency,
		GET_REAL ("Lowest frequency"), GET_INTEGER("Number of components"),
		GET_REAL ("Frequency change"), GET_REAL ("Amplitude range"));
	
	if (! Sound_create_check (sound, startingTime, finishingTime, samplingFrequency) ||
		! praat_new (sound, GET_STRING ("Name"))) return 0; 
END
*/

FORM (Sound_to_TextGrid_detectSilences, "Sound: To TextGrid (silences)", 
	"Sound: To TextGrid (silences)...")
	LABEL ("", "Parameters for the intensity analysis")
	POSITIVE ("Minimum pitch (Hz)", "100")
	REAL ("Time step (s)", "0.0 (= auto)")
	LABEL ("","Silent intervals detection")
	REAL ("Silence threshold (dB)", "-25.0")
	POSITIVE ("Minimum silent interval duration (s)", "0.1")
	POSITIVE ("Minimum sounding interval duration (s)", "0.1")
	WORD ("Silent interval label", "silent")
	WORD ("Sounding interval label", "sounding")
	OK
DO
	EVERY_TO (Sound_to_TextGrid_detectSilences (OBJECT, GET_REAL ("Minimum pitch"), GET_REAL ("Time step"), 
		GET_REAL ("Silence threshold"), GET_REAL ("Minimum silent interval duration"),
		GET_REAL ("Minimum sounding interval duration"), GET_STRINGW (L"Silent interval label"), 
		GET_STRINGW (L"Sounding interval label")))	

END

FORM (Sound_to_BarkFilter, "Sound: To BarkFilter", 
	"Sound: To BarkFilter...")
	POSITIVE ("Analysis window duration (s)", "0.015")
	POSITIVE ("Time step (s)", "0.005")
	LABEL("","Filter bank parameters")
	POSITIVE ("Position of first filter (bark)", "1.0")
	POSITIVE ("Distance between filters (bark)", "1.0")
	REAL ("Maximum frequency (bark)", "0");
	OK
DO
	EVERY_TO (Sound_to_BarkFilter (OBJECT, GET_REAL ("Analysis window duration"),
		GET_REAL ("Time step"), GET_REAL ("Position of first filter"), 
		GET_REAL ("Maximum frequency"), GET_REAL ("Distance between filters")))
END

FORM (Sound_to_FormantFilter, "Sound: To FormantFilter", 
	"Sound: To FormantFilter...")
	POSITIVE ("Analysis window duration (s)", "0.015")
	POSITIVE ("Time step (s)", "0.005")
	LABEL("","Filter bank parameters")
	POSITIVE ("Position of first filter (Hz)", "100.0")
	POSITIVE ("Distance between filters (Hz)", "50.0")
	REAL ("Maximum frequency", "0");
	POSITIVE ("Relative bandwidth", "1.1")
	LABEL ("", "Pitch analysis")
	REAL ("Minimum pitch (Hz)", "75.0")
	REAL ("Maximum pitch (Hz)", "600.0")
	OK
DO
	EVERY_TO (Sound_to_FormantFilter (OBJECT, GET_REAL ("Analysis window duration"),
		GET_REAL ("Time step"), GET_REAL ("Position of first filter"), 
		GET_REAL ("Maximum frequency"), GET_REAL ("Distance between filters"),
		GET_REAL ("Relative bandwidth"), GET_REAL ("Minimum pitch"),
		GET_REAL ("Maximum pitch")))
END

FORM (Sound_to_MelFilter, "Sound: To MelFilter", 
	"Sound: To MelFilter...")
	POSITIVE ("Analysis window duration (s)", "0.015")
	POSITIVE ("Time step (s)", "0.005")
	LABEL("","Filter bank parameters")
	POSITIVE ("Position of first filter (mel)", "100.0")
	POSITIVE ("Distance between filters (mel)", "100.0")
	REAL ("Maximum frequency (mel)", "0.0");
	OK
DO
	EVERY_TO (Sound_to_MelFilter (OBJECT, GET_REAL ("Analysis window duration"),
		GET_REAL ("Time step"), GET_REAL ("Position of first filter"), 
		GET_REAL ("Maximum frequency"), GET_REAL ("Distance between filters")))
END

FORM (Sound_to_Pitch_shs, "Sound: To Pitch (shs)",
	"Sound: To Pitch (shs)...")
	POSITIVE ("Time step (s)", "0.01")
	POSITIVE ("Minimum pitch (Hz)", "50.0")
	NATURAL ("Max. number of candidates (Hz)", "15")
	LABEL ("", "Algorithm parameters")
	POSITIVE ("Maximum frequency component (Hz)", "1250.0")
	NATURAL ("Max. number of subharmonics", "15")
	POSITIVE ("Compression factor (<=1)", "0.84")
	POSITIVE ("Ceiling (Hz)", "600.0")
	NATURAL ("Number of points per octave", "48");
	OK
DO
	double minimumPitch = GET_REAL ("Minimum pitch");
	double fmax = GET_REAL ("Maximum frequency component");
	double ceiling = GET_REAL ("Ceiling");
	REQUIRE (minimumPitch < ceiling, "Minimum pitch should be smaller "
		"than ceiling.")
	REQUIRE (ceiling <= fmax, "Maximum frequency must be greaterequal ceiling.")
	EVERY_TO (Sound_to_Pitch_shs (OBJECT, GET_REAL ("Time step"),
		minimumPitch, fmax, ceiling,
		GET_INTEGER ("Max. number of subharmonics"),
		GET_INTEGER ("Max. number of candidates"),
		GET_REAL ("Compression factor"),
		GET_INTEGER ("Number of points per octave")))
END

FORM (Sound_to_Pitch_SPINET,
	"Sound: To SPINET", "Sound: To SPINET...")
	POSITIVE ("Time step (s)", "0.005")
	POSITIVE ("Analysis window duration (s)", "0.040")
	LABEL("","Gammatone filter bank")
	POSITIVE ("Minimum filter frequency (Hz)", "70.0")
	POSITIVE ("Maximum filter frequency (Hz)", "5000.0")
	NATURAL ("Number of filters", "250");
	POSITIVE ("Ceiling (Hz)", "500.0")
	NATURAL ("Max. number of candidates", "15")
	OK
DO
	double fmin = GET_REAL ("Minimum filter frequency");
	double fmax = GET_REAL ("Maximum filter frequency");
	REQUIRE (fmax > fmin, "Maximum frequency must be larger than "
		"minimum frequency.")
	EVERY_TO (Sound_to_Pitch_SPINET (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Analysis window duration"),
		fmin, fmax, GET_INTEGER ("Number of filters"),
		GET_REAL ("Ceiling"), GET_INTEGER ("Max. number of candidates")))
END

FORM (Sound_filterByGammaToneFilter4, "Sound: Filter (gammatone)", "Sound: Filter (gammatone)...")
	POSITIVE ("Centre frequency (Hz)","1000.0")
	POSITIVE ("Bandwidth (Hz)", "150.0")
	OK
DO
	EVERY_TO (Sound_filterByGammaToneFilter4 (OBJECT,
		GET_REAL ("Centre frequency"), GET_REAL ("Bandwidth")))
END

FORM (Sound_changeSpeaker, "Sound: Change speaker", "Sound: Change speaker...")
	LABEL ("", "Pitch measurement parameters")
	POSITIVE ("Pitch floor (Hz)", "75.0")
	POSITIVE ("Pitch ceiling (Hz)", "600.0")
	LABEL ("", "Modification parameters")
	POSITIVE ("Multiply formants by", "1.2")
	POSITIVE ("Multiply pitch by", "1.0")
	REAL ("Multiply pitch range by", "1.0 (=no change)")
	POSITIVE ("Multiply duration by", "1.0")
	OK
DO
	double minimumPitch = GET_REAL ("Pitch floor");
	double maximumPitch = GET_REAL ("Pitch ceiling");
	REQUIRE (minimumPitch < maximumPitch, "Maximum pitch should be greater than minimum pitch.")
	EVERY_TO (Sound_changeSpeaker (OBJECT, minimumPitch, maximumPitch,
		GET_REAL ("Multiply formants by"), GET_REAL ("Multiply pitch by"),
		GET_REAL ("Multiply pitch range by"), GET_REAL ("Multiply duration by")))
END

FORM (Sound_changeGender, "Sound: Change gender", "Sound: Change gender...")
	LABEL ("", "Pitch measurement parameters")
	POSITIVE ("Pitch floor (Hz)", "75.0")
	POSITIVE ("Pitch ceiling (Hz)", "600.0")
	LABEL ("", "Modification parameters")
	POSITIVE ("Formant shift ratio", "1.2")
	REAL ("New pitch median (Hz)", "0.0 (=no change)")
	REAL ("Pitch range factor", "1.0 (=no change)")
	POSITIVE ("Duration factor", "1.0")
	OK
DO
	double minimumPitch = GET_REAL ("Pitch floor");
	double maximumPitch = GET_REAL ("Pitch ceiling");
	double pitchrf = GET_REAL ("Pitch range factor");
	REQUIRE (minimumPitch < maximumPitch, "Maximum pitch should be greater than minimum pitch.")
	REQUIRE (pitchrf >= 0, "Pitch range factor may not be negative")
	EVERY_TO (Sound_changeGender_old (OBJECT, minimumPitch, maximumPitch,
		GET_REAL ("Formant shift ratio"), GET_REAL ("New pitch median"),
		pitchrf, GET_REAL ("Duration factor")))
END

FORM_READ (Sound_readFromRawFileLE, "Read Sound from raw Little Endian file", 0)
	if (! praat_new1 (Sound_readFromRawFile (file, NULL, 16, 1, 0, 0,
		16000), MelderFile_nameW (file))) return 0;
END

FORM_READ (Sound_readFromRawFileBE, "Read Sound from raw 16-bit Little Endian  file", 0)
	if (! praat_new1 (Sound_readFromRawFile (file, NULL, 16, 0, 0, 0,
		16000), MelderFile_nameW (file))) return 0;
END

FORM_WRITE (Sound_writeToRawFileBE, "Sound: Write to raw 16-bit Big Endian file",
	0, "raw")
	if (! Sound_writeToRawFile (ONLY_OBJECT, file, 0, 0, 16, 0)) return 0;
END

FORM_WRITE (Sound_writeToRawFileLE, "Sound: Write to raw 16-bit Little Endian file",
	0, "raw")
	if (! Sound_writeToRawFile (ONLY_OBJECT, file, 0, 1, 16, 0)) return 0;
END

/************ Spectrograms *********************************************/

FORM (Spectrograms_to_DTW, "Spectrograms: To DTW", 0)
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

FORM (Spectrum_drawPhases, "Spectrum: Draw phases", "Spectrum: Draw phases...")
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "0.0")
	REAL ("Minimum phase (dB/Hz)", "0.0 (= auto)")
	REAL ("Maximum phase (dB/Hz)", "0.0 (= auto)")
	BOOLEAN ("Unwrap", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Spectrum_drawPhases (OBJECT, GRAPHICS,
		GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		GET_REAL ("Minimum phase"), GET_REAL ("Maximum phase"),
		GET_INTEGER ("Unwrap"), GET_INTEGER ("Garnish")))
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

FORM (Spline_drawKnots, "Spline: Draw knots", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Spline_drawKnots (OBJECT, GRAPHICS,
		GET_REAL ("Xmin"), GET_REAL ("Xmax"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Garnish")))
END

DIRECT (Spline_getOrder)
	Melder_information1 (Melder_integer (Spline_getOrder (ONLY_OBJECT)));
END

FORM (Spline_scaleX, "Spline: Scale x", "Spline: Scale x...")
	LABEL ("", "New domain")
	REAL ("Xmin", "-1.0") 
	REAL ("Xmax", "1.0")
	OK
DO
	double xmin = GET_REAL ("Xmin"), xmax = GET_REAL ("Xmax");
	REQUIRE (xmin < xmax, "Xmin must be smaller than Xmax.")
	EVERY_TO (Spline_scaleX (OBJECT, xmin, xmax))
END

/************ SSCP ***************************************************/

DIRECT (SSCP_help) Melder_help (L"SSCP"); END

FORM (SSCP_drawConfidenceEllipse, "SSCP: Draw confidence ellipse", 0)
	POSITIVE ("Confidence level", "0.95")
	NATURAL ("Index for X-axis", "1")
	NATURAL ("Index for Y-axis", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (SSCP_drawConcentrationEllipse (OBJECT, GRAPHICS,
		GET_REAL ("Confidence level"), 1,
		GET_INTEGER ("Index for X-axis"), GET_INTEGER ("Index for Y-axis"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"), GET_REAL ("left Vertical range"),
		GET_REAL ("right Vertical range"), GET_INTEGER ("Garnish")))
END

FORM (SSCP_drawSigmaEllipse, "SSCP: Draw sigma ellipse",
	"SSCP: Draw sigma ellipse...")
	POSITIVE ("Number of sigmas", "1.0")
	NATURAL ("Index for X-axis", "1")
	NATURAL ("Index for Y-axis", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (SSCP_drawConcentrationEllipse (OBJECT, GRAPHICS,
		GET_REAL ("Number of sigmas"), 0,
		GET_INTEGER ("Index for X-axis"), GET_INTEGER ("Index for Y-axis"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"), GET_INTEGER ("Garnish")))
END

DIRECT (SSCP_extractCentroid)
	EVERY_CHECK (praat_new2 (SSCP_extractCentroid (OBJECT),
		Thing_getNameW (OBJECT), L"_centroid"))
END

FORM (SSCP_getConfidenceEllipseArea, "SSCP: Get confidence ellipse area", 
	"SSCP: Get confidence ellipse area...")
	POSITIVE ("Confidence level", "0.95")
	NATURAL ("Index for X-axis", "1")
	NATURAL ("Index for Y-axis", "2")
	OK
DO
	double conf = GET_REAL ("Confidence level");
	long d1 = GET_INTEGER ("Index for X-axis");
	long d2 = GET_INTEGER ("Index for Y-axis");
	Melder_information1 (Melder_double (SSCP_getConcentrationEllipseArea (ONLY_OBJECT, conf, 1, d1, d2)));
END

FORM (SSCP_getFractionVariation, "SSCP: Get fraction variation", 
	"SSCP: Get fraction variation...")
	NATURAL ("From dimension", "1")
	NATURAL ("To dimension", "1")
	OK
DO
	Melder_information1 (Melder_double (SSCP_getFractionVariation (ONLY_OBJECT,
		GET_INTEGER ("From dimension"), GET_INTEGER ("To dimension"))));
END


FORM (SSCP_getConcentrationEllipseArea, "SSCP: Get sigma ellipse area",
	"SSCP: Get sigma ellipse area...")
	POSITIVE ("Number of sigmas", "1.0")
	NATURAL ("Index for X-axis", "1")
	NATURAL ("Index for Y-axis", "2")
	OK
DO
	double nsigmas = GET_REAL ("Number of sigmas");
	long d1 = GET_INTEGER ("Index for X-axis");
	long d2 = GET_INTEGER ("Index for Y-axis");
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

FORM (SSCP_getCentroidElement, "SSCP: Get centroid element",
	"SSCP: Get centroid element")
	NATURAL ("Number", "1")
	OK
DO
	SSCP me = ONLY_OBJECT;
	long number = GET_INTEGER ("Number");
	if (number < 1 || number > my numberOfColumns)
	{
		return Melder_error ("SSCP_getCentroidElement: "
		"\"Number\" must be smaller than %ld.", my numberOfColumns + 1);
	}
	Melder_information1 (Melder_double (my centroid[number]));
END

DIRECT (SSCP_getLnDeterminant)
	Melder_information1 (Melder_double (SSCP_getLnDeterminant (ONLY_OBJECT)));
END

FORM (SSCP_testDiagonality_bartlett, "SSCP: Get diagonality (bartlett)", 
	"SSCP: Get diagonality (bartlett)...")
	NATURAL ("Number of contraints", "1")
	OK
DO
	double chisq, p;
	long nc = GET_INTEGER ("Number of contraints");
	SSCP me = ONLY_OBJECT;
	SSCP_testDiagonality_bartlett (me, nc, &chisq, &p);
	Melder_information6 (Melder_double (p), L" (=probability for chisq = ", Melder_double (chisq), L" and ndf = ", 
		Melder_integer (my numberOfRows * (my numberOfRows - 1) / 2), L")");
END
 
DIRECT (SSCP_to_Correlation)
	EVERY_TO (SSCP_to_Correlation (OBJECT))
END

FORM (SSCP_to_Covariance, "SSCP: To Covariance", "SSCP: To Covariance...")
	NATURAL ("Number of constraints", "1")
	OK
DO
	long noc = GET_INTEGER ("Number of constraints");
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

/*DIRECT (Strings_append)
	Strings s1 = NULL, s2 = NULL;
	WHERE (SELECTED)
	{
		if (s1) s2 = OBJECT;
		else s1 = OBJECT;
	}	
	if (! praat_new (Strings_append (s1, s2), "%s_%s", Thing_getName (s1), Thing_getName (s2))) return 0;
END*/

DIRECT (Strings_to_Categories)
	EVERY_TO (Strings_to_Categories (OBJECT))
END

FORM (Strings_setString, "Strings: Set string", "Strings: Set string...")
	NATURAL ("Index", "1")
	SENTENCE ("String", "")
	OK	
DO
	if (! Strings_setString (ONLY (classStrings), GET_STRINGW (L"String"),
		GET_INTEGER ("Index"))) return 0;
END

FORM (Strings_change, "Strings: Change", "Strings: Change")
	SENTENCE ("Search", "a")
	SENTENCE ("Replace", "a")
	INTEGER ("Replace limit", "0 (=unlimited)")
	RADIO ("Search and replace are:", 1)
	RADIOBUTTON ("Literals")
	RADIOBUTTON ("Regular Expressions")	
	OK
DO
	long nmatches, nstringmatches;
	EVERY_TO (Strings_change (OBJECT, GET_STRINGW (L"Search"),
		GET_STRINGW (L"Replace"), GET_INTEGER ("Replace limit"), &nmatches, 
		&nstringmatches, GET_INTEGER ("Search and replace are") - 1))
END

FORM (Strings_extractPart, "Strings: Extract part", "")
	NATURAL ("From index", "1")
	NATURAL ("To index", "1")
	OK
DO
	EVERY_TO (Strings_extractPart (OBJECT, GET_INTEGER ("From index"), GET_INTEGER ("To index")))
END

FORM (Strings_to_Permutation, "Strings: To Permutation", "Strings: To Permutation...")
	BOOLEAN ("Sort", 1)
	OK
DO
	EVERY_TO (Strings_to_Permutation (OBJECT, GET_INTEGER ("Sort")))
END

DIRECT (Strings_and_Permutation_permuteStrings)
	NEW (Strings_and_Permutation_permuteStrings (ONLY (classStrings), ONLY (classPermutation)))
END

FORM (SVD_to_TableOfReal, "SVD: To TableOfReal", "SVD: To TableOfReal...")
	NATURAL ("First component", "1")
	INTEGER ("Last component", "0 (=all)")
	OK
DO
	EVERY_TO (SVD_to_TableOfReal (OBJECT, GET_INTEGER ("First component"),
		GET_INTEGER ("Last component")))
END

DIRECT (SVD_extractLeftSingularVectors)
	SVD svd = ONLY (classSVD);
	if (! praat_new2 (SVD_extractLeftSingularVectors (svd), Thing_getNameW (svd), L"_lsv")) return 0;
END

DIRECT (SVD_extractRightSingularVectors)
	SVD svd = ONLY (classSVD);
	if (! praat_new2 (SVD_extractRightSingularVectors (svd), Thing_getNameW (svd), L"_rsv")) return 0;
END

DIRECT (SVD_extractSingularValues)
	SVD svd = ONLY (classSVD);
	if (! praat_new2 (SVD_extractSingularValues (svd), Thing_getNameW (svd), L"_sv")) return 0;
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

/******************* TableOfReal ****************************/

DIRECT (TableOfReal_and_Permutation_permuteRows)
	TableOfReal t = ONLY (classTableOfReal);
	Permutation p = ONLY (classPermutation);
	if (! praat_new3 (TableOfReal_and_Permutation_permuteRows (t, p),
		Thing_getNameW (t), L"_", Thing_getNameW (p))) return 0;
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
    
FORM (TableOfReal_createFromPolsData_50males, "Create TableOfReal (Pols 1973)",
	"Create TableOfReal (Pols 1973)...")
	BOOLEAN ("Include formant levels", 0)
	OK
DO
	if (! praat_new1 (TableOfReal_createFromPolsData_50males
		(GET_INTEGER ("Include formant levels")), L"pols_50males")) return 0;
END

FORM (TableOfReal_createFromVanNieropData_25females, "Create TableOfReal (Van Nierop 1973)...",
	"Create TableOfReal (Van Nierop 1973)...")
	BOOLEAN ("Include formant levels", 0)
	OK
DO
	if (! praat_new1 (TableOfReal_createFromVanNieropData_25females
		(GET_INTEGER ("Include formant levels")), L"vannierop_25females")) return 0;
END

FORM (TableOfReal_createFromWeeninkData,"Create TableOfReal (Weenink 1985)...",
	"Create TableOfReal (Weenink 1985)...")
	RADIO ("Speakers group", 1)
	RADIOBUTTON ("Men")
	RADIOBUTTON ("Women")
	RADIOBUTTON ("Children")
	OK
DO
	int type = GET_INTEGER ("Speakers group");
	if (! praat_new1 (TableOfReal_createFromWeeninkData (type), 
		(type == 1 ? L"m10" : type == 2 ? L"w10" : L"c10"))) return 0;
END

FORM (TableOfReal_drawScatterPlot, "TableOfReal: Draw scatter plot", "TableOfReal: Draw scatter plot...")
	LABEL ("","Select the part of the table")
	NATURAL ("Horizontal axis column number", "1")
	NATURAL ("Vertical axis column number", "2")
	INTEGER ("left Row number range", "0")
	INTEGER ("right Row number range", "0")
	LABEL ("", "Select the drawing area limits")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	NATURAL ("Label size", "12")
	BOOLEAN ("Use row labels", 0)
	WORD ("Label", "+")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawScatterPlot (OBJECT, GRAPHICS,
			GET_INTEGER ("Horizontal axis column number"), 
			GET_INTEGER ("Vertical axis column number"),
			GET_INTEGER ("left Row number range"), GET_INTEGER ("right Row number range"),
			GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
			GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
			GET_INTEGER ("Label size"), GET_INTEGER ("Use row labels"), 
			GET_STRINGW (L"Label"), GET_INTEGER ("Garnish")))
END

FORM (TableOfReal_drawScatterPlotMatrix, "TableOfReal: Draw scatter plots matrix", 0)
	INTEGER ("From column", "0")
	INTEGER ("To column", "0")
	POSITIVE ("Fraction white", "0.1")
	OK
DO
	EVERY_DRAW (TableOfReal_drawScatterPlotMatrix (OBJECT, GRAPHICS,
		GET_INTEGER ("From column"), GET_INTEGER ("To column"),
		GET_REAL ("Fraction white")))
END

FORM (TableOfReal_drawBiplot, "TableOfReal: Draw biplot", "TableOfReal: Draw biplot...")
	LABEL ("","")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	REAL ("Split factor", "0.5")
	INTEGER ("Label size", "10")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawBiplot (OBJECT, GRAPHICS, 
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"), GET_REAL ("Split factor"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

FORM (TableOfReal_drawBoxPlots, "TableOfReal: Draw box plots", 
	"TableOfReal: Draw box plots...")
	INTEGER ("From row", "0")
	INTEGER ("To row", "0")
	INTEGER ("From column", "0")
	INTEGER ("To column", "0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawBoxPlots (OBJECT, GRAPHICS,
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("From column"), GET_INTEGER ("To column"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"), GET_INTEGER ("Garnish")))
END

FORM (TableOfReal_to_Configuration_lda, "TableOfReal: To Configuration (lda)",
	"TableOfReal: To Configuration (lda)...")
	INTEGER ("Number of dimensions", "0")
	OK
DO
	long dimension = GET_INTEGER ("Number of dimensions");
	REQUIRE (dimension >= 0, "Number of dimensions must be greater equal zero.")
	EVERY_TO (TableOfReal_to_Configuration_lda (OBJECT, dimension))
END

FORM (TableOfReal_to_CCA, "TableOfReal: To CCA", "TableOfReal: To CCA...")
	NATURAL ("Dimension of dependent variate", "2")
	OK
DO
	EVERY_TO (TableOfReal_to_CCA (OBJECT, GET_INTEGER ("Dimension of dependent variate")))
END

FORM (TableOfReal_to_Configuration_pca, "TableOfReal: To Configuration (pca)",
	"TableOfReal: To Configuration (pca)...")
	NATURAL ("Number of dimensions", "2")
	OK
DO
	EVERY_TO (TableOfReal_to_Configuration_pca (OBJECT,
		GET_INTEGER ("Number of dimensions")))
END

DIRECT (TableOfReal_to_Discriminant)
	EVERY_TO (TableOfReal_to_Discriminant (OBJECT))
END

DIRECT (TableOfReal_to_PCA)
	EVERY_TO (TableOfReal_to_PCA (OBJECT))
END

FORM (TableOfReal_to_SSCP, "TableOfReal: To SSCP", "TableOfReal: To SSCP...")
	INTEGER ("Begin row", "0")
	INTEGER ("End row", "0")
	INTEGER ("Begin column", "0")
	INTEGER ("End column", "0")
	OK
DO
	EVERY_TO (TableOfReal_to_SSCP (OBJECT, GET_INTEGER ("Begin row"), GET_INTEGER ("End row"),
		GET_INTEGER ("Begin column"), GET_INTEGER ("End column")))
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

FORM (TableOfReal_choleskyDecomposition, "TableOfReal: Cholesky decomposition", 0)
	BOOLEAN ("Upper (else L)", 0)
	BOOLEAN ("Inverse", 0)
	OK
DO
	EVERY_TO (TableOfReal_choleskyDecomposition (OBJECT, GET_INTEGER ("Upper"), GET_INTEGER ("Inverse")))
END

FORM (TableOfReal_to_Pattern_and_Categories, "TableOfReal: To Pattern and Categories", 
	"TableOfReal: To Pattern and Categories...")
	INTEGER ("left Row range", "0")
	INTEGER ("right Row range", "0 (=all)")
	INTEGER ("left Column range", "0")
	INTEGER ("right Column range", "0 (=all)")
	OK
DO
	Pattern p; Categories c; TableOfReal t = ONLY_OBJECT;
	if (TableOfReal_to_Pattern_and_Categories (t, GET_INTEGER ("left Row range"),
		GET_INTEGER ("right Row range"), GET_INTEGER ("left Column range"),
		GET_INTEGER ("right Column range"), &p, &c))
	{
		wchar_t *name = Thing_getNameW (t);
		praat_new1 (p, name);
		praat_new1 (c, name);
	}
END

FORM (TableOfReal_getColumnSum, "TableOfReal: Get column sum", "")
	INTEGER ("Column", "1")
	OK
DO
	Melder_information1 (Melder_double (TableOfReal_getColumnSum (ONLY_GENERIC(classTableOfReal), GET_INTEGER ("Column"))));
END

FORM (TableOfReal_getRowSum, "TableOfReal: Get row sum", "")
	INTEGER ("Row", "1")
	OK
DO
	Melder_information1 (Melder_double (TableOfReal_getRowSum (ONLY_GENERIC(classTableOfReal), GET_INTEGER ("Row"))));
END

DIRECT (TableOfReal_getGrandSum)
	Melder_information1 (Melder_double (TableOfReal_getGrandSum (ONLY_GENERIC(classTableOfReal))));
END

FORM (TableOfReal_meansByRowLabels, "TableOfReal: Means by row labels", "TableOfReal: To TableOfReal (means by row labels)...")
    BOOLEAN ("Expand", 0)
	OK
DO
	EVERY_CHECK(praat_new2 (TableOfReal_meansByRowLabels (OBJECT, GET_INTEGER ("Expand"), 0), NAMEW, L"_byrowlabels"))
END

FORM (TableOfReal_mediansByRowLabels, "TableOfReal: Medians by row labels", "TableOfReal: To TableOfReal (medians by row labels)...")
    BOOLEAN ("Expand", 0)
	OK
DO
	EVERY_CHECK(praat_new2 (TableOfReal_meansByRowLabels (OBJECT, GET_INTEGER ("Expand"), 1), NAMEW, L"_byrowlabels"))
END

/***** TableOfReal and FilterBank  *****/

FORM (TextGrid_extendTime, "TextGrid: Extend time", "TextGrid: Extend time...")
	LABEL ("", "")
	POSITIVE ("Extend domain by (s)", "1.0")
	RADIO ("At", 1)
	RADIOBUTTON ("End")
	RADIOBUTTON ("Start")
	OK
DO
	WHERE (SELECTED)
	{
		TextGrid_extendTime (OBJECT, GET_REAL ("Extend domain by"), GET_INTEGER ("At") - 1);
		praat_dataChanged (OBJECT);
	} 
END

FORM (TextGrid_replaceIntervalTexts, "TextGrid: Replace interval text", "TextGrid: Replace interval text...")
	LABEL ("", "")
	NATURAL ("Tier number", "1")
	INTEGER ("left Interval range", "0")
	INTEGER ("right Interval range", "0")
	SENTENCE ("Search", "a")
	SENTENCE ("Replace", "a")	
	RADIO ("Search and replace strings are:", 1)
	RADIOBUTTON ("Literals")
	RADIOBUTTON ("Regular Expressions")	
	OK	
DO
	long from = GET_INTEGER ("left Interval range");
	long to = GET_INTEGER ("right Interval range");
	int regexp = GET_INTEGER ("Search and replace strings are") - 1;
	wchar_t *search = GET_STRINGW (L"Search");
	long nmatches, nstringmatches;
	
	WHERE (SELECTED)
	{
		if (! TextGrid_changeLabels (OBJECT, GET_INTEGER ("Tier number"), from, to, search, GET_STRINGW (L"Replace"),
			regexp, &nmatches, &nstringmatches)) return 0;
		praat_dataChanged (OBJECT);
	}
END


FORM (TextGrid_replacePointTexts, "TextGrid: Replace point text", "TextGrid: Replace point text...")
	LABEL ("", "")
	NATURAL ("Tier number", "1")
	INTEGER ("left Interval range", "0")
	INTEGER ("right Interval range", "0")
	SENTENCE ("Search", "a")
	SENTENCE ("Replace", "a")	
	RADIO ("Search and replace strings are:", 1)
	RADIOBUTTON ("Literals")
	RADIOBUTTON ("Regular Expressions")	
	OK	
DO
	long from = GET_INTEGER ("left Interval range");
	long to = GET_INTEGER ("right Interval range");
	long nmatches, nstringmatches;
	WHERE (SELECTED)
	{
		if (! TextGrid_changeLabels (OBJECT, GET_INTEGER ("Tier number"), from, to, GET_STRINGW (L"Search"), GET_STRINGW (L"Replace"),
			GET_INTEGER ("Search and replace strings are")-1, &nmatches, &nstringmatches)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setTierName, "TextGrid: Set tier name", "TextGrid: Set tier name...")
	NATURAL ("Tier number:", "1")
	SENTENCE ("Name", "");
	OK
DO
	if (! TextGrid_setTierName (ONLY_OBJECT, GET_INTEGER ("Tier number"), 
		GET_STRINGW (L"Name"))) return 0;
		praat_dataChanged (OBJECT);
END	

static Any cmuAudioFileRecognizer (int nread, const char *header, MelderFile fs)
{
	return nread < 12 || header [0] != 6 || header [1] != 0 ?
	 NULL : Sound_readFromCmuAudioFile (fs);
}

void praat_CC_init (void *klas)
{
	praat_addAction1 (klas, 1, "Paint...", 0, 1,
		DO_CC_paint);
	praat_addAction1 (klas, 1, "Draw...", 0, 1,
		DO_CC_drawC0);
	praat_addAction1 (klas, 1, QUERY_BUTTON, 0, 0, 0);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 0, "To Matrix", 0, 0, DO_CC_to_Matrix);
	praat_addAction1 (klas, 2, "To DTW...", 0, 0, DO_CCs_to_DTW);
}

static void praat_Eigen_Matrix_project (void *klase, void *klasm);
static void praat_Eigen_Matrix_project (void *klase, void *klasm)
{
	praat_addAction2 (klase, 1, klasm, 1, "Project...", 0, 0, 
		DO_Eigen_and_Matrix_project);
}

static void praat_Eigen_query_init (void *klas)
{
	praat_addAction1 (klas, 1, "Get eigenvalue...", 0, 1,
		DO_Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, "Get sum of eigenvalues...", 0, 1,
		DO_Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, "Get number of eigenvectors", 0, 1,
		DO_Eigen_getNumberOfEigenvalues);
	praat_addAction1 (klas, 1, "Get eigenvector dimension", 0, 1,
		DO_Eigen_getDimension);
	praat_addAction1 (klas, 1, "Get eigenvector element...", 0, 1,
		DO_Eigen_getEigenvectorElement);
}

static void praat_Eigen_draw_init (void *klas)
{
	praat_addAction1 (klas, 0, "Draw eigenvalues...", 0, 1, DO_Eigen_drawEigenvalues);
	praat_addAction1 (klas, 0, "Draw eigenvalues (scree)...", 0, 
		praat_DEPTH_1 | praat_HIDDEN, DO_Eigen_drawEigenvalues_scree);
	praat_addAction1 (klas, 0, "Draw eigenvector...", 0, 1, DO_Eigen_drawEigenvector);
}

static void praat_Index_init (void *klas)
{
    praat_addAction1 (klas, 1, "Get number of classes", 0, 0, DO_Index_getNumberOfClasses);
    praat_addAction1 (klas, 1, "To Permutation...", 0, 0, DO_Index_to_Permutation);
    praat_addAction1 (klas, 1, "Extract part...", 0, 0, DO_Index_extractPart);
}

static void praat_FilterBank_query_init (void *klas);
static void praat_FilterBank_query_init (void *klas)
{
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
	praat_Matrixft_query_init (klas);
	praat_addAction1 (klas, 0, "-- frequency scales --", 0, 1, 0);
	praat_addAction1 (klas, 1, "Get frequency in Hertz...", 0, 1, 
		DO_FilterBank_getFrequencyInHertz);
	praat_addAction1 (klas, 1, "Get frequency in Bark...", 0, 1, 
		DO_FilterBank_getFrequencyInBark);
	praat_addAction1 (klas, 1, "Get frequency in mel...", 0, 1, 
		DO_FilterBank_getFrequencyInMel);	
}

static void praat_FilterBank_modify_init (void *klas);
static void praat_FilterBank_modify_init (void *klas)
{
	praat_addAction1 (klas, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, "Equalize intensities...", 0, 1,
			DO_FilterBank_equalizeIntensities);
}

static void praat_FilterBank_draw_init (void *klas);
static void praat_FilterBank_draw_init (void *klas)
{
	praat_addAction1 (klas, 0, DRAW_BUTTON, 0, 0, 0);
		praat_addAction1 (klas, 0, "Draw filters...", 0, 1,
			DO_FilterBank_drawFilters);
		praat_addAction1 (klas, 0, "Draw one contour...", 0, 1,
			DO_FilterBank_drawOneContour);
		praat_addAction1 (klas, 0, "Draw contours...", 0, 1,
			DO_FilterBank_drawContours);
		praat_addAction1 (klas, 0, "Paint image...", 0, 1,
			DO_FilterBank_paintImage);
		praat_addAction1 (klas, 0, "Paint contours...", 0, 1,
			DO_FilterBank_paintContours);
		praat_addAction1 (klas, 0, "Paint cells...", 0, 1,
			DO_FilterBank_paintCells);
		praat_addAction1 (klas, 0, "Paint surface...", 0, 1,
			DO_FilterBank_paintSurface);
		praat_addAction1 (klas, 0, "-- frequency scales --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Draw frequency scales...", 0, 1,
			DO_FilterBank_drawFrequencyScales);
		
}

static void praat_FilterBank_all_init (void *klas);
static void praat_FilterBank_all_init (void *klas)
{
	praat_FilterBank_draw_init (klas);
	praat_FilterBank_query_init (klas);
	praat_FilterBank_modify_init (klas);
	praat_addAction1 (klas, 0, "To Intensity", 
		0, 0, DO_FilterBank_to_Intensity); 	
	praat_addAction1 (klas, 0, "To Matrix", 
		0, 0, DO_FilterBank_to_Matrix);
}

static void praat_FunctionTerms_init (void *klas)
{
	praat_addAction1 (klas, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, "Draw...", 0, 1, DO_FunctionTerms_draw);
	praat_addAction1 (klas, 0, "Draw basis function...", 0, 1,
		DO_FunctionTerms_drawBasisFunction);
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
		praat_addAction1 (klas, 1, "Get number of coefficients", 0, 1,
			DO_FunctionTerms_getNumberOfCoefficients);
		praat_addAction1 (klas, 1, "Get coefficient...", 0, 1,
			DO_FunctionTerms_getCoefficient);
		praat_addAction1 (klas, 1, "Get degree", 0, 1,
			DO_FunctionTerms_getDegree);
		praat_addAction1 (klas, 0, "-- function specifics --", 0, 1, 0);
		praat_addAction1 (klas, 1, "Get value...", 0, 1,
			DO_FunctionTerms_evaluate);
		praat_addAction1 (klas, 1, "Get minimum...", 0, 1,
			DO_FunctionTerms_getMinimum);
		praat_addAction1 (klas, 1, "Get x of minimum...", 0, 1,
			DO_FunctionTerms_getXOfMinimum);
		praat_addAction1 (klas, 1, "Get maximum...", 0, 1,
			DO_FunctionTerms_getMaximum);
		praat_addAction1 (klas, 1, "Get x of maximum...", 0, 1,
			DO_FunctionTerms_getXOfMaximum);
	praat_addAction1 (klas, 0, "Modify -               ", 0, 0, 0);
		praat_addAction1 (klas, 1, "Set domain...", 0, 1,
			DO_FunctionTerms_setDomain);
		praat_addAction1 (klas, 1, "Set coefficient...", 0, 1,
			DO_FunctionTerms_setCoefficient);
	praat_addAction1 (klas, 0, "Analyse", 0, 0, 0);
}

/* Query buttons for frame-based frequency x time subclasses of matrix. */

void praat_Matrixft_query_init (void *klas)
{
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, "Get time from column...", 0, 1,
		DO_Matrixft_getXofColumn);
	praat_addAction1 (klas, 1, "-- frequencies --", 0, 1, 0);
	praat_addAction1 (klas, 1, "Get lowest frequency", 0, 1,
		DO_Matrixft_getLowestFrequency);
	praat_addAction1 (klas, 1, "Get highest frequency", 0, 1,
		DO_Matrixft_getHighestFrequency);	
	praat_addAction1 (klas, 1, "Get number of frequencies", 0, 1,
		DO_Matrixft_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, "Get frequency distance", 0, 1,
		DO_Matrixft_getFrequencyDistance);
	praat_addAction1 (klas, 1, "Get frequency from row...", 0, 1,
		DO_Matrixft_getFrequencyOfRow);
	praat_addAction1 (klas, 1, "-- get value --", 0, 1, 0);
	praat_addAction1 (klas, 1, "Get value in cell...", 0, 1,
		DO_Matrixft_getValueInCell);		
}

static void praat_Spline_init (void *klas)
{
	praat_FunctionTerms_init (klas);
	praat_addAction1 (klas, 0, "Draw knots...", "Draw basis function...",
		1, DO_Spline_drawKnots);
	praat_addAction1 (klas, 1, "Get order", "Get degree", 1,
		DO_Spline_getOrder);
	praat_addAction1 (klas, 1, "Scale x...", "Analyse",
		0, DO_Spline_scaleX);

}

static void praat_SSCP_query_init (void *klas)
{
	praat_addAction1 (klas, 1, "-- statistics --", "Get value...", 1, 0);
	praat_addAction1 (klas, 1, "Get number of observations",
		"-- statistics --", 1, DO_SSCP_getNumberOfObservations);
	praat_addAction1 (klas, 1, "Get degrees of freedom",
		"Get number of observations", 1, DO_SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, "Get centroid element...", 
		"Get degrees of freedom",1, DO_SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, "Get ln(determinant)", 
		"Get centroid element...", 1, DO_SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (void *klas)
{
	praat_addAction1 (klas, 1, "Extract centroid", EXTRACT_BUTTON,
		1, DO_SSCP_extractCentroid);
}

void praat_TableOfReal_init2  (void *klas)
{
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, "To TableOfReal",
		"To Matrix", 1, DO_TableOfReal_to_TableOfReal);
}

/* Query buttons for frame-based time-based subclasses of Sampled.
void praat_TimeFrameSampled_query_init (void *klas)
{
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, "Get number of frames", 0, 1,
		DO_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, "Get frame length", 0, 1,
		DO_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, "Get time from frame...", 0, 1,
		DO_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, "Get frame from time...", 0, 1,
		DO_TimeFrameSampled_getFrameFromTime);
}
*/
void praat_uvafon_David_init (void);
void praat_uvafon_David_init (void)
{
	Data_recognizeFileType (TextGrid_TIMITLabelFileRecognizer);
	Data_recognizeFileType (cmuAudioFileRecognizer);
	
    Thing_recognizeClassesByName (classActivation, classBarkFilter,
		classCategories, classCCA,
		classChebyshevSeries,classClassificationTable, classConfusion, 
    	classCorrelation, classCovariance, classDiscriminant, classDTW,
		classEigen, classExcitations, classFormantFilter, classPermutation,
		classISpline, classLegendreSeries,
		classMelFilter,
		classMSpline, classPattern, classPCA, classPolynomial, classRoots,
		classSimpleString, classStringsIndex, classSSCP, classSVD, NULL);

    praat_addMenuCommand ("Objects", "Goodies", "Report floating point properties",
		0, 0, DO_Praat_ReportFloatingPointProperties);
		
	praat_addMenuCommand ("Objects", "New", "Create Permutation...", 0, 0, DO_Permutation_create);

    praat_addMenuCommand ("Objects", "New", "Polynomial", 0, 0, 0);
    	praat_addMenuCommand ("Objects", "New", "Create Polynomial...", 0, 1,
			DO_Polynomial_create);
    	praat_addMenuCommand ("Objects", "New", "Create LegendreSeries...",
			0, 1, DO_LegendreSeries_create);
    	praat_addMenuCommand ("Objects", "New", "Create ChebyshevSeries...",
			0, 1, DO_ChebyshevSeries_create);
    	praat_addMenuCommand ("Objects", "New", "Create MSpline...",
			0, 1, DO_MSpline_create);
    	praat_addMenuCommand ("Objects", "New", "Create ISpline...",
			0, 1, DO_ISpline_create);
	praat_addMenuCommand ("Objects", "New", "Create Sound from gamma-tone...",
		"Create Sound from tone complex...", 1, DO_Sound_createFromGammaTone);
	praat_addMenuCommand ("Objects", "New", "Create Sound from Shepard tone...",
		"Create Sound from gamma-tone...", 1, DO_Sound_createFromShepardTone);
	praat_addMenuCommand ("Objects", "New", "Create formant table (Pols & Van Nierop 1973)", "Create Table...",
		1, DO_Table_createFromPolsVanNieropData);
	praat_addMenuCommand ("Objects", "New", "Create formant table (Peterson & Barney 1952)", 
		"Create Table...", 1, DO_Table_createFromPetersonBarneyData);
	praat_addMenuCommand ("Objects", "New", "Create formant table (Weenink 1985)", 
		"Create formant table (Peterson & Barney 1952)",1, DO_Table_createFromWeeninkData);
	praat_addMenuCommand ("Objects", "New", "Create TableOfReal (Pols 1973)...",
		"Create TableOfReal...", 1, DO_TableOfReal_createFromPolsData_50males);
	praat_addMenuCommand ("Objects", "New", "Create TableOfReal (Van Nierop 1973)...",
		"Create TableOfReal (Pols 1973)...", 1, DO_TableOfReal_createFromVanNieropData_25females);
	praat_addMenuCommand ("Objects", "New", "Create TableOfReal (Weenink 1985)...",
		"Create TableOfReal (Van Nierop 1973)...", 1, DO_TableOfReal_createFromWeeninkData);
	
	praat_addMenuCommand ("Objects", "Read", "Read Sound from raw 16-bit "
		"Little Endian file...", "Read from special sound file", 1,
		 DO_Sound_readFromRawFileLE);
	praat_addMenuCommand ("Objects", "Read", "Read Sound from raw 16-bit "
		"Big Endian file...", "Read Sound from raw 16-bit Little Endian "
		"file...", 1, DO_Sound_readFromRawFileBE);

    praat_addAction1 (classActivation, 0, "Modify", 0, 0, 0);
    praat_addAction1 (classActivation, 0, "Formula...", 0, 0,
		DO_Activation_formula);
    praat_addAction1 (classActivation, 0, "Hack", 0, 0, 0);
    praat_addAction1 (classActivation, 0, "To Matrix", 0, 0,
		DO_Activation_to_Matrix);
    
	praat_addAction2 (classActivation, 1, classCategories, 1, "To TableOfReal",
	    0, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classBarkFilter, 0, "BarkFilter help", 0, 0,
		DO_BarkFilter_help);
	praat_FilterBank_all_init (classBarkFilter);
	praat_addAction1 (classBarkFilter, 0, "Draw spectrum (slice)...", 
		"Draw filters...", 1, DO_BarkFilter_drawSpectrum);
	praat_addAction1 (classBarkFilter, 1, "Draw filter functions...", 
		"Draw filters...", 1, DO_BarkFilter_drawSekeyHansonFilterFunctions);

    praat_addAction1 (classCategories, 0, "Edit", 0, 0, DO_Categories_edit);
    praat_addAction1 (classCategories, 0, QUERY_BUTTON, 0, 0, 0);
    praat_addAction1 (classCategories, 2, "Get difference", QUERY_BUTTON,
		praat_HIDDEN | praat_DEPTH_1, DO_Categories_difference);
     praat_addAction1 (classCategories, 2, "Get number of differences", QUERY_BUTTON,
		1, DO_Categories_getNumberOfDifferences);
      praat_addAction1 (classCategories, 2, "Get fraction different", QUERY_BUTTON,
		1, DO_Categories_getFractionDifferent);
	praat_addAction1 (classCategories, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_addAction1 (classCategories, 1, "Append 1 category...", MODIFY_BUTTON,
		1, DO_Categories_append);	
    praat_addAction1 (classCategories, 0, "Extract", 0, 0, 0);
    praat_addAction1 (classCategories, 0, "To unique Categories", 0, 0,
		DO_Categories_selectUniqueItems);
    praat_addAction1 (classCategories, 0, "Analyse", 0, 0, 0);
    praat_addAction1 (classCategories, 2, "To Confusion", 0, 0,
		DO_Categories_to_Confusion);
    praat_addAction1 (classCategories, 0, "Synthesize", 0, 0, 0);
    praat_addAction1 (classCategories, 2, "Join", 0, 0, DO_Categories_join);
    praat_addAction1 (classCategories, 0, "Permute items", 0, 0, DO_Categories_permuteItems);
    praat_addAction1 (classCategories, 0, "To Strings", 0, 0,
		DO_Categories_to_Strings);

	praat_addAction1 (classChebyshevSeries, 0, "ChebyshevSeries help", 0, 0,
		DO_ChebyshevSeries_help);
	praat_FunctionTerms_init (classChebyshevSeries);
	praat_addAction1 (classChebyshevSeries, 0, "To Polynomial", "Analyse",
		0, DO_ChebyshevSeries_to_Polynomial);

	praat_addAction1 (classCCA, 1, "Draw eigenvector...", 0, 0,
		 DO_CCA_drawEigenvector);
	praat_addAction1 (classCCA, 1, "Get number of correlations", 0, 0,
		 DO_CCA_getNumberOfCorrelations);
	praat_addAction1 (classCCA, 1, "Get correlation...", 0, 0,
		 DO_CCA_getCorrelationCoefficient);
	praat_addAction1 (classCCA, 1, "Get eigenvector element...", 0, 0, DO_CCA_getEigenvectorElement);
	praat_addAction1 (classCCA, 1, "Get zero correlation probability...", 0, 0,
		 DO_CCA_getZeroCorrelationProbability);

	praat_addAction2 (classCCA, 1, classTableOfReal, 1, "To TableOfReal (scores)...",
		0, 0, DO_CCA_and_TableOfReal_scores);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, "To TableOfReal (loadings)",
		0, 0, DO_CCA_and_TableOfReal_factorLoadings);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, "Predict...", 0, 0,
		DO_CCA_and_TableOfReal_predict);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, "To TableOfReal (loadings)", 
		0, 0, DO_CCA_and_Correlation_factorLoadings);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, "Get variance fraction...",
		0, 0, DO_CCA_and_Correlation_getVarianceFraction);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, "Get redundancy (sl)...",
		0, 0, DO_CCA_and_Correlation_getRedundancy_sl);
    
	praat_addAction1 (classConfusion, 0, "Confusion help", 0, 0,
		DO_Confusion_help);
    praat_TableOfReal_init2 (classConfusion);
	praat_addAction1 (classConfusion, 0, "-- confusion statistics --",
		"Get value...", 1, 0);
	praat_addAction1 (classConfusion, 1, "Get fraction correct",
		"-- confusion statistics --", 1, DO_Confusion_getFractionCorrect);
	praat_addAction1 (classConfusion, 1, "Get row sum...", 
		"Get fraction correct", 1, DO_TableOfReal_getRowSum);	
 	praat_addAction1 (classConfusion, 1, "Get column sum...", 
		"Get row sum...", 1, DO_TableOfReal_getColumnSum);
	praat_addAction1 (classConfusion, 1, "Get grand sum", 
		"Get column sum...", 1, DO_TableOfReal_getGrandSum);
	praat_addAction1 (classConfusion, 0, "To TableOfReal (marginals)", 
		"To TableOfReal", 0, DO_Confusion_to_TableOfReal_marginals);	
	praat_addAction1 (classConfusion, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classConfusion, 0, "Condense...", 0, 0, 
		DO_Confusion_condense);
    praat_addAction1 (classConfusion, 2, "To difference matrix", 0, 0,
		DO_Confusion_difference);

    praat_addAction2 (classConfusion, 1, classMatrix, 1, "Draw", 0, 0, 0);
    praat_addAction2 (classConfusion, 1, classMatrix, 1, "Draw confusion...",
		0, 0, DO_Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, "Covariance help", 0, 0,
		DO_Covariance_help);
    praat_TableOfReal_init2 (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
	praat_addAction1 (classCovariance, 1, "Get diagonality (bartlett)...",
		"Get ln(determinant)", 1, DO_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classCovariance, 1, "Get significance of one mean...",
		"Get diagonality (bartlett)...", 1,
		DO_Covariance_getSignificanceOfOneMean);
	praat_addAction1 (classCovariance, 1, "Get significance of means "
		"difference...", "Get significance of one mean...", 1,
		DO_Covariance_getSignificanceOfMeansDifference);
	praat_addAction1 (classCovariance, 1, "Get significance of one variance...",
		"Get significance of means difference...", 1,
		DO_Covariance_getSignificanceOfOneVariance);
	praat_addAction1 (classCovariance, 1, "Get significance of variances ratio...",
		"Get significance of one variance...", 1,
		DO_Covariance_getSignificanceOfVariancesRatio);
	praat_addAction1 (classCovariance, 1, "Get fraction variance...",
		"Get significance of variances ratio...", 1, DO_Covariance_getFractionVariance);
	
	praat_addAction1 (classCovariance, 0, "To TableOfReal (random sampling)...",
		0, 0, DO_Covariance_to_TableOfReal_randomSampling);

	praat_addAction1 (classCovariance, 2, "Difference", 0, 0,
		DO_Covariance_difference);
	praat_addAction1 (classCovariance, 0, "To Correlation", 0, 0,
		DO_Covariance_to_Correlation);
	praat_addAction1 (classCovariance, 0, "To PCA", 0, 0,
		DO_Covariance_to_PCA);

	praat_addAction1 (classClassificationTable, 0, "ClassificationTable help", 
		0, 0, DO_ClassificationTable_help);
	praat_TableOfReal_init (classClassificationTable);
	praat_addAction1 (classClassificationTable, 0, "To Confusion", 0, 0,
		DO_ClassificationTable_to_Confusion);
	praat_addAction1 (classClassificationTable, 0, "To Strings (max. prob.)", 0, 0,
		DO_ClassificationTable_to_Strings_maximumProbability);

	praat_addAction1 (classCorrelation, 0, "Correlation help", 0, 0,
		DO_Correlation_help);
    praat_TableOfReal_init2 (classCorrelation);
	praat_SSCP_query_init (classCorrelation);
	praat_SSCP_extract_init (classCorrelation);
	praat_addAction1 (classCorrelation, 1, "Get diagonality (bartlett)...",
		"Get ln(determinant)", 1, DO_Correlation_testDiagonality_bartlett);
	praat_addAction1 (classCorrelation, 0, "Confidence intervals...", 0, 0,
		DO_Correlation_confidenceIntervals);
	praat_addAction1 (classCorrelation, 0, "To PCA", 0, 0,
		DO_Correlation_to_PCA);
		
	praat_addAction1 (classDiscriminant, 0, "Discriminant help", 0, 0,
		DO_Discriminant_help);
	praat_addAction1 (classDiscriminant, 0, DRAW_BUTTON, 0, 0, 0);
		praat_Eigen_draw_init (classDiscriminant);
		praat_addAction1 (classDiscriminant, 0, "-- sscps --", 0, 1, 0);
		praat_addAction1 (classDiscriminant, 0, "Draw sigma ellipses...",
			0, 1, DO_Discriminant_drawSigmaEllipses);
		praat_addAction1 (classDiscriminant, 0, "Draw one sigma ellipse...",
			0, 1, DO_Discriminant_drawOneSigmaEllipse);	
		praat_addAction1 (classDiscriminant, 0, "Draw confidence ellipses...",
			0, 1, DO_Discriminant_drawConfidenceEllipses);

    praat_addAction1 (classDiscriminant, 1, QUERY_BUTTON, 0, 0, 0);
		praat_addAction1 (classDiscriminant, 1, "-- eigen structure --",
			0, 1, 0);
		praat_Eigen_query_init (classDiscriminant);
		praat_addAction1 (classDiscriminant, 1, "-- discriminant --", 0, 1, 0);
		praat_addAction1 (classDiscriminant, 1, "Get number of functions",
			0, 1, DO_Discriminant_getNumberOfFunctions);
		praat_addAction1 (classDiscriminant, 1, "Get dimension of functions",
			0, 1, DO_Discriminant_getDimensionOfFunctions);
		praat_addAction1 (classDiscriminant, 1, "Get number of groups",
			0, 1, DO_Discriminant_getNumberOfGroups);
		praat_addAction1 (classDiscriminant, 1, "Get number of observations...",
			0, 1, DO_Discriminant_getNumberOfObservations);
		praat_addAction1 (classDiscriminant, 1, "-- tests --", 0, 1, 0);
		praat_addAction1 (classDiscriminant, 1, "Get Wilks lambda...",
			0, 1, DO_Discriminant_getWilksLambda);
		praat_addAction1 (classDiscriminant, 1, "Get cumulative contribution of"
			" components...", 0, 1, 
			DO_Discriminant_getCumulativeContributionOfComponents);
		praat_addAction1 (classDiscriminant, 1, "Get partial discrimination "
			"probability...", 0, 1, 
			DO_Discriminant_getPartialDiscriminationProbability);
		praat_addAction1 (classDiscriminant, 1, "Get homogeneity of "
			"covariances (box)", 0, 1, 
			DO_Discriminant_getHomegeneityOfCovariances_box);
		praat_addAction1 (classDiscriminant, 1, "-- ellipses --", 0, 1, 0);
		praat_addAction1 (classDiscriminant, 1, "Get sigma ellipse area...", 
			0, 1, DO_Discriminant_getConcentrationEllipseArea);
		praat_addAction1 (classDiscriminant, 1, "Get confidence ellipse "
			"area...", 0, 1, DO_Discriminant_getConfidenceEllipseArea);
		praat_addAction1 (classDiscriminant, 1, "Get ln(determinant_group)...",
			 0, 1, DO_Discriminant_getLnDeterminant_group);
		praat_addAction1 (classDiscriminant, 1, "Get ln(determinant_total)",
			0, 1, DO_Discriminant_getLnDeterminant_total);
			
	praat_addAction1 (classDiscriminant, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_addAction1 (classDiscriminant, 1, "Invert eigenvector...",
			0, 1, DO_Discriminant_invertEigenvector);
		praat_addAction1 (classDiscriminant, 0, "Align eigenvectors",
			0, 1, DO_Eigens_alignEigenvectors);

	praat_addAction1 (classDiscriminant, 0, EXTRACT_BUTTON, 0, 0, 0);
		praat_addAction1 (classDiscriminant, 1,
			"Extract pooled within-groups SSCP", 0, 1,
			DO_Discriminant_extractPooledWithinGroupsSSCP);
		praat_addAction1 (classDiscriminant, 1, "Extract within-group SSCP...",
			0, 1, DO_Discriminant_extractWithinGroupSSCP);
		praat_addAction1 (classDiscriminant, 1, "Extract between-groups SSCP",
			0, 1, DO_Discriminant_extractBetweenGroupsSSCP);
		praat_addAction1 (classDiscriminant, 1, "Extract group centroids",
			0, 1, DO_Discriminant_extractGroupCentroids);
		praat_addAction1 (classDiscriminant, 1, 
			"Extract group standard deviations", 0, 1,
			DO_Discriminant_extractGroupStandardDeviations);
		praat_addAction1 (classDiscriminant, 1, 
			"Extract group labels", 0, 1,
			DO_Discriminant_extractGroupLabels);
			
	praat_addAction1 (classDiscriminant , 0, "& TableOfReal: To ClassificationTable?", 0, 0, DO_hint_Discriminant_and_TableOfReal_to_ClassificationTable);
		
/*		praat_addAction1 (classDiscriminant, 1, "Extract coefficients...", 0, 1, DO_Discriminant_extractCoefficients);*/



	praat_Eigen_Matrix_project (classDiscriminant, classFormantFilter);
	praat_Eigen_Matrix_project (classDiscriminant, classBarkFilter);
	praat_Eigen_Matrix_project (classDiscriminant, classMelFilter);

	praat_addAction2 (classDiscriminant, 1, classPattern, 1,
		"To Categories...", 0, 0, DO_Discriminant_and_Pattern_to_Categories);
	praat_addAction2 (classDiscriminant, 1, classSSCP, 1,
		"Project", 0, 0, DO_Eigen_and_SSCP_project);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, 
		"Modify Discriminant", 0, 0, 0);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, 
		"Set group labels", 0, 0, DO_Discriminant_setGroupLabels);

	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, 
		"To Configuration...", 0, 0,
		DO_Discriminant_and_TableOfReal_to_Configuration);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1,
		"To ClassificationTable...", 0, 0,
		DO_Discriminant_and_TableOfReal_to_ClassificationTable);
        
	praat_addAction1 (classDTW, 0, "DTW help", 0, 0, DO_DTW_help);
	praat_addAction1 (classDTW, 0, "Draw", 0, 0, 0);
    praat_addAction1 (classDTW, 0, "Draw path...", 0, 0, DO_DTW_drawPath);
/*    praat_addAction1 (classDTW, 0, "Draw distances a path...", 0, 0, DO_DTW_drawDistancesAlongPath);*/
    praat_addAction1 (classDTW, 0, "Paint distances...", 0, 0,
		DO_DTW_paintDistances);
    praat_addAction1 (classDTW, 0, QUERY_BUTTON, 0, 0, 0);
    praat_addAction1 (classDTW, 1, "Get distance (weighted)", 0, 1, DO_DTW_getWeightedDistance);
	praat_addAction1 (classDTW, 1, "Get maximum consecutive steps...", 0, 1, DO_DTW_getMaximumConsecutiveSteps);
    praat_addAction1 (classDTW, 1, "Get time along path...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_DTW_getPathY);
    praat_addAction1 (classDTW, 1, "Get y time...", 0, 1, DO_DTW_getYTime);
    praat_addAction1 (classDTW, 1, "Get x time...", 0, 1, DO_DTW_getXTime);
		

    praat_addAction1 (classDTW, 0, "Analyse", 0, 0, 0);
    praat_addAction1 (classDTW, 0, "Find path...", 0, 0, DO_DTW_findPath);
	praat_addAction1 (classDTW, 0, "Find path (band)...", 0, 0, DO_DTW_pathFinder_band);
#ifdef INCLUDE_DTW_SLOPES 
	praat_addAction1 (classDTW, 0, "Find path (slopes)...", 0, 0, DO_DTW_pathFinder_slopes);
#endif
	praat_addAction1 (classDTW, 0, "To Polygon (band)...", 0, 0, DO_DTW_to_Polygon_band);
	praat_addAction1 (classDTW, 0, "To Polygon (slopes)...", 0, 0, DO_DTW_to_Polygon_slopes);
    praat_addAction1 (classDTW, 0, "To Matrix (distances)", 0, 0, DO_DTW_distancesToMatrix);
	praat_addAction1 (classDTW, 0, "Swap axes", 0, 0, DO_DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classTextGrid, 1, "To TextGrid (warp times)", 0, 0,
		DO_DTW_and_TextGrid_to_TextGrid);
	
	praat_Index_init (classStringsIndex);
    praat_addAction1 (classIndex, 0, "Index help", 0, 0, DO_Index_help);
	praat_addAction1 (classStringsIndex, 1, "Get class label...", 0, 0, DO_StringsIndex_getClassLabel);
    praat_addAction1 (classStringsIndex, 1, "Get class index...", 0, 0, DO_StringsIndex_getClassIndex);
    praat_addAction1 (classStringsIndex, 1, "Get label...", 0, 0, DO_StringsIndex_getLabel);
    praat_addAction1 (classIndex, 1, "Get index...", 0, 0, DO_Index_getIndex);
	praat_addAction1 (classStringsIndex, 1, "To Strings", 0, 0, DO_StringsIndex_to_Strings);
	
    praat_addAction1 (classExcitation, 0, "Synthesize", "To Formant...", 0, 0);
    praat_addAction1 (classExcitation, 0, "To Excitations", "Synthesize", 0,
		DO_Excitation_to_Excitations);

    praat_addAction1 (classExcitations, 0, "Modify", 0, 0, 0);
    praat_addAction1 (classExcitations, 0, "Formula...", 0, 0,
		DO_Excitations_formula);
    praat_addAction1 (classExcitations, 0, "Extract", 0, 0, 0);
    praat_addAction1 (classExcitations, 0, "Extract Excitation...", 0, 0,
		DO_Excitations_getItem);
    praat_addAction1 (classExcitations, 0, "Synthesize", 0, 0, 0);
    praat_addAction1 (classExcitations, 0, "Append", 0, 0,
		DO_Excitations_append);
    praat_addAction1 (classExcitations, 0, "Convert", 0, 0, 0);
    praat_addAction1 (classExcitations, 0, "To Pattern...", 0, 0,
		DO_Excitations_to_Pattern);
    praat_addAction1 (classExcitations, 0, "To TableOfReal", 0, 0,
		DO_Excitations_to_TableOfReal);
    
    praat_addAction2 (classExcitations, 1, classExcitation, 0,
		"Add to Excitations", 0, 0, DO_Excitations_addItem);
		

	praat_addAction1 (classFormantFilter, 0, "FormantFilter help", 0, 0,
		DO_FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, "Draw spectrum (slice)...", 
		"Draw filters...", 1, DO_FormantFilter_drawSpectrum);
	praat_addAction1 (classFormantFilter, 0, "Draw filter functions...", 
		"Draw filters...", 1, DO_FormantFilter_drawFilterFunctions);

	praat_addAction1 (classIntensity, 0, "To TextGrid (silences)...", "To IntensityTier (valleys)",
		0, DO_Intensity_to_TextGrid_detectSilences);
	
	praat_addAction1 (classISpline, 0, "ISpline help", 0, 0, DO_ISpline_help);
	praat_Spline_init (classISpline);

	praat_addAction1 (classLegendreSeries, 0, "LegendreSeries help", 0, 0,
		DO_LegendreSeries_help);
	praat_FunctionTerms_init (classLegendreSeries);
	praat_addAction1 (classLegendreSeries, 0, "To Polynomial", "Analyse", 0,
		DO_LegendreSeries_to_Polynomial);
		
	praat_addAction1 (classLongSound, 0, "Append to existing sound file...", 0, 0,
		DO_LongSounds_appendToExistingSoundFile);
	praat_addAction1 (classSound, 0, "Append to existing sound file...", 0, 0,
		DO_LongSounds_appendToExistingSoundFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, "Append to existing sound file...",
		0, 0, DO_LongSounds_appendToExistingSoundFile);
		
	praat_addAction1 (classLongSound, 2, "Write to stereo AIFF file...", 
		"Write to NIST file...", 1, DO_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, "Write to stereo AIFC file...", 
		"Write to stereo AIFF file...", 1, DO_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, "Write to stereo WAV file...", 
		"Write to stereo AIFC file...", 1, DO_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, "Write to stereo NeXt/Sun file...", 
		"Write to stereo WAV file...", 1, DO_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, "Write to stereo NIST file...", 
		"Write to stereo NeXt/Sun file...", 1, DO_LongSounds_writeToStereoNistFile);

	praat_addAction1 (classMatrix, 0, "Scatter plot...", "Paint cells...", 1,
		DO_Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, "Draw as squares...", "Scatter plot...", 1,
		DO_Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, "Draw distribution...",
		"Draw as squares...", 1, DO_Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, "Draw cumulative distribution...",
		"Draw distribution...", 1, DO_Matrix_drawCumulativeDistribution);
	praat_addAction1 (classMatrix, 0, "Transpose", "Synthesize", 0,
		DO_Matrix_transpose);
	praat_addAction1 (classMatrix, 0, "Solve equation...", "Analyse", 0,
		DO_Matrix_solveEquation);
    praat_addAction1 (classMatrix, 0, "To Pattern...", "To VocalTract", 1,
		DO_Matrix_to_Pattern);
	praat_addAction1 (classMatrix, 0, "To Activation", "To Pattern...", 1,
		DO_Matrix_to_Activation);
	praat_addAction1 (classMatrix, 2, "To DTW...", "To ParamCurve", 1, DO_Matrices_to_DTW);

	praat_addAction2 (classMatrix, 1, classCategories, 1, "To TableOfReal", 
		0, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classMelFilter, 0, "MelFilter help", 0, 0,
		DO_MelFilter_help);
	praat_FilterBank_all_init (classMelFilter);
	praat_addAction1 (classMelFilter, 0, "Draw spectrum (slice)...", 
		"Draw filters...", 1, DO_MelFilter_drawSpectrum);
	praat_addAction1 (classMelFilter, 0, "Draw filter functions...", 
		"Draw filters...", 1, DO_MelFilter_drawFilterFunctions);
	praat_addAction1 (classMelFilter, 0, "To MFCC...",
		0, 0, DO_MelFilter_to_MFCC);

	praat_addAction1 (classMFCC, 0, "MFCC help", 0, 0,
		DO_MFCC_help);
	praat_CC_init (classMFCC);
	praat_addAction1 (classMFCC, 0, "To MelFilter...", 0, 0,
		DO_MFCC_to_MelFilter);
	
	praat_addAction1 (classMSpline, 0, "MSpline help", 0, 0, DO_MSpline_help);
	praat_Spline_init (classMSpline);
    
    praat_addAction1 (classPattern, 0, "Draw", 0, 0, 0);
    praat_addAction1 (classPattern, 0, "Draw...", 0, 0, DO_Pattern_draw);
	praat_addAction1 (classPattern, 0, MODIFY_BUTTON, 0, 0, 0);
    praat_addAction1 (classPattern, 0, "Formula...", 0, 1, DO_Pattern_formula);
    praat_addAction1 (classPattern, 0, "Set value...", 0, 1, DO_Pattern_setValue);
    praat_addAction1 (classPattern, 0, "To Matrix", 0, 0, DO_Pattern_to_Matrix);
    
    praat_addAction2 (classPattern, 1, classCategories, 1, "To TableOfReal",
	    0, 0, DO_Matrix_Categories_to_TableOfReal);

    praat_addAction2 (classPattern, 1, classCategories, 1, "To Discriminant",
	    0, 0, DO_Pattern_and_Categories_to_Discriminant);

	praat_addAction1 (classPCA, 0, "PCA help", 0, 0, DO_PCA_help);
	praat_addAction1 (classPCA, 0, DRAW_BUTTON, 0, 0, 0);
		praat_Eigen_draw_init (classPCA);
	praat_addAction1 (classPCA, 0, QUERY_BUTTON, 0, 0, 0);
		praat_Eigen_query_init (classPCA);
		praat_addAction1 (classPCA, 1, "-- pca --", 0, 1, 0);
		praat_addAction1 (classPCA, 1, "Get equality of eigenvalues...", 
			0, 1, DO_PCA_getEqualityOfEigenvalues);
		praat_addAction1 (classPCA, 1, "Get fraction variance accounted "
			"for...", 0, 1, DO_PCA_getFractionVAF);
		praat_addAction1 (classPCA, 1, "Get number of components (VAF)...", 
			0, 1, DO_PCA_getNumberOfComponentsVAF);
		praat_addAction1 (classPCA, 2, "Get angle between pc1-pc2 planes", 0, 1, DO_PCAs_getAngleBetweenPc1Pc2Plane_degrees);
	praat_addAction1 (classPCA, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_addAction1 (classPCA, 1, "Invert eigenvector...", 0, 1,
			DO_PCA_invertEigenvector);
		praat_addAction1 (classPCA, 0, "Align eigenvectors",
			0, 1, DO_Eigens_alignEigenvectors);
	praat_addAction1 (classPCA, 2, "To Procrustes...", 0, 0, DO_PCAs_to_Procrustes);
	praat_addAction1 (classPCA, 0, "To TableOfReal (reconstruct 1)...", 0, 0, 
		DO_PCA_to_TableOfReal_reconstruct1);
	praat_addAction1 (classPCA, 0, "& TableOfReal: To Configuration?", 0, 0, DO_hint_PCA_and_TableOfReal_to_Configuration);
	praat_addAction1 (classPCA, 0, "& Configuration (reconstruct)?", 0, 0, DO_hint_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction1 (classPCA, 0, "& Covariance: Project?", 0, 0, DO_hint_PCA_and_Covariance_Project);
	praat_addAction2 (classPCA, 1, classConfiguration, 1, 
		"To TableOfReal (reconstruct)", 0, 0, DO_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction2 (classPCA, 1, classSSCP, 1,
		"Project", 0, 0, DO_Eigen_and_SSCP_project);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1,
		"To Configuration...", 0, 0, DO_PCA_and_TableOfReal_to_Configuration);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1,
		"Get fraction variance...", 0, 0, DO_PCA_and_TableOfReal_getFractionVariance);
	praat_addAction2 (classPCA, 1, classCovariance, 1,
		"Project", 0, 0, DO_Eigen_and_Covariance_project);

	praat_Eigen_Matrix_project (classPCA, classFormantFilter);
	praat_Eigen_Matrix_project (classPCA, classBarkFilter);
	praat_Eigen_Matrix_project (classPCA, classMelFilter);
	
	praat_addAction1 (classPermutation, 0, "Permutation help", 0, 0, DO_Permutation_help);
	praat_addAction1 (classPermutation, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, "Get number of elements", 0, 1, DO_Permutation_getNumberOfElements);
	praat_addAction1 (classPermutation, 1, "Get value...", 0, 1, DO_Permutation_getValueAtIndex);
	praat_addAction1 (classPermutation, 1, "Get index...", 0, 1, DO_Permutation_getIndexAtValue);
	praat_addAction1 (classPermutation, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, "Sort", 0, 1, DO_Permutation_sort);
	praat_addAction1 (classPermutation, 1, "Swap blocks...", 0, 1, DO_Permutation_swapBlocks);
	praat_addAction1 (classPermutation, 1, "Swap one from range...", 0, 1, DO_Permutation_swapOneFromRange);
	praat_addAction1 (classPermutation, 1, "Permute randomly...", 0, 0, DO_Permutation_permuteRandomly);
	praat_addAction1 (classPermutation, 1, "Permute randomly (blocks)...", 0, 0, DO_Permutation_permuteBlocksRandomly);
	praat_addAction1 (classPermutation, 1, "Interleave...", 0, 0, DO_Permutation_interleave);
	praat_addAction1 (classPermutation, 1, "Rotate...", 0, 0, DO_Permutation_rotate);
	praat_addAction1 (classPermutation, 1, "Reverse...", 0, 0, DO_Permutation_reverse);
	praat_addAction1 (classPermutation, 1, "Invert", 0, 0, DO_Permutation_invert);
	praat_addAction1 (classPermutation, 0, "Multiply", 0, 0, DO_Permutations_multiply);

	praat_addAction1 (classPitch, 2, "To DTW...", "To PointProcess", praat_HIDDEN, DO_Pitches_to_DTW);

	praat_addAction1 (classPitchTier, 0, "To Pitch...", "To Sound (sine)...", 1, DO_PitchTier_to_Pitch);
	praat_addAction1 (classPolygon, 0, "Translate...", "Modify", 0,
		DO_Polygon_translate);
	praat_addAction1 (classPolygon, 0, "Rotate...", "Translate...", 0,
		DO_Polygon_rotate);
	praat_addAction1 (classPolygon, 0, "Scale...", "Rotate...", 0,
		DO_Polygon_scale);
	praat_addAction1 (classPolygon, 0, "Reverse X", "Scale...", 0,
		DO_Polygon_reverseX);
	praat_addAction1 (classPolygon, 0, "Reverse Y", "Reverse X", 0,
		DO_Polygon_reverseY);

	praat_addAction2 (classPolygon, 1, classCategories, 1, "Draw...", 
		0, 0, DO_Polygon_Categories_draw);

	praat_addAction1 (classPolynomial, 0, "Polynomial help", 0, 0,
		DO_Polynomial_help);
	praat_FunctionTerms_init (classPolynomial);
		praat_addAction1 (classPolynomial, 0, "-- area --",
			"Get x of maximum...", 1, 0);
		praat_addAction1 (classPolynomial, 1, "Get area...", "-- area --",
			1, DO_Polynomial_getArea);
		praat_addAction1 (classPolynomial, 0, "-- monic --", 
			"Set coefficient...", 1, 0);
		praat_addAction1 (classPolynomial, 0, "Scale coefficients (monic)",
			"-- monic --", 1, DO_Polynomial_scaleCoefficients_monic);
	praat_addAction1 (classPolynomial, 1, "Get value (complex)...", "Get value...",
		1, DO_Polynomial_evaluate_z);
	praat_addAction1 (classPolynomial, 0, "To Spectrum...", "Analyse",
		0, DO_Polynomial_to_Spectrum);
	praat_addAction1 (classPolynomial, 0, "To Roots", 0, 0,
		DO_Polynomial_to_Roots);
	praat_addAction1 (classPolynomial, 0, "To Polynomial (derivative)", 0, 0,
		DO_Polynomial_getDerivative);
	praat_addAction1 (classPolynomial, 0, "To Polynomial (primitive)", 0, 0,
		DO_Polynomial_getPrimitive);
	praat_addAction1 (classPolynomial, 0, "Scale x...", 0, 0,
		DO_Polynomial_scaleX);
	praat_addAction1 (classPolynomial, 2, "Multiply", 0, 0,
		DO_Polynomials_multiply);
	praat_addAction1 (classPolynomial, 2, "Divide...", 0, 0,
		DO_Polynomials_divide);

	praat_addAction1 (classRoots, 1, "Roots help", 0, 0, DO_Roots_help);
	praat_addAction1 (classRoots, 1, "Draw...", 0, 0, DO_Roots_draw);
	praat_addAction1 (classRoots, 1, QUERY_BUTTON, 0, 0, 0);
		praat_addAction1 (classRoots, 1, "Get number of roots", 0, 1,
			DO_Roots_getNumberOfRoots);
		praat_addAction1 (classRoots, 1, "-- roots --", 0, 1, 0);
		praat_addAction1 (classRoots, 1, "Get root...", 0, 1, DO_Roots_getRoot);
		praat_addAction1 (classRoots, 1, "Get real part of root...", 0, 1,
			DO_Roots_getRealPartOfRoot);
		praat_addAction1 (classRoots, 1, "Get imaginary part of root...", 0, 1,
			DO_Roots_getImaginaryPartOfRoot);
	praat_addAction1 (classRoots, 1, MODIFY_BUTTON, 0, 0, 0);
		praat_addAction1 (classRoots, 1, "Set root...", 0, 1, DO_Roots_setRoot);
	praat_addAction1 (classRoots, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classRoots, 0, "To Spectrum...", 0, 0,
		DO_Roots_to_Spectrum);

	praat_addAction2 (classRoots, 1, classPolynomial, 1,"Polish roots", 0, 0,
		DO_Roots_and_Polynomial_polish);

	praat_addAction1 (classSound, 1, "Write to raw 16-bit Big Endian file...",
		0, 0, DO_Sound_writeToRawFileBE);
	praat_addAction1 (classSound, 1, "Write to raw 16-bit Little Endian "
		"file...", 0, 0, DO_Sound_writeToRawFileLE);
		
	praat_addAction1 (classSound, 0, "To TextGrid (silences)...", "To IntervalTier",
		1, DO_Sound_to_TextGrid_detectSilences);
		
	praat_addAction1 (classSound, 0, "To Pitch (shs)...", "To Pitch (cc)...",
		1, DO_Sound_to_Pitch_shs);
	praat_addAction1 (classSound, 0, "To Pitch (SPINET)...",
		"To Pitch (cc)...", 1, DO_Sound_to_Pitch_SPINET);

	praat_addAction1 (classSound, 0, "To FormantFilter...", 
		"To Cochleagram (edb)...", 1, DO_Sound_to_FormantFilter);
		
	praat_addAction1 (classSound, 0, "To BarkFilter...", 
		"To FormantFilter...", 1, DO_Sound_to_BarkFilter);
		
	praat_addAction1 (classSound, 0, "To MelFilter...", 
		"To BarkFilter...", 1, DO_Sound_to_MelFilter);
		
	praat_addAction1 (classSound, 0, "Filter (gammatone)...",
		"Filter (formula)...", 1, DO_Sound_filterByGammaToneFilter4);

	praat_addAction1 (classSound, 0, "Change gender...",
		"Deepen band modulation...", 1, DO_Sound_changeGender);
	
	praat_addAction1 (classSound, 0, "Change speaker...",
		"Deepen band modulation...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_changeSpeaker);

	praat_addAction2 (classSound, 1, classPitch, 1, "To FormantFilter...",
		0, 0, DO_Sound_and_Pitch_to_FormantFilter);

	praat_addAction2 (classSound, 1, classPitch, 1, "Change gender...",
		0, 0, DO_Sound_and_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, "Change speaker...",
		0, praat_HIDDEN, DO_Sound_and_Pitch_changeSpeaker);

    praat_addAction1 (classSpectrogram, 2, "To DTW...", 
		"To Spectrum (slice)...", 0, DO_Spectrograms_to_DTW);

	praat_addAction1 (classSpectrum, 0, "Draw phases...",
		"Draw (log freq)...", 1, DO_Spectrum_drawPhases);
	praat_addAction1 (classSpectrum, 0, "Conjugate",
		"Formula...", praat_HIDDEN | praat_DEPTH_1, DO_Spectrum_conjugate);
	praat_addAction1 (classSpectrum, 2, "Multiply", "To Sound (fft)", 
		praat_HIDDEN, DO_Spectra_multiply);
	praat_addAction1 (classSpectrum, 0, "To Matrix (unwrap)",
		"To Matrix", 0, DO_Spectrum_unwrap);
	praat_addAction1 (classSpectrum, 0, "To Cepstrum",
		"To Spectrogram", 0, DO_Spectrum_to_Cepstrum);

	praat_addAction1 (classSSCP, 0, "SSCP help", 0, 0, DO_SSCP_help);
	praat_TableOfReal_init2 (classSSCP);
	praat_removeAction (classSSCP, NULL, NULL, L"Append");
	praat_addAction1 (classSSCP, 0, "Draw sigma ellipse...",
		DRAW_BUTTON, 1, DO_SSCP_drawSigmaEllipse);
	praat_addAction1 (classSSCP, 0, "Draw confidence ellipse...", 
		DRAW_BUTTON, 1, DO_SSCP_drawConfidenceEllipse);
	praat_SSCP_query_init (classSSCP);
	praat_addAction1 (classSSCP, 1, "Get diagonality (bartlett)...",
		"Get ln(determinant)", 1, DO_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classSSCP, 1, "Get total variance", 
		"Get diagonality (bartlett)...", 1, DO_SSCP_getTotalVariance);
	praat_addAction1 (classSSCP, 1, "Get sigma ellipse area...", 
		"Get total variance", 1, DO_SSCP_getConcentrationEllipseArea);
	praat_addAction1 (classSSCP, 1, "Get confidence ellipse area...",
		"Get sigma ellipse area...", 1, DO_SSCP_getConfidenceEllipseArea);
	praat_addAction1 (classSSCP, 1, "Get fraction variation...",
		"Get confidence ellipse area...", 1, DO_SSCP_getFractionVariation);
	praat_SSCP_extract_init (classSSCP);
	praat_addAction1 (classSSCP, 0, "To PCA", 0, 0, DO_SSCP_to_PCA);
	praat_addAction1 (classSSCP, 0, "To Correlation", 0, 0,
		DO_SSCP_to_Correlation);
	praat_addAction1 (classSSCP, 0, "To Covariance...", 0, 0,
		DO_SSCP_to_Covariance);
	
	praat_addAction1 (classStrings, 0, "To Categories", 0, 0, DO_Strings_to_Categories);
	praat_addAction1 (classStrings, 0, "Append", 0, 0, DO_Strings_append);
	praat_addAction1 (classStrings, 1, "Set string...", "Genericize",
		 0, DO_Strings_setString);
	praat_addAction1 (classStrings, 0, "Change...", "Set string...",
		 0, DO_Strings_change);
	praat_addAction1 (classStrings, 0, "Extract part...", "Change...",
		 0, DO_Strings_extractPart);
	praat_addAction1 (classStrings, 0, "To Permutation...", "To Distributions",
		 0, DO_Strings_to_Permutation);

	praat_addAction1 (classSVD, 0, "To TableOfReal...", 0, 0, DO_SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, "Extract left singular vectors", 0, 0, DO_SVD_extractLeftSingularVectors);
	praat_addAction1 (classSVD, 0, "Extract right singular vectors", 0, 0, DO_SVD_extractRightSingularVectors);
	praat_addAction1 (classSVD, 0, "Extract singular values", 0, 0, DO_SVD_extractSingularValues);

	
	praat_addAction1 (classTableOfReal, 0, "Append columns", 
		"Append", 1, DO_TableOfReal_appendColumns);
	praat_addAction1 (classTableOfReal, 0, "Multivariate statistics -",
		0, 0, 0);
	praat_addAction1 (classTableOfReal, 0, "To Discriminant", 0, 1,
		DO_TableOfReal_to_Discriminant);
	praat_addAction1 (classTableOfReal, 0, "To PCA", 0, 1,
		DO_TableOfReal_to_PCA);
	praat_addAction1 (classTableOfReal, 0, "To SSCP...", 0, 1,
		DO_TableOfReal_to_SSCP);
	praat_addAction1 (classTableOfReal, 0, "To Covariance", 0, 1,
		DO_TableOfReal_to_Covariance);
	praat_addAction1 (classTableOfReal, 0, "To Correlation", 0, 1,
		DO_TableOfReal_to_Correlation);
	praat_addAction1 (classTableOfReal, 0, "To Correlation (rank)", 0, 1,
		DO_TableOfReal_to_Correlation_rank);
	praat_addAction1 (classTableOfReal, 0, "To CCA...", 0, 1,
		DO_TableOfReal_to_CCA);
	praat_addAction1 (classTableOfReal, 0, "To TableOfReal (means by row labels)...", 0, 1,
		DO_TableOfReal_meansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, "To TableOfReal (medians by row labels)...", 0, 1,
		DO_TableOfReal_mediansByRowLabels);
	
	praat_addAction1 (classTableOfReal, 0, "-- configurations --", 0, 1, 0);
	praat_addAction1 (classTableOfReal, 0, "To Configuration (pca)...",
		0, 1, DO_TableOfReal_to_Configuration_pca);
	praat_addAction1 (classTableOfReal, 0, "To Configuration (lda)...", 0, 1,
		DO_TableOfReal_to_Configuration_lda);
	praat_addAction1 (classTableOfReal, 1, "To Pattern and "
		"Categories...", "To Matrix", 1, DO_TableOfReal_to_Pattern_and_Categories);
	praat_addAction1 (classTableOfReal, 1, "Split into Pattern and Categories...",
		"To Pattern and Categories...",
		praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_to_Pattern_and_Categories);
	praat_addAction1 (classTableOfReal, 0, "To Permutation (sort row labels)",
		"To Matrix", 1, DO_TableOfReal_to_Permutation_sortRowlabels);

	praat_addAction1 (classTableOfReal, 1, "To SVD", 
		0, praat_HIDDEN, DO_TableOfReal_to_SVD);
	praat_addAction1 (classTableOfReal, 2, "To GSVD",
		0, praat_HIDDEN, DO_TablesOfReal_to_GSVD);
	praat_addAction1 (classTableOfReal, 2, "To Eigen (gsvd)",
		0, praat_HIDDEN, DO_TablesOfReal_to_Eigen_gsvd);
 	praat_addAction1 (classTableOfReal, 0, "To TableOfReal (cholesky)...",
		0, praat_HIDDEN, DO_TableOfReal_choleskyDecomposition);

	praat_addAction1 (classTableOfReal, 0, "-- scatter plots --", 
		"Draw top and bottom lines...", 1, 0);
	praat_addAction1 (classTableOfReal, 0, "Draw scatter plot...", 
		"-- scatter plots --", 1, DO_TableOfReal_drawScatterPlot);
	praat_addAction1 (classTableOfReal, 0, "Draw scatter plot matrix...", 
		"Draw scatter plot...", 1, DO_TableOfReal_drawScatterPlotMatrix);
	praat_addAction1 (classTableOfReal, 0, "Draw box plots...",
		"Draw scatter plot matrix...", 1, DO_TableOfReal_drawBoxPlots);
	praat_addAction1 (classTableOfReal, 0, "Draw biplot...", 
		"Draw box plots...", 1, DO_TableOfReal_drawBiplot);
		
	praat_addAction2 (classStrings, 1, classPermutation, 1, "Permute strings",
		0, 0, DO_Strings_and_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, "Permute rows",
		0, 0, DO_TableOfReal_and_Permutation_permuteRows);

	praat_addAction1 (classTextGrid, 0, "Extend time...", "Scale times...", 2, DO_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, "Set tier name...", "Remove tier...", 1, DO_TextGrid_setTierName);
			praat_addAction1 (classTextGrid, 0, "Replace interval text...", "Set interval text...", 2, DO_TextGrid_replaceIntervalTexts);
			praat_addAction1 (classTextGrid, 0, "Replace point text...", "Set point text...", 2, DO_TextGrid_replacePointTexts);

    INCLUDE_LIBRARY (praat_uvafon_MDS_init)
	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)
}

/* End of file praat_David.c */
