#ifndef _FFNet_Pattern_Categories_h_
#define _FFNet_Pattern_Categories_h_
/* FFNet_Pattern_Categories.h
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
 djmw 19960821
 djmw 20020712 GPL header
 djmw 20110307 Latest mofification.
*/

#include "FFNet.h"
#include "Pattern.h"
#include "Categories.h"
#include "Minimizers.h"

#ifdef __cplusplus
	extern "C" {
#endif

void FFNet_Pattern_Categories_learnSD (FFNet me, Pattern p, Categories c, long maxNumOfEpochs,
    double tolerance, Any parameters, int costFunctionType);
/* Steepest descent */

void FFNet_Pattern_Categories_learnSM (FFNet me, Pattern p, Categories c, long maxNumOfEpochs,
    double tolerance, Any parameters, int costFunctionType);
/* Conj. Gradient vdSmagt */

double FFNet_Pattern_Categories_getCosts_total (FFNet me, Pattern p, Categories c, int costFunctionType);
double FFNet_Pattern_Categories_getCosts_average (FFNet me, Pattern p, Categories c, int costFunctionType);

Categories FFNet_Pattern_to_Categories (FFNet me, Pattern p, int labeling);
/* classify the Pattern */
/* labeling = 1 : winner-takes-all */
/* labeling = 2 : stochastic */
/* Preconditions: I have labels */


#ifdef __cplusplus
	}
#endif

#endif /* _FFNet_Pattern_Categories_h_ */
