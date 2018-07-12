#ifndef _OTMulti_h_
#define _OTMulti_h_
/* OTMulti.h
 *
 * Copyright (C) 2005-2009,2011,2012,2014-2018 Paul Boersma
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

#include "Graphics.h"
#include "PairDistribution.h"
#include "Distributions.h"
#include "OTGrammar.h"

#include "OTMulti_def.h"

integer OTMulti_getConstraintIndexFromName (OTMulti me, conststring32 name);

void OTMulti_checkIndex (OTMulti me);

void OTMulti_sort (OTMulti me);
/* Low level: meant to maintain the invariant
 *      my constraints [my index [i]]. disharmony >= my constraints [my index [i+1]]. disharmony
 * Therefore, call after every direct assignment to the 'disharmony' attribute.
 * Tied constraints should not exist.
 */

void OTMulti_newDisharmonies (OTMulti me, double evaluationNoise);

int OTMulti_candidateMatches (OTMulti me, integer icand, conststring32 form1, conststring32 form2);
int OTMulti_compareCandidates (OTMulti me, integer icand1, integer icand2);
integer OTMulti_getWinner (OTMulti me, conststring32 form1, conststring32 form2);

#define OTMulti_LEARN_FORWARD  1
#define OTMulti_LEARN_BACKWARD  2
#define OTMulti_LEARN_BIDIRECTIONALLY  3
int OTMulti_learnOne (OTMulti me, conststring32 form1, conststring32 form2,
	enum kOTGrammar_rerankingStrategy updateRule, int direction, double plasticity, double relativePlasticityNoise);
void OTMulti_PairDistribution_learn (OTMulti me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int direction,
	double initialPlasticity, integer replicationsPerPlasticity, double plasticityDecrement,
	integer numberOfPlasticities, double relativePlasticityNoise, integer storeHistoryEvery, autoTable *history_out);

void OTMulti_drawTableau (OTMulti me, Graphics g, conststring32 form1, conststring32 form2, bool vertical, bool showDisharmonies);

autoOTMulti OTMulti_create_metrics (
	kOTGrammar_createMetricsGrammar_initialRanking equal_footForm_wsp,
	int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress,
	int includeClashAndLapse, int includeCodas);

void OTMulti_reset (OTMulti me, double ranking);
void OTMulti_setRanking (OTMulti me, integer constraint, double ranking, double disharmony);
void OTMulti_setConstraintPlasticity (OTMulti me, integer constraint, double plasticity);
void OTMulti_removeConstraint (OTMulti me, conststring32 constraintName);

autostring32 OTMulti_generateOptimalForm (OTMulti me, conststring32 form1, conststring32 form2, double evaluationNoise);
autoStrings OTMulti_generateOptimalForms (OTMulti me, conststring32 form1, conststring32 form2, integer numberOfTrials, double evaluationNoise);
autoDistributions OTMulti_to_Distribution (OTMulti me, conststring32 form1, conststring32 form2, integer numberOfTrials, double evaluationNoise);
autoStrings OTMulti_Strings_generateOptimalForms (OTMulti me, Strings forms, double evaluationNoise);

/* End of file OTMulti.h */
#endif
