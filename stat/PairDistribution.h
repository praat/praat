#ifndef _PairDistribution_h_
#define _PairDistribution_h_
/* PairDistribution.h
 *
 * Copyright (C) 1997-2011,2015,2017 Paul Boersma
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
#include "Table.h"
#include "Distributions.h"

#include "PairDistribution_def.h"

autoPairProbability PairProbability_create (conststring32 string1, conststring32 string2, double weight);

int PairProbability_compare (PairProbability me, PairProbability thee) noexcept;

autoPairDistribution PairDistribution_create ();

conststring32 PairDistribution_getString1 (PairDistribution me, integer pairNumber);
conststring32 PairDistribution_getString2 (PairDistribution me, integer pairNumber);
double PairDistribution_getWeight (PairDistribution me, integer pairNumber);

void PairDistribution_add (PairDistribution me, conststring32 string1, conststring32 string2, double weight);
void PairDistribution_removeZeroWeights (PairDistribution me);
void PairDistribution_to_Stringses (PairDistribution me, integer nout, autoStrings *strings1, autoStrings *strings2);
void PairDistribution_peekPair (PairDistribution me, conststring32 *out_string1, conststring32 *out_string2);

void PairDistribution_swapInputsAndOutputs (PairDistribution me);

double PairDistribution_getFractionCorrect_maximumLikelihood (PairDistribution me);
double PairDistribution_getFractionCorrect_probabilityMatching (PairDistribution me);
double PairDistribution_Distributions_getFractionCorrect (PairDistribution me, Distributions thee, integer column);

autoTable PairDistribution_to_Table (PairDistribution me);

/* End of file PairDistribution.h */
#endif
