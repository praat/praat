/* praat_HMM_init.cpp
 *
 * Copyright (C) 2010-2011,2015-2016 David Weenink
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

#define GaussianMixture_OPTION_MENU_CRITERIA \
	OPTIONMENUVAR (criterion, U"Criterion based on", 1) \
		OPTION (U"Likelihood") \
		OPTION (U"Message length") \
		OPTION (U"Bayes information") \
		OPTION (U"Akaike information") \
		OPTION (U"Akaike corrected") \
		OPTION (U"Complete-data ML")

DIRECT (HELP_GaussianMixture_help) {
	Melder_help (U"GaussianMixture");
END }

FORM (GRAPHICS_GaussianMixture_drawConcentrationEllipses, U"GaussianMixture: Draw concentration ellipses", U"GaussianMixture: Draw concentration ellipses...") {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEANVAR (principalComponentPlane, U"Principal component plane", true)
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
	autoPraatPicture picture;
	LOOP {
		iam (GaussianMixture);
		GaussianMixture_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, 0, 0, principalComponentPlane, xDimension, yDimension,
			xmin, xmax,
			ymin, ymax,
			labelSize, garnish);
	}
END }

FORM (GRAPHICS_GaussianMixture_and_PCA_drawConcentrationEllipses, U"GaussianMixture & PCA: Draw concentration ellipses", U"GaussianMixture & PCA: Draw concentration ellipses...") {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
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
	GaussianMixture me = FIRST (GaussianMixture);
	PCA pca = FIRST (PCA);
	GaussianMixture_and_PCA_drawConcentrationEllipses (me, pca, GRAPHICS, numberOfSigmas, 0, nullptr, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
END }

FORM (GRAPHICS_GaussianMixture_drawMarginalPdf, U"GaussianMixture: Draw marginal pdf", U"GaussianMixture: Draw marginal pdf...") {
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	NATURALVAR (numberOfPoints, U"Number of points", U"500")
	INTEGERVAR (numberOfBins, U"Number of bins", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (GaussianMixture);
		GaussianMixture_drawMarginalPdf (me, GRAPHICS, xDimension,
			xmin, xmax,
			ymin, ymax,
			numberOfPoints, numberOfBins, garnish);
	}
END }

DIRECT (INTEGER_GaussianMixture_getNumberOfComponents) {
	LOOP {
		iam (GaussianMixture);
		Melder_information (my numberOfComponents, U" (= number of components)");
	}
END }

DIRECT (INTEGER_GaussianMixture_getDimensionOfComponent) {
	LOOP {
		iam (GaussianMixture);
		Melder_information (my dimension, U" (= dimension of component)");
	}
END }

FORM (REAL_GaussianMixture_getProbabilityAtPosition, U"GaussianMixture: Get probability at position", nullptr) {
	SENTENCEVAR (position_string, U"Position", U"100.0 300.0")
	OK
DO
	LOOP {
		iam (GaussianMixture);
		double p = GaussianMixture_getProbabilityAtPosition_string (me, position_string);
		Melder_information (p, U" (= probability at position ", position_string);
	}
END }

FORM (MODIFY_GaussianMixture_splitComponent, U"GaussianMixture: Split component", U"GaussianMixture: Split component...") {
	NATURALVAR (component, U"Component", U"1")
	OK
DO
	LOOP {
		iam (GaussianMixture);
		GaussianMixture_splitComponent (me, component);
	}
END }

FORM (GRAPHICS_GaussianMixture_and_PCA_drawMarginalPdf, U"GaussianMixture & PCA: Draw pdf function", U"GaussianMixture: Draw marginal pdf...") {
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	NATURALVAR (numberOfPoints, U"Number of points", U"500")
	INTEGERVAR (numberOfBins, U"Number of bins", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	GaussianMixture me = FIRST (GaussianMixture);
	PCA thee = FIRST (PCA);
	GaussianMixture_and_PCA_drawMarginalPdf (me, thee, GRAPHICS, xDimension, xmin, xmax, ymin, ymax, numberOfPoints, numberOfBins, garnish);
END }

FORM (NEW1_GaussianMixture_and_PCA_to_Matrix_density, U"GaussianMixture & PCA: To Matrix density", U"GaussianMixture & PCA: To Matrix (density)...") {
	INTEGERVAR (xDimension, U"X-dimension", U"1")
	INTEGERVAR (yDimension, U"Y-dimension", U"2")
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"0.0")
	NATURALVAR (numberOfColumns, U"Number of columns", U"100")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"0.0")
	NATURALVAR (numberOfRows, U"Number of rows", U"100")
	OK
DO
	GaussianMixture me = FIRST (GaussianMixture);
	PCA thee = FIRST (PCA);
	autoMatrix result = GaussianMixture_and_PCA_to_Matrix_density (me, thee, xDimension, yDimension, xmin, xmax, numberOfColumns, ymin, ymax, numberOfRows);
	praat_new (result.move(), my name, U"_", thy name);
END }

FORM (NEW_GaussianMixture_extractComponent, U"GaussianMixture: Extract component", nullptr) {
	NATURALVAR (component, U"Component", U"1")
	OK
DO
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_extractComponent (me, component);
		praat_new (result.move(), my name, U"_", result -> name);
	}
END }

DIRECT (NEW_GaussianMixture_extractCentroids) {
	LOOP {
		iam (GaussianMixture);
		autoTableOfReal result = GaussianMixture_extractCentroids (me);
		praat_new (result.move(), my name);
	}
END }

DIRECT (NEW_GaussianMixture_extractMixingProbabilities) {
	LOOP {
		iam (GaussianMixture);
		autoTableOfReal result = GaussianMixture_extractMixingProbabilities (me);
		praat_new (result.move(), my name);
	}
END }

DIRECT (NEW_GaussianMixture_to_PCA) {
	LOOP {
		iam (GaussianMixture);
		autoPCA result = GaussianMixture_to_PCA (me);
		praat_new (result.move(), my name);
	}
END }

FORM (NEW_GaussianMixture_to_TableOfReal_randomSampling, U"GaussianMixture: To TableOfReal (random sampling)", U"GaussianMixture: To TableOfReal (random sampling)...") {
	NATURALVAR (numberOfPoints, U"Number of data points", U"100")
	OK
DO
	LOOP {
		iam (GaussianMixture);
		autoTableOfReal result = GaussianMixture_to_TableOfReal_randomSampling (me, numberOfPoints);
		praat_new (result.move(), my name);
	}
END }

DIRECT (NEW_GaussianMixture_to_Covariance_between) {
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_to_Covariance_between (me);
		praat_new (result.move(), my name, U"_b");
	}
END }

DIRECT (NEW_GaussianMixture_to_Covariance_within) {
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_to_Covariance_within (me);
		praat_new (result.move(), my name, U"_w");
	}
END }

DIRECT (NEW_GaussianMixture_to_Covariance_total) {
	LOOP {
		iam (GaussianMixture);
		autoCovariance result = GaussianMixture_to_Covariance_total (me);
		praat_new (result.move(), my name, U"_t");
	}
END }

FORM (REAL_GaussianMixture_and_TableOfReal_getLikelihoodValue, U"GaussianMixture & TableOfReal: Get likelihood value",
      U"GaussianMixture & TableOfReal: Get likelihood value...") {
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	const char32 *criterionText = GaussianMixture_criterionText (criterion);
	double lnpdn = GaussianMixture_and_TableOfReal_getLikelihoodValue (me, thee, criterion - 1);
	Melder_information (lnpdn / thy numberOfRows, U" (= ", criterionText, U", n = ",
		thy numberOfRows, U")");
END }

DIRECT (HELP_HMM_help) {
	Melder_help (U"HMM");
END }

FORM (NEW_HMM_create, U"Create HMM", nullptr) {
	WORDVAR (name, U"Name", U"hmm")
	BOOLEANVAR (leftToRightModel, U"Left to right model", false)
	NATURALVAR (numberOfStates, U"Number of states", U"3")
	NATURALVAR (numberOfObservations, U"Number of observations", U"3")
	OK
DO
	autoHMM thee = HMM_create (leftToRightModel, numberOfStates, numberOfObservations);
	praat_new (thee.move(), name);
END }

FORM (NEW_HMM_createSimple, U"HMM: Create simple", U"HMM: Create simple HMM...") {
	WORDVAR (name, U"Name", U"weather")
	BOOLEANVAR (leftToRightModel, U"Left to right model", false)
	SENTENCEVAR (states_string, U"States", U"Rainy Sunny")
	SENTENCEVAR (observations_string, U"Observations", U"Walk Shop Clean")
	OK
DO
autoHMM thee = HMM_createSimple (leftToRightModel, states_string, observations_string);
	praat_new (thee.move(), name);
END }

FORM (NEW_HMM_createContinuousModel, U"HMM: Create continuous model", nullptr) {
	WORDVAR (name, U"Name", U"cm")
	BOOLEANVAR (leftToRightModel, U"Left to right model", false)
	NATURALVAR (numberOfStates, U"Number of states", U"3")
	NATURALVAR (numberOfSymbols, U"Number of symbols", U"10")
	LABEL (U"", U"For the Gaussian mixtures:")
	NATURALVAR (numberOfComponents, U"Number of components", U"3")
	NATURALVAR (componentDimension, U"Dimension of component", U"39")
	OPTIONMENUVAR (matricesType, U"Covariance matrices are", 1)
		OPTION (U"Complete")
		OPTION (U"Diagonal")
	OK
DO
	REQUIRE (matricesType >= 0 && matricesType <= componentDimension, U"Not a valid covariance matrix type")
	autoHMM thee = HMM_createContinuousModel (leftToRightModel, numberOfStates, numberOfSymbols, numberOfComponents, componentDimension, matricesType - 1);
	praat_new (thee.move(), name);
END }

FORM (NEW_HMMObservationSequence_to_HMM, U"HMMObservationSequence: To HMM", nullptr) {
	LABEL (U"", U"(0 states gives a non-hidden model) ")
	INTEGERVAR (numberOfHiddenStates, U"Number of hidden states", U"2")
	BOOLEANVAR (leftToRightModel, U"Left to right model", false)
	OK
DO
	LOOP {
		iam (HMMObservationSequence);
		autoHMM result = HMM_createFromHMMObservationSequence (me, numberOfHiddenStates, leftToRightModel);
		praat_new (result.move(), my name, U"_", numberOfHiddenStates);
	}
END }

FORM (GRAPHICS_HMM_draw, U"HMM: Draw", nullptr) {
	BOOLEANVAR (garnish, U"Garnish", false)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (HMM);
		HMM_draw (me, GRAPHICS,
			garnish);
	}
END }

FORM (GRAPHICS_HMM_and_HMMStateSequence_drawTrellis, U"HMM & Strings: Draw trellis", nullptr) {
	BOOLEANVAR (connect, U"Connect", true);
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	autoPraatPicture picture;
	HMM me = FIRST (HMM);
	HMMStateSequence hmmStateSequence = FIRST (HMMStateSequence);
	HMM_and_HMMStateSequence_drawTrellis (me, hmmStateSequence, GRAPHICS, connect, garnish);
END }

DIRECT (GRAPHICS_HMM_drawForwardProbabilitiesIllustration) {
	autoPraatPicture picture;
	HMM_drawForwardProbabilitiesIllustration (GRAPHICS, true);
END }

DIRECT (GRAPHICS_HMM_drawBackwardProbabilitiesIllustration) {
	autoPraatPicture picture;
	HMM_drawBackwardProbabilitiesIllustration (GRAPHICS, true);
END }

DIRECT (GRAPHICS_HMM_drawForwardAndBackwardProbabilitiesIllustration) {
	autoPraatPicture picture;
	HMM_drawForwardAndBackwardProbabilitiesIllustration (GRAPHICS, true);
END }

FORM (REAL_HMM_getTransitionProbability, U"HMM: Get transition probability", U"HMM: Get transition probability...") {
	NATURALVAR (fromState, U"From state number", U"1")
	NATURALVAR (toState, U"To state number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		REQUIRE (fromState <= my numberOfStates && toState <= my numberOfStates, U"State number(s) too high.")
		Melder_information (my transitionProbs[fromState][toState], U" : [ ", fromState, U", ", toState, U" ]");
	}
END }

FORM (REAL_HMM_getEmissionProbability, U"HMM: Get emission probability", U"HMM: Get emission probability...") {
	NATURALVAR (fromState, U"From state number", U"1")
	NATURALVAR (toState, U"To state number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		REQUIRE (fromState <= my numberOfStates, U"State number too high.")
		REQUIRE (toState <= my numberOfObservationSymbols, U"Symbol number too high.")
		Melder_information (my emissionProbs[fromState][toState], U" : [ ", fromState, U", ", toState, U" ]");
	}
END }

FORM (REAL_HMM_getStartProbability, U"HMM: Get start probability", U"HMM: Get start probability...") {
	NATURALVAR (stateNumber, U"State number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		REQUIRE (stateNumber <= my numberOfStates, U"State number too high.")
		Melder_information (my transitionProbs[0][stateNumber], U" : [ ", stateNumber, U" ]");
	}
END }

FORM (REAL_HMM_getProbabilityAtTimeBeingInState, U"HMM: Get probability of being in state at time",
      U"HMM: Get p (time, state)...") {
	NATURALVAR (timeIndex, U"Time index", U"10")
	NATURALVAR (stateNumber, U"State number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		double lnp = HMM_getProbabilityAtTimeBeingInState (me, timeIndex, stateNumber);
		Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U") Being in state ", stateNumber, U" at time ", timeIndex);
	}
END }

FORM (REAL_HMM_getProbabilityAtTimeBeingInStateEmittingSymbol, U"HMM: get probability being at time in state emitting symbol", U"HMM: Get p (time, state, symbol)...") {
	NATURALVAR (timeIndex, U"Time index", U"10")
	NATURALVAR (stateNumber, U"State number", U"1")
	NATURALVAR (symbolNumber, U"Symbol number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		double lnp = HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (me, timeIndex, stateNumber, symbolNumber);
		Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U") Being in state ", stateNumber, U" emitting symbol ", symbolNumber, U" at time ", timeIndex);
	}
END }

FORM (REAL_HMM_getProbabilityOfStayingInState, U"HMM: Get probability of staying in state", U"HMM: Get probability staying in state...") {
	NATURALVAR (stateNumber, U"State number", U"1")
	NATURALVAR (numberOfTimeUnits, U"Number of time units", U"2")
	OK
DO
	LOOP {
		iam (HMM);
		Melder_informationReal (HMM_getProbabilityOfStayingInState (me, stateNumber, numberOfTimeUnits), nullptr);
	}
END }

FORM (REAL_HMM_getExpectedValueOfDurationInState, U"HMM: Get expected value of duration in state",
      U"HMM: Get expected duration in state...") {
	NATURALVAR (stateNumber, U"State number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		Melder_informationReal (HMM_getExpectedValueOfDurationInState (me, stateNumber), U" time units");
	}
END }

FORM (INFO_HMM_getSymbolLabel, U"HMM: Get symbol label", nullptr) {
	NATURALVAR (symbolNumber, U"Symbol number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		REQUIRE (symbolNumber <= my numberOfObservationSymbols, U"Symbol number too high.")
		HMMObservation s = my observationSymbols->at [symbolNumber];
		Melder_information (s -> label);
	}
END }

FORM (INFO_HMM_getStateLabel, U"HMM: Get state label", nullptr) {
	NATURALVAR (stateNumber, U"State number", U"1")
	OK
DO
	LOOP {
		iam (HMM);
		REQUIRE (stateNumber <= my numberOfStates, U"State number too high.")
		HMMState state = my states->at [stateNumber];
		Melder_information (state -> label);
	}
END }

FORM (REAL_HMM_and_HMM_getCrossEntropy, U"HMM & HMM: Get cross-entropy...", U"HMM & HMM: Get cross-entropy...") {
	NATURALVAR (observationLength, U"Observation length", U"2000")
	BOOLEANVAR (symmetric, U"Symmetric", true)
	OK
DO
	HMM m1 = nullptr, m2 = nullptr;
	LOOP {
		iam (HMM);
		(m1 ? m2 : m1) = me;
	}
	Melder_assert (m1 && m2);
	double ce = HMM_and_HMM_getCrossEntropy (m1, m2, observationLength, symmetric);
	Melder_information (ce, U" (= ", (symmetric ? U"symmetric " : U""), U" cross-entropy between models for observation length = ", observationLength, U")");
END }

DIRECT (REAL_HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy) {
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
END }

FORM (NEW_HMM_to_HMMObservationSequence, U"HMM: To HMMObservationSequence (generate observations)", U"HMM: To HMMObservationSequence...") {
	INTEGERVAR (startState, U"Start state", U"0")
	NATURALVAR (numberOfObservations, U"Number of observations", U"20")
	OK
DO
	LOOP {
		iam (HMM);
		autoHMMObservationSequence thee = HMM_to_HMMObservationSequence (me, startState, numberOfObservations);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (REAL_HMM_and_HMMStateSequence_getProbability) {
	HMM me = FIRST (HMM);
	HMMStateSequence hmm_ss = FIRST (HMMStateSequence);
	double lnp = HMM_and_HMMStateSequence_getProbability (me, hmm_ss);
	Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U")");
END }

DIRECT (REAL_HMM_and_HMMObservationSequence_getProbability) {
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	double lnp = HMM_and_HMMObservationSequence_getProbability (me, hmm_os);
	Melder_information (lnp, U" (= ln(p), p = ", Melder_naturalLogarithm (lnp), U")");
END }

DIRECT (REAL_HMM_and_HMMObservationSequence_getCrossEntropy) {
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	double ce = HMM_and_HMMObservationSequence_getCrossEntropy (me, hmm_os);
	Melder_information (ce, U" (= cross-entropy)");
END }

DIRECT (REAL_HMM_and_HMMObservationSequence_getPerplexity) {
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	double py = HMM_and_HMMObservationSequence_getPerplexity (me, hmm_os);
	Melder_information (py, U" (= perplexity)");
END }

DIRECT (NEW1_HMM_and_HMMObservationSequence_to_HMMStateSequence) {
	HMM me = FIRST (HMM);
	HMMObservationSequence thee = FIRST (HMMObservationSequence);
	autoHMMStateSequence result = HMM_and_HMMObservationSequence_to_HMMStateSequence (me, thee);
	praat_new (result.move(), my name, U"_", thy name, U"_states");
END }

FORM (MODIFY_HMM_and_HMMObservationSequence_learn, U"HMM & HMMObservationSequence: Learn", U"HMM & HMMObservationSequences: Learn...") {
	POSITIVEVAR (relativePrecision_log, U"Relative precision in log(p)", U"0.001")
	REALVAR (minimumProbability, U"Minimum probability", U"0.00000000001")
	BOOLEANVAR (showProgress, U"Learning history in Info window", false)
	OK
DO
	REQUIRE (minimumProbability >= 0 && minimumProbability < 1, U"A probabilty must be >= 0 and < 1!")
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
	HMM_and_HMMObservationSequenceBag_learn (hmm, hmmObservationSequences.get(), relativePrecision_log, minimumProbability, showProgress);
END }

FORM (MODIFY_HMM_setTransitionProbabilities, U"HMM: Set transition probabilities", U"HMM: Set transition probabilities...") {
	NATURALVAR (stateNumber, U"State number", U"1")
	SENTENCEVAR (probabilities_string, U"Probabilities", U"0.1 0.9")
	OK
DO
	LOOP {
		iam (HMM);
		HMM_setTransitionProbabilities (me, stateNumber, probabilities_string);
	}
END }

FORM (MODIFY_HMM_setEmissionProbabilities, U"HMM: Set emission probabilities", U"HMM: Set emission probabilities...") {
	NATURALVAR (stateNumber, U"State number", U"1")
	SENTENCEVAR (probabilities_string, U"Probabilities", U"0.1 0.7 0.2")
	OK
DO
	LOOP {
		iam (HMM);
		HMM_setEmissionProbabilities (me, stateNumber, probabilities_string);
	}
END }

FORM (MODIFY_HMM_setStartProbabilities, U"HMM: Set start probabilities", U"HMM: Set start probabilities...") {
	SENTENCEVAR (probabilities_string, U"Probabilities", U"0.1 0.9")
	OK
DO
	LOOP {
		iam (HMM);
		HMM_setStartProbabilities (me, probabilities_string);
	}
END }

DIRECT (NEW_HMM_extractTransitionProbabilities) {
	LOOP {
		iam (HMM);
		autoTableOfReal result = HMM_extractTransitionProbabilities (me);
		praat_new (result.move(), my name, U"_t");
	}
END }

DIRECT (NEW_HMM_extractEmissionProbabilities) {
	LOOP {
		iam (HMM);
		autoTableOfReal result = HMM_extractEmissionProbabilities (me);
		praat_new (result.move(), my name, U"_e");
	}
END }

FORM (NEW_HMMObservationSequence_to_TableOfReal, U"HMMObservationSequence: To TableOfReal ", U"HMMObservationSequence: To TableOfReal (bigrams)...") {
	BOOLEANVAR (asProbabilities, U"As probabilities", true)
	OK
DO
	LOOP {
		iam (HMMObservationSequence);
		autoTableOfReal result = HMMObservationSequence_to_TableOfReal_transitions (me, asProbabilities);
		praat_new (result.move(), my name);
	}
END }

FORM (NEW1_HMM_and_HMMObservationSequence_to_TableOfReal, U"HMM & HMMObservationSequence: To TableOfReal", U"HMM & HMMObservationSequence: To TableOfReal (bigrams)...") {
	BOOLEANVAR (asProbabilities, U"As probabilities", true)
	OK
DO
	HMM me = FIRST (HMM);
	HMMObservationSequence hmm_os = FIRST (HMMObservationSequence);
	autoTableOfReal thee = HMM_and_HMMObservationSequence_to_TableOfReal_transitions (me, hmm_os, asProbabilities);
	praat_new (thee.move(), hmm_os -> name, U"_m");
END }

FORM (NEW1_HMM_and_HMMStateSequence_to_TableOfReal, U"HMM & HMMStateSequence: To TableOfReal", nullptr) {
	BOOLEANVAR (asProbabilities, U"As probabilities", true)
	OK
DO
	HMM me = FIRST (HMM);
	HMMStateSequence hmm_ss = FIRST (HMMStateSequence);
	autoTableOfReal thee = HMM_and_HMMStateSequence_to_TableOfReal_transitions (me, hmm_ss, asProbabilities);
	praat_new (thee.move(), Thing_getName (hmm_ss), U"_m");
END }

FORM (NEW_HMMStateSequence_to_TableOfReal, U"HMMStateSequence: To TableOfReal", nullptr) {
	BOOLEANVAR (asProbabilities, U"As probabilities", true)
	OK
DO
	LOOP {
		iam (HMMStateSequence);
		autoTableOfReal thee = Strings_to_TableOfReal_transitions (me, asProbabilities);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_HMMObservationSequence_to_Strings) {
	LOOP {
		iam (HMMObservationSequence);
		autoStrings thee = HMMObservationSequence_to_Strings (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Strings_to_HMMObservationSequence) {
	LOOP {
		iam (Strings);
		autoHMMObservationSequence thee = Strings_to_HMMObservationSequence (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_HMMStateSequence_to_Strings) {
	LOOP {
		iam (HMMStateSequence);
		autoStrings thee = HMMStateSequence_to_Strings (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_TableOfReal_to_GaussianMixture_fromRowlabels, U"TableOfReal: To GaussianMixture from row labels", U"TableOfReal: To GaussianMixture (row labels)...") {
	OPTIONMENUVAR (matricesType, U"Covariance matrices are", 1)
		OPTION (U"Complete")
		OPTION (U"Diagonal")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoGaussianMixture thee = TableOfReal_to_GaussianMixture_fromRowLabels (me, matricesType - 1);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_TableOfReal_to_GaussianMixture, U"TableOfReal: To GaussianMixture (no labels)", U"TableOfReal: To GaussianMixture...") {
	NATURALVAR (numberOfComponents, U"Number of components", U"2")
	POSITIVEVAR (tolerance, U"Tolerance of minimizer", U"0.001")
	INTEGERVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	REALVAR (lambda, U"Stability coefficient lambda", U"0.001")
	OPTIONMENUVAR (matricesType, U"Covariance matrices are", 1)
		OPTION (U"Complete")
		OPTION (U"Diagonal")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	REQUIRE (lambda >= 0.0 && lambda < 1.0, U"Lambda must be in interval [0,1).")
	LOOP {
		iam (TableOfReal);
		autoGaussianMixture thee = TableOfReal_to_GaussianMixture (me, numberOfComponents, tolerance, maximumNumberOfIterations, lambda, matricesType - 1, criterion - 1);
		praat_new (thee.move(), my name);
	}
END }

FORM (MODIFY_GaussianMixture_and_TableOfReal_improveLikelihood, U"GaussianMixture & TableOfReal: Improve likelihood", U"GaussianMixture & TableOfReal: Improve likelihood...") {
	POSITIVEVAR (tolerance, U"Tolerance of minimizer", U"0.001")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	REALVAR (lambda, U"Stability coefficient lambda", U"0.001")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	REQUIRE (lambda >= 0.0 && lambda < 1.0, U"Lambda must be in interval [0,1).")
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	REQUIRE (thy numberOfColumns == my dimension, U"The number of columns and the dimension of the model do not agree.");
	REQUIRE (my numberOfComponents < thy numberOfRows / 2, U"Not enough data points.")
	GaussianMixture_and_TableOfReal_improveLikelihood (me, thee, tolerance, maximumNumberOfIterations, lambda, criterion - 1);
END }

FORM (NEW1_GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM, U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)", U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)...") {
	INTEGERVAR (minimumNumberOfComponents, U"Minimum number of components", U"1")
	POSITIVEVAR (tolerance, U"Tolerance of minimizer", U"0.001")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	REALVAR (lambda, U"Stability coefficient lambda", U"0.001")
	GaussianMixture_OPTION_MENU_CRITERIA
	OK
DO
	REQUIRE (lambda >= 0.0 && lambda < 1.0, U"Lambda must be in interval [0,1).")
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	REQUIRE (thy numberOfColumns == my dimension, U"The number of columns and the dimension of the model do not agree.");
	REQUIRE (my numberOfComponents < thy numberOfRows / 2, U"Not enough data points.")
	autoGaussianMixture him = GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM (me, thee, minimumNumberOfComponents, tolerance, maximumNumberOfIterations, lambda, criterion - 1);
	praat_new (him.move(), my name);
END }

DIRECT (NEW1_GaussianMixture_and_TableOfReal_to_ClassificationTable) {
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	autoClassificationTable him = GaussianMixture_and_TableOfReal_to_ClassificationTable (me, thee);
	praat_new (him.move(), my name, U"_", thy name);
END }

DIRECT (NEW1_GaussianMixture_and_TableOfReal_to_Correlation) {
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	autoCorrelation him = GaussianMixture_and_TableOfReal_to_Correlation (me, thee);
	praat_new (him.move(), my name, U"_", thy name);
END }

FORM (NEW1_GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests, U"GaussianMixture & TableOfReal: To TableOfReal BHEP normality tests", U"GaussianMixture & TableOfReal: To TableOfReal (BHEP normality tests)...") {
	REALVAR (smoothingParameter, U"Smoothing parameter", U"1.41")
	OK
DO
	GaussianMixture me = FIRST (GaussianMixture);
	TableOfReal thee = FIRST (TableOfReal);
	autoTableOfReal him = GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests (me, thee, smoothingParameter);
	praat_new (him.move(), my name, U"_", thy name);
END }

void praat_HMM_init ();
void praat_HMM_init () {
	Thing_recognizeClassesByName (classHMM, classHMMState, classHMMObservation, classHMMObservationSequence, classHMMStateSequence, classGaussianMixture, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Markov models", nullptr, praat_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW_HMM_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW_HMM_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create continuous HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW_HMM_createContinuousModel);
	praat_addMenuCommand (U"Objects", U"New", U"--drawings--", nullptr, praat_HIDDEN + praat_DEPTH_1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Draw forward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, GRAPHICS_HMM_drawForwardProbabilitiesIllustration);
	praat_addMenuCommand (U"Objects", U"New", U"Draw backward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, GRAPHICS_HMM_drawBackwardProbabilitiesIllustration);
	praat_addMenuCommand (U"Objects", U"New", U"Draw forward and backward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, GRAPHICS_HMM_drawForwardAndBackwardProbabilitiesIllustration);

	praat_addAction1 (classGaussianMixture, 0, U"GaussianMixture help", nullptr, 0, HELP_GaussianMixture_help);
	praat_addAction1 (classGaussianMixture, 0, U"Draw concentration ellipses...", nullptr, 0, GRAPHICS_GaussianMixture_drawConcentrationEllipses);
	praat_addAction1 (classGaussianMixture, 0, U"Draw marginal pdf...", nullptr, 0, GRAPHICS_GaussianMixture_and_PCA_drawConcentrationEllipses);
	praat_addAction1 (classGaussianMixture, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classGaussianMixture, 1, U"Get number of components", nullptr, 1, INTEGER_GaussianMixture_getNumberOfComponents);
	praat_addAction1 (classGaussianMixture, 1, U"Get dimension of component", nullptr, 1, INTEGER_GaussianMixture_getDimensionOfComponent);
	praat_addAction1 (classGaussianMixture, 1, U"Get probability at position...", nullptr, 1, REAL_GaussianMixture_getProbabilityAtPosition);
	praat_addAction1 (classGaussianMixture, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classGaussianMixture, 1, U"Split component...", nullptr, 1, MODIFY_GaussianMixture_splitComponent);
	praat_addAction1 (classGaussianMixture, 0, U"Extract -", nullptr, 0, nullptr);
	praat_addAction1 (classGaussianMixture, 0, U"Extract mixing probabilities", nullptr, 1, NEW_GaussianMixture_extractMixingProbabilities);
	praat_addAction1 (classGaussianMixture, 0, U"Extract component...", nullptr, 1, NEW_GaussianMixture_extractComponent);
	praat_addAction1 (classGaussianMixture, 0, U"Extract centroids", nullptr, 1, NEW_GaussianMixture_extractCentroids);
	praat_addAction1 (classGaussianMixture, 0, U"To Covariance (between)", nullptr, 0, NEW_GaussianMixture_to_Covariance_between);
	praat_addAction1 (classGaussianMixture, 0, U"To Covariance (within)", nullptr, 0, NEW_GaussianMixture_to_Covariance_within);
	praat_addAction1 (classGaussianMixture, 0, U"To Covariance (total)", nullptr, 0, NEW_GaussianMixture_to_Covariance_total);
	praat_addAction1 (classGaussianMixture, 0, U"To PCA", nullptr, 0, NEW_GaussianMixture_to_PCA);
	praat_addAction1 (classGaussianMixture, 0, U"To TableOfReal (random sampling)...", nullptr, 0, NEW_GaussianMixture_to_TableOfReal_randomSampling);

	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"Get likelihood value...", nullptr, 0, REAL_GaussianMixture_and_TableOfReal_getLikelihoodValue);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"Improve likelihood...", nullptr, 0, MODIFY_GaussianMixture_and_TableOfReal_improveLikelihood);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To GaussianMixture (CEMM)...", nullptr, 0, NEW1_GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To ClassificationTable", nullptr, 0, NEW1_GaussianMixture_and_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To Correlation", nullptr, 0, NEW1_GaussianMixture_and_TableOfReal_to_Correlation);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To TableOfReal (BHEP normality tests)...", nullptr, 0, NEW1_GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests);

	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"Draw concentration ellipses...", nullptr, 0, GRAPHICS_GaussianMixture_and_PCA_drawConcentrationEllipses);
	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"Draw marginal pdf...", nullptr, 0, GRAPHICS_GaussianMixture_and_PCA_drawMarginalPdf);
	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"To Matrix (density)...", nullptr, 0, NEW1_GaussianMixture_and_PCA_to_Matrix_density);

	praat_addAction1 (classHMM, 0, U"HMM help ", nullptr, 0, HELP_HMM_help);
	praat_addAction1 (classHMM, 0, U"Draw...", nullptr, 0, GRAPHICS_HMM_draw);
	praat_addAction1 (classHMM, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 1, U"Get transition probability...", nullptr, 1, REAL_HMM_getTransitionProbability);
	praat_addAction1 (classHMM, 1, U"Get emission probability...", nullptr, 1, REAL_HMM_getEmissionProbability);
	praat_addAction1 (classHMM, 1, U"Get start probability...", nullptr, 1, REAL_HMM_getStartProbability);
	praat_addAction1 (classHMM, 1, U"Get p (time, state)...", nullptr, 1, REAL_HMM_getProbabilityAtTimeBeingInState);
	praat_addAction1 (classHMM, 1, U"Get p (time, state, symbol)...", nullptr, 1, REAL_HMM_getProbabilityAtTimeBeingInStateEmittingSymbol);
	praat_addAction1 (classHMM, 1, U"Get probability staying in state...", nullptr, 1, REAL_HMM_getProbabilityOfStayingInState);
	praat_addAction1 (classHMM, 1, U"Get expected duration in state...", nullptr, 1, REAL_HMM_getExpectedValueOfDurationInState);
	praat_addAction1 (classHMM, 1, U"---- states / symbols -----", nullptr, 1, nullptr);
	praat_addAction1 (classHMM, 1, U"Get state label...", nullptr, 1, INFO_HMM_getStateLabel);
	praat_addAction1 (classHMM, 1, U"Get symbol label...", nullptr, 1, INFO_HMM_getSymbolLabel);
	praat_addAction1 (classHMM, 0, U"--- multiple HMMs ----", nullptr, 1, nullptr);
	praat_addAction1 (classHMM, 2, U"Get cross-entropy...", nullptr, 1, REAL_HMM_and_HMM_getCrossEntropy);

	praat_addAction1 (classHMM, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 1, U"Set transition probabilities...", nullptr, 1, MODIFY_HMM_setTransitionProbabilities);
	praat_addAction1 (classHMM, 1, U"Set emission probabilities...", nullptr, 1, MODIFY_HMM_setEmissionProbabilities);
	praat_addAction1 (classHMM, 1, U"Set start probabilities...", nullptr, 1, MODIFY_HMM_setStartProbabilities);

	praat_addAction1 (classHMM, 0, U"Extract -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 0, U"Extract transition probabilities", nullptr, 1, NEW_HMM_extractTransitionProbabilities);
	praat_addAction1 (classHMM, 0, U"Extract emission probabilities", nullptr, 1, NEW_HMM_extractEmissionProbabilities);

	praat_addAction1 (classHMM, 0, U"To HMMObservationSequence...", nullptr, 0, NEW_HMM_to_HMMObservationSequence);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"Draw trellis...", nullptr, 0, GRAPHICS_HMM_and_HMMStateSequence_drawTrellis);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"Get probability", nullptr, 0, REAL_HMM_and_HMMStateSequence_getProbability);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"To TableOfReal (bigrams)...", nullptr, 0, NEW1_HMM_and_HMMStateSequence_to_TableOfReal);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get probability", nullptr, 0, REAL_HMM_and_HMMObservationSequence_getProbability);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get cross-entropy", nullptr, 0, REAL_HMM_and_HMMObservationSequence_getCrossEntropy);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get perplexity", nullptr, 0, REAL_HMM_and_HMMObservationSequence_getPerplexity);


	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"To HMMStateSequence", nullptr, 0, NEW1_HMM_and_HMMObservationSequence_to_HMMStateSequence);
	praat_addAction2 (classHMM, 2, classHMMObservationSequence, 1, U"Get cross-entropy", nullptr, 0, REAL_HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"To TableOfReal (bigrams)...", nullptr, 0, NEW1_HMM_and_HMMObservationSequence_to_TableOfReal);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 0, U"Learn...", nullptr, 0, MODIFY_HMM_and_HMMObservationSequence_learn);

	praat_addAction1 (classHMMObservationSequence, 0, U"To TableOfReal (bigrams)...", nullptr, 0, NEW_HMMObservationSequence_to_TableOfReal);
	praat_addAction1 (classHMMObservationSequence, 0, U"To Strings", nullptr, 0, NEW_HMMObservationSequence_to_Strings);
	praat_addAction1 (classHMMStateSequence, 0, U"To TableOfReal (bigrams)...", nullptr, 0, NEW_HMMStateSequence_to_TableOfReal);
	praat_addAction1 (classHMMStateSequence, 0, U"To Strings", nullptr, 0, NEW_HMMStateSequence_to_Strings);

	praat_addAction1 (classHMMObservationSequence, 0, U"To HMM...", nullptr, 1, NEW_HMMObservationSequence_to_HMM);

	praat_addAction1 (classStrings, 0, U"To HMMObservationSequence", nullptr, praat_HIDDEN, NEW_Strings_to_HMMObservationSequence);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture (row labels)...", U"To Covariance", praat_HIDDEN + praat_DEPTH_1, NEW_TableOfReal_to_GaussianMixture_fromRowlabels);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture...", U"To Covariance", praat_HIDDEN + praat_DEPTH_1, NEW_TableOfReal_to_GaussianMixture);

	INCLUDE_MANPAGES (manual_HMM)
}

/* End of file praat_HMM_init.cpp 941*/
