#ifndef _HMM_h_
#define _HMM_h_
/* HMM.h
 *
 * Copyright (C) 2010-2019 David Weenink
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

#include "Strings_extensions.h"
#include "TableOfReal.h"
#include "Collection.h"
#include "GaussianMixture.h"
#include "SSCP.h"
#include "Index.h"


Thing_declare (HMMStateList);
Thing_declare (HMMObservationList);

#include "HMM_enums.h"

#include "HMM_def.h"

/********** class HMMStateList **********/

Collection_define (HMMStateList, OrderedOf, HMMState) {
};

/********** class HMMObservationList **********/

Collection_define (HMMObservationList, OrderedOf, HMMObservation) {
};

/********** class HMMBaumWelch **********/

Thing_define (HMMBaumWelch, Daata) {
	integer capacity;
	integer numberOfTimes;
	integer totalNumberOfSequences;
	integer numberOfStates;
	integer numberOfSymbols;
	double lnProb;
	double minProb;
	autoMAT alpha;
	autoMAT beta;
	autoVEC scale;
	autoMAT gamma;
	autoTEN3 xi;
	autoVEC aij_num_p0;
	autoMAT aij_num;
	autoVEC aij_denom_p0;
	autoMAT aij_denom;
	autoMAT bik_num;
	autoMAT bik_denom;
};

Thing_define (HMMStateSequence, Strings) {
};

Thing_define (HMMObservationSequence, Table) {
};
// First column is always a symbol, if only 1 column then symbols only

Collection_define (HMMObservationSequenceBag, CollectionOf, HMMObservationSequence) {
};

autoHMMObservationSequence HMMObservationSequence_create (integer numberOfItems, integer dataLength);

void HMMObservationSequence_removeObservation (HMMObservationSequence me, integer index);

autoStrings HMMObservationSequence_to_Strings (HMMObservationSequence me);

autoHMMObservationSequence Strings_to_HMMObservationSequence (Strings me);

autoStringsIndex HMMObservationSequence_to_StringsIndex (HMMObservationSequence me);

autoStringsIndex HMM_HMMObservationSequence_to_StringsIndex (HMM me, HMMObservationSequence thee);

autoTableOfReal HMMObservationSequence_to_TableOfReal_transitions (HMMObservationSequence me, int probabilities);

autoTableOfReal HMM_HMMObservationSequence_to_TableOfReal_transitions (HMM me, HMMObservationSequence thee, int probabilities);

integer HMMObservationSequence_getNumberOfObservations (HMMObservationSequence me);

integer HMMObservationSequenceBag_getLongestSequence (HMMObservationSequenceBag me);

autoTableOfReal HMMStateSequence_to_TableOfReal_transitions (HMMStateSequence me);

autoTableOfReal HMM_HMMStateSequence_to_TableOfReal_transitions (HMM me, HMMStateSequence thee, int probabilities);

autoHMMStateSequence HMMStateSequence_create (integer numberOfItems);

autoStrings HMMStateSequence_to_Strings (HMMStateSequence me);

void HMMState_setLabel (HMMState me, char32 *label);


autoHMM HMM_create (int leftToRight, integer numberOfStates, integer numberOfObservationSymbols);

autoHMM HMM_createSimple (int leftToRight, conststring32 states_string, conststring32 symbols_string);

autoHMM HMM_createContinuousModel (int leftToRight, integer numberOfStates, integer numberOfObservationSymbols,
	integer numberOfMixtureComponentsPerSymbol, integer componentDimension, kHMMstorage componentStorage);

autoHMM HMM_createFullContinuousModel (int leftToRight, integer numberOfStates, integer numberOfObservationSymbols,
	integer numberOfFeatureStreams, integer *dimensionOfStream, integer *numberOfGaussiansforStream);

autoHMM HMM_createFromHMMObservationSequence (HMMObservationSequence me, integer numberOfStates, int leftToRight);

void HMM_draw (HMM me, Graphics g, bool garnish);

void HMM_drawBackwardProbabilitiesIllustration (Graphics g, bool garnish);

void HMM_drawForwardProbabilitiesIllustration (Graphics g, bool garnish);

void HMM_drawForwardAndBackwardProbabilitiesIllustration (Graphics g, bool garnish);

void HMM_addObservation_move (HMM me, autoHMMObservation thee);

void HMM_addState_move (HMM me, autoHMMState thee);

void HMM_setDefaultStates (HMM me);

void HMM_setDefaultObservations (HMM me);

void HMM_setDefaultTransitionProbs (HMM me);

void HMM_setDefaultInitialStateProbs (HMM me);

void HMM_setDefaultEmissionProbs (HMM me);

void HMM_setDefaultMixingProbabilities (HMM me);

void HMM_unExpandPCA (HMM me);
/* Unexpand all the PCA's in the gaussian mixtures */



/*
	Set the probabilities. A probability zero value indicates that this p cannot be changed during training/learning.
*/
void HMM_setTransitionProbabilities (HMM me, integer state_number, conststring32 state_probs);

void HMM_setEmissionProbabilities (HMM me, integer state_number, conststring32 emission_probs);

void HMM_setStartProbabilities (HMM me, conststring32 probs);


double HMM_getProbabilityAtTimeBeingInState (HMM me, integer itime, integer istate);

double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, integer itime, integer istate, integer isymbol);

double HMM_getExpectedValueOfDurationInState (HMM me, integer istate);

double HMM_getProbabilityOfStayingInState (HMM me, integer istate, integer numberOfTimeUnits);

double HMM_HMM_getCrossEntropy (HMM me, HMM thee, integer observationLength, int symmetric);

double HMM_HMM_HMMObservationSequence_getCrossEntropy (HMM me, HMM thee, HMMObservationSequence him);

autoTableOfReal HMM_extractTransitionProbabilities (HMM me);

autoTableOfReal HMM_extractEmissionProbabilities (HMM me);

/* HMM & HMMObservationSequence ****/

autoHMMObservationSequence HMM_to_HMMObservationSequence (HMM me, integer initialState, integer numberOfItems);

autoHMMStateSequence HMM_HMMObservationSequence_to_HMMStateSequence (HMM me, HMMObservationSequence thee);

double HMM_HMMStateSequence_getProbability (HMM me, HMMStateSequence thee);

void HMM_HMMObservationSequenceBag_learn (HMM me, HMMObservationSequenceBag thee, double delta_lnp, double minProb, int info);

void HMM_HMMStateSequence_drawTrellis (HMM me, HMMStateSequence thee, Graphics g, bool connect, bool garnish);

double HMM_HMMObservationSequence_getProbability (HMM me, HMMObservationSequence thee);

double HMM_HMMObservationSequence_getCrossEntropy (HMM me, HMMObservationSequence thee);

double HMM_HMMObservationSequence_getPerplexity (HMM me, HMMObservationSequence thee);

// somewhere else
void MelderInfo_lnp (double logp);
autoTableOfReal Strings_to_TableOfReal_transitions (Strings me, int probabilities);

#endif /* _HMM_h_ */
