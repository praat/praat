#ifndef _AffineTransform_h_
#define _AffineTransform_h_
/* AffineTransform.h
 *
 * Copyright (C) 2001-2011 David Weenink
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

#include "Data.h"
#include "TableOfReal.h"

#include "AffineTransform_def.h"
oo_CLASS_CREATE (AffineTransform, Data);

void AffineTransform_init (I, long n);

AffineTransform AffineTransform_create (long n);

Any AffineTransform_invert (I);
/*
	Get inverse transform for y = A x + t:
		x = A^(-1)y - A^(-1) t
*/

TableOfReal AffineTransform_extractMatrix (I);

TableOfReal AffineTransform_extractTranslationVector (I);

#endif /* _AffineTransform_h_ */
