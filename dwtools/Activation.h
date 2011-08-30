#ifndef _Activation_h_
#define _Activation_h_
/* Activation.h
 * 
 * Copyright (C) 1993-2011 David Weenink
 * 
 * This program is free oftware; you can redistribute it and/or modify
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
 djmw 20020315 GPL header.
 djmw 20110505 Latest modification.
 */

#include "Matrix.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_define (Activation, Matrix) {
};

/* Attributes:
   xmin				:1
   xmax				:#units
   nx				:#units
   dx				:1
   x1				:1
   ymin				:1
   ymax				:#patterns
   ny				:#patterns
   dy				:1
   y1				:1
   z[iy][ix]		:the activities
*/

void Activation_init (I, long ny, long nx);

Activation Activation_create (long ny, long nx);

Activation Matrix_to_Activation (I);

Matrix Activation_to_Matrix (I);


int _Activation_checkElements (Activation me);
/* Return 1 if all elements are in interval [0,1] else 0. */

#ifdef __cplusplus
	}
#endif

#endif /* _Activation_h_ */
