/* TextGridTierNavigator.cpp
 *
 * Copyright (C) 2021 David Weenink
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

#include "enums_getText.h"
#include "TextGridTierNavigator_enums.h"
#include "enums_getValue.h"
#include "TextGridTierNavigator_enums.h"

#include "oo_DESTROY.h"
#include "TextGridTierNavigator_def.h"
#include "oo_COPY.h"
#include "TextGridTierNavigator_def.h"
#include "oo_EQUAL.h"
#include "TextGridTierNavigator_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "TextGridTierNavigator_def.h"
#include "oo_WRITE_TEXT.h"
#include "TextGridTierNavigator_def.h"
#include "oo_READ_TEXT.h"
#include "TextGridTierNavigator_def.h"
#include "oo_WRITE_BINARY.h"
#include "TextGridTierNavigator_def.h"
#include "oo_READ_BINARY.h"
#include "TextGridTierNavigator_def.h"
#include "oo_DESCRIPTION.h"
#include "TextGridTierNavigator_def.h"

static integer IntervalTier_getSize (Function anyTier) {
	IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
	return my intervals.size;
}

static integer IntervalTier_getIndexFromTime (Function anyTier, double time) {
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

static double IntervalTier_getStartTime (Function anyTier, integer index) {
	IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
	if (index < 1 || index > my intervals.size)
		return undefined;
	TextInterval interval = my intervals . at [index];
	return interval -> xmin;
}
	
static double IntervalTier_getEndTime (Function anyTier, integer index) {
	IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
	if (index < 1 || index > my intervals.size)
		return undefined;
	TextInterval interval = my intervals . at [index];
	return interval -> xmax;
}
	
static conststring32 IntervalTier_getLabel (Function anyTier, integer index) {
	IntervalTier me = reinterpret_cast<IntervalTier> (anyTier);
	if (index < 1 || index > my intervals.size)
		return U"-- undefined --";
	TextInterval interval = my intervals . at [index];
	return interval -> text.get();
}

static integer TextTier_getSize (Function anyTier) {
	TextTier me = reinterpret_cast<TextTier> (anyTier);
	return my points.size;
}

static integer TextTier_getIndexFromTime (Function anyTier, double time) {
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

static double TextTier_getStartTime (Function anyTier, integer index) {
	TextTier me = reinterpret_cast<TextTier> (anyTier);
	if (index < 1 || index > my points.size)
		return undefined;
	TextPoint point = my points . at [index];
	return point -> number;
}

static double TextTier_getEndTime (Function anyTier, integer index) {
	TextTier me = reinterpret_cast<TextTier> (anyTier);
	if (index < 1 || index > my points.size)
		return undefined;
	TextPoint point = my points . at [index];
	return point -> number;;
}

static conststring32 TextTier_getLabel (Function anyTier, integer index) {
	TextTier me = reinterpret_cast<TextTier> (anyTier);
	if (index < 1 || index > my points.size)
		return U"-- undefined --";
	TextPoint point = my points . at [index];
	return point -> mark.get();
}

integer structTextGridTierNavigator :: v_getSize () {
	return ( anyTier -> classInfo == classIntervalTier ? IntervalTier_getSize (anyTier.get()) : 
		TextTier_getSize (anyTier.get()) );
}

integer structTextGridTierNavigator :: v_getIndexFromTime (double time) {
	return ( anyTier -> classInfo == classIntervalTier ? IntervalTier_getIndexFromTime (anyTier.get(), time) : 
		TextTier_getIndexFromTime (anyTier.get(), time) );
}

double structTextGridTierNavigator :: v_getStartTime (integer index) {
	return ( anyTier -> classInfo == classIntervalTier ? IntervalTier_getStartTime (anyTier.get(), index) : 
		TextTier_getStartTime (anyTier.get(), index) );
}

double structTextGridTierNavigator :: v_getEndTime (integer index) {
	return ( anyTier -> classInfo == classIntervalTier ? IntervalTier_getEndTime (anyTier.get(), index) : 
		TextTier_getEndTime (anyTier.get(), index) );
}

conststring32 structTextGridTierNavigator :: v_getLabel (integer index) {
	return ( anyTier -> classInfo == classIntervalTier ? IntervalTier_getLabel (anyTier.get(), index) : 
		TextTier_getLabel (anyTier.get(), index) );
}


void structTextGridTierNavigator :: v_info () {
	const integer tierSize = our v_getSize ();
	MelderInfo_writeLine (U"\tNumber of matches on tier: ");
	MelderInfo_writeLine (U"\t\tTopic labels only: ",
		TextGridTierNavigator_getNumberOfTopicMatches (this), U" (from ", tierSize, U")");
	MelderInfo_writeLine (U"\t\tBefore labels only: ",
		TextGridTierNavigator_getNumberOfBeforeMatches (this), U" (from ", tierSize, U")");
	MelderInfo_writeLine (U"\t\tAfter labels only: ",
		TextGridTierNavigator_getNumberOfAfterMatches (this), U" (from ", tierSize, U")");
	MelderInfo_writeLine (U"\t\tCombined: ", TextGridTierNavigator_getNumberOfMatches (this),  U" (from ", tierSize, U")");
	MelderInfo_writeLine (U"\tMatch domain: ", kMatchDomain_getText (our matchDomain));
}

static void NavigationContext_checkMatchDomain (NavigationContext me, kMatchDomain matchDomain) {
	if (matchDomain == kMatchDomain::BEFORE_START_TO_TOPIC_END)
		Melder_require (my useCriterion == kContext_use::BEFORE || my useCriterion != kContext_use::BEFORE_AND_AFTER,
			U"You should not use the match domain <", kMatchDomain_getText (kMatchDomain::BEFORE_START_TO_TOPIC_END), U"> if you don't always use Before in the matching <", kContext_use_getText (my useCriterion), U">.");
	else if (matchDomain == kMatchDomain::BEFORE_START_TO_AFTER_END)
		Melder_require (my useCriterion == kContext_use::BEFORE_AND_AFTER,
			U"You should not use the match domain <", kMatchDomain_getText (kMatchDomain::BEFORE_START_TO_AFTER_END), U"> if you don't always use Before and After in the matching <", kContext_use_getText (my useCriterion), U">.");
	else if (matchDomain == kMatchDomain::TOPIC_START_TO_AFTER_END)
		Melder_require (my useCriterion == kContext_use::AFTER || my useCriterion != kContext_use::BEFORE_AND_AFTER,
			U"You should not use the match domain <", kMatchDomain_getText (kMatchDomain::TOPIC_START_TO_AFTER_END), U"> if you don't always use After in the matching <", kContext_use_getText (my useCriterion), U">.");
	else if (matchDomain == kMatchDomain::BEFORE_START_TO_BEFORE_END)
		Melder_require (my useCriterion == kContext_use::BEFORE || my useCriterion == kContext_use::BEFORE_AND_AFTER,
			U"You should not use the match domain <", kMatchDomain_getText (kMatchDomain::BEFORE_START_TO_BEFORE_END), U"> if you don't always use Before in the matching <", kContext_use_getText (my useCriterion), U">.");
	else if (matchDomain == kMatchDomain::AFTER_START_TO_AFTER_END)
		Melder_require (my useCriterion == kContext_use::AFTER || my useCriterion == kContext_use::BEFORE_AND_AFTER,
			U"You should not use the match domain <", kMatchDomain_getText (kMatchDomain::AFTER_START_TO_AFTER_END), U"> if you don't always use After in the matching <", kContext_use_getText (my useCriterion), U">.");
	// else MATCH_START_TO_MATCH_END || TOPIC_START_TO_TOPIC_END are always ok.
}

autoTextGridTierNavigator TextGridTierNavigator_create (Function me, NavigationContext thee, kMatchDomain matchDomain) {
	try {
		NavigationContext_checkMatchDomain (thee, matchDomain);
		autoTextGridTierNavigator thee = Thing_new (TextGridTierNavigator);
		Function_init (thee.get(), my xmin, my xmax);  
		thy anyTier = Data_copy (me);
		thy beforeRange.first = thy beforeRange.last = 1;
		thy afterRange.first = thy afterRange.last = 1;
		thy matchDomain = matchDomain;
	} catch (MelderError) {
		Melder_throw (U"TextGridTierNavigator not created");
	}
}


autoTextGridTierNavigator TextGrid_and_NavigationContext_to_TextGridTierNavigator (TextGrid me, NavigationContext thee, integer tierNumber, kMatchDomain matchDomain) {
	try {
		Function tier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		autoTextGridTierNavigator him = TextGridTierNavigator_create (tier, thee, matchDomain);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": could not create TextGridTierNavigator.");
	}	
}

void TextGridTierNavigator_replaceNavigationContext (TextGridTierNavigator me, NavigationContext thee) {
	try {
		my navigationContext -> topicLabels = Data_copy (thy topicLabels.get());
		my navigationContext -> topicCriterion = thy topicCriterion;
		my navigationContext -> beforeLabels = Data_copy (thy beforeLabels.get());
		my navigationContext -> beforeCriterion = thy beforeCriterion;
		my navigationContext -> afterLabels = Data_copy (thy afterLabels.get());
		my navigationContext -> afterCriterion = thy afterCriterion;
		my navigationContext -> useCriterion = thy useCriterion;
		my navigationContext -> excludeTopicMatch = thy excludeTopicMatch;		
	} catch (MelderError) {
		Melder_throw (me, U": could not replace navigation context.");
	}
}

autoNavigationContext TextGridTierNavigator_extractNavigationContext (TextGridTierNavigator me) {
	try {
		autoNavigationContext thee = Thing_new (NavigationContext);
		thy topicLabels = Data_copy (my navigationContext -> topicLabels.get());
		thy topicCriterion = my navigationContext -> topicCriterion;
		thy beforeLabels = Data_copy (my navigationContext -> beforeLabels.get());
		thy beforeCriterion = my navigationContext -> beforeCriterion;
		thy afterLabels = Data_copy (my navigationContext -> afterLabels.get());
		thy afterCriterion = my navigationContext -> afterCriterion;
		thy useCriterion = my navigationContext -> useCriterion;
		thy excludeTopicMatch = my navigationContext -> excludeTopicMatch;
		return thee;
		
	} catch (MelderError) {
		Melder_throw (me, U": could not extract navigation context.");
	}
}

void TextGridTierNavigator_replaceTextGrid (TextGridTierNavigator me, Function thee) {
	try {
		Melder_require (my anyTier -> classInfo == thy classInfo,
			U"The tier should be of the same kind as the one you want to replace.");
		my anyTier = Data_copy (thee);
		my currentTopicIndex = 0; // offLeft
	} catch (MelderError) {
		Melder_throw (me, U": cannot reset with ", thee, U".");
	}
}

void TextGridTierNavigator_modifyBeforeRange (TextGridTierNavigator me, integer from, integer to) {
	Melder_require (from > 0 &&  to > 0,
		U"Both numbers in the Before range should be positive.");
	my beforeRange.first = std::min (from, to);
	my beforeRange.last = std::max (from, to);
}

void TextGridTierNavigator_modifyAfterRange (TextGridTierNavigator me, integer from, integer to) {
	Melder_require (from > 0 &&  to > 0,
		U"Both numbers in the after range should be positive.");
	my afterRange.first = std::min (from, to);
	my afterRange.last = std::max (from, to);
}

void TextGridTierNavigator_modifyTopicCriterion (TextGridTierNavigator me, kMelder_string newCriterion) {
	NavigationContext_modifyTopicCriterion (my navigationContext.get(), newCriterion);
}

void TextGridTierNavigator_modifyBeforeCriterion (TextGridTierNavigator me, kMelder_string newCriterion) {
	NavigationContext_modifyBeforeCriterion (my navigationContext.get(), newCriterion);
}

void TextGridTierNavigator_modifyAfterCriterion (TextGridTierNavigator me, kMelder_string newCriterion) {
	NavigationContext_modifyAfterCriterion (my navigationContext.get(), newCriterion);
}

void TextGridTierNavigator_modifyUseCriterion (TextGridTierNavigator me, kContext_use newUse, bool excludeTopicMatch) {
	NavigationContext_modifyUseCriterion (my navigationContext.get(), newUse, excludeTopicMatch);
}

void TextGridTierNavigator_modifyMatchingRange (TextGridTierNavigator me, integer maximumLookAhead, integer maximumLookBack) {
	my maximumLookAhead = maximumLookAhead;
	my maximumLookBack = maximumLookBack;
}

static bool TextGridTierNavigator_isTopicMatch (TextGridTierNavigator me, integer index) {
	conststring32 label = my v_getLabel (index);
	return NavigationContext_isTopicLabel (my navigationContext.get(), label);
}

static integer TextGridTierNavigator_getBeforeIndex (TextGridTierNavigator me, integer topicIndex) {
	if (! my navigationContext -> beforeLabels)
		return 0;
	if (topicIndex - my beforeRange.first < 1 || topicIndex > my v_getSize ())
		return 0;
	const integer startIndex = std::max (1_integer, topicIndex - my beforeRange.first);
	const integer endIndex = std::max (1_integer, topicIndex - my beforeRange.last);
	for (integer index = startIndex; index >= endIndex; index --) {
		conststring32 label = my v_getLabel (index);
		if (NavigationContext_isBeforeLabel (my navigationContext.get(), label))
			return index;
	}
	return 0;
}

static integer TextGridTierNavigator_getAfterIndex (TextGridTierNavigator me, integer topicIndex) {
	if (! my navigationContext -> afterLabels)
		return 0;
	integer mySize = my v_getSize ();
	if (topicIndex + my afterRange.first > mySize || topicIndex < 1)
		return 0;
	const integer startInterval = std::min (mySize, topicIndex + my afterRange.last);
	const integer endInterval = std::min (mySize, topicIndex + my afterRange.last);
	for (integer index = startInterval; index <= endInterval; index ++) {
		conststring32 label = my v_getLabel (index);
		if (NavigationContext_isAfterLabel (my navigationContext.get(), label))
			return index;
	}
	return 0;
}

static inline bool TextGridTierNavigator_isAfterMatch (TextGridTierNavigator me, integer topicIndex) {
	return TextGridTierNavigator_getAfterIndex (me, topicIndex) > 0;
}

static inline bool TextGridTierNavigator_isBeforeMatch (TextGridTierNavigator me, integer topicIndex) {
	return TextGridTierNavigator_getBeforeIndex (me, topicIndex) > 0;
}

integer TextGridTierNavigator_getNumberOfAfterMatches (TextGridTierNavigator me) {
	if (my navigationContext -> afterLabels -> numberOfStrings == 0)
		return 0;
	integer numberOfMatches = 0;
	for (integer index = 1; index <= my v_getSize (); index ++) {
		conststring32 label = my v_getLabel (index);
		if (NavigationContext_isAfterLabel (my navigationContext.get(), label))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

integer TextGridTierNavigator_getNumberOfBeforeMatches (TextGridTierNavigator me) {
	if (my navigationContext -> beforeLabels -> numberOfStrings == 0)
		return 0;
	integer numberOfMatches = 0;
	for (integer index = 1; index <= my v_getSize (); index ++) {
		conststring32 label = my v_getLabel (index);
		if (NavigationContext_isBeforeLabel (my navigationContext.get(), label))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

integer TextGridTierNavigator_getNumberOfTopicOnlyMatches (TextGridTierNavigator me) {
	if (my navigationContext -> topicLabels -> numberOfStrings == 0)
		return 0;
	integer numberOfMatches = 0;
	for (integer index = 1; index <= my v_getSize (); index ++) {
		conststring32 label = my v_getLabel (index);
		if (NavigationContext_isTopicLabel (my navigationContext.get(), label))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

bool TextGridTierNavigator_isMatch (TextGridTierNavigator me, integer topicIndex) {
	if (topicIndex < 1 && topicIndex > my v_getSize ())
		return false;
	const bool isTopicMatch = ( my navigationContext -> excludeTopicMatch ? true : TextGridTierNavigator_isTopicMatch (me, topicIndex) );
	if (! isTopicMatch || my navigationContext -> useCriterion == kContext_use::NO_BEFORE_AND_NO_AFTER)
		return isTopicMatch;
	bool isMatch = false;
	if (my navigationContext -> useCriterion == kContext_use::BEFORE_AND_AFTER)
		isMatch = TextGridTierNavigator_isBeforeMatch (me, topicIndex) &&
			TextGridTierNavigator_isAfterMatch (me, topicIndex);
	else if (my navigationContext -> useCriterion == kContext_use::AFTER)
		isMatch = TextGridTierNavigator_isAfterMatch (me, topicIndex);
	else if (my navigationContext -> useCriterion == kContext_use::BEFORE)
		isMatch = TextGridTierNavigator_isBeforeMatch (me, topicIndex);
	else if (my navigationContext -> useCriterion == kContext_use::BEFORE_OR_AFTER_OR_BOTH)
		isMatch = TextGridTierNavigator_isBeforeMatch (me, topicIndex) ||
			TextGridTierNavigator_isAfterMatch (me, topicIndex);
	else if (my navigationContext -> useCriterion == kContext_use::BEFORE_OR_AFTER_NOT_BOTH)
		isMatch = TextGridTierNavigator_isBeforeMatch (me, topicIndex) != TextGridTierNavigator_isAfterMatch (me, topicIndex);
	return isMatch;
}

integer TextGridTierNavigator_getNumberOfMatches (TextGridTierNavigator me) {
	integer numberOfMatches = 0;
	for (integer index = 1; index <= my v_getSize (); index ++)
		if (TextGridTierNavigator_isMatch (me, index))
			numberOfMatches ++;
	return numberOfMatches;
}

integer TextGridTierNavigator_getNumberOfTopicMatches (TextGridTierNavigator me) {
	integer numberOfMatches = 0;
	for (integer index = 1; index <= my v_getSize (); index ++)
		if (TextGridTierNavigator_isTopicMatch (me, index))
			numberOfMatches ++;
	return numberOfMatches;
}

static void TextGridTierNavigator_getMatchDomain (TextGridTierNavigator me, integer index, double *out_startTime, double *out_endTime) {
	kContext_where startWhere, endWhere;
	if (my matchDomain == kMatchDomain::MATCH_START_TO_MATCH_END) {
		if (my navigationContext -> useCriterion == kContext_use::NO_BEFORE_AND_NO_AFTER) {
			startWhere = kContext_where::TOPIC;
			endWhere = kContext_where::TOPIC;
		} else if (my navigationContext -> useCriterion == kContext_use::BEFORE) {
			startWhere = kContext_where::BEFORE;
			endWhere = ( my navigationContext -> excludeTopicMatch ? kContext_where::BEFORE : kContext_where::TOPIC );
		} else if (my navigationContext -> useCriterion == kContext_use::AFTER) {
			startWhere = ( my navigationContext -> excludeTopicMatch ? kContext_where::AFTER : kContext_where::TOPIC );
			endWhere = kContext_where::AFTER;
		} else if (my navigationContext -> useCriterion == kContext_use::BEFORE_AND_AFTER) {
			startWhere = kContext_where::BEFORE;
			endWhere = kContext_where::AFTER;
		} else if (my navigationContext -> useCriterion == kContext_use::BEFORE_OR_AFTER_NOT_BOTH) {
			if (TextGridTierNavigator_isBeforeMatch (me, index)) {
				startWhere = kContext_where::BEFORE;
				endWhere = ( my navigationContext -> excludeTopicMatch ? kContext_where::BEFORE : kContext_where::TOPIC );
			} else {
				startWhere = ( my navigationContext -> excludeTopicMatch ? kContext_where::AFTER : kContext_where::TOPIC );
				endWhere = kContext_where::AFTER;
			}
		} else if (my navigationContext -> useCriterion == kContext_use::BEFORE_OR_AFTER_OR_BOTH) {
			bool isBeforeMatch = TextGridTierNavigator_isBeforeMatch (me, index);
			bool isAfterMatch = TextGridTierNavigator_isAfterMatch (me, index);
			if (isBeforeMatch && isAfterMatch) {
				startWhere = kContext_where::BEFORE;
				endWhere = kContext_where::AFTER;
			} else if (isBeforeMatch) {
				startWhere = kContext_where::BEFORE;
				endWhere = ( my navigationContext -> excludeTopicMatch ? kContext_where::BEFORE : kContext_where::TOPIC );
			} else {
				startWhere = ( my navigationContext -> excludeTopicMatch ? kContext_where::AFTER : kContext_where::TOPIC );
				endWhere = kContext_where::AFTER;
			}
		}
	} else if (my matchDomain == kMatchDomain::TOPIC_START_TO_TOPIC_END) {
		startWhere = kContext_where::TOPIC;
		endWhere = kContext_where::TOPIC;
	} else if (my matchDomain == kMatchDomain::BEFORE_START_TO_TOPIC_END) {
		startWhere = kContext_where::BEFORE;
		endWhere = kContext_where::TOPIC;
	} else if (my matchDomain == kMatchDomain::TOPIC_START_TO_AFTER_END) {
		startWhere = kContext_where::TOPIC;
		endWhere = kContext_where::AFTER;
	} else if (my matchDomain == kMatchDomain::BEFORE_START_TO_AFTER_END) {
		startWhere = kContext_where::BEFORE;
		endWhere = kContext_where::AFTER;
	} else if (my matchDomain == kMatchDomain::BEFORE_START_TO_BEFORE_END) {
		startWhere = kContext_where::BEFORE;
		endWhere = kContext_where::BEFORE;
	} else if (my matchDomain == kMatchDomain::AFTER_START_TO_AFTER_END) {
		startWhere = kContext_where::AFTER;
		endWhere = kContext_where::AFTER;
	}
	if (out_startTime)
		*out_startTime = TextGridTierNavigator_getStartTime (me, startWhere);
	if (out_endTime)
		*out_endTime = TextGridTierNavigator_getEndTime (me, endWhere);
}

static integer TextGridTierNavigator_setCurrentAtTime (TextGridTierNavigator me, double time) {
	const integer index = my v_getIndexFromTime (time);
	my currentTopicIndex = index;
	return index;
}

integer TextGridTierNavigator_locateNext (TextGridTierNavigator me) {
	const integer currentTopicIndex = my currentTopicIndex, size = my v_getSize ();
	for (integer index = currentTopicIndex + 1; index <= size; index ++) {
		if (TextGridTierNavigator_isMatch (me, index)) {
			my currentTopicIndex = index;
			return index;
		}
	}
	my currentTopicIndex = size + 1; // offRight
	return my currentTopicIndex;
}

integer TextGridTierNavigator_locateNextAfterTime (TextGridTierNavigator me, double time) {
	TextGridTierNavigator_setCurrentAtTime (me, time);
	return TextGridTierNavigator_locateNext (me);
}

integer TextGridTierNavigator_locatePrevious (TextGridTierNavigator me) {
	const integer currentTopicIndex = my currentTopicIndex;
	for (integer index = currentTopicIndex - 1; index > 0; index --) {
		if (TextGridTierNavigator_isMatch (me, index)) {
			my currentTopicIndex = index;
			return index;
		}
	}
	my currentTopicIndex = 0;
	return 0;
}

integer TextGridTierNavigator_locatePreviousBeforeTime (TextGridTierNavigator me, double time) {
	TextGridTierNavigator_setCurrentAtTime (me, time);
	return TextGridTierNavigator_locatePrevious (me);
}

//double TextGridTierNavigator_getStartTime (TextGridTierNavigator me, integer index) {
//	return my v_getStartTime (index);
//}

double TextGridTierNavigator_getStartTime (TextGridTierNavigator me, kContext_where where) {
	const integer index = ( where == kContext_where::TOPIC ? my currentTopicIndex :
		where == kContext_where::BEFORE ? TextGridTierNavigator_getBeforeIndex (me, my currentTopicIndex) : 
		where == kContext_where::AFTER ? TextGridTierNavigator_getAfterIndex (me, my currentTopicIndex): 0);
	return my v_getStartTime (index);
}

conststring32 TextGridTierNavigator_getLabel (TextGridTierNavigator me, kContext_where where) {
	const integer index = ( where == kContext_where::TOPIC ? my currentTopicIndex :
		where == kContext_where::BEFORE ? TextGridTierNavigator_getBeforeIndex (me, my currentTopicIndex) : 
		where == kContext_where::AFTER ? TextGridTierNavigator_getAfterIndex (me, my currentTopicIndex): 0);
	return my v_getLabel (index);
}

double TextGridTierNavigator_getEndTime (TextGridTierNavigator me, kContext_where where) {
	const integer index = ( where == kContext_where::TOPIC ? my currentTopicIndex :
		where == kContext_where::BEFORE ? TextGridTierNavigator_getBeforeIndex (me, my currentTopicIndex) : 
		where == kContext_where::AFTER ? TextGridTierNavigator_getAfterIndex (me, my currentTopicIndex): 0);
	return my v_getEndTime (index);
}

integer TextGridTierNavigator_getIndex (TextGridTierNavigator me, integer tierNumber, kContext_where where) {
	const integer index = ( where == kContext_where::TOPIC ? my currentTopicIndex :
		where == kContext_where::BEFORE ? TextGridTierNavigator_getBeforeIndex (me, my currentTopicIndex) : 
		where == kContext_where::AFTER ? TextGridTierNavigator_getAfterIndex (me, my currentTopicIndex): 0);
	return ( index > my v_getSize () ? 0 : index );
}

/* End of file TextGridTierNavigator.cpp */
