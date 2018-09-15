/* Configuration_def.h
 *
 * Copyright (C) 1993-2018 David Weenink
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


#define ooSTRUCT Configuration
oo_DEFINE_CLASS (Configuration, TableOfReal)

	oo_INTEGER (metric)
	oo_VEC (w, numberOfColumns)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Configuration)	
#undef ooSTRUCT


/* End of file Configuration_def.h */	
