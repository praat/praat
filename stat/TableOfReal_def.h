/* TableOfReal_def.h
 *
 * Copyright (C) 1992-2005,2007,2011,2012,2015-2018,2022,2023 Paul Boersma
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
		void v1_info ()
			override;
		bool v_hasGetNrow () const
			override { return true; }
		double v_getNrow () const
			override { return numberOfRows; }
		bool v_hasGetNcol () const
			override { return true; }
		double v_getNcol () const
			override { return numberOfColumns; }
		bool v_hasGetRowStr () const
			override { return true; }
		conststring32 v_getRowStr (integer irow) const
			override;
		bool v_hasGetColStr () const
			override { return true; }
		conststring32 v_getColStr (integer icol) const
			override;
		bool v_hasGetMatrix () const
			override { return true; }
		double v_getMatrix (integer irow, integer icol) const
			override;
		bool v_hasGetRowIndex () const
			override { return true; }
		double v_getRowIndex (conststring32 rowLabel) const
			override;
		bool v_hasGetColIndex () const
			override { return true; }
		double v_getColIndex (conststring32 columnLabel) const
			override;
	#endif

oo_END_CLASS (TableOfReal)
#undef ooSTRUCT


/* End of file TableOfReal_def.h */
