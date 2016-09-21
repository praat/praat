/* praat_HMM_init.cpp
 *
 * Copyright (C) 2010-2011,2015 David Weenink
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
#include "GaussianMixture.h"
#include "HMM.h"
#include "Strings_extensions.h"
#include "TableOfReal.h"

#undef iam
#define iam iam_LOOP

//#undef praat_HIDDEN
//#define praat_HIDDEN 0

#define GaussianMixture_OPTION_MENU_CRITERIA \
	OPTIONMENU (U"Criterion based on", 1) \
		OPTION (U"Likelihood") \
		OPTION (U"Message length") \
		OPTION (U"Bayes information") \
		OPTION (U"Akaike information") \
		OPTION (U"Akaike corrected") \
		OPTION (U"Complete-data ML")

DIRECT (GaussianMixture_help)
	Melder_help (U"GaussianMixture");
END

FORM (GaussianMixture_drawConcentrationEllipses, U"GaussianMixture: Draw concentration ellipses", U"GaussianMixture: Draw concentration ellipses...")
	POSITIVE (U"Number of sigmas", U"1.0")
	BOOLEAN (U"Principal component plane", true)
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
		iam (GaussianMixture);
		GaussianMixture_drawConcentrationEllipses (me, GRAPHICS,
			GET_REAL (U"Number of sigmas"), 0, 0, GET_INTEGER (U"Principal component plane"),
			GET_INTEGER (U"X-dimension"), GET_INTEGER (U"Y-dimension"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Label size"), GET_INTEGER (U"Garnish"));
	}
END

FORM (GaussianMixture_and_PCA_drawConcentrationEllipses, U"GaussianMixture & PCA: Draw concentration ellipses", U"GaussianMixture & PCA: Draw concentration ellipses...")
	POSITIVE (U"Number of sigmas", U"1.0")
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
	GaussianMixture me = FIRST (GaussianMixture);
	PCA pca = FIRST (PCA);
	GaussianMixture_and_PCA_drawConcentrationEllipses (me, pca, GRAPHICS,
		GET_REAL (U"Number of sigmas"), 0, nullptr, GET_INTEGER (U"X-dimension"), GET_INTEGER (U"Y-dimension"),
		GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
		GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
		GET_INTEGER (U"Label size"), GET_INTEGER (U"Garnish"));
END

FORM (GaussianMixture_drawMarginalPdf, U"GaussianMixture: Draw marginal pdf", U"GaussianMixture: Draw marginal pdf...")
	INTEGER (U"X-dimension", U"1")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	NATURAL (U"Number of points", U"500")
	INTEGER (U"Number of bins", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (GaussianMixture);
		GaussianMixture_drawMarginalPdf (me, GRAPHICS, GET_INTEGER (U"X-dimension"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Number of points"), GET_INTEGER (U"Number of bins"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (GaussianMixture_getNumberOfComponents)
	LOOP {
		iam (GaussianMixture);
		Melder_information (my numberOfComponents, U" (= number of components)");
	}
END

DIRECT (GaussianMixture_getDimensionOfComponent)
	LOOP {
		iam (GaussianMixture);
		Melder_information (my dimension, U" (= dimension of component)");
	}
END

FORM (GaussianMixture_getProbabilityAtPosition, U"GaussianMixture: Get probability at position", nullptr)
	SENTENCE (U"Position", U"100.0 300.0")
	OK
DO
	const char32 *position = GET_STRING (U"Position");
	LOOP {
		iam (GaussianMixture);
		double p = GaussianMixture_getProbabilityAtPosition_string (me, position);
		Melder_information (p, U" (= probability at position ", position);
	}
END

FORM (GaussianMixture_splitComponent, U"GaussianMixture: Split component", U"GaussianMixture: Split component...")
	NATURAL (U"Component", U"1")
	OK
DO
	LOOP {
		iam (GaussianMixture);
		GaussianMixture_splitComponent (me, GET_INTEGER (U"Component"));
	}
END

FORM (GaussianMixture_and_PCA_drawMarginalPdf, U"GaussianMixture & PCA: Draw pdf function", U"GaussianMixture: Draw marginal pdf...")
	INTEGER (U"X-dimension", U"1")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	NATURAL (U"Number of points", U"500")
	INTEGER (U"Number of bins", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	GaussianMixture me = FIRST (GaussianMixture);
	PCA thee = FIRST (PCA);
	GaussianMixture_and_PCA_drawMarginalPdf (me, thee, GRAPHICS,
		GET_INTEGER (U"X-dimension"),
		GET_REAL (U"left Horizontal range"),
		GET_REAL (U"right Horizontal range"),
		GET_REAL (U"left Vertical range"),
		GET_REAL (U"right Vertical range"),
		GET_INTEGER (U"Number of points"),
		GET_INTEGER (U"Number of bins"),
		GET_INTEGER (U"Garnish"));
END

FORM (GaussianMixture_and_PCA_to_Matrix_density, U"GaussianMixture & PCA: To Matrix density", U"GaussianMixture & PCA: To Matrix (density)...")
	INTEGER (U"X-dimension", U"1")
	INTEGER (U"Y-dimension", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	NATURAL (U"Number of columns", U"100")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	NATURAL (U"Number of rows", U"100")
	OK
DO
	GaussianMixture me = FIRST (GaussianMixture);
	PCA thee = FIRST (PCA);
	autoMatrix result = GaussianMixture_and_PCA_to_Matrix_density (me, thee,
		GET_INTEGER (U"X-dimension"),
		GET_INTEGER (U"Y-dimension"),
		GET_REAL (U"left Horizontal range"),
		GET_REAL (U"right Horizontal range"),
		GET_INTEGER (U"Number of columns"),
		GET_REAL (U"left Vertical range"),
		GET_REAL (U"right Vertical range"),
		GET_INTEGER (U"Number of rows"));
	praat_new (result.move(), my name, U"_", thy name);
END

FORM (GaussianMixture_extractComponent, U"GaussianMixture: Extract component", nullptr)
	NATURAL (U"Component", U"1")
	OK
DO
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_extractComponent (me,
			GET_INTEGER (U"Component"));
		praat_new (result.move(), my name, U"_", result -> name);
	}
END

DIRECT (GaussianMixture_extractCentroids)
	LOOP {
		iam (GaussianMixture);
		autoTableOfReal result = GaussianMixture_extractCentroids (me);
		praat_new (result.move(), my name);
	}
END

DIRECT (GaussianMixture_extractMixingProbabilities)
	LOOP {
		iam (GaussianMixture);
		autoTableOfReal result = GaussianMixture_extractMixingProbabilities (me);
		praat_new (result.move(), my name);
	}
END

DIRECT (GaussianMixture_to_PCA)
	LOOP {
		iam (GaussianMixture);
		autoPCA result = GaussianMixture_to_PCA (me);
		praat_new (result.move(), my name);
	}
END

FORM (GaussianMixture_to_TableOfReal_randomSampling, U"GaussianMixture: To TableOfReal (random sampling)", U"GaussianMixture: To TableOfReal (random sampling)...")
	NATURAL (U"Number of data points", U"100")
	OK
DO
	long numberOfpoints = GET_INTEGER (U"Number of data points");
	LOOP {
		iam (GaussianMixture);
		autoTableOfReal result = GaussianMixture_to_TableOfReal_randomSampling (me, numberOfpoints);
		praat_new (result.move(), my name);
	}
END

DIRECT (GaussianMixture_to_Covariance_between)
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_to_Covariance_between (me);
		praat_new (result.move(), my name, U"_b");
	}
END

DIRECT (GaussianMixture_to_Covariance_within)
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_to_Covariance_within (me);
		praat_new (result.move(), my name, U"_w");
	}
END

DIRECT (GaussianMixture_to_Covariance_total)
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_to_Covariance_total (me);
		praat_new (result.move(), my name, U"_t");
	}
END

FORM (GaussianMixture_and_TableOfReal_getLikelihoodValue, U"GaussianMixture & TableOfReal: Get likelihood value",
      U"GaussianMixture & TableOfReal: Get likelihood value...")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	int criterion = GET_INTEGER (U"Criterion based on") - 1;
	const char32 *criterionText = GaussianMixture_criterionText (criterion);
	double lnpdn = GaussianMixture_and_TableOfReal_getLikelihoodValue (me, thee, criterion);
	Melder_information (lnpdn / thy numberOfRows, U" (= ", criterionText, U", n = ",
		thy numberOfRows, U")");
END

DIRECT (HMM_help)
	Melder_help (U"HMM");
END

FORM (HMM_create, U"Create HMM", nullptr)
	WORD (U"Name", U"hmm")
	BOOLEAN (U"Left to right model", false)
	NATURAL (U"Number of states", U"3")
	NATURAL (U"Number of observations", U"3")
	OK
DO
	autoHMM thee = HMM_create (
		GET_INTEGER (U"Left to right model"),
		GET_INTEGER (U"Number of states"),
		GET_INTEGER (U"Number of observations"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END

FORM (HMM_createSimple, U"HMM: Create simple", U"HMM: Create simple HMM...")
	WORD (U"Name", U"weather")
	BOOLEAN (U"Left to right model", false)
	SENTENCE (U"States", U"Rainy Sunny")
	SENTENCE (U"Observations", U"Walk Shop Clean")
	OK
DO
	autoHMM thee = HMM_createSimple (GET_INTEGER (U"Left to right model"), GET_STRING (U"States"), GET_STRING (U"Observations"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END

FORM (HMM_createContinuousModel, U"HMM: Create continuous model", nullptr)
	WORD (U"Name", U"cm")
	BOOLEAN (U"Left to right model", false)
	NATURAL (U"Number of states", U"3")
	NATURAL (U"Number of symbols", U"10")
	LABEL (U"", U"For the Gaussian mixtures:")
	NATURAL (U"Number of components", U"3")
	NATURAL (U"Dimension of component", U"39")
	OPTIONMENU (U"Covariance matrices are", 1)
		OPTION (U"Complete")
		OPTION (U"Diagonal")
	OK
DO
	long componentStorage = GET_INTEGER (U"Covariance matrices are") - 1;
	long dimension = GET_INTEGER (U"Dimension of component");
	REQUIRE (componentStorage >= 0 && componentStorage <= dimension, U"Not a valid covariance matrix type")
	autoHMM thee = HMM_createContinuousModel (
		GET_INTEGER (U"Left to right model"),
		GET_INTEGER (U"Number of states"),
		GET_INTEGER (U"Number of symbols"),
		GET_INTEGER (U"Number of components"),
		dimension,
		componentStorage);
	praat_new (thee.move(), GET_STRING (U"Name"));
END

FORM (HMMObservationSequence_to_HMM, U"HMMObservationSequence: To HMM", nullptr)
	LABEL (U"", U"(0 states gives a non-hidden model) ")
	INTEGER (U"Number of hidden states", U"2")
	BOOLEAN (U"Left to right model", false)
	OK
DO
	long numberOfStates = GET_INTEGER (U"Number of hidden states");
	LOOP {
		iam (HMMObservationSequence);
		autoHMM result = HMM_createFromHMMObservationSequence (me,
			numberOfStates,
			GET_INTEGER (U"Left to right model"));
		praat_new (result.move(), my name, U"_", numberOfStates);
	}
END

FORM (HMM_draw, U"HMM: Draw", nullptr)
	BOOLEAN (U"Garnish", false)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (HMM);
		HMM_draw (me, GRAPHICS,
			GET_INTEGER (U"Garnish"));
	}
END

FORM (HMM_and_HMMStateSequence_drawTrellis, U"HMM & Strings: Draw trellis", nullptr)
	BOOLEAN (U"Connect", true);
	BOOLEAN (U"Garnish", true);
	OK
DO
	autoPraatPicture picture;
	HMM me = FIRST (HMM);
	HMMStateSequence hmmStateSequence = FIRST (HMMStateSequence);
	HMM_and_HMMStateSequence_drawTrellis (me, hmmStateSequence, GRAPHICS,
		GET_INTEGER (U"Connect"),
		GET_INTEGER (U"Garnish"));
END

DIRECT (HMM_drawForwardProbabilitiesIllustration)
	autoPraatPicture picture;
	HMM_drawForwardProbabilitiesIllustration (GRAPHICS, true);
END

DIRECT (HMM_drawBackwardProbabilitiesIllustration)
	autoPraatPicture picture;
	HMM_drawBackwardProbabilitiesIllustration (GRAPHICS, true);
END

DIRECT (HMM_drawForwardAndBackwardProbabilitiesIllustration)
	autoPraatPicture picture;
	HMM_drawForwardAndBackwardProbabilitiesIllustration (GRAPHICS, true);
END

FORM (HMM_getTransitionProbability, U"HMM: Get transition probability", U"HMM: Get transition probability...")
	NATURAL (U"From state number", U"1")
	NATURAL (U"To state number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		long s1 = GET_INTEGER (U"From state number"), s2 = GET_INTEGER (U"To state number");
		REQUIRE (s1 <= my numberOfStates && s2 <= my numberOfStates, U"State number(s) too high.")
		Melder_information (my transitionProbs[s1][s2], U" : [ ", s1, U", ", s2, U" ]");
	}
END

FORM (HMM_getEmissionProbability, U"HMM: Get emission probability", U"HMM: Get emission probability...")
	NATURAL (U"From state number", U"1")
	NATURAL (U"To symbol number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		long s1 = GET_INTEGER (U"From state number"), s2 = GET_INTEGER (U"To symbol number");
		REQUIRE (s1 <= my numberOfStates, U"State number too high.")
		REQUIRE (s2 <= my numberOfObservationSymbols, U"Symbol number too high.")
		Melder_information (my emissionProbs[s1][s2], U" : [ ", s1, U", ", s2, U" ]");
	}
END

FORM (HMM_getStartProbability, U"HMM: Get start probability", U"HMM: Get start probability...")
	NATURAL (U"State number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		long s1 = GET_INTEGER (U"State number");
		REQUIRE (s1 <= my numberOfStates, U"State number too high.")
		Melder_information (my transitionProbs[0][s1], U" : [ ", s1, U" ]");
	}
END

FORM (HMM_getProbabilityAtTimeBeingInState, U"HMM: Get probability of being in state at time",
      U"HMM: Get p (time, state)...")
	NATURAL (U"Time index", U"10")
	NATURAL (U"State number", U"1")
	OK
DO
	long istate = GET_INTEGER (U"State number");
	long itime = GET_INTEGER (U"Time index");
	LOOP {
		iam (HMM);
		double lnp = HMM_getProbabilityAtTimeBeingInState (me, itime, istate);
		Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U") Being in state ",
			istate, U" at time ", itime);
	}
END

FORM (HMM_getProbabilityAtTimeBeingInStateEmittingSymbol, U"HMM: get probability being at time in state emitting symbol", U"HMM: Get p (time, state, symbol)...")
	NATURAL (U"Time index", U"10")
	NATURAL (U"State number", U"1")
	NATURAL (U"Symbol number", U"1")
	OK
DO
	long istate = GET_INTEGER (U"State number");
	long itime = GET_INTEGER (U"Time index");
	long isymbol = GET_INTEGER (U"Symbol number");
	LOOP {
		iam (HMM);
		double lnp = HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (me,
			itime,
			istate,
			isymbol);
		Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U") Being in state ",
			istate, U" emitting symbol ", isymbol, U" at time ", itime);
	}
END

FORM (HMM_getProbabilityOfStayingInState, U"HMM: Get probability of staying in state", U"HMM: Get probability staying in state...")
	NATURAL (U"State number", U"1")
	NATURAL (U"Number of times", U"2")
	OK
DO
	LOOP {
		iam (HMM);
		Melder_informationReal (HMM_getProbabilityOfStayingInState (me,
			GET_INTEGER (U"State number"),
			GET_INTEGER (U"Number of times")), nullptr);
	}
END

FORM (HMM_getExpectedValueOfDurationInState, U"HMM: Get expected value of duration in state",
      U"HMM: Get expected duration in state...")
	NATURAL (U"State number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		Melder_informationReal (HMM_getExpectedValueOfDurationInState (me,
			GET_INTEGER (U"State number")), U" time units");
	}
END

FORM (HMM_getSymbolLabel, U"HMM: Get symbol label", nullptr)
	NATURAL (U"Symbol number", U"1")
	OK
DO
	long is = GET_INTEGER (U"Symbol number");
	LOOP {
		iam (HMM);
		REQUIRE (is <= my numberOfObservationSymbols, U"Symbol number too high.")
		HMMObservation s = my observationSymbols->at [is];
		Melder_information (s -> label);
	}
END

FORM (HMM_getStateLabel, U"HMM: Get state label", nullptr)
	NATURAL (U"State number", U"1")
	OK
DO
	long stateNumber = GET_INTEGER (U"State number");
	LOOP {
		iam (HMM);
		REQUIRE (stateNumber <= my numberOfStates, U"State number too high.")
		HMMState state = my states->at [stateNumber];
		Melder_information (state -> label);
	}
END

FORM (HMM_and_HMM_getCrossEntropy, U"HMM & HMM: Get cross-entropy...", U"HMM & HMM: Get cross-entropy...")
	NATURAL (U"Observation length", U"2000")
	BOOLEAN (U"Symmetric", true)
	OK
DO
	long n = GET_INTEGER (U"Observation length");
	int sym = GET_INTEGER (U"Symmetric");
	HMM m1 = nullptr, m2 = nullptr;
	LOOP {
		iam (HMM);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	double ce = HMM_and_HMM_getCrossEntropy (m1, m2, n, sym);
	Melder_information (ce, U" (= ", (sym ? U"symmetric " : U""),
		U" cross-entropy between models for observation length = ", n, U")");
END

DIRECT (HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy)
	HMM m1 = nullptr, m2 = nullptr;
	HMMObservationSequence hmm_os = nullptr;
	LOOP {
		if (CLASS == classHMMObservationSequence) {
			hmm_os = (HMMObservationSequence) OBJECT;
		} else { (m1 ? m2 : m1) = (HMM) OBJECT; }
	}
	Melder_assert (m1 && m2 && hmm_os);
	double ce = HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy (m1, m2, hmm_os);
	Melder_information (ce, U" (= symmetric cross-entropy between models)");
END

FORM (HMM_to_HMMObservationSequence, U"HMM: To HMMObservationSequence (generate observations)", U"HMM: To HMMObservationSequence...")
	INTEGER (U"Start state", U"0")
	NATURAL (U"Number of observations", U"20")
	OK
DO
	LOOP {
		iam (HMM);
		autoHMMObservationSequence thee = HMM_to_HMMObservationSequence (me, GET_INTEGER (U"Start state"), GET_INTEGER (U"Number of observations"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (HMM_and_HMMStateSequence_getProbability)
	HMM me = FIRST (HMM);
	HMMStateSequence hmm_ss = FIRST (HMMStateSequence);
	double lnp = HMM_and_HMMStateSequence_getProbability (me, hmm_ss);
	Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U")");
END

DIRECT (HMM_and_HMMObservationSequence_getProbability)
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	double lnp = HMM_and_HMMObservationSequence_getProbability (me, hmm_os);
	Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U")");
END

DIRECT (HMM_and_HMMObservationSequence_getCrossEntropy)
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	double ce = HMM_and_HMMObservationSequence_getCrossEntropy (me, hmm_os);
	Melder_information (ce, U" (= cross-entropy)");
END

DIRECT (HMM_and_HMMObservationSequence_getPerplexity)
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	double py = HMM_and_HMMObservationSequence_getPerplexity (me, hmm_os);
	Melder_information (py, U" (= perplexity)");
END

DIRECT (HMM_and_HMMObservationSequence_to_HMMStateSequence)
	HMM me = FIRST (HMM);
	HMMObservationSequence thee = FIRST (HMMObservationSequence);
	autoHMMStateSequence result = HMM_and_HMMObservationSequence_to_HMMStateSequence (me, thee);
	praat_new (result.move(), my name, U"_", thy name, U"_states");
END

FORM (HMM_and_HMMObservationSequence_learn, U"HMM & HMMObservationSequence: Learn", U"HMM & HMMObservationSequences: Learn...")
	POSITIVE (U"Relative precision in log(p)", U"0.001")
	REAL (U"Minimum probability", U"0.00000000001")
	BOOLEAN (U"Learning history in Info window", false)
	OK
DO
	double minProb = GET_REAL (U"Minimum probability");
	REQUIRE (minProb >= 0 && minProb < 1, U"A probabilty must be >= 0 and < 1!")
	autoHMMObservationSequenceBag hmmObservationSequences = HMMObservationSequenceBag_create ();
	HMM hmm = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classHMMObservationSequence) {
			hmmObservationSequences -> addItem_ref ((HMMObservationSequence) me);
		} else {
			hmm = (HMM) me;
		}
	}
	HMM_and_HMMObservationSequenceBag_learn (hmm, hmmObservationSequences.get(),
		GET_REAL (U"Relative precision in log"),
		minProb,
		GET_INTEGER (U"Learning history in Info window"));
END

FORM (HMM_setTransitionProbabilities, U"HMM: Set transition probabilities", U"HMM: Set transition probabilities...")
	NATURAL (U"State number", U"1")
	SENTENCE (U"Probabilities", U"0.1 0.9")
	OK
DO
	LOOP {
		iam (HMM);
		HMM_setTransitionProbabilities (me,
			GET_INTEGER (U"State number"),
			GET_STRING (U"Probabilities"));
	}
END

FORM (HMM_setEmissionProbabilities, U"HMM: Set emission probabilities", U"HMM: Set emission probabilities...")
	NATURAL (U"State number", U"1")
	SENTENCE (U"Probabilities", U"0.1 0.7 0.2")
	OK
DO
	LOOP {
		iam (HMM);
		HMM_setEmissionProbabilities (me,
			GET_INTEGER (U"State number"),
			GET_STRING (U"Probabilities"));
	}
END

FORM (HMM_setStartProbabilities, U"HMM: Set start probabilities", U"HMM: Set start probabilities...")
	SENTENCE (U"Probabilities", U"0.1 0.9")
	OK
DO
	LOOP {
		iam (HMM);
		HMM_setStartProbabilities (me,
			GET_STRING (U"Probabilities"));
	}
END

DIRECT (HMM_extractTransitionProbabilities)
	LOOP {
		iam (HMM);
		autoTableOfReal result = HMM_extractTransitionProbabilities (me);
		praat_new (result.move(), my name, U"_t");
	}
END

DIRECT (HMM_extractEmissionProbabilities)
	LOOP {
		iam (HMM);
		autoTableOfReal result = HMM_extractEmissionProbabilities (me);
		praat_new (result.move(), my name, U"_e");
	}
END

FORM (HMMObservationSequence_to_TableOfReal, U"HMMObservationSequence: To TableOfReal ", U"HMMObservationSequence: To TableOfReal (bigrams)...")
	BOOLEAN (U"As probabilities", true)
	OK
DO
	LOOP {
		iam (HMMObservationSequence);
		autoTableOfReal result = HMMObservationSequence_to_TableOfReal_transitions (me,
			GET_INTEGER (U"As probabilities"));
		praat_new (result.move(), my name);
	}
END

FORM (HMM_and_HMMObservationSequence_to_TableOfReal, U"HMM & HMMObservationSequence: To TableOfReal", U"HMM & HMMObservationSequence: To TableOfReal (bigrams)...")
	BOOLEAN (U"As probabilities", true)
	OK
DO
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	autoTableOfReal thee = HMM_and_HMMObservationSequence_to_TableOfReal_transitions (me, hmm_os,GET_INTEGER (U"As probabilities"));
	praat_new (thee.move(), hmm_os -> name, U"_m");
END

FORM (HMM_and_HMMStateSequence_to_TableOfReal, U"HMM & HMMStateSequence: To TableOfReal", nullptr)
	BOOLEAN (U"As probabilities", true)
	OK
DO
	HMM me = FIRST (HMM);
	HMMStateSequence hmm_ss = FIRST (HMMStateSequence);
	autoTableOfReal thee = HMM_and_HMMStateSequence_to_TableOfReal_transitions (me, hmm_ss, GET_INTEGER (U"As probabilities"));
	praat_new (thee.move(), Thing_getName (hmm_ss), U"_m");
END

FORM (HMMStateSequence_to_TableOfReal, U"HMMStateSequence: To TableOfReal", nullptr)
	BOOLEAN (U"As probabilities", true)
	OK
DO
	LOOP {
		iam (HMMStateSequence);
		autoTableOfReal thee = Strings_to_TableOfReal_transitions (me, GET_INTEGER (U"As probabilities"));
		praat_new (thee.move(), my name);
	}
END

DIRECT (HMMObservationSequence_to_Strings)
	LOOP {
		iam (HMMObservationSequence);
		autoStrings thee = HMMObservationSequence_to_Strings (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (Strings_to_HMMObservationSequence)
	LOOP {
		iam (Strings);
		autoHMMObservationSequence thee = Strings_to_HMMObservationSequence (me);
		praat_new (thee.move(), my name);
	}
END

DIRECT (HMMStateSequence_to_Strings)
	LOOP {
		iam (HMMStateSequence);
		autoStrings thee = HMMStateSequence_to_Strings (me);
		praat_new (thee.move(), my name);
	}
END

FORM (TableOfReal_to_GaussianMixture_fromRowlabels, U"TableOfReal: To GaussianMixture from row labels", U"TableOfReal: To GaussianMixture (row labels)...")
	OPTIONMENU (U"Covariance matrices are", 1)
		OPTION (U"Complete")
		OPTION (U"Diagonal")
	OK
DO
	long storage = GET_INTEGER (U"Covariance matrices are") - 1;
	LOOP {
		iam (TableOfReal);
		autoGaussianMixture thee = TableOfReal_to_GaussianMixture_fromRowLabels (me, storage);
		praat_new (thee.move(), my name);
	}
END

FORM (TableOfReal_to_GaussianMixture, U"TableOfReal: To GaussianMixture (no labels)", U"TableOfReal: To GaussianMixture...")
	NATURAL (U"Number of components", U"2")
	POSITIVE (U"Tolerance of minimizer", U"0.001")
	INTEGER (U"Maximum number of iterations", U"200")
	REAL (U"Stability coefficient lambda", U"0.001")
	OPTIONMENU (U"Covariance matrices are", 1)
		OPTION (U"Complete")
		OPTION (U"Diagonal")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	double lambda = GET_REAL (U"Stability coefficient lambda");
	REQUIRE (lambda >= 0.0 && lambda < 1.0, U"Lambda must be in interval [0,1).")
	LOOP {
		iam (TableOfReal);
		autoGaussianMixture thee = TableOfReal_to_GaussianMixture (me,
			GET_INTEGER (U"Number of components"),
			GET_REAL (U"Tolerance of minimizer"),
			GET_INTEGER (U"Maximum number of iterations"),
			lambda,
			GET_INTEGER (U"Covariance matrices are") - 1,
			GET_INTEGER (U"Criterion based on") - 1);
		praat_new (thee.move(), my name);
	}
END

FORM (GaussianMixture_and_TableOfReal_improveLikelihood, U"GaussianMixture & TableOfReal: Improve likelihood", U"GaussianMixture & TableOfReal: Improve likelihood...")
	POSITIVE (U"Tolerance of minimizer", U"0.001")
	NATURAL (U"Maximum number of iterations", U"200")
	REAL (U"Stability coefficient lambda", U"0.001")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	double lambda = GET_REAL (U"Stability coefficient lambda");
	REQUIRE (lambda >= 0.0 && lambda < 1.0, U"Lambda must be in interval [0,1).")
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	REQUIRE (thy numberOfColumns == my dimension, U"The number of columns and the dimension of the model do not agree.");
	REQUIRE (my numberOfComponents < thy numberOfRows / 2, U"Not enough data points.")
	GaussianMixture_and_TableOfReal_improveLikelihood (me, thee,
		GET_REAL (U"Tolerance of minimizer"),
		GET_INTEGER (U"Maximum number of iterations"),
		lambda,
		GET_INTEGER (U"Criterion based on") - 1);
END

FORM (GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM, U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)", U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)...")
	INTEGER (U"Minimum number of components", U"1")
	POSITIVE (U"Tolerance of minimizer", U"0.001")
	NATURAL (U"Maximum number of iterations", U"200")
	REAL (U"Stability coefficient lambda", U"0.001")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	double lambda = GET_REAL (U"Stability coefficient lambda");
	REQUIRE (lambda >= 0.0 && lambda < 1.0, U"Lambda must be in interval [0,1).")
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	REQUIRE (thy numberOfColumns == my dimension, U"The number of columns and the dimension of the model do not agree.");
	REQUIRE (my numberOfComponents < thy numberOfRows / 2, U"Not enough data points.")
	autoGaussianMixture him = GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM (me, thee,
		GET_INTEGER (U"Minimum number of components"),
		GET_REAL (U"Tolerance of minimizer"),
		GET_INTEGER (U"Maximum number of iterations"),
		lambda,
		GET_INTEGER (U"Criterion based on") - 1);
	praat_new (him.move(), my name);
END

DIRECT (GaussianMixture_and_TableOfReal_to_ClassificationTable)
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	autoClassificationTable him = GaussianMixture_and_TableOfReal_to_ClassificationTable (me, thee);
	praat_new (him.move(), my name, U"_", thy name);
END

DIRECT (GaussianMixture_and_TableOfReal_to_Correlation)
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	autoCorrelation him = GaussianMixture_and_TableOfReal_to_Correlation (me, thee);
	praat_new (him.move(), my name, U"_", thy name);
END

FORM (GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests, U"GaussianMixture & TableOfReal: To TableOfReal BHEP normality tests", U"GaussianMixture & TableOfReal: To TableOfReal (BHEP normality tests)...")
	REAL (U"Smoothing parameter", U"1.41")
	OK
DO
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	double h = GET_REAL (U"Smoothing parameter");
	autoTableOfReal him = GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests (me, thee, h);
	praat_new (him.move(), my name, U"_", thy name);
END

void praat_HMM_init ();
void praat_HMM_init () {
	Thing_recognizeClassesByName (classHMM, classHMMState, classHMMObservation, classHMMObservationSequence, classHMMStateSequence, classGaussianMixture, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Markov models", nullptr, praat_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_HMM_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_HMM_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create continuous HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_HMM_createContinuousModel);
	praat_addMenuCommand (U"Objects", U"New", U"--drawings--", nullptr, praat_HIDDEN + praat_DEPTH_1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Draw forward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_HMM_drawForwardProbabilitiesIllustration);
	praat_addMenuCommand (U"Objects", U"New", U"Draw backward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_HMM_drawBackwardProbabilitiesIllustration);
	praat_addMenuCommand (U"Objects", U"New", U"Draw forward and backward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_HMM_drawForwardAndBackwardProbabilitiesIllustration);

	praat_addAction1 (classGaussianMixture, 0, U"GaussianMixture help", nullptr, 0, DO_GaussianMixture_help);
	praat_addAction1 (classGaussianMixture, 0, U"Draw concentration ellipses...", nullptr, 0, DO_GaussianMixture_drawConcentrationEllipses);
	praat_addAction1 (classGaussianMixture, 0, U"Draw marginal pdf...", nullptr, 0, DO_GaussianMixture_drawMarginalPdf);
	praat_addAction1 (classGaussianMixture, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classGaussianMixture, 1, U"Get number of components", nullptr, 1, DO_GaussianMixture_getNumberOfComponents);
	praat_addAction1 (classGaussianMixture, 1, U"Get dimension of component", nullptr, 1, DO_GaussianMixture_getDimensionOfComponent);
	praat_addAction1 (classGaussianMixture, 1, U"Get probability at position...", nullptr, 1, DO_GaussianMixture_getProbabilityAtPosition);
	praat_addAction1 (classGaussianMixture, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classGaussianMixture, 1, U"Split component...", nullptr, 1, DO_GaussianMixture_splitComponent);
	praat_addAction1 (classGaussianMixture, 0, U"Extract -", nullptr, 0, nullptr);
	praat_addAction1 (classGaussianMixture, 0, U"Extract mixing probabilities", nullptr, 1, DO_GaussianMixture_extractMixingProbabilities);
	praat_addAction1 (classGaussianMixture, 0, U"Extract component...", nullptr, 1, DO_GaussianMixture_extractComponent);
	praat_addAction1 (classGaussianMixture, 0, U"Extract centroids", nullptr, 1, DO_GaussianMixture_extractCentroids);
	praat_addAction1 (classGaussianMixture, 0, U"To Covariance (between)", nullptr, 0, DO_GaussianMixture_to_Covariance_between);
	praat_addAction1 (classGaussianMixture, 0, U"To Covariance (within)", nullptr, 0, DO_GaussianMixture_to_Covariance_within);
	praat_addAction1 (classGaussianMixture, 0, U"To Covariance (total)", nullptr, 0, DO_GaussianMixture_to_Covariance_total);
	praat_addAction1 (classGaussianMixture, 0, U"To PCA", nullptr, 0, DO_GaussianMixture_to_PCA);
	praat_addAction1 (classGaussianMixture, 0, U"To TableOfReal (random sampling)...", nullptr, 0, DO_GaussianMixture_to_TableOfReal_randomSampling);

	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"Get likelihood value...", nullptr, 0, DO_GaussianMixture_and_TableOfReal_getLikelihoodValue);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"Improve likelihood...", nullptr, 0, DO_GaussianMixture_and_TableOfReal_improveLikelihood);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To GaussianMixture (CEMM)...", nullptr, 0, DO_GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To ClassificationTable", nullptr, 0, DO_GaussianMixture_and_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To Correlation", nullptr, 0, DO_GaussianMixture_and_TableOfReal_to_Correlation);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To TableOfReal (BHEP normality tests)...", nullptr, 0, DO_GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests);

	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"Draw concentration ellipses...", nullptr, 0, DO_GaussianMixture_and_PCA_drawConcentrationEllipses);
	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"Draw marginal pdf...", nullptr, 0, DO_GaussianMixture_and_PCA_drawMarginalPdf);
	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"To Matrix (density)...", nullptr, 0, DO_GaussianMixture_and_PCA_to_Matrix_density);

	praat_addAction1 (classHMM, 0, U"HMM help ", nullptr, 0, DO_HMM_help);
	praat_addAction1 (classHMM, 0, U"Draw...", nullptr, 0, DO_HMM_draw);
	praat_addAction1 (classHMM, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 1, U"Get transition probability...", nullptr, 1, DO_HMM_getTransitionProbability);
	praat_addAction1 (classHMM, 1, U"Get emission probability...", nullptr, 1, DO_HMM_getEmissionProbability);
	praat_addAction1 (classHMM, 1, U"Get start probability...", nullptr, 1, DO_HMM_getStartProbability);
	praat_addAction1 (classHMM, 1, U"Get p (time, state)...", nullptr, 1, DO_HMM_getProbabilityAtTimeBeingInState);
	praat_addAction1 (classHMM, 1, U"Get p (time, state, symbol)...", nullptr, 1, DO_HMM_getProbabilityAtTimeBeingInStateEmittingSymbol);
	praat_addAction1 (classHMM, 1, U"Get probability staying in state...", nullptr, 1, DO_HMM_getProbabilityOfStayingInState);
	praat_addAction1 (classHMM, 1, U"Get expected duration in state...", nullptr, 1, DO_HMM_getExpectedValueOfDurationInState);
	praat_addAction1 (classHMM, 1, U"---- states / symbols -----", nullptr, 1, nullptr);
	praat_addAction1 (classHMM, 1, U"Get state label...", nullptr, 1, DO_HMM_getStateLabel);
	praat_addAction1 (classHMM, 1, U"Get symbol label...", nullptr, 1, DO_HMM_getSymbolLabel);
	praat_addAction1 (classHMM, 0, U"--- multiple HMMs ----", nullptr, 1, nullptr);
	praat_addAction1 (classHMM, 2, U"Get cross-entropy...", nullptr, 1, DO_HMM_and_HMM_getCrossEntropy);

	praat_addAction1 (classHMM, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 1, U"Set transition probabilities...", nullptr, 1, DO_HMM_setTransitionProbabilities);
	praat_addAction1 (classHMM, 1, U"Set emission probabilities...", nullptr, 1, DO_HMM_setEmissionProbabilities);
	praat_addAction1 (classHMM, 1, U"Set start probabilities...", nullptr, 1, DO_HMM_setStartProbabilities);

	praat_addAction1 (classHMM, 0, U"Extract -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 0, U"Extract transition probabilities", nullptr, 1, DO_HMM_extractTransitionProbabilities);
	praat_addAction1 (classHMM, 0, U"Extract emission probabilities", nullptr, 1, DO_HMM_extractEmissionProbabilities);

	praat_addAction1 (classHMM, 0, U"To HMMObservationSequence...", nullptr, 0, DO_HMM_to_HMMObservationSequence);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"Draw trellis...", nullptr, 0, DO_HMM_and_HMMStateSequence_drawTrellis);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"Get probability", nullptr, 0, DO_HMM_and_HMMStateSequence_getProbability);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"To TableOfReal (bigrams)...", nullptr, 0, DO_HMM_and_HMMStateSequence_to_TableOfReal);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get probability", nullptr, 0, DO_HMM_and_HMMObservationSequence_getProbability);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get cross-entropy", nullptr, 0, DO_HMM_and_HMMObservationSequence_getCrossEntropy);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get perplexity", nullptr, 0, DO_HMM_and_HMMObservationSequence_getPerplexity);


	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"To HMMStateSequence", nullptr, 0, DO_HMM_and_HMMObservationSequence_to_HMMStateSequence);
	praat_addAction2 (classHMM, 2, classHMMObservationSequence, 1, U"Get cross-entropy", nullptr, 0, DO_HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"To TableOfReal (bigrams)...", nullptr, 0, DO_HMM_and_HMMObservationSequence_to_TableOfReal);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 0, U"Learn...", nullptr, 0, DO_HMM_and_HMMObservationSequence_learn);

	praat_addAction1 (classHMMObservationSequence, 0, U"To TableOfReal (bigrams)...", nullptr, 0, DO_HMMObservationSequence_to_TableOfReal);
	praat_addAction1 (classHMMObservationSequence, 0, U"To Strings", nullptr, 0, DO_HMMObservationSequence_to_Strings);
	praat_addAction1 (classHMMStateSequence, 0, U"To TableOfReal (bigrams)...", nullptr, 0, DO_HMMStateSequence_to_TableOfReal);
	praat_addAction1 (classHMMStateSequence, 0, U"To Strings", nullptr, 0, DO_HMMStateSequence_to_Strings);

	praat_addAction1 (classHMMObservationSequence, 0, U"To HMM...", nullptr, 1, DO_HMMObservationSequence_to_HMM);

	praat_addAction1 (classStrings, 0, U"To HMMObservationSequence", nullptr, praat_HIDDEN, DO_Strings_to_HMMObservationSequence);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture (row labels)...", U"To Covariance", praat_HIDDEN + praat_DEPTH_1, DO_TableOfReal_to_GaussianMixture_fromRowlabels);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture...", U"To Covariance", praat_HIDDEN + praat_DEPTH_1, DO_TableOfReal_to_GaussianMixture);

	INCLUDE_MANPAGES (manual_HMM)
}

/* End of file praat_HMM_init.cpp */
