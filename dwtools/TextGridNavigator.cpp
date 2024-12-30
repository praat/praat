/* TextGridNavigator.cpp
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
#include "TextGridNavigator.h"

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



Thing_implement (TextGridNavigator, Function, 0);

void structTextGridNavigator :: v1_info () {
	// skipping parent classes
	const integer topicTierNumber = tierNavigators.at [1] -> tierNumber;
	integer topicTierSize = tierNavigators.at [1] -> v_getSize ();
	MelderInfo_writeLine (U"Number of complete matches: ", TextGridNavigator_getNumberOfMatches (this),  U" (from ", topicTierSize, U")");
	for (integer item = 1; item <= tierNavigators.size; item ++) {
		const TextGridTierNavigator tn = tierNavigators.at [item];
		const integer tierNumber = tn -> tierNumber;
		MelderInfo_writeLine (U"Tier number: ", tierNumber, U" (item number: ", item, U")");
		tn -> v1_info ();
		MelderInfo_writeLine (U"\tMatch location to tier number ", topicTierNumber, U": ", 
			kMatchDomainAlignment_getText (tn -> matchDomainAlignment));
	}	
}

static void TextGridNavigator_checkTierNumberNotInUse (TextGridNavigator me, integer tierNumber) {
	for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
		TextGridTierNavigator tn = my tierNavigators.at [inum];
		Melder_require (tn -> tierNumber != tierNumber,
			U"The tiernumber you want to add is already in use.");
	}
}

autoTextGridNavigator TextGrid_and_NavigationContext_to_TextGridNavigator (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber, kMatchDomain matchDomain) {
	try {
		autoTextGridTierNavigator thee = TextGrid_and_NavigationContext_to_TextGridTierNavigator (textgrid, navigationContext, tierNumber, matchDomain);
		autoTextGridNavigator me = Thing_new (TextGridNavigator);
		Function_init (me.get(), textgrid -> xmin, textgrid -> xmax);
		my tierNavigators. addItem_move (thee.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigator could not be created from ", textgrid, U" and ", navigationContext);
	}
}

autoTextGridNavigator TextGridNavigator_createSimple (double xmin, double xmax, integer tierNumber,
	constSTRVEC const& topicLabels)
{
	autoTextGridNavigator me = Thing_new (TextGridNavigator);
	Function_init (me.get(), xmin, xmax);
	autoNavigationContext navigationContext = NavigationContext_createTopicOnly (topicLabels, kMelder_string::EQUAL_TO,
		kMatchBoolean::OR_);
	autoIntervalTier tier = IntervalTier_create (xmin, xmax);
	autoTextGridTierNavigator tgtn = TextGridTierNavigator_create (tier.get(), navigationContext.get(),kMatchDomain::TOPIC_START_TO_TOPIC_END);
	tgtn -> tierNumber = tierNumber;
	my tierNavigators. addItem_move (tgtn.move());
	return me;
}

autoTextGridNavigator TextGridTierNavigator_to_TextGridNavigator (TextGridTierNavigator me) {
	try {
		autoTextGridNavigator thee = Thing_new (TextGridNavigator);
		Function_init (thee.get(), my xmin, my xmax);
		autoTextGridTierNavigator tn = Data_copy (me);
		tn -> matchDomainAlignment = kMatchDomainAlignment::IS_ANYWHERE;
		thy tierNavigators. addItem_move (tn.move());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create TextGridNavigator.");
	}
}

autoTextGridNavigator TextGrid_to_TextGridNavigator_topicSearch (TextGrid me, integer tierNumber,
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean, kMatchDomain matchDomain)
{
	try {
		autoTextGridTierNavigator tn = TextGrid_to_TextGridTierNavigator_topic (me, tierNumber,
				topicLabels, topicCriterion, topicMatchBoolean, matchDomain);
		autoTextGridNavigator thee = TextGridTierNavigator_to_TextGridNavigator (tn.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create TextGridNavigator from TextGrid.");
	}
}

autoTextGridNavigator TextGrid_to_TextGridNavigator (TextGrid me, integer tierNumber, 
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	constSTRVEC const& beforeLabels, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean,
	constSTRVEC const& afterLabels, kMelder_string afterCriterion, kMatchBoolean afterMatchBoolean,
	kContext_combination combinationCriterion, bool excludeTopic, kMatchDomain matchDomain) {
try {
		autoNavigationContext navigationContext = NavigationContext_create (
			topicLabels, topicCriterion, topicMatchBoolean,
			beforeLabels, beforeCriterion, beforeMatchBoolean,
			afterLabels, afterCriterion, afterMatchBoolean,
			combinationCriterion, excludeTopic
		);
		autoTextGridTierNavigator thee = TextGrid_and_NavigationContext_to_TextGridTierNavigator (me, navigationContext.get(), tierNumber, matchDomain);
		autoTextGridNavigator him = TextGridTierNavigator_to_TextGridNavigator (thee.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": could not create TextGridTierNavigator from TextGrid.");	
	}
}

void TextGridNavigator_and_TextGrid_addSearchTier (TextGridNavigator me, TextGrid thee, integer tierNumber, 
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	constSTRVEC const& beforeLabels, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean,
	constSTRVEC const& afterLabels, kMelder_string afterCriterion, kMatchBoolean afterMatchBoolean,
	kContext_combination combinationCriterion, bool excludeTopic, kMatchDomain matchDomain, kMatchDomainAlignment matchDomainAlignment) {
	try {
		/*
			We can't require the domains of the navigator and the new tier to be equal because they might be slightly different.  
		*/
		TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
		TextGridNavigator_checkTierNumberNotInUse (me, tierNumber);
		autoNavigationContext navigationContext = NavigationContext_create (
			topicLabels, topicCriterion, topicMatchBoolean,
			beforeLabels, beforeCriterion, beforeMatchBoolean,
			afterLabels, afterCriterion, afterMatchBoolean,
			combinationCriterion, excludeTopic
		);
		autoTextGridTierNavigator him = TextGrid_and_NavigationContext_to_TextGridTierNavigator (thee, navigationContext.get(), tierNumber, matchDomain);
		TextGridNavigator_addTextGridTierNavigator (me, him.get(), matchDomainAlignment);
	} catch (MelderError) {
		Melder_throw (me, U": could not add search tier from TextGrid.");	
	}
}

void TextGridNavigator_and_TextGrid_addSearchTier_topicOnly (TextGridNavigator me, TextGrid thee, integer tierNumber, 
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	kMatchDomain matchDomain, kMatchDomainAlignment matchDomainAlignment)
{
	try {
		TextGridNavigator_and_TextGrid_addSearchTier (me, thee, tierNumber,
			topicLabels, topicCriterion, topicMatchBoolean,
			{ }, kMelder_string::EQUAL_TO, kMatchBoolean::OR_,
			{ }, kMelder_string::EQUAL_TO, kMatchBoolean::OR_,
			kContext_combination::NO_BEFORE_AND_NO_AFTER, false, matchDomain, matchDomainAlignment
		);
	} catch (MelderError) {
		Melder_throw (me, U": could not add search topic tier from TextGrid.");	
	}
}
	
void TextGridNavigator_addNewTierNavigation (TextGridNavigator me, TextGrid thee, NavigationContext navigationContext,
	integer tierNumber, kMatchDomain matchDomain, kMatchDomainAlignment matchDomainAlignment)
{
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (thee, tierNumber);
		TextGridNavigator_checkTierNumberNotInUse (me, tierNumber);
		autoTextGridTierNavigator tn = TextGrid_and_NavigationContext_to_TextGridTierNavigator (thee, navigationContext, tierNumber, matchDomain);
		tn -> matchDomainAlignment = matchDomainAlignment;
		my tierNavigators.addItem_move (tn.move());
	} catch (MelderError) {
		Melder_throw (me, U": a new tier navigator could not be added.");
	}
}
											 
void TextGridNavigator_addTextGridTierNavigator (TextGridNavigator me, TextGridTierNavigator thee, kMatchDomainAlignment matchDomainAlignment) {
	try {
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			TextGridTierNavigator tn = my tierNavigators.at [inum];
			Melder_require (tn -> tierNumber != thy tierNumber,
				U"The tiernumber of the tier you want tor add is already in use.");
		}
		autoTextGridTierNavigator tn = Data_copy (thee);
		tn -> matchDomainAlignment = matchDomainAlignment;
		my tierNavigators. addItem_move (tn.move());
	} catch (MelderError) {
		Melder_throw (me, U": could not add TextGridTierNavigator.");
	}
}

static integer TextGridNavigator_checkNavigatorNumberFromTierNumber (TextGridNavigator me, integer tierNumber) {
	integer navigatorNumber = 0;
	for (integer i = 1; i <= my tierNavigators.size; i ++)
		if (my tierNavigators.at [i] -> tierNumber == tierNumber) {
			navigatorNumber = i;
			break;
		}
	Melder_require (navigatorNumber > 0,
		U"Tier number (", tierNumber, U") not used.");
	return navigatorNumber;
}

void TextGridNavigator_replaceNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
		TextGridTierNavigator_replaceNavigationContext (tn, thee);
	} catch (MelderError) {
		Melder_throw (me, U": could not replace navigation context.");
	}
}

autoNavigationContext TextGridNavigator_extractNavigationContext (TextGridNavigator me, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];;
		return TextGridTierNavigator_extractNavigationContext (tn);
	} catch (MelderError) {
		Melder_throw (me, U": could not extract navigation context.");
	}
}

void TextGridNavigator_replaceTiers (TextGridNavigator me, TextGrid thee) {
	try {
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			const TextGridTierNavigator tn = my tierNavigators.at [inum];
			const integer tierNumber = tn -> tierNumber;
			Melder_require (tierNumber <= thy tiers -> size,
				U"The number of tiers in the TextGrid is too small.");
			Melder_require (thy tiers -> at [tierNumber] -> classInfo == tn -> tier -> classInfo, 
				U"The TextGrid should have the same kind of tiers at the same positions as one you want to replace.");
		}
		my xmin = thy xmin;
		my xmax = thy xmax;
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			const TextGridTierNavigator tn = my tierNavigators.at [inum];
			const integer tierNumber = tn -> tierNumber;
			TextGridTierNavigator_replaceTier (tn, thee, tierNumber);
		}
	} catch (MelderError) {
		Melder_throw (me, U": cannot reset with ", thee, U".");
	}
}

integer TextGridNavigator_getTierNumberFromTierNavigator (TextGridNavigator me, integer navigatorNumber) {
	Melder_require (navigatorNumber > 0 && navigatorNumber <= my tierNavigators.size,
		U"The context number should not exceed ",  my tierNavigators.size, U".");
	return my tierNavigators.at [navigatorNumber] -> tierNumber;
}

void TextGridNavigator_modifyBeforeRange (TextGridNavigator me, integer tierNumber, integer from, integer to) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyBeforeRange (tn, from, to);
	} catch (MelderError) {
		Melder_throw (me, U": Before range not changed.");
	}
}

void TextGridNavigator_modifyAfterRange (TextGridNavigator me, integer tierNumber, integer from, integer to) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyAfterRange (tn, from, to);
	} catch (MelderError) {
		Melder_throw (me, U": After range not changed.");
	}
}

void TextGridNavigator_modifyMatchDomainAlignment (TextGridNavigator me, integer tierNumber, kMatchDomainAlignment matchDomainAlignment) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		tn -> matchDomainAlignment = matchDomainAlignment;
	} catch (MelderError) {
		Melder_throw (me, U": location criterion not changed.");
	}
}

void TextGridNavigator_modifyMatchDomain (TextGridNavigator me, integer tierNumber, kMatchDomain matchDomain) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyMatchDomain (tn, matchDomain);
	} catch (MelderError) {
		Melder_throw (me, U": match domain not changed.");
	}
}

void TextGridNavigator_modifyTopicCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion, kMatchBoolean matchBoolean) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyTopicCriterion (tn, newCriterion, matchBoolean);
	} catch (MelderError) {
		Melder_throw (me, U": Topic criterion not changed.");
	}
}

void TextGridNavigator_modifyBeforeCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion, kMatchBoolean matchBoolean) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyBeforeCriterion (tn, newCriterion, matchBoolean);
	} catch (MelderError) {
		Melder_throw (me, U": Before criterion not changed.");
	}
}

void TextGridNavigator_modifyAfterCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion, kMatchBoolean matchBoolean) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyAfterCriterion (tn, newCriterion, matchBoolean);
	} catch (MelderError) {
		Melder_throw (me, U": After criterion not changed.");
	}
}

void TextGridNavigator_modifyCombinationCriterion (TextGridNavigator me, integer tierNumber, kContext_combination newCriterion, bool excludeTopicMatch) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyUseCriterion (tn, newCriterion, excludeTopicMatch);
	} catch (MelderError) {
		Melder_throw (me, U": Use criterion not changed.");
	}
}

integer TextGridNavigator_getNumberOfMatchesForTier (TextGridNavigator me, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		const integer numberOfMatches = TextGridTierNavigator_getNumberOfMatches (tn);
		return numberOfMatches;
	} catch (MelderError) {
		Melder_throw (me, U": number of matches for tier not counted.");
	}
}

integer TextGridNavigator_getNumberOfMatches (TextGridNavigator me) {
	try {
		const TextGridTierNavigator tn = my tierNavigators.at [1];
		integer numberOfMatches = 0;
		for (integer index = 1; index <= tn -> v_getSize (); index ++)
			if (TextGridNavigator_isMatch (me, index, nullptr, nullptr))
				numberOfMatches ++;
		return numberOfMatches;
	} catch (MelderError) {
		Melder_throw (me, U": number of matches not counted.");
	}
}

integer TextGridNavigator_getNumberOfTopicMatches (TextGridNavigator me, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
		return TextGridTierNavigator_getNumberOfTopicMatches (tn);
	} catch (MelderError) {
		Melder_throw (me, U": number of Topic matches not counted.");
	}
}

integer TextGridNavigator_getNumberOfBeforeMatches (TextGridNavigator me, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
		return TextGridTierNavigator_getNumberOfBeforeMatches (tn);
	} catch (MelderError) {
		Melder_throw (me, U": number of Before matches not counted.");
	}
}

integer TextGridNavigator_getNumberOfAfterMatches (TextGridNavigator me, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
		return TextGridTierNavigator_getNumberOfAfterMatches (tn);
	} catch (MelderError) {
		Melder_throw (me, U": number of After matches not counted.");
	}
}

bool TextGridNavigator_isMatch (TextGridNavigator me, integer topicIndex, integer *out_beforeIndex, integer *out_afterIndex) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	integer beforeIndex, afterIndex;
	if (! TextGridTierNavigator_isMatch (tn, topicIndex, & beforeIndex, & afterIndex))
			return false;
	if (my tierNavigators.size == 1) {
		if (out_beforeIndex)
			*out_beforeIndex = beforeIndex;
		if (out_afterIndex)
			*out_afterIndex = afterIndex;
		return true;
	}
	/*
		We have a match at the topic tier, now check the subordinate tiers
	*/
	double startTime, endTime;
	TextGridTierNavigator_getMatchDomain (tn, tn -> matchDomain, topicIndex, beforeIndex, afterIndex, & startTime, & endTime);
	const double midTime = 0.5 * (tn -> v_getStartTime (topicIndex) + tn -> v_getEndTime (topicIndex));
	for (integer inum = 2; inum <= my tierNavigators.size; inum ++) {
		const TextGridTierNavigator tni = my tierNavigators.at [inum];
		const integer referenceIndex = tni -> v_timeToIndex (midTime);
		const integer tierSize = tni -> v_getSize ();
		integer beforeIndex_sub, afterIndex_sub;
		double startTime_sub, endTime_sub;
		
		tni -> currentTopicIndex = 0;
		const integer startIndex = 1;
		const integer endIndex =  tierSize;
		const kMatchDomainAlignment matchDomainAlignment = tni -> matchDomainAlignment;
		const kMatchDomain matchDomain = tni -> matchDomain;
		if (matchDomainAlignment == kMatchDomainAlignment::IS_ANYWHERE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					tni -> currentTopicIndex = index;
					break;
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::IS_BEFORE) {
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, nullptr, & endTime_sub);
					if (endTime_sub <= startTime) {
						tni -> currentTopicIndex = index;
						break;
					}
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::TOUCHES_BEFORE) {
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, nullptr, & endTime_sub);
					if (endTime_sub == startTime) {
						tni -> currentTopicIndex = index;
						break;
					} else if (endTime_sub < startTime)
						break;
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::OVERLAPS_BEFORE) {
			// OVERLAPS_BEFORE	tmin2 < tmin && tmax2 <= tmax
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, & startTime_sub, & endTime_sub);
					if (startTime_sub < startTime && endTime_sub <= endTime ) {
						tni -> currentTopicIndex = index;
						break;
					} else if (endTime_sub < startTime)
						break;
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::IS_INSIDE) { // TODO checken of tmid correct is
			if (TextGridTierNavigator_isMatch (tni, referenceIndex, & beforeIndex_sub, & afterIndex_sub)) {
				TextGridTierNavigator_getMatchDomain (tni, matchDomain, referenceIndex, beforeIndex_sub, afterIndex_sub, & startTime_sub, & endTime_sub);
				if (startTime_sub >= startTime && endTime_sub <= endTime)
					tni -> currentTopicIndex = referenceIndex;
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::OVERLAPS_AFTER) {
			// OVERLAPS_AFTER	tmin2 >= tmin && tmax2 > tmax
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, & startTime_sub, & endTime_sub);
					if (startTime_sub >= startTime && endTime_sub > endTime) {
						tni -> currentTopicIndex = index;
						break;
					} else if (startTime_sub >= endTime)
						break;
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::TOUCHES_AFTER) {
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, & startTime_sub, nullptr);
					if (startTime_sub == endTime) {
						tni -> currentTopicIndex = index;
						break;
					} else if (startTime_sub > endTime)
						break;
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::IS_AFTER) {
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
					TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, & startTime_sub, nullptr);
					if (startTime_sub >= endTime) {
						tni -> currentTopicIndex = index;
						break;
					}
				}
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::OVERLAPS_BEFORE_AND_AFTER) {
			if (TextGridTierNavigator_isMatch (tni, referenceIndex, & beforeIndex_sub, & afterIndex_sub)) {
				TextGridTierNavigator_getMatchDomain (tni, matchDomain, referenceIndex, beforeIndex_sub, afterIndex_sub, & startTime_sub, & endTime_sub);
				if (startTime_sub <= startTime && endTime_sub >= endTime)
					tni -> currentTopicIndex = referenceIndex;
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::TOUCHES_BEFORE_AND_AFTER) {
			if (TextGridTierNavigator_isMatch (tni, referenceIndex, & beforeIndex_sub, & afterIndex_sub)) {
				TextGridTierNavigator_getMatchDomain (tni, matchDomain, referenceIndex, beforeIndex_sub, afterIndex_sub, & startTime_sub, & endTime_sub);
				if (startTime_sub == startTime && endTime_sub == endTime)
					tni -> currentTopicIndex = referenceIndex;
			}
		} else if (matchDomainAlignment == kMatchDomainAlignment::IS_OUTSIDE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index, & beforeIndex_sub, & afterIndex_sub)) {
				TextGridTierNavigator_getMatchDomain (tni, matchDomain, index, beforeIndex_sub, afterIndex_sub, & startTime_sub, & endTime_sub);
					if (endTime_sub <= startTime || startTime_sub >= endTime) {
						tni -> currentTopicIndex = index;
						break;
					}
				}
			}
		}
		if (tni -> currentTopicIndex == 0)
			return false;
	}
	if (out_beforeIndex)
		*out_beforeIndex = beforeIndex;
	if (out_afterIndex)
		*out_afterIndex = afterIndex;
	return true;
}

static integer TextGridNavigator_timeToLowIndex (TextGridNavigator me, double time) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	tn -> currentTopicIndex = tn -> v_timeToLowIndex (time);
	return tn -> currentTopicIndex;
}

static integer TextGridNavigator_timeToIndex (TextGridNavigator me, double time) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	tn -> currentTopicIndex = tn -> v_timeToIndex (time);
	return tn -> currentTopicIndex;
}

static integer TextGridNavigator_timeToHighIndex (TextGridNavigator me, double time) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	tn -> currentTopicIndex = tn -> v_timeToHighIndex (time);
	return tn -> currentTopicIndex;
}

integer TextGridNavigator_findNext (TextGridNavigator me) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	const integer currentTopicIndex = tn -> currentTopicIndex, size = tn -> v_getSize ();
	for (integer index = currentTopicIndex + 1; index <= size; index ++) {
		if (TextGridNavigator_isMatch (me, index, nullptr, nullptr)) {
			tn -> currentTopicIndex = index;
			return index;
		}
	}
	tn -> currentTopicIndex = size + 1; // offRight
	return tn -> currentTopicIndex;
}

integer TextGridNavigator_findNextAfterTime (TextGridNavigator me, double time) {
	TextGridNavigator_timeToIndex (me, time);
	return TextGridNavigator_findNext (me);
}

integer TextGridNavigator_findPrevious (TextGridNavigator me) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	const integer currentTopicIndex = tn -> currentTopicIndex;
	for (integer index = currentTopicIndex - 1; index > 0; index --) {
		if (TextGridNavigator_isMatch (me, index, nullptr, nullptr)) {
			tn -> currentTopicIndex = index;
			return index;
		}
	}
	tn -> currentTopicIndex = 0;
	return tn -> currentTopicIndex;
}

integer TextGridNavigator_findPreviousBeforeTime (TextGridNavigator me, double time) {
	TextGridNavigator_timeToIndex (me, time);;
	return TextGridNavigator_findPrevious (me);
}

double TextGridNavigator_getStartTime (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return tn -> v_getStartTime (index);
}

conststring32 TextGridNavigator_getLabel (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return tn -> v_getLabel (index);
}

double TextGridNavigator_getEndTime (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return tn -> v_getEndTime (index);
}

integer TextGridNavigator_getIndex (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators.at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return index;
}

autoINTVEC TextGridNavigator_listIndices (TextGridNavigator me, kContext_where where) {
	try {
		TextGridTierNavigator tn = my tierNavigators.at [1];
		autoINTVEC indices;
		integer position = 0;
		for (integer index = 1; index <= tn -> v_getSize(); index ++) {
			integer beforeIndex, afterIndex;
			if (TextGridNavigator_isMatch (me, index, & beforeIndex, & afterIndex)) {
				integer iwhere = (where == kContext_where::TOPIC ? index :
					where == kContext_where::BEFORE ? beforeIndex : afterIndex );
				indices.insert (++ position, iwhere);
			}
		}
		return indices;
	} catch (MelderError) {
		Melder_throw (me, U": could not list indices.");
	}	
}

autoVEC TextGridNavigator_listStartTimes (TextGridNavigator me, kContext_where where) {
	try {
		autoINTVEC indices = TextGridNavigator_listIndices (me, where);
		autoVEC startTimes = raw_VEC (indices.size);
		TextGridTierNavigator tn = my tierNavigators.at [1];
		for (integer i = 1; i <= indices.size; i ++)
			startTimes [i] = tn -> v_getStartTime (indices [i]);
		return startTimes;
	} catch (MelderError) {
		Melder_throw (me, U": could not list indices.");
	}	
}

autoVEC TextGridNavigator_listEndTimes (TextGridNavigator me, kContext_where where) {
	try {
		autoINTVEC indices = TextGridNavigator_listIndices (me, where);
		autoVEC endTimes = raw_VEC (indices.size);
		TextGridTierNavigator tn = my tierNavigators.at [1];
		for (integer i = 1; i <= indices.size; i ++)
			endTimes [i] = tn -> v_getEndTime (indices [i]);
		return endTimes;
	} catch (MelderError) {
		Melder_throw (me, U": could not list indices.");
	}	
}

autoSTRVEC TextGridNavigator_listLabels (TextGridNavigator me, kContext_where where) {
	try {
		autoINTVEC indices = TextGridNavigator_listIndices (me, where);
		autoSTRVEC labels = autoSTRVEC (indices.size);
		TextGridTierNavigator tn = my tierNavigators.at [1];
		for (integer i = 1; i <= indices.size; i ++)
			labels [i] = Melder_dup (tn -> v_getLabel (indices [i]));
		return labels;
	} catch (MelderError) {
		Melder_throw (me, U": could not list indices.");
	}	
}

autoMAT TextGridNavigator_listDomains (TextGridNavigator me, kMatchDomain matchDomain) {
	try {
		autoINTVEC topicIndices = TextGridNavigator_listIndices (me, kContext_where::TOPIC);
		autoINTVEC beforeIndices = TextGridNavigator_listIndices (me, kContext_where::BEFORE);
		autoINTVEC afterIndices = TextGridNavigator_listIndices (me, kContext_where::AFTER);
		Melder_assert (topicIndices.size == beforeIndices.size && beforeIndices.size == afterIndices.size);
		TextGridTierNavigator tn = my tierNavigators.at [1];
		autoMAT domains = raw_MAT (topicIndices.size, 2);
		for (integer i = 1; i <= topicIndices.size; i ++) {
			double startTime, endTime;
			TextGridTierNavigator_getMatchDomain (tn, matchDomain, topicIndices [i], beforeIndices [i], afterIndices [i], & startTime, & endTime);
			domains [i] [1] = startTime;
			domains [i] [2] = endTime;
		}
		return domains;
	} catch (MelderError) {
		Melder_throw (me, U": could not list indices.");
	}	
}

/* End of file TextGridNavigator.cpp */
