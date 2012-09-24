/* TableOfReal_def.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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


#define ooSTRUCT TableOfReal
oo_DEFINE_CLASS (TableOfReal, Data)

	oo_LONG (numberOfRows)
	oo_LONG (numberOfColumns)
	oo_STRING_VECTOR (rowLabels, numberOfRows)
	oo_STRING_VECTOR (columnLabels, numberOfColumns)
	oo_DOUBLE_MATRIX (data, numberOfRows, numberOfColumns)

	#if oo_DECLARING
		// overridden methods:
		virtual void v_info ();
		virtual bool v_hasGetNrow     () { return true; }   virtual double        v_getNrow     () { return numberOfRows; }
		virtual bool v_hasGetNcol     () { return true; }   virtual double        v_getNcol     () { return numberOfColumns; }
		virtual bool v_hasGetRowStr   () { return true; }   virtual const wchar_t * v_getRowStr   (long irow);
		virtual bool v_hasGetColStr   () { return true; }   virtual const wchar_t * v_getColStr   (long icol);
		virtual bool v_hasGetMatrix   () { return true; }   virtual double        v_getMatrix   (long irow, long icol);
		virtual bool v_hasGetRowIndex () { return true; }   virtual double        v_getRowIndex (const wchar_t *rowLabel);
		virtual bool v_hasGetColIndex () { return true; }   virtual double        v_getColIndex (const wchar_t *columnLabel);
	#endif

oo_END_CLASS (TableOfReal)
#undef ooSTRUCT


/* End of file TableOfReal_def.h */
