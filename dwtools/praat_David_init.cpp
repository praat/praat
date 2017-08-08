/* praat_David_init.cpp
 *
 * Copyright (C) 1993-2017 David Weenink, 2015 Paul Boersma
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

#include "NUMcomplex.h"
#include "NUMlapack.h"
#include "NUMmachar.h"

#include "ActivationList.h"
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
#include "PatternList.h"
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
#include "Discriminant_PatternList_Categories.h"
#include "DTW_and_TextGrid.h"
#include "Permutation_and_Index.h"
#include "Pitch_extensions.h"
#include "Sound_and_Spectrogram_extensions.h"
#include "Sound_to_Pitch2.h"
#include "Sound_to_SPINET.h"
#include "TableOfReal_and_SVD.h"
#include "TextGrid_and_DurationTier.h"
#include "TextGrid_and_PitchTier.h"
#include "VowelEditor.h"

#include "praat_TimeFrameSampled.h"
#include "praat_Matrix.h"
#include "praat_TableOfReal.h"
#include "praat_uvafon.h"

#undef iam
#define iam iam_LOOP

static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *DRAW_BUTTON    = U"Draw -";
static const char32 *MODIFY_BUTTON  = U"Modify -";
static const char32 *EXTRACT_BUTTON = U"Extract -";

void praat_TableOfReal_init2 (ClassInfo klas);
void praat_SSCP_as_TableOfReal_init (ClassInfo klas);

void praat_CC_init (ClassInfo klas);
void praat_BandFilterSpectrogram_query_init (ClassInfo klas);
void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas);

#define DTW_constraints_addCommonFields(matchStart,matchEnd,slopeConstraint) \
	LABEL (U"", U"Boundary conditions") \
	BOOLEANVAR (matchStart, U"Match begin positions", false) \
	BOOLEANVAR (matchEnd, U"Match end positions", false) \
	RADIOVAR (slopeConstraint, U"Slope constraint", 1) \
		RADIOBUTTON (U"no restriction") \
		RADIOBUTTON (U"1/3 < slope < 3") \
		RADIOBUTTON (U"1/2 < slope < 2") \
		RADIOBUTTON (U"2/3 < slope < 3/2")

#undef INCLUDE_DTW_SLOPES

#define FIND_ONE_AND_GENERIC(klas1,klas2) \
	klas1 me = nullptr; klas2 you = nullptr; \
	LOOP { if (CLASS == class##klas1) me = (klas1) OBJECT; \
	else if (Thing_isSubclass (CLASS, class##klas2)) { you = (klas2) OBJECT; } } \
	Melder_assert (me && you);

#define CONVERT_ONE_AND_GENERIC(klas1,klas2) FIND_ONE_AND_GENERIC(klas1,klas2)
#define CONVERT_ONE_AND_GENERIC_END(...)  praat_new (result.move(), __VA_ARGS__); END

	
/********************** Activation *******************************************/

FORM (MODIFY_ActivationList_formula, U"ActivationList: Formula", nullptr) {
	LABEL (U"label", U"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }}")
	TEXTVAR (formula, U"formula", U"self")
	OK
DO
	MODIFY_EACH (ActivationList)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_END
}

DIRECT (NEW_ActivationList_to_Matrix) {
	CONVERT_EACH (ActivationList)
		autoMatrix result = ActivationList_to_Matrix (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_ActivationList_to_PatternList) {
	CONVERT_EACH (ActivationList)
		autoPatternList result = ActivationList_to_PatternList (me);
	CONVERT_EACH_END (my name)
}

/********************** BandFilterSpectrogram *******************************************/

FORM (GRAPHICS_BandFilterSpectrogram_drawFrequencyScale, U"", U"") {
	REALVAR (fromFrequency, U"left Horizontal frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Horizontal frequency range (Hz)", U"0.0")
	REALVAR (yFromFrequency, U"left Vertical frequency range (mel)", U"0.0")
	REALVAR (yToFrequency, U"right Vertical frequency range (mel)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_drawFrequencyScale (me, GRAPHICS, fromFrequency, toFrequency, yFromFrequency, yToFrequency, garnish);
	GRAPHICS_EACH_END
}

/********************** BarkFilter *******************************************/

DIRECT (HELP_BarkFilter_help) {
	HELP (U"BarkFilter")
}

DIRECT (HELP_BarkSpectrogram_help) {
	HELP (U"BarkSpectrogram")
}

FORM (GRAPHICS_BarkFilter_drawSpectrum, U"BarkFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REALVAR (time, U"Time (s)", U"0.1")
	REALVAR (fromFrequency, U"left Frequency range (Bark)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Bark)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Barks", garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkFilter_drawSekeyHansonFilterFunctions, U"BarkFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGERVAR (fromFilter, U"left Filter range", U"0")
	INTEGERVAR (toFilter, U"right Filter range", U"0")
	RADIOVAR (frequencyScale, U"Frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	BOOLEANVAR (amplitudeScale_dB, U"Amplitude scale in dB", true)
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		BarkFilter_drawSekeyHansonFilterFunctions (me, GRAPHICS, frequencyScale, fromFilter, toFilter, fromFrequency, toFrequency, amplitudeScale_dB, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkSpectrogram_drawSekeyHansonAuditoryFilters, U"BarkSpectrogram: Draw Sekey-Hanson auditory filters", U"BarkSpectrogram: Draw Sekey-Hanson auditory filters...") {
	INTEGERVAR (fromFilter, U"left Filter range", U"0")
	INTEGERVAR (toFilter, U"right Filter range", U"0")
	RADIOVAR (frequencyScale, U"Frequency scale", 2)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	BOOLEANVAR (amplitudeScale_dB, U"Amplitude scale in dB", 1)
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkSpectrogram)
		bool xIsHertz = (frequencyScale == 1);
		BarkSpectrogram_drawSekeyHansonFilterFunctions (me, GRAPHICS, xIsHertz, fromFilter, toFilter, fromFrequency, toFrequency, amplitudeScale_dB, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkFilter_paint, U"FilterBank: Paint", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (bark)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		FilterBank_paint ((FilterBank) me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_BarkFilter_to_BarkSpectrogram) {
	CONVERT_EACH (BarkFilter)
		autoBarkSpectrogram result = BarkFilter_to_BarkSpectrogram (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_MelFilter_to_MelSpectrogram) {
	CONVERT_EACH (MelFilter)
		autoMelSpectrogram result = MelFilter_to_MelSpectrogram (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_FormantFilter_to_Spectrogram) {
	CONVERT_EACH (FormantFilter);
		autoSpectrogram result = FormantFilter_to_Spectrogram (me);
	CONVERT_EACH_END (my name)
}

/********************** Categories  ****************************************/

FORM (MODIFY_Categories_append, U"Categories: Append 1 category", U"Categories: Append 1 category...") {
	SENTENCE (U"Category", U"")
	OK
DO
	MODIFY_EACH (Categories)
		OrderedOfString_append (me, GET_STRING (U"Category"));
	MODIFY_EACH_END
}

DIRECT (WINDOW_Categories_edit) {
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
END }

DIRECT (INTEGER_Categories_getNumberOfCategories) {
	INTEGER_ONE (Categories)
		long result = my size;
	INTEGER_ONE_END (U" categories")
}

DIRECT (INTEGER_Categories_getNumberOfDifferences) {
	NUMBER_COUPLE (Categories)
		long result = OrderedOfString_getNumberOfDifferences (me, you);
	NUMBER_COUPLE_END (U" differences")
}
DIRECT (REAL_Categories_getFractionDifferent) {
	NUMBER_COUPLE (Categories)
		double result = OrderedOfString_getFractionDifferent (me, you);
	NUMBER_COUPLE_END (U" (fraction different)")
}

DIRECT (INTEGER_Categories_difference) {
	NUMBER_COUPLE (Categories)
		long result;
		double fraction;
		OrderedOfString_difference (me, you, & result, &fraction);
	NUMBER_COUPLE_END (U" (difference)")
}

DIRECT (NEW_Categories_selectUniqueItems) {
	CONVERT_EACH (Categories)
		autoCategories result = Categories_selectUniqueItems (me);
	CONVERT_EACH_END (my name, U"_uniq")
}

DIRECT (NEW_Categories_to_Confusion) {
	CONVERT_COUPLE (Categories)
		autoConfusion result = Categories_to_Confusion (me, you);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

DIRECT (NEW_Categories_to_Strings) {
	CONVERT_EACH (Categories)
		autoStrings result = Categories_to_Strings (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_Categories_join) {
	CONVERT_COUPLE (Categories)
		autoOrderedOfString result = OrderedOfString_joinItems (me, you);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

DIRECT (NEW_Categories_permuteItems) {
	CONVERT_EACH (Collection)
		autoCollection result = Collection_permuteItems (me);
	CONVERT_EACH_END (my name, U"_perm")
}

/***************** CC ****************************************/

FORM (INTEGER_CC_getNumberOfCoefficients, U"Get number of coefficients", nullptr) {
	NATURALVAR (frameNumber, U"Frame number", U"1")
	OK
DO
	INTEGER_ONE (CC)
		long result = CC_getNumberOfCoefficients (me, frameNumber);
	INTEGER_ONE_END (U" (number of coefficients)")
}

FORM (REAL_CC_getValue, U"CC: Get value", U"CC: Get value...") {
	REALVAR (time, U"Time (s)", U"0.1")
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (CC)
		double result = CC_getValue (me, time, index);
	NUMBER_ONE_END (U" value")
}

FORM (REAL_CC_getValueInFrame, U"CC: Get value in frame", U"CC: Get value in frame...") {
	NATURALVAR (frameNumber, U"Frame number", U"1")
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (CC)
		double result = CC_getValueInFrame (me, frameNumber, index);
	NUMBER_ONE_END (U" value")
}

FORM (REAL_CC_getC0ValueInFrame, U"CC: Get c0 value in frame", U"CC: Get c0 value in frame...") {
	NATURALVAR (frameNumber, U"Frame number", U"1")
	OK
DO
	NUMBER_ONE (CC)
		double result = CC_getC0ValueInFrame (me, frameNumber);
	NUMBER_ONE_END (U" value")
}

FORM (GRAPHICS_CC_paint, U"CC: Paint", U"CC: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	INTEGERVAR (fromCoefficient, U"From coefficient", U"0")
	INTEGERVAR (toCoefficient, U"To coefficient", U"0")
	REALVAR (minimum, U"Minimum", U"0.0")
	REALVAR (maximum, U"Maximum", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CC)
		CC_paint (me, GRAPHICS, fromTime, toTime, fromCoefficient, toCoefficient, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_CC_drawC0, U"CC: Draw c0", U"CC: Draw c0...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CC)
		CC_drawC0 (me, GRAPHICS, fromTime, toTime,fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW1_CCs_to_DTW, U"CC: To DTW", U"CC: To DTW...") {
	LABEL (U"", U"Distance  between cepstral coefficients")
	REALVAR (cepstralWeight, U"Cepstral weight", U"1.0")
	REALVAR (logEnergyWeight, U"Log energy weight", U"0.0")
	REALVAR (regressionWeight, U"Regression weight", U"0.0")
	REALVAR (regressionLogEnergyWeight, U"Regression log energy weight", U"0.0")
	REALVAR (regressionWindowLength, U"Regression window length (s)", U"0.056")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (CC)
		autoDTW result = CCs_to_DTW (me, you, cepstralWeight, logEnergyWeight, regressionWeight, regressionLogEnergyWeight, regressionWindowLength);
		DTW_findPath (result.get(), matchStart, matchEnd, slopeConstraint);
	CONVERT_COUPLE_END (my name, U"_", your name);
}

DIRECT (NEW_CC_to_Matrix) {
	CONVERT_EACH (CC)
		autoMatrix result = CC_to_Matrix (me);
	CONVERT_EACH_END (my name)
}

/******************* class CCA ********************************/

DIRECT (HELP_CCA_help) {
	HELP (U"CCA")
}

FORM (GRAPHICS_CCA_drawEigenvector, U"CCA: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	OPTIONMENUVAR (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	INTEGERVAR (eigenVectorNumber, U"Eigenvector number", U"1")
	LABEL (U"", U"Multiply by eigenvalue?")
	BOOLEANVAR (useComponentLoadings, U"Component loadings", false)
	LABEL (U"", U"Select part of the eigenvector:")
	INTEGERVAR (fromElement, U"left Element range", U"0")
	INTEGERVAR (toElement, U"right Element range", U"0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"-1.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (connectPoints, U"Connect points", true)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CCA)
		CCA_drawEigenvector (me, GRAPHICS, xOrY, eigenVectorNumber, fromElement, toElement, fromAmplitude, toAmplitude, useComponentLoadings, markSize_mm, mark_string, connectPoints, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_CCA_getNumberOfCorrelations) {
	INTEGER_ONE (CCA)
		long result = my numberOfCoefficients;
	INTEGER_ONE_END (U"")
}

FORM (REAL_CCA_getCorrelationCoefficient, U"CCA: Get canonical correlation coefficient", U"CCA: Get canonical correlation coefficient") {
	NATURALVAR (coefficientNuber, U"Coefficient number", U"1")
	OK
DO
	NUMBER_ONE (CCA)
		double result = CCA_getCorrelationCoefficient (me, coefficientNuber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_CCA_getEigenvectorElement, U"CCA: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	OPTIONMENUVAR (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURALVAR (elementNumber, U"Element number", U"1")
	OK
DO
	NUMBER_ONE (CCA)
		double result = CCA_getEigenvectorElement (me, xOrY, eigenvectorNumber, elementNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_CCA_getZeroCorrelationProbability, U"CCA: Get zero correlation probability", U"CCA: Get zero correlation probability...") {
	NATURALVAR (coefficientNumber, U"Coefficient number", U"1")
	OK
DO
	NUMBER_ONE (CCA)
		double result, chisq, df;
		CCA_getZeroCorrelationProbability (me, coefficientNumber, & result, & chisq, & df);
	NUMBER_ONE_END (U" (= probability for chisq = ", chisq, U" and ndf = ", df, U")");
}

DIRECT (NEW1_CCA_and_Correlation_factorLoadings) {
	CONVERT_TWO (CCA, Correlation)
		autoTableOfReal result = CCA_and_Correlation_factorLoadings (me, you);
	CONVERT_TWO_END (my name, U"_loadings")
}

FORM (REAL_CCA_and_Correlation_getVarianceFraction, U"CCA & Correlation: Get variance fraction", U"CCA & Correlation: Get variance fraction...") {
	LABEL (U"", U"Get the fraction of variance from the data in set...")
	OPTIONMENUVAR (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	LABEL (U"", U"extracted by...")
	NATURALVAR (fromCanonicalVariate, U"left Canonical variate range", U"1")
	NATURALVAR (toCanonicalVariate, U"right Canonical variate range", U"1")
	OK
DO
	NUMBER_TWO (CCA, Correlation)
		double result = CCA_and_Correlation_getVarianceFraction (me, you, xOrY, fromCanonicalVariate, toCanonicalVariate);
	NUMBER_TWO_END (U" (fraction variance from ", (xOrY == 1 ? U"y" : U"x"), U", extracted by canonical variates ", fromCanonicalVariate, U" to ", toCanonicalVariate, U")")
}

FORM (REAL_CCA_and_Correlation_getRedundancy_sl, U"CCA & Correlation: Get Stewart-Love redundancy", U"CCA & Correlation: Get redundancy (sl)...") {
	LABEL (U"", U"Get the redundancy of the data in set...")
	OPTIONMENUVAR (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	LABEL (U"", U"extracted by...")
	NATURALVAR (fromCanonicalVariate, U"left Canonical variate range", U"1")
	NATURALVAR (toCanonicalVariate, U"right Canonical variate range", U"1")
	LABEL (U"", U"...given the availability of the data in the other set.")
	OK
DO
	NUMBER_TWO (CCA, Correlation)
		double result = CCA_and_Correlation_getRedundancy_sl (me, you, xOrY, fromCanonicalVariate, toCanonicalVariate);
	NUMBER_TWO_END (U" (redundancy from ", (xOrY == 1 ? U"y" : U"x"), U" extracted by canonical variates ", fromCanonicalVariate, U" to ", toCanonicalVariate, U")")
}

DIRECT (NEW_CCA_and_TableOfReal_factorLoadings) {
	CONVERT_TWO (CCA, TableOfReal)
		autoTableOfReal result = CCA_and_TableOfReal_factorLoadings (me, you);
	CONVERT_TWO_END (my name, U"_loadings")
}

FORM (NEW_CCA_and_TableOfReal_scores, U"CCA & TableOfReal: To TableOfReal (scores)", U"CCA & TableOfReal: To TableOfReal (scores)...") {
	INTEGERVAR (numberOfCanonicalVariates, U"Number of canonical correlations", U"0 (= all)")
	OK
DO
	CONVERT_TWO (CCA, TableOfReal)
		autoTableOfReal result = CCA_and_TableOfReal_scores (me, you, numberOfCanonicalVariates);
	CONVERT_TWO_END (my name, U"_scores");
}

FORM (NEW1_CCA_and_TableOfReal_predict, U"CCA & TableOfReal: Predict", U"CCA & TableOfReal: Predict...") {
	LABEL (U"", U"The data set from which to predict starts at...")
	INTEGERVAR (columnNumber, U"Column number", U"1")
	OK
DO
	CONVERT_TWO (CCA, TableOfReal)
		autoTableOfReal result = CCA_and_TableOfReal_predict (me, you, columnNumber);
	CONVERT_TWO_END (your name, U"_", my name)
}

FORM (NEW_CCA_extractEigen, U"CCA: Exxtract Eigen", nullptr) {
	OPTIONMENUVAR (choice, U"variablesType", 1)
		OPTION (U"Dependent")
		OPTION (U"Independent")
	OK
DO
		CONVERT_EACH (CCA)
			autoEigen result = choice == 1 ? Data_copy (my y.get()) : Data_copy (my x.get());
		CONVERT_EACH_END (my name, (choice == 1 ? U"_y" : U"_x"))
}

/***************** ChebyshevSeries ****************************************/

DIRECT (HELP_ChebyshevSeries_help) {
	HELP (U"ChebyshevSeries")
}

FORM (NEW1_ChebyshevSeries_create, U"Create ChebyshevSeries", U"Create ChebyshevSeries...") {
	WORDVAR (name, U"Name", U"cs")
	LABEL (U"", U"Domain")
	REALVAR (xmin, U"Xmin", U"-1")
	REALVAR (xmax, U"Xmax", U"1")
	LABEL (U"", U"ChebyshevSeries(x) = c[1] T[0](x) + c[2] T[1](x) + ... c[n+1] T[n](x)")
	LABEL (U"", U"T[k] is a Chebyshev polynomial of degree k")
	SENTENCEVAR (coefficients_string, U"Coefficients (c[k])", U"0 0 1.0")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CREATE_ONE
		autoChebyshevSeries result = ChebyshevSeries_createFromString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

DIRECT (NEW_ChebyshevSeries_to_Polynomial) {
	CONVERT_EACH (ChebyshevSeries)
		autoPolynomial result = ChebyshevSeries_to_Polynomial (me);
	CONVERT_EACH_END (my name);
}

/***************** ClassificationTable ****************************************/

DIRECT (HELP_ClassificationTable_help) {
	HELP (U"ClassificationTable")
}

FORM (INTEGER_ClassificationTable_getClassIndexAtMaximumInRow, U"ClassificationTable: Get class index at maximum in row", nullptr) {
	NATURALVAR (rowNumber, U"Row number", U"1")
	OK
DO
	INTEGER_ONE (ClassificationTable)
		long result = TableOfReal_getColumnIndexAtMaximumInRow (me, rowNumber);
	INTEGER_ONE_END (U" class index at maximum in row")
}

FORM (INTEGER_ClassificationTable_getClassLabelAtMaximumInRow, U"ClassificationTable: Get class label at maximum in row", nullptr) {
	NATURALVAR (rowNumber, U"Row number", U"1")
	OK
DO
	STRING_ONE (ClassificationTable)
		const char32 *result = TableOfReal_getColumnLabelAtMaximumInRow (me, rowNumber);
	STRING_ONE_END
}

// deprecated 2014
DIRECT (NEW_ClassificationTable_to_Confusion_old) {
	CONVERT_EACH (ClassificationTable)
		autoConfusion result = ClassificationTable_to_Confusion (me, false);
	CONVERT_EACH_END (my name)
}

FORM (NEW_ClassificationTable_to_Confusion, U"ClassificationTable: To Confusion", U"ClassificationTable: To Confusion...") {
	BOOLEAN4 (onlyClassLabels, U"Only class labels", true)
	OK
DO
	CONVERT_EACH (ClassificationTable)
		autoConfusion result = ClassificationTable_to_Confusion (me, onlyClassLabels);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_ClassificationTable_to_Correlation_columns) {
	CONVERT_EACH (ClassificationTable)
		autoCorrelation result = ClassificationTable_to_Correlation_columns (me);
	CONVERT_EACH_END (my name, U"_col")
}

DIRECT (NEW_ClassificationTable_to_Strings_maximumProbability) {
	CONVERT_EACH (ClassificationTable)
		autoStrings result = ClassificationTable_to_Strings_maximumProbability (me);
	CONVERT_EACH_END (my name)
}

/********************** Confusion *******************************************/

DIRECT (HELP_Confusion_help) {
	HELP (U"Confusion")
}

FORM (NEW1_Confusion_createSimple, U"Create simple Confusion", U"Create simple Confusion...") {
	WORDVAR (name, U"Name", U"simple")
	SENTENCEVAR (labels, U"Labels", U"u i a")
	OK
DO
	CREATE_ONE
		autoConfusion result = Confusion_createSimple (labels);
	CREATE_ONE_END (name)
}

FORM (MODIFY_Confusion_increase, U"Confusion: Increase", U"Confusion: Increase...") {
	WORDVAR (stimulus, U"Stimulus", U"u")
	WORDVAR (response, U"Response", U"i")
	OK
DO
	MODIFY_EACH (Confusion)
		Confusion_increase (me, stimulus, response);
	MODIFY_EACH_END
}

FORM (REAL_Confusion_getValue, U"Confusion: Get value", nullptr) {
	WORDVAR (stimulus, U"Stimulus", U"u")
	WORDVAR (response, U"Response", U"i")
	OK
DO
	NUMBER_ONE (Confusion)
		double result = Confusion_getValue (me, stimulus, response);
	NUMBER_ONE_END (U" ([\"", stimulus, U"\", \"",  response, U"\"])")
}

FORM (REAL_Confusion_getResponseSum, U"Confusion: Get response sum", U"Confusion: Get response sum...") {
	WORDVAR (response, U"Response", U"u")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getColumnSumByLabel (me, response);
	NUMBER_ONE_END (U" (response sum)")
}

FORM (REAL_Confusion_getStimulusSum, U"Confusion: Get stimulus sum", U"Confusion: Get stimulus sum...") {
	WORDVAR (stimulus, U"Stimulus", U"u")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getRowSumByLabel (me, stimulus);
	NUMBER_ONE_END (U" (stimulus sum)")
}

DIRECT (NEW_Confusion_to_TableOfReal_marginals) {
	CONVERT_EACH (Confusion)
		autoTableOfReal result = Confusion_to_TableOfReal_marginals (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_Confusion_difference) {
	CONVERT_COUPLE (Confusion)
		autoMatrix result = Confusion_difference (me, you);
	CONVERT_COUPLE_END (U"diffs")
}

FORM (NEW_Confusion_condense, U"Confusion: Condense", U"Confusion: Condense...") {
	SENTENCEVAR (search_string, U"Search", U"^(u|i)$")
	SENTENCEVAR (replace_string, U"Replace", U"high")
	INTEGERVAR (replaceLimit, U"Replace limit", U"0 (= unlimited)")
	RADIOVARx (matchType, U"Search and replace are", 2, 0)
		RADIOBUTTON (U"Literals")
		RADIOBUTTON (U"Regular Expressions")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_condense (me, search_string, replace_string, replaceLimit, matchType);
	CONVERT_EACH_END (my name, U"_cnd")
}

FORM (NEW_Confusion_group, U"Confusion: Group stimuli & responses", U"Confusion: Group...") {
	SENTENCEVAR (labels, U"Stimuli & Responses", U"u i")
	SENTENCEVAR (newLabel, U"New label", U"high")
	INTEGERVAR (newPosition, U"New label position", U"0 (= at start)")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_group (me, labels, newLabel, newPosition);
	CONVERT_EACH_END (my name, U"_sr", newLabel)
}

FORM (NEW_Confusion_groupStimuli, U"Confusion: Group stimuli", U"Confusion: Group stimuli...") {
	SENTENCEVAR (stimuli_string, U"Stimuli", U"u i")
	SENTENCEVAR (newLabel, U"New label", U"high")
	INTEGERVAR (newPosition, U"New label position", U"0")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_groupStimuli (me,stimuli_string, newLabel, newPosition);
	CONVERT_EACH_END (my name, U"_s", newLabel);
}

FORM (NEW_Confusion_groupResponses, U"Confusion: Group responses", U"Confusion: Group responses...") {
	SENTENCEVAR (responses_string, U"Responses", U"a i")
	SENTENCEVAR (newLabel, U"New label", U"front")
	INTEGERVAR (newPosition, U"New label position", U"0")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_groupResponses (me, responses_string, newLabel, newPosition);
	CONVERT_EACH_END (my name, U"_s", newLabel);
}

FORM (GRAPHICS_Confusion_drawAsNumbers, U"Confusion: Draw as numbers", nullptr) {
	BOOLEANVAR (drawMarginals, U"Draw marginals", true)
	RADIOVAR (format, U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURALVAR (precision, U"Precision", U"5")
	OK
DO
	GRAPHICS_EACH (Confusion)
		Confusion_drawAsNumbers (me, GRAPHICS, drawMarginals, format, precision);
	GRAPHICS_EACH_END
}

DIRECT (REAL_Confusion_getFractionCorrect) {
	NUMBER_ONE (Confusion)
		double result;
		Confusion_getFractionCorrect (me, & result, nullptr);
	NUMBER_ONE_END (U" (fraction correct)")
}

DIRECT (MODIFY_Confusion_and_ClassificationTable_increase) {
	MODIFY_FIRST_OF_TWO (Confusion, ClassificationTable)
		Confusion_and_ClassificationTable_increase (me, you);
	MODIFY_FIRST_OF_TWO_END
}

/******************* Confusion & Matrix *************************************/

FORM (GRAPHICS_Confusion_Matrix_draw, U"Confusion & Matrix: Draw confusions with arrows", nullptr) {
	INTEGERVAR (categoryPosition, U"Category position", U"0 (= all)")
	REALVAR (lowerLevel, U"Lower level (%)", U"0")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	REQUIRE (categoryPosition >= 0, U"Category position must be >= 0")
	GRAPHICS_TWO (Confusion, Matrix)
		Confusion_Matrix_draw (me, you, GRAPHICS, categoryPosition, lowerLevel, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_TWO_END
}

/********************** ComplexSpectrogram *******************************************/

DIRECT (HELP_ComplexSpectrogram_help) {
	HELP (U"ComplexSpectrogram_help")
}

FORM (NEW_ComplexSpectrogram_to_Sound, U"ComplexSpectrogram: To Sound", nullptr) {
	POSITIVEVAR (durationFactor, U"Duration factor", U"1.0")
	OK
DO
	CONVERT_EACH (ComplexSpectrogram)
		autoSound result = ComplexSpectrogram_to_Sound (me, durationFactor);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_ComplexSpectrogram_to_Spectrogram) {
	CONVERT_EACH (ComplexSpectrogram)
		autoSpectrogram result = ComplexSpectrogram_to_Spectrogram (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_ComplexSpectrogram_to_Spectrum, U"ComplexSpectrogram: To Spectrum (slice)", nullptr) {
	REALVAR (time, U"Time (s)", U"0.0")
	OK
DO
	CONVERT_EACH (ComplexSpectrogram)
		autoSpectrum result = ComplexSpectrogram_to_Spectrum (me, time);
	CONVERT_EACH_END (my name)
}

/********************** Correlation *******************************************/

FORM (NEW1_Correlation_createSimple, U"Create simple Correlation", U"Create simple Correlation...") {
	WORDVAR (name, U"Name", U"correlation")
	SENTENCEVAR (correlations_string, U"Correlations", U"1.0 0.5 1.0")
	SENTENCEVAR (centroid_string, U"Centroid", U"0.0 0.0")
	NATURALVAR (numberOfObservations, U"Number of observations", U"100")
	OK
DO
	CREATE_ONE
		autoCorrelation result = Correlation_createSimple (correlations_string, centroid_string, numberOfObservations);
	CREATE_ONE_END (name)
}

DIRECT (HELP_Correlation_help) {
	HELP (U"Correlation")
}

FORM (NEW_Correlation_confidenceIntervals, U"Correlation: Confidence intervals...", U"Correlation: Confidence intervals...") {
	POSITIVEVAR (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	INTEGERVAR (numberOfTests, U"Number of tests (Bonferroni correction)", U"0")
	RADIOVAR (approximation, U"Approximation", 1)
		RADIOBUTTON (U"Ruben")
		RADIOBUTTON (U"Fisher")
	OK
DO
	CONVERT_EACH (Correlation)
		autoTableOfReal result = Correlation_confidenceIntervals (me, confidenceLevel, numberOfTests, approximation);
	CONVERT_EACH_END (my name, U"_conf_intervals")
}

FORM (REAL_Correlation_testDiagonality_bartlett, U"Correlation: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...") {
	NATURALVAR (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	NUMBER_ONE (Correlation)
		double chisq, result, df;
		Correlation_testDiagonality_bartlett (me, numberOfConstraints, & chisq, & result, & df);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", df, U")")
}

DIRECT (NEW_Correlation_to_PCA) {
	CONVERT_EACH (Correlation)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_END (my name)
}

/********************** Covariance *******************************************/

DIRECT (HELP_Covariance_help) {
	HELP (U"Covariance")
}

FORM (NEW1_Covariance_createSimple, U"Create simple Covariance", U"Create simple Covariance...") {
	WORDVAR (name, U"Name", U"c")
	SENTENCEVAR (covariances_string, U"Covariances", U"1.0 0.0 1.0")
	SENTENCEVAR (centroid_string, U"Centroid", U"0.0 0.0")
	NATURALVAR (numberOfObservations, U"Number of observations", U"100")
	OK
DO
	CREATE_ONE
		autoCovariance result = Covariance_createSimple (covariances_string, centroid_string, numberOfObservations);
	CREATE_ONE_END (name)
}

FORM (REAL_Covariance_getProbabilityAtPosition, U"Covariance: Get probability at position", nullptr) {
	SENTENCEVAR (position_string, U"Position", U"10.0 20.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result = Covariance_getProbabilityAtPosition_string (me, position_string);
	NUMBER_ONE_END (U" (= probability at position ", position_string, U")")
}

FORM (REAL_Covariance_getSignificanceOfOneMean, U"Covariance: Get significance of one mean", U"Covariance: Get significance of one mean...") {
	LABEL (U"", U"Get probability that the estimated mean for")
	NATURALVAR (index, U"Index", U"1")
	LABEL (U"", U"(or an estimated mean even further away)")
	LABEL (U"", U"could arise if the true mean were")
	REALVAR (value, U"Value", U"0.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result, t, ndf;
		Covariance_getSignificanceOfOneMean (me, index, value, & result, & t, & ndf);
	NUMBER_ONE_END (U" (= probability, based on t = ", t, U" and ndf = ", ndf)
}

FORM (REAL_Covariance_getSignificanceOfMeansDifference, U"Covariance: Get significance of means difference", U"Covariance: Get significance of means difference...") {
	LABEL (U"", U"Get probability that the estimated difference between the means for")
	NATURALVAR (index1, U"Index1", U"1")
	NATURALVAR (index2, U"Index2", U"2")
	LABEL (U"", U"could arise if the true mean were")
	REALVAR (value, U"Value", U"0.0")
	LABEL (U"", U"Assume the means are ")
	BOOLEANVAR (paired, U"Paired", true)
	LABEL (U"", U"and have")
	BOOLEANVAR (equalVariances, U"Equal variances", true)
	OK
DO
	NUMBER_ONE (Covariance)
		double result, t, ndf;
		Covariance_getSignificanceOfMeansDifference (me, index1, index2, value, paired, equalVariances, & result, & t, & ndf);
	NUMBER_ONE_END (U" (= probability, based on t = ", t, U"and ndf = ", ndf, U")")
}

FORM (REAL_Covariance_getSignificanceOfOneVariance, U"Covariance: Get significance of one variance", U"Covariance: Get significance of one variance...") {
	LABEL (U"", U"Get the probability that the estimated variance for")
	NATURALVAR (index, U"Index", U"1")
	LABEL (U"", U"(or an even larger estimated variance)")
	LABEL (U"", U"could arise if the true variance were")
	REALVAR (value, U"Value", U"0.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result, chisq; long ndf;
		Covariance_getSignificanceOfOneVariance (me, index, value, & result, & chisq, & ndf);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", ndf)
}

FORM (REAL_Covariance_getSignificanceOfVariancesRatio, U"Covariance: Get significance of variances ratio", nullptr) {
	LABEL (U"", U"Get the probability that the estimated variance ratio observed for")
	NATURALVAR (index1, U"Index1", U"1")
	NATURALVAR (index2, U"Index2", U"2")
	LABEL (U"", U"(or an estimated ratio even further away)")
	LABEL (U"", U"could arise if the true ratio were")
	REALVAR (value, U"Value", U"1.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result, f, df;
		Covariance_getSignificanceOfVariancesRatio (me, index1, index2, value, & result, & f , & df);
	NUMBER_ONE_END (U" (= probability, based on F = ", f, U" and ndf1 = ", df, U" and ndf2 = ", df)
}

FORM (REAL_Covariance_getFractionVariance, U"Covariance: Get fraction variance", U"Covariance: Get fraction variance...") {
	NATURALVAR (fromDimension, U"From dimension", U"1")
	NATURALVAR (toDimension, U"To dimension", U"1")
	OK
DO
	NUMBER_ONE (Covariance)
		double result = SSCP_getFractionVariation (me, fromDimension, toDimension);
	NUMBER_ONE_END (U"")
}

FORM (INFO_Covariances_reportMultivariateMeanDifference, U"Covariances: Report multivariate mean difference", U"Covariances: Report multivariate mean difference...") {
	LABEL (U"", U"Get probability that the estimated multivariate means difference could arise ")
	LABEL (U"", U"if the actual means were equal.")
	LABEL (U"", U"")
	LABEL (U"", U"Assume for both means we have")
	BOOLEANVAR (covariancesAreEqual, U"Equal covariances", true)
	OK
DO
	INFO_COUPLE (Covariance)
		double prob, fisher, df1, df2, difference;
		MelderInfo_open ();
		difference = Covariances_getMultivariateCentroidDifference (me, you, covariancesAreEqual, & prob, & fisher, & df1, & df2);
		MelderInfo_writeLine (U"Under the assumption that the two covariances are", (covariancesAreEqual ? U" " : U" not "), U"equal:");
		MelderInfo_writeLine (U"Difference between multivariate means = ", difference);
		MelderInfo_writeLine (U"Fisher's F = ", fisher);
		MelderInfo_writeLine (U"Significance from zero = ", prob);
		MelderInfo_writeLine (U"Degrees of freedom = ", df1, U", ", df2);
		MelderInfo_writeLine (U"(Number of observations = ", me -> numberOfObservations, U", ", you -> numberOfObservations);
		MelderInfo_writeLine (U"Dimension of covariance matrices = ", me -> numberOfRows, U")");
		MelderInfo_close ();
	INFO_COUPLE_END
}

FORM (NEW_Covariance_to_TableOfReal_randomSampling, U"Covariance: To TableOfReal (random sampling)", U"Covariance: To TableOfReal (random sampling)...") {
	INTEGERVAR (numberOfDataPoints, U"Number of data points", U"0")
	OK
DO
	CONVERT_EACH (Covariance)
		autoTableOfReal result = Covariance_to_TableOfReal_randomSampling (me, numberOfDataPoints);
	CONVERT_EACH_END (my name)
}

DIRECT (INFO_Covariances_reportEquality) {
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
END }

DIRECT (NEW_Covariance_to_Correlation) {
	CONVERT_EACH (Covariance)
		autoCorrelation result = SSCP_to_Correlation (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Covariance_to_PCA) {
	CONVERT_EACH (Covariance)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_Covariances_pool) {
	CONVERT_TYPED_LIST (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_pool (list.get());
	CONVERT_TYPED_LIST_END (U"pool")
}

FORM (NEW1_Covariance_and_TableOfReal_mahalanobis, U"Covariance & TableOfReal: To TableOfReal (mahalanobis)", U"Covariance & TableOfReal: To TableOfReal (mahalanobis)...") {
	BOOLEANVAR (centroidFromTable, U"Centroid from table", 0)
	OK
DO
	CONVERT_TWO (Covariance, TableOfReal)
		autoTableOfReal result = Covariance_and_TableOfReal_mahalanobis (me, you, centroidFromTable);
	CONVERT_TWO_END (U"mahalanobis")
}

/********************** Discriminant **********************************/

DIRECT (HELP_Discriminant_help) {
	HELP (U"Discriminant")
}

DIRECT (MODIFY_Discriminant_setGroupLabels) {
	MODIFY_FIRST_OF_TWO (Discriminant, Strings)
		Discriminant_setGroupLabels (me, you);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW1_Discriminant_and_PatternList_to_Categories, U"Discriminant & PatternList: To Categories", U"Discriminant & PatternList: To Categories...") {
	BOOLEANVAR (poolCovariances, U"Pool covariance matrices", true)
	BOOLEANVAR (useAPrioriProbabilities, U"Use apriori probabilities", true)
	OK
DO
	CONVERT_TWO (Discriminant, PatternList)
		autoCategories result = Discriminant_and_PatternList_to_Categories (me, you, poolCovariances, useAPrioriProbabilities);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Discriminant_and_TableOfReal_to_Configuration, U"Discriminant & TableOfReal: To Configuration", U"Discriminant & TableOfReal: To Configuration...") {
		INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0")
		OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_TWO (Discriminant, TableOfReal)
		autoConfiguration result = Discriminant_and_TableOfReal_to_Configuration (me, you, numberOfDimensions);
	CONVERT_TWO_END (my name, U"_", your name)
}

DIRECT (hint_Discriminant_and_TableOfReal_to_ClassificationTable) {
	Melder_information (U"You can use the Discriminant as a classifier by \nselecting a Discriminant and a TableOfReal object together.");
END }

FORM (NEW1_Discriminant_and_TableOfReal_to_ClassificationTable, U"Discriminant & TableOfReal: To ClassificationTable", U"Discriminant & TableOfReal: To ClassificationTable...") {
	BOOLEANVAR (poolCovariances, U"Pool covariance matrices", true)
	BOOLEANVAR (useAPrioriProbabilities, U"Use apriori probabilities", true)
	OK
DO
	CONVERT_TWO (Discriminant, TableOfReal)
		autoClassificationTable result = Discriminant_and_TableOfReal_to_ClassificationTable (me, you, poolCovariances, useAPrioriProbabilities);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Discriminant_and_TableOfReal_mahalanobis, U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)", U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...") {
	SENTENCEVAR (groupLabel, U"Group label", U"")
	BOOLEANVAR (poolCovariances, U"Pool covariance matrices", false)
	OK
DO
	CONVERT_TWO (Discriminant, TableOfReal)
		long group = Discriminant_groupLabelToIndex (me, groupLabel);
		REQUIRE (group > 0, U"Group label does not exist.")
		autoTableOfReal result = Discriminant_and_TableOfReal_mahalanobis (me, you, group, poolCovariances);
	CONVERT_TWO_END (U"mahalanobis")
}

DIRECT (INTEGER_Discriminant_getNumberOfEigenvalues) {
	INTEGER_ONE (Discriminant)
		long result = my eigen -> numberOfEigenvalues;
	INTEGER_ONE_END (U" (number of eigenvalues)")
}

DIRECT (INTEGER_Discriminant_getDimension) {
	INTEGER_ONE (Discriminant)
		long result = my eigen -> dimension;
	INTEGER_ONE_END (U" (dimension)")
}

FORM (REAL_Discriminant_getEigenvalue, U"Discriminant: Get eigenvalue", U"Eigen: Get eigenvalue...") {
	NATURALVAR (eigenvalueNumber, U"Eigenvalue number", U"1")
	OK
DO
	NUMBER_ONE (Discriminant)
		if (eigenvalueNumber > my eigen -> numberOfEigenvalues) {
			Melder_throw (U"Eigenvalue number must be smaller than ", my eigen -> numberOfEigenvalues + 1);
		}
		long result = my eigen -> eigenvalues[eigenvalueNumber];
	NUMBER_ONE_END (U" (eigenvalue [)", eigenvalueNumber, U"])")
}

FORM (REAL_Discriminant_getSumOfEigenvalues, U"Discriminant:Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...") {
	INTEGERVAR (fromEigenvalue, U"left Eigenvalue range",  U"0")
	INTEGERVAR (toEigenvalue, U"right Eigenvalue range", U"0")
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Eigen_getSumOfEigenvalues (my eigen.get(), fromEigenvalue, toEigenvalue);
	NUMBER_ONE_END (U"")
}

FORM (REAL_Discriminant_getEigenvectorElement, U"Discriminant: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURALVAR (elementNumber, U"Element number", U"1")
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Eigen_getEigenvectorElement (my eigen.get(), eigenvectorNumber, elementNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_Discriminant_getWilksLambda, U"Discriminant: Get Wilks' lambda", U"Discriminant: Get Wilks' lambda...") {
	LABEL (U"", U"Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	INTEGERVAR (from, U"From", U"1") //TODO better name
	OK
DO
	REQUIRE (from >= 1, U"Number must be greater than or equal to one.")
	NUMBER_ONE (Discriminant)
		double result = Discriminant_getWilksLambda (me, from);
	NUMBER_ONE_END (U" (wilks lambda)")
}

FORM (REAL_Discriminant_getCumulativeContributionOfComponents, U"Discriminant: Get cumulative contribution of components", U"Eigen: Get cumulative contribution of components...") {
	NATURALVAR (fromComponent, U"From component", U"1")
	NATURALVAR (toComponent, U"To component", U"1")
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Eigen_getCumulativeContributionOfComponents (my eigen.get(), fromComponent, toComponent);
	NUMBER_ONE_END (U" (cumulative contribution)")
}


FORM (REAL_Discriminant_getPartialDiscriminationProbability, U"Discriminant: Get partial discrimination probability", U"Discriminant: Get partial discrimination probability...") {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"1")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater than or equal to zero.")
	NUMBER_ONE (Discriminant)
		double result, chisq, df;
		Discriminant_getPartialDiscriminationProbability (me, numberOfDimensions, & result, & chisq, & df);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", df, U")");
}

DIRECT (REAL_Discriminant_getHomegeneityOfCovariances_box) {
	NUMBER_ONE (Discriminant)
		double chisq, result, ndf;
		SSCPList_getHomegeneityOfCovariances_box (my groups.get(), & result, & chisq, & ndf);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", ndf, U")")
}

DIRECT (INFO_Discriminant_reportEqualityOfCovariances_wald) {
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
END }

FORM (REAL_Discriminant_getConcentrationEllipseArea, U"Discriminant: Get concentration ellipse area", U"Discriminant: Get concentration ellipse area...") {
	SENTENCEVAR (groupLabel, U"Group label", U"")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEANVAR (discriminatPlane, U"Discriminant plane", true)
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	OK
DO
	NUMBER_ONE (Discriminant)
		long group = Discriminant_groupLabelToIndex (me, groupLabel);
		REQUIRE (group > 0, U"Group label does not exist.")
		double result = Discriminant_getConcentrationEllipseArea (me, group, numberOfSigmas, false, discriminatPlane, xDimension, yDimension);
	NUMBER_ONE_END (U" (concentration ellipse area)")
}

FORM (REAL_Discriminant_getConfidenceEllipseArea, U"Discriminant: Get confidence ellipse area", U"Discriminant: Get confidence ellipse area...") {
	SENTENCEVAR (groupLabel, U"Group label", U"")
	POSITIVEVAR (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEANVAR (discriminatPlane, U"Discriminant plane", true)
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	OK
DO
	NUMBER_ONE (Discriminant)
		long group = Discriminant_groupLabelToIndex (me, groupLabel);
		REQUIRE (group > 0, U"Group label does not exist.")
		double result = Discriminant_getConcentrationEllipseArea (me, group, confidenceLevel, true, discriminatPlane, xDimension, yDimension);
	NUMBER_ONE_END (U" (confidence ellipse area)")
}

FORM (REAL_Discriminant_getLnDeterminant_group, U"Discriminant: Get determinant (group)", U"Discriminant: Get determinant (group)...")
	SENTENCEVAR (groupLabel, U"Group label", U"") {
	OK
DO
	NUMBER_ONE (Discriminant)
		long group = Discriminant_groupLabelToIndex (me, groupLabel);
		REQUIRE (group > 0, U"Group label does not exist.")
		double result = Discriminant_getLnDeterminant_group (me, group);
	NUMBER_ONE_END (U" (ln(determinant) group")
}

DIRECT (REAL_Discriminant_getLnDeterminant_total) {
	NUMBER_ONE (Discriminant)
		double result = Discriminant_getLnDeterminant_total (me);
	NUMBER_ONE_END (U" (ln(determinant) total")
}

FORM (MODIFY_Discriminant_invertEigenvector, U"Discriminant: Invert eigenvector", nullptr) {
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Discriminant)
		Eigen_invertEigenvector (my eigen.get(), GET_INTEGER (U"Index of eigenvector"));
	MODIFY_EACH_END
}

FORM (GRAPHICS_Discriminant_drawEigenvalues, U"Discriminant: Draw eigenvalues", U"Eigen: Draw eigenvalues...") {
	INTEGERVAR (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGERVAR (toEigenvalue, U"right Eigenvalue range", U"0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (showFractions, U"Fraction of eigenvalues summed", false)
	BOOLEANVAR (showCumulativeValues, U"Cumulative", false)
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Eigen_drawEigenvalues (my eigen.get(), GRAPHICS, fromEigenvalue, toEigenvalue, fromAmplitude, toAmplitude, showFractions, showCumulativeValues, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawEigenvector, U"Discriminant: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	INTEGERVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	BOOLEANVAR (componentLoadings, U"Component loadings", false)
	INTEGERVAR (fromElement, U"left Element range", U"0")
	INTEGERVAR (toElement, U"right Element range", U"0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"-1.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (connectPoints, U"Connect points", true)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Eigen_drawEigenvector (my eigen.get(), GRAPHICS, eigenvectorNumber, fromElement, toElement, fromAmplitude, toAmplitude, componentLoadings, markSize_mm, mark_string, connectPoints, nullptr,  garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_Discriminant_drawSigmaEllipses, U"Discriminant: Draw sigma ellipses", U"Discriminant: Draw sigma ellipses...") {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEANVAR (discriminantPlane, U"Discriminant plane", true)
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false, nullptr, discriminantPlane, xDimension, yDimension,xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawOneSigmaEllipse, U"Discriminant: Draw one sigma ellipse", U"Discriminant: Draw one sigma ellipse...") {
	SENTENCEVAR (label, U"Label", U"")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEANVAR (discriminatPlane, U"Discriminant plane", true)
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false,  label, discriminatPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawConfidenceEllipses, U"Discriminant: Draw confidence ellipses", nullptr) {
	POSITIVEVAR (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEANVAR (discriminatPlane, U"Discriminant plane", true)
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, nullptr, discriminatPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawOneConfidenceEllipse, U"Discriminant: Draw one confidence ellipse", nullptr) {
	SENTENCEVAR (label, U"Label", U"")
	POSITIVEVAR (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEANVAR (discriminatPlane, U"Discriminant plane", true)
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, label, discriminatPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_Discriminant_extractBetweenGroupsSSCP) {
	CONVERT_EACH (Discriminant)
		autoSSCP result = Discriminant_extractBetweenGroupsSSCP (me);
	CONVERT_EACH_END (my name, U"_between")
}

DIRECT (NEW_Discriminant_extractGroupCentroids) {
	CONVERT_EACH (Discriminant)
		autoTableOfReal result = Discriminant_extractGroupCentroids (me);
	CONVERT_EACH_END (my name, U"_centroids")
}

DIRECT (NEW_Discriminant_extractGroupStandardDeviations) {
	CONVERT_EACH (Discriminant)
		autoTableOfReal result = Discriminant_extractGroupStandardDeviations (me);
	CONVERT_EACH_END (U"group_stddevs")
}

DIRECT (NEW_Discriminant_extractGroupLabels) {
	CONVERT_EACH (Discriminant)
		autoStrings result = Discriminant_extractGroupLabels (me);
	CONVERT_EACH_END (U"group_labels")
}

DIRECT (NEW_Discriminant_extractEigen) {
	CONVERT_EACH (Discriminant)
		autoEigen result = Data_copy (my eigen.get());
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Discriminant_extractPooledWithinGroupsSSCP) {
	CONVERT_EACH (Discriminant)
		autoSSCP result = Discriminant_extractPooledWithinGroupsSSCP (me);
	CONVERT_EACH_END (U"pooled_within")
}

FORM (NEW_Discriminant_extractWithinGroupSSCP, U"Discriminant: Extract within-group SSCP", U"Discriminant: Extract within-group SSCP...") {
	NATURALVAR (groupIndex, U"Group index", U"1")
	OK
DO
	CONVERT_EACH (Discriminant)
		autoSSCP result = Discriminant_extractWithinGroupSSCP (me, groupIndex);
	CONVERT_EACH_END (my name, U"_g", groupIndex)
}

DIRECT (INTEGER_Discriminant_getNumberOfFunctions) {
	INTEGER_ONE (Discriminant)
		long result = Discriminant_getNumberOfFunctions (me);
	INTEGER_ONE_END (U"")
}

DIRECT (INTEGER_Discriminant_getDimensionOfFunctions) {
	INTEGER_ONE (Discriminant)
		long result = Eigen_getDimensionOfComponents (my eigen.get());
	INTEGER_ONE_END (U"")
}

DIRECT (INTEGER_Discriminant_getNumberOfGroups) {
	INTEGER_ONE (Discriminant)
		long result = Discriminant_getNumberOfGroups (me);
	INTEGER_ONE_END (U"")
}

FORM (INTEGER_Discriminant_getNumberOfObservations, U"Discriminant: Get number of observations", U"Discriminant: Get number of observations...") {
	INTEGERVAR (group, U"Group", U"0 (= total)")
	OK
DO
	INTEGER_ONE (Discriminant)
		long result = Discriminant_getNumberOfObservations (me, group);
	INTEGER_ONE_END (U"")
}


/********************** DTW *******************************************/

FORM (MODIFY_DTW_and_Polygon_findPathInside, U"DTW & Polygon: Find path inside", nullptr) {
    RADIOVAR (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
	MODIFY_FIRST_OF_TWO (DTW, Polygon)
		DTW_and_Polygon_findPathInside (me, you, slopeConstraint, 0);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW1_DTW_and_Polygon_to_Matrix_cumulativeDistances, U"DTW & Polygon: To Matrix (cum. distances)", nullptr) {
    RADIOVAR (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_TWO (DTW, Polygon)
		autoMatrix result = DTW_and_Polygon_to_Matrix_cumulativeDistances (me, you, slopeConstraint);
     CONVERT_TWO_END (my name, U"_", slopeConstraint);
}

FORM (GRAPHICS_DTW_and_Sounds_draw, U"DTW & Sounds: Draw", U"DTW & Sounds: Draw...") {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_COUPLE_AND_ONE (Sound, DTW)
		DTW_and_Sounds_draw (him, you, me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_COUPLE_AND_ONE_END
}

FORM (GRAPHICS_DTW_and_Sounds_drawWarpX, U"DTW & Sounds: Draw warp (x)", U"DTW & Sounds: Draw warp (x)...") {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (time, U"Time (s)", U"0.1")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_COUPLE_AND_ONE (Sound, DTW)
		DTW_and_Sounds_drawWarpX (him, you, me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_COUPLE_AND_ONE_END
}

DIRECT (HELP_DTW_help) {
	HELP (U"DTW")
}

FORM (GRAPHICS_DTW_drawPath, U"DTW: Draw path", nullptr) {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawPath (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_DTW_drawDistancesAlongPath, U"DTW: Draw distances along path", nullptr) {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawDistancesAlongPath (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_DTW_paintDistances, U"DTW: Paint distances", nullptr) {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (minimum, U"Minimum", U"0.0")
	REALVAR (maximum, U"Maximum", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_paintDistances (me, GRAPHICS, xmin, xmax, ymin, ymax, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_DTW_drawWarpX, U"DTW: Draw warp (x)", U"DTW: Draw warp (x)...") {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (time, U"Time (s)", U"0.1")
	BOOLEANVAR (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawWarpX (me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_DTW_drawWarpY, U"DTW: Draw warp (y)", U"DTW: Draw warp (y)...") {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (time, U"Time (s)", U"0.1")
	BOOLEANVAR (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawWarpY (me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_EACH_END
}

DIRECT (REAL_DTW_getStartTimeX) {
	NUMBER_ONE (DTW)
		double result = my xmin;
	NUMBER_ONE_END (U" s (= start time along x)")
}

DIRECT (REAL_DTW_getEndTimeX) {
	NUMBER_ONE (DTW)
		double result = my xmax;
	NUMBER_ONE_END (U" s (= end time along x)");
}

DIRECT (REAL_DTW_getTotalDurationX) {
	NUMBER_ONE (DTW)
		double result = my xmax - my xmin;
	NUMBER_ONE_END (U" s (= total duration along x)");
}

DIRECT (REAL_DTW_getStartTimeY) {
	NUMBER_ONE (DTW)
		double result = my ymin;
	NUMBER_ONE_END (U" s (= start time along y)");
}

DIRECT (REAL_DTW_getEndTimeY) {
	NUMBER_ONE (DTW)
		double result = my ymax;
	NUMBER_ONE_END (U" s (= end time along y)");
}

DIRECT (REAL_DTW_getTotalDurationY) {
	NUMBER_ONE (DTW)
		double result = my ymax - my ymin;
	NUMBER_ONE_END (U" s (= total duration along y)")
}

DIRECT (INTEGER_DTW_getNumberOfFramesX) {
	INTEGER_ONE (DTW)
		long result = my nx; 
	INTEGER_ONE_END (U" (= number of frames along x)")
}


DIRECT (REAL_DTW_getTimeStepX) {
	NUMBER_ONE (DTW)
		double result = my dx;
	NUMBER_ONE_END (U" s (= time step along x)")
}

FORM (REAL_DTW_getTimeFromFrameNumberX, U"DTW: Get time from frame number (x)", nullptr) {
	NATURALVAR (frameNumber, U"Frame number (x)", U"1")
	OK
DO
	NUMBER_ONE (DTW)
		double result = Matrix_columnToX (me, frameNumber);
	NUMBER_ONE_END (U" s (= y time at x frame ", frameNumber, U")")
}

FORM (INTEGER_DTW_getFrameNumberFromTimeX, U"DTW: Get frame number from time (x)", nullptr) {
	REALVAR (xTime, U"Time along x (s)", U"0.1")
	OK
DO
	INTEGER_ONE (DTW)
		if (xTime < my xmin || xTime > my xmax) {
			Melder_throw (me, U"Time outside x domain.");
		}
		long result = lround (Matrix_xToColumn (me, xTime));
	INTEGER_ONE_END (U" (= x frame at y time ", xTime, U")")
}

DIRECT (INTEGER_DTW_getNumberOfFramesY) {
	INTEGER_ONE (DTW)
		long result = my ny;
	INTEGER_ONE_END (U" (= number of frames along y)")
}

DIRECT (REAL_DTW_getTimeStepY) {
	NUMBER_ONE (DTW)
		double result = my dy;
	NUMBER_ONE_END (U" s (= time step along y)")
}


FORM (REAL_DTW_getTimeFromFrameNumberY, U"DTW: Get time from frame number (y)", nullptr) {
	NATURALVAR (frameNumber, U"Frame number (y)", U"1")
	OK
DO
	NUMBER_ONE (DTW)
		double result = Matrix_rowToY (me, frameNumber);
	NUMBER_ONE_END (U" s (= x time at y frame ", frameNumber, U")")
}

FORM (INTEGER_DTW_getFrameNumberFromTimeY, U"DTW: Get frame number from time (y)", nullptr) {
	REALVAR (yTime, U"Time along y (s)", U"0.1")
	OK
DO
	INTEGER_ONE (DTW)
		if (yTime < my ymin || yTime > my ymax) {
			Melder_throw (me, U"Time outside y domain.");
		}
		long result = lround (Matrix_yToRow (me, yTime));
	INTEGER_ONE_END (U" (= y frame at x time ", yTime, U")")
}

FORM (REAL_DTW_getPathY, U"DTW: Get time along path", U"DTW: Get time along path...") {
	REALVAR (xTime, U"Time (s)", U"0.0")
	OK
DO
	NUMBER_ONE (DTW)
		double result = DTW_getYTimeFromXTime (me, xTime);
	NUMBER_ONE_END (U"")
}

FORM (REAL_DTW_getYTimeFromXTime, U"DTW: Get y time from x time", U"DTW: Get y time from x time...") {
	REALVAR (xTime, U"Time at x (s)", U"0.0")
	OK
DO
	NUMBER_ONE (DTW)
		double result = DTW_getYTimeFromXTime (me, xTime);
	NUMBER_ONE_END (U" s (= y time at x time ", xTime, U")")
}

FORM (REAL_DTW_getXTimeFromYTime, U"DTW: Get x time from y time", U"DTW: Get x time from y time...") {
	REALVAR (yTime, U"Time at y (s)", U"0.0")
	OK
DO
	NUMBER_ONE (DTW)
		double result = DTW_getXTimeFromYTime (me, yTime);
	NUMBER_ONE_END (U" s (= x time at y time ", yTime, U")")
}

FORM (INTEGER_DTW_getMaximumConsecutiveSteps, U"DTW: Get maximum consecutive steps", U"DTW: Get maximum consecutive steps...") {
	OPTIONMENUVAR (direction, U"Direction", 1)
		OPTION (U"X")
		OPTION (U"Y")
		OPTION (U"Diagonaal")
	OK
DO
	int direction_code [] = { DTW_START, DTW_X, DTW_Y, DTW_XANDY };
	const char32 *direction_string [] = { U"", U"x", U"y", U"diagonal" };
	INTEGER_ONE (DTW)
		long result = DTW_getMaximumConsecutiveSteps (me, direction_code [direction]);
	INTEGER_ONE_END (U" (= maximum number of consecutive steps in ", direction_string [direction], U" direction)")
}

DIRECT (REAL_DTW_getWeightedDistance) {
	NUMBER_ONE (DTW)
		double result = my weightedDistance;
	NUMBER_ONE_END (U" (weighted distance)")
}

FORM (REAL_DTW_getDistanceValue, U"DTW: Get distance value", nullptr) {
	REALVAR (xTime, U"Time at x (s)", U"0.1")
	REALVAR (yTime, U"Time at y (s)", U"0.1")
	OK
DO
	NUMBER_ONE (DTW)
		double result = undefined;
		if ((xTime >= my xmin && xTime <= my xmax) && (yTime >= my ymin && yTime <= my ymax)) {
			long irow = Matrix_yToNearestRow (me, yTime);
			long icol = Matrix_xToNearestColumn (me, xTime);
			result = my z[irow][icol];
		}
		NUMBER_ONE_END (U" (= distance at (", xTime, U", ", yTime, U"))")
}

DIRECT (REAL_DTW_getMinimumDistance) {
	NUMBER_ONE (DTW)
		double result, maximum;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & result, & maximum);
	NUMBER_ONE_END (U" (minimum)")
}

DIRECT (REAL_DTW_getMaximumDistance) {
	NUMBER_ONE (DTW)
		double minimum, result;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & result);
	NUMBER_ONE_END (U" (maximum)")
}

FORM (MODIFY_DTW_formulaDistances, U"DTW: Formula (distances)", nullptr) {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTVAR (formula, U"formula", U"self")
	OK
DO
	LOOP {
		iam (DTW);
		autoMatrix cp = DTW_to_Matrix_distances (me);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), formula, interpreter, 0);
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
END }

FORM (MODIFY_DTW_setDistanceValue, U"DTW: Set distance value", nullptr) {
	REALVAR (xTime, U"Time at x (s)", U"0.1")
	REALVAR (yTime, U"Time at y (s)", U"0.1")
	REALVAR (newDistance, U"New value", U"0.0")
	OK
DO
	if (newDistance < 0) {
		Melder_throw (U"Distances cannot be negative.");
	}
	MODIFY_EACH (DTW)
		if (xTime < my xmin || xTime > my xmax) {
			Melder_throw (U"Time at x outside domain.");
		}
		if (yTime < my ymin || yTime > my ymax) {
			Melder_throw (U"Time at y outside domain.");
		}
		long irow = Matrix_yToNearestRow (me, yTime);
		long icol = Matrix_xToNearestColumn (me, xTime);
		my z[irow][icol] = newDistance;
	MODIFY_EACH_END
}

FORM (MODIFY_DTW_findPath, U"DTW: Find path", nullptr) {
	DTW_constraints_addCommonFields(matchStart,matchEnd,slopeConstraint)
	OK
DO
	MODIFY_EACH (DTW)
		DTW_findPath (me, matchStart, matchEnd, slopeConstraint);
	MODIFY_EACH_END
}

FORM (MODIFY_DTW_findPath_bandAndSlope, U"DTW: find path (band & slope)", nullptr) {
    REALVAR (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.05")
    RADIOVAR (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    MODIFY_EACH (DTW)
        DTW_findPath_bandAndSlope (me, sakoeChibaBand, slopeConstraint, nullptr);
	MODIFY_EACH_END
}

FORM (NEW_DTW_to_Matrix_cumulativeDistances, U"DTW: To Matrix", nullptr) {
    REALVAR (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.05")
    RADIOVAR (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_EACH (DTW)
        autoMatrix result = DTW_to_Matrix_cumulativeDistances (me, sakoeChibaBand, slopeConstraint);
	CONVERT_EACH_END (my name, U"_cd")
}

FORM (NEW_DTW_to_Polygon, U"DTW: To Polygon...", nullptr) {
    REALVAR (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.1")
    RADIOVAR (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_EACH (DTW)
        autoPolygon result = DTW_to_Polygon (me, sakoeChibaBand, slopeConstraint);
    CONVERT_EACH_END (my name)
}

DIRECT (NEW_DTW_to_Matrix_distances) {
	CONVERT_EACH (DTW)
		autoMatrix result = DTW_to_Matrix_distances (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_DTW_swapAxes) {
	CONVERT_EACH (DTW)
		autoDTW result = DTW_swapAxes (me);
	CONVERT_EACH_END (my name, U"_axesSwapped")
}

DIRECT (MODIFY_DTW_and_Matrix_replace) {
	MODIFY_FIRST_OF_TWO (DTW, Matrix)
		DTW_and_Matrix_replace (me, you);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (NEW1_DTW_and_TextGrid_to_TextGrid) {
	CONVERT_TWO (DTW, TextGrid)
		autoTextGrid result = DTW_and_TextGrid_to_TextGrid (me, you, 0);
	CONVERT_TWO_END (your name, U"_", my name)
}

DIRECT (NEW1_DTW_and_IntervalTier_to_Table) {
	CONVERT_TWO (DTW, IntervalTier)
		autoTable result = DTW_and_IntervalTier_to_Table (me, you, 1.0/44100);
	CONVERT_TWO_END (my name)
}

/******************** EditDistanceTable & EditCostsTable ********************************************/

DIRECT (HELP_EditDistanceTable_help) {
	HELP (U"EditDistanceTable")
}

DIRECT (NEW_EditDistanceTable_to_TableOfReal_directions) {
	CONVERT_EACH (EditDistanceTable)
		autoTableOfReal result = EditDistanceTable_to_TableOfReal_directions (me);
	CONVERT_EACH_END (my name);
}

DIRECT (MODIFY_EditDistanceTable_setEditCosts) {
	MODIFY_FIRST_OF_TWO (EditDistanceTable, EditCostsTable)
		EditDistanceTable_setEditCosts (me, you);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_EditDistanceTable_setDefaultCosts, U"", nullptr) {
	REALVAR (insertionCosts, U"Insertion costs", U"1.0")
	REALVAR (deletionCosts, U"Deletion costs", U"1.0")
	REALVAR (substitutionCosts, U"Substitution costs", U"2.0")
	OK
DO
	if (insertionCosts < 0) {
		Melder_throw (U"Insertion costs cannot be negative.");
	}
	if (deletionCosts < 0) {
		Melder_throw (U"Deletion costs cannot be negative.");
	}
	if (substitutionCosts < 0) {
		Melder_throw (U"Substitution costs cannot be negative.");
	}
	MODIFY_EACH (EditDistanceTable)
		EditDistanceTable_setDefaultCosts (me, insertionCosts, deletionCosts, substitutionCosts);
	MODIFY_EACH_END
}

FORM (GRAPHICS_EditDistanceTable_draw, U"EditDistanceTable_draw", nullptr) {
	RADIOVAR (format, U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURALVAR (precision, U"Precision", U"1")
	REALVAR (angle, U"Rotate source labels by (degrees)", U"0.0")
	OK
DO
	GRAPHICS_EACH (EditDistanceTable)
		EditDistanceTable_draw (me, GRAPHICS, format, precision, angle);
	GRAPHICS_EACH_END
}

DIRECT (GRAPHICS_EditDistanceTable_drawEditOperations) {
	GRAPHICS_EACH (EditDistanceTable)
		EditDistanceTable_drawEditOperations (me, GRAPHICS);
	GRAPHICS_EACH_END
}

DIRECT (HELP_EditCostsTable_help) {
	HELP (U"EditCostsTable")
}

FORM (INTEGER_EditCostsTable_getTargetIndex, U"EditCostsTable: Get target index", nullptr) {
	SENTENCEVAR (target, U"Target", U"")
	OK
DO
	INTEGER_ONE (EditCostsTable)
		long result = EditCostsTable_getTargetIndex (me, target);
	INTEGER_ONE_END (U" (target index)")
}

FORM (INTEGER_EditCostsTable_getSourceIndex, U"EditCostsTable: Get source index", nullptr) {
	SENTENCEVAR (source, U"Source", U"")
	OK
DO
	INTEGER_ONE (EditCostsTable)
		long result = EditCostsTable_getSourceIndex (me, source);
	INTEGER_ONE_END (U" (source index)")
}

FORM (REAL_EditCostsTable_getInsertionCost, U"EditCostsTable: Get insertion cost", nullptr) {
	SENTENCEVAR (target, U"Target", U"")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getInsertionCost (me, target);
	NUMBER_ONE_END (U" (insertion cost)")
}

FORM (REAL_EditCostsTable_getDeletionCost, U"EditCostsTable: Get deletion cost", nullptr) {
	SENTENCEVAR (source, U"Source", U"")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getDeletionCost (me, source);
	NUMBER_ONE_END (U" (deletion cost)")
}

FORM (REAL_EditCostsTable_getSubstitutionCost, U"EditCostsTable: Get substitution cost", nullptr) {
	SENTENCEVAR (target, U"Target", U"")
	SENTENCEVAR (source, U"Source", U"")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getSubstitutionCost (me, target, source);
	NUMBER_ONE_END (U" (substitution cost)")
}

FORM (REAL_EditCostsTable_getOthersCost, U"EditCostsTable: Get cost (others)", nullptr) {
	RADIOVAR (costTypes, U"Others cost type", 1)
		RADIOBUTTON (U"Insertion")
		RADIOBUTTON (U"Deletion")
		RADIOBUTTON (U"Equality")
		RADIOBUTTON (U"Inequality")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getOthersCost (me,costTypes);
	NUMBER_ONE_END (U" (cost)")
}

FORM (MODIFY_EditCostsTable_setTargetSymbol_index, U"EditCostsTable: Set target symbol (index)", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	SENTENCEVAR (target, U"Target", U"a")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		TableOfReal_setRowLabel (me, index, target);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setSourceSymbol_index, U"EditCostsTable: Set source symbol (index)", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	SENTENCEVAR (source, U"Source", U"a")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		TableOfReal_setColumnLabel (me, index, source);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setInsertionCosts, U"EditCostsTable: Set insertion costs", nullptr) {
	SENTENCEVAR (targets, U"Targets", U"")
	REALVAR (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setInsertionCosts (me, targets, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setDeletionCosts, U"EditCostsTable: Set deletion costs", nullptr) {
	SENTENCEVAR (sources, U"Sources", U"")
	REALVAR (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setDeletionCosts (me, sources, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setSubstitutionCosts, U"EditCostsTable: Set substitution costs", nullptr) {
	SENTENCEVAR (targets, U"Targets", U"a i u")
	SENTENCEVAR (sources, U"Sources", U"a i u")
	REALVAR (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setSubstitutionCosts (me, targets, sources, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setOthersCosts, U"EditCostsTable: Set costs (others)", nullptr) {
	LABEL (U"", U"Others costs")
	REALVAR (insertionCosts, U"Insertion", U"1.0")
	REALVAR (deletionCosts, U"Deletion", U"1.0")
	LABEL ( U"", U"Substitution costs")
	REALVAR (equalityCosts, U"Equality", U"0.0")
	REALVAR (inequalityCosts, U"Inequality", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setOthersCosts (me, insertionCosts, deletionCosts, equalityCosts, inequalityCosts);
	MODIFY_EACH_END
}

DIRECT (NEW_EditCostsTable_to_TableOfReal) {
	CONVERT_EACH (EditCostsTable)
		autoTableOfReal result = EditCostsTable_to_TableOfReal (me);
	CONVERT_EACH_END (my name);
}

FORM (NEW_EditCostsTable_createEmpty, U"Create empty EditCostsTable", U"Create empty EditCostsTable...") {
	SENTENCEVAR (name, U"Name", U"editCosts")
	INTEGERVAR (numberOfTargetSymbols, U"Number of target symbols", U"0")
	INTEGERVAR (numberOfSourceSymbols, U"Number of source symbols", U"0")
	OK
DO
	CREATE_ONE
		numberOfTargetSymbols = numberOfTargetSymbols < 0 ? 0 : numberOfTargetSymbols;
		numberOfSourceSymbols = numberOfSourceSymbols < 0 ? 0 : numberOfSourceSymbols;
		autoEditCostsTable result = EditCostsTable_create (numberOfTargetSymbols, numberOfSourceSymbols);
	CREATE_ONE_END (name)
}

/******************** Eigen ********************************************/

DIRECT (HELP_Eigen_help) {
	HELP (U"Eigen")
}

DIRECT (GRAPHICS_Eigen_drawEigenvalues_scree) {
	Melder_warning (U"The command \"Draw eigenvalues (scree)...\" has been "
		"removed.\n To get a scree plot use \"Draw eigenvalues...\" with the "
		"arguments\n 'Fraction of eigenvalues summed' and 'Cumulative' unchecked.");
	END 	
}

FORM (GRAPHICS_Eigen_drawEigenvalues, U"Eigen: Draw eigenvalues", U"Eigen: Draw eigenvalues...") {
	INTEGERVAR (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGERVAR (toEigenvalue, U"right Eigenvalue range", U"0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (fractionSummed, U"Fraction of eigenvalues summed", false)
	BOOLEANVAR (cumulative, U"Cumulative", false)
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Eigen)
		Eigen_drawEigenvalues (me, GRAPHICS, fromEigenvalue, toEigenvalue, fromAmplitude, toAmplitude, fractionSummed, cumulative, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Eigen_drawEigenvector, U"Eigen: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	INTEGERVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	BOOLEANVAR (loadings, U"Component loadings", false)
	INTEGERVAR (fromElement, U"left Element range", U"0")
	INTEGERVAR (toElement, U"right Element range", U"0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"-1.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (connectPoints, U"Connect points", true)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Eigen)
		Eigen_drawEigenvector (me, GRAPHICS, eigenvectorNumber, fromElement, toElement, fromAmplitude, toAmplitude, loadings, markSize_mm, mark_string, connectPoints, nullptr,  garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_Eigen_getNumberOfEigenvalues) {
	INTEGER_ONE (Eigen)
		long result = my numberOfEigenvalues;
	INTEGER_ONE_END (U" (number of eigenvalues/vectors)")
}

DIRECT (INTEGER_Eigen_getDimension) {
	INTEGER_ONE (Eigen)
		long result = my dimension;
	INTEGER_ONE_END (U" (dimension)")
}

FORM (REAL_Eigen_getEigenvalue, U"Eigen: Get eigenvalue", U"Eigen: Get eigenvalue...") {
	NATURALVAR (eigenvalueNumber, U"Eigenvalue number", U"1")
	OK
DO
	NUMBER_ONE (Eigen)
		double result = undefined;
		if (eigenvalueNumber > 0 && eigenvalueNumber <= my numberOfEigenvalues) {
			result = my eigenvalues [eigenvalueNumber];
		}
	NUMBER_ONE_END (U" (eigenvalue [", eigenvalueNumber, U"])")
}

FORM (REAL_Eigen_getSumOfEigenvalues, U"Eigen:Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...") {
	INTEGERVAR (fromEigenvalue, U"left Eigenvalue range",  U"0")
	INTEGERVAR (toEigenvalue, U"right Eigenvalue range", U"0")
	OK
DO
	NUMBER_ONE (Eigen)
		double result = Eigen_getSumOfEigenvalues (me, fromEigenvalue, toEigenvalue);
	NUMBER_ONE_END (U" (sum of eigenvalues [", fromEigenvalue, U"..", toEigenvalue, U"])")
}

FORM (REAL_Eigen_getEigenvectorElement, U"Eigen: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURALVAR (elementNumber, U"Element number", U"1")
	OK
DO
	NUMBER_ONE (Eigen)
		double result = Eigen_getEigenvectorElement (me, eigenvectorNumber, elementNumber);
	NUMBER_ONE_END (U" (eigenvector [", eigenvectorNumber, U"] element [", elementNumber, U"])")
}

FORM (MODIFY_Eigen_invertEigenvector, U"Eigen: Invert eigenvector", nullptr) {
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Eigen)
		Eigen_invertEigenvector (me, eigenvectorNumber);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Eigens_alignEigenvectors) {
	FIND_LIST (Eigen)
		Eigens_alignEigenvectors (& list);
	END
}

FORM (NEW1_Eigen_and_Matrix_projectColumns, U"Eigen & Matrix: Project columns", U"Eigen & Matrix: Project...") {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0")
	OK
DO
	CONVERT_TWO (Eigen, Matrix)
		autoMatrix result = Eigen_and_Matrix_to_Matrix_projectColumns (me, you, GET_INTEGER (U"Number of dimensions"));
	CONVERT_TWO_END (my name, U"_", your name)
}

DIRECT (NEW1_Eigen_and_SSCP_project) {
	CONVERT_TWO (Eigen, SSCP)
		autoSSCP result = Eigen_and_SSCP_project (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

DIRECT (NEW1_Eigen_and_Covariance_project) {
	CONVERT_TWO (Eigen, Covariance)
		autoCovariance result = Eigen_and_Covariance_project (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

/******************** Index ********************************************/

DIRECT (HELP_Index_help) {
	HELP (U"Index")
}

DIRECT (INTEGER_Index_getNumberOfClasses) {
	INTEGER_ONE (Index)
		long result = my classes -> size;
	INTEGER_ONE_END (U" (number of classes)")
}

FORM (INFO_StringsIndex_getClassLabelFromClassIndex, U"StringsIndex: Get class label", U"StringsIndex: Get class label...") {
	NATURALVAR (index, U"Class index", U"1")
	OK
DO
	STRING_ONE (StringsIndex)
		const char32* result = StringsIndex_getClassLabelFromClassIndex (me, index);
	STRING_ONE_END
}

FORM (INFO_StringsIndex_getItemLabelFromItemIndex, U"StringsIndex: Get item label", nullptr) {
	NATURALVAR (itemIndex, U"Item index", U"1")
	OK
DO
	STRING_ONE (StringsIndex)
		const char32 *result = StringsIndex_getItemLabelFromItemIndex (me, itemIndex);
	STRING_ONE_END
}

FORM (INTEGER_Index_getClassIndexFromItemIndex, U"Index: Get item index", nullptr) {
	NATURALVAR (itemIndex, U"Item index", U"1")
	OK
DO
	INTEGER_ONE (Index)
		long result = Index_getClassIndexFromItemIndex (me, itemIndex);
	INTEGER_ONE_END (U" (class index)")
}

FORM (INTEGER_StringsIndex_getClassIndexFromClassLabel, U"StringsIndex: Get class index from calss label", nullptr) {
	WORDVAR (klasLabel, U"Class label", U"label")
	OK
DO
	INTEGER_ONE (StringsIndex)
		long result = StringsIndex_getClassIndexFromClassLabel (me, klasLabel);
	INTEGER_ONE_END (U" (class index)")
}

FORM (NEW_Index_extractPart, U"Index: Extract part", U"Index: Extract part...") {
	INTEGERVAR (fromItem, U"left Item range", U"0")
	INTEGERVAR (toItem, U"right Item range", U"0")
	OK
DO
	CONVERT_EACH (Index);
		autoIndex result = Index_extractPart (me, fromItem, toItem);
	CONVERT_EACH_END (my name, U"_part")
}

FORM (NEW_Index_to_Permutation, U"Index: To Permutation", U"Index: To Permutation...") {
	BOOLEANVAR (permuteWithinClasses, U"Permute within classes", true)
	OK
DO
	CONVERT_EACH (Index);
		autoPermutation result = Index_to_Permutation_permuteRandomly (me, permuteWithinClasses);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_StringsIndex_to_Strings) {
	CONVERT_EACH (StringsIndex)
		autoStrings result = StringsIndex_to_Strings (me);
	CONVERT_EACH_END (my name)
}

/******************** Excitation ********************************************/

DIRECT (NEW1_Excitations_to_ExcitationList) {
	CONVERT_LIST (Excitation)
		autoExcitationList result = Excitations_to_ExcitationList (& list);
	CONVERT_LIST_END (U"appended")
}

/******************** ExcitationList ********************************************/

FORM (MODIFY_ExcitationList_formula, U"ExcitationList: Formula", nullptr) {
	LABEL (U"label", U"for all objects in ExcitationList do { for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	TEXTVAR (formula, U"formula", U"self")
	OK
DO
	MODIFY_EACH (ExcitationList)
		for (long j = 1; j <= my size; j ++) {
			Matrix_formula (my at [j], formula, interpreter, nullptr);
		}
	MODIFY_EACH_END
}

DIRECT (MODIFY_ExcitationList_addItem) {
	MODIFY_FIRST_OF_ONE_AND_LIST (ExcitationList, Excitation)
		ExcitationList_addItems (me, & list);
	MODIFY_FIRST_OF_ONE_AND_LIST_END
}

FORM (NEW_ExcitationList_extractItem, U"ExcitationList: Extract item", nullptr) {
	NATURALVAR (itemIndex, U"Item index", U"1")
	OK
DO
	CONVERT_EACH (ExcitationList)
		autoExcitation result = ExcitationList_extractItem (me, itemIndex);
	CONVERT_EACH_END (my name, U"_", itemIndex)
}

DIRECT (NEW1_ExcitationList_append) {
	CONVERT_COUPLE (ExcitationList)
		autoExcitationList result = Data_copy (me);
		result -> merge (you);
	CONVERT_COUPLE_END (U"appended")
}

FORM (NEW_ExcitationList_to_PatternList, U"Excitations: To PatternList", nullptr) {
	NATURAL (U"Join", U"1")
	OK
DO
	CONVERT_EACH (ExcitationList)
		autoPatternList result = ExcitationList_to_PatternList (me, GET_INTEGER (U"Join"));
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_ExcitationList_to_TableOfReal) {
	CONVERT_EACH (ExcitationList)
		autoTableOfReal result = ExcitationList_to_TableOfReal (me);
	CONVERT_EACH_END (my name)
}


/************************* FileInMemory ***********************************/


FORM_READ (READ1_FileInMemory_create, U"Create file in memory", nullptr, true) {
	autoFileInMemory me = FileInMemory_create (file);
	praat_new (me.move(), MelderFile_name (file));
END }

FORM (MODIFY_FileInMemory_setId, U"FileInMemory: Set id", nullptr) {
	SENTENCEVAR (newId, U"New id", U"New id")
	OK
DO
	MODIFY_EACH (FileInMemory)
		FileInMemory_setId (me, newId);
	MODIFY_EACH_END
}

FORM (INFO_FileInMemory_showAsCode, U"FileInMemory: Show as code", nullptr) {
	WORDVAR (name, U"Name", U"example")
	INTEGERVAR (numberOfByterPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemory)
		MelderInfo_open ();
		FileInMemory_showAsCode (me, name, numberOfByterPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

/************************* FileInMemorySet ***********************************/

FORM (NEW_FileInMemorySet_createFromDirectoryContents, U"Create files in memory from directory contents", nullptr) {
	SENTENCEVAR (name, U"Name", U"list")
	LABEL (U"", U"Directory:")
	TEXTVAR (directory, U"Directory", U"/home/david/praat/src/espeak-work/espeak-1.46.13/espeak-data")
	WORDVAR (fileGlobber, U"Only files that match pattern", U"*.txt")
	OK
DO
	CREATE_ONE
		autoFileInMemorySet result = FileInMemorySet_createFromDirectoryContents (directory, fileGlobber);
	CREATE_ONE_END (name)
}

FORM (NEW_FileInMemorySet_createCopyFromFileInMemorySet, U"", nullptr) {
	OPTIONMENUVAR (whichFile, U"Espeakdata", 5)
		OPTION (U"phons")
		OPTION (U"dicts")
		OPTION (U"voices")
		OPTION (U"variants")
		OPTION (U"voices_names")
		OPTION (U"variants_names")
	OK
DO
	CREATE_ONE
		autoDaata result;
		const char32 *name;
		if (whichFile == 1) {
			result = Data_copy (espeakdata_phons.get());
			name = U"espeakdata_phons";
		}
		else if (whichFile == 2) {
			result = Data_copy (espeakdata_dicts.get());
			name = U"espeakdata_dicts";
		}
		else if (whichFile == 3) {
			result = Data_copy (espeakdata_voices.get());
			name = U"espeakdata_voices";
		}
		else if (whichFile == 4) {
			result = Data_copy (espeakdata_variants.get());
			name = U"espeakdata_variants";
		}
		else if (whichFile == 5) {
			result = Data_copy (espeakdata_voices_names.get());
			name = U"espeakdata_voices_names";
		}
		else if (whichFile == 6) {
			result = Data_copy (espeakdata_variants_names.get());
			name =  U"espeakdata_variants_names";
		} else {
			name = U"";
		}
	CREATE_ONE_END (name)
}

FORM (INFO_FileInMemorySet_showAsCode, U"FileInMemorySet: Show as code", nullptr) {
	WORDVAR (name, U"Name", U"example")
	INTEGER (U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemorySet)
		MelderInfo_open ();
		FileInMemorySet_showAsCode (me, name, GET_INTEGER (U"Number of bytes per line"));
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_FileInMemorySet_showOneFileAsCode, U"FileInMemorySet: Show one file as code", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	WORDVAR (name, U"Name", U"example")
	INTEGER (U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemorySet)
		MelderInfo_open ();
		FileInMemorySet_showOneFileAsCode (me, index, name, GET_INTEGER (U"Number of bytes per line"));
		MelderInfo_close ();
	INFO_ONE_END
}

DIRECT (NEW1_FileInMemory_to_FileInMemorySet) {
	autoFileInMemorySet thee = FileInMemorySet_create ();
	LOOP {
		iam (FileInMemory);
		autoFileInMemory him = Data_copy (me);
		thy addItem_move (him.move());
	}
	praat_new (thee.move(), U"files");
END }

DIRECT (MODIFY_FileInMemorySet_addItems) {
	MODIFY_FIRST_OF_TWO (FileInMemorySet, FileInMemory)
		autoFileInMemory him = Data_copy (you);
		my addItem_move (him.move());
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (NEW1_FileInMemorySets_merge) {
	CONVERT_COUPLE (FileInMemorySet)
		autoFileInMemorySet result = Data_copy (me);
		result -> merge (you);
	CONVERT_COUPLE_END (my name, U"_", your name);
}

DIRECT (NEW_FileInMemorySet_to_Strings_id) {
	CONVERT_EACH (FileInMemorySet)
		autoStrings result = FileInMemorySet_to_Strings_id (me);
	CONVERT_EACH_END (my name);
}

/************************* FilterBank ***********************************/

FORM (GRAPHICS_FilterBank_drawFilters, U"FilterBank: Draw filters", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawRows (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_drawOneContour, U"FilterBank: Draw one contour", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (height, U"Height (dB)", U"40.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawOneContour (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, height);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_drawContours, U"FilterBank: Draw contours", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawContours (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_drawFrequencyScales, U"FilterBank: Draw frequency scales", U"FilterBank: Draw frequency scales...") {
	RADIOVAR (xFrequencyScale, U"Horizontal frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REALVAR (xFromFrequency, U"left Horizontal frequency range", U"0.0")
	REALVAR (xToFrequency, U"right Horizontal frequency range", U"0.0")
	RADIOVAR (yFrequencyScale, U"Vertical frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REALVAR (yFromFrequency, U"left Vertical frequency range", U"0.0")
	REALVAR (yToFrequency, U"right Vertical frequency range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawFrequencyScales (me, GRAPHICS, xFrequencyScale, xFromFrequency, xToFrequency, yFrequencyScale, yFromFrequency, yToFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelSpectrogram_paintImage, U"MelSpectrogram: Paint image", U"MelSpectrogram: Paint image...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (mel)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkSpectrogram_paintImage, U"BarkSpectrogram: Paint image", U"BarkSpectrogram: Paint image...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (bark)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_paintImage, U"FilterBank: Paint image", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_paintContours, U"FilterBank: Paint contours", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintContours (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_FilterBank_paintCells, U"FilterBank: Paint cells", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintCells (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_paintSurface, U"FilterBank: Paint surface", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintSurface (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude,30, 45);
	GRAPHICS_EACH_END
}

FORM (REAL_FilterBank_getFrequencyInHertz, U"FilterBank: Get frequency in Hertz", U"FilterBank: Get frequency in Hertz...") {
	REALVAR (frequency, U"Frequency", U"10.0")
	RADIOVAR (frequencyUnit, U"Unit", 2)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = FilterBank_getFrequencyInHertz (me, frequency, frequencyUnit);
	NUMBER_ONE_END (U" hertz")
}

FORM (REAL_FilterBank_getFrequencyInBark, U"FilterBank: Get frequency in Bark", U"FilterBank: Get frequency in Bark...") {
	REALVAR (frequency, U"Frequency", U"93.17")
	RADIOVAR (frequencyUnit, U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = FilterBank_getFrequencyInBark (me, frequency, frequencyUnit);
	NUMBER_ONE_END (U" bark")
}

FORM (REAL_FilterBank_getFrequencyInMel, U"FilterBank: Get frequency in mel", U"FilterBank: Get frequency in mel...") {
	REALVAR (frequency, U"Frequency", U"1000.0")
	RADIOVAR (frequencyUnit, U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = FilterBank_getFrequencyInMel (me, frequency, frequencyUnit);
	NUMBER_ONE_END (U" mel")
}

FORM (MODIFY_FilterBank_equalizeIntensities, U"FilterBank: Equalize intensities", nullptr) {
	REALVAR (intensity_dB, U"Intensity (dB)", U"80.0")
	OK
DO
	MODIFY_EACH (FilterBank)
		FilterBank_equalizeIntensities (me, intensity_dB);
	MODIFY_EACH_END
}

FORM (MODIFY_BandFilterSpectrogram_equalizeIntensities, U"BandFilterSpectrogram: Equalize intensities", nullptr) {
	REALVAR (intensity_dB, U"Intensity (dB)", U"80.0")
	OK
DO
	MODIFY_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_equalizeIntensities (me, intensity_dB);
	MODIFY_EACH_END
}

DIRECT (NEW_FilterBank_to_Matrix) {
	CONVERT_EACH (FilterBank)
		autoMatrix result = FilterBank_to_Matrix (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_BandFilterSpectrogram_to_Matrix, U"BandFilterSpectrogram: To Matrix", nullptr) {
	BOOLEANVAR (convertToDB, U"Convert to dB values", 1)
	OK
DO
	CONVERT_EACH (BandFilterSpectrogram)
		autoMatrix result = BandFilterSpectrogram_to_Matrix (me, convertToDB);
	CONVERT_EACH_END (my name)
}

FORM (NEW1_FilterBanks_crossCorrelate, U"FilterBanks: Cross-correlate", nullptr) {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	CONVERT_COUPLE (FilterBank)
		autoSound result = FilterBanks_crossCorrelate (me, you,
			GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
			GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW1_BandFilterSpectrograms_crossCorrelate, U"BandFilterSpectrograms: Cross-correlate", nullptr) {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	CONVERT_COUPLE (BandFilterSpectrogram)
		autoSound result = BandFilterSpectrograms_crossCorrelate (me, you,
			GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
			GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW1_FilterBanks_convolve, U"FilterBanks: Convolve", nullptr) {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	CONVERT_COUPLE (FilterBank)
		autoSound result = FilterBanks_convolve (me, you,
			GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
			GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW1_BandFilterSpectrograms_convolve, U"BandFilterSpectrograms: Convolve", nullptr) {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	CONVERT_COUPLE (BandFilterSpectrogram)
		autoSound result = BandFilterSpectrograms_convolve (me, you,
			GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
			GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	CONVERT_COUPLE_END (my name, U"_", your name)
}

DIRECT (NEW_FilterBank_to_Intensity) {
	CONVERT_EACH (FilterBank)
		autoIntensity result = FilterBank_to_Intensity (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_BandFilterSpectrogram_to_Intensity) {
	CONVERT_EACH (BandFilterSpectrogram)
		autoIntensity result = BandFilterSpectrogram_to_Intensity (me);
	CONVERT_EACH_END (my name)
}

/*********** FormantFilter *******************************************/

DIRECT (HELP_FormantFilter_help) {
	HELP (U"FormantFilter")
}

FORM (GRAPHICS_FormantFilter_drawFilterFunctions, U"FormantFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGERVAR (fromFilter, U"left Filter range", U"0")
	INTEGERVAR (toFilter, U"right Filter range", U"0")
	POSITIVEVAR (bandwidth, U"Bandwidth (Hz)", U"100.0")
	RADIOVAR (frequencyScale, U"Frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	BOOLEANVAR (dBScale, U"Amplitude scale in dB", 1)
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantFilter)
		FormantFilter_drawFilterFunctions (me, GRAPHICS, bandwidth, frequencyScale, fromFilter, toFilter, fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantFilter_drawSpectrum, U"FormantFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REALVAR (time, U"Time (s)", U"0.1")
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Hz", garnish);
	GRAPHICS_EACH_END
}

/****************** FormantGrid  *********************************/

FORM (GRAPHICS_old_FormantGrid_draw, U"FormantGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0 (= auto)")
	BOOLEANVAR (bandwidths, U"Bandwidths", false)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantGrid)
		FormantGrid_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, bandwidths, garnish, U"lines and speckles");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantGrid_draw, U"FormantGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0 (= auto)")
	BOOLEANVAR (bandwidths, U"Bandwidths", false)
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENUSTRVAR (drawingMethod, U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_FormantGrid_draw)
	GRAPHICS_EACH (FormantGrid)
		FormantGrid_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, bandwidths, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

/****************** FunctionTerms  *********************************/

FORM (GRAPHICS_FunctionTerms_draw, U"FunctionTerms: Draw", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (extrapolate, U"Extrapolate", false)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FunctionTerms)
		FunctionTerms_draw (me, GRAPHICS, xmin, xmax, ymin, ymax,extrapolate, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FunctionTerms_drawBasisFunction, U"FunctionTerms: Draw basis function", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (extrapolate, U"Extrapolate", false)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FunctionTerms)
		FunctionTerms_drawBasisFunction (me, GRAPHICS, index, xmin, xmax, ymin, ymax, extrapolate, garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_FunctionTerms_evaluate, U"FunctionTerms: Evaluate", nullptr) {
	REALVAR (x, U"X", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_evaluate (me, x);
	NUMBER_ONE_END (U"")
}

DIRECT (INTEGER_FunctionTerms_getNumberOfCoefficients) {
	INTEGER_ONE (FunctionTerms)
		long result = my numberOfCoefficients;
	INTEGER_ONE_END (U"")
}

FORM (REAL_FunctionTerms_getCoefficient, U"FunctionTerms: Get coefficient", nullptr) {
	LABEL (U"", U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = ( index > 0 && index <= my numberOfCoefficients ? my coefficients [index] : undefined );
	NUMBER_ONE_END (U"")
}

DIRECT (INTEGER_FunctionTerms_getDegree) {
	INTEGER_ONE (FunctionTerms)
		long result = FunctionTerms_getDegree (me);
	INTEGER_ONE_END (U"")
}

FORM (REAL_FunctionTerms_getMaximum, U"FunctionTerms: Get maximum", U"Polynomial: Get maximum...") {
	LABEL (U"", U"Interval")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getMaximum (me, xmin, xmax);
	NUMBER_ONE_END (U" (maximum)")
}

FORM (REAL_FunctionTerms_getMinimum, U"FunctionTerms: Get minimum", U"Polynomial: Get minimum...") {
	LABEL (U"", U"Interval")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getMinimum (me, xmin, xmax);
	NUMBER_ONE_END (U" (minimum)")
}

FORM (REAL_FunctionTerms_getXOfMaximum, U"FunctionTerms: Get x of maximum", U"Polynomial: Get x of maximum...") {
	LABEL (U"", U"Interval")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getXOfMaximum (me, xmin, xmax);
	NUMBER_ONE_END (U" (x of maximum)")
}

FORM (REAL_FunctionTerms_getXOfMinimum, U"FunctionTerms: Get x of minimum", U"Polynomial: Get x of minimum...") {
	LABEL (U"", U"Interval")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getXOfMinimum (me, xmin, xmax);
	NUMBER_ONE_END (U" (x of minimum)")
}

FORM (MODIFY_FunctionTerms_setCoefficient, U"FunctionTerms: Set coefficient", nullptr) {
	LABEL (U"", U"p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	LABEL (U"", U"F[k] is of degree k")
	NATURALVAR (index, U"Index", U"1")
	REALVAR (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FunctionTerms)
		FunctionTerms_setCoefficient (me, index, value);
	MODIFY_EACH_END
}

FORM (MODIFY_FunctionTerms_setDomain, U"FunctionTerms: Set domain", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"2.0")
	OK
DO
	MODIFY_EACH (FunctionTerms)
		if (xmax <= xmin) {
			Melder_throw (U"Xmax should be larger than Xmin.");
		}
		FunctionTerms_setDomain (me, xmin, xmax);
	MODIFY_EACH_END
}

/***************** Intensity ***************************************************/

FORM (NEW_Intensity_to_TextGrid_detectSilences, U"Intensity: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...") {
	REALVAR (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVEVAR (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVEVAR (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	WORDVAR (silenceLabel, U"Silent interval label", U"silent")
	WORDVAR (soundingLabel, U"Sounding interval label", U"sounding")
	OK
DO
	CONVERT_EACH (Intensity)
		autoTextGrid result = Intensity_to_TextGrid_detectSilences (me, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel);
	CONVERT_EACH_END (my name)
}

/***************** IntensityTier ***************************************************/

FORM (NEW_IntensityTier_to_TextGrid_detectSilences, U"IntensityTier: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...") {
	REALVAR (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVEVAR (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVEVAR (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	WORDVAR (silenceLabel, U"Silent interval label", U"silent")
	WORDVAR (soundingLabel, U"Sounding interval label", U"sounding")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.001")
	OK
DO
	CONVERT_EACH (IntensityTier)
		autoTextGrid result = IntensityTier_to_TextGrid_detectSilences (me, timeStep, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel);
	CONVERT_EACH_END (my name)
}

FORM (NEW_IntensityTier_to_Intensity, U"", nullptr) {
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.001")
	OK
DO
	CONVERT_EACH (IntensityTier)
		autoIntensity result = IntensityTier_to_Intensity (me, timeStep);
	CONVERT_EACH_END (my name)
}

/***************** ISpline ***************************************************/

DIRECT (HELP_ISpline_help) {
	HELP (U"ISpline")
}

FORM (NEW1_ISpline_create, U"Create ISpline", U"Create ISpline...") {
	WORDVAR (name, U"Name", U"ispline")
	LABEL (U"", U"Domain")
	REALVAR (xmin, U"Xmin", U"0")
	REALVAR (xmax, U"Xmax", U"1")
	LABEL (U"", U"ISpline(x) = c[1] I[1](x) + c[2] I[1](x) + ... c[n] I[n](x)")
	LABEL (U"", U"all I[k] are polynomials of degree \"Degree\"")
	LABEL (U"", U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree")
	INTEGERVAR (degree, U"Degree", U"3")
	SENTENCEVAR (coefficients_string, U"Coefficients (c[k])", U"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCEVAR (knots_string, U"Interior knots" , U"0.3 0.5 0.6")
	OK
DO
	CREATE_ONE
		if (xmax <= xmin) {
			Melder_throw (U"Xmin should be smaller than Xmax.");
		}
		autoISpline result = ISpline_createFromStrings (xmin, xmax, degree, coefficients_string, knots_string);
	CREATE_ONE_END (name)
}

/******************* KlattTable  *********************************/

DIRECT (HELP_KlattTable_help) {
	HELP (U"KlattTable")
}

DIRECT (NEW1_KlattTable_createExample) {
	CREATE_ONE
		autoKlattTable result = KlattTable_createExample ();
	CREATE_ONE_END (U"example")
}

FORM (NEW_KlattTable_to_Sound, U"KlattTable: To Sound", U"KlattTable: To Sound...") {
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"16000")
	RADIOVAR (synthesisModel, U"Synthesis model", 1)
		RADIOBUTTON (U"Cascade")
		RADIOBUTTON (U"Parallel")
	NATURALVAR (numberOfFormants, U"Number of formants", U"5")
	POSITIVEVAR (frameDuration, U"Frame duration (s)", U"0.005")
	REALVAR (flutter_percentage, U"Flutter percentage (%)", U"0.0")   // ppgb: foutgevoelig
	OPTIONMENUVAR (voicingSource, U"Voicing source", 1)
		OPTION (U"Impulsive")
		OPTION (U"Natural")
	OPTIONMENUVAR (soundOutputType, U"Output type", 1)
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
	if (flutter_percentage < 0.0 || flutter_percentage > 100.0) {
		Melder_throw (U"Flutter should be between 0 and 100%.");
	}
	CONVERT_EACH (KlattTable)
		autoSound result = KlattTable_to_Sound (me, samplingFrequency, synthesisModel, numberOfFormants, frameDuration, voicingSource, flutter_percentage, soundOutputType);
	CONVERT_EACH_END (my name)
}

FORM (NEW_KlattTable_to_KlattGrid, U"KlattTable: To KlattGrid", nullptr) {
	POSITIVEVAR (frameDuration, U"Frame duration (s)", U"0.002")
	OK
DO
	CONVERT_EACH (KlattTable)
		autoKlattGrid result = KlattTable_to_KlattGrid (me, frameDuration);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_KlattTable_to_Table) {
	CONVERT_EACH (KlattTable)
		autoTable result = KlattTable_to_Table (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Table_to_KlattTable) {
	CONVERT_EACH (Table)
		autoKlattTable result = Table_to_KlattTable (me);
	CONVERT_EACH_END (my name)
}

FORM (REAL_Table_getMedianAbsoluteDeviation, U"Table: Get median absolute deviation", U"Table: Get median absolute deviation...") {
	SENTENCEVAR (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getMedianAbsoluteDeviation (me, icol);
	NUMBER_ONE_END (U"")
}

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

FORM (INTEGER_Table_getNumberOfRowsWhere, U"", nullptr) {
	LABEL (U"", U"Count only rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"M\"")
	OK
DO
	INTEGER_ONE (Table)
		long result = Table_getNumberOfRowsWhere (me, formula, interpreter);
	INTEGER_ONE_END (U"")
}

FORM (INFO_Table_reportOneWayAnova, U"Table: Report one-way anova",  U"Table: Report one-way anova...") {
	SENTENCEVAR (dataColumn_string, U"Column with data", U"F0")
	SENTENCEVAR (factor_string, U"Factor", U"Vowel")
	BOOLEANVAR (wantMeans, U"Table with means", false);
	BOOLEANVAR (wantDifferencesBetweenMeans, U"Table with differences between means", false)
	BOOLEANVAR (wantTukeyPostHoc, U"Table with Tukey's post-hoc HSD test", false)
	OK
DO
	INFO_ONE (Table)
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factor_string);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		autoTable means, meansDiff, meansDiffProbabilities;
		autoTable anova = Table_getOneWayAnalysisOfVarianceF (me, dataColumn, factorColumn, &means, &meansDiff, & meansDiffProbabilities);
		MelderInfo_open ();
		MelderInfo_writeLine (U"One-way analysis of \"", dataColumn_string, U"\" by \"", factor_string, U"\".\n");
		Table_printAsAnovaTable (anova.get());
		MelderInfo_writeLine (U"\nMeans:\n");
		print_means (means.get());
		MelderInfo_close ();
		if (wantMeans) {
			praat_new (means.move(), my name, U"_groupMeans");
		}
		if (wantDifferencesBetweenMeans) {
			praat_new (meansDiff.move(), my name, U"_meansDiff");
		}
		if (wantTukeyPostHoc) {
			praat_new (meansDiffProbabilities.move(), my name, U"_meansDiffP");
		}
	INFO_ONE_END
}

FORM (INFO_Table_reportTwoWayAnova, U"Table: Report two-way anova", U"Table: Report two-way anova...") {
	SENTENCEVAR (dataColumn_string, U"Column with data", U"Data")
	SENTENCEVAR (firstFactor_string, U"First factor", U"A")
	SENTENCEVAR (secondFactor_string, U"Second factor", U"B")
	BOOLEANVAR (wantMeans, U"Table with means", 0);
	OK
DO
	INFO_ONE (Table)
		long firstFactorColumn = Table_getColumnIndexFromColumnLabel (me, firstFactor_string);
		long secondFactorColumn = Table_getColumnIndexFromColumnLabel (me, secondFactor_string);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		autoTable means, sizes;
		autoTable anova = Table_getTwoWayAnalysisOfVarianceF (me, dataColumn, firstFactorColumn, secondFactorColumn, &means, &sizes);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Two-way analysis of \"", dataColumn_string, U"\" by \"", firstFactor_string, U"\" and \"", secondFactor_string, U".\n");
			Table_printAsAnovaTable (anova.get());
			MelderInfo_writeLine (U"\nMeans:\n");
			Table_printAsMeansTable (means.get());
			MelderInfo_writeLine (U"\nCell sizes:\n");
			Table_printAsMeansTable (sizes.get());
		MelderInfo_close ();
		if (wantMeans) {
			praat_new (means.move(), my name, U"_groupMeans");
		}
	INFO_ONE_END
}

FORM (INFO_Table_reportOneWayKruskalWallis, U"Table: Report one-way Kruskal-Wallis", U"Table: Report one-way Kruskal-Wallis...") {
	SENTENCEVAR (dataColumn_string, U"Column with data", U"Data")
	SENTENCEVAR (factor_string, U"Factor", U"Group")
	OK
DO
	INFO_ONE (Table)
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factor_string);
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		double df, kruskalWallis, prob;
		autoTable result = Table_getOneWayKruskalWallis (me, dataColumn, factorColumn, & prob, & kruskalWallis, & df);
		MelderInfo_open ();
			MelderInfo_writeLine (U"One-way Kruskal-Wallis of \"", dataColumn_string, U"\" by \"", factor_string, U"\".\n");
			MelderInfo_writeLine (U"Chi squared: ", kruskalWallis);
			MelderInfo_writeLine (U"Degrees of freedom: ", df);
			MelderInfo_writeLine (U"Probability: ", prob);
			MelderInfo_writeLine (U"\nMeans:\n");
			print_means (result.get());
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (NEW_Table_to_StringsIndex_column, U"Table: To StringsIndex (column)", nullptr) {
	SENTENCEVAR (columnLabel, U"Column label", U"")
	OK
DO
	CONVERT_EACH (Table)
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoStringsIndex result = Table_to_StringsIndex_column (me, icol);
	CONVERT_EACH_END (my name, U"_", columnLabel)
}

/******************* LegendreSeries *********************************/

FORM (NEW1_LegendreSeries_create, U"Create LegendreSeries", U"Create LegendreSeries...") {
	WORDVAR (name, U"Name", U"ls")
	LABEL (U"", U"Domain")
	REALVAR (xmin, U"Xmin", U"-1")
	REALVAR (xmax, U"Xmax", U"1")
	LABEL (U"", U"LegendreSeries(x) = c[1] P[0](x) + c[2] P[1](x) + ... c[n+1] P[n](x)")
	LABEL (U"", U"P[k] is a Legendre polynomial of degree k")
	SENTENCEVAR (coefficients_string, U"Coefficients", U"0 0 1.0")
	OK
DO
	REQUIRE  (xmin < xmax, U"Xmin must be smaller than Xmax.") 
	CREATE_ONE
		autoLegendreSeries result = LegendreSeries_createFromString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

DIRECT (HELP_LegendreSeries_help) {
	HELP (U"LegendreSeries")
}

DIRECT (NEW_LegendreSeries_to_Polynomial) {
	CONVERT_EACH (LegendreSeries)
		autoPolynomial result = LegendreSeries_to_Polynomial (me);
	CONVERT_EACH_END (my name)
}


/********************* LongSound **************************************/

FORM_READ (READ1_LongSounds_appendToExistingSoundFile, U"LongSound: Append to existing sound file", 0, false) {
	OrderedOf<structSampled> list;
	LOOP {
		iam (Sampled);
		list. addItem_ref (me);
	}
	LongSounds_appendToExistingSoundFile (& list, file);
END }

FORM_SAVE (SAVE_LongSounds_writeToStereoAiffFile, U"LongSound: Save as AIFF file", 0, U"aiff") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_AIFF, file);
	END
}

FORM_SAVE (SAVE_LongSounds_writeToStereoAifcFile, U"LongSound: Save as AIFC file", 0, U"aifc") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_AIFC, file);
	END
}


FORM_SAVE (SAVE_LongSounds_writeToStereoWavFile, U"LongSound: Save as WAV file", 0, U"wav") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_WAV, file);
	END
}

FORM_SAVE (SAVE_LongSounds_writeToStereoNextSunFile, U"LongSound: Save as NeXT/Sun file", 0, U"au") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_NEXT_SUN, file);
	END
}

FORM_SAVE (SAVE_LongSounds_writeToStereoNistFile, U"LongSound: Save as NIST file", 0, U"nist") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_NIST, file);
	END
}

/******************* Matrix **************************************************/

FORM (GRAPHICS_Matrix_drawAsSquares, U"Matrix: Draw as squares", U"Matrix: Draw as squares...") {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawAsSquares (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_drawDistribution, U"Matrix: Draw distribution", U"Matrix: Draw distribution...") {
	LABEL (U"", U"Selection of (part of) Matrix")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	LABEL (U"", U"Selection of Matrix values")
	REALVAR (minimumValue, U"Minimum value", U"0.0")
	REALVAR (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURALVAR (numberOfBins, U"Number of bins", U"10")
	REALVAR (minimumFrequency, U"Minimum frequency", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawDistribution (me, GRAPHICS, xmin, xmax, ymin, ymax, minimumValue, maximumValue, numberOfBins, minimumFrequency, maximumFrequency, false, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_drawCumulativeDistribution, U"Matrix: Draw cumulative distribution", nullptr) {
	LABEL (U"", U"Selection of (part of) Matrix")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	LABEL (U"", U"Selection of Matrix values")
	REALVAR (minimumValue, U"Minimum value", U"0.0")
	REALVAR (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURALVAR (numberOfBins, U"Number of bins", U"10")
	REALVAR (minimum, U"Minimum", U"0.0")
	REALVAR (maximum, U"Maximum", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawDistribution (me, GRAPHICS, xmin, xmax, ymin, ymax, minimumValue, maximumValue, numberOfBins, minimum, maximum, true, garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_Matrix_getMean, U"Matrix: Get mean", nullptr) {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	OK
DO
	NUMBER_ONE (Matrix)
		double result = Matrix_getMean (me, xmin, xmax, ymin, ymax);
	NUMBER_ONE_END (U" (mean)")
}

FORM (REAL_Matrix_getStandardDeviation, U"Matrix: Get standard deviation", nullptr) {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	OK
DO
	NUMBER_ONE (Matrix)
		double result = Matrix_getStandardDeviation (me, xmin, xmax, ymin, ymax);
	NUMBER_ONE_END (U" (std dev)")
}

FORM (MODIFY_Matrix_scale, U"Matrix: Scale", nullptr) {
	LABEL (U"", U"self[row, col] := self[row, col] / `Scale factor'")
	RADIOVAR (scaleMethod, U"Scale factor", 1)
		RADIOBUTTON (U"Extremum in matrix")
		RADIOBUTTON (U"Extremum in each row")
		RADIOBUTTON (U"Extremum in each column")
	OK
DO
	MODIFY_EACH (Matrix)
		Matrix_scale (me, scaleMethod);
	MODIFY_EACH_END
}

DIRECT (NEW_Matrix_transpose) {
	CONVERT_EACH (Matrix)
		autoMatrix result = Matrix_transpose (me);
	CONVERT_EACH_END (my name, U"_transposed")
}

DIRECT (NEW_Matrix_to_PCA_byColumns) {
	CONVERT_EACH (Matrix)
		autoPCA result = Matrix_to_PCA_byColumns (me);
	CONVERT_EACH_END (my name, U"_columns");
}

DIRECT (NEW_Matrix_to_PCA_byRows) {
	CONVERT_EACH (Matrix)
		autoPCA result = Matrix_to_PCA_byRows (me);
	CONVERT_EACH_END (my name, U"_rows")
}

FORM (NEW_Matrix_solveEquation, U"Matrix: Solve equation", U"Matrix: Solve equation...") {
	REALVAR (tolerance, U"Tolerance", U"1.19e-7")
	OK
DO
	CONVERT_EACH (Matrix)
		autoMatrix result = Matrix_solveEquation (me, tolerance);
	CONVERT_EACH_END (my name, U"_solution")
}

DIRECT (NEW1_Matrix_Categories_to_TableOfReal) {
	CONVERT_ONE_AND_GENERIC (Categories, Matrix)
		autoTableOfReal result = Matrix_and_Categories_to_TableOfReal (you, me);
	CONVERT_ONE_AND_GENERIC_END (my name, U"_", your name)
}

DIRECT (NEW1_ActivationList_Categories_to_TableOfReal) {
	CONVERT_TWO (ActivationList, Categories)
		autoTableOfReal result = Matrix_and_Categories_to_TableOfReal (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (GRAPHICS_Matrix_scatterPlot, U"Matrix: Scatter plot", nullptr) {
	NATURALVAR (xColumn, U"Column for X-axis", U"1")
	NATURALVAR (yColumn, U"Column for Y-axis", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO

	GRAPHICS_EACH (Matrix)
		Matrix_scatterPlot (me, GRAPHICS, xColumn, yColumn, xmin, xmax, ymin, ymax, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_Matrix_to_ActivationList) {
	CONVERT_EACH (Matrix)
		autoActivationList result = Matrix_to_ActivationList (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW1_Matrices_to_DTW, U"Matrices: To DTW", U"Matrix: To DTW...") {
	LABEL (U"", U"Distance  between cepstral coefficients")
	REALVAR (distanceMetric, U"Distance metric", U"2.0")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (Matrix)
		autoDTW result = Matrices_to_DTW (me, you, matchStart, matchEnd, slopeConstraint, distanceMetric);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW_Matrix_to_PatternList, U"Matrix: To PatternList", nullptr) {
	NATURALVAR (join, U"Join", U"1")
	OK
DO
	CONVERT_EACH (Matrix)
		autoPatternList result = Matrix_to_PatternList (me, join);
	CONVERT_EACH_END (my name)
}

/**** Filterbank (deprecated) *******/

DIRECT (REAL_FilterBank_getHighestFrequency) {
	NUMBER_ONE (FilterBank)
		double result = my ymax;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (REAL_FilterBank_getLowestFrequency) {
	NUMBER_ONE (FilterBank)
		double result = my ymin;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (INTEGER_FilterBank_getNumberOfFrequencies) {
	INTEGER_ONE (FilterBank)
		long result = my ny;
	INTEGER_ONE_END (U"")
}

DIRECT (REAL_FilterBank_getFrequencyDistance) {
	NUMBER_ONE (FilterBank)
		double result = my dy;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_FilterBank_getXofColumn, U"Get time of column", nullptr) {
	NATURALVAR (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = Matrix_columnToX (me, columnNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_FilterBank_getFrequencyOfRow, U"Get frequency of row", nullptr) {
	NATURALVAR (rowNumber, U"Row number", U"1")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = Matrix_rowToY (me, rowNumber);
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_FilterBank_getValueInCell, U"Get value in cell", nullptr) {
	REALVAR (time, U"Time (s)", U"0.5")
	POSITIVEVAR (frequency, U"Frequency", U"1.0")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = undefined;
		if ((frequency >= my ymin && frequency <= my ymax) && (time >+ my xmin && time <= my ymin)) {
			long col = Matrix_xToNearestColumn (me, time);
			if (col < 1) {
				col = 1;
			}
			if (col > my nx) {
				col = my nx;
			}
			long row = Matrix_yToNearestRow (me, frequency);
			if (row < 1) {
				row = 1;
			}
			if (row > my ny) {
				row = my ny;
			}
			result = my z[row][col];
		}
	NUMBER_ONE_END (U"")
}

/***** MATRIXFT *************/

DIRECT (REAL_BandFilterSpectrogram_getHighestFrequency) {
	NUMBER_ONE (BandFilterSpectrogram)
		double result = my ymax;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (REAL_BandFilterSpectrogram_getLowestFrequency) {
	NUMBER_ONE (BandFilterSpectrogram)
		double result = my ymin;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (INTEGER_BandFilterSpectrogram_getNumberOfFrequencies) {
	INTEGER_ONE (BandFilterSpectrogram)
		long result = my ny;
	INTEGER_ONE_END (U"")
}

DIRECT (REAL_BandFilterSpectrogram_getFrequencyDistance) {
	NUMBER_ONE (BandFilterSpectrogram)
		double result = my dy;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_BandFilterSpectrogram_getFrequencyOfRow, U"Get frequency of row", nullptr) {
	NATURALVAR (rowNumber, U"Row number", U"1")
	OK
DO
	NUMBER_ONE (BandFilterSpectrogram)
		double result = Matrix_rowToY (me, rowNumber);
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_BandFilterSpectrogram_getXofColumn, U"Get time of column", nullptr) {
	NATURALVAR (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_ONE (BandFilterSpectrogram)
		double result = Matrix_columnToX (me, columnNumber);
	NUMBER_ONE_END (U" s")
}

FORM (REAL_BandFilterSpectrogram_getValueInCell, U"Get value in cell", nullptr) {
	REALVAR (time, U"Time (s)", U"0.5")
	POSITIVEVAR (frequency, U"Frequency", U"1")
	OK
DO
	NUMBER_ONE (BandFilterSpectrogram)
		double result = undefined;
		if ((frequency >= my ymin && frequency <= my ymax) && (time >+ my xmin && time <= my ymin)) {
			long col = Matrix_xToNearestColumn (me, time);
			if (col < 1) {
				col = 1;
			}
			if (col > my nx) {
				col = my nx;
			}
			long row = Matrix_yToNearestRow (me, frequency);
			if (row < 1) {
				row = 1;
			}
			if (row > my ny) {
				row = my ny;
			}
			result = my z[row][col];
		}
	NUMBER_ONE_END (U"")
}

/**************** MelFilter *******************************************/

DIRECT (HELP_MelFilter_help) {
	HELP (U"MelFilter")
}

DIRECT (HELP_MelSpectrogram_help) {
	HELP (U"MelSpectrogram")
}

FORM (GRAPHICS_MelFilter_drawFilterFunctions, U"MelFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGERVAR (fromFilter, U"left Filter range", U"0")
	INTEGERVAR (toFilter, U"right Filter range", U"0")
	RADIOVAR (frequencyScale, U"Frequency scale", 1)
	RADIOBUTTON (U"Hertz")
	RADIOBUTTON (U"Bark")
	RADIOBUTTON (U"Mel")
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	BOOLEANVAR (dBScale, U"Amplitude scale in dB", 0)
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelFilter)
		MelFilter_drawFilterFunctions (me, GRAPHICS, frequencyScale, fromFilter, toFilter, fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelSpectrogram_drawTriangularFilterFunctions, U"MelSpectrogram: Draw triangulat filter functions", U"MelSpectrogram: Draw filter functions...") {
	INTEGERVAR (fromFilter, U"left Filter range", U"0")
	INTEGERVAR (toFilter, U"right Filter range", U"0")
	RADIOVAR (frequencyScale, U"Frequency scale", 1)
	RADIOBUTTON (U"Mel")
	RADIOBUTTON (U"Hertz")
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right Frequency range", U"0.0")
	BOOLEANVAR (dBScale, U"Amplitude scale in dB", 0)
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		MelSpectrogram_drawTriangularFilterFunctions (me, GRAPHICS, frequencyScale - 1, fromFilter, toFilter, fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelFilter_drawSpectrum, U"MelFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REALVAR (time, U"Time (s)", U"0.1")
	REALVAR (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (mel)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Mels", garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelSpectrogram_drawSpectrumAtNearestTimeSlice, U"MelSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...") {
	REALVAR (time, U"Time (s)", U"0.1")
	REALVAR (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (mel)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkSpectrogram_drawSpectrumAtNearestTimeSlice, U"BarkSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...") {
	REALVAR (time, U"Time (s)", U"0.1")
	REALVAR (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (bark)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelFilter_paint, U"FilterBank: Paint", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (mel)", U"0.0")
	REALVAR (fromAmplitude, U"left Amplitude range", U"0.0")
	REALVAR (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (Matrix)
		FilterBank_paint ((FilterBank) me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW_MelFilter_to_MFCC, U"MelFilter: To MFCC", U"MelSpectrogram: To MFCC...") {
	NATURALVAR (numberOfCoefficients, U"Number of coefficients", U"12")
	OK
DO
	CONVERT_EACH (MelFilter)
		autoMFCC result = MelFilter_to_MFCC (me, numberOfCoefficients);
	CONVERT_EACH_END (my name)
}

FORM (NEW_MelSpectrogram_to_MFCC, U"MelSpectrogram: To MFCC", U"MelSpectrogram: To MFCC...") {
	NATURALVAR (numberOfCoefficients, U"Number of coefficients", U"12")
	OK
DO
	CONVERT_EACH (MelSpectrogram)
		autoMFCC result = MelSpectrogram_to_MFCC (me, numberOfCoefficients);
	CONVERT_EACH_END (my name)
}

/**************** Ltas *******************************************/

#include "UnicodeData.h"
FORM (INFO_Ltas_reportSpectralTilt, U"Ltas: Report spectral tilt", nullptr) {
	POSITIVEVAR (fromFrequency, U"left Frequency range (Hz)", U"100.0")
	POSITIVEVAR (toFrequency, U"right Frequency range (Hz)", U"5000.0")
	OPTIONMENUVAR (frequencyScale, U"Frequency scale", 1)
		OPTION (U"Linear")
		OPTION (U"Logarithmic")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	bool logScale = frequencyScale == 2;
	INFO_ONE (Ltas)
		double a, b;
		Ltas_fitTiltLine (me, fromFrequency, toFrequency, logScale, fitMethod, &a, &b);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Spectral model: amplitude_dB(frequency_Hz) " UNITEXT_ALMOST_EQUAL_TO " ", logScale ? U"offset + slope * log (frequency_Hz)" : U"offset + slope * frequency_Hz");
			MelderInfo_writeLine (U"Slope: ", a, logScale ? U" dB/decade" : U" dB/Hz");
			MelderInfo_writeLine (U"Offset: ", b, U" dB");
		MelderInfo_close ();
	INFO_ONE_END
}


/**************** MFCC *******************************************/

DIRECT (HELP_MFCC_help) {
	HELP (U"MFCC")
}

FORM (NEW_MFCC_to_MelFilter, U"MFCC: To MelFilter", nullptr) {
	INTEGERVAR (fromCoefficient, U"From coefficient", U"0")
	INTEGERVAR (toCoefficient, U"To coefficient", U"0")
	OK
DO
	CONVERT_EACH (MFCC)
		autoMelFilter result = MFCC_to_MelFilter (me, fromCoefficient, toCoefficient);
	CONVERT_EACH_END (my name)
}

FORM (NEW_MFCC_to_MelSpectrogram, U"MFCC: MelSpectrogram", U"MFCC: To MelSpectrogram...") {
	INTEGERVAR (fromCoefficient, U"From coefficient", U"0")
	INTEGERVAR (toCoefficient, U"To coefficient", U"0")
	BOOLEANVAR (includeConstant, U"Include constant term", true)
	OK
DO
	CONVERT_EACH (MFCC)
		autoMelSpectrogram result = MFCC_to_MelSpectrogram (me, fromCoefficient, toCoefficient, includeConstant);
	CONVERT_EACH_END (my name)
}

FORM (NEW_MFCC_to_TableOfReal, U"MFCC: To TableOfReal", U"MFCC: To TableOfReal...") {
	BOOLEANVAR (includeEnergy, U"Include energy", false)
	OK
DO
	CONVERT_EACH (MFCC)
		autoTableOfReal result = MFCC_to_TableOfReal (me, includeEnergy);
	CONVERT_EACH_END (my name)
}

FORM (NEW_MFCC_to_Matrix_features, U"MFCC: To Matrix (features)", nullptr) {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	BOOLEANVAR (includeEnergy, U"Include energy", false)
	OK
DO
	CONVERT_EACH (MFCC)
		autoMatrix result = MFCC_to_Matrix_features (me, windowLength, includeEnergy);
	CONVERT_EACH_END (my name)
}

FORM (NEW1_MFCCs_crossCorrelate, U"MFCC & MFCC: Cross-correlate", nullptr) {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	CONVERT_COUPLE (MFCC)
		autoSound result = MFCCs_crossCorrelate (me, you,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	CONVERT_COUPLE_END (my name, U"_",your name)
}

FORM (NEW1_MFCCs_convolve, U"MFCC & MFCC: Convolve", nullptr) {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	CONVERT_COUPLE (MFCC)
		autoSound result = MFCCs_convolve (me, you,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	CONVERT_COUPLE_END (my name, U"_",your name)
}

DIRECT (NEW_MFCC_to_Sound) {
	CONVERT_EACH (MFCC)
		autoSound result = MFCC_to_Sound (me);
	CONVERT_EACH_END (my name)
}

/**************** MSpline *******************************************/

FORM (NEW_MSpline_create, U"Create MSpline", U"Create MSpline...") {
	WORDVAR (name, U"Name", U"mspline")
	LABEL (U"", U"Domain")
	REALVAR (xmin, U"Xmin", U"0")
	REALVAR (xmax, U"Xmax", U"1")
	LABEL (U"", U"MSpline(x) = c[1] M[1](x) + c[2] M[1](x) + ... c[n] M[n](x)")
	LABEL (U"", U"all M[k] are polynomials of degree \"Degree\"")
	LABEL (U"", U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree + 1")
	INTEGERVAR (degree, U"Degree", U"2")
	SENTENCEVAR (coefficients_string, U"Coefficients (c[k])", U"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCEVAR (knots_string, U"Interior knots" , U"0.3 0.5 0.6")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CREATE_ONE
		autoMSpline result = MSpline_createFromStrings (xmin, xmax, degree, coefficients_string, knots_string);
	CREATE_ONE_END (name)
}

DIRECT (HELP_MSpline_help) {
	HELP (U"MSpline")
}

/********************** PatternList *******************************************/

DIRECT (NEW1_PatternList_and_Categories_to_Discriminant) {
	CONVERT_TWO (PatternList, Categories)
		autoDiscriminant result = PatternList_and_Categories_to_Discriminant (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (GRAPHICS_PatternList_draw, U"PatternList: Draw", nullptr) {
	NATURALVAR (patternNumber, U"Pattern number", U"1")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PatternList)
		PatternList_draw (me, GRAPHICS, patternNumber, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_PatternList_getNumberOfPatterns) {
	INTEGER_ONE (PatternList)
		long result = my ny;
	INTEGER_ONE_END (U" (number of patterns)")
}

DIRECT (INTEGER_PatternList_getPatternSize) {
	INTEGER_ONE (PatternList)
		long result = my nx;
	INTEGER_ONE_END (U" (pattern size)")
}

FORM (REAL_PatternList_getValue, U"", nullptr) {
	NATURALVAR (patternNumber, U"Pattern number", U"1")
	NATURALVAR (nodeNumber, U"Node number", U"2")
	OK
DO
	NUMBER_ONE (PatternList)
		double result = ( patternNumber <= my ny && nodeNumber <= my nx ? my z [patternNumber] [nodeNumber] : undefined );
	NUMBER_ONE_END (U"")
}

FORM (MODIFY_PatternList_formula, U"PatternList: Formula", nullptr) {
	LABEL (U"label", U"        y := 1; for row := 1 to nrow do { x := 1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + 1 } "
		"y := y + 1 }}")
	TEXTVAR (formula, U"formula", U"self")
	OK
DO
	MODIFY_EACH (PatternList)
		Matrix_formula (reinterpret_cast <Matrix> (me), formula, interpreter, nullptr);
	MODIFY_EACH_END
}

FORM (MODIFY_PatternList_setValue, U"PatternList: Set value", U"PatternList: Set value...") {
	NATURALVAR (rowNumber, U"Row number", U"1")
	NATURALVAR (columnNumber, U"Column number", U"1")
	REALVAR (newValue, U"New value", U"0.0")
	OK
DO
	MODIFY_EACH (PatternList)
		REQUIRE (rowNumber <= my ny, U"Row number must not be greater than number of rows.")
		REQUIRE (columnNumber <= my nx, U"Column number must not be greater than number of columns.")
		my z [rowNumber] [columnNumber] = newValue;
	MODIFY_EACH_END
}

DIRECT (NEW_PatternList_to_Matrix) {
	CONVERT_EACH (PatternList)
		autoMatrix result = PatternList_to_Matrix (me);
	CONVERT_EACH_END (my name)
}


/******************* PCA ******************************/

DIRECT (HELP_PCA_help) {
	HELP (U"PCA")
}

DIRECT (HINT_hint_PCA_and_TableOfReal_to_Configuration) {
	Melder_information (U"You can get principal components by selecting a PCA and a TableOfReal\n"
		"together and choosing \"To Configuration...\".");
	END 

}

DIRECT (HINT_hint_PCA_and_Covariance_Project) {
	Melder_information (U"You can get a new Covariance object rotated to the directions of the direction vectors\n"
		" in the PCA object by selecting a PCA and a Covariance object together.");
END }

DIRECT (HINT_hint_PCA_and_Configuration_to_TableOfReal_reconstruct) {
	Melder_information (U"You can reconstruct the original TableOfReal as well as possible from\n"
		" the principal components in the Configuration and the direction vectors in the PCA object.");
END }

FORM (REAL_PCA_and_TableOfReal_getFractionVariance, U"PCA & TableOfReal: Get fraction variance", U"PCA & TableOfReal: Get fraction variance...") {
	NATURALVAR (fromPrincipalComponent, U"left Principal component range", U"1")
	NATURALVAR (toPrincipalComponent, U"right Principal component range", U"1")
	OK
DO
	NUMBER_TWO (PCA, TableOfReal)
		double result = PCA_and_TableOfReal_getFractionVariance (me, you, fromPrincipalComponent, toPrincipalComponent);
	NUMBER_TWO_END (U"")
}

DIRECT (NEW_PCA_and_Configuration_to_TableOfReal_reconstruct) {
	CONVERT_TWO (PCA, Configuration)
		autoTableOfReal result = PCA_and_Configuration_to_TableOfReal_reconstruct (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_PCA_and_TableOfReal_to_TableOfReal_projectRows, U"PCA & TableOfReal: To TableOfReal (project rows)", U"PCA & TableOfReal: To Configuration...") {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions to keep", U"0 (= all)")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_ONE_AND_GENERIC (PCA, TableOfReal)
		autoTableOfReal result = PCA_and_TableOfReal_to_TableOfReal_projectRows (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name, U"_", your name)
}

FORM (NEW1_PCA_and_TableOfReal_to_Configuration, U"PCA & TableOfReal: To Configuration", U"PCA & TableOfReal: To Configuration...") {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_ONE_AND_GENERIC (PCA, TableOfReal)
		autoConfiguration result = PCA_and_TableOfReal_to_Configuration (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name, U"_", your name)
}

FORM (NEW1_PCA_and_TableOfReal_to_TableOfReal_zscores, U"PCA & TableOfReal: To TableOfReal (z-scores)", U"PCA & TableOfReal: To TableOfReal (z-scores)...") {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_ONE_AND_GENERIC (PCA, TableOfReal)
		autoTableOfReal result = PCA_and_TableOfReal_to_TableOfReal_zscores (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name, U"_", your name, U"_zscores")
}

FORM (NEW1_PCA_and_Matrix_to_Matrix_projectRows, U"PCA & Matrix: To Matrix (project rows)", nullptr) {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_TWO (PCA, Matrix)
		autoMatrix result = Eigen_and_Matrix_to_Matrix_projectRows (me, you, numberOfDimensions);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_PCA_and_Matrix_to_Matrix_projectColumns, U"PCA & Matrix: To Matrix (project columns)", nullptr) {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_ONE_AND_GENERIC (PCA, Matrix)
		autoMatrix result = Eigen_and_Matrix_to_Matrix_projectColumns (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name, U"_", your name)
}

FORM (REAL_PCA_getCentroidElement, U"PCA: Get centroid element...", nullptr) {
	NATURALVAR (number, U"Number", U"1")
	OK
DO
	NUMBER_ONE (PCA)
		REQUIRE (number <= my dimension, Melder_cat (U"Number may not be larger than ", my dimension, U"."))
		double result = my centroid [number];
	NUMBER_ONE_END (U" (element ", number, U")")
}

FORM (REAL_PCA_getEqualityOfEigenvalues, U"PCA: Get equality of eigenvalues", U"PCA: Get equality of eigenvalues...") {
	INTEGERVAR (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGERVAR (toEigenvalue, U"right Eigenvalue range", U"0")
	BOOLEANVAR (conservativeTest, U"Conservative test", false)
	OK
DO
	NUMBER_ONE (PCA)
		double result, chisq, df;
		PCA_getEqualityOfEigenvalues (me, fromEigenvalue,
		toEigenvalue, conservativeTest, & result, & chisq, & df);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and df = ", df)
}

FORM (INTEGER_PCA_getNumberOfComponentsVAF, U"PCA: Get number of components (VAF)", U"PCA: Get number of components (VAF)...") {
	POSITIVEVAR (varianceFraction, U"Variance fraction (0-1)", U"0.95")
	OK
DO
	REQUIRE (varianceFraction >= 0.0 && varianceFraction <= 1.0, U"The variance fraction must be in interval (0-1).")
	NUMBER_ONE (PCA)
		double result = Eigen_getDimensionOfFraction (me, varianceFraction);
	NUMBER_ONE_END (U" (variance fraction)")
}

FORM (REAL_PCA_getFractionVAF, U"PCA: Get fraction variance accounted for", U"PCA: Get fraction variance accounted for...") {
	NATURALVAR (fromPrincipalComponent, U"left Principal component range", U"1")
	NATURALVAR (toPrincipalComponent, U"right Principal component range", U"1")
	OK
DO
	REQUIRE (fromPrincipalComponent <= toPrincipalComponent, U"The second component must be greater than or equal to the first component.")
	NUMBER_ONE (PCA)
		double result = Eigen_getCumulativeContributionOfComponents (me, fromPrincipalComponent, toPrincipalComponent);
	NUMBER_ONE_END (U"")
}

FORM (MODIFY_PCA_invertEigenvector, U"PCA: Invert eigenvector", nullptr) {
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Eigen)
		Eigen_invertEigenvector (me, eigenvectorNumber);
	MODIFY_EACH_END
}

FORM (NEW_PCA_extractEigenvector, U"PCA: Extract eigenvector", U"Eigen: Extract eigenvector...") {
	NATURALVAR (eigenvectorNumber, U"Eigenvector number", U"1")
	LABEL (U"", U"Reshape as")
	INTEGERVAR (numberOfRows, U"Number of rows", U"0")
	INTEGERVAR (numberOfColumns, U"Number of columns", U"0")
	OK
DO
	REQUIRE (numberOfRows >= 0, U"Number of rows must be >= 0.")
	REQUIRE (numberOfColumns >= 0, U"Number of columns must be >= 0.")
	CONVERT_EACH (PCA);
		autoMatrix result = Eigen_extractEigenvector (me, eigenvectorNumber, numberOfRows, numberOfColumns);
	CONVERT_EACH_END (my name, U"_ev", eigenvectorNumber)
}

DIRECT (NEW_PCA_extractEigen) {
	CONVERT_EACH (PCA)
		autoEigen result = PCA_to_Eigen (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PCA_to_TableOfReal_reconstruct1, U"PCA: To TableOfReal (reconstruct)", U"PCA: To TableOfReal (reconstruct 1)...") {
	SENTENCEVAR (coefficients_string, U"Coefficients", U"1.0 1.0")
	OK
DO
	CONVERT_EACH (PCA)
		autoTableOfReal result = PCA_to_TableOfReal_reconstruct1 (me, coefficients_string);
	CONVERT_EACH_END (my name, U"_reconstructed")
}

FORM (NEW1_PCAs_to_Procrustes, U"PCA & PCA: To Procrustes", U"PCA & PCA: To Procrustes...") {
	NATURALVAR (fromEigenvector, U"left Eigenvector range", U"1")
	NATURALVAR (toEigenvector, U"right Eigenvector range", U"2")
	OK
DO
	CONVERT_COUPLE (PCA)
		autoProcrustes result = Eigens_to_Procrustes (me, you, fromEigenvector, toEigenvector);
	CONVERT_COUPLE_END (my name, U"_", your name)
}


DIRECT (REAL_PCAs_getAngleBetweenPc1Pc2Plane_degrees) {
	NUMBER_COUPLE (PCA)
		double result = Eigens_getAngleBetweenEigenplanes_degrees (me, you);
	NUMBER_COUPLE_END (U" degrees (= angle of intersection between the two pc1-pc2 eigenplanes)")
}

/******************* Permutation **************************************/

DIRECT (HELP_Permutation_help) {
	HELP (U"Permutation")
}

FORM (NEW_Permutation_create, U"Create Permutation", U"Create Permutation...") {
	WORDVAR (name, U"Name", U"p")
	NATURALVAR (numberOfElements, U"Number of elements", U"10")
	BOOLEANVAR (identity, U"Identity Permutation", true)
	OK
DO
	CREATE_ONE
		autoPermutation result = Permutation_create (numberOfElements);
		if (! identity) {
			Permutation_permuteRandomly_inline (result.get(), 0, 0);
		}
	CREATE_ONE_END (name)
}

DIRECT (INTEGER_Permutation_getNumberOfElements) {
	INTEGER_ONE (Permutation)
		long result = my numberOfElements;
	INTEGER_ONE_END (U" (number of elements)")
}

FORM (INTEGER_Permutation_getValueAtIndex, U"Permutation: Get value", U"Permutation: Get value...") {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	INTEGER_ONE (Permutation)
		long result = Permutation_getValueAtIndex (me, index);
	INTEGER_ONE_END (U" (value, at index = ", index, U")")
}

FORM (INTEGER_Permutation_getIndexAtValue, U"Permutation: Get index", U"Permutation: Get index...") {
	NATURALVAR (value, U"Value", U"1")
	OK
DO
	INTEGER_ONE (Permutation)
		long result = Permutation_getIndexAtValue (me, value);
	INTEGER_ONE_END (U" (index, at value = ", value, U")")
}

DIRECT (MODIFY_Permutation_sort) {
	MODIFY_EACH (Permutation)
		Permutation_sort (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapBlocks, U"Permutation: Swap blocks", U"Permutation: Swap blocks...") {
	NATURALVAR (fromIndex, U"From index", U"1")
	NATURALVAR (toIndex, U"To index", U"2")
	NATURALVAR (blockSize, U"Block size", U"1")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapBlocks (me, fromIndex, toIndex, blockSize);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapPositions, U"Permutation: Swap positions", U"Permutation: Swap positions...") {
	NATURALVAR (firstIndex, U"First index", U"1")
	NATURALVAR (secondIndex, U"Second index", U"2")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapPositions (me, firstIndex, secondIndex);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Permutation_swapNumbers, U"Permutation: Swap numbers", U"Permutation: Swap numbers...") {
	NATURALVAR (firstNumber, U"First number", U"1")
	NATURALVAR (secondNumber, U"Second number", U"2")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapNumbers (me, firstNumber, secondNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapOneFromRange, U"Permutation: Swap one from range", U"Permutation: Swap one from range...") {
	LABEL (U"", U"A randomly chosen element from ")
	INTEGERVAR (fromIndex, U"left Index range", U"0")
	INTEGERVAR (toIndex, U"right Index range", U"0")
	LABEL (U"", U"is swapped with the element at")
	NATURALVAR (index, U"Index", U"1")
	BOOLEANVAR (forbidSame, U"Forbid same", 1)
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapOneFromRange (me, fromIndex, toIndex, index, forbidSame);
	MODIFY_EACH_END
}

FORM (NEW_Permutation_permuteRandomly, U"Permutation: Permute randomly", U"Permutation: Permute randomly...") {
	INTEGERVAR (fromIndex, U"left Index range", U"0")
	INTEGERVAR (toIndex, U"right Index range", U"0")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_permuteRandomly (me, fromIndex, toIndex);
	CONVERT_EACH_END (my name, U"_rdm")
}

FORM (NEW_Permutation_rotate, U"Permutation: Rotate", U"Permutation: Rotate...") {
	INTEGERVAR (fromIndex, U"left Index range", U"0")
	INTEGERVAR (toIndex, U"right Index range", U"0")
	INTEGERVAR (step, U"Step size", U"1")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_rotate (me, fromIndex, toIndex, step);
	CONVERT_EACH_END (my name, U"_rot", step)
}

FORM (NEW_Permutation_reverse, U"Permutation: Reverse", U"Permutation: Reverse...") {
	INTEGERVAR (fromIndex, U"left Index range", U"0")
	INTEGERVAR (toIndex, U"right Index range", U"0")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_reverse (me, fromIndex, toIndex);
	CONVERT_EACH_END (my name, U"_rev")
}

FORM (NEW_Permutation_permuteBlocksRandomly, U"Permutation: Permute blocks randomly", U"Permutation: Permute randomly (blocks)...") {
	INTEGERVAR (fromIndex, U"left Index range", U"0")
	INTEGERVAR (toIndex, U"right Index range", U"0")
	NATURALVAR (blockSize, U"Block size", U"12")
	BOOLEANVAR (permuteWithinBlocks, U"Permute within blocks", 1)
	BOOLEANVAR (noDoublets, U"No doublets", 0)
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_permuteBlocksRandomly (me, fromIndex, toIndex, blockSize, permuteWithinBlocks, noDoublets);
	CONVERT_EACH_END (my name, U"_pbr", blockSize)
}

FORM (NEW_Permutation_interleave, U"Permutation: Interleave", U"Permutation: Interleave...") {
	INTEGERVAR (fromIndex, U"left Index range", U"0")
	INTEGERVAR (toIndex, U"right Index range", U"0")
	NATURALVAR (blockSize, U"Block size", U"12")
	INTEGERVAR (offset, U"Offset", U"0")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_interleave (me, fromIndex, toIndex, blockSize, offset);
	CONVERT_EACH_END (my name, U"_itl")
}

DIRECT (NEW_Permutation_invert) {
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_invert (me);
	CONVERT_EACH_END (my name, U"_inv")
}

DIRECT (NEW1_Permutations_multiply) {
	CONVERT_LIST (Permutation)
		autoPermutation result = Permutations_multiply (& list);
	CONVERT_LIST_END (U"mul_", list.size);
}

DIRECT (MODIFY_Permutations_next) {
	MODIFY_EACH (Permutation)
		Permutation_next_inline (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Permutations_previous) {
	MODIFY_EACH (Permutation)
		Permutation_previous_inline (me);
	MODIFY_EACH_END
}

FORM (NEW1_Pitches_to_DTW, U"Pitches: To DTW", U"Pitches: To DTW...") {
	REALVAR (vuvCosts, U"Voiced-unvoiced costs", U"24.0")
	REALVAR (weight, U"Time costs weight", U"10.0")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (Pitch)
		autoDTW result = Pitches_to_DTW (me, you, vuvCosts, weight, matchStart, matchEnd, slopeConstraint);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW_PitchTier_to_Pitch, U"PitchTier: To Pitch", U"PitchTier: To Pitch...") {
	POSITIVEVAR (stepSize, U"Step size", U"0.02")
	POSITIVEVAR (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVEVAR (pitchCeiling, U"Pitch ceiling (Hz)", U"400.0")
	OK
DO
	CONVERT_EACH (PitchTier)
		autoPitch result = PitchTier_to_Pitch (me, stepSize, pitchFloor, pitchCeiling);
	CONVERT_EACH_END (my name)
}

FORM (MODIFY_PitchTier_modifyInterval, U"PitchTier: Modify interval", U"PitchTier: Modify interval...") {
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range", U"0.0 (= all)")
	LABEL (U"", U"")
	SENTENCEVAR (timesString, U"Relative times", U"0.0 0.5 1.0")
	OPTIONMENUVAR (timeOffset, U"...are...", 1)
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"independent")
	LABEL (U"", U"...of the interval duration which will be added...")
	LABEL (U"", U"...to the start time of the interval.")
	SENTENCEVAR (pitches_string, U"The \"pitch\" values", U"100 200 100")
	OPTIONMENUVAR (pitch_as, U"...are...", 1)
		OPTION (U"frequencies")
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"start and slopes")
		OPTION (U"slopes and end")
		OPTION (U"music notes")
//		OPTION (U"semitones")
	LABEL (U"", U"...to be added to the anchor value (if used)...")
	OPTIONMENUVAR (pitch_is, U"...which is the...", 1)
		OPTION (U"not used")
		OPTION (U"current")
		OPTION (U"start")
		OPTION (U"end")
		OPTION (U"mean of the curve")
		OPTION (U"mean of the points")
		OPTION (U"maximum")
		OPTION (U"minimum")
	LABEL (U"", U"...frequency value in the interval.")
	LABEL (U"", U"")
	OPTIONMENUVAR (pitch_unit, U"Pitch frequency unit", 1)
		OPTION (U"Hertz")

	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_modifyInterval (me, fromTime, toTime, timesString, timeOffset, pitches_string, pitch_unit, pitch_as, pitch_is);
	MODIFY_EACH_END
}


FORM (MODIFY_PitchTier_modifyInterval_toneLevels, U"PitchTier: Modify interval (tone levels)", U"PitchTier: Modify interval (tone levels)...") {
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range", U"0.0 (= all)")
	REAL4 (fmin, U"left Pitch range (Hz)", U"80.0")
	REAL4 (fmax, U"right Pitch range", U"200.0")
	NATURAL4 (numberOfToneLevels, U"Number of tone levels", U"5")
	LABEL (U"", U"")
	SENTENCEVAR (times_string, U"Relative times", U"0.0 0.5 1.0")
	OPTIONMENUVAR (time_offset, U"...are...", 1)
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"independent")
	LABEL (U"", U"...of the interval duration which will be added...")
	LABEL (U"", U"...to the start time of the interval.")
	SENTENCEVAR (pitches_string, U"Tone levels", U"2.1 2.1 5.0")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_modifyInterval_toneLevels (me, fromTime, toTime, fmin, fmax, numberOfToneLevels, times_string, time_offset, pitches_string);
	MODIFY_EACH_END
}

/******************* Polygon & Categories *************************************/

FORM (NEW1_Polygon_createSimple, U"Create simple Polygon", U"Create simple Polygon...") {
	WORDVAR (name, U"Name", U"p")
	SENTENCEVAR (vertices_string, U"Vertices as X-Y pairs", U"0.0 0.0  0.0 1.0  1.0 0.0")
	OK
DO
	CREATE_ONE
		autoPolygon result = Polygon_createSimple (vertices_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_Polygon_createFromRandomPoints, U"Polygon: Create from random points", nullptr) {
	WORDVAR (name, U"Name", U"p")
	NATURALVAR (numberOfPoints, U"Number of points", U"10")
	REALVAR (xmin, U"left X range", U"0.0")
	REALVAR (xmax, U"right X range", U"1.0")
	REALVAR (ymin, U"left Y range", U"0.0")
	REALVAR (ymax, U"right Y range", U"1.0")
	OK
DO
	CREATE_ONE
		autoPolygon result = Polygon_createFromRandomPoints (numberOfPoints, xmin, xmax, ymin, ymax);
	CREATE_ONE_END (name)
}

DIRECT (INTEGER_Polygon_getNumberOfPoints) {
	INTEGER_ONE (Polygon)
		long result = my numberOfPoints;
	INTEGER_ONE_END (U" (number of points)")
}

FORM (REAL_Polygon_getPointX, U"Polygon: Get point (x)", nullptr) {
	NATURALVAR (pointNumber, U"Point number", U"1")
	OK
DO
	NUMBER_ONE (Polygon)
		double result = ( pointNumber <= my numberOfPoints ? my x [pointNumber] : undefined );
	NUMBER_ONE_END (U" (x [", pointNumber, U"])")
}

FORM (REAL_Polygon_getPointY, U"Polygon: Get point (y)", nullptr) {
	NATURALVAR (pointNumber, U"Point number", U"1")
	OK
DO
	NUMBER_ONE (Polygon)
		double result = ( pointNumber <= my numberOfPoints ? my y [pointNumber] : undefined );
	NUMBER_ONE_END (U" (y [", pointNumber, U"])")
}

FORM (INFO_Polygon_getLocationOfPoint, U"Get location of point", U"Polygon: Get location of point...") {
	LABEL (U"", U"Point is (I)n, (O)ut, (E)dge or (V)ertex?")
	REALVAR (x, U"X", U"0.0")
	REALVAR (y, U"Y", U"0.0")
	REALVAR (eps, U"Precision", U"1.64e-15")
	OK
DO
	REQUIRE (eps >= 0, U"The precision cannot be negative.")
	STRING_ONE (Polygon)
		int loc = Polygon_getLocationOfPoint (me, x, y, eps);
		const char32 * result = ( loc == Polygon_INSIDE ? U"I" : loc == Polygon_OUTSIDE ? U"O" :
			loc == Polygon_EDGE ? U"E" : U"V" );
	STRING_ONE_END
}

DIRECT (REAL_Polygon_getAreaOfConvexHull) {
	NUMBER_ONE (Polygon)
		double result = Polygon_getAreaOfConvexHull (me);
	NUMBER_ONE_END (U" (area convex hull)")
}

FORM (NEW_Polygon_circularPermutation, U"Polygon: Circular permutation", nullptr) {
	INTEGERVAR (shift, U"Shift", U"1")
	OK
DO
	CONVERT_EACH (Polygon)
		autoPolygon result = Polygon_circularPermutation (me, shift);
	CONVERT_EACH_END (my name, U"_", shift)
}


DIRECT (NEW_Polygon_simplify) {
	CONVERT_EACH (Polygon)
		autoPolygon result = Polygon_simplify (me);
	CONVERT_EACH_END (my name, U"_s")
}

DIRECT (NEW_Polygon_convexHull) {
	CONVERT_EACH (Polygon)
		autoPolygon result = Polygon_convexHull (me);
	CONVERT_EACH_END (my name, U"_hull")
}

FORM (MODIFY_Polygon_translate, U"Polygon: Translate", U"Polygon: Translate...") {
	REALVAR (xDistance, U"X distance", U"0.0")
	REALVAR (yDistance, U"Y distance", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_translate (me, xDistance, yDistance);
	MODIFY_EACH_END
}

FORM (MODIFY_Polygon_rotate, U"Polygon: Rotate", U"Polygon: Rotate...") {
	LABEL (U"", U"Rotate counterclockwise over the")
	REALVAR (angle_degrees, U"Angle (degrees)", U"0.0")
	LABEL (U"", U"With respect to the point")
	REALVAR (x, U"X", U"0.0")
	REALVAR (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_rotate (me, angle_degrees, x, y);
	MODIFY_EACH_END
}

FORM (MODIFY_Polygon_scale, U"Polygon: Scale polygon", nullptr) {
	REALVAR (x, U"X", U"0.0")
	REALVAR (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_scale (me, x, y);
	MODIFY_EACH_END
}

FORM (GRAPHICS_Polygon_Categories_draw, U"Polygon & Categories: Draw", nullptr) {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (Polygon, Categories)
		Polygon_Categories_draw (me, you, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_TWO_END
}

DIRECT (MODIFY_Polygon_reverseX) {
	MODIFY_EACH (Polygon)
		Polygon_reverseX (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Polygon_reverseY) {
	MODIFY_EACH (Polygon)
		Polygon_reverseY (me);
	MODIFY_EACH_END
}

/***************** Polynomial *******************/

DIRECT (HELP_Polynomial_help) {
	HELP (U"Polynomial")
}

FORM (NEW1_Polynomial_create, U"Create Polynomial from coefficients", U"Create Polynomial...") {
	WORDVAR (name, U"Name", U"p")
	LABEL (U"", U"Domain of polynomial")
	REALVAR (xmin, U"Xmin", U"-3.0")
	REALVAR (xmax, U"Xmax", U"4.0")
	LABEL (U"", U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	SENTENCEVAR (coefficients_string, U"Coefficients", U"2.0 -1.0 -2.0 1.0")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_Polynomial_createFromProducts, U"Create Polynomial from second order products", nullptr) {
	WORDVAR (name, U"Name", U"p")
	LABEL (U"", U"Domain of polynomial")
	REALVAR (xmin, U"Xmin", U"-2.0")
	REALVAR (xmax, U"Xmax", U"2.0")
	LABEL (U"", U"(1+a[1]*x+x^2)*(1+a[2]*x+x^2)*...*(1+a[n]*x+x^2)")
	SENTENCEVAR (coefficients_string, U"The a's", U"1.0 2.0")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromProductOfSecondOrderTermsString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_Polynomial_createFromZeros, U"Create Polynomial from first order products", nullptr) {
	WORDVAR (name, U"Name", U"p")
	LABEL (U"", U"Domain of polynomial")
	REALVAR (xmin, U"Xmin", U"-3.0")
	REALVAR (xmax, U"Xmax", U"3.0")
	LABEL (U"", U"(P(x) = (x-zero[1])*(1-zero[2])*...*(x-zero[n])")
	SENTENCEVAR (zeros_string, U"The zero's", U"1.0 2.0")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromRealRootsString (xmin, xmax, zeros_string);
	CREATE_ONE_END (name)
}

FORM (MODIFY_Polynomial_divide_secondOrderFactor, U"Polynomial: Divide second order factor", nullptr) {
	LABEL (U"", U"P(x) / (x^2 - factor)")
	REALVAR (factor, U"Factor", U"1.0")
	OK
DO
	MODIFY_EACH (Polynomial)
		Polynomial_divide_secondOrderFactor (me, factor);
	MODIFY_EACH_END
}

FORM (REAL_Polynomial_getArea, U"Polynomial: Get area", U"Polynomial: Get area...") {
	LABEL (U"", U"Interval")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (Polynomial)
		double result = Polynomial_getArea (me, xmin, xmax);
	NUMBER_ONE_END (U" (area)")
}

FORM (REAL_Polynomial_getRemainderAfterDivision, U"Polynomial: Get remainder after division", nullptr) {
	LABEL (U"", U"P(x) / (x - factor)")
	REALVAR (factor, U"Monomial factor", U"1.0")
	OK
DO
	NUMBER_ONE (Polynomial)
		double result;
		autoPolynomial p = Data_copy (me);
		Polynomial_divide_firstOrderFactor (p.get(), factor, & result);
	NUMBER_ONE_END (U" (remainder)")
}

FORM (INFO_Polynomial_getDerivativesAtX, U"Polynomial: Get derivatives at X", nullptr) {
	REALVAR (x, U"X", U"0.5")
	INTEGERVAR (numberOfDerivatives, U"Number of derivatives", U"2")
	OK
DO
	autoNUMvector<double> derivatives (0L, numberOfDerivatives);
	INFO_ONE (Polynomial)
		Polynomial_evaluateDerivatives (me, x, derivatives.peek(), numberOfDerivatives);
		MelderInfo_open ();
			for (long i = 0; i <= numberOfDerivatives; i++) {
				MelderInfo_writeLine (i, U": ", i < my numberOfCoefficients ? derivatives [i] : undefined);
			}
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (REAL_Polynomial_getOneRealRoot, U"Polynomial: Get one real root", nullptr) {
	LABEL (U"", U"Interval: ")
	REALVAR (xmin, U"left X Range", U"-1.0")
	REALVAR (xmax, U"right X Range", U"1.0")
	OK
DO
	NUMBER_ONE (Polynomial)
		double result = Polynomial_findOneSimpleRealRoot_nr (me, xmin, xmax);
	NUMBER_ONE_END (U" (root)")
}

DIRECT (NEW_Polynomial_getDerivative) {
	CONVERT_EACH (Polynomial)
		autoPolynomial result = Polynomial_getDerivative (me);
	CONVERT_EACH_END (my name, U"_derivative")
}

FORM (NEW_Polynomial_getPrimitive, U"Polynomial: Get primitive", nullptr) {
	REALVAR (constant, U"Constant", U"0.0")
	OK
DO
	CONVERT_EACH (Polynomial)
		autoPolynomial result = Polynomial_getPrimitive (me,constant);
	CONVERT_EACH_END (my name, U"_primitive")
}

FORM (NEW_Polynomial_scaleX, U"Polynomial: Scale x", U"Polynomial: Scale x...") {
	LABEL (U"", U"New domain")
	REALVAR (xmin, U"Xmin", U"-1.0")
	REALVAR (xmax, U"Xmax", U"1.0")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CONVERT_EACH (Polynomial)
		autoPolynomial result = Polynomial_scaleX (me, xmin, xmax);
	CONVERT_EACH_END (my name, U"_scaleX")
}

DIRECT (MODIFY_Polynomial_scaleCoefficients_monic) {
	MODIFY_EACH (Polynomial)
		Polynomial_scaleCoefficients_monic (me);
	MODIFY_EACH_END
}

DIRECT (NEW_Polynomial_to_Roots) {
	CONVERT_EACH (Polynomial)
		autoRoots result = Polynomial_to_Roots (me);
	CONVERT_EACH_END (my name)
}

FORM (INFO_Polynomial_evaluate_z, U"Polynomial: Get value (complex)", U"Polynomial: Get value (complex)...") {
	REALVAR (x, U"Real part", U"0.0")
	REALVAR (y, U"Imaginary part", U"0.0")
	OK
DO
	dcomplex p, z = dcomplex_create (x, y);
	INFO_ONE (Polynomial)
		Polynomial_evaluate_z (me, & z, & p);
		MelderInfo_open ();
			MelderInfo_writeLine (p.re, U" + ", p.im, U" i");
		MelderInfo_close ();	
	INFO_ONE_END
}

FORM (NEW_Polynomial_to_Spectrum, U"Polynomial: To Spectrum", U"Polynomial: To Spectrum...") {
	POSITIVEVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	NATURALVAR (numberOfFrequencies, U"Number of frequencies (>1)", U"1025")
	OK
DO
	CONVERT_EACH (Polynomial)
		autoSpectrum result = Polynomial_to_Spectrum (me, maximumFrequency, numberOfFrequencies, 1.0);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Polynomials_multiply) {
	CONVERT_COUPLE (Polynomial)
		autoPolynomial result = Polynomials_multiply (me, you);
	CONVERT_COUPLE_END (my name, U"_x_", your name)
}

FORM (NEWMANY_Polynomials_divide, U"Polynomials: Divide", U"Polynomials: Divide...") {
	BOOLEANVAR (wantQuotient, U"Want quotient", true)
	BOOLEANVAR (wantRemainder, U"Want remainder", true)
	OK
DO
	/* With gcc (GCC) 3.2.2 20030217 (Red Hat Linux 8.0 3.2.2-2)
		The following line initiates pq = nullptr and I don't know why
		Polynomial p1 = nullptr, p2 = nullptr, pq, pr;
	*/

	REQUIRE (wantQuotient || wantRemainder, U"Either \'Want quotient\' or \'Want remainder\' should be chosen")
	FIND_COUPLE (Polynomial)
		autoPolynomial aq, ar;
		Polynomials_divide (me, you, wantQuotient ? & aq : nullptr, wantRemainder ? & ar : nullptr);
		if (wantQuotient) {
			praat_new (aq.move(), my name, U"_q");
		}
		if (wantRemainder) {
			praat_new (ar.move(), my name, U"_r");
		}
	END
}

/********************* Roots ******************************/

DIRECT (HELP_Roots_help) {
	HELP (U"Roots")
}

FORM (GRAPHICS_Roots_draw, U"Roots: Draw", nullptr) {
	REALVAR (xmin, U"Minimum of real axis", U"0.0")
	REALVAR (xmax, U"Maximum of real axis", U"0.0")
	REALVAR (ymin, U"Minimum of imaginary axis", U"0.0")
	REALVAR (ymax, U"Maximum of imaginary axis", U"0.0")
	SENTENCEVAR (mark_string, U"Mark string (+x0...)", U"o")
	NATURALVAR (markSize, U"Mark size", U"12")
	BOOLEANVAR (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (Roots)
		Roots_draw (me, GRAPHICS, xmin, xmax, ymin, ymax, mark_string, markSize, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_Roots_getNumberOfRoots) {
	INTEGER_ONE (Roots)
		long result = Roots_getNumberOfRoots (me);
	INTEGER_ONE_END (U"")
}

FORM (COMPLEX_Roots_getRoot, U"Roots: Get root", nullptr) {
	NATURALVAR (rootNumber, U"Root number", U"1")
	OK
DO
	INFO_ONE (Roots)
		dcomplex z = Roots_getRoot (me, rootNumber);
		MelderInfo_open ();
			MelderInfo_writeLine (z.re, U"+", z.im,  U"i");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (REAL_Roots_getRealPartOfRoot, U"Roots: Get real part", nullptr) {
	NATURALVAR (rootNumber, U"Root number", U"1")
	OK
DO
	NUMBER_ONE (Roots)
		dcomplex z = Roots_getRoot (me, rootNumber);
		double result = z.re;
	NUMBER_ONE_END (U"")
}

FORM (REAL_Roots_getImaginaryPartOfRoot, U"Roots: Get imaginary part", nullptr) {
	NATURALVAR (rootNumber, U"Root number", U"1")
	OK
DO
	NUMBER_ONE (Roots)
		dcomplex z = Roots_getRoot (me, GET_INTEGER (U"Root number"));
		double result = z.im;
	NUMBER_ONE_END (U" i")
}

FORM (MODIFY_Roots_setRoot, U"Roots: Set root", nullptr) {
	NATURALVAR (rootNumber, U"Root number", U"1")
	REALVAR (x, U"Real part", U"1.0/sqrt(2)")
	REALVAR (y, U"Imaginary part", U"1.0/sqrt(2)")
	OK
DO
	MODIFY_EACH (Roots)
		Roots_setRoot (me, rootNumber, x, y);
	MODIFY_EACH_END
}

FORM (NEW_Roots_to_Spectrum, U"Roots: To Spectrum", U"Roots: To Spectrum...") {
	POSITIVEVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	NATURALVAR (numberOfFrequencies, U"Number of frequencies (>1)", U"1025")
	OK
DO
	CONVERT_EACH (Roots)
		autoSpectrum result = Roots_to_Spectrum (me, maximumFrequency, numberOfFrequencies, 1.0);
	CONVERT_EACH_END (my name)
}

DIRECT (MODIFY_Roots_and_Polynomial_polish) {
	MODIFY_FIRST_OF_TWO (Roots, Polynomial)
		Roots_and_Polynomial_polish (me, you);
	MODIFY_FIRST_OF_TWO_END
}

/*****************************************************************************/

DIRECT (INFO_Praat_ReportFloatingPointProperties) {
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
	MelderInfo_writeLine (U"\nA long double is ", sizeof (long double), U" bytes");
	MelderInfo_close ();
END }

FORM (REAL_Praat_getTukeyQ, U"Get TukeyQ", nullptr) {
	POSITIVEVAR (criticalValue, U"Critical value", U"2.0")
	NATURALVAR (numberOfMeans, U"Number of means", U"3")
	POSITIVEVAR (degreesOfFreedon, U"Degrees of freedom", U"10.0")
	NATURALVAR (numberOfRows, U"Number of rows", U"1")
	OK
DO
	double result = NUMtukeyQ (criticalValue, numberOfMeans, degreesOfFreedon, numberOfRows);
	Melder_information (result, U" (tukeyQ)");
END }

FORM (REAL_Praat_getInvTukeyQ, U"Get invTukeyQ", nullptr) {
	REALVAR (probability, U"Probability", U"0.05")
	NATURALVAR (numberOfMeans, U"Number of means", U"3")
	POSITIVEVAR (degreesOfFreedon, U"Degrees of freedom", U"10.0")
	NATURALVAR (numberOfRows, U"Number of rows", U"1")
	OK
DO
	REQUIRE (probability >= 0 && probability <= 1, U"Probability must be in (0,1).")
	double result = NUMinvTukeyQ (probability, numberOfMeans, degreesOfFreedon, numberOfRows);
	Melder_information (result, U" (inv tukeyQ)");
END }

FORM (COMPLEX_Praat_getIncompleteGamma, U"Get incomplete gamma", U"Get incomplete gamma...") {
	POSITIVEVAR (reAlpha, U"Real part of alpha", U"4.0")
	REALVAR (imAlpha, U"Imaginary part of alpha", U"0.0")
	REALVAR (reX, U"Real part of X", U"4.0")
	REALVAR (imX, U"Imaginary part of X", U"0.0")
	OK
DO
	double result_re, result_im;
	NUMincompleteGammaFunction (reAlpha, imAlpha, reX, imX, & result_re, & result_im);
	Melder_information (result_re, U"+", result_im, U"i");
END }

/******************** Sound ****************************************/

#define Sound_create_addCommonFields(startTime,endTime,samplingFrequency) \
	REALVAR (startTime, U"Start time (s)", U"0.0") \
	REALVAR (endTime, U"End time (s)", U"1.0") \
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")


static void Sound_create_checkCommonFields (double startTime, double endTime, double samplingFrequency) {
	double numberOfSamples_real;
	numberOfSamples_real = round ( (endTime - startTime) * samplingFrequency);
	if (endTime <= startTime) {
		if (endTime == startTime) {
			Melder_throw (U"A Sound cannot have a duration of zero.");
		} else {
			Melder_throw (U"A Sound cannot have a duration less than zero.");
		}
		if (startTime == 0.0) {
			Melder_throw (U"Please set the finishing time to something greater than 0 seconds.");
		} else {
			Melder_throw (U"Please lower the starting time or raise the finishing time.");
		}
	}
	if (samplingFrequency <= 0.0)
		Melder_throw (U"A Sound cannot have a negative sampling frequency.\n"
		U"Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");

	if (numberOfSamples_real < 1.0) {
		Melder_appendError (U"A Sound cannot have zero samples.\n");
		if (startTime == 0.0) {
			Melder_throw (U"Please raise the finishing time.");
		} else {
			Melder_throw (U"Please lower the starting time or raise the finishing time.");
		}
	}
	if (numberOfSamples_real > LONG_MAX) {   // ppgb: kan niet in een 64-bit-omgeving
		Melder_throw (U"A Sound cannot have ", Melder_bigInteger ((long) numberOfSamples_real), U" samples; the maximum is ", Melder_bigInteger (LONG_MAX), U" samples.\n");
	}
}

FORM (NEW1_Sound_and_Pitch_to_FormantFilter, U"Sound & Pitch: To FormantFilter", U"Sound & Pitch: To Spectrogram...") {
	POSITIVEVAR (windowLength, U"Analysis window duration (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0");
	POSITIVEVAR (relativeBandwidth, U"Relative bandwidth", U"1.1")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoFormantFilter result = Sound_and_Pitch_to_FormantFilter (me, you, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Sound_and_Pitch_to_Spectrogram, U"Sound & Pitch: To Spectrogram", U"Sound & Pitch: To Spectrogram...") {
	POSITIVEVAR (windowLength, U"Analysis window duration (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0");
	POSITIVEVAR (relativeBandwidth, U"Relative bandwidth", U"1.1")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoSpectrogram result = Sound_and_Pitch_to_Spectrogram (me, you, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Sound_and_Pitch_changeGender, U"Sound & Pitch: Change gender", U"Sound & Pitch: Change gender...") {
	POSITIVEVAR (formantShiftRatio, U"Formant shift ratio", U"1.2")
	REALVAR (newPitchMedian, U"New pitch median (Hz)", U"0.0 (= no change)")
	POSITIVEVAR (pitchRangeFactor, U"Pitch range factor", U"1.0 (= no change)")
	POSITIVEVAR (durationFactor, U"Duration factor", U"1.0")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoSound result = Sound_and_Pitch_changeGender_old (me, you, formantShiftRatio, newPitchMedian, pitchRangeFactor, durationFactor);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Sound_and_Pitch_changeSpeaker, U"Sound & Pitch: Change speaker", U"Sound & Pitch: Change speaker...") {
	POSITIVEVAR (formantFrequencyMultiplicationFactor, U"Multiply formants by", U"1.1 (male->female)")
	POSITIVEVAR (pitchMultiplicationFactor, U"Multiply pitch by", U"1.8 (male->female")
	REALVAR (pitchRangeMultiplicationFactor, U"Multiply pitch range by", U"1.0 (= no change)")
	POSITIVEVAR (durationMultiplicationFactor, U"Multiply duration", U"1.0")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoSound result = Sound_and_Pitch_changeSpeaker (me, you, formantFrequencyMultiplicationFactor, pitchMultiplicationFactor, pitchRangeMultiplicationFactor, durationMultiplicationFactor);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Sound_and_IntervalTier_cutPartsMatchingLabel, U"Sound & IntervalTier: Cut parts matching label", nullptr) {
	SENTENCEVAR (label, U"Label", U"cut")
	OK
DO
	CONVERT_TWO (Sound, IntervalTier)
		autoSound result = Sound_and_IntervalTier_cutPartsMatchingLabel (me, you, label);
	CONVERT_TWO_END (my name, U"_cut")
}

FORM (NEW1_Sound_createAsGammaTone, U"Create a gammatone", U"Create Sound as gammatone...") {
	WORDVAR (name, U"Name", U"gammatone")
	Sound_create_addCommonFields (startTime, endTime, samplingFrequency)
	INTEGERVAR (gamma, U"Gamma", U"4")
	POSITIVEVAR (frequency, U"Frequency (Hz)", U"1000.0")
	POSITIVEVAR (bandwidth, U"Bandwidth (Hz)", U"150.0")
	REALVAR (initialPhase, U"Initial phase (radians)", U"0.0")
	REALVAR (additionFactor, U"Addition factor", U"0.0")
	BOOLEANVAR (scaleAmplitudes, U"Scale amplitudes", 1)
	OK
DO
	CREATE_ONE
		Sound_create_checkCommonFields (startTime, endTime, samplingFrequency);
		REQUIRE (frequency < samplingFrequency / 2, Melder_cat (U"Frequency cannot be larger than half the sampling frequency.\nPlease use a frequency smaller than ", 	samplingFrequency / 2, U"."))
		REQUIRE (gamma >= 0, U"Gamma cannot be negative.\nPlease use a positive or zero gamma.")
		autoSound result = Sound_createGammaTone (startTime, endTime, samplingFrequency, gamma, frequency, bandwidth, initialPhase, additionFactor, scaleAmplitudes);
	CREATE_ONE_END (name)
}

FORM (NEW_Sound_createAsShepardTone, U"Create a Shepard tone", U"Create Sound as Shepard tone...") {
	WORDVAR (name, U"Name", U"shepardTone")
	Sound_create_addCommonFields (startTime, endTime, samplingFrequency)
	POSITIVEVAR (minimumFrequency, U"Lowest frequency (Hz)", U"4.863")
	NATURALVAR (numberOfComponents, U"Number of components", U"10")
	REALVAR (frequencyChange, U"Frequency change (semitones/s)", U"4.0")
	POSITIVEVAR (amplitudeRange_dB, U"Amplitude range (dB)", U"30.0")
	REALVAR (octaveShiftFraction, U"Octave shift fraction ([0,1))", U"0.0")
	OK
DO
	CREATE_ONE
		Sound_create_checkCommonFields (startTime, endTime, samplingFrequency);
		autoSound result = Sound_createShepardToneComplex (startTime, endTime, samplingFrequency, minimumFrequency, numberOfComponents, frequencyChange, amplitudeRange_dB, octaveShiftFraction);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_Sound_drawWhere, U"Sound: Draw where", U"Sound: Draw where...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENUSTRVAR (drawingMethod, U"Drawing method", 1)
		OPTION (U"Curve")
		OPTION (U"Bars")
		OPTION (U"Poles")
		OPTION (U"Speckles")
	LABEL (U"", U"Draw only those parts where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"x < xmin + (xmax - xmin) / 2; first half")
	OK
DO
	long numberOfBisections = 10;
	GRAPHICS_EACH (Sound)
		Sound_drawWhere (me, GRAPHICS, fromTime, toTime, ymin, ymax, garnish, drawingMethod, numberOfBisections, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (PLAY_Sound_playOneChannel, U"Sound: Play one channel", nullptr) {
    NATURALVAR (channel, U"Channel", U"1")
    OK
DO
    LOOP {
        iam (Sound);
		if (channel > my ny) {
			Melder_throw (me, U": there is no channel ", channel, U". Sound has only ", my ny, U" channel",
				  (my ny > 1 ? U"s." : U"."));
        }
        autoSound thee = Sound_extractChannel (me, channel);
        Sound_play (thee.get(), 0, 0);
    }
END }

FORM (PLAY_Sound_playAsFrequencyShifted, U"Sound: Play as frequency shifted", U"Sound: Play as frequency shifted...") {
	REALVAR (frequencyShift, U"Shift by (Hz)", U"1000.0")
	POSITIVEVAR (samplingFrequency, U"New sampling frequency (Hz)", U"44100.0")
	NATURALVAR (samplePrecision, U"Precision (samples)", U"50")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_playAsFrequencyShifted (me, frequencyShift, samplingFrequency, samplePrecision);
	}
END }

FORM (NEW1_Sounds_to_DTW, U"Sounds: To DTW", nullptr) {
    POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
    POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
    LABEL (U"", U"")
    REALVAR (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.1")
    RADIOVAR (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_COUPLE (Sound)
		autoDTW result = Sounds_to_DTW (me, you, windowLength, timeStep, sakoeChibaBand, slopeConstraint);
   CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW_Sound_to_TextGrid_detectSilences, U"Sound: To TextGrid (silences)", U"Sound: To TextGrid (silences)...") {
	LABEL (U"", U"Parameters for the intensity analysis")
	POSITIVEVAR (minimumPitch, U"Minimum pitch (Hz)", U"100")
	REALVAR (timeStep, U"Time step (s)", U"0.0 (= auto)")
	LABEL (U"", U"Silent intervals detection")
	REALVAR (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVEVAR (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVEVAR (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
	WORDVAR (silenceLabel, U"Silent interval label", U"silent")
	WORDVAR (soundingLabel, U"Sounding interval label", U"sounding")
	OK
DO
	CONVERT_EACH (Sound)
		autoTextGrid result = Sound_to_TextGrid_detectSilences (me, minimumPitch, timeStep, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_copyChannelRanges, U"Sound: Copy channel ranges", nullptr) {
	LABEL (U"", U"Create a new Sound from the following channels:")
	TEXTVAR (channels_string, U"Ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_copyChannelRanges (me, channels_string);
	CONVERT_EACH_END (my name, U"_channels")
}

FORM (NEW_Sound_trimSilences, U"Sound: Trim silences", U"Sound: Trim silences...") {
    REALVAR (trimDuration, U"Trim duration (s)", U"0.08")
	BOOLEANVAR (onlyAtStartAndEnd, U"Only at start and end", true);
	LABEL (U"", U"Parameters for the intensity analysis")
	POSITIVEVAR (minimumPitch, U"Minimum pitch (Hz)", U"100")
	REALVAR (timeStep, U"Time step (s)", U"0.0 (= auto)")
	LABEL (U"", U"Silent intervals detection")
	REALVAR (silenceThreshold, U"Silence threshold (dB)", U"-35.0")
	POSITIVEVAR (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVEVAR (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
    BOOLEANVAR (saveTextGrid, U"Save trimming info as TextGrid", false)
    WORDVAR (trim_string, U"Trim label", U"trimmed")
	OK
DO
    trimDuration = trimDuration < 0.0 ? 0.0 : trimDuration;
	CONVERT_EACH (Sound)
        autoTextGrid tg;
		autoSound result = Sound_trimSilences (me, trimDuration, onlyAtStartAndEnd, minimumPitch, timeStep, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, (saveTextGrid ? &tg : nullptr ), trim_string);
		if (saveTextGrid) {
            praat_new (tg.move(), my name, U"_trimmed");
        }
	CONVERT_EACH_END (my name, U"_trimmed")
}

// deprecated
FORM (NEW_Sound_to_BarkFilter, U"Sound: To BarkFilter", U"Sound: To BarkSpectrogram...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (bark)", U"1.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (bark)", U"1.0")
	REALVAR (maximumFrequency, U"Maximum frequency (bark)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoBarkFilter result = Sound_to_BarkFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_BarkSpectrogram, U"Sound: To BarkSpectrogram", U"Sound: To BarkSpectrogram...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (bark)", U"1.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (bark)", U"1.0")
	REALVAR (maximumFrequency, U"Maximum frequency (bark)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoBarkSpectrogram result = Sound_to_BarkSpectrogram (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name)
}

// deprecated
FORM (NEW_Sound_to_FormantFilter, U"Sound: To FormantFilter", U"Sound: To FormantFilter...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0.0");
	POSITIVEVAR (relativeBandwidth, U"Relative bandwidth", U"1.1")
	LABEL (U"", U"Pitch analysis")
	REALVAR (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	REALVAR (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormantFilter result = Sound_to_FormantFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth, minimumPitch, maximumPitch);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_Spectrogram_pitchDependent, U"Sound: To Spectrogram (pitch-dependent)", U"Sound: To Spectrogram (pitch-dependent)...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0.0");
	POSITIVEVAR (relativeBandwidth, U"Relative bandwidth", U"1.1")
	LABEL (U"", U"Pitch analysis")
	REALVAR (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	REALVAR (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSpectrogram result = Sound_to_Spectrogram_pitchDependent (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth, minimumPitch, maximumPitch);
	CONVERT_EACH_END (my name)
}

// deprecated
FORM (NEW_Sound_to_MelFilter, U"Sound: To MelFilter", U"Sound: To MelFilter...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (mel)", U"100.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (mel)", U"100.0")
	REALVAR (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoMelFilter result = Sound_to_MelFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_MelSpectrogram, U"Sound: To MelSpectrogram", U"Sound: To MelSpectrogram...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFrequency, U"Position of first filter (mel)", U"100.0")
	POSITIVEVAR (deltaFrequency, U"Distance between filters (mel)", U"100.0")
	REALVAR (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoMelSpectrogram result = Sound_to_MelSpectrogram (me, windowLength, timeStep,  firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_ComplexSpectrogram, U"Sound: To ComplexSpectrogram", nullptr) {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step", U"0.005")
	OK
DO
	CONVERT_EACH (Sound)
		autoComplexSpectrogram result = Sound_to_ComplexSpectrogram (me, windowLength, timeStep);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_Pitch_shs, U"Sound: To Pitch (shs)", U"Sound: To Pitch (shs)...") {
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.01")
	POSITIVEVAR (pitchFloor, U"Minimum pitch (Hz)", U"50.0")
	NATURALVAR (maximumNumberOfCandidates, U"Max. number of candidates (Hz)", U"15")
	LABEL (U"", U"Algorithm parameters")
	POSITIVEVAR (maximumFrequency, U"Maximum frequency component (Hz)", U"1250.0")
	NATURALVAR (maximumNumberOfSubharmonics, U"Max. number of subharmonics", U"15")
	POSITIVEVAR (compressionFactor, U"Compression factor (<=1)", U"0.84")
	POSITIVEVAR (pitchCeiling, U"Ceiling (Hz)", U"600.0")
	NATURALVAR (numberOfPointsPerOctave, U"Number of points per octave", U"48");
	OK
DO
	REQUIRE (pitchFloor < pitchCeiling, U"Minimum pitch should be smaller than ceiling.")
	REQUIRE (pitchCeiling < maximumFrequency, U"Maximum frequency must be greater than or equal to ceiling.")
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch_shs (me, timeStep, pitchFloor, maximumFrequency, pitchCeiling, maximumNumberOfSubharmonics, maximumNumberOfCandidates, compressionFactor, numberOfPointsPerOctave);
	CONVERT_EACH_END (my name)
}

FORM (MODIFY_Sound_fadeIn, U"Sound: Fade in", U"Sound: Fade in...") {
	CHANNELVAR (channel, U"Channel (number, 0 = (all))", U"1")
	REALVAR (time, U"Time (s)", U"-10000.0")
	REALVAR (fadeTime, U"Fade time (s)", U"0.005")
	BOOLEANVAR (silentFromStart, U"Silent from start", false)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_fade (me, channel, time, fadeTime, -1, silentFromStart);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_fadeOut, U"Sound: Fade out", U"Sound: Fade out...") {
	CHANNELVAR (channel, U"Channel (number, 0 = (all))", U"1")
	REALVAR (time, U"Time (s)", U"10000.0")
	REALVAR (fadeTime, U"Fade time (s)", U"-0.005")
	BOOLEANVAR (silentToEnd, U"Silent to end", false)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_fade (me, channel, time, fadeTime, 1, silentToEnd);
	MODIFY_EACH_END
}

FORM (NEW_Sound_to_KlattGrid_simple, U"Sound: To KlattGrid (simple)", U"Sound: To KlattGrid (simple)...") {
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Formant determination")
	NATURALVAR (numberOfFormants, U"Max. number of formants", U"5")
	POSITIVEVAR (maximumFormant, U"Maximum formant (Hz)", U"5500 (= adult female)")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	LABEL (U"", U"Pitch determination")
	POSITIVEVAR (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVEVAR (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"", U"Intensity determination")
	POSITIVEVAR (minimumPitch, U"Minimum pitch (Hz)", U"100.0")
	BOOLEANVAR (subtractMean, U"Subtract mean", true)
	OK
DO
	CONVERT_EACH (Sound)
		autoKlattGrid result = Sound_to_KlattGrid_simple (me, timeStep, numberOfFormants, maximumFormant, windowLength, preEmphasisFrequency, pitchFloor, pitchCeiling, minimumPitch, subtractMean);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_Pitch_SPINET, U"Sound: To SPINET", U"Sound: To SPINET...") {
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.040")
	LABEL (U"", U"Gammatone filter bank")
	POSITIVEVAR (minimumFrequency, U"Minimum filter frequency (Hz)", U"70.0")
	POSITIVEVAR (maximumFrequency, U"Maximum filter frequency (Hz)", U"5000.0")
	NATURALVAR (numberOfFilters, U"Number of filters", U"250");
	POSITIVEVAR (pitchCeiling, U"Ceiling (Hz)", U"500.0")
	NATURALVAR (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	OK
DO
	REQUIRE (minimumFrequency < maximumFrequency, U"Maximum frequency must be larger than minimum frequency.")
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch_SPINET (me, timeStep, windowLength, minimumFrequency, maximumFrequency, numberOfFilters, pitchCeiling, maximumNumberOfCandidates);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_Polygon, U"Sound: To Polygon", U"Sound: To Polygon...") {
	CHANNELVAR (channel, U"Channel (number, Left, or Right)", U"1")
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (connectionY, U"Connection y-value", U"0.0")
	OK
DO
	CONVERT_EACH (Sound)
		channel = channel > my ny ? 1 : channel;
		autoPolygon result = Sound_to_Polygon (me, channel, fromTime, toTime, ymin, ymax, connectionY);
	CONVERT_EACH_END (my name)
}

FORM (NEW1_Sounds_to_Polygon_enclosed, U"Sounds: To Polygon (enclosed)", U"Sounds: To Polygon (enclosed)...") {
	CHANNELVAR (channel, U"Channel (number, Left, or Right)", U"1")
		OPTION (U"Left")
		OPTION (U"Right")
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	OK
DO
	CONVERT_COUPLE (Sound)
		autoPolygon result = Sounds_to_Polygon_enclosed (me, you, channel, fromTime, toTime, ymin, ymax);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW_Sound_filterByGammaToneFilter4, U"Sound: Filter (gammatone)", U"Sound: Filter (gammatone)...") {
	POSITIVEVAR (centreFrequency, U"Centre frequency (Hz)", U"1000.0")
	POSITIVEVAR (bandwidth, U"Bandwidth (Hz)", U"150.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filterByGammaToneFilter4 (me, centreFrequency, bandwidth);
	CONVERT_EACH_END (my name, U"_filtered")
}

FORM (NEW_Sound_removeNoise, U"Sound: Remove noise", U"Sound: Remove noise...") {
	REALVAR (fromTime, U"left Noise time range (s)", U"0.0")
	REALVAR (toTime, U"right Noise time range (s)", U"0.0")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	LABEL (U"", U"Filter")
	REALVAR (fromFrequency, U"left Filter frequency range (Hz)", U"80.0")
	REALVAR (toFrequency, U"right Filter frequency range (Hz)", U"10000.0")
	POSITIVEVAR (smoothingBandwidth, U"Smoothing bandwidth, (Hz)", U"40.0")
	OPTIONMENUVAR (noiseReductionMethod, U"Noise reduction method", 1)
		OPTION (U"Spectral subtraction")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_removeNoise (me, fromTime, toTime, windowLength, fromFrequency, toFrequency, smoothingBandwidth, noiseReductionMethod);
	CONVERT_EACH_END (my name, U"_denoised")
}

FORM (NEW_Sound_changeSpeaker, U"Sound: Change speaker", U"Sound: Change speaker...") {
	LABEL (U"", U"Pitch measurement parameters")
	POSITIVEVAR (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVEVAR (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"", U"Modification parameters")
	POSITIVEVAR (formantMultiplicationFactor, U"Multiply formants by", U"1.2")
	POSITIVEVAR (pitchMultiplicationFactor, U"Multiply pitch by", U"1.0")
	REALVAR (pitchRangeMultiplicationFactor, U"Multiply pitch range by", U"1.0 (= no change)")
	POSITIVEVAR (durationMultiplicationFactor, U"Multiply duration by", U"1.0")
	OK
DO
	REQUIRE (pitchFloor < pitchCeiling, U"Maximum pitch should be greater than minimum pitch.")
	CONVERT_EACH (Sound)
		autoSound result = Sound_changeSpeaker (me, pitchFloor, pitchCeiling, formantMultiplicationFactor, pitchMultiplicationFactor, pitchRangeMultiplicationFactor, durationMultiplicationFactor);
	CONVERT_EACH_END (my name, U"_changeSpeaker")
}

FORM (NEW_Sound_changeGender, U"Sound: Change gender", U"Sound: Change gender...") {
	LABEL (U"", U"Pitch measurement parameters")
	POSITIVEVAR (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVEVAR (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"", U"Modification parameters")
	POSITIVEVAR (formantShiftRatio, U"Formant shift ratio", U"1.2")
	REALVAR (pitchMedian, U"New pitch median (Hz)", U"0.0 (= no change)")
	REALVAR (pitchRangeMultiplicationFactor, U"Pitch range factor", U"1.0 (= no change)")
	POSITIVEVAR (durationMultiplicationFactor, U"Duration factor", U"1.0")
	OK
DO
	REQUIRE (pitchFloor < pitchCeiling, U"Maximum pitch should be greater than minimum pitch.")
	CONVERT_EACH (Sound)
		autoSound result = Sound_changeGender_old (me, pitchFloor, pitchCeiling, formantShiftRatio, pitchMedian, pitchRangeMultiplicationFactor, durationMultiplicationFactor);
	CONVERT_EACH_END (my name, U"_changeGender");
}

FORM (GRAPHICS_Sound_paintWhere, U"Sound paint where", U"Sound: Paint where...") {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (level, U"Fill from level", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Paint only those parts where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; always")
	OK
DO
	Graphics_Colour colour = GET_COLOUR (U"Colour");
	long numberOfBisections = 10;
	GRAPHICS_EACH (Sound)
		Sound_paintWhere (me, GRAPHICS, colour, fromTime, toTime, ymin, ymax, level, garnish, numberOfBisections, formula,
		interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Sounds_paintEnclosed, U"Sounds paint enclosed", U"Sounds: Paint enclosed...") {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REALVAR (xFromTime, U"left Horizontal time range (s)", U"0.0")
	REALVAR (xToTime, U"right Horizontal time range (s)", U"0.0")
	REALVAR (yFromTime, U"left Vertical time range", U"0.0")
	REALVAR (yToTime, U"right Vertical time range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	Graphics_Colour colour = GET_COLOUR (U"Colour");
	GRAPHICS_COUPLE (Sound)
		Sounds_paintEnclosed (me, you, GRAPHICS, colour, xFromTime, xToTime, yFromTime, yToTime, garnish);
	GRAPHICS_COUPLE_END
}

FORM_READ (READ1_Sound_readFromRawFileLE, U"Read Sound from raw Little Endian file", nullptr, true) {
	autoSound thee = Sound_readFromRawFile (file, nullptr, 16, 1, 0, 0, 16000.0);
	praat_new (thee.move(), MelderFile_name (file));
END }

FORM_READ (READ1_Sound_readFromRawFileBE, U"Read Sound from raw 16-bit Little Endian file", nullptr, true) {
	autoSound thee = Sound_readFromRawFile (file, nullptr, 16, 0, 0, 0, 16000.0);
	praat_new (thee.move(), MelderFile_name (file));
END }

FORM_READ (READ1_KlattTable_readFromRawTextFile, U"KlattTable_readFromRawTextFile", nullptr, true) {
	autoKlattTable thee = KlattTable_readFromRawTextFile (file);
	praat_new (thee.move(), MelderFile_name (file));
END }

/************ Spectrograms *********************************************/

FORM (NEW1_Spectrograms_to_DTW, U"Spectrograms: To DTW", nullptr) {
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (Spectrogram)
		autoDTW result = Spectrograms_to_DTW (me, you, matchStart, matchEnd, slopeConstraint, 1.0);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

/**************** Spectrum *******************************************/

FORM (GRAPHICS_Spectrum_drawPhases, U"Spectrum: Draw phases", U"Spectrum: Draw phases...") {
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0")
	REALVAR (fromPhase, U"Minimum phase (dB/Hz)", U"0.0 (= auto)")
	REALVAR (toPhase, U"Maximum phase (dB/Hz)", U"0.0 (= auto)")
	BOOLEANVAR (unwrap, U"Unwrap", 1)
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_drawPhases (me, GRAPHICS, fromFrequency, toFrequency, fromPhase, toPhase, unwrap, garnish);
	GRAPHICS_EACH_END
}

FORM (MODIFY_Spectrum_setRealValueInBin, U"Spectrum: Set real value in bin", nullptr) {
	NATURALVAR (binNumber, U"Bin number", U"100")
	REALVAR (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		REQUIRE (binNumber <= my nx, U"Bin number must not exceed number of bins.")
		my z[1][binNumber]= value;
	MODIFY_EACH_END
}

FORM (MODIFY_Spectrum_setImaginaryValueInBin, U"Spectrum: Set imaginary value in bin", nullptr) {
	NATURALVAR (binNumber, U"Bin number", U"100")
	REALVAR (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		REQUIRE (binNumber <= my nx, U"Bin number must not exceed number of bins.")
		my z[2][binNumber]= value;
	MODIFY_EACH_END
}

DIRECT (MODIFY_Spectrum_conjugate) {
	MODIFY_EACH (Spectrum)
		Spectrum_conjugate (me);
	MODIFY_EACH_END
}

FORM (NEW_Spectrum_shiftFrequencies, U"Spectrum: Shift frequencies", U"Spectrum: Shift frequencies...") {
	REALVAR (frequencyShift, U"Shift by (Hz)", U"1000.0")
	POSITIVEVAR (maximumFrequency, U"New maximum frequency (Hz)", U"22050")
	NATURALVAR (interpolationDepth, U"Interpolation depth", U"50")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_shiftFrequencies (me, frequencyShift, maximumFrequency, interpolationDepth);
	CONVERT_EACH_END (my name, (frequencyShift < 0 ? U"_m" : U"_"), (long) floor (frequencyShift))
}

DIRECT (NEW_Spectra_multiply) {
	CONVERT_COUPLE (Spectrum)
		autoSpectrum result = Spectra_multiply (me, you);
	CONVERT_COUPLE_END (my name, U"_x_", your name)
}

FORM (NEW_Spectrum_resample, U"Spectrum: Resample", nullptr) {
	NATURALVAR (numberOfFrequencies, U"New number of frequencies", U"256")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_resample (me, numberOfFrequencies);
	CONVERT_EACH_END (my name, U"_", numberOfFrequencies)
}

FORM (NEW_Spectrum_compressFrequencyDomain, U"Spectrum: Compress frequency domain", nullptr) {
	POSITIVEVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	INTEGERVAR (interpolationDepth, U"Interpolation depth", U"50")
	RADIOVAR (scale, U"Interpolation scale", 1)
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Logarithmic")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_compressFrequencyDomain (me, maximumFrequency, interpolationDepth, scale, 1);
	CONVERT_EACH_END (my name, U"_", (long) floor (maximumFrequency))
}

DIRECT (NEW_Spectrum_unwrap) {
	CONVERT_EACH (Spectrum)
		autoMatrix result = Spectrum_unwrap (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Spectrum_to_PowerCepstrum) {
	CONVERT_EACH (Spectrum)
		autoPowerCepstrum result = Spectrum_to_PowerCepstrum (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Spectrum_to_Cepstrum) {
	CONVERT_EACH (Spectrum)
		autoCepstrum result = Spectrum_to_Cepstrum (me);
	CONVERT_EACH_END (my name)
}

/************* SpeechSynthesizer *************************************************/

DIRECT (HELP_SpeechSynthesizer_help) {
	HELP (U"SpeechSynthesizer")
}

FORM (NEW1_SpeechSynthesizer_create, U"Create SpeechSynthesizer", U"Create SpeechSynthesizer...") {
	/* 
	 * In the speech synthesis world a language variant is called a "voice", we use the same terminology 
	 * in our coding. However for the user interface we use "language" instead of "voice".
	 */
	static long prefLanguage = Strings_findString (espeakdata_voices_names.get(), U"English");
	if (prefLanguage == 0) {
		prefLanguage = 1;
	}
	// LIST does not scroll to the line with "prefLanguage"
	LISTVAR (languageIndex, U"Language", espeakdata_voices_names -> numberOfStrings, (const char32 **) espeakdata_voices_names -> strings, prefLanguage)
	static long prefVoiceVariant = Strings_findString (espeakdata_variants_names.get(), U"default");
	LISTVAR (voiceVariantIndex, U"Voice variant", espeakdata_variants_names -> numberOfStrings,
		(const char32 **) espeakdata_variants_names -> strings, prefVoiceVariant)
	OK
DO
	CREATE_ONE
		autoSpeechSynthesizer result = SpeechSynthesizer_create (espeakdata_voices_names -> strings[languageIndex], espeakdata_variants_names -> strings[voiceVariantIndex]);
    CREATE_ONE_END (espeakdata_voices_names -> strings[languageIndex], U"_", espeakdata_variants_names -> strings[voiceVariantIndex])
}

FORM (PLAY_SpeechSynthesizer_playText, U"SpeechSynthesizer: Play text", U"SpeechSynthesizer: Play text...") {
	TEXTVAR (text, U"Text", U"This is some text.")
	OK
DO
	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_playText (me, text);
	}
END }

FORM (NEWMANY_SpeechSynthesizer_to_Sound, U"SpeechSynthesizer: To Sound", U"SpeechSynthesizer: To Sound...") {
	TEXTVAR (text, U"Text", U"This is some text.")
	BOOLEANVAR (wantTextGrid, U"Create TextGrid with annotations", false);
	OK
DO
	CONVERT_EACH (SpeechSynthesizer)
		autoTextGrid tg;
		autoTable t;
		autoSound result = SpeechSynthesizer_to_Sound (me, text, (wantTextGrid ? & tg : nullptr), (Melder_debug == -2 ? & t : nullptr ));
		if (wantTextGrid) {
			praat_new (tg.move(), my name);
		}
		if (Melder_debug == -2) {
			praat_new (t.move(), my name);
		}
	CONVERT_EACH_END (my name)
}

DIRECT (INFO_SpeechSynthesizer_getVoiceName) {
	STRING_ONE (SpeechSynthesizer)
		const char32 *result = my d_voiceLanguageName;
	STRING_ONE_END
}

DIRECT (INFO_SpeechSynthesizer_getVoiceVariant) {
	STRING_ONE (SpeechSynthesizer)
		const char32 *result = my d_voiceVariantName;
	STRING_ONE_END
}

FORM (MODIFY_SpeechSynthesizer_setTextInputSettings, U"SpeechSynthesizer: Set text input settings", U"SpeechSynthesizer: Set text input settings...") {
	OPTIONMENUVAR (inputTextFormat, U"Input text format is", 1)
		OPTION (U"Text only")
		OPTION (U"Phoneme codes only")
		OPTION (U"Mixed with tags")
	OPTIONMENUVAR (inputPhonemeCoding, U"Input phoneme codes are", 1)
		OPTION (U"Kirshenbaum_espeak")
	OK
DO
	int inputPhonemeCoding_always = SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM;
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setTextInputSettings (me, inputTextFormat, inputPhonemeCoding_always);
	MODIFY_EACH_END
}

FORM (MODIFY_SpeechSynthesizer_setSpeechOutputSettings, U"SpeechSynthesizer: Set speech output settings", U"SpeechSynthesizer: Set speech output settings...") {
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	REALVAR (wordGap, U"Gap between words (s)", U"0.01")
	INTEGERVAR (pitchAdjustment, U"Pitch adjustment (0-99)", U"50")
	INTEGERVAR (pitchRange_0_99, U"Pitch range (0-99)", U"50");
	NATURALVAR (wordsPerMinute, U"Words per minute (80-450)", U"175");
	BOOLEANVAR (estimateWordsPerMinute, U"Estimate rate from data", true);
	OPTIONMENUVAR (outputPhonemeCodes, U"Output phoneme codes are", 2)
		OPTION (U"Kirshenbaum_espeak")
		OPTION (U"IPA")
	OK
DO
	if (wordGap < 0) wordGap = 0;
	if (pitchAdjustment < 0) pitchAdjustment = 0;
	if (pitchAdjustment > 99) pitchAdjustment = 99;
	if (pitchRange_0_99 < 0) pitchRange_0_99 = 0;
	if (pitchRange_0_99 > 99) pitchRange_0_99 = 99;
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setSpeechOutputSettings (me, samplingFrequency, wordGap, pitchAdjustment, pitchRange_0_99, wordsPerMinute, estimateWordsPerMinute, outputPhonemeCodes);
	MODIFY_EACH_END
}

/************* SpeechSynthesizer and TextGrid ************************/

FORM (NEW1_SpeechSynthesizer_and_TextGrid_to_Sound, U"SpeechSynthesizer & TextGrid: To Sound", nullptr) {
	NATURALVAR (tierNumber, U"Tier number", U"1")
	NATURALVAR (intervalNumber, U"Interval number", U"1")
	BOOLEANVAR (createAnnotations, U"Create TextGrid with annotations", false);
	OK
DO
	CONVERT_TWO (SpeechSynthesizer, TextGrid)
		autoTextGrid annotations;
		autoSound result = SpeechSynthesizer_and_TextGrid_to_Sound (me, you, tierNumber, intervalNumber, (createAnnotations ? & annotations : nullptr ));
		if (createAnnotations) {
			praat_new (annotations.move(), my name);
		}
	CONVERT_TWO_END (my name)
}

FORM (NEW1_SpeechSynthesizer_and_Sound_and_TextGrid_align, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align)", nullptr) {
	NATURALVAR (tierNumber, U"Tier number", U"1")
	NATURALVAR (fromInterval, U"From interval number", U"1")
	NATURALVAR (toInterval, U"To interval number", U"1")
	REALVAR (silenceThreshold_dB, U"Silence threshold (dB)", U"-35.0")
	POSITIVEVAR (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVEVAR (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
	OK
DO
	CONVERT_THREE (SpeechSynthesizer, Sound, TextGrid)
		autoTextGrid result = SpeechSynthesizer_and_Sound_and_TextGrid_align (me, you, him, tierNumber, fromInterval, toInterval, silenceThreshold_dB, minimumSilenceDuration, minimumSoundingDuration);
	CONVERT_THREE_END (your name, U"_aligned")
}

FORM (NEW1_SpeechSynthesizer_and_Sound_and_TextGrid_align2, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align, trim)", nullptr) {
    NATURALVAR (tierNumber, U"Tier number", U"1")
    NATURALVAR (fromInterval, U"From interval number", U"1")
    NATURALVAR (toInterval, U"To interval number", U"1")
    REALVAR (silenceThreshold_dB, U"Silence threshold (dB)", U"-35.0")
    POSITIVEVAR (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
    POSITIVEVAR (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
    REALVAR (trimDuration, U"Silence trim duration (s)", U"0.08")
    OK
DO
   trimDuration = trimDuration < 0.0 ? 0.0 : trimDuration;
    CONVERT_THREE (SpeechSynthesizer, Sound, TextGrid)
		autoTextGrid result = SpeechSynthesizer_and_Sound_and_TextGrid_align2 (me, you, him, tierNumber, fromInterval, toInterval, silenceThreshold_dB, minimumSilenceDuration, minimumSoundingDuration, trimDuration);
    CONVERT_THREE_END (his name, U"_aligned")
}

/************* Spline *************************************************/

FORM (GRAPHICS_Spline_drawKnots, U"Spline: Draw knots", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spline)
		Spline_drawKnots (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_Spline_getOrder) {
	INTEGER_ONE (Spline)
		long result = Spline_getOrder (me);
	INTEGER_ONE_END (U" (order)")
}

FORM (NEW_Spline_scaleX, U"Spline: Scale x", U"Spline: Scale x...") {
	LABEL (U"", U"New domain")
	REALVAR (xmin, U"Xmin", U"-1.0")
	REALVAR (xmax, U"Xmax", U"1.0")
	OK
DO
	REQUIRE (xmin < xmax, U"Xmin must be smaller than Xmax.")
	CONVERT_EACH (Spline)
		autoSpline result = Spline_scaleX (me, xmin, xmax);
	CONVERT_EACH_END (my name, U"_scaleX")
}

/************ SSCP ***************************************************/

DIRECT (HELP_SSCP_help) {
	HELP (U"SSCP")
}

FORM (GRAPHICS_SSCP_drawConfidenceEllipse, U"SSCP: Draw confidence ellipse", nullptr) {
	POSITIVEVAR (confidenceLevel, U"Confidence level", U"0.95")
	NATURALVAR (xIndex, U"Index for X-axis", U"1")
	NATURALVAR (yIndex, U"Index for Y-axis", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SSCP)
		SSCP_drawConcentrationEllipse (me, GRAPHICS, confidenceLevel, 1, xIndex, yIndex, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_SSCP_drawSigmaEllipse, U"SSCP: Draw sigma ellipse", U"SSCP: Draw sigma ellipse...") {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	NATURALVAR (xIndex, U"Index for X-axis", U"1")
	NATURALVAR (yIndex, U"Index for Y-axis", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SSCP)
		SSCP_drawConcentrationEllipse (me, GRAPHICS, numberOfSigmas, 0, xIndex, yIndex, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_SSCP_extractCentroid) {
	CONVERT_EACH (SSCP)
		autoTableOfReal result = SSCP_extractCentroid (me);
	CONVERT_EACH_END (my name, U"_centroid")
}

FORM (REAL_SSCP_getConfidenceEllipseArea, U"SSCP: Get confidence ellipse area", U"SSCP: Get confidence ellipse area...") {
	POSITIVEVAR (confidenceLevel, U"Confidence level", U"0.95")
	NATURALVAR (xIndex, U"Index for X-axis", U"1")
	NATURALVAR (yIndex, U"Index for Y-axis", U"2")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = SSCP_getConcentrationEllipseArea (me, confidenceLevel, 1, xIndex, yIndex);
	NUMBER_ONE_END (U" (confidence ellipse area)")
}

FORM (REAL_SSCP_getFractionVariation, U"SSCP: Get fraction variation", U"SSCP: Get fraction variation...") {
	NATURALVAR (fromDimension, U"From dimension", U"1")
	NATURALVAR (toDimension, U"To dimension", U"1")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = SSCP_getFractionVariation (me, fromDimension, toDimension);
	NUMBER_ONE_END (U" (fraction)")
}


FORM (REAL_SSCP_getConcentrationEllipseArea, U"SSCP: Get sigma ellipse area", U"SSCP: Get sigma ellipse area...") {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	NATURALVAR (xIndex, U"Index for X-axis", U"1")
	NATURALVAR (yIndex, U"Index for Y-axis", U"2")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = SSCP_getConcentrationEllipseArea (me, numberOfSigmas, 0, xIndex, yIndex);
	NUMBER_ONE_END (U" (concentation ellipse area)")
}

DIRECT (INTEGER_SSCP_getDegreesOfFreedom) {
	INTEGER_ONE (SSCP)
		long result = SSCP_getDegreesOfFreedom (me);
	INTEGER_ONE_END (U" (degrees of freedom)")
}

DIRECT (INTEGER_SSCP_getNumberOfObservations) {
	INTEGER_ONE (SSCP)
		long result = (long) floor (my numberOfObservations);   // ppgb: blijf ik raar vinden
	INTEGER_ONE_END (U" (number of observations)")
}

DIRECT (REAL_SSCP_getTotalVariance) {
	NUMBER_ONE (SSCP)
		double result = SSCP_getTotalVariance (me);
	NUMBER_ONE_END (U" (total variance)")
}

FORM (REAL_SSCP_getCentroidElement, U"SSCP: Get centroid element", U"SSCP: Get centroid element") {
	NATURALVAR (number, U"Number", U"1")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = undefined;
		if (number > 0 && number <= my numberOfColumns) {
			result = my centroid [number];
		}
	NUMBER_ONE_END (U" (centroid [", number, U"])")
}

DIRECT (REAL_SSCP_getLnDeterminant) {
	NUMBER_ONE (SSCP)
		double result = SSCP_getLnDeterminant (me);
	NUMBER_ONE_END (U" (ln (determinant))")
}

FORM (REAL_SSCP_testDiagonality_bartlett, U"SSCP: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...") {
	NATURALVAR (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	NUMBER_ONE (SSCP)
		double chisq, result, df;
		SSCP_testDiagonality_bartlett (me, numberOfConstraints, & chisq, & result, & df);
	NUMBER_ONE_END (U" (= probability for chisq = ", chisq, U" and ndf = ", df, U")")
}

DIRECT (NEW_SSCP_to_Correlation) {
	CONVERT_EACH (SSCP)
		autoCorrelation result = SSCP_to_Correlation (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_SSCP_to_Covariance, U"SSCP: To Covariance", U"SSCP: To Covariance...") {
	NATURALVAR (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	CONVERT_EACH (SSCP)
		autoCovariance result = SSCP_to_Covariance (me, numberOfConstraints);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_SSCP_to_PCA) {
	CONVERT_EACH (SSCP)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_END (my name)
}

/******************* Strings ****************************/

//DIRECT (NEW1_Strings_createAsEspeakVoices) {
//	//praat_new (nullptr, U"voices"); // TODO ??
//END }

FORM (NEW1_Strings_createAsCharacters, U"Strings: Create as characters", nullptr) {
	SENTENCEVAR (text, U"Text", U"intention")
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsCharacters (text);
	CREATE_ONE_END (U"chars")
}

FORM (NEW1_old_Strings_createAsTokens, U"Strings: Create as tokens", nullptr) {
	TEXTVAR (text, U"Text", U"There are seven tokens in this text")
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsTokens (text, U" ");
	CREATE_ONE_END (U"tokens")
}

FORM (NEW1_Strings_createAsTokens, U"Strings: Create as tokens", U"Create Strings as tokens...") {
	TEXTVAR (text, U"Text", U"There are seven tokens in this text")
	SENTENCEVAR (separators, U"Separators", U" ,")
	OK
DO_ALTERNATIVE (NEW1_old_Strings_createAsTokens)
	CREATE_ONE
		autoStrings result = Strings_createAsTokens (text, separators);
	CREATE_ONE_END (U"tokens")
}

DIRECT (NEW1_Strings_append) {
	CONVERT_LIST (Strings)
		autoStrings result = Strings_append (& list);
	CONVERT_LIST_END (U"appended")
}

DIRECT (NEW_Strings_to_Categories) {
	CONVERT_EACH (Strings)
		autoCategories result = Strings_to_Categories (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Strings_change, U"Strings: Change", U"Strings: Change") {
	SENTENCEVAR (search_string, U"Search", U"a")
	SENTENCEVAR (replace_string, U"Replace", U"a")
	INTEGERVAR (replaceLimit, U"Replace limit", U"0 (= unlimited)")
	RADIOVAR (stringType, U"Search and replace are:", 1)
	RADIOBUTTON (U"Literals")
	RADIOBUTTON (U"Regular Expressions")
	OK
DO
	CONVERT_EACH (Strings)
		long nmatches, nstringmatches;
		autoStrings result = Strings_change (me, search_string, replace_string, replaceLimit, &nmatches, &nstringmatches, stringType - 1);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Strings_extractPart, U"Strings: Extract part", nullptr) {
	NATURALVAR (fromIndex, U"From index", U"1")
	NATURALVAR (toIndex, U"To index", U"1")
	OK
DO
	CONVERT_EACH (Strings)
		autoStrings result = Strings_extractPart (me, fromIndex, toIndex);
	CONVERT_EACH_END (my name, U"_part");
}

DIRECT (NEW_Strings_to_EditDistanceTable) {
	CONVERT_COUPLE (Strings)
		autoEditDistanceTable result = EditDistanceTable_create (me, you);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW_Strings_to_Permutation, U"Strings: To Permutation", U"Strings: To Permutation...") {
	BOOLEANVAR (sort, U"Sort", true)
	OK
DO
	CONVERT_EACH (Strings)
		autoPermutation result = Strings_to_Permutation (me, sort);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_Strings_and_Permutation_permuteStrings) {
	CONVERT_TWO (Strings, Permutation)
		autoStrings result = Strings_and_Permutation_permuteStrings (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW_SVD_to_TableOfReal, U"SVD: To TableOfReal", U"SVD: To TableOfReal...") {
	NATURALVAR (fromComponent, U"First component", U"1")
	INTEGERVAR (toComponent, U"Last component", U"0 (= all)")
	OK
DO
	CONVERT_EACH (SVD)
		autoTableOfReal result = SVD_to_TableOfReal (me, fromComponent, toComponent);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_SVD_extractLeftSingularVectors) {
	LOOP {
		iam (SVD);
		autoTableOfReal result = SVD_extractLeftSingularVectors (me);
	CONVERT_EACH_END (my name, U"_lsv")
}

DIRECT (NEW_SVD_extractRightSingularVectors) {
	CONVERT_EACH (SVD)
		autoTableOfReal result = SVD_extractRightSingularVectors (me);
	CONVERT_EACH_END (my name, U"_rsv")
}

DIRECT (NEW_SVD_extractSingularValues) {
	CONVERT_EACH (SVD)
		autoTableOfReal result = SVD_extractSingularValues (me);
	CONVERT_EACH_END (my name, U"_sv");
}

/******************* Table ****************************/

DIRECT (NEW1_Table_create_petersonBarney1952) {
	CREATE_ONE
		autoTable result = Table_create_petersonBarney1952 ();
	CREATE_ONE_END (U"pb")
}

DIRECT (NEW1_Table_create_polsVanNierop1973) {
	CREATE_ONE
		autoTable result = Table_create_polsVanNierop1973 ();
	CREATE_ONE_END (U"pvn")
}

DIRECT (NEW1_Table_create_weenink1983) {
	CREATE_ONE
		autoTable result = Table_create_weenink1983 ();
	CREATE_ONE_END (U"m10w10c10")
}

FORM (GRAPHICS_Table_scatterPlotWhere, U"Table: Scatter plot where", nullptr) {
	WORDVAR (xColumn_string, U"Horizontal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORDVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORDVAR (markColumn_string, U"Column with marks", U"")
	NATURALVAR (fontSize, U"Font size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long markColumn = Table_getColumnIndexFromColumnLabel (me, markColumn_string);
		autoTable part = Table_extractRowsWhere (me, formula, interpreter);
		Table_scatterPlot (part.get(), GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, markColumn, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_scatterPlotMarkWhere, U"Scatter plot where (marks)", nullptr) {
	WORDVAR (xColumn_string, U"Horizontal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORDVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	POSITIVEVAR (markSize_mm, U"Mark size (mm)", U"1.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	SENTENCEVAR (mark_string, U"Mark string (+xo.)", U"+")
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		autoTable part = Table_extractRowsWhere (me, formula, interpreter);
		Table_scatterPlot_mark (part.get(), GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_barPlotWhere, U"Table: Bar plot where", U"Table: Bar plot where...") {
	SENTENCEVAR (yColumns_string, U"Vertical column(s)", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCEVAR (markColumn_string, U"Column with labels", U"")
	LABEL (U"", U"Distances are in units of 'bar width'")
	REALVAR (distanceFromBorder, U"Distance of first bar from border", U"1.0")
	REALVAR (distanceBetweenGroups, U"Distance between bar groups", U"1.0")
	REALVAR (distanceWithinGroup, U"Distance between bars within group", U"0.0")
	SENTENCEVAR (colours, U"Colours", U"Grey")
	REALVAR (angle_degrees, U"Label text angle (degrees)", U"0.0");
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"row >= 1 and row <= 8")
	OK
DO
	GRAPHICS_EACH (Table)
		Table_barPlotWhere (me, GRAPHICS, yColumns_string, ymin, ymax, markColumn_string, distanceFromBorder, distanceWithinGroup, distanceBetweenGroups, colours, angle_degrees, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_LineGraphWhere, U"Table: Line graph where", U"Table: Line graph where...") {
	SENTENCEVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCEVAR (xColumn_string, U"Horizonal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORDVAR (text, U"Text", U"+")
	REALVAR (angle_degrees, U"Label text angle (degrees)", U"0.0");
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Use only data from rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; (= everything)")
	OK
DO
	GRAPHICS_EACH (Table)
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long xcolumn = Table_findColumnIndexFromColumnLabel (me, xColumn_string);
		Table_lineGraphWhere (me, GRAPHICS, xcolumn, xmin, xmax,ycolumn, ymin, ymax, text, angle_degrees, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_boxPlots, U"Table: Box plots", nullptr) {
	WORDVAR (dataColumns_string, U"Data columns", U"F1 F2")
	WORDVAR (factorColumn_string, U"Factor column", U"Sex")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_boxPlotsWhere (me, GRAPHICS, dataColumns_string, factorColumn, ymin, ymax, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_boxPlotsWhere, U"Table: Box plots where", U"Table: Box plots where...") {
	SENTENCEVAR (dataColumns_string, U"Data columns", U"F1 F2")
	WORDVAR (factorColumn_string, U"Factor column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_boxPlotsWhere (me, GRAPHICS, dataColumns_string, factorColumn, ymin, ymax, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipseWhere, U"Draw ellipse (standard deviation)", nullptr) {
	WORDVAR (xColumn_string, U"Horizontal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORDVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"2.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		autoTable thee = Table_extractRowsWhere (me, formula, interpreter);
		Table_drawEllipse_e (thee.get(), GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, numberOfSigmas, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipses, U"Table: Draw ellipses", nullptr) {
	WORDVAR (xColumn_string, U"Horizontal column", U"F2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORDVAR (yColumn_string, U"Vertical column", U"F1")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORDVAR (factorColumn_string, U"Factor column", U"Vowel")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	INTEGERVAR (fontSize, U"Font size", U"12 (0 = no label)")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long factorcolumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_drawEllipsesWhere (me, GRAPHICS, xcolumn, ycolumn, factorcolumn, xmin, xmax, ymin, ymax, numberOfSigmas, fontSize, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipsesWhere, U"Table: Draw ellipses where", nullptr) {
	WORDVAR (xColumn_string, U"Horizontal column", U"F2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORDVAR (yColumn_string, U"Vertical column", U"F1")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORDVAR (factorColumn_string, U"Factor column", U"Vowel")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	INTEGERVAR (fontSize, U"Font size", U"12 (0 = no label)")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long factorcolumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_drawEllipsesWhere (me, GRAPHICS, xcolumn, ycolumn, factorcolumn, xmin,  xmax, ymin, ymax,  numberOfSigmas, fontSize, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_Table_normalProbabilityPlot, U"Table: Normal probability plot", U"Table: Normal probability plot...") {
	WORDVAR (column_string, U"Column", U"F1")
	NATURALVAR (numberOfQuantiles, U"Number of quantiles", U"100")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long column = Table_getColumnIndexFromColumnLabel (me, column_string);
		Table_normalProbabilityPlot (me, GRAPHICS, column, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_normalProbabilityPlotWhere, U"Table: Normal probability plot where", U"Table: Normal probability plot...") {
	WORDVAR (column_string, U"Column", U"F0")
	NATURALVAR (numberOfQuantiles, U"Number of quantiles", U"100")
	REALVAR (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long column = Table_getColumnIndexFromColumnLabel (me, column_string);
		autoTable thee = Table_extractRowsWhere (me, formula, interpreter);
		Table_normalProbabilityPlot (thee.get(), GRAPHICS, column, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_quantileQuantilePlot, U"Table: Quantile-quantile plot", U"Table: Quantile-quantile plot...") {
	WORDVAR (xColumn_string, U"Horizontal axis column", U"")
	WORDVAR (yColumn_string, U"Vertical axis column", U"")
	NATURALVAR (numberOfQuantiles, U"Number of quantiles", U"100")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		Table_quantileQuantilePlot (me, GRAPHICS, xcolumn, ycolumn, numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_quantileQuantilePlot_betweenLevels, U"Table: Quantile-quantile plot (between levels)", U"Table: Quantile-quantile plot...") {
	WORDVAR (dataColumn_string, U"Data column", U"F0")
	WORDVAR (factorColumn_string, U"Factor column", U"Sex")
	WORDVAR (xLevel_string, U"Horizontal factor level", U"")
	WORDVAR (yLevelString, U"Vertical factor level", U"")
	NATURALVAR (numberOfQuantiles, U"Number of quantiles", U"100")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		long factorColumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_quantileQuantilePlot_betweenLevels (me, GRAPHICS, dataColumn, factorColumn, xLevel_string, yLevelString, numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_lagPlot, U"Table: lag plot", nullptr) {
	WORDVAR (dataColumn_string, U"Data column", U"errors")
	NATURALVAR (lag, U"Lag", U"1")
	REALVAR (fromXY, U"left Horizontal and vertical range", U"0.0")
	REALVAR (toXY, U"right Horizontal and vertical range", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_lagPlotWhere (me, GRAPHICS, dataColumn, lag, fromXY, toXY, label, labelSize, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_Table_lagPlotWhere, U"Table: lag plot where", nullptr) {
	WORDVAR (dataColumn_string, U"Data column", U"errors")
	NATURALVAR (lag, U"Lag", U"1")
	REALVAR (fromXY, U"left Horizontal and vertical range", U"0.0")
	REALVAR (toXY, U"right Horizontal and vertical range", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_lagPlotWhere (me, GRAPHICS, dataColumn, lag, fromXY, toXY, label, labelSize, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_distributionPlot, U"Table: Distribution plot", nullptr) {
	WORDVAR (dataColumn_string, U"Data column", U"data")
	REALVAR (minimumValue, U"Minimum value", U"0.0")
	REALVAR (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURALVAR (numberOfBins, U"Number of bins", U"10")
	REALVAR (minimumFrequency, U"Minimum frequency", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, minimumValue, maximumValue, numberOfBins, minimumFrequency, maximumFrequency, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_distributionPlotWhere, U"Table: Distribution plot where", nullptr) {
	WORDVAR (dataColumn_string, U"Data column", U"data")
	REALVAR (minimumValue, U"Minimum value", U"0.0")
	REALVAR (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"", U"Display of the distribution")
	NATURALVAR (numberOfBins, U"Number of bins", U"10")
	REALVAR (minimumFrequency, U"Minimum frequency", U"0.0")
	REALVAR (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, minimumValue, maximumValue, numberOfBins, minimumFrequency, maximumFrequency, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_horizontalErrorBarsPlot, U"Table: Horizontal error bars plot", U"Table: Horizontal error bars plot...") {
	WORDVAR (xColumn_string, U"Horizontal column", U"x")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	WORDVAR (yColumn_string, U"Vertical column", U"y")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	WORDVAR (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORDVAR (upperErrorColumn_string, U"Upper error value column", U"error2")
	REALVAR (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long xl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		long xu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_horizontalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, xl, xu, barSize_mm, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_horizontalErrorBarsPlotWhere, U"Table: Horizontal error bars plot where", U"Table: Horizontal error bars plot where...") {
	WORDVAR (xColumn_string, U"Horizontal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	WORDVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	WORDVAR (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORDVAR (upperErrorColumn_string, U"Upper error value column", U"error2")
	REALVAR (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long xl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		long xu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_horizontalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, xl, xu, barSize_mm, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_verticalErrorBarsPlot, U"Table: Vertical error bars plot", U"Table: Vertical error bars plot...") {
	WORDVAR (xColumn_string, U"Horizontal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	WORDVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	WORDVAR (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORDVAR (upperErrorColumn_string, U"Upper error value column", U"error2")
	REALVAR (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long yl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		long yu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_verticalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, yl, yu, barSize_mm, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_verticalErrorBarsPlotWhere, U"Table: Vertical error bars plot where", U"Table: Vertical error bars plot where...") {
	WORDVAR (xColumn_string, U"Horizontal column", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	WORDVAR (yColumn_string, U"Vertical column", U"")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	WORDVAR (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORDVAR (upperErrorColumn_string, U"Upper error value column", U"error2")
	REALVAR (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	LABEL (U"", U"Use only data in rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		long yl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		long yu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_verticalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, yl, yu, barSize_mm, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (NEW_Table_extractRowsWhere, U"Table: Extract rows where", nullptr) {
	LABEL (U"", U"Extract rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_extractRowsWhere (me, formula, interpreter);
	CONVERT_EACH_END (my name, U"_formula")
}

FORM (NEW_Table_extractRowsMahalanobisWhere, U"Table: Extract rows where (mahalanobis)", nullptr) {
	SENTENCEVAR (dataColumns_string, U"Extract all rows where columns...", U"F1 F2 F3")
	RADIO_ENUM (U"...have a mahalanobis distance...", kMelder_number, GREATER_THAN)
	REALVAR (numberOfSigmas, U"...the number", U"2.0")
	WORDVAR (factorColumn_string, U"Factor column", U"")
	LABEL (U"", U"Process only rows where the following condition holds:")
	TEXTVAR (formula, U"Formula", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_extractMahalanobisWhere(me, dataColumns_string, factorColumn_string, numberOfSigmas, GET_ENUM (kMelder_number, U"...have a mahalanobis distance..."), formula, interpreter);
	CONVERT_EACH_END (my name, U"_mahalanobis")
}

FORM (NEW_Table_extractColumnRanges, U"Table: Extract column ranges", nullptr) {
	LABEL (U"", U"Create a new Table from the following columns:")
	TEXTVAR (columnRanges_string, U"Ranges", U"1 2")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_extractColumnRanges (me, columnRanges_string);
	CONVERT_EACH_END (my name, U"_columns")
}

/******************* TableOfReal ****************************/

DIRECT (NEW1_CreateIrisDataset) {
	CREATE_ONE
		autoTableOfReal result = TableOfReal_createIrisDataset ();
	CREATE_ONE_END (U"iris")
}

FORM (INFO_TableOfReal_reportMultivariateNormality, U"TableOfReal: Report multivariate normality (BHEP)", U"TableOfReal: Report multivariate normality (BHEP)...") {
	REALVAR (h, U"Smoothing parameter", U"0.0")
	OK
DO
	INFO_ONE (TableOfReal)
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
		MelderInfo_close ();
	INFO_ONE_END
}

DIRECT (NEW1_TableOfReal_and_Permutation_permuteRows) {
	CONVERT_TWO (TableOfReal, Permutation)
		autoTableOfReal result = TableOfReal_and_Permutation_permuteRows (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

DIRECT (NEW_TableOfReal_to_Permutation_sortRowlabels) {
	CONVERT_EACH (TableOfReal)
		autoPermutation result = TableOfReal_to_Permutation_sortRowLabels (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_TableOfReal_appendColumns) {
	CONVERT_TYPED_LIST (TableOfReal, TableOfRealList)
		autoTableOfReal result = TableOfRealList_appendColumnsMany (list.get());
	CONVERT_TYPED_LIST_END (U"columns_appended");
}

FORM (NEW1_TableOfReal_create_pols1973, U"Create TableOfReal (Pols 1973)", U"Create TableOfReal (Pols 1973)...") {
	BOOLEANVAR (includeFormantAmplitudeLevels, U"Include formant levels", false)
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_pols1973 (includeFormantAmplitudeLevels);
	CREATE_ONE_END (U"pols1973")
}

DIRECT (NEW_Table_create_esposito2006) {
	CREATE_ONE
		autoTable result = Table_create_esposito2006 ();
	CREATE_ONE_END (U"esposito2006")
}

DIRECT (NEW_Table_create_ganong1980) {
	CREATE_ONE
		autoTable result = Table_create_ganong1980 ();
	CREATE_ONE_END (U"ganong")
}

FORM (NEW_TableOfReal_create_vanNierop1973, U"Create TableOfReal (Van Nierop 1973)...", U"Create TableOfReal (Van Nierop 1973)...") {
	BOOLEANVAR (includeFormantAmplitudeLevels, U"Include formant levels", false)
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_vanNierop1973 (includeFormantAmplitudeLevels);
	CREATE_ONE_END (U"vanNierop1973")
}

FORM (NEW_TableOfReal_create_weenink1983, U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Weenink 1985)...") {
	RADIOVAR (speakerGroup, U"Speakers group", 1)
		RADIOBUTTON (U"Men")
		RADIOBUTTON (U"Women")
		RADIOBUTTON (U"Children")
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_weenink1983 (speakerGroup);
	CREATE_ONE_END ((speakerGroup == 1 ? U"m10" : speakerGroup == 2 ? U"w10" : U"c10"));
}

FORM (GRAPHICS_TableOfReal_drawAsScalableSquares, U"TableOfReal: Draw as scalable squares", 0)
	REALVAR (zmin, U"left Value range", U"0.0");
	REALVAR (zmax, U"right Value range", U"0.0");
	POSITIVEVAR (scaleFactor, U"Cell size scale factor", U"0.95")
	BOOLEANVAR (randomFill, U"Random fill", 0)
	BOOLEANVAR (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawAsScalableSquares (me, GRAPHICS, zmin, zmax, scaleFactor, randomFill, garnish);
	GRAPHICS_EACH_END

FORM (GRAPHICS_TableOfReal_drawScatterPlot, U"TableOfReal: Draw scatter plot", U"TableOfReal: Draw scatter plot...") {
	LABEL (U"", U"Select the part of the table")
	NATURALVAR (xColumn, U"Horizontal axis column number", U"1")
	NATURALVAR (yColumn, U"Vertical axis column number", U"2")
	INTEGERVAR (fromRow, U"left Row number range", U"0")
	INTEGERVAR (toRow, U"right Row number range", U"0")
	LABEL (U"", U"Select the drawing area limits")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	NATURALVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (useRowLabels, U"Use row labels", false)
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawScatterPlot (me, GRAPHICS, xColumn, yColumn, fromRow, toRow, xmin, xmax, ymin, ymax, labelSize, useRowLabels, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawScatterPlotMatrix, U"TableOfReal: Draw scatter plots matrix", nullptr) {
	INTEGERVAR (fromColumn, U"From column", U"0")
	INTEGERVAR (toColumn, U"To column", U"0")
	POSITIVEVAR (fractionWhite, U"Fraction white", U"0.1")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawScatterPlotMatrix (me, GRAPHICS, fromColumn, toColumn, fractionWhite);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawBiplot, U"TableOfReal: Draw biplot", U"TableOfReal: Draw biplot...") {
	LABEL (U"", U"")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	REALVAR (splitFactor, U"Split factor", U"0.5")
	INTEGERVAR (labelSize, U"Label size", U"10")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawBiplot (me, GRAPHICS, xmin, xmax, ymin, ymax, splitFactor, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawVectors, U"Draw vectors", U"TableOfReal: Draw vectors...") {
	LABEL (U"", U"From (x1, y1) to (x2, y2)")
	NATURALVAR (x1Column, U"X1 column", U"1")
	NATURALVAR (y1Column, U"Y1 column", U"2")
	NATURALVAR (x2Column, U"X2 column", U"3")
	NATURALVAR (y2Column, U"Y2 column", U"4")
	LABEL (U"", U"Select the drawing area")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	RADIOVAR (vectorType, U"Vector type", 1)
		RADIOBUTTON (U"Arrow")
		RADIOBUTTON (U"Double arrow")
		RADIOBUTTON (U"Line")
	INTEGERVAR (labelSize, U"Label size", U"10")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawVectors (me, GRAPHICS, x1Column, y1Column, x2Column, y2Column, xmin, xmax, ymin, ymax, vectorType, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawRowAsHistogram, U"Draw row as histogram", U"TableOfReal: Draw rows as histogram...") {
	LABEL (U"", U"Select from the table")
	WORDVAR (rowNumber, U"Row number", U"1")
	INTEGERVAR (fromColumn, U"left Column range", U"0")
	INTEGERVAR (toColumn, U"right Column range", U"0")
	LABEL (U"", U"Vertical drawing range")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	LABEL (U"", U"Offset and distance in units of 'bar width'")
	REALVAR (xOffset, U"Horizontal offset", U"0.5")
	REALVAR (distanceBetweenBars, U"Distance between bars", U"1.0")
	WORDVAR (greys, U"Grey value (1=white)", U"0.7")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, rowNumber, fromColumn, toColumn, ymin, ymax, xOffset, 0, distanceBetweenBars, greys, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawRowsAsHistogram, U"Draw rows as histogram", U"TableOfReal: Draw rows as histogram...") {
	LABEL (U"", U"Select from the table")
	SENTENCEVAR (rowNumbers_string, U"Row numbers", U"1 2")
	INTEGERVAR (fromColumn, U"left Column range", U"0")
	INTEGERVAR (toColumn, U"right Column range", U"0")
	LABEL (U"", U"Vertical drawing range")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	LABEL (U"", U"Offset and distance in units of 'bar width'")
	REALVAR (xOffset, U"Horizontal offset", U"1.0")
	REALVAR (distanceBetweenBarGroups, U"Distance between bar groups", U"1.0")
	REALVAR (distanceBetweenBars, U"Distance between bars", U"0.0")
	SENTENCEVAR (greys_string, U"Grey values (1=white)", U"1 1")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, rowNumbers_string, fromColumn, toColumn, ymin, ymax,xOffset, distanceBetweenBars,distanceBetweenBarGroups, greys_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawBoxPlots, U"TableOfReal: Draw box plots", U"TableOfReal: Draw box plots...") {
	INTEGERVAR (fromRow, U"From row", U"0")
	INTEGERVAR (toRow, U"To row", U"0")
	INTEGERVAR (fromColumn, U"From column", U"0")
	INTEGERVAR (toColumn, U"To column", U"0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawBoxPlots (me, GRAPHICS, fromRow, toRow, fromColumn, toColumn, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawColumnAsDistribution, U"TableOfReal: Draw column as distribution", U"TableOfReal: Draw column as distribution...") {
	NATURALVAR (columnNumber, U"Column number", U"1")
	REALVAR (fromValue, U"left Value range", U"0.0")
	REALVAR (toValue, U"right Value range", U"0.0")
	REALVAR (fromFrequency, U"left Frequency range", U"0.0")
	REALVAR (toFrequency, U"right frequency range", U"0.0")
	NATURALVAR (numberOfBins, U"Number of bins", U"10")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawColumnAsDistribution (me, GRAPHICS, columnNumber, fromValue, toValue, numberOfBins, fromFrequency, toFrequency, false, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW_TableOfReal_to_Configuration_lda, U"TableOfReal: To Configuration (lda)", U"TableOfReal: To Configuration (lda)...") {
	INTEGERVAR (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	REQUIRE (numberOfDimensions >= 0, U"Number of dimensions must be greater equal zero.")
	CONVERT_EACH (TableOfReal)
		autoConfiguration result = TableOfReal_to_Configuration_lda (me, numberOfDimensions);
	CONVERT_EACH_END (my name, U"_lda")
}

FORM (NEW_TableOfReal_to_CCA, U"TableOfReal: To CCA", U"TableOfReal: To CCA...") {
	NATURAL (U"Dimension of dependent variate", U"2")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoCCA result = TableOfReal_to_CCA (me, GET_INTEGER (U"Dimension of dependent variate"));
	CONVERT_EACH_END (my name)
}

FORM (NEW_TableOfReal_to_Configuration_pca, U"TableOfReal: To Configuration (pca)", U"TableOfReal: To Configuration (pca)...") {
	NATURAL (U"Number of dimensions", U"2")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoConfiguration result = TableOfReal_to_Configuration_pca (me, GET_INTEGER (U"Number of dimensions"));
	CONVERT_EACH_END (my name, U"_pca")
}

DIRECT (NEW_TableOfReal_to_Discriminant) {
	CONVERT_EACH (TableOfReal)
		autoDiscriminant result = TableOfReal_to_Discriminant (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_TableOfReal_to_PCA_byRows) {
	CONVERT_EACH (TableOfReal)
		autoPCA result = TableOfReal_to_PCA_byRows (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_TableOfReal_to_SSCP, U"TableOfReal: To SSCP", U"TableOfReal: To SSCP...") {
	INTEGERVAR (fromRow, U"Begin row", U"0")
	INTEGERVAR (toRow, U"End row", U"0")
	INTEGERVAR (fromColumn, U"Begin column", U"0")
	INTEGERVAR (toColumn, U"End column", U"0")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoSSCP result = TableOfReal_to_SSCP (me, fromRow, toRow, fromColumn, toColumn);
	CONVERT_EACH_END (my name)
}

/* For the inheritors */
DIRECT (NEW_TableOfReal_to_TableOfReal) {
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_to_TableOfReal (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_TableOfReal_to_Correlation) {
	CONVERT_EACH (TableOfReal)
		autoCorrelation result = TableOfReal_to_Correlation (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_TableOfReal_to_Correlation_rank) {
	CONVERT_EACH (TableOfReal)
		autoCorrelation result = TableOfReal_to_Correlation_rank (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_TableOfReal_to_Covariance) {
	CONVERT_EACH (TableOfReal)
		autoCovariance result = TableOfReal_to_Covariance (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_TableOfReal_to_SVD) {
	CONVERT_EACH (TableOfReal)
		autoSVD result = TableOfReal_to_SVD (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_TablesOfReal_to_Eigen_gsvd) {
	CONVERT_COUPLE (TableOfReal)
		autoEigen result = TablesOfReal_to_Eigen_gsvd (me, you);
	CONVERT_COUPLE_END (U"gsvd");
}

FORM (NEW1_TableOfReal_and_TableOfReal_crossCorrelations, U"TableOfReal & TableOfReal: Cross-correlations", nullptr) {
	OPTIONMENUVAR (between, U"Correlations between", 1)
		OPTION (U"Rows")
		OPTION (U"Columns")
	BOOLEANVAR (center, U"Center", false)
	BOOLEANVAR (normalize, U"Normalize", false)
	OK
DO
	int by_columns = between - 1;
	CONVERT_COUPLE (TableOfReal)
		autoTableOfReal result = TableOfReal_and_TableOfReal_crossCorrelations (me, you, by_columns, center, normalize);
	CONVERT_COUPLE_END (U"cc_", by_columns ? U"by_cols" : U"by_rows")
}

void praat_TableOfReal_init3 (ClassInfo klas);
void praat_TableOfReal_init3 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 2, U"To TableOfReal (cross-correlations)...", 0, 0, NEW1_TableOfReal_and_TableOfReal_crossCorrelations);
}

DIRECT (NEW1_TablesOfReal_to_GSVD) {
	CONVERT_COUPLE (TableOfReal)
		autoGSVD result = TablesOfReal_to_GSVD (me, you);
	CONVERT_COUPLE_END (my name, U"_", your name)
}

FORM (NEW_TableOfReal_choleskyDecomposition, U"TableOfReal: Cholesky decomposition", nullptr) {
	BOOLEANVAR (wantUpper, U"Upper (else L)", 0)
	BOOLEANVAR (wantInverse, U"Inverse", 0)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_choleskyDecomposition (me, wantUpper, wantInverse);
	CONVERT_EACH_END (my name)
}

FORM (NEWMANY_TableOfReal_to_PatternList_and_Categories, U"TableOfReal: To PatternList and Categories", U"TableOfReal: To PatternList and Categories...") {
	INTEGERVAR (fromRow, U"left Row range", U"0")
	INTEGERVAR (toRow, U"right Row range", U"0 (= all)")
	INTEGERVAR (fromColumn, U"left Column range", U"0")
	INTEGERVAR (toColumn, U"right Column range", U"0 (= all)")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoPatternList ap; 
		autoCategories result;
		TableOfReal_to_PatternList_and_Categories (me, fromRow, toRow, fromColumn, toColumn, & ap, & result);
		praat_new (ap.move(), my name);
	CONVERT_EACH_END (my name)
}

FORM (REAL_TableOfReal_getColumnSum, U"TableOfReal: Get column sum", U"") {
	INTEGERVAR (columnNumber, U"Column", U"1")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getColumnSum (me, columnNumber);
	NUMBER_ONE_END (U"(column sum)")
}

FORM (REAL_TableOfReal_getRowSum, U"TableOfReal: Get row sum", U"") {
	INTEGERVAR (rowNumber, U"Row", U"1")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getRowSum (me, rowNumber);
	NUMBER_ONE_END (U"(row sum)")
}

DIRECT (REAL_TableOfReal_getGrandSum) {
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getGrandSum (me);
	NUMBER_ONE_END (U"(grand sum)")
}

FORM (NEW_TableOfReal_meansByRowLabels, U"TableOfReal: Means by row labels", U"TableOfReal: To TableOfReal (means by row labels)...") {
	BOOLEANVAR (wantExpand, U"Expand", 0)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_meansByRowLabels (me, wantExpand, 0);
	CONVERT_EACH_END (my name, U"_byrowlabels")
}

FORM (NEW_TableOfReal_mediansByRowLabels, U"TableOfReal: Medians by row labels", U"TableOfReal: To TableOfReal (medians by row labels)...") {
	BOOLEANVAR (wantExpand, U"Expand", 0)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_meansByRowLabels (me, wantExpand, 1);
	CONVERT_EACH_END (my name, U"_byrowlabels")
}

/***** TableOfReal and FilterBank  *****/

FORM (MODIFY_TextGrid_extendTime, U"TextGrid: Extend time", U"TextGrid: Extend time...") {
	LABEL (U"", U"")
	POSITIVEVAR (extension_s, U"Extend domain by (s)", U"1.0")
	RADIOVAR (position, U"At", 1)
	RADIOBUTTON (U"End")
	RADIOBUTTON (U"Start")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_extendTime (me,extension_s, position - 1);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_replaceIntervalTexts, U"TextGrid: Replace interval text", U"TextGrid: Replace interval text...") {
	LABEL (U"", U"")
	NATURALVAR (tierNumber, U"Tier number", U"1")
	INTEGERVAR (fromInterval, U"left Interval range", U"0")
	INTEGERVAR (toInterval, U"right Interval range", U"0")
	SENTENCEVAR (search_string, U"Search", U"a")
	SENTENCEVAR (replace_string, U"Replace", U"a")
	RADIOVAR (searchType, U"Search and replace strings are:", 1)
		RADIOBUTTON (U"Literals")
		RADIOBUTTON (U"Regular Expressions")
	OK
DO
	MODIFY_EACH (TextGrid)
		long nmatches, nstringmatches;
		TextGrid_changeLabels (me, tierNumber, fromInterval, toInterval, search_string, replace_string, searchType - 1, &nmatches, &nstringmatches);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_replacePointTexts, U"TextGrid: Replace point text", U"TextGrid: Replace point text...") {
	LABEL (U"", U"")
	NATURALVAR (tierNumber, U"Tier number", U"1")
	INTEGERVAR (fromInterval, U"left Interval range", U"0")
	INTEGERVAR (toInterval, U"right Interval range", U"0")
	SENTENCEVAR (search_string, U"Search", U"a")
	SENTENCEVAR (replace_string, U"Replace", U"a")
	RADIOVAR (searchType, U"Search and replace strings are:", 1)
		RADIOBUTTON (U"Literals")
		RADIOBUTTON (U"Regular Expressions")
	OK
DO
	MODIFY_EACH (TextGrid)
		long nmatches, nstringmatches;
		TextGrid_changeLabels (me, tierNumber, fromInterval, toInterval, search_string, replace_string, searchType - 1, &nmatches, &nstringmatches);
	MODIFY_EACH_END
}

FORM (NEW1_TextGrids_to_Table_textAlignmentment, U"TextGrids: To Table (text alignment)", nullptr) {
	NATURALVAR (targetTierNumber, U"Target tier", U"1")
	NATURALVAR (sourceTierNumber, U"Source tier", U"1")
	OK
DO
	CONVERT_COUPLE (TextGrid)
 		autoTable result = TextGrids_to_Table_textAlignmentment (me, targetTierNumber, you, sourceTierNumber, nullptr);
	CONVERT_COUPLE_END (my name, U"_", your name);
}

FORM (NEW_TextGrid_to_DurationTier, U"TextGrid: To DurationTier", U"TextGrid: To DurationTier...") {
	NATURALVAR (tierNumber, U"Tier number", U"1")
	POSITIVEVAR (timeScaleFactor, U"Time scale factor", U"2.0")
	POSITIVEVAR (leftTransitionDuration, U"Left transition duration (s)", U"1e-10")
	POSITIVEVAR (rightTransitionDuration, U"Right transition duration (s)", U"1e-10")
	OPTIONMENU_ENUM4 (___, U"Scale intervals whose label ", kMelder_string, DEFAULT)
	SENTENCE4 (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoDurationTier result = TextGrid_to_DurationTier (me,tierNumber, timeScaleFactor,leftTransitionDuration, rightTransitionDuration, ___, ___theText);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_TextGrid_and_DurationTier_to_TextGrid) {
	CONVERT_TWO (TextGrid, DurationTier)
		autoTextGrid result = TextGrid_and_DurationTier_scaleTimes (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_TextGrids_and_EditCostsTable_to_Table_textAlignmentment, U"TextGrids & EditCostsTable: To Table(text alignmentment)", nullptr) {
	NATURALVAR (targetTierNumber, U"Target tier", U"1")
	NATURALVAR (sourceTierNumber, U"Source tier", U"1")
	OK
DO
	CONVERT_COUPLE_AND_ONE (TextGrid, EditCostsTable)
		autoTable result = TextGrids_to_Table_textAlignmentment (me, targetTierNumber, you, sourceTierNumber, him);
	CONVERT_COUPLE_AND_ONE_END (my name, U"_", your name)
}

FORM (MODIFY_TextGrid_setTierName, U"TextGrid: Set tier name", U"TextGrid: Set tier name...") {
	NATURALVAR (tierNUmber, U"Tier number:", U"1")
	SENTENCEVAR (name, U"Name", U"");
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_setTierName (me, tierNUmber, name);
	MODIFY_EACH_END
}

DIRECT (WINDOW_VowelEditor_create) {
	if (theCurrentPraatApplication -> batch) {
		Melder_throw (U"Cannot edit from batch.");
	}
	autoVowelEditor vowelEditor = VowelEditor_create (U"VowelEditor", nullptr);
	vowelEditor.releaseToUser();
END }

static autoDaata cmuAudioFileRecognizer (int nread, const char *header, MelderFile file) {
	return nread < 12 || header [0] != 6 || header [1] != 0 ?
	       autoSound () : Sound_readFromCmuAudioFile (file);
}

void praat_CC_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Paint...", nullptr, 1, GRAPHICS_CC_paint);
	praat_addAction1 (klas, 1, U"Draw...", nullptr, 1, GRAPHICS_CC_drawC0);
	praat_addAction1 (klas, 1, QUERY_BUTTON, nullptr, 0, 0);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get number of coefficients...", nullptr, 1, INTEGER_CC_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, U"Get value in frame...", nullptr, 1, REAL_CC_getValueInFrame);
	praat_addAction1 (klas, 1, U"Get c0 value in frame...", nullptr, 1, REAL_CC_getC0ValueInFrame);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, praat_HIDDEN + praat_DEPTH_1, REAL_CC_getValue);
	praat_addAction1 (klas, 0, U"To Matrix", nullptr, 0, NEW_CC_to_Matrix);
	praat_addAction1 (klas, 2, U"To DTW...", nullptr, 0, NEW1_CCs_to_DTW);
}

static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm); // deprecated 2014
static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, praat_HIDDEN, NEW1_Eigen_and_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, praat_HIDDEN, NEW1_Eigen_and_Matrix_projectColumns);
}

static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm);
static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, praat_HIDDEN, NEW1_Eigen_and_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, 0, NEW1_Eigen_and_Matrix_projectColumns);
}

static void praat_Eigen_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Get eigenvalue...", nullptr, 1, REAL_Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, U"Get sum of eigenvalues...", nullptr, 1, REAL_Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, U"Get number of eigenvectors", nullptr, 1, INTEGER_Eigen_getNumberOfEigenvalues);
	praat_addAction1 (klas, 1, U"Get eigenvector dimension", nullptr, 1, INTEGER_Eigen_getDimension);
	praat_addAction1 (klas, 1, U"Get eigenvector element...", nullptr, 1, REAL_Eigen_getEigenvectorElement);
}

static void praat_Eigen_draw_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Draw eigenvalues...", nullptr, 1,GRAPHICS_Eigen_drawEigenvalues);
	praat_addAction1 (klas, 0, U"Draw eigenvalues (scree)...", U"*Draw eigenvalues...", praat_DEPRECATED_2010 | praat_DEPTH_1, GRAPHICS_Eigen_drawEigenvalues_scree);
	praat_addAction1 (klas, 0, U"Draw eigenvector...", nullptr, 1, GRAPHICS_Eigen_drawEigenvector);
}

static void praat_Index_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Get number of classes", nullptr, 0, INTEGER_Index_getNumberOfClasses);
	praat_addAction1 (klas, 1, U"To Permutation...", nullptr, 0, NEW_Index_to_Permutation);
	praat_addAction1 (klas, 1, U"Extract part...", nullptr, 0, NEW_Index_extractPart);
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
	praat_addAction1 (klas, 0, U"Draw frequency scale...", nullptr, 1, GRAPHICS_BandFilterSpectrogram_drawFrequencyScale);
}

static void praat_FilterBank_query_init (ClassInfo klas);
static void praat_FilterBank_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getXofColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, INTEGER_FilterBank_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyOfRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getValueInCell);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get frequency in Hertz...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyInHertz);
	praat_addAction1 (klas, 1, U"Get frequency in Bark...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyInBark);
	praat_addAction1 (klas, 1, U"Get frequency in mel...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyInMel);
}

static void praat_FilterBank_modify_init (ClassInfo klas);
static void praat_FilterBank_modify_init (ClassInfo klas) {
	// praat_addAction1 (klas, 0, MODIFY_BUTTON, nullptr, 0, 0); 
	praat_addAction1 (klas, 0, U"Equalize intensities...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, MODIFY_FilterBank_equalizeIntensities);
}

static void praat_FilterBank_draw_init (ClassInfo klas);
static void praat_FilterBank_draw_init (ClassInfo klas) {
	// praat_addAction1 (klas, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 0, U"Draw filters...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_drawFilters);
	praat_addAction1 (klas, 0, U"Draw one contour...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_drawOneContour);
	praat_addAction1 (klas, 0, U"Draw contours...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_drawContours);
	praat_addAction1 (klas, 0, U"Paint image...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_paintImage);
	praat_addAction1 (klas, 0, U"Paint contours...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_paintContours);
	praat_addAction1 (klas, 0, U"Paint cells...", nullptr,praat_DEPRECATED_2014 |  praat_DEPTH_1, GRAPHICS_FilterBank_paintCells);
	praat_addAction1 (klas, 0, U"Paint surface...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_paintSurface);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, 0);
	praat_addAction1 (klas, 0, U"Draw frequency scales...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, GRAPHICS_FilterBank_drawFrequencyScales);
}

static void praat_FilterBank_all_init (ClassInfo klas);
static void praat_FilterBank_all_init (ClassInfo klas) {
	praat_FilterBank_draw_init (klas);
	praat_FilterBank_query_init (klas);
	praat_FilterBank_modify_init (klas);
	praat_addAction1 (klas, 0, U"To Intensity", nullptr, praat_DEPRECATED_2014, NEW_FilterBank_to_Intensity);
	praat_addAction1 (klas, 0, U"To Matrix", nullptr, praat_DEPRECATED_2014, NEW_FilterBank_to_Matrix);
	praat_addAction1 (klas, 2, U"Cross-correlate...", nullptr, praat_DEPRECATED_2014, NEW1_FilterBanks_crossCorrelate);
	praat_addAction1 (klas, 2, U"Convolve...", nullptr, praat_DEPRECATED_2014, NEW1_FilterBanks_convolve);
}

static void praat_FunctionTerms_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (klas, 0, U"Draw...", nullptr, 1, GRAPHICS_FunctionTerms_draw);
	praat_addAction1 (klas, 0, U"Draw basis function...", nullptr, 1, GRAPHICS_FunctionTerms_drawBasisFunction);
	praat_addAction1 (klas, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (klas, 1, U"Get number of coefficients", nullptr, 1, INTEGER_FunctionTerms_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, U"Get coefficient...", nullptr, 1, REAL_FunctionTerms_getCoefficient);
	praat_addAction1 (klas, 1, U"Get degree", nullptr, 1, INTEGER_FunctionTerms_getDegree);
	praat_addAction1 (klas, 0, U"-- function specifics --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, REAL_FunctionTerms_evaluate);
	praat_addAction1 (klas, 1, U"Get minimum...", nullptr, 1, REAL_FunctionTerms_getMinimum);
	praat_addAction1 (klas, 1, U"Get x of minimum...", nullptr, 1, REAL_FunctionTerms_getXOfMinimum);
	praat_addAction1 (klas, 1, U"Get maximum...", nullptr, 1, REAL_FunctionTerms_getMaximum);
	praat_addAction1 (klas, 1, U"Get x of maximum...", nullptr, 1, REAL_FunctionTerms_getXOfMaximum);
	praat_addAction1 (klas, 0, U"Modify -", nullptr, 0, 0);
	praat_addAction1 (klas, 1, U"Set domain...", nullptr, 1, MODIFY_FunctionTerms_setDomain);
	praat_addAction1 (klas, 1, U"Set coefficient...", nullptr, 1, MODIFY_FunctionTerms_setCoefficient);
	praat_addAction1 (klas, 0, U"Analyse", nullptr, 0, 0);
}

/* Query buttons for frame-based frequency x time subclasses of matrix. */

void praat_BandFilterSpectrogram_query_init (ClassInfo klas) {
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, 1, REAL_BandFilterSpectrogram_getXofColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, 1, REAL_BandFilterSpectrogram_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, 1, REAL_BandFilterSpectrogram_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, 1, INTEGER_BandFilterSpectrogram_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, 1, REAL_BandFilterSpectrogram_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, 1, REAL_BandFilterSpectrogram_getFrequencyOfRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, 1, REAL_BandFilterSpectrogram_getValueInCell);
}

static void praat_PatternList_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (klas, 1, U"Get number of patterns", nullptr, 1, INTEGER_PatternList_getNumberOfPatterns);
	praat_addAction1 (klas, 1, U"Get pattern size", nullptr, 1, INTEGER_PatternList_getPatternSize);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, REAL_PatternList_getValue);

}

static void praat_Spline_init (ClassInfo klas) {
	praat_FunctionTerms_init (klas);
	praat_addAction1 (klas, 0, U"Draw knots...", U"Draw basis function...", 1, GRAPHICS_Spline_drawKnots);
	praat_addAction1 (klas, 1, U"Get order", U"Get degree", 1, INTEGER_Spline_getOrder);
	praat_addAction1 (klas, 1, U"Scale x...", U"Analyse",	0, NEW_Spline_scaleX);
}

static void praat_SSCP_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"-- statistics --", U"Get value...", 1, 0);
	praat_addAction1 (klas, 1, U"Get number of observations", U"-- statistics --", 1, INTEGER_SSCP_getNumberOfObservations);
	praat_addAction1 (klas, 1, U"Get degrees of freedom", U"Get number of observations", 1, INTEGER_SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, U"Get centroid element...", U"Get degrees of freedom", 1, REAL_SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, U"Get ln(determinant)", U"Get centroid element...", 1, REAL_SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Extract centroid", EXTRACT_BUTTON, 1, NEW_SSCP_extractCentroid);
}

FORM (MODIFY_SSCP_setValue, U"Covariance: Set value", U"Covariance: Set value...") {
	NATURALVAR (rowNumber, U"Row number", U"1")
	NATURALVAR (columnNumber, U"Column number", U"1")
	REALVAR (value, U"New value", U"1.0")
	OK
DO
	MODIFY_EACH (SSCP)
		SSCP_setValue (me, rowNumber, columnNumber, value);
	MODIFY_EACH_END
}

FORM (MODIFY_SSCP_setCentroid, U"", nullptr) {
	NATURALVAR (elementNumber, U"Element number", U"1")
	REALVAR (value, U"New value", U"1.0")
	OK
DO
	MODIFY_EACH (SSCP)
		SSCP_setCentroid (me, elementNumber, value);
	MODIFY_EACH_END
}

void praat_SSCP_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_removeAction (klas, nullptr, nullptr, U"Set value...");
	praat_addAction1 (klas, 1, U"Set centroid...", U"Formula...", 1, MODIFY_SSCP_setCentroid);
	praat_addAction1 (klas, 1, U"Set value...", U"Formula...", 1, MODIFY_SSCP_setValue);
	praat_addAction1 (klas, 0, U"To TableOfReal", U"To Matrix", 1, NEW_TableOfReal_to_TableOfReal);

}

void praat_TableOfReal_init2 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, U"To TableOfReal", U"To Matrix", 1, NEW_TableOfReal_to_TableOfReal);
}

void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, U"Set default costs...", U"Formula...", 1, MODIFY_EditDistanceTable_setDefaultCosts);
	praat_removeAction (klas, nullptr, nullptr, U"Draw as numbers...");
	praat_addAction1 (klas, 0, U"Draw...", U"Draw -", 1, GRAPHICS_EditDistanceTable_draw);
	praat_addAction1 (klas, 0, U"Draw edit operations", U"Draw...", 1, GRAPHICS_EditDistanceTable_drawEditOperations);
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
	
	Thing_recognizeClassesByName (classActivationList, classBarkFilter, classBarkSpectrogram,
		classCategories, classCepstrum, classCCA,
		classChebyshevSeries, classClassificationTable, classComplexSpectrogram, classConfusion,
		classCorrelation, classCovariance, classDiscriminant, classDTW,
		classEigen, classExcitationList, classEditCostsTable, classEditDistanceTable,
		classFileInMemory, classFileInMemorySet, classFormantFilter,
		classIndex, classKlattTable,
		classPermutation, classISpline, classLegendreSeries,
		classMelFilter, classMelSpectrogram, classMSpline, classPatternList, classPCA, classPolynomial, classRoots,
		classSimpleString, classStringsIndex, classSpeechSynthesizer, classSPINET, classSSCP,
		classSVD, nullptr);
	Thing_recognizeClassByOtherName (classExcitationList, U"Excitations");
	Thing_recognizeClassByOtherName (classActivationList, U"Activation");
	Thing_recognizeClassByOtherName (classPatternList, U"Pattern");
	Thing_recognizeClassByOtherName (classFileInMemorySet, U"FilesInMemory");

	VowelEditor_prefs ();

	espeakdata_praat_init ();

	praat_addMenuCommand (U"Objects", U"Technical", U"Report floating point properties", U"Report integer properties", 0, INFO_Praat_ReportFloatingPointProperties);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get TukeyQ...", 0, praat_HIDDEN, REAL_Praat_getTukeyQ);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get invTukeyQ...", 0, praat_HIDDEN, REAL_Praat_getInvTukeyQ);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get incomplete gamma...", 0, praat_HIDDEN, COMPLEX_Praat_getIncompleteGamma);
//	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as espeak voices", U"Create Strings as directory list...", praat_DEPTH_1 + praat_HIDDEN, NEW1_Strings_createAsEspeakVoices);
	praat_addMenuCommand (U"Objects", U"New", U"Create iris data set", U"Create TableOfReal...", 1, NEW1_CreateIrisDataset);
	praat_addMenuCommand (U"Objects", U"New", U"Create Permutation...", nullptr, 0, NEW_Permutation_create);
	praat_addMenuCommand (U"Objects", U"New", U"Polynomial", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial...", nullptr, 1, NEW1_Polynomial_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial from product terms...", nullptr, 1, NEW1_Polynomial_createFromProducts);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial from real zeros...", nullptr, 1, NEW1_Polynomial_createFromZeros);
	praat_addMenuCommand (U"Objects", U"New", U"Create LegendreSeries...", nullptr, 1, NEW1_LegendreSeries_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create ChebyshevSeries...", nullptr, 1, NEW1_ChebyshevSeries_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create MSpline...", nullptr, 1, NEW_MSpline_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create ISpline...", nullptr, 1, NEW1_ISpline_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound as gammatone...", U"Create Sound as tone complex...", 1, NEW1_Sound_createAsGammaTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from gammatone...", U"*Create Sound as gammatone...", praat_DEPTH_1 | praat_DEPRECATED_2016, NEW1_Sound_createAsGammaTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound as Shepard tone...", U"Create Sound as gammatone...", praat_DEPTH_1, NEW_Sound_createAsShepardTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from Shepard tone...", U"*Create Sound as Shepard tone...", praat_DEPTH_1 | praat_DEPRECATED_2016, NEW_Sound_createAsShepardTone);
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from VowelEditor...", U"Create Sound as Shepard tone...", praat_DEPTH_1 | praat_NO_API, WINDOW_VowelEditor_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create SpeechSynthesizer...", U"Create Sound from VowelEditor...", praat_DEPTH_1, NEW1_SpeechSynthesizer_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Pols & Van Nierop 1973)", U"Create Table...", 1, NEW1_Table_create_polsVanNierop1973);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Peterson & Barney 1952)", U"Create Table...", 1, NEW1_Table_create_petersonBarney1952);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Weenink 1985)", U"Create formant table (Peterson & Barney 1952)", 1, NEW1_Table_create_weenink1983);
	praat_addMenuCommand (U"Objects", U"New", U"Create H1H2 table (Esposito 2006)", U"Create formant table (Weenink 1985)", praat_DEPTH_1+ praat_HIDDEN, NEW_Table_create_esposito2006);
	praat_addMenuCommand (U"Objects", U"New", U"Create Table (Ganong 1980)", U"Create H1H2 table (Esposito 2006)", praat_DEPTH_1+ praat_HIDDEN, NEW_Table_create_ganong1980);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Pols 1973)...", U"Create TableOfReal...", 1, NEW1_TableOfReal_create_pols1973);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Van Nierop 1973)...", U"Create TableOfReal (Pols 1973)...", 1, NEW_TableOfReal_create_vanNierop1973);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Van Nierop 1973)...", 1, NEW_TableOfReal_create_weenink1983);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Confusion...", U"Create TableOfReal (Weenink 1985)...", 1, NEW1_Confusion_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Covariance...", U"Create simple Confusion...", 1, NEW1_Covariance_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Correlation...", U"Create simple Covariance...", 1, NEW1_Correlation_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create empty EditCostsTable...", U"Create simple Covariance...", 1, NEW_EditCostsTable_createEmpty);

	praat_addMenuCommand (U"Objects", U"New", U"Create KlattTable example", U"Create TableOfReal (Weenink 1985)...", praat_DEPTH_1 + praat_HIDDEN, NEW1_KlattTable_createExample);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as tokens...", U"Create Strings as directory list...", 1, NEW1_Strings_createAsTokens);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as characters...", U"Create Strings as tokens...",  praat_DEPTH_1 + praat_HIDDEN, NEW1_Strings_createAsCharacters);

	praat_addMenuCommand (U"Objects", U"New", U"Create simple Polygon...", nullptr, praat_HIDDEN, NEW1_Polygon_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polygon (random vertices)...", nullptr, praat_DEPRECATED_2016, NEW1_Polygon_createFromRandomPoints);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polygon (random points)...", nullptr, praat_HIDDEN, NEW1_Polygon_createFromRandomPoints);
	praat_addMenuCommand (U"Objects", U"New", U"FileInMemory", nullptr, praat_HIDDEN, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemory...", nullptr, praat_DEPTH_1 + praat_HIDDEN, READ1_FileInMemory_create);
		//praat_addMenuCommand (U"Objects", U"New", U"Create copy from FilesInMemory...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_FileInMemorySet_createCopyFromFileInMemorySet);
		praat_addMenuCommand (U"Objects", U"New", U"Create copy from FileInMemorySet...", nullptr, praat_DEPTH_1 + praat_HIDDEN, NEW_FileInMemorySet_createCopyFromFileInMemorySet);
		praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemorySet from directory contents...", nullptr, praat_DEPTH_1 + praat_HIDDEN, NEW_FileInMemorySet_createFromDirectoryContents);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Little Endian file...", U"Read from special sound file", 1, READ1_Sound_readFromRawFileLE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Big Endian file...", U"Read Sound from raw 16-bit Little Endian file...", 1, READ1_Sound_readFromRawFileBE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read KlattTable from raw text file...", U"Read Matrix from raw text file...", praat_HIDDEN, READ1_KlattTable_readFromRawTextFile);

	praat_addAction1 (classActivationList, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classActivationList, 0, U"Formula...", nullptr, 0, MODIFY_ActivationList_formula);
	praat_addAction1 (classActivationList, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classActivationList, 0, U"To Matrix", nullptr, 0, NEW_ActivationList_to_Matrix);
	praat_addAction1 (classActivationList, 0, U"To PatternList", nullptr, 0, NEW_ActivationList_to_PatternList);

	praat_addAction2 (classActivationList, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, NEW1_ActivationList_Categories_to_TableOfReal);

	praat_addAction1 (classBarkFilter, 0, U"BarkFilter help", nullptr, 0, HELP_BarkFilter_help);
	praat_FilterBank_all_init (classBarkFilter);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPRECATED_2014, GRAPHICS_BarkFilter_drawSpectrum);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 1, U"Draw filter functions...", U"Draw filters...", praat_DEPRECATED_2014, GRAPHICS_BarkFilter_drawSekeyHansonFilterFunctions);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"Paint...", U"Draw filters...", praat_DEPTH_1, GRAPHICS_BarkFilter_paint);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"To BarkSpectrogram", nullptr, 0, NEW_BarkFilter_to_BarkSpectrogram);

	praat_addAction1 (classBarkSpectrogram, 0, U"BarkSpectrogram help", nullptr, 0, HELP_BarkSpectrogram_help);
	praat_BandFilterSpectrogram_draw_init (classBarkSpectrogram);
	praat_addAction1 (classBarkSpectrogram, 0, U"Paint image...", nullptr, 1, GRAPHICS_BarkSpectrogram_paintImage);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw Sekey-Hanson auditory filters...", nullptr, 1, GRAPHICS_BarkSpectrogram_drawSekeyHansonAuditoryFilters);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw spectrum at nearest time slice...", nullptr, 1, GRAPHICS_BarkSpectrogram_drawSpectrumAtNearestTimeSlice);
	praat_addAction1 (classBarkSpectrogram, 0, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_BandFilterSpectrogram_query_init (classBarkSpectrogram);
	praat_addAction1 (classBarkSpectrogram, 0, U"Equalize intensities...", nullptr, 0, MODIFY_BandFilterSpectrogram_equalizeIntensities);
	praat_addAction1 (classBarkSpectrogram, 0, U"To Intensity", nullptr, 0, NEW_BandFilterSpectrogram_to_Intensity);
	praat_addAction1 (classBarkSpectrogram, 0, U"To Matrix...", nullptr, 0, NEW_BandFilterSpectrogram_to_Matrix);
	praat_addAction1 (classBarkSpectrogram, 2, U"Cross-correlate...", nullptr, 0, NEW1_BandFilterSpectrograms_crossCorrelate);
	praat_addAction1 (classBarkSpectrogram, 2, U"Convolve...", nullptr, 0, NEW1_BandFilterSpectrograms_convolve);
	
	
	praat_addAction1 (classCategories, 0, U"View & Edit", nullptr, praat_NO_API, WINDOW_Categories_edit);
	praat_addAction1 (classCategories, 0,   U"Edit", U"*View & Edit", praat_DEPRECATED_2015 | praat_NO_API, WINDOW_Categories_edit);
	praat_addAction1 (classCategories, 0, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 1, U"Get number of categories", QUERY_BUTTON, 1, INTEGER_Categories_getNumberOfCategories);
	praat_addAction1 (classCategories, 2, U"Get difference", QUERY_BUTTON, praat_HIDDEN | praat_DEPTH_1, INTEGER_Categories_difference);
	praat_addAction1 (classCategories, 2, U"Get number of differences", QUERY_BUTTON, 1, INTEGER_Categories_getNumberOfDifferences);
	praat_addAction1 (classCategories, 2, U"Get fraction different", QUERY_BUTTON, 1, REAL_Categories_getFractionDifferent);
	praat_addAction1 (classCategories, 0, MODIFY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 1, U"Append category...", MODIFY_BUTTON, 1, MODIFY_Categories_append);
	praat_addAction1 (classCategories, 1, U"Append 1 category...", U"Append category...", praat_HIDDEN | praat_DEPTH_1, MODIFY_Categories_append);
	praat_addAction1 (classCategories, 0, U"Extract", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 0, U"To unique Categories", nullptr, 0, NEW_Categories_selectUniqueItems);
	praat_addAction1 (classCategories, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 2, U"To Confusion", nullptr, 0, NEW_Categories_to_Confusion);
	praat_addAction1 (classCategories, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 2, U"Join", nullptr, 0, NEW1_Categories_join);
	praat_addAction1 (classCategories, 0, U"Permute items", nullptr, 0, NEW_Categories_permuteItems);
	praat_addAction1 (classCategories, 0, U"To Strings", nullptr, 0, NEW_Categories_to_Strings);

	praat_addAction1 (classChebyshevSeries, 0, U"ChebyshevSeries help", nullptr, 0, HELP_ChebyshevSeries_help);
	praat_FunctionTerms_init (classChebyshevSeries);
	praat_addAction1 (classChebyshevSeries, 0, U"To Polynomial", U"Analyse", 0, NEW_ChebyshevSeries_to_Polynomial);

	praat_addAction1 (classCCA, 1, U"CCA help", nullptr, 0, HELP_CCA_help);
	praat_addAction1 (classCCA, 1, U"Draw eigenvector...", nullptr, 0, GRAPHICS_CCA_drawEigenvector);
	praat_addAction1 (classCCA, 1, U"Get number of correlations", nullptr, 0, INTEGER_CCA_getNumberOfCorrelations);
	praat_addAction1 (classCCA, 1, U"Get correlation...", nullptr, 0, REAL_CCA_getCorrelationCoefficient);
	praat_addAction1 (classCCA, 1, U"Get eigenvector element...", nullptr, 0, REAL_CCA_getEigenvectorElement);
	praat_addAction1 (classCCA, 1, U"Get zero correlation probability...", nullptr, 0, REAL_CCA_getZeroCorrelationProbability);
	praat_addAction1 (classCCA, 1, U"Extract Eigen...", nullptr, 0, NEW_CCA_extractEigen);

	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (scores)...", nullptr, 0, NEW_CCA_and_TableOfReal_scores);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (loadings)", nullptr, 0, NEW_CCA_and_TableOfReal_factorLoadings);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"Predict...", nullptr, 0, NEW1_CCA_and_TableOfReal_predict);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"To TableOfReal (loadings)", nullptr, 0, NEW1_CCA_and_Correlation_factorLoadings);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get variance fraction...", nullptr, 0, REAL_CCA_and_Correlation_getVarianceFraction);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get redundancy (sl)...", nullptr, 0, REAL_CCA_and_Correlation_getRedundancy_sl);
	
	
	praat_addAction1 (classComplexSpectrogram, 0, U"ComplexSpectrogram help", nullptr, 0, HELP_ComplexSpectrogram_help);
	praat_addAction1 (classComplexSpectrogram, 0, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Sound...", nullptr, 0, NEW_ComplexSpectrogram_to_Sound);
	praat_addAction1 (classComplexSpectrogram, 0, U"Down to Spectrogram", nullptr, 0, NEW_ComplexSpectrogram_to_Spectrogram);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Spectrum (slice)...", nullptr, 0, NEW_ComplexSpectrogram_to_Spectrum);
	//praat_addAction1 (classComplexSpectrogram, 0, U"Paint...", 0, 1, DO_Spectrogram_paint);

	praat_addAction1 (classConfusion, 0, U"Confusion help", nullptr, 0, HELP_Confusion_help);
	praat_TableOfReal_init2 (classConfusion);
	praat_removeAction (classConfusion, nullptr, nullptr, U"Draw as numbers...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by label...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by column...");
	praat_addAction1 (classConfusion, 0, U"Draw as numbers...", U"Draw -", 1, GRAPHICS_Confusion_drawAsNumbers);
	praat_addAction1 (classConfusion, 1, U"Get value (labels)...", U"Get value...", 1, REAL_Confusion_getValue);
	praat_addAction1 (classConfusion, 0, U"-- confusion statistics --", U"Get value (labels)...", 1, nullptr);
	praat_addAction1 (classConfusion, 1, U"Get fraction correct", U"-- confusion statistics --", 1, REAL_Confusion_getFractionCorrect);
	praat_addAction1 (classConfusion, 1, U"Get stimulus sum...", U"Get fraction correct", 1, REAL_Confusion_getStimulusSum);
	praat_addAction1 (classConfusion, 1, U"Get row sum...", U"Get fraction correct", praat_DEPTH_1 | praat_HIDDEN, REAL_TableOfReal_getRowSum);
	praat_addAction1 (classConfusion, 1, U"Get response sum...", U"Get stimulus sum...", 1, REAL_Confusion_getResponseSum);
	praat_addAction1 (classConfusion, 1, U"Get column sum...", U"Get row sum...", praat_DEPTH_1 | praat_HIDDEN, REAL_TableOfReal_getColumnSum);
	praat_addAction1 (classConfusion, 1, U"Get grand sum", U"Get response sum...", 1, REAL_TableOfReal_getGrandSum);
	praat_addAction1 (classConfusion, 0, U"Increase...", U"Formula...", 1, MODIFY_Confusion_increase);
	praat_addAction1 (classConfusion, 0, U"To TableOfReal (marginals)", U"To TableOfReal", 0, NEW_Confusion_to_TableOfReal_marginals);
	praat_addAction1 (classConfusion, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classConfusion, 0, U"Condense...", nullptr, praat_HIDDEN, NEW_Confusion_condense);
	praat_addAction1 (classConfusion, 0, U"Group...", nullptr, 0, NEW_Confusion_group);
	praat_addAction1 (classConfusion, 0, U"Group stimuli...", nullptr, 0, NEW_Confusion_groupStimuli);
	praat_addAction1 (classConfusion, 0, U"Group responses...", nullptr, 0, NEW_Confusion_groupResponses);
	praat_addAction1 (classConfusion, 2, U"To difference matrix", nullptr, 0, NEW1_Confusion_difference);
	
	praat_addAction2 (classConfusion, 1, classClassificationTable, 1, U"Increase confusion count", nullptr, 0, MODIFY_Confusion_and_ClassificationTable_increase);

	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw confusion...", nullptr, 0, GRAPHICS_Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, U"Covariance help", nullptr, 0, HELP_Covariance_help);
	praat_SSCP_as_TableOfReal_init (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
	praat_addAction1 (classCovariance, 1, U"Get probability at position...", U"Get value...", 1, REAL_Covariance_getProbabilityAtPosition);
	praat_addAction1 (classCovariance, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, REAL_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classCovariance, 1, U"Get significance of one mean...", U"Get diagonality (bartlett)...", 1, REAL_Covariance_getSignificanceOfOneMean);
	praat_addAction1 (classCovariance, 1, U"Get significance of means difference...", U"Get significance of one mean...", 1, REAL_Covariance_getSignificanceOfMeansDifference);
	praat_addAction1 (classCovariance, 1, U"Get significance of one variance...", U"Get significance of means difference...", 1, REAL_Covariance_getSignificanceOfOneVariance);
	praat_addAction1 (classCovariance, 1, U"Get significance of variances ratio...", U"Get significance of one variance...", 1, REAL_Covariance_getSignificanceOfVariancesRatio);
	praat_addAction1 (classCovariance, 1, U"Get fraction variance...", U"Get significance of variances ratio...", 1, REAL_Covariance_getFractionVariance);
	praat_addAction1 (classCovariance, 2, U"Report multivariate mean difference...", U"Get fraction variance...", 1, INFO_Covariances_reportMultivariateMeanDifference);
	praat_addAction1 (classCovariance, 2, U"Difference", U"Report multivariate mean difference...", praat_DEPTH_1 | praat_HIDDEN, INFO_Covariances_reportEquality);
	praat_addAction1 (classCovariance, 0, U"Report equality of covariances", U"Report multivariate mean difference...", praat_DEPTH_1 | praat_HIDDEN, INFO_Covariances_reportEquality);

	praat_addAction1 (classCovariance, 0, U"To TableOfReal (random sampling)...", nullptr, 0, NEW_Covariance_to_TableOfReal_randomSampling);

	praat_addAction1 (classCovariance, 0, U"To Correlation", nullptr, 0, NEW_Covariance_to_Correlation);
	praat_addAction1 (classCovariance, 0, U"To PCA", nullptr, 0, NEW_Covariance_to_PCA);
	
	praat_addAction1 (classCovariance, 0, U"Pool", nullptr, 0, NEW1_Covariances_pool);

	praat_addAction2 (classCovariance, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0, NEW1_Covariance_and_TableOfReal_mahalanobis);

	praat_addAction1 (classClassificationTable, 0, U"ClassificationTable help", nullptr, 0, HELP_ClassificationTable_help);
	praat_TableOfReal_init (classClassificationTable);
	praat_addAction1 (classClassificationTable, 0, U"Get class index at maximum in row...", U"Get column index...", 1, INTEGER_ClassificationTable_getClassIndexAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"Get class label at maximum in row...", U"Get class index at maximum in row...", 1, INTEGER_ClassificationTable_getClassLabelAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"To Confusion", U"*To Confusion...", praat_DEPRECATED_2014, NEW_ClassificationTable_to_Confusion_old); // deprecated 2014
	praat_addAction1 (classClassificationTable, 0, U"To Confusion...", nullptr, 0, NEW_ClassificationTable_to_Confusion);
	praat_addAction1 (classClassificationTable, 0, U"To Correlation (columns)", nullptr, 0, NEW_ClassificationTable_to_Correlation_columns);
	praat_addAction1 (classClassificationTable, 0, U"To Strings (max. prob.)", nullptr, 0, NEW_ClassificationTable_to_Strings_maximumProbability);

	praat_addAction1 (classCorrelation, 0, U"Correlation help", nullptr, 0, HELP_Correlation_help);
	praat_TableOfReal_init2 (classCorrelation);
	praat_SSCP_query_init (classCorrelation);
	praat_SSCP_extract_init (classCorrelation);
	praat_addAction1 (classCorrelation, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, REAL_Correlation_testDiagonality_bartlett);
	praat_addAction1 (classCorrelation, 0, U"Confidence intervals...", nullptr, 0, NEW_Correlation_confidenceIntervals);
	praat_addAction1 (classCorrelation, 0, U"To PCA", nullptr, 0, NEW_Correlation_to_PCA);

	praat_addAction1 (classDiscriminant, 0, U"Discriminant help", 0, 0, HELP_Discriminant_help);
	praat_addAction1 (classDiscriminant, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvalues...", nullptr, 1, GRAPHICS_Discriminant_drawEigenvalues);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvalues (scree)...", nullptr, praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Eigen_drawEigenvalues_scree);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvector...", nullptr, 1, GRAPHICS_Discriminant_drawEigenvector);

	praat_addAction1 (classDiscriminant, 0, U"-- sscps --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 0, U"Draw sigma ellipses...", 0, 1, GRAPHICS_Discriminant_drawSigmaEllipses);
	praat_addAction1 (classDiscriminant, 0, U"Draw one sigma ellipse...", 0, 1, GRAPHICS_Discriminant_drawOneSigmaEllipse);
	praat_addAction1 (classDiscriminant, 0, U"Draw confidence ellipses...", 0, 1, GRAPHICS_Discriminant_drawConfidenceEllipses);

	praat_addAction1 (classDiscriminant, 1, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 1, U"-- eigen structure --", nullptr, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvalue...", nullptr, 1, REAL_Discriminant_getEigenvalue);
	praat_addAction1 (classDiscriminant, 1, U"Get sum of eigenvalues...", nullptr, 1, REAL_Discriminant_getSumOfEigenvalues);
	praat_addAction1 (classDiscriminant, 1, U"Get number of eigenvectors", nullptr, 1, INTEGER_Discriminant_getNumberOfEigenvalues);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector dimension", nullptr, 1, INTEGER_Discriminant_getDimension);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector element...", nullptr, 1, REAL_Discriminant_getEigenvectorElement);

	praat_addAction1 (classDiscriminant, 1, U"-- discriminant --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get number of functions", 0, 1, INTEGER_Discriminant_getNumberOfFunctions);
	praat_addAction1 (classDiscriminant, 1, U"Get dimension of functions", 0, 1, INTEGER_Discriminant_getDimensionOfFunctions);
	praat_addAction1 (classDiscriminant, 1, U"Get number of groups", 0, 1, INTEGER_Discriminant_getNumberOfGroups);
	praat_addAction1 (classDiscriminant, 1, U"Get number of observations...", 0, 1, INTEGER_Discriminant_getNumberOfObservations);
	praat_addAction1 (classDiscriminant, 1, U"-- tests --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get Wilks lambda...", 0, 1, REAL_Discriminant_getWilksLambda);
	praat_addAction1 (classDiscriminant, 1, U"Get cumulative contribution of components...", 0, 1, REAL_Discriminant_getCumulativeContributionOfComponents);
	praat_addAction1 (classDiscriminant, 1, U"Get partial discrimination probability...", 0, 1, REAL_Discriminant_getPartialDiscriminationProbability);
	praat_addAction1 (classDiscriminant, 1, U"Get homogeneity of covariances (box)", 0, praat_DEPTH_1 | praat_HIDDEN, REAL_Discriminant_getHomegeneityOfCovariances_box);
	praat_addAction1 (classDiscriminant, 1, U"Report equality of covariance matrices", 0, 1, INFO_Discriminant_reportEqualityOfCovariances_wald);
	praat_addAction1 (classDiscriminant, 1, U"-- ellipses --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get sigma ellipse area...", 0, 1, REAL_Discriminant_getConcentrationEllipseArea);
	praat_addAction1 (classDiscriminant, 1, U"Get confidence ellipse area...", 0, 1, REAL_Discriminant_getConfidenceEllipseArea);
	praat_addAction1 (classDiscriminant, 1, U"Get ln(determinant_group)...", 0, 1, REAL_Discriminant_getLnDeterminant_group);
	praat_addAction1 (classDiscriminant, 1, U"Get ln(determinant_total)", 0, 1, REAL_Discriminant_getLnDeterminant_total);

	praat_addAction1 (classDiscriminant, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDiscriminant, 1, U"Invert eigenvector...", nullptr, 1, MODIFY_Discriminant_invertEigenvector);
	praat_addAction1 (classDiscriminant, 0, U"Align eigenvectors", nullptr, 1, MODIFY_Eigens_alignEigenvectors);

	praat_addAction1 (classDiscriminant, 0, U"Extract -", nullptr, 0, 0);
		praat_addAction1 (classDiscriminant, 0, U"Extract pooled within-groups SSCP", nullptr, 1, NEW_Discriminant_extractPooledWithinGroupsSSCP);
		praat_addAction1 (classDiscriminant, 0, U"Extract within-group SSCP...", nullptr, 1, NEW_Discriminant_extractWithinGroupSSCP);
		praat_addAction1 (classDiscriminant, 0, U"Extract between-groups SSCP", nullptr, 1, NEW_Discriminant_extractBetweenGroupsSSCP);
		praat_addAction1 (classDiscriminant, 0, U"Extract group centroids", nullptr, 1, NEW_Discriminant_extractGroupCentroids);
		praat_addAction1 (classDiscriminant, 0, U"Extract group standard deviations", nullptr, 1, NEW_Discriminant_extractGroupStandardDeviations);
		praat_addAction1 (classDiscriminant, 0, U"Extract group labels", nullptr, 1, NEW_Discriminant_extractGroupLabels);
		praat_addAction1 (classDiscriminant, 0, U"Extract Eigen", nullptr, 1, NEW_Discriminant_extractEigen);

	praat_addAction1 (classDiscriminant , 0, U"& TableOfReal: To ClassificationTable?", nullptr, 0, hint_Discriminant_and_TableOfReal_to_ClassificationTable);

	/*		praat_addAction1 (classDiscriminant, 1, U"Extract coefficients...", nullptr, 1, DO_Discriminant_extractCoefficients);*/

	//praat_addAction2 (classDiscriminant, 1, classMatrix, 1, U"To Matrix (pc)...", nullptr, 0, DO_Discriminant_and_Matrix_to_Matrix_pc);

	praat_Eigen_Spectrogram_project (classDiscriminant, classSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classMelSpectrogram);
	
	praat_Eigen_Matrix_project (classDiscriminant, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classMelFilter); // deprecated 2014

	praat_addAction2 (classDiscriminant, 1, classPatternList, 1, U"To Categories...", nullptr, 0, NEW1_Discriminant_and_PatternList_to_Categories);
	praat_addAction2 (classDiscriminant, 1, classSSCP, 1, U"Project", nullptr, 0, NEW1_Eigen_and_SSCP_project);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Modify Discriminant", nullptr, 0, 0);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Set group labels", nullptr, 0, MODIFY_Discriminant_setGroupLabels);

	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0, NEW1_Discriminant_and_TableOfReal_to_Configuration);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To ClassificationTable...", nullptr, 0, NEW1_Discriminant_and_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0, NEW1_Discriminant_and_TableOfReal_mahalanobis);


	praat_addAction1 (classDTW, 0, U"DTW help", nullptr, 0, HELP_DTW_help);
	praat_addAction1 (classDTW, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 0, U"Draw path...", nullptr, 1, GRAPHICS_DTW_drawPath);
	praat_addAction1 (classDTW, 0, U"Paint distances...", nullptr, 1, GRAPHICS_DTW_paintDistances);
	praat_addAction1 (classDTW, 0, U"Draw warp (x)...", nullptr, 1, GRAPHICS_DTW_drawWarpX);
	praat_addAction1 (classDTW, 0, U"Draw warp (y)...", nullptr, 1, GRAPHICS_DTW_drawWarpY);
	praat_addAction1 (classDTW, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 1, U"Query time domains", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (x)", nullptr, 2, REAL_DTW_getStartTimeX);
	praat_addAction1 (classDTW, 1, U"Get end time (x)", nullptr, 2, REAL_DTW_getEndTimeX);
	praat_addAction1 (classDTW, 1, U"Get total duration (x)", nullptr, 2, REAL_DTW_getTotalDurationX);
	praat_addAction1 (classDTW, 1, U"-- time domain x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (y)", nullptr, 2, REAL_DTW_getStartTimeY);
	praat_addAction1 (classDTW, 1, U"Get end time (y)", nullptr, 2, REAL_DTW_getEndTimeY);
	praat_addAction1 (classDTW, 1, U"Get total duration (y)", nullptr, 2, REAL_DTW_getTotalDurationY);
	praat_addAction1 (classDTW, 1, U"Query time samplings", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (x)", nullptr, 2, INTEGER_DTW_getNumberOfFramesX);
	praat_addAction1 (classDTW, 1, U"Get time step (x)", nullptr, 2, REAL_DTW_getTimeStepX);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (x)...", nullptr, 2, REAL_DTW_getTimeFromFrameNumberX);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (x)...", nullptr, 2, INTEGER_DTW_getFrameNumberFromTimeX);
	praat_addAction1 (classDTW, 1, U"-- time sampling x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (y)", nullptr, 2, INTEGER_DTW_getNumberOfFramesY);
	praat_addAction1 (classDTW, 1, U"Get time step (y)", nullptr, 2, REAL_DTW_getTimeStepY);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (y)...", nullptr, 2, REAL_DTW_getTimeFromFrameNumberY);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (y)...", nullptr, 2, INTEGER_DTW_getFrameNumberFromTimeY);

	praat_addAction1 (classDTW, 1, U"Get y time from x time...", nullptr, 1, REAL_DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, U"Get x time from y time...", nullptr, 1, REAL_DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, U"Get y time...", nullptr, praat_HIDDEN + praat_DEPTH_1, REAL_DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, U"Get x time...", nullptr, praat_HIDDEN + praat_DEPTH_1, REAL_DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, U"Get maximum consecutive steps...", nullptr, 1, INTEGER_DTW_getMaximumConsecutiveSteps);
	praat_addAction1 (classDTW, 1, U"Get time along path...", nullptr, praat_DEPTH_1 | praat_HIDDEN, REAL_DTW_getPathY);
	praat_addAction1 (classDTW, 1, U"-- distance queries --", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get distance value...", nullptr, 1, REAL_DTW_getDistanceValue);
	praat_addAction1 (classDTW, 1, U"Get minimum distance", nullptr, 1, REAL_DTW_getMinimumDistance);
	praat_addAction1 (classDTW, 1, U"Get maximum distance", nullptr, 1, REAL_DTW_getMaximumDistance);
	praat_addAction1 (classDTW, 1, U"Get distance (weighted)", nullptr, 1, REAL_DTW_getWeightedDistance);
	praat_addAction1 (classDTW, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 0, U"Formula (distances)...", nullptr, 1, MODIFY_DTW_formulaDistances);
	praat_addAction1 (classDTW, 0, U"Set distance value...", nullptr, 1, MODIFY_DTW_setDistanceValue);

	praat_addAction1 (classDTW, 0, U"Analyse", nullptr, 0, 0);
    praat_addAction1 (classDTW, 0, U"Find path...", nullptr, praat_HIDDEN, MODIFY_DTW_findPath);
    praat_addAction1 (classDTW, 0, U"Find path (band & slope)...", nullptr, 0, MODIFY_DTW_findPath_bandAndSlope);
    praat_addAction1 (classDTW, 0, U"To Polygon...", nullptr, 1, NEW_DTW_to_Polygon);
	praat_addAction1 (classDTW, 0, U"To Matrix (distances)", nullptr, 0, NEW_DTW_to_Matrix_distances);
    praat_addAction1 (classDTW, 0, U"To Matrix (cum. distances)...", nullptr, 0, NEW_DTW_to_Matrix_cumulativeDistances);
	praat_addAction1 (classDTW, 0, U"Swap axes", nullptr, 0, NEW_DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classMatrix, 1, U"Replace matrix", nullptr, 0, MODIFY_DTW_and_Matrix_replace);
	praat_addAction2 (classDTW, 1, classTextGrid, 1, U"To TextGrid (warp times)", nullptr, 0, NEW1_DTW_and_TextGrid_to_TextGrid);
	praat_addAction2 (classDTW, 1, classIntervalTier, 1, U"To Table (distances)", nullptr, 0, NEW1_DTW_and_IntervalTier_to_Table);

    praat_addAction2 (classDTW, 1, classPolygon, 1, U"Find path inside...", nullptr, 0, MODIFY_DTW_and_Polygon_findPathInside);
    praat_addAction2 (classDTW, 1, classPolygon, 1, U"To Matrix (cum. distances)...", nullptr, 0, NEW1_DTW_and_Polygon_to_Matrix_cumulativeDistances);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw...", nullptr, 0, GRAPHICS_DTW_and_Sounds_draw);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw warp (x)...", nullptr, 0, GRAPHICS_DTW_and_Sounds_drawWarpX);

	praat_addAction1 (classEditDistanceTable, 1, U"EditDistanceTable help", nullptr, 0, HELP_EditDistanceTable_help);
	praat_EditDistanceTable_as_TableOfReal_init (classEditDistanceTable);
	praat_addAction1 (classEditDistanceTable, 1, U"To TableOfReal (directions)...", nullptr, praat_HIDDEN, NEW_EditDistanceTable_to_TableOfReal_directions);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, U"Set new edit costs", nullptr, 0, MODIFY_EditDistanceTable_setEditCosts);

	praat_addAction1 (classEditCostsTable, 1, U"EditCostsTable help", nullptr, 0, HELP_EditCostsTable_help);
	praat_addAction1 (classEditCostsTable, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, U"Get target index...", nullptr, 1, INTEGER_EditCostsTable_getTargetIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get source index...", nullptr, 1, INTEGER_EditCostsTable_getSourceIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get insertion cost...", nullptr, 1, REAL_EditCostsTable_getInsertionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get deletion cost...", nullptr, 1, REAL_EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get substitution cost...", nullptr, 1, REAL_EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get cost (others)...", nullptr, 1, REAL_EditCostsTable_getOthersCost);
	praat_addAction1 (classEditCostsTable, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, U"Set target symbol (index)...", nullptr, 1, MODIFY_EditCostsTable_setTargetSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set source symbol (index)...", nullptr, 1, MODIFY_EditCostsTable_setSourceSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set insertion costs...", nullptr, 1, MODIFY_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set deletion costs...", nullptr, 1, MODIFY_EditCostsTable_setDeletionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set substitution costs...", nullptr, 1, MODIFY_EditCostsTable_setSubstitutionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set costs (others)...", nullptr, 1, MODIFY_EditCostsTable_setOthersCosts);
	praat_addAction1 (classEditCostsTable, 1, U"To TableOfReal", nullptr, 0, NEW_EditCostsTable_to_TableOfReal);

	praat_Index_init (classStringsIndex);
	praat_addAction1 (classIndex, 0, U"Index help", nullptr, 0, HELP_Index_help);
	praat_addAction1 (classStringsIndex, 1, U"Get class label...", nullptr, 0, INFO_StringsIndex_getClassLabelFromClassIndex);
	praat_addAction1 (classStringsIndex, 1, U"Get class index...", nullptr, 0, INTEGER_StringsIndex_getClassIndexFromClassLabel);
	praat_addAction1 (classStringsIndex, 1, U"Get label...", nullptr, 0, INFO_StringsIndex_getItemLabelFromItemIndex);
	praat_addAction1 (classStringsIndex, 1, U"Get class index from item index...", nullptr, 0, INTEGER_Index_getClassIndexFromItemIndex);
	praat_addAction1 (classIndex, 1, U"Get index...", nullptr, 0, INTEGER_Index_getClassIndexFromItemIndex);
	praat_addAction1 (classStringsIndex, 1, U"To Strings", nullptr, 0, NEW_StringsIndex_to_Strings);

	praat_addAction1 (classEigen, 0, U"Eigen help", nullptr, 0, HELP_Eigen_help);
	praat_addAction1 (classEigen, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 0, U"Draw eigenvalues (scree)...", nullptr, praat_DEPTH_1 | praat_DEPRECATED_2010, GRAPHICS_Eigen_drawEigenvalues_scree);
		praat_addAction1 (classEigen, 0, U"Draw eigenvalues...", nullptr, 1, GRAPHICS_Eigen_drawEigenvalues);
		praat_addAction1 (classEigen, 0, U"Draw eigenvector...", nullptr, 1, GRAPHICS_Eigen_drawEigenvector);
	praat_addAction1 (classEigen, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 1, U"Get number of eigenvalues", nullptr, 1, INTEGER_Eigen_getNumberOfEigenvalues);
		praat_addAction1 (classEigen, 1, U"Get eigenvalue...", nullptr, 1, REAL_Eigen_getEigenvalue);
		praat_addAction1 (classEigen, 1, U"Get sum of eigenvalues...", nullptr, 1, REAL_Eigen_getSumOfEigenvalues);
	praat_addAction1 (classEigen, 1, U"-- eigenvectors --", nullptr, 1, 0);
		praat_addAction1 (classEigen, 1, U"Get number of eigenvectors", nullptr, 1, INTEGER_Eigen_getNumberOfEigenvalues);
		praat_addAction1 (classEigen, 1, U"Get eigenvector dimension", nullptr, 1, INTEGER_Eigen_getDimension);
		praat_addAction1 (classEigen, 1, U"Get eigenvector element...", nullptr, 1, REAL_Eigen_getEigenvectorElement);
	praat_addAction1 (classEigen, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 1, U"Invert eigenvector...", nullptr, 1, MODIFY_Eigen_invertEigenvector);
	praat_addAction1 (classExcitation, 0, U"Synthesize", U"To Formant...", 0, 0);
	praat_addAction1 (classExcitation, 0, U"To ExcitationList", U"Synthesize", 0, NEW1_Excitations_to_ExcitationList);
	praat_addAction1 (classExcitation, 0, U"To Excitations", U"Synthesize", praat_DEPRECATED_2015, NEW1_Excitations_to_ExcitationList);

	praat_addAction1 (classExcitationList, 0, U"Modify", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"Formula...", nullptr, 0, MODIFY_ExcitationList_formula);
	praat_addAction1 (classExcitationList, 0, U"Extract", nullptr, 0, 0);
		praat_addAction1 (classExcitationList, 0, U"Extract Excitation...", nullptr, 0, NEW_ExcitationList_extractItem);
	praat_addAction1 (classExcitationList, 0, U"Synthesize", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 2, U"Append", nullptr, 0, NEW1_ExcitationList_append);
	praat_addAction1 (classExcitationList, 0, U"Convert", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"To PatternList...", nullptr, 0, NEW_ExcitationList_to_PatternList);
	praat_addAction1 (classExcitationList, 0, U"To Pattern...", nullptr, praat_HIDDEN, NEW_ExcitationList_to_PatternList);
	praat_addAction1 (classExcitationList, 0, U"To TableOfReal", nullptr, 0, NEW_ExcitationList_to_TableOfReal);

	praat_addAction2 (classExcitationList, 1, classExcitation, 0, U"Add to ExcitationList", nullptr, 0, MODIFY_ExcitationList_addItem);
	praat_addAction2 (classExcitationList, 1, classExcitation, 0, U"Add to Excitations", nullptr, praat_HIDDEN, MODIFY_ExcitationList_addItem);

	praat_addAction1 (classFileInMemory, 1, U"Show as code...", nullptr, 0, INFO_FileInMemory_showAsCode);
	praat_addAction1 (classFileInMemory, 1, U"Set id...", nullptr, 0, MODIFY_FileInMemory_setId);
	praat_addAction1 (classFileInMemory, 0, U"To FileInMemorySet", nullptr, 0, NEW1_FileInMemory_to_FileInMemorySet);
	praat_addAction1 (classFileInMemory, 0, U"To FilesInMemory", nullptr, praat_DEPRECATED_2015, NEW1_FileInMemory_to_FileInMemorySet);

	praat_addAction1 (classFileInMemorySet, 1, U"Show as code...", nullptr, 0, INFO_FileInMemorySet_showAsCode);
	praat_addAction1 (classFileInMemorySet, 1, U"Show one file as code...", nullptr, 0, INFO_FileInMemorySet_showOneFileAsCode);
	praat_addAction1 (classFileInMemorySet, 2, U"Merge", nullptr, 0, NEW1_FileInMemorySets_merge);
	praat_addAction1 (classFileInMemorySet, 0, U"To Strings (id)", nullptr, 0, NEW_FileInMemorySet_to_Strings_id);

	praat_addAction2 (classFileInMemorySet, 1, classFileInMemory, 0, U"Add items to Collection", nullptr, 0, MODIFY_FileInMemorySet_addItems);

	praat_addAction1 (classFormantFilter, 0, U"FormantFilter help", nullptr, praat_DEPRECATED_2015, HELP_FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPTH_1 | praat_DEPRECATED_2014, GRAPHICS_FormantFilter_drawSpectrum);
	praat_addAction1 (classFormantFilter, 0, U"Draw filter functions...", U"Draw filters...",  praat_DEPTH_1  | praat_DEPRECATED_2014, GRAPHICS_FormantFilter_drawFilterFunctions);
	praat_addAction1 (classFormantFilter, 0, U"To Spectrogram", nullptr, 0, NEW_FormantFilter_to_Spectrogram);

	praat_addAction1 (classFormantGrid, 0, U"Draw...", U"Edit", praat_DEPTH_1 + praat_HIDDEN, GRAPHICS_FormantGrid_draw);

	praat_addAction1 (classIntensity, 0, U"To TextGrid (silences)...", U"To IntensityTier (valleys)", 0, NEW_Intensity_to_TextGrid_detectSilences);
	praat_addAction1 (classIntensityTier, 0, U"To TextGrid (silences)...", nullptr, 0, NEW_IntensityTier_to_TextGrid_detectSilences);
	praat_addAction1 (classIntensityTier, 0, U"To Intensity...", nullptr, praat_HIDDEN, NEW_IntensityTier_to_Intensity);

	praat_addAction1 (classISpline, 0, U"ISpline help", nullptr, 0, HELP_ISpline_help);
	praat_Spline_init (classISpline);

	praat_addAction1 (classKlattTable, 0, U"KlattTable help", nullptr, 0, HELP_KlattTable_help);
	praat_addAction1 (classKlattTable, 0, U"To Sound...", nullptr, 0, NEW_KlattTable_to_Sound);
	praat_addAction1 (classKlattTable, 0, U"To KlattGrid...", nullptr, 0, NEW_KlattTable_to_KlattGrid);
	praat_addAction1 (classKlattTable, 0, U"To Table", nullptr, 0, NEW_KlattTable_to_Table);

	praat_addAction1 (classLegendreSeries, 0, U"LegendreSeries help", nullptr, 0, HELP_LegendreSeries_help);
	praat_FunctionTerms_init (classLegendreSeries);
	praat_addAction1 (classLegendreSeries, 0, U"To Polynomial", U"Analyse", 0, NEW_LegendreSeries_to_Polynomial);

	praat_addAction1 (classLongSound, 0, U"Append to existing sound file...", nullptr, 0, READ1_LongSounds_appendToExistingSoundFile);
	praat_addAction1 (classSound, 0, U"Append to existing sound file...", nullptr, 0, READ1_LongSounds_appendToExistingSoundFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Append to existing sound file...", nullptr, 0, READ1_LongSounds_appendToExistingSoundFile);

	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFF file...", U"Save as NIST file...", 1, SAVE_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFF file...", U"Write to NIST file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_writeToStereoAiffFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFC file...", U"Save as stereo AIFF file...", 1, SAVE_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFC file...", U"Write to stereo AIFF file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_writeToStereoAifcFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo WAV file...", U"Save as stereo AIFC file...", 1, SAVE_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo WAV file...", U"Write to stereo AIFC file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_writeToStereoWavFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NeXt/Sun file...", U"Save as stereo WAV file...", 1, SAVE_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NeXt/Sun file...", U"Write to stereo WAV file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_writeToStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NIST file...", U"Save as stereo NeXt/Sun file...", 1, SAVE_LongSounds_writeToStereoNistFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NIST file...", U"Write to stereo NeXt/Sun file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_writeToStereoNistFile);

	praat_addAction1 (classLtas, 0, U"Report spectral tilt...", U"Get slope...", 1, INFO_Ltas_reportSpectralTilt);

	praat_addAction1 (classMatrix, 0, U"Scatter plot...", U"Paint cells...", 1, GRAPHICS_Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, U"Draw as squares...", U"Scatter plot...", 1, GRAPHICS_Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, U"Draw distribution...", U"Draw as squares...", 1, GRAPHICS_Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, U"Draw cumulative distribution...", U"Draw distribution...", 1, GRAPHICS_Matrix_drawCumulativeDistribution);
	praat_addAction1 (classMatrix, 0, U"Get mean...", U"Get sum", 1, REAL_Matrix_getMean);
	praat_addAction1 (classMatrix, 0, U"Get standard deviation...", U"Get mean...", 1, REAL_Matrix_getStandardDeviation);
	praat_addAction1 (classMatrix, 0, U"Transpose", U"Synthesize", 0, NEW_Matrix_transpose);
	praat_addAction1 (classMatrix, 0, U"Solve equation...", U"Analyse", 0, NEW_Matrix_solveEquation);
	praat_addAction1 (classMatrix, 0, U"To PCA (by rows)", U"Solve equation...", 0, NEW_Matrix_to_PCA_byRows);
	praat_addAction1 (classMatrix, 0, U"To PCA (by columns)", U"To PCA (by rows)", 0, NEW_Matrix_to_PCA_byColumns);
	praat_addAction1 (classMatrix, 0, U"To PatternList...", U"To VocalTract", 1, NEW_Matrix_to_PatternList);
	praat_addAction1 (classMatrix, 0, U"To Pattern...", U"*To PatternList...", praat_DEPRECATED_2016, NEW_Matrix_to_PatternList);
	praat_addAction1 (classMatrix, 0, U"To ActivationList", U"To PatternList...", 1, NEW_Matrix_to_ActivationList);
	praat_addAction1 (classMatrix, 0, U"To Activation", U"*To ActivationList", praat_DEPRECATED_2016, NEW_Matrix_to_ActivationList);
	praat_addAction1 (classMatrix, 2, U"To DTW...", U"To ParamCurve", 1, NEW1_Matrices_to_DTW);

	praat_addAction2 (classMatrix, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, NEW1_Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classMelSpectrogram, 0, U"MelSpectrogram help", nullptr, 0, HELP_MelSpectrogram_help);
	praat_BandFilterSpectrogram_draw_init (classMelSpectrogram);
	praat_addAction1 (classMelSpectrogram, 0, U"Paint image...", nullptr, 1, GRAPHICS_MelSpectrogram_paintImage);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw triangular filter functions...", nullptr, 1, GRAPHICS_MelSpectrogram_drawTriangularFilterFunctions);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw spectrum at nearest time slice...", nullptr, 1, GRAPHICS_MelSpectrogram_drawSpectrumAtNearestTimeSlice);
	praat_addAction1 (classMelSpectrogram, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_BandFilterSpectrogram_query_init (classMelSpectrogram);

	praat_addAction1 (classMelSpectrogram, 0, U"Equalize intensities...", nullptr, 0, MODIFY_BandFilterSpectrogram_equalizeIntensities);
	praat_addAction1 (classMelSpectrogram, 0, U"To MFCC...", nullptr, 0, NEW_MelSpectrogram_to_MFCC);
	praat_addAction1 (classMelSpectrogram, 0, U"To Intensity", nullptr, 0, NEW_BandFilterSpectrogram_to_Intensity);
	praat_addAction1 (classMelSpectrogram, 0, U"To Matrix...", nullptr, 0, NEW_BandFilterSpectrogram_to_Matrix);
	praat_addAction1 (classMelSpectrogram, 2, U"Cross-correlate...", nullptr, 0, NEW1_BandFilterSpectrograms_crossCorrelate);
	praat_addAction1 (classMelSpectrogram, 2, U"Convolve...", nullptr, 0, NEW1_BandFilterSpectrograms_convolve);
	
	praat_addAction1 (classMelFilter, 0, U"MelFilter help", U"*MelSpectrogram help", 0, HELP_MelFilter_help);
	praat_FilterBank_all_init (classMelFilter); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPTH_1 | praat_DEPRECATED_2014, GRAPHICS_MelFilter_drawSpectrum); 
	praat_addAction1 (classMelFilter, 0, U"Draw filter functions...", U"Draw filters...", praat_DEPTH_1 | praat_DEPRECATED_2014, GRAPHICS_MelFilter_drawFilterFunctions);
	praat_addAction1 (classMelFilter, 0, U"Paint...", U"Draw filter functions...", praat_DEPTH_1 | praat_DEPRECATED_2014, GRAPHICS_MelFilter_paint);
	praat_addAction1 (classMelFilter, 0, U"To MFCC...", nullptr, praat_DEPRECATED_2014, NEW_MelFilter_to_MFCC); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"To MelSpectrogram", nullptr, 0, NEW_MelFilter_to_MelSpectrogram);
	
	praat_addAction1 (classMFCC, 0, U"MFCC help", nullptr, 0, HELP_MFCC_help);
	praat_CC_init (classMFCC);
	praat_addAction1 (classMFCC, 0, U"To MelFilter...", nullptr, praat_DEPRECATED_2014, NEW_MFCC_to_MelFilter);
	praat_addAction1 (classMFCC, 0, U"To MelSpectrogram...", nullptr, 0, NEW_MFCC_to_MelSpectrogram);
	praat_addAction1 (classMFCC, 0, U"To TableOfReal...", nullptr, 0, NEW_MFCC_to_TableOfReal);
	praat_addAction1 (classMFCC, 0, U"To Matrix (features)...", nullptr, praat_HIDDEN, NEW_MFCC_to_Matrix_features);
	praat_addAction1 (classMFCC, 0, U"To Sound", nullptr, praat_HIDDEN, NEW_MFCC_to_Sound);
	praat_addAction1 (classMFCC, 2, U"Cross-correlate...", nullptr, 0, NEW1_MFCCs_crossCorrelate);
	praat_addAction1 (classMFCC, 2, U"Convolve...", nullptr, 0, NEW1_MFCCs_convolve);

	praat_addAction1 (classMSpline, 0, U"MSpline help", nullptr, 0, HELP_MSpline_help);
	praat_Spline_init (classMSpline);

	praat_addAction1 (classPatternList, 0, U"Draw", nullptr, 0, 0);
	praat_addAction1 (classPatternList, 0, U"Draw...", nullptr, 0, GRAPHICS_PatternList_draw);
	praat_PatternList_query_init (classPatternList);
	praat_addAction1 (classPatternList, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classPatternList, 0, U"Formula...", nullptr, 1, MODIFY_PatternList_formula);
	praat_addAction1 (classPatternList, 0, U"Set value...", nullptr, 1, MODIFY_PatternList_setValue);
	praat_addAction1 (classPatternList, 0, U"To Matrix", nullptr, 0, NEW_PatternList_to_Matrix);

	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, NEW1_Matrix_Categories_to_TableOfReal);
	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To Discriminant", nullptr, 0, NEW1_PatternList_and_Categories_to_Discriminant);

	praat_addAction1 (classPCA, 0, U"PCA help", nullptr, 0, HELP_PCA_help);
	praat_addAction1 (classPCA, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_Eigen_draw_init (classPCA);
	praat_addAction1 (classPCA, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_Eigen_query_init (classPCA);
	praat_addAction1 (classPCA, 1, U"-- pca --", nullptr, 1, 0);
	praat_addAction1 (classPCA, 1, U"Get centroid element...", nullptr, 1, REAL_PCA_getCentroidElement);
	praat_addAction1 (classPCA, 1, U"Get equality of eigenvalues...", nullptr, 1, REAL_PCA_getEqualityOfEigenvalues);
	praat_addAction1 (classPCA, 1, U"Get fraction variance accounted for...", nullptr, 1, REAL_PCA_getFractionVAF);
	praat_addAction1 (classPCA, 1, U"Get number of components (VAF)...", nullptr, 1, INTEGER_PCA_getNumberOfComponentsVAF);
	praat_addAction1 (classPCA, 2, U"Get angle between pc1-pc2 planes", nullptr, 1, REAL_PCAs_getAngleBetweenPc1Pc2Plane_degrees);
	praat_addAction1 (classPCA, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classPCA, 1, U"Invert eigenvector...", nullptr, 1, MODIFY_PCA_invertEigenvector);
	praat_addAction1 (classPCA, 0, U"Align eigenvectors", nullptr, 1, MODIFY_Eigens_alignEigenvectors);
	praat_addAction1 (classPCA, 0, U"Extract -", nullptr, 0, 0);
		praat_addAction1 (classPCA, 0, U"Extract eigenvector...", nullptr, 1, NEW_PCA_extractEigenvector);
		praat_addAction1 (classPCA, 0, U"Extract Eigen", nullptr, 1, NEW_PCA_extractEigen);
	praat_addAction1 (classPCA, 2, U"To Procrustes...", nullptr, 0, NEW1_PCAs_to_Procrustes);
	praat_addAction1 (classPCA, 0, U"To TableOfReal (reconstruct 1)...", nullptr, 0, NEW_PCA_to_TableOfReal_reconstruct1);
	
	praat_addAction1 (classPCA, 0, U"& TableOfReal: To Configuration?", nullptr, praat_NO_API, HINT_hint_PCA_and_TableOfReal_to_Configuration);
	praat_addAction1 (classPCA, 0, U"& Configuration (reconstruct)?", nullptr, praat_NO_API, HINT_hint_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction1 (classPCA, 0, U"& Covariance: Project?", nullptr, praat_NO_API, HINT_hint_PCA_and_Covariance_Project);
	praat_addAction2 (classPCA, 1, classConfiguration, 1, U"To TableOfReal (reconstruct)", nullptr, 0, NEW_PCA_and_Configuration_to_TableOfReal_reconstruct);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (pc)...", nullptr, praat_HIDDEN, NEW1_PCA_and_Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project rows)...", nullptr, 0, NEW1_PCA_and_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project columns)...", nullptr, 0, NEW1_PCA_and_Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classPatternList, 1, U"To Matrix (project rows)...", nullptr, 0, NEW1_PCA_and_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classSSCP, 1, U"Project", nullptr, 0, NEW1_Eigen_and_SSCP_project);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (project rows)...", nullptr, 0, NEW1_PCA_and_TableOfReal_to_TableOfReal_projectRows);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal...", U"*To TableOfReal (project rows)...", praat_DEPRECATED_2016, NEW1_PCA_and_TableOfReal_to_TableOfReal_projectRows);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0, NEW1_PCA_and_TableOfReal_to_Configuration);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (z-scores)...", nullptr, 0, NEW1_PCA_and_TableOfReal_to_TableOfReal_zscores);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"Get fraction variance...", nullptr, 0, REAL_PCA_and_TableOfReal_getFractionVariance);
	praat_addAction2 (classPCA, 1, classCovariance, 1, U"Project", nullptr, 0, NEW1_Eigen_and_Covariance_project);

	praat_Eigen_Spectrogram_project (classPCA, classSpectrogram);
	praat_Eigen_Spectrogram_project (classPCA, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classPCA, classMelSpectrogram);

	praat_Eigen_Matrix_project (classPCA, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classPCA, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classPCA, classMelFilter); // deprecated 2014

	praat_addAction1 (classPermutation, 0, U"Permutation help", nullptr, 0, HELP_Permutation_help);
	praat_addAction1 (classPermutation, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, U"Get number of elements", nullptr, 1, INTEGER_Permutation_getNumberOfElements);
	praat_addAction1 (classPermutation, 1, U"Get value...", nullptr, 1, INTEGER_Permutation_getValueAtIndex);
	praat_addAction1 (classPermutation, 1, U"Get index...", nullptr, 1, INTEGER_Permutation_getIndexAtValue);
	praat_addAction1 (classPermutation, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classPermutation, 1, U"Sort", nullptr, 1, MODIFY_Permutation_sort);
	praat_addAction1 (classPermutation, 1, U"Swap blocks...", nullptr, 1, MODIFY_Permutation_swapBlocks);
	praat_addAction1 (classPermutation, 1, U"Swap numbers...", nullptr, 1, MODIFY_Permutation_swapNumbers);
	praat_addAction1 (classPermutation, 1, U"Swap positions...", nullptr, 1, MODIFY_Permutation_swapPositions);
	praat_addAction1 (classPermutation, 1, U"Swap one from range...", nullptr, 1, MODIFY_Permutation_swapOneFromRange);
	praat_addAction1 (classPermutation, 0, U"-- sequential permutations --", nullptr, 1, 0);
	praat_addAction1 (classPermutation, 0, U"Next", nullptr, 1, MODIFY_Permutations_next);
	praat_addAction1 (classPermutation, 0, U"Previous", nullptr, 1, MODIFY_Permutations_previous);
	
	praat_addAction1 (classPermutation, 1, U"Permute randomly...", nullptr, 0, NEW_Permutation_permuteRandomly);
	praat_addAction1 (classPermutation, 1, U"Permute randomly (blocks)...", nullptr, 0, NEW_Permutation_permuteBlocksRandomly);
	praat_addAction1 (classPermutation, 1, U"Interleave...", nullptr, 0, NEW_Permutation_interleave);
	praat_addAction1 (classPermutation, 1, U"Rotate...", nullptr, 0, NEW_Permutation_rotate);
	praat_addAction1 (classPermutation, 1, U"Reverse...", nullptr, 0, NEW_Permutation_reverse);
	praat_addAction1 (classPermutation, 1, U"Invert", nullptr, 0, NEW_Permutation_invert);
	praat_addAction1 (classPermutation, 0, U"Multiply", nullptr, 0, NEW1_Permutations_multiply);

	praat_addAction1 (classPitch, 2, U"To DTW...", U"To PointProcess", praat_HIDDEN, NEW1_Pitches_to_DTW);

	praat_addAction1 (classPitchTier, 0, U"To Pitch...", U"To Sound (sine)...", 1, NEW_PitchTier_to_Pitch);
	praat_addAction1 (classPitchTier, 0, U"Modify interval...", U"Add point...", 1, MODIFY_PitchTier_modifyInterval); 
	praat_addAction1 (classPitchTier, 0, U"Modify interval (tone levels)...", U"Modify interval...", 1, MODIFY_PitchTier_modifyInterval_toneLevels); 
	praat_addAction1 (classPolygon, 0, QUERY_BUTTON, U"Paint circles...", 0, 0);
	praat_addAction1 (classPolygon, 0, U"Get number of points", QUERY_BUTTON, 1, INTEGER_Polygon_getNumberOfPoints);
	praat_addAction1 (classPolygon, 0, U"Get point (x)...", U"Get number of points", 1, REAL_Polygon_getPointX);
	praat_addAction1 (classPolygon, 0, U"Get point (y)...",  U"Get point (x)...", 1, REAL_Polygon_getPointY);
	praat_addAction1 (classPolygon, 0, U"-- other queries --",  U"Get point (y)...", 1, 0);
	praat_addAction1 (classPolygon, 0, U"Get location of point...", U"-- other queries --", 1, INFO_Polygon_getLocationOfPoint);
	praat_addAction1 (classPolygon, 0, U"Get area of convex hull...", U"Get location of point...", praat_DEPTH_1 + praat_HIDDEN, REAL_Polygon_getAreaOfConvexHull);
	
	praat_addAction1 (classPolygon, 0, U"Translate...", MODIFY_BUTTON, 1, MODIFY_Polygon_translate);
	praat_addAction1 (classPolygon, 0, U"Rotate...", U"Translate...", 1, MODIFY_Polygon_rotate);
	praat_addAction1 (classPolygon, 0, U"Scale...", U"Rotate...", 1, MODIFY_Polygon_scale);
	praat_addAction1 (classPolygon, 0, U"Reverse X", U"Scale...", 1, MODIFY_Polygon_reverseX);
	praat_addAction1 (classPolygon, 0, U"Reverse Y", U"Reverse X", 1, MODIFY_Polygon_reverseY);
	praat_addAction1 (classPolygon, 0, U"Simplify", nullptr, praat_HIDDEN, NEW_Polygon_simplify);
	praat_addAction1 (classPolygon, 0, U"Convex hull", nullptr, 0, NEW_Polygon_convexHull);
	praat_addAction1 (classPolygon, 0, U"Circular permutation...", nullptr, praat_HIDDEN, NEW_Polygon_circularPermutation);

	praat_addAction2 (classPolygon, 1, classCategories, 1, U"Draw...", nullptr, 0, GRAPHICS_Polygon_Categories_draw);

	praat_addAction1 (classPolynomial, 0, U"Polynomial help", nullptr, 0, HELP_Polynomial_help);
	praat_FunctionTerms_init (classPolynomial);
	praat_addAction1 (classPolynomial, 0, U"-- area --", U"Get x of maximum...", 1, 0);
	praat_addAction1 (classPolynomial, 1, U"Get area...", U"-- area --", 1, REAL_Polynomial_getArea);
	praat_addAction1 (classPolynomial, 1, U"Get remainder after division...", U"Get area...", 1, REAL_Polynomial_getRemainderAfterDivision);
	praat_addAction1 (classPolynomial, 1, U"Get remainder...", U"*Get remainder after division...", praat_DEPRECATED_2016, REAL_Polynomial_getRemainderAfterDivision);
	praat_addAction1 (classPolynomial, 0, U"-- monic --", U"Set coefficient...", 1, 0);
	praat_addAction1 (classPolynomial, 0, U"Scale coefficients (monic)", U"-- monic --", 1, MODIFY_Polynomial_scaleCoefficients_monic);
	praat_addAction1 (classPolynomial, 1, U"Divide (second order factor)...", U"Scale coefficients (monic)", 1, MODIFY_Polynomial_divide_secondOrderFactor);
	
	praat_addAction1 (classPolynomial, 1, U"Get value (complex)...", U"Get value...", 1, INFO_Polynomial_evaluate_z);
	praat_addAction1 (classPolynomial, 1, U"Get derivatives at X...", U"Get value (complex)...", 1, INFO_Polynomial_getDerivativesAtX);
	praat_addAction1 (classPolynomial, 1, U"Get one real root...", U"Get derivatives at X...", 1, REAL_Polynomial_getOneRealRoot);
	praat_addAction1 (classPolynomial, 0, U"To Spectrum...", U"Analyse", 0, NEW_Polynomial_to_Spectrum);
	praat_addAction1 (classPolynomial, 0, U"To Roots", nullptr, 0, NEW_Polynomial_to_Roots);
	praat_addAction1 (classPolynomial, 0, U"To Polynomial (derivative)", nullptr, 0, NEW_Polynomial_getDerivative);
	praat_addAction1 (classPolynomial, 0, U"To Polynomial (primitive)...", nullptr, 0, NEW_Polynomial_getPrimitive);
	praat_addAction1 (classPolynomial, 0, U"Scale x...", nullptr, 0, NEW_Polynomial_scaleX);
	praat_addAction1 (classPolynomial, 2, U"Multiply", nullptr, 0, NEW_Polynomials_multiply);
	praat_addAction1 (classPolynomial, 2, U"Divide...", nullptr, 0, NEWMANY_Polynomials_divide);

	praat_addAction1 (classRoots, 1, U"Roots help", nullptr, 0, HELP_Roots_help);
	praat_addAction1 (classRoots, 1, U"Draw...", nullptr, 0, GRAPHICS_Roots_draw);
	praat_addAction1 (classRoots, 1, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classRoots, 1, U"Get number of roots", nullptr, 1, INTEGER_Roots_getNumberOfRoots);
	praat_addAction1 (classRoots, 1, U"-- roots --", nullptr, 1, 0);
	praat_addAction1 (classRoots, 1, U"Get root...", nullptr, 1, COMPLEX_Roots_getRoot);
	praat_addAction1 (classRoots, 1, U"Get real part of root...", nullptr, 1, REAL_Roots_getRealPartOfRoot);
	praat_addAction1 (classRoots, 1, U"Get imaginary part of root...", nullptr, 1, REAL_Roots_getImaginaryPartOfRoot);
	praat_addAction1 (classRoots, 1, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classRoots, 1, U"Set root...", nullptr, 1, MODIFY_Roots_setRoot);
	praat_addAction1 (classRoots, 0, U"Analyse", nullptr, 0, 0);
	praat_addAction1 (classRoots, 0, U"To Spectrum...", nullptr, 0, NEW_Roots_to_Spectrum);

	praat_addAction2 (classRoots, 1, classPolynomial, 1, U"Polish roots", nullptr, 0, MODIFY_Roots_and_Polynomial_polish);

	praat_addAction1 (classSound, 0, U"To TextGrid (silences)...", U"To IntervalTier", 1, NEW_Sound_to_TextGrid_detectSilences);
    praat_addAction1 (classSound, 0, U"Play one channel...", U"Play", praat_HIDDEN, PLAY_Sound_playOneChannel);
    praat_addAction1 (classSound, 0, U"Play as frequency shifted...", U"Play", praat_HIDDEN, PLAY_Sound_playAsFrequencyShifted);
	praat_addAction1 (classSound, 0, U"Draw where...", U"Draw...", 1, GRAPHICS_Sound_drawWhere);
	//	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_paintWhere);
	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", 1, GRAPHICS_Sound_paintWhere);
	//	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sounds_paintEnclosed);
	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", 1, GRAPHICS_Sounds_paintEnclosed);

	praat_addAction1 (classSound, 0, U"To Pitch (shs)...", U"To Pitch (cc)...", 1, NEW_Sound_to_Pitch_shs);
	praat_addAction1 (classSound, 0, U"Fade in...", U"Multiply by window...", praat_HIDDEN + praat_DEPTH_1, MODIFY_Sound_fadeIn);
	praat_addAction1 (classSound, 0, U"Fade out...", U"Fade in...", praat_HIDDEN + praat_DEPTH_1, MODIFY_Sound_fadeOut);
	praat_addAction1 (classSound, 0, U"To Pitch (SPINET)...", U"To Pitch (cc)...", 1, NEW_Sound_to_Pitch_SPINET);

	praat_addAction1 (classSound, 0, U"To FormantFilter...", U"To Cochleagram (edb)...", praat_DEPRECATED_2014 | praat_DEPTH_1, NEW_Sound_to_FormantFilter);
	praat_addAction1 (classSound, 0, U"To Spectrogram (pitch-dependent)...", U"To Cochleagram (edb)...", 1, NEW_Sound_to_Spectrogram_pitchDependent);

	praat_addAction1 (classSound, 0, U"To BarkFilter...", U"To FormantFilter...", praat_DEPRECATED_2014 | praat_DEPTH_1, NEW_Sound_to_BarkFilter);
	praat_addAction1 (classSound, 0, U"To BarkSpectrogram...", U"To FormantFilter...", praat_DEPTH_1, NEW_Sound_to_BarkSpectrogram);

	praat_addAction1 (classSound, 0, U"To MelFilter...", U"To BarkFilter...", praat_DEPRECATED_2014 | praat_DEPTH_1, NEW_Sound_to_MelFilter);
	praat_addAction1 (classSound, 0, U"To MelSpectrogram...", U"To BarkSpectrogram...", praat_DEPTH_1, NEW_Sound_to_MelSpectrogram);
	praat_addAction1 (classSound, 0, U"To ComplexSpectrogram...", U"To MelSpectrogram...", praat_DEPTH_1 + praat_HIDDEN, NEW_Sound_to_ComplexSpectrogram);

	praat_addAction1 (classSound, 0, U"To Polygon...", U"Down to Matrix", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_to_Polygon);
    praat_addAction1 (classSound, 2, U"To Polygon (enclosed)...", U"Cross-correlate...", praat_DEPTH_1 | praat_HIDDEN, NEW1_Sounds_to_Polygon_enclosed);
    praat_addAction1 (classSound, 2, U"To DTW...", U"Cross-correlate...", praat_DEPTH_1, NEW1_Sounds_to_DTW);

	praat_addAction1 (classSound, 1, U"Filter (gammatone)...", U"Filter (de-emphasis)...", 1, NEW_Sound_filterByGammaToneFilter4);
	praat_addAction1 (classSound, 0, U"Remove noise...", U"Filter (formula)...", 1, NEW_Sound_removeNoise);

	praat_addAction1 (classSound, 0, U"Change gender...", U"Deepen band modulation...", 1, NEW_Sound_changeGender);

	praat_addAction1 (classSound, 0, U"Change speaker...", U"Deepen band modulation...", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_changeSpeaker);
	praat_addAction1 (classSound, 0, U"Copy channel ranges...", U"Extract all channels", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_copyChannelRanges);
	praat_addAction1 (classSound, 0, U"Trim silences...", U"Resample...", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_trimSilences);
	praat_addAction1 (classSound, 0, U"To KlattGrid (simple)...", U"To Manipulation...", 1, NEW_Sound_to_KlattGrid_simple);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To FormantFilter...", nullptr, praat_DEPRECATED_2014, NEW1_Sound_and_Pitch_to_FormantFilter);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To Spectrogram (pitch-dependent)...", nullptr, 0,NEW1_Sound_and_Pitch_to_Spectrogram);

	praat_addAction2 (classSound, 1, classPitch, 1, U"Change gender...", nullptr, 0, NEW1_Sound_and_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, U"Change speaker...", nullptr, praat_HIDDEN, NEW1_Sound_and_Pitch_changeSpeaker);
	praat_addAction2 (classSound, 1, classIntervalTier, 1, U"Cut parts matching label...", nullptr, 0, NEW1_Sound_and_IntervalTier_cutPartsMatchingLabel);
	praat_addAction1 (classSpectrogram, 2, U"To DTW...", U"To Spectrum (slice)...", 1, NEW1_Spectrograms_to_DTW);
	praat_addAction1 (classSpectrum, 0, U"Draw phases...", U"Draw (log freq)...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Spectrum_drawPhases);
	praat_addAction1 (classSpectrum, 0, U"Set real value in bin...", U"Formula...", praat_HIDDEN | praat_DEPTH_1, MODIFY_Spectrum_setRealValueInBin);
	praat_addAction1 (classSpectrum, 0, U"Set imaginary value in bin...", U"Formula...", praat_HIDDEN | praat_DEPTH_1, MODIFY_Spectrum_setImaginaryValueInBin);
	praat_addAction1 (classSpectrum, 0, U"Conjugate", U"Formula...", praat_HIDDEN | praat_DEPTH_1, MODIFY_Spectrum_conjugate);
	praat_addAction1 (classSpectrum, 2, U"Multiply", U"To Sound (fft)", praat_HIDDEN, NEW_Spectra_multiply);
	praat_addAction1 (classSpectrum, 0, U"To Matrix (unwrap)", U"To Matrix", praat_HIDDEN, NEW_Spectrum_unwrap);
	praat_addAction1 (classSpectrum, 0, U"Shift frequencies...", U"To Matrix", praat_HIDDEN, NEW_Spectrum_shiftFrequencies);
	praat_addAction1 (classSpectrum, 0, U"Compress frequency domain...", U"Shift frequencies...", praat_HIDDEN, NEW_Spectrum_compressFrequencyDomain);
	praat_addAction1 (classSpectrum, 0, U"Resample...", U"Compress frequency domain...", praat_HIDDEN, NEW_Spectrum_resample);
	praat_addAction1 (classSpectrum, 0, U"To Cepstrum", U"To Spectrogram", 1, NEW_Spectrum_to_Cepstrum);
	praat_addAction1 (classSpectrum, 0, U"To PowerCepstrum", U"To Cepstrum", 1, NEW_Spectrum_to_PowerCepstrum);

	praat_addAction1 (classSpeechSynthesizer, 0, U"SpeechSynthesizer help", nullptr, 0, HELP_SpeechSynthesizer_help);
	praat_addAction1 (classSpeechSynthesizer, 0, U"Play text...", nullptr, 0, PLAY_SpeechSynthesizer_playText);
	praat_addAction1 (classSpeechSynthesizer, 0, U"To Sound...", nullptr, 0, NEWMANY_SpeechSynthesizer_to_Sound);
	praat_addAction1 (classSpeechSynthesizer, 0, QUERY_BUTTON, nullptr, 0, 0);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice name", nullptr, 1, INFO_SpeechSynthesizer_getVoiceName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice variant", nullptr, 1, INFO_SpeechSynthesizer_getVoiceVariant);
	praat_addAction1 (classSpeechSynthesizer, 0, MODIFY_BUTTON, nullptr, 0, 0);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set text input settings...", nullptr, 1, MODIFY_SpeechSynthesizer_setTextInputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set speech output settings...", nullptr, 1, MODIFY_SpeechSynthesizer_setSpeechOutputSettings);
	praat_addAction2 (classSpeechSynthesizer, 1, classTextGrid, 1, U"To Sound...", nullptr, 0, NEW1_SpeechSynthesizer_and_TextGrid_to_Sound);

	praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align)...", nullptr, 0, NEW1_SpeechSynthesizer_and_Sound_and_TextGrid_align);
    praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align,trim)...", nullptr, 0, NEW1_SpeechSynthesizer_and_Sound_and_TextGrid_align2);

	praat_addAction1 (classSSCP, 0, U"SSCP help", nullptr, 0, HELP_SSCP_help);
	praat_TableOfReal_init2 (classSSCP);
	praat_removeAction (classSSCP, nullptr, nullptr, U"Append");
	praat_addAction1 (classSSCP, 0, U"Draw sigma ellipse...", DRAW_BUTTON, 1, GRAPHICS_SSCP_drawSigmaEllipse);
	praat_addAction1 (classSSCP, 0, U"Draw confidence ellipse...", DRAW_BUTTON, 1, GRAPHICS_SSCP_drawConfidenceEllipse);
	praat_SSCP_query_init (classSSCP);
	praat_addAction1 (classSSCP, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, REAL_SSCP_testDiagonality_bartlett);
	praat_addAction1 (classSSCP, 1, U"Get total variance", U"Get diagonality (bartlett)...", 1, REAL_SSCP_getTotalVariance);
	praat_addAction1 (classSSCP, 1, U"Get sigma ellipse area...", U"Get total variance", 1, REAL_SSCP_getConcentrationEllipseArea);
	praat_addAction1 (classSSCP, 1, U"Get confidence ellipse area...", U"Get sigma ellipse area...", 1, REAL_SSCP_getConfidenceEllipseArea);
	praat_addAction1 (classSSCP, 1, U"Get fraction variation...", U"Get confidence ellipse area...", 1, REAL_SSCP_getFractionVariation);
	praat_SSCP_extract_init (classSSCP);
	praat_addAction1 (classSSCP, 0, U"To PCA", nullptr, 0, NEW_SSCP_to_PCA);
	praat_addAction1 (classSSCP, 0, U"To Correlation", nullptr, 0, NEW_SSCP_to_Correlation);
	praat_addAction1 (classSSCP, 0, U"To Covariance...", nullptr, 0, NEW_SSCP_to_Covariance);

	praat_addAction1 (classStrings, 0, U"To Categories", nullptr, 0, NEW_Strings_to_Categories);
	praat_addAction1 (classStrings, 0, U"Append", nullptr, 0, NEW1_Strings_append);
	praat_addAction1 (classStrings, 0, U"Change...", U"Replace all...", praat_HIDDEN, NEW_Strings_change);
	praat_addAction1 (classStrings, 0, U"Extract part...", U"Replace all...", 0, NEW_Strings_extractPart);
	praat_addAction1 (classStrings, 0, U"To Permutation...", U"To Distributions", 0, NEW_Strings_to_Permutation);
	praat_addAction1 (classStrings, 2, U"To EditDistanceTable", U"To Distributions", 0, NEW_Strings_to_EditDistanceTable);

	praat_addAction1 (classSVD, 0, U"To TableOfReal...", nullptr, 0, NEW_SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, U"Extract left singular vectors", nullptr, 0, NEW_SVD_extractLeftSingularVectors);
	praat_addAction1 (classSVD, 0, U"Extract right singular vectors", nullptr, 0, NEW_SVD_extractRightSingularVectors);
	praat_addAction1 (classSVD, 0, U"Extract singular values", nullptr, 0, NEW_SVD_extractSingularValues);
		praat_addAction1 (classTable, 0, U"Draw ellipses...", U"Draw ellipse (standard deviation)...", praat_DEPTH_1, GRAPHICS_Table_drawEllipses);
		praat_addAction1 (classTable, 0, U"Box plots...", U"Draw ellipses...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Table_boxPlots);
		praat_addAction1 (classTable, 0, U"Normal probability plot...", U"Box plots...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Table_normalProbabilityPlot);
		praat_addAction1 (classTable, 0, U"Quantile-quantile plot...", U"Normal probability plot...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Table_quantileQuantilePlot);
		praat_addAction1 (classTable, 0, U"Quantile-quantile plot (between levels)...", U"Quantile-quantile plot...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Table_quantileQuantilePlot_betweenLevels);
		praat_addAction1 (classTable, 0, U"Lag plot...", U"Quantile-quantile plot (between levels)...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Table_lagPlot);
		praat_addAction1 (classTable, 0, U"Horizontal error bars plot...", U"Scatter plot (mark)...", praat_DEPTH_1, GRAPHICS_Table_horizontalErrorBarsPlot);
		praat_addAction1 (classTable, 0, U"Vertical error bars plot...", U"Scatter plot (mark)...", praat_DEPTH_1, GRAPHICS_Table_verticalErrorBarsPlot);
		praat_addAction1 (classTable, 0, U"Distribution plot...", U"Quantile-quantile plot...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_Table_distributionPlot);
		praat_addAction1 (classTable, 1, U"Draw where",  U"Lag plot...", 1 , 0);
			praat_addAction1 (classTable, 0, U"Scatter plot where...", U"Draw where", 2, GRAPHICS_Table_scatterPlotWhere);
			praat_addAction1 (classTable, 0, U"Scatter plot where (mark)...", U"Scatter plot where...", 2, GRAPHICS_Table_scatterPlotMarkWhere);
			praat_addAction1 (classTable, 0, U"Horizontal error bars plot where...", U"Scatter plot where (mark)...", praat_DEPTH_2, GRAPHICS_Table_horizontalErrorBarsPlotWhere);
			praat_addAction1 (classTable, 0, U"Vertical error bars plot where...", U"Scatter plot where (mark)...", praat_DEPTH_2, GRAPHICS_Table_verticalErrorBarsPlotWhere);
			
			praat_addAction1 (classTable, 0, U"Distribution plot where...", U"Scatter plot where (mark)...", 2, GRAPHICS_Table_distributionPlotWhere);
			praat_addAction1 (classTable, 0, U"Draw ellipse where (standard deviation)...", U"Distribution plot where...", 2, GRAPHICS_Table_drawEllipseWhere);
			praat_addAction1 (classTable, 0, U"Box plots where...", U"Draw ellipse where (standard deviation)...", 2, GRAPHICS_Table_boxPlotsWhere);
			praat_addAction1 (classTable, 0, U"Normal probability plot where...", U"Box plots where...", 2, GRAPHICS_Table_normalProbabilityPlotWhere);
			praat_addAction1 (classTable, 0, U"Bar plot where...", U"Normal probability plot where...", 2, GRAPHICS_Table_barPlotWhere);
			praat_addAction1 (classTable, 0, U"Line graph where...", U"Bar plot where...", 2, GRAPHICS_Table_LineGraphWhere);
			praat_addAction1 (classTable, 0, U"Lag plot where...", U"Line graph where...", 2, GRAPHICS_Table_lagPlotWhere);
			praat_addAction1 (classTable, 0, U"Draw ellipses where...", U"Lag plot where...", 2, GRAPHICS_Table_drawEllipsesWhere);

	praat_addAction1 (classTable, 1, U"Get number of rows where...", U"Get number of rows", praat_DEPTH_1 | praat_HIDDEN,	INTEGER_Table_getNumberOfRowsWhere);
	praat_addAction1 (classTable, 1, U"Report one-way anova...", U"Report group difference (Wilcoxon rank sum)...", praat_DEPTH_1 | praat_HIDDEN,	INFO_Table_reportOneWayAnova);
	praat_addAction1 (classTable, 1, U"Report one-way Kruskal-Wallis...", U"Report one-way anova...", praat_DEPTH_1 | praat_HIDDEN, INFO_Table_reportOneWayKruskalWallis);
	praat_addAction1 (classTable, 1, U"Report two-way anova...", U"Report one-way Kruskal-Wallis...", praat_DEPTH_1 | praat_HIDDEN, INFO_Table_reportTwoWayAnova);
	praat_addAction1 (classTable, 0, U"Extract rows where...", U"Extract rows where column (text)...", praat_DEPTH_1, NEW_Table_extractRowsWhere);
	praat_addAction1 (classTable, 0, U"Extract rows where (mahalanobis)...", U"Extract rows where...", praat_DEPTH_1| praat_HIDDEN, NEW_Table_extractRowsMahalanobisWhere);
	praat_addAction1 (classTable, 0, U"-- Extract columns ----", U"Extract rows where (mahalanobis)...", praat_DEPTH_1| praat_HIDDEN, 0);
	praat_addAction1 (classTable, 0, U"Extract column ranges...", U"-- Extract columns ----", praat_DEPTH_1| praat_HIDDEN, NEW_Table_extractColumnRanges);

	praat_addAction1 (classTable, 0, U"To KlattTable", nullptr, praat_HIDDEN, NEW_Table_to_KlattTable);
	praat_addAction1 (classTable, 1, U"Get median absolute deviation...", U"Get standard deviation...", 1, REAL_Table_getMedianAbsoluteDeviation);
	praat_addAction1 (classTable, 0, U"To StringsIndex (column)...", nullptr, praat_HIDDEN, NEW_Table_to_StringsIndex_column);

	praat_addAction1 (classTableOfReal, 1, U"Report multivariate normality...", U"Get column stdev (label)...", praat_DEPTH_1 | praat_HIDDEN, INFO_TableOfReal_reportMultivariateNormality);
	praat_addAction1 (classTableOfReal, 0, U"Append columns", U"Append", 1, NEW1_TableOfReal_appendColumns);
	praat_addAction1 (classTableOfReal, 0, U"Multivariate statistics -", nullptr, 0, 0);
	praat_addAction1 (classTableOfReal, 0, U"To Discriminant", nullptr, 1, NEW_TableOfReal_to_Discriminant);
	praat_addAction1 (classTableOfReal, 0, U"To PCA", nullptr, 1, NEW_TableOfReal_to_PCA_byRows);
	praat_addAction1 (classTableOfReal, 0, U"To SSCP...", nullptr, 1, NEW_TableOfReal_to_SSCP);
	praat_addAction1 (classTableOfReal, 0, U"To Covariance", nullptr, 1, NEW_TableOfReal_to_Covariance);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation", nullptr, 1, NEW_TableOfReal_to_Correlation);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation (rank)", nullptr, 1, NEW_TableOfReal_to_Correlation_rank);
	praat_addAction1 (classTableOfReal, 0, U"To CCA...", nullptr, 1, NEW_TableOfReal_to_CCA);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (means by row labels)...", nullptr, 1, NEW_TableOfReal_meansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (medians by row labels)...", nullptr, 1, NEW_TableOfReal_mediansByRowLabels);

	praat_addAction1 (classTableOfReal, 0, U"-- configurations --", nullptr, 1, 0);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (pca)...", nullptr, 1, NEW_TableOfReal_to_Configuration_pca);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (lda)...", nullptr, 1, NEW_TableOfReal_to_Configuration_lda);
	praat_addAction1 (classTableOfReal, 2, U"-- between tables --", U"To Configuration (lda)...", 1, 0);
	praat_addAction1 (classTableOfReal, 2, U"To TableOfReal (cross-correlations)...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_TableOfReal_and_TableOfReal_crossCorrelations);

	praat_addAction1 (classTableOfReal, 1, U"To PatternList and Categories...", U"To Matrix", 1, NEWMANY_TableOfReal_to_PatternList_and_Categories);
	praat_addAction1 (classTableOfReal, 1, U"To Pattern and Categories...", U"*To PatternList and Categories...", praat_DEPTH_1 | praat_DEPRECATED_2015, NEWMANY_TableOfReal_to_PatternList_and_Categories);
	praat_addAction1 (classTableOfReal, 1, U"Split into Pattern and Categories...", U"*To PatternList and Categories...", praat_DEPTH_1 | praat_DEPRECATED_2015, NEWMANY_TableOfReal_to_PatternList_and_Categories);
	praat_addAction1 (classTableOfReal, 0, U"To Permutation (sort row labels)", U"To Matrix", 1, NEW_TableOfReal_to_Permutation_sortRowlabels);

	praat_addAction1 (classTableOfReal, 1, U"To SVD", nullptr, praat_HIDDEN, NEW_TableOfReal_to_SVD);
	praat_addAction1 (classTableOfReal, 2, U"To GSVD", nullptr, praat_HIDDEN, NEW1_TablesOfReal_to_GSVD);
	praat_addAction1 (classTableOfReal, 2, U"To Eigen (gsvd)", nullptr, praat_HIDDEN, NEW1_TablesOfReal_to_Eigen_gsvd);

	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (cholesky)...", nullptr, praat_HIDDEN, NEW_TableOfReal_choleskyDecomposition);

	
	praat_addAction1 (classTableOfReal, 0, U"Draw as scalable squares...", U"Draw as squares...", 1, GRAPHICS_TableOfReal_drawAsScalableSquares);
	praat_addAction1 (classTableOfReal, 0, U"-- scatter plots --", U"Draw top and bottom lines...", 1, 0);
	praat_addAction1 (classTableOfReal, 0, U"Draw scatter plot...", U"-- scatter plots --", 1, GRAPHICS_TableOfReal_drawScatterPlot);
	praat_addAction1 (classTableOfReal, 0, U"Draw scatter plot matrix...", U"Draw scatter plot...", 1, GRAPHICS_TableOfReal_drawScatterPlotMatrix);
	praat_addAction1 (classTableOfReal, 0, U"Draw box plots...", U"Draw scatter plot matrix...", 1, GRAPHICS_TableOfReal_drawBoxPlots);
	praat_addAction1 (classTableOfReal, 0, U"Draw biplot...", U"Draw box plots...", 1, GRAPHICS_TableOfReal_drawBiplot);
	praat_addAction1 (classTableOfReal, 0, U"Draw vectors...", U"Draw box plots...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_TableOfReal_drawVectors);
	praat_addAction1 (classTableOfReal, 1, U"Draw row as histogram...", U"Draw biplot...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_TableOfReal_drawRowAsHistogram);
	praat_addAction1 (classTableOfReal, 1, U"Draw rows as histogram...", U"Draw row as histogram...", praat_DEPTH_1 | praat_HIDDEN, GRAPHICS_TableOfReal_drawRowsAsHistogram);
	praat_addAction1 (classTableOfReal, 1, U"Draw column as distribution...", U"Draw rows as histogram...", praat_DEPTH_1, GRAPHICS_TableOfReal_drawColumnAsDistribution);

	praat_addAction2 (classStrings, 1, classPermutation, 1, U"Permute strings", nullptr, 0, NEW1_Strings_and_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, U"Permute rows", nullptr, 0, NEW1_TableOfReal_and_Permutation_permuteRows);

	praat_addAction1 (classTextGrid, 0, U"Extend time...", U"Scale times...", 2, MODIFY_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, U"Set tier name...", U"Remove tier...", 1, MODIFY_TextGrid_setTierName);
	praat_addAction1 (classTextGrid, 0, U"Replace interval text...", U"Set interval text...", 2, MODIFY_TextGrid_replaceIntervalTexts);
	praat_addAction1 (classTextGrid, 0, U"Replace point text...", U"Set point text...", 2, MODIFY_TextGrid_replacePointTexts);
	praat_addAction1 (classTextGrid, 2, U"To Table (text alignment)...", U"Extract part...", 0, NEW1_TextGrids_to_Table_textAlignmentment);
	
	praat_addAction1 (classTextGrid, 0, U"To DurationTier...", U"Concatenate", 0, NEW_TextGrid_to_DurationTier);
	praat_addAction2 (classTextGrid, 1, classDurationTier, 1, U"To TextGrid (scale times)", nullptr, 0, NEW_TextGrid_and_DurationTier_to_TextGrid);
	praat_addAction2 (classTextGrid, 2, classEditCostsTable, 1, U"To Table (text alignment)...", nullptr, 0, NEW1_TextGrids_and_EditCostsTable_to_Table_textAlignmentment);

	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)

	INCLUDE_LIBRARY (praat_uvafon_MDS_init)
	INCLUDE_LIBRARY (praat_KlattGrid_init)
	INCLUDE_LIBRARY (praat_HMM_init)
	INCLUDE_LIBRARY (praat_BSS_init)
}

/* End of file praat_David.cpp 9790*/
