/* DTW_def.h
 *
 * Copyright (C) 1993-2008 David Weenink
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
 djmw 19981207
 djmw 20020813 GPL header
 djmw 20070216 Latest modification
*/

#define ooSTRUCT DTW_Path
oo_DEFINE_STRUCT (DTW_Path)
	oo_LONG (x)
	oo_LONG (y)
oo_END_STRUCT (DTW_Path) 
#undef ooSTRUCT

#define ooSTRUCT DTW_Path_Index
oo_DEFINE_STRUCT (DTW_Path_Index)
	oo_LONG (ibegin)
	oo_LONG (iend)
oo_END_STRUCT (DTW_Path_Index)
#undef ooSTRUCT

#define ooSTRUCT DTW_Path_xytime
oo_DEFINE_STRUCT (DTW_Path_xytime)
	oo_DOUBLE (x)
	oo_DOUBLE (y)
oo_END_STRUCT (DTW_Path_xytime) 
#undef ooSTRUCT

#define ooSTRUCT DTW_Path_Query
oo_DEFINE_STRUCT (DTW_Path_Query)
	oo_LONG (nx)
	oo_LONG (ny)
	oo_LONG (nxy)
	oo_STRUCT_VECTOR (DTW_Path_xytime, xytimes, nxy)
	oo_STRUCT_VECTOR (DTW_Path_Index, xindex, nx)
	oo_STRUCT_VECTOR (DTW_Path_Index, yindex, ny)
oo_END_STRUCT (DTW_Path_Query) 
#undef ooSTRUCT

#define ooSTRUCT DTW
oo_DEFINE_CLASS (DTW, Matrix)
	oo_DOUBLE (weightedDistance)
	oo_LONG (pathLength)
	oo_STRUCT_VECTOR (DTW_Path, path, pathLength)
	#if ! oo_READING && ! oo_WRITING
		oo_DOUBLE (wx)
		oo_DOUBLE (wy)
		oo_DOUBLE (wd)
		oo_STRUCT (DTW_Path_Query, pathQuery)
	#endif
	#if oo_READING
		DTW_Path_Query_init (& pathQuery, ny, nx);
		DTW_Path_recode (this);
	#endif
	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif
oo_END_CLASS (DTW)
#undef ooSTRUCT

/* End of file DTW_def.h */
