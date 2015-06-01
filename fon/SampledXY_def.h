/* SampledXY_def.h
 *
 * Copyright (C) 1992-2011,2013,2014,2015 Paul Boersma
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
 * pb 20131022 split off from Matrix
 */

#define ooSTRUCT SampledXY
oo_DEFINE_CLASS (SampledXY, Sampled)

	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_LONG (ny)
	oo_DOUBLE (dy)
	oo_DOUBLE (y1)

	#if oo_DECLARING
		bool v_hasGetYmin ()
			override { return true; }
		double v_getYmin ()
			override { return ymin; }
		bool v_hasGetYmax ()
			override { return true; }
		double v_getYmax ()
			override { return ymax; }
		bool v_hasGetNy ()
			override { return true; }
		double v_getNy ()
			override { return ny; }
		bool v_hasGetDy ()
			override { return true; }
		double v_getDy ()
			override { return dy; }
		bool v_hasGetY ()
			override { return true; }
		double v_getY (long iy)
			override { return y1 + (iy - 1) * dy; }
	#endif

oo_END_CLASS (SampledXY)
#undef ooSTRUCT

/* End of file SampledXY_def.h */
