/* praat_BSS_init.cpp
 *
 * Copyright (C) 2010-2017 David Weenink, 2015 Paul Boersma
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
	praat_TimeFunction_RANGE (fromTime,toTime)
	REALVAR (lagTime, U"Lag step (s)", U"0.05")
	TEXTVAR (channelRanges, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCrossCorrelationTable result = EEG_to_CrossCorrelationTable (me, fromTime, toTime, lagTime, channelRanges);
	CONVERT_EACH_END (my name, U"_", round (lagTime * 1000.0))
}

FORM (NEW_EEG_to_Covariance, U"EEG: To Covariance", U"EEG: To Covariance...") {
	praat_TimeFunction_RANGE (fromTime,toTime)
	TEXTVAR (channelRanges, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCovariance result = EEG_to_Covariance (me, fromTime, toTime, channelRanges);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_CrossCorrelationTableList, U"EEG: To CrossCorrelationTableList", U"EEG: To CrossCorrelationTableList...") {
	praat_TimeFunction_RANGE (fromTime,toTime)
	POSITIVEVAR (lagTime, U"Lag step (s)", U"0.02")
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTVAR (channelRanges, U"Channel ranges", U"1:64")
	OK
DO
	CONVERT_EACH (EEG)
		autoCrossCorrelationTableList result = EEG_to_CrossCorrelationTableList (me, fromTime, toTime,
			lagTime, numberOfCrossCorrelations, channelRanges);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_EEG_bss, U"EEG: To EEG (bss)", U"EEG: To EEG (bss)...") {
	praat_TimeFunction_RANGE (fromTime,toTime)
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVEVAR (lagTime, U"Lag step (s)", U"0.002")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTVAR (channels, U"Channel ranges", U"1:64")
	LABEL (U"", U"Pre-whitening parameters")
	OPTIONMENUVAR (whiteningMethod, U"Whitening method", 1)
		OPTION (U"No whitening")
		OPTION (U"Covariance")
		OPTION (U"Correlation")
	LABEL (U"", U"Iteration parameters")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (EEG)
		autoEEG result = EEG_to_EEG_bss (me, fromTime, toTime, numberOfCrossCorrelations, lagTime, channels, whiteningMethod - 1, diagonalizationMethod, maximumNumberOfIterations, tolerance);
	CONVERT_EACH_END (my name, U"_bss")
}


FORM (NEW_EEG_to_PCA, U"EEG: To PCA", U"EEG: To PCA...") {
	praat_TimeFunction_RANGE (fromTime,toTime)
	TEXTVAR (channelRanges, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OPTIONMENUVAR (method, U"Method", 1)
		OPTION (U"Covariance")
		OPTION (U"Correlation")
	OK
DO
	CONVERT_EACH (EEG)
		autoPCA result = EEG_to_PCA (me, fromTime, toTime, channelRanges, (method == 2));
	CONVERT_EACH_END (my name)
}

FORM (NEW1_EEG_and_PCA_to_EEG_principalComponents, U"EEG & PCA: To EEG (principal components)", U"EEG & PCA: To EEG (principal components)...") {
	INTEGERVAR (numberOfComponents, U"Number of components", U"0 (= all)")
	OK
DO
	CONVERT_TWO (EEG, PCA)
		autoEEG result = EEG_and_PCA_to_EEG_principalComponents (me, you, numberOfComponents);
	CONVERT_TWO_END (my name, U"_pc")
}

FORM (NEW1_EEG_and_PCA_to_EEG_whiten, U"EEG & PCA: To EEG (whiten)", U"EEG & PCA: To EEG (whiten)...") {
	INTEGERVAR (numberOfComponents, U"Number of components", U"0 (= all)")
	OK
DO
	CONVERT_TWO (EEG, PCA)
		autoEEG result = EEG_and_PCA_to_EEG_whiten (me, you, numberOfComponents);
	CONVERT_TWO_END (my name, U"_white");
}

FORM (NEW_EEG_to_Sound_modulated, U"EEG: To Sound (modulated)", nullptr) {
	POSITIVEVAR (baseFrequency, U"Base frequency (Hz)", U"100.0")
	POSITIVEVAR (channelBandwidth, U"Channel bandwidth (Hz)", U"100.0")
	TEXTVAR (channelRanges, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoSound result = EEG_to_Sound_modulated (me, baseFrequency, channelBandwidth, channelRanges);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_Sound_frequencyShifted, U"EEG: To Sound (frequency shifted)", 0) {
	NATURALVAR (channel, U"Channel", U"1")
	POSITIVEVAR (frequencyShift, U"Frequency shift (Hz)", U"100.0")
	POSITIVEVAR (samplingFrequency, U"Sampling frequecy (Hz)", U"11025.0")
	REALVAR (maximumAmplitude, U"Maximum amplitude", U"0.99")
	OK
DO
	CONVERT_EACH (EEG)
		autoSound result = EEG_to_Sound_frequencyShifted (me, channel, frequencyShift, samplingFrequency, maximumAmplitude);
	CONVERT_EACH_END (my name, U"_ch", channel)
}

/********************** CrossCorrelationTable(s) ******************/

FORM (NEW1_CrossCorrelationTableList_createTestSet, U"CrossCorrelationTableList: Create test set", U"CrossCorrelationTableList: Create test set...") {
	WORDVAR (name, U"Name", U"5x5")
	NATURALVAR (matrixDimension, U"Matrix dimension", U"5")
	NATURALVAR (numberOfMatrices, U"Number of matrices", U"20")
	BOOLEANVAR (firstIsPositiveDefinite, U"First is positive definite", true)
	REALVAR (sigma, U"Sigma", U"0.02")
	OK
DO
	CREATE_ONE
		autoCrossCorrelationTableList result = CrossCorrelationTableList_createTestSet (matrixDimension, numberOfMatrices, firstIsPositiveDefinite, sigma);
	CREATE_ONE_END (name)
}

FORM (NEW1_CrossCorrelationTable_createSimple, U"Create simple CrossCorrelationTable", nullptr) {
	WORDVAR (name, U"Name", U"ct")
	SENTENCEVAR (crosscorrelations_string, U"Cross correlations", U"1.0 0.0 1.0")
	SENTENCEVAR (centroid_string, U"Centroid", U"0.0 0.0")
	NATURALVAR (numberOfSamples, U"Number of samples", U"100")
	OK
DO
	CREATE_ONE
		autoCrossCorrelationTable result = CrossCorrelationTable_createSimple (crosscorrelations_string, centroid_string, numberOfSamples);
	CREATE_ONE_END (name)
}

FORM (NEW1_MixingMatrix_createSimple, U"Create simple MixingMatrix", U"MixingMatrix") {
	WORDVAR (name, U"Name", U"mm")
	NATURALVAR (numberOfInputs, U"Number of inputs", U"2")
	NATURALVAR (numberOfOutputs, U"Number of outputs", U"2")
	SENTENCEVAR (mixingCoefficients_string, U"Mixing coefficients", U"1.0 0.0 0.0 1.0")
	OK
DO
	CREATE_ONE
		autoMixingMatrix result = MixingMatrix_createSimple (numberOfOutputs, numberOfInputs, mixingCoefficients_string);
	CREATE_ONE_END (name)
}

DIRECT (HELP_CrossCorrelationTable_help) {
	HELP (U"CrossCorrelationTable");
}

FORM (NEW1_Sound_and_PCA_principalComponents, U"Sound & PCA: To Sound (principal components)", nullptr) {
	NATURALVAR (numberOfComponents, U"Number of components", U"10")
	OK
DO
	CONVERT_TWO (Sound, PCA)
		autoSound result = Sound_and_PCA_principalComponents (me, you, numberOfComponents);
	CONVERT_TWO_END (my name, U"_pc")
}

FORM (NEW1_Sound_and_PCA_whitenChannels, U"Sound & PCA: To Sound (white channels)", nullptr) {
	NATURALVAR (numberOfComponents, U"Number of components", U"10")
	OK
DO
	CONVERT_TWO (Sound, PCA)
		autoSound result = Sound_and_PCA_whitenChannels (me, you, numberOfComponents);
	CONVERT_TWO_END (my name, U"_white")
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
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_CrossCorrelationTable, U"Sound: To CrossCorrelationTable", U"Sound: To CrossCorrelationTable...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (lagStep, U"Lag step (s)", U"0.0")
    OK
DO
    CONVERT_EACH (Sound)
		autoCrossCorrelationTable result = Sound_to_CrossCorrelationTable (me, fromTime, toTime, lagStep);
    CONVERT_EACH_END (my name)
}

FORM (NEW1_Sounds_to_CrossCorrelationTable_combined, U"Sound: To CrossCorrelationTable (combined)", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (lagStep, U"Lag step (s)", U"0.0")
	OK
DO
	CONVERT_COUPLE (Sound)
		autoCrossCorrelationTable result = Sounds_to_CrossCorrelationTable_combined (me, you, fromTime, toTime, lagStep);
	CONVERT_COUPLE_END (my name, U"_", your name, U"_cc")
}

DIRECT (HELP_CrossCorrelationTableList_help) {
	HELP (U"CrossCorrelationTableList")
}

FORM (REAL_CrossCorrelationTableList_getDiagonalityMeasure, U"CrossCorrelationTableList: Get diagonality measure", U"CrossCorrelationTableList: Get diagonality measure...") {
	NATURALVAR (fromTable, U"First table", U"1")
	NATURALVAR (toTable, U"Last table", U"100")
	OK
DO
	NUMBER_ONE (CrossCorrelationTableList)
		double result = CrossCorrelationTableList_getDiagonalityMeasure (me, 0, fromTable, toTable);
	NUMBER_ONE_END (U" (= average sum of squared off-diagonal elements)")
}

FORM (NEW_CrossCorrelationTableList_extractCrossCorrelationTable, U"CrossCorrelationTableList: Extract one CrossCorrelationTable", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	CONVERT_EACH (CrossCorrelationTableList)
		if (index > my size) {
			Melder_throw (U"Index too large.");
		}
		autoCrossCorrelationTable result = Data_copy (my at [index]);
	CONVERT_EACH_END (my name, U"_", index)
}

FORM (NEW_CrossCorrelationTableList_to_Diagonalizer, U"CrossCorrelationTableList: To Diagonalizer", nullptr) {
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (CrossCorrelationTableList)
		autoDiagonalizer result = CrossCorrelationTableList_to_Diagonalizer (me, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name)
}

FORM (MODIFY_Diagonalizer_and_CrossCorrelationTableList_improveDiagonality, U"Diagonalizer & CrossCorrelationTableList: Improve diagonality", nullptr) {
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	MODIFY_FIRST_OF_TWO (Diagonalizer, CrossCorrelationTableList)
		Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (me, you, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	MODIFY_FIRST_OF_TWO_END	
}

FORM (REAL_CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure, U"CrossCorrelationTableList & Diagonalizer: Get diagonality measure", nullptr) {
	NATURALVAR (fromTable, U"First table", U"1")
	NATURALVAR (toTable, U"Last table", U"100")
	OK
DO
	NUMBER_TWO (CrossCorrelationTableList, Diagonalizer)
		double result = CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure (me, you, nullptr, fromTable, toTable);
	NUMBER_TWO_END (U" (= average sum of squared off-diagonal elements)")
}

DIRECT (NEW1_CrossCorrelationTable_and_Diagonalizer_diagonalize) {
	CONVERT_TWO (CrossCorrelationTable, Diagonalizer)
		autoCrossCorrelationTable result = CrossCorrelationTable_and_Diagonalizer_diagonalize (me, you);
	CONVERT_TWO_END (me -> name, U"_", you -> name)
}

DIRECT (NEW1_CrossCorrelationTableList_and_Diagonalizer_diagonalize) {
	CONVERT_TWO (CrossCorrelationTableList, Diagonalizer)
		autoCrossCorrelationTableList result = CrossCorrelationTableList_and_Diagonalizer_diagonalize (me, you);
	CONVERT_TWO_END (me -> name, U"_", you -> name);
}

FORM (MODIFY_CrossCorrelationTableList_and_MixingMatrix_improveUnmixing, U"", nullptr) {
	LABEL (U"", U"Iteration parameters")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	MODIFY_FIRST_OF_TWO (MixingMatrix, CrossCorrelationTableList)
		MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (me, you, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (NEW_Diagonalizer_to_MixingMatrix) {
	CONVERT_EACH (Diagonalizer)
		autoMixingMatrix result = Diagonalizer_to_MixingMatrix (me);
	CONVERT_EACH_END (my name)
}

DIRECT (MODIFY_MixingMatrix_setStandardChannelInterpretation) {
	MODIFY_EACH (MixingMatrix)
		MixingMatrix_setStandardChannelInterpretation (me);
	MODIFY_EACH_END
}

FORM (NEW_Sound_to_MixingMatrix, U"Sound: To MixingMatrix", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVEVAR (lagTime, U"Lag step (s)", U"0.002")
	LABEL (U"", U"Iteration parameters")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (Sound)
		autoMixingMatrix result = Sound_to_MixingMatrix (me, fromTime, toTime, numberOfCrossCorrelations, lagTime, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_CrossCorrelationTableList, U"Sound: To CrossCorrelationTableList", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVEVAR (lagTime, U"Lag step (s)", U"0.002")
	OK
DO
	CONVERT_EACH (Sound)
		autoCrossCorrelationTableList result = Sound_to_CrossCorrelationTableList (me, fromTime, toTime, lagTime, numberOfCrossCorrelations);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Sound_to_Sound_bss, U"Sound: To Sound (blind source separation)", U"Sound: To Sound (blind source separation)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVEVAR (lagStep, U"Lag step (s)", U"0.002")
	LABEL (U"", U"Iteration parameters")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_to_Sound_BSS (me, fromTime, toTime, numberOfCrossCorrelations, lagStep, maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name, U"_bss")
}

FORM (NEW_Sound_to_Sound_whiteChannels, U"Sound: To Sound (white channels)", U"Sound: To Sound (white channels)...") {
	POSITIVEVAR (varianceFraction, U"Variance fraction to keep", U"0.99")
    OK
DO
    if (varianceFraction > 1.0) varianceFraction = 1.0;
    long permille = (long) floor (varianceFraction * 1000.0);
    CONVERT_EACH (Sound)
		autoSound result = Sound_whitenChannels (me, varianceFraction);
    CONVERT_EACH_END (my name, U"_", permille);
}

DIRECT (PLAY_Sound_and_MixingMatrix_play) {
	FIND_TWO (Sound, MixingMatrix);
		Sound_and_MixingMatrix_play (me, you, nullptr, nullptr);
	END
}

DIRECT (NEW1_Sound_and_MixingMatrix_mix) {
	CONVERT_TWO (Sound, MixingMatrix)
		autoSound result = Sound_and_MixingMatrix_mix (me, you);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_Sound_and_MixingMatrix_mixPart, U"Sound & MixingMatrix: Mix part", U"MixingMatrix") {
	REALVAR (fromTime, U"left Time_range (s)", U"0.0")
	REALVAR (toTime, U"right Time_range (s)", U"0.0 (=all)")
	OK
DO
	if (toTime < fromTime) {
		Melder_throw (U"The start time must be lower than the end time.");
	}
	CONVERT_TWO (Sound, MixingMatrix)
		autoSound result = Sound_and_MixingMatrix_mixPart (me, you, fromTime, toTime);
	CONVERT_TWO_END (my name, U"_", your name)
}

DIRECT (NEW1_Sound_and_MixingMatrix_unmix) {
	CONVERT_TWO (Sound, MixingMatrix)
		autoSound result = Sound_and_MixingMatrix_unmix (me, you);
	CONVERT_TWO_END (my name, U"_unmixed");
}

DIRECT (NEW_TableOfReal_to_MixingMatrix) {
	CONVERT_EACH (TableOfReal)
		autoMixingMatrix result = TableOfReal_to_MixingMatrix (me);
	CONVERT_EACH_END (my name)
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

	praat_TableOfReal_init3 (classDiagonalizer);
	praat_addAction1 (classDiagonalizer, 0, U"To MixingMatrix", 0, 0,NEW_Diagonalizer_to_MixingMatrix);

	praat_addAction1 (classEEG, 0, U"To Sound (mc modulated)...", U"To ERPTier...", praat_HIDDEN, NEW_EEG_to_Sound_modulated);
	praat_addAction1 (classEEG, 0, U"To Sound (frequency shifted)...", U"To ERPTier...", 0, NEW_EEG_to_Sound_frequencyShifted);
	praat_addAction1 (classEEG, 0, U"To PCA...", U"To ERPTier...", 0, NEW_EEG_to_PCA);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTable...", U"To PCA...", praat_HIDDEN, NEW_EEG_to_CrossCorrelationTable);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTables...", U"To PCA...", praat_HIDDEN, NEW_EEG_to_CrossCorrelationTableList);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTableList...", U"To PCA...", praat_HIDDEN, NEW_EEG_to_CrossCorrelationTableList);

	praat_addAction1 (classEEG, 0, U"To Covariance...", U"To CrossCorrelationTable...", praat_HIDDEN, NEW_EEG_to_Covariance);
	praat_addAction1 (classEEG, 0, U"To EEG (bss)...", U"To CrossCorrelationTable...", praat_HIDDEN, NEW_EEG_to_EEG_bss);

	praat_addAction2 (classEEG, 1, classPCA, 1, U"To EEG (principal components)...", 0, 0, NEW1_EEG_and_PCA_to_EEG_principalComponents);
	praat_addAction2 (classEEG, 1, classPCA, 1, U"To EEG (whiten)...", 0, 0, NEW1_EEG_and_PCA_to_EEG_whiten);

	praat_TableOfReal_init3 (classMixingMatrix);
	//praat_addAction1 (classMixingMatrix, 0, U"-- set MixingMatrix --", U"Set column label (label)...", praat_DEPTH_1, nullptr);
	praat_addAction1 (classMixingMatrix, 0, U"Set standard channel interpretation", U"Set column label (label)...", praat_DEPTH_1, MODIFY_MixingMatrix_setStandardChannelInterpretation);

	praat_addAction1 (classSound, 0, U"To MixingMatrix...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_MixingMatrix);
    praat_addAction1 (classSound, 0, U"To CrossCorrelationTable...",  U"Resample...", 1, NEW_Sound_to_CrossCorrelationTable);
    praat_addAction1 (classSound, 0, U"To Covariance (channels)...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_Covariance_channels);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTables...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_CrossCorrelationTableList);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTableList...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_CrossCorrelationTableList);

	praat_addAction1 (classSound, 0, U"To Sound (bss)...", U"Resample...", 1, NEW_Sound_to_Sound_bss);
    praat_addAction1 (classSound, 0, U"To Sound (white channels)...", U"Resample...", 1, NEW_Sound_to_Sound_whiteChannels);
    praat_addAction1 (classSound, 2, U"To CrossCorrelationTable (combined)...",  U"Cross-correlate...", 1, NEW1_Sounds_to_CrossCorrelationTable_combined);

	praat_addAction1 (classTableOfReal, 0, U"To MixingMatrix", U"To Configuration", praat_HIDDEN, NEW_TableOfReal_to_MixingMatrix);

	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Play", 0, 0, PLAY_Sound_and_MixingMatrix_play);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Mix", 0, 0, NEW1_Sound_and_MixingMatrix_mix);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Mix part...", 0, 0, NEW1_Sound_and_MixingMatrix_mixPart);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Unmix", 0, 0, NEW1_Sound_and_MixingMatrix_unmix);

	praat_addAction2 (classSound, 1, classPCA, 1, U"To Sound (white channels)...", 0 , 0, NEW1_Sound_and_PCA_whitenChannels);
	praat_addAction2 (classSound, 1, classPCA, 1, U"To Sound (principal components)...", 0 , 0, NEW1_Sound_and_PCA_principalComponents);

	praat_addAction2 (classCrossCorrelationTable, 1, classDiagonalizer, 1, U"Diagonalize", 0 , 0, NEW1_CrossCorrelationTable_and_Diagonalizer_diagonalize);

	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Get diagonality measure...", 0 , 0, REAL_CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure);
	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Diagonalize", 0 , 0, NEW1_CrossCorrelationTableList_and_Diagonalizer_diagonalize);
	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Improve diagonality...", 0 , 0, MODIFY_Diagonalizer_and_CrossCorrelationTableList_improveDiagonality);

	praat_addAction2 (classCrossCorrelationTableList, 1, classMixingMatrix, 1, U"Improve unmixing...", 0 , 0, MODIFY_CrossCorrelationTableList_and_MixingMatrix_improveUnmixing);

	INCLUDE_MANPAGES (manual_BSS)
}

/* End of file praat_BSS_init.cpp */
