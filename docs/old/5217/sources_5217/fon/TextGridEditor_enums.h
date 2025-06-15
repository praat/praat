/* TextGridEditor_enums.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2007/12/07
 */

enums_begin (kTextGridEditor_showNumberOf, 1)
	enums_add (kTextGridEditor_showNumberOf, 1, NOTHING, L"nothing")
	enums_add (kTextGridEditor_showNumberOf, 2, INTERVALS_OR_POINTS, L"intervals or points")
	enums_add (kTextGridEditor_showNumberOf, 3, NONEMPTY_INTERVALS_OR_POINTS, L"non-empty intervals or points")
	/* For reading old preferences files: */
	enums_alt (kTextGridEditor_showNumberOf, NOTHING, L"1")
	enums_alt (kTextGridEditor_showNumberOf, INTERVALS_OR_POINTS, L"2")
	enums_alt (kTextGridEditor_showNumberOf, NONEMPTY_INTERVALS_OR_POINTS, L"3")
enums_end (kTextGridEditor_showNumberOf, 3, INTERVALS_OR_POINTS)

/* End of file TextGridEditor_enums.h */
