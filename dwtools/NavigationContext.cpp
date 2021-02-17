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

/*
	1. To determine if an item is in a set, we can simply start to test whether it is equal to the first element
		if so we are done. If not check the second element etc. until we get a match (or not).
	2. To determine whether an item is not in the set, we need to reverse the test until it fails.
*/
static bool STRVEChasMatch (constSTRVEC const& labels, kMelder_string criterion, conststring32 label) {
	if (criterion == kMelder_string :: EQUAL_TO                   || criterion == kMelder_string :: CONTAINS ||
		criterion == kMelder_string :: STARTS_WITH                || criterion == kMelder_string :: ENDS_WITH ||
		criterion == kMelder_string :: CONTAINS_WORD              || criterion == kMelder_string :: CONTAINS_WORD_STARTING_WITH || 
		criterion == kMelder_string :: CONTAINS_WORD_ENDING_WITH  || criterion == kMelder_string :: CONTAINS_INK ||
		criterion == kMelder_string :: CONTAINS_INK_STARTING_WITH || criterion == kMelder_string :: CONTAINS_INK_ENDING_WITH) {
		for (integer istring = 1; istring <= labels.size; istring ++)
			if (Melder_stringMatchesCriterion (label, criterion, labels [istring], true))
				return true;
	} else {
		for (integer istring = 1; istring <= labels.size; istring ++)
			if (! Melder_stringMatchesCriterion (label, criterion, labels [istring], true))
				return false;
		return true;
	}
	return false;
}

void structNavigationContext :: v_info () {
	if (topicLabels && topicLabels -> strings.size > 0) {
		MelderInfo_writeLine (U"\tTopic criterion: ", kMelder_string_getText (topicCriterion));
		MelderInfo_writeLine (U"\tNumber of Topic labels: ", topicLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo Topic labels defined");
	}
	if (beforeLabels && beforeLabels -> strings.size > 0) {
		MelderInfo_writeLine (U"\tBefore criterion: ", kMelder_string_getText (beforeCriterion));
		MelderInfo_writeLine (U"\tNumber of Before labels: ", beforeLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo Before labels defined");
	}
	if (afterLabels && afterLabels -> strings.size > 0) {
		MelderInfo_writeLine (U"\tAfter criterion: ", kMelder_string_getText (afterCriterion));
		MelderInfo_writeLine (U"\tNumber of After labels: ", afterLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo After labels defined");
	}
	MelderInfo_writeLine (U"\tBefore and After use: ", kContext_use_getText (useCriterion));
	MelderInfo_writeLine (U"\tExclude topic match: ", ( excludeTopicMatch ? U"yes" : U"no" ));
}

autoNavigationContext Strings_to_NavigationContext (Strings me, kMelder_string topicCriterion) {
	try {
		autoNavigationContext thee = Thing_new (NavigationContext);
		thy topicLabels = Data_copy (me);
		thy topicCriterion = topicCriterion;
		thy useCriterion = kContext_use::NO_BEFORE_AND_NO_AFTER;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to NavigationContext.");
	}
}

autoNavigationContext NavigationContext_create (conststring32 topic_string, kMelder_string topicCriterion, conststring32 before_string, kMelder_string beforeCriterion, conststring32 after_string, kMelder_string afterCriterion, kContext_use useCriterion, bool contextOnly) {
	try {
		autoNavigationContext me = Thing_new (NavigationContext);
		my topicLabels = Strings_createAsTokens (topic_string, U" ");
		my topicCriterion = topicCriterion;
		my beforeLabels = Strings_createAsTokens (before_string, U" ");
		my beforeCriterion = beforeCriterion;
		my afterLabels = Strings_createAsTokens (after_string, U" ");
		my afterCriterion = afterCriterion;
		my useCriterion = useCriterion;
		my excludeTopicMatch = contextOnly;
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

void NavigationContext_modifyTopicCriterion (NavigationContext me, kMelder_string criterion) {
	Melder_require (my topicLabels && my topicLabels -> numberOfStrings > 0,
		U"There are no Topic labels.");
	my topicCriterion = criterion;
}

void NavigationContext_modifyBeforeCriterion (NavigationContext me, kMelder_string criterion) {
	Melder_require (my beforeLabels && my beforeLabels -> numberOfStrings > 0,
		U"There are no Before labels.");
	my beforeCriterion = criterion;
}

void NavigationContext_modifyAfterCriterion (NavigationContext me, kMelder_string criterion) {
	Melder_require (my afterLabels && my afterLabels -> numberOfStrings > 0,
		U"There are no After labels.");
	my afterCriterion = criterion;
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
	return ( my topicLabels && STRVEChasMatch (my topicLabels -> strings.get(), my topicCriterion, label) );
}

bool NavigationContext_isBeforeLabel (NavigationContext me, conststring32 label) {
	return ( my beforeLabels && STRVEChasMatch (my beforeLabels -> strings.get(), my beforeCriterion, label) );
}

bool NavigationContext_isAfterLabel (NavigationContext me, conststring32 label) {
	return ( my afterLabels && STRVEChasMatch (my afterLabels -> strings.get(), my afterCriterion, label) );
}

/* End of file NavigationContext.cpp */
