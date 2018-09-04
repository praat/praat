/* Artword_def.h
 *
 * Copyright (C) 1992-2005,2008,2009,2011,2015-2018 Paul Boersma
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


#define ooSTRUCT ArtwordData
oo_DEFINE_STRUCT (ArtwordData)

	oo_INT16 (numberOfTargets)
	oo_VEC (targets, numberOfTargets)
	oo_VEC (times, numberOfTargets)

	#if oo_DECLARING
		oo_INT16 (_iTarget)
	#endif

oo_END_STRUCT (ArtwordData)
#undef ooSTRUCT


#define ooSTRUCT Artword
oo_DEFINE_CLASS (Artword, Daata)

	oo_DOUBLE (totalTime)
	oo_STRUCT_SET (ArtwordData, data, kArt_muscle)

oo_END_CLASS (Artword)
#undef ooSTRUCT


/* End of file Artword_def.h */
