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
	enums_add (kNavigatableTier_match, 1, IS_LEFT, U"is left")
	enums_add (kNavigatableTier_match, 2, TOUCHES_LEFT, U"touches left")
	enums_add (kNavigatableTier_match, 3, OVERLAPS_LEFT, U"overlaps left")
	enums_add (kNavigatableTier_match, 4, IS_INSIDE, U"is inside")
	enums_add (kNavigatableTier_match, 5, OVERLAPS_RIGHT, U"overlaps right")
	enums_add (kNavigatableTier_match, 6, TOUCHES_RIGHT, U"touches right")
	enums_add (kNavigatableTier_match, 7, IS_RIGHT, U"is right")
	enums_add (kNavigatableTier_match, 8, OVERLAPS_LEFT_AND_RIGHT, U"overlaps left and right")
	enums_add (kNavigatableTier_match, 9, TOUCHES_LEFT_AND_RIGHT, U"touches left and right")
	enums_add (kNavigatableTier_match,10, IS_OUTSIDE, U"is outside")
	enums_add (kNavigatableTier_match,11, IS_SOMEWHERE, U"is somewhere")
enums_end (kNavigatableTier_match, 11, OVERLAPS_LEFT_AND_RIGHT)

/* End of fileTextGridNavigator_enums_enums.h */
