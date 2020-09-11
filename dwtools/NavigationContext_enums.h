/* NavigationContext_enums.h
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

enums_begin (kContext_combination, 1)
	enums_add (kContext_combination, 1, LEFT, U"left")
	enums_add (kContext_combination, 2, RIGHT, U"right")
	enums_add (kContext_combination, 3, LEFT_AND_RIGHT, U"left and right")
	enums_add (kContext_combination, 4, LEFT_OR_RIGHT_NOT_BOTH, U"left or right, not both")
	enums_add (kContext_combination, 5, LEFT_OR_RIGHT_OR_BOTH, U"left or right or both")
	enums_add (kContext_combination, 6, NO_LEFT_AND_NO_RIGHT, U"no left and no right")
enums_end (kContext_combination, 6, NO_LEFT_AND_NO_RIGHT)

/* End of file NavigationContext_enums.h */
