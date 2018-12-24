/* Image_def.h
 *
 * Copyright (C) 1992-2011,2017,2018 Paul Boersma
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


#define ooSTRUCT Image
oo_DEFINE_CLASS (Image, Sampled)

	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_INTEGER (ny)
	oo_DOUBLE (dy)
	oo_DOUBLE (y1)
	oo_BYTEMAT (z, ny, nx)

oo_END_CLASS (Image)
#undef ooSTRUCT


/* End of file Image_def.h */
