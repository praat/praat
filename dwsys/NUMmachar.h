#ifndef _NUMmachar_h_
#define _NUMmachar_h_
/* NUMmachar.h
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020812 GPL header
 djmw 20110308 Latest modification
*/

struct structmachar_Table
{
	int base;		/* Radix in which numbers are presented. */
	int t;			/* Number of base digits in mantissa*/
	int emin;		/* Minimum exponent before (gradual) underflow */
	int emax;		/* Largest exponent before overflow */
	int rnd;		/* 1 when rounding occurs in addition, 0 otherwise */
	double prec;	/* Quantization step (eps*base) */
	double eps;		/* Quantization error (relative machine precision) */
	double rmin;	/* Underflow threshold - base**(emin-1) */
	double sfmin;	/* Safe minimum, such that 1/sfmin does not overflow */
	double rmax;	/* Overflow threshold  - (base**emax)*(1-eps)*/
};

typedef struct structmachar_Table *machar_Table;

extern machar_Table NUMfpp;

void NUMmachar ();

#endif /* _NUMmachar_h_ */
