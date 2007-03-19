#ifndef _PairDistribution_h_
#define _PairDistribution_h_
/* PairDistribution.h
 *
 * Copyright (C) 1997-2007 Paul Boersma
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
 * pb 2007/03/18
 */

#ifndef _Strings_h_
	#include "Strings.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _Distributions_h_
	#include "Distributions.h"
#endif

#include "PairDistribution_def.h"

#define PairProbability_methods Data_methods
oo_CLASS_CREATE (PairProbability, Data)

#define PairDistribution_methods Data_methods
oo_CLASS_CREATE (PairDistribution, Data)

PairProbability PairProbability_create (const char *string1, const char *string2, double weight);
PairDistribution PairDistribution_create (void);
int PairDistribution_add (PairDistribution me, const char *string1, const char *string2, double weight);
void PairDistribution_removeZeroWeights (PairDistribution me);
int PairDistribution_to_Stringses (PairDistribution me, long nout, Strings *strings1, Strings *strings2);
int PairDistribution_peekPair (PairDistribution me, char **string1, char **string2);

double PairDistribution_getFractionCorrect_maximumLikelihood (PairDistribution me);
double PairDistribution_getFractionCorrect_probabilityMatching (PairDistribution me);
double PairDistribution_Distributions_getFractionCorrect (PairDistribution me, Distributions thee, long column);

int PairDistribution_formula (PairDistribution me, const char *formula);
void PairDistribution_drawAsNumbers (PairDistribution me, Graphics g, int iformat, int precision);
void PairDistribution_drawAsNumbers_if (PairDistribution me, Graphics g, int iformat, int precision, const char *conditionFormula);
PairDistribution PairDistribution_append (PairDistribution me, PairDistribution thee);

#endif

/* End of file PairDistribution.h */
