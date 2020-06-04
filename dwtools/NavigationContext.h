#ifndef _NavigationContext_h_
#define _NavigationContext_h_
/* NavigationContext.h
 *
 * Copyright (C) 2020 David Weenink
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
#include "TextGrid.h"
#include "melder.h"

#include "NavigationContext_enums.h"

#include "NavigationContext_def.h"

autoNavigationContext NavigationContext_createDefault ();

void NavigationContext_setNavigationLabels (NavigationContext me, Strings labels, kMelder_string criterion);

void NavigationContext_setLeftContextLabels (NavigationContext me, Strings labels, kMelder_string criterion);

void NavigationContext_setRightContextLabels (NavigationContext me, Strings labels, kMelder_string criterion);


bool NavigationContext_isNavigationLabel (NavigationContext me, conststring32 label);

bool NavigationContext_isLeftContextLabel (NavigationContext me, conststring32 label);

bool NavigationContext_isRightContextLabel (NavigationContext me, conststring32 label);

void NavigationContext_setNavigationContextCombination (NavigationContext me, kContext_combination combinationCriterion, bool matchContextOnly);


#endif /* _NavigationContext_h_ */
