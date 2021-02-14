/* TextGridNavigator_enums.h
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

enums_begin (kNavigatableTier_match, 0)
	enums_add (kNavigatableTier_match, 1, IS_BEFORE, U"is before")
	enums_add (kNavigatableTier_match, 2, TOUCHES_BEFORE, U"touches before")
	enums_add (kNavigatableTier_match, 3, OVERLAPS_BEFORE, U"overlaps before")
	enums_add (kNavigatableTier_match, 4, IS_INSIDE, U"is inside")
	enums_add (kNavigatableTier_match, 5, OVERLAPS_AFTER, U"overlaps after")
	enums_add (kNavigatableTier_match, 6, TOUCHES_AFTER, U"touches after")
	enums_add (kNavigatableTier_match, 7, IS_AFTER, U"is after")
	enums_add (kNavigatableTier_match, 8, OVERLAPS_BEFORE_AND_AFTER, U"overlaps before and after")
	enums_add (kNavigatableTier_match, 9, TOUCHES_BEFORE_AND_AFTER, U"touches before and after")
	enums_add (kNavigatableTier_match,10, IS_OUTSIDE, U"is outside")
	enums_add (kNavigatableTier_match,11, IS_SOMEWHERE, U"is somewhere")
enums_end (kNavigatableTier_match, 11, OVERLAPS_BEFORE_AND_AFTER)

/* End of fileTextGridNavigator_enums_enums.h */
