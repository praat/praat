#ifndef _NavigationContext_h_
#define _NavigationContext_h_
/* NavigationContext.h
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

#include "Strings_.h"
#include "TextGrid.h"
#include "melder.h"

#include "NavigationContext_enums.h"

#include "NavigationContext_def.h"

autoNavigationContext NavigationContext_createTopicOnly (
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean
);

autoNavigationContext NavigationContext_createBeforeAndTopic (
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	constSTRVEC const& beforeLabels, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean
);

autoNavigationContext NavigationContext_create (
	constSTRVEC const& topicLabels, kMelder_string topicCriterion, kMatchBoolean topicMatchBoolean,
	constSTRVEC const& beforeLabels, kMelder_string beforeCriterion, kMatchBoolean beforeMatchBoolean,
	constSTRVEC const& afterLabels, kMelder_string afterCriterion, kMatchBoolean afterMatchBoolean,
	kContext_combination combinationCriterion, bool contextOnly
);

void NavigationContext_replaceTopicLabels (NavigationContext me, Strings labels);
void NavigationContext_replaceBeforeLabels (NavigationContext me, Strings labels);
void NavigationContext_replaceAfterLabels (NavigationContext me, Strings labels);

void NavigationContext_modifyTopicCriterion (NavigationContext me, kMelder_string newCriterion, kMatchBoolean matchBoolean);
void NavigationContext_modifyBeforeCriterion (NavigationContext me, kMelder_string newCriterion, kMatchBoolean matchBoolean);
void NavigationContext_modifyAfterCriterion (NavigationContext me, kMelder_string newCriterion, kMatchBoolean matchBoolean);

bool NavigationContext_isTopicLabel (NavigationContext me, conststring32 label);
bool NavigationContext_isBeforeLabel (NavigationContext me, conststring32 label);
bool NavigationContext_isAfterLabel (NavigationContext me, conststring32 label);

void NavigationContext_modifyUseCriterion (NavigationContext me, kContext_combination combinationCriterion, bool excludeTopicMatch);

#endif /* _NavigationContext_h_ */
