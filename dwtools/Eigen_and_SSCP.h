#ifndef _Eigen_and_SSCP_h_
#define _Eigen_and_SSCP_h_
/* Eigen_and_SSCP.h
 *
 * Copyright (C) 1993-2004 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020327 Initial version
 djmw 20040219 GPL header
*/

#ifndef _Eigen_h_
	#include "Eigen.h"
#endif

#ifndef _SSCP_h_
	#include "SSCP.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

SSCP Eigen_and_SSCP_project (I, SSCP thee);
Covariance Eigen_and_Covariance_project (I, Covariance thee);
/*
	Purpose: project the SSCP (thee) on the eigenspace (me): S' = E' S E
	Returns SSCP-object with square matrix dimension 'my numberOfEigenvalues'
*/

#ifdef __cplusplus
	}
#endif

#endif /* _Eigen_and_SSCP_h_ */
