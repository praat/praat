#ifndef _IntervalTierNavigator_h_
#define _IntervalTierNavigator_h_
/* IntervalTierNavigator.h
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

#include "IntervalTierNavigator_enums.h"

/*
	Behaviour: if no navigationLabels are defined then next/previous simply do currentIntervalNumber ++ / --.
	If navigationLabels are present the move is to the next/previous interval that matches the criterion.
	(offLeft) 0 <= currentIntervalNumber <=  intervals.size + 1 (offRight)
*/

Thing_define (IntervalTierNavigator, Function) {
	IntervalTier intervalTier;
	autoSTRVEC leftContextLabels;
	kMelder_string leftContextCriterion;
	autoSTRVEC rightContextLabels;
	kMelder_string rightContextCriterion;
	autoSTRVEC navigationLabels;
	integer currentIntervalNumber;
	kMelder_string criterion;
	kContextUse contextUse;
	void v_info ()
		override;
	bool offLeft () {
		return currentIntervalNumber == 0;
	}
	bool offRight () {
		return currentIntervalNumber == intervalTier -> intervals . size + 1;
	}
};

autoIntervalTierNavigator IntervalTierNavigator_createFromTextGrid (TextGrid me, integer navigationTier);

autoIntervalTierNavigator IntervalTierNavigator_create (IntervalTier me);

autoIntervalTierNavigator IntervalTierNavigator_createEmpty (IntervalTier me);

/*
	Especially for editors where the TextGrid(View) is not stable
*/
void IntervalTierNavigator_setBeginPosition (IntervalTierNavigator me, double time);

bool IntervalTierNavigator_isLabelMatch (IntervalTierNavigator me, integer intervalNumber);

integer IntervalTierNavigator_getNumberOfMatches (IntervalTierNavigator me);

integer IntervalTierNavigator_nextIntervalNumber (IntervalTierNavigator me);
TextInterval IntervalTierNavigator_nextInterval (IntervalTierNavigator me);

integer IntervalTierNavigator_previousIntervalNumber (IntervalTierNavigator me);
TextInterval IntervalTierNavigator_previousInterval (IntervalTierNavigator me);

void IntervalTierNavigator_setNavigationLabels (IntervalTierNavigator me, Strings navigationLabels, kMelder_string criterion);
void IntervalTierNavigator_setLeftContextLabels (IntervalTierNavigator me, Strings leftContextLabels, kMelder_string criterion);
void IntervalTierNavigator_setRightContextLabels (IntervalTierNavigator me, Strings rightContextLabels, kMelder_string criterion);
void IntervalTierNavigator_modifyContextUse (IntervalTierNavigator me, kContextUse contextUse);

#endif /* _IntervalTierNavigator_h_ */
