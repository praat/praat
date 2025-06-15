/* Editor_enums.h
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
 * pb 2007/12/05
 */

enums_begin (kEditor_writeNameAtTop, 0)
	enums_add (kEditor_writeNameAtTop, 0, NO, L"no")
	enums_add (kEditor_writeNameAtTop, 1, FAR, L"far")
	enums_add (kEditor_writeNameAtTop, 2, NEAR, L"near")
	/* For reading old preferences files: */
	enums_alt (kEditor_writeNameAtTop, NO, L"0")
	enums_alt (kEditor_writeNameAtTop, FAR, L"1")
	enums_alt (kEditor_writeNameAtTop, NEAR, L"2")
enums_end (kEditor_writeNameAtTop, 2, FAR)

/* End of file Editor_enums.h */
