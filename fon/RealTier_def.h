/* RealTier_def.h
 *
 * Copyright (C) 1992-2012 Paul Boersma
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


#define ooSTRUCT RealPoint
oo_DEFINE_CLASS (RealPoint, AnyPoint)

	oo_DOUBLE (value)

oo_END_CLASS (RealPoint)
#undef ooSTRUCT


#define ooSTRUCT RealTier
oo_DEFINE_CLASS (RealTier, Function)

	oo_COLLECTION (SortedSetOfDouble, points, RealPoint, 0)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
			virtual bool v_hasGetNx        () { return true; }   virtual double v_getNx   ()        { return points -> size; }
			virtual bool v_hasGetX         () { return true; }   virtual double v_getX    (long ix) { return ((RealPoint) points -> item [ix]) -> number; }
			virtual bool v_hasGetNcol      () { return true; }   virtual double v_getNcol ()        { return points -> size; }
			virtual bool v_hasGetVector    () { return true; }   virtual double v_getVector    (long irow, long icol);
			virtual bool v_hasGetFunction1 () { return true; }   virtual double v_getFunction1 (long irow, double x);
			virtual const wchar_t * v_getUnitText (long ilevel, int unit, unsigned long flags)
				{ (void) ilevel; (void) unit; (void) flags; return L"Time (s)"; }
			virtual void v_shiftX (double xfrom, double xto);
			virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
		// new functions:
			long f_getNumberOfPoints () { return points -> size; }
			RealPoint f_peekPoint (long ipoint) { return (RealPoint) points -> item [ipoint]; }
	#endif

oo_END_CLASS (RealTier)
#undef ooSTRUCT


/* End of file RealTier_def.h */
