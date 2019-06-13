/* praat_David_init.cpp
 *
 * Copyright (C) 1993-2019 David Weenink, 2015 Paul Boersma
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
 djmw 20031023 Added Spectra_multiply, Spectrum_conjugate and modified interface for CCA_TableOfReal_scores.
 djmw 20031030 Added TableOfReal_appendColumns.
 djmw 20031107 Added TablesOfReal_to_GSVD.
 djmw 20040303 Latest modification
 djmw 20040305 Added hints for PCA.
 djmw 20040323 Added hint for Discriminant.
 djmw 20040324 Added PCA_TableOfReal_getFractionVariance.
 djmw 20040331 Modified Eigen_drawEigenvalues interface.
 djmw 20040406 Extensive checks for creation of Sounds.
 djmw 20040414 Forms texts.
 djmw 20040523 Discriminant_TableOfReal_to_ClassificationTable: give new object a name.
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
 djmw 20051116 TableOfReal_drawScatterPlot horizontal and vertical axes indices should be positive numbers
 djmw SVD extract lef/right singular vectors
 djmw 20060111 TextGrid: Extend time moved from depth 1 to depth 2.
 djmw 20060308 Thing_recognizeClassesByName: StringsIndex, CCA
 djmw 20070206 Sound_changeGender: pitch range factor should be >= 0
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
 djmw 20091230 Covariance_TableOfReal_mahalanobis
 djmw 20100212 Standardize on Window length
 djmw 20100511 Categories_getNumberOfCategories
 djmw 20120813 Latest modification.
*/

#include "NUMcomplex.h"
#include "NUMmachar.h"

#include "ActivationList.h"
#include "AmplitudeTier.h"
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
#include "FileInMemoryManager.h"
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
#include "Matrix_and_NMF.h"
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

static const conststring32 QUERY_BUTTON   = U"Query -";
static const conststring32 DRAW_BUTTON    = U"Draw -";
static const conststring32 MODIFY_BUTTON  = U"Modify -";
static const conststring32 EXTRACT_BUTTON = U"Extract -";

void praat_TableOfReal_init2 (ClassInfo klas);
void praat_SSCP_as_TableOfReal_init (ClassInfo klas);

void praat_CC_init (ClassInfo klas);
void praat_BandFilterSpectrogram_query_init (ClassInfo klas);
void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas);

#define DTW_constraints_addCommonFields(matchStart,matchEnd,slopeConstraint) \
	LABEL (U"Boundary conditions") \
	BOOLEAN (matchStart, U"Match begin positions", false) \
	BOOLEAN (matchEnd, U"Match end positions", false) \
	RADIO (slopeConstraint, U"Slope constraint", 1) \
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
	LABEL (U"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }}")
	TEXTFIELD (formula, U"Formula:", U"self")
	OK
DO
	MODIFY_EACH (ActivationList)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_END
}

DIRECT (NEW_ActivationList_to_Matrix) {
	CONVERT_EACH (ActivationList)
		autoMatrix result = ActivationList_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_ActivationList_to_PatternList) {
	CONVERT_EACH (ActivationList)
		autoPatternList result = ActivationList_to_PatternList (me);
	CONVERT_EACH_END (my name.get())
}

FORM (REAL_AmplitudeTier_getValueAtTime, U"AmplitudeTier: Get value at time", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = RealTier_getValueAtTime (me, time);
	NUMBER_ONE_END (U" Hz")
}
	
FORM (REAL_AmplitudeTier_getValueAtIndex, U"AmplitudeTier: Get value at index", nullptr) {
	INTEGER (pointNumber, U"Point number", U"10")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = RealTier_getValueAtIndex (me, pointNumber);
	NUMBER_ONE_END (U" Hz")
}


/********************** BandFilterSpectrogram *******************************************/

FORM (GRAPHICS_BandFilterSpectrogram_drawFrequencyScale, U"", U"") {
	REAL (fromFrequency, U"left Horizontal frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Horizontal frequency range (Hz)", U"0.0")
	REAL (yFromFrequency, U"left Vertical frequency range (mel)", U"0.0")
	REAL (yToFrequency, U"right Vertical frequency range (mel)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
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
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (Bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Barks", garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkFilter_drawSekeyHansonFilterFunctions, U"BarkFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	RADIO (frequencyScale, U"Frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (amplitudeScale_dB, U"Amplitude scale in dB", true)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		BarkFilter_drawSekeyHansonFilterFunctions (me, GRAPHICS, frequencyScale, fromFilter, toFilter, fromFrequency, toFrequency, amplitudeScale_dB, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkSpectrogram_drawSekeyHansonAuditoryFilters, U"BarkSpectrogram: Draw Sekey-Hanson auditory filters", U"BarkSpectrogram: Draw Sekey-Hanson auditory filters...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	RADIO (frequencyScale, U"Frequency scale", 2)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (amplitudeScale_dB, U"Amplitude scale in dB", true)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkSpectrogram)
		bool xIsHertz = (frequencyScale == 1);
		BarkSpectrogram_drawSekeyHansonFilterFunctions (me, GRAPHICS, xIsHertz, fromFilter, toFilter, fromFrequency, toFrequency, amplitudeScale_dB, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkFilter_paint, U"FilterBank: Paint", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		FilterBank_paint ((FilterBank) me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_BarkFilter_to_BarkSpectrogram) {
	CONVERT_EACH (BarkFilter)
		autoBarkSpectrogram result = BarkFilter_to_BarkSpectrogram (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_MelFilter_to_MelSpectrogram) {
	CONVERT_EACH (MelFilter)
		autoMelSpectrogram result = MelFilter_to_MelSpectrogram (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_FormantFilter_to_Spectrogram) {
	CONVERT_EACH (FormantFilter);
		autoSpectrogram result = FormantFilter_to_Spectrogram (me);
	CONVERT_EACH_END (my name.get())
}

/********************** Categories  ****************************************/

FORM (MODIFY_Categories_appendCategory, U"Categories: Append 1 category", U"Categories: Append 1 category...") {
	SENTENCE (category, U"Category", U"")
	OK
DO
	MODIFY_EACH (Categories)
		OrderedOfString_append (me, category);
	MODIFY_EACH_END
}

DIRECT (WINDOW_Categories_edit) {
	if (theCurrentPraatApplication -> batch) {
		Melder_throw (U"Cannot edit a Categories from batch.");
	} else {
		LOOP {
			iam (Categories);
			autoCategoriesEditor editor = CategoriesEditor_create (my name.get(), me);
			praat_installEditor (editor.get(), IOBJECT);
			editor.releaseToUser();
		}
	}
END }

DIRECT (INTEGER_Categories_getNumberOfCategories) {
	INTEGER_ONE (Categories)
		integer result = my size;
	INTEGER_ONE_END (U" categories")
}

DIRECT (INTEGER_Categories_getNumberOfDifferences) {
	NUMBER_COUPLE (Categories)
		integer result = OrderedOfString_getNumberOfDifferences (me, you);
	NUMBER_COUPLE_END (U" differences")
}

DIRECT (REAL_Categories_getFractionDifferent) {
	NUMBER_COUPLE (Categories)
		double result = OrderedOfString_getFractionDifferent (me, you);
	NUMBER_COUPLE_END (U" (fraction different)")
}

DIRECT (NEW_Categories_selectUniqueItems) {
	CONVERT_EACH (Categories)
		autoCategories result = Categories_selectUniqueItems (me);
	CONVERT_EACH_END (my name.get(), U"_uniq")
}

DIRECT (NEW_Categories_to_Confusion) {
	CONVERT_COUPLE (Categories)
		autoConfusion result = Categories_to_Confusion (me, you);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW_Categories_to_Strings) {
	CONVERT_EACH (Categories)
		autoStrings result = Categories_to_Strings (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_Categories_join) {
	CONVERT_COUPLE (Categories)
		autoStringList result = OrderedOfString_joinItems (me, you);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW_Categories_permuteItems) {
	CONVERT_EACH (Collection)
		autoCollection result = Collection_permuteItems (me);
	CONVERT_EACH_END (my name.get(), U"_perm")
}

/***************** CC ****************************************/

FORM (INTEGER_CC_getNumberOfCoefficients, U"Get number of coefficients", nullptr) {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	INTEGER_ONE (CC)
		integer result = CC_getNumberOfCoefficients (me, frameNumber);
	INTEGER_ONE_END (U" (number of coefficients)")
}

FORM (REAL_CC_getValue, U"CC: Get value", U"CC: Get value...") {
	REAL (time, U"Time (s)", U"0.1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (CC)
		double result = CC_getValue (me, time, index);
	NUMBER_ONE_END (U" value")
}

FORM (REAL_CC_getValueInFrame, U"CC: Get value in frame", U"CC: Get value in frame...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (CC)
		double result = CC_getValueInFrame (me, frameNumber, index);
	NUMBER_ONE_END (U" value")
}

FORM (REAL_CC_getC0ValueInFrame, U"CC: Get c0 value in frame", U"CC: Get c0 value in frame...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	NUMBER_ONE (CC)
		double result = CC_getC0ValueInFrame (me, frameNumber);
	NUMBER_ONE_END (U" value")
}

FORM (GRAPHICS_CC_paint, U"CC: Paint", U"CC: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	INTEGER (fromCoefficient, U"From coefficient", U"0")
	INTEGER (toCoefficient, U"To coefficient", U"0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CC)
		CC_paint (me, GRAPHICS, fromTime, toTime, fromCoefficient, toCoefficient, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_CC_drawC0, U"CC: Draw c0", U"CC: Draw c0...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CC)
		CC_drawC0 (me, GRAPHICS, fromTime, toTime,fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW1_CCs_to_DTW, U"CC: To DTW", U"CC: To DTW...") {
	LABEL (U"Distance  between cepstral coefficients")
	REAL (cepstralWeight, U"Cepstral weight", U"1.0")
	REAL (logEnergyWeight, U"Log energy weight", U"0.0")
	REAL (regressionWeight, U"Regression weight", U"0.0")
	REAL (regressionLogEnergyWeight, U"Regression log energy weight", U"0.0")
	REAL (regressionWindowLength, U"Regression window length (s)", U"0.056")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (CC)
		autoDTW result = CCs_to_DTW (me, you, cepstralWeight, logEnergyWeight, regressionWeight, regressionLogEnergyWeight, regressionWindowLength);
		DTW_findPath (result.get(), matchStart, matchEnd, slopeConstraint);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get());
}

DIRECT (NEW_CC_to_Matrix) {
	CONVERT_EACH (CC)
		autoMatrix result = CC_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

/******************* class CCA ********************************/

DIRECT (HELP_CCA_help) {
	HELP (U"CCA")
}

FORM (GRAPHICS_CCA_drawEigenvector, U"CCA: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	INTEGER (eigenVectorNumber, U"Eigenvector number", U"1")
	LABEL (U"Multiply by eigenvalue?")
	BOOLEAN (useComponentLoadings, U"Component loadings", false)
	LABEL (U"Select part of the eigenvector:")
	INTEGER (fromElement, U"left Element range", U"0")
	INTEGER (toElement, U"right Element range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"-1.0")
	REAL (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (connectPoints, U"Connect points", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CCA)
		CCA_drawEigenvector (me, GRAPHICS, xOrY, eigenVectorNumber, fromElement, toElement, fromAmplitude, toAmplitude, useComponentLoadings, markSize_mm, mark_string, connectPoints, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_CCA_getNumberOfCorrelations) {
	INTEGER_ONE (CCA)
		integer result = my numberOfCoefficients;
	INTEGER_ONE_END (U"")
}

FORM (REAL_CCA_getCorrelation, U"CCA: Get canonical correlation coefficient", U"CCA: Get canonical correlation coefficient") {
	NATURAL (coefficientNuber, U"Coefficient number", U"1")
	OK
DO
	NUMBER_ONE (CCA)
		double result = CCA_getCorrelationCoefficient (me, coefficientNuber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_CCA_getEigenvectorElement, U"CCA: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURAL (elementNumber, U"Element number", U"1")
	OK
DO
	NUMBER_ONE (CCA)
		double result = CCA_getEigenvectorElement (me, xOrY, eigenvectorNumber, elementNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_CCA_getZeroCorrelationProbability, U"CCA: Get zero correlation probability", U"CCA: Get zero correlation probability...") {
	NATURAL (coefficientNumber, U"Coefficient number", U"1")
	OK
DO
	NUMBER_ONE (CCA)
		double result, chisq, df;
		CCA_getZeroCorrelationProbability (me, coefficientNumber, & result, & chisq, & df);
	NUMBER_ONE_END (U" (= probability for chisq = ", chisq, U" and ndf = ", df, U")");
}

DIRECT (NEW1_CCA_Correlation_to_TableOfReal_loadings) {
	CONVERT_TWO (CCA, Correlation)
		autoTableOfReal result = CCA_Correlation_factorLoadings (me, you);
	CONVERT_TWO_END (my name.get(), U"_loadings")
}

FORM (REAL_CCA_Correlation_getVarianceFraction, U"CCA & Correlation: Get variance fraction", U"CCA & Correlation: Get variance fraction...") {
	LABEL (U"Get the fraction of variance from the data in set...")
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	LABEL (U"extracted by...")
	NATURAL (fromCanonicalVariate, U"left Canonical variate range", U"1")
	NATURAL (toCanonicalVariate, U"right Canonical variate range", U"1")
	OK
DO
	NUMBER_TWO (CCA, Correlation)
		double result = CCA_Correlation_getVarianceFraction (me, you, xOrY, fromCanonicalVariate, toCanonicalVariate);
	NUMBER_TWO_END (U" (fraction variance from ", (xOrY == 1 ? U"y" : U"x"), U", extracted by canonical variates ", fromCanonicalVariate, U" to ", toCanonicalVariate, U")")
}

FORM (REAL_CCA_Correlation_getRedundancy_sl, U"CCA & Correlation: Get Stewart-Love redundancy", U"CCA & Correlation: Get redundancy (sl)...") {
	LABEL (U"Get the redundancy of the data in set...")
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	LABEL (U"extracted by...")
	NATURAL (fromCanonicalVariate, U"left Canonical variate range", U"1")
	NATURAL (toCanonicalVariate, U"right Canonical variate range", U"1")
	LABEL (U"...given the availability of the data in the other set.")
	OK
DO
	NUMBER_TWO (CCA, Correlation)
		double result = CCA_Correlation_getRedundancy_sl (me, you, xOrY, fromCanonicalVariate, toCanonicalVariate);
	NUMBER_TWO_END (U" (redundancy from ", (xOrY == 1 ? U"y" : U"x"), U" extracted by canonical variates ", fromCanonicalVariate, U" to ", toCanonicalVariate, U")")
}

DIRECT (NEW_CCA_TableOfReal_to_TableOfReal_loadings) {
	CONVERT_TWO (CCA, TableOfReal)
		autoTableOfReal result = CCA_TableOfReal_factorLoadings (me, you);
	CONVERT_TWO_END (my name.get(), U"_loadings")
}

FORM (NEW_CCA_TableOfReal_to_TableOfReal_scores, U"CCA & TableOfReal: To TableOfReal (scores)", U"CCA & TableOfReal: To TableOfReal (scores)...") {
	INTEGER (numberOfCanonicalVariates, U"Number of canonical correlations", U"0 (= all)")
	OK
DO
	CONVERT_TWO (CCA, TableOfReal)
		autoTableOfReal result = CCA_TableOfReal_scores (me, you, numberOfCanonicalVariates);
	CONVERT_TWO_END (my name.get(), U"_scores");
}

FORM (NEW1_CCA_TableOfReal_predict, U"CCA & TableOfReal: Predict", U"CCA & TableOfReal: Predict...") {
	LABEL (U"The data set from which to predict starts at...")
	INTEGER (columnNumber, U"Column number", U"1")
	OK
DO
	CONVERT_TWO (CCA, TableOfReal)
		autoTableOfReal result = CCA_TableOfReal_predict (me, you, columnNumber);
	CONVERT_TWO_END (your name.get(), U"_", my name.get())
}

FORM (NEW_CCA_extractEigen, U"CCA: Exxtract Eigen", nullptr) {
	OPTIONMENU (choice, U"variablesType", 1)
		OPTION (U"Dependent")
		OPTION (U"Independent")
	OK
DO
	CONVERT_EACH (CCA)
		autoEigen result = choice == 1 ? Data_copy (my y.get()) : Data_copy (my x.get());
	CONVERT_EACH_END (my name.get(), ( choice == 1 ? U"_y" : U"_x" ))
}

/***************** ChebyshevSeries ****************************************/

DIRECT (HELP_ChebyshevSeries_help) {
	HELP (U"ChebyshevSeries")
}

FORM (NEW1_ChebyshevSeries_create, U"Create ChebyshevSeries", U"Create ChebyshevSeries...") {
	WORD (name, U"Name", U"cs")
	LABEL (U"Domain")
	REAL (xmin, U"Xmin", U"-1")
	REAL (xmax, U"Xmax", U"1")
	LABEL (U"ChebyshevSeries(x) = c[1] T[0](x) + c[2] T[1](x) + ... c[n+1] T[n](x)")
	LABEL (U"T[k] is a Chebyshev polynomial of degree k")
	SENTENCE (coefficients_string, U"Coefficients (c[k])", U"0 0 1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be smaller than Xmax.");
	CREATE_ONE
		autoChebyshevSeries result = ChebyshevSeries_createFromString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

DIRECT (NEW_ChebyshevSeries_to_Polynomial) {
	CONVERT_EACH (ChebyshevSeries)
		autoPolynomial result = ChebyshevSeries_to_Polynomial (me);
	CONVERT_EACH_END (my name.get());
}

/***************** ClassificationTable ****************************************/

DIRECT (HELP_ClassificationTable_help) {
	HELP (U"ClassificationTable")
}

FORM (INTEGER_ClassificationTable_getClassIndexAtMaximumInRow, U"ClassificationTable: Get class index at maximum in row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	INTEGER_ONE (ClassificationTable)
		integer result = TableOfReal_getColumnIndexAtMaximumInRow (me, rowNumber);
	INTEGER_ONE_END (U" (class index at maximum in row)")
}

FORM (INTEGER_ClassificationTable_getClassLabelAtMaximumInRow, U"ClassificationTable: Get class label at maximum in row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	STRING_ONE (ClassificationTable)
		conststring32 result = TableOfReal_getColumnLabelAtMaximumInRow (me, rowNumber);
	STRING_ONE_END
}

// deprecated 2014
DIRECT (NEW_ClassificationTable_to_Confusion_old) {
	CONVERT_EACH (ClassificationTable)
		autoConfusion result = ClassificationTable_to_Confusion (me, false);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_ClassificationTable_to_Confusion, U"ClassificationTable: To Confusion", U"ClassificationTable: To Confusion...") {
	BOOLEAN (onlyClassLabels, U"Only class labels", true)
	OK
DO
	CONVERT_EACH (ClassificationTable)
		autoConfusion result = ClassificationTable_to_Confusion (me, onlyClassLabels);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_ClassificationTable_to_Correlation_columns) {
	CONVERT_EACH (ClassificationTable)
		autoCorrelation result = ClassificationTable_to_Correlation_columns (me);
	CONVERT_EACH_END (my name.get(), U"_col")
}

DIRECT (NEW_ClassificationTable_to_Strings_maximumProbability) {
	CONVERT_EACH (ClassificationTable)
		autoStrings result = ClassificationTable_to_Strings_maximumProbability (me);
	CONVERT_EACH_END (my name.get())
}

/********************** Confusion *******************************************/

DIRECT (HELP_Confusion_help) {
	HELP (U"Confusion")
}

FORM (NEW1_Confusion_createSimple, U"Create simple Confusion", U"Create simple Confusion...") {
	WORD (name, U"Name", U"simple")
	SENTENCE (labels, U"Labels", U"u i a")
	OK
DO
	CREATE_ONE
		autoConfusion result = Confusion_createSimple (labels);
	CREATE_ONE_END (name)
}

FORM (MODIFY_Confusion_increase, U"Confusion: Increase", U"Confusion: Increase...") {
	WORD (stimulus, U"Stimulus", U"u")
	WORD (response, U"Response", U"i")
	OK
DO
	MODIFY_EACH (Confusion)
		Confusion_increase (me, stimulus, response);
	MODIFY_EACH_END
}

FORM (REAL_Confusion_getValue_labels, U"Confusion: Get value", nullptr) {
	WORD (stimulus, U"Stimulus", U"u")
	WORD (response, U"Response", U"i")
	OK
DO
	NUMBER_ONE (Confusion)
		double result = Confusion_getValue (me, stimulus, response);
	NUMBER_ONE_END (U" ([\"", stimulus, U"\", \"",  response, U"\"])")
}

FORM (REAL_Confusion_getResponseSum, U"Confusion: Get response sum", U"Confusion: Get response sum...") {
	WORD (response, U"Response", U"u")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getColumnSumByLabel (me, response);
	NUMBER_ONE_END (U" (response sum)")
}

FORM (REAL_Confusion_getStimulusSum, U"Confusion: Get stimulus sum", U"Confusion: Get stimulus sum...") {
	WORD (stimulus, U"Stimulus", U"u")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getRowSumByLabel (me, stimulus);
	NUMBER_ONE_END (U" (stimulus sum)")
}

DIRECT (NEW_Confusion_to_TableOfReal_marginals) {
	CONVERT_EACH (Confusion)
		autoTableOfReal result = Confusion_to_TableOfReal_marginals (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_Confusion_difference) {
	CONVERT_COUPLE (Confusion)
		autoMatrix result = Confusion_difference (me, you);
	CONVERT_COUPLE_END (U"diffs")
}

FORM (NEW_Confusion_condense, U"Confusion: Condense", U"Confusion: Condense...") {
	SENTENCE (search_string, U"Search", U"^(u|i)$")
	SENTENCE (replace_string, U"Replace", U"high")
	INTEGER (replaceLimit, U"Replace limit", U"0 (= unlimited)")
	RADIOx (matchType, U"Search and replace are", 2, 0)
		RADIOBUTTON (U"Literals")
		RADIOBUTTON (U"Regular Expressions")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_condense (me, search_string, replace_string, replaceLimit, matchType == 2);
	CONVERT_EACH_END (my name.get(), U"_cnd")
}

FORM (NEW_Confusion_group, U"Confusion: Group stimuli & responses", U"Confusion: Group...") {
	SENTENCE (labels, U"Stimuli & Responses", U"u i")
	SENTENCE (newLabel, U"New label", U"high")
	INTEGER (newPosition, U"New label position", U"0 (= at start)")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_group (me, labels, newLabel, newPosition);
	CONVERT_EACH_END (my name.get(), U"_sr", newLabel)
}

FORM (NEW_Confusion_groupStimuli, U"Confusion: Group stimuli", U"Confusion: Group stimuli...") {
	SENTENCE (stimuli_string, U"Stimuli", U"u i")
	SENTENCE (newLabel, U"New label", U"high")
	INTEGER (newPosition, U"New label position", U"0")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_groupStimuli (me,stimuli_string, newLabel, newPosition);
	CONVERT_EACH_END (my name.get(), U"_s", newLabel);
}

FORM (NEW_Confusion_groupResponses, U"Confusion: Group responses", U"Confusion: Group responses...") {
	SENTENCE (responses_string, U"Responses", U"a i")
	SENTENCE (newLabel, U"New label", U"front")
	INTEGER (newPosition, U"New label position", U"0")
	OK
DO
	CONVERT_EACH (Confusion)
		autoConfusion result = Confusion_groupResponses (me, responses_string, newLabel, newPosition);
	CONVERT_EACH_END (my name.get(), U"_s", newLabel);
}

FORM (GRAPHICS_Confusion_drawAsNumbers, U"Confusion: Draw as numbers", nullptr) {
	BOOLEAN (drawMarginals, U"Draw marginals", true)
	RADIO (format, U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (precision, U"Precision", U"5")
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

DIRECT (MODIFY_Confusion_ClassificationTable_increaseConfusionCount) {
	MODIFY_FIRST_OF_TWO (Confusion, ClassificationTable)
		Confusion_ClassificationTable_increase (me, you);
	MODIFY_FIRST_OF_TWO_END
}

/******************* Confusion & Matrix *************************************/

FORM (GRAPHICS_Confusion_Matrix_draw, U"Confusion & Matrix: Draw confusions with arrows", nullptr) {
	INTEGER (categoryPosition, U"Category position", U"0 (= all)")
	REAL (lowerLevel, U"Lower level (%)", U"0")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	Melder_require (categoryPosition >= 0, U"Your category position should be at least 0.");
	GRAPHICS_TWO (Confusion, Matrix)
		Confusion_Matrix_draw (me, you, GRAPHICS, categoryPosition, lowerLevel, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_TWO_END
}

/********************** ComplexSpectrogram *******************************************/

DIRECT (HELP_ComplexSpectrogram_help) {
	HELP (U"ComplexSpectrogram_help")
}

FORM (NEW_ComplexSpectrogram_to_Sound, U"ComplexSpectrogram: To Sound", nullptr) {
	POSITIVE (durationFactor, U"Duration factor", U"1.0")
	OK
DO
	CONVERT_EACH (ComplexSpectrogram)
		autoSound result = ComplexSpectrogram_to_Sound (me, durationFactor);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_ComplexSpectrogram_downto_Spectrogram) {
	CONVERT_EACH (ComplexSpectrogram)
		autoSpectrogram result = ComplexSpectrogram_to_Spectrogram (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_ComplexSpectrogram_to_Spectrum_slice, U"ComplexSpectrogram: To Spectrum (slice)", nullptr) {
	REAL (time, U"Time (s)", U"0.0")
	OK
DO
	CONVERT_EACH (ComplexSpectrogram)
		autoSpectrum result = ComplexSpectrogram_to_Spectrum (me, time);
	CONVERT_EACH_END (my name.get())
}

DIRECT (MODIFY_ComplexSpectrogram_Spectrogram_replaceAmplitudes) {
	MODIFY_FIRST_OF_TWO (ComplexSpectrogram, Spectrogram)
		ComplexSpectrogram_Spectrogram_replaceAmplitudes (me, you);
	MODIFY_FIRST_OF_TWO_END
}

/********************** Correlation *******************************************/

FORM (NEW1_Correlation_createSimple, U"Create simple Correlation", U"Create simple Correlation...") {
	WORD (name, U"Name", U"correlation")
	SENTENCE (correlations_string, U"Correlations", U"1.0 0.5 1.0")
	SENTENCE (centroid_string, U"Centroid", U"0.0 0.0")
	NATURAL (numberOfObservations, U"Number of observations", U"100")
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
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	INTEGER (numberOfTests, U"Number of tests (Bonferroni correction)", U"0")
	RADIO (approximation, U"Approximation", 1)
		RADIOBUTTON (U"Ruben")
		RADIOBUTTON (U"Fisher")
	OK
DO
	CONVERT_EACH (Correlation)
		autoTableOfReal result = Correlation_confidenceIntervals (me, confidenceLevel, numberOfTests, approximation);
	CONVERT_EACH_END (my name.get(), U"_conf_intervals")
}

FORM (REAL_Correlation_getDiagonality_bartlett, U"Correlation: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...") {
	NATURAL (numberOfConstraints, U"Number of constraints", U"1")
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
	CONVERT_EACH_END (my name.get())
}

/********************** Covariance *******************************************/

DIRECT (HELP_Covariance_help) {
	HELP (U"Covariance")
}

FORM (NEW1_Covariance_createSimple, U"Create simple Covariance", U"Create simple Covariance...") {
	WORD (name, U"Name", U"c")
	SENTENCE (covariances_string, U"Covariances", U"1.0 0.0 1.0")
	SENTENCE (centroid_string, U"Centroid", U"0.0 0.0")
	NATURAL (numberOfObservations, U"Number of observations", U"100")
	OK
DO
	CREATE_ONE
		autoCovariance result = Covariance_createSimple (covariances_string, centroid_string, numberOfObservations);
	CREATE_ONE_END (name)
}

FORM (REAL_Covariance_getProbabilityAtPosition, U"Covariance: Get probability at position", nullptr) {
	SENTENCE (position_string, U"Position", U"10.0 20.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result = Covariance_getProbabilityAtPosition_string (me, position_string);
	NUMBER_ONE_END (U" (= probability at position ", position_string, U")")
}

FORM (REAL_Covariance_getSignificanceOfOneMean, U"Covariance: Get significance of one mean", U"Covariance: Get significance of one mean...") {
	LABEL (U"Get probability that the estimated mean for")
	NATURAL (index, U"Index", U"1")
	LABEL (U"(or an estimated mean even further away)")
	LABEL (U"could arise if the true mean were")
	REAL (value, U"Value", U"0.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result, t, ndf;
		Covariance_getSignificanceOfOneMean (me, index, value, & result, & t, & ndf);
	NUMBER_ONE_END (U" (= probability, based on t = ", t, U" and ndf = ", ndf)
}

FORM (REAL_Covariance_getSignificanceOfMeansDifference, U"Covariance: Get significance of means difference", U"Covariance: Get significance of means difference...") {
	LABEL (U"Get probability that the estimated difference between the means for")
	NATURAL (index1, U"Index1", U"1")
	NATURAL (index2, U"Index2", U"2")
	LABEL (U"could arise if the true mean were")
	REAL (value, U"Value", U"0.0")
	LABEL (U"Assume the means are ")
	BOOLEAN (paired, U"Paired", true)
	LABEL (U"and have")
	BOOLEAN (equalVariances, U"Equal variances", true)
	OK
DO
	NUMBER_ONE (Covariance)
		double result, t, ndf;
		Covariance_getSignificanceOfMeansDifference (me, index1, index2, value, paired, equalVariances, & result, & t, & ndf);
	NUMBER_ONE_END (U" (= probability, based on t = ", t, U"and ndf = ", ndf, U")")
}

FORM (REAL_Covariance_getSignificanceOfOneVariance, U"Covariance: Get significance of one variance", U"Covariance: Get significance of one variance...") {
	LABEL (U"Get the probability that the estimated variance for")
	NATURAL (index, U"Index", U"1")
	LABEL (U"(or an even larger estimated variance)")
	LABEL (U"could arise if the true variance were")
	REAL (value, U"Value", U"0.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result, chisq, ndf;
		Covariance_getSignificanceOfOneVariance (me, index, value, & result, & chisq, & ndf);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", ndf, U")")
}

FORM (REAL_Covariance_getSignificanceOfVariancesRatio, U"Covariance: Get significance of variances ratio", nullptr) {
	LABEL (U"Get the probability that the estimated variance ratio observed for")
	NATURAL (index1, U"Index1", U"1")
	NATURAL (index2, U"Index2", U"2")
	LABEL (U"(or an estimated ratio even further away)")
	LABEL (U"could arise if the true ratio were")
	REAL (value, U"Value", U"1.0")
	OK
DO
	NUMBER_ONE (Covariance)
		double result, f, df;
		Covariance_getSignificanceOfVariancesRatio (me, index1, index2, value, & result, & f , & df);
	NUMBER_ONE_END (U" (= probability, based on F = ", f, U" and ndf1 = ", df, U" and ndf2 = ", df, U")")
}

FORM (REAL_Covariance_getFractionVariance, U"Covariance: Get fraction variance", U"Covariance: Get fraction variance...") {
	NATURAL (fromDimension, U"From dimension", U"1")
	NATURAL (toDimension, U"To dimension", U"1")
	OK
DO
	NUMBER_ONE (Covariance)
		double result = SSCP_getFractionVariation (me, fromDimension, toDimension);
	NUMBER_ONE_END (U"")
}

FORM (INFO_Covariances_reportMultivariateMeanDifference, U"Covariances: Report multivariate mean difference", U"Covariances: Report multivariate mean difference...") {
	LABEL (U"Get probability that the estimated multivariate means difference could arise ")
	LABEL (U"if the actual means were equal.")
	LABEL (U"")
	LABEL (U"Assume for both means we have")
	BOOLEAN (covariancesAreEqual, U"Equal covariances", true)
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
	INTEGER (numberOfDataPoints, U"Number of data points", U"0")
	OK
DO
	CONVERT_EACH (Covariance)
		autoTableOfReal result = Covariance_to_TableOfReal_randomSampling (me, numberOfDataPoints);
	CONVERT_EACH_END (my name.get())
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
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Covariance_to_PCA) {
	CONVERT_EACH (Covariance)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_Covariances_pool) {
	CONVERT_TYPED_LIST (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_pool (list.get());
	CONVERT_TYPED_LIST_END (U"pool")
}

DIRECT (NEW1_Covariances_to_Covariance_between) {
	CONVERT_TYPED_LIST (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_between (list.get());
	CONVERT_TYPED_LIST_END (U"between")	
}

DIRECT (NEW1_Covariances_to_Covariance_within) {
	CONVERT_TYPED_LIST (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_within (list.get());
	CONVERT_TYPED_LIST_END (U"within")
}

DIRECT (NEW1_Covariances_to_CovarianceList) {
	autoCovarianceList result = CovarianceList_create ();
	CREATE_ONE
	LOOP {
		iam (Covariance);
		autoCovariance cov = Data_copy (me);
		result -> addItem_move (cov.move());
	}
	CREATE_ONE_END (U"List_of_", Melder_integer (result -> size))
}

FORM (NEW1_Covariance_TableOfReal_mahalanobis, U"Covariance & TableOfReal: To TableOfReal (mahalanobis)", U"Covariance & TableOfReal: To TableOfReal (mahalanobis)...") {
	BOOLEAN (centroidFromTable, U"Centroid from table", false)
	OK
DO
	CONVERT_TWO (Covariance, TableOfReal)
		autoTableOfReal result = Covariance_TableOfReal_mahalanobis (me, you, centroidFromTable);
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

FORM (NEW1_Discriminant_PatternList_to_Categories, U"Discriminant & PatternList: To Categories", U"Discriminant & PatternList: To Categories...") {
	BOOLEAN (poolCovariances, U"Pool covariance matrices", true)
	BOOLEAN (useAPrioriProbabilities, U"Use apriori probabilities", true)
	OK
DO
	CONVERT_TWO (Discriminant, PatternList)
		autoCategories result = Discriminant_PatternList_to_Categories (me, you, poolCovariances, useAPrioriProbabilities);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Discriminant_TableOfReal_to_Configuration, U"Discriminant & TableOfReal: To Configuration", U"Discriminant & TableOfReal: To Configuration...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (=all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"\"Number of dimensions\" should not be less than zero.");
	CONVERT_TWO (Discriminant, TableOfReal)
		autoConfiguration result = Discriminant_TableOfReal_to_Configuration (me, you, numberOfDimensions);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (hint_Discriminant_TableOfReal_to_ClassificationTable) {
	Melder_information (U"You can use the Discriminant as a classifier by \nselecting a Discriminant and a TableOfReal object together.");
END }

FORM (NEW1_Discriminant_TableOfReal_to_ClassificationTable, U"Discriminant & TableOfReal: To ClassificationTable", U"Discriminant & TableOfReal: To ClassificationTable...") {
	BOOLEAN (poolCovariances, U"Pool covariance matrices", true)
	BOOLEAN (useAPrioriProbabilities, U"Use apriori probabilities", true)
	OK
DO
	CONVERT_TWO (Discriminant, TableOfReal)
		autoClassificationTable result = Discriminant_TableOfReal_to_ClassificationTable (me, you, poolCovariances, useAPrioriProbabilities);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Discriminant_TableOfReal_mahalanobis, U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)", U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...") {
	SENTENCE (groupLabel, U"Group label", U"")
	BOOLEAN (poolCovariances, U"Pool covariance matrices", false)
	OK
DO
	CONVERT_TWO (Discriminant, TableOfReal)
		integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, U"Your group label \"", groupLabel, U"\" does not exist.");
		autoTableOfReal result = Discriminant_TableOfReal_mahalanobis (me, you, group, poolCovariances);
	CONVERT_TWO_END (U"mahalanobis")
}

DIRECT (INTEGER_Discriminant_getNumberOfEigenvalues) {
	INTEGER_ONE (Discriminant)
		integer result = my eigen -> numberOfEigenvalues;
	INTEGER_ONE_END (U" (number of eigenvalues)")
}

DIRECT (INTEGER_Discriminant_getNumberOfEigenvectors) {
	INTEGER_ONE (Discriminant)
		integer result = my eigen -> numberOfEigenvalues;
	INTEGER_ONE_END (U" (number of eigenvectors)")
}

DIRECT (INTEGER_Discriminant_getEigenvectorDimension) {
	INTEGER_ONE (Discriminant)
		integer result = my eigen -> dimension;
	INTEGER_ONE_END (U" (dimension)")
}

FORM (REAL_Discriminant_getEigenvalue, U"Discriminant: Get eigenvalue", U"Eigen: Get eigenvalue...") {
	NATURAL (eigenvalueNumber, U"Eigenvalue number", U"1")
	OK
DO
	NUMBER_ONE (Discriminant)
		Melder_require (eigenvalueNumber <= my eigen -> numberOfEigenvalues, 
			U"Eigenvalue number should be smaller than ", my eigen -> numberOfEigenvalues + 1);
		double result = my eigen -> eigenvalues[eigenvalueNumber];
	NUMBER_ONE_END (U" (eigenvalue [)", eigenvalueNumber, U"])")
}

FORM (REAL_Discriminant_getSumOfEigenvalues, U"Discriminant:Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range",  U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Eigen_getSumOfEigenvalues (my eigen.get(), fromEigenvalue, toEigenvalue);
	NUMBER_ONE_END (U"")
}

FORM (REAL_Discriminant_getEigenvectorElement, U"Discriminant: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURAL (elementNumber, U"Element number", U"1")
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Eigen_getEigenvectorElement (my eigen.get(), eigenvectorNumber, elementNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_Discriminant_getWilksLambda, U"Discriminant: Get Wilks' lambda", U"Discriminant: Get Wilks' lambda...") {
	LABEL (U"Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	NATURAL (from, U"From", U"1") //TODO better name
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Discriminant_getWilksLambda (me, from);
	NUMBER_ONE_END (U" (wilks lambda)")
}

FORM (REAL_Discriminant_getCumulativeContributionOfComponents, U"Discriminant: Get cumulative contribution of components", U"Eigen: Get cumulative contribution of components...") {
	NATURAL (fromComponent, U"From component", U"1")
	NATURAL (toComponent, U"To component", U"1")
	OK
DO
	NUMBER_ONE (Discriminant)
		double result = Eigen_getCumulativeContributionOfComponents (my eigen.get(), fromComponent, toComponent);
	NUMBER_ONE_END (U" (cumulative contribution)")
}


FORM (REAL_Discriminant_getPartialDiscriminationProbability, U"Discriminant: Get partial discrimination probability", U"Discriminant: Get partial discrimination probability...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"1")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"The number of dimensions should be at least zero.");
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

DIRECT (INFO_Discriminant_reportEqualityOfCovarianceMatrices) {
	MelderInfo_open ();
	LOOP {
		iam (Discriminant);
		structCovarianceList list;
		for (integer i = 1; i <= my groups->size; i ++) {
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

FORM (REAL_Discriminant_getSigmaEllipseArea, U"Discriminant: Get concentration ellipse area", U"Discriminant: Get concentration ellipse area...") {
	SENTENCE (groupLabel, U"Group label", U"")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	OK
DO
	NUMBER_ONE (Discriminant)
		integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, U"The group label \"", groupLabel, U"\" does not exist.");
		double result = Discriminant_getConcentrationEllipseArea (me, group, numberOfSigmas, false, discriminatPlane, xDimension, yDimension);
	NUMBER_ONE_END (U" (concentration ellipse area)")
}

FORM (REAL_Discriminant_getConfidenceEllipseArea, U"Discriminant: Get confidence ellipse area", U"Discriminant: Get confidence ellipse area...") {
	SENTENCE (groupLabel, U"Group label", U"")
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	OK
DO
	NUMBER_ONE (Discriminant)
		integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, U"The group label \"", groupLabel, U"\" does not exist.");
		double result = Discriminant_getConcentrationEllipseArea (me, group, confidenceLevel, true, discriminatPlane, xDimension, yDimension);
	NUMBER_ONE_END (U" (confidence ellipse area)")
}

FORM (REAL_Discriminant_getLnDeterminant_group, U"Discriminant: Get determinant (group)", U"Discriminant: Get determinant (group)...")
	SENTENCE (groupLabel, U"Group label", U"") {
	OK
DO
	NUMBER_ONE (Discriminant)
		integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, U"The group label \"", groupLabel, U"\" does not exist.");
		double result = Discriminant_getLnDeterminant_group (me, group);
	NUMBER_ONE_END (U" (ln(determinant) group")
}

DIRECT (REAL_Discriminant_getLnDeterminant_total) {
	NUMBER_ONE (Discriminant)
		double result = Discriminant_getLnDeterminant_total (me);
	NUMBER_ONE_END (U" (ln(determinant) total")
}

FORM (MODIFY_Discriminant_invertEigenvector, U"Discriminant: Invert eigenvector", nullptr) {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Discriminant)
		Eigen_invertEigenvector (my eigen.get(), eigenvectorNumber);
	MODIFY_EACH_END
}

FORM (GRAPHICS_Discriminant_drawEigenvalues, U"Discriminant: Draw eigenvalues", U"Eigen: Draw eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (showFractions, U"Fraction of eigenvalues summed", false)
	BOOLEAN (showCumulativeValues, U"Cumulative", false)
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Eigen_drawEigenvalues (my eigen.get(), GRAPHICS, fromEigenvalue, toEigenvalue, fromAmplitude, toAmplitude, showFractions, showCumulativeValues, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawEigenvector, U"Discriminant: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	INTEGER (eigenvectorNumber, U"Eigenvector number", U"1")
	BOOLEAN (componentLoadings, U"Component loadings", false)
	INTEGER (fromElement, U"left Element range", U"0")
	INTEGER (toElement, U"right Element range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"-1.0")
	REAL (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (connectPoints, U"Connect points", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Eigen_drawEigenvector (my eigen.get(), GRAPHICS, eigenvectorNumber, fromElement, toElement, fromAmplitude, toAmplitude, componentLoadings, markSize_mm, mark_string, connectPoints, nullptr,  garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_Discriminant_drawSigmaEllipses, U"Discriminant: Draw sigma ellipses", U"Discriminant: Draw sigma ellipses...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (discriminantPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false, nullptr, discriminantPlane, xDimension, yDimension,xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawOneSigmaEllipse, U"Discriminant: Draw one sigma ellipse", U"Discriminant: Draw one sigma ellipse...") {
	SENTENCE (label, U"Label", U"")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false,  label, discriminatPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawConfidenceEllipses, U"Discriminant: Draw confidence ellipses", nullptr) {
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, nullptr, discriminatPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Discriminant_drawOneConfidenceEllipse, U"Discriminant: Draw one confidence ellipse", nullptr) {
	SENTENCE (label, U"Label", U"")
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, label, discriminatPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_Discriminant_extractBetweenGroupsSSCP) {
	CONVERT_EACH (Discriminant)
		autoSSCP result = Discriminant_extractBetweenGroupsSSCP (me);
	CONVERT_EACH_END (my name.get(), U"_between")
}

DIRECT (NEW_Discriminant_extractGroupCentroids) {
	CONVERT_EACH (Discriminant)
		autoTableOfReal result = Discriminant_extractGroupCentroids (me);
	CONVERT_EACH_END (my name.get(), U"_centroids")
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
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Discriminant_extractPooledWithinGroupsSSCP) {
	CONVERT_EACH (Discriminant)
		autoSSCP result = Discriminant_extractPooledWithinGroupsSSCP (me);
	CONVERT_EACH_END (U"pooled_within")
}

FORM (NEW_Discriminant_extractWithinGroupSSCP, U"Discriminant: Extract within-group SSCP", U"Discriminant: Extract within-group SSCP...") {
	NATURAL (groupIndex, U"Group index", U"1")
	OK
DO
	CONVERT_EACH (Discriminant)
		autoSSCP result = Discriminant_extractWithinGroupSSCP (me, groupIndex);
	CONVERT_EACH_END (my name.get(), U"_g", groupIndex)
}

DIRECT (INTEGER_Discriminant_getNumberOfFunctions) {
	INTEGER_ONE (Discriminant)
		integer result = Discriminant_getNumberOfFunctions (me);
	INTEGER_ONE_END (U"")
}

DIRECT (INTEGER_Discriminant_getDimensionOfFunctions) {
	INTEGER_ONE (Discriminant)
		integer result = Eigen_getDimensionOfComponents (my eigen.get());
	INTEGER_ONE_END (U"")
}

DIRECT (INTEGER_Discriminant_getNumberOfGroups) {
	INTEGER_ONE (Discriminant)
		integer result = Discriminant_getNumberOfGroups (me);
	INTEGER_ONE_END (U"")
}

FORM (INTEGER_Discriminant_getNumberOfObservations, U"Discriminant: Get number of observations", U"Discriminant: Get number of observations...") {
	INTEGER (group, U"Group", U"0 (= total)")
	OK
DO
	INTEGER_ONE (Discriminant)
		integer result = Discriminant_getNumberOfObservations (me, group);
	INTEGER_ONE_END (U"")
}


/********************** DTW *******************************************/

FORM (MODIFY_DTW_Polygon_findPathInside, U"DTW & Polygon: Find path inside", nullptr) {
    RADIO (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
	MODIFY_FIRST_OF_TWO (DTW, Polygon)
		DTW_Polygon_findPathInside (me, you, slopeConstraint, 0);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW1_DTW_Polygon_to_Matrix_cumulativeDistances, U"DTW & Polygon: To Matrix (cum. distances)", nullptr) {
    RADIO (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_TWO (DTW, Polygon)
		autoMatrix result = DTW_Polygon_to_Matrix_cumulativeDistances (me, you, slopeConstraint);
     CONVERT_TWO_END (my name.get(), U"_", slopeConstraint);
}

FORM (GRAPHICS_DTW_Sounds_draw, U"DTW & Sounds: Draw", U"DTW & Sounds: Draw...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_COUPLE_AND_ONE (Sound, DTW)
		DTW_Sounds_draw (him, you, me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_COUPLE_AND_ONE_END
}

FORM (GRAPHICS_DTW_Sounds_drawWarp_x, U"DTW & Sounds: Draw warp (x)", U"DTW & Sounds: Draw warp (x)...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (time, U"Time (s)", U"0.1")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_COUPLE_AND_ONE (Sound, DTW)
		DTW_Sounds_drawWarpX (him, you, me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_COUPLE_AND_ONE_END
}

DIRECT (HELP_DTW_help) {
	HELP (U"DTW")
}

FORM (GRAPHICS_DTW_drawPath, U"DTW: Draw path", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawPath (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_DTW_drawDistancesAlongPath, U"DTW: Draw distances along path", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawDistancesAlongPath (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_DTW_paintDistances, U"DTW: Paint distances", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_paintDistances (me, GRAPHICS, xmin, xmax, ymin, ymax, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_DTW_drawWarp_x, U"DTW: Draw warp (x)", U"DTW: Draw warp (x)...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (time, U"Time (s)", U"0.1")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawWarpX (me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_DTW_drawWarp_y, U"DTW: Draw warp (y)", U"DTW: Draw warp (y)...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (time, U"Time (s)", U"0.1")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawWarpY (me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_EACH_END
}

DIRECT (REAL_DTW_getStartTime_x) {
	NUMBER_ONE (DTW)
		double result = my xmin;
	NUMBER_ONE_END (U" s (= start time along x)")
}

DIRECT (REAL_DTW_getEndTime_x) {
	NUMBER_ONE (DTW)
		double result = my xmax;
	NUMBER_ONE_END (U" s (= end time along x)");
}

DIRECT (REAL_DTW_getTotalDuration_x) {
	NUMBER_ONE (DTW)
		double result = my xmax - my xmin;
	NUMBER_ONE_END (U" s (= total duration along x)");
}

DIRECT (REAL_DTW_getStartTime_y) {
	NUMBER_ONE (DTW)
		double result = my ymin;
	NUMBER_ONE_END (U" s (= start time along y)");
}

DIRECT (REAL_DTW_getEndTime_y) {
	NUMBER_ONE (DTW)
		double result = my ymax;
	NUMBER_ONE_END (U" s (= end time along y)");
}

DIRECT (REAL_DTW_getTotalDuration_y) {
	NUMBER_ONE (DTW)
		double result = my ymax - my ymin;
	NUMBER_ONE_END (U" s (= total duration along y)")
}

DIRECT (INTEGER_DTW_getNumberOfFrames_x) {
	INTEGER_ONE (DTW)
		integer result = my nx;
	INTEGER_ONE_END (U" (= number of frames along x)")
}


DIRECT (REAL_DTW_getTimeStep_x) {
	NUMBER_ONE (DTW)
		double result = my dx;
	NUMBER_ONE_END (U" s (= time step along x)")
}

FORM (REAL_DTW_getTimeFromFrameNumber_x, U"DTW: Get time from frame number (x)", nullptr) {
	NATURAL (frameNumber, U"Frame number (x)", U"1")
	OK
DO
	NUMBER_ONE (DTW)
		double result = Matrix_columnToX (me, frameNumber);
	NUMBER_ONE_END (U" s (= y time at x frame ", frameNumber, U")")
}

FORM (INTEGER_DTW_getFrameNumberFromTime_x, U"DTW: Get frame number from time (x)", nullptr) {
	REAL (xTime, U"Time along x (s)", U"0.1")
	OK
DO
	INTEGER_ONE (DTW)
		Melder_require (xTime >= my xmin && xTime <= my xmax, U"Time outside x domain.");
		integer result = Melder_iround (Matrix_xToColumn (me, xTime));
	INTEGER_ONE_END (U" (= x frame at y time ", xTime, U")")
}

DIRECT (INTEGER_DTW_getNumberOfFrames_y) {
	INTEGER_ONE (DTW)
		integer result = my ny;
	INTEGER_ONE_END (U" (= number of frames along y)")
}

DIRECT (REAL_DTW_getTimeStep_y) {
	NUMBER_ONE (DTW)
		double result = my dy;
	NUMBER_ONE_END (U" s (= time step along y)")
}


FORM (REAL_DTW_getTimeFromFrameNumber_y, U"DTW: Get time from frame number (y)", nullptr) {
	NATURAL (frameNumber, U"Frame number (y)", U"1")
	OK
DO
	NUMBER_ONE (DTW)
		double result = Matrix_rowToY (me, frameNumber);
	NUMBER_ONE_END (U" s (= x time at y frame ", frameNumber, U")")
}

FORM (INTEGER_DTW_getFrameNumberFromTime_y, U"DTW: Get frame number from time (y)", nullptr) {
	REAL (yTime, U"Time along y (s)", U"0.1")
	OK
DO
	INTEGER_ONE (DTW)
		Melder_require (yTime >= my ymin && yTime <= my ymax, U"Time outside y domain.");
		integer result = Melder_iround (Matrix_yToRow (me, yTime));
	INTEGER_ONE_END (U" (= y frame at x time ", yTime, U")")
}

FORM (REAL_DTW_getPathY, U"DTW: Get time along path", U"DTW: Get time along path...") {
	REAL (xTime, U"Time (s)", U"0.0")
	OK
DO
	NUMBER_ONE (DTW)
		double result = DTW_getYTimeFromXTime (me, xTime);
	NUMBER_ONE_END (U"")
}

FORM (REAL_DTW_getYTimeFromXTime, U"DTW: Get y time from x time", U"DTW: Get y time from x time...") {
	REAL (xTime, U"Time at x (s)", U"0.0")
	OK
DO
	NUMBER_ONE (DTW)
		double result = DTW_getYTimeFromXTime (me, xTime);
	NUMBER_ONE_END (U" s (= y time at x time ", xTime, U")")
}

FORM (REAL_DTW_getXTimeFromYTime, U"DTW: Get x time from y time", U"DTW: Get x time from y time...") {
	REAL (yTime, U"Time at y (s)", U"0.0")
	OK
DO
	NUMBER_ONE (DTW)
		double result = DTW_getXTimeFromYTime (me, yTime);
	NUMBER_ONE_END (U" s (= x time at y time ", yTime, U")")
}

FORM (INTEGER_DTW_getMaximumConsecutiveSteps, U"DTW: Get maximum consecutive steps", U"DTW: Get maximum consecutive steps...") {
	OPTIONMENU (direction, U"Direction", 1)
		OPTION (U"X")
		OPTION (U"Y")
		OPTION (U"Diagonaal")
	OK
DO
	int direction_code [] = { DTW_START, DTW_X, DTW_Y, DTW_XANDY };
	conststring32 direction_string [] = { U"", U"x", U"y", U"diagonal" };
	INTEGER_ONE (DTW)
		integer result = DTW_getMaximumConsecutiveSteps (me, direction_code [direction]);
	INTEGER_ONE_END (U" (= maximum number of consecutive steps in ", direction_string [direction], U" direction)")
}

DIRECT (REAL_DTW_getDistance_weighted) {
	NUMBER_ONE (DTW)
		double result = my weightedDistance;
	NUMBER_ONE_END (U" (weighted distance)")
}

FORM (REAL_DTW_getDistanceValue, U"DTW: Get distance value", nullptr) {
	REAL (xTime, U"Time at x (s)", U"0.1")
	REAL (yTime, U"Time at y (s)", U"0.1")
	OK
DO
	NUMBER_ONE (DTW)
		double result = undefined;
		if ((xTime >= my xmin && xTime <= my xmax) && (yTime >= my ymin && yTime <= my ymax)) {
			integer irow = Matrix_yToNearestRow (me, yTime);
			integer icol = Matrix_xToNearestColumn (me, xTime);
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

FORM (MODIFY_DTW_formula_distances, U"DTW: Formula (distances)", nullptr) {
	LABEL (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (formula, U"Formula:", U"self")
	OK
DO
	LOOP {
		iam (DTW);
		autoMatrix cp = DTW_to_Matrix_distances (me);
		try {
			Matrix_formula (me, formula, interpreter, 0);
			double minimum, maximum;
			Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
			if (minimum < 0) {
				DTW_Matrix_replace (me, cp.get()); // restore original
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
	REAL (xTime, U"Time at x (s)", U"0.1")
	REAL (yTime, U"Time at y (s)", U"0.1")
	REAL (newDistance, U"New value", U"0.0")
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
		integer irow = Matrix_yToNearestRow (me, yTime);
		integer icol = Matrix_xToNearestColumn (me, xTime);
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
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.05")
    RADIO (slopeConstraint, U"Slope constraint", 1)
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
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.05")
    RADIO (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_EACH (DTW)
        autoMatrix result = DTW_to_Matrix_cumulativeDistances (me, sakoeChibaBand, slopeConstraint);
	CONVERT_EACH_END (my name.get(), U"_cd")
}

FORM (NEW_DTW_to_Polygon, U"DTW: To Polygon...", nullptr) {
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.1")
    RADIO (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_EACH (DTW)
        autoPolygon result = DTW_to_Polygon (me, sakoeChibaBand, slopeConstraint);
    CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_DTW_to_Matrix_distances) {
	CONVERT_EACH (DTW)
		autoMatrix result = DTW_to_Matrix_distances (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_DTW_swapAxes) {
	CONVERT_EACH (DTW)
		autoDTW result = DTW_swapAxes (me);
	CONVERT_EACH_END (my name.get(), U"_axesSwapped")
}

DIRECT (MODIFY_DTW_Matrix_replace) {
	MODIFY_FIRST_OF_TWO (DTW, Matrix)
		DTW_Matrix_replace (me, you);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (NEW1_DTW_TextGrid_to_TextGrid) {
	CONVERT_TWO (DTW, TextGrid)
		autoTextGrid result = DTW_TextGrid_to_TextGrid (me, you, 0);
	CONVERT_TWO_END (your name.get(), U"_", my name.get())
}

DIRECT (NEW1_DTW_IntervalTier_to_Table) {
	CONVERT_TWO (DTW, IntervalTier)
		autoTable result = DTW_IntervalTier_to_Table (me, you, 1.0/44100);
	CONVERT_TWO_END (my name.get())
}

/******************** EditDistanceTable & EditCostsTable ********************************************/

DIRECT (HELP_EditDistanceTable_help) {
	HELP (U"EditDistanceTable")
}

DIRECT (NEW_EditDistanceTable_to_TableOfReal_directions) {
	CONVERT_EACH (EditDistanceTable)
		autoTableOfReal result = EditDistanceTable_to_TableOfReal_directions (me);
	CONVERT_EACH_END (my name.get());
}

DIRECT (MODIFY_EditDistanceTable_setEditCosts) {
	MODIFY_FIRST_OF_TWO (EditDistanceTable, EditCostsTable)
		EditDistanceTable_setEditCosts (me, you);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_EditDistanceTable_setDefaultCosts, U"", nullptr) {
	REAL (insertionCosts, U"Insertion costs", U"1.0")
	REAL (deletionCosts, U"Deletion costs", U"1.0")
	REAL (substitutionCosts, U"Substitution costs", U"2.0")
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
	RADIO (format, U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (precision, U"Precision", U"1")
	REAL (angle, U"Rotate source labels by (degrees)", U"0.0")
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
	SENTENCE (target, U"Target", U"")
	OK
DO
	INTEGER_ONE (EditCostsTable)
		integer result = EditCostsTable_getTargetIndex (me, target);
	INTEGER_ONE_END (U" (target index)")
}

FORM (INTEGER_EditCostsTable_getSourceIndex, U"EditCostsTable: Get source index", nullptr) {
	SENTENCE (source, U"Source", U"")
	OK
DO
	INTEGER_ONE (EditCostsTable)
		integer result = EditCostsTable_getSourceIndex (me, source);
	INTEGER_ONE_END (U" (source index)")
}

FORM (REAL_EditCostsTable_getInsertionCosts, U"EditCostsTable: Get insertion cost", nullptr) {
	SENTENCE (target, U"Target", U"")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getInsertionCost (me, target);
	NUMBER_ONE_END (U" (insertion cost)")
}

FORM (REAL_EditCostsTable_getDeletionCost, U"EditCostsTable: Get deletion cost", nullptr) {
	SENTENCE (source, U"Source", U"")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getDeletionCost (me, source);
	NUMBER_ONE_END (U" (deletion cost)")
}

FORM (REAL_EditCostsTable_getSubstitutionCost, U"EditCostsTable: Get substitution cost", nullptr) {
	SENTENCE (target, U"Target", U"")
	SENTENCE (source, U"Source", U"")
	OK
DO
	NUMBER_ONE (EditCostsTable)
		double result = EditCostsTable_getSubstitutionCost (me, target, source);
	NUMBER_ONE_END (U" (substitution cost)")
}

FORM (REAL_EditCostsTable_getCosts_others, U"EditCostsTable: Get cost (others)", nullptr) {
	RADIO (costTypes, U"Others cost type", 1)
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
	NATURAL (index, U"Index", U"1")
	SENTENCE (target, U"Target", U"a")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		TableOfReal_setRowLabel (me, index, target);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setSourceSymbol_index, U"EditCostsTable: Set source symbol (index)", nullptr) {
	NATURAL (index, U"Index", U"1")
	SENTENCE (source, U"Source", U"a")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		TableOfReal_setColumnLabel (me, index, source);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setInsertionCosts, U"EditCostsTable: Set insertion costs", nullptr) {
	SENTENCE (targets, U"Targets", U"")
	REAL (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setInsertionCosts (me, targets, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setDeletionCosts, U"EditCostsTable: Set deletion costs", nullptr) {
	SENTENCE (sources, U"Sources", U"")
	REAL (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setDeletionCosts (me, sources, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setSubstitutionCosts, U"EditCostsTable: Set substitution costs", nullptr) {
	SENTENCE (targets, U"Targets", U"a i u")
	SENTENCE (sources, U"Sources", U"a i u")
	REAL (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setSubstitutionCosts (me, targets, sources, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setCosts_others, U"EditCostsTable: Set costs (others)", nullptr) {
	LABEL (U"Others costs")
	REAL (insertionCosts, U"Insertion", U"1.0")
	REAL (deletionCosts, U"Deletion", U"1.0")
	LABEL (U"Substitution costs")
	REAL (equalityCosts, U"Equality", U"0.0")
	REAL (inequalityCosts, U"Inequality", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setOthersCosts (me, insertionCosts, deletionCosts, equalityCosts, inequalityCosts);
	MODIFY_EACH_END
}

DIRECT (NEW_EditCostsTable_to_TableOfReal) {
	CONVERT_EACH (EditCostsTable)
		autoTableOfReal result = EditCostsTable_to_TableOfReal (me);
	CONVERT_EACH_END (my name.get());
}

FORM (NEW_EditCostsTable_createEmpty, U"Create empty EditCostsTable", U"Create empty EditCostsTable...") {
	SENTENCE (name, U"Name", U"editCosts")
	INTEGER (numberOfTargetSymbols, U"Number of target symbols", U"0")
	INTEGER (numberOfSourceSymbols, U"Number of source symbols", U"0")
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
	INTEGER (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (fractionSummed, U"Fraction of eigenvalues summed", false)
	BOOLEAN (cumulative, U"Cumulative", false)
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Eigen)
		Eigen_drawEigenvalues (me, GRAPHICS, fromEigenvalue, toEigenvalue, fromAmplitude, toAmplitude, fractionSummed, cumulative, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Eigen_drawEigenvector, U"Eigen: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	INTEGER (eigenvectorNumber, U"Eigenvector number", U"1")
	BOOLEAN (loadings, U"Component loadings", false)
	INTEGER (fromElement, U"left Element range", U"0")
	INTEGER (toElement, U"right Element range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"-1.0")
	REAL (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (connectPoints, U"Connect points", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Eigen)
		Eigen_drawEigenvector (me, GRAPHICS, eigenvectorNumber, fromElement, toElement, fromAmplitude, toAmplitude, loadings, markSize_mm, mark_string, connectPoints, nullptr,  garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_Eigen_getNumberOfEigenvalues) {
	INTEGER_ONE (Eigen)
		integer result = my numberOfEigenvalues;
	INTEGER_ONE_END (U" (number of eigenvalues)")
}

DIRECT (INTEGER_Eigen_getNumberOfEigenvectors) {
	INTEGER_ONE (Eigen)
		integer result = my numberOfEigenvalues;
	INTEGER_ONE_END (U" (number of eigenvectors)")
}

DIRECT (INTEGER_Eigen_getEigenvectorDimension) {
	INTEGER_ONE (Eigen)
		integer result = my dimension;
	INTEGER_ONE_END (U" (dimension)")
}

FORM (REAL_Eigen_getEigenvalue, U"Eigen: Get eigenvalue", U"Eigen: Get eigenvalue...") {
	NATURAL (eigenvalueNumber, U"Eigenvalue number", U"1")
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
	INTEGER (fromEigenvalue, U"left Eigenvalue range",  U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	OK
DO
	NUMBER_ONE (Eigen)
		double result = Eigen_getSumOfEigenvalues (me, fromEigenvalue, toEigenvalue);
	NUMBER_ONE_END (U" (sum of eigenvalues [", fromEigenvalue, U"..", toEigenvalue, U"])")
}

FORM (REAL_Eigen_getEigenvectorElement, U"Eigen: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURAL (elementNumber, U"Element number", U"1")
	OK
DO
	NUMBER_ONE (Eigen)
		double result = Eigen_getEigenvectorElement (me, eigenvectorNumber, elementNumber);
	NUMBER_ONE_END (U" (eigenvector [", eigenvectorNumber, U"] element [", elementNumber, U"])")
}

FORM (MODIFY_Eigen_invertEigenvector, U"Eigen: Invert eigenvector", nullptr) {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
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

FORM (NEW1_Eigen_Matrix_projectColumns, U"Eigen & Matrix: Project columns", U"Eigen & Matrix: Project...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0")
	OK
DO
	CONVERT_TWO (Eigen, Matrix)
		autoMatrix result = Eigen_Matrix_to_Matrix_projectColumns (me, you, numberOfDimensions);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_Eigen_SSCP_project) {
	CONVERT_TWO (Eigen, SSCP)
		autoSSCP result = Eigen_SSCP_project (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_Eigen_Covariance_project) {
	CONVERT_TWO (Eigen, Covariance)
		autoCovariance result = Eigen_Covariance_project (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

/******************** Index ********************************************/

DIRECT (HELP_Index_help) {
	HELP (U"Index")
}

DIRECT (INTEGER_Index_getNumberOfClasses) {
	INTEGER_ONE (Index)
		integer result = my classes -> size;
	INTEGER_ONE_END (U" (number of classes)")
}

FORM (INFO_StringsIndex_getClassLabelFromClassIndex, U"StringsIndex: Get class label", U"StringsIndex: Get class label...") {
	NATURAL (index, U"Class index", U"1")
	OK
DO
	STRING_ONE (StringsIndex)
		const char32* result = StringsIndex_getClassLabelFromClassIndex (me, index);
	STRING_ONE_END
}

FORM (INFO_StringsIndex_getItemLabelFromItemIndex, U"StringsIndex: Get item label", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	STRING_ONE (StringsIndex)
		conststring32 result = StringsIndex_getItemLabelFromItemIndex (me, itemIndex);
	STRING_ONE_END
}

FORM (INTEGER_StringsIndex_getClassIndexFromItemIndex, U"StringsIndex: Get item index", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	INTEGER_ONE (Index)
		integer result = Index_getClassIndexFromItemIndex (me, itemIndex);
	INTEGER_ONE_END (U" (class index)")
}

FORM (INTEGER_Index_getIndex, U"Index: Get item index", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	INTEGER_ONE (Index)
		integer result = Index_getClassIndexFromItemIndex (me, itemIndex);
	INTEGER_ONE_END (U" (class index)")
}

FORM (INTEGER_StringsIndex_getClassIndex, U"StringsIndex: Get class index from calss label", nullptr) {
	WORD (klasLabel, U"Class label", U"label")
	OK
DO
	INTEGER_ONE (StringsIndex)
		integer result = StringsIndex_getClassIndexFromClassLabel (me, klasLabel);
	INTEGER_ONE_END (U" (class index)")
}

FORM (NEW_Index_extractPart, U"Index: Extract part", U"Index: Extract part...") {
	INTEGER (fromItem, U"left Item range", U"0")
	INTEGER (toItem, U"right Item range", U"0")
	OK
DO
	CONVERT_EACH (Index);
		autoIndex result = Index_extractPart (me, fromItem, toItem);
	CONVERT_EACH_END (my name.get(), U"_part")
}

FORM (NEW_Index_to_Permutation, U"Index: To Permutation", U"Index: To Permutation...") {
	BOOLEAN (permuteWithinClasses, U"Permute within classes", true)
	OK
DO
	CONVERT_EACH (Index);
		autoPermutation result = Index_to_Permutation_permuteRandomly (me, permuteWithinClasses);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_StringsIndex_to_Strings) {
	CONVERT_EACH (StringsIndex)
		autoStrings result = StringsIndex_to_Strings (me);
	CONVERT_EACH_END (my name.get())
}

/******************** Excitation ********************************************/

DIRECT (NEW1_Excitations_to_ExcitationList) {
	CONVERT_LIST (Excitation)
		autoExcitationList result = Excitations_to_ExcitationList (& list);
	CONVERT_LIST_END (U"appended")
}

/******************** ExcitationList ********************************************/

FORM (MODIFY_ExcitationList_formula, U"ExcitationList: Formula", nullptr) {
	LABEL (U"for all objects in ExcitationList do { for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	TEXTFIELD (formula, U"Formula:", U"self")
	OK
DO
	MODIFY_EACH (ExcitationList)
		for (integer j = 1; j <= my size; j ++) {
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
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	CONVERT_EACH (ExcitationList)
		autoExcitation result = ExcitationList_extractItem (me, itemIndex);
	CONVERT_EACH_END (my name.get(), U"_", itemIndex)
}

DIRECT (NEW1_ExcitationList_append) {
	CONVERT_COUPLE (ExcitationList)
		autoExcitationList result = Data_copy (me);
		result -> merge (you);
	CONVERT_COUPLE_END (U"appended")
}

FORM (NEW_ExcitationList_to_PatternList, U"Excitations: To PatternList", nullptr) {
	NATURAL (join, U"Join", U"1")
	OK
DO
	CONVERT_EACH (ExcitationList)
		autoPatternList result = ExcitationList_to_PatternList (me, join);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_ExcitationList_to_TableOfReal) {
	CONVERT_EACH (ExcitationList)
		autoTableOfReal result = ExcitationList_to_TableOfReal (me);
	CONVERT_EACH_END (my name.get())
}


/************************* FileInMemory ***********************************/


FORM_READ (READ1_FileInMemory_create, U"Create file in memory", nullptr, true) {
	autoFileInMemory me = FileInMemory_create (file);
	praat_new (me.move(), MelderFile_name (file));
END }

FORM (MODIFY_FileInMemory_setId, U"FileInMemory: Set id", nullptr) {
	SENTENCE (newId, U"New id", U"New id")
	OK
DO
	MODIFY_EACH (FileInMemory)
		FileInMemory_setId (me, newId);
	MODIFY_EACH_END
}

FORM (INFO_FileInMemory_showAsCode, U"FileInMemory: Show as code", nullptr) {
	WORD (name, U"Name", U"example")
	INTEGER (numberOfByterPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemory)
		MelderInfo_open ();
		FileInMemory_showAsCode (me, name, numberOfByterPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

/************************* FileInMemorySet ***********************************/

DIRECT (INFO_FileInMemorySet_getNumberOfFiles) {
	NUMBER_ONE (FileInMemorySet)
		integer result = my size;
	NUMBER_ONE_END (U" (number of files)")
}

FORM (INFO_FileInMemorySet_hasDirectory, U"FileInMemorySet: Has directory?", nullptr) {
	WORD (name, U"Name", U"aav")
	OK
DO
	NUMBER_ONE (FileInMemorySet)
		bool result = FileInMemorySet_hasDirectory (me, name);
	NUMBER_ONE_END (U" (has directory?)")
}



/************************* FileInMemoryManager ***********************************/

DIRECT (NEW1_FileInMemoryManager_create) {
	CREATE_ONE
		autoFileInMemoryManager result = Data_copy (espeak_ng_FileInMemoryManager.get());
	CREATE_ONE_END (U"filesInMemory")
}

DIRECT (INFO_FileInMemoryManager_getNumberOfFiles) {
	NUMBER_ONE (FileInMemoryManager)
		integer result = my files -> size;
	NUMBER_ONE_END (U" (number of files)")
}
DIRECT (INFO_FileInMemoryManager_getNumberOfOpenFiles) {
	NUMBER_ONE (FileInMemoryManager)
		integer result = my openFiles -> size;
	NUMBER_ONE_END (U" (number of open files)")
}

FORM (INFO_FileInMemoryManager_hasDirectory, U"FileInMemoryManager: Has directory?", nullptr) {
	WORD (name, U"Name", U"aav")
	OK
DO
	NUMBER_ONE (FileInMemoryManager)
		bool result = FileInMemoryManager_hasDirectory (me, name);
	NUMBER_ONE_END (U" (has directory?)")
}

FORM (NEW1_FileInMemoryManager_extractFiles, U"FileInMemoryManager: Extract files", nullptr) {
	LABEL (U"Extract all files where the file name ")
	OPTIONMENU_ENUM (kMelder_string, which, U"...", kMelder_string::CONTAINS)
	SENTENCE (criterion, U"...the text", U"/voices/")
	OK
DO
	CONVERT_EACH (FileInMemoryManager)
		autoFileInMemorySet result = FileInMemoryManager_extractFiles (me, which, criterion);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_FileInMemoryManager_downto_Table, U"FileInMemoryManager: Down to Table", nullptr) {
	BOOLEAN (openFilesOnly, U"Open files only?", false)
	OK
DO
	CONVERT_EACH (FileInMemoryManager)
		autoTable result = FileInMemoryManager_downto_Table (me, openFilesOnly);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_FileInMemorySet_createFromDirectoryContents, U"Create files in memory from directory contents", nullptr) {
	SENTENCE (name, U"Name", U"list")
	TEXTFIELD (directory, U"Directory:", U"/home/david/projects/espeak-ng/espeak-ng-data/voices/!v")
	WORD (fileGlobber, U"Only files that match pattern", U"*")
	OK
DO
	CREATE_ONE
		autoFileInMemorySet result = FileInMemorySet_createFromDirectoryContents (directory, fileGlobber);
	CREATE_ONE_END (name)
}

FORM (NEW1_FileInMemorySet_extractFiles, U"FileInMemorySet: Extract files", nullptr) {
	LABEL (U"Extract all files where the file name ")
	OPTIONMENU_ENUM (kMelder_string, which, U"...", kMelder_string::CONTAINS)
	SENTENCE (criterion, U"...the text", U"/voices/")
	OK
DO
	CONVERT_EACH (FileInMemorySet)
		autoFileInMemorySet result = FileInMemorySet_extractFiles (me, which, criterion);
	CONVERT_EACH_END (my name.get())
}

FORM (INFO_FileInMemorySet_showAsCode, U"FileInMemorySet: Show as code", nullptr) {
	WORD (name, U"Name", U"example")
	INTEGER (numberOfBytesPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemorySet)
		MelderInfo_open ();
		FileInMemorySet_showAsCode (me, name, numberOfBytesPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_FileInMemorySet_showOneFileAsCode, U"FileInMemorySet: Show one file as code", nullptr) {
	NATURAL (index, U"Index", U"1")
	WORD (name, U"Name", U"example")
	INTEGER (numberOfBytesPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemorySet)
		MelderInfo_open ();
		FileInMemorySet_showOneFileAsCode (me, index, name, numberOfBytesPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

DIRECT (NEW1_FilesInMemory_to_FileInMemorySet) {
	CONVERT_LIST (FileInMemory)
		autoFileInMemorySet result = FilesInMemory_to_FileInMemorySet (list);
	CONVERT_LIST_END (U"merged");
}

DIRECT (MODIFY_FileInMemorySet_addItemsToSet) {
	MODIFY_FIRST_OF_TWO (FileInMemorySet, FileInMemory)
		autoFileInMemory him = Data_copy (you);
		my addItem_move (him.move());
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (NEW1_FileInMemorySets_merge) {
	CONVERT_LIST (FileInMemorySet)
		autoFileInMemorySet result = FileInMemorySets_merge (list);
	CONVERT_LIST_END (U"merge");
}

DIRECT (NEW_FileInMemorySet_to_Strings_id) {
	CONVERT_EACH (FileInMemorySet)
		autoStrings result = FileInMemorySet_to_Strings_id (me);
	CONVERT_EACH_END (my name.get());
}

/************************* FilterBank ***********************************/

FORM (GRAPHICS_FilterBank_drawFilters, U"FilterBank: Draw filters", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawRows (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_drawOneContour, U"FilterBank: Draw one contour", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (height, U"Height (dB)", U"40.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawOneContour (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, height);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_drawContours, U"FilterBank: Draw contours", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawContours (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_drawFrequencyScales, U"FilterBank: Draw frequency scales", U"FilterBank: Draw frequency scales...") {
	RADIO (xFrequencyScale, U"Horizontal frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (xFromFrequency, U"left Horizontal frequency range", U"0.0")
	REAL (xToFrequency, U"right Horizontal frequency range", U"0.0")
	RADIO (yFrequencyScale, U"Vertical frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (yFromFrequency, U"left Vertical frequency range", U"0.0")
	REAL (yToFrequency, U"right Vertical frequency range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawFrequencyScales (me, GRAPHICS, xFrequencyScale, xFromFrequency, xToFrequency, yFrequencyScale, yFromFrequency, yToFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelSpectrogram_paintImage, U"MelSpectrogram: Paint image", U"MelSpectrogram: Paint image...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkSpectrogram_paintImage, U"BarkSpectrogram: Paint image", U"BarkSpectrogram: Paint image...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_paintImage, U"FilterBank: Paint image", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_paintContours, U"FilterBank: Paint contours", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintContours (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_FilterBank_paintCells, U"FilterBank: Paint cells", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintCells (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FilterBank_paintSurface, U"FilterBank: Paint surface", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintSurface (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude,30, 45);
	GRAPHICS_EACH_END
}

FORM (REAL_FilterBank_getFrequencyInHertz, U"FilterBank: Get frequency in Hertz", U"FilterBank: Get frequency in Hertz...") {
	REAL (frequency, U"Frequency", U"10.0")
	RADIO (frequencyUnit, U"Unit", 2)
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
	REAL (frequency, U"Frequency", U"93.17")
	RADIO (frequencyUnit, U"Unit", 1)
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
	REAL (frequency, U"Frequency", U"1000.0")
	RADIO (frequencyUnit, U"Unit", 1)
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
	REAL (intensity_dB, U"Intensity (dB)", U"80.0")
	OK
DO
	MODIFY_EACH (FilterBank)
		FilterBank_equalizeIntensities (me, intensity_dB);
	MODIFY_EACH_END
}

FORM (MODIFY_BandFilterSpectrogram_equalizeIntensities, U"BandFilterSpectrogram: Equalize intensities", nullptr) {
	REAL (intensity_dB, U"Intensity (dB)", U"80.0")
	OK
DO
	MODIFY_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_equalizeIntensities (me, intensity_dB);
	MODIFY_EACH_END
}

DIRECT (NEW_FilterBank_to_Matrix) {
	CONVERT_EACH (FilterBank)
		autoMatrix result = FilterBank_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_BandFilterSpectrogram_to_Matrix, U"BandFilterSpectrogram: To Matrix", nullptr) {
	BOOLEAN (convertToDB, U"Convert to dB values", 1)
	OK
DO
	CONVERT_EACH (BandFilterSpectrogram)
		autoMatrix result = BandFilterSpectrogram_to_Matrix (me, convertToDB);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_FilterBanks_crossCorrelate, U"FilterBanks: Cross-correlate", nullptr) {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (FilterBank)
		autoSound result = FilterBanks_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_BandFilterSpectrograms_crossCorrelate, U"BandFilterSpectrograms: Cross-correlate", nullptr) {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (BandFilterSpectrogram)
		autoSound result = BandFilterSpectrograms_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_FilterBanks_convolve, U"FilterBanks: Convolve", nullptr) {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (FilterBank)
		autoSound result = FilterBanks_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_BandFilterSpectrograms_convolve, U"BandFilterSpectrograms: Convolve", nullptr) {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (BandFilterSpectrogram)
		autoSound result = BandFilterSpectrograms_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW_FilterBank_to_Intensity) {
	CONVERT_EACH (FilterBank)
		autoIntensity result = FilterBank_to_Intensity (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_BandFilterSpectrogram_to_Intensity) {
	CONVERT_EACH (BandFilterSpectrogram)
		autoIntensity result = BandFilterSpectrogram_to_Intensity (me);
	CONVERT_EACH_END (my name.get())
}

/*********** FormantFilter *******************************************/

DIRECT (HELP_FormantFilter_help) {
	HELP (U"FormantFilter")
}

FORM (GRAPHICS_FormantFilter_drawFilterFunctions, U"FormantFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	RADIO (frequencyScale, U"Frequency scale", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
		RADIOBUTTON (U"mel")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (dBScale, U"Amplitude scale in dB", 1)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantFilter)
		FormantFilter_drawFilterFunctions (me, GRAPHICS, bandwidth, frequencyScale, fromFilter, toFilter, fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantFilter_drawSpectrum_slice, U"FormantFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Hz", garnish);
	GRAPHICS_EACH_END
}

/****************** FormantGrid  *********************************/

FORM (GRAPHICS_old_FormantGrid_draw, U"FormantGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= auto)")
	BOOLEAN (bandwidths, U"Bandwidths", false)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantGrid)
		FormantGrid_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, bandwidths, garnish, U"lines and speckles");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FormantGrid_draw, U"FormantGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= auto)")
	BOOLEAN (bandwidths, U"Bandwidths", false)
	BOOLEAN (garnish, U"Garnish", true)
	LABEL (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
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
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (extrapolate, U"Extrapolate", false)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FunctionTerms)
		FunctionTerms_draw (me, GRAPHICS, xmin, xmax, ymin, ymax,extrapolate, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_FunctionTerms_drawBasisFunction, U"FunctionTerms: Draw basis function", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (extrapolate, U"Extrapolate", false)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FunctionTerms)
		FunctionTerms_drawBasisFunction (me, GRAPHICS, index, xmin, xmax, ymin, ymax, extrapolate, garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_FunctionTerms_getValue, U"FunctionTerms: Evaluate", nullptr) {
	REAL (x, U"X", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_evaluate (me, x);
	NUMBER_ONE_END (U"")
}

DIRECT (INTEGER_FunctionTerms_getNumberOfCoefficients) {
	INTEGER_ONE (FunctionTerms)
		integer result = my numberOfCoefficients;
	INTEGER_ONE_END (U"")
}

FORM (REAL_FunctionTerms_getCoefficient, U"FunctionTerms: Get coefficient", nullptr) {
	LABEL (U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = ( index > 0 && index <= my numberOfCoefficients ? my coefficients [index] : undefined );
	NUMBER_ONE_END (U"")
}

DIRECT (INTEGER_FunctionTerms_getDegree) {
	INTEGER_ONE (FunctionTerms)
		integer result = FunctionTerms_getDegree (me);
	INTEGER_ONE_END (U"")
}

FORM (REAL_FunctionTerms_getMaximum, U"FunctionTerms: Get maximum", U"Polynomial: Get maximum...") {
	LABEL (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getMaximum (me, xmin, xmax);
	NUMBER_ONE_END (U" (maximum)")
}

FORM (REAL_FunctionTerms_getMinimum, U"FunctionTerms: Get minimum", U"Polynomial: Get minimum...") {
	LABEL (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getMinimum (me, xmin, xmax);
	NUMBER_ONE_END (U" (minimum)")
}

FORM (REAL_FunctionTerms_getXOfMaximum, U"FunctionTerms: Get x of maximum", U"Polynomial: Get x of maximum...") {
	LABEL (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getXOfMaximum (me, xmin, xmax);
	NUMBER_ONE_END (U" (x of maximum)")
}

FORM (REAL_FunctionTerms_getXOfMinimum, U"FunctionTerms: Get x of minimum", U"Polynomial: Get x of minimum...") {
	LABEL (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (FunctionTerms)
		double result = FunctionTerms_getXOfMinimum (me, xmin, xmax);
	NUMBER_ONE_END (U" (x of minimum)")
}

FORM (MODIFY_FunctionTerms_setCoefficient, U"FunctionTerms: Set coefficient", nullptr) {
	LABEL (U"p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	LABEL (U"F[k] is of degree k")
	NATURAL (index, U"Index", U"1")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FunctionTerms)
		FunctionTerms_setCoefficient (me, index, value);
	MODIFY_EACH_END
}

FORM (MODIFY_FunctionTerms_setDomain, U"FunctionTerms: Set domain", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"2.0")
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

FORM (NEW_Intensity_to_TextGrid_silences, U"Intensity: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...") {
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	WORD (silenceLabel, U"Silent interval label", U"silent")
	WORD (soundingLabel, U"Sounding interval label", U"sounding")
	OK
DO
	CONVERT_EACH (Intensity)
		autoTextGrid result = Intensity_to_TextGrid_detectSilences (me, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel);
	CONVERT_EACH_END (my name.get())
}

/***************** IntensityTier ***************************************************/

FORM (NEW_IntensityTier_to_TextGrid_silences, U"IntensityTier: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...") {
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	WORD (silenceLabel, U"Silent interval label", U"silent")
	WORD (soundingLabel, U"Sounding interval label", U"sounding")
	POSITIVE (timeStep, U"Time step (s)", U"0.001")
	OK
DO
	CONVERT_EACH (IntensityTier)
		autoTextGrid result = IntensityTier_to_TextGrid_detectSilences (me, timeStep, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_IntensityTier_to_Intensity, U"", nullptr) {
	POSITIVE (timeStep, U"Time step (s)", U"0.001")
	OK
DO
	CONVERT_EACH (IntensityTier)
		autoIntensity result = IntensityTier_to_Intensity (me, timeStep);
	CONVERT_EACH_END (my name.get())
}

/***************** ISpline ***************************************************/

DIRECT (HELP_ISpline_help) {
	HELP (U"ISpline")
}

FORM (NEW1_ISpline_create, U"Create ISpline", U"Create ISpline...") {
	WORD (name, U"Name", U"ispline")
	LABEL (U"Domain")
	REAL (xmin, U"Xmin", U"0")
	REAL (xmax, U"Xmax", U"1")
	LABEL (U"ISpline(x) = c[1] I[1](x) + c[2] I[1](x) + ... c[n] I[n](x)")
	LABEL (U"all I[k] are polynomials of degree \"Degree\"")
	LABEL (U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree")
	INTEGER (degree, U"Degree", U"3")
	SENTENCE (coefficients_string, U"Coefficients (c[k])", U"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE (knots_string, U"Interior knots" , U"0.3 0.5 0.6")
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
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"16000")
	RADIO (synthesisModel, U"Synthesis model", 1)
		RADIOBUTTON (U"Cascade")
		RADIOBUTTON (U"Parallel")
	NATURAL (numberOfFormants, U"Number of formants", U"5")
	POSITIVE (frameDuration, U"Frame duration (s)", U"0.005")
	REAL (flutter_percentage, U"Flutter percentage (%)", U"0.0")   // ppgb: foutgevoelig
	OPTIONMENU (voicingSource, U"Voicing source", 1)
		OPTION (U"Impulsive")
		OPTION (U"Natural")
	OPTIONMENU (soundOutputType, U"Output type", 1)
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
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_KlattTable_to_KlattGrid, U"KlattTable: To KlattGrid", nullptr) {
	POSITIVE (frameDuration, U"Frame duration (s)", U"0.002")
	OK
DO
	CONVERT_EACH (KlattTable)
		autoKlattGrid result = KlattTable_to_KlattGrid (me, frameDuration);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_KlattTable_to_Table) {
	CONVERT_EACH (KlattTable)
		autoTable result = KlattTable_to_Table (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Table_to_KlattTable) {
	CONVERT_EACH (Table)
		autoKlattTable result = Table_to_KlattTable (me);
	CONVERT_EACH_END (my name.get())
}

FORM (REAL_Table_getMedianAbsoluteDeviation, U"Table: Get median absolute deviation", U"Table: Get median absolute deviation...") {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getMedianAbsoluteDeviation (me, columnNumber);
	NUMBER_ONE_END (U"")
}

FORM (INFO_Table_reportRobustStatistics, U"Table: Report robust statistics", U"Table: Report robust statistics...") {
	SENTENCE (columnLabel, U"Column label", U"F1")
	POSITIVE (k_stdev, U"Number of standard deviations", U"1.5")
	POSITIVE (tolerance, U"Tolerance", U"1e-6")
	NATURAL (maximumNumberOfiterations, U"Maximum number of iterations", U"30")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double location, scale;
		Table_reportHuberMStatistics (me, columnNumber, k_stdev, tolerance, & location, & scale, maximumNumberOfiterations);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Location: ", location);
			MelderInfo_writeLine (U"Scale: ", scale);
		MelderInfo_close();
	INFO_ONE_END
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
		Melder_padOrTruncate (15, my columnHeaders[1]. label.get()), U"\t",
		Melder_padOrTruncate (15, my columnHeaders[2]. label.get()), U"\t",
		Melder_padOrTruncate (15, my columnHeaders[3]. label.get()));
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		MelderInfo_writeLine (
			Melder_padOrTruncate (15, row -> cells[1]. string.get()), U"\t",
			Melder_padOrTruncate (15, Melder_double (row -> cells[2]. number)), U"\t",
			Melder_padOrTruncate (15, Melder_double (row -> cells[3]. number)));
	}
}

FORM (INTEGER_Table_getNumberOfRowsWhere, U"", nullptr) {
	TEXTFIELD (formula, U"Count only rows where the following condition holds:", U"1; self$[\"gender\"]=\"M\"")
	OK
DO
	INTEGER_ONE (Table)
		integer result = Table_getNumberOfRowsWhere (me, formula, interpreter);
	INTEGER_ONE_END (U"")
}

FORM (INFO_Table_reportOneWayAnova, U"Table: Report one-way anova",  U"Table: Report one-way anova...") {
	SENTENCE (dataColumn_string, U"Column with data", U"F0")
	SENTENCE (factor_string, U"Factor", U"Vowel")
	BOOLEAN (wantMeans, U"Table with means", false);
	BOOLEAN (wantDifferencesBetweenMeans, U"Table with differences between means", false)
	BOOLEAN (wantTukeyPostHoc, U"Table with Tukey's post-hoc HSD test", false)
	OK
DO
	INFO_ONE (Table)
		integer factorColumn = Table_getColumnIndexFromColumnLabel (me, factor_string);
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		autoTable means, meansDiff, meansDiffProbabilities;
		autoTable anova = Table_getOneWayAnalysisOfVarianceF (me, dataColumn, factorColumn, &means, &meansDiff, & meansDiffProbabilities);
		MelderInfo_open ();
		MelderInfo_writeLine (U"One-way analysis of \"", dataColumn_string, U"\" by \"", factor_string, U"\".\n");
		Table_printAsAnovaTable (anova.get());
		MelderInfo_writeLine (U"\nMeans:\n");
		print_means (means.get());
		MelderInfo_close ();
		if (wantMeans) {
			praat_new (means.move(), my name.get(), U"_groupMeans");
		}
		if (wantDifferencesBetweenMeans) {
			praat_new (meansDiff.move(), my name.get(), U"_meansDiff");
		}
		if (wantTukeyPostHoc) {
			praat_new (meansDiffProbabilities.move(), my name.get(), U"_meansDiffP");
		}
	INFO_ONE_END
}

FORM (INFO_Table_reportTwoWayAnova, U"Table: Report two-way anova", U"Table: Report two-way anova...") {
	SENTENCE (dataColumn_string, U"Column with data", U"Data")
	SENTENCE (firstFactor_string, U"First factor", U"A")
	SENTENCE (secondFactor_string, U"Second factor", U"B")
	BOOLEAN (wantMeans, U"Table with means", false);
	OK
DO
	INFO_ONE (Table)
		integer firstFactorColumn = Table_getColumnIndexFromColumnLabel (me, firstFactor_string);
		integer secondFactorColumn = Table_getColumnIndexFromColumnLabel (me, secondFactor_string);
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
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
			praat_new (means.move(), my name.get(), U"_groupMeans");
		}
	INFO_ONE_END
}

FORM (INFO_Table_reportOneWayKruskalWallis, U"Table: Report one-way Kruskal-Wallis", U"Table: Report one-way Kruskal-Wallis...") {
	SENTENCE (dataColumn_string, U"Column with data", U"Data")
	SENTENCE (factor_string, U"Factor", U"Group")
	OK
DO
	INFO_ONE (Table)
		integer factorColumn = Table_getColumnIndexFromColumnLabel (me, factor_string);
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
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
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	CONVERT_EACH (Table)
		integer icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoStringsIndex result = Table_to_StringsIndex_column (me, icol);
	CONVERT_EACH_END (my name.get(), U"_", columnLabel)
}

/******************* LegendreSeries *********************************/

FORM (NEW1_LegendreSeries_create, U"Create LegendreSeries", U"Create LegendreSeries...") {
	WORD (name, U"Name", U"ls")
	LABEL (U"Domain")
	REAL (xmin, U"Xmin", U"-1")
	REAL (xmax, U"Xmax", U"1")
	LABEL (U"LegendreSeries(x) = c[1] P[0](x) + c[2] P[1](x) + ... c[n+1] P[n](x)")
	LABEL (U"P[k] is a Legendre polynomial of degree k")
	SENTENCE (coefficients_string, U"Coefficients", U"0 0 1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be smaller than Xmax.");
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
	CONVERT_EACH_END (my name.get())
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

FORM_SAVE (SAVE_LongSounds_saveAsStereoAIFFFile, U"LongSound: Save as AIFF file", 0, U"aiff") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_AIFF, file);
	END
}

FORM_SAVE (SAVE_LongSounds_saveAsStereoAIFCFile, U"LongSound: Save as AIFC file", 0, U"aifc") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_AIFC, file);
	END
}


FORM_SAVE (SAVE_LongSounds_saveAsStereoWAVFile, U"LongSound: Save as WAV file", 0, U"wav") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_WAV, file);
	END
}

FORM_SAVE (SAVE_LongSounds_saveAsStereoNeXtSunFile, U"LongSound: Save as NeXT/Sun file", 0, U"au") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_NEXT_SUN, file);
	END
}

FORM_SAVE (SAVE_LongSounds_saveAsStereoNISTFile, U"LongSound: Save as NIST file", 0, U"nist") {
	FIND_COUPLE (LongSound)
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_NIST, file);
	END
}

/******************* Matrix **************************************************/

FORM (GRAPHICS_Matrix_drawAsSquares, U"Matrix: Draw as squares", U"Matrix: Draw as squares...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawAsSquares (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_drawDistribution, U"Matrix: Draw distribution", U"Matrix: Draw distribution...") {
	LABEL (U"Selection of (part of) Matrix")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	LABEL (U"Selection of Matrix values")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimumFrequency, U"Minimum frequency", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawDistribution (me, GRAPHICS, xmin, xmax, ymin, ymax, minimumValue, maximumValue, numberOfBins, minimumFrequency, maximumFrequency, false, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_drawCumulativeDistribution, U"Matrix: Draw cumulative distribution", nullptr) {
	LABEL (U"Selection of (part of) Matrix")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	LABEL (U"Selection of Matrix values")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawDistribution (me, GRAPHICS, xmin, xmax, ymin, ymax, minimumValue, maximumValue, numberOfBins, minimum, maximum, true, garnish);
	GRAPHICS_EACH_END
}

FORM (REAL_Matrix_getMean, U"Matrix: Get mean", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	NUMBER_ONE (Matrix)
		double result = Matrix_getMean (me, xmin, xmax, ymin, ymax);
	NUMBER_ONE_END (U" (mean)")
}

FORM (REAL_Matrix_getStandardDeviation, U"Matrix: Get standard deviation", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	NUMBER_ONE (Matrix)
		double result = Matrix_getStandardDeviation (me, xmin, xmax, ymin, ymax);
	NUMBER_ONE_END (U" (std dev)")
}

FORM (REAL_Matrix_getNorm, U"Matrix: Get norm", U"Matrix: Get norm...") {
	REAL (power, U"Power", U"2.0")
	OK
DO
	NUMBER_ONE (Matrix)
		double result = NUMnorm (my z.all(), power);
	NUMBER_ONE_END (U" (norm with power = ", power, U")")
}
	
FORM (MODIFY_Matrix_scale, U"Matrix: Scale", nullptr) {
	LABEL (U"self[row, col] := self[row, col] / `Scale factor'")
	RADIO (scaleMethod, U"Scale factor", 1)
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
	CONVERT_EACH_END (my name.get(), U"_transposed")
}

DIRECT (NEW_Matrix_to_PCA_byColumns) {
	CONVERT_EACH (Matrix)
		autoPCA result = Matrix_to_PCA_byColumns (me);
	CONVERT_EACH_END (my name.get(), U"_columns");
}

DIRECT (NEW_Matrix_to_PCA_byRows) {
	CONVERT_EACH (Matrix)
		autoPCA result = Matrix_to_PCA_byRows (me);
	CONVERT_EACH_END (my name.get(), U"_rows")
}

FORM (NEW_Matrix_solveEquation, U"Matrix: Solve equation", U"Matrix: Solve equation...") {
	REAL (tolerance, U"Tolerance", U"1.0e-7")
	OK
DO
	CONVERT_EACH (Matrix)
		autoMatrix result = Matrix_solveEquation (me, tolerance);
	CONVERT_EACH_END (my name.get(), U"_solution")
}

FORM (NEW_Matrix_solveMatrixEquation, U"Matrix: Solve matrix equation", U"Matrix: Solve matrix equation...") {
	REAL (tolerance, U"Tolerance", U"1.0e-7")
	OK
DO
	CONVERT_COUPLE (Matrix)
		autoMatrix result = Matrix_solveEquation (me, you, tolerance);
	CONVERT_COUPLE_END (U"solution")
}

DIRECT (NEW1_Matrix_Categories_to_TableOfReal) {
	CONVERT_ONE_AND_GENERIC (Categories, Matrix)
		autoTableOfReal result = Matrix_Categories_to_TableOfReal (you, me);
	CONVERT_ONE_AND_GENERIC_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_ActivationList_Categories_to_TableOfReal) {
	CONVERT_TWO (ActivationList, Categories)
		autoTableOfReal result = Matrix_Categories_to_TableOfReal (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (GRAPHICS_Matrix_scatterPlot, U"Matrix: Scatter plot", nullptr) {
	NATURAL (xColumn, U"Column for X-axis", U"1")
	NATURAL (yColumn, U"Column for Y-axis", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO

	GRAPHICS_EACH (Matrix)
		Matrix_scatterPlot (me, GRAPHICS, xColumn, yColumn, xmin, xmax, ymin, ymax, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_Matrix_to_ActivationList) {
	CONVERT_EACH (Matrix)
		autoActivationList result = Matrix_to_ActivationList (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Matrix_to_Eigen) {
	CONVERT_EACH (Matrix)
		autoEigen result = Matrix_to_Eigen (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Matrix_to_SVD) {
	CONVERT_EACH (Matrix)
		autoSVD result = SVD_createFromGeneralMatrix (my z.get());
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEWTIMES2_Matrix_eigen_complex) {
	LOOP {
		iam_LOOP (Matrix);
		autoMatrix vectors, values;
		Matrix_Eigen_complex (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	}
END }

FORM (NEW_Matrix_to_NMF_mu, U"Matrix: To NMF (m.u.)", U"Matrix: To NMF (m.u.)...") {
	NATURAL (numberOfFeatures, U"Number of features", U"2")
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"400")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	OPTIONMENU_ENUM (kNMF_Initialization, initializationMethod, U"Initialisation method", kNMF_Initialization::RandomUniform)
	BOOLEAN (info, U"Info", 0)
	OK
DO
	Melder_require (maximumNumberOfIterations >= 0, U"The maximum number of iterations should not e negative.");
	CONVERT_EACH (Matrix)
		autoNMF result = Matrix_to_NMF_mu (me, numberOfFeatures, maximumNumberOfIterations, tolx, told, initializationMethod, info);
	CONVERT_EACH_END (my name.get(), U"_mu")
}

FORM (NEW_Matrix_to_NMF_als, U"Matrix: To NMF (ALS)", U"Matrix: To NMF (ALS)...") {
	NATURAL (numberOfFeatures, U"Number of features", U"2")
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"20")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	OPTIONMENU_ENUM (kNMF_Initialization, initializationMethod, U"Initialisation method", kNMF_Initialization::RandomUniform)
	BOOLEAN (info, U"Info", 0)
	OK
DO
	Melder_require (maximumNumberOfIterations >= 0, U"The maximum number of iterations should not e negative.");
	CONVERT_EACH (Matrix)
		autoNMF result = Matrix_to_NMF_als (me, numberOfFeatures, maximumNumberOfIterations, tolx, told, initializationMethod, info);
	CONVERT_EACH_END (my name.get(), U"_als")
}

DIRECT (REAL_NMF_Matrix_getEuclideanDistance) {
	NUMBER_TWO (NMF, Matrix)
		double result = NMF_getEuclideanDistance (me, your z.get());
	NUMBER_TWO_END (U" (= ", result / (your ny * your nx), U" * nrow * ncol)")
}

FORM (MODIFY_NMF_Matrix_improveFactorization_mu, U"NMF & Matrix: Improve factorization (m.u.)", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	BOOLEAN (info, U"Info", 0)
	OK
DO
	MODIFY_FIRST_OF_TWO (NMF, Matrix)
		NMF_improveFactorization_mu (me, your z.get(), maximumNumberOfIterations, tolx, told, info);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_NMF_Matrix_improveFactorization_als, U"NMF & Matrix: Improve factorization (ALS)", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"10")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	BOOLEAN (info, U"Info", 0)
	OK
DO
	MODIFY_FIRST_OF_TWO (NMF, Matrix)
		NMF_improveFactorization_als (me, your z.get(), maximumNumberOfIterations, tolx, told, info);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW1_Matrices_to_DTW, U"Matrices: To DTW", U"Matrix: To DTW...") {
	LABEL (U"Distance  between cepstral coefficients")
	REAL (distanceMetric, U"Distance metric", U"2.0")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (Matrix)
		autoDTW result = Matrices_to_DTW (me, you, matchStart, matchEnd, slopeConstraint, distanceMetric);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW_Matrix_to_PatternList, U"Matrix: To PatternList", nullptr) {
	NATURAL (join, U"Join", U"1")
	OK
DO
	CONVERT_EACH (Matrix)
		autoPatternList result = Matrix_to_PatternList (me, join);
	CONVERT_EACH_END (my name.get())
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
		integer result = my ny;
	INTEGER_ONE_END (U"")
}

DIRECT (REAL_FilterBank_getFrequencyDistance) {
	NUMBER_ONE (FilterBank)
		double result = my dy;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_FilterBank_getTimeFromColumn, U"Get time of column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = Matrix_columnToX (me, columnNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_FilterBank_getFrequencyFromRow, U"Get frequency of row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = Matrix_rowToY (me, rowNumber);
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_FilterBank_getValueInCell, U"Get value in cell", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	POSITIVE (frequency, U"Frequency", U"1.0")
	OK
DO
	NUMBER_ONE (FilterBank)
		double result = undefined;
		if ((frequency >= my ymin && frequency <= my ymax) && (time >+ my xmin && time <= my ymin)) {
			integer col = Matrix_xToNearestColumn (me, time);
			if (col < 1) {
				col = 1;
			}
			if (col > my nx) {
				col = my nx;
			}
			integer row = Matrix_yToNearestRow (me, frequency);
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
		integer result = my ny;
	INTEGER_ONE_END (U"")
}

DIRECT (REAL_BandFilterSpectrogram_getFrequencyDistance) {
	NUMBER_ONE (BandFilterSpectrogram)
		double result = my dy;
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_BandFilterSpectrogram_getFrequencyFromRow, U"Get frequency of row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	NUMBER_ONE (BandFilterSpectrogram)
		double result = Matrix_rowToY (me, rowNumber);
	NUMBER_ONE_END (U" ", my v_getFrequencyUnit ())
}

FORM (REAL_BandFilterSpectrogram_getTimeFromColumn, U"Get time of column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_ONE (BandFilterSpectrogram)
		double result = Matrix_columnToX (me, columnNumber);
	NUMBER_ONE_END (U" s")
}

FORM (REAL_BandFilterSpectrogram_getValueInCell, U"Get value in cell", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	POSITIVE (frequency, U"Frequency", U"1")
	OK
DO
	NUMBER_ONE (BandFilterSpectrogram)
		double result = undefined;
		if ((frequency >= my ymin && frequency <= my ymax) && (time >+ my xmin && time <= my ymin)) {
			integer col = Matrix_xToNearestColumn (me, time);
			if (col < 1) {
				col = 1;
			}
			if (col > my nx) {
				col = my nx;
			}
			integer row = Matrix_yToNearestRow (me, frequency);
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
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	RADIO (frequencyScale, U"Frequency scale", 1)
	RADIOBUTTON (U"Hertz")
	RADIOBUTTON (U"Bark")
	RADIOBUTTON (U"Mel")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (dBScale, U"Amplitude scale in dB", false)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelFilter)
		MelFilter_drawFilterFunctions (me, GRAPHICS, frequencyScale, fromFilter, toFilter, fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelSpectrogram_drawTriangularFilterFunctions, U"MelSpectrogram: Draw triangulat filter functions", U"MelSpectrogram: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	RADIO (frequencyScale, U"Frequency scale", 1)
	RADIOBUTTON (U"Mel")
	RADIOBUTTON (U"Hertz")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (dBScale, U"Amplitude scale in dB", false)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		MelSpectrogram_drawTriangularFilterFunctions (me, GRAPHICS, frequencyScale - 1, fromFilter, toFilter, fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelFilter_drawSpectrum, U"MelFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Mels", garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelSpectrogram_drawSpectrumAtNearestTimeSlice, U"MelSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_BarkSpectrogram_drawSpectrumAtNearestTimeSlice, U"BarkSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_MelFilter_paint, U"FilterBank: Paint", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (Matrix)
		FilterBank_paint ((FilterBank) me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW_MelFilter_to_MFCC, U"MelFilter: To MFCC", U"MelSpectrogram: To MFCC...") {
	NATURAL (numberOfCoefficients, U"Number of coefficients", U"12")
	OK
DO
	CONVERT_EACH (MelFilter)
		autoMFCC result = MelFilter_to_MFCC (me, numberOfCoefficients);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_MelSpectrogram_to_MFCC, U"MelSpectrogram: To MFCC", U"MelSpectrogram: To MFCC...") {
	NATURAL (numberOfCoefficients, U"Number of coefficients", U"12")
	OK
DO
	CONVERT_EACH (MelSpectrogram)
		autoMFCC result = MelSpectrogram_to_MFCC (me, numberOfCoefficients);
	CONVERT_EACH_END (my name.get())
}

/**************** Ltas *******************************************/

#include "../kar/UnicodeData.h"
FORM (INFO_Ltas_reportSpectralTilt, U"Ltas: Report spectral tilt", nullptr) {
	POSITIVE (fromFrequency, U"left Frequency range (Hz)", U"100.0")
	POSITIVE (toFrequency, U"right Frequency range (Hz)", U"5000.0")
	OPTIONMENU (frequencyScale, U"Frequency scale", 1)
		OPTION (U"Linear")
		OPTION (U"Logarithmic")
	OPTIONMENU (fitMethod, U"Fit method", 2)
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
	INTEGER (fromCoefficient, U"From coefficient", U"0")
	INTEGER (toCoefficient, U"To coefficient", U"0")
	OK
DO
	CONVERT_EACH (MFCC)
		autoMelFilter result = MFCC_to_MelFilter (me, fromCoefficient, toCoefficient);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_MFCC_to_MelSpectrogram, U"MFCC: MelSpectrogram", U"MFCC: To MelSpectrogram...") {
	INTEGER (fromCoefficient, U"From coefficient", U"0")
	INTEGER (toCoefficient, U"To coefficient", U"0")
	BOOLEAN (includeConstant, U"Include constant term", true)
	OK
DO
	CONVERT_EACH (MFCC)
		autoMelSpectrogram result = MFCC_to_MelSpectrogram (me, fromCoefficient, toCoefficient, includeConstant);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_MFCC_to_TableOfReal, U"MFCC: To TableOfReal", U"MFCC: To TableOfReal...") {
	BOOLEAN (includeEnergy, U"Include energy", false)
	OK
DO
	CONVERT_EACH (MFCC)
		autoTableOfReal result = MFCC_to_TableOfReal (me, includeEnergy);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_MFCC_to_Matrix_features, U"MFCC: To Matrix (features)", nullptr) {
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	BOOLEAN (includeEnergy, U"Include energy", false)
	OK
DO
	CONVERT_EACH (MFCC)
		autoMatrix result = MFCC_to_Matrix_features (me, windowLength, includeEnergy);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_MFCCs_crossCorrelate, U"MFCC & MFCC: Cross-correlate", nullptr) {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (MFCC)
		autoSound result = MFCCs_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_",your name.get())
}

FORM (NEW1_MFCCs_convolve, U"MFCC & MFCC: Convolve", nullptr) {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (MFCC)
		autoSound result = MFCCs_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_",your name.get())
}

DIRECT (NEW_MFCC_to_Sound) {
	CONVERT_EACH (MFCC)
		autoSound result = MFCC_to_Sound (me);
	CONVERT_EACH_END (my name.get())
}

/**************** MSpline *******************************************/

FORM (NEW_MSpline_create, U"Create MSpline", U"Create MSpline...") {
	WORD (name, U"Name", U"mspline")
	LABEL (U"Domain")
	REAL (xmin, U"Xmin", U"0")
	REAL (xmax, U"Xmax", U"1")
	LABEL (U"MSpline(x) = c[1] M[1](x) + c[2] M[1](x) + ... c[n] M[n](x)")
	LABEL (U"all M[k] are polynomials of degree \"Degree\"")
	LABEL (U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree + 1")
	INTEGER (degree, U"Degree", U"2")
	SENTENCE (coefficients_string, U"Coefficients (c[k])", U"1.2 2.0 1.2 1.2 3.0 0.0")
	SENTENCE (knots_string, U"Interior knots" , U"0.3 0.5 0.6")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be smaller than Xmax.");
	CREATE_ONE
		autoMSpline result = MSpline_createFromStrings (xmin, xmax, degree, coefficients_string, knots_string);
	CREATE_ONE_END (name)
}

DIRECT (HELP_MSpline_help) {
	HELP (U"MSpline")
}

DIRECT (HELP_NMF_help) {
	HELP (U"NMF")
}

DIRECT (NEW_NMF_to_Matrix) {
	CONVERT_EACH (NMF)
		autoMatrix result = NMF_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}
/********************** PatternList *******************************************/

DIRECT (NEW1_PatternList_Categories_to_Discriminant) {
	CONVERT_TWO (PatternList, Categories)
		autoDiscriminant result = PatternList_Categories_to_Discriminant (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (GRAPHICS_PatternList_draw, U"PatternList: Draw", nullptr) {
	NATURAL (patternNumber, U"Pattern number", U"1")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PatternList)
		PatternList_draw (me, GRAPHICS, patternNumber, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_PatternList_getNumberOfPatterns) {
	INTEGER_ONE (PatternList)
		integer result = my ny;
	INTEGER_ONE_END (U" (number of patterns)")
}

DIRECT (INTEGER_PatternList_getPatternSize) {
	INTEGER_ONE (PatternList)
		integer result = my nx;
	INTEGER_ONE_END (U" (pattern size)")
}

FORM (REAL_PatternList_getValue, U"", nullptr) {
	NATURAL (patternNumber, U"Pattern number", U"1")
	NATURAL (nodeNumber, U"Node number", U"2")
	OK
DO
	NUMBER_ONE (PatternList)
		double result = ( patternNumber <= my ny && nodeNumber <= my nx ? my z [patternNumber] [nodeNumber] : undefined );
	NUMBER_ONE_END (U"")
}

DIRECT (NUMMAT_PatternList_getAllValues) {
	NUMMAT_ONE (PatternList)
		autoMAT result = newMATcopy (my z.all());
	NUMMAT_ONE_END
}

FORM (MODIFY_PatternList_formula, U"PatternList: Formula", nullptr) {
	LABEL (U"# `col` is the node number, `row` is the pattern number")
	LABEL (U"for row from 1 to nrow   ; for all patterns")
	LABEL (U"   for col from 1 to ncol   ; for all nodes")
	LABEL (U"      self [row, col] =")
	TEXTFIELD (formula, nullptr, U"5 * exp (-0.5 * ((col - 10 - row/100) / 1.5) ^ 2) - 0.5   ; sliding peak")
	LABEL (U"   endfor")
	LABEL (U"endfor")
	OK
DO
	MODIFY_EACH (PatternList)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_END
}

FORM (MODIFY_PatternList_setValue, U"PatternList: Set value", U"PatternList: Set value...") {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (newValue, U"New value", U"0.0")
	OK
DO
	MODIFY_EACH (PatternList)
		Melder_require (rowNumber <= my ny, U"The row number should not be greater than the number of rows.");
		Melder_require (columnNumber <= my nx, U"The column number should not be greater than the number of columns.");
		my z [rowNumber] [columnNumber] = newValue;
	MODIFY_EACH_END
}

DIRECT (NEW_PatternList_to_Matrix) {
	CONVERT_EACH (PatternList)
		autoMatrix result = PatternList_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}


/******************* PCA ******************************/

DIRECT (HELP_PCA_help) {
	HELP (U"PCA")
}

DIRECT (HINT_hint_PCA_TableOfReal_to_Configuration) {
	Melder_information (U"You can get principal components by selecting a PCA and a TableOfReal\n"
		"together and choosing \"To Configuration...\".");
END }

DIRECT (HINT_hint_PCA_Covariance_Project) {
	Melder_information (U"You can get a new Covariance object rotated to the directions of the direction vectors\n"
		" in the PCA object by selecting a PCA and a Covariance object together.");
END }

DIRECT (HINT_hint_PCA_Configuration_to_TableOfReal_reconstruct) {
	Melder_information (U"You can reconstruct the original TableOfReal as well as possible from\n"
		" the principal components in the Configuration and the direction vectors in the PCA object.");
END }

FORM (REAL_PCA_TableOfReal_getFractionVariance, U"PCA & TableOfReal: Get fraction variance", U"PCA & TableOfReal: Get fraction variance...") {
	NATURAL (fromPrincipalComponent, U"left Principal component range", U"1")
	NATURAL (toPrincipalComponent, U"right Principal component range", U"1")
	OK
DO
	NUMBER_TWO (PCA, TableOfReal)
		double result = PCA_TableOfReal_getFractionVariance (me, you, fromPrincipalComponent, toPrincipalComponent);
	NUMBER_TWO_END (U"")
}

DIRECT (NEW_PCA_Configuration_to_TableOfReal_reconstruct) {
	CONVERT_TWO (PCA, Configuration)
		autoTableOfReal result = PCA_Configuration_to_TableOfReal_reconstruct (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_PCA_TableOfReal_to_TableOfReal_projectRows, U"PCA & TableOfReal: To TableOfReal (project rows)", U"PCA & TableOfReal: To Configuration...") {
	INTEGER (numberOfDimensionsToKeep, U"Number of dimensions to keep", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensionsToKeep >= 0, U"The number of dimensions to keep should be at least zero.");
	CONVERT_ONE_AND_GENERIC (PCA, TableOfReal)
		autoTableOfReal result = PCA_TableOfReal_to_TableOfReal_projectRows (me, you, numberOfDimensionsToKeep);
	CONVERT_ONE_AND_GENERIC_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_PCA_TableOfReal_to_Configuration, U"PCA & TableOfReal: To Configuration", U"PCA & TableOfReal: To Configuration...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_GENERIC (PCA, TableOfReal)
		autoConfiguration result = PCA_TableOfReal_to_Configuration (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_PCA_TableOfReal_to_TableOfReal_zscores, U"PCA & TableOfReal: To TableOfReal (z-scores)", U"PCA & TableOfReal: To TableOfReal (z-scores)...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_GENERIC (PCA, TableOfReal)
		autoTableOfReal result = PCA_TableOfReal_to_TableOfReal_zscores (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name.get(), U"_", your name.get(), U"_zscores")
}

FORM (NEW1_PCA_Matrix_to_Matrix_projectRows, U"PCA & Matrix: To Matrix (project rows)", nullptr) {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"The number of dimensions should be at least zero.");
	CONVERT_TWO (PCA, Matrix)
		autoMatrix result = Eigen_Matrix_to_Matrix_projectRows (me, you, numberOfDimensions);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_PCA_Matrix_to_Matrix_projectColumns, U"PCA & Matrix: To Matrix (project columns)", nullptr) {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_GENERIC (PCA, Matrix)
		autoMatrix result = Eigen_Matrix_to_Matrix_projectColumns (me, you, numberOfDimensions);
	CONVERT_ONE_AND_GENERIC_END (my name.get(), U"_", your name.get())
}

FORM (REAL_PCA_getCentroidElement, U"PCA: Get centroid element...", nullptr) {
	NATURAL (number, U"Number", U"1")
	OK
DO
	NUMBER_ONE (PCA)
		Melder_require (number <= my dimension, U"Number should not be greater than ", my dimension, U".");
		double result = my centroid [number];
	NUMBER_ONE_END (U" (element ", number, U")")
}

FORM (REAL_PCA_getEqualityOfEigenvalues, U"PCA: Get equality of eigenvalues", U"PCA: Get equality of eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	BOOLEAN (conservativeTest, U"Conservative test", false)
	OK
DO
	NUMBER_ONE (PCA)
		double result, chisq, df;
		PCA_getEqualityOfEigenvalues (me, fromEigenvalue, toEigenvalue,
				conservativeTest, & result, & chisq, & df);
	NUMBER_ONE_END (U" (= probability, based on chisq = ", chisq, U" and df = ", df)
}

FORM (INTEGER_PCA_getNumberOfComponents_VAF, U"PCA: Get number of components (VAF)", U"PCA: Get number of components (VAF)...") {
	POSITIVE (varianceFraction, U"Variance fraction (0-1)", U"0.95")
	OK
DO
	Melder_require (varianceFraction >= 0.0 && varianceFraction <= 1.0, U"The variance fraction should be in interval [0-1].");
	NUMBER_ONE (PCA)
		double result = Eigen_getDimensionOfFraction (me, varianceFraction);
	NUMBER_ONE_END (U" (variance fraction)")
}

FORM (REAL_PCA_getFractionVAF, U"PCA: Get fraction variance accounted for", U"PCA: Get fraction variance accounted for...") {
	NATURAL (fromPrincipalComponent, U"left Principal component range", U"1")
	NATURAL (toPrincipalComponent, U"right Principal component range", U"1")
	OK
DO
	Melder_require (fromPrincipalComponent <= toPrincipalComponent, U"The second component should be greater than or equal to the first component.");
	NUMBER_ONE (PCA)
		double result = Eigen_getCumulativeContributionOfComponents (me, fromPrincipalComponent, toPrincipalComponent);
	NUMBER_ONE_END (U"")
}

FORM (MODIFY_PCA_invertEigenvector, U"PCA: Invert eigenvector", nullptr) {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Eigen)
		Eigen_invertEigenvector (me, eigenvectorNumber);
	MODIFY_EACH_END
}

FORM (NEW_PCA_extractEigenvector, U"PCA: Extract eigenvector", U"Eigen: Extract eigenvector...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	LABEL (U"Reshape as")
	INTEGER (numberOfRows, U"Number of rows", U"0")
	INTEGER (numberOfColumns, U"Number of columns", U"0")
	OK
DO
	Melder_require (numberOfRows >= 0, U"The number of rows should be at least 0.");
	Melder_require (numberOfColumns >= 0, U"The number of columns should be at least 0.");
	CONVERT_EACH (PCA);
		autoMatrix result = Eigen_extractEigenvector (me, eigenvectorNumber, numberOfRows, numberOfColumns);
	CONVERT_EACH_END (my name.get(), U"_ev", eigenvectorNumber)
}

DIRECT (NEW_PCA_extractEigen) {
	CONVERT_EACH (PCA)
		autoEigen result = PCA_to_Eigen (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_PCA_to_TableOfReal_reconstruct1, U"PCA: To TableOfReal (reconstruct)", U"PCA: To TableOfReal (reconstruct 1)...") {
	SENTENCE (coefficients_string, U"Coefficients", U"1.0 1.0")
	OK
DO
	CONVERT_EACH (PCA)
		autoTableOfReal result = PCA_to_TableOfReal_reconstruct1 (me, coefficients_string);
	CONVERT_EACH_END (my name.get(), U"_reconstructed")
}

FORM (NEW1_PCAs_to_Procrustes, U"PCA & PCA: To Procrustes", U"PCA & PCA: To Procrustes...") {
	NATURAL (fromEigenvector, U"left Eigenvector range", U"1")
	NATURAL (toEigenvector, U"right Eigenvector range", U"2")
	OK
DO
	CONVERT_COUPLE (PCA)
		autoProcrustes result = Eigens_to_Procrustes (me, you, fromEigenvector, toEigenvector);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}


DIRECT (REAL_PCAs_getAngleBetweenPc1Pc2Plane_degrees) {
	NUMBER_COUPLE (PCA)
		Melder_require (my numberOfEigenvalues > 1, U"There must be at least two eigenvectors in the first PCA.");
		Melder_require (your numberOfEigenvalues > 1, U"There must be at least two eigenvectors in the second PCA.");
		double result = Eigens_getAngleBetweenEigenplanes_degrees (me, you);
	NUMBER_COUPLE_END (U" degrees (= angle of intersection between the two pc1-pc2 eigenplanes)")
}

DIRECT (NEW1_PCA_SSCP_project) {
	CONVERT_TWO (PCA, SSCP)
		autoSSCP result = Eigen_SSCP_project (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

/******************* Permutation **************************************/

DIRECT (HELP_Permutation_help) {
	HELP (U"Permutation")
}

FORM (NEW_Permutation_create, U"Create Permutation", U"Create Permutation...") {
	WORD (name, U"Name", U"p")
	NATURAL (numberOfElements, U"Number of elements", U"10")
	BOOLEAN (identity, U"Identity Permutation", true)
	OK
DO
	CREATE_ONE
		autoPermutation result = Permutation_create (numberOfElements);
		if (! identity) {
			Permutation_permuteRandomly_inplace (result.get(), 0, 0);
		}
	CREATE_ONE_END (name)
}

DIRECT (INTEGER_Permutation_getNumberOfElements) {
	INTEGER_ONE (Permutation)
		integer result = my numberOfElements;
	INTEGER_ONE_END (U" (number of elements)")
}

FORM (INTEGER_Permutation_getValueAtIndex, U"Permutation: Get value", U"Permutation: Get value...") {
	NATURAL (index, U"Index", U"1")
	OK
DO
	INTEGER_ONE (Permutation)
		integer result = Permutation_getValueAtIndex (me, index);
	INTEGER_ONE_END (U" (value, at index = ", index, U")")
}

FORM (INTEGER_Permutation_getIndexAtValue, U"Permutation: Get index", U"Permutation: Get index...") {
	NATURAL (value, U"Value", U"1")
	OK
DO
	INTEGER_ONE (Permutation)
		integer result = Permutation_getIndexAtValue (me, value);
	INTEGER_ONE_END (U" (index, at value = ", value, U")")
}

FORM (MODIFY_Permutation_tableJump, U"Permutation: Table jump", U"Permutation: Table jump...") {
	NATURAL (jumpSize, U"Jump size", U"4")
	NATURAL (first, U"First", U"1")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_tableJump_inline (me, jumpSize, first);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Permutation_sort) {
	MODIFY_EACH (Permutation)
		Permutation_sort (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapBlocks, U"Permutation: Swap blocks", U"Permutation: Swap blocks...") {
	NATURAL (fromIndex, U"From index", U"1")
	NATURAL (toIndex, U"To index", U"2")
	NATURAL (blockSize, U"Block size", U"1")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapBlocks (me, fromIndex, toIndex, blockSize);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapPositions, U"Permutation: Swap positions", U"Permutation: Swap positions...") {
	NATURAL (firstIndex, U"First index", U"1")
	NATURAL (secondIndex, U"Second index", U"2")
	OK
DO
	LOOP {
		iam (Permutation);
		Permutation_swapPositions (me, firstIndex, secondIndex);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Permutation_swapNumbers, U"Permutation: Swap numbers", U"Permutation: Swap numbers...") {
	NATURAL (firstNumber, U"First number", U"1")
	NATURAL (secondNumber, U"Second number", U"2")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapNumbers (me, firstNumber, secondNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapOneFromRange, U"Permutation: Swap one from range", U"Permutation: Swap one from range...") {
	LABEL (U"A randomly chosen element from ")
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	LABEL (U"is swapped with the element at")
	NATURAL (index, U"Index", U"1")
	BOOLEAN (forbidSame, U"Forbid same", true)
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapOneFromRange (me, fromIndex, toIndex, index, forbidSame);
	MODIFY_EACH_END
}

FORM (NEW_Permutation_permuteRandomly, U"Permutation: Permute randomly", U"Permutation: Permute randomly...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_permuteRandomly (me, fromIndex, toIndex);
	CONVERT_EACH_END (my name.get(), U"_rdm")
}

FORM (NEW_Permutation_rotate, U"Permutation: Rotate", U"Permutation: Rotate...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	INTEGER (step, U"Step size", U"1")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_rotate (me, fromIndex, toIndex, step);
	CONVERT_EACH_END (my name.get(), U"_rot", step)
}

FORM (NEW_Permutation_reverse, U"Permutation: Reverse", U"Permutation: Reverse...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_reverse (me, fromIndex, toIndex);
	CONVERT_EACH_END (my name.get(), U"_rev")
}

FORM (NEW_Permutation_permuteBlocksRandomly, U"Permutation: Permute blocks randomly", U"Permutation: Permute randomly (blocks)...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	NATURAL (blockSize, U"Block size", U"12")
	BOOLEAN (permuteWithinBlocks, U"Permute within blocks", true)
	BOOLEAN (noDoublets, U"No doublets", false)
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_permuteBlocksRandomly (me, fromIndex, toIndex, blockSize, permuteWithinBlocks, noDoublets);
	CONVERT_EACH_END (my name.get(), U"_pbr", blockSize)
}

FORM (NEW_Permutation_interleave, U"Permutation: Interleave", U"Permutation: Interleave...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	NATURAL (blockSize, U"Block size", U"12")
	INTEGER (offset, U"Offset", U"0")
	OK
DO
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_interleave (me, fromIndex, toIndex, blockSize, offset);
	CONVERT_EACH_END (my name.get(), U"_itl")
}

DIRECT (NEW_Permutation_invert) {
	CONVERT_EACH (Permutation)
		autoPermutation result = Permutation_invert (me);
	CONVERT_EACH_END (my name.get(), U"_inv")
}

DIRECT (NEW1_Permutations_multiply) {
	CONVERT_LIST (Permutation)
		autoPermutation result = Permutations_multiply (& list);
	CONVERT_LIST_END (U"mul_", list.size);
}

DIRECT (MODIFY_Permutations_next) {
	MODIFY_EACH (Permutation)
		Permutation_next_inplace (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Permutations_previous) {
	MODIFY_EACH (Permutation)
		Permutation_previous_inplace (me);
	MODIFY_EACH_END
}

FORM (NEW1_Pitches_to_DTW, U"Pitches: To DTW", U"Pitches: To DTW...") {
	REAL (vuvCosts, U"Voiced-unvoiced costs", U"24.0")
	REAL (weight, U"Time costs weight", U"10.0")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_COUPLE (Pitch)
		autoDTW result = Pitches_to_DTW (me, you, vuvCosts, weight, matchStart, matchEnd, slopeConstraint);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW_PitchTier_to_Pitch, U"PitchTier: To Pitch", U"PitchTier: To Pitch...") {
	POSITIVE (stepSize, U"Step size", U"0.02")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"400.0")
	OK
DO
	CONVERT_EACH (PitchTier)
		autoPitch result = PitchTier_to_Pitch (me, stepSize, pitchFloor, pitchCeiling);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_PitchTier_modifyInterval, U"PitchTier: Modify interval", U"PitchTier: Modify interval...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	LABEL (U"")
	SENTENCE (timesString, U"Relative times", U"0.0 0.5 1.0")
	OPTIONMENU (timeOffset, U"...are...", 1)
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"independent")
	LABEL (U"...of the interval duration which will be added...")
	LABEL (U"...to the start time of the interval.")
	SENTENCE (pitches_string, U"The \"pitch\" values", U"100 200 100")
	OPTIONMENU (pitch_as, U"...are...", 1)
		OPTION (U"frequencies")
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"start and slopes")
		OPTION (U"slopes and end")
		OPTION (U"music notes")
//		OPTION (U"semitones")
	LABEL (U"...to be added to the anchor value (if used)...")
	OPTIONMENU (pitch_is, U"...which is the...", 1)
		OPTION (U"not used")
		OPTION (U"current")
		OPTION (U"start")
		OPTION (U"end")
		OPTION (U"mean of the curve")
		OPTION (U"mean of the points")
		OPTION (U"maximum")
		OPTION (U"minimum")
	LABEL (U"...frequency value in the interval.")
	LABEL (U"")
	OPTIONMENU (pitch_unit, U"Pitch frequency unit", 1)
		OPTION (U"Hertz")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_modifyInterval (me, fromTime, toTime, timesString, timeOffset, pitches_string, pitch_unit, pitch_as, pitch_is);
	MODIFY_EACH_END
}


FORM (MODIFY_PitchTier_modifyInterval_toneLevels, U"PitchTier: Modify interval (tone levels)", U"PitchTier: Modify interval (tone levels)...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	REAL (fmin, U"left Pitch range (Hz)", U"80.0")
	REAL (fmax, U"right Pitch range", U"200.0")
	NATURAL (numberOfToneLevels, U"Number of tone levels", U"5")
	LABEL (U"")
	SENTENCE (times_string, U"Relative times", U"0.0 0.5 1.0")
	OPTIONMENU (time_offset, U"...are...", 1)
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"independent")
	LABEL (U"...of the interval duration which will be added...")
	LABEL (U"...to the start time of the interval.")
	SENTENCE (pitches_string, U"Tone levels", U"2.1 2.1 5.0")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_modifyInterval_toneLevels (me, fromTime, toTime, fmin, fmax, numberOfToneLevels, times_string, time_offset, pitches_string);
	MODIFY_EACH_END
}

/******************* Polygon & Categories *************************************/

FORM (NEW1_Polygon_createSimple, U"Create simple Polygon", U"Create simple Polygon...") {
	WORD (name, U"Name", U"p")
	SENTENCE (vertices_string, U"Vertices as X-Y pairs", U"0.0 0.0  0.0 1.0  1.0 0.0")
	OK
DO
	CREATE_ONE
		autoPolygon result = Polygon_createSimple (vertices_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_Polygon_createFromRandomPoints, U"Polygon: Create from random points", nullptr) {
	WORD (name, U"Name", U"p")
	NATURAL (numberOfPoints, U"Number of points", U"10")
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"1.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"1.0")
	OK
DO
	CREATE_ONE
		autoPolygon result = Polygon_createFromRandomPoints (numberOfPoints, xmin, xmax, ymin, ymax);
	CREATE_ONE_END (name)
}

DIRECT (INTEGER_Polygon_getNumberOfPoints) {
	INTEGER_ONE (Polygon)
		integer result = my numberOfPoints;
	INTEGER_ONE_END (U" (number of points)")
}

FORM (REAL_Polygon_getPointX, U"Polygon: Get point (x)", nullptr) {
	NATURAL (pointNumber, U"Point number", U"1")
	OK
DO
	NUMBER_ONE (Polygon)
		double result = ( pointNumber <= my numberOfPoints ? my x [pointNumber] : undefined );
	NUMBER_ONE_END (U" (x [", pointNumber, U"])")
}

FORM (REAL_Polygon_getPointY, U"Polygon: Get point (y)", nullptr) {
	NATURAL (pointNumber, U"Point number", U"1")
	OK
DO
	NUMBER_ONE (Polygon)
		double result = ( pointNumber <= my numberOfPoints ? my y [pointNumber] : undefined );
	NUMBER_ONE_END (U" (y [", pointNumber, U"])")
}

FORM (INFO_Polygon_getLocationOfPoint, U"Get location of point", U"Polygon: Get location of point...") {
	LABEL (U"Point is (I)n, (O)ut, (E)dge or (V)ertex?")
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	REAL (eps, U"Precision", U"1.64e-15")
	OK
DO
	Melder_require (eps >= 0.0, U"The precision cannot be negative.");
	STRING_ONE (Polygon)
		int loc = Polygon_getLocationOfPoint (me, x, y, eps);
		conststring32 result = ( loc == Polygon_INSIDE ? U"I" : loc == Polygon_OUTSIDE ? U"O" :
			loc == Polygon_EDGE ? U"E" : U"V" );
	STRING_ONE_END
}

DIRECT (REAL_Polygon_getAreaOfConvexHull) {
	NUMBER_ONE (Polygon)
		double result = Polygon_getAreaOfConvexHull (me);
	NUMBER_ONE_END (U" (area convex hull)")
}

FORM (NEW_Polygon_circularPermutation, U"Polygon: Circular permutation", nullptr) {
	INTEGER (shift, U"Shift", U"1")
	OK
DO
	CONVERT_EACH (Polygon)
		autoPolygon result = Polygon_circularPermutation (me, shift);
	CONVERT_EACH_END (my name.get(), U"_", shift)
}


DIRECT (NEW_Polygon_simplify) {
	CONVERT_EACH (Polygon)
		autoPolygon result = Polygon_simplify (me);
	CONVERT_EACH_END (my name.get(), U"_s")
}

DIRECT (NEW_Polygon_convexHull) {
	CONVERT_EACH (Polygon)
		autoPolygon result = Polygon_convexHull (me);
	CONVERT_EACH_END (my name.get(), U"_hull")
}

FORM (MODIFY_Polygon_translate, U"Polygon: Translate", U"Polygon: Translate...") {
	REAL (xDistance, U"X distance", U"0.0")
	REAL (yDistance, U"Y distance", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_translate (me, xDistance, yDistance);
	MODIFY_EACH_END
}

FORM (MODIFY_Polygon_rotate, U"Polygon: Rotate", U"Polygon: Rotate...") {
	LABEL (U"Rotate counterclockwise over the")
	REAL (angle_degrees, U"Angle (degrees)", U"0.0")
	LABEL (U"With respect to the point")
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_rotate (me, angle_degrees, x, y);
	MODIFY_EACH_END
}

FORM (MODIFY_Polygon_scale, U"Polygon: Scale polygon", nullptr) {
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_scale (me, x, y);
	MODIFY_EACH_END
}

FORM (GRAPHICS_Polygon_Categories_draw, U"Polygon & Categories: Draw", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
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
	WORD (name, U"Name", U"p")
	LABEL (U"Domain of polynomial")
	REAL (xmin, U"Xmin", U"-3.0")
	REAL (xmax, U"Xmax", U"4.0")
	LABEL (U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	SENTENCE (coefficients_string, U"Coefficients", U"2.0 -1.0 -2.0 1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_Polynomial_createFromProductTerms, U"Create Polynomial from second order products", nullptr) {
	WORD (name, U"Name", U"p")
	LABEL (U"Domain of polynomial")
	REAL (xmin, U"Xmin", U"-2.0")
	REAL (xmax, U"Xmax", U"2.0")
	LABEL (U"(1+a[1]*x+x^2)*(1+a[2]*x+x^2)*...*(1+a[n]*x+x^2)")
	SENTENCE (coefficients_string, U"The a's", U"1.0 2.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromProductOfSecondOrderTermsString (xmin, xmax, coefficients_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_Polynomial_createFromRealZeros, U"Create Polynomial from first order products", nullptr) {
	WORD (name, U"Name", U"p")
	LABEL (U"Domain of polynomial")
	REAL (xmin, U"Xmin", U"-3.0")
	REAL (xmax, U"Xmax", U"3.0")
	LABEL (U"(P(x) = (x-zero[1])*(1-zero[2])*...*(x-zero[n])")
	SENTENCE (zeros_string, U"The zero's", U"1.0 2.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromRealRootsString (xmin, xmax, zeros_string);
	CREATE_ONE_END (name)
}

FORM (MODIFY_Polynomial_divide_secondOrderFactor, U"Polynomial: Divide second order factor", nullptr) {
	LABEL (U"P(x) / (x^2 - factor)")
	REAL (factor, U"Factor", U"1.0")
	OK
DO
	MODIFY_EACH (Polynomial)
		Polynomial_divide_secondOrderFactor (me, factor);
	MODIFY_EACH_END
}

FORM (REAL_Polynomial_getArea, U"Polynomial: Get area", U"Polynomial: Get area...") {
	LABEL (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	NUMBER_ONE (Polynomial)
		double result = Polynomial_getArea (me, xmin, xmax);
	NUMBER_ONE_END (U" (area)")
}

FORM (REAL_Polynomial_getRemainderAfterDivision, U"Polynomial: Get remainder after division", nullptr) {
	LABEL (U"P(x) / (x - factor)")
	REAL (factor, U"Monomial factor", U"1.0")
	OK
DO
	NUMBER_ONE (Polynomial)
		double result;
		autoPolynomial p = Data_copy (me);
		Polynomial_divide_firstOrderFactor (p.get(), factor, & result);
	NUMBER_ONE_END (U" (remainder)")
}

FORM (INFO_Polynomial_getDerivativesAtX, U"Polynomial: Get derivatives at X", nullptr) {
	REAL (x, U"X", U"0.5")
	INTEGER (numberOfDerivatives, U"Number of derivatives", U"2")
	OK
DO
	INFO_ONE (Polynomial)
		autoVEC derivatives = Polynomial_evaluateDerivatives (me, x, numberOfDerivatives);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Function value: ", derivatives [1]);
			for (integer i = 2; i <= numberOfDerivatives + 1; i ++) {
				MelderInfo_writeLine (U"Derivative ", i - 1, U": ", i < my numberOfCoefficients ? derivatives [i] : undefined, U"");
			}
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (REAL_Polynomial_getOneRealRoot, U"Polynomial: Get one real root", nullptr) {
	LABEL (U"Interval: ")
	REAL (xmin, U"left X Range", U"-1.0")
	REAL (xmax, U"right X Range", U"1.0")
	OK
DO
	NUMBER_ONE (Polynomial)
		double result = Polynomial_findOneSimpleRealRoot_nr (me, xmin, xmax);
	NUMBER_ONE_END (U" (root)")
}

DIRECT (NEW_Polynomial_getDerivative) {
	CONVERT_EACH (Polynomial)
		autoPolynomial result = Polynomial_getDerivative (me);
	CONVERT_EACH_END (my name.get(), U"_derivative")
}

FORM (NEW_Polynomial_getPrimitive, U"Polynomial: Get primitive", nullptr) {
	REAL (constant, U"Constant", U"0.0")
	OK
DO
	CONVERT_EACH (Polynomial)
		autoPolynomial result = Polynomial_getPrimitive (me,constant);
	CONVERT_EACH_END (my name.get(), U"_primitive")
}

FORM (NEW_Polynomial_scaleX, U"Polynomial: Scale x", U"Polynomial: Scale x...") {
	LABEL (U"New domain")
	REAL (xmin, U"Xmin", U"-1.0")
	REAL (xmax, U"Xmax", U"1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CONVERT_EACH (Polynomial)
		autoPolynomial result = Polynomial_scaleX (me, xmin, xmax);
	CONVERT_EACH_END (my name.get(), U"_scaleX")
}

DIRECT (MODIFY_Polynomial_scaleCoefficients_monic) {
	MODIFY_EACH (Polynomial)
		Polynomial_scaleCoefficients_monic (me);
	MODIFY_EACH_END
}

DIRECT (NEW_Polynomial_to_Roots) {
	CONVERT_EACH (Polynomial)
		autoRoots result = Polynomial_to_Roots (me);
	CONVERT_EACH_END (my name.get())
}

FORM (INFO_Polynomial_evaluate_z, U"Polynomial: Get value (complex)", U"Polynomial: Get value (complex)...") {
	REAL (x, U"Real part", U"0.0")
	REAL (y, U"Imaginary part", U"0.0")
	OK
DO
	dcomplex z { x, y };
	INFO_ONE (Polynomial)
		dcomplex result = Polynomial_evaluate_z (me, z);
		MelderInfo_open ();
		MelderInfo_writeLine (result);
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (NEW_Polynomial_to_Spectrum, U"Polynomial: To Spectrum", U"Polynomial: To Spectrum...") {
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	NATURAL (numberOfFrequencies, U"Number of frequencies (>1)", U"1025")
	OK
DO
	CONVERT_EACH (Polynomial)
		autoSpectrum result = Polynomial_to_Spectrum (me, maximumFrequency, numberOfFrequencies, 1.0);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Polynomials_multiply) {
	CONVERT_COUPLE (Polynomial)
		autoPolynomial result = Polynomials_multiply (me, you);
	CONVERT_COUPLE_END (my name.get(), U"_x_", your name.get())
}

FORM (NEWMANY_Polynomials_divide, U"Polynomials: Divide", U"Polynomials: Divide...") {
	BOOLEAN (wantQuotient, U"Want quotient", true)
	BOOLEAN (wantRemainder, U"Want remainder", true)
	OK
DO
	Melder_require (wantQuotient || wantRemainder, U"You should select \"Want quotient\", \"Want remainder\", or both.");
	FIND_COUPLE (Polynomial)
		autoPolynomial aq, ar;
		Polynomials_divide (me, you, wantQuotient ? & aq : nullptr, wantRemainder ? & ar : nullptr);
		if (wantQuotient) {
			praat_new (aq.move(), my name.get(), U"_q");
		}
		if (wantRemainder) {
			praat_new (ar.move(), my name.get(), U"_r");
		}
	END
}

/********************* Roots ******************************/

DIRECT (HELP_Roots_help) {
	HELP (U"Roots")
}

FORM (GRAPHICS_Roots_draw, U"Roots: Draw", nullptr) {
	REAL (xmin, U"Minimum of real axis", U"0.0")
	REAL (xmax, U"Maximum of real axis", U"0.0")
	REAL (ymin, U"Minimum of imaginary axis", U"0.0")
	REAL (ymax, U"Maximum of imaginary axis", U"0.0")
	SENTENCE (mark_string, U"Mark string (+x0...)", U"o")
	POSITIVE (markSize, U"Mark size", U"12")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (Roots)
		Roots_draw (me, GRAPHICS, xmin, xmax, ymin, ymax, mark_string, markSize, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_Roots_getNumberOfRoots) {
	INTEGER_ONE (Roots)
		integer result = Roots_getNumberOfRoots (me);
	INTEGER_ONE_END (U"")
}

FORM (COMPLEX_Roots_getRoot, U"Roots: Get root", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	OK
DO
	NUMBER_ONE (Roots)
		dcomplex result = Roots_getRoot (me, rootNumber);
	NUMBER_ONE_END (U"")
}

FORM (REAL_Roots_getRealPartOfRoot, U"Roots: Get real part", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	OK
DO
	NUMBER_ONE (Roots)
		dcomplex z = Roots_getRoot (me, rootNumber);
		double result = z.re;
	NUMBER_ONE_END (U"")
}

FORM (REAL_Roots_getImaginaryPartOfRoot, U"Roots: Get imaginary part", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	OK
DO
	NUMBER_ONE (Roots)
		dcomplex z = Roots_getRoot (me, rootNumber);
		double result = z.im;
	NUMBER_ONE_END (U" i")
}

FORM (MODIFY_Roots_setRoot, U"Roots: Set root", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	REAL (x, U"Real part", U"1.0/sqrt(2)")
	REAL (y, U"Imaginary part", U"1.0/sqrt(2)")
	OK
DO
	MODIFY_EACH (Roots)
		Roots_setRoot (me, rootNumber, x, y);
	MODIFY_EACH_END
}

FORM (NEW_Roots_to_Spectrum, U"Roots: To Spectrum", U"Roots: To Spectrum...") {
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	NATURAL (numberOfFrequencies, U"Number of frequencies (>1)", U"1025")
	OK
DO
	CONVERT_EACH (Roots)
		autoSpectrum result = Roots_to_Spectrum (me, maximumFrequency, numberOfFrequencies, 1.0);
	CONVERT_EACH_END (my name.get())
}

DIRECT (MODIFY_Roots_Polynomial_polish) {
	MODIFY_FIRST_OF_TWO (Roots, Polynomial)
		Roots_Polynomial_polish (me, you);
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
	POSITIVE (criticalValue, U"Critical value", U"2.0")
	NATURAL (numberOfMeans, U"Number of means", U"3")
	POSITIVE (degreesOfFreedon, U"Degrees of freedom", U"10.0")
	NATURAL (numberOfRows, U"Number of rows", U"1")
	OK
DO
	double result = NUMtukeyQ (criticalValue, numberOfMeans, degreesOfFreedon, numberOfRows);
	Melder_information (result, U" (tukeyQ)");
END }

FORM (REAL_Praat_getInvTukeyQ, U"Get invTukeyQ", nullptr) {
	REAL (probability, U"Probability", U"0.05")
	NATURAL (numberOfMeans, U"Number of means", U"3")
	POSITIVE (degreesOfFreedon, U"Degrees of freedom", U"10.0")
	NATURAL (numberOfRows, U"Number of rows", U"1")
	OK
DO
	Melder_require (probability >= 0.0 && probability <= 1.0, U"The probability should be in the interval [0, 1].");
	double result = NUMinvTukeyQ (probability, numberOfMeans, degreesOfFreedon, numberOfRows);
	Melder_information (result, U" (inv tukeyQ)");
END }

FORM (COMPLEX_Praat_getIncompleteGamma, U"Get incomplete gamma", U"Get incomplete gamma...") {
	POSITIVE (reAlpha, U"Real part of alpha", U"4.0")
	REAL (imAlpha, U"Imaginary part of alpha", U"0.0")
	REAL (reX, U"Real part of X", U"4.0")
	REAL (imX, U"Imaginary part of X", U"0.0")
	OK
DO
	dcomplex result = NUMincompleteGammaFunction (reAlpha, imAlpha, reX, imX);
	Melder_information (result);
END }

/******************** Sound ****************************************/

#define Sound_create_addCommonFields(startTime,endTime,samplingFrequency) \
	REAL (startTime, U"Start time (s)", U"0.0") \
	REAL (endTime, U"End time (s)", U"1.0") \
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")


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
		Melder_throw (U"A Sound cannot have ", Melder_bigInteger ((integer) numberOfSamples_real), U" samples; the maximum is ", Melder_bigInteger (LONG_MAX), U" samples.\n");
	}
}

FORM (NEW1_Sound_Pitch_to_FormantFilter, U"Sound & Pitch: To FormantFilter", U"Sound & Pitch: To Spectrogram...") {
	POSITIVE (windowLength, U"Analysis window duration (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoFormantFilter result = Sound_Pitch_to_FormantFilter (me, you, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Sound_Pitch_to_Spectrogram, U"Sound & Pitch: To Spectrogram", U"Sound & Pitch: To Spectrogram...") {
	POSITIVE (windowLength, U"Analysis window duration (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoSpectrogram result = Sound_Pitch_to_Spectrogram (me, you, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Sound_Pitch_changeGender, U"Sound & Pitch: Change gender", U"Sound & Pitch: Change gender...") {
	POSITIVE (formantShiftRatio, U"Formant shift ratio", U"1.2")
	REAL (newPitchMedian, U"New pitch median (Hz)", U"0.0 (= no change)")
	POSITIVE (pitchRangeFactor, U"Pitch range factor", U"1.0 (= no change)")
	POSITIVE (durationFactor, U"Duration factor", U"1.0")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoSound result = Sound_Pitch_changeGender_old (me, you, formantShiftRatio, newPitchMedian, pitchRangeFactor, durationFactor);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Sound_Pitch_changeSpeaker, U"Sound & Pitch: Change speaker", U"Sound & Pitch: Change speaker...") {
	POSITIVE (formantFrequencyMultiplicationFactor, U"Multiply formants by", U"1.1 (male->female)")
	POSITIVE (pitchMultiplicationFactor, U"Multiply pitch by", U"1.8 (male->female")
	REAL (pitchRangeMultiplicationFactor, U"Multiply pitch range by", U"1.0 (= no change)")
	POSITIVE (durationMultiplicationFactor, U"Multiply duration by", U"1.0")
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoSound result = Sound_Pitch_changeSpeaker (me, you, formantFrequencyMultiplicationFactor, pitchMultiplicationFactor, pitchRangeMultiplicationFactor, durationMultiplicationFactor);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Sound_IntervalTier_cutPartsMatchingLabel, U"Sound & IntervalTier: Cut parts matching label", nullptr) {
	SENTENCE (label, U"Label", U"cut")
	OK
DO
	CONVERT_TWO (Sound, IntervalTier)
		autoSound result = Sound_IntervalTier_cutPartsMatchingLabel (me, you, label);
	CONVERT_TWO_END (my name.get(), U"_cut")
}

FORM (NEW1_Sound_createAsGammaTone, U"Create a gammatone", U"Create Sound as gammatone...") {
	WORD (name, U"Name", U"gammatone")
	Sound_create_addCommonFields (startTime, endTime, samplingFrequency)
	INTEGER (gamma, U"Gamma", U"4")
	POSITIVE (frequency, U"Frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"150.0")
	REAL (initialPhase, U"Initial phase (radians)", U"0.0")
	REAL (additionFactor, U"Addition factor", U"0.0")
	BOOLEAN (scaleAmplitudes, U"Scale amplitudes", true)
	OK
DO
	CREATE_ONE
		Sound_create_checkCommonFields (startTime, endTime, samplingFrequency);
		Melder_require (frequency < 0.5 * samplingFrequency,
			U"Your frequency should not be greater than half the sampling frequency. Use a frequency less than ", 0.5 * samplingFrequency, U".");
		Melder_require (gamma >= 0, U"Gamma should not be negative. Use a positive or zero gamma.");
		autoSound result = Sound_createGammaTone (startTime, endTime, samplingFrequency, gamma, frequency, bandwidth, initialPhase, additionFactor, scaleAmplitudes);
	CREATE_ONE_END (name)
}

FORM (NEW_Sound_createAsShepardTone, U"Create a Shepard tone", U"Create Sound as Shepard tone...") {
	WORD (name, U"Name", U"shepardTone")
	Sound_create_addCommonFields (startTime, endTime, samplingFrequency)
	POSITIVE (minimumFrequency, U"Lowest frequency (Hz)", U"4.863")
	NATURAL (numberOfComponents, U"Number of components", U"10")
	REAL (frequencyChange, U"Frequency change (semitones/s)", U"4.0")
	POSITIVE (amplitudeRange_dB, U"Amplitude range (dB)", U"30.0")
	REAL (octaveShiftFraction, U"Octave shift fraction ([0,1))", U"0.0")
	OK
DO
	CREATE_ONE
		Sound_create_checkCommonFields (startTime, endTime, samplingFrequency);
		autoSound result = Sound_createShepardToneComplex (startTime, endTime, samplingFrequency, minimumFrequency, numberOfComponents, frequencyChange, amplitudeRange_dB, octaveShiftFraction);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_Sound_drawWhere, U"Sound: Draw where", U"Sound: Draw where...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	LABEL (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
		OPTION (U"Curve")
		OPTION (U"Bars")
		OPTION (U"Poles")
		OPTION (U"Speckles")
	TEXTFIELD (formula, U"Draw only those parts where the following condition holds:", U"x < xmin + (xmax - xmin) / 2; first half")
	OK
DO
	integer numberOfBisections = 10;
	GRAPHICS_EACH (Sound)
		Sound_drawWhere (me, GRAPHICS, fromTime, toTime, ymin, ymax, garnish, drawingMethod, numberOfBisections, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (PLAY_Sound_playOneChannel, U"Sound: Play one channel", nullptr) {
    NATURAL (channel, U"Channel", U"1")
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
	REAL (frequencyShift, U"Shift by (Hz)", U"1000.0")
	POSITIVE (samplingFrequency, U"New sampling frequency (Hz)", U"44100.0")
	NATURAL (samplePrecision, U"Precision (samples)", U"50")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_playAsFrequencyShifted (me, frequencyShift, samplingFrequency, samplePrecision);
	}
END }

FORM (REAL_Sound_getNearestLevelCrossing, U"Sound: Get nearest level crossing", U"Sound: Get nearest level crossing...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	REAL (time, U"Time (s)", U"0.1")
	REAL (level, U"Level", U"0.1")
	OPTIONMENU_ENUM (kSoundSearchDirection, searchDirection, U"Search direction", kSoundSearchDirection::DEFAULT)
	OK
DO
	NUMBER_ONE (Sound)
		if (channel > my ny) channel = 1;
		double result = Sound_getNearestLevelCrossing (me, channel, time, level, searchDirection);
	NUMBER_ONE_END (U" seconds")
}

FORM (NEW1_Sounds_to_DTW, U"Sounds: To DTW", nullptr) {
    POSITIVE (windowLength, U"Window length (s)", U"0.015")
    POSITIVE (timeStep, U"Time step (s)", U"0.005")
    LABEL (U"")
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.1")
    RADIO (slopeConstraint, U"Slope constraint", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_COUPLE (Sound)
		autoDTW result = Sounds_to_DTW (me, you, windowLength, timeStep, sakoeChibaBand, slopeConstraint);
   CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW_Sound_to_TextGrid_detectSilences, U"Sound: To TextGrid (silences)", U"Sound: To TextGrid (silences)...") {
	LABEL (U"Parameters for the intensity analysis")
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"100")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	LABEL (U"Silent intervals detection")
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
	WORD (silenceLabel, U"Silent interval label", U"silent")
	WORD (soundingLabel, U"Sounding interval label", U"sounding")
	OK
DO
	CONVERT_EACH (Sound)
		autoTextGrid result = Sound_to_TextGrid_detectSilences (me, minimumPitch, timeStep, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_copyChannelRanges, U"Sound: Copy channel ranges", nullptr) {
	TEXTFIELD (channels, U"Create a new Sound from the following channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_copyChannelRanges (me, channels);
	CONVERT_EACH_END (my name.get(), U"_channels")
}

FORM (NEWMANY_Sound_trimSilences, U"Sound: Trim silences", U"Sound: Trim silences...") {
    REAL (trimDuration, U"Trim duration (s)", U"0.08")
	BOOLEAN (onlyAtStartAndEnd, U"Only at start and end", true);
	LABEL (U"Parameters for the intensity analysis")
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"100")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	LABEL (U"Silent intervals detection")
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-35.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
    BOOLEAN (saveTextGrid, U"Save trimming info as TextGrid", false)
    WORD (trim_string, U"Trim label", U"trimmed")
	OK
DO
    trimDuration = ( trimDuration < 0.0 ? 0.0 : trimDuration );
	CONVERT_EACH (Sound)
        autoTextGrid tg;
		autoSound result = Sound_trimSilences (me, trimDuration, onlyAtStartAndEnd, minimumPitch, timeStep, silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, (saveTextGrid ? &tg : nullptr ), trim_string);
		if (saveTextGrid) {
            praat_new (tg.move(), my name.get(), U"_trimmed");
        }
	CONVERT_EACH_END (my name.get(), U"_trimmed")
}

// deprecated
FORM (NEW_Sound_to_BarkFilter, U"Sound: To BarkFilter", U"Sound: To BarkSpectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (bark)", U"1.0")
	POSITIVE (deltaFrequency, U"Distance between filters (bark)", U"1.0")
	REAL (maximumFrequency, U"Maximum frequency (bark)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoBarkFilter result = Sound_to_BarkFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_BarkSpectrogram, U"Sound: To BarkSpectrogram", U"Sound: To BarkSpectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (bark)", U"1.0")
	POSITIVE (deltaFrequency, U"Distance between filters (bark)", U"1.0")
	REAL (maximumFrequency, U"Maximum frequency (bark)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoBarkSpectrogram result = Sound_to_BarkSpectrogram (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name.get())
}

// deprecated
FORM (NEW_Sound_to_FormantFilter, U"Sound: To FormantFilter", U"Sound: To FormantFilter...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	LABEL (U"Pitch analysis")
	REAL (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	REAL (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormantFilter result = Sound_to_FormantFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth, minimumPitch, maximumPitch);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Spectrogram_pitchDependent, U"Sound: To Spectrogram (pitch-dependent)", U"Sound: To Spectrogram (pitch-dependent)...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	LABEL (U"Pitch analysis")
	REAL (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	REAL (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSpectrogram result = Sound_to_Spectrogram_pitchDependent (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth, minimumPitch, maximumPitch);
	CONVERT_EACH_END (my name.get())
}

// deprecated
FORM (NEW_Sound_to_MelFilter, U"Sound: To MelFilter", U"Sound: To MelFilter...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (mel)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (mel)", U"100.0")
	REAL (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoMelFilter result = Sound_to_MelFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_MelSpectrogram, U"Sound: To MelSpectrogram", U"Sound: To MelSpectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (mel)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (mel)", U"100.0")
	REAL (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	CONVERT_EACH (Sound)
		autoMelSpectrogram result = Sound_to_MelSpectrogram (me, windowLength, timeStep,  firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_ComplexSpectrogram, U"Sound: To ComplexSpectrogram", nullptr) {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step", U"0.005")
	OK
DO
	CONVERT_EACH (Sound)
		autoComplexSpectrogram result = Sound_to_ComplexSpectrogram (me, windowLength, timeStep);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Pitch_shs, U"Sound: To Pitch (shs)", U"Sound: To Pitch (shs)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (pitchFloor, U"Minimum pitch (Hz)", U"50.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates (Hz)", U"15")
	LABEL (U"Algorithm parameters")
	POSITIVE (maximumFrequency, U"Maximum frequency component (Hz)", U"1250.0")
	NATURAL (maximumNumberOfSubharmonics, U"Max. number of subharmonics", U"15")
	POSITIVE (compressionFactor, U"Compression factor (<=1)", U"0.84")
	POSITIVE (pitchCeiling, U"Ceiling (Hz)", U"600.0")
	NATURAL (numberOfPointsPerOctave, U"Number of points per octave", U"48");
	OK
DO
	Melder_require (pitchFloor < pitchCeiling, U"The minimum pitch should be less than the ceiling.");
	Melder_require (pitchCeiling < maximumFrequency, U"The maximum frequency should be greater than or equal to the ceiling.");
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch_shs (me, timeStep, pitchFloor, maximumFrequency, pitchCeiling, maximumNumberOfSubharmonics, maximumNumberOfCandidates, compressionFactor, numberOfPointsPerOctave);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_Sound_fadeIn, U"Sound: Fade in", U"Sound: Fade in...") {
	CHANNEL (channel, U"Channel (number; 0 = all)", U"1")
	REAL (time, U"Time (s)", U"-10000.0")
	REAL (fadeTime, U"Fade time (s)", U"0.005")
	BOOLEAN (silentFromStart, U"Silent from start", false)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_fade (me, channel, time, fadeTime, -1.0, silentFromStart);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_fadeOut, U"Sound: Fade out", U"Sound: Fade out...") {
	CHANNEL (channel, U"Channel (number; 0 = all)", U"1")
	REAL (time, U"Time (s)", U"10000.0")
	REAL (fadeTime, U"Fade time (s)", U"-0.005")
	BOOLEAN (silentToEnd, U"Silent to end", false)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_fade (me, channel, time, fadeTime, 1, silentToEnd);
	MODIFY_EACH_END
}

FORM (NEW_Sound_to_KlattGrid_simple, U"Sound: To KlattGrid (simple)", U"Sound: To KlattGrid (simple)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Formant determination")
	NATURAL (numberOfFormants, U"Max. number of formants", U"5")
	POSITIVE (maximumFormant, U"Maximum formant (Hz)", U"5500 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	LABEL (U"Pitch determination")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"Intensity determination")
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"100.0")
	BOOLEAN (subtractMean, U"Subtract mean", true)
	OK
DO
	CONVERT_EACH (Sound)
		autoKlattGrid result = Sound_to_KlattGrid_simple (me, timeStep, numberOfFormants, maximumFormant, windowLength, preEmphasisFrequency, pitchFloor, pitchCeiling, minimumPitch, subtractMean);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Pitch_SPINET, U"Sound: To SPINET", U"Sound: To SPINET...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (windowLength, U"Window length (s)", U"0.040")
	LABEL (U"Gammatone filter bank")
	POSITIVE (minimumFrequency, U"Minimum filter frequency (Hz)", U"70.0")
	POSITIVE (maximumFrequency, U"Maximum filter frequency (Hz)", U"5000.0")
	NATURAL (numberOfFilters, U"Number of filters", U"250");
	POSITIVE (pitchCeiling, U"Ceiling (Hz)", U"500.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	OK
DO
	Melder_require (minimumFrequency < maximumFrequency, U"The maximum frequency should be greater than the minimum frequency.");
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch_SPINET (me, timeStep, windowLength, minimumFrequency, maximumFrequency, numberOfFilters, pitchCeiling, maximumNumberOfCandidates);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Polygon, U"Sound: To Polygon", U"Sound: To Polygon...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (connectionY, U"Connection y-value", U"0.0")
	OK
DO
	CONVERT_EACH (Sound)
		channel = channel > my ny ? 1 : channel;
		autoPolygon result = Sound_to_Polygon (me, channel, fromTime, toTime, ymin, ymax, connectionY);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_Sounds_to_Polygon_enclosed, U"Sounds: To Polygon (enclosed)", U"Sounds: To Polygon (enclosed)...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	CONVERT_COUPLE (Sound)
		autoPolygon result = Sounds_to_Polygon_enclosed (me, you, channel, fromTime, toTime, ymin, ymax);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW_Sound_filterByGammaToneFilter4, U"Sound: Filter (gammatone)", U"Sound: Filter (gammatone)...") {
	POSITIVE (centreFrequency, U"Centre frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"150.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filterByGammaToneFilter4 (me, centreFrequency, bandwidth);
	CONVERT_EACH_END (my name.get(), U"_filtered")
}

FORM (NEW_Sound_reduceNoise, U"Sound: Reduce noise", U"Sound: Reduce noise...") {
	REAL (fromTime, U"left Noise time range (s)", U"0.0")
	REAL (toTime, U"right Noise time range (s)", U"0.0")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	LABEL (U"Filter")
	REAL (fromFrequency, U"left Filter frequency range (Hz)", U"80.0")
	REAL (toFrequency, U"right Filter frequency range (Hz)", U"10000.0")
	POSITIVE (smoothingBandwidth, U"Smoothing bandwidth, (Hz)", U"40.0")
	REAL (noiseReduction_dB, U"Noise reduction (dB)", U"-20.0")
	OPTIONMENU_ENUM (kSoundNoiseReductionMethod, noiseReductionMethod, U"Noise reduction method", kSoundNoiseReductionMethod::DEFAULT)
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_reduceNoise (me, fromTime, toTime, windowLength, fromFrequency, toFrequency, smoothingBandwidth, noiseReduction_dB, noiseReductionMethod);
	CONVERT_EACH_END (my name.get(), U"_denoised")
}

FORM (NEW_Sound_removeNoise, U"Sound: Remove noise", U"Sound: Reduce noise...") {
	REAL (fromTime, U"left Noise time range (s)", U"0.0")
	REAL (toTime, U"right Noise time range (s)", U"0.0")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	LABEL (U"Filter")
	REAL (fromFrequency, U"left Filter frequency range (Hz)", U"80.0")
	REAL (toFrequency, U"right Filter frequency range (Hz)", U"10000.0")
	POSITIVE (smoothingBandwidth, U"Smoothing bandwidth, (Hz)", U"40.0")
	OPTIONMENU_ENUM (kSoundNoiseReductionMethod, noiseReductionMethod, U"Noise reduction method", kSoundNoiseReductionMethod::DEFAULT)
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_removeNoise (me, fromTime, toTime, windowLength, fromFrequency, toFrequency, smoothingBandwidth, noiseReductionMethod);
	CONVERT_EACH_END (my name.get(), U"_denoised")
}

FORM (NEW_Sound_changeSpeaker, U"Sound: Change speaker", U"Sound: Change speaker...") {
	LABEL (U"Pitch measurement parameters")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"Modification parameters")
	POSITIVE (formantMultiplicationFactor, U"Multiply formants by", U"1.2")
	POSITIVE (pitchMultiplicationFactor, U"Multiply pitch by", U"1.0")
	REAL (pitchRangeMultiplicationFactor, U"Multiply pitch range by", U"1.0 (= no change)")
	POSITIVE (durationMultiplicationFactor, U"Multiply duration by", U"1.0")
	OK
DO
	Melder_require (pitchFloor < pitchCeiling, U"The maximum pitch should be greater than the minimum pitch.");
	CONVERT_EACH (Sound)
		autoSound result = Sound_changeSpeaker (me, pitchFloor, pitchCeiling, formantMultiplicationFactor, pitchMultiplicationFactor, pitchRangeMultiplicationFactor, durationMultiplicationFactor);
	CONVERT_EACH_END (my name.get(), U"_changeSpeaker")
}

FORM (NEW_Sound_changeGender, U"Sound: Change gender", U"Sound: Change gender...") {
	LABEL (U"Pitch measurement parameters")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	LABEL (U"Modification parameters")
	POSITIVE (formantShiftRatio, U"Formant shift ratio", U"1.2")
	REAL (pitchMedian, U"New pitch median (Hz)", U"0.0 (= no change)")
	REAL (pitchRangeMultiplicationFactor, U"Pitch range factor", U"1.0 (= no change)")
	POSITIVE (durationMultiplicationFactor, U"Duration factor", U"1.0")
	OK
DO
	Melder_require (pitchFloor < pitchCeiling, U"The maximum pitch should be greater than the minimum pitch.");
	CONVERT_EACH (Sound)
		autoSound result = Sound_changeGender_old (me, pitchFloor, pitchCeiling, formantShiftRatio, pitchMedian, pitchRangeMultiplicationFactor, durationMultiplicationFactor);
	CONVERT_EACH_END (my name.get(), U"_changeGender");
}

FORM (GRAPHICS_Sound_paintWhere, U"Sound paint where", U"Sound: Paint where...") {
	COLOUR (colour, U"Colour (0-1, name, or {r,g,b})", U"0.5")
	praat_TimeFunction_RANGE(fromTime,toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (level, U"Fill from level", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Paint only those parts where the following condition holds:", U"1; always")
	OK
DO
	integer numberOfBisections = 10;
	GRAPHICS_EACH (Sound)
		Sound_paintWhere (me, GRAPHICS, colour, fromTime, toTime, ymin, ymax, level, garnish, numberOfBisections, formula,
			interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Sounds_paintEnclosed, U"Sounds paint enclosed", U"Sounds: Paint enclosed...") {
	COLOUR (colour, U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (xFromTime, U"left Horizontal time range (s)", U"0.0")
	REAL (xToTime, U"right Horizontal time range (s)", U"0.0")
	REAL (yFromTime, U"left Vertical time range", U"0.0")
	REAL (yToTime, U"right Vertical time range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
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
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

/**************** Spectrum *******************************************/

FORM (GRAPHICS_Spectrum_drawPhases, U"Spectrum: Draw phases", U"Spectrum: Draw phases...") {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0")
	REAL (fromPhase, U"Minimum phase (dB/Hz)", U"0.0 (= auto)")
	REAL (toPhase, U"Maximum phase (dB/Hz)", U"0.0 (= auto)")
	BOOLEAN (unwrap, U"Unwrap", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_drawPhases (me, GRAPHICS, fromFrequency, toFrequency, fromPhase, toPhase, unwrap, garnish);
	GRAPHICS_EACH_END
}

FORM (MODIFY_Spectrum_setRealValueInBin, U"Spectrum: Set real value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		Melder_require (binNumber <= my nx, U"Your bin number should not exceed the number of bins (", my nx, U").");
		my z[1][binNumber]= value;
	MODIFY_EACH_END
}

FORM (MODIFY_Spectrum_setImaginaryValueInBin, U"Spectrum: Set imaginary value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		Melder_require (binNumber <= my nx, U"Your bin number should not exceed the number of bins (", my nx, U").");
		my z[2][binNumber]= value;
	MODIFY_EACH_END
}

DIRECT (MODIFY_Spectrum_conjugate) {
	MODIFY_EACH (Spectrum)
		Spectrum_conjugate (me);
	MODIFY_EACH_END
}

FORM (NEW_Spectrum_shiftFrequencies, U"Spectrum: Shift frequencies", U"Spectrum: Shift frequencies...") {
	REAL (frequencyShift, U"Shift by (Hz)", U"1000.0")
	POSITIVE (maximumFrequency, U"New maximum frequency (Hz)", U"22050")
	NATURAL (interpolationDepth, U"Interpolation depth", U"50")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_shiftFrequencies (me, frequencyShift, maximumFrequency, interpolationDepth);
	CONVERT_EACH_END (my name.get(), ( frequencyShift < 0.0 ? U"_m" : U"_" ), Melder_ifloor (fabs (frequencyShift)))
}

DIRECT (NEW_Spectra_multiply) {
	CONVERT_COUPLE (Spectrum)
		autoSpectrum result = Spectra_multiply (me, you);
	CONVERT_COUPLE_END (my name.get(), U"_x_", your name.get())
}

FORM (NEW_Spectrum_resample, U"Spectrum: Resample", nullptr) {
	NATURAL (numberOfFrequencies, U"New number of frequencies", U"256")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_resample (me, numberOfFrequencies);
	CONVERT_EACH_END (my name.get(), U"_", numberOfFrequencies)
}

FORM (NEW_Spectrum_compressFrequencyDomain, U"Spectrum: Compress frequency domain", nullptr) {
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	INTEGER (interpolationDepth, U"Interpolation depth", U"50")
	RADIO (scale, U"Interpolation scale", 1)
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Logarithmic")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_compressFrequencyDomain (me, maximumFrequency, interpolationDepth, scale, 1);
	CONVERT_EACH_END (my name.get(), U"_", Melder_ifloor (maximumFrequency))
}

DIRECT (NEW_Spectrum_unwrap) {
	CONVERT_EACH (Spectrum)
		autoMatrix result = Spectrum_unwrap (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Spectrum_to_PowerCepstrum) {
	CONVERT_EACH (Spectrum)
		autoPowerCepstrum result = Spectrum_to_PowerCepstrum (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Spectrum_to_Cepstrum) {
	CONVERT_EACH (Spectrum)
		autoCepstrum result = Spectrum_to_Cepstrum (me);
	CONVERT_EACH_END (my name.get())
}

/************* SpeechSynthesizer *************************************************/

DIRECT (HELP_SpeechSynthesizer_help) {
	HELP (U"SpeechSynthesizer")
}

FORM (NEW1_ExtractEspeakData, U"SpeechSynthesizer: Extract espeak data", nullptr) {
	OPTIONMENU (which, U"Data", 1)
		OPTION (U"Language properties")
		OPTION (U"Voices properties")
	OK
DO
	CREATE_ONE
		autoTable result;
		conststring32 name = U"languages";
		if (which == 1) {
			result = Data_copy (espeakdata_languages_propertiesTable.get());
		} else if (which == 2) {
			result = Data_copy (espeakdata_voices_propertiesTable.get());
			name = U"voices";
		}
	CREATE_ONE_END (name)
}

FORM (NEW1_SpeechSynthesizer_create, U"Create SpeechSynthesizer", U"Create SpeechSynthesizer...") {
	OPTIONMENUSTR (language_string, U"Language", (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)"))
	for (integer i = 1; i <= espeakdata_languages_names -> numberOfStrings; i ++) {
		OPTION (espeakdata_languages_names -> strings [i].get());
	}
	OPTIONMENUSTR (voice_string, U"Voice variant", (int) Strings_findString (espeakdata_voices_names.get(), U"Female1"))
	for (integer i = 1; i <= espeakdata_voices_names -> numberOfStrings; i ++) {
		OPTION (espeakdata_voices_names -> strings [i].get());
	}
	OK
DO
	CREATE_ONE
		int languageIndex, voiceIndex;
		espeakdata_getIndices (language_string, voice_string, & languageIndex, & voiceIndex);
		autoSpeechSynthesizer result = SpeechSynthesizer_create (
			espeakdata_languages_names -> strings [languageIndex].get(),
			espeakdata_voices_names -> strings [voiceIndex].get()
		);
    CREATE_ONE_END (espeakdata_languages_names -> strings [languageIndex].get(), U"_", espeakdata_voices_names -> strings [voiceIndex].get())
}

FORM (MODIFY_SpeechSynthesizer_modifyPhonemeSet, U"SpeechSynthesizer: Modify phoneme set", nullptr) {
	OPTIONMENU (phoneneSetIndex, U"Language", (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)"))
	for (integer i = 1; i <= espeakdata_languages_names -> numberOfStrings; i ++) {
			OPTION (espeakdata_languages_names -> strings [i].get());
	}
	OK
/*	Does not work because me is not defined here.
	int prefPhonemeSet = (int) Strings_findString (espeakdata_languages_names.get(), my d_phonemeSet);
	if (prefPhonemeSet == 0) {
		prefVoice = (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)");
	}
	SET_OPTION (phoneneSetIndex, prefPhonemeSet)*/
DO
	MODIFY_EACH (SpeechSynthesizer)
		my d_phonemeSet = Melder_dup (espeakdata_languages_names -> strings [phoneneSetIndex].get());
	MODIFY_EACH_END
}

FORM (PLAY_SpeechSynthesizer_playText, U"SpeechSynthesizer: Play text", U"SpeechSynthesizer: Play text...") {
	TEXTFIELD (text, U"Text:", U"This is some text.")
	OK
DO
	MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::INTERRUPTABLE);
	LOOP {
		iam (SpeechSynthesizer);
		SpeechSynthesizer_playText (me, text);
	}
	MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::ASYNCHRONOUS);

END }

FORM (NEWMANY_SpeechSynthesizer_to_Sound, U"SpeechSynthesizer: To Sound", U"SpeechSynthesizer: To Sound...") {
	TEXTFIELD (text, U"Text:", U"This is some text.")
	BOOLEAN (wantTextGrid, U"Create TextGrid with annotations", false);
	OK
DO
	CONVERT_EACH (SpeechSynthesizer)
		autoTextGrid tg;
		autoTable t;
		autoSound result = SpeechSynthesizer_to_Sound (me, text, (wantTextGrid ? & tg : nullptr), (Melder_debug == -2 ? & t : nullptr ));
		if (wantTextGrid) {
			praat_new (tg.move(), my name.get());
		}
		if (Melder_debug == -2) {
			praat_new (t.move(), my name.get());
		}
	CONVERT_EACH_END (my name.get())
}

DIRECT (INFO_SpeechSynthesizer_getLanguageName) {
	STRING_ONE (SpeechSynthesizer)
		conststring32 result = my d_languageName.get();
	STRING_ONE_END
}

DIRECT (INFO_SpeechSynthesizer_getVoiceName) {
	STRING_ONE (SpeechSynthesizer)
		conststring32 result = my d_voiceName.get();
	STRING_ONE_END
}

DIRECT (INFO_SpeechSynthesizer_getPhonemeSetName) {
	STRING_ONE (SpeechSynthesizer)
		conststring32 result = my d_phonemeSet.get();
	STRING_ONE_END
}

FORM (MODIFY_SpeechSynthesizer_setTextInputSettings, U"SpeechSynthesizer: Set text input settings", U"SpeechSynthesizer: Set text input settings...") {
	OPTIONMENU (inputTextFormat, U"Input text format is", 1)
		OPTION (U"Text only")
		OPTION (U"Phoneme codes only")
		OPTION (U"Mixed with tags")
	OPTIONMENU (inputPhonemeCoding, U"Input phoneme codes are", 1)
		OPTION (U"Kirshenbaum_espeak")
	OK
DO
	int inputPhonemeCoding_always = SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM;
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setTextInputSettings (me, inputTextFormat, inputPhonemeCoding_always);
	MODIFY_EACH_END
}

FORM (MODIFY_SpeechSynthesizer_estimateSpeechRateFromSpeech, U"SpeechSynthesizer: Estimate speech rate from speech", U"SpeechSynthesizer: Estimate speech rate from speech...") {
	BOOLEAN (estimate, U"Estimate speech rate from speech", true);
	OK
DO
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setEstimateSpeechRateFromSpeech (me, estimate);
	MODIFY_EACH_END
}

FORM (MODIFY_SpeechSynthesizer_speechOutputSettings, U"SpeechSynthesizer: Set speech output settings", U"SpeechSynthesizer: Set speech output settings...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	REAL (wordGap, U"Gap between words (s)", U"0.01")
	POSITIVE (pitchAdjustment, U"Pitch multiplier (0.5-2.0)", U"1.0")
	REAL (pitchRange, U"Pitch range multiplier (0-2.0)", U"1.0");
	POSITIVE (wordsPerMinute, U"Words per minute (80-450)", U"175.0");
	OPTIONMENU (outputPhonemeCodes, U"Output phoneme codes are", 2)
		OPTION (U"Kirshenbaum_espeak")
		OPTION (U"IPA")
	OK
DO
	if (wordGap < 0.0) wordGap = 0.0;
	Melder_require (pitchAdjustment >= 0.5 && pitchAdjustment <= 2.0, U"The pitch adjustment should be between 0.5 and 2.0.");
	Melder_require (pitchRange >= 0.0 && pitchRange <= 2.0, U"The pitch range multiplier should be between 0.0 and 2.0.");
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setSpeechOutputSettings (me, samplingFrequency, wordGap, pitchAdjustment, pitchRange, wordsPerMinute, outputPhonemeCodes);
	MODIFY_EACH_END
}

FORM (MODIFY_SpeechSynthesizer_setSpeechOutputSettings, U"SpeechSynthesizer: Set speech output settings", U"SpeechSynthesizer: Set speech output settings...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	REAL (wordGap, U"Gap between words (s)", U"0.01")
	INTEGER (pitchAdjustment_0_99, U"Pitch adjustment (0-99)", U"50")
	INTEGER (pitchRange_0_99, U"Pitch range (0-99)", U"50");
	NATURAL (wordsPerMinute, U"Words per minute (80-450)", U"175");
	BOOLEAN (estimateWordsPerMinute, U"Estimate rate from data", true);
	OPTIONMENU (outputPhonemeCodes, U"Output phoneme codes are", 2)
		OPTION (U"Kirshenbaum_espeak")
		OPTION (U"IPA")
	OK
DO
	if (wordGap < 0) wordGap = 0;
	if (pitchAdjustment_0_99 < 0) pitchAdjustment_0_99 = 0;
	if (pitchAdjustment_0_99 > 99) pitchAdjustment_0_99 = 99;
	if (pitchRange_0_99 < 0) pitchRange_0_99 = 0;
	if (pitchRange_0_99 > 99) pitchRange_0_99 = 99;
	double pitchAdjustment = (1.5/99.0 * pitchAdjustment_0_99 + 0.5);
	double pitchRange = (pitchRange_0_99 / 49.5);
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setSpeechOutputSettings (me, samplingFrequency, wordGap, pitchAdjustment, pitchRange, wordsPerMinute,  outputPhonemeCodes);
		SpeechSynthesizer_setEstimateSpeechRateFromSpeech (me, estimateWordsPerMinute);
	MODIFY_EACH_END
}

/************* SpeechSynthesizer and TextGrid ************************/

FORM (NEWMANY_SpeechSynthesizer_TextGrid_to_Sound, U"SpeechSynthesizer & TextGrid: To Sound", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	NATURAL (intervalNumber, U"Interval number", U"1")
	BOOLEAN (createAnnotations, U"Create TextGrid with annotations", false);
	OK
DO
	CONVERT_TWO (SpeechSynthesizer, TextGrid)
		autoTextGrid annotations;
		autoSound result = SpeechSynthesizer_TextGrid_to_Sound (me, you, tierNumber, intervalNumber, (createAnnotations ? & annotations : nullptr ));
		if (createAnnotations) {
			praat_new (annotations.move(), my name.get());
		}
	CONVERT_TWO_END (my name.get())
}

FORM (NEW1_SpeechSynthesizer_Sound_TextGrid_align, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align)", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	NATURAL (fromInterval, U"From interval number", U"1")
	NATURAL (toInterval, U"To interval number", U"1")
	REAL (silenceThreshold_dB, U"Silence threshold (dB)", U"-35.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
	OK
DO
	CONVERT_THREE (SpeechSynthesizer, Sound, TextGrid)
		autoTextGrid result = SpeechSynthesizer_Sound_TextGrid_align (me, you, him, tierNumber, fromInterval, toInterval, silenceThreshold_dB, minimumSilenceDuration, minimumSoundingDuration);
	CONVERT_THREE_END (your name.get(), U"_aligned")
}

FORM (NEW1_SpeechSynthesizer_Sound_TextGrid_align2, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align, trim)", nullptr) {
    NATURAL (tierNumber, U"Tier number", U"1")
    NATURAL (fromInterval, U"From interval number", U"1")
    NATURAL (toInterval, U"To interval number", U"1")
    REAL (silenceThreshold_dB, U"Silence threshold (dB)", U"-35.0")
    POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
    POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
    REAL (trimDuration, U"Silence trim duration (s)", U"0.08")
    OK
DO
   trimDuration = trimDuration < 0.0 ? 0.0 : trimDuration;
    CONVERT_THREE (SpeechSynthesizer, Sound, TextGrid)
		autoTextGrid result = SpeechSynthesizer_Sound_TextGrid_align2 (me, you, him, tierNumber, fromInterval, toInterval, silenceThreshold_dB, minimumSilenceDuration, minimumSoundingDuration, trimDuration);
    CONVERT_THREE_END (his name.get(), U"_aligned")
}

/************* Spline *************************************************/

FORM (GRAPHICS_Spline_drawKnots, U"Spline: Draw knots", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spline)
		Spline_drawKnots (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_Spline_getOrder) {
	INTEGER_ONE (Spline)
		integer result = Spline_getOrder (me);
	INTEGER_ONE_END (U" (order)")
}

FORM (NEW_Spline_scaleX, U"Spline: Scale x", U"Spline: Scale x...") {
	LABEL (U"New domain")
	REAL (xmin, U"Xmin", U"-1.0")
	REAL (xmax, U"Xmax", U"1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CONVERT_EACH (Spline)
		autoSpline result = Spline_scaleX (me, xmin, xmax);
	CONVERT_EACH_END (my name.get(), U"_scaleX")
}

/************ SSCP ***************************************************/

DIRECT (HELP_SSCP_help) {
	HELP (U"SSCP")
}

FORM (GRAPHICS_SSCP_drawConfidenceEllipse, U"SSCP: Draw confidence ellipse", nullptr) {
	POSITIVE (confidenceLevel, U"Confidence level", U"0.95")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SSCP)
		SSCP_drawConcentrationEllipse (me, GRAPHICS, confidenceLevel, 1, xIndex, yIndex, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_SSCP_drawSigmaEllipse, U"SSCP: Draw sigma ellipse", U"SSCP: Draw sigma ellipse...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SSCP)
		SSCP_drawConcentrationEllipse (me, GRAPHICS, numberOfSigmas, 0, xIndex, yIndex, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_SSCP_extractCentroid) {
	CONVERT_EACH (SSCP)
		autoTableOfReal result = SSCP_extractCentroid (me);
	CONVERT_EACH_END (my name.get(), U"_centroid")
}

FORM (REAL_SSCP_getConfidenceEllipseArea, U"SSCP: Get confidence ellipse area", U"SSCP: Get confidence ellipse area...") {
	POSITIVE (confidenceLevel, U"Confidence level", U"0.95")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = SSCP_getConcentrationEllipseArea (me, confidenceLevel, 1, xIndex, yIndex);
	NUMBER_ONE_END (U" (confidence ellipse area)")
}

FORM (REAL_SSCP_getFractionVariation, U"SSCP: Get fraction variation", U"SSCP: Get fraction variation...") {
	NATURAL (fromDimension, U"From dimension", U"1")
	NATURAL (toDimension, U"To dimension", U"1")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = SSCP_getFractionVariation (me, fromDimension, toDimension);
	NUMBER_ONE_END (U" (fraction)")
}


FORM (REAL_SSCP_getConcentrationEllipseArea, U"SSCP: Get sigma ellipse area", U"SSCP: Get sigma ellipse area...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	OK
DO
	NUMBER_ONE (SSCP)
		double result = SSCP_getConcentrationEllipseArea (me, numberOfSigmas, 0, xIndex, yIndex);
	NUMBER_ONE_END (U" (concentation ellipse area)")
}

DIRECT (NUMBER_SSCP_getDegreesOfFreedom) {
	NUMBER_ONE (SSCP)
		double result = SSCP_getDegreesOfFreedom (me);
	NUMBER_ONE_END (U" (degrees of freedom)")
}

DIRECT (INTEGER_SSCP_getNumberOfObservations) {
	INTEGER_ONE (SSCP)
		integer result = Melder_ifloor (my numberOfObservations);   // ppgb: blijf ik raar vinden
	INTEGER_ONE_END (U" (number of observations)")
}

DIRECT (REAL_SSCP_getTotalVariance) {
	NUMBER_ONE (SSCP)
		double result = SSCP_getTotalVariance (me);
	NUMBER_ONE_END (U" (total variance)")
}

FORM (REAL_SSCP_getCentroidElement, U"SSCP: Get centroid element", U"SSCP: Get centroid element") {
	NATURAL (number, U"Number", U"1")
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

FORM (REAL_SSCP_getDiagonality_bartlett, U"SSCP: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...") {
	NATURAL (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	NUMBER_ONE (SSCP)
		double chisq, result, df;
		SSCP_getDiagonality_bartlett (me, numberOfConstraints, & chisq, & result, & df);
	NUMBER_ONE_END (U" (= probability for chisq = ", chisq, U" and ndf = ", df, U")")
}

DIRECT (NEW_SSCP_to_Correlation) {
	CONVERT_EACH (SSCP)
		autoCorrelation result = SSCP_to_Correlation (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_SSCP_to_Covariance, U"SSCP: To Covariance", U"SSCP: To Covariance...") {
	NATURAL (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	CONVERT_EACH (SSCP)
		autoCovariance result = SSCP_to_Covariance (me, numberOfConstraints);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_SSCP_to_PCA) {
	CONVERT_EACH (SSCP)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_END (my name.get())
}

/******************* Strings ****************************/

//DIRECT (NEW1_Strings_createAsEspeakVoices) {
//	//praat_new (nullptr, U"voices"); // TODO ??
//END }

FORM (NEW1_Strings_createAsCharacters, U"Strings: Create as characters", nullptr) {
	SENTENCE (text, U"Text", U"intention")
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsCharacters (text);
	CREATE_ONE_END (U"chars")
}

FORM (NEW1_old_Strings_createAsTokens, U"Strings: Create as tokens", nullptr) {
	TEXTFIELD (text, U"Text:", U"There are seven tokens in this text")
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsTokens (text, U" ");
	CREATE_ONE_END (U"tokens")
}

FORM (NEW1_Strings_createAsTokens, U"Strings: Create as tokens", U"Create Strings as tokens...") {
	TEXTFIELD (text, U"Text:", U"There are seven tokens in this text")
	SENTENCE (separators, U"Separators", U" ,")
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
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Strings_change, U"Strings: Change", U"Strings: Change") {
	SENTENCE (search_string, U"Search", U"a")
	SENTENCE (replace_string, U"Replace", U"a")
	INTEGER (replaceLimit, U"Replace limit", U"0 (= unlimited)")
	RADIO (stringType, U"Search and replace are:", 1)
	RADIOBUTTON (U"Literals")
	RADIOBUTTON (U"Regular Expressions")
	OK
DO
	CONVERT_EACH (Strings)
		integer nmatches, nstringmatches;
		autoStrings result = Strings_change (me, search_string, replace_string, replaceLimit, &nmatches, &nstringmatches, stringType - 1);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Strings_extractPart, U"Strings: Extract part", nullptr) {
	NATURAL (fromIndex, U"From index", U"1")
	NATURAL (toIndex, U"To index", U"1")
	OK
DO
	CONVERT_EACH (Strings)
		autoStrings result = Strings_extractPart (me, fromIndex, toIndex);
	CONVERT_EACH_END (my name.get(), U"_part");
}

DIRECT (NEW_Strings_to_EditDistanceTable) {
	CONVERT_COUPLE (Strings)
		autoEditDistanceTable result = EditDistanceTable_create (me, you);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW_Strings_to_Permutation, U"Strings: To Permutation", U"Strings: To Permutation...") {
	BOOLEAN (sort, U"Sort", true)
	OK
DO
	CONVERT_EACH (Strings)
		autoPermutation result = Strings_to_Permutation (me, sort);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_Strings_Permutation_permuteStrings) {
	CONVERT_TWO (Strings, Permutation)
		autoStrings result = Strings_Permutation_permuteStrings (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

/****************************** SVD *****************************************/

DIRECT (HELP_SVD_help) {
	HELP (U"singular value decomposition")
}

DIRECT (INTEGER_SVD_getNumberOfRows) {
	INTEGER_ONE (SVD)
		integer result = my isTransposed ? my numberOfColumns : my numberOfRows;
	INTEGER_ONE_END (U" (number of rows)")	
}

DIRECT (INTEGER_SVD_getNumberOfColumns) {
	INTEGER_ONE (SVD)
		integer result = my isTransposed ? my numberOfRows : my numberOfColumns;
	INTEGER_ONE_END (U" (= number of columns)")	
}

DIRECT (REAL_SVD_getConditionNumber) {
	NUMBER_ONE (SVD)
		double result = SVD_getConditionNumber (me);
	NUMBER_ONE_END (U" (= condition number)")
}

FORM (REAL_SVD_getSingularValue, U"SVD: Get singular values", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	NUMBER_ONE (SVD)
		Melder_require (index <= my numberOfColumns, U"Index must be in the range [1,", my numberOfColumns, U"].");
		double result = my d [index];
	NUMBER_ONE_END (U" (= singular value [", index, U"])")
}

FORM (REAL_SVD_getSumOfSingularValues, U"SVD: Get sum of singular values", nullptr) {
	NATURAL (from, U"From", U"1")
	INTEGER (to, U"To", U"0 (=last)")
	OK
DO
	NUMBER_ONE (SVD)
		double result = SVD_getSumOfSingularValues (me, from, to);
	NUMBER_ONE_END (U" (sum of singular values)")
}

FORM (REAL_SVD_getSumOfSingularValuesAsFractionOfTotal, U"SVD: Get sum of singular values as fraction of total", nullptr) {
	NATURAL (from, U"From", U"1")
	INTEGER (to, U"To", U"0 (=last)")
	OK
DO
	NUMBER_ONE (SVD)
		double result = SVD_getSumOfSingularValuesAsFractionOfTotal (me, from, to);
	NUMBER_ONE_END (U" (= fraction of total sum of singular values)")
}

FORM (INTEGER_SVD_getMinimumNumberOfSingularValues, U"SVD: Get minimum number of singular values", U"SVD: Get minimum number of singular values...") {
	POSITIVE (fraction, U"Fraction of total sum", U"0.95")
	OK
DO
	Melder_require (fraction <= 1.0, U"Fraction must be a number in (0,1).");
	INTEGER_ONE (SVD)
		integer result = SVD_getMinimumNumberOfSingularValues (me, fraction);
	INTEGER_ONE_END (U" (= number of singular values needed)")
}

FORM (NEW_SVD_to_Matrix, U"SVD: To Matrix", U"SVD: To Matrix...") {
	NATURAL (fromComponent, U"First component", U"1")
	INTEGER (toComponent, U"Last component", U"0 (= all)")
	OK
DO
	CONVERT_EACH (SVD)
		autoMatrix result = SVD_to_Matrix (me, fromComponent, toComponent);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_SVD_to_TableOfReal, U"SVD: To TableOfReal", U"SVD: To TableOfReal...") {
	NATURAL (fromComponent, U"First component", U"1")
	INTEGER (toComponent, U"Last component", U"0 (= all)")
	OK
DO
	CONVERT_EACH (SVD)
		autoTableOfReal result = SVD_to_TableOfReal (me, fromComponent, toComponent);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_SVD_extractLeftSingularVectors) {
	LOOP {
		iam (SVD);
		autoTableOfReal result = SVD_extractLeftSingularVectors (me);
	CONVERT_EACH_END (my name.get(), U"_lsv")
}

DIRECT (NEW_SVD_extractRightSingularVectors) {
	CONVERT_EACH (SVD)
		autoTableOfReal result = SVD_extractRightSingularVectors (me);
	CONVERT_EACH_END (my name.get(), U"_rsv")
}

DIRECT (NEW_SVD_extractSingularValues) {
	CONVERT_EACH (SVD)
		autoTableOfReal result = SVD_extractSingularValues (me);
	CONVERT_EACH_END (my name.get(), U"_sv");
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
	WORD (xColumn_string, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORD (markColumn_string, U"Column with marks", U"")
	POSITIVE (fontSize, U"Font size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Use only data from rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer markColumn = Table_getColumnIndexFromColumnLabel (me, markColumn_string);
		autoTable part = Table_extractRowsWhere (me, formula, interpreter);
		Table_scatterPlot (part.get(), GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, markColumn, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_scatterPlotMarkWhere, U"Scatter plot where (marks)", nullptr) {
	WORD (xColumn_string, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true)
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	TEXTFIELD (formula, U"Use only data from rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		autoTable part = Table_extractRowsWhere (me, formula, interpreter);
		Table_scatterPlot_mark (part.get(), GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_barPlotWhere, U"Table: Bar plot where", U"Table: Bar plot where...") {
	SENTENCE (yColumns_string, U"Vertical column(s)", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (markColumn_string, U"Column with labels", U"")
	LABEL (U"Distances are in units of 'bar width'")
	REAL (distanceFromBorder, U"Distance of first bar from border", U"1.0")
	REAL (distanceBetweenGroups, U"Distance between bar groups", U"1.0")
	REAL (distanceWithinGroup, U"Distance between bars within group", U"0.0")
	SENTENCE (colours, U"Colours", U"Grey")
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Use only data from rows where the following condition holds:", U"row >= 1 and row <= 8")
	OK
DO
	GRAPHICS_EACH (Table)
		Table_barPlotWhere (me, GRAPHICS, yColumns_string, ymin, ymax, markColumn_string, distanceFromBorder, distanceWithinGroup, distanceBetweenGroups, colours, angle, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_LineGraphWhere, U"Table: Line graph where", U"Table: Line graph where...") {
	SENTENCE (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (xColumn_string, U"Horizonal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (text, U"Text", U"+")
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Use only data from rows where the following condition holds:", U"1; (= everything)")
	OK
DO
	GRAPHICS_EACH (Table)
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer xcolumn = Table_findColumnIndexFromColumnLabel (me, xColumn_string);
		Table_lineGraphWhere (me, GRAPHICS, xcolumn, xmin, xmax,ycolumn, ymin, ymax, text, angle, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_boxPlots, U"Table: Box plots", nullptr) {
	SENTENCE (dataColumns_string, U"Data columns", U"F1 F2")
	WORD (factorColumn_string, U"Factor column", U"Sex")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer factorColumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_boxPlotsWhere (me, GRAPHICS, dataColumns_string, factorColumn, ymin, ymax, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_boxPlotsWhere, U"Table: Box plots where", U"Table: Box plots where...") {
	SENTENCE (dataColumns_string, U"Data columns", U"F1 F2")
	WORD (factorColumn_string, U"Factor column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true);
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer factorColumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_boxPlotsWhere (me, GRAPHICS, dataColumns_string, factorColumn, ymin, ymax, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipseWhere, U"Draw ellipse (standard deviation)", nullptr) {
	WORD (xColumn_string, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"2.0")
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		autoTable thee = Table_extractRowsWhere (me, formula, interpreter);
		Table_drawEllipse_e (thee.get(), GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, numberOfSigmas, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipses, U"Table: Draw ellipses", nullptr) {
	WORD (xColumn_string, U"Horizontal column", U"F2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumn_string, U"Vertical column", U"F1")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORD (factorColumn_string, U"Factor column", U"Vowel")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (fontSize, U"Font size", U"12 (0 = no label)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer factorcolumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_drawEllipsesWhere (me, GRAPHICS, xcolumn, ycolumn, factorcolumn, xmin, xmax, ymin, ymax, numberOfSigmas, fontSize, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipsesWhere, U"Table: Draw ellipses where", nullptr) {
	WORD (xColumn_string, U"Horizontal column", U"F2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumn_string, U"Vertical column", U"F1")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORD (factorColumn_string, U"Factor column", U"Vowel")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (fontSize, U"Font size", U"12 (0 = no label)")
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer factorcolumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_drawEllipsesWhere (me, GRAPHICS, xcolumn, ycolumn, factorcolumn, xmin,  xmax, ymin, ymax,  numberOfSigmas, fontSize, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_Table_normalProbabilityPlot, U"Table: Normal probability plot", U"Table: Normal probability plot...") {
	WORD (column_string, U"Column", U"F1")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer column = Table_getColumnIndexFromColumnLabel (me, column_string);
		Table_normalProbabilityPlot (me, GRAPHICS, column, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_normalProbabilityPlotWhere, U"Table: Normal probability plot where", U"Table: Normal probability plot...") {
	WORD (column_string, U"Column", U"F0")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer column = Table_getColumnIndexFromColumnLabel (me, column_string);
		autoTable thee = Table_extractRowsWhere (me, formula, interpreter);
		Table_normalProbabilityPlot (thee.get(), GRAPHICS, column, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_quantileQuantilePlot, U"Table: Quantile-quantile plot", U"Table: Quantile-quantile plot...") {
	WORD (xColumn_string, U"Horizontal axis column", U"")
	WORD (yColumn_string, U"Vertical axis column", U"")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		Table_quantileQuantilePlot (me, GRAPHICS, xcolumn, ycolumn, numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_quantileQuantilePlot_betweenLevels, U"Table: Quantile-quantile plot (between levels)", U"Table: Quantile-quantile plot...") {
	WORD (dataColumn_string, U"Data column", U"F0")
	WORD (factorColumn_string, U"Factor column", U"Sex")
	WORD (xLevel_string, U"Horizontal factor level", U"")
	WORD (yLevelString, U"Vertical factor level", U"")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		integer factorColumn = Table_getColumnIndexFromColumnLabel (me, factorColumn_string);
		Table_quantileQuantilePlot_betweenLevels (me, GRAPHICS, dataColumn, factorColumn, xLevel_string, yLevelString, numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_lagPlot, U"Table: lag plot", nullptr) {
	WORD (dataColumn_string, U"Data column", U"errors")
	NATURAL (lag, U"Lag", U"1")
	REAL (fromXY, U"left Horizontal and vertical range", U"0.0")
	REAL (toXY, U"right Horizontal and vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_lagPlotWhere (me, GRAPHICS, dataColumn, lag, fromXY, toXY, label, labelSize, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_Table_lagPlotWhere, U"Table: lag plot where", nullptr) {
	WORD (dataColumn_string, U"Data column", U"errors")
	NATURAL (lag, U"Lag", U"1")
	REAL (fromXY, U"left Horizontal and vertical range", U"0.0")
	REAL (toXY, U"right Horizontal and vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_lagPlotWhere (me, GRAPHICS, dataColumn, lag, fromXY, toXY, label, labelSize, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_distributionPlot, U"Table: Distribution plot", nullptr) {
	WORD (dataColumn_string, U"Data column", U"data")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimumFrequency, U"Minimum frequency", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, minimumValue, maximumValue, numberOfBins, minimumFrequency, maximumFrequency, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_distributionPlotWhere, U"Table: Distribution plot where", nullptr) {
	WORD (dataColumn_string, U"Data column", U"data")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	LABEL (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimumFrequency, U"Minimum frequency", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer dataColumn = Table_getColumnIndexFromColumnLabel (me, dataColumn_string);
		Table_distributionPlotWhere (me, GRAPHICS, dataColumn, minimumValue, maximumValue, numberOfBins, minimumFrequency, maximumFrequency, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_horizontalErrorBarsPlot, U"Table: Horizontal error bars plot", U"Table: Horizontal error bars plot...") {
	WORD (xColumn_string, U"Horizontal column", U"x")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	WORD (yColumn_string, U"Vertical column", U"y")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	WORD (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORD (upperErrorColumn_string, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer xl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		integer xu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_horizontalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, xl, xu, barSize_mm, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_horizontalErrorBarsPlotWhere, U"Table: Horizontal error bars plot where", U"Table: Horizontal error bars plot where...") {
	WORD (xColumn_string, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	WORD (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	WORD (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORD (upperErrorColumn_string, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer xl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		integer xu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_horizontalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, xl, xu, barSize_mm, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_verticalErrorBarsPlot, U"Table: Vertical error bars plot", U"Table: Vertical error bars plot...") {
	WORD (xColumn_string, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	WORD (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	WORD (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORD (upperErrorColumn_string, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer yl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		integer yu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_verticalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, yl, yu, barSize_mm, garnish, U"1", interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_verticalErrorBarsPlotWhere, U"Table: Vertical error bars plot where", U"Table: Vertical error bars plot where...") {
	WORD (xColumn_string, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	WORD (yColumn_string, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	WORD (lowerErrorColumn_string, U"Lower error value column", U"error1")
	WORD (upperErrorColumn_string, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	TEXTFIELD (formula, U"Use only data in rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		integer xcolumn = Table_getColumnIndexFromColumnLabel (me, xColumn_string);
		integer ycolumn = Table_getColumnIndexFromColumnLabel (me, yColumn_string);
		integer yl = Table_findColumnIndexFromColumnLabel (me, lowerErrorColumn_string);
		integer yu = Table_findColumnIndexFromColumnLabel (me, upperErrorColumn_string);
		Table_verticalErrorBarsPlotWhere (me, GRAPHICS, xcolumn, ycolumn, xmin, xmax, ymin, ymax, yl, yu, barSize_mm, garnish, formula, interpreter);
	GRAPHICS_EACH_END
}

FORM (NEW_Table_extractRowsWhere, U"Table: Extract rows where", nullptr) {
	TEXTFIELD (formula, U"Extract rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_extractRowsWhere (me, formula, interpreter);
	CONVERT_EACH_END (my name.get(), U"_formula")
}

FORM (NEW_Table_extractRowsMahalanobisWhere, U"Table: Extract rows where (mahalanobis)", nullptr) {
	SENTENCE (dataColumns_string, U"Extract all rows where columns...", U"F1 F2 F3")
	RADIO_ENUM (kMelder_number, haveAMahalanobisDistance,
			U"...have a mahalanobis distance...", kMelder_number::GREATER_THAN)
	REAL (numberOfSigmas, U"...the number", U"2.0")
	WORD (factorColumn_string, U"Factor column", U"")
	TEXTFIELD (formula, U"Process only rows where the following condition holds:", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_extractMahalanobisWhere (me, dataColumns_string, factorColumn_string, numberOfSigmas, haveAMahalanobisDistance, formula, interpreter);
	CONVERT_EACH_END (my name.get(), U"_mahalanobis")
}

FORM (NEW_Table_extractColumnRanges, U"Table: Extract column ranges", nullptr) {
	TEXTFIELD (columnRanges, U"Create a new Table from the following columns:", U"1 2")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_extractColumnRanges (me, columnRanges);
	CONVERT_EACH_END (my name.get(), U"_columns")
}

/******************* TableOfReal ****************************/

DIRECT (NEW1_CreateIrisDataset) {
	CREATE_ONE
		autoTableOfReal result = TableOfReal_createIrisDataset ();
	CREATE_ONE_END (U"iris")
}

FORM (INFO_TableOfReal_reportMultivariateNormality, U"TableOfReal: Report multivariate normality (BHEP)", U"TableOfReal: Report multivariate normality (BHEP)...") {
	REAL (h, U"Smoothing parameter", U"0.0")
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

DIRECT (NEW1_TableOfReal_Permutation_permuteRows) {
	CONVERT_TWO (TableOfReal, Permutation)
		autoTableOfReal result = TableOfReal_Permutation_permuteRows (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW_TableOfReal_to_Permutation_sortRowlabels) {
	CONVERT_EACH (TableOfReal)
		autoPermutation result = TableOfReal_to_Permutation_sortRowLabels (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_TableOfReal_appendColumns) {
	CONVERT_TYPED_LIST (TableOfReal, TableOfRealList)
		autoTableOfReal result = TableOfRealList_appendColumnsMany (list.get());
	CONVERT_TYPED_LIST_END (U"columns_appended");
}

FORM (NEW1_TableOfReal_create_pols1973, U"Create TableOfReal (Pols 1973)", U"Create TableOfReal (Pols 1973)...") {
	BOOLEAN (includeFormantAmplitudeLevels, U"Include formant levels", false)
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
	BOOLEAN (includeFormantAmplitudeLevels, U"Include formant levels", false)
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_vanNierop1973 (includeFormantAmplitudeLevels);
	CREATE_ONE_END (U"vanNierop1973")
}

FORM (NEW_TableOfReal_create_weenink1983, U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Weenink 1985)...") {
	RADIO (speakerGroup, U"Speakers group", 1)
		RADIOBUTTON (U"Men")
		RADIOBUTTON (U"Women")
		RADIOBUTTON (U"Children")
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_weenink1983 (speakerGroup);
	CREATE_ONE_END ((speakerGroup == 1 ? U"m10" : speakerGroup == 2 ? U"w10" : U"c10"));
}

DIRECT (NEW_Table_create_sandwell1987) {
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_sandwell1987 ();
	CREATE_ONE_END (U"Sandwell1987");
}

FORM (GRAPHICS_TableOfReal_drawAsScalableSquares, U"TableOfReal: Draw as scalable squares", U"TableOfReal: Draw as scalable squares...") {
	NATURAL (rowmin, U"From row", U"1");
	INTEGER (rowmax, U"To row", U"0 (=all)");
	NATURAL (colmin, U"From column", U"1");
	INTEGER (colmax, U"To column", U"0 (=all)");
	OPTIONMENU_ENUM (kGraphicsMatrixOrigin, origin,
			U"Origin", kGraphicsMatrixOrigin::DEFAULT)
	POSITIVE (scaleFactor, U"Cell area scale factor", U"0.95")
	OPTIONMENU_ENUM (kGraphicsMatrixCellDrawingOrder, drawingOrder,
			U"Filling order", kGraphicsMatrixCellDrawingOrder::DEFAULT)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawAsScalableSquares (me, GRAPHICS, rowmin, rowmax, colmin, colmax,
				origin, scaleFactor, drawingOrder, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawScatterPlot, U"TableOfReal: Draw scatter plot", U"TableOfReal: Draw scatter plot...") {
	LABEL (U"Select the part of the table")
	NATURAL (xColumn, U"Horizontal axis column number", U"1")
	NATURAL (yColumn, U"Vertical axis column number", U"2")
	INTEGER (fromRow, U"left Row number range", U"0")
	INTEGER (toRow, U"right Row number range", U"0")
	LABEL (U"Select the drawing area limits")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	BOOLEAN (useRowLabels, U"Use row labels", false)
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawScatterPlot (me, GRAPHICS, xColumn, yColumn, fromRow, toRow, xmin, xmax, ymin, ymax, labelSize, useRowLabels, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawScatterPlotMatrix, U"TableOfReal: Draw scatter plots matrix", nullptr) {
	INTEGER (fromColumn, U"From column", U"0")
	INTEGER (toColumn, U"To column", U"0")
	POSITIVE (fractionWhite, U"Fraction white", U"0.1")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawScatterPlotMatrix (me, GRAPHICS, fromColumn, toColumn, fractionWhite);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawBiplot, U"TableOfReal: Draw biplot", U"TableOfReal: Draw biplot...") {
	LABEL (U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (splitFactor, U"Split factor", U"0.5")
	INTEGER (labelSize, U"Label size", U"10")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawBiplot (me, GRAPHICS, xmin, xmax, ymin, ymax, splitFactor, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawVectors, U"Draw vectors", U"TableOfReal: Draw vectors...") {
	LABEL (U"From (x1, y1) to (x2, y2)")
	NATURAL (x1Column, U"X1 column", U"1")
	NATURAL (y1Column, U"Y1 column", U"2")
	NATURAL (x2Column, U"X2 column", U"3")
	NATURAL (y2Column, U"Y2 column", U"4")
	LABEL (U"Select the drawing area")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	RADIO (vectorType, U"Vector type", 1)
		RADIOBUTTON (U"Arrow")
		RADIOBUTTON (U"Double arrow")
		RADIOBUTTON (U"Line")
	INTEGER (labelSize, U"Label size", U"10")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawVectors (me, GRAPHICS, x1Column, y1Column, x2Column, y2Column, xmin, xmax, ymin, ymax, vectorType, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawRowAsHistogram, U"Draw row as histogram", U"TableOfReal: Draw rows as histogram...") {
	LABEL (U"Select from the table")
	WORD (rowNumber, U"Row number", U"1")
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0")
	LABEL (U"Vertical drawing range")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	LABEL (U"Offset and distance in units of 'bar width'")
	REAL (xOffset, U"Horizontal offset", U"0.5")
	REAL (distanceBetweenBars, U"Distance between bars", U"1.0")
	WORD (greys, U"Grey value (1=white)", U"0.7")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, rowNumber, fromColumn, toColumn, ymin, ymax, xOffset, 0, distanceBetweenBars, greys, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawRowsAsHistogram, U"Draw rows as histogram", U"TableOfReal: Draw rows as histogram...") {
	LABEL (U"Select from the table")
	SENTENCE (rowNumbers_string, U"Row numbers", U"1 2")
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0")
	LABEL (U"Vertical drawing range")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	LABEL (U"Offset and distance in units of 'bar width'")
	REAL (xOffset, U"Horizontal offset", U"1.0")
	REAL (distanceBetweenBarGroups, U"Distance between bar groups", U"1.0")
	REAL (distanceBetweenBars, U"Distance between bars", U"0.0")
	SENTENCE (greys_string, U"Grey values (1=white)", U"1 1")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, rowNumbers_string, fromColumn, toColumn, ymin, ymax,xOffset, distanceBetweenBars,distanceBetweenBarGroups, greys_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawBoxPlots, U"TableOfReal: Draw box plots", U"TableOfReal: Draw box plots...") {
	INTEGER (fromRow, U"From row", U"0")
	INTEGER (toRow, U"To row", U"0")
	INTEGER (fromColumn, U"From column", U"0")
	INTEGER (toColumn, U"To column", U"0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawBoxPlots (me, GRAPHICS, fromRow, toRow, fromColumn, toColumn, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawColumnAsDistribution, U"TableOfReal: Draw column as distribution", U"TableOfReal: Draw column as distribution...") {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (fromValue, U"left Value range", U"0.0")
	REAL (toValue, U"right Value range", U"0.0")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right frequency range", U"0.0")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawColumnAsDistribution (me, GRAPHICS, columnNumber, fromValue, toValue, numberOfBins, fromFrequency, toFrequency, false, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW_TableOfReal_to_Configuration_lda, U"TableOfReal: To Configuration (lda)", U"TableOfReal: To Configuration (lda)...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, U"The number of dimensions should be at least zero.");
	CONVERT_EACH (TableOfReal)
		autoConfiguration result = TableOfReal_to_Configuration_lda (me, numberOfDimensions);
	CONVERT_EACH_END (my name.get(), U"_lda")
}

FORM (NEW_TableOfReal_to_CCA, U"TableOfReal: To CCA", U"TableOfReal: To CCA...") {
	NATURAL (dimensionOfDependentVariate, U"Dimension of dependent variate", U"2")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoCCA result = TableOfReal_to_CCA (me, dimensionOfDependentVariate);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TableOfReal_to_Configuration_pca, U"TableOfReal: To Configuration (pca)", U"TableOfReal: To Configuration (pca)...") {
	NATURAL (numberOfDimensions, U"Number of dimensions", U"2")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoConfiguration result = TableOfReal_to_Configuration_pca (me, numberOfDimensions);
	CONVERT_EACH_END (my name.get(), U"_pca")
}

DIRECT (NEW_TableOfReal_to_Discriminant) {
	CONVERT_EACH (TableOfReal)
		autoDiscriminant result = TableOfReal_to_Discriminant (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TableOfReal_to_PCA_byRows) {
	CONVERT_EACH (TableOfReal)
		autoPCA result = TableOfReal_to_PCA_byRows (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TableOfReal_to_SSCP, U"TableOfReal: To SSCP", U"TableOfReal: To SSCP...") {
	INTEGER (fromRow, U"Begin row", U"0")
	INTEGER (toRow, U"End row", U"0")
	INTEGER (fromColumn, U"Begin column", U"0")
	INTEGER (toColumn, U"End column", U"0")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoSSCP result = TableOfReal_to_SSCP (me, fromRow, toRow, fromColumn, toColumn);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TableOfReal_to_SSCP_rowWeights, U"TableOfReal: To SSCP (row weights)", U"TableOfReal: To SSCP (row weights)...") {
	INTEGER (fromRow, U"Begin row", U"0")
	INTEGER (toRow, U"End row", U"0")
	INTEGER (fromColumn, U"Begin column", U"0")
	INTEGER (toColumn, U"End column", U"0")
	INTEGER (rowWeights, U"Weights column number", U"0")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoSSCP result = TableOfReal_to_SSCP_rowWeights (me, fromRow, toRow, fromColumn, toColumn, rowWeights);
	CONVERT_EACH_END (my name.get())
}

/* For the inheritors */
DIRECT (NEW_TableOfReal_to_TableOfReal) {
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_to_TableOfReal (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TableOfReal_to_Correlation) {
	CONVERT_EACH (TableOfReal)
		autoCorrelation result = TableOfReal_to_Correlation (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TableOfReal_to_Correlation_rank) {
	CONVERT_EACH (TableOfReal)
		autoCorrelation result = TableOfReal_to_Correlation_rank (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TableOfReal_to_Covariance) {
	CONVERT_EACH (TableOfReal)
		autoCovariance result = TableOfReal_to_Covariance (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TableOfReal_to_SVD) {
	CONVERT_EACH (TableOfReal)
		autoSVD result = TableOfReal_to_SVD (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_TablesOfReal_to_Eigen_gsvd) {
	CONVERT_COUPLE (TableOfReal)
		autoEigen result = TablesOfReal_to_Eigen_gsvd (me, you);
	CONVERT_COUPLE_END (U"gsvd");
}

FORM (NEW1_TableOfReal_TableOfReal_crossCorrelations, U"TableOfReal & TableOfReal: Cross-correlations", nullptr) {
	OPTIONMENU (between, U"Correlations between", 1)
		OPTION (U"Rows")
		OPTION (U"Columns")
	BOOLEAN (center, U"Center", false)
	BOOLEAN (normalize, U"Normalize", false)
	OK
DO
	int by_columns = between - 1;
	CONVERT_COUPLE (TableOfReal)
		autoTableOfReal result = TableOfReal_TableOfReal_crossCorrelations (me, you, by_columns, center, normalize);
	CONVERT_COUPLE_END (U"cc_", by_columns ? U"by_cols" : U"by_rows")
}

void praat_TableOfReal_init3 (ClassInfo klas);
void praat_TableOfReal_init3 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 2, U"To TableOfReal (cross-correlations)...", 0, 0, NEW1_TableOfReal_TableOfReal_crossCorrelations);
}

DIRECT (NEW1_TablesOfReal_to_GSVD) {
	CONVERT_COUPLE (TableOfReal)
		autoGSVD result = TablesOfReal_to_GSVD (me, you);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW_TableOfReal_choleskyDecomposition, U"TableOfReal: Cholesky decomposition", nullptr) {
	BOOLEAN (wantUpper, U"Upper (else L)", false)
	BOOLEAN (wantInverse, U"Inverse", false)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_choleskyDecomposition (me, wantUpper, wantInverse);
	CONVERT_EACH_END (my name.get())
}

FORM (NEWMANY_TableOfReal_to_PatternList_and_Categories, U"TableOfReal: To PatternList and Categories", U"TableOfReal: To PatternList and Categories...") {
	INTEGER (fromRow, U"left Row range", U"0")
	INTEGER (toRow, U"right Row range", U"0 (= all)")
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0 (= all)")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoPatternList ap;
		autoCategories result;
		TableOfReal_to_PatternList_and_Categories (me, fromRow, toRow, fromColumn, toColumn, & ap, & result);
		praat_new (ap.move(), my name.get());
	CONVERT_EACH_END (my name.get())
}

FORM (REAL_TableOfReal_getColumnSum, U"TableOfReal: Get column sum", U"") {
	INTEGER (columnNumber, U"Column", U"1")
	OK
DO
	NUMBER_ONE (TableOfReal)
		double result = TableOfReal_getColumnSum (me, columnNumber);
	NUMBER_ONE_END (U"(column sum)")
}

FORM (REAL_TableOfReal_getRowSum, U"TableOfReal: Get row sum", U"") {
	INTEGER (rowNumber, U"Row", U"1")
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
	BOOLEAN (expand, U"Expand", false)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_meansByRowLabels (me, expand, 0);
	CONVERT_EACH_END (my name.get(), U"_byrowlabels")
}

FORM (NEW_TableOfReal_mediansByRowLabels, U"TableOfReal: Medians by row labels", U"TableOfReal: To TableOfReal (medians by row labels)...") {
	BOOLEAN (expand, U"Expand", false)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_meansByRowLabels (me, expand, 1);
	CONVERT_EACH_END (my name.get(), U"_byrowlabels")
}

FORM (NEW_TableOfReal_to_TableOfReal_rankColumns, U"TableOfReal: Rank columns", U"TableOfReal: To TableOfReal (rank columns)...") {
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0 (=all)")
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoTableOfReal result = TableOfReal_rankColumns (me, fromColumn, toColumn);
	CONVERT_EACH_END (my name.get(), U"_byrowlabels")
}

/***** TableOfReal and FilterBank  *****/

FORM (REAL_TextGrid_getTotalDurationOfIntervalsWhere, U"Total duration of intervals where", nullptr) {
	INTEGER (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMelder_string, countIntervalsWhoseLabel___,
			U"Intervals whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	NUMBER_ONE (TextGrid)
		double result = TextGrid_getTotalDurationOfIntervalsWhere (me, tierNumber, countIntervalsWhoseLabel___, ___theText);
	NUMBER_ONE_END (U" s (duration of intervals containing \"", ___theText, U"\")");
}

FORM (MODIFY_TextGrid_extendTime, U"TextGrid: Extend time", U"TextGrid: Extend time...") {
	POSITIVE (extendDomainBy, U"Extend domain by (s)", U"1.0")
	RADIO (position, U"At", 1)
		RADIOBUTTON (U"End")
		RADIOBUTTON (U"Start")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_extendTime (me, extendDomainBy, position - 1);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_replaceIntervalTexts, U"TextGrid: Replace interval texts", U"TextGrid: Replace interval texts...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	INTEGER (fromInterval, U"left Interval range", U"1")
	INTEGER (toInterval, U"right Interval range", U"0 (= all)")
	SENTENCE (search_string, U"Search", U"a")
	SENTENCE (replace_string, U"Replace", U"b")
	RADIO (searchType, U"Search and replace strings are:", 1)
		RADIOBUTTON (U"Literals")
		RADIOBUTTON (U"Regular Expressions")
	OK
DO
	MODIFY_EACH (TextGrid)
		integer nmatches, nstringmatches;
		TextGrid_changeLabels (me, tierNumber, fromInterval, toInterval, search_string, replace_string, searchType - 1, &nmatches, &nstringmatches);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_replacePointTexts, U"TextGrid: Replace point texts", U"TextGrid: Replace point texts...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	INTEGER (fromInterval, U"left Interval range", U"1")
	INTEGER (toInterval, U"right Interval range", U"0 (= all)")
	SENTENCE (search_string, U"Search", U"a")
	SENTENCE (replace_string, U"Replace", U"b")
	RADIO (searchType, U"Search and replace strings are:", 1)
		RADIOBUTTON (U"Literals")
		RADIOBUTTON (U"Regular Expressions")
	OK
DO
	MODIFY_EACH (TextGrid)
		integer nmatches, nstringmatches;
		TextGrid_changeLabels (me, tierNumber, fromInterval, toInterval, search_string, replace_string, searchType - 1, &nmatches, &nstringmatches);
	MODIFY_EACH_END
}

FORM (NEW1_TextGrids_to_Table_textAlignment, U"TextGrids: To Table (text alignment)", nullptr) {
	NATURAL (targetTierNumber, U"Target tier", U"1")
	NATURAL (sourceTierNumber, U"Source tier", U"1")
	OK
DO
	CONVERT_COUPLE (TextGrid)
 		autoTable result = TextGrids_to_Table_textAlignment (me, targetTierNumber, you, sourceTierNumber, nullptr);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get());
}

FORM (NEW_TextGrid_to_DurationTier, U"TextGrid: To DurationTier", U"TextGrid: To DurationTier...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	POSITIVE (timeScaleFactor, U"Time scale factor", U"2.0")
	POSITIVE (leftTransitionDuration, U"Left transition duration (s)", U"1e-10")
	POSITIVE (rightTransitionDuration, U"Right transition duration (s)", U"1e-10")
	OPTIONMENU_ENUM (kMelder_string, scaleIntervalsWhoseLabel___,
			U"Scale intervals whose label... ", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoDurationTier result = TextGrid_to_DurationTier (me,tierNumber, timeScaleFactor,
			leftTransitionDuration, rightTransitionDuration, scaleIntervalsWhoseLabel___, ___theText);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TextGrid_DurationTier_to_TextGrid) {
	CONVERT_TWO (TextGrid, DurationTier)
		autoTextGrid result = TextGrid_DurationTier_scaleTimes (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_TextGrids_EditCostsTable_to_Table_textAlignment, U"TextGrids & EditCostsTable: To Table (text alignment)", nullptr) {
	NATURAL (targetTierNumber, U"Target tier", U"1")
	NATURAL (sourceTierNumber, U"Source tier", U"1")
	OK
DO
	CONVERT_COUPLE_AND_ONE (TextGrid, EditCostsTable)
		autoTable result = TextGrids_to_Table_textAlignment (me, targetTierNumber, you, sourceTierNumber, him);
	CONVERT_COUPLE_AND_ONE_END (my name.get(), U"_", your name.get())
}

FORM (MODIFY_TextGrid_setTierName, U"TextGrid: Set tier name", U"TextGrid: Set tier name...") {
	NATURAL (tierNUmber, U"Tier number:", U"1")
	SENTENCE (name, U"Name", U"");
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_setTierName (me, tierNUmber, name);
	MODIFY_EACH_END
}

static void cb_publish (Editor /*editor*/, autoDaata publish) {
	try {
		praat_new (publish.move(), U"");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}

DIRECT (WINDOW_VowelEditor_create) {
	if (theCurrentPraatApplication -> batch) {
		Melder_throw (U"Cannot edit from batch.");
	}
	autoVowelEditor vowelEditor = VowelEditor_create (U"VowelEditor", nullptr);
	Editor_setPublicationCallback (vowelEditor.get(), cb_publish);
	vowelEditor.releaseToUser();
END }

static autoDaata cmuAudioFileRecognizer (integer nread, const char *header, MelderFile file) {
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
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, praat_HIDDEN, NEW1_Eigen_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, praat_HIDDEN, NEW1_Eigen_Matrix_projectColumns);
}

static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm);
static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, praat_HIDDEN, NEW1_Eigen_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, 0, NEW1_Eigen_Matrix_projectColumns);
}

static void praat_Eigen_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Get eigenvalue...", nullptr, 1, REAL_Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, U"Get sum of eigenvalues...", nullptr, 1, REAL_Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, U"Get number of eigenvectors", nullptr, 1, INTEGER_Eigen_getNumberOfEigenvectors);
	praat_addAction1 (klas, 1, U"Get eigenvector dimension", nullptr, 1, INTEGER_Eigen_getEigenvectorDimension);
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
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getTimeFromColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, INTEGER_FilterBank_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, praat_DEPRECATED_2014 | praat_DEPTH_1, REAL_FilterBank_getFrequencyFromRow);
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
	praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, REAL_FunctionTerms_getValue);
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
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, 1, REAL_BandFilterSpectrogram_getTimeFromColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, 1, REAL_BandFilterSpectrogram_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, 1, REAL_BandFilterSpectrogram_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, 1, INTEGER_BandFilterSpectrogram_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, 1, REAL_BandFilterSpectrogram_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, 1, REAL_BandFilterSpectrogram_getFrequencyFromRow);
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
	praat_addAction1 (klas, 1, U"Get degrees of freedom", U"Get number of observations", 1, NUMBER_SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, U"Get centroid element...", U"Get degrees of freedom", 1, REAL_SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, U"Get ln(determinant)", U"Get centroid element...", 1, REAL_SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Extract centroid", EXTRACT_BUTTON, 1, NEW_SSCP_extractCentroid);
}

FORM (MODIFY_SSCP_setValue, U"Covariance: Set value", U"Covariance: Set value...") {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (value, U"New value", U"1.0")
	OK
DO
	MODIFY_EACH (SSCP)
		SSCP_setValue (me, rowNumber, columnNumber, value);
	MODIFY_EACH_END
}

FORM (MODIFY_SSCP_setCentroid, U"", nullptr) {
	NATURAL (elementNumber, U"Element number", U"1")
	REAL (value, U"New value", U"1.0")
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
		classFileInMemory, classFileInMemorySet, classFileInMemoryManager, classFormantFilter,
		classIndex, classKlattTable, classNMF,
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
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial from product terms...", nullptr, 1, NEW1_Polynomial_createFromProductTerms);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial from real zeros...", nullptr, 1, NEW1_Polynomial_createFromRealZeros);
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
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Sandwell 1987)", U"Create TableOfReal (Weenink 1985)...", praat_DEPTH_1+ praat_HIDDEN, NEW_Table_create_sandwell1987);
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
	praat_addMenuCommand (U"Objects", U"New", U"FileInMemoryManager", nullptr, praat_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemoryManager", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_FileInMemoryManager_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemory...", nullptr, praat_HIDDEN + praat_DEPTH_1, READ1_FileInMemory_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemorySet from directory contents...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW_FileInMemorySet_createFromDirectoryContents);
	praat_addMenuCommand (U"Objects", U"New", U"Extract espeak data...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_ExtractEspeakData);

	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Little Endian file...", U"Read from special sound file", 1, READ1_Sound_readFromRawFileLE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Big Endian file...", U"Read Sound from raw 16-bit Little Endian file...", 1, READ1_Sound_readFromRawFileBE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read KlattTable from raw text file...", U"Read Matrix from raw text file...", praat_HIDDEN, READ1_KlattTable_readFromRawTextFile);

	praat_addAction1 (classActivationList, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classActivationList, 0, U"Formula...", nullptr, 0, MODIFY_ActivationList_formula);
	praat_addAction1 (classActivationList, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classActivationList, 0, U"To Matrix", nullptr, 0, NEW_ActivationList_to_Matrix);
	praat_addAction1 (classActivationList, 0, U"To PatternList", nullptr, 0, NEW_ActivationList_to_PatternList);

	praat_addAction2 (classActivationList, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, NEW1_ActivationList_Categories_to_TableOfReal);
	
	praat_addAction1 (classAmplitudeTier, 0, U"Get value at time...", U"Get time from index...", 1, REAL_AmplitudeTier_getValueAtTime);
	praat_addAction1 (classAmplitudeTier, 0, U"Get value at index...", U"Get value at time...", 1, REAL_AmplitudeTier_getValueAtIndex);

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
	praat_addAction1 (classCategories, 2, U"Get number of differences", QUERY_BUTTON, 1, INTEGER_Categories_getNumberOfDifferences);
	praat_addAction1 (classCategories, 2, U"Get fraction different", QUERY_BUTTON, 1, REAL_Categories_getFractionDifferent);
	praat_addAction1 (classCategories, 0, MODIFY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 1, U"Append category...", MODIFY_BUTTON, 1, MODIFY_Categories_appendCategory);
	praat_addAction1 (classCategories, 1, U"Append 1 category...", U"Append category...", praat_HIDDEN | praat_DEPTH_1, MODIFY_Categories_appendCategory);
	praat_addAction1 (classCategories, 0, U"Extract", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 0, U"To unique Categories", nullptr, 0, NEW_Categories_selectUniqueItems);
	praat_addAction1 (classCategories, 0, U"Analyse", nullptr, 0, nullptr); // TODO name
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
	praat_addAction1 (classCCA, 1, U"Get correlation...", nullptr, 0, REAL_CCA_getCorrelation);
	praat_addAction1 (classCCA, 1, U"Get eigenvector element...", nullptr, 0, REAL_CCA_getEigenvectorElement);
	praat_addAction1 (classCCA, 1, U"Get zero correlation probability...", nullptr, 0, REAL_CCA_getZeroCorrelationProbability);
	praat_addAction1 (classCCA, 1, U"Extract Eigen...", nullptr, 0, NEW_CCA_extractEigen);

	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (scores)...", nullptr, 0, NEW_CCA_TableOfReal_to_TableOfReal_scores);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (loadings)", nullptr, 0, NEW_CCA_TableOfReal_to_TableOfReal_loadings);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"Predict...", nullptr, 0, NEW1_CCA_TableOfReal_predict);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"To TableOfReal (loadings)", nullptr, 0, NEW1_CCA_Correlation_to_TableOfReal_loadings);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get variance fraction...", nullptr, 0, REAL_CCA_Correlation_getVarianceFraction);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get redundancy (sl)...", nullptr, 0, REAL_CCA_Correlation_getRedundancy_sl);

	praat_addAction1 (classComplexSpectrogram, 0, U"ComplexSpectrogram help", nullptr, 0, HELP_ComplexSpectrogram_help);
	praat_addAction1 (classComplexSpectrogram, 0, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Sound...", nullptr, 0, NEW_ComplexSpectrogram_to_Sound);
	praat_addAction1 (classComplexSpectrogram, 0, U"Down to Spectrogram", nullptr, 0, NEW_ComplexSpectrogram_downto_Spectrogram);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Spectrum (slice)...", nullptr, 0, NEW_ComplexSpectrogram_to_Spectrum_slice);
	//praat_addAction1 (classComplexSpectrogram, 0, U"Paint...", 0, 1, DO_Spectrogram_paint);
	praat_addAction2 (classComplexSpectrogram, 1, classSpectrogram, 1, U"Replace amplitudes", nullptr, 0, MODIFY_ComplexSpectrogram_Spectrogram_replaceAmplitudes);

	praat_addAction1 (classConfusion, 0, U"Confusion help", nullptr, 0, HELP_Confusion_help);
	praat_TableOfReal_init2 (classConfusion);
	praat_removeAction (classConfusion, nullptr, nullptr, U"Draw as numbers...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by label...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by column...");
	praat_addAction1 (classConfusion, 0, U"Draw as numbers...", U"Draw -", 1, GRAPHICS_Confusion_drawAsNumbers);
	praat_addAction1 (classConfusion, 1, U"Get value (labels)...", U"Get value...", 1, REAL_Confusion_getValue_labels);
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
	praat_addAction1 (classConfusion, 2, U"To difference matrix", nullptr, 0, NEW1_Confusion_difference); // TODO to_Matrix_difference ?

	praat_addAction2 (classConfusion, 1, classClassificationTable, 1, U"Increase confusion count", nullptr, 0, MODIFY_Confusion_ClassificationTable_increaseConfusionCount);

	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw confusion...", nullptr, 0, GRAPHICS_Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, U"Covariance help", nullptr, 0, HELP_Covariance_help);
	praat_SSCP_as_TableOfReal_init (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
	praat_addAction1 (classCovariance, 1, U"Get probability at position...", U"Get value...", 1, REAL_Covariance_getProbabilityAtPosition);
	praat_addAction1 (classCovariance, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, REAL_SSCP_getDiagonality_bartlett);
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
	praat_addAction1 (classCovariance, 0, U"To Covariance (between)", nullptr, 0, NEW1_Covariances_to_Covariance_between);
	praat_addAction1 (classCovariance, 0, U"To Covariance (within)", nullptr, 0, NEW1_Covariances_to_Covariance_within);

	praat_addAction2 (classCovariance, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0, NEW1_Covariance_TableOfReal_mahalanobis);

	praat_addAction1 (classClassificationTable, 0, U"ClassificationTable help", nullptr, 0, HELP_ClassificationTable_help);
	praat_TableOfReal_init (classClassificationTable);
	praat_addAction1 (classClassificationTable, 0, U"Get class index at maximum in row...", U"Get column index...", 1, INTEGER_ClassificationTable_getClassIndexAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"Get class label at maximum in row...", U"Get class index at maximum in row...", 1, INTEGER_ClassificationTable_getClassLabelAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"To Confusion", U"*To Confusion...", praat_DEPRECATED_2014, NEW_ClassificationTable_to_Confusion_old); // deprecated 2014
	praat_addAction1 (classClassificationTable, 0, U"To Confusion...", nullptr, 0, NEW_ClassificationTable_to_Confusion);
	praat_addAction1 (classClassificationTable, 0, U"To Correlation (columns)", nullptr, 0, NEW_ClassificationTable_to_Correlation_columns);
	praat_addAction1 (classClassificationTable, 0, U"To Strings (max. prob.)", nullptr, 0, NEW_ClassificationTable_to_Strings_maximumProbability); // TODO name?

	praat_addAction1 (classCorrelation, 0, U"Correlation help", nullptr, 0, HELP_Correlation_help);
	praat_TableOfReal_init2 (classCorrelation);
	praat_SSCP_query_init (classCorrelation);
	praat_SSCP_extract_init (classCorrelation);
	praat_addAction1 (classCorrelation, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, REAL_Correlation_getDiagonality_bartlett);
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
	praat_addAction1 (classDiscriminant, 1, U"Get number of eigenvectors", nullptr, 1, INTEGER_Discriminant_getNumberOfEigenvectors);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector dimension", nullptr, 1, INTEGER_Discriminant_getEigenvectorDimension);
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
	praat_addAction1 (classDiscriminant, 1, U"Report equality of covariance matrices", 0, 1, INFO_Discriminant_reportEqualityOfCovarianceMatrices);
	praat_addAction1 (classDiscriminant, 1, U"-- ellipses --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get sigma ellipse area...", 0, 1, REAL_Discriminant_getSigmaEllipseArea);
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

	praat_addAction1 (classDiscriminant , 0, U"& TableOfReal: To ClassificationTable?", nullptr, 0, hint_Discriminant_TableOfReal_to_ClassificationTable);

	/*		praat_addAction1 (classDiscriminant, 1, U"Extract coefficients...", nullptr, 1, DO_Discriminant_extractCoefficients);*/

	//praat_addAction2 (classDiscriminant, 1, classMatrix, 1, U"To Matrix (pc)...", nullptr, 0, DO_Discriminant_Matrix_to_Matrix_pc);

	praat_Eigen_Spectrogram_project (classDiscriminant, classSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classMelSpectrogram);

	praat_Eigen_Matrix_project (classDiscriminant, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classMelFilter); // deprecated 2014

	praat_addAction2 (classDiscriminant, 1, classPatternList, 1, U"To Categories...", nullptr, 0, NEW1_Discriminant_PatternList_to_Categories);
	praat_addAction2 (classDiscriminant, 1, classSSCP, 1, U"Project", nullptr, 0, NEW1_Eigen_SSCP_project);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Modify Discriminant", nullptr, 0, 0);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Set group labels", nullptr, 0, MODIFY_Discriminant_setGroupLabels);

	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0, NEW1_Discriminant_TableOfReal_to_Configuration);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1,U"To ClassificationTable...", nullptr, 0, NEW1_Discriminant_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0, NEW1_Discriminant_TableOfReal_mahalanobis);

	praat_addAction1 (classDTW, 0, U"DTW help", nullptr, 0, HELP_DTW_help);
	praat_addAction1 (classDTW, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 0, U"Draw path...", nullptr, 1, GRAPHICS_DTW_drawPath);
	praat_addAction1 (classDTW, 0, U"Paint distances...", nullptr, 1, GRAPHICS_DTW_paintDistances);
	praat_addAction1 (classDTW, 0, U"Draw warp (x)...", nullptr, 1, GRAPHICS_DTW_drawWarp_x);
	praat_addAction1 (classDTW, 0, U"Draw warp (y)...", nullptr, 1, GRAPHICS_DTW_drawWarp_y);
	praat_addAction1 (classDTW, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 1, U"Query time domains", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (x)", nullptr, 2, REAL_DTW_getStartTime_x);
	praat_addAction1 (classDTW, 1, U"Get end time (x)", nullptr, 2, REAL_DTW_getEndTime_x);
	praat_addAction1 (classDTW, 1, U"Get total duration (x)", nullptr, 2, REAL_DTW_getTotalDuration_x);
	praat_addAction1 (classDTW, 1, U"-- time domain x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (y)", nullptr, 2, REAL_DTW_getStartTime_y);
	praat_addAction1 (classDTW, 1, U"Get end time (y)", nullptr, 2, REAL_DTW_getEndTime_y);
	praat_addAction1 (classDTW, 1, U"Get total duration (y)", nullptr, 2, REAL_DTW_getTotalDuration_y);
	praat_addAction1 (classDTW, 1, U"Query time samplings", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (x)", nullptr, 2, INTEGER_DTW_getNumberOfFrames_x);
	praat_addAction1 (classDTW, 1, U"Get time step (x)", nullptr, 2, REAL_DTW_getTimeStep_x);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (x)...", nullptr, 2, REAL_DTW_getTimeFromFrameNumber_x);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (x)...", nullptr, 2, INTEGER_DTW_getFrameNumberFromTime_x);
	praat_addAction1 (classDTW, 1, U"-- time sampling x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (y)", nullptr, 2, INTEGER_DTW_getNumberOfFrames_y);
	praat_addAction1 (classDTW, 1, U"Get time step (y)", nullptr, 2, REAL_DTW_getTimeStep_y);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (y)...", nullptr, 2, REAL_DTW_getTimeFromFrameNumber_y);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (y)...", nullptr, 2, INTEGER_DTW_getFrameNumberFromTime_y);

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
	praat_addAction1 (classDTW, 1, U"Get distance (weighted)", nullptr, 1, REAL_DTW_getDistance_weighted);
	praat_addAction1 (classDTW, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classDTW, 0, U"Formula (distances)...", nullptr, 1, MODIFY_DTW_formula_distances);
	praat_addAction1 (classDTW, 0, U"Set distance value...", nullptr, 1, MODIFY_DTW_setDistanceValue);

	praat_addAction1 (classDTW, 0, U"Analyse", nullptr, 0, 0);
    praat_addAction1 (classDTW, 0, U"Find path...", nullptr, praat_HIDDEN, MODIFY_DTW_findPath);
    praat_addAction1 (classDTW, 0, U"Find path (band & slope)...", nullptr, 0, MODIFY_DTW_findPath_bandAndSlope);
    praat_addAction1 (classDTW, 0, U"To Polygon...", nullptr, 1, NEW_DTW_to_Polygon);
	praat_addAction1 (classDTW, 0, U"To Matrix (distances)", nullptr, 0, NEW_DTW_to_Matrix_distances);
    praat_addAction1 (classDTW, 0, U"To Matrix (cum. distances)...", nullptr, 0, NEW_DTW_to_Matrix_cumulativeDistances);
	praat_addAction1 (classDTW, 0, U"Swap axes", nullptr, 0, NEW_DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classMatrix, 1, U"Replace matrix", nullptr, 0, MODIFY_DTW_Matrix_replace);
	praat_addAction2 (classDTW, 1, classTextGrid, 1, U"To TextGrid (warp times)", nullptr, 0, NEW1_DTW_TextGrid_to_TextGrid);
	praat_addAction2 (classDTW, 1, classIntervalTier, 1, U"To Table (distances)", nullptr, 0, NEW1_DTW_IntervalTier_to_Table);

    praat_addAction2 (classDTW, 1, classPolygon, 1, U"Find path inside...", nullptr, 0, MODIFY_DTW_Polygon_findPathInside);
    praat_addAction2 (classDTW, 1, classPolygon, 1, U"To Matrix (cum. distances)...", nullptr, 0, NEW1_DTW_Polygon_to_Matrix_cumulativeDistances);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw...", nullptr, 0, GRAPHICS_DTW_Sounds_draw);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw warp (x)...", nullptr, 0, GRAPHICS_DTW_Sounds_drawWarp_x);

	praat_addAction1 (classEditDistanceTable, 1, U"EditDistanceTable help", nullptr, 0, HELP_EditDistanceTable_help);
	praat_EditDistanceTable_as_TableOfReal_init (classEditDistanceTable);
	praat_addAction1 (classEditDistanceTable, 1, U"To TableOfReal (directions)...", nullptr, praat_HIDDEN, NEW_EditDistanceTable_to_TableOfReal_directions);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, U"Set edit costs", nullptr, 0, MODIFY_EditDistanceTable_setEditCosts);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, U"Set new edit costs", nullptr, praat_DEPRECATED_2017, MODIFY_EditDistanceTable_setEditCosts);

	praat_addAction1 (classEditCostsTable, 1, U"EditCostsTable help", nullptr, 0, HELP_EditCostsTable_help);
	praat_addAction1 (classEditCostsTable, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, U"Get target index...", nullptr, 1, INTEGER_EditCostsTable_getTargetIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get source index...", nullptr, 1, INTEGER_EditCostsTable_getSourceIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get insertion costs...", nullptr, 1, REAL_EditCostsTable_getInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Get insertion cost...", nullptr, praat_DEPRECATED_2017, REAL_EditCostsTable_getInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Get deletion costs...", nullptr, 1, REAL_EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get deletion cost...", nullptr, praat_DEPRECATED_2017, REAL_EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get substitution costs...", nullptr, 1, REAL_EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get substitution cost...", nullptr, praat_DEPRECATED_2017, REAL_EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get costs (others)...", nullptr, 1, REAL_EditCostsTable_getCosts_others);
	praat_addAction1 (classEditCostsTable, 1, U"Get cost (others)...", nullptr, praat_DEPRECATED_2017, REAL_EditCostsTable_getCosts_others);
	praat_addAction1 (classEditCostsTable, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classEditCostsTable, 1, U"Set target symbol (index)...", nullptr, 1, MODIFY_EditCostsTable_setTargetSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set source symbol (index)...", nullptr, 1, MODIFY_EditCostsTable_setSourceSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set insertion costs...", nullptr, 1, MODIFY_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set insertion cost...", nullptr, praat_DEPRECATED_2017, MODIFY_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set deletion costs...", nullptr, 1, MODIFY_EditCostsTable_setDeletionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set substitution costs...", nullptr, 1, MODIFY_EditCostsTable_setSubstitutionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set costs (others)...", nullptr, 1, MODIFY_EditCostsTable_setCosts_others);
	praat_addAction1 (classEditCostsTable, 1, U"To TableOfReal", nullptr, 0, NEW_EditCostsTable_to_TableOfReal);

	praat_Index_init (classStringsIndex);
	praat_addAction1 (classIndex, 0, U"Index help", nullptr, 0, HELP_Index_help);
	praat_addAction1 (classStringsIndex, 1, U"Get class label...", nullptr, 0, INFO_StringsIndex_getClassLabelFromClassIndex);
	praat_addAction1 (classStringsIndex, 1, U"Get class index...", nullptr, 0, INTEGER_StringsIndex_getClassIndex);
	praat_addAction1 (classStringsIndex, 1, U"Get label...", nullptr, 0, INFO_StringsIndex_getItemLabelFromItemIndex);
	praat_addAction1 (classStringsIndex, 1, U"Get class index from item index...", nullptr, 0, INTEGER_StringsIndex_getClassIndexFromItemIndex);
	praat_addAction1 (classIndex, 1, U"Get index...", nullptr, 0, INTEGER_Index_getIndex);
	praat_addAction1 (classStringsIndex, 1, U"To Strings", nullptr, 0, NEW_StringsIndex_to_Strings);

	praat_addAction1 (classEigen, 0, U"Eigen help", nullptr, 0, HELP_Eigen_help);
	praat_addAction1 (classEigen, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 0, U"Draw eigenvalues (scree)...", nullptr, praat_DEPTH_1 | praat_DEPRECATED_2010, GRAPHICS_Eigen_drawEigenvalues_scree);
		praat_addAction1 (classEigen, 0, U"Draw eigenvalues...", nullptr, 1, GRAPHICS_Eigen_drawEigenvalues);
		praat_addAction1 (classEigen, 0, U"Draw eigenvector...", nullptr, 1, GRAPHICS_Eigen_drawEigenvector);
	praat_addAction1 (classEigen, 0, QUERY_BUTTON, nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 1, U"Get number of eigenvalues", nullptr, 1, INTEGER_Eigen_getNumberOfEigenvalues);
		praat_addAction1 (classEigen, 1, U"Get eigenvalue...", nullptr, 1, REAL_Eigen_getEigenvalue);
		praat_addAction1 (classEigen, 1, U"Get sum of eigenvalues...", nullptr, 1, REAL_Eigen_getSumOfEigenvalues);
	praat_addAction1 (classEigen, 1, U"-- eigenvectors --", nullptr, 1, 0);
		praat_addAction1 (classEigen, 1, U"Get number of eigenvectors", nullptr, 1, INTEGER_Eigen_getNumberOfEigenvectors);
		praat_addAction1 (classEigen, 1, U"Get eigenvector dimension", nullptr, 1, INTEGER_Eigen_getEigenvectorDimension);
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
	praat_addAction1 (classFileInMemory, 0, U"To FileInMemorySet", nullptr, 0, NEW1_FilesInMemory_to_FileInMemorySet);
	praat_addAction1 (classFileInMemory, 0, U"To FilesInMemory", nullptr, praat_DEPRECATED_2015, NEW1_FilesInMemory_to_FileInMemorySet);

	praat_addAction1 (classFileInMemorySet, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classFileInMemorySet, 1, U"Get number of files", nullptr, 1, INFO_FileInMemorySet_getNumberOfFiles);
	praat_addAction1 (classFileInMemorySet, 1, U"Has directory?", nullptr, 1, INFO_FileInMemorySet_hasDirectory);

	praat_addAction1 (classFileInMemorySet, 1, U"Show as code...", nullptr, 0, INFO_FileInMemorySet_showAsCode);
	praat_addAction1 (classFileInMemorySet, 1, U"Show one file as code...", nullptr, 0, INFO_FileInMemorySet_showOneFileAsCode);
	praat_addAction1 (classFileInMemorySet, 0, U"Merge", nullptr, 0, NEW1_FileInMemorySets_merge);
	praat_addAction1 (classFileInMemorySet, 0, U"To Strings (id)", nullptr, 0, NEW_FileInMemorySet_to_Strings_id);
	praat_addAction1 (classFileInMemorySet, 0, U"Extract files...", nullptr, 0, NEW1_FileInMemorySet_extractFiles);
	praat_addAction2 (classFileInMemorySet, 1, classFileInMemory, 0, U"Add items to set", nullptr, 0, MODIFY_FileInMemorySet_addItemsToSet);

	praat_addAction1 (classFileInMemoryManager, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classFileInMemoryManager, 1, U"Get number of files", nullptr, 1, INFO_FileInMemoryManager_getNumberOfFiles);
	praat_addAction1 (classFileInMemoryManager, 1, U"Get number of open files", nullptr, 1, INFO_FileInMemoryManager_getNumberOfOpenFiles);
	praat_addAction1 (classFileInMemoryManager, 1, U"Has directory?", nullptr, 1, INFO_FileInMemoryManager_hasDirectory);
	
	praat_addAction1 (classFileInMemoryManager, 0, U"Extract files...", nullptr, 0, NEW1_FileInMemoryManager_extractFiles);
	praat_addAction1 (classFileInMemoryManager, 0, U"Down to Table...", nullptr, 0, NEW1_FileInMemoryManager_downto_Table);
	
	praat_addAction1 (classFormantFilter, 0, U"FormantFilter help", nullptr, praat_DEPRECATED_2015, HELP_FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", praat_DEPTH_1 | praat_DEPRECATED_2014, GRAPHICS_FormantFilter_drawSpectrum_slice);
	praat_addAction1 (classFormantFilter, 0, U"Draw filter functions...", U"Draw filters...",  praat_DEPTH_1  | praat_DEPRECATED_2014, GRAPHICS_FormantFilter_drawFilterFunctions);
	praat_addAction1 (classFormantFilter, 0, U"To Spectrogram", nullptr, 0, NEW_FormantFilter_to_Spectrogram);

	praat_addAction1 (classFormantGrid, 0, U"Draw...", U"Edit", praat_DEPTH_1 + praat_HIDDEN, GRAPHICS_FormantGrid_draw);

	praat_addAction1 (classIntensity, 0, U"To TextGrid (silences)...", U"To IntensityTier (valleys)", 0, NEW_Intensity_to_TextGrid_silences);
	praat_addAction1 (classIntensityTier, 0, U"To TextGrid (silences)...", nullptr, 0, NEW_IntensityTier_to_TextGrid_silences);
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

	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFF file...", U"Save as NIST file...", 1, SAVE_LongSounds_saveAsStereoAIFFFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFF file...", U"Write to NIST file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_saveAsStereoAIFFFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFC file...", U"Save as stereo AIFF file...", 1, SAVE_LongSounds_saveAsStereoAIFCFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFC file...", U"Write to stereo AIFF file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_saveAsStereoAIFCFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo WAV file...", U"Save as stereo AIFC file...", 1, SAVE_LongSounds_saveAsStereoWAVFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo WAV file...", U"Write to stereo AIFC file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_saveAsStereoWAVFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NeXt/Sun file...", U"Save as stereo WAV file...", 1, SAVE_LongSounds_saveAsStereoNeXtSunFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NeXt/Sun file...", U"Write to stereo WAV file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_saveAsStereoNeXtSunFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NIST file...", U"Save as stereo NeXt/Sun file...", 1, SAVE_LongSounds_saveAsStereoNISTFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NIST file...", U"Write to stereo NeXt/Sun file...", praat_HIDDEN + praat_DEPTH_1, SAVE_LongSounds_saveAsStereoNISTFile);

	praat_addAction1 (classLtas, 0, U"Report spectral tilt...", U"Get slope...", 1, INFO_Ltas_reportSpectralTilt);

	praat_addAction1 (classMatrix, 0, U"Scatter plot...", U"Paint cells...", 1, GRAPHICS_Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, U"Draw as squares...", U"Scatter plot...", 1, GRAPHICS_Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, U"Draw distribution...", U"Draw as squares...", 1, GRAPHICS_Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, U"Draw cumulative distribution...", U"Draw distribution...", 1, GRAPHICS_Matrix_drawCumulativeDistribution);
	praat_addAction1 (classMatrix, 0, U"Get mean...", U"Get sum", 1, REAL_Matrix_getMean);
	praat_addAction1 (classMatrix, 0, U"Get standard deviation...", U"Get mean...", 1, REAL_Matrix_getStandardDeviation);
	praat_addAction1 (classMatrix, 0, U"Mathematical -", U"Get standard deviation...", 1, nullptr);
	praat_addAction1 (classMatrix, 0, U"Get norm...", U"Mathematical -", 2, REAL_Matrix_getNorm);
	
	praat_addAction1 (classMatrix, 0, U"Transpose", U"Synthesize", 0, NEW_Matrix_transpose);
	praat_addAction1 (classMatrix, 0, U"Solve equation...", U"Analyse", 0, NEW_Matrix_solveEquation);
	praat_addAction1 (classMatrix, 2, U"Solve matrix equation...", U"Solve equation...", 0, NEW_Matrix_solveMatrixEquation);
	praat_addAction1 (classMatrix, 0, U"To PCA (by rows)", U"Solve matrix equation...", 0, NEW_Matrix_to_PCA_byRows);
	praat_addAction1 (classMatrix, 0, U"To PCA (by columns)", U"To PCA (by rows)", 0, NEW_Matrix_to_PCA_byColumns);
	praat_addAction1 (classMatrix, 0, U"To PatternList...", U"To VocalTract", 1, NEW_Matrix_to_PatternList);
	praat_addAction1 (classMatrix, 0, U"To Pattern...", U"*To PatternList...", praat_DEPRECATED_2016, NEW_Matrix_to_PatternList);
	praat_addAction1 (classMatrix, 0, U"To ActivationList", U"To PatternList...", 1, NEW_Matrix_to_ActivationList);
	praat_addAction1 (classMatrix, 0, U"To Activation", U"*To ActivationList", praat_DEPRECATED_2016, NEW_Matrix_to_ActivationList);
	praat_addAction1 (classMatrix, 0, U"To Eigen", U"Eigen", praat_HIDDEN, NEW_Matrix_to_Eigen);
	praat_addAction1 (classMatrix, 0, U"To SVD", U"To Eigen", praat_HIDDEN, NEW_Matrix_to_SVD);
	praat_addAction1 (classMatrix, 0, U"To NMF (m.u.)...", U"To SVD", praat_HIDDEN, NEW_Matrix_to_NMF_mu);
	praat_addAction1 (classMatrix, 0, U"To NMF (ALS)...", U"To SVD", praat_HIDDEN, NEW_Matrix_to_NMF_als);
	praat_addAction1 (classMatrix, 0, U"Eigen (complex)", U"Eigen", praat_HIDDEN, NEWTIMES2_Matrix_eigen_complex);
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

	praat_addAction1 (classNMF, 0, U"NMF help", nullptr, 0, HELP_NMF_help);
	praat_addAction1 (classNMF, 0, U"To Matrix", nullptr, 0, NEW_NMF_to_Matrix);
	
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Get Euclidean distance", nullptr, 0, REAL_NMF_Matrix_getEuclideanDistance);
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Improve factorization (ALS)...", nullptr, 0, MODIFY_NMF_Matrix_improveFactorization_als);
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Improve factorization (m.u.)...", nullptr, 0, MODIFY_NMF_Matrix_improveFactorization_mu);
	
	praat_addAction1 (classPatternList, 0, U"Draw", nullptr, 0, 0);
	praat_addAction1 (classPatternList, 0, U"Draw...", nullptr, 0, GRAPHICS_PatternList_draw);
	praat_PatternList_query_init (classPatternList);
	praat_addAction1 (classPatternList, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classPatternList, 0, U"Formula...", nullptr, 1, MODIFY_PatternList_formula);
	praat_addAction1 (classPatternList, 0, U"Set value...", nullptr, 1, MODIFY_PatternList_setValue);
	praat_addAction1 (classPatternList, 0, U"Get all values", nullptr, 0, NUMMAT_PatternList_getAllValues);
	praat_addAction1 (classPatternList, 0, U"To Matrix", nullptr, 0, NEW_PatternList_to_Matrix);

	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To TableOfReal", nullptr, 0, NEW1_Matrix_Categories_to_TableOfReal);
	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To Discriminant", nullptr, 0, NEW1_PatternList_Categories_to_Discriminant);

	praat_addAction1 (classPCA, 0, U"PCA help", nullptr, 0, HELP_PCA_help);
	praat_addAction1 (classPCA, 0, DRAW_BUTTON, nullptr, 0, 0);
	praat_Eigen_draw_init (classPCA);
	praat_addAction1 (classPCA, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_Eigen_query_init (classPCA);
	praat_addAction1 (classPCA, 1, U"-- pca --", nullptr, 1, 0);
	praat_addAction1 (classPCA, 1, U"Get centroid element...", nullptr, 1, REAL_PCA_getCentroidElement);
	praat_addAction1 (classPCA, 1, U"Get equality of eigenvalues...", nullptr, 1, REAL_PCA_getEqualityOfEigenvalues);
	praat_addAction1 (classPCA, 1, U"Get fraction variance accounted for...", nullptr, 1, REAL_PCA_getFractionVAF);
	praat_addAction1 (classPCA, 1, U"Get number of components (VAF)...", nullptr, 1, INTEGER_PCA_getNumberOfComponents_VAF);
	praat_addAction1 (classPCA, 2, U"Get angle between pc1-pc2 planes", nullptr, 1, REAL_PCAs_getAngleBetweenPc1Pc2Plane_degrees);
	praat_addAction1 (classPCA, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classPCA, 1, U"Invert eigenvector...", nullptr, 1, MODIFY_PCA_invertEigenvector);
	praat_addAction1 (classPCA, 0, U"Align eigenvectors", nullptr, 1, MODIFY_Eigens_alignEigenvectors);
	praat_addAction1 (classPCA, 0, U"Extract -", nullptr, 0, 0);
		praat_addAction1 (classPCA, 0, U"Extract eigenvector...", nullptr, 1, NEW_PCA_extractEigenvector);
		praat_addAction1 (classPCA, 0, U"Extract Eigen", nullptr, 1, NEW_PCA_extractEigen);
	praat_addAction1 (classPCA, 2, U"To Procrustes...", nullptr, 0, NEW1_PCAs_to_Procrustes);
	praat_addAction1 (classPCA, 0, U"To TableOfReal (reconstruct 1)...", nullptr, 0, NEW_PCA_to_TableOfReal_reconstruct1);

	praat_addAction1 (classPCA, 0, U"& TableOfReal: To Configuration?", nullptr, praat_NO_API, HINT_hint_PCA_TableOfReal_to_Configuration);
	praat_addAction1 (classPCA, 0, U"& Configuration (reconstruct)?", nullptr, praat_NO_API, HINT_hint_PCA_Configuration_to_TableOfReal_reconstruct);
	praat_addAction1 (classPCA, 0, U"& Covariance: Project?", nullptr, praat_NO_API, HINT_hint_PCA_Covariance_Project);
	praat_addAction2 (classPCA, 1, classConfiguration, 1, U"To TableOfReal (reconstruct)", nullptr, 0, NEW_PCA_Configuration_to_TableOfReal_reconstruct);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (pc)...", nullptr, praat_HIDDEN, NEW1_PCA_Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project rows)...", nullptr, 0, NEW1_PCA_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project columns)...", nullptr, 0, NEW1_PCA_Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classPatternList, 1, U"To Matrix (project rows)...", nullptr, 0, NEW1_PCA_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classSSCP, 1, U"Project", nullptr, 0, NEW1_PCA_SSCP_project);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (project rows)...", nullptr, 0, NEW1_PCA_TableOfReal_to_TableOfReal_projectRows);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal...", U"*To TableOfReal (project rows)...", praat_DEPRECATED_2016, NEW1_PCA_TableOfReal_to_TableOfReal_projectRows);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0, NEW1_PCA_TableOfReal_to_Configuration);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (z-scores)...", nullptr, 0, NEW1_PCA_TableOfReal_to_TableOfReal_zscores);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"Get fraction variance...", nullptr, 0, REAL_PCA_TableOfReal_getFractionVariance);
	praat_addAction2 (classPCA, 1, classCovariance, 1, U"Project", nullptr, 0, NEW1_Eigen_Covariance_project);

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
	praat_addAction1 (classPermutation, 1, U"Jump...", nullptr, 1, MODIFY_Permutation_tableJump);
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

	praat_addAction2 (classRoots, 1, classPolynomial, 1, U"Polish roots", nullptr, 0, MODIFY_Roots_Polynomial_polish);

	praat_addAction1 (classSound, 0, U"To TextGrid (silences)...", U"To IntervalTier", 1, NEW_Sound_to_TextGrid_detectSilences);
    praat_addAction1 (classSound, 0, U"Play one channel...", U"Play", praat_HIDDEN, PLAY_Sound_playOneChannel);
    praat_addAction1 (classSound, 0, U"Play as frequency shifted...", U"Play", praat_HIDDEN, PLAY_Sound_playAsFrequencyShifted);
	praat_addAction1 (classSound, 0, U"Draw where...", U"Draw...", 1, GRAPHICS_Sound_drawWhere);
	//	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sound_paintWhere);
	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", 1, GRAPHICS_Sound_paintWhere);
	//	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", praat_DEPTH_1 | praat_HIDDEN, DO_Sounds_paintEnclosed);
	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", 1, GRAPHICS_Sounds_paintEnclosed);

	praat_addAction1 (classSound, 1, U"Get nearest level crossing...", U"Get nearest zero crossing...", 1, REAL_Sound_getNearestLevelCrossing);

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
	praat_addAction1 (classSound, 0, U"Remove noise...", U"Filter (formula)...", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_removeNoise);
	praat_addAction1 (classSound, 0, U"Reduce noise...", U"Filter (formula)...", praat_DEPTH_1, NEW_Sound_reduceNoise);

	praat_addAction1 (classSound, 0, U"Change gender...", U"Deepen band modulation...", 1, NEW_Sound_changeGender);

	praat_addAction1 (classSound, 0, U"Change speaker...", U"Deepen band modulation...", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_changeSpeaker);
	praat_addAction1 (classSound, 0, U"Copy channel ranges...", U"Extract all channels", praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_copyChannelRanges);
	praat_addAction1 (classSound, 0 /* TODO produces 2N objects */, U"Trim silences...", U"Resample...", praat_DEPTH_1 | praat_HIDDEN, NEWMANY_Sound_trimSilences);
	praat_addAction1 (classSound, 0, U"To KlattGrid (simple)...", U"To Manipulation...", 1, NEW_Sound_to_KlattGrid_simple);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To FormantFilter...", nullptr, praat_DEPRECATED_2014, NEW1_Sound_Pitch_to_FormantFilter);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To Spectrogram (pitch-dependent)...", nullptr, 0,NEW1_Sound_Pitch_to_Spectrogram);

	praat_addAction2 (classSound, 1, classPitch, 1, U"Change gender...", nullptr, 0, NEW1_Sound_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, U"Change speaker...", nullptr, praat_HIDDEN, NEW1_Sound_Pitch_changeSpeaker);
	praat_addAction2 (classSound, 1, classIntervalTier, 1, U"Cut parts matching label...", nullptr, 0, NEW1_Sound_IntervalTier_cutPartsMatchingLabel);

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
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get language name", nullptr, 1, INFO_SpeechSynthesizer_getLanguageName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice name", nullptr, 1, INFO_SpeechSynthesizer_getVoiceName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get phoneme set name", nullptr, 1, INFO_SpeechSynthesizer_getPhonemeSetName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice variant", nullptr, praat_DEPRECATED_2017, INFO_SpeechSynthesizer_getVoiceName);
	praat_addAction1 (classSpeechSynthesizer, 0, MODIFY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classSpeechSynthesizer, 0, U"Modify phoneme set...", nullptr, praat_DEPTH_1, MODIFY_SpeechSynthesizer_modifyPhonemeSet);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set text input settings...", nullptr, 1, MODIFY_SpeechSynthesizer_setTextInputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Speech output settings...", nullptr, 1, MODIFY_SpeechSynthesizer_speechOutputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Estimate speech rate from speech...", nullptr, 1, MODIFY_SpeechSynthesizer_estimateSpeechRateFromSpeech);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set speech output settings...", nullptr, praat_DEPTH_1 |praat_DEPRECATED_2017, MODIFY_SpeechSynthesizer_setSpeechOutputSettings);

	praat_addAction2 (classSpeechSynthesizer, 1, classTextGrid, 1, U"To Sound...", nullptr, 0, NEWMANY_SpeechSynthesizer_TextGrid_to_Sound);
	praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align)...", nullptr, 0, NEW1_SpeechSynthesizer_Sound_TextGrid_align);
    praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align,trim)...", nullptr, 0, NEW1_SpeechSynthesizer_Sound_TextGrid_align2);

	praat_addAction1 (classSSCP, 0, U"SSCP help", nullptr, 0, HELP_SSCP_help);
	praat_TableOfReal_init2 (classSSCP);
	praat_removeAction (classSSCP, nullptr, nullptr, U"Append");
	praat_addAction1 (classSSCP, 0, U"Draw sigma ellipse...", DRAW_BUTTON, 1, GRAPHICS_SSCP_drawSigmaEllipse);
	praat_addAction1 (classSSCP, 0, U"Draw confidence ellipse...", DRAW_BUTTON, 1, GRAPHICS_SSCP_drawConfidenceEllipse);
	praat_SSCP_query_init (classSSCP);
	praat_addAction1 (classSSCP, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, REAL_SSCP_getDiagonality_bartlett);
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

	praat_addAction1 (classSVD, 0, U"SVD help", nullptr, 0, HELP_SVD_help);
	praat_addAction1 (classSVD, 0, QUERY_BUTTON, nullptr, 0, 0);
	praat_addAction1 (classSVD, 1, U"Get number of rows", nullptr, 1, INTEGER_SVD_getNumberOfRows);
	praat_addAction1 (classSVD, 1, U"Get number of columns", nullptr, 1, INTEGER_SVD_getNumberOfColumns);
	praat_addAction1 (classSVD, 1, U"Get condition number", nullptr, 1, REAL_SVD_getConditionNumber);
	praat_addAction1 (classSVD, 0, U"-- singular values ---", nullptr, 1, nullptr);
	praat_addAction1 (classSVD, 1, U"Get singular value...", nullptr, 1, REAL_SVD_getSingularValue);
	praat_addAction1 (classSVD, 1, U"Get sum of singular values...", nullptr, 1, REAL_SVD_getSumOfSingularValues);
	praat_addAction1 (classSVD, 1, U"Get sum of singular values (fraction)...", nullptr, 1, REAL_SVD_getSumOfSingularValuesAsFractionOfTotal);
	praat_addAction1 (classSVD, 1, U"Get minimum number of singular values...", nullptr, 1, INTEGER_SVD_getMinimumNumberOfSingularValues);
	
	praat_addAction1 (classSVD, 0, U"To TableOfReal...", nullptr, 0, NEW_SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, U"To Matrix...", nullptr, 0, NEW_SVD_to_Matrix);
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
	praat_addAction1 (classTable, 1, U"Report robust statistics...", U"Report two-way anova...", praat_DEPTH_1 | praat_HIDDEN, INFO_Table_reportRobustStatistics);
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
	praat_addAction1 (classTableOfReal, 0, U"To SSCP (row weights)...", nullptr, 1, NEW_TableOfReal_to_SSCP_rowWeights);
	praat_addAction1 (classTableOfReal, 0, U"To Covariance", nullptr, 1, NEW_TableOfReal_to_Covariance);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation", nullptr, 1, NEW_TableOfReal_to_Correlation);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation (rank)", nullptr, 1, NEW_TableOfReal_to_Correlation_rank);
	praat_addAction1 (classTableOfReal, 0, U"To CCA...", nullptr, 1, NEW_TableOfReal_to_CCA);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (means by row labels)...", nullptr, 1, NEW_TableOfReal_meansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (medians by row labels)...", nullptr, 1, NEW_TableOfReal_mediansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (rank columns)...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW_TableOfReal_to_TableOfReal_rankColumns);
	praat_addAction1 (classTableOfReal, 0, U"-- configurations --", nullptr, 1, 0);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (pca)...", nullptr, 1, NEW_TableOfReal_to_Configuration_pca);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (lda)...", nullptr, 1, NEW_TableOfReal_to_Configuration_lda);
	praat_addAction1 (classTableOfReal, 2, U"-- between tables --", nullptr, 1, 0);
	praat_addAction1 (classTableOfReal, 2, U"To TableOfReal (cross-correlations)...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_TableOfReal_TableOfReal_crossCorrelations);

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

	praat_addAction2 (classStrings, 1, classPermutation, 1, U"Permute strings", nullptr, 0, NEW1_Strings_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, U"Permute rows", nullptr, 0, NEW1_TableOfReal_Permutation_permuteRows);

	praat_addAction1 (classTextGrid, 1, U"Get total duration of intervals where...", U"Count intervals where...", 2, REAL_TextGrid_getTotalDurationOfIntervalsWhere);
	praat_addAction1 (classTextGrid, 0, U"Extend time...", U"Scale times...", 2, MODIFY_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, U"Set tier name...", U"Remove tier...", 1, MODIFY_TextGrid_setTierName);
	praat_addAction1 (classTextGrid, 0, U"Replace interval texts...", U"Set interval text...", 2, MODIFY_TextGrid_replaceIntervalTexts);
	praat_addAction1 (classTextGrid, 0, U"Replace interval text...", U"*Replace interval texts...", praat_DEPTH_2 | praat_DEPRECATED_2018, MODIFY_TextGrid_replaceIntervalTexts);
	praat_addAction1 (classTextGrid, 0, U"Replace point texts...", U"Set point text...", 2, MODIFY_TextGrid_replacePointTexts);
	praat_addAction1 (classTextGrid, 0, U"Replace point text...", U"*Replace point texts...", praat_DEPTH_2 | praat_DEPRECATED_2018, MODIFY_TextGrid_replacePointTexts);
	praat_addAction1 (classTextGrid, 2, U"To Table (text alignment)...", U"Extract part...", 0, NEW1_TextGrids_to_Table_textAlignment);
	praat_addAction1 (classTextGrid, 0, U"To DurationTier...", U"Concatenate", 0, NEW_TextGrid_to_DurationTier);
	praat_addAction2 (classTextGrid, 1, classDurationTier, 1, U"To TextGrid (scale times)", nullptr, 0, NEW_TextGrid_DurationTier_to_TextGrid);
	praat_addAction2 (classTextGrid, 2, classEditCostsTable, 1, U"To Table (text alignment)...", nullptr, 0, NEW1_TextGrids_EditCostsTable_to_Table_textAlignment);

	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)

	INCLUDE_LIBRARY (praat_uvafon_MDS_init)
	INCLUDE_LIBRARY (praat_KlattGrid_init)
	INCLUDE_LIBRARY (praat_HMM_init)
	INCLUDE_LIBRARY (praat_BSS_init)
}

/* End of file praat_David.cpp */
