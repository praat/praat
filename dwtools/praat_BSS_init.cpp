/* praat_BSS_init.c
 *
 * Copyright (C) 2010-2011 David Weenink
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

FORM (EEG_to_CrossCorrelationTable, L"EEG: To CrossCorrelationTable", L"EEG: To CrossCorrelationTable...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	REAL (L"Lag time (s)", L"0.05")
	TEXTFIELD (L"Channel ranges", L"1:64")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	double startTime = GET_REAL (L"left Time range"), endTime = GET_REAL (L"right Time range");
	double lagTime = GET_REAL (L"Lag time");
	const wchar_t *channelRanges = GET_STRING (L"Channel ranges");
	LOOP {
		iam (EEG);
		autoCrossCorrelationTable cct = EEG_to_CrossCorrelationTable (me, startTime, endTime, lagTime, channelRanges);
		praat_new (cct.transfer(), my name, L"_", Melder_integer (lagTime*1000)); // lagTime to ms
	}
END

FORM (EEG_to_Covariance, L"EEG: To Covariance", L"EEG: To Covariance...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	TEXTFIELD (L"Channel ranges", L"1:64")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	double startTime = GET_REAL (L"left Time range"), endTime = GET_REAL (L"right Time range");
	const wchar_t *channelRanges = GET_STRING (L"Channel ranges");
	LOOP {
		iam (EEG);
		autoCovariance cov = EEG_to_Covariance (me, startTime, endTime, channelRanges);
		praat_new (cov.transfer(), my name);
	}
END

FORM (EEG_to_CrossCorrelationTables, L"EEG: To CrossCorrelationTables", L"EEG: To CrossCorrelationTables...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	POSITIVE (L"Lag time (s)", L"0.02")
	NATURAL (L"Number of cross-correlations", L"40")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTFIELD (L"Channel ranges", L"1:64")
	OK
DO
	LOOP {
		iam (EEG);
		autoCrossCorrelationTables thee = EEG_to_CrossCorrelationTables (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Lag time"), GET_INTEGER (L"Number of cross-correlations"), GET_STRING (L"Channel ranges"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (EEG_to_EEG_bss, L"EEG: To EEG (bss)", L"EEG: To EEG (bss)...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	NATURAL (L"Number of cross-correlations", L"40")
	POSITIVE (L"Lag times (s)", L"0.002")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	TEXTFIELD (L"Channel ranges", L"1:64")
	LABEL (L"", L"Pre-whitening parameters")
	OPTIONMENU (L"Whitening method", 1)
		OPTION (L"No whitening")
		OPTION (L"Covariance")
		OPTION (L"Correlation")
	LABEL (L"", L"Iteration parameters")
	NATURAL (L"Maximum number of iterations", L"100")
	POSITIVE (L"Tolerance", L"0.001")
	OPTIONMENU (L"Diagonalization method", 2)
	OPTION (L"qdiag")
	OPTION (L"ffdiag")
	OK
DO
	int whiteningMethod = GET_INTEGER (L"Whitening method") - 1;
	LOOP {
		iam (EEG);
		autoEEG thee = EEG_to_EEG_bss (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"Number of cross-correlations"), GET_REAL (L"Lag times"), GET_STRING (L"Channel ranges"),
			whiteningMethod, GET_INTEGER (L"Diagonalization method"),
			GET_INTEGER (L"Maximum number of iterations"), GET_REAL (L"Tolerance"));
		praat_new (thee.transfer(), my name, L"_bss");
	}
END

FORM (EEG_to_PCA, L"EEG: To PCA", L"EEG: To PCA...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	TEXTFIELD (L"Channel ranges", L"1:64")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OPTIONMENU (L"Use", 1)
		OPTION (L"Covariance")
		OPTION (L"Correlation")
	OK
DO
	double startTime = GET_REAL (L"left Time range"), endTime = GET_REAL (L"right Time range");
	const wchar_t *channelRanges = GET_STRING (L"Channel ranges");
	bool useCorrelation = GET_INTEGER (L"Use") == 2;
	LOOP {
		iam (EEG);
		autoPCA pca = EEG_to_PCA (me, startTime, endTime, channelRanges, useCorrelation);
		praat_new (pca.transfer(), my name);
	}
END

FORM (EEG_and_PCA_to_EEG_principalComponents, L"EEG & PCA: To EEG (principal components)", L"EEG & PCA: To EEG (principal components)...")
	INTEGER (L"Number of components", L"0 (=all)")
	OK
DO
	EEG me = FIRST (EEG);
	PCA thee = FIRST (PCA);
	autoEEG him = EEG_and_PCA_to_EEG_principalComponents (me, thee, GET_INTEGER (L"Number of components"));
	praat_new (him.transfer(), my name, L"_pc");
END

FORM (EEG_and_PCA_to_EEG_whiten, L"EEG & PCA: To EEG (whiten)", L"EEG & PCA: To EEG (whiten)...")
	INTEGER (L"Number of components", L"0 (=all)")
	OK
DO
	EEG me = FIRST (EEG);
	PCA thee = FIRST (PCA);
	autoEEG him = EEG_and_PCA_to_EEG_whiten (me, thee, GET_INTEGER (L"Number of components"));
	praat_new (him.transfer(), my name, L"_white");
END

FORM (EEG_to_Sound_modulated, L"EEG: To Sound (modulated)", 0)
	POSITIVE (L"Start frequency (Hz)", L"100.0")
	POSITIVE (L"Channel bandwidth (Hz)", L"100.0")
	TEXTFIELD (L"Channel ranges", L"1:64")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	LOOP {
		iam (EEG);
		autoSound thee = EEG_to_Sound_modulated (me, GET_REAL (L"Start frequency"), GET_REAL (L"Channel bandwidth"),
			GET_STRING (L"Channel ranges"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (EEG_to_Sound_frequencyShifted, L"EEG: To Sound (frequency shifted)", 0)
	NATURAL (L"Channel", L"1")
	POSITIVE (L"Frequency shift (Hz)", L"100.0")
	POSITIVE (L"Sampling frequecy (Hz)", L"11025.0")
	REAL (L"Maximum amplitude", L"0.99")
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (EEG);
		autoSound thee = EEG_to_Sound_frequencyShifted (me, channel, GET_REAL (L"Frequency shift"),
			GET_REAL (L"Sampling frequecy"), GET_REAL (L"Maximum amplitude"));
		praat_new (thee.transfer(), my name, L"_ch", Melder_integer (channel));
	}
END

/********************** CrossCorrelationTable(s) ******************/

FORM (CrossCorrelationTables_createTestSet, L"CrossCorrelationTables: Create test set", L"CrossCorrelationTables: Create test set...")
	WORD (L"Name", L"5x5")
	NATURAL (L"Matrix dimension", L"5")
	NATURAL (L"Number of matrices", L"20")
	BOOLEAN (L"First is positive definite", 1)
	REAL (L"Sigma", L"0.02")
	OK
DO
	praat_new (CrossCorrelationTables_createTestSet (GET_INTEGER (L"Matrix dimension"),
		GET_INTEGER (L"Number of matrices"), GET_INTEGER (L"First is positive definite"), GET_REAL (L"Sigma")), GET_STRING (L"Name"));
END

FORM (CrossCorrelationTable_createSimple, L"Create simple CrossCorrelationTable", 0)
	WORD (L"Name", L"ct")
	SENTENCE (L"Cross correlations", L"1.0 0.0 1.0")
	SENTENCE (L"Centroid", L"0.0 0.0")
	POSITIVE (L"Number of samples", L"100.0")
	OK
DO
	praat_new (CrossCorrelationTable_createSimple (GET_STRING (L"Cross correlations"), GET_STRING (L"Centroid"),
		GET_REAL (L"Number of samples")), GET_STRING (L"Name"));
END

FORM (MixingMatrix_createSimple, L"Create simple MixingMatrix", 0)
	WORD (L"Name", L"mm")
	NATURAL (L"Number of channels", L"2")
	NATURAL (L"Number of components", L"2")
	SENTENCE (L"Mixing coefficients", L"1.0 1.0 1.0 1.0")
	OK
DO
	praat_new (MixingMatrix_createSimple (GET_INTEGER (L"Number of channels"), GET_INTEGER (L"Number of components"),
		GET_STRING (L"Mixing coefficients")), GET_STRING (L"Name"));
END

DIRECT (CrossCorrelationTable_help)
	Melder_help (L"CrossCorrelationTable");
END

FORM (Sound_and_PCA_principalComponents, L"Sound & PCA: To Sound (principal components)", 0)
	NATURAL (L"Number of components", L"10")
	OK
DO
	Sound me = FIRST (Sound);
	PCA thee = FIRST (PCA);
	praat_new (Sound_and_PCA_principalComponents (me, thee, GET_INTEGER (L"Number of components")), Thing_getName (me), L"_pc");
END

FORM (Sound_and_PCA_whitenChannels, L"Sound & PCA: To Sound (white channels)", 0)
	NATURAL (L"Number of components", L"10")
	OK
DO
	Sound me = FIRST (Sound);
	PCA thee = FIRST (PCA);
	praat_new (Sound_and_PCA_whitenChannels (me, thee, GET_INTEGER (L"Number of components")), Thing_getName (me), L"_white");
END

DIRECT (CrossCorrelationTable_to_CrossCorrelationTables)
	autoCrossCorrelationTables thee = CrossCorrelationTables_create ();
	long nrows = 0, ncols = 0, nselected = 0;
	LOOP {
		iam (CrossCorrelationTable); nselected++;
		if (nselected == 1) {
			nrows = my numberOfRows;
			ncols = my numberOfColumns;
		}
		if (my numberOfRows != nrows || my numberOfColumns != ncols) Melder_throw ("Dimensions of table ",
			IOBJECT, " differs from the rest.");
		autoCrossCorrelationTable myc = Data_copy (me);
		Collection_addItem (thee.peek(), myc.transfer());
	}
	praat_new (thee.transfer(), L"ct_", Melder_integer (nselected));
END

FORM (Sound_to_Covariance_channels, L"Sound: To Covariance (channels)", L"Sound: To Covariance (channels)...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Covariance_channels (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range")), my name);
	}
END

FORM (Sound_to_CrossCorrelationTable, L"Sound: To CrossCorrelationTable", L"Sound: To CrossCorrelationTable...")
    REAL (L"left Time range (s)", L"0.0")
    REAL (L"right Time range (s)", L"10.0")
    REAL (L"Lag time (s)", L"0.0")
    OK
DO
	double lagTime = fabs (GET_REAL (L"Lag time"));
    LOOP {
        iam (Sound);
        praat_new (Sound_to_CrossCorrelationTable (me, GET_REAL (L"left Time range"),
        GET_REAL (L"right Time range"), lagTime), my name);
    }
END

FORM (Sounds_to_CrossCorrelationTable_combined, L"Sound: To CrossCorrelationTable (combined)", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	REAL (L"Lag time (s)", L"0.0")
	OK
DO
	Sound s1 = NULL, s2 = NULL;
	LOOP {
		iam (Sound);
		(s1 ? s2 : s1) = me;
	}
	Melder_assert (s1 != NULL && s2 != NULL);
	autoCrossCorrelationTable thee = Sounds_to_CrossCorrelationTable_combined (s1, s2, GET_REAL (L"left Time range"),
		GET_REAL (L"right Time range"), GET_REAL (L"Lag time"));
	praat_new (thee.transfer(), s1 -> name, L"_", s2 -> name, L"_cc");
END

DIRECT (CrossCorrelationTables_help)
Melder_help (L"CrossCorrelationTables");
	END

FORM (CrossCorrelationTables_getDiagonalityMeasure, L"CrossCorrelationTables: Get diagonality measure", L"CrossCorrelationTables: Get diagonality measure...")
	NATURAL (L"First table", L"1")
	NATURAL (L"Last table", L"100")
	OK
DO
	LOOP {
		iam (CrossCorrelationTables);
		double dm = CrossCorrelationTables_getDiagonalityMeasure (me, 0, GET_INTEGER (L"First table"),
		GET_INTEGER (L"Last table"));
		Melder_information (Melder_double (dm), L" (= average sum of squared off-diagonal elements)");
	}
END

FORM (CrossCorrelationTables_extractCrossCorrelationTable, L"CrossCorrelationTables: Extract one CrossCorrelationTable", 0)
	NATURAL (L"Index", L"1")
	OK
DO
	long index = GET_INTEGER (L"Index");
	LOOP {
		iam (CrossCorrelationTables);
		if (index > my size) {
			Melder_throw (L"Index too large.");
		}
		autoCrossCorrelationTable thee = Data_copy ( (CrossCorrelationTable) my item[index]);
		praat_new (thee.transfer(), Thing_getName (me), L"_", Melder_integer (index));
	}
END

FORM (CrossCorrelationTables_to_Diagonalizer, L"CrossCorrelationTables: To Diagonalizer", 0)
	NATURAL (L"Maximum number of iterations", L"100")
	POSITIVE (L"Tolerance", L"0.001")
	OPTIONMENU (L"Diagonalization method", 2)
	OPTION (L"qdiag")
	OPTION (L"ffdiag")
	OK
DO
	LOOP {
		iam (CrossCorrelationTables);
		praat_new (CrossCorrelationTables_to_Diagonalizer (me, GET_INTEGER (L"Maximum number of iterations"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Diagonalization method")), my name);
	}
END

FORM (Diagonalizer_and_CrossCorrelationTables_improveDiagonality, L"Diagonalizer & CrossCorrelationTables: Improve diagonality", 0)
	NATURAL (L"Maximum number of iterations", L"100")
	POSITIVE (L"Tolerance", L"0.001")
	OPTIONMENU (L"Diagonalization method", 2)
	OPTION (L"qdiag")
	OPTION (L"ffdiag")
	OK
DO
	Diagonalizer d = FIRST (Diagonalizer);
	CrossCorrelationTables ccts = FIRST (CrossCorrelationTables);
	Diagonalizer_and_CrossCorrelationTables_improveDiagonality (d, ccts, GET_INTEGER (L"Maximum number of iterations"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Diagonalization method"));
END

FORM (CrossCorrelationTables_and_Diagonalizer_getDiagonalityMeasure, L"CrossCorrelationTables & Diagonalizer: Get diagonality measure", 0)
	NATURAL (L"First table", L"1")
	NATURAL (L"Last table", L"100")
	OK
DO
	CrossCorrelationTables ccts = FIRST (CrossCorrelationTables);
	Diagonalizer d = FIRST (Diagonalizer);
	double dm = CrossCorrelationTables_and_Diagonalizer_getDiagonalityMeasure (ccts, d, 0, GET_INTEGER (L"First table"),
		GET_INTEGER (L"Last table"));
	Melder_information (Melder_double (dm), L" (= average sum of squared off-diagonal elements)");
END

DIRECT (CrossCorrelationTable_and_Diagonalizer_diagonalize)
	CrossCorrelationTable cct = FIRST (CrossCorrelationTable);
	Diagonalizer d = FIRST (Diagonalizer);
	praat_new (CrossCorrelationTable_and_Diagonalizer_diagonalize (cct, d), cct->name, L"_", d->name);
END

DIRECT (CrossCorrelationTables_and_Diagonalizer_diagonalize)
	CrossCorrelationTables ccts = FIRST (CrossCorrelationTables);
	Diagonalizer d = FIRST (Diagonalizer);
	praat_new (CrossCorrelationTables_and_Diagonalizer_diagonalize (ccts, d), ccts->name, L"_", d->name);
END

FORM (CrossCorrelationTables_and_MixingMatrix_improveUnmixing, L"", 0)
	LABEL (L"", L"Iteration parameters")
	NATURAL (L"Maximum number of iterations", L"100")
	POSITIVE (L"Tolerance", L"0.001")
	OPTIONMENU (L"Diagonalization method", 2)
	OPTION (L"qdiag")
	OPTION (L"ffdiag")
	OK
DO
	MixingMatrix mm = FIRST (MixingMatrix);
	CrossCorrelationTables ccts = FIRST (CrossCorrelationTables);
	MixingMatrix_and_CrossCorrelationTables_improveUnmixing (mm, ccts,
		GET_INTEGER (L"Maximum number of iterations"), GET_REAL (L"Tolerance"), GET_INTEGER (L"Diagonalization method"));
END

DIRECT (Diagonalizer_to_MixingMatrix)
	LOOP {
		iam (Diagonalizer);
		praat_new (Diagonalizer_to_MixingMatrix (me), my name);
	}
END

FORM (Sound_to_MixingMatrix, L"Sound: To MixingMatrix", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	NATURAL (L"Number of cross-correlations", L"40")
	POSITIVE (L"Lag times (s)", L"0.002")
	LABEL (L"", L"Iteration parameters")
	NATURAL (L"Maximum number of iterations", L"100")
	POSITIVE (L"Tolerance", L"0.001")
	OPTIONMENU (L"Diagonalization method", 2)
	OPTION (L"qdiag")
	OPTION (L"ffdiag")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_MixingMatrix (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Number of cross-correlations"),
			GET_REAL (L"Lag times"), GET_INTEGER (L"Maximum number of iterations"), GET_REAL (L"Tolerance"),
			GET_INTEGER (L"Diagonalization method")), my name);
	}
END

FORM (Sound_to_CrossCorrelationTables, L"Sound: To CrossCorrelationTables", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	NATURAL (L"Number of cross-correlations", L"40")
	POSITIVE (L"Lag times (s)", L"0.002")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_CrossCorrelationTables (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Lag times"), GET_INTEGER (L"Number of cross-correlations")), my name);
	}
END

FORM (Sound_to_Sound_bss, L"Sound: To Sound (blind source separation)", L"Sound: To Sound (blind source separation)...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	NATURAL (L"Number of cross-correlations", L"40")
	POSITIVE (L"Lag times (s)", L"0.002")
	LABEL (L"", L"Iteration parameters")
	NATURAL (L"Maximum number of iterations", L"100")
	POSITIVE (L"Tolerance", L"0.001")
	OPTIONMENU (L"Diagonalization method", 2)
	OPTION (L"qdiag")
	OPTION (L"ffdiag")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Sound_BSS (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"Number of cross-correlations"), GET_REAL (L"Lag times"),
			GET_INTEGER (L"Maximum number of iterations"), GET_REAL (L"Tolerance"),
			GET_INTEGER (L"Diagonalization method")), my name, L"_bss");
	}
END

FORM (Sound_to_Sound_whiteChannels, L"Sound: To Sound (white channels)", L"Sound: To Sound (white channels)...")
    POSITIVE (L"Variance fraction to keep", L"0.99")
    OK
DO
    double varianceFraction = GET_REAL (L"Variance fraction to keep");
    if (varianceFraction > 1) varianceFraction = 1;
    long permille = varianceFraction * 1000.0;
    LOOP {
        iam (Sound);
        praat_new (Sound_whitenChannels (me, varianceFraction), my name, L"_", Melder_integer (permille));
    }
END

DIRECT (Sound_and_MixingMatrix_mix)
	Sound s = FIRST (Sound);
	MixingMatrix mm = FIRST (MixingMatrix);
	praat_new (Sound_and_MixingMatrix_mix (s, mm), Thing_getName (s), L"_mixed");
END

DIRECT (Sound_and_MixingMatrix_unmix)
	Sound s = FIRST (Sound);
	MixingMatrix mm = FIRST (MixingMatrix);
	praat_new (Sound_and_MixingMatrix_unmix (s, mm), Thing_getName (s), L"_unmixed");
END

DIRECT (TableOfReal_to_MixingMatrix)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_MixingMatrix (me), my name);
	}
END

FORM (TableOfReal_and_TableOfReal_crossCorrelations, L"TableOfReal & TableOfReal: Cross-correlations", 0)
	OPTIONMENU (L"Correlations between", 1)
	OPTION (L"Rows")
	OPTION (L"Columns")
	BOOLEAN (L"Center", 0)
	BOOLEAN (L"Normalize", 0)
	OK
DO
	TableOfReal t1 = 0, t2 = 0;
	LOOP {
		iam (TableOfReal);
		(t1 ? t2 : t1) = me;
	}
	Melder_assert (t1 != NULL && t2 != NULL);
	int by_columns = GET_INTEGER (L"Correlations between") - 1;
	praat_new (TableOfReal_and_TableOfReal_crossCorrelations (t1, t2, by_columns,
		GET_INTEGER (L"Center"), GET_INTEGER (L"Normalize")),
		(by_columns ? L"by_columns" : L"by_rows"));
END

void praat_TableOfReal_init3 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 2, L"To TableOfReal (cross-correlations)...", 0, 0, DO_TableOfReal_and_TableOfReal_crossCorrelations);
}

void praat_BSS_init (void);
void praat_BSS_init (void) {
	Thing_recognizeClassesByName (classDiagonalizer, classMixingMatrix, classCrossCorrelationTable, classCrossCorrelationTables, NULL);

	praat_addMenuCommand (L"Objects", L"New", L"Create simple CrossCorrelationTable...", L"Create simple Covariance...", praat_HIDDEN + praat_DEPTH_1, DO_CrossCorrelationTable_createSimple);
	praat_addMenuCommand (L"Objects", L"New", L"Create test CrossCorrelationTables...", L"Create simple CrossCorrelationTable...", praat_HIDDEN + praat_DEPTH_1, DO_CrossCorrelationTables_createTestSet);
	praat_addMenuCommand (L"Objects", L"New", L"Create simple MixingMatrix...", L"Create test CrossCorrelationTables...", praat_HIDDEN + praat_DEPTH_1, DO_MixingMatrix_createSimple);

	praat_addAction1 (classCrossCorrelationTable, 0, L"CrossCorrelationTable help", 0, 0, DO_CrossCorrelationTable_help);
	praat_SSCP_as_TableOfReal_init (classCrossCorrelationTable);

	praat_addAction1 (classCrossCorrelationTable, 0, L"To CrossCorrelationTables", 0, 0, DO_CrossCorrelationTable_to_CrossCorrelationTables);

	praat_addAction1 (classCrossCorrelationTables, 0, L"CrossCorrelationTables help", 0, 0, DO_CrossCorrelationTables_help);
	praat_addAction1 (classCrossCorrelationTables, 1, L"Extract CrossCorrelationTable...", 0, 0, DO_CrossCorrelationTables_extractCrossCorrelationTable);
	praat_addAction1 (classCrossCorrelationTables, 1, L"Get diagonality measure...", 0, 0, DO_CrossCorrelationTables_getDiagonalityMeasure);
	praat_addAction1 (classCrossCorrelationTables, 0, L"To Diagonalizer...", 0, 0, DO_CrossCorrelationTables_to_Diagonalizer);

	praat_TableOfReal_init3 (classDiagonalizer);
	praat_addAction1 (classDiagonalizer, 0, L"To MixingMatrix", 0, 0, DO_Diagonalizer_to_MixingMatrix);

	praat_addAction1 (classEEG, 0, L"To Sound (mc modulated)...", L"To ERPTier...", praat_HIDDEN, DO_EEG_to_Sound_modulated);
	praat_addAction1 (classEEG, 0, L"To Sound (frequency shifted)...", L"To ERPTier...", 0, DO_EEG_to_Sound_frequencyShifted);
	praat_addAction1 (classEEG, 0, L"To PCA...", L"To ERPTier...", 0, DO_EEG_to_PCA);
	praat_addAction1 (classEEG, 0, L"To CrossCorrelationTable...", L"To PCA...", praat_HIDDEN, DO_EEG_to_CrossCorrelationTable);
	praat_addAction1 (classEEG, 0, L"To CrossCorrelationTables...", L"To PCA...", praat_HIDDEN, DO_EEG_to_CrossCorrelationTables);

	praat_addAction1 (classEEG, 0, L"To Covariance...", L"To CrossCorrelationTable...", praat_HIDDEN, DO_EEG_to_Covariance);
	praat_addAction1 (classEEG, 0, L"To EEG (bss)...", L"To CrossCorrelationTable...", praat_HIDDEN, DO_EEG_to_EEG_bss);

	praat_addAction2 (classEEG, 1, classPCA, 1, L"To EEG (principal components)...", 0, 0, DO_EEG_and_PCA_to_EEG_principalComponents);
	praat_addAction2 (classEEG, 1, classPCA, 1, L"To EEG (whiten)...", 0, 0, DO_EEG_and_PCA_to_EEG_whiten);


	praat_TableOfReal_init3 (classMixingMatrix);

	praat_addAction1 (classSound, 0, L"To MixingMatrix...",  L"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_MixingMatrix);
    praat_addAction1 (classSound, 0, L"To CrossCorrelationTable...",  L"Resample...", 1, DO_Sound_to_CrossCorrelationTable);
    praat_addAction1 (classSound, 0, L"To Covariance (channels)...",  L"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_Covariance_channels);
	praat_addAction1 (classSound, 0, L"To CrossCorrelationTables...",  L"Resample...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_CrossCorrelationTables);

	praat_addAction1 (classSound, 0, L"To Sound (bss)...", L"Resample...", 1, DO_Sound_to_Sound_bss);
    praat_addAction1 (classSound, 0, L"To Sound (white channels)...", L"Resample...", 1, DO_Sound_to_Sound_whiteChannels);
    praat_addAction1 (classSound, 2, L"To CrossCorrelationTable (combined)...",  L"Cross-correlate...", 1, DO_Sounds_to_CrossCorrelationTable_combined);

	praat_addAction1 (classTableOfReal, 0, L"To MixingMatrix", L"To Configuration", praat_HIDDEN, DO_TableOfReal_to_MixingMatrix);

	praat_addAction2 (classSound, 1, classMixingMatrix, 1, L"Mix", 0, 0, DO_Sound_and_MixingMatrix_mix);
	praat_addAction2 (classSound, 1, classMixingMatrix, 1, L"Unmix", 0, 0, DO_Sound_and_MixingMatrix_unmix);

	praat_addAction2 (classSound, 1, classPCA, 1, L"To Sound (white channels)...", 0 , 0, DO_Sound_and_PCA_whitenChannels);
	praat_addAction2 (classSound, 1, classPCA, 1, L"To Sound (principal components)...", 0 , 0, DO_Sound_and_PCA_principalComponents);

	praat_addAction2 (classCrossCorrelationTable, 1, classDiagonalizer, 1, L"Diagonalize", 0 , 0, DO_CrossCorrelationTable_and_Diagonalizer_diagonalize);

	praat_addAction2 (classCrossCorrelationTables, 1, classDiagonalizer, 1, L"Get diagonality measure...", 0 , 0, DO_CrossCorrelationTables_and_Diagonalizer_getDiagonalityMeasure);
	praat_addAction2 (classCrossCorrelationTables, 1, classDiagonalizer, 1, L"Diagonalize", 0 , 0, DO_CrossCorrelationTables_and_Diagonalizer_diagonalize);
	praat_addAction2 (classCrossCorrelationTables, 1, classDiagonalizer, 1, L"Improve diagonality...", 0 , 0, DO_Diagonalizer_and_CrossCorrelationTables_improveDiagonality);

	praat_addAction2 (classCrossCorrelationTables, 1, classMixingMatrix, 1, L"Improve unmixing...", 0 , 0, DO_CrossCorrelationTables_and_MixingMatrix_improveUnmixing);

	INCLUDE_MANPAGES (manual_BSS)
}

/* End of file praat_BSS_init.c */
