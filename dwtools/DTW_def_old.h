/* DTW_def.h
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 20070211 Latest modification
*/
#define ooSTRUCT Path_query
oo_DEFINE_STRUCT (Path_query)
	oo_LONG (x1)
	oo_LONG (y1)
	oo_LONG (x2)
	oo_LONG (y2)
oo_END_STRUCT (Path_query) 
#undef ooSTRUCT
#define ooSTRUCT DTW_Path
oo_DEFINE_STRUCT (DTW_Path)
	oo_LONG (x)
	oo_LONG (y)
oo_END_STRUCT (DTW_Path) 
#undef ooSTRUCT

#define ooSTRUCT DTW
oo_DEFINE_CLASS (DTW, Matrix)
	oo_DOUBLE (weightedDistance)
	oo_LONG (pathLength)
	#if oo_WRITING || oo_DESCRIBING || oo_EQUALLING || oo_COMPARING
		oo_STRUCT_VECTOR (DTW_Path, path, my pathLength)
		#if ! oo_WRITING
			oo_STRUCT_VECTOR (Path_query, xquery, my nx)
			oo_STRUCT_VECTOR (Path_query, yquery, my ny)
		#endif
	#elif oo_READING_ASCII
		{
		long i;
		if (! (my path = NUMstructvector (DTW_Path, 1, my nx + my ny - 1))) return 0;
		if (! (my xquery = NUMstructvector (Path_query, 1, my nx))) return 0;
		if (! (my yquery = NUMstructvector (Path_query, 1, my ny))) return 0;
		for (i = 1; i <= my pathLength; i++)
			if (! DTW_Path_readAscii (& my path [i], f)) return 0;
		DTW_pathRemoveRedundantNodes (me);
		DTW_pathQueryRecode (me);
		}
	#elif oo_READING_BINARY	
		{
		long i;
		if (! (my path = NUMstructvector (DTW_Path, 1, my nx + my ny - 1))) return 0;
		if (! (my xquery = NUMstructvector (Path_query, 1, my nx))) return 0;
		if (! (my yquery = NUMstructvector (Path_query, 1, my ny))) return 0;
		for (i = 1; i <= my pathLength; i++)
			if (! DTW_Path_readBinary (& my path [i], f)) return 0;
		DTW_pathRemoveRedundantNodes (me);
		DTW_pathQueryRecode (me);
		}
	#else
		oo_STRUCT_VECTOR (DTW_Path, path, my nx + my ny - 1)
		oo_STRUCT_VECTOR (Path_query, xquery, my nx)
		oo_STRUCT_VECTOR (Path_query, yquery, my ny)
	#endif
oo_END_CLASS (DTW)
#undef ooSTRUCT

/* End of file DTW_def.h */
