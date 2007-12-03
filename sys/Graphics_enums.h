/* Graphics_enums.h
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
 * pb 2007/12/03
 */

enums_begin (kGraphics_horizontalAlignment, 0)
	enums_add (kGraphics_horizontalAlignment, LEFT, L"left", 0)
	enums_alt (kGraphics_horizontalAlignment, LEFT, L"Left")
	enums_add (kGraphics_horizontalAlignment, CENTRE, L"centre", 1)
	enums_alt (kGraphics_horizontalAlignment, CENTRE, L"Centre")
	enums_alt (kGraphics_horizontalAlignment, CENTRE, L"center")
	enums_alt (kGraphics_horizontalAlignment, CENTRE, L"Center")
	enums_add (kGraphics_horizontalAlignment, RIGHT, L"right", 2)
	enums_alt (kGraphics_horizontalAlignment, RIGHT, L"Right")
	/* For reading old preferences files: */
	enums_alt (kGraphics_horizontalAlignment, LEFT, L"0")
	enums_alt (kGraphics_horizontalAlignment, CENTRE, L"1")
	enums_alt (kGraphics_horizontalAlignment, RIGHT, L"2")
enums_end (kGraphics_horizontalAlignment, CENTRE, 2)

/* End of file Graphics_enums.h */
