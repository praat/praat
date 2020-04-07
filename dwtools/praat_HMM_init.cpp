/* praat_HMM_init.cpp
 *
 * Copyright (C) 2010-2020 David Weenink
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

DIRECT (HELP_GaussianMixture_help) {
	HELP (U"GaussianMixture")
}

FORM (GRAPHICS_GaussianMixture_drawConcentrationEllipses, U"GaussianMixture: Draw concentration ellipses", U"GaussianMixture: Draw concentration ellipses...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (principalComponentPlane, U"Principal component plane", true)
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
	GRAPHICS_EACH (GaussianMixture)
		GaussianMixture_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false, nullptr, principalComponentPlane, xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_GaussianMixture_PCA_drawConcentrationEllipses, U"GaussianMixture & PCA: Draw concentration ellipses", U"GaussianMixture & PCA: Draw concentration ellipses...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
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
	GRAPHICS_TWO (GaussianMixture, PCA)
		GaussianMixture_PCA_drawConcentrationEllipses (me, you, GRAPHICS, numberOfSigmas, false, nullptr,
				xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	GRAPHICS_TWO_END	
}

FORM (GRAPHICS_GaussianMixture_drawMarginalPdf, U"GaussianMixture: Draw marginal pdf", U"GaussianMixture: Draw marginal pdf...") {
	INTEGER (xDimension, U"X-dimension", U"1")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (numberOfPoints, U"Number of points", U"500")
	INTEGER (numberOfBins, U"Number of bins", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (GaussianMixture)
		GaussianMixture_drawMarginalPdf (me, GRAPHICS, xDimension, xmin, xmax, ymin, ymax, numberOfPoints, numberOfBins, garnish);
	GRAPHICS_EACH_END
}

DIRECT (INTEGER_GaussianMixture_getNumberOfComponents) {
	INTEGER_ONE (GaussianMixture)
		integer result = my numberOfComponents;
	INTEGER_ONE_END (U"(= number of components)")
}

DIRECT (INTEGER_GaussianMixture_getDimensionOfComponent) {
	INTEGER_ONE (GaussianMixture)
		integer result = my dimension;
	INTEGER_ONE_END (U"(= dimension of component)")
}

FORM (REAL_GaussianMixture_getProbabilityAtPosition, U"GaussianMixture: Get probability at position", nullptr) {
	SENTENCE (position_string, U"Position", U"100.0 300.0")
	OK
DO
	NUMBER_ONE (GaussianMixture)
		double result = GaussianMixture_getProbabilityAtPosition_string (me, position_string);
	NUMBER_ONE_END (U" (= probability at position ", position_string, U")")
}

FORM (MODIFY_GaussianMixture_splitComponent, U"GaussianMixture: Split component", U"GaussianMixture: Split component...") {
	NATURAL (component, U"Component", U"1")
	OK
DO
	MODIFY_EACH (GaussianMixture)
		GaussianMixture_splitComponent (me, component);
	MODIFY_EACH_END
}

FORM (GRAPHICS_GaussianMixture_PCA_drawMarginalPdf, U"GaussianMixture & PCA: Draw pdf function", U"GaussianMixture: Draw marginal pdf...") {
	INTEGER (xDimension, U"X-dimension", U"1")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (numberOfPoints, U"Number of points", U"500")
	INTEGER (numberOfBins, U"Number of bins", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (GaussianMixture, PCA)
		GaussianMixture_PCA_drawMarginalPdf (me, you, GRAPHICS, xDimension, xmin, xmax, ymin, ymax, numberOfPoints, numberOfBins, garnish);
	GRAPHICS_TWO_END
}

FORM (NEW1_GaussianMixture_PCA_to_Matrix_density, U"GaussianMixture & PCA: To Matrix density", U"GaussianMixture & PCA: To Matrix (density)...") {
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	NATURAL (numberOfColumns, U"Number of columns", U"100")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (numberOfRows, U"Number of rows", U"100")
	OK
DO
	CONVERT_TWO (GaussianMixture, PCA)
		autoMatrix result = GaussianMixture_PCA_to_Matrix_density (me, you, xDimension, yDimension, xmin, xmax, numberOfColumns, ymin, ymax, numberOfRows);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW_GaussianMixture_extractComponent, U"GaussianMixture: Extract component", nullptr) {
	NATURAL (component, U"Component", U"1")
	OK
DO
	CONVERT_EACH (GaussianMixture)
		autoCovariance result = GaussianMixture_extractComponent (me, component);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_GaussianMixture_extractCentroids) {
	CONVERT_EACH (GaussianMixture)
		autoTableOfReal result = GaussianMixture_extractCentroids (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_GaussianMixture_extractMixingProbabilities) {
	CONVERT_EACH (GaussianMixture)
		autoTableOfReal result = GaussianMixture_extractMixingProbabilities (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_GaussianMixture_to_PCA) {
	CONVERT_EACH (GaussianMixture)
		autoPCA result = GaussianMixture_to_PCA (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_GaussianMixture_to_TableOfReal_randomSampling, U"GaussianMixture: To TableOfReal (random sampling)", U"GaussianMixture: To TableOfReal (random sampling)...") {
	NATURAL (numberOfPoints, U"Number of data points", U"100")
	OK
DO
	CONVERT_EACH (GaussianMixture)
		autoTableOfReal result = GaussianMixture_to_TableOfReal_randomSampling (me, numberOfPoints);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_GaussianMixture_to_Covariance_between) {
	CONVERT_EACH (GaussianMixture)
		autoCovariance result = GaussianMixture_to_Covariance_between (me);
	CONVERT_EACH_END (my name.get(), U"_b")
}

DIRECT (NEW_GaussianMixture_to_Covariance_within) {
	CONVERT_EACH (GaussianMixture)
		autoCovariance result = GaussianMixture_to_Covariance_within (me);
	CONVERT_EACH_END (my name.get(), U"_w");
}

DIRECT (NEW_GaussianMixture_to_Covariance_total) {
	CONVERT_EACH (GaussianMixture)
		autoCovariance result = GaussianMixture_to_Covariance_total (me);
	CONVERT_EACH_END (my name.get(), U"_t");
}

FORM (REAL_GaussianMixture_TableOfReal_getLikelihoodValue, U"GaussianMixture & TableOfReal: Get likelihood value",
      U"GaussianMixture & TableOfReal: Get likelihood value...") {
	OPTIONMENU_ENUM (kGaussianMixtureCriterion, criterion, U"Criterion based on", kGaussianMixtureCriterion::DEFAULT)	OK
DO
	NUMBER_TWO (GaussianMixture, TableOfReal)
		conststring32 criterionText = GaussianMixture_criterionText (criterion);
		double lnpdn = GaussianMixture_TableOfReal_getLikelihoodValue (me, you, criterion);
		double result = lnpdn / you -> numberOfRows;
	NUMBER_TWO_END (U" (= ", criterionText, U", n = ", you -> numberOfRows, U")")
}

DIRECT (HELP_HMM_help) {
	HELP (U"HMM")
}

FORM (NEW1_HMM_create, U"Create HMM", nullptr) {
	WORD (name, U"Name", U"hmm")
	BOOLEAN (leftToRightModel, U"Left to right model", false)
	NATURAL (numberOfStates, U"Number of states", U"3")
	NATURAL (numberOfObservations, U"Number of observations", U"3")
	OK
DO
	CREATE_ONE
		autoHMM result = HMM_create (leftToRightModel, numberOfStates, numberOfObservations);
	CREATE_ONE_END (name)
}

FORM (NEW1_HMM_createSimple, U"HMM: Create simple", U"HMM: Create simple HMM...") {
	WORD (name, U"Name", U"weather")
	BOOLEAN (leftToRightModel, U"Left to right model", false)
	SENTENCE (states_string, U"States", U"Rainy Sunny")
	SENTENCE (observations_string, U"Observations", U"Walk Shop Clean")
	OK
DO
	CREATE_ONE
		autoHMM result = HMM_createSimple (leftToRightModel, states_string, observations_string);
	CREATE_ONE_END (name)
}

FORM (NEW1_HMM_createContinuousModel, U"HMM: Create continuous model", nullptr) {
	WORD (name, U"Name", U"cm")
	BOOLEAN (leftToRightModel, U"Left to right model", false)
	NATURAL (numberOfStates, U"Number of states", U"3")
	NATURAL (numberOfSymbols, U"Number of symbols", U"10")
	LABEL (U"For the Gaussian mixtures:")
	NATURAL (numberOfComponents, U"Number of components", U"3")
	NATURAL (componentDimension, U"Dimension of component", U"39")
	RADIO_ENUM (kHMMstorage, storage,
			U"Covariance matrices are", kHMMstorage::DEFAULT)
	OK
DO
	CREATE_ONE
		autoHMM result = HMM_createContinuousModel (leftToRightModel, numberOfStates, numberOfSymbols, numberOfComponents, componentDimension, storage);
	CREATE_ONE_END (name)
}

FORM (NEW_HMMObservationSequence_to_HMM, U"HMMObservationSequence: To HMM", nullptr) {
	LABEL (U"(0 states gives a non-hidden model) ")
	INTEGER (numberOfHiddenStates, U"Number of hidden states", U"2")
	BOOLEAN (leftToRightModel, U"Left to right model", false)
	OK
DO
	CONVERT_EACH (HMMObservationSequence)
		autoHMM result = HMM_createFromHMMObservationSequence (me, numberOfHiddenStates, leftToRightModel);
	CONVERT_EACH_END (my name.get(), U"_", numberOfHiddenStates)
}

FORM (GRAPHICS_HMM_draw, U"HMM: Draw", nullptr) {
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (HMM)
		HMM_draw (me, GRAPHICS, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_HMM_HMMStateSequence_drawTrellis, U"HMM & Strings: Draw trellis", nullptr) {
	BOOLEAN (connect, U"Connect", true);
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_TWO (HMM, HMMStateSequence)
		HMM_HMMStateSequence_drawTrellis (me, you, GRAPHICS, connect, garnish);
	GRAPHICS_TWO_END
}

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
	NATURAL (fromState, U"From state number", U"1")
	NATURAL (toState, U"To state number", U"1")
	OK
DO
	NUMBER_ONE (HMM)
		Melder_require (fromState <= my numberOfStates && toState <= my numberOfStates, U"State number(s) too high.");
		double result = my transitionProbs [fromState] [toState];
	NUMBER_ONE_END (U" : [ ", fromState, U", ", toState, U" ]")
}

FORM (REAL_HMM_getEmissionProbability, U"HMM: Get emission probability", U"HMM: Get emission probability...") {
	NATURAL (fromState, U"From state number", U"1")
	NATURAL (symbolIndex, U"Symbol index", U"1")
	OK
DO
	NUMBER_ONE (HMM)
		Melder_require (fromState <= my numberOfStates, U"State number too high.");
		Melder_require (symbolIndex <= my numberOfObservationSymbols, U"Symbol number too high.");
		double result = my emissionProbs[fromState][symbolIndex];
	NUMBER_ONE_END (U" : [ ", fromState, U", ", symbolIndex, U" ]")
}

FORM (REAL_HMM_getStartProbability, U"HMM: Get start probability", U"HMM: Get start probability...") {
	NATURAL (stateNumber, U"State number", U"1")
	OK
DO
	NUMBER_ONE (HMM)
		Melder_require (stateNumber <= my numberOfStates, U"State number too high.");
		double result = my initialStateProbs [stateNumber];
	NUMBER_ONE_END (U" : [ ", stateNumber, U" ]")
}

FORM (REAL_HMM_getProbabilityAtTimeBeingInState, U"HMM: Get probability of being in state at time",
      U"HMM: Get p (time, state)...") {
	NATURAL (timeIndex, U"Time index", U"10")
	NATURAL (stateNumber, U"State number", U"1")
	OK
DO
	NUMBER_ONE (HMM)
		double result = HMM_getProbabilityAtTimeBeingInState (me, timeIndex, stateNumber);
	NUMBER_ONE_END (U" (= ln(p), p = ", Melder_naturalLogarithm (result), U") Being in state ", stateNumber, U" at time ", timeIndex)
}

FORM (REAL_HMM_getProbabilityAtTimeBeingInStateEmittingSymbol, U"HMM: get probability being at time in state emitting symbol", U"HMM: Get p (time, state, symbol)...") {
	NATURAL (timeIndex, U"Time index", U"10")
	NATURAL (stateNumber, U"State number", U"1")
	NATURAL (symbolNumber, U"Symbol number", U"1")
	OK
DO
	NUMBER_ONE (HMM)
		double result = HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (me, timeIndex, stateNumber, symbolNumber);
	NUMBER_ONE_END (U" (= ln(p), p = ", Melder_naturalLogarithm (result), U") Being in state ", stateNumber, U" emitting symbol ", symbolNumber, U" at time ", timeIndex)
}

FORM (REAL_HMM_getProbabilityOfStayingInState, U"HMM: Get probability of staying in state", U"HMM: Get probability staying in state...") {
	NATURAL (stateNumber, U"State number", U"1")
	NATURAL (numberOfTimeUnits, U"Number of time units", U"2")
	OK
DO
	NUMBER_ONE (HMM)
		double result = HMM_getProbabilityOfStayingInState (me, stateNumber, numberOfTimeUnits);
	NUMBER_ONE_END (U"(probability of staying in state)")
}

FORM (REAL_HMM_getExpectedDurationInState, U"HMM: Get expected value of duration in state",
      U"HMM: Get expected duration in state...") {
	NATURAL (stateNumber, U"State number", U"1")
	OK
DO
	NUMBER_ONE (HMM)
		double result = HMM_getExpectedValueOfDurationInState (me, stateNumber);
	NUMBER_ONE_END (U" time units")
}

FORM (INFO_HMM_getSymbolLabel, U"HMM: Get symbol label", nullptr) {
	NATURAL (symbolNumber, U"Symbol number", U"1")
	OK
DO
	STRING_ONE (HMM)
		Melder_require (symbolNumber <= my numberOfObservationSymbols, U"Symbol number too high.");
		HMMObservation observation = my observationSymbols->at [symbolNumber];
		conststring32 result = observation -> label.get();
	STRING_ONE_END
}

FORM (INFO_HMM_getStateLabel, U"HMM: Get state label", nullptr) {
	NATURAL (stateNumber, U"State number", U"1")
	OK
DO
	STRING_ONE (HMM)
		Melder_require (stateNumber <= my numberOfStates, U"State number too high.");
		HMMState state = my states->at [stateNumber];
		conststring32 result = state -> label.get();
	STRING_ONE_END
}

FORM (REAL_HMM_HMM_getCrossEntropy, U"HMM & HMM: Get cross-entropy...", U"HMM & HMM: Get cross-entropy...") {
	NATURAL (observationLength, U"Observation length", U"2000")
	BOOLEAN (symmetric, U"Symmetric", true)
	OK
DO
	NUMBER_COUPLE (HMM)
		double result = HMM_HMM_getCrossEntropy (me, you, observationLength, symmetric);
	NUMBER_COUPLE_END (U" (= ", (symmetric ? U"symmetric " : U""), U" cross-entropy between models for observation length = ", observationLength, U")")
}

DIRECT (REAL_HMM_HMM_HMMObservationSequence_getCrossEntropy) {
	NUMBER_COUPLE_AND_ONE (HMM, HMMObservationSequence)
		double result = HMM_HMM_HMMObservationSequence_getCrossEntropy (me, you, him);
	NUMBER_COUPLE_AND_ONE_END (U"(= symmetric cross-entropy between models)")
}

FORM (NEW_HMM_to_HMMObservationSequence, U"HMM: To HMMObservationSequence (generate observations)", U"HMM: To HMMObservationSequence...") {
	INTEGER (startState, U"Start state", U"0")
	NATURAL (numberOfObservations, U"Number of observations", U"20")
	OK
DO
	CONVERT_EACH (HMM)
		autoHMMObservationSequence result = HMM_to_HMMObservationSequence (me, startState, numberOfObservations);
	CONVERT_EACH_END (my name.get())
}

DIRECT (REAL_HMM_HMMStateSequence_getProbability) {
	NUMBER_TWO (HMM, HMMStateSequence)
		double result = HMM_HMMStateSequence_getProbability (me, you);
	NUMBER_TWO_END (U" (= ln(p), p = ", Melder_naturalLogarithm (result), U")")
}

DIRECT (REAL_HMM_HMMObservationSequence_getProbability) {
	NUMBER_TWO (HMM, HMMObservationSequence)
		double result = HMM_HMMObservationSequence_getProbability (me, you);
	NUMBER_TWO_END (U" (= ln(p), p = ", Melder_naturalLogarithm (result), U")")
}

DIRECT (REAL_HMM_HMMObservationSequence_getCrossEntropy) {
	NUMBER_TWO (HMM, HMMObservationSequence)
	double result = HMM_HMMObservationSequence_getCrossEntropy (me, you);
	NUMBER_TWO_END (U" (= cross-entropy)")
}

DIRECT (REAL_HMM_HMMObservationSequence_getPerplexity) {
	NUMBER_TWO (HMM, HMMObservationSequence)
		double result = HMM_HMMObservationSequence_getPerplexity (me, you);
	NUMBER_TWO_END (U" (= perplexity)")
}

DIRECT (NEW1_HMM_HMMObservationSequence_to_HMMStateSequence) {
	CONVERT_TWO (HMM, HMMObservationSequence)
		autoHMMStateSequence result = HMM_HMMObservationSequence_to_HMMStateSequence (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get(), U"_states")
}

FORM (MODIFY_HMM_HMMObservationSequence_learn, U"HMM & HMMObservationSequence: Learn", U"HMM & HMMObservationSequences: Learn...") {
	POSITIVE (relativePrecision_log, U"Relative precision in log(p)", U"0.001")
	REAL (minimumProbability, U"Minimum probability", U"0.00000000001")
	BOOLEAN (showProgress, U"Learning history in Info window", false)
	OK
DO
	Melder_require (minimumProbability >= 0.0 && minimumProbability < 1.0, U"The minimum probabilty should be in [0, 1).");
	MODIFY_FIRST_OF_ONE_AND_LIST(HMM, HMMObservationSequence)
		HMM_HMMObservationSequenceBag_learn (me, (HMMObservationSequenceBag) & list, relativePrecision_log, minimumProbability, showProgress);
	MODIFY_FIRST_OF_ONE_AND_LIST_END
}

FORM (MODIFY_HMM_setTransitionProbabilities, U"HMM: Set transition probabilities", U"HMM: Set transition probabilities...") {
	NATURAL (stateNumber, U"State number", U"1")
	SENTENCE (probabilities_string, U"Probabilities", U"0.1 0.9")
	OK
DO
	MODIFY_EACH (HMM)
		HMM_setTransitionProbabilities (me, stateNumber, probabilities_string);
	MODIFY_EACH_END
}

FORM (MODIFY_HMM_setEmissionProbabilities, U"HMM: Set emission probabilities", U"HMM: Set emission probabilities...") {
	NATURAL (stateNumber, U"State number", U"1")
	SENTENCE (probabilities_string, U"Probabilities", U"0.1 0.7 0.2")
	OK
DO
	MODIFY_EACH (HMM)
		HMM_setEmissionProbabilities (me, stateNumber, probabilities_string);
	MODIFY_EACH_END
}

FORM (MODIFY_HMM_setStartProbabilities, U"HMM: Set start probabilities", U"HMM: Set start probabilities...") {
	SENTENCE (probabilities_string, U"Probabilities", U"0.1 0.9")
	OK
DO
	MODIFY_EACH (HMM)
		HMM_setStartProbabilities (me, probabilities_string);
	MODIFY_EACH_END
}

DIRECT (NEW_HMM_extractTransitionProbabilities) {
	CONVERT_EACH (HMM)
		autoTableOfReal result = HMM_extractTransitionProbabilities (me);
	CONVERT_EACH_END (my name.get(), U"_t")
}

DIRECT (NEW_HMM_extractEmissionProbabilities) {
	CONVERT_EACH (HMM)
		autoTableOfReal result = HMM_extractEmissionProbabilities (me);
	CONVERT_EACH_END (my name.get(), U"_e")
}

FORM (NEW_HMMObservationSequence_to_TableOfReal_bigrams, U"HMMObservationSequence: To TableOfReal ", U"HMMObservationSequence: To TableOfReal (bigrams)...") {
	BOOLEAN (asProbabilities, U"As probabilities", true)
	OK
DO
	CONVERT_EACH (HMMObservationSequence)
		autoTableOfReal result = HMMObservationSequence_to_TableOfReal_transitions (me, asProbabilities);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_HMM_HMMObservationSequence_to_TableOfReal_bigrams, U"HMM & HMMObservationSequence: To TableOfReal", U"HMM & HMMObservationSequence: To TableOfReal (bigrams)...") {
	BOOLEAN (asProbabilities, U"As probabilities", true)
	OK
DO
	CONVERT_TWO (HMM, HMMObservationSequence)
		autoTableOfReal result = HMM_HMMObservationSequence_to_TableOfReal_transitions (me, you, asProbabilities);
	CONVERT_TWO_END (your name.get(), U"_m")
}

FORM (NEW1_HMM_HMMStateSequence_to_TableOfReal_bigrams, U"HMM & HMMStateSequence: To TableOfReal", nullptr) {
	BOOLEAN (asProbabilities, U"As probabilities", true)
	OK
DO
	CONVERT_TWO (HMM, HMMStateSequence)
		autoTableOfReal result = HMM_HMMStateSequence_to_TableOfReal_transitions (me, you, asProbabilities);
	CONVERT_TWO_END (your name.get(), U"_m")
}

FORM (NEW_HMMStateSequence_to_TableOfReal_bigrams, U"HMMStateSequence: To TableOfReal", nullptr) {
	BOOLEAN (asProbabilities, U"As probabilities", true)
	OK
DO
	CONVERT_EACH (HMMStateSequence)
		autoTableOfReal result = Strings_to_TableOfReal_transitions (me, asProbabilities);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_HMMObservationSequence_to_Strings) {
	CONVERT_EACH (HMMObservationSequence)
		autoStrings result = HMMObservationSequence_to_Strings (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Strings_to_HMMObservationSequence) {
	CONVERT_EACH (Strings)
		autoHMMObservationSequence result = Strings_to_HMMObservationSequence (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_HMMStateSequence_to_Strings) {
	CONVERT_EACH (HMMStateSequence)
		autoStrings result = HMMStateSequence_to_Strings (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TableOfReal_to_GaussianMixture_rowlabels, U"TableOfReal: To GaussianMixture from row labels", U"TableOfReal: To GaussianMixture (row labels)...") {
	RADIO_ENUM (kGaussianMixtureStorage, storage,
		U"Covariance matrices are", kGaussianMixtureStorage::DEFAULT)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoGaussianMixture result = TableOfReal_to_GaussianMixture_fromRowLabels (me, storage);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TableOfReal_to_GaussianMixture, U"TableOfReal: To GaussianMixture (no labels)", U"TableOfReal: To GaussianMixture...") {
	NATURAL (numberOfComponents, U"Number of components", U"2")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"0.001")
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	REAL (lambda, U"Stability coefficient lambda", U"0.001")
	RADIO_ENUM (kGaussianMixtureStorage, storage,
			U"Covariance matrices are", kGaussianMixtureStorage::DEFAULT)
	OPTIONMENU_ENUM (kGaussianMixtureCriterion, criterion, U"Criterion based on", kGaussianMixtureCriterion::DEFAULT)
	OK
DO
	Melder_require (lambda >= 0.0 && lambda < 1.0, U"Lambda should be in the interval [0, 1).");
	CONVERT_EACH (TableOfReal)
		autoGaussianMixture result = TableOfReal_to_GaussianMixture (me, numberOfComponents, tolerance, maximumNumberOfIterations, lambda, storage, criterion);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_TableOfReal_to_GaussianMixture_CEMM, U"TableOfReal: To GaussianMixture (CEMM)", U"TableOfReal: To GaussianMixture (CEMM)...") {
	INTEGER (minimumNumberOfComponents, U"Minimum number of components", U"1")
	INTEGER (maximumNumberOfComponents, U"Maximum number of components", U"10")
	RADIO_ENUM (kGaussianMixtureStorage, storage,
			U"Covariance matrices are", kGaussianMixtureStorage::DEFAULT)
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"0.00001")
	BOOLEAN (info, U"Info", false)
	OK
DO
	CONVERT_EACH (TableOfReal)
		autoGaussianMixture result = TableOfReal_to_GaussianMixture_CEMM (me, minimumNumberOfComponents, maximumNumberOfComponents, storage, maximumNumberOfIterations, tolerance, info);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_GaussianMixture_TableOfReal_improveLikelihood, U"GaussianMixture & TableOfReal: Improve likelihood", U"GaussianMixture & TableOfReal: Improve likelihood...") {
	POSITIVE (tolerance, U"Tolerance of minimizer", U"0.001")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	REAL (lambda, U"Stability coefficient lambda", U"0.001")
	OPTIONMENU_ENUM (kGaussianMixtureCriterion, criterion, U"Criterion based on", kGaussianMixtureCriterion::DEFAULT)
	OK
DO
	Melder_require (lambda >= 0.0 && lambda < 1.0, U"Lambda should be in the interval [0, 1).");
	MODIFY_FIRST_OF_TWO (GaussianMixture, TableOfReal)
		GaussianMixture_TableOfReal_improveLikelihood (me, you, tolerance, maximumNumberOfIterations, lambda, criterion);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW1_GaussianMixture_TableOfReal_to_GaussianMixture_CEMM, U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)", U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)...") {
	INTEGER (minimumNumberOfComponents, U"Minimum number of components", U"1")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"0.001")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"200")
	BOOLEAN (info, U"Info", false)
	OK
DO
	CONVERT_TWO (GaussianMixture, TableOfReal)
		autoGaussianMixture result = GaussianMixture_TableOfReal_to_GaussianMixture_CEMM (me, you, minimumNumberOfComponents, maximumNumberOfIterations, tolerance, info);
	CONVERT_TWO_END (my name.get())
}

DIRECT (NEW1_GaussianMixture_TableOfReal_to_ClassificationTable) {
	CONVERT_TWO (GaussianMixture, TableOfReal)
		autoClassificationTable result = GaussianMixture_TableOfReal_to_ClassificationTable (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_GaussianMixture_TableOfReal_to_TableOfReal_probabilities) {
	CONVERT_TWO (GaussianMixture, TableOfReal)
		autoTableOfReal result = GaussianMixture_TableOfReal_to_TableOfReal_probabilities (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_GaussianMixture_TableOfReal_to_TableOfReal_responsibilities) {
	CONVERT_TWO (GaussianMixture, TableOfReal)
		autoTableOfReal result = GaussianMixture_TableOfReal_to_TableOfReal_responsibilities (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW1_GaussianMixture_TableOfReal_to_Correlation) {
	CONVERT_TWO (GaussianMixture, TableOfReal)
		autoCorrelation result = GaussianMixture_TableOfReal_to_Correlation (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_GaussianMixture_TableOfReal_to_Table_BHEPNormalityTests, U"GaussianMixture & TableOfReal: To Table (BHEP normality tests)", U"GaussianMixture & TableOfReal: To Table (BHEP normality tests)...") {
	REAL (smoothingParameter, U"Smoothing parameter", U"1.41")
	OK
DO
	CONVERT_TWO (GaussianMixture, TableOfReal)
		autoTable result = GaussianMixture_TableOfReal_to_Table_BHEPNormalityTests (me, you, smoothingParameter);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

void praat_HMM_init ();
void praat_HMM_init () {
	Thing_recognizeClassesByName (classHMM, classHMMState, classHMMObservation, classHMMObservationSequence, classHMMStateSequence, classGaussianMixture, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Markov models", nullptr, praat_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_HMM_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_HMM_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create continuous HMM...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW1_HMM_createContinuousModel);
	praat_addMenuCommand (U"Objects", U"New", U"--drawings--", nullptr, praat_HIDDEN + praat_DEPTH_1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Draw forward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, GRAPHICS_HMM_drawForwardProbabilitiesIllustration);
	praat_addMenuCommand (U"Objects", U"New", U"Draw backward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, GRAPHICS_HMM_drawBackwardProbabilitiesIllustration);
	praat_addMenuCommand (U"Objects", U"New", U"Draw forward and backward probabilities illustration", nullptr, praat_HIDDEN + praat_DEPTH_1, GRAPHICS_HMM_drawForwardAndBackwardProbabilitiesIllustration);
	
	praat_addAction1 (classGaussianMixture, 0, U"GaussianMixture help", nullptr, 0, HELP_GaussianMixture_help);
	praat_addAction1 (classGaussianMixture, 0, U"Draw concentration ellipses...", nullptr, 0, GRAPHICS_GaussianMixture_drawConcentrationEllipses);
	praat_addAction1 (classGaussianMixture, 0, U"Draw marginal pdf...", nullptr, 0, GRAPHICS_GaussianMixture_drawMarginalPdf);
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

	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"Get likelihood value...", nullptr, 0, REAL_GaussianMixture_TableOfReal_getLikelihoodValue);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"Improve likelihood...", nullptr, 0, MODIFY_GaussianMixture_TableOfReal_improveLikelihood);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To GaussianMixture (CEMM)...", nullptr, 0, NEW1_GaussianMixture_TableOfReal_to_GaussianMixture_CEMM);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To TableOfReal (probabilities)", nullptr, 0, NEW1_GaussianMixture_TableOfReal_to_TableOfReal_probabilities);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To TableOfReal (responsibilities)", nullptr, 0, NEW1_GaussianMixture_TableOfReal_to_TableOfReal_responsibilities);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To ClassificationTable", nullptr, 0, NEW1_GaussianMixture_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To Correlation", nullptr, 0, NEW1_GaussianMixture_TableOfReal_to_Correlation);
	praat_addAction2 (classGaussianMixture, 1, classTableOfReal, 1, U"To Table (BHEP normality tests)...", nullptr, 0, NEW1_GaussianMixture_TableOfReal_to_Table_BHEPNormalityTests);

	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"Draw concentration ellipses...", nullptr, 0, GRAPHICS_GaussianMixture_PCA_drawConcentrationEllipses);
	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"Draw marginal pdf...", nullptr, 0, GRAPHICS_GaussianMixture_PCA_drawMarginalPdf);
	praat_addAction2 (classGaussianMixture, 1, classPCA, 1, U"To Matrix (density)...", nullptr, 0, NEW1_GaussianMixture_PCA_to_Matrix_density);

	praat_addAction1 (classHMM, 0, U"HMM help ", nullptr, 0, HELP_HMM_help);
	praat_addAction1 (classHMM, 0, U"Draw...", nullptr, 0, GRAPHICS_HMM_draw);
	praat_addAction1 (classHMM, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 1, U"Get transition probability...", nullptr, 1, REAL_HMM_getTransitionProbability);
	praat_addAction1 (classHMM, 1, U"Get emission probability...", nullptr, 1, REAL_HMM_getEmissionProbability);
	praat_addAction1 (classHMM, 1, U"Get start probability...", nullptr, 1, REAL_HMM_getStartProbability);
	praat_addAction1 (classHMM, 1, U"Get p (time, state)...", nullptr, 1, REAL_HMM_getProbabilityAtTimeBeingInState);
	praat_addAction1 (classHMM, 1, U"Get p (time, state, symbol)...", nullptr, 1, REAL_HMM_getProbabilityAtTimeBeingInStateEmittingSymbol);
	praat_addAction1 (classHMM, 1, U"Get probability staying in state...", nullptr, 1, REAL_HMM_getProbabilityOfStayingInState);
	praat_addAction1 (classHMM, 1, U"Get expected duration in state...", nullptr, 1, REAL_HMM_getExpectedDurationInState);
	praat_addAction1 (classHMM, 1, U"---- states / symbols -----", nullptr, 1, nullptr);
	praat_addAction1 (classHMM, 1, U"Get state label...", nullptr, 1, INFO_HMM_getStateLabel);
	praat_addAction1 (classHMM, 1, U"Get symbol label...", nullptr, 1, INFO_HMM_getSymbolLabel);
	praat_addAction1 (classHMM, 0, U"--- multiple HMMs ----", nullptr, 1, nullptr);
	praat_addAction1 (classHMM, 2, U"Get cross-entropy...", nullptr, 1, REAL_HMM_HMM_getCrossEntropy);

	praat_addAction1 (classHMM, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 1, U"Set transition probabilities...", nullptr, 1, MODIFY_HMM_setTransitionProbabilities);
	praat_addAction1 (classHMM, 1, U"Set emission probabilities...", nullptr, 1, MODIFY_HMM_setEmissionProbabilities);
	praat_addAction1 (classHMM, 1, U"Set start probabilities...", nullptr, 1, MODIFY_HMM_setStartProbabilities);

	praat_addAction1 (classHMM, 0, U"Extract -", nullptr, 0, nullptr);
	praat_addAction1 (classHMM, 0, U"Extract transition probabilities", nullptr, 1, NEW_HMM_extractTransitionProbabilities);
	praat_addAction1 (classHMM, 0, U"Extract emission probabilities", nullptr, 1, NEW_HMM_extractEmissionProbabilities);

	praat_addAction1 (classHMM, 0, U"To HMMObservationSequence...", nullptr, 0, NEW_HMM_to_HMMObservationSequence);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"Draw trellis...", nullptr, 0, GRAPHICS_HMM_HMMStateSequence_drawTrellis);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"Get probability", nullptr, 0, REAL_HMM_HMMStateSequence_getProbability);
	praat_addAction2 (classHMM, 1, classHMMStateSequence, 1, U"To TableOfReal (bigrams)...", nullptr, 0, NEW1_HMM_HMMStateSequence_to_TableOfReal_bigrams);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get probability", nullptr, 0, REAL_HMM_HMMObservationSequence_getProbability);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get cross-entropy", nullptr, 0, REAL_HMM_HMMObservationSequence_getCrossEntropy);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"Get perplexity", nullptr, 0, REAL_HMM_HMMObservationSequence_getPerplexity);


	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"To HMMStateSequence", nullptr, 0, NEW1_HMM_HMMObservationSequence_to_HMMStateSequence);
	praat_addAction2 (classHMM, 2, classHMMObservationSequence, 1, U"Get cross-entropy", nullptr, 0, REAL_HMM_HMM_HMMObservationSequence_getCrossEntropy);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 1, U"To TableOfReal (bigrams)...", nullptr, 0, NEW1_HMM_HMMObservationSequence_to_TableOfReal_bigrams);
	praat_addAction2 (classHMM, 1, classHMMObservationSequence, 0, U"Learn...", nullptr, 0, MODIFY_HMM_HMMObservationSequence_learn);

	praat_addAction1 (classHMMObservationSequence, 0, U"To TableOfReal (bigrams)...", nullptr, 0, NEW_HMMObservationSequence_to_TableOfReal_bigrams);
	praat_addAction1 (classHMMObservationSequence, 0, U"To Strings", nullptr, 0, NEW_HMMObservationSequence_to_Strings);
	praat_addAction1 (classHMMStateSequence, 0, U"To TableOfReal (bigrams)...", nullptr, 0, NEW_HMMStateSequence_to_TableOfReal_bigrams);
	praat_addAction1 (classHMMStateSequence, 0, U"To Strings", nullptr, 0, NEW_HMMStateSequence_to_Strings);

	praat_addAction1 (classHMMObservationSequence, 0, U"To HMM...", nullptr, 1, NEW_HMMObservationSequence_to_HMM);

	praat_addAction1 (classStrings, 0, U"To HMMObservationSequence", nullptr, praat_HIDDEN, NEW_Strings_to_HMMObservationSequence);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture (row labels)...", U"To Covariance", praat_HIDDEN + praat_DEPTH_1, NEW_TableOfReal_to_GaussianMixture_rowlabels);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture...", U"To Covariance", praat_HIDDEN + praat_DEPTH_1, NEW_TableOfReal_to_GaussianMixture);
	praat_addAction1 (classTableOfReal, 0, U"To GaussianMixture (CEMM)...", U"To GaussianMixture...", praat_HIDDEN + praat_DEPTH_1, NEW1_TableOfReal_to_GaussianMixture_CEMM);
	INCLUDE_MANPAGES (manual_HMM)
}

/* End of file praat_HMM_init.cpp 941*/
