/* TableOfReal_def.h
 *
 * Copyright (C) 1992-2005,2007,2011,2012,2015-2018 Paul Boersma
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


#define ooSTRUCT TableOfReal
oo_DEFINE_CLASS (TableOfReal, Daata)

	oo_INTEGER (numberOfRows)
	oo_INTEGER (numberOfColumns)
	oo_STRING_VECTOR (rowLabels, numberOfRows)
	oo_STRING_VECTOR (columnLabels, numberOfColumns)
	oo_MAT (data, numberOfRows, numberOfColumns)

	#if oo_DECLARING
		void v_info ()
			override;
		bool v_hasGetNrow ()
			override { return true; }
		double v_getNrow ()
			override { return numberOfRows; }
		bool v_hasGetNcol ()
			override { return true; }
		double v_getNcol ()
			override { return numberOfColumns; }
		bool v_hasGetRowStr ()
			override { return true; }
		conststring32 v_getRowStr (integer irow)
			override;
		bool v_hasGetColStr ()
			override { return true; }
		conststring32 v_getColStr (integer icol)
			override;
		bool v_hasGetMatrix ()
			override { return true; }
		double v_getMatrix (integer irow, integer icol)
			override;
		bool v_hasGetRowIndex ()
			override { return true; }
		double v_getRowIndex (conststring32 rowLabel)
			override;
		bool v_hasGetColIndex ()
			override { return true; }
		double v_getColIndex (conststring32 columnLabel)
			override;
	#endif

oo_END_CLASS (TableOfReal)
#undef ooSTRUCT


/* End of file TableOfReal_def.h */
