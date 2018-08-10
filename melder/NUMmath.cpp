/* NUMmath.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * pb 2002/03/07 GPL
 * pb 2003/07/09 gsl
 * pb 2005/07/08 NUMpow
 * pb 2008/01/19 double
 * pb 2008/09/21 NUMshift
 * pb 2008/09/22 NUMscale
 * pb 2011/03/29 C++
 */

#include "melder.h"

double NUMpow (double base, double exponent) { return base <= 0.0 ? 0.0 : pow (base, exponent); }

#include "gsl_errno.h"

void NUMshift (double *x, double xfrom, double xto) {
	if (*x == xfrom) *x = xto; else *x += xto - xfrom;
}

void NUMscale (double *x, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	if (*x == xminfrom) *x = xminto;
	else if (*x == xmaxfrom) *x = xmaxto;
	else *x = xminto + (xmaxto - xminto) * ((*x - xminfrom) / (xmaxfrom - xminfrom));
}

void NUMinit () {
	gsl_set_error_handler_off ();
	NUMrandom_init ();
}

/* End of file NUMmath.cpp */
