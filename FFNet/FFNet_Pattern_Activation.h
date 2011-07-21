#ifndef _FFNet_Pattern_Activation_h_
#define _FFNet_Pattern_Activation_h_
/* FFNet_Pattern_Activation.h
 *
 * Copyright (C) 1994-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 1994.
 djmw 20020712 GPL header.
 djmw 20030701 Removed non-GPL minimizations.
 djmw 20110714 Latest modification.
*/


#include "FFNet.h"
#include "Pattern.h"
#include "Activation.h"
#include "Minimizers.h"

#ifdef __cplusplus
	extern "C" {
#endif

void FFNet_Pattern_Activation_learnSD (FFNet me, Pattern p, Activation a, long maxNumOfEpochs,
    double tolerance, Any parameters, int costFunctionType);
/* Steepest Descent minimization */

void FFNet_Pattern_Activation_learnSM (FFNet me, Pattern p, Activation a, long maxNumOfEpochs,
    double tolerance, Any parameters, int costFunctionType);

double FFNet_Pattern_Activation_getCosts_total (FFNet me, Pattern p, Activation a, int costFunctionType);
double FFNet_Pattern_Activation_getCosts_average (FFNet me, Pattern p, Activation a, int costFunctionType);

Activation FFNet_Pattern_to_Activation (FFNet me, Pattern p, long layer);
/* Calculate the activations at a layer */
/* if (layer<1 || layer > my nLayers) layer = my nLayers; */

#ifdef __cplusplus
	}
#endif

#endif /* _FFNet_Pattern_Activation_h_ */
