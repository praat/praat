#ifndef _WorkvectorPool_h_
#define _WorkvectorPool_h_
/* WorkvectorPool.h
 *
 * Copyright (C) 2024 David Weenink
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
#include "melder.h"

#include "WorkvectorPool_def.h"

autoWorkvectorPool WorkvectorPool_create (constINTVEC const& sizes, bool reusable);

#endif /* _WorkvectorPool_h_ */
