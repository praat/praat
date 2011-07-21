#ifndef _NUMf2c_h_
#define _NUMf2c_h_
 /* NUMf2c.h  --  Standard Fortran to C header file
 *                Auxiliary f2c routines.
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

/*
 djmw 20020923 GPL header
 djmw 20110308 Latest modification
*/

#ifdef __cplusplus
	extern "C" {
#endif

double d_sign(double *a, double *b);
/*
	returns sign(b)*fabs(a)
*/

long int lsame_(const char *ca, const char *cb);
/*
    Returns true if ca[0] is the same letter as cb[0] regardless of case.
*/

double pow_di(double *ap, long *bp);

void s_cat(char *lp, const char *rpp[], long rnp[], long *np, long ll);
/* Unless compiled with -DNO_OVERWRITE, this variant of s_cat allows the
 * target of a concatenation to appear on its right-hand side (contrary
 * to the Fortran 77 Standard, but in accordance with Fortran 90).
 */

long s_cmp(const char *a0, const char *b0, long la, long lb);

void s_copy(register char *a, register char *b, long la, long lb);
/* assign strings:  a = b (when no null byte at end of string)*/

#ifdef __cplusplus
	}
#endif

#endif /* _NUMf2c_h_ */
