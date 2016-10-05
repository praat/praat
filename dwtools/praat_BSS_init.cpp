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

#include "EEG_extensions.h"
#include "ICA.h"
#include "Sound_and_PCA.h"

#undef iam
#define iam iam_LOOP

void praat_SSCP_as_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init3 (ClassInfo klas);


/******************** EEG ********************************************/

FORM (EEG_to_CrossCorrelationTable, U"EEG: To CrossCorrelationTable", U"EEG: To CrossCorrelationTable...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	REAL (U"Lag step (s)", U"0.05")
	TEXTFIELD (U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK2
DO
	double startTime = GET_REAL (U"left Time range"), endTime = GET_REAL (U"right Time range");
	double lagTime = GET_REAL (U"Lag step");
	const char32 *channelRanges = GET_STRING (U"Channel ranges");
	LOOP {
		iam (EEG);
		autoCrossCorrelationTable cct = EEG_to_CrossCorrelationTable (me, startTime, endTime, lagTime, channelRanges);
		praat_new (cct.move(), my name, U"_", (long) floor (lagTime*1000)); // lagTime to ms   ppgb: geeft afrondingsfouten; waarom niet round?
	}
END2 }

FORM (EEG_to_Covariance, U"EEG: To Covariance", U"EEG: To Covariance...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	TEXTFIELD (U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK2
DO
	double startTime = GET_REAL (U"left Time range"), endTime = GET_REAL (U"right Time range");
	const char32 *channelRanges = GET_STRING (U"Channel ranges");
	LOOP {
		iam (EEG);
		autoCovariance cov = EEG_to_Covariance (me, startTime, endTime, channelRanges);
		praat_new (cov.move(), my name);
	}
END2 }

FORM (EEG_to_CrossCorrelationTableList, U"EEG: To CrossCorrelationTableList", U"EEG: To CrossCorrelationTableList...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	POSITIVE (U"Lag step (s)", U"0.02")
	NATURAL (U"Number of cross-correlations", U"40")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTFIELD (U"Channel ranges", U"1:64")
	OK2
DO
	LOOP {
		iam (EEG);
		autoCrossCorrelationTableList thee = EEG_to_CrossCorrelationTableList (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Lag step"), GET_INTEGER (U"Number of cross-correlations"), GET_STRING (U"Channel ranges"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (EEG_to_EEG_bss, U"EEG: To EEG (bss)", U"EEG: To EEG (bss)...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	NATURAL (U"Number of cross-correlations", U"40")
	POSITIVE (U"Lag step (s)", U"0.002")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTFIELD (U"Channel ranges", U"1:64")
	LABEL (U"", U"Pre-whitening parameters")
	OPTIONMENU (U"Whitening method", 1)
		OPTION (U"No whitening")
		OPTION (U"Covariance")
		OPTION (U"Correlation")
	LABEL (U"", U"Iteration parameters")
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
	OPTION (U"qdiag")
	OPTION (U"ffdiag")
	OK2
DO
	int whiteningMethod = GET_INTEGER (U"Whitening method") - 1;
	LOOP {
		iam (EEG);
		autoEEG thee = EEG_to_EEG_bss (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"Number of cross-correlations"), GET_REAL (U"Lag step"), GET_STRING (U"Channel ranges"),
			whiteningMethod, GET_INTEGER (U"Diagonalization method"),
			GET_INTEGER (U"Maximum number of iterations"), GET_REAL (U"Tolerance"));
		praat_new (thee.move(), my name, U"_bss");
	}
END2 }

FORM (EEG_to_PCA, U"EEG: To PCA", U"EEG: To PCA...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	TEXTFIELD (U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OPTIONMENU (U"Use", 1)
		OPTION (U"Covariance")
		OPTION (U"Correlation")
	OK2
DO
	double startTime = GET_REAL (U"left Time range"), endTime = GET_REAL (U"right Time range");
	const char32 *channelRanges = GET_STRING (U"Channel ranges");
	bool useCorrelation = GET_INTEGER (U"Use") == 2;
	LOOP {
		iam (EEG);
		autoPCA pca = EEG_to_PCA (me, startTime, endTime, channelRanges, useCorrelation);
		praat_new (pca.move(), my name);
	}
END2 }

FORM (EEG_and_PCA_to_EEG_principalComponents, U"EEG & PCA: To EEG (principal components)", U"EEG & PCA: To EEG (principal components)...") {
	INTEGER (U"Number of components", U"0 (= all)")
	OK2
DO
	EEG me = FIRST (EEG);
	PCA thee = FIRST (PCA);
	autoEEG him = EEG_and_PCA_to_EEG_principalComponents (me, thee, GET_INTEGER (U"Number of components"));
	praat_new (him.move(), my name, U"_pc");
END2 }

FORM (EEG_and_PCA_to_EEG_whiten, U"EEG & PCA: To EEG (whiten)", U"EEG & PCA: To EEG (whiten)...") {
	INTEGER (U"Number of components", U"0 (= all)")
	OK2
DO
	EEG me = FIRST (EEG);
	PCA thee = FIRST (PCA);
	autoEEG him = EEG_and_PCA_to_EEG_whiten (me, thee, GET_INTEGER (U"Number of components"));
	praat_new (him.move(), my name, U"_white");
END2 }

FORM (EEG_to_Sound_modulated, U"EEG: To Sound (modulated)", 0) {
	POSITIVE (U"Start frequency (Hz)", U"100.0")
	POSITIVE (U"Channel bandwidth (Hz)", U"100.0")
	TEXTFIELD (U"Channel ranges", U"1:64")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK2
DO
	LOOP {
		iam (EEG);
		autoSound thee = EEG_to_Sound_modulated (me, GET_REAL (U"Start frequency"), GET_REAL (U"Channel bandwidth"),
			GET_STRING (U"Channel ranges"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (EEG_to_Sound_frequencyShifted, U"EEG: To Sound (frequency shifted)", 0) {
	NATURAL (U"Channel", U"1")
	POSITIVE (U"Frequency shift (Hz)", U"100.0")
	POSITIVE (U"Sampling frequecy (Hz)", U"11025.0")
	REAL (U"Maximum amplitude", U"0.99")
	OK2
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (EEG);
		autoSound thee = EEG_to_Sound_frequencyShifted (me, channel, GET_REAL (U"Frequency shift"),
			GET_REAL (U"Sampling frequecy"), GET_REAL (U"Maximum amplitude"));
		praat_new (thee.move(), my name, U"_ch", channel);
	}
END2 }

/********************** CrossCorrelationTable(s) ******************/

FORM (CrossCorrelationTableList_createTestSet, U"CrossCorrelationTableList: Create test set", U"CrossCorrelationTableList: Create test set...") {
	WORD (U"Name", U"5x5")
	NATURAL (U"Matrix dimension", U"5")
	NATURAL (U"Number of matrices", U"20")
	BOOLEAN (U"First is positive definite", 1)
	REAL (U"Sigma", U"0.02")
	OK2
DO
	autoCrossCorrelationTableList thee = CrossCorrelationTableList_createTestSet (GET_INTEGER (U"Matrix dimension"),
		GET_INTEGER (U"Number of matrices"), GET_INTEGER (U"First is positive definite"), GET_REAL (U"Sigma"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

FORM (CrossCorrelationTable_createSimple, U"Create simple CrossCorrelationTable", nullptr) {
	WORD (U"Name", U"ct")
	SENTENCE (U"Cross correlations", U"1.0 0.0 1.0")
	SENTENCE (U"Centroid", U"0.0 0.0")
	NATURAL (U"Number of samples", U"100")
	OK2
DO
	autoCrossCorrelationTable thee = CrossCorrelationTable_createSimple (GET_STRING (U"Cross correlations"), GET_STRING (U"Centroid"),
		GET_INTEGER (U"Number of samples"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

FORM (MixingMatrix_createSimple, U"Create simple MixingMatrix", nullptr) {
	WORD (U"Name", U"mm")
	NATURAL (U"Number of channels", U"2")
	NATURAL (U"Number of components", U"2")
	SENTENCE (U"Mixing coefficients", U"1.0 1.0 1.0 1.0")
	OK2
DO
	autoMixingMatrix thee = MixingMatrix_createSimple (GET_INTEGER (U"Number of channels"), GET_INTEGER (U"Number of components"),
		GET_STRING (U"Mixing coefficients"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (CrossCorrelationTable_help) {
	Melder_help (U"CrossCorrelationTable");
END2 }

FORM (Sound_and_PCA_principalComponents, U"Sound & PCA: To Sound (principal components)", nullptr) {
	NATURAL (U"Number of components", U"10")
	OK2
DO
	Sound me = FIRST (Sound);
	PCA thee = FIRST (PCA);
	autoSound him = Sound_and_PCA_principalComponents (me, thee, GET_INTEGER (U"Number of components"));
	praat_new (him.move(), my name, U"_pc");
END2 }

FORM (Sound_and_PCA_whitenChannels, U"Sound & PCA: To Sound (white channels)", nullptr) {
	NATURAL (U"Number of components", U"10")
	OK2
DO
	Sound me = FIRST (Sound);
	PCA thee = FIRST (PCA);
	autoSound him = Sound_and_PCA_whitenChannels (me, thee, GET_INTEGER (U"Number of components"));
	praat_new (him.move(), my name, U"_white");
END2 }

DIRECT2 (CrossCorrelationTable_to_CrossCorrelationTableList) {
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
END2 }

FORM (Sound_to_Covariance_channels, U"Sound: To Covariance (channels)", U"Sound: To Covariance (channels)...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoCovariance thee = Sound_to_Covariance_channels (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_CrossCorrelationTable, U"Sound: To CrossCorrelationTable", U"Sound: To CrossCorrelationTable...") {
    REAL (U"left Time range (s)", U"0.0")
    REAL (U"right Time range (s)", U"10.0")
    REAL (U"Lag step (s)", U"0.0")
    OK2
DO
	double lagTime = fabs (GET_REAL (U"Lag step"));
    LOOP {
        iam (Sound);
		autoCrossCorrelationTable thee = Sound_to_CrossCorrelationTable (me, GET_REAL (U"left Time range"),
        GET_REAL (U"right Time range"), lagTime);
        praat_new (thee.move(), my name);
    }
END2 }

FORM (Sounds_to_CrossCorrelationTable_combined, U"Sound: To CrossCorrelationTable (combined)", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	REAL (U"Lag step (s)", U"0.0")
	OK2
DO
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 && s2);
	autoCrossCorrelationTable thee = Sounds_to_CrossCorrelationTable_combined (s1, s2, GET_REAL (U"left Time range"),
		GET_REAL (U"right Time range"), GET_REAL (U"Lag step"));
	praat_new (thee.move(), s1 -> name, U"_", s2 -> name, U"_cc");
END2 }

DIRECT2 (CrossCorrelationTableList_help) {
	Melder_help (U"CrossCorrelationTableList");
END2 }

FORM (CrossCorrelationTableList_getDiagonalityMeasure, U"CrossCorrelationTableList: Get diagonality measure", U"CrossCorrelationTableList: Get diagonality measure...") {
	NATURAL (U"First table", U"1")
	NATURAL (U"Last table", U"100")
	OK2
DO
	LOOP {
		iam (CrossCorrelationTableList);
		double dm = CrossCorrelationTableList_getDiagonalityMeasure (me, 0, GET_INTEGER (U"First table"),
		GET_INTEGER (U"Last table"));
		Melder_information (dm, U" (= average sum of squared off-diagonal elements)");
	}
END2 }

FORM (CrossCorrelationTableList_extractCrossCorrelationTable, U"CrossCorrelationTableList: Extract one CrossCorrelationTable", nullptr) {
	NATURAL (U"Index", U"1")
	OK2
DO
	long index = GET_INTEGER (U"Index");
	LOOP {
		iam (CrossCorrelationTableList);
		if (index > my size) {
			Melder_throw (U"Index too large.");
		}
		autoCrossCorrelationTable thee = Data_copy (my at [index]);
		praat_new (thee.move(), Thing_getName (me), U"_", index);
	}
END2 }

FORM (CrossCorrelationTableList_to_Diagonalizer, U"CrossCorrelationTableList: To Diagonalizer", nullptr) {
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
	OPTION (U"qdiag")
	OPTION (U"ffdiag")
	OK2
DO
	LOOP {
		iam (CrossCorrelationTableList);
		autoDiagonalizer thee = CrossCorrelationTableList_to_Diagonalizer (me, GET_INTEGER (U"Maximum number of iterations"),
		GET_REAL (U"Tolerance"), GET_INTEGER (U"Diagonalization method"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Diagonalizer_and_CrossCorrelationTableList_improveDiagonality, U"Diagonalizer & CrossCorrelationTableList: Improve diagonality", nullptr) {
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
	OPTION (U"qdiag")
	OPTION (U"ffdiag")
	OK2
DO
	Diagonalizer d = FIRST (Diagonalizer);
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (d, ccts, GET_INTEGER (U"Maximum number of iterations"),
		GET_REAL (U"Tolerance"), GET_INTEGER (U"Diagonalization method"));
END2 }

FORM (CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure, U"CrossCorrelationTableList & Diagonalizer: Get diagonality measure", nullptr) {
	NATURAL (U"First table", U"1")
	NATURAL (U"Last table", U"100")
	OK2
DO
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	Diagonalizer d = FIRST (Diagonalizer);
	double dm = CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure (ccts, d, 0, GET_INTEGER (U"First table"),
		GET_INTEGER (U"Last table"));
	Melder_information (dm, U" (= average sum of squared off-diagonal elements)");
END2 }

DIRECT2 (CrossCorrelationTable_and_Diagonalizer_diagonalize) {
	CrossCorrelationTable cct = FIRST (CrossCorrelationTable);
	Diagonalizer d = FIRST (Diagonalizer);
	autoCrossCorrelationTable thee = CrossCorrelationTable_and_Diagonalizer_diagonalize (cct, d);
	praat_new (thee.move(), cct -> name, U"_", d -> name);
END2 }

DIRECT2 (CrossCorrelationTableList_and_Diagonalizer_diagonalize) {
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	Diagonalizer d = FIRST (Diagonalizer);
	autoCrossCorrelationTableList thee = CrossCorrelationTableList_and_Diagonalizer_diagonalize (ccts, d);
	praat_new (thee.move(), ccts->name, U"_", d->name);
END2 }

FORM (CrossCorrelationTableList_and_MixingMatrix_improveUnmixing, U"", nullptr) {
	LABEL (U"", U"Iteration parameters")
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
	OPTION (U"qdiag")
	OPTION (U"ffdiag")
	OK2
DO
	MixingMatrix mm = FIRST (MixingMatrix);
	CrossCorrelationTableList ccts = FIRST (CrossCorrelationTableList);
	MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (mm, ccts,
		GET_INTEGER (U"Maximum number of iterations"), GET_REAL (U"Tolerance"), GET_INTEGER (U"Diagonalization method"));
END2 }

DIRECT2 (Diagonalizer_to_MixingMatrix) {
	LOOP {
		iam (Diagonalizer);
		autoMixingMatrix thee = Diagonalizer_to_MixingMatrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_MixingMatrix, U"Sound: To MixingMatrix", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	NATURAL (U"Number of cross-correlations", U"40")
	POSITIVE (U"Lag step (s)", U"0.002")
	LABEL (U"", U"Iteration parameters")
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK2
DO
	LOOP {
		iam (Sound);
		autoMixingMatrix thee = Sound_to_MixingMatrix (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 
			GET_INTEGER (U"Number of cross-correlations"), GET_REAL (U"Lag step"), GET_INTEGER (U"Maximum number of iterations"),
			GET_REAL (U"Tolerance"), GET_INTEGER (U"Diagonalization method"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_CrossCorrelationTableList, U"Sound: To CrossCorrelationTableList", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	NATURAL (U"Number of cross-correlations", U"40")
	POSITIVE (U"Lag step (s)", U"0.002")
	OK2
DO
	LOOP {
		iam (Sound);
		autoCrossCorrelationTableList thee = Sound_to_CrossCorrelationTableList (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Lag step"), GET_INTEGER (U"Number of cross-correlations"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Sound_bss, U"Sound: To Sound (blind source separation)", U"Sound: To Sound (blind source separation)...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	NATURAL (U"Number of cross-correlations", U"40")
	POSITIVE (U"Lag step (s)", U"0.002")
	LABEL (U"", U"Iteration parameters")
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_to_Sound_BSS (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"Number of cross-correlations"), GET_REAL (U"Lag step"),
			GET_INTEGER (U"Maximum number of iterations"), GET_REAL (U"Tolerance"),
			GET_INTEGER (U"Diagonalization method"));
		praat_new (thee.move(), my name, U"_bss");
	}
END2 }

FORM (Sound_to_Sound_whiteChannels, U"Sound: To Sound (white channels)", U"Sound: To Sound (white channels)...") {
    POSITIVE (U"Variance fraction to keep", U"0.99")
    OK2
DO
    double varianceFraction = GET_REAL (U"Variance fraction to keep");
    if (varianceFraction > 1) varianceFraction = 1;
    long permille = (long) floor (varianceFraction * 1000.0);
    LOOP {
        iam (Sound);
		autoSound thee = Sound_whitenChannels (me, varianceFraction);
        praat_new (thee.move(), my name, U"_", permille);
    }
END2 }

DIRECT2 (Sound_and_MixingMatrix_mix) {
	Sound s = FIRST (Sound);
	MixingMatrix mm = FIRST (MixingMatrix);
	autoSound thee = Sound_and_MixingMatrix_mix (s, mm);
	praat_new (thee.move(), Thing_getName (s), U"_mixed");
END2 }

DIRECT2 (Sound_and_MixingMatrix_unmix) {
	Sound s = FIRST (Sound);
	MixingMatrix mm = FIRST (MixingMatrix);
	autoSound thee = Sound_and_MixingMatrix_unmix (s, mm);
	praat_new (thee.move(), Thing_getName (s), U"_unmixed");
END2 }

DIRECT2 (TableOfReal_to_MixingMatrix) {
	LOOP {
		iam (TableOfReal);
		autoMixingMatrix thee = TableOfReal_to_MixingMatrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

void praat_BSS_init ();
void praat_BSS_init () {
	Thing_recognizeClassesByName (classDiagonalizer, classMixingMatrix, classCrossCorrelationTable, classCrossCorrelationTableList, nullptr);
	Thing_recognizeClassByOtherName (classCrossCorrelationTableList, U"CrossCorrelationTables");

	praat_addMenuCommand (U"Objects", U"New", U"Create simple CrossCorrelationTable...", U"Create simple Covariance...", praat_HIDDEN + praat_DEPTH_1, DO_CrossCorrelationTable_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create test CrossCorrelationTableList...", U"Create simple CrossCorrelationTable...", praat_HIDDEN + praat_DEPTH_1, DO_CrossCorrelationTableList_createTestSet);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple MixingMatrix...", U"Create test CrossCorrelationTableList...", praat_HIDDEN + praat_DEPTH_1, DO_MixingMatrix_createSimple);

	praat_addAction1 (classCrossCorrelationTable, 0, U"CrossCorrelationTable help", 0, 0, DO_CrossCorrelationTable_help);
	praat_SSCP_as_TableOfReal_init (classCrossCorrelationTable);

	praat_addAction1 (classCrossCorrelationTable, 0, U"To CrossCorrelationTableList", nullptr, 0, DO_CrossCorrelationTable_to_CrossCorrelationTableList);
	praat_addAction1 (classCrossCorrelationTable, 0, U"To CrossCorrelationTables", nullptr, praat_HIDDEN, DO_CrossCorrelationTable_to_CrossCorrelationTableList);

	praat_addAction1 (classCrossCorrelationTableList, 0, U"CrossCorrelationTableList help", 0, 0, DO_CrossCorrelationTableList_help);
	praat_addAction1 (classCrossCorrelationTableList, 1, U"Extract CrossCorrelationTable...", 0, 0, DO_CrossCorrelationTableList_extractCrossCorrelationTable);
	praat_addAction1 (classCrossCorrelationTableList, 1, U"Get diagonality measure...", 0, 0, DO_CrossCorrelationTableList_getDiagonalityMeasure);
	praat_addAction1 (classCrossCorrelationTableList, 0, U"To Diagonalizer...", 0, 0, DO_CrossCorrelationTableList_to_Diagonalizer);

	praat_TableOfReal_init3 (classDiagonalizer);
	praat_addAction1 (classDiagonalizer, 0, U"To MixingMatrix", 0, 0, DO_Diagonalizer_to_MixingMatrix);

	praat_addAction1 (classEEG, 0, U"To Sound (mc modulated)...", U"To ERPTier...", praat_HIDDEN, DO_EEG_to_Sound_modulated);
	praat_addAction1 (classEEG, 0, U"To Sound (frequency shifted)...", U"To ERPTier...", 0, DO_EEG_to_Sound_frequencyShifted);
	praat_addAction1 (classEEG, 0, U"To PCA...", U"To ERPTier...", 0, DO_EEG_to_PCA);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTable...", U"To PCA...", praat_HIDDEN, DO_EEG_to_CrossCorrelationTable);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTables...", U"To PCA...", praat_HIDDEN, DO_EEG_to_CrossCorrelationTableList);
	praat_addAction1 (classEEG, 0, U"To CrossCorrelationTableList...", U"To PCA...", praat_HIDDEN, DO_EEG_to_CrossCorrelationTableList);

	praat_addAction1 (classEEG, 0, U"To Covariance...", U"To CrossCorrelationTable...", praat_HIDDEN, DO_EEG_to_Covariance);
	praat_addAction1 (classEEG, 0, U"To EEG (bss)...", U"To CrossCorrelationTable...", praat_HIDDEN, DO_EEG_to_EEG_bss);

	praat_addAction2 (classEEG, 1, classPCA, 1, U"To EEG (principal components)...", 0, 0, DO_EEG_and_PCA_to_EEG_principalComponents);
	praat_addAction2 (classEEG, 1, classPCA, 1, U"To EEG (whiten)...", 0, 0, DO_EEG_and_PCA_to_EEG_whiten);

	praat_TableOfReal_init3 (classMixingMatrix);

	praat_addAction1 (classSound, 0, U"To MixingMatrix...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_MixingMatrix);
    praat_addAction1 (classSound, 0, U"To CrossCorrelationTable...",  U"Resample...", 1, DO_Sound_to_CrossCorrelationTable);
    praat_addAction1 (classSound, 0, U"To Covariance (channels)...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_Covariance_channels);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTables...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_CrossCorrelationTableList);
	praat_addAction1 (classSound, 0, U"To CrossCorrelationTableList...",  U"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_CrossCorrelationTableList);

	praat_addAction1 (classSound, 0, U"To Sound (bss)...", U"Resample...", 1, DO_Sound_to_Sound_bss);
    praat_addAction1 (classSound, 0, U"To Sound (white channels)...", U"Resample...", 1, DO_Sound_to_Sound_whiteChannels);
    praat_addAction1 (classSound, 2, U"To CrossCorrelationTable (combined)...",  U"Cross-correlate...", 1, DO_Sounds_to_CrossCorrelationTable_combined);

	praat_addAction1 (classTableOfReal, 0, U"To MixingMatrix", U"To Configuration", praat_HIDDEN, DO_TableOfReal_to_MixingMatrix);

	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Mix", 0, 0, DO_Sound_and_MixingMatrix_mix);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, U"Unmix", 0, 0, DO_Sound_and_MixingMatrix_unmix);

	praat_addAction2 (classSound, 1, classPCA, 1, U"To Sound (white channels)...", 0 , 0, DO_Sound_and_PCA_whitenChannels);
	praat_addAction2 (classSound, 1, classPCA, 1, U"To Sound (principal components)...", 0 , 0, DO_Sound_and_PCA_principalComponents);

	praat_addAction2 (classCrossCorrelationTable, 1, classDiagonalizer, 1, U"Diagonalize", 0 , 0, DO_CrossCorrelationTable_and_Diagonalizer_diagonalize);

	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Get diagonality measure...", 0 , 0, DO_CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure);
	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Diagonalize", 0 , 0, DO_CrossCorrelationTableList_and_Diagonalizer_diagonalize);
	praat_addAction2 (classCrossCorrelationTableList, 1, classDiagonalizer, 1, U"Improve diagonality...", 0 , 0, DO_Diagonalizer_and_CrossCorrelationTableList_improveDiagonality);

	praat_addAction2 (classCrossCorrelationTableList, 1, classMixingMatrix, 1, U"Improve unmixing...", 0 , 0, DO_CrossCorrelationTableList_and_MixingMatrix_improveUnmixing);

	INCLUDE_MANPAGES (manual_BSS)
}

/* End of file praat_BSS_init.cpp */
