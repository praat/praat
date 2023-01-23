/* Photo_def.h
 *
 * Copyright (C) 2013-2018,2022,2023 Paul Boersma
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


#define ooSTRUCT Photo
oo_DEFINE_CLASS (Photo, SampledXY)

	oo_OBJECT (Matrix, 2, d_red)
	oo_OBJECT (Matrix, 2, d_green)
	oo_OBJECT (Matrix, 2, d_blue)
	oo_OBJECT (Matrix, 2, d_transparency)

	#if oo_DECLARING
		void v1_info ()
			override;
		bool v_hasGetNrow () const
			override { return true; }
		double v_getNrow () const
			override { return ny; }
		bool v_hasGetNcol () const
			override { return true; }
		double v_getNcol () const
			override { return nx; }
		bool v_hasGetYmin () const
			override { return true; }
		double v_getYmin () const
			override { return ymin; }
		bool v_hasGetYmax () const
			override { return true; }
		double v_getYmax () const
			override { return ymax; }
		bool v_hasGetNy () const
			override { return true; }
		double v_getNy () const
			override { return ny; }
		bool v_hasGetDy () const
			override { return true; }
		double v_getDy () const
			override { return dy; }
		bool v_hasGetY () const
			override { return true; }
		double v_getY (const integer iy) const
			override { return y1 + (iy - 1) * dy; }
	#endif

oo_END_CLASS (Photo)
#undef ooSTRUCT


/* End of file Matrix_def.h */
