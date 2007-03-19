/* TableOfReal_def.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1997/03/01
 * pb 2002/07/16 GPL
 */


#define ooSTRUCT TableOfReal
oo_DEFINE_CLASS (TableOfReal, Data)

	oo_LONG (numberOfRows)
	oo_LONG (numberOfColumns)
	oo_STRING_VECTOR (rowLabels, my numberOfRows)
	oo_STRING_VECTOR (columnLabels, my numberOfColumns)
	oo_DOUBLE_MATRIX (data, my numberOfRows, my numberOfColumns)

oo_END_CLASS (TableOfReal)
#undef ooSTRUCT


/* End of file TableOfReal_def.h */
