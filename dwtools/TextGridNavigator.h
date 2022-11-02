#ifndef _TextGridNavigator_h_
#define _TextGridNavigator_h_
/* TextGridNavigator.h
 *
 * Copyright (C) 2020-2022 David Weenink
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

#include "TextGridTierNavigator.h"
#include "melder.h"

#include "TextGridNavigator_def.h"

/*
	The purpose of a TextGridNavigator is to find successive elements that match a criterion on one selected tier, the navigation tier.
	The criterion may depend on "navigation contexts".
	Each navigation context handles only one particular tier. By combining different navigation contexts we 
	can construct searches that extend over multiple tiers. The simplest navigation context consists of only a single label that
	has to be matched.
	A somewhat more involved context has, instead of a single label, a set of topic labels where one of the set has to be matched. 
	The most extensive navigation context involves an After and a Before context set of labels, that have to be matched too.
	
	As an example consider a TextGrid with a tier whose intervals have been labeled with IPA symbols like /a, i, \ct, o, f, p etc/.
	A very simple navigation context that consists of the navigiation set with IPA vowel symbols (u, i, a), a Before context 
	set with /b, p/ and an After context set with /f, s/ would find all vowels /u, e, a/ that are preceded by a /b/ or a /p/ and 
	followed by a /f/ or and /s/ (if for all the matches the criterion "is equal to" is used).
	If another tier of the TextGrid contains syntactic labels we can construct a new navigation context for this tier and 
	combine it with the previous context to search for items that also match the syntactic context, etc.
*/

autoTextGridNavigator TextGridNavigator_createSimple (double xmin, double xmax, integer tierNumber,
	constSTRVEC const& topicLabels); 

autoTextGridNavigator TextGrid_and_NavigationContext_to_TextGridNavigator (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber, kMatchDomain matchDomain);

/*
	Add navigation context for a tier.
	The matchDomainAlignment determines how a matched location in this tier relates to the matched location on the topic tier.
	Suppose the interval on the topic tier that matched has domain [tmin, tmax].
	(a TextPoint has tmin == tmax). The potential match in the tier in one of the other contexts has domain [tmin2, tmax2].
	Constraint:					Relation between matched domains:
	IS_BEFORE					tmax2 <= tmin
	TOUCHES_BEFORE				tmax2 == tmin
	OVERLAPS_BEFORE				tmin2 < tmin && tmax2 <= tmax
	INSIDE						tmin2 >= tmin && tmax2 <= tmax
	OVERLAPS_AFTER				tmin2 >= tmin && tmax2 > tmax
	TOUCHES_AFTER				tmin2 == tmax
	IS_AFTER					tmin2 >= tmax
	IS_OUTSIDE					tmax2 <= tmin || tmin2 >= tmax  (IS_BEFORE || IS_AFTER)
	OVERLAPS_BEFORE_AND_AFTER	tmin2 < tmin && tmax2 > tmax
	TOUCHES_BEFORE_AND_AFTER	tmin2 == tmin && tmax2 == tmax
*/

void TextGridNavigator_replaceNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber);

void TextGridNavigator_addTextGridTierNavigator (TextGridNavigator me, TextGridTierNavigator thee, kMatchDomainAlignment matchDomainAlignment);

autoNavigationContext TextGridNavigator_extractNavigationContext (TextGridNavigator me, integer tierNumber);

autoTextGridNavigator TextGridTierNavigator_to_TextGridNavigator (TextGridTierNavigator me);

void TextGridNavigator_modifyBeforeRange (TextGridNavigator me, integer tierNumber, integer from, integer to);
void TextGridNavigator_modifyAfterRange (TextGridNavigator me, integer tierNumber, integer from, integer to);

void TextGridNavigator_modifyTopicCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion, kMatchBoolean matchBoolean);
void TextGridNavigator_modifyBeforeCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion, kMatchBoolean matchBoolean);
void TextGridNavigator_modifyAfterCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion, kMatchBoolean matchBoolean);

void TextGridNavigator_modifyCombinationCriterion (TextGridNavigator me, integer tierNumber, kContext_combination newUse, bool excludeTopicMatch);

void TextGridNavigator_modifyMatchDomain (TextGridNavigator me, integer tierNumber, kMatchDomain newDomain);

void TextGridNavigator_modifyMatchDomainAlignment (TextGridNavigator me, integer tierNumber, kMatchDomainAlignment matchDomainAlignment);

void TextGridNavigator_replaceTiers (TextGridNavigator me, TextGrid thee);

bool TextGridNavigator_isMatch (TextGridNavigator me, integer indexInTopicTier, integer *out_beforeIndex, integer *out_afterIndex);

autoINTVEC TextGridNavigator_listIndices (TextGridNavigator me, kContext_where where);
autoVEC TextGridNavigator_listStartTimes (TextGridNavigator me, kContext_where where);
autoVEC TextGridNavigator_listEndTimes (TextGridNavigator me, kContext_where where);
autoMAT TextGridNavigator_listDomains (TextGridNavigator me, kMatchDomain matchDomain);
autoSTRVEC TextGridNavigator_listLabels (TextGridNavigator me, kContext_where where);


integer TextGridNavigator_getNumberOfMatches (TextGridNavigator me);
integer TextGridNavigator_getNumberOfTopicMatches (TextGridNavigator me, integer tierNumber);
integer TextGridNavigator_getNumberOfBeforeMatches (TextGridNavigator me, integer tierNumber);
integer TextGridNavigator_getNumberOfAfterMatches (TextGridNavigator me, integer tierNumber);

integer TextGridNavigator_getTierNumberFromContextNumber (TextGridNavigator me, integer tierNumber);
integer TextGridNavigator_getTierNumberFromTierNavigator (TextGridNavigator me, integer navigatorNumber);

integer TextGridNavigator_findNext (TextGridNavigator me);

integer TextGridNavigator_findNextAfterTime (TextGridNavigator me, double time);

integer TextGridNavigator_findPrevious (TextGridNavigator me);

integer TextGridNavigator_findPreviousBeforeTime (TextGridNavigator me, double time);

integer TextGridNavigator_getIndex (TextGridNavigator me, integer tierNumber, kContext_where where);
double TextGridNavigator_getStartTime (TextGridNavigator me, integer tierNumber, kContext_where where);
conststring32 TextGridNavigator_getLabel (TextGridNavigator me, integer tierNumber, kContext_where where);
double TextGridNavigator_getEndTime (TextGridNavigator me, integer tierNumber, kContext_where where);

static inline integer TextGridNavigator_findFirst (TextGridNavigator me) {
	return TextGridNavigator_findNextAfterTime (me, my xmin - 0.1);
}

static inline integer TextGridNavigator_findLast (TextGridNavigator me) {
	return TextGridNavigator_findPreviousBeforeTime (me, my xmax + 0.1);
}

/* Split off later */

autoTextGridNavigator TextGrid_to_TextGridNavigator (TextGrid me, integer tierNumber, 
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	constSTRVEC const& beforeLabels, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean,
	constSTRVEC const& afterLabels, kMelder_string afterCriterion, kMatchBoolean afterMatchBoolean,
	kContext_combination combinationCriterion, bool excludeTopic, kMatchDomain matchDomain
);

autoTextGridNavigator TextGrid_to_TextGridNavigator_topicSearch (TextGrid me, integer tierNumber,
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	kMatchDomain matchDomain
);

void TextGridNavigator_and_TextGrid_addSearchTier_topicOnly (TextGridNavigator me, TextGrid thee, integer tierNumber, 
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	kMatchDomain matchDomain, kMatchDomainAlignment matchDomainAlignment
);

void TextGridNavigator_and_TextGrid_addSearchTier (TextGridNavigator me, TextGrid thee, integer tierNumber, 
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	constSTRVEC const& beforeLabels, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean,
	constSTRVEC const& afterLabels, kMelder_string afterCriterion, kMatchBoolean afterMatchBoolean,
	kContext_combination combinationCriterion, bool excludeTopic, kMatchDomain matchDomain, kMatchDomainAlignment matchDomainAlignment
);

#endif /* _TextGridNavigator_h_ */
