#ifndef _AffineTransform_h_
#define _AffineTransform_h_
/* AffineTransform.h
 * 
 * Copyright (C) 2001-2005 David Weenink
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
 djmw 2001
 djmw 20020315 GPL header
 */

#ifndef _Data_h_
	#include "Data.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

#define AffineTransform_members Data_members \
	long n;	\
	double *t; \
	double **r;

#define AffineTransform_methods Data_methods \
	void (*transform) (I, double **in, long nrows, double **out); \
	Any (*invert) (I);
class_create (AffineTransform, Data)

int AffineTransform_init (I, long n);

AffineTransform AffineTransform_create (long n);

Any AffineTransform_invert (I);
/*
	Get inverse transform for y = A x + t:
		x = A^(-1)y - A^(-1) t
*/

TableOfReal AffineTransform_extractMatrix (I);

#endif /* _AffineTransform_h_ */
