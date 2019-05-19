#ifndef _FFNet_Matrix_h_
#define _FFNet_Matrix_h_
/* FFNet_Matrix.h
 *
 * Copyright (C) 1997-2017 David Weenink
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
 djmw 19950206
 djmw 20020712 GPL header
 djmw 20110307 Latest modification
*/

#include "Matrix.h"
#include "FFNet.h"

/* The Matrix organization is as follows:									*/
/*																			*/
/* nx == numberOfUnitsInLayer[layer]										*/
/* ny == numberOfUnitsInLayer[layer-1]+1									*/
/* xmin == 1, xmax == nx													*/
/* ymin == 1, ymax == ny													*/
/* dx == dy == 1															*/
/* x1 == y1 == 1															*/
/*																			*/

autoMatrix FFNet_weightsToMatrix (FFNet me, integer layer, bool deltaWeights);
/* (delta) weights connected to layer into Matrix */

autoFFNet FFNet_weightsFromMatrix (FFNet me, Matrix matrix, integer layer);
/* creates a new FFNet in which the weights that connect to layer are		*/
/* replaced by the weights in the matrix									*/

#endif /* _FFNet_Matrix_h_ */
