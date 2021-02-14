/* TextGridNavigator.cpp
 *
 * Copyright (C) 2020-2021 David Weenink
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

#include "TextGridNavigator.h"

#include "enums_getText.h"
#include "TextGridNavigator_enums.h"
#include "enums_getValue.h"
#include "TextGridNavigator_enums.h"

#include "oo_DESTROY.h"
#include "TextGridNavigator_def.h"
#include "oo_COPY.h"
#include "TextGridNavigator_def.h"
#include "oo_EQUAL.h"
#include "TextGridNavigator_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "TextGridNavigator_def.h"
#include "oo_WRITE_TEXT.h"
#include "TextGridNavigator_def.h"
#include "oo_READ_TEXT.h"
#include "TextGridNavigator_def.h"
#include "oo_WRITE_BINARY.h"
#include "TextGridNavigator_def.h"
#include "oo_READ_BINARY.h"
#include "TextGridNavigator_def.h"
#include "oo_DESCRIPTION.h"
#include "TextGridNavigator_def.h"


Thing_implement (TierNavigationContext, Daata, 0);
Thing_implement (IntervalTierNavigationContext, TierNavigationContext, 0);
Thing_implement (TextTierNavigationContext, TierNavigationContext, 0);

void structTierNavigationContext :: v_info () {
	structNavigationContext :: v_info ();
	MelderInfo_writeLine (U"Tier number: ", tierNumber);
}

integer structTierNavigationContext :: v_getSize (Function /* tier */) {
	return 0;	
};
	
integer structTierNavigationContext :: v_getIndexFromTime (Function /* anyTier */, double /* time */) {
	return 0;
}
	
double structTierNavigationContext :: v_getStartTime (Function anyTier, integer /* index */) {
	return anyTier -> xmin;
}
		
double structTierNavigationContext :: v_getEndTime (Function anyTier, integer /* index */) {
	return anyTier -> xmax;
}
		
conststring32 structTierNavigationContext ::v_getLabel (Function /* anyTier */, integer /* index */) {
	return U"";
}

void structIntervalTierNavigationContext :: v_info () {
	structNavigationContext :: v_info ();
}

void structTextTierNavigationContext :: v_info () {
	structNavigationContext :: v_info ();
}

static void TierNavigationContext_init (TierNavigationContext me, NavigationContext thee, integer tierNumber) {
	my topicLabels = Data_copy (thy topicLabels.get());
	my topicCriterion = thy topicCriterion;
	my beforeLabels = Data_copy (thy beforeLabels.get());
	my beforeCriterion = thy beforeCriterion;
	my afterLabels = Data_copy (thy afterLabels.get());
	my afterCriterion = thy afterCriterion;
	my combinationCriterion = thy combinationCriterion;
	my matchContextOnly = thy matchContextOnly;		
	my tierNumber = tierNumber;
	my rightContextFrom = my rightContextTo = 1;
	my leftContextFrom = my leftContextTo = 1;
}

autoIntervalTierNavigationContext IntervalTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber) {
	try {
		autoIntervalTierNavigationContext me = Thing_new (IntervalTierNavigationContext);
		TierNavigationContext_init (me.get(), navigationContext, tierNumber);
		return me;
	} catch (MelderError) {
		Melder_throw (U"IntervalTierNavigationContext not created from ", navigationContext);
	}	
}

autoTextTierNavigationContext TextTierNavigationContext_create (NavigationContext navigationContext, integer tierNumber) {
	try {
		autoTextTierNavigationContext me = Thing_new (TextTierNavigationContext);
		TierNavigationContext_init (me.get(), navigationContext, tierNumber);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextTierNavigationContext not created from ", navigationContext);
	}	
}

/*
static void TierNavigationContext_setItemOrientation (TierNavigationContext me, kNavigatableTier_match matchCriterion) {
	my matchCriterion = matchCriterion;
}
*/

Thing_implement (TextGridNavigator, Function, 0);

void structTextGridNavigator :: v_info () {
	const integer navigationTierNumber = TextGridNavigator_getTierNumberFromContextNumber (this, 1);
	integer navigationTierSize;
	for (integer icontext = 1; icontext <= tierNavigationContexts.size; icontext ++) {
		const integer tierNumber = TextGridNavigator_getTierNumberFromContextNumber (this, icontext);
		const TierNavigationContext tnc = our tierNavigationContexts.at [icontext];
		const Function anyTier = our textgrid -> tiers -> at [tierNumber];
		const integer tierSize = tnc -> v_getSize (anyTier);
		if (icontext == 1)
			navigationTierSize = tierSize;
		MelderInfo_writeLine (U"Tier number: ", tierNumber, ( icontext== 1 ? U" (navigation tier)" : U" (sub search tier)" ));
		tnc -> v_info ();
		MelderInfo_writeLine (U"\tNumber of matches on tier ", tierNumber, U":");
		MelderInfo_writeLine (U"\t\tNavigation labels only: ",
			Tier_getNumberOfNavigationOnlyMatches (anyTier, tnc), U" (from ", tierSize, U")");
		MelderInfo_writeLine (U"\t\tLeft context labels only: ",
			Tier_getNumberOfLeftContextOnlyMatches (anyTier, tnc), U" (from ", tierSize, U")");
		MelderInfo_writeLine (U"\t\tRight context labels only: ",
			Tier_getNumberOfRightContextOnlyMatches (anyTier, tnc), U" (from ", tierSize, U")");
		MelderInfo_writeLine (U"\t\tCombined contexts: ", Tier_getNumberOfMatches (anyTier, tnc),  U" (from ", tierSize, U")");
		if (icontext > 1)
			MelderInfo_writeLine (U"\tMatch criterion to tier number ", navigationTierNumber, U": ", kNavigatableTier_match_getText (tnc -> matchCriterion));
	}
	
	MelderInfo_writeLine (U"Number of complete matches: ", TextGridNavigator_getNumberOfMatches (this),  U" (from ", navigationTierSize, U")");
}

autoTextGridNavigator TextGridNavigator_create (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber) {
	try {
		autoTextGridNavigator me = Thing_new (TextGridNavigator);
		Function_init (me.get(), textgrid -> xmin, textgrid -> xmax);
		my textgrid = Data_copy (textgrid);
		TextGridNavigator_addNavigationContext (me.get(), navigationContext, tierNumber, kNavigatableTier_match::TOUCHES_BEFORE_AND_AFTER);
		Melder_require (TextGridNavigator_getNumberOfMatches (me.get())> 0,
			U"There are no matches on tier number (", tierNumber, U"). Maybe you should change the tier number?");
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigator could not be created from ", textgrid, U" and ", navigationContext);
	}
}

static bool TextGridNavigator_isNavigatableTierInUse (TextGridNavigator me, integer tierNumber) {
	if (my tierNavigationContexts.size == 0)
		return false;
	for (integer icontext = 1; icontext <= my tierNavigationContexts. size; icontext ++)
		if (my tierNavigationContexts. at [icontext] -> tierNumber == tierNumber)
			return true;
	return false;
}

static void TextGridNavigator_checkNavigatableTierIsNotInUse (TextGridNavigator me, integer tierNumber) {
	Melder_require (! TextGridNavigator_isNavigatableTierInUse (me, tierNumber),
		U": tier number ", tierNumber, U" is already in use.");
}

static bool TextGridNavigator_hasLeftContext (TextGridNavigator me, integer contextNumber) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContexts.size,
		U"Your context number (", contextNumber, U") should not be larger than the number of contexts (", my tierNavigationContexts.size, U").");
	TierNavigationContext tnc = my tierNavigationContexts . at [contextNumber];
	return tnc -> beforeLabels && tnc ->  beforeLabels -> strings.size > 0;
}

static bool TextGridNavigator_hasRightContext (TextGridNavigator me, integer contextNumber) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContexts.size,
		U"Your context number (", contextNumber, U") should not be larger than the number of contexts (", my tierNavigationContexts.size, U").");
	TierNavigationContext tnc = my tierNavigationContexts . at [contextNumber];
	return tnc -> afterLabels && tnc ->  afterLabels -> strings.size > 0;
}

void TextGridNavigator_addNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber, kNavigatableTier_match matchCriterion) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (my textgrid.get(), tierNumber);
		TextGridNavigator_checkNavigatableTierIsNotInUse (me, tierNumber);
		autoTierNavigationContext tierNavigationContexts;
		if (my textgrid -> tiers -> at [tierNumber] -> classInfo == classIntervalTier)
			tierNavigationContexts = IntervalTierNavigationContext_create (thee, tierNumber);
		else
			tierNavigationContexts = TextTierNavigationContext_create (thee, tierNumber);
		tierNavigationContexts -> matchCriterion = matchCriterion;
		my tierNavigationContexts.addItem_move (tierNavigationContexts.move());
	} catch (MelderError) {
		Melder_throw (me, U": could not add navigation context ", thee, U".");
	}
}

void TextGridNavigator_replaceTextGrid (TextGridNavigator me, TextGrid thee) {
	try {
		Melder_require (thy tiers -> size == my textgrid -> tiers -> size,
			U"The TextGrid should have the same number of tiers as the one you want to replace (", my textgrid -> tiers->size, U").");
		for (integer icontext = 1; icontext <= my tierNavigationContexts. size; icontext ++) {
			const TierNavigationContext navigationContext = my tierNavigationContexts. at [icontext];
			const integer tierNumber = navigationContext -> tierNumber;
			Melder_require (thy tiers -> at [tierNumber] -> classInfo == my textgrid -> tiers -> at [tierNumber] -> classInfo, 
				U"The TextGrid should have the same kind of tiers at the same positions as the original you want to replace ");
		}
		my textgrid = Data_copy (thee);
		
		my tierNavigationContexts. at [1] -> current = 0; // offLeft
	} catch (MelderError) {
		Melder_throw (me, U": cannot reset with ", thee, U".");
	}
}

integer TextGridNavigator_getTierNumberFromContextNumber (TextGridNavigator me, integer contextNumber) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContexts . size,
		U"The context number should be between 1 and ", my tierNavigationContexts . size, U".)");
	return my tierNavigationContexts . at [contextNumber] -> tierNumber;
}

integer TextGridNavigator_getContextNumberFromTierNumber (TextGridNavigator me, integer tierNumber) {
	TextGrid_checkSpecifiedTierNumberWithinRange (my textgrid.get(), tierNumber);
	for (integer icontext = 1; icontext <= my tierNavigationContexts . size; icontext ++) {
		const TierNavigationContext tnc = my tierNavigationContexts . at [icontext];
		if (tnc -> tierNumber == tierNumber)
			return tierNumber;
	}
	return 0;
}

void TextGridNavigator_modifyNavigationContextCriterions (TextGridNavigator me, integer tierNumber, kContext_combination combinationCriterion, bool matchContextOnly, kNavigatableTier_match matchCriterion) {
	const integer contextNumber = TextGridNavigator_getContextNumberFromTierNumber (me, tierNumber);
	Melder_require (contextNumber > 0,
		U"The tier number you specified has no navigation. ");
	const TierNavigationContext tnc = my tierNavigationContexts . at [contextNumber];
	const bool hasLeftContext = ( tnc -> beforeLabels && tnc ->  beforeLabels -> strings.size > 0 );
	const bool hasRightContext = ( tnc -> afterLabels && tnc -> afterLabels -> strings.size > 0 );
	if (combinationCriterion == kContext_combination::BEFORE)
		Melder_require (hasLeftContext,
			U"For this option you should have left context labels installed.");
	if (combinationCriterion == kContext_combination::AFTER)
		Melder_require (hasRightContext,
			U"For this option you should have right context labels installed.");
	if (combinationCriterion == kContext_combination::BEFORE_AND_AFTER || combinationCriterion == kContext_combination::BEFORE_OR_AFTER_NOT_BOTH || 
		combinationCriterion == kContext_combination::BEFORE_OR_AFTER_OR_BOTH)
		Melder_require (hasLeftContext && hasRightContext,
			U"For this option you should have left and right context labels installed.");
	if (matchContextOnly)
		Melder_require (hasLeftContext || hasRightContext,
			U"It is not possible to match only the context because you have neither left nor right context labels installed.");
	tnc -> matchContextOnly = matchContextOnly;
	tnc -> combinationCriterion = combinationCriterion;
	tnc -> matchCriterion = matchCriterion;
}

void TextGridNavigator_modifyLeftAndRightContextRange (TextGridNavigator me, integer tierNumber, integer leftContextFrom, integer leftContextTo, integer rightContextFrom, integer rightContextTo) {
	const integer contextNumber = TextGridNavigator_getContextNumberFromTierNumber (me, tierNumber);
	Melder_require (contextNumber > 0,
		U"The tier number you specified has no navigation. ");
	TierNavigationContext tnc = my tierNavigationContexts .at [contextNumber];
	Melder_require (leftContextFrom > 0 &&  leftContextTo > 0,
		U"The left context interval distance should be positive.");
	Melder_require (rightContextFrom > 0 && rightContextTo > 0,
		U"The right context interval distance should be positive.");
	tnc -> leftContextFrom = std::min (leftContextFrom, leftContextTo);
	tnc -> leftContextTo = std::max (leftContextTo, leftContextTo);
	tnc -> rightContextFrom = std::min (rightContextFrom, rightContextTo);
	tnc -> rightContextTo = std::max (rightContextFrom, rightContextTo);
}

void TextGridNavigator_modifyMatchingRange (TextGridNavigator me, integer tierNumber, integer maximumLookAhead, integer maximumLookBack) {
	const integer contextNumber = TextGridNavigator_getContextNumberFromTierNumber (me, tierNumber);
	Melder_require (contextNumber > 0,
		U"The tier number you specified has no navigation. ");
	TierNavigationContext tnc = my tierNavigationContexts . at [contextNumber];
	tnc -> maximumLookAhead = maximumLookAhead;
	tnc -> maximumLookBack = maximumLookBack;
}

static bool Tier_isNavigationMatch (Function me, integer index, TierNavigationContext tnc) {
	conststring32 label = tnc -> v_getLabel (me, index);
	return NavigationContext_isTopicLabel (tnc, label);
}

static integer Tier_getBeforeIndex (Function me, integer index, TierNavigationContext tnc) {
	if (! tnc -> beforeLabels)
		return 0;
	if (index - tnc -> leftContextFrom < 1 || index > tnc -> v_getSize (me))
		return 0;
	const integer startIndex = std::max (1_integer, index - tnc -> leftContextFrom);
	const integer endIndex = std::max (1_integer, index - tnc -> leftContextTo);
	for (integer i = startIndex; i >= endIndex; i --) {
		conststring32 label = tnc -> v_getLabel (me, i);
		if (NavigationContext_isBeforeLabel (tnc, label))
			return i;
	}
	return 0;
}

static integer Tier_getAfterIndex (Function me, integer index, TierNavigationContext tnc) {
	if (! tnc -> afterLabels)
		return 0;
	integer mySize = tnc -> v_getSize (me);
	if (index + tnc -> rightContextFrom > mySize || index < 1)
		return 0;
	const integer startInterval = std::min (mySize, index + tnc -> rightContextTo);
	const integer endInterval = std::min (mySize, index + tnc -> rightContextTo);
	for (integer i = startInterval; i <= endInterval; i ++) {
		conststring32 label = tnc -> v_getLabel (me, i);
		if (NavigationContext_isAfterLabel (tnc, label))
			return i;
	}
	return 0;
}

static inline bool Tier_isRightContextMatch (Function me, integer index, TierNavigationContext tnc) {
	return Tier_getAfterIndex (me, index, tnc) > 0;
}

static inline bool Tier_isLeftContextMatch (Function me, integer index, TierNavigationContext tnc) {
	return Tier_getBeforeIndex (me, index, tnc) > 0;
}

integer Tier_getNumberOfRightContextOnlyMatches (Function me, TierNavigationContext tnc) {
	if (tnc -> afterLabels -> numberOfStrings == 0)
		return 0;
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (me); index ++) {
		conststring32 label = tnc -> v_getLabel (me, index);
		if (NavigationContext_isAfterLabel (tnc, label))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

integer Tier_getNumberOfLeftContextOnlyMatches (Function me, TierNavigationContext tnc) {
	if (tnc -> beforeLabels -> numberOfStrings == 0)
		return 0;
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (me); index ++) {
		conststring32 label = tnc -> v_getLabel (me, index);
		if (NavigationContext_isBeforeLabel (tnc, label))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

integer Tier_getNumberOfNavigationOnlyMatches (Function me, TierNavigationContext tnc) {
	if (tnc -> topicLabels -> numberOfStrings == 0)
		return 0;
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (me); index ++) {
		conststring32 label = tnc -> v_getLabel (me, index);
		if (NavigationContext_isTopicLabel (tnc, label))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

static bool Tier_isLabelMatch (Function me, integer index, TierNavigationContext tnc) {
	if (index < 1 && index > tnc -> v_getSize (me))
		return false;
	const bool isNavigationMatch = ( tnc -> matchContextOnly ? true : Tier_isNavigationMatch (me, index, tnc) );
	if (! isNavigationMatch || tnc -> combinationCriterion == kContext_combination::NO_BEFORE_AND_NO_AFTER)
		return isNavigationMatch;
	bool isMatch = false;
	if (tnc -> combinationCriterion == kContext_combination::BEFORE_AND_AFTER)
		isMatch = Tier_isLeftContextMatch (me, index, tnc) &&
			Tier_isRightContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::AFTER)
		isMatch = Tier_isRightContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::BEFORE)
		isMatch = Tier_isLeftContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::BEFORE_OR_AFTER_OR_BOTH)
		isMatch = Tier_isLeftContextMatch (me, index, tnc) ||
			Tier_isRightContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::BEFORE_OR_AFTER_NOT_BOTH)
		isMatch = Tier_isLeftContextMatch (me, index, tnc) == ! Tier_isRightContextMatch (me, index, tnc);
	return isMatch;
}

integer Tier_getNumberOfMatches(Function me, TierNavigationContext tnc) {
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (me); index ++) {
		if (Tier_isLabelMatch (me, index, tnc))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

integer TextGridNavigator_getNumberOfMatchesInAContext (TextGridNavigator me, integer icontext) {
	Melder_require (icontext > 0 && icontext <= my tierNavigationContexts.size,
		U"The context number should be between 1 and ", my tierNavigationContexts.size, U".");
	const TierNavigationContext tnc = my tierNavigationContexts . at [icontext];
	const Function anyTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	const integer numberOfMatches = Tier_getNumberOfMatches (anyTier, tnc);
	return numberOfMatches;
}

integer TextGridNavigator_getNumberOfMatches (TextGridNavigator me) {
	const TierNavigationContext tnc = my tierNavigationContexts . at [1];
	const Function anyTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (anyTier); index ++)
		if (TextGridNavigator_isLabelMatch (me, index))
			numberOfMatches ++;
	return numberOfMatches;
}

bool TextGridNavigator_isLabelMatch (TextGridNavigator me, integer indexInNavigationTier) {
	const TierNavigationContext tnc1 = my tierNavigationContexts . at [1];
	const Function navigationTier = my textgrid -> tiers -> at [tnc1 -> tierNumber];
	if (! Tier_isLabelMatch (navigationTier, indexInNavigationTier, tnc1))
			return false;
	if (my tierNavigationContexts.size == 1)
		return true;
	/*
		We have a match at the navigation tier, now check the subordinate tiers
	*/
	const double startTime = tnc1 -> v_getStartTime (navigationTier, indexInNavigationTier);
	const double endTime = tnc1 -> v_getEndTime (navigationTier, indexInNavigationTier);
	const double midTime = 0.5 * (startTime + endTime);
	for (integer icontext = 2; icontext <= my tierNavigationContexts . size; icontext ++) {
		const TierNavigationContext tnc = my tierNavigationContexts . at [icontext];
		const Function anyTier = my textgrid -> tiers -> at [tnc -> tierNumber];
		const integer referenceIndex = tnc -> v_getIndexFromTime (anyTier, midTime);
		const integer tierSize = tnc -> v_getSize (anyTier);
		
		tnc -> current = 0;
		const integer startIndex = ( tnc -> maximumLookBack > 0 ? std::max (1_integer, referenceIndex - tnc -> maximumLookBack) : 1 );
		const integer endIndex = ( tnc -> maximumLookAhead > 0 ? std::min (referenceIndex + tnc -> maximumLookAhead, tierSize) : tierSize );
		if (tnc -> matchCriterion == kNavigatableTier_match::IS_SOMEWHERE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					tnc -> current = index;
					break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_BEFORE) {
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double endTime_sub = tnc -> v_getEndTime (anyTier, index);
					if (endTime_sub <= startTime) {
						tnc -> current = index;
						break;
					}
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::TOUCHES_BEFORE) {
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double endTime_sub = tnc -> v_getEndTime (anyTier, index);
					if (endTime_sub == startTime) {
						tnc -> current = index;
						break;
					} else if (endTime_sub < startTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::OVERLAPS_BEFORE) {
			// OVERLAPS_BEFORE	tmin2 < tmin && tmax2 <= tmax
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double endTime_sub = tnc -> v_getEndTime (anyTier, index);
					const double startTime_sub = tnc -> v_getStartTime (anyTier, index);
					if (startTime_sub < startTime && endTime_sub <= endTime ) {
						tnc -> current = index;
						break;
					} else if (endTime_sub < startTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_INSIDE) {
			if (Tier_isLabelMatch (anyTier, referenceIndex, tnc)) {
				const double endTime_sub = tnc -> v_getEndTime (anyTier, referenceIndex);
				const double startTime_sub = tnc -> v_getStartTime (anyTier, referenceIndex);
				if (startTime_sub >= startTime && endTime_sub <= endTime)
					tnc -> current = referenceIndex;
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::OVERLAPS_AFTER) {
			// OVERLAPS_AFTER	tmin2 >= tmin && tmax2 > tmax
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double endTime_sub = tnc -> v_getEndTime (anyTier, index);
					const double startTime_sub = tnc -> v_getStartTime (anyTier, index);
					if (startTime_sub >= startTime && endTime_sub > endTime) {
						tnc -> current = index;
						break;
					} else if (startTime_sub >= endTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::TOUCHES_AFTER) {
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double startTime_sub = tnc -> v_getStartTime (anyTier, index);
					if (startTime_sub == endTime) {
						tnc -> current = index;
						break;
					} else if (startTime_sub > endTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_AFTER) {
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double startTime_sub = tnc -> v_getStartTime (anyTier, index);
					if (startTime_sub >= endTime) {
						tnc -> current = index;
						break;
					}
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::OVERLAPS_BEFORE_AND_AFTER) {
			if (Tier_isLabelMatch (anyTier, referenceIndex, tnc)) {
				const double endTime_sub = tnc -> v_getEndTime (anyTier, referenceIndex);
				const double startTime_sub = tnc -> v_getStartTime (anyTier, referenceIndex);
				if (startTime_sub <= startTime && endTime_sub >= endTime)
					tnc -> current = referenceIndex;
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::TOUCHES_BEFORE_AND_AFTER) {
			if (Tier_isLabelMatch (anyTier, referenceIndex, tnc)) {
				const double endTime_sub = tnc -> v_getEndTime (anyTier, referenceIndex);
				const double startTime_sub = tnc -> v_getStartTime (anyTier, referenceIndex);
				if (startTime_sub == startTime && endTime_sub == endTime)
					tnc -> current = referenceIndex;
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_OUTSIDE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					const double endTime_sub = tnc -> v_getEndTime (anyTier, index);
					const double startTime_sub = tnc -> v_getStartTime (anyTier, index);
					if (endTime_sub <= startTime || startTime_sub >= endTime) {
						tnc -> current = index;
						break;
					}
				}
			}
		}
		if (tnc -> current == 0)
			return false;
	}
	return true;
}

static integer TextGridNavigator_setCurrentAtTime (TextGridNavigator me, double time) {
	const TierNavigationContext tnc = my tierNavigationContexts. at [1];
	const Function anyTier = my textgrid -> tiers-> at [tnc -> tierNumber];	
	const integer index = tnc -> v_getIndexFromTime (anyTier, time);
	tnc -> current = index;
	return index;
}

integer TextGridNavigator_locateNext (TextGridNavigator me) {
	const TierNavigationContext tnc = my tierNavigationContexts.at [1];
	const Function anyTier = my textgrid -> tiers-> at [tnc -> tierNumber];
	const integer current = tnc -> current, size = tnc -> v_getSize (anyTier);
	for (integer index = current + 1; index <= size; index ++) {
		if (TextGridNavigator_isLabelMatch (me, index)) {
			tnc -> current = index;
			return index;
		}
	}
	tnc -> current = size + 1; // offRight
	return tnc -> current;
}

integer TextGridNavigator_locateNextAfterTime (TextGridNavigator me, double time) {
	TextGridNavigator_setCurrentAtTime (me, time);
	return TextGridNavigator_locateNext (me);
}

integer TextGridNavigator_locatePrevious (TextGridNavigator me) {
	const TierNavigationContext tnc = my tierNavigationContexts.at [1];
	const integer current = tnc -> current;
	for (integer index = current - 1; index > 0; index --) {
		if (TextGridNavigator_isLabelMatch (me, index)) {
			tnc -> current = index;
			return index;
		}
	}
	tnc -> current = 0;
	return 0;
}

integer TextGridNavigator_locatePreviousBeforeTime (TextGridNavigator me, double time) {
	TextGridNavigator_setCurrentAtTime (me, time);
	return TextGridNavigator_locatePrevious (me);
}

double TextGridNavigator_getStartTime (TextGridNavigator me, integer contextNumber) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContexts.size,
		U"Your context number (", contextNumber, U") should not be larger than the number of contexts (", my tierNavigationContexts.size, U").");
	const TierNavigationContext tnc = my tierNavigationContexts.at [contextNumber];
	const Function navigationTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	return tnc -> v_getStartTime (navigationTier, tnc -> current);
}

double TextGridNavigator_getStartTime (TextGridNavigator me, integer contextNumber, kContext_where where) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContexts.size,
		U"Your context number (", contextNumber, U") should not be larger than the number of contexts (", my tierNavigationContexts.size, U").");
	const TierNavigationContext tnc = my tierNavigationContexts.at [contextNumber];
	const Function navigationTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	const integer index = ( where == kContext_where::TOPIC ? tnc -> current :
		where == kContext_where::BEFORE ? Tier_getBeforeIndex (navigationTier, tnc -> current, tnc) : 
		where == kContext_where::AFTER ? Tier_getAfterIndex (navigationTier, tnc -> current, tnc): 0);
	return tnc -> v_getStartTime (navigationTier, index);
}

conststring32 TextGridNavigator_getLabel (TextGridNavigator me, integer contextNumber, kContext_where where) {
	const integer index = TextGridNavigator_getIndex (me, contextNumber, where);
	const TierNavigationContext tnc = my tierNavigationContexts.at [contextNumber];
	const Function navigationTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	return tnc -> v_getLabel (navigationTier, index);
}

double TextGridNavigator_getEndTime (TextGridNavigator me, integer contextNumber, kContext_where where) {
	const integer index = TextGridNavigator_getIndex (me, contextNumber, where);
	const TierNavigationContext tnc = my tierNavigationContexts.at [contextNumber];
	const Function navigationTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	return tnc -> v_getEndTime (navigationTier, index);
}

integer TextGridNavigator_getIndex (TextGridNavigator me, integer contextNumber, kContext_where where) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContexts.size,
		U"Context number (", contextNumber, U") should not be larger than the number of contexts (", my tierNavigationContexts.size, U").");
	const TierNavigationContext tnc = my tierNavigationContexts.at [contextNumber];
	const Function navigationTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	const integer index = ( where == kContext_where::TOPIC ? tnc -> current :
		where == kContext_where::BEFORE ? Tier_getBeforeIndex (navigationTier, tnc -> current, tnc) : 
		where == kContext_where::AFTER ? Tier_getAfterIndex (navigationTier, tnc -> current, tnc): 0);
	return index;
}

/* End of file TextGridNavigator.cpp */
