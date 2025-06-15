/* Table_def.h
 *
 * Copyright (C) 2002-2007 Paul Boersma
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
 * pb 2007/08/10
 */


#define ooSTRUCT TableCell
oo_DEFINE_STRUCT (TableCell)

	oo_STRINGW (string)
	#if oo_DECLARING || oo_COPYING
		oo_DOUBLE (number)
	#endif

oo_END_STRUCT (TableCell)
#undef ooSTRUCT


#define ooSTRUCT TableRow
oo_DEFINE_CLASS (TableRow, Data)

	#if oo_DECLARING || oo_COPYING
		oo_LONG (sortingIndex)
	#endif
	oo_LONG (numberOfColumns)
	oo_STRUCT_VECTOR (TableCell, cells, my numberOfColumns)

oo_END_CLASS (TableRow)
#undef ooSTRUCT


#define ooSTRUCT TableColumnHeader
oo_DEFINE_STRUCT (TableColumnHeader)

	oo_STRINGW (label)
	#if oo_DECLARING || oo_COPYING
		oo_INT (numericized)
	#endif

oo_END_STRUCT (TableColumnHeader)
#undef ooSTRUCT


#define ooSTRUCT Table
oo_DEFINE_CLASS (Table, Data)

	oo_LONG (numberOfColumns)
	oo_STRUCT_VECTOR (TableColumnHeader, columnHeaders, my numberOfColumns)
	oo_COLLECTION (Ordered, rows, TableRow, 0)

oo_END_CLASS (Table)
#undef ooSTRUCT


/* End of file Table_def.h */
