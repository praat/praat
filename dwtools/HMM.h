#ifndef _HMM_h_
#define _HMM_h_
/* HMM.h
 *
 * Copyright (C) 2010-2011, 2015 David Weenink
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

#include "Strings_extensions.h"
#include "TableOfReal.h"
#include "Collection.h"
#include "GaussianMixture.h"
#include "SSCP.h"
#include "Index.h"

Thing_declare (HMMStateList);
Thing_declare (HMMObservationList);

#include "HMM_def.h"

/********** class HMMStateList **********/

Collection_define (HMMStateList, OrderedOf, HMMState) {
};

/********** class HMMObservationList **********/

Collection_define (HMMObservationList, OrderedOf, HMMObservation) {
};

/********** class HMMBaumWelch **********/

Thing_define (HMMBaumWelch, Daata) {
	long capacity;
	long numberOfTimes;
	long totalNumberOfSequences;
	long numberOfStates;
	long numberOfSymbols;
	double lnProb;
	double minProb;
	double **alpha;
	double **beta;
	double *scale;
	double **gamma;
	double ***xi;
	double **aij_num, **aij_denom;
	double **bik_num, **bik_denom;

	void v_destroy () noexcept
		override;
};

Thing_define (HMMStateSequence, Strings) {
};

Thing_define (HMMObservationSequence, Table) {
};
// First column is always a symbol, if only 1 column then symbols only

Collection_define (HMMObservationSequenceBag, CollectionOf, HMMObservationSequence) {
};

autoHMMObservationSequence HMMObservationSequence_create (long numberOfItems, long dataLength);

void HMMObservationSequence_removeObservation (HMMObservationSequence me, long index);

autoStrings HMMObservationSequence_to_Strings (HMMObservationSequence me);

autoHMMObservationSequence Strings_to_HMMObservationSequence (Strings me);

autoStringsIndex HMMObservationSequence_to_StringsIndex (HMMObservationSequence me);

autoStringsIndex HMM_and_HMMObservationSequence_to_StringsIndex (HMM me, HMMObservationSequence thee);

autoTableOfReal HMMObservationSequence_to_TableOfReal_transitions (HMMObservationSequence me, int probabilities);

autoTableOfReal HMM_and_HMMObservationSequence_to_TableOfReal_transitions (HMM me, HMMObservationSequence thee, int probabilities);

long HMMObservationSequence_getNumberOfObservations (HMMObservationSequence me);

long HMMObservationSequenceBag_getLongestSequence (HMMObservationSequenceBag me);

autoTableOfReal HMMStateSequence_to_TableOfReal_transitions (HMMStateSequence me);

autoTableOfReal HMM_and_HMMStateSequence_to_TableOfReal_transitions (HMM me, HMMStateSequence thee, int probabilities);

autoHMMStateSequence HMMStateSequence_create (long numberOfItems);

autoStrings HMMStateSequence_to_Strings (HMMStateSequence me);

void HMMState_setLabel (HMMState me, char32 *label);


autoHMM HMM_create (int leftToRight, long numberOfStates, long numberOfObservationSymbols);

autoHMM HMM_createSimple (int leftToRight, const char32 *states_string, const char32 *symbols_string);

autoHMM HMM_createContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
	long numberOfMixtureComponentsPerSymbol, long componentDimension, long componentStorage);

autoHMM HMM_createFullContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
	long numberOfFeatureStreams, long *dimensionOfStream, long *numberOfGaussiansforStream);

autoHMM HMM_createFromHMMObservationSequence (HMMObservationSequence me, long numberOfStates, int leftToRight);

void HMM_draw (HMM me, Graphics g, int garnish);

void HMM_drawBackwardProbabilitiesIllustration (Graphics g, bool garnish);

void HMM_drawForwardProbabilitiesIllustration (Graphics g, bool garnish);

void HMM_drawForwardAndBackwardProbabilitiesIllustration (Graphics g, bool garnish);

void HMM_addObservation_move (HMM me, autoHMMObservation thee);

void HMM_addState_move (HMM me, autoHMMState thee);

void HMM_setDefaultStates (HMM me);

void HMM_setDefaultObservations (HMM me);

void HMM_setDefaultTransitionProbs (HMM me);

void HMM_setDefaultStartProbs (HMM me);

void HMM_setDefaultEmissionProbs (HMM me);

void HMM_setDefaultMixingProbabilities (HMM me);

void HMM_unExpandPCA (HMM me);
/* Unexpand all the PCA's in the gaussian mixtures */



/*
	Set the probabilities. A probability zero value indicates that this p cannot be changed during training/learning.
*/
void HMM_setTransitionProbabilities (HMM me, long state_number, char32 *state_probs);

void HMM_setEmissionProbabilities (HMM me, long state_number, char32 *emission_probs);

void HMM_setStartProbabilities (HMM me, char32 *probs);


double HMM_getProbabilityAtTimeBeingInState (HMM me, long itime, long istate);

double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, long itime, long istate, long isymbol);

double HMM_getExpectedValueOfDurationInState (HMM me, long istate);

double HMM_getProbabilityOfStayingInState (HMM me, long istate, long numberOfTimeUnits);

double HMM_and_HMM_getCrossEntropy (HMM me, HMM thee, long observationLength, int symmetric);

double HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy (HMM me, HMM thee, HMMObservationSequence him);

autoTableOfReal HMM_extractTransitionProbabilities (HMM me);

autoTableOfReal HMM_extractEmissionProbabilities (HMM me);

/* HMM & HMMObservationSequence ****/

autoHMMObservationSequence HMM_to_HMMObservationSequence (HMM me, long initialState, long numberOfItems);

autoHMMStateSequence HMM_and_HMMObservationSequence_to_HMMStateSequence (HMM me, HMMObservationSequence thee);

double HMM_and_HMMStateSequence_getProbability (HMM me, HMMStateSequence thee);

void HMM_and_HMMObservationSequenceBag_learn (HMM me, HMMObservationSequenceBag thee, double delta_lnp, double minProb, int info);

void HMM_and_HMMStateSequence_drawTrellis (HMM me, HMMStateSequence thee, Graphics g, int connect, int garnish);

double HMM_and_HMMObservationSequence_getProbability (HMM me, HMMObservationSequence thee);

double HMM_and_HMMObservationSequence_getCrossEntropy (HMM me, HMMObservationSequence thee);

double HMM_and_HMMObservationSequence_getPerplexity (HMM me, HMMObservationSequence thee);

// somewhere else
void MelderInfo_lnp (double logp);
autoTableOfReal Strings_to_TableOfReal_transitions (Strings me, int probabilities);

#endif /* _HMM_h_ */
