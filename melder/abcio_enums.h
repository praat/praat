/* abcio_enums.h
 *
 * Copyright (C) 1992-2009,2015,2017 Paul Boersma
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

enums_begin (kBoolean, 0)
	enums_add (kBoolean, 0, FALSE_, U"false")
	enums_add (kBoolean, 1, TRUE_, U"true")
enums_end (kBoolean, 1, FALSE_)

enums_begin (kQuestion, 0)
	enums_add (kQuestion, 0, NO_, U"no")
	enums_add (kQuestion, 1, YES_, U"yes")
enums_end (kQuestion, 1, NO_)

enums_begin (kExistence, 0)
	enums_add (kExistence, 0, ABSENT, U"absent")
	enums_add (kExistence, 1, EXISTS, U"exists")
enums_end (kExistence, 1, ABSENT)

/* End of file abcio_enums.h */
