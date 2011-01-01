#ifndef _HMM_h_
#define _HMM_h_
/* HMM.h
 *
 * Copyright (C) 2010 David Weenink
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
#ifndef _Strings_extensions_h_
	#include "Strings_extensions.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif
#ifndef _Collection_h_
 #include "Collection.h"
#endif
#ifndef _GaussianMixture_h_
 #include "GaussianMixture.h"
#endif
#ifndef _SSCP_h_
 #include "SSCP.h"
#endif
#ifndef _Index_h_
 #include "Index.h"
#endif

#include "HMM_def.h"

#define HMM_State_methods Data_methods
oo_CLASS_CREATE (HMM_State, Data);

#define HMM_Observation_methods Data_methods
oo_CLASS_CREATE (HMM_Observation, Data);

#define HMM_Viterbi_methods Data_methods
oo_CLASS_CREATE (HMM_Viterbi, Data);

#define HMM_methods Data_methods
oo_CLASS_CREATE (HMM, Data);

#define HMM_BaumWelch_members Data_members \
	long capacity; \
	long numberOfTimes; \
	long totalNumberOfSequences; \
	long numberOfStates; \
	long numberOfSymbols; \
	double lnProb; \
	double minProb; \
	double **alpha; \
	double **beta; \
	double *scale; \
	double **gamma; \
	double ***xi; \
	double **aij_num, **aij_denom; \
	double **bik_num, **bik_denom;
#define HMM_BaumWelch_methods Data_methods
class_create (HMM_BaumWelch, Data);

/* the definition of String_members is essential */
#define Strings_members Data_members \
	long numberOfStrings; \
	wchar_t **strings;
#define HMM_StateSequence_members Strings_members
#define HMM_StateSequence_methods Strings_methods
class_create (HMM_StateSequence, Strings);

#define HMM_ObservationSequence_members Table_members
#define HMM_ObservationSequence_methods Table_methods
class_create (HMM_ObservationSequence, Table);
// First column is always a symbol, if only 1 column then symbols only

#define HMM_ObservationSequences_members Collection_members
#define HMM_ObservationSequences_methods Collection_methods
class_create (HMM_ObservationSequences, Collection);

HMM_ObservationSequence HMM_ObservationSequence_create (long numberOfItems, long dataLength);
void HMM_ObservationSequence_removeObservation (HMM_ObservationSequence me, long index);
Strings HMM_ObservationSequence_to_Strings (HMM_ObservationSequence me);
HMM_ObservationSequence Strings_to_HMM_ObservationSequence (Strings me);
StringsIndex HMM_ObservationSequence_to_StringsIndex (HMM_ObservationSequence me);
StringsIndex HMM_and_HMM_ObservationSequence_to_StringsIndex (HMM me, HMM_ObservationSequence thee);
TableOfReal HMM_ObservationSequence_to_TableOfReal_transitions (HMM_ObservationSequence me, int probabilities);
TableOfReal HMM_and_HMM_ObservationSequence_to_TableOfReal_transitions (HMM me, HMM_ObservationSequence thee, int probabilities);

HMM_ObservationSequences HMM_ObservationSequences_create (void);
long HMM_ObservationSequence_getNumberOfObservations (HMM_ObservationSequence me);
long HMM_ObservationSequences_getLongestSequence (HMM_ObservationSequences me);

TableOfReal HMM_StateSequence_to_TableOfReal_transitions (HMM_StateSequence me);
TableOfReal HMM_and_HMM_StateSequence_to_TableOfReal_transitions (HMM me, HMM_StateSequence thee, int probabilities);
HMM_StateSequence HMM_StateSequence_create (long numberOfItems);
Strings HMM_StateSequence_to_Strings (HMM_StateSequence me);

void HMM_State_setLabel (HMM_State me, wchar_t *label);


HMM HMM_create (int leftToRight, long numberOfStates, long numberOfObservationSymbols);
HMM HMM_createSimple (int leftToRight, wchar_t *states_string, wchar_t *symbols_string);
HMM HMM_createContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
	long numberOfMixtureComponentsPerSymbol, long componentDimension, long componentStorage);
HMM HMM_createFullContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
	long numberOfFeatureStreams, long *dimensionOfStream, long *numberOfGaussiansforStream);
HMM HMM_createFromHMM_ObservationSequence (HMM_ObservationSequence me, long numberOfStates, int leftToRight);
void HMM_draw (HMM me, Graphics g, int garnish);
int HMM_addObservation (HMM me, thou);
int HMM_addState (HMM me, thou);
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
int HMM_setTransitionProbabilities (HMM me, long state_number, wchar_t *state_probs);
int HMM_setEmissionProbabilities (HMM me, long state_number, wchar_t *emission_probs);
int HMM_setStartProbabilities (HMM me, wchar_t *probs);

double HMM_getProbabilityAtTimeBeingInState (HMM me, long itime, long istate);
double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, long itime, long istate, long isymbol);
double HMM_getExpectedValueOfDurationInState (HMM me, long istate);
double HMM_getProbabilityOfStayingInState (HMM me, long istate, long numberOfTimeUnits);

double HMM_and_HMM_getCrossEntropy (HMM me, HMM thee, long observationLength, int symmetric);
double HMM_and_HMM_and_HMM_ObservationSequence_getCrossEntropy (HMM me, HMM thee, HMM_ObservationSequence him);


TableOfReal HMM_extractTransitionProbabilities (HMM me);
TableOfReal HMM_extractEmissionProbabilities (HMM me);

/* HMM & HMM_ObservationSequence ****/

HMM_ObservationSequence HMM_to_HMM_ObservationSequence (HMM me, long initialState, long numberOfItems);
HMM_StateSequence HMM_and_HMM_ObservationSequence_to_HMM_StateSequence (HMM me, HMM_ObservationSequence thee);
double HMM_and_HMM_StateSequence_getProbability (HMM me, HMM_StateSequence thee);
int HMM_and_HMM_ObservationSequences_learn (HMM me, HMM_ObservationSequences thee, double delta_lnp, double minProb);
void HMM_and_HMM_StateSequence_drawTrellis (HMM me, HMM_StateSequence thee, Graphics g, int connect, int garnish);
double HMM_and_HMM_ObservationSequence_getProbability (HMM me, HMM_ObservationSequence thee);
double HMM_and_HMM_ObservationSequence_getCrossEntropy (HMM me, HMM_ObservationSequence thee);
double HMM_and_HMM_ObservationSequence_getPerplexity (HMM me, HMM_ObservationSequence thee);

// somewhere else
void MelderInfo_lnp (double logp);
TableOfReal Strings_to_TableOfReal_transitions (I, int probabilities);

#endif /* _HMM_h_ */
