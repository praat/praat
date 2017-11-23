#ifndef _FFNet_PatternList_Categories_h_
#define _FFNet_PatternList_Categories_h_
/* FFNet_PatternList_Categories.h
 *
 * Copyright (C) 1994-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 19960821
 djmw 20020712 GPL header
 djmw 20110307 Latest mofification.
*/

#include "FFNet.h"
#include "PatternList.h"
#include "Categories.h"
#include "Minimizers.h"

void FFNet_PatternList_Categories_learnSD (FFNet me, PatternList p, Categories c, integer maxNumOfEpochs,
    double tolerance, double learningRate, double momentum, int costFunctionType);
/* Steepest descent */

void FFNet_PatternList_Categories_learnSM (FFNet me, PatternList p, Categories c, integer maxNumOfEpochs,
    double tolerance, int costFunctionType);
/* Conj. Gradient vdSmagt */

double FFNet_PatternList_Categories_getCosts_total (FFNet me, PatternList p, Categories c, int costFunctionType);
double FFNet_PatternList_Categories_getCosts_average (FFNet me, PatternList p, Categories c, int costFunctionType);

autoCategories FFNet_PatternList_to_Categories (FFNet me, PatternList p, int labeling);
/* classify the PatternList */
/* labeling = 1 : winner-takes-all */
/* labeling = 2 : stochastic */
/* Preconditions: I have labels */

#endif /* _FFNet_PatternList_Categories_h_ */
