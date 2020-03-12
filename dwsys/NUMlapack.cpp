/* NUMlapack.cpp
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

#include "clapack.h"

#include "NUMlapack.h"

integer getLeadingDimension (constMATVU const& m) {
	return m.rowStride > 1 ? m.rowStride : m.columnStride; 
}

integer NUMlapack_dgesvd_query (char jobu, char jobvt, MATVU const& a, VEC const& inout_singularValues, MATVU const& inout_u, MATVU const& inout_vt) {
	
	integer lwork = -1, lda = getLeadingDimension (a);
	double wt;
	dgesvd_ (& jobu, & jobvt, & a.nrow, & a.ncol, & a [1] [1], & lda, & , & my v [1] [1], & ldu, nullptr, & ldvt, & wt, & lwork, & info);
	
	integer result = (integer) (wt+0.1)
}


/* Subroutine */ int dgesvd_(char *jobu, char *jobvt, integer *m, integer *n, 
	doublereal *a, integer *lda, doublereal *s, doublereal *u, integer *
	ldu, doublereal *vt, integer *ldvt, doublereal *work, integer *lwork, 
	integer *info);

/*End of file NUMlapack.cpp */
