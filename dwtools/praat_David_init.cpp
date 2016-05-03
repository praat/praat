/* praat_David_init.cpp
 *
 * Copyright (C) 1993-2016 David Weenink, 2015 Paul Boersma
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
 djmw 20071011 REQUIRE requires U"".
 djmw 20071202 Melder_warning<n>
 djmw 20080521 Confusion_drawAsnumbers
 djmw 20090109 KlattGrid formulas for formant
 djmw 20090708 KlattTable <-> Table
 djmw 20090822 Thing_recognizeClassesByName: added classCepstrum, classIndex, classKlattTable
 djmw 20090914 Excitation to Excitations crashed because of nullptr reference
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
#include "ComplexSpectrogram.h"
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
#include "FilterBank.h"
#include "Formula.h"
#include "FormantGridEditor.h"
#include "DataModeler.h"
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
#include "Permutation_and_Index.h"
#include "Pitch_extensions.h"
#include "Sound_and_Spectrogram_extensions.h"
#include "Sound_to_Pitch2.h"
#include "Sound_to_SPINET.h"
#include "TableOfReal_and_SVD.h"
#include "VowelEditor.h"

#undef iam
#define iam iam_LOOP

static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *DRAW_BUTTON    = U"Draw -";
static const char32 *MODIFY_BUTTON  = U"Modify -";
static const char32 *EXTRACT_BUTTON = U"Extract -";

void praat_TimeFunction_query_init (ClassInfo klas);
void praat_TimeFrameSampled_query_init (ClassInfo klas);
void praat_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init2 (ClassInfo klas);
void praat_SSCP_as_TableOfReal_init (ClassInfo klas);

void praat_CC_init (ClassInfo klas);
void DTW_constraints_addCommonFields (UiForm dia);
void DTW_constraints_getCommonFields (UiForm dia, int *begin, int *end, int *slope);
void praat_BandFilterSpectrogram_query_init (ClassInfo klas);
int praat_Fon_formula (UiForm dia, Interpreter interpreter);
void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas);

#undef INCLUDE_DTW_SLOPES

/********************** Activation *******************************************/

FORM (Activation_formula, U"Activation: Formula", nullptr)
	LABEL (U"label", U"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }}")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	praat_Fon_formula (dia, interpreter);
END

DIRECT (Activation_to_Matrix)
	LOOP {
		iam (Activation);
		autoMatrix thee = Activation_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END

/********************** BandFilterSpectrogram *******************************************/

FORM (BandFilterSpectrogram_drawFrequencyScale, U"", U"")
	REAL (U"left Horizontal frequency range (Hz)", U"0.0")
	REAL (U"right Horizontal frequency range (Hz)", U"0.0")
	REAL (U"left Vertical frequency range (mel)", U"0.0")
	REAL (U"right Vertical frequency range (mel)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BandFilterSpectrogram);
		BandFilterSpectrogram_drawFrequencyScale (me, GRAPHICS, GET_REAL (U"left Horizontal frequency range"), 
			GET_REAL (U"right Horizontal frequency range"),
			GET_REAL (U"left Vertical frequency range"), GET_REAL (U"right Vertical frequency range"),
			GET_INTEGER (U"Garnish"));
	}
END

/********************** BarkFilter *******************************************/

DIRECT (BarkFilter_help)
	Melder_help (U"BarkFilter");
END

DIRECT (BarkSpectrogram_help)
	Melder_help (U"BarkSpectrogram");
END

FORM (BarkFilter_drawSpectrum, U"BarkFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...")
	POSITIVE (U"Time (s)", U"0.1")
	REAL (U"left Frequency range (Bark)", U"0.0")
	REAL (U"right Frequency range (Bark)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BarkFilter);
		FilterBank_drawTimeSlice (me, GRAPHICS, GET_REAL (U"Time"), GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), U"Barks", GET_INTEGER (U"Garnish"));
	}
END

FORM (BarkFilter_drawSekeyHansonFilterFunctions, U"BarkFilter: Draw filter functions", U"FilterBank: Draw filter functions...")
	INTEGER (U"left Filter range", U"0")
	INTEGER (U"right Filter range", U"0")
	RADIO (U"Frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	BOOLEAN (U"Amplitude scale in dB", true)
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BarkFilter);
		BarkFilter_drawSekeyHansonFilterFunctions (me, GRAPHICS, GET_INTEGER (U"Frequency scale"),
			GET_INTEGER (U"left Filter range"), GET_INTEGER (U"right Filter range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Amplitude scale in dB"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (BarkSpectrogram_drawSekeyHansonAuditoryFilters, U"BarkSpectrogram: Draw Sekey-Hanson auditory filters", U"BarkSpectrogram: Draw Sekey-Hanson auditory filters...")
	INTEGER (U"left Filter range", U"0")
	INTEGER (U"right Filter range", U"0")
	RADIO (U"Frequency scale", 2)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	BOOLEAN (U"Amplitude scale in dB", 1)
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BarkSpectrogram);
		bool xIsHertz = ( GET_INTEGER (U"Frequency scale") == 1 );
		BarkSpectrogram_drawSekeyHansonFilterFunctions (me, GRAPHICS, xIsHertz,
			GET_INTEGER (U"left Filter range"), GET_INTEGER (U"right Filter range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Amplitude scale in dB"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (BarkFilter_paint, U"FilterBank: Paint", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range (bark)", U"0.0")
	REAL (U"right Frequency range (bark)", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", false)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		FilterBank_paint ((FilterBank) me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
		GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (BarkFilter_to_BarkSpectrogram)
	LOOP {
		iam (BarkFilter);
		praat_new (BarkFilter_to_BarkSpectrogram (me), my name);
	}
END

DIRECT (MelFilter_to_MelSpectrogram)
	LOOP {
		iam (MelFilter);
		praat_new (MelFilter_to_MelSpectrogram (me), my name);
	}
END

DIRECT (FormantFilter_to_Spectrogram)
	LOOP {
		iam (FormantFilter);
		praat_new (FormantFilter_to_Spectrogram (me), my name);
	}
END

/********************** Categories  ****************************************/

FORM (Categories_append, U"Categories: Append 1 category", U"Categories: Append 1 category...")
	SENTENCE (U"Category", U"")
	OK
DO
	LOOP {
		iam (Categories);
		OrderedOfString_append (me, GET_STRING (U"Category"));
	}
END

DIRECT (Categories_edit)
	if (theCurrentPraatApplication -> batch) {
		Melder_throw (U"Cannot edit a Categories from batch.");
	} else {
		LOOP {
			iam (Categories);
			autoCategoriesEditor editor = CategoriesEditor_create (my name, me);
			praat_installEditor (editor.get(), IOBJECT);
			editor.releaseToUser();
		}
	}
END

DIRECT (Categories_getNumberOfCategories)
	LOOP {
		iam (Categories);
		Melder_information (my size, U" categories");
	}
END

DIRECT (Categories_getNumberOfDifferences)
	Categories c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);

	long numberOfDifferences = OrderedOfString_getNumberOfDifferences (c1, c2);
	if (numberOfDifferences < 0) {
		Melder_information (U"-1 (undefined: number of elements differ!)");
	} else {
		Melder_information (numberOfDifferences, U" differences");
	}
END

DIRECT (Categories_getFractionDifferent)
	Categories c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	Melder_information (OrderedOfString_getFractionDifferent (c1, c2));
END

DIRECT (Categories_difference)
	Categories c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	long n;
	double fraction;
	OrderedOfString_difference (c1, c2, &n, &fraction);
	Melder_information (n, U" differences");
END

DIRECT (Categories_selectUniqueItems)
	LOOP {
		iam (Categories);
		autoCategories thee = Categories_selectUniqueItems (me);
		praat_new (thee.move(), my name, U"_uniq");
	}
END

DIRECT (Categories_to_Confusion)
	Categories c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	autoConfusion thee = Categories_to_Confusion (c1, c2);
	praat_new (thee.move(), Thing_getName (c1), U"_", Thing_getName (c2));
END

DIRECT (Categories_to_Strings)
	LOOP {
		iam (Categories);
		autoStrings thee = Categories_to_Strings (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Categories_join)
	Categories c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Categories);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	autoOrderedOfString thee = OrderedOfString_joinItems (c1, c2);
	praat_new (thee.move(), c1 -> name, U"_", c2 -> name);
END

DIRECT (Categories_permuteItems)
	LOOP {
		iam (Collection);
		autoCollection thee = Collection_permuteItems (me);
		praat_new (thee.move(), my name, U"_perm");
	}
END

/***************** CC ****************************************/

FORM (CC_getNumberOfCoefficients, U"Get number of coefficients", nullptr)
	NATURAL (U"Frame number", U"1")
	OK
DO
	LOOP {
		iam (CC);
		long numberOfCoefficients = CC_getNumberOfCoefficients (me, GET_INTEGER (U"Frame number"));
		Melder_information (numberOfCoefficients);
	}
END

FORM (CC_getValue, U"CC: Get value", U"CC: Get value...")
	REAL (U"Time (s)", U"0.1")
	NATURAL (U"Index", U"1")
	OK
DO
	LOOP {
		iam (CC); // ?? generic
		Melder_informationReal (CC_getValue (me, GET_REAL (U"Time"), GET_INTEGER (U"Index")), nullptr);
	}
END

FORM (CC_getValueInFrame, U"CC: Get value in frame", U"CC: Get value in frame...")
	NATURAL (U"Frame number", U"1")
	NATURAL (U"Index", U"1")
	OK
DO
	LOOP {
		iam (CC); // ?? generic
		Melder_informationReal (CC_getValueInFrame (me, GET_INTEGER (U"Frame number"), GET_INTEGER (U"Index")), nullptr);
	}
END

FORM (CC_getC0ValueInFrame, U"CC: Get c0 value in frame", U"CC: Get c0 value in frame...")
	NATURAL (U"Frame number", U"1")
	OK
DO
	LOOP {
		iam (CC); // ?? generic
		Melder_informationReal (CC_getC0ValueInFrame (me, GET_INTEGER (U"Frame number")), nullptr);
	}
END

FORM (CC_paint, U"CC: Paint", U"CC: Paint...")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	INTEGER (U"From coefficient", U"0")
	INTEGER (U"To coefficient", U"0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (CC);
		CC_paint (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"From coefficient"), GET_INTEGER (U"To coefficient"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END

FORM (CC_drawC0, U"CC: Draw c0", U"CC: Draw c0...")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (CC);
		CC_drawC0 (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (CCs_to_DTW, U"CC: To DTW", U"CC: To DTW...")
	LABEL (U"", U"Distance  between cepstral coefficients")
	REAL (U"Cepstral weight", U"1.0")
	REAL (U"Log energy weight", U"0.0")
	REAL (U"Regression weight", U"0.0")
	REAL (U"Regression weight log energy", U"0.0")
	REAL (U"Regression coefficients window (s)", U"0.056")
	DTW_constraints_addCommonFields (dia);
	OK
DO
	CC c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (CC);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, &begin, &end, &slope);
    autoDTW thee = CCs_to_DTW (c1, c2, GET_REAL (U"Cepstral weight"), GET_REAL (U"Log energy weight"),
        GET_REAL (U"Regression weight"), GET_REAL (U"Regression weight log energy"),
        GET_REAL (U"Regression coefficients window"));
    DTW_findPath (thee.get(), begin, end, slope);
	praat_new (thee.move(), U"");
END

DIRECT (CC_to_Matrix)
	LOOP {
		iam (CC);
		autoMatrix thee = CC_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END

/******************* class CCA ********************************/

FORM (CCA_drawEigenvector, U"CCA: Draw eigenvector", U"Eigen: Draw eigenvector...")
	OPTIONMENU (U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	INTEGER (U"Eigenvector number", U"1")
	LABEL (U"", U"Multiply by eigenvalue?")
	BOOLEAN (U"Component loadings", false)
	LABEL (U"", U"Select part of the eigenvector:")
	INTEGER (U"left Element range", U"0")
	INTEGER (U"right Element range", U"0")
	REAL (U"left Amplitude range", U"-1.0")
	REAL (U"right Amplitude range", U"1.0")
	POSITIVE (U"Mark size (mm)", U"1.0")
	SENTENCE (U"Mark string (+xo.)", U"+")
	BOOLEAN (U"Connect points", true)
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (CCA);
		CCA_drawEigenvector (me, GRAPHICS, GET_INTEGER (U"X or Y"), GET_INTEGER (U"Eigenvector number"),
			GET_INTEGER (U"left Element range"), GET_INTEGER (U"right Element range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Component loadings"), GET_REAL (U"Mark size"),
			GET_STRING (U"Mark string"), GET_INTEGER (U"Connect points"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (CCA_getNumberOfCorrelations)
	LOOP {
		iam (CCA);
		Melder_information (my numberOfCoefficients);
	}
END

FORM (CCA_getCorrelationCoefficient, U"CCA: Get canonical correlation coefficient", U"CCA: Get canonical correlation coefficient")
	NATURAL (U"Coefficient number", U"1")
	OK
DO
	LOOP {
		iam (CCA);
		Melder_information (CCA_getCorrelationCoefficient (me, GET_INTEGER (U"Coefficient number")));
	}
END

FORM (CCA_getEigenvectorElement, U"CCA: Get eigenvector element", U"Eigen: Get eigenvector element...")
	OPTIONMENU (U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	NATURAL (U"Eigenvector number", U"1")
	NATURAL (U"Element number", U"1")
	OK
DO
	LOOP {
		iam (CCA);
		Melder_information (CCA_getEigenvectorElement (me, GET_INTEGER (U"X or Y"),
			GET_INTEGER (U"Eigenvector number"), GET_INTEGER (U"Element number")));
	}
END

FORM (CCA_getZeroCorrelationProbability, U"CCA: Get zero correlation probability", U"CCA: Get zero correlation probability...")
	NATURAL (U"Coefficient number", U"1")
	OK
DO
	LOOP {
		iam (CCA);
		double p, chisq, df;
		CCA_getZeroCorrelationProbability (me, GET_INTEGER (U"Coefficient number"), & p, & chisq, & df);
		Melder_information (p, U" (=probability for chisq = ", chisq,
			U" and ndf = ", df, U")");
	}
END

DIRECT (CCA_and_Correlation_factorLoadings)
	CCA me = FIRST (CCA);
	Correlation thee = FIRST (Correlation);
	autoTableOfReal result = CCA_and_Correlation_factorLoadings (me, thee);
	praat_new (result.move(), Thing_getName (me), U"_loadings");
END

FORM (CCA_and_Correlation_getVarianceFraction, U"CCA & Correlation: Get variance fraction", U"CCA & Correlation: Get variance fraction...")
	LABEL (U"", U"Get the fraction of variance from the data in set...")
	OPTIONMENU (U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	LABEL (U"", U"extracted by...")
	NATURAL (U"left Canonical variate range", U"1")
	NATURAL (U"right Canonical variate range", U"1")
	OK
DO
	CCA me = FIRST (CCA);
	Correlation thee = FIRST (Correlation);
	int x_or_y = GET_INTEGER (U"X or Y");
	int cv_from = GET_INTEGER (U"left Canonical variate range");
	int cv_to = GET_INTEGER (U"right Canonical variate range");
	Melder_information (CCA_and_Correlation_getVarianceFraction (me, thee, x_or_y, cv_from, cv_to),
		U" (fraction variance from ", (x_or_y == 1 ? U"y" : U"x"), U", extracted by canonical variates ",
		cv_from, U" to ", cv_to, U")");
END

FORM (CCA_and_Correlation_getRedundancy_sl, U"CCA & Correlation: Get Stewart-Love redundancy", U"CCA & Correlation: Get redundancy (sl)...")
	LABEL (U"", U"Get the redundancy of the data in set...")
	OPTIONMENU (U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	LABEL (U"", U"extracted by...")
	NATURAL (U"left Canonical variate range", U"1")
	NATURAL (U"right Canonical variate range", U"1")
	LABEL (U"", U"...given the availability of the data in the other set.")
	OK
DO
	CCA me = FIRST (CCA);
	Correlation thee = FIRST (Correlation);
	int x_or_y = GET_INTEGER (U"X or Y");
	int cv_from = GET_INTEGER (U"left Canonical variate range");
	int cv_to = GET_INTEGER (U"right Canonical variate range");
	Melder_information (CCA_and_Correlation_getRedundancy_sl (me, thee, x_or_y, cv_from, cv_to),
		U" (redundancy from ", (x_or_y == 1 ? U"y" : U"x"), U" extracted by canonical variates ",
		cv_from, U" to ", cv_to, U")");
END

DIRECT (CCA_and_TableOfReal_factorLoadings)
	CCA me = FIRST (CCA);
	TableOfReal thee = FIRST (TableOfReal);
	autoTableOfReal result = CCA_and_TableOfReal_factorLoadings (me, thee);
	praat_new (result.move(), Thing_getName (me), U"_loadings");
END

FORM (CCA_and_TableOfReal_scores, U"CCA & TableOfReal: To TableOfReal (scores)", U"CCA & TableOfReal: To TableOfReal (scores)...")
	INTEGER (U"Number of canonical correlations", U"0 (=all)")
	OK
DO
	CCA me = FIRST (CCA);
	TableOfReal thee = FIRST (TableOfReal);
	autoTableOfReal result = CCA_and_TableOfReal_scores (me, thee, GET_INTEGER (U"Number of canonical correlations"));
	praat_new (result.move(), Thing_getName (me), U"_scores");
END

FORM (CCA_and_TableOfReal_predict, U"CCA & TableOfReal: Predict", U"CCA & TableOfReal: Predict...")
	LABEL (U"", U"The data set from which to predict starts at...")
	INTEGER (U"Column number", U"1")
	OK
DO
	CCA me = FIRST (CCA);
	TableOfReal thee = FIRST (TableOfReal);
	autoTableOfReal result = CCA_and_TableOfReal_predict (me, thee, GET_INTEGER (U"Column number"));
	praat_new (result.move(), thy name, U"_", my name);
END

/***************** ChebyshevSeries ****************************************/

DIRECT (ChebyshevSeries_help)
	Melder_help (U"ChebyshevSeries");
END

FORM (ChebyshevSeries_create, U"Create ChebyshevSeries", U"Create ChebyshevSeries...")
	WORD (U"Name", U"cs")
	LABEL (U"", U"Domain")
	REAL (U"Xmin", U"-1")
	REAL (U"Xmax", U"1")
	LABEL (U"", U"ChebyshevSeries(x) = c[1] T[0](x) + c[2] T[1](x) + ... c[n+1] T[n](x)")
	LABEL (U"", U"T[k] is a Chebyshev polynomial of degree k")
	SENTENCE (U"Coefficients (c[k])", U"0 0 1.0")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	autoChebyshevSeries me = ChebyshevSeries_createFromString (xmin, xmax, GET_STRING (U"Coefficients"));
	praat_new (me.move(), GET_STRING (U"Name"));
END

DIRECT (ChebyshevSeries_to_Polynomial)
	LOOP {
		iam (ChebyshevSeries);
		autoPolynomial thee = ChebyshevSeries_to_Polynomial (me);
		praat_new (thee.move(), my name);
	}
END

/***************** ClassificationTable ****************************************/

DIRECT (ClassificationTable_help)
	Melder_help (U"ClassificationTable");
END

FORM (ClassificationTable_getClassIndexAtMaximumInRow, U"ClassificationTable: Get class index at maximum in row", nullptr)
	NATURAL (U"Row number", U"1")
	OK
DO
	LOOP {
		iam (ClassificationTable);
		long klasIndex = TableOfReal_getColumnIndexAtMaximumInRow (me, GET_INTEGER (U"Row number"));
		Melder_information (klasIndex);
	}
END

FORM (ClassificationTable_getClassLabelAtMaximumInRow, U"ClassificationTable: Get class label at maximum in row", nullptr)
	NATURAL (U"Row number", U"1")
	OK
DO
	LOOP {
		iam (ClassificationTable);
		const char32 *klasLabel = TableOfReal_getColumnLabelAtMaximumInRow (me, GET_INTEGER (U"Row number"));
		Melder_information (klasLabel);
	}
END

// deprecated 2014
DIRECT (ClassificationTable_to_Confusion_old)
	LOOP {
		iam (ClassificationTable);
		autoConfusion thee = ClassificationTable_to_Confusion (me, false);
		praat_new (thee.move(), my name);
	}
END

FORM (ClassificationTable_to_Confusion, U"ClassificationTable: To Confusion", U"ClassificationTable: To Confusion...")
	BOOLEAN (U"Only class labels", true)
	OK
DO
	LOOP {
		iam (ClassificationTable);
		autoConfusion thee = ClassificationTable_to_Confusion (me, GET_INTEGER (U"Only class labels"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (ClassificationTable_to_Correlation_columns)
	LOOP {
		iam (ClassificationTable);
		autoCorrelation thee = ClassificationTable_to_Correlation_columns (me);
		praat_new (thee.move(), my name, U"_col");
	}
END

DIRECT (ClassificationTable_to_Strings_maximumProbability)
	LOOP {
		iam (ClassificationTable);
		autoStrings thee = ClassificationTable_to_Strings_maximumProbability (me);
		praat_new (thee.move(), my name);
	}
END

/********************** Confusion *******************************************/

DIRECT (Confusion_help)
	Melder_help (U"Confusion");
END

FORM (Confusion_createSimple, U"Create simple Confusion", U"Create simple Confusion...")
	WORD (U"Name", U"simple")
	SENTENCE (U"Labels", U"u i a")
	OK
DO
	autoConfusion thee = Confusion_createSimple (GET_STRING (U"Labels"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END

FORM (Confusion_increase, U"Confusion: Increase", U"Confusion: Increase...")
	WORD (U"Stimulus", U"u")
	WORD (U"Response", U"i")
	OK
DO
	LOOP {
		iam (Confusion);
		Confusion_increase (me, GET_STRING (U"Stimulus"), GET_STRING (U"Response"));
		praat_dataChanged (me);
	}
END

FORM (Confusion_getValue, U"Confusion: Get value", nullptr)
	WORD (U"Stimulus", U"u")
	WORD (U"Response", U"i")
	OK
DO
	char32 *stim = GET_STRING (U"Stimulus");
	char32 *resp = GET_STRING (U"Response");
	LOOP {
		iam (Confusion);
		Melder_information (Confusion_getValue (me, stim, resp),
		U" ( [\"", stim, U"\", \"",  resp, U"\"] )");
	}
END

FORM (Confusion_getResponseSum, U"Confusion: Get response sum", U"Confusion: Get response sum...")
	WORD (U"Response", U"u")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (TableOfReal_getColumnSumByLabel (me, GET_STRING (U"Response")));
	}
END

FORM (Confusion_getStimulusSum, U"Confusion: Get stimulus sum", U"Confusion: Get stimulus sum...")
	WORD (U"Stimulus", U"u")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (TableOfReal_getRowSumByLabel (me, GET_STRING (U"Stimulus")));
	}
END

DIRECT (Confusion_to_TableOfReal_marginals)
	LOOP {
		iam (Confusion);
		autoTableOfReal thee = Confusion_to_TableOfReal_marginals (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Confusion_difference)
	Confusion c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Confusion);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	autoMatrix thee = Confusion_difference (c1, c2);
	praat_new (thee.move(), U"diffs");
END

FORM (Confusion_condense, U"Confusion: Condense", U"Confusion: Condense...")
	SENTENCE (U"Search", U"^(u|i)$")
	SENTENCE (U"Replace", U"high")
	INTEGER (U"Replace limit", U"0 (=unlimited)")
	RADIO (U"Search and replace are", 2)
	RADIOBUTTON (U"Literals")
	RADIOBUTTON (U"Regular Expressions")
	OK
DO
	LOOP {
		iam (Confusion);
		autoConfusion thee = Confusion_condense (me, GET_STRING (U"Search"), GET_STRING (U"Replace"),
			GET_INTEGER (U"Replace limit"), GET_INTEGER (U"Search and replace are") - 1);
		praat_new (thee.move(), my name, U"_cnd");
	}
END

FORM (Confusion_group, U"Confusion: Group stimuli & responses", U"Confusion: Group...")
	SENTENCE (U"Stimuli & Responses", U"u i")
	SENTENCE (U"New label", U"high")
	INTEGER (U"New label position", U"0 (=at start)")
	OK
DO
	const char32 *newlabel = GET_STRING (U"New label");
	LOOP {
		iam (Confusion);
		autoConfusion thee = Confusion_group (me, GET_STRING (U"Stimuli & Responses"), newlabel,
			GET_INTEGER (U"New label position"));
		praat_new (thee.move(), my name, U"_sr", newlabel);
	}
END

FORM (Confusion_groupStimuli, U"Confusion: Group stimuli", U"Confusion: Group stimuli...")
	SENTENCE (U"Stimuli", U"u i")
	SENTENCE (U"New label", U"high")
	INTEGER (U"New label position", U"0")
	OK
DO
	const char32 *newlabel = GET_STRING (U"New label");
	LOOP {
		iam (Confusion);
		autoConfusion thee = Confusion_groupStimuli (me, GET_STRING (U"Stimuli"), newlabel,
			GET_INTEGER (U"New label position"));
		praat_new (thee.move(), my name, U"_s", newlabel);
	}
END

FORM (Confusion_groupResponses, U"Confusion: Group responses", U"Confusion: Group responses...")
	SENTENCE (U"Responses", U"a i")
	SENTENCE (U"New label", U"front")
	INTEGER (U"New label position", U"0")
	OK
DO
	const char32 *newlabel = GET_STRING (U"New label");
	LOOP {
		iam (Confusion);
		autoConfusion thee = Confusion_groupResponses (me, GET_STRING (U"Responses"), newlabel,
			GET_INTEGER (U"New label position"));
		praat_new (thee.move(), my name, U"_s", newlabel);
	}
END

FORM (Confusion_drawAsNumbers, U"", U"")
	BOOLEAN (U"Draw marginals", true)
	RADIO (U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (U"Precision", U"5")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Confusion);
		Confusion_drawAsNumbers (me, GRAPHICS, GET_INTEGER (U"Draw marginals"),
			GET_INTEGER (U"Format"), GET_INTEGER (U"Precision"));
	}
END

DIRECT (Confusion_getFractionCorrect)
	LOOP {
		iam (Confusion);
		double f;
		long n;
		Confusion_getFractionCorrect (me, &f, &n);
		Melder_information (f, U" (fraction correct)");
	}
END

DIRECT (Confusion_and_ClassificationTable_increase)
	Confusion me = FIRST (Confusion);
	ClassificationTable thee = FIRST (ClassificationTable);
	Confusion_and_ClassificationTable_increase (me, thee);
END

/******************* Confusion & Matrix *************************************/

FORM (Confusion_Matrix_draw, U"Confusion & Matrix: Draw confusions with arrows", nullptr)
	INTEGER (U"Category position", U"0 (=all)")
	REAL (U"Lower level (%)", U"0")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	long categoryPosition = GET_INTEGER (U"Category position");
	REQUIRE (categoryPosition >= 0, U"Category position must be >= 0")
	Confusion conf = FIRST (Confusion);
	Matrix mat = FIRST (Matrix);
	Confusion_Matrix_draw (conf, mat, GRAPHICS, categoryPosition, GET_REAL (U"Lower level"),
		GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
END

/********************** ComplexSpectrogram *******************************************/

DIRECT (ComplexSpectrogram_help)
	Melder_help (U"ComplexSpectrogram_help");
END

FORM (ComplexSpectrogram_to_Sound, U"ComplexSpectrogram: To Sound", nullptr)
	POSITIVE (U"Duration factor", U"1.0")
	OK
DO
	LOOP {
		iam (ComplexSpectrogram);
		autoSound thee = ComplexSpectrogram_to_Sound (me, GET_REAL (U"Duration factor"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (ComplexSpectrogram_to_Spectrogram)
	LOOP {
		iam (ComplexSpectrogram);
		autoSpectrogram thee = ComplexSpectrogram_to_Spectrogram (me);
		praat_new (thee.move(), my name);
	}
END

FORM (ComplexSpectrogram_to_Spectrum, U"ComplexSpectrogram: To Spectrum (slice)", nullptr)
	REAL (U"Time (s)", U"0.0")
	OK
DO
	LOOP {
		iam (ComplexSpectrogram);
		autoSpectrum thee = ComplexSpectrogram_to_Spectrum (me, GET_REAL (U"Time"));
		praat_new (thee.move(), my name);
	}
END

/********************** Correlation *******************************************/

FORM (Correlation_createSimple, U"Create simple Correlation", U"Create simple Correlation...")
	WORD (U"Name", U"correlation")
	SENTENCE (U"Correlations", U"1.0 0.5 1.0")
	SENTENCE (U"Centroid", U"0.0 0.0")
	NATURAL (U"Number of observations", U"100")
	OK
DO
	autoCorrelation me = Correlation_createSimple (GET_STRING (U"Correlations"), GET_STRING (U"Centroid"),
		GET_INTEGER (U"Number of observations"));
	praat_new (me.move(), GET_STRING (U"Name"));
END

DIRECT (Correlation_help)
	Melder_help (U"Correlation");
END

FORM (Correlation_confidenceIntervals, U"Correlation: Confidence intervals...", U"Correlation: Confidence intervals...")
	POSITIVE (U"Confidence level (0-1)", U"0.95")
	INTEGER (U"Number of tests (Bonferroni correction)", U"0")
	RADIO (U"Approximation", 1)
		RADIOBUTTON (U"Ruben")
		RADIOBUTTON (U"Fisher")
	OK
DO
	double cl = GET_REAL (U"Confidence level");
	long numberOfTests = GET_INTEGER (U"Number of tests");
	LOOP {
		iam (Correlation);
		autoTableOfReal thee = Correlation_confidenceIntervals (me, cl, numberOfTests, GET_INTEGER (U"Approximation"));
		praat_new (thee.move(), U"conf_intervals");
	}
END

FORM (Correlation_testDiagonality_bartlett, U"Correlation: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...")
	NATURAL (U"Number of contraints", U"1")
	OK
DO
	long nc = GET_INTEGER (U"Number of contraints");
	LOOP {
		iam (Correlation);
		double chisq, p, df;
		Correlation_testDiagonality_bartlett (me, nc, & chisq, & p, & df);
		Melder_information (p, U" (=probability, based on chisq = ", chisq, U" and ndf = ", df, U")");
	}
END

DIRECT (Correlation_to_PCA)
	LOOP {
		iam (Correlation);
		autoPCA thee = SSCP_to_PCA (me);
		praat_new (thee.move(), my name);
	}
END

/********************** Covariance *******************************************/

DIRECT (Covariance_help)
	Melder_help (U"Covariance");
END

FORM (Covariance_createSimple, U"Create simple Covariance", U"Create simple Covariance...")
	WORD (U"Name", U"c")
	SENTENCE (U"Covariances", U"1.0 0.0 1.0")
	SENTENCE (U"Centroid", U"0.0 0.0")
	NATURAL (U"Number of observations", U"100")
	OK
DO
	autoCovariance me = Covariance_createSimple (GET_STRING (U"Covariances"), GET_STRING (U"Centroid"),
		GET_INTEGER (U"Number of observations"));
	praat_new (me.move(), GET_STRING (U"Name"));
END

FORM (Covariance_getProbabilityAtPosition, U"Covariance: Get probability at position", nullptr)
	SENTENCE (U"Position", U"10.0 20.0")
	OK
DO
	char32 *position = GET_STRING (U"Position");
	LOOP {
		iam (Covariance);
		double p = Covariance_getProbabilityAtPosition_string (me, position);
		Melder_information (p, U" (= probability at position ", position, U")");
	}
END

FORM (Covariance_getSignificanceOfOneMean, U"Covariance: Get significance of one mean", U"Covariance: Get significance of one mean...")
	LABEL (U"", U"Get probability that the estimated mean for")
	NATURAL (U"Index", U"1")
	LABEL (U"", U"(or an estimated mean even further away)")
	LABEL (U"", U"could arise if the true mean were")
	REAL (U"Value", U"0.0")
	OK
DO
	LOOP {
		iam (Covariance);
		double p, t, ndf;
		Covariance_getSignificanceOfOneMean (me, GET_INTEGER (U"Index"), GET_REAL (U"Value"), & p, & t, & ndf);
		Melder_information (p, U" (=probability, based on t = ", t, U" and ndf = ", ndf);
	}
END

FORM (Covariance_getSignificanceOfMeansDifference, U"Covariance: Get significance of means difference", U"Covariance: Get significance of means difference...")
	LABEL (U"", U"Get probability that the estimated difference between the means for")
	NATURAL (U"Index1", U"1")
	NATURAL (U"Index2", U"2")
	LABEL (U"", U"could arise if the true mean were")
	REAL (U"Value", U"0.0")
	LABEL (U"", U"Assume the means are ")
	BOOLEAN (U"Paired", true)
	LABEL (U"", U"and have")
	BOOLEAN (U"Equal variances", true)
	OK
DO
	LOOP {
		iam (Covariance);
		double p, t, ndf;
		Covariance_getSignificanceOfMeansDifference (me, GET_INTEGER (U"Index1"), GET_INTEGER (U"Index2"),
			GET_REAL (U"Value"), GET_INTEGER (U"Paired"), GET_INTEGER (U"Equal variances"), & p, & t, & ndf);
		Melder_information (p, U" (=probability, based on t = ",
			t, U"and ndf = ", ndf, U")");
	}
END

FORM (Covariance_getSignificanceOfOneVariance, U"Covariance: Get significance of one variance", U"Covariance: Get significance of one variance...")
	LABEL (U"", U"Get the probability that the estimated variance for")
	NATURAL (U"Index", U"1")
	LABEL (U"", U"(or an even larger estimated variance)")
	LABEL (U"", U"could arise if the true variance were")
	REAL (U"Value", U"0.0")
	OK
DO
	LOOP {
		iam (Covariance);
		double p, chisq; long ndf;
		Covariance_getSignificanceOfOneVariance (me, GET_INTEGER (U"Index"), GET_REAL (U"Value"), & p, & chisq, & ndf);
		Melder_information (p, U" (=probability, based on chisq = ", chisq, U" and ndf = ", ndf);
	}
END

FORM (Covariance_getSignificanceOfVariancesRatio, U"Covariance: Get significance of variances ratio", nullptr)
	LABEL (U"", U"Get the probability that the estimated variance ratio observed for")
	NATURAL (U"Index1", U"1")
	NATURAL (U"Index2", U"2")
	LABEL (U"", U"(or an estimated ratio even further away)")
	LABEL (U"", U"could arise if the true ratio were")
	REAL (U"Value", U"1.0")
	OK
DO
	LOOP {
		iam (Covariance);
		double p, f, df;
		Covariance_getSignificanceOfVariancesRatio (me, GET_INTEGER (U"Index1"), GET_INTEGER (U"Index2"),
			GET_REAL (U"Value"), & p, & f , & df);
		Melder_information (p, U" (=probability, based on F = ", f, U" and ndf1 = ", df, U" and ndf2 = ", df);

	}
END

FORM (Covariance_getFractionVariance, U"Covariance: Get fraction variance", U"Covariance: Get fraction variance...")
	NATURAL (U"From dimension", U"1")
	NATURAL (U"To dimension", U"1")
	OK
DO
	LOOP {
		iam (Covariance);
		Melder_information (SSCP_getFractionVariation (me, GET_INTEGER (U"From dimension"), GET_INTEGER (U"To dimension")));
	}
END

FORM (Covariances_reportMultivariateMeanDifference, U"Covariances: Report multivariate mean difference", U"Covariances: Report multivariate mean difference...")
	LABEL (U"", U"Get probability that the estimated multivariate means difference could arise ")
	LABEL (U"", U"if the actual means were equal.")
	LABEL (U"", U"")
	LABEL (U"", U"Assume for both means we have")
	BOOLEAN (U"Equal covariances", true)
	OK
DO
	Covariance c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Covariance);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	double prob, fisher, df1, df2, difference;
	bool equalCovariances = GET_INTEGER (U"Equal covariances");
	MelderInfo_open ();
	difference = Covariances_getMultivariateCentroidDifference (c1, c2, equalCovariances, & prob, & fisher, & df1, & df2);
	MelderInfo_writeLine (U"Under the assumption that the two covariances are", (equalCovariances ? U" " : U" not "), U"equal:");
	MelderInfo_writeLine (U"Difference between multivariate means = ", difference);
	MelderInfo_writeLine (U"Fisher's F = ", fisher);
	MelderInfo_writeLine (U"Significance from zero = ", prob);
	MelderInfo_writeLine (U"Degrees of freedom = ", df1, U", ", df2);
	MelderInfo_writeLine (U"(Number of observations = ", c1->numberOfObservations, U", ", c2->numberOfObservations);
	MelderInfo_writeLine (U"Dimension of covariance matrices = ", c1-> numberOfRows, U")");
	MelderInfo_close ();
END

FORM (Covariance_to_TableOfReal_randomSampling, U"Covariance: To TableOfReal (random sampling)", U"Covariance: To TableOfReal (random sampling)...")
	INTEGER (U"Number of data points", U"0")
	OK
DO
	LOOP {
		iam (Covariance);
		autoTableOfReal thee = Covariance_to_TableOfReal_randomSampling (me, GET_INTEGER (U"Number of data points"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (Covariances_reportEquality)
	autoCovarianceList covariances = CovarianceList_create ();
	LOOP {
		iam (Covariance);
		covariances -> addItem_ref (me);
	}
	MelderInfo_open ();
	double p, chisq, df;
	Covariances_equality (covariances.get(), 1, & p, & chisq, & df);
	MelderInfo_writeLine (U"Difference between covariance matrices:");
	MelderInfo_writeLine (U"Significance of difference (bartlett) = ", p);
	MelderInfo_writeLine (U"Chi-squared (bartlett) = ", chisq);
	MelderInfo_writeLine (U"Degrees of freedom (bartlett) = ", df);
	
	Covariances_equality (covariances.get(), 2, &p, &chisq, &df);
	MelderInfo_writeLine (U"Significance of difference (wald) = ", p);
	MelderInfo_writeLine (U"Chi-squared (wald) = ", chisq);
	MelderInfo_writeLine (U"Degrees of freedom (wald) = ", df);
	MelderInfo_close ();
END

DIRECT (Covariance_to_Correlation)
	LOOP {
		iam (Covariance);
		autoCorrelation thee = SSCP_to_Correlation (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Covariance_to_PCA)
	LOOP {
		iam (Covariance);
		autoPCA thee = SSCP_to_PCA (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Covariances_pool)
	autoCovarianceList covariances = CovarianceList_create ();
	LOOP {
		iam (Covariance);
		covariances -> addItem_ref (me);
	}
	autoCovariance thee = CovarianceList_to_Covariance_pool (covariances.get());
	praat_new (thee.move(), U"pool");
END

FORM (Covariance_and_TableOfReal_mahalanobis, U"Covariance & TableOfReal: To TableOfReal (mahalanobis)", U"Covariance & TableOfReal: To TableOfReal (mahalanobis)...")
	BOOLEAN (U"Centroid from table", 0)
	OK
DO
	Covariance me = FIRST (Covariance);
	TableOfReal thee = FIRST (TableOfReal);
	praat_new (Covariance_and_TableOfReal_mahalanobis (me, thee, GET_INTEGER (U"Centroid from table")), U"mahalanobis");
END

/********************** Discriminant **********************************/

DIRECT (Discriminant_help)
	Melder_help (U"Discriminant");
END

DIRECT (Discriminant_setGroupLabels)
	Discriminant me = FIRST (Discriminant);
	Strings ss = FIRST (Strings);
	Discriminant_setGroupLabels (me, ss);
	praat_dataChanged (me);
END

FORM (Discriminant_and_Pattern_to_Categories, U"Discriminant & Pattern: To Categories", U"Discriminant & Pattern: To Categories...")
	BOOLEAN (U"Pool covariance matrices", true)
	BOOLEAN (U"Use apriori probabilities", true)
	OK
DO
	Discriminant me = FIRST (Discriminant);
	Pattern pat = FIRST (Pattern);
	autoCategories thee = Discriminant_and_Pattern_to_Categories (me, pat, GET_INTEGER (U"Pool covariance matrices"), GET_INTEGER (U"Use apriori probabilities"));
	praat_new (thee.move(), my name, U"_", pat->name);
END

FORM (Discriminant_and_TableOfReal_to_Configuration, U"Discriminant & TableOfReal: To Configuration", U"Discriminant & TableOfReal: To Configuration...")
		INTEGER (U"Number of dimensions", U"0")
		OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions");
	REQUIRE (dimension >= 0, U"Number of dimensions must be greater equal zero.")
	Discriminant me = FIRST (Discriminant);
	TableOfReal tr = FIRST_GENERIC (TableOfReal);
	autoConfiguration thee = Discriminant_and_TableOfReal_to_Configuration (me, tr, dimension);
	praat_new (thee.move(), my name, U"_", tr->name);
END

DIRECT (hint_Discriminant_and_TableOfReal_to_ClassificationTable)
	Melder_information (U"You can use the Discriminant as a classifier by \nselecting a Discriminant and a TableOfReal object together.");
END

FORM (Discriminant_and_TableOfReal_to_ClassificationTable, U"Discriminant & TableOfReal: To ClassificationTable", U"Discriminant & TableOfReal: To ClassificationTable...")
	BOOLEAN (U"Pool covariance matrices", true)
	BOOLEAN (U"Use apriori probabilities", true)
	OK
DO
	Discriminant me = FIRST (Discriminant);
	TableOfReal tr = FIRST_GENERIC (TableOfReal);
	autoClassificationTable thee = Discriminant_and_TableOfReal_to_ClassificationTable (me, tr,
		GET_INTEGER (U"Pool covariance matrices"), GET_INTEGER (U"Use apriori probabilities"));
	praat_new (thee.move(), my name, U"_", tr->name);
END

FORM (Discriminant_and_TableOfReal_mahalanobis, U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)", U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...")
	SENTENCE (U"Group label", U"")
	BOOLEAN (U"Pool covariance matrices", false)
	OK
DO
	Discriminant me = FIRST (Discriminant);
	TableOfReal tr = FIRST (TableOfReal);
	long group = Discriminant_groupLabelToIndex (me, GET_STRING (U"Group label"));
	REQUIRE (group > 0, U"Group label does not exist.")
	autoTableOfReal thee = Discriminant_and_TableOfReal_mahalanobis (me, tr, group, GET_INTEGER (U"Pool covariance matrices"));
	praat_new (thee.move(), U"mahalanobis");
END

DIRECT (Discriminant_getNumberOfEigenvalues)
	LOOP {
		iam (Discriminant);
		Melder_information (my eigen -> numberOfEigenvalues);
	}
END

DIRECT (Discriminant_getDimension)
	LOOP {
		iam (Discriminant);
		Melder_information (my eigen -> dimension);
	}
END

FORM (Discriminant_getEigenvalue, U"Discriminant: Get eigenvalue", U"Eigen: Get eigenvalue...")
	NATURAL (U"Eigenvalue number", U"1")
	OK
DO
	LOOP {
		iam (Discriminant);
		long number = GET_INTEGER (U"Eigenvalue number");
		if (number > my eigen -> numberOfEigenvalues) {
			Melder_throw (U"Eigenvalue number must be smaller than ", my eigen -> numberOfEigenvalues + 1);
		}
		Melder_information (my eigen -> eigenvalues[number]);
	}
END

FORM (Discriminant_getSumOfEigenvalues, U"Discriminant:Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...")
	INTEGER (U"left Eigenvalue range",  U"0")
	INTEGER (U"right Eigenvalue range", U"0")
	OK
DO
	LOOP {
		iam (Discriminant);
		Melder_information (Eigen_getSumOfEigenvalues (my eigen.get(), GET_INTEGER (U"left Eigenvalue range"), GET_INTEGER (U"right Eigenvalue range")));
	}
END

FORM (Discriminant_getEigenvectorElement, U"Discriminant: Get eigenvector element", U"Eigen: Get eigenvector element...")
	NATURAL (U"Eigenvector number", U"1")
	NATURAL (U"Element number", U"1")
	OK
DO
	LOOP {
		iam (Discriminant);
		Melder_information (Eigen_getEigenvectorElement (my eigen.get(), GET_INTEGER (U"Eigenvector number"), GET_INTEGER (U"Element number")));
	}
END

FORM (Discriminant_getWilksLambda, U"Discriminant: Get Wilks' lambda", U"Discriminant: Get Wilks' lambda...")
	LABEL (U"", U"Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	INTEGER (U"From", U"1")
	OK
DO
	long from = GET_INTEGER (U"From");
	REQUIRE (from >= 1, U"Number must be greater than or equal to one.")
	LOOP {
		iam (Discriminant);
		Melder_information (Discriminant_getWilksLambda (me, from));
	}
END

FORM (Discriminant_getCumulativeContributionOfComponents, U"Discriminant: Get cumulative contribution of components", U"Eigen: Get cumulative contribution of components...")
	NATURAL (U"From component", U"1")
	NATURAL (U"To component", U"1")
	OK
DO
	LOOP {
		iam (Discriminant);
		Melder_information (Eigen_getCumulativeContributionOfComponents (my eigen.get(),
			GET_INTEGER (U"From component"), GET_INTEGER (U"To component")));
	}
END


FORM (Discriminant_getPartialDiscriminationProbability, U"Discriminant: Get partial discrimination probability", U"Discriminant: Get partial discrimination probability...")
	INTEGER (U"Number of dimensions", U"1")
	OK
DO
	long n = GET_INTEGER (U"Number of dimensions");
	REQUIRE (n >= 0, U"Number of dimensions must be greater than or equal to zero.")
	LOOP {
		iam (Discriminant);
		double p, chisq, df;
		Discriminant_getPartialDiscriminationProbability (me, n, & p, & chisq, & df);
		Melder_information (p, U" (=probability, based on chisq = ", chisq, U"and ndf = ", df, U")");
	}
END

DIRECT (Discriminant_getHomegeneityOfCovariances_box)
	LOOP {
		iam (Discriminant);
		double chisq, p; double ndf;
		SSCPList_getHomegeneityOfCovariances_box (my groups.get(), &p, &chisq, &ndf);
		Melder_information (p, U" (=probability, based on chisq = ",
			chisq, U"and ndf = ", ndf, U")");
	}
END

DIRECT (Discriminant_reportEqualityOfCovariances_wald)
	MelderInfo_open ();
	LOOP {
		iam (Discriminant);
		structCovarianceList list;
		for (long i = 1; i <= my groups->size; i ++) {
			SSCP sscp = my groups->at [i];
			autoCovariance covariance = SSCP_to_Covariance (sscp, 1);   // FIXME numberOfConstraints shouldn't be 1, should it?
			list. addItem_move (covariance.move());
		}
		double chisq, prob, df;
		Covariances_equality (& list, 2, & prob, & chisq, & df);
		MelderInfo_writeLine (U"Wald test for equality of covariance matrices:");
		MelderInfo_writeLine (U"Chi squared: ", chisq);
		MelderInfo_writeLine (U"Significance: ", prob);
		MelderInfo_writeLine (U"Degrees of freedom: ", df);
		MelderInfo_writeLine (U"Number of matrices: ", list.size);
	}
	MelderInfo_close ();
END

FORM (Discriminant_getConcentrationEllipseArea, U"Discriminant: Get concentration ellipse area", U"Discriminant: Get concentration ellipse area...")
	SENTENCE (U"Group label", U"")
	POSITIVE (U"Number of sigmas", U"1.0")
	BOOLEAN (U"Discriminant plane", true)
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	OK
DO
	LOOP {
		iam (Discriminant);
		long group = Discriminant_groupLabelToIndex (me, GET_STRING (U"Group label"));
		REQUIRE (group > 0, U"Group label does not exist.")
		Melder_information (Discriminant_getConcentrationEllipseArea (me,
			group,
			GET_REAL (U"Number of sigmas"),
			false,   // confidence
			GET_INTEGER (U"Discriminant plane"),
			GET_INTEGER (U"X-dimension"),
			GET_INTEGER (U"Y-dimension")));
	}
END

FORM (Discriminant_getConfidenceEllipseArea, U"Discriminant: Get confidence ellipse area", U"Discriminant: Get confidence ellipse area...")
	SENTENCE (U"Group label", U"")
	POSITIVE (U"Confidence level (0-1)", U"0.95")
	BOOLEAN (U"Discriminant plane", true)
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	OK
DO
	LOOP {
		iam (Discriminant);
		long group = Discriminant_groupLabelToIndex (me, GET_STRING (U"Group label"));
		REQUIRE (group > 0, U"Group label does not exist.")
		Melder_information (Discriminant_getConcentrationEllipseArea (me,
			group,
			GET_REAL (U"Confidence level"),
			true,    //confidence
			GET_INTEGER (U"Discriminant plane"),
			GET_INTEGER (U"X-dimension"),
			GET_INTEGER (U"Y-dimension")));
	}
END

FORM (Discriminant_getLnDeterminant_group, U"Discriminant: Get determinant (group)", U"Discriminant: Get determinant (group)...")
	SENTENCE (U"Group label", U"")
	OK
DO
	LOOP {
		iam (Discriminant);
		long group = Discriminant_groupLabelToIndex (me, GET_STRING (U"Group label"));
		REQUIRE (group > 0, U"Group label does not exist.")
		Melder_information (Discriminant_getLnDeterminant_group (me, group));
	}
END

DIRECT (Discriminant_getLnDeterminant_total)
	LOOP {
		iam (Discriminant);
		Melder_information (Discriminant_getLnDeterminant_total (me));
	}
END

FORM (Discriminant_invertEigenvector, U"Discriminant: Invert eigenvector", nullptr)
	NATURAL (U"Index of eigenvector", U"1")
	OK
DO
	LOOP {
		iam (Discriminant);
		Eigen_invertEigenvector (my eigen.get(), GET_INTEGER (U"Index of eigenvector"));
		praat_dataChanged (me);
	}
END

FORM (Discriminant_drawEigenvalues, U"Discriminant: Draw eigenvalues", U"Eigen: Draw eigenvalues...")
	INTEGER (U"left Eigenvalue range", U"0")
	INTEGER (U"right Eigenvalue range", U"0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Fraction of eigenvalues summed", false)
	BOOLEAN (U"Cumulative", false)
	POSITIVE (U"Mark size (mm)", U"1.0")
	SENTENCE (U"Mark string (+xo.)", U"+")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Eigen_drawEigenvalues (my eigen.get(), GRAPHICS,
			GET_INTEGER (U"left Eigenvalue range"),
			GET_INTEGER (U"right Eigenvalue range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Fraction of eigenvalues summed"),
			GET_INTEGER (U"Cumulative"),
			GET_REAL (U"Mark size"),
			GET_STRING (U"Mark string"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (Discriminant_drawEigenvector, U"Discriminant: Draw eigenvector", U"Eigen: Draw eigenvector...")
	INTEGER (U"Eigenvector number", U"1")
	BOOLEAN (U"Component loadings", false)
	INTEGER (U"left Element range", U"0")
	INTEGER (U"right Element range", U"0")
	REAL (U"left Amplitude range", U"-1.0")
	REAL (U"right Amplitude range", U"1.0")
	POSITIVE (U"Mark size (mm)", U"1.0")
	SENTENCE (U"Mark string (+xo.)", U"+")
	BOOLEAN (U"Connect points", true)
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Eigen_drawEigenvector (my eigen.get(), GRAPHICS,
			GET_INTEGER (U"Eigenvector number"),
			GET_INTEGER (U"left Element range"),
			GET_INTEGER (U"right Element range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Component loadings"),
			GET_REAL (U"Mark size"),
			GET_STRING (U"Mark string"),
			GET_INTEGER (U"Connect points"),
			nullptr,   // rowLabels
			GET_INTEGER (U"Garnish"));
	}
END


FORM (Discriminant_drawSigmaEllipses, U"Discriminant: Draw sigma ellipses", U"Discriminant: Draw sigma ellipses...")
	POSITIVE (U"Number of sigmas", U"1.0")
	BOOLEAN (U"Discriminant plane", true)
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	INTEGER (U"Label size", U"12")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS,
			GET_REAL (U"Number of sigmas"),
			false,   // confidence
			nullptr,   // label
			GET_INTEGER (U"Discriminant plane"),
			GET_INTEGER (U"X-dimension"),
			GET_INTEGER (U"Y-dimension"),
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (Discriminant_drawOneSigmaEllipse, U"Discriminant: Draw one sigma ellipse", U"Discriminant: Draw one sigma ellipse...")
	SENTENCE (U"Label", U"")
	POSITIVE (U"Number of sigmas", U"1.0")
	BOOLEAN (U"Discriminant plane", true)
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	INTEGER (U"Label size", U"12")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS,
			GET_REAL (U"Number of sigmas"),
			false,   // confidence
			GET_STRING (U"Label"),
			GET_INTEGER (U"Discriminant plane"),
			GET_INTEGER (U"X-dimension"),
			GET_INTEGER (U"Y-dimension"),
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (Discriminant_drawConfidenceEllipses, U"Discriminant: Draw confidence ellipses", nullptr)
	POSITIVE (U"Confidence level (0-1)", U"0.95")
	BOOLEAN (U"Discriminant plane", true)
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	INTEGER (U"Label size", U"12")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS,
			GET_REAL (U"Confidence level"),
			true,   // confidence
			nullptr,   // label
			GET_INTEGER (U"Discriminant plane"),
			GET_INTEGER (U"X-dimension"),
			GET_INTEGER (U"Y-dimension"),
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"));
	}
END


FORM (Discriminant_drawOneConfidenceEllipse, U"Discriminant: Draw one confidence ellipse", nullptr)
	SENTENCE (U"Label", U"")
	POSITIVE (U"Confidence level (0-1)", U"0.95")
	BOOLEAN (U"Discriminant plane", true)
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	INTEGER (U"Label size", U"12")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Discriminant);
		Discriminant_drawConcentrationEllipses (me, GRAPHICS,
			GET_REAL (U"Confidence level"),
			true,   // confidence
			GET_STRING (U"Label"),
			GET_INTEGER (U"Discriminant plane"),
			GET_INTEGER (U"X-dimension"),
			GET_INTEGER (U"Y-dimension"),
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"));
	}
END

DIRECT (Discriminant_extractBetweenGroupsSSCP)
	LOOP {
		iam (Discriminant);
		autoSSCP thee = Discriminant_extractBetweenGroupsSSCP (me);
		praat_new (thee.move());
	}
END

DIRECT (Discriminant_extractGroupCentroids)
	LOOP {
		iam (Discriminant);
		autoTableOfReal thee = Discriminant_extractGroupCentroids (me);
		praat_new (thee.move(), U"centroids");
	}
END

DIRECT (Discriminant_extractGroupStandardDeviations)
	LOOP {
		iam (Discriminant);
		autoTableOfReal thee = Discriminant_extractGroupStandardDeviations (me);
		praat_new (thee.move(), U"group_stddevs");
	}
END

DIRECT (Discriminant_extractGroupLabels)
	LOOP {
		iam (Discriminant);
		autoStrings thee = Discriminant_extractGroupLabels (me);
		praat_new (thee.move(), U"group_labels");
	}
END

DIRECT (Discriminant_extractPooledWithinGroupsSSCP)
	LOOP {
		iam (Discriminant);
		autoSSCP thee = Discriminant_extractPooledWithinGroupsSSCP (me);
		praat_new (thee.move(), U"pooled_within");
	}
END

FORM (Discriminant_extractWithinGroupSSCP, U"Discriminant: Extract within-group SSCP", U"Discriminant: Extract within-group SSCP...")
	NATURAL (U"Group index", U"1")
	OK
DO
	long index = GET_INTEGER (U"Group index");
	LOOP {
		iam (Discriminant);
		autoSSCP thee = Discriminant_extractWithinGroupSSCP (me, index);
		praat_new (thee.move(), my name, U"_g", index);
	}
END

DIRECT (Discriminant_getNumberOfFunctions)
	LOOP {
		iam (Discriminant);
		Melder_information (Discriminant_getNumberOfFunctions (me));
	}
END

DIRECT (Discriminant_getDimensionOfFunctions)
	LOOP {
		iam (Discriminant);
		Melder_information (Eigen_getDimensionOfComponents (my eigen.get()));
	}
END

DIRECT (Discriminant_getNumberOfGroups)
	LOOP {
		iam (Discriminant);
		Melder_information (Discriminant_getNumberOfGroups (me));
	}
END

FORM (Discriminant_getNumberOfObservations, U"Discriminant: Get number of observations", U"Discriminant: Get number of observations...")
	INTEGER (U"Group", U"0 (=total)")
	OK
DO
	LOOP {
		iam (Discriminant);
		Melder_information (Discriminant_getNumberOfObservations (me, GET_INTEGER (U"Group")));
	}
END


/********************** DTW *******************************************/

FORM (DTW_and_Polygon_findPathInside, U"DTW & Polygon: Find path inside", nullptr)
    RADIO (U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    int localSlope = GET_INTEGER (U"Slope constraint");
    DTW me = FIRST (DTW);
    Polygon thee = FIRST (Polygon);
    DTW_and_Polygon_findPathInside (me, thee, localSlope, 0);

END

FORM (DTW_and_Polygon_to_Matrix_cummulativeDistances, U"DTW & Polygon: To Matrix (cumm. distances)", nullptr)
    RADIO (U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    int localSlope = GET_INTEGER (U"Slope constraint");
    DTW me = FIRST (DTW);
    Polygon thee = FIRST (Polygon);
    autoMatrix him = DTW_and_Polygon_to_Matrix_cummulativeDistances (me, thee, localSlope);
    praat_new (him.move(), my name, U"_", localSlope);
END

FORM (DTW_and_Sounds_draw, U"DTW & Sounds: Draw", U"DTW & Sounds: Draw...")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	Sound s1 = nullptr, s2 = nullptr;
	DTW dtw = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classSound) {
			(s1 ? s2 : s1) = (Sound) me;
		} else if (CLASS == classDTW) {
			dtw = (DTW) me;
		}
	}
	Melder_assert (s1 && s2 && dtw);
	autoPraatPicture picture;
	DTW_and_Sounds_draw (dtw, s2, s1, GRAPHICS, GET_REAL (U"left Horizontal range"),
		GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_INTEGER (U"Garnish"));
END

FORM (DTW_and_Sounds_drawWarpX, U"DTW & Sounds: Draw warp (x)", U"DTW & Sounds: Draw warp (x)...")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Time (s)", U"0.1")
	BOOLEAN (U"Garnish", true)
	OK
DO
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	DTW dtw = FIRST (DTW);
	autoPraatPicture picture;
	DTW_and_Sounds_drawWarpX (dtw, s2, s1, GRAPHICS,
		GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_REAL (U"Time"), GET_INTEGER (U"Garnish"));
END

void DTW_constraints_addCommonFields (UiForm dia) {
	UiField radio;
	LABEL (U"", U"Boundary conditions")
	BOOLEAN (U"Match begin positions", false)
	BOOLEAN (U"Match end positions", false)
	RADIO (U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
}

void DTW_constraints_getCommonFields (UiForm dia, int *begin, int *end, int *slope) {
	*begin = GET_INTEGER (U"Match begin positions");
	*end = GET_INTEGER (U"Match end positions");
	*slope = GET_INTEGER (U"Slope constraint");
}

DIRECT (DTW_help) Melder_help (U"DTW"); END

FORM (DTW_drawPath, U"DTW: Draw path", nullptr)
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", false);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawPath (me, GRAPHICS,
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (DTW_drawDistancesAlongPath, U"DTW: Draw distances along path", nullptr)
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", false);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawDistancesAlongPath (me,
			GRAPHICS, GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (DTW_paintDistances, U"DTW: Paint distances", nullptr)
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	BOOLEAN (U"Garnish", false);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_paintDistances (me, GRAPHICS,
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_REAL (U"Minimum"),
			GET_REAL (U"Maximum"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (DTW_drawWarpX, U"DTW: Draw warp (x)", U"DTW: Draw warp (x)...")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Time (s)", U"0.1")
	BOOLEAN (U"Garnish", false);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawWarpX (me, GRAPHICS,
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_REAL (U"Time"),
			GET_INTEGER (U"Garnish"));
	}
END


FORM (DTW_drawWarpY, U"DTW: Draw warp (y)", U"DTW: Draw warp (y)...")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Time (s)", U"0.1")
	BOOLEAN (U"Garnish", false);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (DTW);
		DTW_drawWarpY (me, GRAPHICS,
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_REAL (U"Time"),
			GET_INTEGER (U"Garnish"));
	}
END

DIRECT (DTW_getStartTimeX)
	LOOP {
		iam (DTW);
		Melder_information (my xmin, U" s (= start time along x)");
	}
END

DIRECT (DTW_getEndTimeX)
	LOOP {
		iam (DTW);
		Melder_information (my xmax, U" s (= end time along x)");
	}
END

DIRECT (DTW_getTotalDurationX)
	LOOP {
		iam (DTW);
		Melder_information (my xmax - my xmin, U" s (= total duration along x)");
	}
END

DIRECT (DTW_getStartTimeY)
	LOOP {
		iam (DTW);
		Melder_information (my ymin, U" s (= start time along y)");
	}
END

DIRECT (DTW_getEndTimeY)
	LOOP {
		iam (DTW);
		Melder_information (my ymax, U" s (= end time along y)");
	}
END

DIRECT (DTW_getTotalDurationY)
	LOOP {
		iam (DTW);
		Melder_information (my ymax - my ymin, U" s (= total duration along y)");
	}
END

DIRECT (DTW_getNumberOfFramesX)
	LOOP {
		iam (DTW);
		Melder_information (my nx, U" (= number of frames along x)");
	}
END

DIRECT (DTW_getTimeStepX)
	LOOP {
		iam (DTW);
		Melder_information (my dx, U" s (= time step along x)");
	}
END

FORM (DTW_getTimeFromFrameNumberX, U"DTW: Get time from frame number (x)", 0)
	NATURAL (U"Frame number (x)", U"1")
	OK
DO
	long column = GET_INTEGER (U"Frame number");
	LOOP {
		iam (DTW);
		Melder_information (Matrix_columnToX (me, column), U" s (= y time at x frame ", column, U")");
	}
END

FORM (DTW_getFrameNumberFromTimeX, U"DTW: Get frame number from time (x)", 0)
	REAL (U"Time along x (s)", U"0.1")
	OK
DO
	double time = GET_REAL (U"Time along x");
	LOOP {
		iam (DTW);
		if (time < my xmin || time > my xmax) {
			Melder_throw (me, U"Time outside x domain.");
		}
		long iframe = lround (Matrix_xToColumn (me, time));
		Melder_information (iframe, U" (= x frame at y time ", time, U")");
	}
END

DIRECT (DTW_getNumberOfFramesY)
	LOOP {
		iam (DTW);
		Melder_information (my ny, U" (= number of frames along y)");
	}
END

DIRECT (DTW_getTimeStepY)
	LOOP {
		iam (DTW);
		Melder_information (my dy, U" s (= time step along y)");
	}
END

FORM (DTW_getTimeFromFrameNumberY, U"DTW: Get time from frame number (y)", 0)
	NATURAL (U"Frame number (y)", U"1")
	OK
DO
	long row = GET_INTEGER (U"Frame number");
	LOOP {
		iam (DTW);
		Melder_information (Matrix_rowToY (me, row), U" s (= x time at y frame ", row, U")");
	}
END

FORM (DTW_getFrameNumberFromTimeY, U"DTW: Get frame number from time (y)", 0)
	REAL (U"Time along y (s)", U"0.1")
	OK
DO
	double time = GET_REAL (U"Time along y");
	LOOP {
		iam (DTW);
		if (time < my ymin || time > my ymax) {
			Melder_throw (me, U"Time outside y domain.");
		}
		long iframe = lround (Matrix_yToRow (me, time));
		Melder_information (iframe, U" (= y frame at x time ", time, U")");
	}
END


FORM (DTW_getPathY, U"DTW: Get time along path", U"DTW: Get time along path...")
	REAL (U"Time (s)", U"0.0")
	OK
DO
	LOOP {
		iam (DTW);
		Melder_information (DTW_getYTimeFromXTime (me, GET_REAL (U"Time")));
	}
END

FORM (DTW_getYTimeFromXTime, U"DTW: Get y time from x time", U"DTW: Get y time from x time...")
	REAL (U"Time at x (s)", U"0.0")
	OK
DO
	double time = GET_REAL (U"Time at x");
	LOOP {
		iam (DTW);
		Melder_information (DTW_getYTimeFromXTime (me, time), U" s (= y time at x time ", time, U")");
	}
END

FORM (DTW_getXTimeFromYTime, U"DTW: Get x time from y time", U"DTW: Get x time from y time...")
	REAL (U"Time at y (s)", U"0.0")
	OK
DO
	double time = GET_REAL (U"Time at y");
	LOOP {
		iam (DTW);
		Melder_information (DTW_getXTimeFromYTime (me, time), U" s (= x time at y time ", time, U")");
	}
END

FORM (DTW_getMaximumConsecutiveSteps, U"DTW: Get maximum consecutive steps", U"DTW: Get maximum consecutive steps...")
	OPTIONMENU (U"Direction", 1)
		OPTION (U"X")
		OPTION (U"Y")
		OPTION (U"Diagonaal")
	OK
DO
	int direction [] = { DTW_START, DTW_X, DTW_Y, DTW_XANDY };
	const char32 *string [] = { U"", U"x", U"y", U"diagonal" };
	int d = GET_INTEGER (U"Direction");
	LOOP {
		iam (DTW);
		Melder_information (DTW_getMaximumConsecutiveSteps (me, direction [d]),
			U" (= maximum number of consecutive steps in ", string [d], U" direction)");
	}
END

DIRECT (DTW_getWeightedDistance)
	LOOP {
		iam (DTW);
		Melder_information (my weightedDistance);
	}
END

FORM (DTW_getDistanceValue, U"DTW: Get distance value", nullptr)
	REAL (U"Time at x (s)", U"0.1")
	REAL (U"Time at y (s)", U"0.1")
	OK
DO
	double xtime = GET_REAL (U"Time at x");
	double ytime = GET_REAL (U"Time at y");
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
		Melder_information (dist, U" (= distance at (", xtime, U", ", ytime, U"))");
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

FORM (DTW_formulaDistances, U"DTW: Formula (distances)", 0)
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (DTW);
		autoMatrix cp = DTW_to_Matrix_distances (me);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (U"formula"), interpreter, 0);
			double minimum, maximum;
			Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
			if (minimum < 0) {
				DTW_and_Matrix_replace (me, cp.get()); // restore original
				Melder_throw (U"Execution of the formula has made some distance(s) negative which is not allowed.");
			}
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (DTW_setDistanceValue, U"DTW: Set distance value", nullptr)
	REAL (U"Time at x (s)", U"0.1")
	REAL (U"Time at y (s)", U"0.1")
	REAL (U"New value", U"0.0")
	OK
DO
	double xtime = GET_REAL (U"Time at x");
	double ytime = GET_REAL (U"Time at y");
	double val = GET_REAL (U"New value");
	if (val < 0) {
		Melder_throw (U"Distances cannot be negative.");
	}
	LOOP {
		iam (DTW);
		if (xtime < my xmin || xtime > my xmax) {
			Melder_throw (U"Time at x outside domain.");
		}
		if (ytime < my ymin || ytime > my ymax) {
			Melder_throw (U"Time at y outside domain.");
		}
		long irow = Matrix_yToNearestRow (me, ytime);
		long icol = Matrix_xToNearestColumn (me, xtime);
		my z[irow][icol] = GET_REAL (U"New value");
		praat_dataChanged (me);
	}
END

FORM (DTW_findPath, U"DTW: Find path", nullptr)
	DTW_constraints_addCommonFields (dia);
	OK
DO
	int begin, end, slope;
	DTW_constraints_getCommonFields (dia, & begin, & end, & slope);
	LOOP {
		iam (DTW);
		DTW_findPath (me, begin, end, slope);
	}
END

FORM (DTW_findPath_bandAndSlope, U"DTW: find path (band & slope)", nullptr)
    REAL (U"Sakoe-Chiba band (s)", U"0.05")
    RADIO (U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    double band = GET_REAL (U"Sakoe-Chiba band");
    int slope = GET_INTEGER (U"Slope constraint");
    LOOP {
        iam (DTW);
        DTW_findPath_bandAndSlope (me, band, slope, nullptr);
    }
END

FORM (DTW_to_Matrix_cummulativeDistances, U"DTW: To Matrix", nullptr)
    REAL (U"Sakoe-Chiba band (s)", U"0.05")
    RADIO (U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    double band = GET_REAL (U"Sakoe-Chiba band");
    int slope = GET_INTEGER (U"Slope constraint");
    LOOP {
        iam (DTW);
        autoMatrix thee = DTW_to_Matrix_cummulativeDistances (me, band, slope);
        praat_new (thee.move(), my name, U"_cd");
    }
END

FORM (DTW_to_Polygon, U"DTW: To Polygon...", 0)
    REAL (U"Sakoe-Chiba band (s)", U"0.1")
    RADIO (U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    double band = GET_REAL (U"Sakoe-Chiba band");
    int slope = GET_INTEGER (U"Slope constraint");
    LOOP {
        iam (DTW);
        autoPolygon thee = DTW_to_Polygon (me, band, slope);
        praat_new (thee.move(), my name);
    }
END

DIRECT (DTW_to_Matrix_distances)
	LOOP {
		iam (DTW);
		autoMatrix thee = DTW_to_Matrix_distances (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (DTW_swapAxes)
	LOOP {
		iam (DTW);
		autoDTW thee = DTW_swapAxes (me);
		praat_new (thee.move(), my name, U"_axesSwapped");
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
	autoTextGrid thee = DTW_and_TextGrid_to_TextGrid (me, tg, 0);
	praat_new (thee.move(), tg -> name, U"_", my name);
END

DIRECT (DTW_and_IntervalTier_to_Table)
	DTW me = FIRST (DTW);
	IntervalTier ti = FIRST (IntervalTier);
	autoTable thee = DTW_and_IntervalTier_to_Table (me, ti, 1.0/44100);
	praat_new (thee.move(), my name);
END

/******************** EditDistanceTable & EditCostsTable ********************************************/

DIRECT (EditDistanceTable_help)
	Melder_help (U"EditDistanceTable");
END

DIRECT (EditDistanceTable_to_TableOfReal_directions)
	LOOP {
		iam (EditDistanceTable);
		autoTableOfReal thee = EditDistanceTable_to_TableOfReal_directions (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (EditDistanceTable_setEditCosts)
	EditDistanceTable me = FIRST (EditDistanceTable);
	EditCostsTable thee = FIRST(EditCostsTable);
	EditDistanceTable_setEditCosts (me, thee);
END

FORM (EditDistanceTable_setDefaultCosts, U"", nullptr)
	REAL (U"Insertion costs", U"1.0")
	REAL (U"Deletion costs", U"1.0")
	REAL (U"Substitution costs", U"2.0")
	OK
DO
	double insertionCosts = GET_REAL (U"Insertion costs");
	if (insertionCosts < 0) {
		Melder_throw (U"Insertion costs cannot be negative.");
	}
	double deletionCosts = GET_REAL (U"Deletion costs");
	if (deletionCosts < 0) {
		Melder_throw (U"Deletion costs cannot be negative.");
	}
	double substitutionCosts = GET_REAL (U"Substitution costs");
	if (substitutionCosts < 0) {
		Melder_throw (U"Substitution costs cannot be negative.");
	}
	LOOP {
		iam (EditDistanceTable);
		EditDistanceTable_setDefaultCosts (me, insertionCosts, deletionCosts, substitutionCosts);
	}
END

FORM (EditDistanceTable_draw, U"EditDistanceTable_draw", nullptr)
	RADIO (U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (U"Precision", U"1")
	REAL (U"Rotate source labels by (degrees)", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (EditDistanceTable);
		EditDistanceTable_draw (me, GRAPHICS,
			GET_INTEGER (U"Format"),
			GET_INTEGER (U"Precision"),
			GET_REAL (U"Rotate source labels by"));
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
	Melder_help (U"EditCostsTable");
END

FORM (EditCostsTable_getTargetIndex, U"EditCostsTable: Get target index", nullptr)
	SENTENCE (U"Target", U"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getTargetIndex (me,
			GET_STRING (U"Target")), nullptr);
	}
END

FORM (EditCostsTable_getSourceIndex, U"EditCostsTable: Get source index", nullptr)
	SENTENCE (U"Source", U"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getSourceIndex (me,
			GET_STRING (U"Source")), nullptr);
	}
END

FORM (EditCostsTable_getInsertionCost, U"EditCostsTable: Get insertion cost", nullptr)
	SENTENCE (U"Target", U"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getInsertionCost (me,
			GET_STRING (U"Target")), nullptr);
	}
END

FORM (EditCostsTable_getDeletionCost, U"EditCostsTable: Get deletion cost", nullptr)
	SENTENCE (U"Source", U"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getDeletionCost (me,
			GET_STRING (U"Source")), nullptr);
	}
END

FORM (EditCostsTable_getSubstitutionCost, U"EditCostsTable: Get substitution cost", nullptr)
	SENTENCE (U"Target", U"")
	SENTENCE (U"Source", U"")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getSubstitutionCost (me,
			GET_STRING (U"Target"),
			GET_STRING (U"Source")), nullptr);
	}
END

FORM (EditCostsTable_getOthersCost, U"EditCostsTable: Get cost (others)", nullptr)
	RADIO (U"Others cost type", 1)
		RADIOBUTTON (U"Insertion")
		RADIOBUTTON (U"Deletion")
		RADIOBUTTON (U"Equality")
		RADIOBUTTON (U"Inequality")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		Melder_informationReal (EditCostsTable_getOthersCost (me,
			GET_INTEGER (U"Others cost type")), nullptr);
	}
END

FORM (EditCostsTable_setTargetSymbol_index, U"EditCostsTable: Set target symbol (index)", nullptr)
	NATURAL (U"Index", U"1")
	SENTENCE (U"Target", U"a")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setRowLabel (me,
			GET_INTEGER (U"Index"),
			GET_STRING (U"Target"));
	}
END

FORM (EditCostsTable_setSourceSymbol_index, U"EditCostsTable: Set source symbol (index)", nullptr)
	NATURAL (U"Index", U"1")
	SENTENCE (U"Source", U"a")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setColumnLabel (me,
			GET_INTEGER (U"Index"),
			GET_STRING (U"Source"));
	}
END

FORM (EditCostsTable_setInsertionCosts, U"EditCostsTable: Set insertion costs", nullptr)
	SENTENCE (U"Targets", U"")
	REAL (U"Cost", U"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setInsertionCosts (me,
			GET_STRING (U"Targets"),
			GET_REAL (U"Cost"));
	}
END

FORM (EditCostsTable_setDeletionCosts, U"EditCostsTable: Set deletion costs", nullptr)
	SENTENCE (U"Sources", U"")
	REAL (U"Cost", U"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setDeletionCosts (me,
			GET_STRING (U"Sources"),
			GET_REAL (U"Cost"));
	}
END

FORM (EditCostsTable_setSubstitutionCosts, U"EditCostsTable: Set substitution costs", nullptr)
	SENTENCE (U"Targets", U"a i u")
	SENTENCE (U"Sources", U"a i u")
	REAL (U"Cost", U"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setSubstitutionCosts (me,
			GET_STRING (U"Targets"),
			GET_STRING (U"Sources"),
			GET_REAL (U"Cost"));
	}
END

FORM (EditCostsTable_setOthersCosts, U"EditCostsTable: Set costs (others)", nullptr)
	LABEL (U"", U"Others costs")
	REAL (U"Insertion", U"1.0")
	REAL (U"Deletion", U"1.0")
	LABEL (U"", U"Substitution costs")
	REAL (U"Equality", U"0.0")
	REAL (U"Inequality", U"2.0")
	OK
DO
	LOOP {
		iam (EditCostsTable);
		EditCostsTable_setOthersCosts (me,
			GET_REAL (U"Insertion"),
			GET_REAL (U"Deletion"),
			GET_REAL (U"Equality"),
			GET_REAL (U"Inequality"));
	}
END

DIRECT (EditCostsTable_to_TableOfReal)
	LOOP {
		iam (EditCostsTable);
		autoTableOfReal result = EditCostsTable_to_TableOfReal (me);
		praat_new (result.move(), my name);
	}
END

FORM (EditCostsTable_createEmpty, U"Create empty EditCostsTable", U"Create empty EditCostsTable...")
	SENTENCE (U"Name", U"editCosts")
	INTEGER (U"Number of target symbols", U"0")
	INTEGER (U"Number of source symbols", U"0")
	OK
DO
	long numberOfTargetSymbols = GET_INTEGER (U"Number of target symbols");
	numberOfTargetSymbols = numberOfTargetSymbols < 0 ? 0 : numberOfTargetSymbols;
	long numberOfSourceSymbols = GET_INTEGER (U"Number of source symbols");
	numberOfSourceSymbols = numberOfSourceSymbols < 0 ? 0 : numberOfSourceSymbols;
	autoEditCostsTable result = EditCostsTable_create (numberOfTargetSymbols, numberOfSourceSymbols);
	praat_new (result.move(), GET_STRING (U"Name"));
END

/******************** Eigen ********************************************/

DIRECT (Eigen_drawEigenvalues_scree)
	Melder_warning (U"The command \"Draw eigenvalues (scree)...\" has been "
		"removed.\n To get a scree plot use \"Draw eigenvalues...\" with the "
		"arguments\n 'Fraction of eigenvalues summed' and 'Cumulative' unchecked.");
END

FORM (Eigen_drawEigenvalues, U"Eigen: Draw eigenvalues", U"Eigen: Draw eigenvalues...")
	INTEGER (U"left Eigenvalue range", U"0")
	INTEGER (U"right Eigenvalue range", U"0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Fraction of eigenvalues summed", false)
	BOOLEAN (U"Cumulative", false)
	POSITIVE (U"Mark size (mm)", U"1.0")
	SENTENCE (U"Mark string (+xo.)", U"+")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Eigen);
		Eigen_drawEigenvalues (me, GRAPHICS,
			GET_INTEGER (U"left Eigenvalue range"),
			GET_INTEGER (U"right Eigenvalue range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Fraction of eigenvalues summed"),
			GET_INTEGER (U"Cumulative"),
			GET_REAL (U"Mark size"),
			GET_STRING (U"Mark string"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (Eigen_drawEigenvector, U"Eigen: Draw eigenvector", U"Eigen: Draw eigenvector...")
	INTEGER (U"Eigenvector number", U"1")
	BOOLEAN (U"Component loadings", false)
	INTEGER (U"left Element range", U"0")
	INTEGER (U"right Element range", U"0")
	REAL (U"left Amplitude range", U"-1.0")
	REAL (U"right Amplitude range", U"1.0")
	POSITIVE (U"Mark size (mm)", U"1.0")
	SENTENCE (U"Mark string (+xo.)", U"+")
	BOOLEAN (U"Connect points", true)
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Eigen);
		Eigen_drawEigenvector (me, GRAPHICS,
			GET_INTEGER (U"Eigenvector number"),
			GET_INTEGER (U"left Element range"),
			GET_INTEGER (U"right Element range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Component loadings"),
			GET_REAL (U"Mark size"),
			GET_STRING (U"Mark string"),
			GET_INTEGER (U"Connect points"),
			nullptr,   // rowLabels
			GET_INTEGER (U"Garnish"));
	}
END

DIRECT (Eigen_getNumberOfEigenvalues)
	LOOP {
		iam (Eigen);
		Melder_information (my numberOfEigenvalues);
	}
END

DIRECT (Eigen_getDimension)
	LOOP {
		iam (Eigen);
		Melder_information (my dimension);
	}
END

FORM (Eigen_getEigenvalue, U"Eigen: Get eigenvalue", U"Eigen: Get eigenvalue...")
	NATURAL (U"Eigenvalue number", U"1")
	OK
DO
	LOOP {
		iam (Eigen);
		long number = GET_INTEGER (U"Eigenvalue number");
		if (number > my numberOfEigenvalues) {
			Melder_throw (U"Eigenvalue number must be smaller than ", my numberOfEigenvalues + 1);
		}
		Melder_information (my eigenvalues[number]);
	}
END

FORM (Eigen_getSumOfEigenvalues, U"Eigen:Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...")
	INTEGER (U"left Eigenvalue range",  U"0")
	INTEGER (U"right Eigenvalue range", U"0")
	OK
DO
	LOOP {
		iam (Eigen);
		Melder_information (Eigen_getSumOfEigenvalues (me, GET_INTEGER (U"left Eigenvalue range"), GET_INTEGER (U"right Eigenvalue range")));
	}
END

FORM (Eigen_getEigenvectorElement, U"Eigen: Get eigenvector element", U"Eigen: Get eigenvector element...")
	NATURAL (U"Eigenvector number", U"1")
	NATURAL (U"Element number", U"1")
	OK
DO
	LOOP {
		iam (Eigen);
		Melder_information (Eigen_getEigenvectorElement (me, GET_INTEGER (U"Eigenvector number"), GET_INTEGER (U"Element number")));
	}
END

DIRECT (Eigens_alignEigenvectors)
	OrderedOf<structEigen> list;
	LOOP {
		iam (Eigen);
		list. addItem_ref (me);
	}
	Eigens_alignEigenvectors (& list);
END

FORM (Eigen_and_Matrix_projectColumns, U"Eigen & Matrix: Project columns", U"Eigen & Matrix: Project...")
	INTEGER (U"Number of dimensions", U"0")
	OK
DO
	Eigen me = FIRST_GENERIC (Eigen);
	Matrix thee = FIRST_GENERIC (Matrix);
	autoMatrix him = Eigen_and_Matrix_to_Matrix_projectColumns (me, thee, GET_INTEGER (U"Number of dimensions"));
	praat_new (him.move(), my name, U"_", thy name);
END

DIRECT (Eigen_and_SSCP_project)
	Eigen me = FIRST_GENERIC (Eigen);
	SSCP cp = FIRST (SSCP);
	autoSSCP thee = Eigen_and_SSCP_project (me, cp);
	praat_new (thee.move(), my name, U"_", cp->name);
END

DIRECT (Eigen_and_Covariance_project)
	Eigen me = FIRST_GENERIC (Eigen);
	Covariance cv = FIRST (Covariance);
	autoCovariance thee = Eigen_and_Covariance_project (me, cv);
	praat_new (thee.move(), my name, U"_", cv->name);
END

/******************** Index ********************************************/

DIRECT (Index_help)
	Melder_help (U"Index");
END

DIRECT (Index_getNumberOfClasses)
	LOOP {
		iam (Index);
		Melder_information (my classes->size);
	}
END

FORM (StringsIndex_getClassLabel, U"StringsIndex: Get class label", U"StringsIndex: Get class label...")
	NATURAL (U"Class index", U"1")
	OK
DO
	long klas = GET_INTEGER (U"Class index");
	LOOP {
		iam (StringsIndex);
		long numberOfClasses = my classes->size;
		if (klas > numberOfClasses) {
			Melder_throw (U"Index must be less than or equal ", numberOfClasses, U".");
		}
		SimpleString ss = (SimpleString) my classes->at [klas];   // FIXME cast
		Melder_information (ss -> string);
	}
END

FORM (StringsIndex_getLabel, U"StringsIndex: Get label", U"StringsIndex: Get label...")
	NATURAL (U"Element index", U"1")
	OK
DO
	long index = GET_INTEGER (U"Element index");
	LOOP {
		iam (StringsIndex);
		if (index > my numberOfElements) {
			Melder_throw (U"Index must be less than or equal ", my numberOfElements, U".");
		}
		long klas = my classIndex [index];
		SimpleString ss = (SimpleString) my classes->at [klas];   // FIXME cast
		Melder_information (ss -> string);
	}
END

FORM (Index_getIndex, U"Index: Get index", U"Index: Get index...")
	NATURAL (U"Element index", U"1")
	OK
DO
	long index = GET_INTEGER (U"Element index");
	LOOP {
		iam (Index);
		if (index > my numberOfElements) {
			Melder_throw (U"Index must be less than or equal ", my numberOfElements, U".");
		}
		Melder_information (my classIndex [index]);
	}
END

FORM (StringsIndex_getClassIndex, U"StringsIndex: Get class index", U"StringsIndex: Get class index...")
	WORD (U"Class label", U"label")
	OK
DO
	char32 *klasLabel = GET_STRING (U"Class label");
	LOOP {
		iam (StringsIndex);
		long index = StringsIndex_getClass (me, klasLabel);
		Melder_information (index);
	}
END

FORM (Index_extractPart, U"Index: Extract part", U"Index: Extract part...")
	INTEGER (U"left Range", U"0")
	INTEGER (U"right Range", U"0")
	OK
DO
	LOOP {
		iam (Index);
		autoIndex thee = Index_extractPart (me,
			GET_INTEGER (U"left Range"),
			GET_INTEGER (U"right Range"));
		praat_new (thee.move(), Thing_getName (me), U"_part");
	}
END

FORM (Index_to_Permutation, U"Index: To Permutation", U"Index: To Permutation...")
	BOOLEAN (U"Permute within classes", true)
	OK
DO
	LOOP {
		iam (Index);
		autoPermutation result = Index_to_Permutation_permuteRandomly (me,
			GET_INTEGER (U"Permute within classes"));
		praat_new (result.move(), my name);
	}
END

DIRECT (StringsIndex_to_Strings)
	LOOP {
		iam (StringsIndex);
		autoStrings result = StringsIndex_to_Strings (me);
		praat_new (result.move(), my name);
	}
END

/******************** Excitation ********************************************/

DIRECT (Excitations_to_ExcitationList)
	autoExcitationList result = ExcitationList_create ();
	LOOP {
		iam (Excitation);
		autoExcitation copy = Data_copy (me);
		result -> addItem_move (copy.move());
	}
	praat_new (result.move(), U"appended");
END

/******************** Excitations ********************************************/

FORM (ExcitationList_formula, U"ExcitationList: Formula", 0)
	LABEL (U"label", U"for all objects in ExcitationList do { for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (ExcitationList);
		for (long j = 1; j <= my size; j ++) {
			Matrix_formula (my at [j], GET_STRING (U"formula"), interpreter, nullptr);
		}
		praat_dataChanged (me);
	}
END

DIRECT (ExcitationList_addItem)
	ExcitationList list = FIRST (ExcitationList);
	WHERE_DOWN (SELECTED && CLASS == classExcitation) {
		iam (Excitation);
		autoExcitation copy = Data_copy (me);
		list -> addItem_move (copy.move());
	}
END

FORM (ExcitationList_getItem, U"ExcitationList: Get item", nullptr)
	NATURAL (U"Item number", U"1")
	OK
DO
	LOOP {
		iam (ExcitationList);
		autoExcitation result = ExcitationList_getItem (me,
			GET_INTEGER (U"Item number"));
		praat_new (result.move(), my name, U"_item");
	}
END

DIRECT (ExcitationList_append)
	ExcitationList e1 = nullptr, e2 = nullptr;
	LOOP {
		iam (ExcitationList);
		(e1 ? e2 : e1) = me;
	}
	Melder_assert (e1 && e2);
	autoExcitationList result = Data_copy (e1);
	result -> merge (e2);
	praat_new (result.move(), U"appended");
END

FORM (ExcitationList_to_Pattern, U"Excitations: To Pattern", nullptr)
	NATURAL (U"Join", U"1")
	OK
DO
	LOOP {
		iam (ExcitationList);
		autoPattern result = ExcitationList_to_Pattern (me, GET_INTEGER (U"Join"));
		praat_new (result.move(), my name);
	}
END

DIRECT (ExcitationList_to_TableOfReal)
	LOOP {
		iam (ExcitationList);
		autoTableOfReal result = ExcitationList_to_TableOfReal (me);
		praat_new (result.move(), my name);
	}
END


/************************* FileInMemory ***********************************/


FORM_READ2 (FileInMemory_create, U"Create file in memory", nullptr, true) {
	autoFileInMemory me = FileInMemory_create (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

FORM (FileInMemory_setId, U"FileInMemory: Set id", nullptr)
	SENTENCE (U"New id", U"New id")
	OK
DO
	LOOP {
		iam (FileInMemory);
		FileInMemory_setId (me, GET_STRING (U"New id"));
		praat_dataChanged (me);
	}
END

FORM (FileInMemory_showAsCode, U"FileInMemory: Show as code", nullptr)
	WORD (U"Name", U"example")
	INTEGER (U"Number of bytes per line", U"20")
	OK
DO
	const char32 *name = GET_STRING (U"Name");
	LOOP {
		iam (FileInMemory);
		MelderInfo_open ();
		FileInMemory_showAsCode (me, name, GET_INTEGER (U"Number of bytes per line"));
		MelderInfo_close ();
	}
END

/************************* FileInMemorySet ***********************************/

FORM (FileInMemorySet_createFromDirectoryContents, U"Create files in memory from directory contents", nullptr)
	SENTENCE (U"Name", U"list")
	LABEL (U"", U"Directory:")
	TEXTFIELD (U"Directory", U"/home/david/praat/src/espeak-work/espeak-1.46.13/espeak-data")
	WORD (U"Only files that match pattern", U"*.txt")
	OK
DO
	autoFileInMemorySet me = FileInMemorySet_createFromDirectoryContents (
		GET_STRING (U"Directory"),
		GET_STRING (U"Only files that match pattern"));
	praat_new (me.move(), GET_STRING (U"Name"));
END

FORM (FileInMemorySet_createCopyFromFileInMemorySet, U"", nullptr)
	OPTIONMENU (U"Espeakdata", 5)
		OPTION (U"phons")
		OPTION (U"dicts")
		OPTION (U"voices")
		OPTION (U"variants")
		OPTION (U"voices_names")
		OPTION (U"variants_names")
	OK
DO
	long choice = GET_INTEGER (U"Espeakdata");
	if (choice == 1) {
		autoFileInMemorySet f = Data_copy (espeakdata_phons.get());
		praat_new (f.move(), U"espeakdata_phons");
	}
	else if (choice == 2) {
		autoFileInMemorySet f = Data_copy (espeakdata_dicts.get());
		praat_new (f.move(), U"espeakdata_dicts");
	}
	else if (choice == 3) {
		autoFileInMemorySet f = Data_copy (espeakdata_voices.get());
		praat_new (f.move(), U"espeakdata_voices");
	}
	else if (choice == 4) {
		autoFileInMemorySet f = Data_copy (espeakdata_variants.get());
		praat_new (f.move(), U"espeakdata_variants");
	}
	else if (choice == 5) {
		autoStrings s = Data_copy (espeakdata_voices_names.get());
		praat_new (s.move(), U"espeakdata_voices_names");
	}
	else if (choice == 6) {
		autoStrings s = Data_copy (espeakdata_variants_names.get());
		praat_new (s.move(), U"espeakdata_variants_names");
	}
END

FORM (FileInMemorySet_showAsCode, U"FileInMemorySet: Show as code", nullptr)
	WORD (U"Name", U"example")
	INTEGER (U"Number of bytes per line", U"20")
	OK
DO
	LOOP {
		iam (FileInMemorySet);
		MelderInfo_open ();
		FileInMemorySet_showAsCode (me, GET_STRING (U"Name"), GET_INTEGER (U"Number of bytes per line"));
		MelderInfo_close ();
	}
END

FORM (FileInMemorySet_showOneFileAsCode, U"FileInMemorySet: Show one file as code", nullptr)
	NATURAL (U"Index", U"1")
	WORD (U"Name", U"example")
	INTEGER (U"Number of bytes per line", U"20")
	OK
DO
	LOOP {
		iam (FileInMemorySet);
		MelderInfo_open ();
		FileInMemorySet_showOneFileAsCode (me, GET_INTEGER (U"Index"), GET_STRING (U"Name"), GET_INTEGER (U"Number of bytes per line"));
		MelderInfo_close ();
	}
END

DIRECT (FileInMemory_to_FileInMemorySet)
	autoFileInMemorySet thee = FileInMemorySet_create ();
	LOOP {
		iam (FileInMemory);
		autoFileInMemory him = Data_copy (me);
		thy addItem_move (him.move());
	}
	praat_new (thee.move(), U"files");
END

DIRECT (FileInMemorySet_addItems)
	FileInMemorySet thee = FIRST (FileInMemorySet);
	LOOP {
		iam (Daata);
		if (CLASS == classFileInMemory) {
			autoFileInMemory him = Data_copy ((FileInMemory) me);
			thy addItem_move (him.move());
		}
	}
END

DIRECT (FileInMemorySets_merge)
	FileInMemorySet f1 = nullptr, f2 = nullptr;
	LOOP { 
		iam (FileInMemorySet);
		(f1 ? f2 : f1) = me;
	}
	Melder_assert (f1 && f2);
	autoFileInMemorySet result = Data_copy (f1);
	result -> merge (f2);
	praat_new (result.move(), f1 -> name, U"_", f2 -> name);
END

DIRECT (FileInMemorySet_to_Strings_id)
	LOOP {
		iam (FileInMemorySet);
		autoStrings result = FileInMemorySet_to_Strings_id (me);
		praat_new (result.move(), my name);
	}
END

/************************* FilterBank ***********************************/

FORM (FilterBank_drawFilters, U"FilterBank: Draw filters", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawRows (me, GRAPHICS,
			GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"));
	}
END

FORM (FilterBank_drawOneContour, U"FilterBank: Draw one contour", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"Height (dB)", U"40.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawOneContour (me, GRAPHICS,
			GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"),
			GET_REAL (U"Height"));
	}
END

FORM (FilterBank_drawContours, U"FilterBank: Draw contours", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawContours (me, GRAPHICS,
			GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"));
	}
END

FORM (FilterBank_drawFrequencyScales, U"FilterBank: Draw frequency scales", U"FilterBank: Draw frequency scales...")
	RADIO (U"Horizontal frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (U"left Horizontal frequency range", U"0.0")
	REAL (U"right Horizontal frequency range", U"0.0")
	RADIO (U"Vertical frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (U"left Vertical frequency range", U"0.0")
	REAL (U"right Vertical frequency range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FilterBank);
		FilterBank_drawFrequencyScales (me, GRAPHICS, GET_INTEGER (U"Horizontal frequency scale"),
			GET_REAL (U"left Horizontal frequency range"), GET_REAL (U"right Horizontal frequency range"),
			GET_INTEGER (U"Vertical frequency scale"), GET_REAL (U"left Vertical frequency range"),
			GET_REAL (U"right Vertical frequency range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (MelSpectrogram_paintImage, U"MelSpectrogram: Paint image", U"MelSpectrogram: Paint image...")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range (mel)", U"0.0")
	REAL (U"right Frequency range (mel)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (MelSpectrogram);
		BandFilterSpectrogram_paintImage (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (BarkSpectrogram_paintImage, U"BarkSpectrogram: Paint image", U"BarkSpectrogram: Paint image...")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range (bark)", U"0.0")
	REAL (U"right Frequency range (bark)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (BarkSpectrogram);
		BandFilterSpectrogram_paintImage (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (FilterBank_paintImage, U"FilterBank: Paint image", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintImage (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"));
	}
END

FORM (FilterBank_paintContours, U"FilterBank: Paint contours", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintContours (me, GRAPHICS,
			GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"));
	}
END


FORM (FilterBank_paintCells, U"FilterBank: Paint cells", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintCells (me, GRAPHICS,
			GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"));
	}
END

FORM (FilterBank_paintSurface, U"FilterBank: Paint surface", nullptr)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_paintSurface (me, GRAPHICS,
			GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"),
			30,   // elevation in degrees
			45);   // azimuth in degrees
	}
END

FORM (FilterBank_getFrequencyInHertz, U"FilterBank: Get frequency in Hertz", U"FilterBank: Get frequency in Hertz...")
	REAL (U"Frequency", U"10.0")
	RADIO (U"Unit", 2)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	OK
DO
	LOOP {
		iam (FilterBank);
		double f = FilterBank_getFrequencyInHertz (me,
			GET_REAL (U"Frequency"),
			GET_INTEGER (U"Unit"));
		Melder_informationReal (f, U"Hertz");
	}
END

FORM (FilterBank_getFrequencyInBark, U"FilterBank: Get frequency in Bark", U"FilterBank: Get frequency in Bark...")
	REAL (U"Frequency", U"93.17")
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	OK
DO
	LOOP {
		iam (FilterBank);
		double f = FilterBank_getFrequencyInBark (me,
			GET_REAL (U"Frequency"),
			GET_INTEGER (U"Unit"));
		Melder_informationReal (f, U"Bark");
	}
END

FORM (FilterBank_getFrequencyInMel, U"FilterBank: Get frequency in mel", U"FilterBank: Get frequency in mel...")
	REAL (U"Frequency", U"1000.0")
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	OK
DO
	LOOP {
		iam (FilterBank);
		double f = FilterBank_getFrequencyInMel (me,
			GET_REAL (U"Frequency"),
			GET_INTEGER (U"Unit"));
		Melder_informationReal (f, U"mel");
	}
END

FORM (FilterBank_equalizeIntensities, U"FilterBank: Equalize intensities", nullptr)
	REAL (U"Intensity (dB)", U"80.0")
	OK
DO
	LOOP {
		iam (FilterBank);
		FilterBank_equalizeIntensities (me,
			GET_REAL (U"Intensity"));
		praat_dataChanged (me);
	}
END

FORM (BandFilterSpectrogram_equalizeIntensities, U"BandFilterSpectrogram: Equalize intensities", nullptr)
	REAL (U"Intensity (dB)", U"80.0")
	OK
DO
	LOOP {
		iam (BandFilterSpectrogram);
		BandFilterSpectrogram_equalizeIntensities (me,
			GET_REAL (U"Intensity"));
		praat_dataChanged (me);
	}
END

DIRECT (FilterBank_to_Matrix)
	LOOP {
		iam (FilterBank);
		autoMatrix thee = FilterBank_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END

FORM (BandFilterSpectrogram_to_Matrix, U"BandFilterSpectrogram: To Matrix", nullptr)
	BOOLEAN (U"Convert to dB values", 1)
	OK
DO
	LOOP {
		iam (BandFilterSpectrogram);
		autoMatrix thee = BandFilterSpectrogram_to_Matrix (me,
			GET_INTEGER (U"Convert to dB values"));
		praat_new (thee.move(), my name);
	}
END

FORM (FilterBanks_crossCorrelate, U"FilterBanks: Cross-correlate", nullptr)
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	FilterBank f1 = nullptr, f2 = nullptr;
	LOOP {
		iam (FilterBank);
		(f1 ? f2 : f1) = me;
	}
	Melder_assert (f1 && f2);
	autoSound result = FilterBanks_crossCorrelate (f1, f2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	praat_new (result.move(), f1 -> name, U"_", f2 -> name);
END

FORM (BandFilterSpectrograms_crossCorrelate, U"BandFilterSpectrograms: Cross-correlate", nullptr)
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	BandFilterSpectrogram f1 = nullptr, f2 = nullptr;
	LOOP {
		iam (BandFilterSpectrogram); 
		(f1 ? f2 : f1) = me;
	}
	Melder_assert (f1 && f2);
	autoSound result = BandFilterSpectrograms_crossCorrelate (f1, f2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	praat_new (result.move(), f1 -> name, U"_", f2 -> name);
END

FORM (FilterBanks_convolve, U"FilterBanks: Convolve", nullptr)
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	FilterBank f1 = nullptr, f2 = nullptr;
	LOOP { iam (FilterBank); (f1 ? f2 : f1) = me; }
	Melder_assert (f1 && f2);
	autoSound result = FilterBanks_convolve (f1, f2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	praat_new (result.move(), f1 -> name, U"_", f2 -> name);
END

FORM (BandFilterSpectrograms_convolve, U"BandFilterSpectrograms: Convolve", nullptr)
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	BandFilterSpectrogram f1 = nullptr, f2 = nullptr;
	LOOP {
		iam (BandFilterSpectrogram);
		(f1 ? f2 : f1) = me;
	}
	Melder_assert (f1 && f2);
	autoSound result = BandFilterSpectrograms_convolve (f1, f2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	praat_new (result.move(), f1 -> name, U"_", f2 -> name);
END

DIRECT (FilterBank_to_Intensity)
	LOOP {
		iam (FilterBank);
		autoIntensity result = FilterBank_to_Intensity (me);
		praat_new (result.move(), my name);
	}
END

DIRECT (BandFilterSpectrogram_to_Intensity)
	LOOP {
		iam (BandFilterSpectrogram);
		autoIntensity result = BandFilterSpectrogram_to_Intensity (me);
		praat_new (result.move(), my name);
	}
END

/*********** FormantFilter *******************************************/

DIRECT (FormantFilter_help)
	Melder_help (U"FormantFilter");
END

FORM (FormantFilter_drawFilterFunctions, U"FormantFilter: Draw filter functions", U"FilterBank: Draw filter functions...")
	INTEGER (U"left Filter range", U"0")
	INTEGER (U"right Filter range", U"0")
	POSITIVE (U"Bandwidth (Hz)", U"100.0")
	RADIO (U"Frequency scale", 1)
	RADIOBUTTON (U"Hertz")
	RADIOBUTTON (U"Bark")
	RADIOBUTTON (U"mel")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	BOOLEAN (U"Amplitude scale in dB", 1)
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantFilter);
		FormantFilter_drawFilterFunctions (me, GRAPHICS, GET_REAL (U"Bandwidth"), GET_INTEGER (U"Frequency scale"),
			GET_INTEGER (U"left Filter range"), GET_INTEGER (U"right Filter range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Amplitude scale in dB"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (FormantFilter_drawSpectrum, U"FormantFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...")
	POSITIVE (U"Time (s)", U"0.1")
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FilterBank);
		FilterBank_drawTimeSlice (me, GRAPHICS, GET_REAL (U"Time"), GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), U"Hz", GET_INTEGER (U"Garnish"));
	}
END

/****************** FormantGrid  *********************************/

FORM (old_FormantGrid_draw, U"FormantGrid: Draw", 0)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (=all)")
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0 (=auto)")
	BOOLEAN (U"Bandwidths", false)
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FormantGrid);
		FormantGrid_draw (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Bandwidths"), GET_INTEGER (U"Garnish"), U"lines and speckles");
	}
END

FORM (FormantGrid_draw, U"FormantGrid: Draw", 0)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (=all)")
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0 (=auto)")
	BOOLEAN (U"Bandwidths", false)
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
	OPTION (U"lines")
	OPTION (U"speckles")
	OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (old_FormantGrid_draw)
	autoPraatPicture picture;
	LOOP {
		iam (FormantGrid);
		FormantGrid_draw (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Bandwidths"), GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END

/****************** FunctionTerms  *********************************/

FORM (FunctionTerms_draw, U"FunctionTerms: Draw", 0)
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Extrapolate", 0)
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FunctionTerms);
		FunctionTerms_draw (me, GRAPHICS, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Extrapolate"), GET_INTEGER (U"Garnish"));
	}
END

FORM (FunctionTerms_drawBasisFunction, U"FunctionTerms: Draw basis function", 0)
	NATURAL (U"Index", U"1")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Extrapolate", 0)
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FunctionTerms);
		FunctionTerms_drawBasisFunction (me, GRAPHICS, GET_INTEGER (U"Index"), GET_REAL (U"Xmin"),
			GET_REAL (U"Xmax"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Extrapolate"), GET_INTEGER (U"Garnish"));
	}
END

FORM (FunctionTerms_evaluate, U"FunctionTerms: Evaluate", 0)
	REAL (U"X", U"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		Melder_information (FunctionTerms_evaluate (me, GET_REAL (U"X")));
	}
END

DIRECT (FunctionTerms_getNumberOfCoefficients)
	LOOP {
		iam (FunctionTerms);
		Melder_information (my numberOfCoefficients);
	}
END

FORM (FunctionTerms_getCoefficient, U"FunctionTerms: Get coefficient", 0)
	LABEL (U"", U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURAL (U"Index", U"1")
	OK
DO
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (FunctionTerms);
		if (index > my numberOfCoefficients) {
			Melder_throw (U"Index too large.");
		}
		Melder_information (my coefficients[index]);
	}
END

DIRECT (FunctionTerms_getDegree)
	LOOP {
		iam (FunctionTerms);
		Melder_information (FunctionTerms_getDegree (me));
	}
END

FORM (FunctionTerms_getMaximum, U"FunctionTerms: Get maximum", U"Polynomial: Get maximum...")
	LABEL (U"", U"Interval")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getMaximum (me, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"));
		Melder_information (x);
	}
END

FORM (FunctionTerms_getMinimum, U"FunctionTerms: Get minimum", U"Polynomial: Get minimum...")
	LABEL (U"", U"Interval")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getMinimum (me, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"));
		Melder_information (x);
	}
END

FORM (FunctionTerms_getXOfMaximum, U"FunctionTerms: Get x of maximum", U"Polynomial: Get x of maximum...")
	LABEL (U"", U"Interval")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getXOfMaximum (me, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"));
		Melder_information (x);
	}
END

FORM (FunctionTerms_getXOfMinimum, U"FunctionTerms: Get x of minimum", U"Polynomial: Get x of minimum...")
	LABEL (U"", U"Interval")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		double x = FunctionTerms_getXOfMinimum (me, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"));
		Melder_information (x);
	}
END

FORM (FunctionTerms_setCoefficient, U"FunctionTerms: Set coefficient", 0)
	LABEL (U"", U"p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	LABEL (U"", U"F[k] is of degree k")
	NATURAL (U"Index", U"1")
	REAL (U"Value", U"0.0")
	OK
DO
	LOOP {
		iam (FunctionTerms);
		FunctionTerms_setCoefficient (me, GET_INTEGER (U"Index"), GET_REAL (U"Value"));
	}
END

FORM (FunctionTerms_setDomain, U"FunctionTerms: Set domain", 0)
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"2.0")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	LOOP {
		iam (FunctionTerms);
		if (xmax <= xmin) {
			Melder_throw (U"Xmax should be larger than Xmin.");
		}
		FunctionTerms_setDomain (me, xmin, xmax);
	}
END

/***************** Intensity ***************************************************/

FORM (Intensity_to_TextGrid_detectSilences, U"Intensity: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...")
	REAL (U"Silence threshold (dB)", U"-25.0")
	POSITIVE (U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (U"Minimum sounding interval duration (s)", U"0.05")
	WORD (U"Silent interval label", U"silent")
	WORD (U"Sounding interval label", U"sounding")
	OK
DO
	LOOP {
		iam (Intensity);
		autoTextGrid thee = Intensity_to_TextGrid_detectSilences (me, GET_REAL (U"Silence threshold"),
			GET_REAL (U"Minimum silent interval duration"), GET_REAL (U"Minimum sounding interval duration"),
			GET_STRING (U"Silent interval label"), GET_STRING (U"Sounding interval label"));
		praat_new (thee.move(), my name);
	}
END

/***************** IntensityTier ***************************************************/

FORM (IntensityTier_to_TextGrid_detectSilences, U"IntensityTier: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...")
	REAL (U"Silence threshold (dB)", U"-25.0")
	POSITIVE (U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (U"Minimum sounding interval duration (s)", U"0.05")
	WORD (U"Silent interval label", U"silent")
	WORD (U"Sounding interval label", U"sounding")
	POSITIVE (U"Time step (s)", U"0.001")
	OK
DO
	LOOP {
		iam (IntensityTier);
		autoTextGrid thee = IntensityTier_to_TextGrid_detectSilences (me, GET_REAL (U"Time step"), GET_REAL (U"Silence threshold"),
			GET_REAL (U"Minimum silent interval duration"), GET_REAL (U"Minimum sounding interval duration"),
			GET_STRING (U"Silent interval label"), GET_STRING (U"Sounding interval label"));
		praat_new (thee.move(), my name);
	}
END

FORM (IntensityTier_to_Intensity, U"", 0)
	POSITIVE (U"Time step (s)", U"0.001")
	OK
DO
	LOOP {
		iam (IntensityTier);
		autoIntensity thee = IntensityTier_to_Intensity (me, GET_REAL (U"Time step"));
		praat_new (thee.move(), my name);
	}
END

/***************** ISpline ***************************************************/

DIRECT (ISpline_help) Melder_help (U"ISpline"); END

FORM (ISpline_create, U"Create ISpline", U"Create ISpline...")
	WORD (U"Name", U"ispline")
	LABEL (U"", U"Domain")
	REAL (U"Xmin", U"0")
	REAL (U"Xmax", U"1")
	LABEL (U"", U"ISpline(x) = c[1] I[1](x) + c[2] I[1](x) + ... c[n] I[n](x)")
	LABEL (U"", U"all I[k] are polynomials of degree \"Degree\"")
	LABEL (U"", U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree")
	INTEGER (U"Degree", U"3")
	SENTENCE (U"Coefficients (c[k])", U"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE (U"Interior knots" , U"0.3 0.5 0.6")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	long degree = GET_INTEGER (U"Degree");
	if (xmax <= xmin) {
		Melder_throw (U"Xmin should be smaller than Xmax.");
	}
	autoISpline me = ISpline_createFromStrings (xmin, xmax, degree,
		GET_STRING (U"Coefficients"), GET_STRING (U"Interior knots"));
	praat_new (me.move(), GET_STRING (U"Name"));
END

/******************* KlattTable  *********************************/

DIRECT (KlattTable_help) Melder_help (U"KlattTable"); END

DIRECT (KlattTable_createExample)
	autoKlattTable thee = KlattTable_createExample ();
	praat_new (thee.move(), U"example");
END

FORM (KlattTable_to_Sound, U"KlattTable: To Sound", U"KlattTable: To Sound...")
	POSITIVE (U"Sampling frequency", U"16000")
	RADIO (U"Synthesis model", 1)
	RADIOBUTTON (U"Cascade")
	RADIOBUTTON (U"Parallel")
	NATURAL (U"Number of formants", U"5")
	POSITIVE (U"Frame duration (s)", U"0.005")
	REAL (U"Flutter percentage (%)", U"0.0")   // ppgb: foutgevoelig
	OPTIONMENU (U"Voicing source", 1)
	OPTION (U"Impulsive")
	OPTION (U"Natural")
	OPTIONMENU (U"Output type", 1)
	OPTION (U"Sound")
	OPTION (U"Voicing")
	OPTION (U"Aspiration")
	OPTION (U"Frication")
	OPTION (U"Cascade-glottal-output")
	OPTION (U"Parallel-glottal-output")
	OPTION (U"Bypass-output")
	OPTION (U"All-excitations")
	OK
DO
	double flutter = GET_REAL (U"Flutter percentage");
	int outputType = GET_INTEGER (U"Output type") - 1;
	if (flutter < 0.0 || flutter > 100.0) {
		Melder_throw (U"Flutter should be between 0 and 100%.");
	}
	LOOP {
		iam (KlattTable);
		autoSound thee = KlattTable_to_Sound (me, GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Synthesis model"),
			GET_INTEGER (U"Number of formants"), GET_REAL (U"Frame duration"), GET_INTEGER (U"Voicing source"),
			GET_REAL (U"Flutter percentage"), outputType);
		praat_new (thee.move(), my name);
	}
END

FORM (KlattTable_to_KlattGrid, U"KlattTable: To KlattGrid", 0)
	POSITIVE (U"Frame duration (s)", U"0.002")
	OK
DO
	LOOP {
		iam (KlattTable);
		praat_new (KlattTable_to_KlattGrid (me, GET_REAL (U"Frame duration")), my name);
	}
END

DIRECT (KlattTable_to_Table)
	LOOP {
		iam (KlattTable);
		autoTable thee = KlattTable_to_Table (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Table_to_KlattTable)
	LOOP {
		iam (Table);
		autoKlattTable thee = Table_to_KlattTable (me);
		praat_new (thee.move(), my name);
	}
END

FORM (Table_getMedianAbsoluteDeviation, U"Table: Get median absolute deviation", U"Table: Get median absolute deviation...")
	SENTENCE (U"Column label", U"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		double mad = Table_getMedianAbsoluteDeviation (me, icol);
		Melder_information (mad);
	}
END

static void print_means (Table me);
static void print_means (Table me) {
	Table_numericize_Assert (me, 2);
	Table_numericize_Assert (me, 3);
	if (my numberOfColumns < 3) {
		MelderInfo_writeLine (U"Table does not have the right format.");
		return;
	}
	MelderInfo_writeLine (
		Melder_padOrTruncate (15, my columnHeaders[1].label), U"\t",
		Melder_padOrTruncate (15, my columnHeaders[2].label), U"\t",
		Melder_padOrTruncate (15, my columnHeaders[3].label));
	for (long irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		MelderInfo_writeLine (
			Melder_padOrTruncate (15, row -> cells[1].string), U"\t",
			Melder_padOrTruncate (15, Melder_double (row -> cells[2].number)), U"\t",
			Melder_padOrTruncate (15, Melder_double (row -> cells[3].number)));
	}
}

FORM (Table_getNumberOfRowsWhere, U"", 0)
	LABEL (U"", U"Count only rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"M\"")
	OK
DO
	LOOP {
		iam (Table);
		long numberOfRows = Table_getNumberOfRowsWhere (me, GET_STRING (U"Formula"), interpreter);
		Melder_information (numberOfRows);
	}
END

FORM (Table_reportOneWayAnova, U"Table: Report one-way anova",  U"Table: Report one-way anova...")
	SENTENCE (U"Column with data", U"F0")
	SENTENCE (U"Factor", U"Vowel")
	BOOLEAN (U"Table with means", false);
	BOOLEAN (U"Table with differences between means", false)
	BOOLEAN (U"Table with Tukey's post-hoc HSD test", false)
	OK
DO
	char32 *factor = GET_STRING (U"Factor");
	char32 *dataLabel = GET_STRING (U"Column with data");
	bool getMeans = GET_INTEGER (U"Table with means");
	bool getMeansDiff = GET_INTEGER (U"Table with differences between means");
	bool getMeansDiffProbabilities = GET_INTEGER (U"Table with Tukey's post-hoc HSD test");
	LOOP {
		iam (Table);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factor);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataLabel);
		autoTable means, meansDiff, meansDiffProbabilities;
		autoTable anova = Table_getOneWayAnalysisOfVarianceF (me, dataColumn, factorColumn, &means, &meansDiff, & meansDiffProbabilities);
		MelderInfo_open ();
		MelderInfo_writeLine (U"One-way analysis of \"", dataLabel, U"\" by \"", factor, U"\".\n");
		Table_printAsAnovaTable (anova.get());
		MelderInfo_writeLine (U"\nMeans:\n");
		print_means (means.get());
		MelderInfo_close ();
		if (getMeans) {
			praat_new (means.move(), my name, U"_groupMeans");
		}
		if (getMeansDiff) {
			praat_new (meansDiff.move(), my name, U"_meansDiff");
		}
		if (getMeansDiffProbabilities) {
			praat_new (meansDiffProbabilities.move(), my name, U"_meansDiffP");
		}
	}
END

FORM (Table_reportTwoWayAnova, U"Table: Report two-way anova", U"Table: Report two-way anova...")
	SENTENCE (U"Column with data", U"Data")
	SENTENCE (U"First factor", U"A")
	SENTENCE (U"Second factor", U"B")
	BOOLEAN (U"Table with means", 0);
	OK
DO
	char32 *factorA = GET_STRING (U"First factor");
	char32 *factorB = GET_STRING (U"Second factor");
	char32 *dataLabel = GET_STRING (U"Column with data");
	bool getMeans = GET_INTEGER (U"Table with means");
	LOOP {
		iam (Table);
		long factorColumnA = Table_getColumnIndexFromColumnLabel (me, factorA);
		long factorColumnB = Table_getColumnIndexFromColumnLabel (me, factorB);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataLabel);
		autoTable means, sizes;
		autoTable anova = Table_getTwoWayAnalysisOfVarianceF (me, dataColumn, factorColumnA, factorColumnB, &means, &sizes);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Two-way analysis of \"", dataLabel, U"\" by \"", factorA, U"\" and \"", factorB, U".\n");
		Table_printAsAnovaTable (anova.get());
		MelderInfo_writeLine (U"\nMeans:\n");
		Table_printAsMeansTable (means.get());
		MelderInfo_writeLine (U"\nCell sizes:\n");
		Table_printAsMeansTable (sizes.get());
		MelderInfo_close ();
		if (getMeans) {
			praat_new (means.move(), my name, U"_groupMeans");
		}
	}
END

FORM (Table_reportOneWayKruskalWallis, U"Table: Report one-way Kruskal-Wallis", U"Table: Report one-way Kruskal-Wallis...")
	SENTENCE (U"Column with data", U"Data")
	SENTENCE (U"Factor", U"Group")
	OK
DO
	char32 *factor = GET_STRING (U"Factor");
	char32 *dataLabel = GET_STRING (U"Column with data");
	LOOP {
		iam (Table);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factor);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataLabel);
		double df, kruskalWallis, prob;
		autoTable thee = Table_getOneWayKruskalWallis (me, dataColumn, factorColumn, & prob, & kruskalWallis, & df);
		MelderInfo_open ();
		MelderInfo_writeLine (U"One-way Kruskal-Wallis of \"", dataLabel, U"\" by \"", factor, U"\".\n");
		MelderInfo_writeLine (U"Chi squared: ", kruskalWallis);
		MelderInfo_writeLine (U"Degrees of freedom: ", df);
		MelderInfo_writeLine (U"Probability: ", prob);
		MelderInfo_writeLine (U"\nMeans:\n");
		print_means (thee.get());
		MelderInfo_close ();
		//praat_new (move(), my name, U"_groupMeans");
	}
END

FORM (Table_to_StringsIndex_column, U"Table: To StringsIndex (column)", 0)
	SENTENCE (U"Column label", U"")
	OK
DO
	char32 *columnLabel = GET_STRING (U"Column label");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoStringsIndex thee = Table_to_StringsIndex_column (me, icol);
		praat_new (thee.move(), my name, U"_", columnLabel);
	}
END

/******************* LegendreSeries *********************************/

FORM (LegendreSeries_create, U"Create LegendreSeries", U"Create LegendreSeries...")
	WORD (U"Name", U"ls")
	LABEL (U"", U"Domain")
	REAL (U"Xmin", U"-1")
	REAL (U"Xmax", U"1")
	LABEL (U"", U"LegendreSeries(x) = c[1] P[0](x) + c[2] P[1](x) + ... c[n+1] P[n](x)")
	LABEL (U"", U"P[k] is a Legendre polynomial of degree k")
	SENTENCE (U"Coefficients", U"0 0 1.0")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	if (xmin >= xmax) {
		Melder_throw (U"Xmin must be smaller than Xmax.");
	}
	praat_new (LegendreSeries_createFromString (xmin, xmax, GET_STRING (U"Coefficients")), GET_STRING (U"Name"));
END

DIRECT (LegendreSeries_help) Melder_help (U"LegendreSeries"); END

DIRECT (LegendreSeries_to_Polynomial)
	LOOP {
		iam (LegendreSeries);
		praat_new (LegendreSeries_to_Polynomial (me), my name);
	}
END
/********************* LongSound **************************************/

FORM_READ2 (LongSounds_appendToExistingSoundFile, U"LongSound: Append to existing sound file", 0, false) {
	OrderedOf<structSampled> list;
	LOOP {
		iam (Sampled);
		list. addItem_ref (me);
	}
	LongSounds_appendToExistingSoundFile (& list, file);
END2 }

FORM_WRITE (LongSounds_writeToStereoAiffFile, U"LongSound: Save as AIFF file", 0, U"aiff")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_AIFF, file);
END

FORM_WRITE (LongSounds_writeToStereoAifcFile, U"LongSound: Save as AIFC file", 0, U"aifc")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_AIFC, file);
END

FORM_WRITE (LongSounds_writeToStereoWavFile, U"LongSound: Save as WAV file", 0, U"wav")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_WAV, file);
END

FORM_WRITE (LongSounds_writeToStereoNextSunFile, U"LongSound: Save as NeXT/Sun file", 0, U"au")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_NEXT_SUN, file);
END

FORM_WRITE (LongSounds_writeToStereoNistFile, U"LongSound: Save as NIST file", 0, U"nist")
	LongSound s1 = 0, s2 = 0;
	LOOP { iam (LongSound); (s1 ? s2 : s1) = me; }
	Melder_assert (s1 != 0 && s2 != 0);
	LongSounds_writeToStereoAudioFile16 (s1, s2, Melder_NIST, file);
END

/******************* Matrix **************************************************/

FORM (Matrix_drawAsSquares, U"Matrix: Draw as squares", U"Matrix: Draw as squares...")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawAsSquares (me, GRAPHICS, GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Matrix_drawDistribution, U"Matrix: Draw distribution", U"Matrix: Draw distribution...")
	LABEL (U"", U"Selection of (part of) Matrix")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	LABEL (U"", U"Selection of Matrix values")
	REAL (U"Minimum value", U"0.0")
	REAL (U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURAL (U"Number of bins", U"10")
	REAL (U"Minimum frequency", U"0.0")
	REAL (U"Maximum frequency", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawDistribution (me, GRAPHICS, GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"), GET_REAL (U"Minimum value"), GET_REAL (U"Maximum value"),
			GET_INTEGER (U"Number of bins"), GET_REAL (U"Minimum frequency"), GET_REAL (U"Maximum frequency"), false,
			GET_INTEGER (U"Garnish"));
	}
END

FORM (Matrix_drawCumulativeDistribution, U"Matrix: Draw cumulative distribution", U"")
	LABEL (U"", U"Selection of (part of) Matrix")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	LABEL (U"", U"Selection of Matrix values")
	REAL (U"Minimum value", U"0.0")
	REAL (U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURAL (U"Number of bins", U"10")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		Matrix_drawDistribution (me, GRAPHICS, GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Minimum value"), GET_REAL (U"Maximum value"), GET_INTEGER (U"Number of bins"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), true, GET_INTEGER (U"Garnish"));
	}
END

FORM (Matrix_getMean, U"Matrix: Get mean", nullptr)
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double mean = Matrix_getMean (me, GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"));
	Melder_informationReal (mean, nullptr);
END

FORM (Matrix_getStandardDeviation, U"Matrix: Get standard deviation", nullptr)
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double stdev = Matrix_getStandardDeviation (me, GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"));
	Melder_informationReal (stdev, nullptr);
END

FORM (Matrix_scale, U"Matrix: Scale", nullptr)
	LABEL (U"", U"self[row, col] := self[row, col] / `Scale factor'")
	RADIO (U"Scale factor", 1)
	RADIOBUTTON (U"Extremum in matrix")
	RADIOBUTTON (U"Extremum in each row")
	RADIOBUTTON (U"Extremum in each column")
	OK
DO
	int scale = GET_INTEGER (U"Scale factor");
	if (scale < 1 || scale > 3) {
		Melder_throw (U"Scale must be in (1,3) interval.");
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
		autoMatrix thee = Matrix_transpose (me);
		praat_new (thee.move(), my name, U"_transposed");
	}
END

DIRECT (Matrix_to_PCA_byColumns)
	LOOP {
		iam (Matrix);
		autoPCA thee = Matrix_to_PCA_byColumns (me);
		praat_new (thee.move(), my name, U"_columns");
	}
END
DIRECT (Matrix_to_PCA_byRows)
	LOOP {
		iam (Matrix);
		autoPCA thee = Matrix_to_PCA_byRows (me);
		praat_new (thee.move(), my name, U"_rows");
	}
END

FORM (Matrix_solveEquation, U"Matrix: Solve equation", U"Matrix: Solve equation...")
	REAL (U"Tolerance", U"1.19e-7")
	OK
DO
	LOOP {
		iam (Matrix);
		autoMatrix thee = Matrix_solveEquation (me, GET_REAL (U"Tolerance"));
		praat_new (thee.move(), Thing_getName (me), U"_solution");
	}
END

DIRECT (Matrix_Categories_to_TableOfReal)
	Matrix me = FIRST_GENERIC (Matrix);
	Categories cat = FIRST (Categories);
	autoTableOfReal thee = Matrix_and_Categories_to_TableOfReal (me, cat);
	praat_new (thee.move(), my name, U"_", cat -> name);
END

FORM (Matrix_scatterPlot, U"Matrix: Scatter plot", 0)
	NATURAL (U"Column for X-axis", U"1")
	NATURAL (U"Column for Y-axis", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	POSITIVE (U"Mark size (mm)", U"1.0")
	SENTENCE (U"Mark string (+xo.)", U"+")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	long x = GET_INTEGER (U"Column for X-axis");
	long y = GET_INTEGER (U"Column for Y-axis");
	if (x == 0 || y == 0) {
		Melder_throw (U"X and Y component must differ from 0.");
	}
	LOOP {
		iam (Matrix);
		Matrix_scatterPlot (me, GRAPHICS, x, y, GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"), GET_REAL (U"Mark size"), GET_STRING (U"Mark string"),
			GET_INTEGER (U"Garnish"));
	}
END

DIRECT (Matrix_to_Activation)
	LOOP {
		iam (Matrix);
		autoActivation thee = Matrix_to_Activation (me);
		praat_new (thee.move(), my name);
	}
END

FORM (Matrices_to_DTW, U"Matrices: To DTW", U"Matrix: To DTW...")
	LABEL (U"", U"Distance  between cepstral coefficients")
	REAL (U"Distance metric", U"2.0")
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
	autoDTW thee = Matrices_to_DTW (m1, m2, begin, end, slope, GET_REAL (U"Distance metric"));
	praat_new (thee.move(), m1->name, U"_", m2->name);
END

FORM (Matrix_to_Pattern, U"Matrix: To Pattern", 0)
	NATURAL (U"Join", U"1")
	OK
DO
	LOOP {
		iam (Matrix);
		praat_new (Matrix_to_Pattern (me, GET_INTEGER (U"Join")), my name);
	}
END

/**** Filterbank (deprecated) *******/

DIRECT (FilterBank_getHighestFrequency)
	LOOP {
		iam (FilterBank);
		Melder_information (my ymax, U" ", my v_getFrequencyUnit ());
	}
END

DIRECT (FilterBank_getLowestFrequency)
	LOOP {
		iam (FilterBank);
		Melder_information (my ymin, U" ", my v_getFrequencyUnit ());
	}
END

DIRECT (FilterBank_getNumberOfFrequencies)
	LOOP {
		iam (FilterBank);
		Melder_information (my ny);
	}
END

DIRECT (FilterBank_getFrequencyDistance)
	LOOP {
		iam (FilterBank);
		Melder_information (my dy, U" ", my v_getFrequencyUnit ());
	}
END

FORM (FilterBank_getXofColumn, U"Get time of column", 0)
	NATURAL (U"Column number", U"1")
	OK
DO
	LOOP {
		iam (FilterBank);
		Melder_information (Matrix_columnToX (me, GET_INTEGER (U"Column number")));
	}
END

FORM (FilterBank_getFrequencyOfRow, U"Get frequency of row", 0)
	NATURAL (U"Row number", U"1")
	OK
DO
	LOOP {
		iam (FilterBank);
		Melder_information (Matrix_rowToY (me, GET_INTEGER (U"Row number")), U" ", my v_getFrequencyUnit ());
	}
END

FORM (FilterBank_getValueInCell, U"Get value in cell", 0)
	POSITIVE (U"Time (s)", U"0.5")
	POSITIVE (U"Frequency", U"1")
	OK
DO
	double t = GET_REAL (U"Time");
	double f = GET_REAL (U"Frequency");
	LOOP {
		iam (FilterBank);
		if (f < my ymin || f > my ymax) {
			Melder_throw (U"Frequency out of range.");
		}
		if (t < my xmin || t > my xmax) {
			Melder_throw (U"Time out of range.");
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
		Melder_information (my z[row][col], U" (delta t: ", ta - t, U" f: ",
		fa - f, U")");
	}
END

/***** MATRIXFT *************/

DIRECT (BandFilterSpectrogram_getHighestFrequency)
	LOOP {
		iam (BandFilterSpectrogram);
		Melder_information (my ymax, U" ", my v_getFrequencyUnit ());
	}
END

DIRECT (BandFilterSpectrogram_getLowestFrequency)
	LOOP {
		iam (BandFilterSpectrogram);
		Melder_information (my ymin, U" ", my v_getFrequencyUnit ());
	}
END

DIRECT (BandFilterSpectrogram_getNumberOfFrequencies)
	LOOP {
		iam (BandFilterSpectrogram);
		Melder_information (my ny);
	}
END

DIRECT (BandFilterSpectrogram_getFrequencyDistance)
	LOOP {
		iam (BandFilterSpectrogram);
		Melder_information (my dy, U" ", my v_getFrequencyUnit ());
	}
END

FORM (BandFilterSpectrogram_getFrequencyOfRow, U"Get frequency of row", 0)
	NATURAL (U"Row number", U"1")
	OK
DO
	LOOP {
		iam (BandFilterSpectrogram);
		Melder_information (Matrix_rowToY (me, GET_INTEGER (U"Row number")), U" ", my v_getFrequencyUnit ());
	}
END

FORM (BandFilterSpectrogram_getXofColumn, U"Get time of column", 0)
	NATURAL (U"Column number", U"1")
	OK
DO
	LOOP {
		iam (BandFilterSpectrogram);
		Melder_information (Matrix_columnToX (me, GET_INTEGER (U"Column number")));
	}
END

FORM (BandFilterSpectrogram_getValueInCell, U"Get value in cell", 0)
	POSITIVE (U"Time (s)", U"0.5")
	POSITIVE (U"Frequency", U"1")
	OK
DO
	double t = GET_REAL (U"Time");
	double f = GET_REAL (U"Frequency");
	LOOP {
		iam (BandFilterSpectrogram);
		if (f < my ymin || f > my ymax) {
			Melder_throw (U"Frequency out of range.");
		}
		if (t < my xmin || t > my xmax) {
			Melder_throw (U"Time out of range.");
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
		Melder_information (Melder_single (my z[row][col]), U" (delta t: ", ta - t, U" f: ",
		fa - f, U")");
	}
END

/**************** MelFilter *******************************************/

DIRECT (MelFilter_help)
	Melder_help (U"MelFilter");
END

DIRECT (MelSpectrogram_help)
	Melder_help (U"MelSpectrogram");
END

FORM (MelFilter_drawFilterFunctions, U"MelFilter: Draw filter functions", U"FilterBank: Draw filter functions...")
	INTEGER (U"left Filter range", U"0")
	INTEGER (U"right Filter range", U"0")
	RADIO (U"Frequency scale", 1)
	RADIOBUTTON (U"Hertz")
	RADIOBUTTON (U"Bark")
	RADIOBUTTON (U"Mel")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	BOOLEAN (U"Amplitude scale in dB", 0)
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (MelFilter);
		MelFilter_drawFilterFunctions (me, GRAPHICS, GET_INTEGER (U"Frequency scale"),
			GET_INTEGER (U"left Filter range"), GET_INTEGER (U"right Filter range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Amplitude scale in dB"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (MelSpectrogram_drawTriangularFilterFunctions, U"MelSpectrogram: Draw triangulat filter functions", U"MelSpectrogram: Draw filter functions...")
	INTEGER (U"left Filter range", U"0")
	INTEGER (U"right Filter range", U"0")
	RADIO (U"Frequency scale", 1)
	RADIOBUTTON (U"Mel")
	RADIOBUTTON (U"Hertz")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right Frequency range", U"0.0")
	BOOLEAN (U"Amplitude scale in dB", 0)
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (MelSpectrogram);
		MelSpectrogram_drawTriangularFilterFunctions (me, GRAPHICS, GET_INTEGER (U"Frequency scale") - 1,
			GET_INTEGER (U"left Filter range"), GET_INTEGER (U"right Filter range"),
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_INTEGER (U"Amplitude scale in dB"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),
			GET_INTEGER (U"Garnish"));
	}
END


FORM (MelFilter_drawSpectrum, U"MelFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...")
	POSITIVE (U"Time (s)", U"0.1")
	REAL (U"left Frequency range (mel)", U"0.0")
	REAL (U"right Frequency range (mel)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FilterBank);
		FilterBank_drawTimeSlice (me, GRAPHICS, GET_REAL (U"Time"), GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), U"Mels", GET_INTEGER (U"Garnish"));
	}
END

FORM (MelSpectrogram_drawSpectrumAtNearestTimeSlice, U"MelSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...")
	REAL (U"Time (s)", U"0.1")
	REAL (U"left Frequency range (mel)", U"0.0")
	REAL (U"right Frequency range (mel)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (MelSpectrogram);
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, GET_REAL (U"Time"), GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (BarkSpectrogram_drawSpectrumAtNearestTimeSlice, U"BarkSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...")
	REAL (U"Time (s)", U"0.1")
	REAL (U"left Frequency range (bark)", U"0.0")
	REAL (U"right Frequency range (bark)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (MelSpectrogram);
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, GET_REAL (U"Time"), GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"left Amplitude range"),
			GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (MelFilter_paint, U"FilterBank: Paint", 0)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Frequency range (mel)", U"0.0")
	REAL (U"right Frequency range (mel)", U"0.0")
	REAL (U"left Amplitude range", U"0.0")
	REAL (U"right Amplitude range", U"0.0")
	BOOLEAN (U"Garnish", 0)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Matrix);
		FilterBank_paint ((FilterBank) me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
		GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (MelFilter_to_MFCC, U"MelFilter: To MFCC", U"MelSpectrogram: To MFCC...")
	NATURAL (U"Number of coefficients", U"12")
	OK
DO
	LOOP {
		iam (MelFilter);
		praat_new (MelFilter_to_MFCC (me, GET_INTEGER (U"Number of coefficients")), my name);
	}
END

FORM (MelSpectrogram_to_MFCC, U"MelSpectrogram: To MFCC", U"MelSpectrogram: To MFCC...")
	NATURAL (U"Number of coefficients", U"12")
	OK
DO
	LOOP {
		iam (MelSpectrogram);
		autoMFCC thee = MelSpectrogram_to_MFCC (me, GET_INTEGER (U"Number of coefficients"));
		praat_new (thee.move(), my name);
	}
END

/**************** Ltas *******************************************/

#include "UnicodeData.h"
FORM (Ltas_reportSpectralTilt, U"Ltas: Report spectral tilt", 0)
	POSITIVE (U"left Frequency range (Hz)", U"100.0")
	POSITIVE (U"right Frequency range (Hz)", U"5000.0")
	OPTIONMENU (U"Frequency scale", 1)
	OPTION (U"Linear")
	OPTION (U"Logarithmic")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	bool logScale = GET_INTEGER (U"Frequency scale") == 2;
	LOOP {
		iam (Ltas);
		double a, b;
		Ltas_fitTiltLine (me, GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			logScale, GET_INTEGER (U"Fit method"), &a, &b);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Spectral model: amplitude_dB(frequency_Hz) " UNITEXT_ALMOST_EQUAL_TO " ", logScale ? U"offset + slope * log (frequency_Hz)" : U"offset + slope * frequency_Hz");
		MelderInfo_writeLine (U"Slope: ", a, logScale ? U" dB/decade" : U" dB/Hz");
		MelderInfo_writeLine (U"Offset: ", b, U" dB");
		MelderInfo_close ();
	}
END


/**************** MFCC *******************************************/

DIRECT (MFCC_help)
	Melder_help (U"MFCC");
END

FORM (MFCC_to_MelFilter, U"MFCC: To MelFilter", 0)
	INTEGER (U"From coefficient", U"0")
	INTEGER (U"To coefficient", U"0")
	OK
DO
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_MelFilter (me, GET_INTEGER (U"From coefficient"), GET_INTEGER (U"To coefficient")), my name);
	}
END

FORM (MFCC_to_MelSpectrogram, U"MFCC: MelSpectrogram", U"MFCC: To MelSpectrogram...")
	INTEGER (U"From coefficient", U"0")
	INTEGER (U"To coefficient", U"0")
	BOOLEAN (U"Include constant term", 1)
	OK
DO
	LOOP {
		iam (MFCC);
		autoMelSpectrogram thee = MFCC_to_MelSpectrogram (me, GET_INTEGER (U"From coefficient"), GET_INTEGER (U"To coefficient"),
			GET_INTEGER (U"Include constant term"));
		praat_new (thee.move(), my name);
	}
END

FORM (MFCC_to_TableOfReal, U"MFCC: To TableOfReal", U"MFCC: To TableOfReal...")
	BOOLEAN (U"Include energy", 0)
	OK
DO
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_TableOfReal (me, GET_INTEGER (U"Include energy")), my name);
	}
END

FORM (MFCC_to_Matrix_features, U"MFCC: To Matrix (features)", U"")
	POSITIVE (U"Window length (s)", U"0.025")
	BOOLEAN (U"Include energy", 0)
	OK
DO
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_Matrix_features (me, GET_REAL (U"Window length"), GET_INTEGER (U"Include energy")), my name);
	}
END

FORM (MFCCs_crossCorrelate, U"MFCC & MFCC: Cross-correlate", 0)
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	MFCC m1 = 0, m2 = 0;
	LOOP {
		iam (MFCC);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	praat_new (MFCCs_crossCorrelate (m1, m2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is...")),
		m1 -> name, U"_", m2 -> name);
END

FORM (MFCCs_convolve, U"MFCC & MFCC: Convolve", 0)
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	MFCC m1 = 0, m2 = 0;
	LOOP {
		iam (MFCC);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	praat_new (MFCCs_convolve (m1, m2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is...")),
		m1 -> name, U"_", m2 -> name);
END

DIRECT (MFCC_to_Sound)
	LOOP {
		iam (MFCC);
		praat_new (MFCC_to_Sound (me), my name);
	}
END

/**************** MSpline *******************************************/

FORM (MSpline_create, U"Create MSpline", U"Create MSpline...")
	WORD (U"Name", U"mspline")
	LABEL (U"", U"Domain")
	REAL (U"Xmin", U"0")
	REAL (U"Xmax", U"1")
	LABEL (U"", U"MSpline(x) = c[1] M[1](x) + c[2] M[1](x) + ... c[n] M[n](x)")
	LABEL (U"", U"all M[k] are polynomials of degree \"Degree\"")
	LABEL (U"", U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree + 1")
	INTEGER (U"Degree", U"2")
	SENTENCE (U"Coefficients (c[k])", U"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE (U"Interior knots" , U"0.3 0.5 0.6")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	long degree = GET_INTEGER (U"Degree");
	if (xmin >= xmax) {
		Melder_throw (U"Xmin must be smaller than Xmax.");
	}
	praat_new (MSpline_createFromStrings (xmin, xmax, degree,
		GET_STRING (U"Coefficients"), GET_STRING (U"Interior knots")), GET_STRING (U"Name"));
END

DIRECT (MSpline_help) Melder_help (U"MSpline"); END

/********************** Pattern *******************************************/

DIRECT (Pattern_and_Categories_to_Discriminant)
	Pattern me = FIRST (Pattern);
	Categories cat = FIRST (Categories);
	autoDiscriminant thee = Pattern_and_Categories_to_Discriminant (me, cat);
	praat_new (thee.move(), my name, U"_", cat -> name);
END

FORM (Pattern_draw, U"Pattern: Draw", 0)
	NATURAL (U"Pattern number", U"1")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Pattern);
		Pattern_draw (me, GRAPHICS, GET_INTEGER (U"Pattern number"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Pattern_formula, U"Pattern: Formula", 0)
	LABEL (U"label", U"        y := 1; for row := 1 to nrow do { x := 1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + 1 } "
		"y := y + 1 }}")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	praat_Fon_formula (dia, interpreter);
END

FORM (Pattern_setValue, U"Pattern: Set value", U"Pattern: Set value...")
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	REAL (U"New value", U"0.0")
	OK
DO
	LOOP {
		iam (Pattern);
		long row = GET_INTEGER (U"Row number"), column = GET_INTEGER (U"Column number");
		if (row > my ny) {
			Melder_throw (U"Row number must not be greater than number of rows.");
		}
		if (column > my nx) {
			Melder_throw (U"Column number must not be greater than number of columns.");
		}
		my z [row] [column] = GET_REAL (U"New value");
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
	Melder_help (U"PCA");
END

DIRECT (hint_PCA_and_TableOfReal_to_Configuration)
	Melder_information (U"You can get principal components by selecting a PCA and a TableOfReal\n"
		"together and choosing \"To Configuration...\".");
END

DIRECT (hint_PCA_and_Covariance_Project)
	Melder_information (U"You can get a new Covariance object rotated to the directions of the direction vectors\n"
		" in the PCA object by selecting a PCA and a Covariance object together.");
END

DIRECT (hint_PCA_and_Configuration_to_TableOfReal_reconstruct)
	Melder_information (U"You can reconstruct the original TableOfReal as well as possible from\n"
		" the principal components in the Configuration and the direction vectors in the PCA object.");
END

FORM (PCA_and_TableOfReal_getFractionVariance, U"PCA & TableOfReal: Get fraction variance", U"PCA & TableOfReal: Get fraction variance...")
	NATURAL (U"left Principal component range", U"1")
	NATURAL (U"right Principal component range", U"1")
	OK
DO
	PCA me = FIRST (PCA);
	TableOfReal tab = FIRST (TableOfReal);
	Melder_information (PCA_and_TableOfReal_getFractionVariance (me, tab,
		GET_INTEGER (U"left Principal component range"), GET_INTEGER (U"right Principal component range")));
END

DIRECT (PCA_and_Configuration_to_TableOfReal_reconstruct)
	PCA me = FIRST (PCA);
	Configuration conf = FIRST (Configuration);
	autoTableOfReal thee = PCA_and_Configuration_to_TableOfReal_reconstruct (me, conf);
	praat_new (thee.move(), my name, U"_", conf->name);
END

FORM (PCA_and_TableOfReal_to_TableOfReal_projectRows, U"PCA & TableOfReal: To TableOfReal (project rows)", U"PCA & TableOfReal: To Configuration...")
	INTEGER (U"Number of dimensions to keep", U"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions to keep");
	if (dimension < 0) {
		Melder_throw (U"Number of dimensions must be greater equal zero.");
	}
	PCA me = FIRST (PCA);
	TableOfReal tor = FIRST_GENERIC (TableOfReal);
	autoTableOfReal thee = PCA_and_TableOfReal_to_TableOfReal_projectRows (me, tor, dimension);
	praat_new (thee.move(), my name, U"_", tor -> name);
END

FORM (PCA_and_TableOfReal_to_Configuration, U"PCA & TableOfReal: To Configuration", U"PCA & TableOfReal: To Configuration...")
	INTEGER (U"Number of dimensions", U"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions");
	if (dimension < 0) {
		Melder_throw (U"Number of dimensions must be greater equal zero.");
	}
	PCA me = FIRST (PCA);
	TableOfReal tab = FIRST_GENERIC (TableOfReal);
	autoConfiguration thee = PCA_and_TableOfReal_to_Configuration (me, tab, dimension);
	praat_new (thee.move(), my name, U"_", tab->name);
END

FORM (PCA_and_TableOfReal_to_TableOfReal_zscores, U"PCA & TableOfReal: To TableOfReal (z-scores)", U"PCA & TableOfReal: To TableOfReal (z-scores)...")
	INTEGER (U"Number of dimensions", U"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions");
	if (dimension < 0) {
		Melder_throw (U"Number of dimensions must be greater than or equal to zero.");
	}
	PCA me = FIRST (PCA);
	TableOfReal thee = FIRST_GENERIC (TableOfReal);
	autoTableOfReal him = PCA_and_TableOfReal_to_TableOfReal_zscores (me, thee, dimension);
	praat_new (him.move(), my name, U"_", thy name, U"_zscores");
END

FORM (PCA_and_Matrix_to_Matrix_projectRows, U"PCA & Matrix: To Matrix (project rows)", U"")
	INTEGER (U"Number of dimensions", U"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions");
	if (dimension < 0) {
		Melder_throw (U"Number of dimensions must be greater than or equal to zero.");
	}
	PCA me = FIRST (PCA);
	Matrix m = FIRST_GENERIC (Matrix);
	autoMatrix thee = Eigen_and_Matrix_to_Matrix_projectRows (me, m, dimension);
	praat_new (thee.move(), my name, U"_", m -> name);
END

FORM (PCA_and_Matrix_to_Matrix_projectColumns, U"PCA & Matrix: To Matrix (project columns)", U"")
	INTEGER (U"Number of dimensions", U"0 (=all)")
	OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions");
	if (dimension < 0) {
		Melder_throw (U"Number of dimensions must be greater than or equal to zero.");
	}
	PCA me = FIRST (PCA);
	Matrix m = FIRST_GENERIC (Matrix);
	autoMatrix thee = Eigen_and_Matrix_to_Matrix_projectColumns (me, m, dimension);
	praat_new (thee.move(), my name, U"_", m -> name);
END

FORM (PCA_getCentroidElement, U"PCA: Get centroid element...", 0)
	NATURAL (U"Number", U"1")
	OK
DO
	long number = GET_INTEGER (U"Number");
	LOOP {
		iam (PCA);
		if (number > my dimension) {
			Melder_throw (U"Number may not be larger than ", my dimension, U".");
		}
		Melder_information (my centroid[number], U" (element ", number, U")");
	}
END

FORM (PCA_getEqualityOfEigenvalues, U"PCA: Get equality of eigenvalues", U"PCA: Get equality of eigenvalues...")
	INTEGER (U"left Eigenvalue range", U"0")
	INTEGER (U"right Eigenvalue range", U"0")
	BOOLEAN (U"Conservative test", 0)
	OK
DO
	LOOP {
		iam (PCA);
		double p, chisq, df;
		PCA_getEqualityOfEigenvalues (me, GET_INTEGER (U"left Eigenvalue range"),
		GET_INTEGER (U"right Eigenvalue range"), GET_INTEGER (U"Conservative test"), & p, & chisq, & df);
		Melder_information (p, U" (=probability, based on chisq = ",
		chisq, U" and df = ", df);
	}
END

FORM (PCA_getNumberOfComponentsVAF, U"PCA: Get number of components (VAF)", U"PCA: Get number of components (VAF)...")
	POSITIVE (U"Variance fraction (0-1)", U"0.95")
	OK
DO
	double f = GET_REAL (U"Variance fraction");
	LOOP {
		iam (Eigen);
		if (f <= 0 || f > 1) {
			Melder_throw (U"The variance fraction must be in interval (0-1).");
		}
		Melder_information (Eigen_getDimensionOfFraction (me, f));
	}
END

FORM (PCA_getFractionVAF, U"PCA: Get fraction variance accounted for", U"PCA: Get fraction variance accounted for...")
	NATURAL (U"left Principal component range", U"1")
	NATURAL (U"right Principal component range", U"1")
	OK
DO
	long from = GET_INTEGER (U"left Principal component range");
	long to = GET_INTEGER (U"right Principal component range");
	if (from > to) {
		Melder_throw (U"The second component must be greater than or equal to the first component.");
	}
	LOOP {
		iam (Eigen);
		if (from > to) {
			Melder_throw (U"The second component must be greater than or equal to the first component.");
		}
		Melder_information (Eigen_getCumulativeContributionOfComponents (me, from, to));
	}
END

FORM (PCA_invertEigenvector, U"PCA: Invert eigenvector", 0)
	NATURAL (U"Eigenvector number", U"1")
	OK
DO
	LOOP {
		iam (Eigen);
		Eigen_invertEigenvector (me, GET_INTEGER (U"Eigenvector number"));
		praat_dataChanged (me);
	}
END

FORM (PCA_to_TableOfReal_reconstruct1, U"PCA: To TableOfReal (reconstruct)", U"PCA: To TableOfReal (reconstruct 1)...")
	SENTENCE (U"Coefficients", U"1.0 1.0")
	OK
DO
	LOOP {
		iam (PCA);
		autoTableOfReal thee = PCA_to_TableOfReal_reconstruct1 (me, GET_STRING (U"Coefficients"));
		praat_new (thee.move(), my name, U"_reconstructed");
	}
END

FORM (PCAs_to_Procrustes, U"PCA & PCA: To Procrustes", U"PCA & PCA: To Procrustes...")
	NATURAL (U"left Eigenvector range", U"1")
	NATURAL (U"right Eigenvector range", U"2")
	OK
DO
	long from = GET_INTEGER (U"left Eigenvector range");
	long to = GET_INTEGER (U"right Eigenvector range");
	PCA p1 = 0, p2 = 0;
	LOOP {
		iam (PCA);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	autoProcrustes thee = Eigens_to_Procrustes (p1, p2, from, to);
	praat_new (thee.move(), Thing_getName (p1), U"_", Thing_getName (p2));
END


DIRECT (PCAs_getAngleBetweenPc1Pc2Plane_degrees)
	PCA p1 = 0, p2 = 0;
	LOOP {
		iam (PCA);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	Melder_information (Eigens_getAngleBetweenEigenplanes_degrees (p1, p2),
		U" degrees (=angle of intersection between the two pc1-pc2 eigenplanes)");
END

/******************* Permutation **************************************/

DIRECT (Permutation_help)
	Melder_help (U"Permutation");
END

FORM (Permutation_create, U"Create Permutation", U"Create Permutation...")
	WORD (U"Name", U"p")
	NATURAL (U"Number of elements", U"10")
	BOOLEAN (U"Identity Permutation", 1)
	OK
DO
	autoPermutation p = Permutation_create (GET_INTEGER (U"Number of elements"));
	int identity = GET_INTEGER (U"Identity Permutation");
	if (! identity) {
		Permutation_permuteRandomly_inline (p.get(), 0, 0);
	}
	praat_new (p.move(), GET_STRING (U"Name"));
END

DIRECT (Permutation_getNumberOfElements)
	LOOP {
		iam (Permutation);
		Melder_information (my numberOfElements);
	}
END

FORM (Permutation_getValueAtIndex, U"Permutation: Get value", U"Permutation: Get value...")
	NATURAL (U"Index", U"1")
	OK
DO
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (Permutation);
		Melder_information (Permutation_getValueAtIndex (me, index), U" (value, at index = ",
			index, U")");
	}
END

FORM (Permutation_getIndexAtValue, U"Permutation: Get index", U"Permutation: Get index...")
	NATURAL (U"Value", U"1")
	OK
DO
	long value = GET_INTEGER (U"Value");
	LOOP {
		iam (Permutation);
		Melder_information (Permutation_getIndexAtValue (me, value), U" (index, at value = ",
			value, U")");
	}
END

DIRECT (Permutation_sort)
	LOOP {
		iam (Permutation);
		Permutation_sort (me);
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapBlocks, U"Permutation: Swap blocks", U"Permutation: Swap blocks...")
	NATURAL (U"From index", U"1")
	NATURAL (U"To index", U"2")
	NATURAL (U"Block size", U"1")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapBlocks (me, GET_INTEGER (U"From index"), GET_INTEGER (U"To index"), GET_INTEGER (U"Block size"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapPositions, U"Permutation: Swap positions", U"Permutation: Swap positions...")
	NATURAL (U"First index", U"1")
	NATURAL (U"Second index", U"2")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapPositions (me, GET_INTEGER (U"First index"), GET_INTEGER (U"Second index"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapNumbers, U"Permutation: Swap numbers", U"Permutation: Swap numbers...")
	NATURAL (U"First number", U"1")
	NATURAL (U"Second number", U"2")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapNumbers (me, GET_INTEGER (U"First number"), GET_INTEGER (U"Second number"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_swapOneFromRange, U"Permutation: Swap one from range", U"Permutation: Swap one from range...")
	LABEL (U"", U"A randomly chosen element from ")
	INTEGER (U"left Index range", U"0")
	INTEGER (U"right Index range", U"0")
	LABEL (U"", U"is swapped with the element at")
	NATURAL (U"Index", U"1")
	BOOLEAN (U"Forbid same", 1)
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapOneFromRange (me, GET_INTEGER (U"left Index range"), GET_INTEGER (U"right Index range"),
			GET_INTEGER (U"Index"), GET_INTEGER (U"Forbid same"));
		praat_dataChanged (me);
	}
END

FORM (Permutation_permuteRandomly, U"Permutation: Permute randomly", U"Permutation: Permute randomly...")
	INTEGER (U"left Index range", U"0")
	INTEGER (U"right Index range", U"0")
	OK
DO
	LOOP {
		iam (Permutation);
		praat_new (Permutation_permuteRandomly (me, GET_INTEGER (U"left Index range"),
		GET_INTEGER (U"right Index range")), Thing_getName (me), U"_rdm");
	}
END

FORM (Permutation_rotate, U"Permutation: Rotate", U"Permutation: Rotate...")
	INTEGER (U"left Index range", U"0")
	INTEGER (U"right Index range", U"0")
	INTEGER (U"Step size", U"1")
	OK
DO
	long step = GET_INTEGER (U"Step size");
	LOOP {
		iam (Permutation);
		praat_new (Permutation_rotate (me, GET_INTEGER (U"left Index range"), GET_INTEGER (U"right Index range"), step),
			Thing_getName (me), U"_rot", step);
	}
END

FORM (Permutation_reverse, U"Permutation: Reverse", U"Permutation: Reverse...")
	INTEGER (U"left Index range", U"0")
	INTEGER (U"right Index range", U"0")
	OK
DO
	LOOP {
		iam (Permutation);
		praat_new (Permutation_reverse (me, GET_INTEGER (U"left Index range"), GET_INTEGER (U"right Index range")),
			Thing_getName (me), U"_rev");
	}
END

FORM (Permutation_permuteBlocksRandomly, U"Permutation: Permute blocks randomly", U"Permutation: Permute randomly (blocks)...")
	INTEGER (U"left Index range", U"0")
	INTEGER (U"right Index range", U"0")
	NATURAL (U"Block size", U"12")
	BOOLEAN (U"Permute within blocks", 1)
	BOOLEAN (U"No doublets", 0)
	OK
DO
	long blocksize = GET_INTEGER (U"Block size");
	LOOP {
		iam (Permutation);
		praat_new (Permutation_permuteBlocksRandomly (me, GET_INTEGER (U"left Index range"),
			GET_INTEGER (U"right Index range"), blocksize, GET_INTEGER (U"Permute within blocks"),
			GET_INTEGER (U"No doublets")), Thing_getName (me), U"_pbr", blocksize);
	}
END

FORM (Permutation_interleave, U"Permutation: Interleave", U"Permutation: Interleave...")
	INTEGER (U"left Index range", U"0")
	INTEGER (U"right Index range", U"0")
	NATURAL (U"Block size", U"12")
	INTEGER (U"Offset", U"0")
	OK
DO
	LOOP {
		iam (Permutation);
		praat_new (Permutation_interleave (me, GET_INTEGER (U"left Index range"), GET_INTEGER (U"right Index range"),
			GET_INTEGER (U"Block size"), GET_INTEGER (U"Offset")), Thing_getName (me), U"_itl");
	}
END

DIRECT (Permutation_invert)
	LOOP {
		iam (Permutation);
		praat_new (Permutation_invert (me), Thing_getName (me), U"_inv");
	}
END

DIRECT (Permutations_multiply)
	OrderedOf<structPermutation> list;
	LOOP {
		iam (Permutation);
		list. addItem_ref (me);
	}
	autoPermutation result = Permutations_multiply (& list);
	praat_new (result.move(), U"mul_", list.size);
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

FORM (Pitches_to_DTW, U"Pitches: To DTW", U"Pitches: To DTW...")
	REAL (U"Voiced-unvoiced costs", U"24.0")
	REAL (U"Time costs weight", U"10.0")
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
	autoDTW thee = Pitches_to_DTW (p1, p2, GET_REAL (U"Voiced-unvoiced costs"), GET_REAL (U"Time costs weight"), begin, end, slope);
	praat_new (thee.move(), U"dtw_", Thing_getName (p1), U"_", Thing_getName (p2));
END

FORM (PitchTier_to_Pitch, U"PitchTier: To Pitch", U"PitchTier: To Pitch...")
	POSITIVE (U"Step size", U"0.02")
	POSITIVE (U"Pitch floor", U"60.0")
	POSITIVE (U"Pitch ceiling", U"400.0")
	OK
DO
	LOOP {
		iam (PitchTier);
		autoPitch thee = PitchTier_to_Pitch (me, GET_REAL (U"Step size"),
		GET_REAL (U"Pitch floor"), GET_REAL (U"Pitch ceiling"));
		praat_new (thee.move(), my name);
	}
END

/******************* Polygon & Categories *************************************/

FORM (Polygon_createSimple, U"Create simple Polygon", U"Create simple Polygon...")
	WORD (U"Name", U"p")
	SENTENCE (U"Vertices as X-Y pairs", U"0.0 0.0  0.0 1.0  1.0 0.0")
	OK
DO
	autoPolygon thee = Polygon_createSimple (GET_STRING (U"Vertices as X-Y pairs"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END

FORM (Polygon_createFromRandomVertices, U"", 0)
	WORD (U"Name", U"p")
	NATURAL (U"Number of vertices", U"10")
	REAL (U"left X range", U"0.0")
	REAL (U"right X range", U"1.0")
	REAL (U"left Y range", U"0.0")
	REAL (U"right Y range", U"1.0")
	OK
DO
	autoPolygon thee = Polygon_createFromRandomVertices (GET_INTEGER (U"Number of vertices"),
		GET_REAL (U"left X range"), GET_REAL (U"right X range"),
		GET_REAL (U"left Y range"), GET_REAL (U"right Y range"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END

DIRECT (Polygon_getNumberOfPoints)
	LOOP {
		iam (Polygon);
		Melder_information (my numberOfPoints);
	}
END

FORM (Polygon_getPointX, U"Polygon: Get point (x)", 0)
	NATURAL (U"Point number", U"1")
	OK
DO
	long point = GET_INTEGER (U"Point number");
	LOOP {
		iam (Polygon);
		if (point > my numberOfPoints) {
			Melder_throw (U"Point cannot be larger than ", my numberOfPoints, U".");
		}
		Melder_information (my x[point]);
	}
END

FORM (Polygon_getPointY, U"Polygon: Get point (y)", 0)
	NATURAL (U"Point number", U"1")
	OK
DO
	long point = GET_INTEGER (U"Point number");
	LOOP {
		iam (Polygon);
		if (point > my numberOfPoints) {
			Melder_throw (U"Vertex cannot be larger than ", my numberOfPoints, U".");
		}
		Melder_information (my y[point]);
	}
END

FORM (Polygon_getLocationOfPoint, U"Get location of point", U"Polygon: Get location of point...")
	LABEL (U"", U"Point is (I)n, (O)ut, (E)dge or (V)ertex?")
	REAL (U"X", U"0.0")
	REAL (U"Y", U"0.0")
	REAL (U"Precision", U"1.64e-15")
	OK
DO
	double eps = GET_REAL (U"Precision");
	REQUIRE (eps >= 0, U"The precision cannot be negative.")
	LOOP {
		iam (Polygon);
		int loc = Polygon_getLocationOfPoint (me, GET_REAL (U"X"), GET_REAL (U"Y"), eps);
		Melder_information (loc == Polygon_INSIDE ? U"I" : loc == Polygon_OUTSIDE ? U"O" :
		loc == Polygon_EDGE ? U"E" : U"V");
	}
END

DIRECT (Polygon_getAreaOfConvexHull)
	LOOP {
		iam (Polygon);
		Melder_informationReal (Polygon_getAreaOfConvexHull (me), nullptr);
	}
END

FORM (Polygon_circularPermutation, U"Polygon: Circular permutation", 0)
	INTEGER (U"Shift", U"1")
	OK
DO
	long shift = GET_INTEGER (U"Shift");
	LOOP {
		iam (Polygon);
		autoPolygon thee = Polygon_circularPermutation (me, shift);
		praat_new (thee.move(), my name, U"_", shift);
	}
END

DIRECT (Polygon_simplify)
	LOOP {
		iam (Polygon);
		autoPolygon thee = Polygon_simplify (me);
		praat_new (thee.move(), my name, U"_s");
	}
END

DIRECT (Polygon_convexHull)
	LOOP {
		iam (Polygon);
		autoPolygon thee = Polygon_convexHull (me);
		praat_new (thee.move(), my name, U"_hull");
	}
END

FORM (Polygon_translate, U"Polygon: Translate", U"Polygon: Translate...")
	REAL (U"X", U"0.0")
	REAL (U"Y", U"0.0")
	OK
DO
	LOOP {
		iam (Polygon); 
		Polygon_translate (me, GET_REAL (U"X"), GET_REAL (U"Y"));
		praat_dataChanged (me);
	}
END

FORM (Polygon_rotate, U"Polygon: Rotate", U"Polygon: Rotate...")
	LABEL (U"", U"Rotate counterclockwise over the")
	REAL (U"Angle (degrees)", U"0.0")
	LABEL (U"", U"With respect to the point")
	REAL (U"X", U"0.0")
	REAL (U"Y", U"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		Polygon_rotate (me, GET_REAL (U"Angle"), GET_REAL (U"X"), GET_REAL (U"Y"));
		praat_dataChanged (me);
	}
END

FORM (Polygon_scale, U"Polygon: Scale polygon", 0)
	REAL (U"X", U"0.0")
	REAL (U"Y", U"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		Polygon_scale (me, GET_REAL (U"X"), GET_REAL (U"Y"));
		praat_dataChanged (me);
	}
END

FORM (Polygon_Categories_draw, U"Polygon & Categories: Draw", 0)
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	Polygon me = FIRST (Polygon);
	Categories cat = FIRST (Categories);
	Polygon_Categories_draw (me, cat, GRAPHICS, GET_REAL (U"left Horizontal range"),
		GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_INTEGER (U"Garnish"));
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

DIRECT (Polynomial_help) Melder_help (U"Polynomial"); END

FORM (Polynomial_create, U"Create Polynomial", U"Create Polynomial...")
	WORD (U"Name", U"p")
	LABEL (U"", U"Domain of polynomial")
	REAL (U"Xmin", U"-3")
	REAL (U"Xmax", U"4")
	LABEL (U"", U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	SENTENCE (U"Coefficients", U"2.0 -1.0 -2.0 1.0")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	if (xmin >= xmax) {
		Melder_throw (U"Xmin must be smaller than Xmax.");
	}
	praat_new (Polynomial_createFromString (xmin, xmax, GET_STRING (U"Coefficients")), GET_STRING (U"Name"));
END

FORM (Polynomial_getArea, U"Polynomial: Get area", U"Polynomial: Get area...")
	LABEL (U"", U"Interval")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	OK
DO
	LOOP {
		iam (Polynomial);
		double area = Polynomial_getArea (me, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"));
		Melder_information (area);
	}
END

DIRECT (Polynomial_getDerivative)
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_getDerivative (me), my name, U"_derivative");
	}
END

DIRECT (Polynomial_getPrimitive)
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_getPrimitive (me), my name, U"_primitive");
	}
END

FORM (Polynomial_scaleX, U"Polynomial: Scale x", U"Polynomial: Scale x...")
	LABEL (U"", U"New domain")
	REAL (U"Xmin", U"-1.0")
	REAL (U"Xmax", U"1.0")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	if (xmin >= xmax) {
		Melder_throw (U"Xmin must be smaller than Xmax.");
	}
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_scaleX (me, xmin, xmax), my name, U"_scaleX");
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

FORM (Polynomial_evaluate_z, U"Polynomial: Get value (complex)", U"Polynomial: Get value (complex)...")
	REAL (U"Real part", U"0.0")
	REAL (U"Imaginary part", U"0.0")
	OK
DO
	dcomplex p, z = dcomplex_create (GET_REAL (U"Real part"), GET_REAL (U"Imaginary part"));
	LOOP {
		iam (Polynomial);
		Polynomial_evaluate_z (me, &z, &p);
		Melder_information (p.re, U" + ", p.im, U" i");
	}
END


FORM (Polynomial_to_Spectrum, U"Polynomial: To Spectrum", U"Polynomial: To Spectrum...")
	POSITIVE (U"Nyquist frequency (Hz)", U"5000.0")
	NATURAL (U"Number of frequencies (>1)", U"1025")
	OK
DO
	long n = GET_INTEGER (U"Number of frequencies");
	LOOP {
		iam (Polynomial);
		praat_new (Polynomial_to_Spectrum (me, GET_REAL (U"Nyquist frequency"), n, 1.0), my name);
	}
END

DIRECT (Polynomials_multiply)
	Polynomial p1 = 0, p2 = 0;
	LOOP {
		iam (Polynomial);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	praat_new (Polynomials_multiply (p1, p2), Thing_getName (p1), U"_x_", Thing_getName (p2));
END

FORM (Polynomials_divide, U"Polynomials: Divide", U"Polynomials: Divide...")
	BOOLEAN (U"Want quotient", 1)
	BOOLEAN (U"Want remainder", 1)
	OK
DO
/* With gcc (GCC) 3.2.2 20030217 (Red Hat Linux 8.0 3.2.2-2)
	The following line initiates pq = nullptr and I don't know why
Polynomial p1 = nullptr, p2 = nullptr, pq, pr;
*/

	bool wantq = GET_INTEGER (U"Want quotient");
	bool wantr = GET_INTEGER (U"Want remainder");
	if (! wantq && ! wantr) {
		Melder_throw (U"Either \'Want quotient\' or \'Want remainder\' should be chosen");
	}
	Polynomial p1 = nullptr, p2 = nullptr;
	LOOP {
		iam (Polynomial);
		(p1 ? p2 : p1) = me;
	}
	Melder_assert (p1 && p2);
	autoPolynomial aq, ar;
	Polynomials_divide (p1, p2, wantq ? & aq : nullptr, wantr ? & ar : nullptr);
//	autoPolynomial aq = q, ar = r;
	if (wantq) {
		praat_new (aq.move(), Thing_getName (p1), U"_q");
	}
	if (wantr) {
		praat_new (ar.move(), Thing_getName (p1), U"_r");
	}
END

/********************* Roots ******************************/

DIRECT (Roots_help) Melder_help (U"Roots"); END

FORM (Roots_draw, U"Roots: Draw", 0)
	REAL (U"Minimum of real axis", U"0.0")
	REAL (U"Maximum of real axis", U"0.0")
	REAL (U"Minimum of imaginary axis", U"0.0")
	REAL (U"Maximum of imaginary axis", U"0.0")
	SENTENCE (U"Mark string (+x0...)", U"o")
	NATURAL (U"Mark size", U"12")
	BOOLEAN (U"Garnish", 0)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Roots);
		Roots_draw (me, GRAPHICS, GET_REAL (U"Minimum of real axis"), GET_REAL (U"Maximum of real axis"),
		GET_REAL (U"Minimum of imaginary axis"), GET_REAL (U"Maximum of imaginary axis"),
		GET_STRING (U"Mark string"), GET_INTEGER (U"Mark size"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (Roots_getNumberOfRoots)
	LOOP {
		iam (Roots);
		Melder_information (Roots_getNumberOfRoots (me));
	}
END

FORM (Roots_getRoot, U"Roots: Get root", 0)
	NATURAL (U"Root number", U"1")
	OK
DO
	LOOP {
		iam (Roots);
		dcomplex z = Roots_getRoot (me, GET_INTEGER (U"Root number"));
		Melder_information (z.re, (z.im < 0 ? U" - " : U" + "), fabs (z.im), U" i");   // ppgb: waarom hier de absolute waarde maar beneden niet?
	}
END

FORM (Roots_getRealPartOfRoot, U"Roots: Get real part", 0)
	NATURAL (U"Root number", U"1")
	OK
DO
	LOOP {
		iam (Roots);
		dcomplex z = Roots_getRoot (me, GET_INTEGER (U"Root number"));
		Melder_information (z.re);
	}
END

FORM (Roots_getImaginaryPartOfRoot, U"Roots: Get imaginary part", 0)
	NATURAL (U"Root number", U"1")
	OK
DO
	LOOP {
		iam (Roots);
		dcomplex z = Roots_getRoot (me, GET_INTEGER (U"Root number"));
		Melder_information (z.im);   // ppgb: waarom hier de imaginaire waarde zelf, maar boven de absolute waarde daarvan?
	}
END

FORM (Roots_setRoot, U"Roots: Set root", 0)
	NATURAL (U"Root number", U"1")
	REAL (U"Real part", U"1.0/sqrt(2)")
	REAL (U"Imaginary part", U"1.0/sqrt(2)")
	OK
DO
	LOOP {
		iam (Roots);
		Roots_setRoot (me, GET_INTEGER (U"Root number"), GET_REAL (U"Real part"), GET_REAL (U"Imaginary part"));
		praat_dataChanged (me);
	}
END

FORM (Roots_to_Spectrum, U"Roots: To Spectrum", U"Roots: To Spectrum...")
	POSITIVE (U"Nyquist frequency (Hz)", U"5000.0")
	NATURAL (U"Number of frequencies (>1)", U"1025")
	OK
DO
	long n = GET_INTEGER (U"Number of frequencies");
	LOOP {
		iam (Roots);
		praat_new (Roots_to_Spectrum (me, GET_REAL (U"Nyquist frequency"), n, 1.0), my name);
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
	MelderInfo_writeLine (U"Double precision floating point properties of this machine,");
	MelderInfo_writeLine (U"as calculated by algorithms from the Binary Linear Algebra System (BLAS)");
	MelderInfo_writeLine (U"Radix: ", NUMfpp -> base);
	MelderInfo_writeLine (U"Number of digits in mantissa: ", NUMfpp -> t);
	MelderInfo_writeLine (U"Smallest exponent before (gradual) underflow (expmin): ", NUMfpp -> emin);
	MelderInfo_writeLine (U"Largest exponent before overflow (expmax): ", NUMfpp -> emax);
	MelderInfo_writeLine (U"Does rounding occur in addition: ", (NUMfpp -> rnd == 1 ? U"yes" : U"no"));
	MelderInfo_writeLine (U"Quantization step (d): ", NUMfpp -> prec);
	MelderInfo_writeLine (U"Quantization error (eps = d/2): ", NUMfpp -> eps);
	MelderInfo_writeLine (U"Underflow threshold (= radix ^ (expmin - 1)): ", NUMfpp -> rmin);
	MelderInfo_writeLine (U"Safe minimum (such that its inverse does not overflow): ", NUMfpp -> sfmin);
	MelderInfo_writeLine (U"Overflow threshold (= (1 - eps) * radix ^ expmax): ", NUMfpp -> rmax);
	MelderInfo_close ();
END
/*
#ifdef HAVE_PULSEAUDIO
void pulseAudioServer_report ();
DIRECT (Praat_ReportSoundServerProperties)
	pulseAudioServer_report ();
END
#endif
*/
FORM (Praat_getTukeyQ, U"Get TukeyQ", 0)
	REAL (U"Critical value", U"2.0")
	NATURAL (U"Number of means", U"3")
	POSITIVE (U"Degrees of freedom", U"10.0")
	NATURAL (U"Number of rows", U"1")
	OK
DO
	double q = GET_REAL (U"Critical value");
	REQUIRE (q > 0 , U"Critical value must be > 0.")
	double val = NUMtukeyQ (q, GET_INTEGER (U"Number of means"), GET_REAL (U"Degrees of freedom"), GET_INTEGER (U"Number of rows") );
	Melder_informationReal (val, nullptr);
END

FORM (Praat_getInvTukeyQ, U"Get invTukeyQ", 0)
	REAL (U"Probability", U"0.05")
	NATURAL (U"Number of means", U"3")
	POSITIVE (U"Degrees of freedom", U"10.0")
	NATURAL (U"Number of rows", U"1")
	OK
DO
	double p = GET_REAL (U"Probability");
	REQUIRE (p >= 0 && p <= 1, U"Probability must be in (0,1).")
	double val = NUMinvTukeyQ (p, GET_INTEGER (U"Number of means"), GET_REAL (U"Degrees of freedom"), GET_INTEGER (U"Number of rows"));
	Melder_informationReal (val, nullptr);
END

/******************** Sound ****************************************/

static void Sound_create_addCommonFields (UiForm dia, const char32 *endTime) {
	REAL (U"Starting time (s)", U"0.0")
	REAL (U"Finishing time (s)", endTime)
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
}

static void Sound_create_checkCommonFields (UiForm dia, double *startingTime, double *finishingTime,
        double *samplingFrequency) {
	double numberOfSamples_real;
	*startingTime = GET_REAL (U"Starting time");
	*finishingTime = GET_REAL (U"Finishing time");
	*samplingFrequency = GET_REAL (U"Sampling frequency");
	numberOfSamples_real = round ( (*finishingTime - *startingTime) * *samplingFrequency);
	if (*finishingTime <= *startingTime) {
		if (*finishingTime == *startingTime) {
			Melder_throw (U"A Sound cannot have a duration of zero.");
		} else {
			Melder_throw (U"A Sound cannot have a duration less than zero.");
		}
		if (*startingTime == 0.0) {
			Melder_throw (U"Please set the finishing time to something greater than 0 seconds.");
		} else {
			Melder_throw (U"Please lower the starting time or raise the finishing time.");
		}
	}
	if (*samplingFrequency <= 0.0)
		Melder_throw (U"A Sound cannot have a negative sampling frequency.\n"
		              U"Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");

	if (numberOfSamples_real < 1.0) {
		Melder_appendError (U"A Sound cannot have zero samples.\n");
		if (*startingTime == 0.0) {
			Melder_throw (U"Please raise the finishing time.");
		} else {
			Melder_throw (U"Please lower the starting time or raise the finishing time.");
		}
	}
	if (numberOfSamples_real > LONG_MAX) {   // ppgb: kan niet in een 64-bit-omgeving
		Melder_throw (U"A Sound cannot have ", Melder_bigInteger ((long) numberOfSamples_real), U" samples; the maximum is ", Melder_bigInteger (LONG_MAX), U" samples.\n");
#if 0
		if (*startingTime == 0.0) {
			Melder_throw (U"Please lower the finishing time or the sampling frequency.");
		} else {
			Melder_throw (U"Please raise the starting time, lower the finishing time, or lower the sampling frequency.");
		}
#endif
	}
}

FORM (Sound_and_Pitch_to_FormantFilter, U"Sound & Pitch: To FormantFilter", U"Sound & Pitch: To Spectrogram...")
	POSITIVE (U"Analysis window duration (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (Hz)", U"100.0")
	POSITIVE (U"Distance between filters (Hz)", U"50.0")
	REAL (U"Maximum frequency", U"0");
	POSITIVE (U"Relative bandwidth", U"1.1")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch p = FIRST (Pitch);
	praat_new (Sound_and_Pitch_to_FormantFilter (me, p, GET_REAL (U"Analysis window duration"),
		GET_REAL (U"Time step"), GET_REAL (U"Position of first filter"),
		GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"),
		GET_REAL (U"Relative bandwidth")), my name, U"_", p->name);
END

FORM (Sound_and_Pitch_to_Spectrogram, U"Sound & Pitch: To Spectrogram", U"Sound & Pitch: To Spectrogram...")
	POSITIVE (U"Analysis window duration (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (Hz)", U"100.0")
	POSITIVE (U"Distance between filters (Hz)", U"50.0")
	REAL (U"Maximum frequency", U"0");
	POSITIVE (U"Relative bandwidth", U"1.1")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch thee = FIRST (Pitch);
	autoSpectrogram him = Sound_and_Pitch_to_Spectrogram (me, thee, GET_REAL (U"Analysis window duration"),
		GET_REAL (U"Time step"), GET_REAL (U"Position of first filter"),
		GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"),
		GET_REAL (U"Relative bandwidth"));
	praat_new (him.move(), my name, U"_", thy name);
END

FORM (Sound_and_Pitch_changeGender, U"Sound & Pitch: Change gender", U"Sound & Pitch: Change gender...")
	POSITIVE (U"Formant shift ratio", U"1.2")
	REAL (U"New pitch median (Hz)", U"0.0 (=no change)")
	POSITIVE (U"Pitch range factor", U"1.0 (=no change)")
	POSITIVE (U"Duration factor", U"1.0")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch p = FIRST (Pitch);
	autoSound thee = Sound_and_Pitch_changeGender_old (me, p, GET_REAL (U"Formant shift ratio"),
		GET_REAL (U"New pitch median"), GET_REAL (U"Pitch range factor"), GET_REAL (U"Duration factor"));
	praat_new (thee.move(), my name, U"_", p->name);
END

FORM (Sound_and_Pitch_changeSpeaker, U"Sound & Pitch: Change speaker", U"Sound & Pitch: Change speaker...")
	POSITIVE (U"Multiply formants by", U"1.1 (male->female)")
	POSITIVE (U"Multiply pitch by", U"1.8 (male->female")
	REAL (U"Multiply pitch range by", U"1.0 (=no change)")
	POSITIVE (U"Multiply duration", U"1.0")
	OK
DO
	Sound me = FIRST (Sound);
	Pitch p = FIRST (Pitch);
	autoSound thee = Sound_and_Pitch_changeSpeaker (me, p, GET_REAL (U"Multiply formants by"),
		GET_REAL (U"Multiply pitch by"), GET_REAL (U"Multiply pitch range by"), GET_REAL (U"Multiply duration"));
	praat_new (thee.move(), my name, U"_", p -> name);
END

FORM (Sound_and_IntervalTier_cutPartsMatchingLabel, U"Sound & IntervalTier: Cut parts matching label", 0)
	SENTENCE (U"Label", U"cut")
	OK
DO
	const char32 *label = GET_STRING (U"Label");
	Sound me = FIRST (Sound);
	IntervalTier thee = FIRST (IntervalTier);
	autoSound him = Sound_and_IntervalTier_cutPartsMatchingLabel (me, thee, label);
	praat_new (him.move(), my name, U"_cut");
END

FORM (Sound_createFromGammaTone, U"Create a gammatone", U"Create Sound from gammatone...")
	WORD (U"Name", U"gammatone")
	Sound_create_addCommonFields (dia, U"0.1");
	INTEGER (U"Gamma", U"4")
	POSITIVE (U"Frequency (Hz)", U"1000.0")
	REAL (U"Bandwidth (Hz)", U"150.0")
	REAL (U"Initial phase (radians)", U"0.0")
	REAL (U"Addition factor", U"0.0")
	BOOLEAN (U"Scale amplitudes", 1)
	OK
DO
	double startingTime, finishingTime, samplingFrequency;
	long gamma = GET_INTEGER (U"Gamma");
	double bandwidth = GET_REAL (U"Bandwidth");
	double f = GET_REAL (U"Frequency");

	Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency);
	if (f >= samplingFrequency / 2) Melder_throw (U"Frequency cannot be larger than half the sampling frequency.\n"
				U"Please use a frequency smaller than ", samplingFrequency / 2);
	if (gamma < 0) {
		Melder_throw (U"Gamma cannot be negative.\nPlease use a positive or zero gamma.");
	}
	if (bandwidth < 0) {
		Melder_throw (U"Bandwidth cannot be negative.\nPlease use a positive or zero bandwidth.");
	}
	autoSound sound = Sound_createGammaTone (startingTime, finishingTime, samplingFrequency, gamma, f, bandwidth,
		GET_REAL (U"Initial phase"), GET_REAL (U"Addition factor"), GET_INTEGER (U"Scale amplitudes"));
	//Sound_create_check (sound.get(), startingTime, finishingTime, samplingFrequency);//TODO
	praat_new (sound.move(), GET_STRING (U"Name"));
END

FORM (Sound_createFromShepardTone, U"Create a Shepard tone", U"Create Sound from Shepard tone...")
	WORD (U"Name", U"shepardTone")
	Sound_create_addCommonFields (dia, U"1.0");
	POSITIVE (U"Lowest frequency (Hz)", U"4.863")
	NATURAL (U"Number of components", U"10")
	REAL (U"Frequency change (semitones/s)", U"4.0")
	REAL (U"Amplitude range (dB)", U"30.0")
	REAL (U"Octave shift fraction ([0,1))", U"0.0")
	OK
DO
	double startingTime, finishingTime, samplingFrequency;
	double amplitudeRange = GET_REAL (U"Amplitude range");
	double octaveShiftFraction = GET_REAL (U"Octave shift fraction");
	Sound_create_checkCommonFields (dia, &startingTime, &finishingTime, &samplingFrequency);
	if (amplitudeRange < 0) {
		Melder_throw (U"Amplitude range cannot be negative.\nPlease use a positive or zero amplitude range.");
	}
	autoSound sound = Sound_createShepardToneComplex (startingTime, finishingTime, samplingFrequency,
		GET_REAL (U"Lowest frequency"), GET_INTEGER (U"Number of components"),
		GET_REAL (U"Frequency change"), GET_REAL (U"Amplitude range"), octaveShiftFraction);
	// Sound_create_check (sound, startingTime, finishingTime, samplingFrequency); //TODO
	praat_new (sound.move(), GET_STRING (U"Name"));
END

FORM (Sound_drawWhere, U"Sound: Draw where", U"Sound: Draw where...")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range", U"0.0 (= all)")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (U"Garnish", 1)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
	OPTION (U"Curve")
	OPTION (U"Bars")
	OPTION (U"Poles")
	OPTION (U"Speckles")
	LABEL (U"", U"Draw only those parts where the following condition holds:")
	TEXTFIELD (U"Formula", U"x < xmin + (xmax - xmin) / 2; first half")
	OK
DO
	long numberOfBisections = 10;
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_drawWhere (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"),
		GET_STRING (U"Drawing method"), numberOfBisections, GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Sound_playOneChannel, U"Sound: Play one channel", 0)
    NATURAL (U"Channel", U"1")
    OK
DO
    long ichannel = GET_INTEGER (U"Channel");
    LOOP {
        iam (Sound);
        if (ichannel > my ny) {
            Melder_throw (me, U": there is no channel ", ichannel, U". Sound has only ", my ny, U" channel",
				  (my ny > 1 ? U"s." : U"."));
        }
        autoSound thee = Sound_extractChannel (me, ichannel);
        Sound_play (thee.get(), 0, 0);
    }
END

FORM (Sound_playAsFrequencyShifted, U"Sound: Play as frequency shifted", U"Sound: Play as frequency shifted...")
	REAL (U"Shift by (Hz)", U"1000.0")
	POSITIVE (U"New sampling frequency (Hz)", U"44100.0")
	NATURAL (U"Precision (samples)", U"50")
	OK
DO
	double shiftBy = GET_REAL (U"Shift by");
	double newSamplingFrequency = GET_REAL (U"New sampling frequency");
	long precision = GET_INTEGER (U"Precision");
	LOOP {
		iam (Sound);
		Sound_playAsFrequencyShifted (me, shiftBy, newSamplingFrequency, precision);
	}
END

FORM (Sounds_to_DTW, U"Sounds: To DTW", 0)
    POSITIVE (U"Window length (s)", U"0.015")
    POSITIVE (U"Time step (s)", U"0.005")
    LABEL (U"", U"")
    REAL (U"Sakoe-Chiba band (s)", U"0.1")
    RADIO (U"Slope constraint", 1)
    RADIOBUTTON (U"no restriction")
    RADIOBUTTON (U"1/3 < slope < 3")
    RADIOBUTTON (U"1/2 < slope < 2")
    RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    double analysisWidth = GET_REAL (U"Window length");
    double dt = GET_REAL (U"Time step");
    double band = GET_REAL (U"Sakoe-Chiba band");
    int slope = GET_INTEGER (U"Slope constraint");
    Sound s1 = 0, s2 = 0;
    LOOP {
        iam (Sound);
        (s1 ? s2 : s1) = me;
    }
    Melder_assert (s1 && s2);
	autoDTW thee = Sounds_to_DTW (s1, s2, analysisWidth, dt, band, slope);
    praat_new (thee.move(), s1 -> name, U"_", s2 -> name);
END

FORM (Sound_to_TextGrid_detectSilences, U"Sound: To TextGrid (silences)", U"Sound: To TextGrid (silences)...")
	LABEL (U"", U"Parameters for the intensity analysis")
	POSITIVE (U"Minimum pitch (Hz)", U"100")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	LABEL (U"", U"Silent intervals detection")
	REAL (U"Silence threshold (dB)", U"-25.0")
	POSITIVE (U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (U"Minimum sounding interval duration (s)", U"0.1")
	WORD (U"Silent interval label", U"silent")
	WORD (U"Sounding interval label", U"sounding")
	OK
DO
	LOOP {
		iam (Sound);
		autoTextGrid thee = Sound_to_TextGrid_detectSilences (me, GET_REAL (U"Minimum pitch"), GET_REAL (U"Time step"),
			GET_REAL (U"Silence threshold"), GET_REAL (U"Minimum silent interval duration"),
			GET_REAL (U"Minimum sounding interval duration"), GET_STRING (U"Silent interval label"),
			GET_STRING (U"Sounding interval label"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sound_copyChannelRanges, U"Sound: Copy channel ranges", 0)
	LABEL (U"", U"Create a new Sound from the following channels:")
	TEXTFIELD (U"Ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_copyChannelRanges (me, GET_STRING (U"Ranges"));
		praat_new (thee.move(), my name, U"_channels");
	}
END

FORM (Sound_trimSilences, U"Sound: Trim silences", U"Sound: Trim silences...")
    REAL (U"Trim duration (s)", U"0.08")
	BOOLEAN (U"Only at start and end", 1);
	LABEL (U"", U"Parameters for the intensity analysis")
	POSITIVE (U"Minimum pitch (Hz)", U"100")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	LABEL (U"", U"Silent intervals detection")
	REAL (U"Silence threshold (dB)", U"-35.0")
	POSITIVE (U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (U"Minimum sounding interval duration (s)", U"0.05")
    BOOLEAN (U"Save trimming info as TextGrid", 0)
    WORD (U"Trim label", U"trimmed")
	OK
DO
    double trimDuration = GET_REAL (U"Trim duration");
    if (trimDuration < 0) {
        trimDuration = 0;
    }
	bool onlyAtStartAndEnd = GET_INTEGER (U"Only at start and end");
	double minPitch = GET_REAL (U"Minimum pitch");
	double timeStep = GET_REAL (U"Time step");
	double silenceThreshold = GET_REAL (U"Silence threshold");
	double minSilenceDuration = GET_REAL (U"Minimum silent interval duration");
	double minSoundingDuration = GET_REAL (U"Minimum sounding interval duration");
    bool saveTextGrid = GET_INTEGER (U"Save trimming info as TextGrid");
    const char32 *trimlabel = GET_STRING (U"Trim label");
	LOOP {
		iam (Sound);
        autoTextGrid tg;
		autoSound thee = Sound_trimSilences (me, trimDuration, onlyAtStartAndEnd, minPitch, timeStep, silenceThreshold,
			minSilenceDuration, minSoundingDuration, ( saveTextGrid ? &tg : nullptr ), trimlabel);
        if (saveTextGrid) {
            praat_new (tg.move(), my name, U"_trimmed");
        }
		praat_new (thee.move(), my name, U"_trimmed");
	}
END

// deprecated
FORM (Sound_to_BarkFilter, U"Sound: To BarkFilter", U"Sound: To BarkSpectrogram...")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (bark)", U"1.0")
	POSITIVE (U"Distance between filters (bark)", U"1.0")
	REAL (U"Maximum frequency (bark)", U"0");
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_BarkFilter (me, GET_REAL (U"Window length"),
		GET_REAL (U"Time step"), GET_REAL (U"Position of first filter"),
		GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters")), my name);
	}
END

FORM (Sound_to_BarkSpectrogram, U"Sound: To BarkSpectrogram", U"Sound: To BarkSpectrogram...")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (bark)", U"1.0")
	POSITIVE (U"Distance between filters (bark)", U"1.0")
	REAL (U"Maximum frequency (bark)", U"0");
	OK
DO
	LOOP {
		iam (Sound);
		autoBarkSpectrogram thee = Sound_to_BarkSpectrogram (me, GET_REAL (U"Window length"),
			GET_REAL (U"Time step"), GET_REAL (U"Position of first filter"),
			GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"));
		praat_new (thee.move(), my name);
	}
END

// deprecated
FORM (Sound_to_FormantFilter, U"Sound: To FormantFilter", U"Sound: To FormantFilter...")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (Hz)", U"100.0")
	POSITIVE (U"Distance between filters (Hz)", U"50.0")
	REAL (U"Maximum frequency", U"0");
	POSITIVE (U"Relative bandwidth", U"1.1")
	LABEL (U"", U"Pitch analysis")
	REAL (U"Minimum pitch (Hz)", U"75.0")
	REAL (U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_FormantFilter (me, GET_REAL (U"Window length"),
		GET_REAL (U"Time step"), GET_REAL (U"Position of first filter"),
		GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"),
		GET_REAL (U"Relative bandwidth"), GET_REAL (U"Minimum pitch"),
		GET_REAL (U"Maximum pitch")), my name);
	}
END

FORM (Sound_to_Spectrogram_pitchDependent, U"Sound: To Spectrogram (pitch-dependent)", U"Sound: To Spectrogram (pitch-dependent)...")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (Hz)", U"100.0")
	POSITIVE (U"Distance between filters (Hz)", U"50.0")
	REAL (U"Maximum frequency", U"0");
	POSITIVE (U"Relative bandwidth", U"1.1")
	LABEL (U"", U"Pitch analysis")
	REAL (U"Minimum pitch (Hz)", U"75.0")
	REAL (U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	LOOP {
		iam (Sound);
		autoSpectrogram thee = Sound_to_Spectrogram_pitchDependent (me, GET_REAL (U"Window length"), GET_REAL (U"Time step"), 
			GET_REAL (U"Position of first filter"), GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"),
			GET_REAL (U"Relative bandwidth"), GET_REAL (U"Minimum pitch"), GET_REAL (U"Maximum pitch"));
		praat_new (thee.move(), my name);
	}
END

// deprecated
FORM (Sound_to_MelFilter, U"Sound: To MelFilter", U"Sound: To MelFilter...")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (mel)", U"100.0")
	POSITIVE (U"Distance between filters (mel)", U"100.0")
	REAL (U"Maximum frequency (mel)", U"0.0");
	OK
DO
	LOOP {
		iam (Sound);
		autoMelFilter thee = Sound_to_MelFilter (me, GET_REAL (U"Window length"), GET_REAL (U"Time step"), 
			GET_REAL (U"Position of first filter"), GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sound_to_MelSpectrogram, U"Sound: To MelSpectrogram", U"Sound: To MelSpectrogram...")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (mel)", U"100.0")
	POSITIVE (U"Distance between filters (mel)", U"100.0")
	REAL (U"Maximum frequency (mel)", U"0.0");
	OK
DO
	LOOP {
		iam (Sound);
		autoMelSpectrogram thee = Sound_to_MelSpectrogram (me, GET_REAL (U"Window length"), GET_REAL (U"Time step"), 
			GET_REAL (U"Position of first filter"), GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sound_to_ComplexSpectrogram, U"Sound: To ComplexSpectrogram", 0)
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step", U"0.005")
	OK
DO
	LOOP {
		iam (Sound);
		autoComplexSpectrogram thee = Sound_to_ComplexSpectrogram (me, GET_REAL (U"Window length"), GET_REAL (U"Time step"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sound_to_Pitch_shs, U"Sound: To Pitch (shs)", U"Sound: To Pitch (shs)...")
	POSITIVE (U"Time step (s)", U"0.01")
	POSITIVE (U"Minimum pitch (Hz)", U"50.0")
	NATURAL (U"Max. number of candidates (Hz)", U"15")
	LABEL (U"", U"Algorithm parameters")
	POSITIVE (U"Maximum frequency component (Hz)", U"1250.0")
	NATURAL (U"Max. number of subharmonics", U"15")
	POSITIVE (U"Compression factor (<=1)", U"0.84")
	POSITIVE (U"Ceiling (Hz)", U"600.0")
	NATURAL (U"Number of points per octave", U"48");
	OK
DO
	double minimumPitch = GET_REAL (U"Minimum pitch");
	double fmax = GET_REAL (U"Maximum frequency component");
	double ceiling = GET_REAL (U"Ceiling");
	if (minimumPitch >= ceiling) {
		Melder_throw (U"Minimum pitch should be smaller than ceiling.");
	}
	if (ceiling >= fmax) {
		Melder_throw (U"Maximum frequency must be greater than or equal to ceiling.");
	}
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch_shs (me, GET_REAL (U"Time step"), minimumPitch, fmax, ceiling,
		GET_INTEGER (U"Max. number of subharmonics"), GET_INTEGER (U"Max. number of candidates"),
		GET_REAL (U"Compression factor"), GET_INTEGER (U"Number of points per octave"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sound_fadeIn, U"Sound: Fade in", U"Sound: Fade in...")
	CHANNEL (U"Channel (number, 0 = (all))", U"1")
	REAL (U"Time (s)", U"-10000.0")
	REAL (U"Fade time (s)", U"0.005")
	BOOLEAN (U"Silent from start", 0)
	OK
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (Sound);
		Sound_fade (me, channel, GET_REAL (U"Time"), GET_REAL (U"Fade time"), -1, GET_INTEGER (U"Silent from start"));
		praat_dataChanged (me);
	}
END

FORM (Sound_fadeOut, U"Sound: Fade out", U"Sound: Fade out...")
	CHANNEL (U"Channel (number, 0 = (all))", U"1")
	REAL (U"Time (s)", U"10000.0")
	REAL (U"Fade time (s)", U"-0.005")
	BOOLEAN (U"Silent to end", 0)
	OK
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (Sound);
		Sound_fade (me, channel, GET_REAL (U"Time"), GET_REAL (U"Fade time"), 1, GET_INTEGER (U"Silent to end"));
		praat_dataChanged (me);
	}
END

FORM (Sound_to_KlattGrid_simple, U"Sound: To KlattGrid (simple)", U"Sound: To KlattGrid (simple)...")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Formant determination")
	NATURAL (U"Max. number of formants", U"5")
	POSITIVE (U"Maximum formant (Hz)", U"5500 (=adult female)")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	LABEL (U"", U"Pitch determination")
	POSITIVE (U"Pitch floor (Hz)", U"60.0")
	POSITIVE (U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"", U"Intensity determination")
	POSITIVE (U"Minimum pitch (Hz)", U"100.0")
	BOOLEAN (U"Subtract mean", 1)
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_KlattGrid_simple (me, GET_REAL (U"Time step"),
			GET_INTEGER (U"Max. number of formants"), GET_REAL (U"Maximum formant"),
			GET_REAL (U"Window length"), GET_REAL (U"Pre-emphasis from"),
			GET_REAL (U"Pitch floor"), GET_REAL (U"Pitch ceiling"),
			GET_REAL (U"Minimum pitch"), GET_INTEGER (U"Subtract mean")), my name);
	}
END

FORM (Sound_to_Pitch_SPINET, U"Sound: To SPINET", U"Sound: To SPINET...")
	POSITIVE (U"Time step (s)", U"0.005")
	POSITIVE (U"Window length (s)", U"0.040")
	LABEL (U"", U"Gammatone filter bank")
	POSITIVE (U"Minimum filter frequency (Hz)", U"70.0")
	POSITIVE (U"Maximum filter frequency (Hz)", U"5000.0")
	NATURAL (U"Number of filters", U"250");
	POSITIVE (U"Ceiling (Hz)", U"500.0")
	NATURAL (U"Max. number of candidates", U"15")
	OK
DO
	double fmin = GET_REAL (U"Minimum filter frequency");
	double fmax = GET_REAL (U"Maximum filter frequency");
	if (fmax <= fmin) {
		Melder_throw (U"Maximum frequency must be larger than minimum frequency.");
	}
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch_SPINET (me, GET_REAL (U"Time step"), GET_REAL (U"Window length"), fmin, fmax, 
			GET_INTEGER (U"Number of filters"), GET_REAL (U"Ceiling"), GET_INTEGER (U"Max. number of candidates"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sound_to_Polygon, U"Sound: To Polygon", U"Sound: To Polygon...")
	CHANNEL (U"Channel (number, Left, or Right)", U"1")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Connection y-value", U"0.0")
	OK
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (Sound);
		if (channel > my ny) {
			channel = 1;
		}
		autoPolygon thee = Sound_to_Polygon (me, channel, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_REAL (U"Connection y-value"));
		praat_new (thee.move(), my name);
	}
END

FORM (Sounds_to_Polygon_enclosed, U"Sounds: To Polygon (enclosed)", U"Sounds: To Polygon (enclosed)...")
	CHANNEL (U"Channel (number, Left, or Right)", U"1")
	OPTION (U"Left")
	OPTION (U"Right")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	OK
DO
	long channel = GET_INTEGER (U"Channel");
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	autoPolygon thee = Sounds_to_Polygon_enclosed (s1, s2, channel, GET_REAL (U"left Time range"),
		GET_REAL (U"right Time range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"));
	praat_new (thee.move(), s1->name, U"_", s2->name);
END

FORM (Sound_filterByGammaToneFilter4, U"Sound: Filter (gammatone)", U"Sound: Filter (gammatone)...")
	POSITIVE (U"Centre frequency (Hz)", U"1000.0")
	POSITIVE (U"Bandwidth (Hz)", U"150.0")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filterByGammaToneFilter4 (me, GET_REAL (U"Centre frequency"), GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name, U"_filtered");
	}
END

FORM (Sound_removeNoise, U"Sound: Remove noise", U"Sound: Remove noise...")
	REAL (U"left Noise time range (s)", U"0.0")
	REAL (U"right Noise time range (s)", U"0.0")
	POSITIVE (U"Window length (s)", U"0.025")
	LABEL (U"", U"Filter")
	REAL (U"left Filter frequency range (Hz)", U"80.0")
	REAL (U"right Filter frequency range (Hz)", U"10000.0")
	POSITIVE (U"Smoothing (Hz)", U"40.0")
	OPTIONMENU (U"Noise reduction method", 1)
	OPTION (U"Spectral subtraction")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_removeNoise (me, GET_REAL (U"left Noise time range"), GET_REAL (U"right Noise time range"),
			GET_REAL (U"Window length"), GET_REAL (U"left Filter frequency range"),
			GET_REAL (U"right Filter frequency range"), GET_REAL (U"Smoothing"), GET_INTEGER (U"Noise reduction method"));
		praat_new (thee.move(), my name, U"_denoised");
	}
END

FORM (Sound_changeSpeaker, U"Sound: Change speaker", U"Sound: Change speaker...")
	LABEL (U"", U"Pitch measurement parameters")
	POSITIVE (U"Pitch floor (Hz)", U"75.0")
	POSITIVE (U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"", U"Modification parameters")
	POSITIVE (U"Multiply formants by", U"1.2")
	POSITIVE (U"Multiply pitch by", U"1.0")
	REAL (U"Multiply pitch range by", U"1.0 (=no change)")
	POSITIVE (U"Multiply duration by", U"1.0")
	OK
DO
	double minimumPitch = GET_REAL (U"Pitch floor");
	double maximumPitch = GET_REAL (U"Pitch ceiling");
	if (minimumPitch >= maximumPitch) {
		Melder_throw (U"Maximum pitch should be greater than minimum pitch.");
	}
	LOOP {
		iam (Sound);
		autoSound thee = Sound_changeSpeaker (me, minimumPitch, maximumPitch, GET_REAL (U"Multiply formants by"), 
			GET_REAL (U"Multiply pitch by"), GET_REAL (U"Multiply pitch range by"), GET_REAL (U"Multiply duration by"));
		praat_new (thee.move(), my name, U"_changeSpeaker");
	}
END

FORM (Sound_changeGender, U"Sound: Change gender", U"Sound: Change gender...")
	LABEL (U"", U"Pitch measurement parameters")
	POSITIVE (U"Pitch floor (Hz)", U"75.0")
	POSITIVE (U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"", U"Modification parameters")
	POSITIVE (U"Formant shift ratio", U"1.2")
	REAL (U"New pitch median (Hz)", U"0.0 (=no change)")
	REAL (U"Pitch range factor", U"1.0 (=no change)")
	POSITIVE (U"Duration factor", U"1.0")
	OK
DO
	double minimumPitch = GET_REAL (U"Pitch floor");
	double maximumPitch = GET_REAL (U"Pitch ceiling");
	double pitchrf = GET_REAL (U"Pitch range factor");
	if (minimumPitch >= maximumPitch) {
		Melder_throw (U"Maximum pitch should be greater than minimum pitch.");
	}
	if (pitchrf < 0) {
		Melder_throw (U"Pitch range factor may not be negative");
	}
	LOOP {
		iam (Sound);
		autoSound thee = Sound_changeGender_old (me, minimumPitch, maximumPitch, GET_REAL (U"Formant shift ratio"), 
			GET_REAL (U"New pitch median"), pitchrf, GET_REAL (U"Duration factor"));
		praat_new (thee.move(), my name, U"_changeGender");
	}
END

FORM (Sound_paintWhere, U"Sound paint where", U"Sound: Paint where...")
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Fill from level", U"0.0")
	BOOLEAN (U"Garnish", 1)
	LABEL (U"", U"Paint only those parts where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; always")
	OK
DO
	long numberOfBisections = 10;
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_paintWhere (me, GRAPHICS, GET_COLOUR (U"Colour"), GET_REAL (U"left Time range"),
		GET_REAL (U"right Time range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_REAL (U"Fill from level"), GET_INTEGER (U"Garnish"), numberOfBisections, GET_STRING (U"Formula"),
		interpreter);
	}
END

FORM (Sounds_paintEnclosed, U"Sounds paint enclosed", U"Sounds: Paint enclosed...")
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	Sound s1 = 0, s2 = 0;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	autoPraatPicture picture;
	Sounds_paintEnclosed (s1, s2, GRAPHICS, GET_COLOUR (U"Colour"),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
END

FORM_READ2 (Sound_readFromRawFileLE, U"Read Sound from raw Little Endian file", 0, true) {
	autoSound thee = Sound_readFromRawFile (file, nullptr, 16, 1, 0, 0, 16000);
	praat_new (thee.move(), MelderFile_name (file));
END2 }

FORM_READ2 (Sound_readFromRawFileBE, U"Read Sound from raw 16-bit Little Endian file", 0, true) {
	autoSound thee = Sound_readFromRawFile (file, nullptr, 16, 0, 0, 0, 16000);
	praat_new (thee.move(), MelderFile_name (file));
END2 }

FORM_READ2 (KlattTable_readFromRawTextFile, U"KlattTable_readFromRawTextFile", 0, true) {
	autoKlattTable thee = KlattTable_readFromRawTextFile (file);
	praat_new (thee.move(), MelderFile_name (file));
END2 }

/************ Spectrograms *********************************************/

FORM (Spectrograms_to_DTW, U"Spectrograms: To DTW", 0)
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
	autoDTW thee = Spectrograms_to_DTW (s1, s2, begin, end, slope, 1);
	praat_new (thee.move(), s1->name, U"_", s2->name);
END

/**************** Spectrum *******************************************/

FORM (Spectrum_drawPhases, U"Spectrum: Draw phases", U"Spectrum: Draw phases...")
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0")
	REAL (U"Minimum phase (dB/Hz)", U"0.0 (= auto)")
	REAL (U"Maximum phase (dB/Hz)", U"0.0 (= auto)")
	BOOLEAN (U"Unwrap", 1)
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Spectrum);
		Spectrum_drawPhases (me, GRAPHICS, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		GET_REAL (U"Minimum phase"), GET_REAL (U"Maximum phase"),
		GET_INTEGER (U"Unwrap"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Spectrum_setRealValueInBin, U"Spectrum: Set real value in bin", 0)
	NATURAL (U"Bin number", U"100")
	REAL (U"Value", U"0.0")
	OK
DO
	long binNumber = GET_INTEGER (U"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw (U"Bin number must not exceed number of bins.");
		my z[1][binNumber]= GET_REAL (U"Value");
	}
END

FORM (Spectrum_setImaginaryValueInBin, U"Spectrum: Set imaginary value in bin", 0)
	NATURAL (U"Bin number", U"100")
	REAL (U"Value", U"0.0")
	OK
DO
	long binNumber = GET_INTEGER (U"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw (U"Bin number must not exceed number of bins.");
		my z[2][binNumber]= GET_REAL (U"Value");
	}
END

DIRECT (Spectrum_conjugate)
	LOOP {
		iam (Spectrum);
		Spectrum_conjugate (me);
		praat_dataChanged (me);
	}
END

FORM (Spectrum_shiftFrequencies, U"Spectrum: Shift frequencies", U"Spectrum: Shift frequencies...")
	REAL (U"Shift by (Hz)", U"1000.0")
	POSITIVE (U"New maximum frequency (Hz)", U"22050")
	NATURAL (U"Precision", U"50")
	OK
DO
	double shiftBy = GET_REAL (U"Shift by");
	double newMaximumFrequency = GET_REAL (U"New maximum frequency");
	long precision = GET_INTEGER (U"Precision");
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_shiftFrequencies (me, shiftBy, newMaximumFrequency, precision);
		praat_new (thee.move(), my name, (shiftBy < 0 ? U"_m" : U"_"), (long) floor (shiftBy));
	}
END

DIRECT (Spectra_multiply)
	Spectrum s1 = 0, s2 = 0;
	LOOP {
		iam (Spectrum);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	praat_new (Spectra_multiply (s1, s2), Thing_getName (s1), U"_x_", Thing_getName (s2));
END

FORM (Spectrum_resample, U"Spectrum: Resample", nullptr)
	NATURAL (U"New number of frequencies", U"256")
	OK
DO
	long numberOfFrequencies = GET_INTEGER (U"New number of frequencies");
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_resample (me, numberOfFrequencies);
		praat_new (thee.move(), my name, U"_", numberOfFrequencies);
	}
END

FORM (Spectrum_compressFrequencyDomain, U"Spectrum: Compress frequency domain", nullptr)
	POSITIVE (U"Maximum frequency (Hz)", U"5000.0")
	INTEGER (U"Interpolation depth", U"50")
	RADIO (U"Interpolation scale", 1)
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Logarithmic")
	OK
DO
	double maximumFrequency = GET_REAL (U"Maximum frequency");
	long interpolationDepth = GET_INTEGER (U"Interpolation depth");
	int freqScale = GET_INTEGER (U"Interpolation scale");
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_compressFrequencyDomain (me, maximumFrequency, interpolationDepth, freqScale, 1);
		praat_new (thee.move(), my name, U"_", (long) floor (maximumFrequency));
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
		autoPowerCepstrum thee = Spectrum_to_PowerCepstrum (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Spectrum_to_Cepstrum)
	LOOP {
		iam (Spectrum);
		autoCepstrum thee = Spectrum_to_Cepstrum (me);
		praat_new (thee.move(), my name);
	}
END

/************* SpeechSynthesizer *************************************************/

DIRECT (SpeechSynthesizer_help)
	Melder_help (U"SpeechSynthesizer");
END

FORM (SpeechSynthesizer_create, U"Create SpeechSynthesizer", U"Create SpeechSynthesizer...")
	long prefVoice = Strings_findString (espeakdata_voices_names.get(), U"English");
	if (prefVoice == 0) {
		prefVoice = 1;
	}
	LIST (U"Language", espeakdata_voices_names -> numberOfStrings, (const char32 **) espeakdata_voices_names -> strings, prefVoice)
	long prefVariant = Strings_findString (espeakdata_variants_names.get(), U"default");
	LIST (U"Voice variant", espeakdata_variants_names -> numberOfStrings,
		(const char32 **) espeakdata_variants_names -> strings, prefVariant)
	OK
DO
	long voiceIndex = GET_INTEGER (U"Language");
	long variantIndex = GET_INTEGER (U"Voice variant"); // default is not in the list!
	autoSpeechSynthesizer me = SpeechSynthesizer_create (espeakdata_voices_names -> strings[voiceIndex],
		espeakdata_variants_names -> strings[variantIndex]);
    praat_new (me.move(),  espeakdata_voices_names -> strings[voiceIndex], U"_",
        espeakdata_variants_names -> strings[variantIndex]);
END

FORM (SpeechSynthesizer_playText, U"SpeechSynthesizer: Play text", U"SpeechSynthesizer: Play text...")
	TEXTFIELD (U"Text", U"This is some text.")
	OK
DO
	const char32 *text = GET_STRING (U"Text");
	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_playText (me, text);
	}
END

FORM (SpeechSynthesizer_to_Sound, U"SpeechSynthesizer: To Sound", U"SpeechSynthesizer: To Sound...")
	TEXTFIELD (U"Text", U"This is some text.")
	BOOLEAN (U"Create TextGrid with annotations", false);
	OK
DO
	const char32 *text = GET_STRING (U"Text");
	bool createTextGrid = GET_INTEGER (U"Create TextGrid with annotations");
	LOOP {
		iam (SpeechSynthesizer);
		autoTextGrid tg;
		autoTable t;
		autoSound thee = SpeechSynthesizer_to_Sound (me, text, (createTextGrid ? & tg : nullptr), (Melder_debug == -2 ? & t : nullptr ));
		praat_new (thee.move(), my name);
		if (createTextGrid) {
			praat_new (tg.move(), my name);
		}
		if (Melder_debug == -2) {
			praat_new (t.move(), my name);
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

FORM (SpeechSynthesizer_setTextInputSettings, U"SpeechSynthesizer: Set text input settings", U"SpeechSynthesizer: Set text input settings...")
	OPTIONMENU (U"Input text format is", 1)
		OPTION (U"Text only")
		OPTION (U"Phoneme codes only")
		OPTION (U"Mixed with tags")
	OPTIONMENU (U"Input phoneme codes are", 1)
		OPTION (U"Kirshenbaum_espeak")
	OK
DO
	int inputTextFormat = GET_INTEGER (U"Input text format is");
	int inputPhonemeCoding = SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM; //
	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_setTextInputSettings (me, inputTextFormat, inputPhonemeCoding);
	}
END

FORM (SpeechSynthesizer_setSpeechOutputSettings, U"SpeechSynthesizer: Set speech output settings", U"SpeechSynthesizer: Set speech output settings...")
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	REAL (U"Gap between words (s)", U"0.01")
	INTEGER (U"Pitch adjustment (0-99)", U"50")
	INTEGER (U"Pitch range (0-99)", U"50");
	NATURAL (U"Words per minute (80-450)", U"175");
	BOOLEAN (U"Estimate rate from data", true);
	OPTIONMENU (U"Output phoneme codes are", 2)
		OPTION (U"Kirshenbaum_espeak")
		OPTION (U"IPA")
	OK
DO
	double samplingFrequency = GET_REAL (U"Sampling frequency");
	double wordgap = GET_REAL (U"Gap between words");
	if (wordgap < 0) wordgap = 0;
	long pitchAdjustment = GET_INTEGER (U"Pitch adjustment");   // ppgb: waarom was dit een double?
	if (pitchAdjustment < 0) pitchAdjustment = 0;
	if (pitchAdjustment > 99) pitchAdjustment = 99;
	long pitchRange = GET_INTEGER (U"Pitch range");   // ppgb: waarom was dit een double?
	if (pitchRange < 0) pitchRange = 0;
	if (pitchRange > 99) pitchRange = 99;
	long wordsPerMinute = GET_INTEGER (U"Words per minute");   // ppgb: waarom was dit een double?
	bool estimateWordsPerMinute = GET_INTEGER (U"Estimate rate from data");
	int outputPhonemeCodes = GET_INTEGER (U"Output phoneme codes are");

	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_setSpeechOutputSettings (me, samplingFrequency, wordgap, pitchAdjustment, pitchRange, wordsPerMinute, estimateWordsPerMinute, outputPhonemeCodes);
	}
END

/************* SpeechSynthesizer and TextGrid ************************/

FORM (SpeechSynthesizer_and_TextGrid_to_Sound, U"SpeechSynthesizer & TextGrid: To Sound", nullptr)
	NATURAL (U"Tier number", U"1")
	NATURAL (U"Interval number", U"1")
	BOOLEAN (U"Create TextGrid with annotations", false);
	OK
DO
	bool createAnnotations = GET_INTEGER (U"Create TextGrid with annotations");
	SpeechSynthesizer me = FIRST (SpeechSynthesizer);
	TextGrid thee = FIRST (TextGrid);
	autoTextGrid annotations;
	autoSound him = SpeechSynthesizer_and_TextGrid_to_Sound (me, thee, GET_INTEGER (U"Tier number"),
		GET_INTEGER (U"Interval number"), ( createAnnotations ? & annotations : nullptr ));
	praat_new (him.move(), my name);
	if (createAnnotations) {
		praat_new (annotations.move(), my name);
	}
END

FORM (SpeechSynthesizer_and_Sound_and_TextGrid_align, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align)", nullptr)
	NATURAL (U"Tier number", U"1")
	NATURAL (U"From interval number", U"1")
	NATURAL (U"To interval number", U"1")
	REAL (U"Silence threshold (dB)", U"-35.0")
	POSITIVE (U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (U"Minimum sounding interval duration (s)", U"0.1")
	OK
DO
	double silenceThreshold = GET_REAL (U"Silence threshold");
	double minSilenceDuration = GET_REAL (U"Minimum silent interval duration");
	double minSoundingDuration = GET_REAL (U"Minimum sounding interval duration");
	SpeechSynthesizer synth = FIRST (SpeechSynthesizer);
	Sound s = FIRST (Sound);
	TextGrid tg = FIRST (TextGrid);
	autoTextGrid thee = SpeechSynthesizer_and_Sound_and_TextGrid_align (synth, s, tg,
		GET_INTEGER (U"Tier number"), GET_INTEGER (U"From interval number"),
		GET_INTEGER (U"To interval number"), silenceThreshold, minSilenceDuration, minSoundingDuration);
	praat_new (thee.move(), s -> name, U"_aligned");
END

FORM (SpeechSynthesizer_and_Sound_and_TextGrid_align2, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align, trim)", nullptr)
    NATURAL (U"Tier number", U"1")
    NATURAL (U"From interval number", U"1")
    NATURAL (U"To interval number", U"1")
    REAL (U"Silence threshold (dB)", U"-35.0")
    POSITIVE (U"Minimum silent interval duration (s)", U"0.1")
    POSITIVE (U"Minimum sounding interval duration (s)", U"0.1")
    REAL (U"Silence trim duration (s)", U"0.08")
    OK
DO
    double silenceThreshold = GET_REAL (U"Silence threshold");
    double minSilenceDuration = GET_REAL (U"Minimum silent interval duration");
    double minSoundingDuration = GET_REAL (U"Minimum sounding interval duration");
    double trimDuration = GET_REAL (U"Silence trim duration");
    if (trimDuration < 0) {
        trimDuration = 0;
    }
    SpeechSynthesizer synth = FIRST (SpeechSynthesizer);
    Sound s = FIRST (Sound);
    TextGrid tg = FIRST (TextGrid);
    autoTextGrid thee = SpeechSynthesizer_and_Sound_and_TextGrid_align2 (synth, s, tg,
        GET_INTEGER (U"Tier number"), GET_INTEGER (U"From interval number"),
        GET_INTEGER (U"To interval number"), silenceThreshold, minSilenceDuration, minSoundingDuration, trimDuration);
    praat_new (thee.move(), s -> name, U"_aligned");
END

/************* Spline *************************************************/

FORM (Spline_drawKnots, U"Spline: Draw knots", nullptr)
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (Spline);
		Spline_drawKnots (me, GRAPHICS, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_INTEGER (U"Garnish"));
	}
END

DIRECT (Spline_getOrder)
	LOOP {
		iam (Spline);
		Melder_information (Spline_getOrder (me));
	}
END

FORM (Spline_scaleX, U"Spline: Scale x", U"Spline: Scale x...")
	LABEL (U"", U"New domain")
	REAL (U"Xmin", U"-1.0")
	REAL (U"Xmax", U"1.0")
	OK
DO
	double xmin = GET_REAL (U"Xmin"), xmax = GET_REAL (U"Xmax");
	if (xmin >= xmax) {
		Melder_throw (U"Xmin must be smaller than Xmax.");
	}
	LOOP {
		iam (Spline);
		praat_new (Spline_scaleX (me, xmin, xmax), my name, U"_scaleX");
	}
END

/************ SSCP ***************************************************/

DIRECT (SSCP_help) Melder_help (U"SSCP"); END

FORM (SSCP_drawConfidenceEllipse, U"SSCP: Draw confidence ellipse", nullptr)
	POSITIVE (U"Confidence level", U"0.95")
	NATURAL (U"Index for X-axis", U"1")
	NATURAL (U"Index for Y-axis", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (SSCP);
		SSCP_drawConcentrationEllipse (me, GRAPHICS, GET_REAL (U"Confidence level"), 1,
			GET_INTEGER (U"Index for X-axis"), GET_INTEGER (U"Index for Y-axis"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (SSCP_drawSigmaEllipse, U"SSCP: Draw sigma ellipse", U"SSCP: Draw sigma ellipse...")
	POSITIVE (U"Number of sigmas", U"1.0")
	NATURAL (U"Index for X-axis", U"1")
	NATURAL (U"Index for Y-axis", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (SSCP);
		SSCP_drawConcentrationEllipse (me, GRAPHICS, GET_REAL (U"Number of sigmas"), 0,
			GET_INTEGER (U"Index for X-axis"), GET_INTEGER (U"Index for Y-axis"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (SSCP_extractCentroid)
	LOOP {
		iam (SSCP);
		praat_new (SSCP_extractCentroid (me), my name, U"_centroid");
	}
END

FORM (SSCP_getConfidenceEllipseArea, U"SSCP: Get confidence ellipse area", U"SSCP: Get confidence ellipse area...")
	POSITIVE (U"Confidence level", U"0.95")
	NATURAL (U"Index for X-axis", U"1")
	NATURAL (U"Index for Y-axis", U"2")
	OK
DO
	double conf = GET_REAL (U"Confidence level");
	long d1 = GET_INTEGER (U"Index for X-axis");
	long d2 = GET_INTEGER (U"Index for Y-axis");
	LOOP {
		iam (SSCP);
		Melder_information (SSCP_getConcentrationEllipseArea (me, conf, 1, d1, d2));
	}
END

FORM (SSCP_getFractionVariation, U"SSCP: Get fraction variation", U"SSCP: Get fraction variation...")
	NATURAL (U"From dimension", U"1")
	NATURAL (U"To dimension", U"1")
	OK
DO
	LOOP {
		iam (SSCP);
		Melder_information (SSCP_getFractionVariation (me,
			GET_INTEGER (U"From dimension"), GET_INTEGER (U"To dimension")));
	}
END


FORM (SSCP_getConcentrationEllipseArea, U"SSCP: Get sigma ellipse area", U"SSCP: Get sigma ellipse area...")
	POSITIVE (U"Number of sigmas", U"1.0")
	NATURAL (U"Index for X-axis", U"1")
	NATURAL (U"Index for Y-axis", U"2")
	OK
DO
	double nsigmas = GET_REAL (U"Number of sigmas");
	long d1 = GET_INTEGER (U"Index for X-axis");
	long d2 = GET_INTEGER (U"Index for Y-axis");
	LOOP {
		iam (SSCP);
		Melder_information (SSCP_getConcentrationEllipseArea (me, nsigmas, 0, d1, d2));
}
END

DIRECT (SSCP_getDegreesOfFreedom)
	LOOP {
		iam (SSCP);
		Melder_information (SSCP_getDegreesOfFreedom (me));
	}
END

DIRECT (SSCP_getNumberOfObservations)
	LOOP {
		iam (SSCP);
		Melder_information ((long) floor (my numberOfObservations));   // ppgb: blijf ik raar vinden
	}
END

DIRECT (SSCP_getTotalVariance)
	LOOP {
		iam (SSCP);
		Melder_information (SSCP_getTotalVariance (me));
	}
END

FORM (SSCP_getCentroidElement, U"SSCP: Get centroid element", U"SSCP: Get centroid element")
	NATURAL (U"Number", U"1")
	OK
DO
	long number = GET_INTEGER (U"Number");
	LOOP {
		iam (SSCP);
		if (number < 1 || number > my numberOfColumns) {
			Melder_throw (U"\"Number\" must be smaller than ", my numberOfColumns + 1, U".");
		}
		Melder_information (my centroid[number]);
	}
END

DIRECT (SSCP_getLnDeterminant)
	LOOP {
		iam (SSCP);
		Melder_information (SSCP_getLnDeterminant (me));
	}
END

FORM (SSCP_testDiagonality_bartlett, U"SSCP: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...")
	NATURAL (U"Number of contraints", U"1")
	OK
DO
	double chisq, p, df;
	long nc = GET_INTEGER (U"Number of contraints");
	LOOP {
		iam (SSCP);
		SSCP_testDiagonality_bartlett (me, nc, & chisq, & p, & df);
		Melder_information (p, U" (=probability for chisq = ", chisq, U" and ndf = ", df, U")");
	}
END

DIRECT (SSCP_to_Correlation)
	LOOP {
		iam (SSCP);
		praat_new (SSCP_to_Correlation (me), U"");
	}
END

FORM (SSCP_to_Covariance, U"SSCP: To Covariance", U"SSCP: To Covariance...")
	NATURAL (U"Number of constraints", U"1")
	OK
DO
	long noc = GET_INTEGER (U"Number of constraints");
	LOOP {
		iam (SSCP);
		praat_new (SSCP_to_Covariance (me, noc), U"");
	}
END

DIRECT (SSCP_to_PCA)
	LOOP {
		iam (SSCP);
		praat_new (SSCP_to_PCA (me), U"");
	}
END

/******************* Strings ****************************/
DIRECT (Strings_createFromEspeakVoices)
	//praat_new (nullptr, U"voices"); // TODO ??
END

FORM (Strings_createAsCharacters, U"Strings: Create as characters", nullptr)
	SENTENCE (U"Text", U"intention")
	OK
DO
	autoStrings thee = Strings_createAsCharacters (GET_STRING (U"Text"));
	praat_new (thee.move(), U"");
END

FORM (Strings_createAsTokens, U"Strings: Create as tokens", nullptr)
	SENTENCE (U"Text", U"There are seven tokens in this text")
	OK
DO
	autoStrings thee = Strings_createAsTokens (GET_STRING (U"Text"));
	praat_new (thee.move(), U"");
END

DIRECT (Strings_append)
	OrderedOf<structStrings> list;
	LOOP {
		iam (Strings);
		list. addItem_ref (me);
	}
	autoStrings thee = Strings_append (& list);
	praat_new (thee.move(), U"appended");
END

DIRECT (Strings_to_Categories)
	LOOP {
		iam (Strings);
		autoCategories thee = Strings_to_Categories (me);
		praat_new (thee.move(), U"");
	}
END

FORM (Strings_change, U"Strings: Change", U"Strings: Change")
	SENTENCE (U"Search", U"a")
	SENTENCE (U"Replace", U"a")
	INTEGER (U"Replace limit", U"0 (=unlimited)")
	RADIO (U"Search and replace are:", 1)
	RADIOBUTTON (U"Literals")
	RADIOBUTTON (U"Regular Expressions")
	OK
DO
	long nmatches, nstringmatches;
	LOOP {
		iam (Strings);
		autoStrings thee = Strings_change (me, GET_STRING (U"Search"), GET_STRING (U"Replace"),
		GET_INTEGER (U"Replace limit"), &nmatches, &nstringmatches, GET_INTEGER (U"Search and replace are") - 1);
		praat_new (thee.move());
	}
END

FORM (Strings_extractPart, U"Strings: Extract part", nullptr)
	NATURAL (U"From index", U"1")
	NATURAL (U"To index", U"1")
	OK
DO
	LOOP {
		iam (Strings);
		autoStrings thee = Strings_extractPart (me, GET_INTEGER (U"From index"), GET_INTEGER (U"To index"));
		praat_new (thee.move(), my name, U"_part");
	}
END

DIRECT (Strings_to_EditDistanceTable)
	Strings s1 = nullptr, s2 = nullptr;
	LOOP {
		iam(Strings);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	autoEditDistanceTable table = EditDistanceTable_create (s1, s2);
	praat_new (table.move(), s1 -> name, U"_", s2 -> name);
END

FORM (Strings_to_Permutation, U"Strings: To Permutation", U"Strings: To Permutation...")
	BOOLEAN (U"Sort", true)
	OK
DO
	LOOP {
		iam (Strings);
		autoPermutation thee = Strings_to_Permutation (me, GET_INTEGER (U"Sort"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (Strings_and_Permutation_permuteStrings)
	Strings me = FIRST (Strings);
	Permutation p = FIRST (Permutation);
	autoStrings thee = Strings_and_Permutation_permuteStrings (me, p);
	praat_new (thee.move(), my name, U"_", p->name);
END

FORM (SVD_to_TableOfReal, U"SVD: To TableOfReal", U"SVD: To TableOfReal...")
	NATURAL (U"First component", U"1")
	INTEGER (U"Last component", U"0 (=all)")
	OK
DO
	LOOP {
		iam (SVD);
		autoTableOfReal thee = SVD_to_TableOfReal (me, GET_INTEGER (U"First component"), GET_INTEGER (U"Last component"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (SVD_extractLeftSingularVectors)
	LOOP {
		iam (SVD);
		autoTableOfReal thee = SVD_extractLeftSingularVectors (me);
		praat_new (thee.move(), my name, U"_lsv");
	}
END

DIRECT (SVD_extractRightSingularVectors)
	LOOP {
		iam (SVD);
		autoTableOfReal thee = SVD_extractRightSingularVectors (me);
		praat_new (thee.move(), my name, U"_rsv");
	}
END

DIRECT (SVD_extractSingularValues)
	LOOP {
		iam (SVD);
		autoTableOfReal thee = SVD_extractSingularValues (me);
		praat_new (thee.move(), my name, U"_sv");
	}
END

/******************* Table ****************************/

DIRECT (Table_createFromPetersonBarneyData)
	praat_new (Table_createFromPetersonBarneyData (), U"pb");
END

DIRECT (Table_createFromPolsVanNieropData)
	praat_new (Table_createFromPolsVanNieropData (), U"pvn");
END

DIRECT (Table_createFromWeeninkData)
	praat_new (Table_createFromWeeninkData (), U"m10w10c10");
END

FORM (Table_scatterPlotWhere, U"Table: Scatter plot where", nullptr)
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	WORD (U"Column with marks", U"")
	NATURAL (U"Font size", U"12")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long markColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column with marks"));
		autoTable thee = Table_extractRowsWhere (me,  GET_STRING (U"Formula"), interpreter);
		Table_scatterPlot (thee.get(), GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			markColumn, GET_INTEGER (U"Font size"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_scatterPlotMarkWhere, U"Scatter plot where (marks)", nullptr)
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (U"Mark size (mm)", U"1.0")
	BOOLEAN (U"Garnish", true)
	SENTENCE (U"Mark string (+xo.)", U"+")
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		autoTable thee = Table_extractRowsWhere (me,  GET_STRING (U"Formula"), interpreter);
		Table_scatterPlot_mark (thee.get(), GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Mark size"), GET_STRING (U"Mark string"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_barPlotWhere, U"Table: Bar plot where", U"Table: Bar plot where...")
	SENTENCE (U"Vertical column(s)", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (U"Column with labels", U"")
	LABEL (U"", U"Distances are in units of 'bar width'")
	REAL (U"Distance of first bar from border", U"1.0")
	REAL (U"Distance between bar groups", U"1.0")
	REAL (U"Distance between bars within group", U"0.0")
	SENTENCE (U"Colours", U"Grey")
	REAL (U"Label text angle (degrees)", U"0.0");
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"row >= 1 and row <= 8")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		Table_barPlotWhere (me, GRAPHICS, GET_STRING (U"Vertical column"), GET_REAL (U"left Vertical range"), 
			GET_REAL (U"right Vertical range"), GET_STRING (U"Column with labels"),
			GET_REAL (U"Distance of first bar from border"), GET_REAL (U"Distance between bars within group"), GET_REAL (U"Distance between bar groups"),
			GET_STRING (U"Colours"),GET_REAL (U"Label text angle"), GET_INTEGER (U"Garnish"), GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_LineGraphWhere, U"Table: Line graph where", U"Table: Line graph where...")
	SENTENCE (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (U"Horizonal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Text", U"+")
	REAL (U"Label text angle (degrees)", U"0.0");
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; (= everything)")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long xcolumn = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Horizonal column"));
		Table_lineGraphWhere (me, GRAPHICS, xcolumn, GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			ycolumn, GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), 
			GET_STRING (U"Text"), GET_REAL (U"Label text angle"),
		  	GET_INTEGER (U"Garnish"), GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_boxPlots, U"Table: Box plots", nullptr)
	WORD (U"Data columns", U"")
	WORD (U"Factor column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true);
	OK
DO
	autoPraatPicture picture;
	double ymin = GET_REAL (U"left Vertical range");
	double ymax = GET_REAL (U"right Vertical range");
	int garnish = GET_INTEGER (U"Garnish");
	LOOP {
		iam (Table);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Factor column"));
		Table_boxPlotsWhere (me, GRAPHICS, GET_STRING (U"Data columns"), factorColumn, ymin, ymax, garnish, U"1", interpreter);
	}
END

FORM (Table_boxPlotsWhere, U"Table: Box plots where", U"Table: Box plots where...")
	SENTENCE (U"Data columns", U"")
	WORD (U"Factor column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	double ymin = GET_REAL (U"left Vertical range");
	double ymax = GET_REAL (U"right Vertical range");
	int garnish = GET_INTEGER (U"Garnish");
	char32 *dataColumns = GET_STRING (U"Data columns");
	LOOP {
		iam (Table);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Factor column"));
		Table_boxPlotsWhere (me, GRAPHICS, dataColumns, factorColumn, ymin, ymax, garnish, GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_drawEllipseWhere, U"Draw ellipse (standard deviation)", nullptr)
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (U"Number of sigmas", U"2.0")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (U"Formula"), interpreter);
		Table_drawEllipse_e (thee.get(), GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_drawEllipses, U"Table: Draw ellipses", nullptr)
	WORD (U"Horizontal column", U"F2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"F1")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	WORD (U"Factor column", U"Vowel")
	POSITIVE (U"Number of sigmas", U"1.0")
	NATURAL (U"Font size", U"12")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long factorcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Factor column"));
		Table_drawEllipsesWhere (me, GRAPHICS, xcolumn, ycolumn, factorcolumn, GET_REAL (U"left Horizontal range"), 
			 GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			 GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Font size"), GET_INTEGER (U"Garnish"), U"1", interpreter);
	}
END

FORM (Table_drawEllipsesWhere, U"Table: Draw ellipses where", nullptr)
	WORD (U"Horizontal column", U"F2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"F1")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	WORD (U"Factor column", U"Vowel")
	POSITIVE (U"Number of sigmas", U"1.0")
	NATURAL (U"Font size", U"12")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long factorcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Factor column"));
		Table_drawEllipsesWhere (me, GRAPHICS, xcolumn, ycolumn, factorcolumn, GET_REAL (U"left Horizontal range"), 
			 GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			 GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Font size"), GET_INTEGER (U"Garnish"), GET_STRING (U"Formula"), interpreter);
	}
END


FORM (Table_normalProbabilityPlot, U"Table: Normal probability plot", U"Table: Normal probability plot...")
	WORD (U"Column", U"")
	NATURAL (U"Number of quantiles", U"100")
	REAL (U"Number of sigmas", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", true);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column"));
		Table_normalProbabilityPlot (me, GRAPHICS, column,
			GET_INTEGER (U"Number of quantiles"), GET_REAL (U"Number of sigmas"),
			GET_INTEGER (U"Label size"), GET_STRING (U"Label"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_normalProbabilityPlotWhere, U"Table: Normal probability plot where", U"Table: Normal probability plot...")
	WORD (U"Column", U"")
	NATURAL (U"Number of quantiles", U"100")
	REAL (U"Number of sigmas", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column"));
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (U"Formula"), interpreter);
		Table_normalProbabilityPlot (thee.get(), GRAPHICS, column,
			GET_INTEGER (U"Number of quantiles"), GET_REAL (U"Number of sigmas"),
			GET_INTEGER (U"Label size"), GET_STRING (U"Label"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_quantileQuantilePlot, U"Table: Quantile-quantile plot", U"Table: Quantile-quantile plot...")
	WORD (U"Horizontal axis column", U"")
	WORD (U"Vertical axis column", U"")
	NATURAL (U"Number of quantiles", U"100")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", true);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal axis column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical axis column"));
		Table_quantileQuantilePlot (me, GRAPHICS, xcolumn, ycolumn, GET_INTEGER (U"Number of quantiles"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"), GET_STRING (U"Label"), GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_quantileQuantilePlot_betweenLevels, U"Table: Quantile-quantile plot (between levels)", U"Table: Quantile-quantile plot...")
	WORD (U"Data column", U"")
	WORD (U"Factor column", U"")
	WORD (U"Horizontal factor level", U"")
	WORD (U"Vertical factor level", U"")
	NATURAL (U"Number of quantiles", U"100")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Data column"));
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Factor column"));
		char32 *xLevel = GET_STRING (U"Horizontal factor level");
		char32 *yLevel = GET_STRING (U"Vertical factor level");
		Table_quantileQuantilePlot_betweenLevels (me, GRAPHICS, dataColumn, factorColumn, xLevel, yLevel,
			GET_INTEGER (U"Number of quantiles"), GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"), GET_INTEGER (U"Label size"), GET_STRING (U"Label"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (Table_lagPlot, U"Table: lag plot",0)
	WORD (U"Data column", U"errors")
	NATURAL (U"Lag", U"1")
	REAL (U"left Horizontal and vertical range", U"0.0")
	REAL (U"right Horizontal and vertical range", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Data column"));
		Table_lagPlotWhere (me, GRAPHICS, dataColumn, GET_INTEGER (U"Lag"), GET_REAL (U"left Horizontal and vertical range"),
			GET_REAL (U"right Horizontal and vertical range"), GET_STRING (U"Label"), GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"), U"1", interpreter);
	}
END


FORM (Table_lagPlotWhere, U"Table: lag plot where",0)
	WORD (U"Data column", U"errors")
	NATURAL (U"Lag", U"1")
	REAL (U"left Horizontal and vertical range", U"0.0")
	REAL (U"right Horizontal and vertical range", U"0.0")
	NATURAL (U"Label size", U"12")
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", 1);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Data column"));
		Table_lagPlotWhere (me, GRAPHICS, dataColumn, GET_INTEGER (U"Lag"), GET_REAL (U"left Horizontal and vertical range"),
			GET_REAL (U"right Horizontal and vertical range"), GET_STRING (U"Label"), GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"), GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_distributionPlot, U"Table: Distribution plot", 0)
	WORD (U"Data column", U"data")
	REAL (U"Minimum value", U"0.0")
	REAL (U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURAL (U"Number of bins", U"10")
	REAL (U"Minimum frequency", U"0.0")
	REAL (U"Maximum frequency", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Data column"));
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, GET_REAL (U"Minimum value"), GET_REAL (U"Maximum value"),
			GET_INTEGER (U"Number of bins"), GET_REAL (U"Minimum frequency"), GET_REAL (U"Maximum frequency"), 
			GET_INTEGER (U"Garnish"), U"1", interpreter);
	}
END

FORM (Table_distributionPlotWhere, U"Table: Distribution plot where", 0)
	WORD (U"Data column", U"data")
	REAL (U"Minimum value", U"0.0")
	REAL (U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURAL (U"Number of bins", U"10")
	REAL (U"Minimum frequency", U"0.0")
	REAL (U"Maximum frequency", U"0.0")
	BOOLEAN (U"Garnish", 1)
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Data column"));
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, GET_REAL (U"Minimum value"), GET_REAL (U"Maximum value"),
			GET_INTEGER (U"Number of bins"), GET_REAL (U"Minimum frequency"), GET_REAL (U"Maximum frequency"), GET_INTEGER (U"Garnish"),
			GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_horizontalErrorBarsPlot, U"Table: Horizontal error bars plot", U"Table: Horizontal error bars plot...")
	WORD (U"Horizontal column", U"x")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	WORD (U"Vertical column", U"y")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	WORD (U"Lower error value column", U"")
	WORD (U"Upper error value column", U"")
	REAL (U"Bar size (mm)", U"1.0")
	BOOLEAN (U"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long xl = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Lower error value column"));
		long xu = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Upper error value column"));
		
		Table_horizontalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			xl, xu, GET_REAL (U"Bar size"), GET_INTEGER (U"Garnish"), U"1", interpreter);
	}
END

FORM (Table_horizontalErrorBarsPlotWhere, U"Table: Horizontal error bars plot where", U"Table: Horizontal error bars plot where...")
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	WORD (U"Lower error value column", U"")
	WORD (U"Upper error value column", U"")
	REAL (U"Bar size (mm)", U"1.0")
	BOOLEAN (U"Garnish", 1);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long xl = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Lower error value column"));
		long xu = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Upper error value column"));
		
		Table_horizontalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			xl, xu, GET_REAL (U"Bar size"), GET_INTEGER (U"Garnish"), GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_verticalErrorBarsPlot, U"Table: Vertical error bars plot", U"Table: Vertical error bars plot...")
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	WORD (U"Lower error value column", U"")
	WORD (U"Upper error value column", U"")
	REAL (U"Bar size (mm)", U"1.0")
	BOOLEAN (U"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long yl = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Lower error value column"));
		long yu = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Upper error value column"));
		
		Table_verticalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			yl, yu, GET_REAL (U"Bar size"), GET_INTEGER (U"Garnish"), U"1", interpreter);
	}
END

FORM (Table_verticalErrorBarsPlotWhere, U"Table: Vertical error bars plot where", U"Table: Vertical error bars plot where...")
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	WORD (U"Lower error value column", U"")
	WORD (U"Upper error value column", U"")
	REAL (U"Bar size (mm)", U"1.0")
	BOOLEAN (U"Garnish", 1);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long yl = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Lower error value column"));
		long yu = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Upper error value column"));
		
		Table_verticalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			yl, yu, GET_REAL (U"Bar size"), GET_INTEGER (U"Garnish"), GET_STRING (U"Formula"), interpreter);
	}
END

FORM (Table_extractRowsWhere, U"Table: Extract rows where", 0)
	LABEL (U"", U"Extract rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	LOOP {
		iam (Table);
		autoTable thee = Table_extractRowsWhere (me, GET_STRING (U"Formula"), interpreter);
		praat_new (thee.move(), my name, U"_formula");
	}
END

FORM (Table_extractRowsMahalanobisWhere, U"Table: Extract rows where (mahalanobis)", 0)
	SENTENCE (U"Extract all rows where columns...", U"")
	RADIO_ENUM (U"...have a mahalanobis distance...", kMelder_number, GREATER_THAN)
	REAL (U"...the number", U"2.0")
	WORD (U"Factor column", U"")
	LABEL (U"", U"Process only rows where the following condition holds:")
	TEXTFIELD (U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	double numberOfSigmas = GET_REAL (U"...the number");
	LOOP {
		iam (Table);
		autoTable thee = Table_extractMahalanobisWhere(me, GET_STRING (U"Extract all rows where columns..."), 
		   GET_STRING (U"Factor column"), numberOfSigmas, GET_ENUM (kMelder_number, U"...have a mahalanobis distance..."),
		   GET_STRING (U"Formula"), interpreter);
		praat_new (thee.move(), my name, U"_mahalanobis");
	}
END

FORM (Table_extractColumnRanges, U"Table: Extract column ranges", 0)
	LABEL (U"", U"Create a new Table from the following columns:")
	TEXTFIELD (U"Ranges", U"1 2")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	LOOP {
		iam (Table);
		autoTable thee = Table_extractColumnRanges (me, GET_STRING (U"Ranges"));
		praat_new (thee.move(), my name, U"_columns");
	}
END

/******************* TableOfReal ****************************/

DIRECT (New_CreateIrisDataset)
	praat_new (TableOfReal_createIrisDataset (), U"");
END

FORM (TableOfReal_reportMultivariateNormality, U"TableOfReal: Report multivariate normality (BHEP)", U"TableOfReal: Report multivariate normality (BHEP)...")
	REAL (U"Smoothing parameter", U"0.0")
	OK
DO
	double h = GET_REAL (U"Smoothing parameter");
	MelderInfo_open ();
	LOOP {
		iam (TableOfReal);
		double tnb, lnmu, lnvar;
		double prob = TableOfReal_normalityTest_BHEP (me, &h, &tnb, &lnmu, &lnvar);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Baringhaus–Henze–Epps–Pulley normality test:");
		MelderInfo_writeLine (U"Significance of normality: ", prob);
		MelderInfo_writeLine (U"BHEP statistic: ", tnb);
		MelderInfo_writeLine (U"Lognormal mean: ", lnmu);
		MelderInfo_writeLine (U"Lognormal variance: ", lnvar);
		MelderInfo_writeLine (U"Smoothing: ", h);
		MelderInfo_writeLine (U"Sample size: ", my numberOfRows);
		MelderInfo_writeLine (U"Number of variables: ", my numberOfColumns);
	}
	MelderInfo_close ();
END

DIRECT (TableOfReal_and_Permutation_permuteRows)
	TableOfReal me = FIRST (TableOfReal);
	Permutation p = FIRST (Permutation);
	praat_new (TableOfReal_and_Permutation_permuteRows (me, p),
		my name, U"_", p->name);
END

DIRECT (TableOfReal_to_Permutation_sortRowlabels)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Permutation_sortRowLabels (me), my name);
	}
END

DIRECT (TableOfReal_appendColumns)
	autoTableOfRealList list = TableOfRealList_create ();
	LOOP {
		iam (TableOfReal);
		list -> addItem_ref (me);
	}
	autoTableOfReal result = TableOfRealList_appendColumnsMany (list.get());
	praat_new (result.move(), U"columns_appended");
END

FORM (TableOfReal_createFromPolsData_50males, U"Create TableOfReal (Pols 1973)", U"Create TableOfReal (Pols 1973)...")
	BOOLEAN (U"Include formant levels", false)
	OK
DO
	autoTableOfReal result = TableOfReal_createFromPolsData_50males (
		GET_INTEGER (U"Include formant levels"));
	praat_new (result.move(), U"pols_50males");
END

DIRECT (Table_createFromEspositoData)
	praat_new (Table_createFromEspositoData (), U"h1_h2");
END

DIRECT (Table_createFromGanongData)
	praat_new (Table_createFromGanongData (), U"ganong");
END

FORM (TableOfReal_createFromVanNieropData_25females, U"Create TableOfReal (Van Nierop 1973)...", U"Create TableOfReal (Van Nierop 1973)...")
	BOOLEAN (U"Include formant levels", false)
	OK
DO
	praat_new (TableOfReal_createFromVanNieropData_25females (GET_INTEGER (U"Include formant levels")), U"vannierop_25females");
END

FORM (TableOfReal_createFromWeeninkData, U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Weenink 1985)...")
	RADIO (U"Speakers group", 1)
		RADIOBUTTON (U"Men")
		RADIOBUTTON (U"Women")
		RADIOBUTTON (U"Children")
	OK
DO
	int type = GET_INTEGER (U"Speakers group");
	praat_new (TableOfReal_createFromWeeninkData (type),
		(type == 1 ? U"m10" : type == 2 ? U"w10" : U"c10"));
END

FORM (TableOfReal_drawScatterPlot, U"TableOfReal: Draw scatter plot", U"TableOfReal: Draw scatter plot...")
	LABEL (U"", U"Select the part of the table")
	NATURAL (U"Horizontal axis column number", U"1")
	NATURAL (U"Vertical axis column number", U"2")
	INTEGER (U"left Row number range", U"0")
	INTEGER (U"right Row number range", U"0")
	LABEL (U"", U"Select the drawing area limits")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	NATURAL (U"Label size", U"12")
	BOOLEAN (U"Use row labels", false)
	WORD (U"Label", U"+")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawScatterPlot (me, GRAPHICS,
			GET_INTEGER (U"Horizontal axis column number"),
			GET_INTEGER (U"Vertical axis column number"),
			GET_INTEGER (U"left Row number range"),
			GET_INTEGER (U"right Row number range"),
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Use row labels"),
			GET_STRING (U"Label"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_drawScatterPlotMatrix, U"TableOfReal: Draw scatter plots matrix", 0)
	INTEGER (U"From column", U"0")
	INTEGER (U"To column", U"0")
	POSITIVE (U"Fraction white", U"0.1")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawScatterPlotMatrix (me, GRAPHICS,
			GET_INTEGER (U"From column"),
			GET_INTEGER (U"To column"),
			GET_REAL (U"Fraction white"));
	}
END

FORM (TableOfReal_drawBiplot, U"TableOfReal: Draw biplot", U"TableOfReal: Draw biplot...")
	LABEL (U"", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	REAL (U"Split factor", U"0.5")
	INTEGER (U"Label size", U"10")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawBiplot (me, GRAPHICS,
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_REAL (U"Split factor"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_drawVectors, U"Draw vectors", U"TableOfReal: Draw vectors...")
	LABEL (U"", U"From (x1, y1) to (x2, y2)")
	NATURAL (U"left From columns (x1, y1)", U"1")
	NATURAL (U"right From columns (x1, y1)", U"2")
	NATURAL (U"left To columns (x2, y2)", U"3")
	NATURAL (U"right To columns (x2, y2)", U"4")
	LABEL (U"", U"Select the drawing area")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	RADIO (U"Vector type", 1)
	RADIOBUTTON (U"Arrow")
	RADIOBUTTON (U"Double arrow")
	RADIOBUTTON (U"Line")
	INTEGER (U"Label size", U"10")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawVectors (me, GRAPHICS,
			GET_INTEGER (U"left From columns"),
			GET_INTEGER (U"right From columns"),
			GET_INTEGER (U"left To columns"),
			GET_INTEGER (U"right To columns"),
			GET_REAL (U"left Horizontal range"),
			GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Vector type"),
			GET_INTEGER (U"Label size"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_drawRowAsHistogram, U"Draw row as histogram", U"TableOfReal: Draw rows as histogram...")
	LABEL (U"", U"Select from the table")
	WORD (U"Row number", U"1")
	INTEGER (U"left Column range", U"0")
	INTEGER (U"right Column range", U"0")
	LABEL (U"", U"Vertical drawing range")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	LABEL (U"", U"Offset and distance in units of 'bar width'")
	REAL (U"Horizontal offset", U"0.5")
	REAL (U"Distance between bars", U"1.0")
	WORD (U"Grey value (1=white)", U"0.7")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, GET_STRING (U"Row number"),
			GET_INTEGER (U"left Column range"),
			GET_INTEGER (U"right Column range"),
			GET_REAL (U"left Vertical range"),
			GET_REAL (U"right Vertical range"),
			GET_REAL (U"Horizontal offset"),
			0,
			GET_REAL (U"Distance between bars"),
			GET_STRING (U"Grey value"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_drawRowsAsHistogram, U"Draw rows as histogram", U"TableOfReal: Draw rows as histogram...")
	LABEL (U"", U"Select from the table")
	SENTENCE (U"Row numbers", U"1 2")
	INTEGER (U"left Column range", U"0")
	INTEGER (U"right Column range", U"0")
	LABEL (U"", U"Vertical drawing range")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	LABEL (U"", U"Offset and distance in units of 'bar width'")
	REAL (U"Horizontal offset", U"1.0")
	REAL (U"Distance between bar groups", U"1.0")
	REAL (U"Distance between bars", U"0.0")
	SENTENCE (U"Grey values (1=white)", U"1 1")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS,
		GET_STRING (U"Row numbers"),
		GET_INTEGER (U"left Column range"), GET_INTEGER (U"right Column range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_REAL (U"Horizontal offset"), GET_REAL (U"Distance between bars"),
		GET_REAL (U"Distance between bar groups"), GET_STRING (U"Grey values"),
		GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_drawBoxPlots, U"TableOfReal: Draw box plots", U"TableOfReal: Draw box plots...")
	INTEGER (U"From row", U"0")
	INTEGER (U"To row", U"0")
	INTEGER (U"From column", U"0")
	INTEGER (U"To column", U"0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawBoxPlots (me, GRAPHICS, GET_INTEGER (U"From row"), GET_INTEGER (U"To row"),
		GET_INTEGER (U"From column"), GET_INTEGER (U"To column"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_drawColumnAsDistribution, U"TableOfReal: Draw column as distribution", U"TableOfReal: Draw column as distribution...")
	NATURAL (U"Column number", U"1")
	REAL (U"left Value range", U"0.0")
	REAL (U"right Value range", U"0.0")
	REAL (U"left Frequency range", U"0.0")
	REAL (U"right frequency range", U"0.0")
	NATURAL (U"Number of bins", U"10")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawColumnAsDistribution (me, GRAPHICS, GET_INTEGER (U"Column number"),
		GET_REAL (U"left Value range"), GET_REAL (U"right Value range"), GET_INTEGER (U"Number of bins"),
		GET_REAL (U"left Frequency range"), GET_REAL (U"right frequency range"), false, GET_INTEGER (U"Garnish"));
	}
END

FORM (TableOfReal_to_Configuration_lda, U"TableOfReal: To Configuration (lda)", U"TableOfReal: To Configuration (lda)...")
	INTEGER (U"Number of dimensions", U"0 (= all)")
	OK
DO
	long dimension = GET_INTEGER (U"Number of dimensions");
	if (dimension < 0) {
		Melder_throw (U"Number of dimensions must be greater equal zero.");
	}
	LOOP {
		iam (TableOfReal);
		autoConfiguration thee = TableOfReal_to_Configuration_lda (me, dimension);
		praat_new (thee.move(), my name, U"_lda");
	}
END

FORM (TableOfReal_to_CCA, U"TableOfReal: To CCA", U"TableOfReal: To CCA...")
	NATURAL (U"Dimension of dependent variate", U"2")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoCCA thee = TableOfReal_to_CCA (me, GET_INTEGER (U"Dimension of dependent variate"));
		praat_new (thee.move(), my name);
	}
END

FORM (TableOfReal_to_Configuration_pca, U"TableOfReal: To Configuration (pca)", U"TableOfReal: To Configuration (pca)...")
	NATURAL (U"Number of dimensions", U"2")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoConfiguration thee = TableOfReal_to_Configuration_pca (me, GET_INTEGER (U"Number of dimensions"));
		praat_new (thee.move(), my name, U"_pca");
	}
END

DIRECT (TableOfReal_to_Discriminant)
	LOOP {
		iam (TableOfReal);
		autoDiscriminant thee = TableOfReal_to_Discriminant (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (TableOfReal_to_PCA_byRows)
	LOOP {
		iam (TableOfReal);
		autoPCA thee = TableOfReal_to_PCA_byRows (me);
		praat_new (thee.move(), my name);
	}
END

FORM (TableOfReal_to_SSCP, U"TableOfReal: To SSCP", U"TableOfReal: To SSCP...")
	INTEGER (U"Begin row", U"0")
	INTEGER (U"End row", U"0")
	INTEGER (U"Begin column", U"0")
	INTEGER (U"End column", U"0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_SSCP (me, GET_INTEGER (U"Begin row"), GET_INTEGER (U"End row"),
		GET_INTEGER (U"Begin column"), GET_INTEGER (U"End column")), my name);
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
	TableOfReal t1 = nullptr, t2 = nullptr;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 && t2);
	autoEigen thee = TablesOfReal_to_Eigen_gsvd (t1, t2);
	praat_new (thee.move(), U"");
END

FORM (TableOfReal_and_TableOfReal_crossCorrelations, U"TableOfReal & TableOfReal: Cross-correlations", 0)
	OPTIONMENU (U"Correlations between", 1)
	OPTION (U"Rows")
	OPTION (U"Columns")
	BOOLEAN (U"Center", 0)
	BOOLEAN (U"Normalize", 0)
	OK
DO
	int by_columns = GET_INTEGER (U"Correlations between") - 1;
	TableOfReal t1 = 0, t2 = 0;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 && t2);
	praat_new (TableOfReal_and_TableOfReal_crossCorrelations (t1, t2, by_columns,
			GET_INTEGER (U"Center"), GET_INTEGER (U"Normalize")), (by_columns ? U"by_columns" : U"by_rows"), U"cc");
END

DIRECT (TablesOfReal_to_GSVD)
	TableOfReal t1 = 0, t2 = 0;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 && t2);
	praat_new (TablesOfReal_to_GSVD (t1, t2), U"");
END

FORM (TableOfReal_choleskyDecomposition, U"TableOfReal: Cholesky decomposition", 0)
	BOOLEAN (U"Upper (else L)", 0)
	BOOLEAN (U"Inverse", 0)
	OK
	DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_choleskyDecomposition (me, GET_INTEGER (U"Upper"), GET_INTEGER (U"Inverse")), my name);
	}
END

FORM (TableOfReal_to_Pattern_and_Categories, U"TableOfReal: To Pattern and Categories", U"TableOfReal: To Pattern and Categories...")
	INTEGER (U"left Row range", U"0")
	INTEGER (U"right Row range", U"0 (=all)")
	INTEGER (U"left Column range", U"0")
	INTEGER (U"right Column range", U"0 (=all)")
	OK
	DO
	LOOP {
		iam (TableOfReal);
		autoPattern ap; 
		autoCategories ac;
		TableOfReal_to_Pattern_and_Categories (me, GET_INTEGER (U"left Row range"),
		GET_INTEGER (U"right Row range"), GET_INTEGER (U"left Column range"),
		GET_INTEGER (U"right Column range"), & ap, & ac);
		praat_new (ap.move(), Thing_getName (me));
		praat_new (ac.move(), Thing_getName (me));
	}
END

FORM (TableOfReal_getColumnSum, U"TableOfReal: Get column sum", U"")
	INTEGER (U"Column", U"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (TableOfReal_getColumnSum (me, GET_INTEGER (U"Column")));
	}
END

FORM (TableOfReal_getRowSum, U"TableOfReal: Get row sum", U"")
	INTEGER (U"Row", U"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		Melder_information (TableOfReal_getRowSum (me, GET_INTEGER (U"Row")));
	}
END

DIRECT (TableOfReal_getGrandSum)
	LOOP {
		iam (TableOfReal);
		Melder_information (TableOfReal_getGrandSum (me));
	}
END

FORM (TableOfReal_meansByRowLabels, U"TableOfReal: Means by row labels", U"TableOfReal: To TableOfReal (means by row labels)...")
	BOOLEAN (U"Expand", 0)
	OK
	DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_meansByRowLabels (me, GET_INTEGER (U"Expand"), 0), NAME, U"_byrowlabels");
	}
END

FORM (TableOfReal_mediansByRowLabels, U"TableOfReal: Medians by row labels", U"TableOfReal: To TableOfReal (medians by row labels)...")
	BOOLEAN (U"Expand", 0)
	OK
DO
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_meansByRowLabels (me, GET_INTEGER (U"Expand"), 1), Thing_getName (me), U"_byrowlabels");
	}
END

/***** TableOfReal and FilterBank  *****/

FORM (TextGrid_extendTime, U"TextGrid: Extend time", U"TextGrid: Extend time...")
	LABEL (U"", U"")
	POSITIVE (U"Extend domain by (s)", U"1.0")
	RADIO (U"At", 1)
	RADIOBUTTON (U"End")
	RADIOBUTTON (U"Start")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_extendTime (me, GET_REAL (U"Extend domain by"), GET_INTEGER (U"At") - 1);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_replaceIntervalTexts, U"TextGrid: Replace interval text", U"TextGrid: Replace interval text...")
	LABEL (U"", U"")
	NATURAL (U"Tier number", U"1")
	INTEGER (U"left Interval range", U"0")
	INTEGER (U"right Interval range", U"0")
	SENTENCE (U"Search", U"a")
	SENTENCE (U"Replace", U"a")
	RADIO (U"Search and replace strings are:", 1)
	RADIOBUTTON (U"Literals")
	RADIOBUTTON (U"Regular Expressions")
	OK
DO
	long from = GET_INTEGER (U"left Interval range");
	long to = GET_INTEGER (U"right Interval range");
	int isregexp = GET_INTEGER (U"Search and replace strings are") - 1;
	char32 *search = GET_STRING (U"Search");
	LOOP {
		iam (TextGrid);
		long nmatches, nstringmatches;
		TextGrid_changeLabels (me, GET_INTEGER (U"Tier number"), from, to, search, GET_STRING (U"Replace"),
		isregexp, &nmatches, &nstringmatches);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_replacePointTexts, U"TextGrid: Replace point text", U"TextGrid: Replace point text...")
	LABEL (U"", U"")
	NATURAL (U"Tier number", U"1")
	INTEGER (U"left Interval range", U"0")
	INTEGER (U"right Interval range", U"0")
	SENTENCE (U"Search", U"a")
	SENTENCE (U"Replace", U"a")
	RADIO (U"Search and replace strings are:", 1)
	RADIOBUTTON (U"Literals")
	RADIOBUTTON (U"Regular Expressions")
	OK
DO
	long from = GET_INTEGER (U"left Interval range");
	long to = GET_INTEGER (U"right Interval range");
	LOOP {
		iam (TextGrid);
		long nmatches, nstringmatches;
		TextGrid_changeLabels (me, GET_INTEGER (U"Tier number"), from, to, GET_STRING (U"Search"), GET_STRING (U"Replace"),
		GET_INTEGER (U"Search and replace strings are") - 1, &nmatches, &nstringmatches);
		praat_dataChanged (me);
	}
END

FORM (TextGrids_to_Table_textAlignmentment, U"TextGrids: To Table (text alignment)", 0)
	NATURAL (U"Target tier", U"1")
	NATURAL (U"Source tier", U"1")
	OK
DO
	TextGrid tg1 = 0, tg2 = 0;
    LOOP {
        iam (TextGrid);
        (tg1 ? tg2 : tg1) = me;
    }
    Melder_assert (tg1 && tg2);
	praat_new (TextGrids_to_Table_textAlignmentment (tg1, GET_INTEGER (U"Target tier"), tg2, GET_INTEGER (U"Source tier"), 0), tg1 -> name, U"_", tg2 -> name);
END

FORM (TextGrids_and_EditCostsTable_to_Table_textAlignmentment, U"TextGrids & EditCostsTable: To Table(text alignmentment)", 0)
	NATURAL (U"Target tier", U"1")
	NATURAL (U"Source tier", U"1")
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
	praat_new (TextGrids_to_Table_textAlignmentment (tg1, GET_INTEGER (U"Target tier"), tg2, GET_INTEGER (U"Source tier"), ect), tg1 -> name, U"_", tg2 -> name);
END

FORM (TextGrid_setTierName, U"TextGrid: Set tier name", U"TextGrid: Set tier name...")
	NATURAL (U"Tier number:", U"1")
	SENTENCE (U"Name", U"");
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_setTierName (me, GET_INTEGER (U"Tier number"), GET_STRING (U"Name"));
		praat_dataChanged (me);
	}
END

DIRECT (VowelEditor_create)
	if (theCurrentPraatApplication -> batch) {
		Melder_throw (U"Cannot edit from batch.");
	}
	autoVowelEditor vowelEditor = VowelEditor_create (U"VowelEditor", nullptr);
	vowelEditor.releaseToUser();
END

static autoDaata cmuAudioFileRecognizer (int nread, const char *header, MelderFile file) {
	return nread < 12 || header [0] != 6 || header [1] != 0 ?
	       autoSound () : Sound_readFromCmuAudioFile (file);
}

void praat_CC_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Paint...", nullptr, 1, DO_CC_paint);
	praat_addAction1 (klas, 1, U"Draw...", nullptr, 1, DO_CC_drawC0);
	praat_addAction1 (klas, 1, QUERY_BUTTON, nullptr, 0, 0);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get number of coefficients...", nullptr, 1, DO_CC_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, U"Get value in frame...", nullptr, 1, DO_CC_getValueInFrame);
	praat_addAction1 (klas, 1, U"Get c0 value in frame...", nullptr, 1, DO_CC_getC0ValueInFrame);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_CC_getValue);
	praat_addAction1 (klas, 0, U"To Matrix", nullptr, 0, DO_CC_to_Matrix);
	praat_addAction1 (klas, 2, U"To DTW...", nullptr, 0, DO_CCs_to_DTW);
}

static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm); // deprecated 2014
static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, praat_HIDDEN, DO_Eigen_and_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, praat_HIDDEN, DO_Eigen_and_Matrix_projectColumns);
}

static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm);
static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, praat_HIDDEN, DO_Eigen_and_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, 0, DO_Eigen_and_Matrix_projectColumns);
}

static void praat_Eigen_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Get eigenvalue...", nullptr, 1, DO_Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, U"Get sum of eigenvalues...", nullptr, 1, DO_Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, U"Get number of eigenvectors", nullptr, 1, DO_Eigen_getNumberOfEigenvalues);
	praat_addAction1 (klas, 1, U"Get eigenvector dimension", nullptr, 1, DO_Eigen_getDimension);
	praat_addAction1 (klas, 1, U"Get eigenvector element...", nullptr, 1, DO_Eigen_getEigenvectorElement);
}

static void praat_Eigen_draw_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Draw eigenvalues...", nullptr, 1, DO_Eigen_drawEigenvalues);
	praat_addAction1 (klas, 0, U"Draw eigenvalues (scree)...", nullptr, praat_DEPTH_1 | praat_HIDDEN, DO_Eigen_drawEigenvalues_scree);
	praat_addAction1 (klas, 0, U"Draw eigenvector...", nullptr, 1, DO_Eigen_drawEigenvector);
}

static void praat_Index_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Get number of classes", nullptr, 0, DO_Index_getNumberOfClasses);
	praat_addAction1 (klas, 1, U"To Permutation...", nullptr, 0, DO_Index_to_Permutation);
	praat_addAction1 (klas, 1, U"Extract part...", nullptr, 0, DO_Index_extractPart);
}

static void praat_BandFilterSpectrogram_draw_init (ClassInfo klas);
static void praat_BandFilterSpectrogram_draw_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, DRAW_BUTTON, nullptr, 0, nullptr);
//	praat_addAction1 (klas, 0, U"Paint image...", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_paintImage);
//	praat_addAction1 (klas, 0, U"Draw filters...", nullptr, 1, DO_FilterBank_drawFilters);
//	praat_addAction1 (klas, 0, U"Draw one contour...", nullptr, 1, DO_FilterBank_drawOneContour);
//	praat_addAction1 (klas, 0, U"Draw contours...", nullptr, 1, DO_FilterBank_drawContours);
//	praat_addAction1 (klas, 0, U"Paint contours...", nullptr, 1, DO_FilterBank_paintContours);
//	praat_addAction1 (klas, 0, U"Paint cells...", nullptr, 1, DO_FilterBank_paintCells);
//	praat_addAction1 (klas, 0, U"Paint surface...", nullptr, 1, DO_FilterBank_paintSurface);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, 1, nullptr);
	praat_addAction1 (klas, 0, U"Draw frequency scale...", nullptr, 1, DO_BandFilterSpectrogram_drawFrequencyScale);
}

void praat_Matrixft_query_init (ClassInfo klas);
void praat_Matrixft_query_init (ClassInfo klas) {
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getXofColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getFrequencyOfRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, praat_DEPTH_1, DO_BandFilterSpectrogram_getValueInCell);
}

void praat_Matrixtype_query_init (ClassInfo klas);
void praat_Matrixtype_query_init (ClassInfo klas) {
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, praat_DEPTH_1, DO_FilterBank_getXofColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, praat_DEPTH_1, DO_FilterBank_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, praat_DEPTH_1, DO_FilterBank_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, praat_DEPTH_1, DO_FilterBank_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, praat_DEPTH_1, DO_FilterBank_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, praat_DEPTH_1, DO_FilterBank_getFrequencyOfRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, praat_DEPTH_1, DO_FilterBank_getValueInCell);
}

static void praat_FilterBank_query_init (ClassInfo klas);
static void praat_FilterBank_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_Matrixtype_query_init (klas);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get frequency in Hertz...", nullptr, praat_DEPTH_1, DO_FilterBank_getFrequencyInHertz);
	praat_addAction1 (klas, 1, U"Get frequency in Bark...", nullptr, praat_DEPTH_1, DO_FilterBank_getFrequencyInBark);
	praat_addAction1 (klas, 1, U"Get frequency in mel...", nullptr, praat_DEPTH_1, DO_FilterBank_getFrequencyInMel);
}

static void praat_FilterBank_modify_init (ClassInfo klas);
static void praat_FilterBank_modify_init (ClassInfo klas) {
	// praat_addAction1 (klas, 0, MODIFY_BUTTON, nullptr, 0, 0); 
	praat_addAction1 (klas, 0, U"Equalize intensities...", nullptr, praat_DEPTH_1, DO_FilterBank_equalizeIntensities);
}

static void praat_FilterBank_draw_init (ClassInfo klas);
static void praat_FilterBank_draw_init (ClassInfo klas) {
	// praat_addAction1 (klas, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, U"Draw filters...", nullptr, praat_DEPTH_1, DO_FilterBank_drawFilters);
	praat_addAction1 (klas, 0, U"Draw one contour...", nullptr, praat_DEPTH_1, DO_FilterBank_drawOneContour);
	praat_addAction1 (klas, 0, U"Draw contours...", nullptr, praat_DEPTH_1, DO_FilterBank_drawContours);
	praat_addAction1 (klas, 0, U"Paint image...", nullptr, praat_DEPTH_1, DO_FilterBank_paintImage);
	praat_addAction1 (klas, 0, U"Paint contours...", nullptr, praat_DEPTH_1, DO_FilterBank_paintContours);
	praat_addAction1 (klas, 0, U"Paint cells...", nullptr, praat_DEPTH_1, DO_FilterBank_paintCells);
	praat_addAction1 (klas, 0, U"Paint surface...", nullptr, praat_DEPTH_1, DO_FilterBank_paintSurface);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 0, U"Draw frequency scales...", nullptr, praat_DEPTH_1, DO_FilterBank_drawFrequencyScales);
}

static void praat_FilterBank_all_init (ClassInfo klas);
static void praat_FilterBank_all_init (ClassInfo klas) {
	praat_FilterBank_draw_init (klas);
	praat_FilterBank_query_init (klas);
	praat_FilterBank_modify_init (klas);
	praat_addAction1 (klas, 0, U"To Intensity", nullptr, praat_HIDDEN, DO_FilterBank_to_Intensity);
	praat_addAction1 (klas, 0, U"To Matrix", nullptr, praat_HIDDEN, DO_FilterBank_to_Matrix);
	praat_addAction1 (klas, 2, U"Cross-correlate...", nullptr, praat_HIDDEN, DO_FilterBanks_crossCorrelate);
	praat_addAction1 (klas, 2, U"Convolve...", nullptr, praat_HIDDEN, DO_FilterBanks_convolve);
}

static void praat_FunctionTerms_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (klas, 0, U"Draw...", nullptr, 1, DO_FunctionTerms_draw);
	praat_addAction1 (klas, 0, U"Draw basis function...", nullptr, 1, DO_FunctionTerms_drawBasisFunction);
	praat_addAction1 (klas, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (klas, 1, U"Get number of coefficients", nullptr, 1, DO_FunctionTerms_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, U"Get coefficient...", nullptr, 1, DO_FunctionTerms_getCoefficient);
	praat_addAction1 (klas, 1, U"Get degree", nullptr, 1, DO_FunctionTerms_getDegree);
	praat_addAction1 (klas, 0, U"-- function specifics --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, DO_FunctionTerms_evaluate);
	praat_addAction1 (klas, 1, U"Get minimum...", nullptr, 1, DO_FunctionTerms_getMinimum);
	praat_addAction1 (klas, 1, U"Get x of minimum...", nullptr, 1, DO_FunctionTerms_getXOfMinimum);
	praat_addAction1 (klas, 1, U"Get maximum...", nullptr, 1, DO_FunctionTerms_getMaximum);
	praat_addAction1 (klas, 1, U"Get x of maximum...", nullptr, 1, DO_FunctionTerms_getXOfMaximum);
	praat_addAction1 (klas, 0, U"Modify -", nullptr, 0, 0);
	praat_addAction1 (klas, 1, U"Set domain...", nullptr, 1, DO_FunctionTerms_setDomain);
	praat_addAction1 (klas, 1, U"Set coefficient...", nullptr, 1, DO_FunctionTerms_setCoefficient);
	praat_addAction1 (klas, 0, U"Analyse", nullptr, 0, 0);
}

/* Query buttons for frame-based frequency x time subclasses of matrix. */

void praat_BandFilterSpectrogram_query_init (ClassInfo klas) {
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, 1, DO_BandFilterSpectrogram_getXofColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, 1, DO_BandFilterSpectrogram_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, 1, DO_BandFilterSpectrogram_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, 1, DO_BandFilterSpectrogram_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, 1, DO_BandFilterSpectrogram_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, 1, DO_BandFilterSpectrogram_getFrequencyOfRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, 1, DO_BandFilterSpectrogram_getValueInCell);
}

static void praat_Spline_init (ClassInfo klas) {
	praat_FunctionTerms_init (klas);
	praat_addAction1 (klas, 0, U"Draw knots...", U"Draw basis function...", 1, DO_Spline_drawKnots);
	praat_addAction1 (klas, 1, U"Get order", U"Get degree", 1, DO_Spline_getOrder);
	praat_addAction1 (klas, 1, U"Scale x...", U"Analyse",	0, DO_Spline_scaleX);
}

static void praat_SSCP_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"-- statistics --", U"Get value...", 1, 0);
	praat_addAction1 (klas, 1, U"Get number of observations", U"-- statistics --", 1, DO_SSCP_getNumberOfObservations);
	praat_addAction1 (klas, 1, U"Get degrees of freedom", U"Get number of observations", 1, DO_SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, U"Get centroid element...", U"Get degrees of freedom", 1, DO_SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, U"Get ln(determinant)", U"Get centroid element...", 1, DO_SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Extract centroid", EXTRACT_BUTTON, 1, DO_SSCP_extractCentroid);
}

FORM (SSCP_setValue, U"Covariance: Set value", U"Covariance: Set value...")
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	REAL (U"New value", U"1.0")
	OK
DO
	LOOP {
		iam (SSCP);
		SSCP_setValue (me, GET_INTEGER (U"Row number"), GET_INTEGER (U"Column number"), GET_REAL (U"New value"));
	}
END

FORM (SSCP_setCentroid, U"", 0)
	NATURAL (U"Element number", U"1")
	REAL (U"New value", U"1.0")
	OK
DO
	LOOP {
		iam (SSCP);
		SSCP_setCentroid (me, GET_INTEGER (U"Element number"), GET_REAL (U"New value"));
	}
END

void praat_SSCP_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_removeAction (klas, nullptr, nullptr, U"Set value...");
	praat_addAction1 (klas, 1, U"Set centroid...", U"Formula...", 1, DO_SSCP_setCentroid);
	praat_addAction1 (klas, 1, U"Set value...", U"Formula...", 1, DO_SSCP_setValue);
	praat_addAction1 (klas, 0, U"To TableOfReal", U"To Matrix", 1, DO_TableOfReal_to_TableOfReal);

}

void praat_TableOfReal_init2 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, U"To TableOfReal", U"To Matrix", 1, DO_TableOfReal_to_TableOfReal);
}

void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, U"Set default costs...", U"Formula...", 1, DO_EditDistanceTable_setDefaultCosts);
	praat_removeAction (klas, nullptr, nullptr, U"Draw as numbers...");
	praat_addAction1 (klas, 0, U"Draw...", U"Draw -", 1, DO_EditDistanceTable_draw);
	praat_addAction1 (klas, 0, U"Draw edit operations", U"Draw...", 1, DO_EditDistanceTable_drawEditOperations);
	praat_removeAction (klas, nullptr, nullptr, U"Draw as numbers if...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw as squares...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw vertical lines...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw horizontal lines...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw left and right lines...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw top and bottom lines...");
	praat_removeAction (klas, nullptr, nullptr, U"-- draw lines --");
}

void praat_uvafon_David_init ();
void praat_uvafon_David_init () {
	Data_recognizeFileType (TextGrid_TIMITLabelFileRecognizer);
	Data_recognizeFileType (cmuAudioFileRecognizer);
	
	Thing_recognizeClassesByName (classActivation, classBarkFilter, classBarkSpectrogram,
		classCategories, classCepstrum, classCCA,
		classChebyshevSeries, classClassificationTable, classComplexSpectrogram, classConfusion,
		classCorrelation, classCovariance, classDiscriminant, classDTW,
		classEigen, classExcitationList, classEditCostsTable, classEditDistanceTable,
		classFileInMemory, classFileInMemorySet, classFormantFilter,
		classIndex, classKlattTable,
		classPermutation, classISpline, classLegendreSeries,
		classMelFilter, classMelSpectrogram, classMSpline, classPattern, classPCA, classPolynomial, classRoots,
		classSimpleString, classStringsIndex, classSpeechSynthesizer, classSPINET, classSSCP,
		classSVD, nullptr);
	Thing_recognizeClassByOtherName (classExcitationList, U"Excitations");
	Thing_recognizeClassByOtherName (classFileInMemorySet, U"FilesInMemory");

	VowelEditor_prefs ();

	espeakdata_praat_init ();

	praat_addMenuCommand (U"Objects", U"Technical", U"Report floating point properties", U"Report integer properties", 0, DO_Praat_ReportFloatingPointProperties);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get TukeyQ...", 0, praat_HIDDEN, DO_Praat_getTukeyQ);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get invTukeyQ...", 0, praat_HIDDEN, DO_Praat_getInvTukeyQ);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings from espeak voices", U"Create Strings as directory list...", praat_DEPTH_1 + praat_HIDDEN, DO_Strings_createFromEspeakVoices);
	praat_addMenuCommand (U"Objects", U"New", U"Create iris data set", U"Create TableOfReal...", 1, DO_New_CreateIrisDataset);
	praat_addMenuCommand (U"Objects", U"New", U"Create Permutation...", nullptr, 0, DO_Permutation_create);
	praat_addMenuCommand (U"Objects", U"New", U"Polynomial", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial...", nullptr, 1, DO_Polynomial_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create LegendreSeries...", nullptr, 1, DO_LegendreSeries_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create ChebyshevSeries...", nullptr, 1, DO_ChebyshevSeries_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create MSpline...", nullptr, 1, DO_MSpline_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create ISpline...", nullptr, 1, DO_ISpline_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from gammatone...", U"Create Sound from tone complex...", 1, DO_Sound_createFromGammaTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from gamma-tone...", U"Create Sound from tone complex...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_createFromGammaTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from Shepard tone...", U"Create Sound from gammatone...", 1, DO_Sound_createFromShepardTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from VowelEditor...", U"Create Sound from Shepard tone...", praat_DEPTH_1, DO_VowelEditor_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create SpeechSynthesizer...", U"Create Sound from VowelEditor...", praat_DEPTH_1, DO_SpeechSynthesizer_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Pols & Van Nierop 1973)", U"Create Table...", 1, DO_Table_createFromPolsVanNieropData);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Peterson & Barney 1952)", U"Create Table...", 1, DO_Table_createFromPetersonBarneyData);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Weenink 1985)", U"Create formant table (Peterson & Barney 1952)", 1, DO_Table_createFromWeeninkData);
	praat_addMenuCommand (U"Objects", U"New", U"Create H1H2 table (Esposito 2006)", U"Create formant table (Weenink 1985)", praat_DEPTH_1+ praat_HIDDEN, DO_Table_createFromEspositoData);
	praat_addMenuCommand (U"Objects", U"New", U"Create Table (Ganong 1980)", U"Create H1H2 table (Esposito 2006)", praat_DEPTH_1+ praat_HIDDEN, DO_Table_createFromGanongData);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Pols 1973)...", U"Create TableOfReal...", 1, DO_TableOfReal_createFromPolsData_50males);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Van Nierop 1973)...", U"Create TableOfReal (Pols 1973)...", 1, DO_TableOfReal_createFromVanNieropData_25females);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Van Nierop 1973)...", 1, DO_TableOfReal_createFromWeeninkData);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Confusion...", U"Create TableOfReal (Weenink 1985)...", 1, DO_Confusion_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Covariance...", U"Create simple Confusion...", 1, DO_Covariance_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Correlation...", U"Create simple Covariance...", 1, DO_Correlation_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create empty EditCostsTable...", U"Create simple Covariance...", 1, DO_EditCostsTable_createEmpty);

	praat_addMenuCommand (U"Objects", U"New", U"Create KlattTable example", U"Create TableOfReal (Weenink 1985)...", praat_DEPTH_1 + praat_HIDDEN, DO_KlattTable_createExample);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as characters...", U"Create TextGrid...", praat_HIDDEN, DO_Strings_createAsCharacters);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as tokens...", U"Create TextGrid...", praat_HIDDEN, DO_Strings_createAsTokens);

	praat_addMenuCommand (U"Objects", U"New", U"Create simple Polygon...", nullptr, praat_HIDDEN, DO_Polygon_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polygon (random vertices)...", nullptr, praat_HIDDEN, DO_Polygon_createFromRandomVertices);
	praat_addMenuCommand (U"Objects", U"New", U"FileInMemory", nullptr, praat_HIDDEN, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemory...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_FileInMemory_create);
		//praat_addMenuCommand (U"Objects", U"New", U"Create copy from FilesInMemory...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_FileInMemorySet_createCopyFromFileInMemorySet);
		praat_addMenuCommand (U"Objects", U"New", U"Create copy from FileInMemorySet...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_FileInMemorySet_createCopyFromFileInMemorySet);
		praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemorySet from directory contents...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_FileInMemorySet_createFromDirectoryContents);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Little Endian file...", U"Read from special sound file", 1, DO_Sound_readFromRawFileLE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Big Endian file...", U"Read Sound from raw 16-bit Little Endian file...", 1, DO_Sound_readFromRawFileBE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read KlattTable from raw text file...", U"Read Matrix from raw text file...", praat_HIDDEN, DO_KlattTable_readFromRawTextFile);

	praat_addAction1 (classActivation, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classActivation, 0, U"Formula...", nullptr, 0, DO_Activation_formula);
	praat_addAction1 (classActivation, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classActivation, 0, U"To Matrix", nullptr, 0, DO_Activation_to_Matrix);

	praat_addAction2 (classActivation, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classBarkFilter, 0, U"BarkFilter help", nullptr, 0, DO_BarkFilter_help);
	praat_FilterBank_all_init (classBarkFilter);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPTH_1, DO_BarkFilter_drawSpectrum);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 1, U"Draw filter functions...", U"Draw filters...", praat_DEPTH_1, DO_BarkFilter_drawSekeyHansonFilterFunctions);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"Paint...", U"Draw filters...", praat_DEPTH_1, DO_BarkFilter_paint);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"To BarkSpectrogram", nullptr, 0, DO_BarkFilter_to_BarkSpectrogram);

	praat_addAction1 (classBarkSpectrogram, 0, U"BarkSpectrogram help", nullptr, 0, DO_BarkSpectrogram_help);
	praat_BandFilterSpectrogram_draw_init (classBarkSpectrogram);
	praat_addAction1 (classBarkSpectrogram, 0, U"Paint image...", nullptr, 1, DO_BarkSpectrogram_paintImage);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw Sekey-Hanson auditory filters...", nullptr, 1, DO_BarkSpectrogram_drawSekeyHansonAuditoryFilters);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw spectrum at nearest time slice...", nullptr, 1, DO_BarkSpectrogram_drawSpectrumAtNearestTimeSlice);
	praat_addAction1 (classBarkSpectrogram, 0, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_BandFilterSpectrogram_query_init (classBarkSpectrogram);
	praat_addAction1 (classBarkSpectrogram, 0, U"Equalize intensities...", nullptr, 0, DO_BandFilterSpectrogram_equalizeIntensities);
	praat_addAction1 (classBarkSpectrogram, 0, U"To Intensity", nullptr, 0, DO_BandFilterSpectrogram_to_Intensity);
	praat_addAction1 (classBarkSpectrogram, 0, U"To Matrix...", nullptr, 0, DO_BandFilterSpectrogram_to_Matrix);
	praat_addAction1 (classBarkSpectrogram, 2, U"Cross-correlate...", nullptr, 0, DO_BandFilterSpectrograms_crossCorrelate);
	praat_addAction1 (classBarkSpectrogram, 2, U"Convolve...", nullptr, 0, DO_BandFilterSpectrograms_convolve);
	
	
	praat_addAction1 (classCategories, 0, U"View & Edit", nullptr, 0, DO_Categories_edit);
	praat_addAction1 (classCategories, 0, U"Edit", nullptr, praat_HIDDEN, DO_Categories_edit); // deprecated 2015
	praat_addAction1 (classCategories, 0, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 1, U"Get number of categories", QUERY_BUTTON, 1, DO_Categories_getNumberOfCategories);
	praat_addAction1 (classCategories, 2, U"Get difference", QUERY_BUTTON, praat_HIDDEN | praat_DEPTH_1, DO_Categories_difference);
	praat_addAction1 (classCategories, 2, U"Get number of differences", QUERY_BUTTON, 1, DO_Categories_getNumberOfDifferences);
	praat_addAction1 (classCategories, 2, U"Get fraction different", QUERY_BUTTON, 1, DO_Categories_getFractionDifferent);
	praat_addAction1 (classCategories, 0, MODIFY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 1, U"Append category...", MODIFY_BUTTON, 1, DO_Categories_append);
	praat_addAction1 (classCategories, 1, U"Append 1 category...", U"Append category...", praat_HIDDEN | praat_DEPTH_1, DO_Categories_append);
	praat_addAction1 (classCategories, 0, U"Extract", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 0, U"To unique Categories", nullptr, 0, DO_Categories_selectUniqueItems);
	praat_addAction1 (classCategories, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 2, U"To Confusion", nullptr, 0, DO_Categories_to_Confusion);
	praat_addAction1 (classCategories, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 2, U"Join", nullptr, 0, DO_Categories_join);
	praat_addAction1 (classCategories, 0, U"Permute items", nullptr, 0, DO_Categories_permuteItems);
	praat_addAction1 (classCategories, 0, U"To Strings", nullptr, 0, DO_Categories_to_Strings);

	praat_addAction1 (classChebyshevSeries, 0, U"ChebyshevSeries help", nullptr, 0, DO_ChebyshevSeries_help);
	praat_FunctionTerms_init (classChebyshevSeries);
	praat_addAction1 (classChebyshevSeries, 0, U"To Polynomial", U"Analyse", 0, DO_ChebyshevSeries_to_Polynomial);

	praat_addAction1 (classCCA, 1, U"Draw eigenvector...", nullptr, 0, DO_CCA_drawEigenvector);
	praat_addAction1 (classCCA, 1, U"Get number of correlations", nullptr, 0, DO_CCA_getNumberOfCorrelations);
	praat_addAction1 (classCCA, 1, U"Get correlation...", nullptr, 0, DO_CCA_getCorrelationCoefficient);
	praat_addAction1 (classCCA, 1, U"Get eigenvector element...", nullptr, 0, DO_CCA_getEigenvectorElement);
	praat_addAction1 (classCCA, 1, U"Get zero correlation probability...", nullptr, 0, DO_CCA_getZeroCorrelationProbability);

	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (scores)...", nullptr, 0, DO_CCA_and_TableOfReal_scores);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (loadings)", nullptr, 0, DO_CCA_and_TableOfReal_factorLoadings);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"Predict...", nullptr, 0, DO_CCA_and_TableOfReal_predict);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"To TableOfReal (loadings)", nullptr, 0, DO_CCA_and_Correlation_factorLoadings);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get variance fraction...", nullptr, 0, DO_CCA_and_Correlation_getVarianceFraction);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get redundancy (sl)...", nullptr, 0, DO_CCA_and_Correlation_getRedundancy_sl);
	
	
	praat_addAction1 (classComplexSpectrogram, 0, U"ComplexSpectrogram help", nullptr, 0, DO_ComplexSpectrogram_help);
	praat_addAction1 (classComplexSpectrogram, 0, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Sound...", nullptr, 0, DO_ComplexSpectrogram_to_Sound);
	praat_addAction1 (classComplexSpectrogram, 0, U"Down to Spectrogram", nullptr, 0, DO_ComplexSpectrogram_to_Spectrogram);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Spectrum (slice)...", nullptr, 0, DO_ComplexSpectrogram_to_Spectrum);
	//praat_addAction1 (classComplexSpectrogram, 0, U"Paint...", 0, 1, DO_Spectrogram_paint);

	praat_addAction1 (classConfusion, 0, U"Confusion help", nullptr, 0, DO_Confusion_help);
	praat_TableOfReal_init2 (classConfusion);
	praat_removeAction (classConfusion, nullptr, nullptr, U"Draw as numbers...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by label...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by column...");
	praat_addAction1 (classConfusion, 0, U"Draw as numbers...", U"Draw -", 1, DO_Confusion_drawAsNumbers);
	praat_addAction1 (classConfusion, 1, U"Get value (labels)...", U"Get value...", 1, DO_Confusion_getValue);
	praat_addAction1 (classConfusion, 0, U"-- confusion statistics --", U"Get value (labels)...", 1, nullptr);
	praat_addAction1 (classConfusion, 1, U"Get fraction correct", U"-- confusion statistics --", 1, DO_Confusion_getFractionCorrect);
	praat_addAction1 (classConfusion, 1, U"Get stimulus sum...", U"Get fraction correct", 1, DO_Confusion_getStimulusSum);
	praat_addAction1 (classConfusion, 1, U"Get row sum...", U"Get fraction correct", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_getRowSum);
	praat_addAction1 (classConfusion, 1, U"Get response sum...", U"Get stimulus sum...", 1, DO_Confusion_getResponseSum);
	praat_addAction1 (classConfusion, 1, U"Get column sum...", U"Get row sum...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_getColumnSum);
	praat_addAction1 (classConfusion, 1, U"Get grand sum", U"Get response sum...", 1, DO_TableOfReal_getGrandSum);
	praat_addAction1 (classConfusion, 0, U"Increase...", U"Formula...", 1, DO_Confusion_increase);
	praat_addAction1 (classConfusion, 0, U"To TableOfReal (marginals)", U"To TableOfReal", 0, DO_Confusion_to_TableOfReal_marginals);
	praat_addAction1 (classConfusion, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classConfusion, 0, U"Condense...", nullptr, praat_HIDDEN, DO_Confusion_condense);
	praat_addAction1 (classConfusion, 0, U"Group...", nullptr, 0, DO_Confusion_group);
	praat_addAction1 (classConfusion, 0, U"Group stimuli...", nullptr, 0, DO_Confusion_groupStimuli);
	praat_addAction1 (classConfusion, 0, U"Group responses...", nullptr, 0, DO_Confusion_groupResponses);
	praat_addAction1 (classConfusion, 2, U"To difference matrix", nullptr, 0, DO_Confusion_difference);
	
	praat_addAction2 (classConfusion, 1, classClassificationTable, 1, U"Increase confusion count", nullptr, 0, DO_Confusion_and_ClassificationTable_increase);

	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw confusion...", nullptr, 0, DO_Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, U"Covariance help", nullptr, 0, DO_Covariance_help);
	praat_SSCP_as_TableOfReal_init (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
	praat_addAction1 (classCovariance, 1, U"Get probability at position...", U"Get value...", 1, DO_Covariance_getProbabilityAtPosition);
	praat_addAction1 (classCovariance, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, DO_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classCovariance, 1, U"Get significance of one mean...", U"Get diagonality (bartlett)...", 1, DO_Covariance_getSignificanceOfOneMean);
	praat_addAction1 (classCovariance, 1, U"Get significance of means difference...", U"Get significance of one mean...", 1, DO_Covariance_getSignificanceOfMeansDifference);
	praat_addAction1 (classCovariance, 1, U"Get significance of one variance...", U"Get significance of means difference...", 1, DO_Covariance_getSignificanceOfOneVariance);
	praat_addAction1 (classCovariance, 1, U"Get significance of variances ratio...", U"Get significance of one variance...", 1, DO_Covariance_getSignificanceOfVariancesRatio);
	praat_addAction1 (classCovariance, 1, U"Get fraction variance...", U"Get significance of variances ratio...", 1, DO_Covariance_getFractionVariance);
	praat_addAction1 (classCovariance, 2, U"Report multivariate mean difference...", U"Get fraction variance...", 1, DO_Covariances_reportMultivariateMeanDifference);
	praat_addAction1 (classCovariance, 2, U"Difference", U"Report multivariate mean difference...", praat_DEPTH_1 | praat_HIDDEN, DO_Covariances_reportEquality);
	praat_addAction1 (classCovariance, 0, U"Report equality of covariances", U"Report multivariate mean difference...", praat_DEPTH_1 | praat_HIDDEN, DO_Covariances_reportEquality);

	praat_addAction1 (classCovariance, 0, U"To TableOfReal (random sampling)...", nullptr, 0, DO_Covariance_to_TableOfReal_randomSampling);

	praat_addAction1 (classCovariance, 0, U"To Correlation", nullptr, 0, DO_Covariance_to_Correlation);
	praat_addAction1 (classCovariance, 0, U"To PCA", nullptr, 0, DO_Covariance_to_PCA);
	
	praat_addAction1 (classCovariance, 0, U"Pool", nullptr, 0, DO_Covariances_pool);

	praat_addAction2 (classCovariance, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0, DO_Covariance_and_TableOfReal_mahalanobis);

	praat_addAction1 (classClassificationTable, 0, U"ClassificationTable help", nullptr, 0, DO_ClassificationTable_help);
	praat_TableOfReal_init (classClassificationTable);
	praat_addAction1 (classClassificationTable, 0, U"Get class index at maximum in row...", U"Get column index...", 1, DO_ClassificationTable_getClassIndexAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"Get class label at maximum in row...", U"Get class index at maximum in row...", 1, DO_ClassificationTable_getClassLabelAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"To Confusion", nullptr, praat_HIDDEN, DO_ClassificationTable_to_Confusion_old); // deprecated 2014
	praat_addAction1 (classClassificationTable, 0, U"To Confusion...", nullptr, 0, DO_ClassificationTable_to_Confusion);
	praat_addAction1 (classClassificationTable, 0, U"To Correlation (columns)", nullptr, 0, DO_ClassificationTable_to_Correlation_columns);
	praat_addAction1 (classClassificationTable, 0, U"To Strings (max. prob.)", nullptr, 0, DO_ClassificationTable_to_Strings_maximumProbability);

	praat_addAction1 (classCorrelation, 0, U"Correlation help", nullptr, 0, DO_Correlation_help);
	praat_TableOfReal_init2 (classCorrelation);
	praat_SSCP_query_init (classCorrelation);
	praat_SSCP_extract_init (classCorrelation);
	praat_addAction1 (classCorrelation, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, DO_Correlation_testDiagonality_bartlett);
	praat_addAction1 (classCorrelation, 0, U"Confidence intervals...", nullptr, 0, DO_Correlation_confidenceIntervals);
	praat_addAction1 (classCorrelation, 0, U"To PCA", nullptr, 0, DO_Correlation_to_PCA);

	praat_addAction1 (classDiscriminant, 0, U"Discriminant help", 0, 0, DO_Discriminant_help);
	praat_addAction1 (classDiscriminant, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvalues...", nullptr, 1, DO_Discriminant_drawEigenvalues);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvalues (scree)...", nullptr, praat_DEPTH_1 | praat_HIDDEN, DO_Eigen_drawEigenvalues_scree);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvector...", nullptr, 1, DO_Discriminant_drawEigenvector);

	praat_addAction1 (classDiscriminant, 0, U"-- sscps --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 0, U"Draw sigma ellipses...", 0, 1, DO_Discriminant_drawSigmaEllipses);
	praat_addAction1 (classDiscriminant, 0, U"Draw one sigma ellipse...", 0, 1, DO_Discriminant_drawOneSigmaEllipse);
	praat_addAction1 (classDiscriminant, 0, U"Draw confidence ellipses...", 0, 1, DO_Discriminant_drawConfidenceEllipses);

	praat_addAction1 (classDiscriminant, 1, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 1, U"-- eigen structure --", nullptr, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvalue...", nullptr, 1, DO_Discriminant_getEigenvalue);
	praat_addAction1 (classDiscriminant, 1, U"Get sum of eigenvalues...", nullptr, 1, DO_Discriminant_getSumOfEigenvalues);
	praat_addAction1 (classDiscriminant, 1, U"Get number of eigenvectors", nullptr, 1, DO_Discriminant_getNumberOfEigenvalues);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector dimension", nullptr, 1, DO_Discriminant_getDimension);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector element...", nullptr, 1, DO_Discriminant_getEigenvectorElement);

	praat_addAction1 (classDiscriminant, 1, U"-- discriminant --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get number of functions", 0, 1, DO_Discriminant_getNumberOfFunctions);
	praat_addAction1 (classDiscriminant, 1, U"Get dimension of functions", 0, 1, DO_Discriminant_getDimensionOfFunctions);
	praat_addAction1 (classDiscriminant, 1, U"Get number of groups", 0, 1, DO_Discriminant_getNumberOfGroups);
	praat_addAction1 (classDiscriminant, 1, U"Get number of observations...", 0, 1, DO_Discriminant_getNumberOfObservations);
	praat_addAction1 (classDiscriminant, 1, U"-- tests --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get Wilks lambda...", 0, 1, DO_Discriminant_getWilksLambda);
	praat_addAction1 (classDiscriminant, 1, U"Get cumulative contribution of components...", 0, 1, DO_Discriminant_getCumulativeContributionOfComponents);
	praat_addAction1 (classDiscriminant, 1, U"Get partial discrimination probability...", 0, 1,
	                  DO_Discriminant_getPartialDiscriminationProbability);
	praat_addAction1 (classDiscriminant, 1, U"Get homogeneity of covariances (box)", 0, praat_DEPTH_1 | praat_HIDDEN,
	                  DO_Discriminant_getHomegeneityOfCovariances_box);
	praat_addAction1 (classDiscriminant, 1, U"Report equality of covariance matrices", 0, 1,
	                  DO_Discriminant_reportEqualityOfCovariances_wald);
	praat_addAction1 (classDiscriminant, 1, U"-- ellipses --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get sigma ellipse area...", 0, 1, DO_Discriminant_getConcentrationEllipseArea);
	praat_addAction1 (classDiscriminant, 1, U"Get confidence ellipse area...", 0, 1, DO_Discriminant_getConfidenceEllipseArea);
	praat_addAction1 (classDiscriminant, 1, U"Get ln(determinant_group)...", 0, 1, DO_Discriminant_getLnDeterminant_group);
	praat_addAction1 (classDiscriminant, 1, U"Get ln(determinant_total)", 0, 1, DO_Discriminant_getLnDeterminant_total);

	praat_addAction1 (classDiscriminant, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 1, U"Invert eigenvector...", nullptr, 1, DO_Discriminant_invertEigenvector);
	praat_addAction1 (classDiscriminant, 0, U"Align eigenvectors", nullptr, 1, DO_Eigens_alignEigenvectors);

	praat_addAction1 (classDiscriminant, 0, EXTRACT_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 1, U"Extract pooled within-groups SSCP", nullptr, 1,
	                  DO_Discriminant_extractPooledWithinGroupsSSCP);
	praat_addAction1 (classDiscriminant, 1, U"Extract within-group SSCP...", nullptr, 1, DO_Discriminant_extractWithinGroupSSCP);
	praat_addAction1 (classDiscriminant, 1, U"Extract between-groups SSCP", nullptr, 1, DO_Discriminant_extractBetweenGroupsSSCP);
	praat_addAction1 (classDiscriminant, 1, U"Extract group centroids", nullptr, 1, DO_Discriminant_extractGroupCentroids);
	praat_addAction1 (classDiscriminant, 1, U"Extract group standard deviations", nullptr, 1, DO_Discriminant_extractGroupStandardDeviations);
	praat_addAction1 (classDiscriminant, 1, U"Extract group labels", nullptr, 1, DO_Discriminant_extractGroupLabels);

	praat_addAction1 (classDiscriminant , 0, U"& TableOfReal: To ClassificationTable?", nullptr, 0, DO_hint_Discriminant_and_TableOfReal_to_ClassificationTable);

	/*		praat_addAction1 (classDiscriminant, 1, U"Extract coefficients...", nullptr, 1, DO_Discriminant_extractCoefficients);*/

	//praat_addAction2 (classDiscriminant, 1, classMatrix, 1, U"To Matrix (pc)...", nullptr, 0, DO_Discriminant_and_Matrix_to_Matrix_pc);

	praat_Eigen_Spectrogram_project (classDiscriminant, classSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classMelSpectrogram);
	
	praat_Eigen_Matrix_project (classDiscriminant, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classMelFilter); // deprecated 2014

	praat_addAction2 (classDiscriminant, 1, classPattern, 1, U"To Categories...", nullptr, 0, DO_Discriminant_and_Pattern_to_Categories);
	praat_addAction2 (classDiscriminant, 1, classSSCP, 1, U"Project", nullptr, 0, DO_Eigen_and_SSCP_project);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Modify Discriminant", nullptr, 0, 0);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Set group labels", nullptr, 0, DO_Discriminant_setGroupLabels);

	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0, DO_Discriminant_and_TableOfReal_to_Configuration);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To ClassificationTable...", nullptr, 0,
	                  DO_Discriminant_and_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0, DO_Discriminant_and_TableOfReal_mahalanobis);


	praat_addAction1 (classDTW, 0, U"DTW help", nullptr, 0, DO_DTW_help);
	praat_addAction1 (classDTW, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 0, U"Draw path...", nullptr, 1, DO_DTW_drawPath);
	praat_addAction1 (classDTW, 0, U"Paint distances...", nullptr, 1, DO_DTW_paintDistances);
	praat_addAction1 (classDTW, 0, U"Draw warp (x)...", nullptr, 1, DO_DTW_drawWarpX);
	praat_addAction1 (classDTW, 0, U"Draw warp (y)...", nullptr, 1, DO_DTW_drawWarpY);
	praat_addAction1 (classDTW, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 1, U"Query time domains", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (x)", nullptr, 2, DO_DTW_getStartTimeX);
	praat_addAction1 (classDTW, 1, U"Get end time (x)", nullptr, 2, DO_DTW_getEndTimeX);
	praat_addAction1 (classDTW, 1, U"Get total duration (x)", nullptr, 2, DO_DTW_getTotalDurationX);
	praat_addAction1 (classDTW, 1, U"-- time domain x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (y)", nullptr, 2, DO_DTW_getStartTimeY);
	praat_addAction1 (classDTW, 1, U"Get end time (y)", nullptr, 2, DO_DTW_getEndTimeY);
	praat_addAction1 (classDTW, 1, U"Get total duration (y)", nullptr, 2, DO_DTW_getTotalDurationY);
	praat_addAction1 (classDTW, 1, U"Query time samplings", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (x)", nullptr, 2, DO_DTW_getNumberOfFramesX);
	praat_addAction1 (classDTW, 1, U"Get time step (x)", nullptr, 2, DO_DTW_getTimeStepX);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (x)...", nullptr, 2, DO_DTW_getTimeFromFrameNumberX);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (x)...", nullptr, 2, DO_DTW_getFrameNumberFromTimeX);
	praat_addAction1 (classDTW, 1, U"-- time sampling x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (y)", nullptr, 2, DO_DTW_getNumberOfFramesY);
	praat_addAction1 (classDTW, 1, U"Get time step (y)", nullptr, 2, DO_DTW_getTimeStepY);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (y)...", nullptr, 2, DO_DTW_getTimeFromFrameNumberY);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (y)...", nullptr, 2, DO_DTW_getFrameNumberFromTimeY);

	praat_addAction1 (classDTW, 1, U"Get y time from x time...", nullptr, 1, DO_DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, U"Get x time from y time...", nullptr, 1, DO_DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, U"Get y time...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, U"Get x time...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, U"Get maximum consecutive steps...", nullptr, 1, DO_DTW_getMaximumConsecutiveSteps);
	praat_addAction1 (classDTW, 1, U"Get time along path...", nullptr, praat_DEPTH_1 | praat_HIDDEN, DO_DTW_getPathY);
	praat_addAction1 (classDTW, 1, U"-- distance queries --", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get distance value...", nullptr, 1, DO_DTW_getDistanceValue);
	praat_addAction1 (classDTW, 1, U"Get minimum distance", nullptr, 1, DO_DTW_getMinimumDistance);
	praat_addAction1 (classDTW, 1, U"Get maximum distance", nullptr, 1, DO_DTW_getMaximumDistance);
	praat_addAction1 (classDTW, 1, U"Get distance (weighted)", nullptr, 1, DO_DTW_getWeightedDistance);
	praat_addAction1 (classDTW, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 0, U"Formula (distances)...", nullptr, 1, DO_DTW_formulaDistances);
	praat_addAction1 (classDTW, 0, U"Set distance value...", nullptr, 1, DO_DTW_setDistanceValue);

	praat_addAction1 (classDTW, 0, U"Analyse", nullptr, 0, 0);
    praat_addAction1 (classDTW, 0, U"Find path...", nullptr, praat_HIDDEN, DO_DTW_findPath);
    praat_addAction1 (classDTW, 0, U"Find path (band & slope)...", nullptr, 0, DO_DTW_findPath_bandAndSlope);
    praat_addAction1 (classDTW, 0, U"To Polygon...", nullptr, 1, DO_DTW_to_Polygon);
	praat_addAction1 (classDTW, 0, U"To Matrix (distances)", nullptr, 0, DO_DTW_to_Matrix_distances);
    praat_addAction1 (classDTW, 0, U"To Matrix (cumm. distances)...", nullptr, 0, DO_DTW_to_Matrix_cummulativeDistances);
	praat_addAction1 (classDTW, 0, U"Swap axes", nullptr, 0, DO_DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classMatrix, 1, U"Replace matrix", nullptr, 0, DO_DTW_and_Matrix_replace);
	praat_addAction2 (classDTW, 1, classTextGrid, 1, U"To TextGrid (warp times)", nullptr, 0, DO_DTW_and_TextGrid_to_TextGrid);
	praat_addAction2 (classDTW, 1, classIntervalTier, 1, U"To Table (distances)", nullptr, 0, DO_DTW_and_IntervalTier_to_Table);

    praat_addAction2 (classDTW, 1, classPolygon, 1, U"Find path inside...", nullptr, 0, DO_DTW_and_Polygon_findPathInside);
    praat_addAction2 (classDTW, 1, classPolygon, 1, U"To Matrix (cumm. distances)...", nullptr, 0, DO_DTW_and_Polygon_to_Matrix_cummulativeDistances);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw...", nullptr, 0, DO_DTW_and_Sounds_draw);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw warp (x)...", nullptr, 0, DO_DTW_and_Sounds_drawWarpX);

	praat_addAction1 (classEditDistanceTable, 1, U"EditDistanceTable help", nullptr, 0, DO_EditDistanceTable_help);
	praat_EditDistanceTable_as_TableOfReal_init (classEditDistanceTable);
	praat_addAction1 (classEditDistanceTable, 1, U"To TableOfReal (directions)...", nullptr, praat_HIDDEN, DO_EditDistanceTable_to_TableOfReal_directions);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, U"Set new edit costs", nullptr, 0, DO_EditDistanceTable_setEditCosts);

	praat_addAction1 (classEditCostsTable, 1, U"EditCostsTable help", nullptr, 0, DO_EditCostsTable_help);
	praat_addAction1 (classEditCostsTable, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, U"Get target index...", nullptr, 1, DO_EditCostsTable_getTargetIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get source index...", nullptr, 1, DO_EditCostsTable_getSourceIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get insertion cost...", nullptr, 1, DO_EditCostsTable_getInsertionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get deletion cost...", nullptr, 1, DO_EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get substitution cost...", nullptr, 1, DO_EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get cost (others)...", nullptr, 1, DO_EditCostsTable_getOthersCost);
	praat_addAction1 (classEditCostsTable, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, U"Set target symbol (index)...", nullptr, 1, DO_EditCostsTable_setTargetSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set source symbol (index)...", nullptr, 1, DO_EditCostsTable_setSourceSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set insertion costs...", nullptr, 1, DO_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set deletion costs...", nullptr, 1, DO_EditCostsTable_setDeletionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set substitution costs...", nullptr, 1, DO_EditCostsTable_setSubstitutionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set costs (others)...", nullptr, 1, DO_EditCostsTable_setOthersCosts);
	praat_addAction1 (classEditCostsTable, 1, U"To TableOfReal", nullptr, 0, DO_EditCostsTable_to_TableOfReal);

	praat_Index_init (classStringsIndex);
	praat_addAction1 (classIndex, 0, U"Index help", nullptr, 0, DO_Index_help);
	praat_addAction1 (classStringsIndex, 1, U"Get class label...", nullptr, 0, DO_StringsIndex_getClassLabel);
	praat_addAction1 (classStringsIndex, 1, U"Get class index...", nullptr, 0, DO_StringsIndex_getClassIndex);
	praat_addAction1 (classStringsIndex, 1, U"Get label...", nullptr, 0, DO_StringsIndex_getLabel);
	praat_addAction1 (classIndex, 1, U"Get index...", nullptr, 0, DO_Index_getIndex);
	praat_addAction1 (classStringsIndex, 1, U"To Strings", nullptr, 0, DO_StringsIndex_to_Strings);

	praat_addAction1 (classExcitation, 0, U"Synthesize", U"To Formant...", 0, 0);
	praat_addAction1 (classExcitation, 0, U"To ExcitationList", U"Synthesize", 0, DO_Excitations_to_ExcitationList);
	praat_addAction1 (classExcitation, 0, U"To Excitations", U"Synthesize", praat_HIDDEN, DO_Excitations_to_ExcitationList);

	praat_addAction1 (classExcitationList, 0, U"Modify", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"Formula...", nullptr, 0, DO_ExcitationList_formula);
	praat_addAction1 (classExcitationList, 0, U"Extract", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"Extract Excitation...", nullptr, 0, DO_ExcitationList_getItem);
	praat_addAction1 (classExcitationList, 0, U"Synthesize", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 2, U"Append", nullptr, 0, DO_ExcitationList_append);
	praat_addAction1 (classExcitationList, 0, U"Convert", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"To Pattern...", nullptr, 0, DO_ExcitationList_to_Pattern);
	praat_addAction1 (classExcitationList, 0, U"To TableOfReal", nullptr, 0, DO_ExcitationList_to_TableOfReal);

	praat_addAction2 (classExcitationList, 1, classExcitation, 0, U"Add to ExcitationList", nullptr, 0, DO_ExcitationList_addItem);
	praat_addAction2 (classExcitationList, 1, classExcitation, 0, U"Add to Excitations", nullptr, praat_HIDDEN, DO_ExcitationList_addItem);

	praat_addAction1 (classFileInMemory, 1, U"Show as code...", nullptr, 0, DO_FileInMemory_showAsCode);
	praat_addAction1 (classFileInMemory, 1, U"Set id...", nullptr, 0, DO_FileInMemory_setId);
	praat_addAction1 (classFileInMemory, 0, U"To FileInMemorySet", nullptr, 0, DO_FileInMemory_to_FileInMemorySet);
	praat_addAction1 (classFileInMemory, 0, U"To FilesInMemory", nullptr, praat_HIDDEN, DO_FileInMemory_to_FileInMemorySet); // Deprecated since 2015

	praat_addAction1 (classFileInMemorySet, 1, U"Show as code...", nullptr, 0, DO_FileInMemorySet_showAsCode);
	praat_addAction1 (classFileInMemorySet, 1, U"Show one file as code...", nullptr, 0, DO_FileInMemorySet_showOneFileAsCode);
	praat_addAction1 (classFileInMemorySet, 2, U"Merge", nullptr, 0, DO_FileInMemorySets_merge);
	praat_addAction1 (classFileInMemorySet, 0, U"To Strings (id)", nullptr, 0, DO_FileInMemorySet_to_Strings_id);

	praat_addAction2 (classFileInMemorySet, 1, classFileInMemory, 0, U"Add items to Collection", nullptr, 0, DO_FileInMemorySet_addItems);

	praat_addAction1 (classFormantFilter, 0, U"FormantFilter help", nullptr, 0, DO_FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPTH_1 + praat_HIDDEN, DO_FormantFilter_drawSpectrum);
	praat_addAction1 (classFormantFilter, 0, U"Draw filter functions...", U"Draw filters...",  praat_DEPTH_1 + praat_HIDDEN, DO_FormantFilter_drawFilterFunctions);
	praat_addAction1 (classFormantFilter, 0, U"To Spectrogram", nullptr, 0, DO_FormantFilter_to_Spectrogram);

	praat_addAction1 (classFormantGrid, 0, U"Draw...", U"Edit", praat_DEPTH_1 + praat_HIDDEN, DO_FormantGrid_draw);

	praat_addAction1 (classIntensity, 0, U"To TextGrid (silences)...", U"To IntensityTier (valleys)", 0, DO_Intensity_to_TextGrid_detectSilences);
	praat_addAction1 (classIntensityTier, 0, U"To TextGrid (silences)...", nullptr, 0, DO_IntensityTier_to_TextGrid_detectSilences);
	praat_addAction1 (classIntensityTier, 0, U"To Intensity...", nullptr, praat_HIDDEN, DO_IntensityTier_to_Intensity);

	praat_addAction1 (classISpline, 0, U"ISpline help", nullptr, 0, DO_ISpline_help);
	praat_Spline_init (classISpline);

	praat_addAction1 (classKlattTable, 0, U"KlattTable help", nullptr, 0, DO_KlattTable_help);
	praat_addAction1 (classKlattTable, 0, U"To Sound...", nullptr, 0, DO_KlattTable_to_Sound);
	praat_addAction1 (classKlattTable, 0, U"To KlattGrid...", nullptr, 0, DO_KlattTable_to_KlattGrid);
	praat_addAction1 (classKlattTable, 0, U"To Table", nullptr, 0, DO_KlattTable_to_Table);

	praat_addAction1 (classLegendreSeries, 0, U"LegendreSeries help", nullptr, 0, DO_LegendreSeries_help);
	praat_FunctionTerms_init (classLegendreSeries);
	praat_addAction1 (classLegendreSeries, 0, U"To Polynomial", U"Analyse", 0, DO_LegendreSeries_to_Polynomial);

	praat_addAction1 (classLongSound, 0, U"Append to existing sound file...", nullptr, 0, DO_LongSounds_appendToExistingSoundFile);
	praat_addAction1 (classSound, 0, U"Append to existing sound file...", nullptr, 0, DO_LongSounds_appendToExistingSoundFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Append to existing sound file...", nullptr, 0, DO_LongSounds_appendToExistingSoundFile);

	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFF file...", U"Save as NIST file...", 1, DO_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFF file...", U"Write to NIST file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFC file...", U"Save as stereo AIFF file...", 1, DO_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFC file...", U"Write to stereo AIFF file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo WAV file...", U"Save as stereo AIFC file...", 1, DO_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo WAV file...", U"Write to stereo AIFC file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NeXt/Sun file...", U"Save as stereo WAV file...", 1, DO_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NeXt/Sun file...", U"Write to stereo WAV file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NIST file...", U"Save as stereo NeXt/Sun file...", 1, DO_LongSounds_writeToStereoNistFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NIST file...", U"Write to stereo NeXt/Sun file...", praat_HIDDEN + praat_DEPTH_1, DO_LongSounds_writeToStereoNistFile);

	praat_addAction1 (classLtas, 0, U"Report spectral tilt...", U"Get slope...", 1, DO_Ltas_reportSpectralTilt);

	praat_addAction1 (classMatrix, 0, U"Scatter plot...", U"Paint cells...", 1, DO_Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, U"Draw as squares...", U"Scatter plot...", 1, DO_Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, U"Draw distribution...", U"Draw as squares...", 1, DO_Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, U"Draw cumulative distribution...", U"Draw distribution...", 1, DO_Matrix_drawCumulativeDistribution);
	praat_addAction1 (classMatrix, 0, U"Get mean...", U"Get sum", 1, DO_Matrix_getMean);
	praat_addAction1 (classMatrix, 0, U"Get standard deviation...", U"Get mean...", 1, DO_Matrix_getStandardDeviation);
	praat_addAction1 (classMatrix, 0, U"Transpose", U"Synthesize", 0, DO_Matrix_transpose);
	praat_addAction1 (classMatrix, 0, U"Solve equation...", U"Analyse", 0, DO_Matrix_solveEquation);
	praat_addAction1 (classMatrix, 0, U"To PCA (by rows)", U"Solve equation...", 0, DO_Matrix_to_PCA_byRows);
	praat_addAction1 (classMatrix, 0, U"To PCA (by columns)", U"To PCA (by rows)", 0, DO_Matrix_to_PCA_byColumns);
	praat_addAction1 (classMatrix, 0, U"To Pattern...", U"To VocalTract", 1, DO_Matrix_to_Pattern);
	praat_addAction1 (classMatrix, 0, U"To Activation", U"To Pattern...", 1, DO_Matrix_to_Activation);
	praat_addAction1 (classMatrix, 2, U"To DTW...", U"To ParamCurve", 1, DO_Matrices_to_DTW);

	praat_addAction2 (classMatrix, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, DO_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classMelSpectrogram, 0, U"MelSpectrogram help", nullptr, 0, DO_MelSpectrogram_help);
	praat_BandFilterSpectrogram_draw_init (classMelSpectrogram);
	praat_addAction1 (classMelSpectrogram, 0, U"Paint image...", nullptr, 1, DO_MelSpectrogram_paintImage);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw triangular filter functions...", nullptr, 1, DO_MelSpectrogram_drawTriangularFilterFunctions);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw spectrum at nearest time slice...", nullptr, 1, DO_MelSpectrogram_drawSpectrumAtNearestTimeSlice);
	praat_addAction1 (classMelSpectrogram, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_BandFilterSpectrogram_query_init (classMelSpectrogram);

	praat_addAction1 (classMelSpectrogram, 0, U"Equalize intensities...", nullptr, 0, DO_BandFilterSpectrogram_equalizeIntensities);
	praat_addAction1 (classMelSpectrogram, 0, U"To MFCC...", nullptr, 0, DO_MelSpectrogram_to_MFCC);
	praat_addAction1 (classMelSpectrogram, 0, U"To Intensity", nullptr, 0, DO_BandFilterSpectrogram_to_Intensity);
	praat_addAction1 (classMelSpectrogram, 0, U"To Matrix...", nullptr, 0, DO_BandFilterSpectrogram_to_Matrix);
	praat_addAction1 (classMelSpectrogram, 2, U"Cross-correlate...", nullptr, 0, DO_BandFilterSpectrograms_crossCorrelate);
	praat_addAction1 (classMelSpectrogram, 2, U"Convolve...", nullptr, 0, DO_BandFilterSpectrograms_convolve);
	
	praat_addAction1 (classMelFilter, 0, U"MelFilter help", nullptr, 0, DO_MelFilter_help); // deprecated 2014
	praat_FilterBank_all_init (classMelFilter); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPTH_1 + praat_HIDDEN, DO_MelFilter_drawSpectrum); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"Draw filter functions...", U"Draw filters...", praat_DEPTH_1 + praat_HIDDEN, DO_MelFilter_drawFilterFunctions); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"Paint...", U"Draw filter functions...", praat_DEPTH_1 + praat_HIDDEN, DO_MelFilter_paint); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"To MFCC...", nullptr, praat_HIDDEN, DO_MelFilter_to_MFCC); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"To MelSpectrogram", nullptr, 0, DO_MelFilter_to_MelSpectrogram);
	
	praat_addAction1 (classMFCC, 0, U"MFCC help", nullptr, 0, DO_MFCC_help);
	praat_CC_init (classMFCC);
	praat_addAction1 (classMFCC, 0, U"To MelFilter...", nullptr, praat_HIDDEN, DO_MFCC_to_MelFilter);
	praat_addAction1 (classMFCC, 0, U"To MelSpectrogram...", nullptr, 0, DO_MFCC_to_MelSpectrogram);
	praat_addAction1 (classMFCC, 0, U"To TableOfReal...", nullptr, 0, DO_MFCC_to_TableOfReal);
	praat_addAction1 (classMFCC, 0, U"To Matrix (features)...", nullptr, praat_HIDDEN, DO_MFCC_to_Matrix_features);
	praat_addAction1 (classMFCC, 0, U"To Sound", nullptr, praat_HIDDEN, DO_MFCC_to_Sound);
	praat_addAction1 (classMFCC, 2, U"Cross-correlate...", nullptr, 0, DO_MFCCs_crossCorrelate);
	praat_addAction1 (classMFCC, 2, U"Convolve...", nullptr, 0, DO_MFCCs_convolve);

	praat_addAction1 (classMSpline, 0, U"MSpline help", nullptr, 0, DO_MSpline_help);
	praat_Spline_init (classMSpline);

	praat_addAction1 (classPattern, 0, U"Draw", nullptr, 0, 0);
	praat_addAction1 (classPattern, 0, U"Draw...", nullptr, 0, DO_Pattern_draw);
	praat_addAction1 (classPattern, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classPattern, 0, U"Formula...", nullptr, 1, DO_Pattern_formula);
	praat_addAction1 (classPattern, 0, U"Set value...", nullptr, 1, DO_Pattern_setValue);
	praat_addAction1 (classPattern, 0, U"To Matrix", nullptr, 0, DO_Pattern_to_Matrix);

	praat_addAction2 (classPattern, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, DO_Matrix_Categories_to_TableOfReal);
	praat_addAction2 (classPattern, 1, classCategories, 1, U"To Discriminant", nullptr, 0, DO_Pattern_and_Categories_to_Discriminant);

	praat_addAction1 (classPCA, 0, U"PCA help", nullptr, 0, DO_PCA_help);
	praat_addAction1 (classPCA, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_Eigen_draw_init (classPCA);
	praat_addAction1 (classPCA, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_Eigen_query_init (classPCA);
	praat_addAction1 (classPCA, 1, U"-- pca --", nullptr, 1, 0);
	praat_addAction1 (classPCA, 1, U"Get centroid element...", nullptr, 1, DO_PCA_getCentroidElement);
	praat_addAction1 (classPCA, 1, U"Get equality of eigenvalues...", nullptr, 1, DO_PCA_getEqualityOfEigenvalues);
	praat_addAction1 (classPCA, 1, U"Get fraction variance accounted for...", nullptr, 1, DO_PCA_getFractionVAF);
	praat_addAction1 (classPCA, 1, U"Get number of components (VAF)...", nullptr, 1, DO_PCA_getNumberOfComponentsVAF);
	praat_addAction1 (classPCA, 2, U"Get angle between pc1-pc2 planes", nullptr, 1, DO_PCAs_getAngleBetweenPc1Pc2Plane_degrees);
	praat_addAction1 (classPCA, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classPCA, 1, U"Invert eigenvector...", nullptr, 1, DO_PCA_invertEigenvector);
	praat_addAction1 (classPCA, 0, U"Align eigenvectors", nullptr, 1, DO_Eigens_alignEigenvectors);
	praat_addAction1 (classPCA, 2, U"To Procrustes...", nullptr, 0, DO_PCAs_to_Procrustes);
	praat_addAction1 (classPCA, 0, U"To TableOfReal (reconstruct 1)...", nullptr, 0, DO_PCA_to_TableOfReal_reconstruct1);
	praat_addAction1 (classPCA, 0, U"& TableOfReal: To Configuration?", nullptr, 0, DO_hint_PCA_and_TableOfReal_to_Configuration);
	praat_addAction1 (classPCA, 0, U"& Configuration (reconstruct)?", nullptr, 0, DO_hint_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction1 (classPCA, 0, U"& Covariance: Project?", nullptr, 0, DO_hint_PCA_and_Covariance_Project);
	praat_addAction2 (classPCA, 1, classConfiguration, 1, U"To TableOfReal (reconstruct)", nullptr, 0, DO_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (pc)...", nullptr, praat_HIDDEN, DO_PCA_and_Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project rows)...", nullptr, 0, DO_PCA_and_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project columns)...", nullptr, 0, DO_PCA_and_Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classPattern, 1, U"To Matrix (project rows)...", nullptr, 0, DO_PCA_and_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classSSCP, 1, U"Project", nullptr, 0, DO_Eigen_and_SSCP_project);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal...", nullptr, 0, DO_PCA_and_TableOfReal_to_TableOfReal_projectRows);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (project rows)...", nullptr, 0, DO_PCA_and_TableOfReal_to_TableOfReal_projectRows);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0, DO_PCA_and_TableOfReal_to_Configuration);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (z-scores)...", nullptr, 0, DO_PCA_and_TableOfReal_to_TableOfReal_zscores);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"Get fraction variance...", nullptr, 0, DO_PCA_and_TableOfReal_getFractionVariance);
	praat_addAction2 (classPCA, 1, classCovariance, 1, U"Project", nullptr, 0, DO_Eigen_and_Covariance_project);

	praat_Eigen_Spectrogram_project (classPCA, classSpectrogram);
	praat_Eigen_Spectrogram_project (classPCA, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classPCA, classMelSpectrogram);

	praat_Eigen_Matrix_project (classPCA, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classPCA, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classPCA, classMelFilter); // deprecated 2014

	praat_addAction1 (classPermutation, 0, U"Permutation help", nullptr, 0, DO_Permutation_help);
	praat_addAction1 (classPermutation, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, U"Get number of elements", nullptr, 1, DO_Permutation_getNumberOfElements);
	praat_addAction1 (classPermutation, 1, U"Get value...", nullptr, 1, DO_Permutation_getValueAtIndex);
	praat_addAction1 (classPermutation, 1, U"Get index...", nullptr, 1, DO_Permutation_getIndexAtValue);
	praat_addAction1 (classPermutation, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, U"Sort", nullptr, 1, DO_Permutation_sort);
	praat_addAction1 (classPermutation, 1, U"Swap blocks...", nullptr, 1, DO_Permutation_swapBlocks);
	praat_addAction1 (classPermutation, 1, U"Swap numbers...", nullptr, 1, DO_Permutation_swapNumbers);
	praat_addAction1 (classPermutation, 1, U"Swap positions...", nullptr, 1, DO_Permutation_swapPositions);
	praat_addAction1 (classPermutation, 1, U"Swap one from range...", nullptr, 1, DO_Permutation_swapOneFromRange);
	praat_addAction1 (classPermutation, 0, U"-- sequential permutations --", nullptr, 1, 0);
	praat_addAction1 (classPermutation, 0, U"Next", nullptr, 1, DO_Permutations_next);
	praat_addAction1 (classPermutation, 0, U"Previous", nullptr, 1, DO_Permutations_previous);
	praat_addAction1 (classPermutation, 1, U"Permute randomly...", nullptr, 0, DO_Permutation_permuteRandomly);
	praat_addAction1 (classPermutation, 1, U"Permute randomly (blocks)...", nullptr, 0, DO_Permutation_permuteBlocksRandomly);
	praat_addAction1 (classPermutation, 1, U"Interleave...", nullptr, 0, DO_Permutation_interleave);
	praat_addAction1 (classPermutation, 1, U"Rotate...", nullptr, 0, DO_Permutation_rotate);
	praat_addAction1 (classPermutation, 1, U"Reverse...", nullptr, 0, DO_Permutation_reverse);
	praat_addAction1 (classPermutation, 1, U"Invert", nullptr, 0, DO_Permutation_invert);
	praat_addAction1 (classPermutation, 0, U"Multiply", nullptr, 0, DO_Permutations_multiply);

	praat_addAction1 (classPitch, 2, U"To DTW...", U"To PointProcess", praat_HIDDEN, DO_Pitches_to_DTW);

	praat_addAction1 (classPitchTier, 0, U"To Pitch...", U"To Sound (sine)...", 1, DO_PitchTier_to_Pitch);
	praat_addAction1 (classPolygon, 0, QUERY_BUTTON, U"Paint circles...", 0, 0);
	praat_addAction1 (classPolygon, 0, U"Get number of points", QUERY_BUTTON, 1, DO_Polygon_getNumberOfPoints);
	praat_addAction1 (classPolygon, 0, U"Get point (x)...", U"Get number of points", 1, DO_Polygon_getPointX);
	praat_addAction1 (classPolygon, 0, U"Get point (y)...",  U"Get point (x)...", 1, DO_Polygon_getPointY);
	praat_addAction1 (classPolygon, 0, U"-- other queries --",  U"Get point (y)...", 1, 0);
	praat_addAction1 (classPolygon, 0, U"Get location of point...", U"-- other queries --", 1, DO_Polygon_getLocationOfPoint);
	praat_addAction1 (classPolygon, 0, U"Get area of convex hull...", U"Get location of point...", praat_DEPTH_1 + praat_HIDDEN, DO_Polygon_getAreaOfConvexHull);
	
	praat_addAction1 (classPolygon, 0, U"Translate...", MODIFY_BUTTON, 1, DO_Polygon_translate);
	praat_addAction1 (classPolygon, 0, U"Rotate...", U"Translate...", 1, DO_Polygon_rotate);
	praat_addAction1 (classPolygon, 0, U"Scale...", U"Rotate...", 1, DO_Polygon_scale);
	praat_addAction1 (classPolygon, 0, U"Reverse X", U"Scale...", 1, DO_Polygon_reverseX);
	praat_addAction1 (classPolygon, 0, U"Reverse Y", U"Reverse X", 1, DO_Polygon_reverseY);
	praat_addAction1 (classPolygon, 0, U"Simplify", nullptr, praat_HIDDEN, DO_Polygon_simplify);
	praat_addAction1 (classPolygon, 0, U"Convex hull", nullptr, 0, DO_Polygon_convexHull);
	praat_addAction1 (classPolygon, 0, U"Circular permutation...", nullptr, praat_HIDDEN, DO_Polygon_circularPermutation);

	praat_addAction2 (classPolygon, 1, classCategories, 1, U"Draw...", nullptr, 0, DO_Polygon_Categories_draw);

	praat_addAction1 (classPolynomial, 0, U"Polynomial help", nullptr, 0, DO_Polynomial_help);
	praat_FunctionTerms_init (classPolynomial);
	praat_addAction1 (classPolynomial, 0, U"-- area --", U"Get x of maximum...", 1, 0);
	praat_addAction1 (classPolynomial, 1, U"Get area...", U"-- area --", 1, DO_Polynomial_getArea);
	praat_addAction1 (classPolynomial, 0, U"-- monic --", U"Set coefficient...", 1, 0);
	praat_addAction1 (classPolynomial, 0, U"Scale coefficients (monic)", U"-- monic --", 1, DO_Polynomial_scaleCoefficients_monic);
	praat_addAction1 (classPolynomial, 1, U"Get value (complex)...", U"Get value...", 1, DO_Polynomial_evaluate_z);
	praat_addAction1 (classPolynomial, 0, U"To Spectrum...", U"Analyse", 0, DO_Polynomial_to_Spectrum);
	praat_addAction1 (classPolynomial, 0, U"To Roots", nullptr, 0, DO_Polynomial_to_Roots);
	praat_addAction1 (classPolynomial, 0, U"To Polynomial (derivative)", nullptr, 0, DO_Polynomial_getDerivative);
	praat_addAction1 (classPolynomial, 0, U"To Polynomial (primitive)", nullptr, 0, DO_Polynomial_getPrimitive);
	praat_addAction1 (classPolynomial, 0, U"Scale x...", nullptr, 0, DO_Polynomial_scaleX);
	praat_addAction1 (classPolynomial, 2, U"Multiply", nullptr, 0, DO_Polynomials_multiply);
	praat_addAction1 (classPolynomial, 2, U"Divide...", nullptr, 0, DO_Polynomials_divide);

	praat_addAction1 (classRoots, 1, U"Roots help", nullptr, 0, DO_Roots_help);
	praat_addAction1 (classRoots, 1, U"Draw...", nullptr, 0, DO_Roots_draw);
	praat_addAction1 (classRoots, 1, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classRoots, 1, U"Get number of roots", nullptr, 1, DO_Roots_getNumberOfRoots);
	praat_addAction1 (classRoots, 1, U"-- roots --", nullptr, 1, 0);
	praat_addAction1 (classRoots, 1, U"Get root...", nullptr, 1, DO_Roots_getRoot);
	praat_addAction1 (classRoots, 1, U"Get real part of root...", nullptr, 1, DO_Roots_getRealPartOfRoot);
	praat_addAction1 (classRoots, 1, U"Get imaginary part of root...", nullptr, 1, DO_Roots_getImaginaryPartOfRoot);
	praat_addAction1 (classRoots, 1, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classRoots, 1, U"Set root...", nullptr, 1, DO_Roots_setRoot);
	praat_addAction1 (classRoots, 0, U"Analyse", nullptr, 0, 0);
	praat_addAction1 (classRoots, 0, U"To Spectrum...", nullptr, 0, DO_Roots_to_Spectrum);

	praat_addAction2 (classRoots, 1, classPolynomial, 1, U"Polish roots", nullptr, 0, DO_Roots_and_Polynomial_polish);

	praat_addAction1 (classSound, 0, U"To TextGrid (silences)...", U"To IntervalTier", 1, DO_Sound_to_TextGrid_detectSilences);
    praat_addAction1 (classSound, 0, U"Play one channel...", U"Play", praat_HIDDEN, DO_Sound_playOneChannel);
    praat_addAction1 (classSound, 0, U"Play as frequency shifted...", U"Play", praat_HIDDEN, DO_Sound_playAsFrequencyShifted);
	praat_addAction1 (classSound, 0, U"Draw where...", U"Draw...", 1, DO_Sound_drawWhere);
	//	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_paintWhere);
	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", 1, DO_Sound_paintWhere);
	//	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sounds_paintEnclosed);
	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", 1, DO_Sounds_paintEnclosed);

	praat_addAction1 (classSound, 0, U"To Pitch (shs)...", U"To Pitch (cc)...", 1, DO_Sound_to_Pitch_shs);
	praat_addAction1 (classSound, 0, U"Fade in...", U"Multiply by window...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_fadeIn);
	praat_addAction1 (classSound, 0, U"Fade out...", U"Fade in...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_fadeOut);
	praat_addAction1 (classSound, 0, U"To Pitch (SPINET)...", U"To Pitch (cc)...", 1, DO_Sound_to_Pitch_SPINET);

	praat_addAction1 (classSound, 0, U"To FormantFilter...", U"To Cochleagram (edb)...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_FormantFilter);
	praat_addAction1 (classSound, 0, U"To Spectrogram (pitch-dependent)...", U"To Cochleagram (edb)...", 1, DO_Sound_to_Spectrogram_pitchDependent);

	praat_addAction1 (classSound, 0, U"To BarkFilter...", U"To FormantFilter...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_BarkFilter); // deprecated 2014
	praat_addAction1 (classSound, 0, U"To BarkSpectrogram...", U"To FormantFilter...", praat_DEPTH_1, DO_Sound_to_BarkSpectrogram);

	praat_addAction1 (classSound, 0, U"To MelFilter...", U"To BarkFilter...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_MelFilter); // deprecated 2014
	praat_addAction1 (classSound, 0, U"To MelSpectrogram...", U"To BarkSpectrogram...", praat_DEPTH_1, DO_Sound_to_MelSpectrogram);
	praat_addAction1 (classSound, 0, U"To ComplexSpectrogram...", U"To MelSpectrogram...", praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_ComplexSpectrogram);

	praat_addAction1 (classSound, 0, U"To Polygon...", U"Down to Matrix", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_to_Polygon);
    praat_addAction1 (classSound, 2, U"To Polygon (enclosed)...", U"Cross-correlate...", praat_DEPTH_1 | praat_HIDDEN, DO_Sounds_to_Polygon_enclosed);
    praat_addAction1 (classSound, 2, U"To DTW...", U"Cross-correlate...", praat_DEPTH_1, DO_Sounds_to_DTW);

	praat_addAction1 (classSound, 1, U"Filter (gammatone)...", U"Filter (de-emphasis)...", 1, DO_Sound_filterByGammaToneFilter4);
	praat_addAction1 (classSound, 0, U"Remove noise...", U"Filter (formula)...", 1, DO_Sound_removeNoise);

	praat_addAction1 (classSound, 0, U"Change gender...", U"Deepen band modulation...", 1, DO_Sound_changeGender);

	praat_addAction1 (classSound, 0, U"Change speaker...", U"Deepen band modulation...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_changeSpeaker);
	praat_addAction1 (classSound, 0, U"Copy channel ranges...", U"Extract all channels", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_copyChannelRanges);
	praat_addAction1 (classSound, 0, U"Trim silences...", U"Resample...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_trimSilences);
	praat_addAction1 (classSound, 0, U"To KlattGrid (simple)...", U"To Manipulation...", 1, DO_Sound_to_KlattGrid_simple);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To FormantFilter...", nullptr, praat_HIDDEN, DO_Sound_and_Pitch_to_FormantFilter);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To Spectrogram (pitch-dependent)...", nullptr, 0, DO_Sound_and_Pitch_to_Spectrogram);

	praat_addAction2 (classSound, 1, classPitch, 1, U"Change gender...", nullptr, 0, DO_Sound_and_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, U"Change speaker...", nullptr, praat_HIDDEN, DO_Sound_and_Pitch_changeSpeaker);
	praat_addAction2 (classSound, 1, classIntervalTier, 1, U"Cut parts matching label...", nullptr, 0, DO_Sound_and_IntervalTier_cutPartsMatchingLabel);
	praat_addAction1 (classSpectrogram, 2, U"To DTW...", U"To Spectrum (slice)...", 1, DO_Spectrograms_to_DTW);
	praat_addAction1 (classSpectrum, 0, U"Draw phases...", U"Draw (log freq)...", praat_DEPTH_1 | praat_HIDDEN, DO_Spectrum_drawPhases);
	praat_addAction1 (classSpectrum, 0, U"Set real value in bin...", U"Formula...", praat_HIDDEN | praat_DEPTH_1, DO_Spectrum_setRealValueInBin);
	praat_addAction1 (classSpectrum, 0, U"Set imaginary value in bin...", U"Formula...", praat_HIDDEN | praat_DEPTH_1, DO_Spectrum_setImaginaryValueInBin);
	praat_addAction1 (classSpectrum, 0, U"Conjugate", U"Formula...", praat_HIDDEN | praat_DEPTH_1, DO_Spectrum_conjugate);
	praat_addAction1 (classSpectrum, 2, U"Multiply", U"To Sound (fft)", praat_HIDDEN, DO_Spectra_multiply);
	praat_addAction1 (classSpectrum, 0, U"To Matrix (unwrap)", U"To Matrix", praat_HIDDEN, DO_Spectrum_unwrap);
	praat_addAction1 (classSpectrum, 0, U"Shift frequencies...", U"To Matrix", praat_HIDDEN, DO_Spectrum_shiftFrequencies);
	praat_addAction1 (classSpectrum, 0, U"Compress frequency domain...", U"Shift frequencies...", praat_HIDDEN, DO_Spectrum_compressFrequencyDomain);
	praat_addAction1 (classSpectrum, 0, U"Resample...", U"Compress frequency domain...", praat_HIDDEN, DO_Spectrum_resample);
	praat_addAction1 (classSpectrum, 0, U"To Cepstrum", U"To Spectrogram", 1, DO_Spectrum_to_Cepstrum);
	praat_addAction1 (classSpectrum, 0, U"To PowerCepstrum", U"To Cepstrum", 1, DO_Spectrum_to_PowerCepstrum);

	praat_addAction1 (classSpeechSynthesizer, 0, U"SpeechSynthesizer help", nullptr, 0, DO_SpeechSynthesizer_help);
	praat_addAction1 (classSpeechSynthesizer, 0, U"Play text...", nullptr, 0, DO_SpeechSynthesizer_playText);
	praat_addAction1 (classSpeechSynthesizer, 0, U"To Sound...", nullptr, 0, DO_SpeechSynthesizer_to_Sound);
	praat_addAction1 (classSpeechSynthesizer, 0, QUERY_BUTTON, nullptr, 0, 0);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice name", nullptr, 1, DO_SpeechSynthesizer_getVoiceName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice variant", nullptr, 1, DO_SpeechSynthesizer_getVoiceVariant);
	praat_addAction1 (classSpeechSynthesizer, 0, MODIFY_BUTTON, nullptr, 0, 0);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set text input settings...", nullptr, 1, DO_SpeechSynthesizer_setTextInputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set speech output settings...", nullptr, 1, DO_SpeechSynthesizer_setSpeechOutputSettings);
	praat_addAction2 (classSpeechSynthesizer, 1, classTextGrid, 1, U"To Sound...", nullptr, 0, DO_SpeechSynthesizer_and_TextGrid_to_Sound);

	praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align)...", nullptr, 0, DO_SpeechSynthesizer_and_Sound_and_TextGrid_align);
    praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align,trim)...", nullptr, 0, DO_SpeechSynthesizer_and_Sound_and_TextGrid_align2);

	praat_addAction1 (classSSCP, 0, U"SSCP help", nullptr, 0, DO_SSCP_help);
	praat_TableOfReal_init2 (classSSCP);
	praat_removeAction (classSSCP, nullptr, nullptr, U"Append");
	praat_addAction1 (classSSCP, 0, U"Draw sigma ellipse...", DRAW_BUTTON, 1, DO_SSCP_drawSigmaEllipse);
	praat_addAction1 (classSSCP, 0, U"Draw confidence ellipse...", DRAW_BUTTON, 1, DO_SSCP_drawConfidenceEllipse);
	praat_SSCP_query_init (classSSCP);
	praat_addAction1 (classSSCP, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, DO_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classSSCP, 1, U"Get total variance", U"Get diagonality (bartlett)...", 1, DO_SSCP_getTotalVariance);
	praat_addAction1 (classSSCP, 1, U"Get sigma ellipse area...", U"Get total variance", 1, DO_SSCP_getConcentrationEllipseArea);
	praat_addAction1 (classSSCP, 1, U"Get confidence ellipse area...", U"Get sigma ellipse area...", 1, DO_SSCP_getConfidenceEllipseArea);
	praat_addAction1 (classSSCP, 1, U"Get fraction variation...", U"Get confidence ellipse area...", 1, DO_SSCP_getFractionVariation);
	praat_SSCP_extract_init (classSSCP);
	praat_addAction1 (classSSCP, 0, U"To PCA", nullptr, 0, DO_SSCP_to_PCA);
	praat_addAction1 (classSSCP, 0, U"To Correlation", nullptr, 0, DO_SSCP_to_Correlation);
	praat_addAction1 (classSSCP, 0, U"To Covariance...", nullptr, 0, DO_SSCP_to_Covariance);

	praat_addAction1 (classStrings, 0, U"To Categories", nullptr, 0, DO_Strings_to_Categories);
	praat_addAction1 (classStrings, 0, U"Append", nullptr, 0, DO_Strings_append);
	praat_addAction1 (classStrings, 0, U"Change...", U"Replace all...", praat_HIDDEN, DO_Strings_change);
	praat_addAction1 (classStrings, 0, U"Extract part...", U"Replace all...", 0, DO_Strings_extractPart);
	praat_addAction1 (classStrings, 0, U"To Permutation...", U"To Distributions", 0, DO_Strings_to_Permutation);
	praat_addAction1 (classStrings, 2, U"To EditDistanceTable", U"To Distributions", 0, DO_Strings_to_EditDistanceTable);

	praat_addAction1 (classSVD, 0, U"To TableOfReal...", nullptr, 0, DO_SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, U"Extract left singular vectors", nullptr, 0, DO_SVD_extractLeftSingularVectors);
	praat_addAction1 (classSVD, 0, U"Extract right singular vectors", nullptr, 0, DO_SVD_extractRightSingularVectors);
	praat_addAction1 (classSVD, 0, U"Extract singular values", nullptr, 0, DO_SVD_extractSingularValues);
		praat_addAction1 (classTable, 0, U"Draw ellipses...", U"Draw ellipse (standard deviation)...", praat_DEPTH_1, DO_Table_drawEllipses);
		praat_addAction1 (classTable, 0, U"Box plots...", U"Draw ellipses...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_boxPlots);
		praat_addAction1 (classTable, 0, U"Normal probability plot...", U"Box plots...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_normalProbabilityPlot);
		praat_addAction1 (classTable, 0, U"Quantile-quantile plot...", U"Normal probability plot...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_quantileQuantilePlot);
		praat_addAction1 (classTable, 0, U"Quantile-quantile plot (between levels)...", U"Quantile-quantile plot...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_quantileQuantilePlot_betweenLevels);
		praat_addAction1 (classTable, 0, U"Lag plot...", U"Quantile-quantile plot (between levels)...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_lagPlot);
		praat_addAction1 (classTable, 0, U"Horizontal error bars plot...", U"Scatter plot (mark)...", praat_DEPTH_1, DO_Table_horizontalErrorBarsPlot);
		praat_addAction1 (classTable, 0, U"Vertical error bars plot...", U"Scatter plot (mark)...", praat_DEPTH_1, DO_Table_verticalErrorBarsPlot);
		praat_addAction1 (classTable, 0, U"Distribution plot...", U"Quantile-quantile plot...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_distributionPlot);
		praat_addAction1 (classTable, 1, U"Draw where",  U"Lag plot...", 1 , 0);
			praat_addAction1 (classTable, 0, U"Scatter plot where...", U"Draw where", 2, DO_Table_scatterPlotWhere);
			praat_addAction1 (classTable, 0, U"Scatter plot where (mark)...", U"Scatter plot where...", 2, DO_Table_scatterPlotMarkWhere);
			praat_addAction1 (classTable, 0, U"Horizontal error bars plot where...", U"Scatter plot where (mark)...", praat_DEPTH_2, DO_Table_horizontalErrorBarsPlotWhere);
			praat_addAction1 (classTable, 0, U"Vertical error bars plot where...", U"Scatter plot where (mark)...", praat_DEPTH_2, DO_Table_verticalErrorBarsPlotWhere);
			
			praat_addAction1 (classTable, 0, U"Distribution plot where...", U"Scatter plot where (mark)...", 2, DO_Table_distributionPlotWhere);
			praat_addAction1 (classTable, 0, U"Draw ellipse where (standard deviation)...", U"Distribution plot where...", 2, DO_Table_drawEllipseWhere);
			praat_addAction1 (classTable, 0, U"Box plots where...", U"Draw ellipse where (standard deviation)...", 2, DO_Table_boxPlotsWhere);
			praat_addAction1 (classTable, 0, U"Normal probability plot where...", U"Box plots where...", 2, DO_Table_normalProbabilityPlotWhere);
			praat_addAction1 (classTable, 0, U"Bar plot where...", U"Normal probability plot where...", 2, DO_Table_barPlotWhere);
			praat_addAction1 (classTable, 0, U"Line graph where...", U"Bar plot where...", 2, DO_Table_LineGraphWhere);
			praat_addAction1 (classTable, 0, U"Lag plot where...", U"Line graph where...", 2, DO_Table_lagPlotWhere);
			praat_addAction1 (classTable, 0, U"Draw ellipses where...", U"Lag plot where...", 2, DO_Table_drawEllipsesWhere);

	praat_addAction1 (classTable, 1, U"Get number of rows where...", U"Get number of rows", praat_DEPTH_1 | praat_HIDDEN,	DO_Table_getNumberOfRowsWhere);
	praat_addAction1 (classTable, 1, U"Report one-way anova...", U"Report group difference (Wilcoxon rank sum)...", praat_DEPTH_1 | praat_HIDDEN,	DO_Table_reportOneWayAnova);
	praat_addAction1 (classTable, 1, U"Report one-way Kruskal-Wallis...", U"Report one-way anova...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_reportOneWayKruskalWallis);
	praat_addAction1 (classTable, 1, U"Report two-way anova...", U"Report one-way Kruskal-Wallis...", praat_DEPTH_1 | praat_HIDDEN, DO_Table_reportTwoWayAnova);
	praat_addAction1 (classTable, 0, U"Extract rows where...", U"Extract rows where column (text)...", praat_DEPTH_1, DO_Table_extractRowsWhere);
	praat_addAction1 (classTable, 0, U"Extract rows where (mahalanobis)...", U"Extract rows where...", praat_DEPTH_1| praat_HIDDEN, DO_Table_extractRowsMahalanobisWhere);
	praat_addAction1 (classTable, 0, U"-- Extract columns ----", U"Extract rows where (mahalanobis)...", praat_DEPTH_1| praat_HIDDEN, 0);
	praat_addAction1 (classTable, 0, U"Extract column ranges...", U"-- Extract columns ----", praat_DEPTH_1| praat_HIDDEN, DO_Table_extractColumnRanges);

	praat_addAction1 (classTable, 0, U"To KlattTable", nullptr, praat_HIDDEN, DO_Table_to_KlattTable);
	praat_addAction1 (classTable, 1, U"Get median absolute deviation...", U"Get standard deviation...", 1, DO_Table_getMedianAbsoluteDeviation);
	praat_addAction1 (classTable, 0, U"To StringsIndex (column)...", nullptr, praat_HIDDEN, DO_Table_to_StringsIndex_column);

	praat_addAction1 (classTableOfReal, 1, U"Report multivariate normality...", U"Get column stdev (label)...",
		praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_reportMultivariateNormality);
	praat_addAction1 (classTableOfReal, 0, U"Append columns", U"Append", 1, DO_TableOfReal_appendColumns);
	praat_addAction1 (classTableOfReal, 0, U"Multivariate statistics -", nullptr, 0, 0);
	praat_addAction1 (classTableOfReal, 0, U"To Discriminant", nullptr, 1, DO_TableOfReal_to_Discriminant);
	praat_addAction1 (classTableOfReal, 0, U"To PCA", nullptr, 1, DO_TableOfReal_to_PCA_byRows);
	praat_addAction1 (classTableOfReal, 0, U"To SSCP...", nullptr, 1, DO_TableOfReal_to_SSCP);
	praat_addAction1 (classTableOfReal, 0, U"To Covariance", nullptr, 1, DO_TableOfReal_to_Covariance);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation", nullptr, 1, DO_TableOfReal_to_Correlation);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation (rank)", nullptr, 1, DO_TableOfReal_to_Correlation_rank);
	praat_addAction1 (classTableOfReal, 0, U"To CCA...", nullptr, 1, DO_TableOfReal_to_CCA);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (means by row labels)...", nullptr, 1, DO_TableOfReal_meansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (medians by row labels)...", nullptr, 1, DO_TableOfReal_mediansByRowLabels);

	praat_addAction1 (classTableOfReal, 0, U"-- configurations --", nullptr, 1, 0);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (pca)...", nullptr, 1, DO_TableOfReal_to_Configuration_pca);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (lda)...", nullptr, 1, DO_TableOfReal_to_Configuration_lda);
	praat_addAction1 (classTableOfReal, 2, U"-- between tables --", U"To Configuration (lda)...", 1, 0);
	praat_addAction1 (classTableOfReal, 2, U"To TableOfReal (cross-correlations)...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_TableOfReal_and_TableOfReal_crossCorrelations);

	praat_addAction1 (classTableOfReal, 1, U"To Pattern and Categories...", U"To Matrix", 1, DO_TableOfReal_to_Pattern_and_Categories);
	praat_addAction1 (classTableOfReal, 1, U"Split into Pattern and Categories...", U"To Pattern and Categories...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_to_Pattern_and_Categories);
	praat_addAction1 (classTableOfReal, 0, U"To Permutation (sort row labels)", U"To Matrix", 1, DO_TableOfReal_to_Permutation_sortRowlabels);

	praat_addAction1 (classTableOfReal, 1, U"To SVD", nullptr, praat_HIDDEN, DO_TableOfReal_to_SVD);
	praat_addAction1 (classTableOfReal, 2, U"To GSVD", nullptr, praat_HIDDEN, DO_TablesOfReal_to_GSVD);
	praat_addAction1 (classTableOfReal, 2, U"To Eigen (gsvd)", nullptr, praat_HIDDEN, DO_TablesOfReal_to_Eigen_gsvd);

	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (cholesky)...", nullptr, praat_HIDDEN, DO_TableOfReal_choleskyDecomposition);

	praat_addAction1 (classTableOfReal, 0, U"-- scatter plots --", U"Draw top and bottom lines...", 1, 0);
	praat_addAction1 (classTableOfReal, 0, U"Draw scatter plot...", U"-- scatter plots --", 1, DO_TableOfReal_drawScatterPlot);
	praat_addAction1 (classTableOfReal, 0, U"Draw scatter plot matrix...", U"Draw scatter plot...", 1, DO_TableOfReal_drawScatterPlotMatrix);
	praat_addAction1 (classTableOfReal, 0, U"Draw box plots...", U"Draw scatter plot matrix...", 1, DO_TableOfReal_drawBoxPlots);
	praat_addAction1 (classTableOfReal, 0, U"Draw biplot...", U"Draw box plots...", 1, DO_TableOfReal_drawBiplot);
	praat_addAction1 (classTableOfReal, 0, U"Draw vectors...", U"Draw box plots...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawVectors);
	praat_addAction1 (classTableOfReal, 1, U"Draw row as histogram...", U"Draw biplot...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawRowAsHistogram);
	praat_addAction1 (classTableOfReal, 1, U"Draw rows as histogram...", U"Draw row as histogram...", praat_DEPTH_1 | praat_HIDDEN, DO_TableOfReal_drawRowsAsHistogram);
	praat_addAction1 (classTableOfReal, 1, U"Draw column as distribution...", U"Draw rows as histogram...", praat_DEPTH_1, DO_TableOfReal_drawColumnAsDistribution);

	praat_addAction2 (classStrings, 1, classPermutation, 1, U"Permute strings", nullptr, 0, DO_Strings_and_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, U"Permute rows", nullptr, 0, DO_TableOfReal_and_Permutation_permuteRows);

	praat_addAction1 (classTextGrid, 0, U"Extend time...", U"Scale times...", 2, DO_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, U"Set tier name...", U"Remove tier...", 1, DO_TextGrid_setTierName);
	praat_addAction1 (classTextGrid, 0, U"Replace interval text...", U"Set interval text...", 2, DO_TextGrid_replaceIntervalTexts);
	praat_addAction1 (classTextGrid, 0, U"Replace point text...", U"Set point text...", 2, DO_TextGrid_replacePointTexts);
	praat_addAction1 (classTextGrid, 2, U"To Table (text alignment)...", U"Extract part...", 0, DO_TextGrids_to_Table_textAlignmentment);
	praat_addAction2 (classTextGrid, 2, classEditCostsTable, 1, U"To Table (text alignment)...", nullptr, 0, DO_TextGrids_and_EditCostsTable_to_Table_textAlignmentment);

	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)

	INCLUDE_LIBRARY (praat_uvafon_MDS_init)
	INCLUDE_LIBRARY (praat_KlattGrid_init)
	INCLUDE_LIBRARY (praat_HMM_init)
	INCLUDE_LIBRARY (praat_BSS_init)
}

/* End of file praat_David.cpp */
