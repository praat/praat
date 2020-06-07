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
oo_DEFINE_CLASS (FormantPath, Function)

	oo_OBJECT (FunctionList, 0, formants)
	oo_INTEGER (defaultFormant)
	oo_OBJECT (TextGrid, 0, path)
	oo_INTEGER (pathTierNumber) // obsolete
	oo_OBJECT (Sound, 2, sound)
	oo_INTEGER (numberOfFrames)
	oo_INTVEC (formantIndices, numberOfFrames)
	
	#if oo_READING
		FormantPath_reconstructFormant (this);
	#endif
	#if oo_DECLARING
		oo_OBJECT (Formant, 2, formant)
		oo_OBJECT (IntervalTierNavigator, 0, intervalTierNavigator)
		oo_INTEGER (navigationTierNumber);
		void v_info ()
			override;
	#endif

oo_END_CLASS (FormantPath)
#undef ooSTRUCT

/* End of FormantPath_def.h */
