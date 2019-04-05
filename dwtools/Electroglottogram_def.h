/* Electroglottogram_def.h
 *
 * Copyright (C) 2019 David Weenink
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


#define ooSTRUCT Electroglottogram
oo_DEFINE_CLASS (Electroglottogram, Sound)

	oo_OBJECT (Sound, 0, sound)
	
	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Electroglottogram)
#undef ooSTRUCT

/* End of file Electroglottogram_def.h */
