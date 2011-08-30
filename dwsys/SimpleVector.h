#ifndef _SimpleVector_h_
#define _SimpleVector_h_
/* SimpleVector.h
 *
 * Copyright (C) 1994-2011 David Weenink
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

#include "SimpleVector_def.h"
oo_CLASS_CREATE (DoubleVector, Data);
oo_CLASS_CREATE (ComplexVector, Data);

void DoubleVector_init (I, long min, long max);
DoubleVector DoubleVector_create (long min, long max);

void ComplexVector_init (I, long min, long max);
ComplexVector ComplexVector_create (long min, long max);

#endif /* _SimpleVector_h_ */
