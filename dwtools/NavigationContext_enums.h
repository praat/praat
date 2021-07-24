/* NavigationContext_enums.h
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

enums_begin (kContext_combination, 1)
	enums_add (kContext_combination, 1, BEFORE, U"before")
	enums_add (kContext_combination, 2, AFTER, U"after")
	enums_add (kContext_combination, 3, BEFORE_AND_AFTER, U"before and after")
	enums_add (kContext_combination, 4, BEFORE_OR_AFTER_NOT_BOTH, U"before or after, not both")
	enums_add (kContext_combination, 5, BEFORE_OR_AFTER_OR_BOTH, U"before or after or both")
	enums_add (kContext_combination, 6, NO_BEFORE_AND_NO_AFTER, U"no before and no after")
enums_end (kContext_combination, 6, NO_BEFORE_AND_NO_AFTER) 

enums_begin (kContext_where, 1)
	enums_add (kContext_where, 1, TOPIC, U"topic")
	enums_add (kContext_where, 2, BEFORE, U"before")
	enums_add (kContext_where, 3, AFTER, U"after")
enums_end (kContext_where, 3, TOPIC)

enums_begin (kMatchBoolean, 1)
	enums_add (kMatchBoolean, 1, AND_, U"AND")
	enums_add (kMatchBoolean, 2, OR_, U"OR")
enums_end (kMatchBoolean, 2, OR_)

/* End of file NavigationContext_enums.h */
