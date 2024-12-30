/* FormantPath_def.h
 *
 * Copyright (C) 2020, 2022-2023 David Weenink
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

	oo_COLLECTION_OF (OrderedOf, formantCandidates, Formant, 2)
	oo_VEC (ceilings, formantCandidates.size)
	
	#if oo_DECLARING
		oo_OBJECT (TextGrid, 0, path)
		autoINTVEC dummyPath;
	#endif
	#if oo_READING
		oo_VERSION_UNTIL (1)
			oo_INTVEC (dummyPath, nx)
			path = FormantPath_to_TextGrid_version0 (this, dummyPath.get());
			dummyPath. reset();
		oo_VERSION_ELSE
			oo_OBJECT (TextGrid, 0, path)
		oo_VERSION_END
	#endif
	#if oo_WRITING || oo_COMPARING
		oo_OBJECT (TextGrid, 0, path)
	#endif

	#if oo_DECLARING
		void v1_info ()
			override;
		int v_domainQuantity () const
			override { return MelderQuantity_TIME_SECONDS; }
		conststring32 v_getUnitText (integer level, int unit, uint32 flags) const
			override;
		double v_getValueAtSample (integer sampleNumber, integer level, int unit) const
			override;
		conststring32 v_getIndexText () const
			override { return U"frame number"; }
		conststring32 v_getNxText () const
			override { return U"the number of frames"; }
	#endif

oo_END_CLASS (FormantPath)
#undef ooSTRUCT

/* End of FormantPath_def.h */
