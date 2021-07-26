/* TextGridTierNavigator_enums.h
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

enums_begin (kMatchDomain, 1)
	enums_add (kMatchDomain, 1, TOPIC_START_TO_TOPIC_END, U"Topic start to Topic end")
	enums_add (kMatchDomain, 2, MATCH_START_TO_MATCH_END, U"Match start to Match end")
	enums_add (kMatchDomain, 3, BEFORE_START_TO_TOPIC_END, U"Before start to Topic end")
	enums_add (kMatchDomain, 4, BEFORE_START_TO_AFTER_END, U"Before start to After end")
	enums_add (kMatchDomain, 5, TOPIC_START_TO_AFTER_END, U"Topic start to After end")
	enums_add (kMatchDomain, 6, BEFORE_START_TO_BEFORE_END, U"Before start to Before end")
	enums_add (kMatchDomain, 7, AFTER_START_TO_AFTER_END, U"After start to After end")
enums_end (kMatchDomain, 7, TOPIC_START_TO_TOPIC_END)

enums_begin (kMatchDomainAlignment, 1)
	enums_add (kMatchDomainAlignment, 1, IS_BEFORE, U"is before")
	enums_add (kMatchDomainAlignment, 2, TOUCHES_BEFORE, U"touches before")
	enums_add (kMatchDomainAlignment, 3, OVERLAPS_BEFORE, U"overlaps before")
	enums_add (kMatchDomainAlignment, 4, IS_INSIDE, U"is inside")
	enums_add (kMatchDomainAlignment, 5, OVERLAPS_AFTER, U"overlaps after")
	enums_add (kMatchDomainAlignment, 6, TOUCHES_AFTER, U"touches after")
	enums_add (kMatchDomainAlignment, 7, IS_AFTER, U"is after")
	enums_add (kMatchDomainAlignment, 8, OVERLAPS_BEFORE_AND_AFTER, U"overlaps before and after")
	enums_add (kMatchDomainAlignment, 9, TOUCHES_BEFORE_AND_AFTER, U"touches before and after")
	enums_add (kMatchDomainAlignment,10, IS_OUTSIDE, U"is outside")
	enums_add (kMatchDomainAlignment,11, IS_ANYWHERE, U"is anywhere")
enums_end (kMatchDomainAlignment, 11, OVERLAPS_BEFORE_AND_AFTER)

/* End of file TextGridTierNavigator_enums.h */
