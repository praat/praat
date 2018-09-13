#ifndef _AffineTransform_h_
#define _AffineTransform_h_
/* AffineTransform.h
 *
 * Copyright (C) 2001-2018 David Weenink
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

#include "Data.h"
#include "TableOfReal.h"

#include "AffineTransform_def.h"

void AffineTransform_init (AffineTransform me, integer dimension);

autoAffineTransform AffineTransform_create (integer dimension);

autoAffineTransform AffineTransform_invert (AffineTransform me);
/*
	Get inverse transform for y = A x + t:
		x = A^(-1)y - A^(-1) t
*/

autoTableOfReal AffineTransform_extractMatrix (AffineTransform me);

autoTableOfReal AffineTransform_extractTranslationVector (AffineTransform me);

#endif /* _AffineTransform_h_ */
