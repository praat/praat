/* NUMmachar.c
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
*/

#include "NUMmachar.h"
#include "melder.h"

#ifndef _NUMcblas_h_
#include "NUMcblas.h"
#endif

static struct structmachar_Table machar_table;
machar_Table NUMfpp = NULL;

/*
Floating point properties:

NR		LAPACK	dlamch
ibeta	base	'B'		base of the machine
it		t		'N'		number of (base) digits in the mantissa
machep
eps		prec	'P'		eps*base ('quantization step')
negep
epsneg	eps		'E'		relative machine precision ('quantization error')
iexp
minexp	emin	'M'		minimum exponent before (gradual) underflow
xmin	rmin	'U'		underflow threshold - base**(emin-1)
maxexp	emax	'U'		largest exponent before overflow
xmax	rmax	'O'		overflow threshold  - (base**emax)*(1-eps)
irnd	rnd		'R'		1 when rounding occurs in addition, 0 otherwise
ngrd
		sfmin	'S'		safe minimum, such that 1/sfmin does not overflow
*/

void NUMmachar () {
	if (NUMfpp) {
		return;
	}

	NUMfpp = & machar_table;

	NUMfpp -> base  = (int) NUMblas_dlamch ("Base");
	NUMfpp -> t     = (int) NUMblas_dlamch ("Number of digits in mantissa");
	NUMfpp -> emin  = (int) NUMblas_dlamch ("Minimum exponent");
	NUMfpp -> emax  = (int) NUMblas_dlamch ("Largest exponent");
	NUMfpp -> rnd   = (int) NUMblas_dlamch ("Rounding mode");
	NUMfpp -> prec  = NUMblas_dlamch ("Precision");
	NUMfpp -> eps   = NUMblas_dlamch ("Epsilon");
	NUMfpp -> rmin  = NUMblas_dlamch ("Underflow threshold");
	NUMfpp -> sfmin = NUMblas_dlamch ("Safe minimum");
	NUMfpp -> rmax  = NUMblas_dlamch ("Overflow threshold");
}

/* End of file NUMmachar.c */
