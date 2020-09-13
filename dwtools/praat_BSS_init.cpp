/* praat_BSS_init.cpp
 *
 * Copyright (C) 2010-2017 David Weenink, 2015,2018 Paul Boersma
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

#include "praat.h"
#include "praat_TimeFunction.h"

#include "EEG_extensions.h"
#include "ICA.h"
#include "praat_TimeFunction.h"
#include "Sound_and_MixingMatrix.h"
#include "Sound_and_PCA.h"

void praat_SSCP_as_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init3 (ClassInfo klas);


/******************** EEG ********************************************/

FORM (NEW_EEG_to_CrossCorrelationTable, U"EEG: To CrossCorrelationTable", U"EEG: To CrossCorrelationTable...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (lagTime, U"Lag time (s)", U"0.05")
	TEXTFIELD (channels, U"Channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCrossCorrelationTable result = EEG_to_CrossCorrelationTable (me, fromTime, toTime, lagTime, channels);
	CONVERT_EACH_END (my name.get(), U"_", Melder_iround (lagTime * 1000.0))
}

FORM (NEW_EEG_to_Covariance, U"EEG: To Covariance", U"EEG: To Covariance...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	TEXTFIELD (channels, U"Channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCovariance result = EEG_to_Covariance (me, fromTime, toTime, channels);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_EEG_to_CrossCorrelationTableList, U"EEG: To CrossCorrelationTableList", U"EEG: To CrossCorrelationTableList...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVE (lagStep, U"Lag step (s)", U"0.002")
	TEXTFIELD (channels, U"Channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCrossCorrelationTableList result = EEG_to_CrossCorrelationTableList (me, fromTime, toTime,
			numberOfCrossCorrelations, lagStep, channels);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_EEG_to_EEG_bss, U"EEG: To EEG (bss)", U"EEG: To EEG (bss)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVE (lagStep, U"Lag step (s)", U"0.002")
	TEXTFIELD (channels, U"Channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	LABEL (U"Pre-whitening parameters")
	OPTIONMENUx (whiteningMethod, U"Whitening method", 1, 0)
		OPTION (U"no whitening")
		OPTION (U"covariance")
		OPTION (U"correlation")
	LABEL (U"Iteration parameters")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	LOOP {
		iam_LOOP (EEG);
		autoEEG resultingEEG;
		autoMixingMatrix resultingMixingMatrix;
		EEG_to_EEG_bss (me, fromTime, toTime, numberOfCrossCorrelations, lagStep, channels,
			whiteningMethod, diagonalizationMethod, maximumNumberOfIterations, tolerance,
			& resultingEEG, & resultingMixingMatrix);
		praat_new (resultingEEG.move(), my name.get(), U"_bss");
		praat_new (resultingMixingMatrix.move(), my name.get());
	}
	END
}


FORM (NEW_EEG_to_PCA, U"EEG: To PCA", U"EEG: To PCA...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	TEXTFIELD (channels, U"Channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OPTIONMENU (method, U"Method", 1)
		OPTION (U"Covariance")
		OPTION (U"Correlation")
	OK
DO
	CONVERT_EACH (EEG)
		autoPCA result = EEG_to_PCA (me, fromTime, toTime, channels, (method == 2));
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_EEG_PCA_to_EEG_principalComponents, U"EEG & PCA: To EEG (principal components)", U"EEG & PCA: To EEG (principal components)...") {
	INTEGER (numberOfComponents, U"Number of components", U"0 (= all)")
	OK
DO
	CONVERT_TWO (EEG, PCA)
		autoEEG result = EEG_PCA_to_EEG_principalComponents (me, you, numberOfComponents);
	CONVERT_TWO_END (my name.get(), U"_pc")
}

FORM (NEW1_EEG_PCA_to_EEG_whiten, U"EEG & PCA: To EEG (whiten)", U"EEG & PCA: To EEG (whiten)...") {
	INTEGER (numberOfComponents, U"Number of components", U"0 (= all)")
	OK
DO
	CONVERT_TWO (EEG, PCA)
		autoEEG result = EEG_PCA_to_EEG_whiten (me, you, numberOfComponents);
	CONVERT_TWO_END (my name.get(), U"_white");
}

FORM (NEW_EEG_to_Sound_modulated, U"EEG: To Sound (modulated)", nullptr) {
	POSITIVE (baseFrequency, U"Base frequency (Hz)", U"100.0")
	POSITIVE (channelBandwidth, U"Channel bandwidth (Hz)", U"100.0")
	TEXTFIELD (channels, U"Channels:", U"1:64")
	LABEL (U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoSound result = EEG_to_Sound_modulated (me, baseFrequency, channelBandwidth, channels);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_EEG_to_Sound_frequencyShifted, U"EEG: To Sound (frequency shifted)", 0) {
	NATURAL (channel, U"Channel", U"1")
	POSITIVE (frequencyShift, U"Frequency shift (Hz)", U"100.0")
	POSITIVE (samplingFrequency, U"Sampling frequecy (Hz)", U"11025.0")
	REAL (maximumAmplitude, U"Maximum amplitude", U"0.99")
	OK
DO
	CONVERT_EACH (EEG)
		autoSound result = EEG_to_Sound_frequencyShifted (me, channel, frequencyShift, samplingFrequency, maximumAmplitude);
	CONVERT_EACH_END (my name.get(), U"_ch", channel)
}

/********************** CrossCorrelationTable(s) ******************/

FORM (NEW1_CrossCorrelationTableList_createTestSet, U"CrossCorrelationTableList: Create test set", U"CrossCorrelationTableList: Create test set...") {
	WORD (name, U"Name", U"5x5")
	NATURAL (matrixDimension, U"Matrix dimension", U"5")
	NATURAL (numberOfMatrices, U"Number of matrices", U"20")
	BOOLEAN (firstIsPositiveDefinite, U"First is positive definite", true)
	REAL (sigma, U"Sigma", U"0.02")
	OK
DO
	CREATE_ONE
		autoCrossCorrelationTableList result = CrossCorrelationTableList_createTestSet (matrixDimension, numberOfMatrices, firstIsPositiveDefinite, sigma);
	CREATE_ONE_END (name)
}

FORM (NEW1_CrossCorrelationTable_createSimple, U"Create simple CrossCorrelationTable", nullptr) {
	WORD (name, U"Name", U"ct")
	SENTENCE (crosscorrelations_string, U"Cross correlations", U"1.0 0.0 1.0")
	SENTENCE (centroid_string, U"Centroid", U"0.0 0.0")
	NATURAL (numberOfSamples, U"Number of samples", U"100")
	OK
DO
	CREATE_ONE
		autoCrossCorrelationTable result = CrossCorrelationTable_createSimple (crosscorrelations_string, centroid_string, numberOfSamples);
	CREATE_ONE_END (name)
}

DIRECT (HELP_MixingMatrix_help) {
	HELP (U"MixingMatrix");
}

FORM (NEW1_MixingMatrix_createSimple, U"Create simple MixingMatrix", U"Create simple MixingMatrix...") {
	WORD (name, U"Name", U"mm")
	NATURAL (numberOfInputs, U"Number of inputs", U"2")
	NATURAL (numberOfOutputs, U"Number of outputs", U"2")
	SENTENCE (mixingCoefficients_string, U"Mixing coefficients", U"1.0 0.0 0.0 1.0")
	OK
DO
	CREATE_ONE
		autoMixingMatrix result = MixingMatrix_createSimple (numberOfOutputs, numberOfInputs, mixingCoefficients_string);
	CREATE_ONE_END (name)
}

DIRECT (HELP_CrossCorrelationTable_help) {
	HELP (U"CrossCorrelationTable");
}

FORM (NEW1_Sound_PCA_principalComponents, U"Sound & PCA: To Sound (principal components)", nullptr) {
	NATURAL (numberOfComponents, U"Number of components", U"10")
	OK
DO
	CONVERT_TWO (Sound, PCA)
		autoSound result = Sound_PCA_principalComponents (me, you, numberOfComponents);
	CONVERT_TWO_END (my name.get(), U"_pc")
}

FORM (NEW1_Sound_PCA_whitenChannels, U"Sound & PCA: To Sound (white channels)", nullptr) {
	NATURAL (numberOfComponents, U"Number of components", U"10")
	OK
DO
	CONVERT_TWO (Sound, PCA)
		autoSound result = Sound_PCA_whitenChannels (me, you, numberOfComponents);
	CONVERT_TWO_END (my name.get(), U"_white")
}

DIRECT (NEW1_CrossCorrelationTables_to_CrossCorrelationTableList) {
	CONVERT_LIST (CrossCorrelationTable)
		autoCrossCorrelationTableList result = CrossCorrelationTables_to_CrossCorrelationTableList (& list);
	CONVERT_LIST_END (U"ct_", result -> size)
}

FORM (NEW_Sound_to_Covariance_channels, U"Sound: To Covariance (channels)", U"Sound: To Covariance (channels)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	CONVERT_EACH (Sound)
		autoCovariance result = Sound_to_Covariance_channels (me, fromTime, toTime);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_CrossCorrelationTable, U"Sound: To CrossCorrelationTable", U"Sound: To CrossCorrelationTable...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (lagStep, U"Lag step (s)", U"0.0")
    OK
DO
    CONVERT_EACH (Sound)
		autoCrossCorrelationTable result = Sound_to_CrossCorrelationTable (me, fromTime, toTime, lagStep);
    CONVERT_EACH_END (my name.get())
}

FORM (NEW1_Sounds_to_CrossCorrelationTable_combined, U"Sound: To CrossCorrelationTable (combined)", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (lagStep, U"Lag step (s)", U"0.0")
	OK
DO
	CONVERT_COUPLE (Sound)
		autoCrossCorrelationTable result = Sounds_to_CrossCorrelationTable_combined (me, you, fromTime, toTime, lagStep);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get(), U"_cc")
}

DIRECT (HELP_CrossCorrelationTableList_help) {
	HELP (U"CrossCorrelationTableList")
}

FORM (REAL_CrossCorrelationTableList_getDiagonalityMeasure, U"CrossCorrelationTableList: Get diagonality measure", U"CrossCorrelationTableList: Get diagonality measure...") {
	NATURAL (fromTable, U"First table", U"1")
	NATURAL (toTable, U"Last table", U"100")
	OK
DO
	NUMBER_ONE (CrossCorrelationTableList)
		double result = CrossCorrelationTableList_getDiagonalityMeasure (me, 0, fromTable, toTable);
	NUMBER_ONE_END (U" (= average sum of squared off-diagonal elements)")
}

FORM (NEW_CrossCorrelationTableList_extractCrossCorrelationTable, U"CrossCorrelationTableList: Extract one CrossCorrelationTable", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	CONVERT_EACH (CrossCorrelationTableList)
		if (index > my size) {
			Melder_throw (U"Index too large.");
		}
		autoCrossCorrelationTable result = Data_copy (my at [index]);
	CONVERT_EACH_END (my name.get(), U"_", index)
}

FORM (NEW_CrossCorrelationTableList_to_Diagonalizer, U"CrossCorrelationTableList: To Diagonalizer", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (CrossCorrelationTableList)
		autoDiagonalizer result = CrossCorrelationTableList_to_Diagonalizer (me, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_Diagonalizer_CrossCorrelationTableList_improveDiagonality, U"Diagonalizer & CrossCorrelationTableList: Improve diagonality", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	MODIFY_FIRST_OF_TWO (Diagonalizer, CrossCorrelationTableList)
		Diagonalizer_CrossCorrelationTableList_improveDiagonality (me, you, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	MODIFY_FIRST_OF_TWO_END	
}

FORM (REAL_CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure, U"CrossCorrelationTableList & Diagonalizer: Get diagonality measure", nullptr) {
	NATURAL (fromTable, U"First table", U"1")
	NATURAL (toTable, U"Last table", U"100")
	OK
DO
	NUMBER_TWO (CrossCorrelationTableList, Diagonalizer)
		double result = CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure (me, you, nullptr, fromTable, toTable);
	NUMBER_TWO_END (U" (= average sum of squared off-diagonal elements)")
}

DIRECT (NEW1_CrossCorrelationTable_Diagonalizer_diagonalize) {
	CONVERT_TWO (CrossCorrelationTable, Diagonalizer)
		autoCrossCorrelationTable result = CrossCorrelationTable_Diagonalizer_diagonalize (me, you);
	CONVERT_TWO_END (me -> name.get(), U"_", you -> name.get())
}

DIRECT (NEW1_CrossCorrelationTableList_Diagonalizer_diagonalize) {
	CONVERT_TWO (CrossCorrelationTableList, Diagonalizer)
		autoCrossCorrelationTableList result = CrossCorrelationTableList_Diagonalizer_diagonalize (me, you);
	CONVERT_TWO_END (me -> name.get(), U"_", you -> name.get());
}

FORM (MODIFY_CrossCorrelationTableList_MixingMatrix_improveUnmixing, U"", nullptr) {
	LABEL (U"Iteration parameters")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	MODIFY_FIRST_OF_TWO (MixingMatrix, CrossCorrelationTableList)
		MixingMatrix_CrossCorrelationTableList_improveUnmixing (me, you, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (HELP_Diagonalizer_help) {
	HELP (U"Diagonalizer");
}

DIRECT (NEW_Diagonalizer_to_MixingMatrix) {
	CONVERT_EACH (Diagonalizer)
		autoMixingMatrix result = Diagonalizer_to_MixingMatrix (me);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_MixingMatrix_multiplyInputChannel, U"MixingMatrix: Multiply input channel", U"MixingMatrix: Multiply input channel...") {
	NATURAL (inputChannel, U"Input channel number", U"1")
	REAL  (value, U"Multiply by", U"1.0")
	OK
DO
	MODIFY_EACH (MixingMatrix)
		MixingMatrix_multiplyInputChannel (me, inputChannel, value);
	MODIFY_EACH_END
}

DIRECT (MODIFY_MixingMatrix_setStandardChannelInterpretation) {
	MODIFY_EACH (MixingMatrix)
		MixingMatrix_setStandardChannelInterpretation (me);
	MODIFY_EACH_END
}

DIRECT (NEW1_MixingMatrix_to_Diagonalizer) {
	CONVERT_EACH (MixingMatrix)
		autoDiagonalizer result = MixingMatrix_to_Diagonalizer (me);
	CONVERT_EACH_END (my name.get())
}
FORM (NEW_Sound_to_MixingMatrix, U"Sound: To MixingMatrix", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVE (lagStep, U"Lag step (s)", U"0.002")
	LABEL (U"Iteration parameters")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (Sound)
		autoMixingMatrix result = Sound_to_MixingMatrix (me,
			fromTime, toTime, numberOfCrossCorrelations, lagStep,
			maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_Sound_MixingMatrix_improveUnmixing, U"", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVE (lagStep, U"Lag step (s)", U"0.002")
	LABEL (U"Iteration parameters")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
		MODIFY_FIRST_OF_TWO (MixingMatrix, Sound)
		MixingMatrix_Sound_improveUnmixing (me, you,
			fromTime, toTime, numberOfCrossCorrelations, lagStep,
			maximumNumberOfIterations, tolerance, diagonalizationMethod);
		MODIFY_FIRST_OF_TWO_END
}

FORM (NEW_Sound_to_CrossCorrelationTableList, U"Sound: To CrossCorrelationTableList", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVE (lagStep, U"Lag step (s)", U"0.002")
	OK
DO
	CONVERT_EACH (Sound)
		autoCrossCorrelationTableList result = Sound_to_CrossCorrelationTableList (me,
			fromTime, toTime, numberOfCrossCorrelations, lagStep);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Sound_bss, U"Sound: To Sound (blind source separation)", U"Sound: To Sound (blind source separation)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVE (lagStep, U"Lag step (s)", U"0.002")
	LABEL (U"Iteration parameters")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_to_Sound_BSS (me,
			fromTime, toTime, numberOfCrossCorrelations, lagStep,
			maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name.get(), U"_bss")
}

FORM (NEW_Sound_to_Sound_whiteChannels, U"Sound: To Sound (white channels)", U"Sound: To Sound (white channels)...") {
	POSITIVE (varianceFraction, U"Variance fraction to keep", U"0.99")
    OK
DO
    if (varianceFraction > 1.0) varianceFraction = 1.0;
    integer permille = Melder_ifloor (varianceFraction * 1000.0);
    CONVERT_EACH (Sound)
		autoSound result = Sound_whitenChannels (me, varianceFraction);
    CONVERT_EACH_END (my name.get(), U"_", permille);
}

DIRECT (PLAY_Sound_MixingMatrix_play) {
	FIND_TWO (Sound, MixingMatrix);
		Sound_MixingMatrix_play (me, you, nullptr, nullptr);
	END
}

DIRECT (NEW1_Sound_MixingMatrix_mix) {
	CONVERT_TWO (Sound, MixingMatrix)
		autoSound result = Sound_MixingMatrix_mix (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Sound_MixingMatrix_mixPart, U"Sound & MixingMatrix: Mix part", U"MixingMatrix") {
	REAL (fromTime, U"left Time_range (s)", U"0.0")
	REAL (toTime, U"right Time_range (s)", U"0.0 (=all)")
	OK
DO
	if (toTime < fromTime) {
		Melder_throw (U"The start time should be lower than the end time.");
	}
	CONVERT_TWO (Sound, MixingMatrix)
		autoSound result = Sound_MixingMatrix_mixPart (me, you, fromTime, toTime);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_Sound_MixingMatrix_unmix) {
	CONVERT_TWO (Sound, MixingMatrix)
		autoSound result = Sound_MixingMatrix_unmix (me, you);
	CONVERT_TWO_END (my name.get(), U"_unmixed");
}

DIRECT (NEW_TableOfReal_to_MixingMatrix) {
	CONVERT_EACH (TableOfReal)
		autoMixingMatrix result = TableOfReal_to_MixingMatrix (me);
	CONVERT_EACH_END (my name.get())
}

void praat_BSS_init ();
void praat_BSS_init () {
	Thing_recognizeClassesByName (classDiagonalizer, classMixingMatrix, classCrossCorrelationTable, classCrossCorrelationTableList, nullptr);
	Thing_recognizeClassByOtherName (classCrossCorrelationTableList, U"CrossCorrelationTables");

	praat_addMenuCommand (U"Objects", U"New", U"Create simple CrossCorrelationTable...", U"Create simple Covariance...", praat_HIDDEN + praat_DEPTH_1, NEW1_CrossCorrelationTable_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create test CrossCorrelationTableList...", U"Create simple CrossCorrelationTable...", praat_HIDDEN + praat_DEPTH_1, NEW1_CrossCorrelationTableList_createTestSet);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple MixingMatrix...", U"Create test CrossCorrelationTableList...", praat_HIDDEN + praat_DEPTH_1, NEW1_MixingMatrix_createSimple);

	praat_addAction1 (classCrossCorrelationTable, 0, U"CrossCorrelationTable help", 0, 0, HELP_CrossCorrelationTable_help);
	praat_SSCP_as_TableOfReal_init (classCrossCorrelationTable);

	praat_addAction1 (classCrossCorrelationTable, 0, U"To CrossCorrelationTableList", nullptr, 0, NEW1_CrossCorrelationTables_to_CrossCorrelationTableList);
	praat_addAction1 (classCrossCorrelationTable, 0, U"To CrossCorrelationTables", U"*To CrossCorrelationTableList", praat_DEPRECATED_2015, NEW1_CrossCorrelationTables_to_CrossCorrelationTableList);

	praat_addAction1 (classCrossCorrelationTableList, 0, U"CrossCorrelationTableList help", 0, 0, HELP_CrossCorrelationTableList_help);
	praat_addAction1 (classCrossCorrelationTableList, 1, U"Extract CrossCorrelationTable...", 0, 0, NEW_CrossCorrelationTableList_extractCrossCorrelationTable);
	praat_addAction1 (classCrossCorrelationTableList, 1, U"Get diagonality measure...", 0, 0, REAL_CrossCorrelationTableList_getDiagonalityMeasure);
	praat_addAction1 (classCrossCorrelationTableList, 0, U"To Diagonalizer...", 0, 0, NEW_CrossCorrelationTableList_to_Diagonalizer);
	
	praat_addAction1 (classDiagonalizer, 0, U"Diagonalizer help", 0, 0, HELP_Diagonalizer_help);
	praat_TableOfReal_init3 (classDiagonalizer);
	praat_addAction1 (classDiagonalizer, 0, U"To MixingMatrix", 0, 0, NEW_Diagonalizer_to_MixingMatrix);

	praat_addAction1 (classEEG, 0, U"To Sound (mc modulated)...", U"To ERPTier...", praat_HIDDEN, NEW_EEG_to_Sound_modulated);
	praat_addAction1 (classEEG, 0, U"To Sound (frequency shifted)...", U"To ERPTier...", 0, NEW_EEG_to_Sound_frequencyShifted);
	praat_addAction1 (classEEG, 0, U"To PCA...", U"To ERPTier...", 0, NEW_EEG_to_PCA);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTable...", U"To PCA...", praat_HIDDEN, NEW_EEG_to_CrossCorrelationTable);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTables...", U"To PCA...", praat_HIDDEN, NEW_EEG_to_CrossCorrelationTableList);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTableList...", U"To PCA...", praat_HIDDEN, NEW_EEG_to_CrossCorrelationTableList);

	praat_addAction1 (classEEG, 0, U"To Covariance...", U"To CrossCorrelationTable...", praat_HIDDEN, NEW_EEG_to_Covariance);
	praat_addAction1 (classEEG, 0, U"To EEG (bss)...", U"To CrossCorrelationTable...", praat_HIDDEN, NEW_EEG_to_EEG_bss);

	praat_addAction2 (classEEG, 1, classPCA, 1, U"To EEG (principal components)...", 0, 0, NEW1_EEG_PCA_to_EEG_principalComponents);
	praat_addAction2 (classEEG, 1, classPCA, 1, U"To EEG (whiten)...", 0, 0, NEW1_EEG_PCA_to_EEG_whiten);

	praat_addAction1 (classMixingMatrix, 0, U"MixingMatrix help", 0, 0, HELP_MixingMatrix_help);
	praat_TableOfReal_init3 (classMixingMatrix);
		praat_addAction1 (classMixingMatrix, 0, U"Multiply input channel...", U"Set value...", praat_DEPTH_1, MODIFY_MixingMatrix_multiplyInputChannel);
		praat_removeAction (classMixingMatrix, nullptr, nullptr, U"Sort by label...");	
		praat_removeAction (classMixingMatrix, nullptr, nullptr, U"Sort by column...");	
	praat_addAction1 (classMixingMatrix, 0, U"Set standard channel interpretation", U"Set column label (label)...", praat_DEPTH_1, MODIFY_MixingMatrix_setStandardChannelInterpretation);
	praat_addAction1 (classMixingMatrix, 0, U"To Diagonalizer", U"To Matrix", praat_DEPTH_1, NEW1_MixingMatrix_to_Diagonalizer);

	praat_addAction1 (classSound, 0, U"To MixingMatrix...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_MixingMatrix);
    praat_addAction1 (classSound, 0, U"To CrossCorrelationTable...",  U"Resample...", 1, NEW_Sound_to_CrossCorrelationTable);
    praat_addAction1 (classSound, 0, U"To Covariance (channels)...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_Covariance_channels);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTables...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_CrossCorrelationTableList);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTableList...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_CrossCorrelationTableList);

	praat_addAction1 (classSound, 0, U"To Sound (bss)...", U"Resample...", 1, NEW_Sound_to_Sound_bss);
    praat_addAction1 (classSound, 0, U"To Sound (white channels)...", U"Resample...", 1, NEW_Sound_to_Sound_whiteChannels);
    praat_addAction1 (classSound, 2, U"To CrossCorrelationTable (combined)...",  U"Cross-correlate...", 1, NEW1_Sounds_to_CrossCorrelationTable_combined);

	praat_addAction1 (classTableOfReal, 0, U"To MixingMatrix", U"To Configuration", praat_HIDDEN, NEW_TableOfReal_to_MixingMatrix);

	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Play", 0, 0, PLAY_Sound_MixingMatrix_play);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Mix", 0, 0, NEW1_Sound_MixingMatrix_mix);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Mix part...", 0, 0, NEW1_Sound_MixingMatrix_mixPart);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Unmix", 0, 0, NEW1_Sound_MixingMatrix_unmix);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Improve unmixing...", 0, 0, MODIFY_Sound_MixingMatrix_improveUnmixing);

	praat_addAction2 (classSound, 1, classPCA, 1, U"To Sound (white channels)...", 0 , 0, NEW1_Sound_PCA_whitenChannels);
	praat_addAction2 (classSound, 1, classPCA, 1, U"To Sound (principal components)...", 0 , 0, NEW1_Sound_PCA_principalComponents);

	praat_addAction2 (classCrossCorrelationTable, 1, classDiagonalizer, 1, U"Diagonalize", 0 , 0, NEW1_CrossCorrelationTable_Diagonalizer_diagonalize);

	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Get diagonality measure...", 0 , 0, REAL_CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure);
	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Diagonalize", 0 , 0, NEW1_CrossCorrelationTableList_Diagonalizer_diagonalize);
	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Improve diagonality...", 0 , 0, MODIFY_Diagonalizer_CrossCorrelationTableList_improveDiagonality);

	praat_addAction2 (classCrossCorrelationTableList, 1, classMixingMatrix, 1, U"Improve unmixing...", 0 , 0, MODIFY_CrossCorrelationTableList_MixingMatrix_improveUnmixing);

	INCLUDE_MANPAGES (manual_BSS)
}

/* End of file praat_BSS_init.cpp */
