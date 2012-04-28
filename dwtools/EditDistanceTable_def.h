/* EditDistanceTable_def.h
 *
 * Copyright (C) 2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20120406 First version
*/

#define ooSTRUCT PairOfInteger
oo_DEFINE_STRUCT (PairOfInteger)
	oo_LONG (x)
	oo_LONG (y)
oo_END_STRUCT (PairOfInteger)
#undef ooSTRUCT

#define ooSTRUCT WarpingPath
oo_DEFINE_CLASS (WarpingPath, Data)
	oo_LONG (_capacity)
	oo_LONG (d_pathLength)
	oo_STRUCT_VECTOR (PairOfInteger, d_path, d_pathLength)
oo_END_CLASS (WarpingPath)
#undef ooSTRUCT

#define ooSTRUCT EditCostsTable
oo_DEFINE_CLASS (EditCostsTable, TableOfReal)
	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
			virtual bool v_matchTargetSymbol (const wchar_t *targetSymbol, const wchar_t *symbol);
			virtual bool v_matchSourceSymbol (const wchar_t *sourceSymbol, const wchar_t *symbol);
			virtual bool v_matchTargetWithSourceSymbol (const wchar_t *targetSymbol, const wchar_t *sourceSymbol);
	#endif
oo_END_CLASS (EditCostsTable)
#undef ooSTRUCT

#define ooSTRUCT EditDistanceTable
	oo_DEFINE_CLASS (EditDistanceTable, TableOfReal)
	oo_OBJECT (WarpingPath, 0, d_warpingPath)
	#if oo_DECLARING
		oo_OBJECT (EditCostsTable, 0, d_editCostsTable)
		// overridden methods:
			virtual void v_info ();
	#endif
oo_END_CLASS (EditDistanceTable)
#undef ooSTRUCT

/* End of file EditDistanceTable_def.h */
