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

/*
	Navigates on one particular tier of a TextGrid (the navigation tier), depending on context.
	Suppose the navigation tier has intervals labeled with phonemes (e.g. a,i, \ct, o, f, p ...).
	Navigation then means that we want to visit only the interval where the phonemes belong to a subset
	of all the possible phonemes. 
	Context is either context on the same tier (i.e. left and/or right context), or,
	a relation between the navigation tier and subordinate tiers.
	



*/

Thing_define (IntervalTierNavigationContext, TierNavigationContext) {
	
	void v_info ()
			override;
			
	integer v_getSize (Function anyTier) {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		return my intervals.size;
	}
	
	integer v_getIndexFromTime (Function anyTier, double time) {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		integer index;
		if (time < my xmin)
			index = 0; // offLeft
		else if (time > my xmax)
			index = my intervals .size + 1; // offRight
		else
			index = IntervalTier_timeToLowIndex (me, time);
		return index;
	}
	
	double v_getLeftTime (Function anyTier, integer index) {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		TextInterval interval = my intervals . at [index];
		return interval -> xmin;
	}
		
	double v_getRightTime (Function anyTier, integer index) {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		TextInterval interval = my intervals . at [index];
		return interval -> xmax;
	}
		
	conststring32 v_getLabel (Function anyTier, integer index) {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		TextInterval interval = my intervals . at [index];
		return interval -> text.get();
	}
};

Thing_define (TextTierNavigationContext, TierNavigationContext) {
	
	void v_info ()
			override;
	
	integer v_getSize (Function anyTier) {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		return my points.size;
	}
	
	integer v_getIndexFromTime (Function anyTier, double time) {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		integer index;
		if (time < my xmin)
			index = 0; // offLeft
		else if (time > my xmax)
			index = my points .size + 1; // offRight
		else
			index = AnyTier_timeToNearestIndex (me -> asAnyTier(), time);
		return index;
	}
	
	double v_getLeftTime (Function anyTier, integer index) {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		TextPoint point = my points . at [index];
		return point -> number;
	}
	
	double v_getRightTime (Function anyTier, integer index) {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		TextPoint point = my points . at [index];
		return point -> number;;
	}
	
	conststring32 v_getLabel (Function anyTier, integer index) {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		TextPoint point = my points . at [index];
		return point -> mark.get();
	}
};

autoIntervalTierNavigationContext IntervalTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber);

autoTextTierNavigationContext TextTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber);

autoTextGridNavigator TextGridNavigator_create (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber);

void TextGridNavigator_addNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber, kNavigatableTier_match matchCriterion);

void TextGridNavigator_modifyMatchingRange (TextGridNavigator me, integer tierNumber, integer maximumLookAhead, integer maximumLookBack);

void TextGridNavigator_modifyLeftAndRightContextRange (TextGridNavigator me, integer tierNumber, integer leftContextFrom, integer leftContextTo, integer rightContextFrom, integer rightContextTo);

void TextGridNavigator_modifyNavigationContextCriterions (TextGridNavigator me, integer tierNumber, kContext_combination combinationCriterion, bool matchContextOnly, kNavigatableTier_match matchCriterion);

void TextGridNavigator_replaceTextGrid (TextGridNavigator me, TextGrid thee);

bool TextGridNavigator_isLabelMatch (TextGridNavigator me, integer indexInNavigationTier);

integer TextGridNavigator_getNumberOfMatchesInAContext (TextGridNavigator me, integer icontext);

integer TextGridNavigator_getNumberOfMatches (TextGridNavigator me);

integer TextGridNavigator_getTierNumberFromContextNumber (TextGridNavigator me, integer contextNumber);
integer TextGridNavigator_getContextNumberFromTierNumber (TextGridNavigator me, integer tierNumber);

integer TextGridNavigator_getCurrentFromTime (TextGridNavigator me, double time);

integer TextGridNavigator_next (TextGridNavigator me);

integer TextGridNavigator_nextFromTime (TextGridNavigator me, double time);

integer TextGridNavigator_previous (TextGridNavigator me);

integer TextGridNavigator_previousFromTime (TextGridNavigator me, double time);


#endif /* _TextGridNavigator_h_ */
