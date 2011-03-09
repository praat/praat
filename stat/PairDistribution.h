#ifndef _PairDistribution_h_
#define _PairDistribution_h_
/* PairDistribution.h
 *
 * Copyright (C) 1997-2011 Paul Boersma
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
 * pb 2011/03/03
 */

#include "Strings.h"
#include "Table.h"
#include "Distributions.h"

#ifdef __cplusplus
	extern "C" {
#endif

#include "PairDistribution_def.h"

#define PairProbability_methods Data_methods
oo_CLASS_CREATE (PairProbability, Data);

#define PairDistribution_methods Data_methods
oo_CLASS_CREATE (PairDistribution, Data);

PairProbability PairProbability_create (const wchar_t *string1, const wchar_t *string2, double weight);
PairDistribution PairDistribution_create (void);
int PairDistribution_add (PairDistribution me, const wchar_t *string1, const wchar_t *string2, double weight);
void PairDistribution_removeZeroWeights (PairDistribution me);
int PairDistribution_to_Stringses (PairDistribution me, long nout, Strings *strings1, Strings *strings2);
int PairDistribution_peekPair (PairDistribution me, wchar_t **string1, wchar_t **string2);

double PairDistribution_getFractionCorrect_maximumLikelihood (PairDistribution me);
double PairDistribution_getFractionCorrect_probabilityMatching (PairDistribution me);
double PairDistribution_Distributions_getFractionCorrect (PairDistribution me, Distributions thee, long column);

int PairDistribution_formula (PairDistribution me, const wchar_t *formula);
void PairDistribution_drawAsNumbers (PairDistribution me, Graphics g, int iformat, int precision);
void PairDistribution_drawAsNumbers_if (PairDistribution me, Graphics g, int iformat, int precision, const wchar_t *conditionFormula);
PairDistribution PairDistribution_append (PairDistribution me, PairDistribution thee);

Table PairDistribution_to_Table (PairDistribution me);

#ifdef __cplusplus
	}
#endif

/* End of file PairDistribution.h */
#endif
