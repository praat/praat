#ifndef _Eigen_and_TableOfReal_h_
#define _Eigen_and_TableOfReal_h_

/* Eigen_and_TableOfReal.h
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
 djmw 20020327 Initial version
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#ifndef _Eigen_h_
	#include "Eigen.h"
#endif

#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

TableOfReal Eigen_and_TableOfReal_project (I, thou, long from,
	long numberOfComponents);
/*
	Purpose: project the rows of the TableOfReal (thee) starting at index 'from'
	on the eigenspace (me).
*/

void Eigen_and_TableOfReal_project_into (I, thou, long thee_from, long thee_to,
	Any void_pointer_to_him, long his_from, long his_to);
/*
	Purpose: project the rows of the TableOfReal (thee) on the
	eigenspace (me). Result in existing TableOfReal (him).
*/

Eigen TablesOfReal_to_Eigen_gsvd (TableOfReal me, TableOfReal thee);
/*
	Solve A'A x -lambda B'B x = 0
*/

#ifdef __cplusplus
	}
#endif

#endif /* _Eigen_and_TableOfReal_h_ */
