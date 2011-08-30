#ifndef _Procrustes_h_
#define _Procrustes_h_
/* Procrustes.h
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
 djmw 20010926
 djmw 20020813 GPL header
 djmw 20110306 Latest modification.
*/

#include "AffineTransform.h"

#include "Procrustes_def.h"
oo_CLASS_CREATE (Procrustes, AffineTransform);

Procrustes Procrustes_create (long n);

#endif /* _Procrustes_h_ */
