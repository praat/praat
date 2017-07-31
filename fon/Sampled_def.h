/* Sampled_def.h
 *
 * Copyright (C) 1992-2011,2014,2015 Paul Boersma
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


#define ooSTRUCT Sampled
oo_DEFINE_CLASS (Sampled, Function)

	oo_INT32 (nx)
	oo_DOUBLE (dx)
	oo_DOUBLE (x1)

	#if oo_READING
		if (xmin > xmax) {
			Melder_throw (U"xmax should be greater than xmin.");
		}
		if (nx < 1) {
			Melder_throw (U"nx should be at least 1.");
		}
		if (dx <= 0.0) {
			Melder_throw (U"dx should be positive.");
		}
	#endif

	#if oo_DECLARING
		bool v_hasGetNx ()
			override { return true; }
		double v_getNx ()
			override { return nx; }
		bool v_hasGetDx ()
			override { return true; }
		double v_getDx ()
			override { return dx; }
		bool v_hasGetX ()
			override { return true; }
		double v_getX (long ix)
			override { return x1 + (ix - 1) * dx; }
		void v_shiftX (double xfrom, double xto)
			override;
		void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
			override;

		virtual double v_getValueAtSample (long /* isamp */, long /* ilevel */, int /* unit */)
			{ return undefined; }
	#endif

oo_END_CLASS (Sampled)
#undef ooSTRUCT


/* End of file Sampled_def.h */
