#ifndef _Eigen_and_SSCP_h_
#define _Eigen_and_SSCP_h_
/* Eigen_and_SSCP.h
 *
 * Copyright (C) 1993-2004, 2015 David Weenink
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
 djmw 20020327 Initial version
 djmw 20040219 GPL header
*/

#include "Eigen.h"
#include "Covariance.h"

autoSSCP Eigen_SSCP_project (Eigen me, SSCP thee);

autoCovariance Eigen_Covariance_project (Eigen me, Covariance thee);
/*
	Purpose: project the SSCP (thee) on the eigenspace (me): S' = E' S E
	Returns SSCP-object with square matrix dimension 'my numberOfEigenvalues'
*/

#endif /* _Eigen_and_SSCP_h_ */
