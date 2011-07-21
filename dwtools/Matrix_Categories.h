#ifndef _Matrix_Categories_h_
#define _Matrix_Categories_h_
/* Matrix_Categories.h
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

/*
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "Matrix.h"
#include "Categories.h"
#include "TableOfReal.h"

#ifdef __cplusplus
	extern "C" {
#endif

TableOfReal Matrix_and_Categories_to_TableOfReal (I, Categories thee);

#ifdef __cplusplus
	}
#endif

#endif /* _Matrix_Categories_h_ */
