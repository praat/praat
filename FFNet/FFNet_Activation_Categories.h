#ifndef _FFNet_Activation_Categories_h_
#define _FFNet_Activation_Categories_h_
/* FFNet_Activation_Categories.h
 *
 * Copyright (C) 1997-2011, 2015 David Weenink
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
 djmw 19950109
 djmw 20020712 GPL header
 djmw 20110307 Latest modification
*/

#include "FFNet.h"
#include "Activation.h"
#include "Categories.h"

autoCategories FFNet_Activation_to_Categories (FFNet me, Activation activation, int labeling);
/* labeling = 1 : winner-takes-all */
/* labeling = 2 : stochastic */

autoActivation FFNet_Categories_to_Activation (FFNet me, Categories labels);
/* Postcondition: my outputCategories != nullptr; */

#endif /* _FFNet_Activation_Categories_h_ */
