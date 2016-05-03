/* Vowel_def.h
 * 
 * Copyright (C) 2011,2015 David Weenink & Paul Boersma
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


#define ooSTRUCT Vowel
oo_DEFINE_CLASS (Vowel, Function)
	oo_AUTO_OBJECT (PitchTier, 0, pt)
	oo_AUTO_OBJECT (FormantTier, 0, ft)
oo_END_CLASS (Vowel)
#undef ooSTRUCT


/* End of file Vowel_def.h */
