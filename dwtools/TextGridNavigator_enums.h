/* TextGridNavigator_enums.h
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

enums_begin (kContextCombination, 1)
	enums_add (kContextCombination, 1, BEFORE, U"left")
	enums_add (kContextCombination, 2, AFTER, U"right")
	enums_add (kContextCombination, 3, BEFORE_AND_AFTER, U"left and right")
	enums_add (kContextCombination, 4, BEFORE_OR_AFTER_NOT_BOTH, U"left or right, not both")
	enums_add (kContextCombination, 5, BEFORE_OR_AFTER_OR_BOTH, U"left or right or both")
	enums_add (kContextCombination, 6, NO_BEFORE_AND_NO_AFTER, U"no left and no right")
enums_end (kContextCombination, 6, NO_BEFORE_AND_NO_AFTER)

enums_begin (kNavigatableTier_location, 0)
	enums_add (kNavigatableTier_location, 1, IS_BEFORE, U"is before")
	enums_add (kNavigatableTier_location, 2, TOUCHES_BEFORE, U"touches before")
	enums_add (kNavigatableTier_location, 3, OVERLAPS_BEFORE, U"overlaps before")
	enums_add (kNavigatableTier_location, 4, IS_INSIDE, U"is inside")
	enums_add (kNavigatableTier_location, 5, OVERLAPS_AFTER, U"overlaps after")
	enums_add (kNavigatableTier_location, 6, TOUCHES_AFTER, U"touches after")
	enums_add (kNavigatableTier_location, 7, IS_AFTER, U"is after")
	enums_add (kNavigatableTier_location, 8, OVERLAPS_BEFORE_AND_AFTER, U"overlaps before and after")
	enums_add (kNavigatableTier_location, 9, TOUCHES_BEFORE_AND_AFTER, U"touches before and after")
	enums_add (kNavigatableTier_location,10, IS_OUTSIDE, U"is outside")
	enums_add (kNavigatableTier_location,11, IS_SOMEWHERE, U"is somewhere")
enums_end (kNavigatableTier_location, 11, OVERLAPS_BEFORE_AND_AFTER)

/* End of fileTextGridNavigator_enums_enums.h */
