#ifndef _NUMf2c_h_
#define _NUMf2c_h_
 /* NUMf2c.h  --  Standard Fortran to C header file
 *                Auxiliary f2c routines.
 *
 * Copyright (C) 1994-2011 David Weenink, 2017 Paul Boersma
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

#include "melder.h"   /* for integer */

double d_sign(double *a, double *b);
/*
	returns sign(b)*fabs(a)
*/

integer lsame_(const char *ca, const char *cb);
/*
    Returns true if ca[0] is the same letter as cb[0] regardless of case.
*/

double pow_di(double *ap, integer *bp);

void s_cat(char *lp, const char *rpp[], integer rnp[], integer *np, integer ll);
/* Unless compiled with -DNO_OVERWRITE, this variant of s_cat allows the
 * target of a concatenation to appear on its right-hand side (contrary
 * to the Fortran 77 Standard, but in accordance with Fortran 90).
 */

integer s_cmp(const char *a0, const char *b0, integer la, integer lb);

void s_copy(register char *a, register char *b, integer la, integer lb);
/* assign strings:  a = b (when no null byte at end of string)*/

#endif /* _NUMf2c_h_ */
