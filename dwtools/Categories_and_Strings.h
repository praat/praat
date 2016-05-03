#ifndef _Categories_and_Strings_h_
#define _Categories_and_Strings_h_
/* Categories_and_Strings.h
 *
 * Copyright (C) 2001-2012, 2015 David Weenink
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
 djmw 20020315 GPL header
 djmw 20121018 Latest modification
 */

#include "Categories.h"
#include "Strings_.h"

autoStrings Categories_to_Strings (Categories me);

autoCategories Strings_to_Categories (Strings me);

#endif /* _Categories_and_Strings_h_ */
