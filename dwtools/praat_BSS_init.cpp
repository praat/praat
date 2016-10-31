/* praat_BSS_init.cpp
 *
 * Copyright (C) 2010-2016 David Weenink, 2015 Paul Boersma
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
	djmw 20101003
	djmw 20110101 Latest modification
*/

#include "praat.h"
#include "praat_TimeFunction.h"

#include "EEG_extensions.h"
#include "ICA.h"
#include "praat_TimeFunction.h"
#include "Sound_and_PCA.h"

#undef iam
#define iam iam_LOOP

void praat_SSCP_as_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init3 (ClassInfo klas);


/******************** EEG ********************************************/

FORM (NEW_EEG_to_CrossCorrelationTable, U"EEG: To CrossCorrelationTable", U"EEG: To CrossCorrelationTable...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (lagTime, U"Lag step (s)", U"0.05")
	TEXTVAR (channels, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCrossCorrelationTable result = EEG_to_CrossCorrelationTable (me, fromTime, toTime, lagTime, channels);
	CONVERT_EACH_END (my name, U"_", round (lagTime*1000))
}

FORM (NEW_EEG_to_Covariance, U"EEG: To Covariance", U"EEG: To Covariance...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	TEXTVAR (channels, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoCovariance result = EEG_to_Covariance (me, fromTime, toTime, channels);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_CrossCorrelationTableList, U"EEG: To CrossCorrelationTableList", U"EEG: To CrossCorrelationTableList...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	POSITIVEVAR (lagTime, U"Lag step (s)", U"0.02")
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTVAR (channels, U"Channel ranges", U"1:64")
	OK
DO
	CONVERT_EACH (EEG)
		autoCrossCorrelationTableList result = EEG_to_CrossCorrelationTableList (me, fromTime, toTime, lagTime, numberOfCrossCorrelations, channels);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_EEG_bss, U"EEG: To EEG (bss)", U"EEG: To EEG (bss)...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
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
	praat_TimeFunction_RANGE(fromTime,toTime)
	TEXTVAR (channels, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OPTIONMENUVAR (use, U"Use", 1)
		OPTION (U"Covariance")
		OPTION (U"Correlation")
	OK
DO
	bool useCorrelation = use == 2;
	CONVERT_EACH (EEG)
		autoPCA result = EEG_to_PCA (me, fromTime, toTime, channels, useCorrelation);
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

FORM (NEW_EEG_to_Sound_modulated, U"EEG: To Sound (modulated)", 0) {
	POSITIVEVAR (fromFrequency, U"Start frequency (Hz)", U"100.0")
	POSITIVEVAR (toFrequency, U"Channel bandwidth (Hz)", U"100.0")
	TEXTVAR (channels, U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	CONVERT_EACH (EEG)
		autoSound result = EEG_to_Sound_modulated (me, fromFrequency, toFrequency, channels);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_Sound_frequencyShifted, U"EEG: To Sound (frequency shifted)", 0) {
	NATURALVAR (channel, U"Channel", U"1")
	POSITIVEVAR (frequencyShift, U"Frequency shift (Hz)", U"100.0")
	POSITIVEVAR (samplingFrequency, U"Sampling frequecy (Hz)", U"11025.0")
	REALVAR (maximumAmplitude, U"Maximum amplitude", U"0.99")
	OK
DO
	LOOP {
		iam (EEG);
		autoSound thee = EEG_to_Sound_frequencyShifted (me, channel, frequencyShift, samplingFrequency, maximumAmplitude);
		praat_new (thee.move(), my name, U"_ch", channel);
	}
END }

/********************** CrossCorrelationTable(s) ******************/

FORM (NEW_CrossCorrelationTableList_createTestSet, U"CrossCorrelationTableList: Create test set", U"CrossCorrelationTableList: Create test set...") {
	WORDVAR (name, U"Name", U"5x5")
	NATURALVAR (matrixDimension, U"Matrix dimension", U"5")
	NATURALVAR (numberOfMatrices, U"Number of matrices", U"20")
	BOOLEANVAR (firstIsPositiveDefinite, U"First is positive definite", 1)
	REALVAR (sigma, U"Sigma", U"0.02")
	OK
DO
autoCrossCorrelationTableList thee = CrossCorrelationTableList_createTestSet (matrixDimension, numberOfMatrices, firstIsPositiveDefinite, sigma);
	praat_new (thee.move(), name);
END }

FORM (NEW_CrossCorrelationTable_createSimple, U"Create simple CrossCorrelationTable", nullptr) {
	WORDVAR (name, U"Name", U"ct")
	SENTENCEVAR (crosscorrelations_string, U"Cross correlations", U"1.0 0.0 1.0")
	SENTENCEVAR (centroid_string, U"Centroid", U"0.0 0.0")
	NATURALVAR (numberOfSamples, U"Number of samples", U"100")
	OK
DO
	autoCrossCorrelationTable thee = CrossCorrelationTable_createSimple (crosscorrelations_string, centroid_string,numberOfSamples);
	praat_new (thee.move(), name);
END }

FORM (NEW_MixingMatrix_createSimple, U"Create simple MixingMatrix", nullptr) {
	WORDVAR (name, U"Name", U"mm")
	NATURALVAR (numberOfChannels, U"Number of channels", U"2")
	NATURALVAR (numberOfComponents, U"Number of components", U"2")
	SENTENCEVAR (mixinCoefficients_string, U"Mixing coefficients", U"1.0 1.0 1.0 1.0")
	OK
DO
	autoMixingMatrix thee = MixingMatrix_createSimple (numberOfChannels, numberOfComponents, mixinCoefficients_string);
	praat_new (thee.move(), name);
END }

DIRECT (HELP_CrossCorrelationTable_help) {
	Melder_help (U"CrossCorrelationTable");
END }

FORM (NEW1_Sound_and_PCA_principalComponents, U"Sound & PCA: To Sound (principal components)", nullptr) {
	NATURALVAR (numberOfComponents, U"Number of components", U"10")
	OK
DO
	Sound me = FIRST (Sound);
	PCA thee = FIRST (PCA);
	autoSound him = Sound_and_PCA_principalComponents (me, thee, numberOfComponents);
	praat_new (him.move(), my name, U"_pc");
END }

FORM (NEW1_Sound_and_PCA_whitenChannels, U"Sound & PCA: To Sound (white channels)", nullptr) {
	NATURALVAR (numberOfComponents, U"Number of components", U"10")
	OK
DO
	Sound me = FIRST (Sound);
	PCA thee = FIRST (PCA);
	autoSound him = Sound_and_PCA_whitenChannels (me, thee, numberOfComponents);
	praat_new (him.move(), my name, U"_white");
END }

DIRECT (NEW1_CrossCorrelationTable_to_CrossCorrelationTableList) {
	autoCrossCorrelationTableList thee = CrossCorrelationTableList_create ();
	long nrows = 0, ncols = 0, nselected = 0;
	LOOP {
		iam (CrossCorrelationTable);
		nselected++;
		if (nselected == 1) {
			nrows = my numberOfRows;
			ncols = my numberOfColumns;
		}
		if (my numberOfRows != nrows || my numberOfColumns != ncols) {
			Melder_throw (U"Dimensions of table ", IOBJECT, U" differs from the rest.");
		}
		autoCrossCorrelationTable myc = Data_copy (me);
		thy addItem_move (myc.move());
	}
	praat_new (thee.move(), U"ct_", nselected);
END }

FORM (NEW_Sound_to_Covariance_channels, U"Sound: To Covariance (channels)", U"Sound: To Covariance (channels)...") {
	praat_TimeFunction_RANGE(fromTime, toTime)
	OK
DO
	LOOP {
		iam (Sound);
		autoCovariance thee = Sound_to_Covariance_channels (me, fromTime, toTime);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_CrossCorrelationTable, U"Sound: To CrossCorrelationTable", U"Sound: To CrossCorrelationTable...") {
	praat_TimeFunction_RANGE(fromTime, toTime)
	REALVAR (lagTime, U"Lag step (s)", U"0.0")
    OK
DO
    LOOP {
        iam (Sound);
		autoCrossCorrelationTable thee = Sound_to_CrossCorrelationTable (me, fromTime, toTime, lagTime);
        praat_new (thee.move(), my name);
    }
END }

FORM (NEW1_Sounds_to_CrossCorrelationTable_combined, U"Sound: To CrossCorrelationTable (combined)", nullptr) {
	praat_TimeFunction_RANGE(fromTime, toTime)
	REALVAR (lagTime, U"Lag step (s)", U"0.0")
	OK
DO
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	autoCrossCorrelationTable thee = Sounds_to_CrossCorrelationTable_combined (s1, s2, fromTime, toTime, lagTime);
	praat_new (thee.move(), s1 -> name, U"_", s2 -> name, U"_cc");
END }

DIRECT (HELP_CrossCorrelationTableList_help) {
	Melder_help (U"CrossCorrelationTableList");
END }

FORM (REAL_CrossCorrelationTableList_getDiagonalityMeasure, U"CrossCorrelationTableList: Get diagonality measure", U"CrossCorrelationTableList: Get diagonality measure...") {
	NATURALVAR (fromTable, U"First table", U"1")
	NATURALVAR (toTable, U"Last table", U"100")
	OK
DO
	LOOP {
		iam (CrossCorrelationTableList);
		double dm = CrossCorrelationTableList_getDiagonalityMeasure (me, 0, fromTable, toTable);
		Melder_information (dm, U" (= average sum of squared off-diagonal elements)");
	}
END }

FORM (NEW_CrossCorrelationTableList_extractCrossCorrelationTable, U"CrossCorrelationTableList: Extract one CrossCorrelationTable", nullptr) {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	LOOP {
		iam (CrossCorrelationTableList);
		if (index > my size) {
			Melder_throw (U"Index too large.");
		}
		autoCrossCorrelationTable thee = Data_copy (my at [index]);
		praat_new (thee.move(), Thing_getName (me), U"_", index);
	}
END }

FORM (NEW_CrossCorrelationTableList_to_Diagonalizer, U"CrossCorrelationTableList: To Diagonalizer", nullptr) {
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	LOOP {
		iam (CrossCorrelationTableList);
		autoDiagonalizer thee = CrossCorrelationTableList_to_Diagonalizer (me, maximumNumberOfIterations, tolerance, diagonalizationMethod);
		praat_new (thee.move(), my name);
	}
END }

FORM (MODIFY_Diagonalizer_and_CrossCorrelationTableList_improveDiagonality, U"Diagonalizer & CrossCorrelationTableList: Improve diagonality", nullptr) {
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	Diagonalizer d = FIRST (Diagonalizer);
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (d, ccts, maximumNumberOfIterations, tolerance, diagonalizationMethod);
END }

FORM (REAL_CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure, U"CrossCorrelationTableList & Diagonalizer: Get diagonality measure", nullptr) {
	NATURALVAR (fromTable, U"First table", U"1")
	NATURALVAR (toTable, U"Last table", U"100")
	OK
DO
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	Diagonalizer d = FIRST (Diagonalizer);
	double dm = CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure (ccts, d, 0, fromTable, toTable);
	Melder_information (dm, U" (= average sum of squared off-diagonal elements)");
END }

DIRECT (NEW1_CrossCorrelationTable_and_Diagonalizer_diagonalize) {
	CrossCorrelationTable cct = FIRST (CrossCorrelationTable);
	Diagonalizer d = FIRST (Diagonalizer);
	autoCrossCorrelationTable thee = CrossCorrelationTable_and_Diagonalizer_diagonalize (cct, d);
	praat_new (thee.move(), cct -> name, U"_", d -> name);
END }

DIRECT (NEW1_CrossCorrelationTableList_and_Diagonalizer_diagonalize) {
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	Diagonalizer d = FIRST (Diagonalizer);
	autoCrossCorrelationTableList thee = CrossCorrelationTableList_and_Diagonalizer_diagonalize (ccts, d);
	praat_new (thee.move(), ccts->name, U"_", d->name);
END }

FORM (MODIFY_CrossCorrelationTableList_and_MixingMatrix_improveUnmixing, U"", nullptr) {
	LABEL (U"", U"Iteration parameters")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVEVAR (tolerance, U"Tolerance", U"0.001")
	OPTIONMENUVAR (diagonalizationMethod, U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	MixingMatrix mm = FIRST (MixingMatrix);
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (mm, ccts, maximumNumberOfIterations, tolerance, diagonalizationMethod);
END }

DIRECT (NEW_Diagonalizer_to_MixingMatrix) {
	LOOP {
		iam (Diagonalizer);
		autoMixingMatrix thee = Diagonalizer_to_MixingMatrix (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_MixingMatrix, U"Sound: To MixingMatrix", nullptr) {
	praat_TimeFunction_RANGE(fromTime, toTime)
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
	LOOP {
		iam (Sound);
		autoMixingMatrix thee = Sound_to_MixingMatrix (me, fromTime, toTime, numberOfCrossCorrelations, lagTime, maximumNumberOfIterations, tolerance, diagonalizationMethod);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_CrossCorrelationTableList, U"Sound: To CrossCorrelationTableList", nullptr) {
	praat_TimeFunction_RANGE(fromTime, toTime)
	NATURALVAR (numberOfCrossCorrelations, U"Number of cross-correlations", U"40")
	POSITIVEVAR (lagTime, U"Lag step (s)", U"0.002")
	OK
DO
	LOOP {
		iam (Sound);
		autoCrossCorrelationTableList thee = Sound_to_CrossCorrelationTableList (me, fromTime, toTime, lagTime, numberOfCrossCorrelations);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_Sound_bss, U"Sound: To Sound (blind source separation)", U"Sound: To Sound (blind source separation)...") {
	praat_TimeFunction_RANGE(fromTime, toTime)
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
	LOOP {
		iam (Sound);
		autoSound thee = Sound_to_Sound_BSS (me, fromTime, toTime, numberOfCrossCorrelations, lagTime, maximumNumberOfIterations, tolerance, diagonalizationMethod);
		praat_new (thee.move(), my name, U"_bss");
	}
END }

FORM (NEW_Sound_to_Sound_whiteChannels, U"Sound: To Sound (white channels)", U"Sound: To Sound (white channels)...") {
	POSITIVEVAR (varianceFraction, U"Variance fraction to keep", U"0.99")
    OK
DO
    if (varianceFraction > 1) varianceFraction = 1;
    long permille = (long) floor (varianceFraction * 1000.0);
    LOOP {
        iam (Sound);
		autoSound thee = Sound_whitenChannels (me, varianceFraction);
        praat_new (thee.move(), my name, U"_", permille);
    }
END }

DIRECT (NEW1_Sound_and_MixingMatrix_mix) {
	Sound s = FIRST (Sound);
	MixingMatrix mm = FIRST (MixingMatrix);
	autoSound thee = Sound_and_MixingMatrix_mix (s, mm);
	praat_new (thee.move(), Thing_getName (s), U"_mixed");
END }

DIRECT (NEW1_Sound_and_MixingMatrix_unmix) {
	Sound s = FIRST (Sound);
	MixingMatrix mm = FIRST (MixingMatrix);
	autoSound thee = Sound_and_MixingMatrix_unmix (s, mm);
	praat_new (thee.move(), Thing_getName (s), U"_unmixed");
END }

DIRECT (NEW_TableOfReal_to_MixingMatrix) {
	LOOP {
		iam (TableOfReal);
		autoMixingMatrix thee = TableOfReal_to_MixingMatrix (me);
		praat_new (thee.move(), my name);
	}
END }

void praat_BSS_init ();
void praat_BSS_init () {
	Thing_recognizeClassesByName (classDiagonalizer, classMixingMatrix, classCrossCorrelationTable, classCrossCorrelationTableList, nullptr);
	Thing_recognizeClassByOtherName (classCrossCorrelationTableList, U"CrossCorrelationTables");

	praat_addMenuCommand (U"Objects", U"New", U"Create simple CrossCorrelationTable...", U"Create simple Covariance...", praat_HIDDEN + praat_DEPTH_1, NEW_CrossCorrelationTable_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create test CrossCorrelationTableList...", U"Create simple CrossCorrelationTable...", praat_HIDDEN + praat_DEPTH_1, NEW_CrossCorrelationTableList_createTestSet);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple MixingMatrix...", U"Create test CrossCorrelationTableList...", praat_HIDDEN + praat_DEPTH_1, NEW_MixingMatrix_createSimple);

	praat_addAction1 (classCrossCorrelationTable, 0, U"CrossCorrelationTable help", 0, 0, HELP_CrossCorrelationTable_help);
	praat_SSCP_as_TableOfReal_init (classCrossCorrelationTable);

	praat_addAction1 (classCrossCorrelationTable, 0, U"To CrossCorrelationTableList", nullptr, 0, NEW1_CrossCorrelationTable_to_CrossCorrelationTableList);
	praat_addAction1 (classCrossCorrelationTable, 0, U"To CrossCorrelationTables", U"*To CrossCorrelationTableList", praat_DEPRECATED_2015, NEW1_CrossCorrelationTable_to_CrossCorrelationTableList);

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

	praat_addAction1 (classSound, 0, U"To MixingMatrix...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_MixingMatrix);
    praat_addAction1 (classSound, 0, U"To CrossCorrelationTable...",  U"Resample...", 1, NEW_Sound_to_CrossCorrelationTable);
    praat_addAction1 (classSound, 0, U"To Covariance (channels)...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_Covariance_channels);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTables...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_CrossCorrelationTableList);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTableList...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_CrossCorrelationTableList);

	praat_addAction1 (classSound, 0, U"To Sound (bss)...", U"Resample...", 1, NEW_Sound_to_Sound_bss);
    praat_addAction1 (classSound, 0, U"To Sound (white channels)...", U"Resample...", 1, NEW_Sound_to_Sound_whiteChannels);
    praat_addAction1 (classSound, 2, U"To CrossCorrelationTable (combined)...",  U"Cross-correlate...", 1, NEW1_Sounds_to_CrossCorrelationTable_combined);

	praat_addAction1 (classTableOfReal, 0, U"To MixingMatrix", U"To Configuration", praat_HIDDEN, NEW_TableOfReal_to_MixingMatrix);

	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Mix", 0, 0, NEW1_Sound_and_MixingMatrix_mix);
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

/* End of file praat_BSS_init.cpp 591*/
