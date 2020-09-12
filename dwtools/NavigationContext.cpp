/* NavigationContext.cpp
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
	if (navigationLabels) {
		MelderInfo_writeLine (U"\tName: ", navigationLabels -> name.get());
		MelderInfo_writeLine (U"\tNumber of labels: ", navigationLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo navigation labels defined");
	}
	if (leftContextLabels) {
		MelderInfo_writeLine (U"\tLeft context name: ", leftContextLabels -> name.get());
		MelderInfo_writeLine (U"\tLeft criterion: ", kMelder_string_getText (leftContextCriterion));
		MelderInfo_writeLine (U"\tNumber of left context labels: ", leftContextLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo left context navigation labels defined");
	}
	if (rightContextLabels) {
		MelderInfo_writeLine (U"\tRight context name: ", rightContextLabels -> name.get());
		MelderInfo_writeLine (U"\tRight criterion: ", kMelder_string_getText (rightContextCriterion));
		MelderInfo_writeLine (U"\tNumber of right context labels: ", rightContextLabels -> strings.size);
	} else {
		MelderInfo_writeLine (U"\tNo right context navigation labels defined");
	}
	MelderInfo_writeLine (U"\tMatch context: ", kContext_combination_getText (combinationCriterion));
	MelderInfo_writeLine (U"\tMatch context only: ", ( matchContextOnly ? U"yes" : U"no" ));
}

void NavigationContext_init (NavigationContext me) {
	my navigationLabels = Thing_new (Strings);
	my leftContextLabels = Thing_new (Strings);
	my rightContextLabels = Thing_new (Strings);
}

autoNavigationContext NavigationContext_createNonEmptyItemNavigation () {
	try {
		autoNavigationContext me = Thing_new (NavigationContext);
		NavigationContext_init (me.get());
		Strings_insert (my navigationLabels.get(), 0, U"");
		my navigationCriterion = kMelder_string::NOT_EQUAL_TO;
		my combinationCriterion = kContext_combination::NO_LEFT_AND_NO_RIGHT;
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigationContext not created.");
	}
}

autoNavigationContext Strings_to_NavigationContext (Strings me, kMelder_string navigationCriterion) {
	try {
		autoNavigationContext thee = Thing_new (NavigationContext);
		NavigationContext_init (thee.get());
		thy navigationLabels = Data_copy (me);
		Thing_setName (thy navigationLabels.get(), my name.get());
		thy navigationCriterion = navigationCriterion;
		thy combinationCriterion = kContext_combination::NO_LEFT_AND_NO_RIGHT;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to NavigationContext.");
	}
}

autoNavigationContext NavigationContext_create (conststring32 name, conststring32 navigationName, conststring32 navigation_string, kMelder_string navigationCriterion, conststring32 leftContextName, conststring32 leftContext_string, kMelder_string leftContextCriterion, conststring32 rightContextName, conststring32 rightContext_string, kMelder_string rightContextCriterion, kContext_combination combinationCriterion, bool contextOnly) {
	try {
		autoNavigationContext me = Thing_new (NavigationContext);
		NavigationContext_init (me.get());
		my navigationLabels = Strings_createAsTokens (navigation_string, U" ");
		Thing_setName (my navigationLabels.get(), navigationName);
		my navigationCriterion = navigationCriterion;
		my leftContextLabels = Strings_createAsTokens (leftContext_string, U" ");
		Thing_setName (my leftContextLabels.get(), leftContextName);
		my leftContextCriterion = leftContextCriterion;
		my rightContextLabels = Strings_createAsTokens (rightContext_string, U" ");
		Thing_setName (my rightContextLabels.get(), rightContextName);
		my rightContextCriterion = rightContextCriterion;
		my combinationCriterion = combinationCriterion;
		my matchContextOnly = contextOnly;
		return me;
	} catch (MelderError) {
		Melder_throw (U"NavigationContext could not be created from vowels string.");
	}
}

void NavigationContext_modifyNavigationLabels (NavigationContext me, Strings labels, kMelder_string criterion) {
	try {
		my navigationLabels = Data_copy (labels);
		Thing_setName (my navigationLabels.get(), labels -> name.get());
		my navigationCriterion = criterion;
		if (my leftContextLabels) {
			if (my rightContextLabels)
				my combinationCriterion = kContext_combination::LEFT_AND_RIGHT;
			else
				my combinationCriterion = kContext_combination::LEFT;
		} else if (my rightContextLabels) {
			my combinationCriterion = kContext_combination::RIGHT;
		} else {
			my combinationCriterion = kContext_combination::NO_LEFT_AND_NO_RIGHT;
		}
	} catch (MelderError) {
		Melder_throw (me, U": cannot set navigation labels from ", labels, U".");
	}
}

void NavigationContext_modifyLeftContextLabels (NavigationContext me, Strings labels, kMelder_string criterion) {
	try {
		my leftContextLabels = Data_copy (labels);
		Thing_setName (my leftContextLabels.get(), labels -> name.get());
		my leftContextCriterion = criterion;
		my combinationCriterion = kContext_combination::LEFT;
		if (! my navigationLabels)
			my matchContextOnly = true;
		if (my rightContextLabels)
			my combinationCriterion = kContext_combination::LEFT_AND_RIGHT;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set left context labels from ", labels, U".");
	}
}

void NavigationContext_modifyRightContextLabels (NavigationContext me, Strings labels, kMelder_string criterion) {
	try {
		my rightContextLabels = Data_copy (labels);
		Thing_setName (my rightContextLabels.get(), labels -> name.get());
		my rightContextCriterion = criterion;
		my combinationCriterion = kContext_combination::LEFT;
		if (! my navigationLabels)
			my matchContextOnly = true;
		if (my leftContextLabels)
			my combinationCriterion = kContext_combination::LEFT_AND_RIGHT;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set right context labels from ", labels, U".");
	}
}

void NavigationContext_modifyContextCombination (NavigationContext me, kContext_combination combinationCriterion, bool matchContextOnly) {
	bool hasLeftContext = ( my leftContextLabels && my leftContextLabels -> strings.size > 0 );
	bool hasRightContext = ( my rightContextLabels && my rightContextLabels -> strings.size > 0 );
	if (combinationCriterion == kContext_combination::LEFT)
		Melder_require (hasLeftContext,
			U"For this option the NavigationContext should have left context labels.");
	if (combinationCriterion == kContext_combination::RIGHT)
		Melder_require (hasRightContext,
			U"For this option the NavigationContext should have right context labels.");
	if (combinationCriterion == kContext_combination::LEFT_AND_RIGHT || combinationCriterion == kContext_combination::LEFT_OR_RIGHT_NOT_BOTH || 
		combinationCriterion == kContext_combination::LEFT_OR_RIGHT_OR_BOTH)
		Melder_require (hasLeftContext && hasRightContext,
			U"For this option the NavigationContext should have left and right context labels.");
	if (matchContextOnly)
		Melder_require (hasLeftContext || hasRightContext,
			U"For this option the NavigationContext should have left or right context labels.");
	my matchContextOnly = matchContextOnly;
	my combinationCriterion = combinationCriterion;
}

bool NavigationContext_isNavigationLabel (NavigationContext me, conststring32 label) {
	return ( my navigationLabels && STRVEChasMatch (my navigationLabels -> strings.get(), my navigationCriterion, label) );
}

bool NavigationContext_isLeftContextLabel (NavigationContext me, conststring32 label) {
	return ( my leftContextLabels && STRVEChasMatch (my leftContextLabels -> strings.get(), my leftContextCriterion, label) );
}

bool NavigationContext_isRightContextLabel (NavigationContext me, conststring32 label) {
	return ( my rightContextLabels && STRVEChasMatch (my rightContextLabels -> strings.get(), my rightContextCriterion, label) );
}

/* End of file NavigationContext.cpp */
