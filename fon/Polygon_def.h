/* Polygon_def.h
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


#define ooSTRUCT Polygon
oo_DEFINE_CLASS (Polygon, Data)

	oo_LONG (numberOfPoints)

	#if oo_READING
		if (localVersion >= 1) {
			oo_DOUBLE_VECTOR (x, numberOfPoints)
			oo_DOUBLE_VECTOR (y, numberOfPoints)
		} else {
			oo_FLOAT_VECTOR (x, numberOfPoints)
			oo_FLOAT_VECTOR (y, numberOfPoints)
		}
	#else
		oo_DOUBLE_VECTOR (x, numberOfPoints)
		oo_DOUBLE_VECTOR (y, numberOfPoints)
	#endif

	#if oo_DECLARING
		// overridden methods:
		virtual void v_info ();
	#endif

oo_END_CLASS (Polygon)
#undef ooSTRUCT


/* End of file Polygon_def.h */
