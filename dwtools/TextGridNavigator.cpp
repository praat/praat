/* TextGridNavigator.cpp
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

void TierNavigationContext_init (TierNavigationContext me, NavigationContext thee, integer tierNumber ) {
	my navigationLabels = Data_copy (thy  navigationLabels.get());
	my navigationCriterion = thy  navigationCriterion;
	my leftContextLabels = Data_copy (thy  leftContextLabels.get());
	my leftContextCriterion = thy  leftContextCriterion;
	my rightContextLabels = Data_copy (thy  rightContextLabels.get());
	my rightContextCriterion = thy  rightContextCriterion;
	my combinationCriterion = thy  combinationCriterion;
	my matchContextOnly = thy  matchContextOnly;		
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

void TierNavigationContext_setItemOrientation (TierNavigationContext me, kNavigatableTier_match matchCriterion) {
	my matchCriterion = matchCriterion;
}

Thing_implement (TextGridNavigator, Function, 0);

void structTextGridNavigator :: v_info () {
	for (integer icontext = 1; icontext <= tierNavigationContext.size; icontext ++) {
	
	}
}

autoTextGridNavigator TextGridNavigator_create (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber) {
	try {
		autoTextGridNavigator me = Thing_new (TextGridNavigator);
		Function_init (me.get(), textgrid -> xmin, textgrid -> xmax);
		my textgrid = Data_copy (textgrid);
		TextGridNavigator_addNavigationContext (me.get(), navigationContext, tierNumber, kNavigatableTier_match::TOUCHES_LEFT_AND_RIGHT);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigator could not be created from ", textgrid, U" and ", navigationContext);
	}
}

static bool TextGridNavigator_isNavigatableTierInUse (TextGridNavigator me, integer tierNumber) {
	if (my tierNavigationContext.size == 0)
		return false;
	for (integer icontext = 1; icontext <= my tierNavigationContext. size; icontext ++)
		if (my tierNavigationContext. at [icontext] -> tierNumber == tierNumber)
			return true;
	return false;
}

static void TextGridNavigator_checkNavigatableTierIsNotInUse (TextGridNavigator me, integer tierNumber) {
	Melder_require (! TextGridNavigator_isNavigatableTierInUse (me, tierNumber),
		U": tier number ", tierNumber, U" is already in use.");
}

void TextGridNavigator_addNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber, kNavigatableTier_match matchCriterion) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (my textgrid.get(), tierNumber);
		TextGridNavigator_checkNavigatableTierIsNotInUse (me, tierNumber);
		autoTierNavigationContext tierNavigationContext;
		if (my textgrid -> tiers -> at [tierNumber] -> classInfo == classIntervalTier)
			tierNavigationContext = IntervalTierNavigationContext_create (thee, tierNumber);
		else
			tierNavigationContext = TextTierNavigationContext_create (thee, tierNumber);
		tierNavigationContext -> matchCriterion = matchCriterion;
		my tierNavigationContext.addItem_move (tierNavigationContext.move());
	} catch (MelderError) {
		Melder_throw (me, U": could not add navigation context ", thee, U".");
	}
}

void TextGridNavigator_replaceTextGrid (TextGridNavigator me, TextGrid thee) {
	try {
		Melder_require (thy tiers -> size == my textgrid -> tiers -> size,
			U"The TextGrid should have the same number of tiers as the one you want to replace (", my textgrid -> tiers->size, U").");
		for (integer icontext = 1; icontext <= my tierNavigationContext. size; icontext ++) {
			TierNavigationContext navigationContext = my tierNavigationContext. at [icontext];
			integer tierNumber = navigationContext -> tierNumber;
			Melder_require (thy tiers -> at [tierNumber] -> classInfo == my textgrid -> tiers -> at [tierNumber] -> classInfo, 
				U"The TextGrid should have the same kind of tiers at the same positions as the original you want to replace ");
		}
		my textgrid = Data_copy (thee);
		
		my tierNavigationContext. at [1] -> current = 0; // offLeft
	} catch (MelderError) {
		Melder_throw (me, U": cannot reset with ", thee, U".");
	}
}

integer TextGridNavigator_getTierNumberFromContextNumber (TextGridNavigator me, integer contextNumber) {
	Melder_require (contextNumber > 0 && contextNumber <= my tierNavigationContext . size,
		U"The context number should be between 1 and ", my tierNavigationContext . size, U".)");
	return my tierNavigationContext . at [contextNumber] -> tierNumber;
}

integer TextGridNavigator_getContextNumberFromTierNumber (TextGridNavigator me, integer tierNumber) {
	TextGrid_checkSpecifiedTierNumberWithinRange (my textgrid.get(), tierNumber);
	for (integer icontext = 1; icontext <= my tierNavigationContext . size; icontext ++) {
		TierNavigationContext tnc = my tierNavigationContext . at [icontext];
		if (tnc -> tierNumber == tierNumber)
			return tierNumber;
	}
	return 0;
}

void TextGridNavigator_modifyNavigationContextCriterions (TextGridNavigator me, integer tierNumber, kContext_combination combinationCriterion, bool matchContextOnly, kNavigatableTier_match matchCriterion) {
	const integer contextNumber = TextGridNavigator_getContextNumberFromTierNumber (me, tierNumber);
	Melder_require (contextNumber > 0,
		U"The tier number you specified has no navigation. ");
	const TierNavigationContext tnc = my tierNavigationContext . at [contextNumber];
	const bool hasLeftContext = ( tnc -> leftContextLabels && tnc ->  leftContextLabels -> strings.size > 0 );
	const bool hasRightContext = ( tnc -> rightContextLabels && tnc -> rightContextLabels -> strings.size > 0 );
	if (combinationCriterion == kContext_combination::LEFT)
		Melder_require (hasLeftContext,
			U"For this option you should have left context labels installed.");
	if (combinationCriterion == kContext_combination::RIGHT)
		Melder_require (hasRightContext,
			U"For this option you should have right context labels installed.");
	if (combinationCriterion == kContext_combination::LEFT_AND_RIGHT || combinationCriterion == kContext_combination::LEFT_OR_RIGHT_NOT_BOTH || 
		combinationCriterion == kContext_combination::LEFT_OR_RIGHT_OR_BOTH)
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
	TierNavigationContext tnc = my tierNavigationContext .at [contextNumber];
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
	TierNavigationContext tnc = my tierNavigationContext . at [contextNumber];
	tnc -> maximumLookAhead = maximumLookAhead;
	tnc -> maximumLookBack = maximumLookBack;
}

static bool Tier_isNavigationMatch (Function me, integer index, TierNavigationContext tnc) {
	conststring32 label = tnc -> v_getLabel (me, index);
	return NavigationContext_isNavigationLabel (tnc, label);
}

static bool Tier_isLeftContextMatch (Function me, integer index, TierNavigationContext tnc) {
	if (! tnc -> leftContextLabels)
		return false;
	if (index - tnc -> leftContextFrom < 1)
		return false;
	integer startIndex = std::max (1_integer, index - tnc -> leftContextFrom);
	integer endIndex = std::max (1_integer, index - tnc -> leftContextTo);
	for (integer i = startIndex; i >= endIndex; i --) {
		conststring32 label = tnc -> v_getLabel (me, i);
		if (NavigationContext_isLeftContextLabel (tnc, label))
			return true;
	}
	return false;
}

static bool Tier_isRightContextMatch (Function me, integer index, TierNavigationContext tnc) {
	if (! tnc -> rightContextLabels)
		return false;
	integer mySize = tnc -> v_getSize (me);
	if (index + tnc -> rightContextFrom > mySize)
		return false;
	const integer startInterval = std::min (mySize, index + tnc -> rightContextTo);
	const integer endInterval = std::min (mySize, index + tnc -> rightContextTo);
	for (integer i = startInterval; i <= endInterval; i ++) {
		conststring32 label = tnc -> v_getLabel (me, i);
		if (NavigationContext_isRightContextLabel (tnc, label))
			return true;
	}
	return false;
}

bool Tier_isLabelMatch (Function me, integer index, TierNavigationContext tnc) {
	if (index < 1 && index > tnc -> v_getSize (me))
		return false;
	const bool isNavigationMatch = ( tnc -> matchContextOnly ? true : Tier_isNavigationMatch (me, index, tnc) );
	if (! isNavigationMatch || tnc -> combinationCriterion == kContext_combination::NO_LEFT_AND_NO_RIGHT)
		return isNavigationMatch;
	bool isMatch = false;
	if (tnc -> combinationCriterion == kContext_combination::LEFT_AND_RIGHT)
		isMatch = Tier_isLeftContextMatch (me, index, tnc) &&
			Tier_isRightContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::RIGHT)
		isMatch = Tier_isRightContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::LEFT)
		isMatch = Tier_isLeftContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::LEFT_OR_RIGHT_OR_BOTH)
		isMatch = Tier_isLeftContextMatch (me, index, tnc) ||
			Tier_isRightContextMatch (me, index, tnc);
	else if (tnc ->  combinationCriterion == kContext_combination::LEFT_OR_RIGHT_NOT_BOTH)
		isMatch = Tier_isLeftContextMatch (me, index, tnc) == ! Tier_isRightContextMatch (me, index, tnc);
	return isMatch;
}

integer TextGridNavigator_getNumberOfMatchesInAContext (TextGridNavigator me, integer icontext) {
	Melder_require (icontext > 0 && icontext <= my tierNavigationContext.size,
		U"The context number should be between 1 and ", my tierNavigationContext.size, U".");
	TierNavigationContext tnc = my tierNavigationContext . at [icontext];
	Function anyTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (anyTier); index ++)
		if (Tier_isLabelMatch (anyTier, index, tnc))
			numberOfMatches ++;
	return numberOfMatches;
}

integer TextGridNavigator_getNumberOfMatches (TextGridNavigator me) {
	TierNavigationContext tnc = my tierNavigationContext . at [1];
	Function anyTier = my textgrid -> tiers -> at [tnc -> tierNumber];
	integer numberOfMatches = 0;
	for (integer index = 1; index <= tnc -> v_getSize (anyTier); index ++)
		if (TextGridNavigator_isLabelMatch (me, index))
			numberOfMatches ++;
	return numberOfMatches;
}

bool TextGridNavigator_isLabelMatch (TextGridNavigator me, integer indexInNavigationTier) {
	const TierNavigationContext tnc1 = my tierNavigationContext . at [1];
	const integer tierNumber1 = tnc1 -> tierNumber;
	const Function navigationTier = my textgrid -> tiers -> at [tierNumber1];
	if (! Tier_isLabelMatch (navigationTier, indexInNavigationTier, tnc1))
			return false;
	if (my tierNavigationContext.size == 1)
		return true;
	/*
		We have a match at the navigation tier, now check the subordinate tiers
	*/
	if (my tierNavigationContext.size == 1)
		return true;
	const double leftTime = tnc1 -> v_getLeftTime (navigationTier, indexInNavigationTier);
	const double rightTime = tnc1 -> v_getRightTime (navigationTier, indexInNavigationTier);
	const double midTime = 0.5 * (leftTime + rightTime);
	for (integer icontext = 2; icontext <= my tierNavigationContext . size; icontext ++) {
		const TierNavigationContext tnc = my tierNavigationContext . at [icontext];
		const integer tierNumber = tnc -> tierNumber;
		const Function anyTier = my textgrid -> tiers -> at [tierNumber];
		const integer myIndex = tnc -> v_getIndexFromTime (anyTier, midTime);
		const integer mySize = tnc -> v_getSize (anyTier);
		
		bool matchFound = false;
		const integer startIndex = ( tnc -> maximumLookBack > 0 ? std::max (1_integer, myIndex - tnc -> maximumLookBack) : 1 );
		const integer endIndex = ( tnc -> maximumLookAhead > 0 ? std::min (myIndex + tnc -> maximumLookAhead, mySize) : mySize );
		if (tnc -> matchCriterion == kNavigatableTier_match::IS_SOMEWHERE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					matchFound = true;
					break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_LEFT) {
			for (integer index = myIndex; index >= startIndex; index --) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double rightTime_sub = tnc -> v_getRightTime (anyTier, index);
					if (rightTime_sub < leftTime) {
						matchFound = true;
						break;
					}
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::TOUCHES_LEFT) {
			for (integer index = myIndex; index >= startIndex; index --) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double rightTime_sub = tnc -> v_getRightTime (anyTier, index);
					if (rightTime_sub == leftTime) {
						matchFound = true;
						break;
					} else if (rightTime_sub < leftTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::OVERLAPS_LEFT) {
			for (integer index = myIndex; index >= startIndex; index --) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double rightTime_sub = tnc -> v_getRightTime (anyTier, index);
					double leftTime_sub = tnc -> v_getLeftTime (anyTier, index);
					if (rightTime_sub > leftTime && leftTime_sub < leftTime) {
						matchFound = true;
						break;
					} else if (rightTime_sub < leftTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_INSIDE) {
			if (Tier_isLabelMatch (anyTier, myIndex, tnc)) {
				double rightTime_sub = tnc -> v_getRightTime (anyTier, myIndex);
				double leftTime_sub = tnc -> v_getLeftTime (anyTier, myIndex);
				if (leftTime_sub >= leftTime && rightTime_sub <= rightTime)
					matchFound = true;
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::OVERLAPS_RIGHT) {
			for (integer index = myIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double rightTime_sub = tnc -> v_getRightTime (anyTier, index);
					double leftTime_sub = tnc -> v_getLeftTime (anyTier, index);
					if (rightTime_sub > rightTime && leftTime_sub < rightTime) {
						matchFound = true;
						break;
					} else if (leftTime_sub >= rightTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::TOUCHES_RIGHT) {
			for (integer index = myIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double leftTime_sub = tnc -> v_getLeftTime (anyTier, index);
					if (leftTime_sub == rightTime) {
						matchFound = true;
						break;
					} else if (leftTime_sub > rightTime)
						break;
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_RIGHT) {
			for (integer index = myIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double leftTime_sub = tnc -> v_getLeftTime (anyTier, index);
					if (leftTime_sub > rightTime) {
						matchFound = true;
						break;
					}
				}
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::OVERLAPS_LEFT_AND_RIGHT) {
			if (Tier_isLabelMatch (anyTier, myIndex, tnc)) {
				double rightTime_sub = tnc -> v_getRightTime (anyTier, myIndex);
				double leftTime_sub = tnc -> v_getLeftTime (anyTier, myIndex);
				if (leftTime_sub <= leftTime && rightTime_sub >= rightTime)
					matchFound = true;
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::TOUCHES_LEFT_AND_RIGHT) {
			if (Tier_isLabelMatch (anyTier, myIndex, tnc)) {
				double rightTime_sub = tnc -> v_getRightTime (anyTier, myIndex);
				double leftTime_sub = tnc -> v_getLeftTime (anyTier, myIndex);
				if (leftTime_sub == leftTime && rightTime_sub == rightTime)
					matchFound = true;
			}
		} else if (tnc -> matchCriterion == kNavigatableTier_match::IS_OUTSIDE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (Tier_isLabelMatch (anyTier, index, tnc)) {
					double rightTime_sub = tnc -> v_getRightTime (anyTier, myIndex);
					double leftTime_sub = tnc -> v_getLeftTime (anyTier, myIndex);
					if (rightTime_sub < leftTime || leftTime_sub > rightTime) {
						matchFound = true;
						break;
					}
				}
			}
		}
		if (! matchFound)
			return false;
	}
	return false;
}

integer TextGridNavigator_setCurrentAtTime (TextGridNavigator me, double time) {
	const TierNavigationContext tnc = my tierNavigationContext. at [1];
	const Function anyTier = my textgrid -> tiers-> at [tnc -> tierNumber];	
	const integer index = tnc -> v_getIndexFromTime (anyTier, time);
	tnc -> current = index;
	return index;
}

integer TextGridNavigator_next (TextGridNavigator me) {
	const TierNavigationContext tnc = my tierNavigationContext.at [1];
	const Function anyTier = my textgrid -> tiers-> at [tnc -> tierNumber];
	const integer current = tnc -> current;
	for (integer index = current + 1; index <= tnc -> v_getSize (anyTier); index ++) {
		if (TextGridNavigator_isLabelMatch (me, index)) {
			tnc -> current = index;
			return index;
		}
	}
	return 0;
}

integer TextGridNavigator_nextFromTime (TextGridNavigator me, double time) {
	TextGridNavigator_setCurrentAtTime (me, time);
	return TextGridNavigator_next (me);
}

integer TextGridNavigator_previous (TextGridNavigator me) {
	const TierNavigationContext tnc = my tierNavigationContext.at [1];
	const integer current = tnc -> current;
	for (integer index = current - 1; index > 0; index --) {
		if (TextGridNavigator_isLabelMatch (me, index)) {
			tnc -> current = index;
			return index;
		}
	}
	return 0;
}

integer TextGridNavigator_previousFromTime (TextGridNavigator me, double time) {
	TextGridNavigator_setCurrentAtTime (me, time);
	return TextGridNavigator_previous (me);
}

/* End of file TextGridNavigator.cpp */
