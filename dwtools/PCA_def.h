/* PCA_def.h
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

/*
 djmw 19981225, 20020813 GPL header, 20110329 latest modification
*/

#define ooSTRUCT PCA
oo_DEFINE_CLASS (PCA, Eigen)
	
	oo_INTEGER (numberOfObservations)
	oo_STRING_VECTOR (labels, dimension)
	oo_VEC (centroid, dimension)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (PCA)	
#undef ooSTRUCT

/* End of file PCA_def.h */	
