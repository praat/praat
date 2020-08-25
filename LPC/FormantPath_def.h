/* FormantPath_def.h
 *
 * Copyright (C) 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#define ooSTRUCT FormantPath
oo_DEFINE_CLASS (FormantPath, Sampled)

	oo_COLLECTION_OF (OrderedOf, formants, Formant, 2)
	oo_VEC (ceilings, formants. size)
	oo_INTVEC (path, nx)
	
	#if oo_DECLARING
		void v_info ()
			override;
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		conststring32 v_getUnitText (integer level, int unit, uint32 flags)
			override;
		double v_getValueAtSample (integer sampleNumber, integer level, int unit)
			override;
		conststring32 v_getIndexText () const
			override { return U"frame number"; }
		conststring32 v_getNxText () const
			override { return U"the number of frames"; }
	#endif

oo_END_CLASS (FormantPath)
#undef ooSTRUCT

/* End of FormantPath_def.h */
