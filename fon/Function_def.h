/* Function_def.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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


#define ooSTRUCT Function
oo_DEFINE_CLASS (Function, Data)

	oo_DOUBLE (xmin)
	oo_DOUBLE (xmax)

	#if oo_READING
		if (xmin > xmax)
			Melder_throw (U"Wrong xmin ", xmin, U" and xmax ", xmax, U".");
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
		bool v_hasGetXmin ()
			override { return true; }
		double v_getXmin ()
			override { return xmin; }
		bool v_hasGetXmax ()
			override { return true; }
		double v_getXmax ()
			override { return xmax; }

		virtual int v_domainQuantity () { return 0; }
		virtual int v_getMinimumUnit (long ilevel) { (void) ilevel; return 0; }
		virtual int v_getMaximumUnit (long ilevel) { (void) ilevel; return 0; }
		virtual const char32 * v_getUnitText (long ilevel, int unit, unsigned long flags)
			{ (void) ilevel; (void) unit; (void) flags; return U""; }
		virtual bool v_isUnitLogarithmic (long ilevel, int unit)
			{ (void) ilevel; (void) unit; return false; }
		virtual double v_convertStandardToSpecialUnit (double value, long ilevel, int unit)
			{ (void) ilevel; (void) unit; return value; }
		virtual double v_convertSpecialToStandardUnit (double value, long ilevel, int unit)
			{ (void) ilevel; (void) unit; return value; }
		virtual void v_shiftX (double xfrom, double xto);
		virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
	#endif

oo_END_CLASS (Function)
#undef ooSTRUCT


/* End of file Function_def.h */
