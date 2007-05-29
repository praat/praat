/* Formula.h
 *
 * Copyright (C) 1990-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2007/05/26
 */

#ifndef _Thing_h_
	#include "Thing.h"
#endif

int Formula_compile (Any interpreter, Any data, const wchar_t *expression, int isStringExpression, int optimize);

int Formula_run (long row, long col, double *numericResult, wchar_t **stringResult);

/* End of file Formula.h */
