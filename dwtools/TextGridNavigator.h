#ifndef _TextGridNavigator_h_
#define _TextGridNavigator_h_
/* TextGridNavigator.h
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

#include "NavigationContext.h"
#include "TextGrid.h"
#include "melder.h"

Thing_define (NavigatableIntervalTier, NavigatableTier) {
};

Thing_define (NavigatablePointTier, NavigatableTier) {
};

#include "TextGridNavigator_enums.h"

#include "TextGridNavigator_def.h"

autoTextGridNavigator TextGridNavigator_createEmpty ();



#endif /* _TextGridNavigator_h_ */
