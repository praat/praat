/* SSCP_def.h
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


#define ooSTRUCT SSCP
oo_DEFINE_CLASS (SSCP, TableOfReal)

	oo_DOUBLE (numberOfObservations)
	oo_VEC (centroid, numberOfColumns)

	/*
		The following definitions are only needed when we want to use many big diagonal or
		almost diagonal matrices like for example in a GaussianMixture,
		or for efficiently calculating many times a distance like a'S^(-1)a
	*/
	#if oo_DECLARING || oo_DESTROYING
		oo_INTEGER (expansionNumberOfRows)
		oo_INT (dataChanged)
		oo_MAT (expansion, expansionNumberOfRows, numberOfColumns)
		oo_DOUBLE (lnd)
		oo_MAT (lowerCholeskyInverse, numberOfColumns, numberOfColumns)
		oo_OBJECT (PCA, 0, pca)
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (SSCP)
#undef ooSTRUCT


/* End of file SSCP_def.h */
