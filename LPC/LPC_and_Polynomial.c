/* LPC_and_Polynomial.c
 *
 * Copyright (C) 1994-2002 David Weenink
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
 djmw 20020812 GPL header
*/

#include "LPC_and_Polynomial.h"

Polynomial LPC_Frame_to_Polynomial (LPC_Frame me)
{
	Polynomial thee; long i, degree = (long) my nCoefficients;

	if ((thee = Polynomial_create (-1, 1, degree)) == NULL) return NULL;

	for (i=1; i <= degree; i++)
	{
		thy coefficients[i] = my a[degree - i + 1];
	}
	thy coefficients[degree + 1] = 1;
	return thee;
}

Polynomial LPC_to_Polynomial (LPC me, double time)
{
	long iFrame = Sampled_xToIndex (me, time);
	
	if (iFrame < 1 || iFrame > my nx) return NULL;
	return LPC_Frame_to_Polynomial (&my frame[iFrame]);
}

/* End of file LPC_and_Polynomial.c */
