/* praat_LPC_init.c
 *
 * Copyright (C) 1994-2006 David Weenink
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
 djmw 20030613 Latest modification
 djmw 20040414 Forms texts.
 djmw 20060428 Latest modification
 djmw 20061218 Changed to Melder_information<x> format.
*/

#include <math.h>
#include "praat.h"
#include "Cepstrumc.h"
#include "DTW.h"
#include "LPC.h"
#include "MFCC.h"
#include "LFCC.h"
#include "LPC_and_Cepstrumc.h"
#include "LPC_and_Formant.h"
#include "LPC_and_LFCC.h"
#include "LPC_and_Polynomial.h"
#include "LPC_and_Tube.h"
#include "LPC_to_Spectrogram.h"
#include "LPC_to_Spectrum.h"
#include "MelFilter_and_MFCC.h"
#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_and_Cepstrum.h"
#include "Sound_to_MFCC.h"
#include "Cepstrum_and_Spectrum.h"

static char *DRAW_BUTTON    = "Draw -                 ";
static char *QUERY_BUTTON   = "Query -                ";

extern void praat_CC_init (void *klas);
extern void praat_TimeFrameSampled_query_init (void *klas);

/********************** Cepstrum  ****************************************/


DIRECT (Cepstrum_help)
	Melder_help (L"Cepstrum");
END

FORM (Cepstrum_draw, "Cepstrum: Draw", "Cepstrum: Draw...")
	REAL ("Minimum quefrency", "0.0")
	REAL ("Maximum quefrency", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
    BOOLEAN ("Garnish", 0);
	OK
DO
	EVERY_DRAW (Cepstrum_draw (OBJECT, GRAPHICS,
		GET_REAL ("Minimum quefrency"), GET_REAL ("Maximum quefrency"),
		GET_REAL ("Minimum"), GET_REAL ("Maximum"),
		GET_INTEGER ("Garnish")))
END

DIRECT (Cepstrum_to_Spectrum)
	EVERY_TO (Cepstrum_to_Spectrum (OBJECT))
END

DIRECT (Cepstrum_to_Matrix)
	EVERY_TO (Cepstrum_to_Matrix (OBJECT))
END

/********************** Cepstrumc  ****************************************/

DIRECT (Cepstrumc_to_LPC)
	EVERY_TO (Cepstrumc_to_LPC (OBJECT))
END

FORM (Cepstrumc_to_DTW, "Cepstrumc: To DTW", "Cepstrumc: To DTW...")
	LABEL ("", "Distance calculation between Cepstra")
	REAL ("Cepstral weight", "1.0")
	REAL ("Log energy weight", "0.0")
	REAL ("Regression weight", "0.0")
	REAL ("Regression weight log energy", "0.0")
	REAL ("Window for regression coefficients (seconds)", "0.056")
	LABEL("", "Boundary conditions for time warp")
	BOOLEAN ("Match begin positions", 0)
	BOOLEAN ("Match end positions", 0)
	RADIO ("Slope constraints", 1)
	RADIOBUTTON ("no restriction")
	RADIOBUTTON ("1/3 < slope < 3")
	RADIOBUTTON ("1/2 < slope < 2")
	RADIOBUTTON ("2/3 < slope < 3/2")
	OK
DO
	Cepstrumc c1 = NULL, c2 = NULL;
	WHERE (SELECTED && CLASS == classCepstrumc) { if (c1) c2 = OBJECT; else c1 = OBJECT; }
	NEW (Cepstrumc_to_DTW (c1, c2, GET_REAL ("Cepstral weight"),
		GET_REAL ("Log energy weight"), GET_REAL ("Regression weight"),
		GET_REAL ("Regression weight log energy"),
		GET_REAL ("Window for regression coefficients"), 
		GET_INTEGER("Match begin positions"), GET_INTEGER("Match end positions"),
		GET_INTEGER("Slope constraints")))
END

DIRECT (Cepstrumc_to_Matrix)
	EVERY_TO (Cepstrumc_to_Matrix (OBJECT))
END

/******************** Formant ********************************************/

FORM (Formant_to_LPC, "Formant: To LPC", 0)
	POSITIVE("Sampling frequency (Hz)", "16000.0")
	OK
DO
	EVERY_TO (Formant_to_LPC (OBJECT, 1.0/GET_REAL("Sampling frequency")))
END

/********************LFCC ********************************************/

DIRECT (LFCC_help)
	Melder_help (L"LFCC");
END

FORM (LFCC_to_LPC, "LFCC: To LPC", "LFCC: To LPC...")
	INTEGER ("Number of coefficients", "0")
	OK
DO
	long ncof = GET_INTEGER ("Number of coefficients");
	REQUIRE (ncof >= 0, "Number of coefficients must be greater or equal zero.")
	EVERY_TO (LFCC_to_LPC (OBJECT, ncof))
END

/********************LPC ********************************************/

DIRECT (LPC_help) Melder_help (L"LPC"); END

FORM (LPC_drawGain, "LPC: Draw gain", "LPC: Draw gain...")
    REAL ("From time (seconds)", "0.0")
    REAL ("To time (seconds)", "0.0 (=all)")
    REAL ("Minimum gain", "0.0")
    REAL ("Maximum gain", "0.0")
    BOOLEAN ("Garnish", 1)
	OK
DO
    EVERY_DRAW (LPC_drawGain (OBJECT, GRAPHICS, 
    	GET_REAL ("From time"), GET_REAL ("To time"),
    	GET_REAL ("Minimum gain"), GET_REAL ("Maximum gain"),
		GET_INTEGER("Garnish")))
END

DIRECT (LPC_getSamplingInterval)
	LPC me = ONLY (classLPC);
	Melder_information2 (Melder_double (my samplingPeriod), L" seconds");
END

FORM (LPC_getNumberOfCoefficients, "LPC: Get number of coefficients", "LPC: Get number of coefficients...")
	NATURAL ("Frame number", "1")
	OK
DO
	LPC me = ONLY (classLPC);
	long iframe = GET_INTEGER ("Frame number");
	if (iframe > my nx)
	{
		(void) Melder_error ("Frame number is too large.\n\nPlease choose a number between 1 and %ld.", my nx);
		Melder_information1 (L"-1 coefficients (frame number was not defined)");
		return 0;
	}
	Melder_information2 (Melder_integer ((my frame[iframe]).nCoefficients), L" coefficients");
END

FORM (LPC_drawPoles, "LPC: Draw poles", "LPC: Draw poles...")
    REAL ("Time (seconds)", "0.0")
    BOOLEAN ("Garnish", 1)
    OK
DO
	EVERY_DRAW (LPC_drawPoles (OBJECT, GRAPHICS, GET_REAL ("Time"),
		GET_INTEGER ("Garnish")))
END

DIRECT (LPC_to_Formant)
	EVERY_TO (LPC_to_Formant (OBJECT, 50))
END

DIRECT (LPC_to_Formant_keep_all)
	EVERY_TO (LPC_to_Formant (OBJECT, 0))
END

DIRECT (LPC_to_Cepstrumc)
	EVERY_TO (LPC_to_Cepstrumc (OBJECT))
END

FORM (LPC_to_LFCC, "LPC: To LFCC", "LPC: To LFCC...")
	INTEGER ("Number of coefficients", "0")
	OK
DO
	long ncof = GET_INTEGER ("Number of coefficients");
	REQUIRE (ncof >= 0, "Number of coefficients must be greater or equal zero.")
	EVERY_TO (LPC_to_LFCC (OBJECT, ncof))
END

FORM (LPC_to_Polynomial, "LPC: To Polynomial", "LPC: To Polynomial (slice)...")
	REAL ("Time (seconds)", "0.0")
	OK
DO
	EVERY_TO (LPC_to_Polynomial (OBJECT, GET_REAL("Time")))
END

FORM (LPC_to_Spectrum, "LPC: To Spectrum", "LPC: To Spectrum (slice)...")
	REAL ("Time (seconds)", "0.0")
	REAL ("Minimum frequency resolution (Hz)", "20.0")
	REAL ("Bandwidth reduction (Hz)", "0.0")
	REAL ("De-emphasis frequency (Hz)", "50.0")
	OK
DO
	EVERY_TO (LPC_to_Spectrum (OBJECT, GET_REAL("Time"),
		GET_REAL("Minimum frequency resolution"),
		GET_REAL("Bandwidth reduction"), GET_REAL("De-emphasis frequency")))
END

FORM (LPC_to_Spectrogram, "LPC: To Spectrogram", "LPC: To Spectrogram...")
	REAL ("Minimum frequency resolution (Hz)", "20.0")
	REAL ("Bandwidth reduction (Hz)", "0.0")
	REAL ("De-emphasis frequency (Hz)", "50.0")
	OK
DO
	EVERY_TO (LPC_to_Spectrogram (OBJECT, GET_REAL("Minimum frequency resolution"),
		GET_REAL("Bandwidth reduction"), GET_REAL("De-emphasis frequency")))
END

FORM (LPC_to_VocalTract, "LPC: To VocalTract", "LPC: To VocalTract (slice)...")
	REAL ("Time (s)", "0.0")
	POSITIVE ("Length (m)", "0.17")
	BOOLEAN("Length according to Wakita", 0)
	OK
DO
	EVERY_TO (LPC_to_VocalTract (OBJECT, GET_REAL("Time"), GET_REAL("Length"),
		GET_INTEGER("Length according to Wakita")))
END

DIRECT (LPC_to_Matrix)
	EVERY_TO (LPC_to_Matrix (OBJECT))
END

/********************** Sound *******************************************/

static void Sound_to_LPC_addCommonFields (void *dia) {
	LABEL ("", "Warning 1:  for formant analysis, use \"To Formant\" instead.")
	LABEL ("", "Warning 2:  if you do use \"To LPC\", you may want to resample first.")
	LABEL ("", "Click Help for more details.")
	LABEL ("", "")
	NATURAL ("Prediction order", "16")
	POSITIVE ("Analysis window duration (s)", "0.025")
	POSITIVE ("Time step (s)", "0.005")
	REAL ("Pre-emphasis frequency (Hz)", "50.0")
}
static int Sound_to_LPC_checkCommonFields (void * dia,
	long *predictionOrder,
	double *analysisWindowDuration,
	double *timeStep,
	double *preemphasisFrequency)
{
	*predictionOrder = GET_INTEGER ("Prediction order");
	*analysisWindowDuration = GET_REAL ("Analysis window duration");
	*timeStep = GET_REAL ("Time step");
	*preemphasisFrequency = GET_REAL ("Pre-emphasis frequency");
	if (*preemphasisFrequency < 0.0) {
		(void) Melder_error ("Pre-emphasis frequencies cannot be negative.\n");
		return Melder_error ("Please use a positive or zero pre-emphasis frequency.");
	}
	return 1;
}

FORM (Sound_to_LPC_auto, "Sound: To LPC (autocorrelation)", "Sound: To LPC (autocorrelation)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	if (! Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency))
		return 0;
	EVERY_TO (Sound_to_LPC_auto (OBJECT, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency))
END

FORM (Sound_to_LPC_covar, "Sound: To LPC (covariance)", "Sound: To LPC (covariance)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	if (! Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency))
		return 0;
	EVERY_TO (Sound_to_LPC_covar (OBJECT, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency))
END

FORM (Sound_to_LPC_burg, "Sound: To LPC (burg)", "Sound: To LPC (burg)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	if (! Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency))
		return 0;
	EVERY_TO (Sound_to_LPC_burg (OBJECT, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency))
END

FORM (Sound_to_LPC_marple, "Sound: To LPC (marple)", "Sound: To LPC (marple)...")
	Sound_to_LPC_addCommonFields (dia);
	POSITIVE("Tolerance 1", "1e-6")
	POSITIVE("Tolerance 2", "1e-6")
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	if (! Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency))
		return 0;
	EVERY_TO (Sound_to_LPC_marple (OBJECT, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency,
		GET_REAL("Tolerance 1"), GET_REAL("Tolerance 2")))
END

DIRECT (Sound_to_Cepstrum)
	EVERY_TO (Sound_to_Cepstrum (OBJECT))
END

FORM (Sound_to_MFCC, "Sound: To MFCC",
	"Sound: To MFCC...")
	NATURAL ("Number of coefficients", "12")
	POSITIVE ("Analysis window duration (s)", "0.015")
	POSITIVE ("Time step (s)", "0.005")
	LABEL("","Filter bank parameters")
	POSITIVE ("Position of first filter (mel)", "100.0")
	POSITIVE ("Distance between filters (mel)", "100.0")
	REAL ("Maximum frequency (mel)", "0.0");
	OK
DO
	long p = GET_INTEGER ("Number of coefficients");
	REQUIRE (p < 25, "Number of coefficients must be < 25.")
	EVERY_TO (Sound_to_MFCC (OBJECT, p, GET_REAL ("Analysis window duration"),
		GET_REAL ("Time step"), GET_REAL ("Position of first filter"), 
		GET_REAL ("Maximum frequency"), GET_REAL ("Distance between filters")))
END

DIRECT (VocalTract_getLength)
	VocalTract v = ONLY_OBJECT;
    double length = v -> xmax - v -> xmin;
    if (length <= 0.02) length = NUMundefined;
	Melder_information2 (Melder_integer (length), L" m");
END

/******************* LPC & Sound *************************************/

FORM (LPC_and_Sound_filter, "LPC & Sound: Filter", "LPC & Sound: Filter...")
	BOOLEAN ("Use LPC gain", 0)
	OK
DO
	NEW (LPC_and_Sound_filter (ONLY(classLPC) , ONLY(classSound),
		GET_INTEGER ("Use LPC gain")))
END

DIRECT (LPC_and_Sound_filterInverse)
	NEW (LPC_and_Sound_filterInverse (ONLY(classLPC) , ONLY(classSound)))
END

FORM (LPC_and_Sound_to_LPC_robust, "Robust LPC analysis",
	"LPC & Sound: To LPC (robust)...")
	POSITIVE ("Analysis width (s)", "0.025")
	POSITIVE ("Pre-emphasis frequency (Hz)", "50.0")
	POSITIVE ("Number of std. dev.", "1.5")
	NATURAL ("Maximum number of iterations", "5")
	REAL ("Tolerance", "0.000001")
	BOOLEAN ("Variable location", 0)
	OK
DO
	Sound sound = ONLY (classSound);	
	if (! praat_new (LPC_and_Sound_to_LPC_robust (ONLY (classLPC), sound,
		GET_REAL ("Analysis width"), GET_REAL ("Pre-emphasis frequency"), 
		GET_REAL ("Number of std. dev."), GET_INTEGER ("Maximum number of iterations"),  
		GET_REAL ("Tolerance"), GET_INTEGER ("Variable location")),
			"%s_r", sound -> name)) return 0;
END

void praat_uvafon_LPC_init (void);
void praat_uvafon_LPC_init (void)
{
	Thing_recognizeClassesByName (classCepstrumc, classLPC, classLFCC,
		classMFCC, NULL);

	praat_addAction1 (classCepstrum, 0, "Cepstrum help", 0, 0,
		DO_Cepstrum_help);
	praat_addAction1 (classCepstrum, 0, "Draw...", 0, 0, DO_Cepstrum_draw);
	praat_addAction1 (classCepstrum, 0, "To Spectrum", 0, 0, 
		DO_Cepstrum_to_Spectrum);
	praat_addAction1 (classCepstrum, 0, "To Matrix", 0, 0, 
		DO_Cepstrum_to_Matrix);


	praat_addAction1 (classCepstrumc, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, "To LPC", 0, 0, DO_Cepstrumc_to_LPC);
	praat_addAction1 (classCepstrumc, 2, "To DTW...", 0, 0, DO_Cepstrumc_to_DTW);
	praat_addAction1 (classCepstrumc, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, "To Matrix", 0, 0, DO_Cepstrumc_to_Matrix);
	
	praat_addAction1 (classFormant, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classFormant, 0, "To LPC...", 0, 0, DO_Formant_to_LPC);
	
	praat_addAction1 (classLFCC, 0, "LFCC help", 0, 0, DO_LFCC_help);
	praat_CC_init (classLFCC);
	praat_addAction1 (classLFCC, 0, "To LPC...", 0, 0, DO_LFCC_to_LPC);
	
	praat_addAction1 (classLPC, 0, "LPC help", 0, 0, DO_LPC_help);
	praat_addAction1 (classLPC, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classLPC, 0, "Draw gain...", 0, 1, DO_LPC_drawGain);
	praat_addAction1 (classLPC, 0, "Draw poles...", 0, 1, DO_LPC_drawPoles);
	praat_addAction1 (classLPC, 0, QUERY_BUTTON, 0, 0, 0);
	praat_TimeFrameSampled_query_init (classLPC);
	praat_addAction1 (classLPC, 1, "Get sampling interval", 0, 1, DO_LPC_getSamplingInterval);
	praat_addAction1 (classLPC, 1, "Get number of coefficients...", 0, 1, DO_LPC_getNumberOfCoefficients);
	praat_addAction1 (classLPC, 0, "Extract", 0, 0, 0);
	
	praat_addAction1 (classLPC, 0, "To Spectrum (slice)...", 0, 0,
		DO_LPC_to_Spectrum);
	praat_addAction1 (classLPC, 0, "To VocalTract (slice)...", 0, 0,
		DO_LPC_to_VocalTract);
	praat_addAction1 (classLPC, 0, "To Polynomial (slice)...", 0, 0,
		DO_LPC_to_Polynomial);
	praat_addAction1 (classLPC, 0, "To Matrix", 0, 0,
		DO_LPC_to_Matrix);
	praat_addAction1 (classLPC, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classLPC, 0, "To Formant", 0, 0, DO_LPC_to_Formant);
	praat_addAction1 (classLPC, 0, "To Formant (keep all)", 0, 0, 
		DO_LPC_to_Formant_keep_all);
/*	praat_addAction1 (classLPC, 0, "To Cepstrumc", 0, 0, DO_LPC_to_Cepstrumc);*/
	praat_addAction1 (classLPC, 0, "To LFCC...", 0, 0, DO_LPC_to_LFCC);
	praat_addAction1 (classLPC, 0, "To Spectrogram...", 0, 0,
		DO_LPC_to_Spectrogram);
	
	praat_addAction2 (classLPC, 1, classSound, 1, "Analyse", 0, 0, 0);
	praat_addAction2 (classLPC, 1, classSound, 1, "Filter...", 0, 0,
		DO_LPC_and_Sound_filter);
	praat_addAction2 (classLPC, 1, classSound, 1, "Filter (inverse)", 0, 0,
		DO_LPC_and_Sound_filterInverse);
	praat_addAction2 (classLPC, 1, classSound, 1, "To LPC (robust)...", 0,
		praat_HIDDEN + praat_DEPTH_1, DO_LPC_and_Sound_to_LPC_robust);

	praat_addAction1 (classSound, 0, "To LPC (autocorrelation)...",
		"To Formant (sl)...", 1, DO_Sound_to_LPC_auto);
	praat_addAction1 (classSound, 0, "To LPC (covariance)...",
		"To LPC (autocorrelation)...", 1, DO_Sound_to_LPC_covar);
	praat_addAction1 (classSound, 0, "To LPC (burg)...",
		"To LPC (covariance)...", 1, DO_Sound_to_LPC_burg);
	praat_addAction1 (classSound, 0, "To LPC (marple)...",
		"To LPC (burg)...", 1, DO_Sound_to_LPC_marple);
/*	praat_addAction1 (classSound, 0, "To Cepstrum",
		"To Spectrum", 1, DO_Sound_to_Cepstrum);*/
	praat_addAction1 (classSound, 0, "To MFCC...","To LPC (marple)...", 1,
		DO_Sound_to_MFCC);

	praat_addAction1 (classVocalTract, 1, "Get length", "Draw", 0,
		DO_VocalTract_getLength);

	INCLUDE_MANPAGES (manual_LPC_init)
	
}

/* End of file praat_LPC_init.c */
