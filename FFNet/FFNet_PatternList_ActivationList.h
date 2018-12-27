#ifndef _FFNet_PatternList_ActivationList_h_
#define _FFNet_PatternList_ActivationList_h_
/* FFNet_PatternList_ActivationList.h
 *
 * Copyright (C) 1994-2017 David Weenink, 2015 Paul Boersma
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
 djmw 1994.
 djmw 20020712 GPL header.
 djmw 20030701 Removed non-GPL minimizations.
 djmw 20110714 Latest modification.
*/


#include "FFNet.h"
#include "PatternList.h"
#include "ActivationList.h"
#include "Minimizers.h"

void FFNet_PatternList_ActivationList_learnSD (FFNet me, PatternList p, ActivationList a, integer maxNumOfEpochs,
    double tolerance, double learningRate, double momentum, int costFunctionType);
/* Steepest Descent minimization */

void FFNet_PatternList_ActivationList_learnSM (FFNet me, PatternList p, ActivationList a, integer maxNumOfEpochs,
    double tolerance, int costFunctionType);

double FFNet_PatternList_ActivationList_getCosts_total (FFNet me, PatternList p, ActivationList a, int costFunctionType);
double FFNet_PatternList_ActivationList_getCosts_average (FFNet me, PatternList p, ActivationList a, int costFunctionType);

autoActivationList FFNet_PatternList_to_ActivationList (FFNet me, PatternList p, integer layer);
/* Calculate the activations at a layer */
/* if (layer<1 || layer > my numberOfLayers) layer = my numberOfLayers; */

#endif /* _FFNet_PatternList_ActivationList_h_ */
