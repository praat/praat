#ifndef _ActivationList_h_
#define _ActivationList_h_
/* ActivationList.h
 * 
 * Copyright (C) 1993-2011,2015,2016 David Weenink, Paul Boersma 2017
 * 
 * This program is free oftware; you can redistribute it and/or modify
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

Thing_define (ActivationList, Matrix) {
};

/* Attributes:
   xmin         : 1
   xmax         : number of units
   nx           : number of units
   dx           : 1
   x1           : 1
   ymin         : 1
   ymax         : number of patterns
   ny           : number of patterns
   dy           : 1
   y1           : 1
   z [iy] [ix]  : the activities
*/

void ActivationList_init (ActivationList me, integer ny, integer nx);

autoActivationList ActivationList_create (integer ny, integer nx);

autoActivationList Matrix_to_ActivationList (Matrix me);

autoMatrix ActivationList_to_Matrix (ActivationList me);

int _ActivationList_checkElements (ActivationList me);
/* Return 1 if all elements are in interval [0,1] else 0. */

/* End of file ActivationList.h */
#endif
