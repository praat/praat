/* Eigen_def.h
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

#define ooSTRUCT Eigen
oo_DEFINE_CLASS (Eigen, Daata)

	oo_INTEGER (numberOfEigenvalues)
	oo_INTEGER (dimension)
	oo_VEC (eigenvalues, numberOfEigenvalues)
	oo_MAT (eigenvectors, numberOfEigenvalues, dimension) // eigenvectors stored in the rows

oo_END_CLASS (Eigen)
#undef ooSTRUCT

/* End of file Eigen_def.h */
