/* Procrustes_def.h
 *
 * Copyright (C) 1993-2019 David Weenink
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


#define ooSTRUCT Procrustes
oo_DEFINE_CLASS (Procrustes, AffineTransform)

	oo_DOUBLE (s)

	#if oo_DECLARING
		void v_transform (MATVU const& out, constMATVU const& in)
			override;
		autoAffineTransform v_invert ()
			override;
	#endif
			
oo_END_CLASS (Procrustes)	
#undef ooSTRUCT


/* End of file Procrustes_def.h */	
