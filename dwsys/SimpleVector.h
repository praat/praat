#ifndef _SimpleVector_h_
#define _SimpleVector_h_
/* SimpleVector.h
 *
 * Copyright (C) 1994-2017 David Weenink
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

#include "SimpleVector_def.h"

void DoubleVector_init (DoubleVector me, integer min, integer max);

autoDoubleVector DoubleVector_create (integer min, integer max);

void ComplexVector_init (ComplexVector me, integer min, integer max);

autoComplexVector ComplexVector_create (integer min, integer max);

#endif /* _SimpleVector_h_ */
