#ifndef _OTGrammar_h_
#define _OTGrammar_h_
/* OTGrammar.h
 *
 * Copyright (C) 1997-2008 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2008/03/31
 */

#ifndef _Strings_h_
	#include "Strings.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _PairDistribution_h_
	#include "PairDistribution.h"
#endif
#ifndef _Distributions_h_
	#include "Distributions.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

#include "OTGrammar_enums.h"

#include "OTGrammar_def.h"
#define OTGrammar_methods Data_methods
oo_CLASS_CREATE (OTGrammar, Data);

#define OTHistory_members  TableOfReal_members
#define OTHistory_methods  TableOfReal_methods
class_create (OTHistory, TableOfReal);

void OTGrammar_sort (OTGrammar me);
/* Low level: meant to maintain the invariant
 *      my constraints [my index [i]]. disharmony >= my constraints [my index [i+1]]. disharmony
 * Therefore, call after every direct assignment to the 'disharmony' attribute.
 * Tied constraints are sorted alphabetically.
 */

void OTGrammar_newDisharmonies (OTGrammar me, double spreading);

long OTGrammar_getTableau (OTGrammar me, const wchar_t *input);
int OTGrammar_compareCandidates (OTGrammar me, long itab1, long icand1, long itab2, long icand2);
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
long OTGrammar_getWinner (OTGrammar me, long itab);
	/* Gives randomized results in case of tied candidates. */
long OTGrammar_getNumberOfOptimalCandidates (OTGrammar me, long itab);
bool OTGrammar_isCandidateGrammatical (OTGrammar me, long itab, long icand);
bool OTGrammar_isCandidateSinglyGrammatical (OTGrammar me, long itab, long icand);

/* Interpretive parsing. */
int OTGrammar_getInterpretiveParse
	(OTGrammar me, const wchar_t *partialOutput, long *bestTableau, long *bestCandidate);
	/* Gives randomized results in case of tied candidates. */
bool OTGrammar_isPartialOutputGrammatical (OTGrammar me, const wchar_t *partialOutput);
	/* Is there an input for which this partial output is contained in any of the optimal outputs? */
bool OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, const wchar_t *partialOutput);
	/* Is every optimal output that contains this partial output the only optimal output in its tableau? */

void OTGrammar_drawTableau (OTGrammar me, Graphics g, const wchar_t *input);

Strings OTGrammar_generateInputs (OTGrammar me, long numberOfTrials);
Strings OTGrammar_getInputs (OTGrammar me);
int OTGrammar_inputToOutput (OTGrammar me, const wchar_t *input, wchar_t *output, double evaluationNoise);
Strings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double evaluationNoise);
Strings OTGrammar_inputToOutputs (OTGrammar me, const wchar_t *input, long n, double evaluationNoise);
Distributions OTGrammar_to_Distribution (OTGrammar me, long trialsPerInput, double evaluationNoise);
PairDistribution OTGrammar_to_PairDistribution (OTGrammar me, long trialsPerInput, double evaluationNoise);
Distributions OTGrammar_measureTypology (OTGrammar me);

#define OTGrammar_DEMOTION_ONLY  0
#define OTGrammar_SYMMETRIC_ONE  1
#define OTGrammar_SYMMETRIC_ALL  2
#define OTGrammar_WEIGHTED_UNCANCELLED  3
#define OTGrammar_WEIGHTED_ALL  4
#define OTGrammar_EDCD  5
#define OTGrammar_EDCD_WITH_VACATION  6
#define OTGrammar_DEMOTE_ONE_WITH_VACATION  7
#define OTGrammar_ALL_UP_ONE_DOWN  8

int OTGrammar_learnOne (OTGrammar me, const wchar_t *input, const wchar_t *adultOutput,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, int newDisharmonies,
	int warnIfStalled, int *grammarHasChanged);
int OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, long numberOfChews);
int OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, int strategy, int honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews);
bool OTGrammar_PairDistribution_findPositiveWeights_e (OTGrammar me, PairDistribution thee, double weightFloor, double marginOfSeparation);
int OTGrammar_learnOneFromPartialOutput (OTGrammar me, const wchar_t *partialAdultOutput,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, long numberOfChews, int warnIfStalled);
int OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double demotionMean, double relativeDemotionSpreading, long numberOfChews, long storeHistoryEvery, OTHistory *history);
int OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, int strategy, int honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, OTHistory *history_out);
int OTGrammar_PairDistribution_getFractionCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfInputs, double *fractionCorrect);
int OTGrammar_Distributions_getFractionCorrect (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, long numberOfInputs, double *fractionCorrect);

void OTGrammar_checkIndex (OTGrammar me);

OTGrammar OTGrammar_create_NoCoda_grammar (void);
OTGrammar OTGrammar_create_NPA_grammar (void);
PairDistribution OTGrammar_create_NPA_distribution (void);
OTGrammar OTGrammar_create_tongueRoot_grammar (int small_large, int equal_random_infant_Wolof);
OTGrammar OTGrammar_create_metrics (int equal_footForm_wsp,
	int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress, int includeClashAndLapse, int includeCodas);
	/* T&S: 1, FALSE, FALSE, FALSE, 1, TRUE, FALSE, FALSE */

void OTGrammar_reset (OTGrammar me, double ranking);
void OTGrammar_resetToRandomTotalRanking (OTGrammar me, double maximumRanking, double rankingDistance);
int OTGrammar_setRanking (OTGrammar me, long constraint, double ranking, double disharmony);
int OTGrammar_setConstraintPlasticity (OTGrammar me, long constraint, double plasticity);

int OTGrammar_removeConstraint (OTGrammar me, const wchar_t *constraintName);
int OTGrammar_removeHarmonicallyBoundedCandidates (OTGrammar me, int singly);
int OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee);
int OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, long columnNumber);

int OTGrammar_writeToHeaderlessSpreadsheetFile (OTGrammar me, MelderFile file);

/* End of file OTGrammar.h */
#endif
