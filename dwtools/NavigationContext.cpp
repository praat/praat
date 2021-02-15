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
		if so we are done. If not check the second element etc until we get a match (or not).
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
	MelderInfo_writeLine (U"Navigation:");
	if (topicLabels) {
		MelderInfo_writeLine (U"\tName: ", topicLabels -> name.get());
		MelderInfo_writeLine (U"\tNumber of labels: ", topicLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo topic labels defined");
	}
	if (beforeLabels) {
		MelderInfo_writeLine (U"\tBefore name: ", beforeLabels -> name.get());
		MelderInfo_writeLine (U"\tBefore criterion: ", kMelder_string_getText (beforeCriterion));
		MelderInfo_writeLine (U"\tNumber of before labels: ", beforeLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo before labels defined");
	}
	if (afterLabels) {
		MelderInfo_writeLine (U"\tAfter name: ", afterLabels -> name.get());
		MelderInfo_writeLine (U"\tAfter criterion: ", kMelder_string_getText (afterCriterion));
		MelderInfo_writeLine (U"\tNumber of after labels: ", afterLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo after labels defined");
	}
	MelderInfo_writeLine (U"\tBefore and after use: ", kContext_combination_getText (combinationCriterion));
	MelderInfo_writeLine (U"\tExclude topic match: ", ( excludeTopicMatch ? U"yes" : U"no" ));
}

void NavigationContext_init (NavigationContext me) {
	my topicLabels = Thing_new (Strings);
	my beforeLabels = Thing_new (Strings);
	my afterLabels = Thing_new (Strings);
}

autoNavigationContext NavigationContext_createNonEmptyItemNavigation () {
	try {
		autoNavigationContext me = Thing_new (NavigationContext);
		NavigationContext_init (me.get());
		Strings_insert (my topicLabels.get(), 0, U"");
		my topicCriterion = kMelder_string::NOT_EQUAL_TO;
		my combinationCriterion = kContext_combination::NO_BEFORE_AND_NO_AFTER;
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigationContext not created.");
	}
}

autoNavigationContext Strings_to_NavigationContext (Strings me, kMelder_string topicCriterion) {
	try {
		autoNavigationContext thee = Thing_new (NavigationContext);
		NavigationContext_init (thee.get());
		thy topicLabels = Data_copy (me);
		Thing_setName (thy topicLabels.get(), my name.get());
		thy topicCriterion = topicCriterion;
		thy combinationCriterion = kContext_combination::NO_BEFORE_AND_NO_AFTER;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to NavigationContext.");
	}
}

autoNavigationContext NavigationContext_create (conststring32 name, conststring32 topicName, conststring32 topic_string, kMelder_string topicCriterion, conststring32 beforeName, conststring32 before_string, kMelder_string beforeCriterion, conststring32 afterName, conststring32 after_string, kMelder_string afterCriterion, kContext_combination combinationCriterion, bool contextOnly) {
	try {
		autoNavigationContext me = Thing_new (NavigationContext);
		NavigationContext_init (me.get());
		my topicLabels = Strings_createAsTokens (topic_string, U" ");
		Thing_setName (my topicLabels.get(), topicName);
		my topicCriterion = topicCriterion;
		my beforeLabels = Strings_createAsTokens (before_string, U" ");
		Thing_setName (my beforeLabels.get(), beforeName);
		my beforeCriterion = beforeCriterion;
		my afterLabels = Strings_createAsTokens (after_string, U" ");
		Thing_setName (my afterLabels.get(), afterName);
		my afterCriterion = afterCriterion;
		my combinationCriterion = combinationCriterion;
		my excludeTopicMatch = contextOnly;
		return me;
	} catch (MelderError) {
		Melder_throw (U"NavigationContext could not be created from vowels string.");
	}
}

void NavigationContext_modifyTopicLabels (NavigationContext me, Strings labels, kMelder_string criterion) {
	try {
		my topicLabels = Data_copy (labels);
		Thing_setName (my topicLabels.get(), labels -> name.get());
		my topicCriterion = criterion;
		if (my beforeLabels) {
			if (my afterLabels)
				my combinationCriterion = kContext_combination::BEFORE_AND_AFTER;
			else
				my combinationCriterion = kContext_combination::BEFORE;
		} else if (my afterLabels) {
			my combinationCriterion = kContext_combination::AFTER;
		} else {
			my combinationCriterion = kContext_combination::NO_BEFORE_AND_NO_AFTER;
		}
	} catch (MelderError) {
		Melder_throw (me, U": cannot set topic labels from ", labels, U".");
	}
}

void NavigationContext_modifyBeforeLabels (NavigationContext me, Strings labels, kMelder_string criterion) {
	try {
		my beforeLabels = Data_copy (labels);
		Thing_setName (my beforeLabels.get(), labels -> name.get());
		my beforeCriterion = criterion;
		my combinationCriterion = kContext_combination::BEFORE;
		if (! my topicLabels)
			my excludeTopicMatch = true;
		if (my afterLabels)
			my combinationCriterion = kContext_combination::BEFORE_AND_AFTER;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set before labels from ", labels, U".");
	}
}

void NavigationContext_modifyAfterLabels (NavigationContext me, Strings labels, kMelder_string criterion) {
	try {
		my afterLabels = Data_copy (labels);
		Thing_setName (my afterLabels.get(), labels -> name.get());
		my afterCriterion = criterion;
		my combinationCriterion = kContext_combination::BEFORE;
		if (! my topicLabels)
			my excludeTopicMatch = true;
		if (my beforeLabels)
			my combinationCriterion = kContext_combination::BEFORE_AND_AFTER;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set after labels from ", labels, U".");
	}
}

void NavigationContext_modifyContextCombination (NavigationContext me, kContext_combination combinationCriterion, bool excludeTopicMatch) {
	bool hasBefore = ( my beforeLabels && my beforeLabels -> strings.size > 0 );
	bool hasAfter = ( my afterLabels && my afterLabels -> strings.size > 0 );
	if (combinationCriterion == kContext_combination::BEFORE)
		Melder_require (hasBefore,
			U"For this option the NavigationContext should have before labels.");
	if (combinationCriterion == kContext_combination::AFTER)
		Melder_require (hasAfter,
			U"For this option the NavigationContext should have after labels.");
	if (combinationCriterion == kContext_combination::BEFORE_AND_AFTER || combinationCriterion == kContext_combination::BEFORE_OR_AFTER_NOT_BOTH || 
		combinationCriterion == kContext_combination::BEFORE_OR_AFTER_OR_BOTH)
		Melder_require (hasBefore && hasAfter,
			U"For this option the NavigationContext should have before and after labels.");
	if (excludeTopicMatch)
		Melder_require (hasBefore || hasAfter,
			U"For this option the NavigationContext should have before or after labels.");
	my excludeTopicMatch = excludeTopicMatch;
	my combinationCriterion = combinationCriterion;
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
