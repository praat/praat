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
*/

#define ooSTRUCT DTW_State
oo_DEFINE_STRUCT (DTW_State)
	oo_LONG (x)
	oo_LONG (y)
oo_END_STRUCT (DTW_State) 
#undef ooSTRUCT

#define ooSTRUCT DTW
oo_DEFINE_CLASS (DTW, Matrix)
	oo_DOUBLE (weightedDistance)
	oo_LONG (pathLength)
	#if oo_WRITING || oo_DESCRIBING || oo_EQUALLING || oo_COMPARING
		oo_STRUCT_VECTOR (DTW_State, path, my pathLength)
	#elif oo_READING_ASCII
		{
		long i;
		if (! (my path = NUMstructvector (DTW_State, 1, my nx + my ny - 1))) return 0;
		for (i = 1; i <= my pathLength; i ++)
			if (! DTW_State_readAscii (& my path [i], f)) return 0;
		}
	#elif oo_READING_BINARY	
		{
		long i;
		if (! (my path = NUMstructvector (DTW_State, 1, my nx + my ny - 1))) return 0;
		for (i = 1; i <= my pathLength; i ++)
			if (! DTW_State_readBinary (& my path [i], f)) return 0;
		}
	#else
		oo_STRUCT_VECTOR (DTW_State, path, my nx + my ny - 1)
	#endif
oo_END_CLASS (DTW)
#undef ooSTRUCT

/* End of file DTW_def.h */
