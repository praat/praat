#ifndef _FFNet_Matrix_h_
#define _FFNet_Matrix_h_
/* FFNet_Matrix.h
 *
 * Copyright (C) 1997-2011 David Weenink
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
 djmw 19950206
 djmw 20020712 GPL header
 djmw 20110307 Latest modification
*/

#include "Matrix.h"
#include "FFNet.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* The Matrix organization is as follows:									*/
/*																			*/
/* nx = nUnitsInLayer[layer]												*/
/* ny = nUnitsInLayer[layer-1]+1											*/
/* xmin = 1 xmax = nx														*/
/* ymin = 1 ymax = ny														*/
/* dx = dy = 1																*/
/* x1 = y1 = 1																*/
/*																			*/

Matrix FFNet_weightsToMatrix (FFNet me, long layer, int deltaWeights);
/* (delta) weights connected to layer into Matrix */

FFNet FFNet_weightsFromMatrix (FFNet me, Matrix matrix, long layer);
/* creates a new FFNet in which the weights that connect to layer are		*/
/* replaced by the weights in the matrix									*/

#ifdef __cplusplus
	}
#endif

#endif /* _FFNet_Matrix_h_ */
