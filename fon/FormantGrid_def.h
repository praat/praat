/* FormantGrid_def.h
 *
 * Copyright (C) 2008-2011,2015,2017 Paul Boersma & David Weenink
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

#define ooSTRUCT FormantGrid
oo_DEFINE_CLASS (FormantGrid, Function)

	oo_COLLECTION_OF (OrderedOf, formants, RealTier, 0)
	oo_COLLECTION_OF (OrderedOf, bandwidths, RealTier, 0)

	#if oo_DECLARING
		bool v_hasGetVector ()
			override { return true; }
		double v_getVector (integer irow, integer icol)
			override;
		bool v_hasGetFunction1 ()
			override { return true; }
		double v_getFunction1 (integer irow, double x)
			override;
		conststring32 v_getUnitText (integer ilevel, int /* unit */, uint32 /* flags */)
			override { return ilevel & 1 ? U"Formant (Hz)" : U"Bandwidth (Hz)"; }
		void v_shiftX (double xfrom, double xto)
			override;
		void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
			override;
	#endif

oo_END_CLASS (FormantGrid)
#undef ooSTRUCT

/* End of file FormantGrid_def.h */
