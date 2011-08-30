#ifndef _Pattern_h_
#define _Pattern_h_
/* Pattern.h
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

#include "Matrix.h"

Thing_define (Pattern, Matrix) {
};

/* Attributes:
   xmin				:index of first input node.
   xmax				:last node index.
   nx				:Number of inputs.
   dx				:1.
   x1				:1.
   ymin				:1.
   ymax				:#patterns.
   dy				:1.
   y1				:1.
   z[iy][ix]		:the inputs. All elements in interval [0,1].
*/

void Pattern_init (I, long ny, long nx);

Pattern Pattern_create (long ny, long nx);

void Pattern_normalize (I, int choice, double pmin, double pmax);
/* choice == 1: z[i][j] = (z[i][j]-pmin) / (pmax-pmin);
 * choice == 2: z[i][j] *= 1.0 / sum(j=1,j=nx, z[i][j]-pmin)
 */

void Pattern_draw (I, Graphics g, long pattern, double xmin, double xmax,
	double ymin, double ymax, int garnish);

Pattern Matrix_to_Pattern (I, int join);
Matrix Pattern_to_Matrix (Pattern me);

int _Pattern_checkElements (Pattern me);
/* Return 1 if all elements are in interval [0,1] else 0. */

#endif /* _Pattern_h_ */
