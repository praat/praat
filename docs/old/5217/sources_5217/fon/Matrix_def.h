/* Matrix_def.h
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2008/01/19 version 2 (not 1, because Spectrum had that already)
 */


#define ooSTRUCT Matrix
oo_DEFINE_CLASS (Matrix, Sampled)

	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_LONG (ny)
	oo_DOUBLE (dy)
	oo_DOUBLE (y1)
	#if oo_READING
		if (localVersion >= 2) {
			oo_DOUBLE_MATRIX (z, my ny, my nx)
		} else {
			oo_FLOAT_MATRIX (z, my ny, my nx)
		}
	#else
		oo_DOUBLE_MATRIX (z, my ny, my nx)
	#endif

oo_END_CLASS (Matrix)
#undef ooSTRUCT


/* End of file Matrix_def.h */
