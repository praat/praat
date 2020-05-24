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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TextGrid.h"
#include "TextGridView.h"
#include "melder.h"

Thing_define (TextGridNavigator, Function) {
	TextGrid textGrid;
	autoStrings navigationLabels;
	kMelder_string criterion;
	void v_info ()
		override;
};

autoTextGridNavigator TextGridNavigator_create (TextGrid me, Strings navigatorLabels, kMelder_string criterion);
autoTextGridNavigator TextGridNavigator_createEmpty (TextGrid me);

bool TextGridNavigator_hasLabelMatch (TextGridNavigator me, conststring32 label);

integer TextGridNavigator_nextNavigationIntervalNumber (TextGridNavigator me, double time, integer navigationTier);

integer TextGridNavigator_previousNavigationIntervalNumber (TextGridNavigator me, double time, integer navigationTier);

void TextGridNavigdNavigator_setNavigatorLabels (TextGridNavigator me, Strings navigatorLabels, kMelder_string criterion);

#endif /* _TextGridNavigator_h_ */
