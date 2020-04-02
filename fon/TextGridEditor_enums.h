/* TextGridEditor_enums.h
 *
 * Copyright (C) 1992-2005,2007,2013,2015,2016,2020 Paul Boersma
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

enums_begin (kTextGridEditor_showNumberOf, 1)
	enums_add (kTextGridEditor_showNumberOf, 1, NOTHING, U"nothing")
	enums_add (kTextGridEditor_showNumberOf, 2, INTERVALS_OR_POINTS, U"intervals or points")
	enums_add (kTextGridEditor_showNumberOf, 3, NONEMPTY_INTERVALS_OR_POINTS, U"non-empty intervals or points")
enums_end (kTextGridEditor_showNumberOf, 3, INTERVALS_OR_POINTS)

/* End of file TextGridEditor_enums.h */
