#ifndef _Eigen_and_Matrix_h_
#define _Eigen_and_Matrix_h_

/* Eigen_and_Matrix.h
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20020327
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#ifndef _Eigen_h_
	#include "Eigen.h"
#endif

#ifndef _Matrix_h_
	#include "Matrix.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

Matrix Eigen_and_Matrix_project (I, thou, long numberOfComponents);
/*
	Purpose: project the columns of the matrix (thou) on the 
	eigenspace (me). 
*/

void Eigen_and_Matrix_project_into (I, thou, Any void_pointer_to_him);
/*
	Purpose: project the columns of the Matrix (thou) on the 
	eigenspace (me). Result in existing Matrix (him). 

*/

#ifdef __cplusplus
	}
#endif

#endif /* _Eigen_and_Matrix_h_ */
