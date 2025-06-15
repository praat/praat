/* abcio_enums.h
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2009/03/18
 */

enums_begin (kBoolean, 0)
	enums_add (kBoolean, 0, FALSE, L"false")
	enums_add (kBoolean, 1, TRUE, L"true")
enums_end (kBoolean, 1, FALSE)

enums_begin (kQuestion, 0)
	enums_add (kQuestion, 0, NO, L"no")
	enums_add (kQuestion, 1, YES, L"yes")
enums_end (kQuestion, 1, NO)

enums_begin (kExistence, 0)
	enums_add (kExistence, 0, ABSENT, L"absent")
	enums_add (kExistence, 1, EXISTS, L"exists")
enums_end (kExistence, 1, ABSENT)

/* End of file abcio_enums.h */
