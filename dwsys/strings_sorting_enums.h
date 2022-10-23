/* strings_sorting_enums.h
 *
 * Copyright (C) 2022 David Weenink
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

enums_begin (kStrings_sorting, 0)
	enums_add (kStrings_sorting, 0, NONE, U"None")
	enums_add (kStrings_sorting, 1, ALPHABETICAL, U"alphabetical")
	enums_add (kStrings_sorting, 2, NATURAL, U"natural")
//	enums_add (kStrings_sorting, 2, ALPHABETICAL_NOCASE, U"alphabetical (no case)")
//	enums_add (kStrings_sorting, 4, NUMERICAL_PREFIX, U"numerical prefix") // 0a, 1.2b, 3a
//	enums_add (kStrings_sorting, 5, NUMERICAL_SUFFIX, U"numerical suffix") // 0a, 4s, s1, 
//	enums_add (kStrings_sorting, 6, WEEKDAY_SHORT, U"weekdays") // Mon, Tue, Wed, ..
//	enums_add (kStrings_sorting, 7, MONTH_SHORT, U"Month") // Jan, Feb, Mar, ...
//	enums_add (kStrings_sorting, 8, SEASON, U"Season") // Spring, Summer, Autumn, Winter
//	enums_add (kStrings_sorting, 9, PART_OF_DAY, U"part of day") // Morning, Afternoon, Evening, Night
enums_end (kStrings_sorting, 2, NATURAL)

/* End of file strings_sorting_enums.h */
