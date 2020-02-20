/* Roots_def.h
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

#define ooSTRUCT Roots
oo_DEFINE_CLASS (Roots, Daata)
	oo_INTEGER (numberOfRoots)
	#if oo_READING
		oo_VERSION_UNTIL (1)
			// oo_INTEGER (min); already in numberOfRoots
			integer max;
			#if oo_READING_BINARY
				max = bingetinteger32BE (_filePointer_);
			#else
				max = texgetinteger (_textSource_);
			#endif
			numberOfRoots = max - numberOfRoots + 1;
			oo_COMPVEC (roots, numberOfRoots)
		oo_VERSION_ELSE
			oo_COMPVEC (roots, numberOfRoots)
		oo_VERSION_END
	#else
		oo_COMPVEC (roots, numberOfRoots)
	#endif
		
	#if oo_DECLARING
		void v_info ()
			override;
	#endif
	
oo_END_CLASS (Roots)	
#undef ooSTRUCT

/* End of file Roots_def.h */	
