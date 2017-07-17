#ifndef _OTGrammar_h_
#define _OTGrammar_h_
/* OTGrammar.h
 *
 * Copyright (C) 1997-2011,2014,2015,2016 Paul Boersma
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

long OTGrammar_getTableau (OTGrammar me, const char32 *input);
int OTGrammar_compareCandidates (OTGrammar me, long itab1, long icand1, long itab2, long icand2) noexcept;
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
long OTGrammar_getWinner (OTGrammar me, long itab) noexcept;
	/* Gives randomized results in case of tied candidates. */
long OTGrammar_getNumberOfOptimalCandidates (OTGrammar me, long itab);
bool OTGrammar_isCandidateGrammatical (OTGrammar me, long itab, long icand);
bool OTGrammar_isCandidateSinglyGrammatical (OTGrammar me, long itab, long icand);

/* Interpretive parsing. */
void OTGrammar_getInterpretiveParse
	(OTGrammar me, const char32 *partialOutput, long *bestTableau, long *bestCandidate);
	/* Gives randomized results in case of tied candidates. */
bool OTGrammar_isPartialOutputGrammatical (OTGrammar me, const char32 *partialOutput);
	/* Is there an input for which this partial output is contained in any of the optimal outputs? */
bool OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, const char32 *partialOutput);
	/* Is every optimal output that contains this partial output the only optimal output in its tableau? */
bool OTGrammar_areAllPartialOutputsGrammatical (OTGrammar me, Strings thee);
bool OTGrammar_areAllPartialOutputsSinglyGrammatical (OTGrammar me, Strings thee);

void OTGrammar_drawTableau (OTGrammar me, Graphics g, bool vertical, const char32 *input);

autoStrings OTGrammar_generateInputs (OTGrammar me, long numberOfTrials);
autoStrings OTGrammar_getInputs (OTGrammar me);
void OTGrammar_inputToOutput (OTGrammar me, const char32 *input, char32 *output, double evaluationNoise);
autoStrings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double evaluationNoise);
autoStrings OTGrammar_inputToOutputs (OTGrammar me, const char32 *input, long n, double evaluationNoise);
autoDistributions OTGrammar_to_Distribution (OTGrammar me, long trialsPerInput, double evaluationNoise);
autoPairDistribution OTGrammar_to_PairDistribution (OTGrammar me, long trialsPerInput, double evaluationNoise);
autoDistributions OTGrammar_measureTypology_WEAK (OTGrammar me);   // WEAK because it has a progress bar

void OTGrammar_learnOne (OTGrammar me, const char32 *input, const char32 *adultOutput,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, bool newDisharmonies,
	bool warnIfStalled, bool *grammarHasChanged);
void OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, long numberOfChews);
void OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews);
bool OTGrammar_PairDistribution_findPositiveWeights_e (OTGrammar me, PairDistribution thee, double weightFloor, double marginOfSeparation);
void OTGrammar_learnOneFromPartialOutput (OTGrammar me, const char32 *partialAdultOutput,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, long numberOfChews, bool warnIfStalled);
void OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double rankingSpreading, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, long numberOfChews, long storeHistoryEvery, autoOTHistory *history);
void OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, autoOTHistory *history_out,
	bool resampleForVirtualProduction, bool compareOnlyPartialOutput, long resampleForCorrectForm);
double OTGrammar_PairDistribution_getFractionCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfInputs);
long OTGrammar_PairDistribution_getMinimumNumberCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfReplications);
double OTGrammar_Distributions_getFractionCorrect (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, long numberOfInputs);

void OTGrammar_checkIndex (OTGrammar me);

autoOTGrammar OTGrammar_create_NoCoda_grammar ();
autoOTGrammar OTGrammar_create_NPA_grammar ();
autoPairDistribution OTGrammar_create_NPA_distribution ();
autoOTGrammar OTGrammar_create_tongueRoot_grammar (int small_large, int equal_random_infant_Wolof);
autoOTGrammar OTGrammar_create_metrics (int equal_footForm_wsp,
	int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress, int includeClashAndLapse, int includeCodas);
	/* T&S: 1, false, false, false, 1, true, false, false */

void OTGrammar_reset (OTGrammar me, double ranking);
void OTGrammar_resetToRandomRanking (OTGrammar me, double mean, double standardDeviation);
void OTGrammar_resetToRandomTotalRanking (OTGrammar me, double maximumRanking, double rankingDistance);
void OTGrammar_setRanking (OTGrammar me, long constraint, double ranking, double disharmony);
void OTGrammar_setConstraintPlasticity (OTGrammar me, long constraint, double plasticity);

void OTGrammar_removeConstraint (OTGrammar me, const char32 *constraintName);
void OTGrammar_removeHarmonicallyBoundedCandidates (OTGrammar me, bool singly);
void OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee);
void OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, long columnNumber);

void OTGrammar_writeToHeaderlessSpreadsheetFile (OTGrammar me, MelderFile file);

/* End of file OTGrammar.h */
#endif
