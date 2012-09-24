#ifndef _OTMulti_h_
#define _OTMulti_h_
/* OTMulti.h
 *
 * Copyright (C) 2005-2011 Paul Boersma
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

#include "Graphics.h"
#include "PairDistribution.h"
#include "Distributions.h"
#include "OTGrammar.h"

#include "OTMulti_def.h"
oo_CLASS_CREATE (OTMulti, Data);

long OTMulti_getConstraintIndexFromName (OTMulti me, const wchar_t *name);

void OTMulti_checkIndex (OTMulti me);

void OTMulti_sort (OTMulti me);
/* Low level: meant to maintain the invariant
 *      my constraints [my index [i]]. disharmony >= my constraints [my index [i+1]]. disharmony
 * Therefore, call after every direct assignment to the 'disharmony' attribute.
 * Tied constraints should not exist.
 */

void OTMulti_newDisharmonies (OTMulti me, double evaluationNoise);

int OTMulti_candidateMatches (OTMulti me, long icand, const wchar_t *form1, const wchar_t *form2);
int OTMulti_compareCandidates (OTMulti me, long icand1, long icand2);
long OTMulti_getWinner (OTMulti me, const wchar_t *form1, const wchar_t *form2);

#define OTMulti_LEARN_FORWARD  1
#define OTMulti_LEARN_BACKWARD  2
#define OTMulti_LEARN_BIDIRECTIONALLY  3
int OTMulti_learnOne (OTMulti me, const wchar_t *form1, const wchar_t *form2,
	enum kOTGrammar_rerankingStrategy updateRule, int direction, double plasticity, double relativePlasticityNoise);
void OTMulti_PairDistribution_learn (OTMulti me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int direction,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long storeHistoryEvery, Table *history_out);

void OTMulti_drawTableau (OTMulti me, Graphics g, const wchar_t *form1, const wchar_t *form2, bool vertical, bool showDisharmonies);

void OTMulti_reset (OTMulti me, double ranking);
void OTMulti_setRanking (OTMulti me, long constraint, double ranking, double disharmony);
void OTMulti_setConstraintPlasticity (OTMulti me, long constraint, double plasticity);
void OTMulti_removeConstraint (OTMulti me, const wchar_t *constraintName);

void OTMulti_generateOptimalForm (OTMulti me, const wchar_t *form1, const wchar_t *form2, wchar_t *optimalForm, double evaluationNoise);
Strings OTMulti_generateOptimalForms (OTMulti me, const wchar_t *form1, const wchar_t *form2, long numberOfTrials, double evaluationNoise);
Distributions OTMulti_to_Distribution (OTMulti me, const wchar_t *form1, const wchar_t *form2, long numberOfTrials, double evaluationNoise);
Strings OTMulti_Strings_generateOptimalForms (OTMulti me, Strings forms, double evaluationNoise);

/* End of file OTMulti.h */
#endif
