/* DTW_def.h
 *
 * Copyright (C) 1993-2008 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 19981207
 djmw 20020813 GPL header
 djmw 20070216 Latest modification
*/

#define ooSTRUCT DTW_Path
oo_DEFINE_STRUCT (DTW_Path)

	oo_INTEGER (x)
	oo_INTEGER (y)

oo_END_STRUCT (DTW_Path)
#undef ooSTRUCT


#define ooSTRUCT DTW_Path_Query
oo_DEFINE_STRUCT (DTW_Path_Query)

	oo_INTEGER (nx)
	oo_INTEGER (ny)
	oo_INTEGER (nxy)
	oo_OBJECT (RealTier, 0, yfromx)
	oo_OBJECT (RealTier, 0, xfromy)

oo_END_STRUCT (DTW_Path_Query)
#undef ooSTRUCT


#define ooSTRUCT DTW
oo_DEFINE_CLASS (DTW, Matrix)

	oo_DOUBLE (weightedDistance)
	oo_INTEGER (pathLength)
	oo_STRUCTVEC (DTW_Path, path, pathLength)

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
		void v_info ()
			override;
	#endif

oo_END_CLASS (DTW)
#undef ooSTRUCT

/* End of file DTW_def.h */
