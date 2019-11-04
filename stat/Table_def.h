/* Table_def.h
 *
 * Copyright (C) 2002-2007,2011,2012,2014-2019 Paul Boersma
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


#define ooSTRUCT TableCell
oo_DEFINE_STRUCT (TableCell)

	oo_STRING (string)

	#if oo_DECLARING || oo_COPYING
		oo_DOUBLE (number)
	#endif

oo_END_STRUCT (TableCell)
#undef ooSTRUCT


#define ooSTRUCT TableRow
oo_DEFINE_CLASS (TableRow, Daata)

	oo_INTEGER (numberOfColumns)
	oo_STRUCTVEC (TableCell, cells, numberOfColumns)

	#if oo_DECLARING || oo_COPYING
		oo_INTEGER (sortingIndex)
	#endif

oo_END_CLASS (TableRow)
#undef ooSTRUCT


#define ooSTRUCT TableColumnHeader
oo_DEFINE_STRUCT (TableColumnHeader)

	oo_STRING (label)

	#if oo_DECLARING || oo_COPYING
		oo_INT16 (numericized)
	#endif

oo_END_STRUCT (TableColumnHeader)
#undef ooSTRUCT


#define ooSTRUCT Table
oo_DEFINE_CLASS (Table, Daata)

	oo_INTEGER (numberOfColumns)
	oo_STRUCTVEC (TableColumnHeader, columnHeaders, numberOfColumns)
	oo_COLLECTION_OF (OrderedOf, rows, TableRow, 0)

	#if oo_DECLARING
		void v_info ()
			override;
		bool v_hasGetNrow ()
			override { return true; }
		double v_getNrow ()
			override { return rows.size; }
		bool v_hasGetNcol ()
			override { return true; }
		double v_getNcol ()
			override { return numberOfColumns; }
		bool v_hasGetColStr ()
			override { return true; }
		conststring32 v_getColStr (integer columnNumber)
			override;
		bool v_hasGetMatrix ()
			override { return true; }
		double v_getMatrix (integer rowNumber, integer columnNumber)
			override;
		bool v_hasGetMatrixStr ()
			override { return true; }
		conststring32 v_getMatrixStr (integer rowNumber, integer columnNumber)
			override;
		bool v_hasGetColIndex ()
			override { return true; }
		double v_getColIndex (conststring32 columnLabel)
			override;
	#endif

oo_END_CLASS (Table)
#undef ooSTRUCT


/* End of file Table_def.h */
