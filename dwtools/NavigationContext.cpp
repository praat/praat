/* NavigationContext.cpp
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Distributions_and_Strings.h"
#include "NavigationContext.h"
#include "NUM2.h"


#include "enums_getText.h"
#include "NavigationContext_enums.h"
#include "enums_getValue.h"
#include "NavigationContext_enums.h"
#include "Strings_extensions.h"

#include "oo_DESTROY.h"
#include "NavigationContext_def.h"
#include "oo_COPY.h"
#include "NavigationContext_def.h"
#include "oo_EQUAL.h"
#include "NavigationContext_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "NavigationContext_def.h"
#include "oo_WRITE_TEXT.h"
#include "NavigationContext_def.h"
#include "oo_READ_TEXT.h"
#include "NavigationContext_def.h"
#include "oo_WRITE_BINARY.h"
#include "NavigationContext_def.h"
#include "oo_READ_BINARY.h"
#include "NavigationContext_def.h"
#include "oo_DESCRIPTION.h"
#include "NavigationContext_def.h"


Thing_implement (NavigationContext, Daata, 0);

static bool STRVEChasMatch (constSTRVEC const& labels, conststring32 label, kMelder_string criterion, kMatchBoolean matchBoolean) {
	bool match = true;
	if (matchBoolean == kMatchBoolean::OR_) {
		for (integer istring = 1; istring <= labels.size; istring ++)
			if (Melder_stringMatchesCriterion (label, criterion, labels [istring], true))
				return true;
		match = false;
	} else { // matchBoolean == kMatchBoolean::AND_
		for (integer istring = 1; istring <= labels.size; istring ++)
			if (! (match = match && Melder_stringMatchesCriterion (label, criterion, labels [istring], true)))
				return false;
		match = true;
	}
	return match;
}

void structNavigationContext :: v_info () {
	if (topicLabels && topicLabels -> strings.size > 0) {
		MelderInfo_writeLine (U"\tTopic criterion: ", kMelder_string_getText (topicCriterion));
		MelderInfo_writeLine (U"\tTopic match boolean: ", kMatchBoolean_getText (topicMatchBoolean));
		MelderInfo_writeLine (U"\tNumber of Topic labels: ", topicLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo Topic labels defined");
	}
	if (beforeLabels && beforeLabels -> strings.size > 0) {
		MelderInfo_writeLine (U"\tBefore criterion: ", kMelder_string_getText (beforeCriterion));
		MelderInfo_writeLine (U"\tBefore match boolean: ", kMatchBoolean_getText (beforeMatchBoolean));
		MelderInfo_writeLine (U"\tNumber of Before labels: ", beforeLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo Before labels defined");
	}
	if (afterLabels && afterLabels -> strings.size > 0) {
		MelderInfo_writeLine (U"\tAfter criterion: ", kMelder_string_getText (afterCriterion));
		MelderInfo_writeLine (U"\tAfter match boolean: ", kMatchBoolean_getText (afterMatchBoolean));
		MelderInfo_writeLine (U"\tNumber of After labels: ", afterLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo After labels defined");
	}
	MelderInfo_writeLine (U"\tBefore and After use: ", kContext_use_getText (useCriterion));
	MelderInfo_writeLine (U"\tExclude topic match: ", ( excludeTopicMatch ? U"yes" : U"no" ));
}

autoNavigationContext NavigationContext_createTopicOnly (conststring32 topic_string, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean) {
	try {
		autoNavigationContext me = NavigationContext_create (topic_string, topicCriterion, topicMatchBoolean, U"", kMelder_string::EQUAL_TO, kMatchBoolean::OR_, U"", kMelder_string::EQUAL_TO, kMatchBoolean::OR_, kContext_use::NO_BEFORE_AND_NO_AFTER, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NavigationContext could not be created for Topic.");
	}
}

autoNavigationContext NavigationContext_createBeforeAndTopic (conststring32 topic_string, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean, conststring32 before_string, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean) {
	try {
		autoNavigationContext me = NavigationContext_create (topic_string, topicCriterion, topicMatchBoolean, before_string, beforeCriterion, kMatchBoolean::OR_, U"", kMelder_string::EQUAL_TO, kMatchBoolean::OR_, kContext_use::BEFORE, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NavigationContext could not be created for Topic.");
	}
}

static autoStrings Strings_createAsUniqueTokens (conststring32 strings, conststring32 where) {
	autoStrings me = Strings_createAsTokens (strings, U" ");
	autoStrings thee = Data_copy (me.get());
	Strings_sort (thee.get());
	for (integer istring = 2; istring <= thy numberOfStrings; istring ++)
		Melder_require (Melder_cmp (thy strings [istring].get(), thy strings [istring - 1].get()) != 0,
			U"The ", where, U" labels should be unique, however \"", thy strings [istring].get(), U"\" occurs more than once.");
	return me;
}

autoNavigationContext NavigationContext_create (conststring32 topic_string, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean, conststring32 before_string, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean, conststring32 after_string, kMelder_string afterCriterion, kMatchBoolean afterMatchBoolean, kContext_use useCriterion, bool excludeTopicMatch) {
	try {
		Melder_require (! (excludeTopicMatch && useCriterion == kContext_use::NO_BEFORE_AND_NO_AFTER), 
			U"You should not exclude Before & After & Topic from matching. One of the three should be included.");
		autoNavigationContext me = Thing_new (NavigationContext);
		my topicLabels = Strings_createAsUniqueTokens (topic_string, U"Topic");
		my topicCriterion = topicCriterion;
		my topicMatchBoolean = topicMatchBoolean;
		my beforeLabels = Strings_createAsUniqueTokens (before_string, U"Before");
		my beforeCriterion = beforeCriterion;
		my beforeMatchBoolean = beforeMatchBoolean;
		my afterLabels = Strings_createAsUniqueTokens (after_string, U"After");
		my afterCriterion = afterCriterion;
		my afterMatchBoolean = afterMatchBoolean;
		my useCriterion = useCriterion;
		if (useCriterion == kContext_use::BEFORE && before_string [0] == U'\0')
			Strings_insert (my beforeLabels.get(), 1, U"");
		else if (useCriterion == kContext_use::AFTER && after_string [0] == U'\0')
			Strings_insert (my afterLabels.get(), 1, U"");
		else if (useCriterion  == kContext_use::BEFORE_AND_AFTER || useCriterion == kContext_use::BEFORE_OR_AFTER_NOT_BOTH ||
			useCriterion == kContext_use::BEFORE_OR_AFTER_OR_BOTH) {
			if (before_string [0] == U'\0')
				Strings_insert (my beforeLabels.get(), 1, U"");
			if (after_string [0] == U'\0')
				Strings_insert (my afterLabels.get(), 1, U"");
		}
		my excludeTopicMatch = excludeTopicMatch;
		return me;
	} catch (MelderError) {
		Melder_throw (U"NavigationContext could not be created.");
	}
}

static void Strings_checkIfUniqueLabels (Strings me) {
	autoDistributions distributions = Strings_to_Distributions (me);
	Melder_require (distributions -> numberOfRows != my numberOfStrings,
		U"There should be no duplicate labels.");
}

void NavigationContext_replaceTopicLabels (NavigationContext me, Strings labels) {
	try {
		Strings_checkIfUniqueLabels (labels);
		my topicLabels = Data_copy (labels);
	} catch (MelderError) {
		Melder_throw (me, U": cannot replace Topic labels from ", labels, U".");
	}
}

void NavigationContext_replaceBeforeLabels (NavigationContext me, Strings labels) {
	try {
		Strings_checkIfUniqueLabels (labels);
		my beforeLabels = Data_copy (labels);
	} catch (MelderError) {
		Melder_throw (me, U": cannot replace Before labels from ", labels, U".");
	}
}

void NavigationContext_replaceAfterLabels (NavigationContext me, Strings labels) {
	try {
		Strings_checkIfUniqueLabels (labels);
		my afterLabels = Data_copy (labels);
	} catch (MelderError) {
		Melder_throw (me, U": cannot replace After labels from ", labels, U".");
	}
}

void NavigationContext_modifyTopicCriterion (NavigationContext me, kMelder_string criterion, kMatchBoolean matchBoolean) {
	Melder_require (my topicLabels && my topicLabels -> numberOfStrings > 0,
		U"There are no Topic labels.");
	my topicCriterion = criterion;
	my topicMatchBoolean = matchBoolean;
}

void NavigationContext_modifyBeforeCriterion (NavigationContext me, kMelder_string criterion, kMatchBoolean matchBoolean) {
	Melder_require (my beforeLabels && my beforeLabels -> numberOfStrings > 0,
		U"There are no Before labels.");
	my beforeCriterion = criterion;
	my beforeMatchBoolean = matchBoolean;
}

void NavigationContext_modifyAfterCriterion (NavigationContext me, kMelder_string criterion, kMatchBoolean matchBoolean) {
	Melder_require (my afterLabels && my afterLabels -> numberOfStrings > 0,
		U"There are no After labels.");
	my afterCriterion = criterion;
	my afterMatchBoolean = matchBoolean;
}

void NavigationContext_modifyUseCriterion (NavigationContext me, kContext_use useCriterion, bool excludeTopicMatch) {
	bool hasBefore = ( my beforeLabels && my beforeLabels -> strings.size > 0 );
	bool hasAfter = ( my afterLabels && my afterLabels -> strings.size > 0 );
	if (useCriterion == kContext_use::BEFORE)
		Melder_require (hasBefore,
			U"For the ", kContext_use_getText (useCriterion), U" criterion the NavigationContext should have Before labels.");
	if (useCriterion == kContext_use::AFTER)
		Melder_require (hasAfter,
			U"For the ", kContext_use_getText (useCriterion), U" criterion the NavigationContext should have After labels.");
	if (useCriterion == kContext_use::BEFORE_AND_AFTER || useCriterion == kContext_use::BEFORE_OR_AFTER_NOT_BOTH || 
		useCriterion == kContext_use::BEFORE_OR_AFTER_OR_BOTH)
		Melder_require (hasBefore && hasAfter,
			U"For the ", kContext_use_getText (useCriterion), U" criterion the NavigationContext should have Before and After labels.");
	if (excludeTopicMatch)
		Melder_require (hasBefore || hasAfter,
			U"For the ", kContext_use_getText (useCriterion), U" criterion the NavigationContext should have Before or After labels.");
	my excludeTopicMatch = excludeTopicMatch;
	my useCriterion = useCriterion;
}

bool NavigationContext_isTopicLabel (NavigationContext me, conststring32 label) {
	return ( my topicLabels && STRVEChasMatch (my topicLabels -> strings.get(), label, my topicCriterion, my topicMatchBoolean) );
}

bool NavigationContext_isBeforeLabel (NavigationContext me, conststring32 label) {
	return ( my beforeLabels && STRVEChasMatch (my beforeLabels -> strings.get(), label, my beforeCriterion, my beforeMatchBoolean) );
}

bool NavigationContext_isAfterLabel (NavigationContext me, conststring32 label) {
	return ( my afterLabels && STRVEChasMatch (my afterLabels -> strings.get(), label, my afterCriterion, my afterMatchBoolean) );
}

/* End of file NavigationContext.cpp */
