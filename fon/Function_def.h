/* Function_def.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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


#define ooSTRUCT Function
oo_DEFINE_CLASS (Function, Daata)

	oo_DOUBLE (xmin)
	oo_DOUBLE (xmax)

	#if oo_READING
		if (xmin > xmax)
			Melder_throw (U"Wrong xmin ", xmin, U" and xmax ", xmax, U".");
	#endif

	#if oo_DECLARING
		void v1_info ()
			override;
		bool v_hasGetXmin () const
			override { return true; }
		double v_getXmin () const
			override { return xmin; }
		bool v_hasGetXmax () const
			override { return true; }
		double v_getXmax () const
			override { return xmax; }

		virtual int v_domainQuantity () const { return 0; }
		virtual int v_getMinimumUnit (integer /* level */) const { return 0; }
		virtual int v_getMaximumUnit (integer /* level */) const { return 0; }
		virtual conststring32 v_getUnitText (integer /* level */, int /* unit */, uint32 /* flags */) const
			{ return U""; }
		virtual bool v_isUnitLogarithmic (integer /* level */, int /* unit */) const
			{ return false; }
		virtual double v_convertStandardToSpecialUnit (double value, integer /* level */, int /* unit */) const
			{ return value; }
		virtual double v_convertSpecialToStandardUnit (double value, integer /* level */, int /* unit */) const
			{ return value; }
		virtual void v_shiftX (double xfrom, double xto);
		virtual void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto);
	#endif

oo_END_CLASS (Function)
#undef ooSTRUCT


/* End of file Function_def.h */
