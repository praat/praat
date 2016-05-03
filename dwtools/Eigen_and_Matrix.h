#ifndef _Eigen_and_Matrix_h_
#define _Eigen_and_Matrix_h_

/* Eigen_and_Matrix.h
 *
 * Copyright (C) 1993-2011,2015-2016 David Weenink
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
 djmw 20020327
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "Eigen.h"
#include "Pattern.h"

autoMatrix Eigen_and_Matrix_to_Matrix_projectRows (Eigen me, Matrix thee, long numberOfDimensionsToKeep);
/*
	Purpose: project the rows of the matrix (thee) on the eigenspace (me). 
*/

autoMatrix Eigen_and_Matrix_to_Matrix_projectColumns (Eigen me, Matrix thee, long numberOfDimensionsToKeep);
/*
	Purpose: project the columnss of the matrix (thee) on the eigenspace (me). 
*/


#endif /* _Eigen_and_Matrix_h_ */
