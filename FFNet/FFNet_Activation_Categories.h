#ifndef _FFNet_Activation_Categories_h_
#define _FFNet_Activation_Categories_h_
/* FFNet_Activation_Categories.h
 *
 * Copyright (C) 1997-2011 David Weenink
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
 djmw 19950109
 djmw 20020712 GPL header
 djmw 20110307 Latest modification
*/

#include "FFNet.h"
#include "Activation.h"
#include "Categories.h"

#ifdef __cplusplus
	extern "C" {
#endif

Categories FFNet_Activation_to_Categories (FFNet me, Activation activation, int labeling);
/* labeling = 1 : winner-takes-all */
/* labeling = 2 : stochastic */

Activation FFNet_Categories_to_Activation (FFNet me, Categories labels);
/* Postcondition: my outputCategories != NULL; */

#ifdef __cplusplus
	}
#endif

#endif /* _FFNet_Activation_Categories_h_ */
