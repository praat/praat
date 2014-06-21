/* Sampled_def.h
 *
 * Copyright (C) 1992-2011,2014 Paul Boersma
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


#define ooSTRUCT Sampled
oo_DEFINE_CLASS (Sampled, Function)

	oo_LONG (nx)
	oo_DOUBLE (dx)
	oo_DOUBLE (x1)

	#if oo_DECLARING
		// overridden methods:
			virtual bool v_hasGetNx () { return true; }   virtual double v_getNx ()        { return nx; }
			virtual bool v_hasGetDx () { return true; }   virtual double v_getDx ()        { return dx; }
			virtual bool v_hasGetX  () { return true; }   virtual double v_getX  (long ix) { return x1 + (ix - 1) * dx; }
			virtual void v_shiftX (double xfrom, double xto);
			virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
		// new methods:
			virtual double v_getValueAtSample (long isamp, long ilevel, int unit)
				{ (void) isamp; (void) ilevel; (void) unit; return NUMundefined; }
		// functions:
			inline double f_indexToX (long   index) { return x1 + (index - 1  ) * dx; }
			inline double f_indexToX (double index) { return x1 + (index - 1.0) * dx; }
			double f_xToIndex (double x) { return (x - x1) / dx + 1.0; }
			long f_xToLowIndex     (double x) { return (long) floor (f_xToIndex (x)); }
			long f_xToHighIndex    (double x) { return (long) ceil  (f_xToIndex (x)); }
			long f_xToNearestIndex (double x) { return (long) round (f_xToIndex (x)); }
			long f_getWindowSamplesX (double xmin, double xmax, long *ixmin, long *ixmax);
	#endif

oo_END_CLASS (Sampled)
#undef ooSTRUCT


/* End of file Sampled_def.h */
