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
	The purpose of a TextGridNavigator is to find successive elements that match a criterion on one selected tier, the navigation tier.
	The criterion depends on "navigation contexts".
	Each navigation context handles only one particular tier. By combining different navigation contexts we 
	can construct searches that extend over multiple tiers. The simplest navigation context consists of only a single label that
	has to be matched.
	A somewhat more involved context has, instead of a single label, a set of labels where one of the set has to be matched. 
	The most extensive navigation context involves right and a left context sets of labels, that have to be matched too.
	
	As an example consider a TextGrid with a tier whose intervals have been labeled with IPA symbols like /a, i, \ct, o, f, p etc/.
	A very simple navigation context that consists of the navigiation set with IPA vowel symbols (u, i, a), a left context 
	set with /b, p/ and a right context set with /f, s/ would find all vowels /u, e, a/ that are preceded by a /b/ or a /p/ and 
	followed by a /f/ or and /s/ (if the match condition EQUALS were used).
	If another tier of the TextGrid contains syntactic labels we can construct a new navigation context on for this tier and 
	combine it with the previous context to search for items that also match the syntactic context, etc
*/

Thing_define (IntervalTierNavigationContext, TierNavigationContext) {
	
	void v_info ()
			override;
			
	integer v_getSize (Function anyTier) override {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		return my intervals.size;
	}
	
	integer v_getIndexFromTime (Function anyTier, double time) override {
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
	
	double v_getLeftTime (Function anyTier, integer index) override {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		if (index < 1 || index > my intervals.size)
			return undefined;
		TextInterval interval = my intervals . at [index];
		return interval -> xmin;
	}
		
	double v_getRightTime (Function anyTier, integer index) override {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		if (index < 1 || index > my intervals.size)
			return undefined;
		TextInterval interval = my intervals . at [index];
		return interval -> xmax;
	}
		
	conststring32 v_getLabel (Function anyTier, integer index) override {
		IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
		if (index < 1 || index > my intervals.size)
			return U"-- undefined --";
		TextInterval interval = my intervals . at [index];
		return interval -> text.get();
	}
};

Thing_define (TextTierNavigationContext, TierNavigationContext) {
	
	void v_info ()
			override;
	
	integer v_getSize (Function anyTier) override {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		return my points.size;
	}
	
	integer v_getIndexFromTime (Function anyTier, double time) override {
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
	
	double v_getLeftTime (Function anyTier, integer index) override {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		if (index < 1 || index > my points.size)
			return undefined;
		TextPoint point = my points . at [index];
		return point -> number;
	}
	
	double v_getRightTime (Function anyTier, integer index) override {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		if (index < 1 || index > my points.size)
			return undefined;
		TextPoint point = my points . at [index];
		return point -> number;;
	}
	
	conststring32 v_getLabel (Function anyTier, integer index) override {
		TextTier me = reinterpret_cast<TextTier> (anyTier);
		if (index < 1 || index > my points.size)
			return U"-- undefined --";
		TextPoint point = my points . at [index];
		return point -> mark.get();
	}
};

autoIntervalTierNavigationContext IntervalTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber);

autoTextTierNavigationContext TextTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber);

autoTextGridNavigator TextGridNavigator_create (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber);


/*
	Add navigation context for a tier.
	The matchCriterion determines how a match in this tier relates to a match on the navigation tier.
	Suppose we have an interval on the navigation tier that matches. Its domain is [tmin, tmax] 
	(a TextPoint has tmin == tmax). The potential match in the tier we add has domain [tmin2, tmax2].
	Constraint:					Relation between matched domains:
	IS_LEFT						tmax2 <= tmin
	TOUCHES_LEFT				tmax2 == tmin
	OVERLAPS_LEFT				tmin2 < tmin && tmax2 <= tmax
	INSIDE						tmin2 >= tmin && tmax2 <= tmax
	OVERLAPS_RIGHT				tmin2 >= tmin && tmax2 > tmax
	TOUCHES_RIGHT				tmin2 == tmax
	IS_RIGHT					tmin2 >= tmax
	IS_OUTSIDE					tmax2 <= tmin || tmin2 >= tmax  (IS_LEFT || IS_RIGHT)
	OVERLAPS_LEFT_AND_RIGHT		tmin2 < tmin && tmax2 > tmax
	TOUCHES_LEFT_AND_RIGHT		tmin2 == tmin && tmax2 == tmax
*/
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

integer TextGridNavigator_getNextMatchAfterTime (TextGridNavigator me, double time);

integer TextGridNavigator_previous (TextGridNavigator me);

integer TextGridNavigator_getPreviousMatchBeforeTime (TextGridNavigator me, double time);

double TextGridNavigator_getCurrentStartTime (TextGridNavigator me);

double TextGridNavigator_getCurrentEndTime (TextGridNavigator me);

static inline integer TextGridNavigator_getFirstMatch (TextGridNavigator me) {
	return TextGridNavigator_getNextMatchAfterTime (me, my xmin - 0.1);
}

static inline integer TextGridNavigator_getLastMatch (TextGridNavigator me) {
	return TextGridNavigator_getPreviousMatchBeforeTime (me, my xmax + 0.1);
}

conststring32 TextGridNavigator_getCurrentLabel (TextGridNavigator me);

integer Tier_getNumberOfLeftContextOnlyMatches (Function me, TierNavigationContext tnc);

integer Tier_getNumberOfRightContextOnlyMatches (Function me, TierNavigationContext tnc);

integer Tier_getNumberOfNavigationOnlyMatches (Function me, TierNavigationContext tnc);

integer Tier_getNumberOfMatches (Function me, TierNavigationContext tnc);

#endif /* _TextGridNavigator_h_ */
