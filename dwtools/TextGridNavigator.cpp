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

void structTextGridNavigator :: v_info () {
	const integer topicTierNumber = tierNavigators.at [1] -> tierNumber;
	integer topicTierSize = tierNavigators. at [1] -> v_getSize ();
	for (integer item = 1; item <= tierNavigators.size; item ++) {
		const TextGridTierNavigator tn = tierNavigators.at [item];
		const integer tierNumber = tn -> tierNumber;
		MelderInfo_writeLine (U"Tier number: ", tierNumber, U" (item number: ", item, U")");
		tn -> v_info ();
		MelderInfo_writeLine (U"\tNumber of matches on tier ", tierNumber, U":");
		MelderInfo_writeLine (U"\tMatch location to tier number ", topicTierNumber, U": ", 
			kMatchLocation_getText (tn -> matchLocation));
	}	
	MelderInfo_writeLine (U"Number of complete matches: ", TextGridNavigator_getNumberOfMatches (this),  U" (from ", topicTierSize, U")");
}

autoTextGridNavigator TextGrid_and_NavigationContext_to_TextGridNavigator (TextGrid textgrid, NavigationContext navigationContext, integer tierNumber, kMatchDomain matchDomain) {
	try {
		autoTextGridTierNavigator thee = TextGrid_and_NavigationContext_to_TextGridTierNavigator (textgrid, navigationContext, tierNumber, matchDomain);
		autoTextGridNavigator me = Thing_new (TextGridNavigator);
		Function_init (me.get(), textgrid -> xmin, textgrid -> xmax);
		my tierNavigators.addItem_move (thee.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigator could not be created from ", textgrid, U" and ", navigationContext);
	}
}

autoTextGridNavigator TextGridTierNavigator_to_TextGridNavigator (TextGridTierNavigator me) {
	try {
		autoTextGridNavigator thee = Thing_new (TextGridNavigator);
		autoTextGridTierNavigator tn = Data_copy (me);
		tn -> matchLocation = kMatchLocation::IS_SOMEWHERE;
		thy tierNavigators.addItem_move (tn.move());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create TextGridNavigator.");
	}
}

void TextGridNavigator_addNewTierNavigation (TextGridNavigator me, TextGrid thee, NavigationContext navigationContext, integer tierNumber, kMatchDomain matchDomain, kMatchLocation matchLocation) {
	try {
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			TextGridTierNavigator tn = my tierNavigators.at [inum];
			Melder_require (tn -> tierNumber != tierNumber,
				U"The tiernumber you want tor add is already in use.");
		}
		autoTextGridTierNavigator tn = TextGrid_and_NavigationContext_to_TextGridTierNavigator (thee, navigationContext, tierNumber, matchDomain);
		tn -> matchLocation = matchLocation;
		my tierNavigators.addItem_move (tn.move());
	} catch (MelderError) {
		Melder_throw (me, U": a new tier navigator could not be added.");
	}
}
											 
void TextGridNavigator_addTextGridTierNavigator (TextGridNavigator me, TextGridTierNavigator thee, kMatchLocation matchLocation) {
	try {
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			TextGridTierNavigator tn = my tierNavigators.at [inum];
			Melder_require (tn -> tierNumber != thy tierNumber,
				U"The tiernumber of the tier you want tor add is already in use.");
		}
		autoTextGridTierNavigator tn = Data_copy (thee);
		tn -> matchLocation = matchLocation;
		my tierNavigators.addItem_move (tn.move());
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

static void NavigationContext_checkUseCriterionCompliesWithMatchDomain (NavigationContext me, kMatchDomain matchDomain) {
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

void TextGridNavigator_replaceNavigationContext (TextGridNavigator me, NavigationContext thee, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];
		TextGridTierNavigator_replaceNavigationContext (tn, thee);
	} catch (MelderError) {
		Melder_throw (me, U": could not replace navigation context.");
	}
}

autoNavigationContext TextGridNavigator_extractNavigationContext (TextGridNavigator me, integer tierNumber) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];;
		return TextGridTierNavigator_extractNavigationContext (tn);
	} catch (MelderError) {
		Melder_throw (me, U": could not extract navigation context.");
	}
}

void TextGridNavigator_replaceTiers (TextGridNavigator me, TextGrid thee) {
	try {
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			const TextGridTierNavigator tn = my tierNavigators. at [inum];
			const integer tierNumber = tn -> tierNumber;
			Melder_require (tierNumber <= thy tiers -> size,
				U"The number of tiers in the TextGrid is too small.");
			Melder_require (thy tiers -> at [tierNumber] -> classInfo == tn -> tier -> classInfo, 
				U"The TextGrid should have the same kind of tiers at the same positions as one you want to replace.");
		}
		for (integer inum = 1; inum <= my tierNavigators.size; inum ++) {
			const TextGridTierNavigator tn = my tierNavigators. at [inum];
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

void TextGridNavigator_modifyTopicCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyTopicCriterion (tn, newCriterion);
	} catch (MelderError) {
		Melder_throw (me, U": Topic criterion not changed.");
	}
}

void TextGridNavigator_modifyBeforeCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyBeforeCriterion (tn, newCriterion);
	} catch (MelderError) {
		Melder_throw (me, U": Before criterion not changed.");
	}
}

void TextGridNavigator_modifyAfterCriterion (TextGridNavigator me, integer tierNumber, kMelder_string newCriterion) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyAfterCriterion (tn, newCriterion);
	} catch (MelderError) {
		Melder_throw (me, U": After criterion not changed.");
	}
}

void TextGridNavigator_modifyUseCriterion (TextGridNavigator me, integer tierNumber, kContext_use newCriterion, bool excludeTopicMatch) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyUseCriterion (tn, newCriterion, excludeTopicMatch);
	} catch (MelderError) {
		Melder_throw (me, U": Use criterion not changed.");
	}
}

void TextGridNavigator_modifyMatchingRange (TextGridNavigator me, integer tierNumber, integer maximumLookAhead, integer maximumLookBack) {
	try {
		const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
		const TextGridTierNavigator tn = my tierNavigators .at [navigatorNumber];
		TextGridTierNavigator_modifyMatchingRange (tn, maximumLookAhead,maximumLookBack);
	} catch (MelderError) {
		Melder_throw (me, U": Matching range not changed.");
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
			if (TextGridNavigator_isMatch (me, index))
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

static void TextGridNavigator_getMatchDomain (TextGridNavigator me, integer navigatorNumber, double *out_startTime, double *out_endTime) {
	Melder_require (navigatorNumber > 0 && navigatorNumber <= my tierNavigators.size,
		U"The navigator number should not exceed ", my tierNavigators.size, U".");
	const TextGridTierNavigator tn = my tierNavigators.at[navigatorNumber];
	kContext_where startWhere, endWhere;
	if (tn -> matchDomain == kMatchDomain::MATCH_START_TO_MATCH_END) {
		const NavigationContext nc = tn -> navigationContext.get();
		if (nc -> useCriterion == kContext_use::NO_BEFORE_AND_NO_AFTER) {
			startWhere = kContext_where::TOPIC;
			endWhere = kContext_where::TOPIC;
		} else if (nc -> useCriterion == kContext_use::BEFORE) {
			startWhere = kContext_where::BEFORE;
			endWhere = ( nc -> excludeTopicMatch ? kContext_where::BEFORE : kContext_where::TOPIC );
		} else if (nc -> useCriterion == kContext_use::AFTER) {
			startWhere = ( nc -> excludeTopicMatch ? kContext_where::AFTER : kContext_where::TOPIC );
			endWhere = kContext_where::AFTER;
		} else if (nc -> useCriterion == kContext_use::BEFORE_AND_AFTER) {
			startWhere = kContext_where::BEFORE;
			endWhere = kContext_where::AFTER;
		} else if (nc -> useCriterion == kContext_use::BEFORE_OR_AFTER_NOT_BOTH) {
			if (TextGridTierNavigator_isBeforeMatch (tn, tn -> currentTopicIndex)) {
				startWhere = kContext_where::BEFORE;
				endWhere = ( nc -> excludeTopicMatch ? kContext_where::BEFORE : kContext_where::TOPIC );
			} else {
				startWhere = ( nc -> excludeTopicMatch ? kContext_where::AFTER : kContext_where::TOPIC );
				endWhere = kContext_where::AFTER;
			}
		} else if (nc -> useCriterion == kContext_use::BEFORE_OR_AFTER_OR_BOTH) {
			bool isBeforeMatch = TextGridTierNavigator_isBeforeMatch (tn, tn -> currentTopicIndex);
			bool isAfterMatch = TextGridTierNavigator_isAfterMatch (tn, tn -> currentTopicIndex);
			if (isBeforeMatch && isAfterMatch) {
				startWhere = kContext_where::BEFORE;
				endWhere = kContext_where::AFTER;
			} else if (isBeforeMatch) {
				startWhere = kContext_where::BEFORE;
				endWhere = ( nc -> excludeTopicMatch ? kContext_where::BEFORE : kContext_where::TOPIC );
			} else {
				startWhere = ( nc -> excludeTopicMatch ? kContext_where::AFTER : kContext_where::TOPIC );
				endWhere = kContext_where::AFTER;
			}
		}
	} else if (tn -> matchDomain == kMatchDomain::TOPIC_START_TO_TOPIC_END) {
		startWhere = kContext_where::TOPIC;
		endWhere = kContext_where::TOPIC;
	} else if (tn -> matchDomain == kMatchDomain::BEFORE_START_TO_TOPIC_END) {
		startWhere = kContext_where::BEFORE;
		endWhere = kContext_where::TOPIC;
	} else if (tn -> matchDomain == kMatchDomain::TOPIC_START_TO_AFTER_END) {
		startWhere = kContext_where::TOPIC;
		endWhere = kContext_where::AFTER;
	} else if (tn -> matchDomain == kMatchDomain::BEFORE_START_TO_AFTER_END) {
		startWhere = kContext_where::BEFORE;
		endWhere = kContext_where::AFTER;
	} else if (tn -> matchDomain == kMatchDomain::BEFORE_START_TO_BEFORE_END) {
		startWhere = kContext_where::BEFORE;
		endWhere = kContext_where::BEFORE;
	} else if (tn -> matchDomain == kMatchDomain::AFTER_START_TO_AFTER_END) {
		startWhere = kContext_where::AFTER;
		endWhere = kContext_where::AFTER;
	}
	if (out_startTime)
		*out_startTime = TextGridNavigator_getStartTime (me, navigatorNumber, startWhere);
	if (out_endTime)
		*out_endTime = TextGridNavigator_getEndTime (me, navigatorNumber, endWhere);
}

bool TextGridNavigator_isMatch (TextGridNavigator me, integer indexInTopicTier) {
	const TextGridTierNavigator tn = my tierNavigators.at [1];
	if (! TextGridTierNavigator_isMatch (tn, indexInTopicTier))
			return false;
	if (my tierNavigators.size == 1)
		return true;
	/*
		We have a match at the topic tier, now check the subordinate tiers
	*/
	const double startTime = tn -> v_getStartTime (indexInTopicTier);
	const double endTime = tn -> v_getEndTime (indexInTopicTier);
	const double midTime = 0.5 * (startTime + endTime);
	for (integer inum = 2; inum <= my tierNavigators.size; inum ++) {
		const TextGridTierNavigator tni = my tierNavigators.at [inum];
		const integer referenceIndex = tni -> v_timeToIndex (midTime);
		const integer tierSize = tni -> v_getSize ();
		
		tni -> currentTopicIndex = 0;
		const integer startIndex = ( tni -> maximumLookBack > 0 ? std::max (1_integer, referenceIndex - tni -> maximumLookBack) : 1 );
		const integer endIndex = ( tni -> maximumLookAhead > 0 ? std::min (referenceIndex + tni -> maximumLookAhead, tierSize) : tierSize );
		const kMatchLocation matchLocation = tni -> matchLocation;
		if (matchLocation == kMatchLocation::IS_SOMEWHERE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					tni -> currentTopicIndex = index;
					break;
				}
			}
		} else if (matchLocation == kMatchLocation::IS_BEFORE) {
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double endTime_sub = tni -> v_getEndTime (index);
					if (endTime_sub <= startTime) {
						tni -> currentTopicIndex = index;
						break;
					}
				}
			}
		} else if (matchLocation == kMatchLocation::TOUCHES_BEFORE) {
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double endTime_sub = tni -> v_getEndTime (index);
					if (endTime_sub == startTime) {
						tni -> currentTopicIndex = index;
						break;
					} else if (endTime_sub < startTime)
						break;
				}
			}
		} else if (matchLocation == kMatchLocation::OVERLAPS_BEFORE) {
			// OVERLAPS_BEFORE	tmin2 < tmin && tmax2 <= tmax
			for (integer index = referenceIndex; index >= startIndex; index --) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double endTime_sub = tni -> v_getEndTime (index);
					const double startTime_sub = tni -> v_getStartTime (index);
					if (startTime_sub < startTime && endTime_sub <= endTime ) {
						tni -> currentTopicIndex = index;
						break;
					} else if (endTime_sub < startTime)
						break;
				}
			}
		} else if (matchLocation == kMatchLocation::IS_INSIDE) {
			if (TextGridTierNavigator_isMatch (tni, referenceIndex)) {
				const double endTime_sub = tni -> v_getEndTime (referenceIndex);
				const double startTime_sub = tni -> v_getStartTime (referenceIndex);
				if (startTime_sub >= startTime && endTime_sub <= endTime)
					tni -> currentTopicIndex = referenceIndex;
			}
		} else if (matchLocation == kMatchLocation::OVERLAPS_AFTER) {
			// OVERLAPS_AFTER	tmin2 >= tmin && tmax2 > tmax
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double endTime_sub = tni -> v_getEndTime (index);
					const double startTime_sub = tni -> v_getStartTime (index);
					if (startTime_sub >= startTime && endTime_sub > endTime) {
						tni -> currentTopicIndex = index;
						break;
					} else if (startTime_sub >= endTime)
						break;
				}
			}
		} else if (matchLocation == kMatchLocation::TOUCHES_AFTER) {
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double startTime_sub = tni -> v_getStartTime (index);
					if (startTime_sub == endTime) {
						tni -> currentTopicIndex = index;
						break;
					} else if (startTime_sub > endTime)
						break;
				}
			}
		} else if (matchLocation == kMatchLocation::IS_AFTER) {
			for (integer index = referenceIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double startTime_sub = tni -> v_getStartTime (index);
					if (startTime_sub >= endTime) {
						tni -> currentTopicIndex = index;
						break;
					}
				}
			}
		} else if (matchLocation == kMatchLocation::OVERLAPS_BEFORE_AND_AFTER) {
			if (TextGridTierNavigator_isMatch (tni, referenceIndex)) {
				const double endTime_sub = tni -> v_getEndTime (referenceIndex);
				const double startTime_sub = tni -> v_getStartTime (referenceIndex);
				if (startTime_sub <= startTime && endTime_sub >= endTime)
					tni -> currentTopicIndex = referenceIndex;
			}
		} else if (matchLocation == kMatchLocation::TOUCHES_BEFORE_AND_AFTER) {
			if (TextGridTierNavigator_isMatch (tni, referenceIndex)) {
				const double endTime_sub = tni -> v_getEndTime (referenceIndex);
				const double startTime_sub = tni -> v_getStartTime (referenceIndex);
				if (startTime_sub == startTime && endTime_sub == endTime)
					tni -> currentTopicIndex = referenceIndex;
			}
		} else if (matchLocation == kMatchLocation::IS_OUTSIDE) {
			for (integer index = startIndex; index <= endIndex; index ++) {
				if (TextGridTierNavigator_isMatch (tni, index)) {
					const double endTime_sub = tni -> v_getEndTime (index);
					const double startTime_sub = tni -> v_getStartTime (index);
					if (endTime_sub <= startTime || startTime_sub >= endTime) {
						tni -> currentTopicIndex = index;
						break;
					}
				}
			}
		}
		if (tn -> currentTopicIndex == 0)
			return false;
	}
	return true;
}

static integer TextGridNavigator_timeToLowIndex (TextGridNavigator me, double time) {
	const TextGridTierNavigator tn = my tierNavigators. at [1];
	return tn -> v_timeToLowIndex (time);
}

static integer TextGridNavigator_timeToIndex (TextGridNavigator me, double time) {
	const TextGridTierNavigator tn = my tierNavigators. at [1];
	return tn -> v_timeToIndex (time);
}

static integer TextGridNavigator_timeToHighIndex (TextGridNavigator me, double time) {
	const TextGridTierNavigator tn = my tierNavigators. at [1];
	return tn -> v_timeToHighIndex (time);
}

integer TextGridNavigator_locateNext (TextGridNavigator me) {
	const TextGridTierNavigator tn = my tierNavigators. at [1];
	const integer currentTopicIndex = tn -> currentTopicIndex, size = tn -> v_getSize ();
	for (integer index = currentTopicIndex + 1; index <= size; index ++) {
		if (TextGridNavigator_isMatch (me, index)) {
			tn -> currentTopicIndex = index;
			return index;
		}
	}
	tn -> currentTopicIndex = size + 1; // offRight
	return tn -> currentTopicIndex;
}

integer TextGridNavigator_locateNextAfterTime (TextGridNavigator me, double time) {
	TextGridNavigator_timeToIndex (me, time);
	return TextGridNavigator_locateNext (me);
}

integer TextGridNavigator_locatePrevious (TextGridNavigator me) {
	const TextGridTierNavigator tn = my tierNavigators. at [1];
	const integer currentTopicIndex = tn -> currentTopicIndex;
	for (integer index = currentTopicIndex - 1; index > 0; index --) {
		if (TextGridNavigator_isMatch (me, index)) {
			tn -> currentTopicIndex = index;
			return index;
		}
	}
	tn -> currentTopicIndex = 0;
	return tn -> currentTopicIndex;
}

integer TextGridNavigator_locatePreviousBeforeTime (TextGridNavigator me, double time) {
	TextGridNavigator_timeToIndex (me, time);;
	return TextGridNavigator_locatePrevious (me);
}

double TextGridNavigator_getStartTime (TextGridNavigator me, integer tierNumber) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];
	return tn -> v_getStartTime (tn -> currentTopicIndex);
}

double TextGridNavigator_getStartTime (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return tn -> v_getStartTime (index);
}

conststring32 TextGridNavigator_getLabel (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return tn -> v_getLabel (index);
}

double TextGridNavigator_getEndTime (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return tn -> v_getEndTime (index);
}

integer TextGridNavigator_getIndex (TextGridNavigator me, integer tierNumber, kContext_where where) {
	const integer navigatorNumber = TextGridNavigator_checkNavigatorNumberFromTierNumber (me, tierNumber);
	const TextGridTierNavigator tn = my tierNavigators. at [navigatorNumber];
	const integer index = TextGridTierNavigator_getIndex (tn, where); 
	return index;
}

/* End of file TextGridNavigator.cpp */
