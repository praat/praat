#ifndef _PatternList_h_
#define _PatternList_h_
/* Pattern.h
 *
 * Copyright (C) 1993-2019 David Weenink
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

#include "Matrix.h"
#include "ActivationList.h"

Thing_define (PatternList, Matrix) {
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

autoPatternList PatternList_create (integer ny, integer nx);

void PatternList_normalize (PatternList me, int choice, double pmin, double pmax);
/* choice == 1: z[i][j] = (z[i][j]-pmin) / (pmax-pmin);
 * choice == 2: z[i][j] *= 1.0 / sum(j=1,j=nx, z[i][j]-pmin)
 */

void PatternList_draw (PatternList me, Graphics g, integer pattern, double xmin, double xmax,
	double ymin, double ymax, bool garnish);

autoPatternList Matrix_to_PatternList (Matrix me, integer join);

autoMatrix PatternList_to_Matrix (PatternList me);

autoPatternList ActivationList_to_PatternList (ActivationList me);

bool _PatternList_checkElements (PatternList me);
/* Return true if all elements are in interval [0,1] else 0. */

#endif /* _PatternList_h_ */
