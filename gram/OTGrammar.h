#ifndef _OTGrammar_h_
#define _OTGrammar_h_
/* OTGrammar.h
 *
 * Copyright (C) 1997-2012,2014-2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Strings_.h"
#include "Graphics.h"
#include "PairDistribution.h"
#include "Distributions.h"
#include "TableOfReal.h"

#include "OTGrammar_enums.h"

#include "OTGrammar_def.h"

Thing_define (OTHistory, TableOfReal) {
};

void OTGrammar_sort (OTGrammar me);
/* Low level: meant to maintain the invariant
 *      my constraints [my index [i]]. disharmony >= my constraints [my index [i+1]]. disharmony
 * Therefore, call after every direct assignment to the 'disharmony' attribute.
 * Tied constraints are sorted alphabetically.
 */

void OTGrammar_newDisharmonies (OTGrammar me, double spreading);

integer OTGrammar_getTableau (OTGrammar me, conststring32 input);
int OTGrammar_compareCandidates (OTGrammar me, integer itab1, integer icand1, integer itab2, integer icand2) noexcept;
	/*
	 * Function:
	 *    to compare the optimality of candidates icand1 in tableau itab1
	 *    and icand2 in tableau itab2.
	 * Return value:
	 *    -1 if candidate icand1 is more optimal than candidate icand2;
	 *    +1 if candidate icand2 is more optimal than candidate icand1;
	 *    0 if both candidates are equally optimal.
	 * Algorithm:
	 *    the current disharmony ranking is considered.
	 *    Takes into account crucial ties: if multiple constraints are ranked at
	 *    exactly the same height (disharmony), they count as a single constraint.
	 *    This is original OT. To get Anttila's and GLA's variationist interpretation
	 *    of ties instead (even in EDCD), make sure you introduce a bit of noise
	 *    (e.g. 1e-6 if the strata are 1.0 apart). That makes
	 *    the probability of ties almost zero.
	 */

/* Productive parsing. */
integer OTGrammar_getWinner (OTGrammar me, integer itab) noexcept;
	/* Gives randomized results in case of tied candidates. */
integer OTGrammar_getNumberOfOptimalCandidates (OTGrammar me, integer itab);
bool OTGrammar_isCandidateGrammatical (OTGrammar me, integer itab, integer icand);
bool OTGrammar_isCandidateSinglyGrammatical (OTGrammar me, integer itab, integer icand);

/* Interpretive parsing. */
void OTGrammar_getInterpretiveParse
	(OTGrammar me, conststring32 partialOutput, integer *bestTableau, integer *bestCandidate);
	/* Gives randomized results in case of tied candidates. */
bool OTGrammar_isPartialOutputGrammatical (OTGrammar me, conststring32 partialOutput);
	/* Is there an input for which this partial output is contained in any of the optimal outputs? */
bool OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, conststring32 partialOutput);
	/* Is every optimal output that contains this partial output the only optimal output in its tableau? */
bool OTGrammar_areAllPartialOutputsGrammatical (OTGrammar me, Strings thee);
bool OTGrammar_areAllPartialOutputsSinglyGrammatical (OTGrammar me, Strings thee);

void OTGrammar_drawTableau (OTGrammar me, Graphics g, bool vertical, conststring32 input);

autoStrings OTGrammar_generateInputs (OTGrammar me, integer numberOfTrials);
autoStrings OTGrammar_getInputs (OTGrammar me);
autostring32 OTGrammar_inputToOutput (OTGrammar me, conststring32 input, double evaluationNoise);
autoStrings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double evaluationNoise);
autoStrings OTGrammar_inputToOutputs (OTGrammar me, conststring32 input, integer n, double evaluationNoise);
autoDistributions OTGrammar_to_Distribution (OTGrammar me, integer trialsPerInput, double evaluationNoise);
autoPairDistribution OTGrammar_to_PairDistribution (OTGrammar me, integer trialsPerInput, double evaluationNoise);
autoDistributions OTGrammar_measureTypology_WEAK (OTGrammar me);   // WEAK because it has a progress bar

void OTGrammar_learnOne (OTGrammar me, conststring32 input, conststring32 adultOutput,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, bool newDisharmonies,
	bool warnIfStalled, bool *grammarHasChanged);
void OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, integer numberOfChews);
void OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, integer replicationsPerPlasticity, double plasticityDecrement,
	integer numberOfPlasticities, double relativePlasticityNoise, integer numberOfChews);
bool OTGrammar_PairDistribution_findPositiveWeights (OTGrammar me, PairDistribution thee, double weightFloor, double marginOfSeparation);
void OTGrammar_learnOneFromPartialOutput (OTGrammar me, conststring32 partialAdultOutput,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, integer numberOfChews, bool warnIfStalled);
void OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, integer numberOfChews, integer storeHistoryEvery, autoOTHistory *history);
void OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, integer columnNumber,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, integer replicationsPerPlasticity, double plasticityDecrement,
	integer numberOfPlasticities, double relativePlasticityNoise, integer numberOfChews,
	integer storeHistoryEvery, autoOTHistory *history_out,
	bool resampleForVirtualProduction, bool compareOnlyPartialOutput, integer resampleForCorrectForm);
double OTGrammar_PairDistribution_getFractionCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, integer numberOfInputs);
integer OTGrammar_PairDistribution_getMinimumNumberCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, integer numberOfReplications);
double OTGrammar_Distributions_getFractionCorrect (OTGrammar me, Distributions thee, integer columnNumber,
	double evaluationNoise, integer numberOfInputs);

void OTGrammar_checkIndex (OTGrammar me);

autoOTGrammar OTGrammar_create_NoCoda_grammar ();
autoOTGrammar OTGrammar_create_NPA_grammar ();
autoPairDistribution OTGrammar_create_NPA_distribution ();
autoOTGrammar OTGrammar_create_tongueRoot_grammar (
	kOTGrammar_createTongueRootGrammar_constraintSet small_large,
	kOTGrammar_createTongueRootGrammar_ranking equal_random_infant_Wolof
);
autoOTGrammar OTGrammar_create_metrics (
	kOTGrammar_createMetricsGrammar_initialRanking equal_footForm_wsp,
	int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress, int includeClashAndLapse, int includeCodas);
	/* T&S: 1, false, false, false, 1, true, false, false */

void OTGrammar_reset (OTGrammar me, double ranking);
void OTGrammar_resetToRandomRanking (OTGrammar me, double mean, double standardDeviation);
void OTGrammar_resetToRandomTotalRanking (OTGrammar me, double maximumRanking, double rankingDistance);
void OTGrammar_setRanking (OTGrammar me, integer constraint, double ranking, double disharmony);
void OTGrammar_setConstraintPlasticity (OTGrammar me, integer constraint, double plasticity);

void OTGrammar_removeConstraint (OTGrammar me, conststring32 constraintName);
void OTGrammar_removeHarmonicallyBoundedCandidates (OTGrammar me, bool singly);
void OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee);
void OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, integer columnNumber);

void OTGrammar_writeToHeaderlessSpreadsheetFile (OTGrammar me, MelderFile file);

/* End of file OTGrammar.h */
#endif
