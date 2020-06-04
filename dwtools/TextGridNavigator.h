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

#include "TextGridNavigator_enums.h"
#include "TextGridNavigator_def.h"

Thing_define (IntervalTierNavigationContext, TierNavigationContext) {
	
	integer v_getSize (IntervalTier me) {
		return my intervals.size;
	}
	integer v_getIndexFromTime (IntervalTier me, double t) {
		return IntervalTier_timeToLowIndex (me, t);
	}
	double v_getLeftTime (IntervalTier me, integer index) {
		TextInterval interval = my intervals . at [index];
		return interval -> xmin;
	}
	double v_getRightTime (IntervalTier me, integer index) {
		TextInterval interval = my intervals . at [index];
		return interval -> xmax;
	}
	conststring32 v_getLabel (IntervalTier me, integer index) {
		TextInterval interval = my intervals . at [index];
		return interval -> text.get();
	}
};

Thing_define (TextTierNavigationContext, TierNavigationContext) {
	
	integer v_getSize (TextTier me) {
		return my points.size;
	}
	integer v_getIndexFromTime (TextTier me, double t) {
		return AnyTier_timeToNearestIndex (me->asAnyTier(), t);
	}
	double v_getLeftTime (TextTier me, integer index) {
		TextPoint point = my points . at [index];
		return point -> number;
	}
	double v_getRightTime (TextTier me, integer index) {
		TextPoint point = my points . at [index];
		return point -> number;;
	}
	conststring32 v_getLabel (TextTier me, integer index) {
		TextPoint point = my points . at [index];
		return point -> mark.get();
	}
};

autoIntervalTierNavigationContext IntervalTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber);

autoTextTierNavigationContext TextTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber);

autoTextGridNavigator TextGridNavigator_create (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber);

void TextGridNavigator_addNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber, kNavigatableTier_match matchCriterion);

void TextGridNavigator_setMatchingRange (TextGridNavigator me, integer tierNumber, integer maximumLookAhead, integer maximumLookBack);

void TextGridNavigator_setLeftAndRightContextRange (TextGridNavigator me, integer tierNumber, integer leftContextFrom, integer leftContextTo, integer rightContextFrom, integer rightContextTo);

void TextGridNavigator_setNavigationContextCriterions (TextGridNavigator me, integer tierNumber, kContext_combination combinationCriterion, bool matchContextOnly, kNavigatableTier_match matchCriterion);

void TextGridNavigator_resetTextGrid (TextGridNavigator me, TextGrid thee);

void TextGridNavigator_setCurrent (TextGridNavigator me, double time);

bool TextGridNavigator_isLabelMatch (TextGridNavigator me, integer indexInNavigationTier);

integer TextGridNavigator_getNumberOfMatchesInAContext (TextGridNavigator me, integer icontext);

integer TextGridNavigator_getNumberOfMatches (TextGridNavigator me);

#endif /* _TextGridNavigator_h_ */
