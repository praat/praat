#ifndef _TableOfReal_and_Eigen_h_
#define _TableOfReal_and_Eigen_h_
/* TableOfReal_and_Eigen.h
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 20010307
 djmw 20020813 GPL header
*/

#ifndef _Eigen_h_
	#include "Eigen.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

int TablesOfReal_to_Eigen (TableOfReal me, TableOfReal thee);
/*
	Solve A'A x -lambda B'B x = 0
*/
#endif /* _TableOfReal_and_Matrix_h_ */
