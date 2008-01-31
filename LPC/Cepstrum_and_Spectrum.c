/* Cepstrum_and_Spectrum.c
 *
 * Copyright (C) 1994-2008 David Weenink
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
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
*/

#include "Cepstrum_and_Spectrum.h"
#include "Spectrum_extensions.h"
#include "Sound_and_Spectrum.h"

Cepstrum Spectrum_to_Cepstrum (Spectrum me)
{
	Cepstrum thee = NULL;
	Sound x = NULL;
	Spectrum sx = NULL;
	Matrix unwrap = NULL;
	long i;
	
	if ((unwrap = Spectrum_unwrap (me)) == NULL) return NULL;

	if ((sx = Data_copy (me)) == NULL) goto end;
	
	/*
		Copy magnitude-squared and unwrapped phase.
	*/
	
	for (i = 1; i <= my nx; i ++)
	{
		double xa = unwrap -> z[1][i];
		sx -> z[1][i] = xa > 0 ? 0.5 * log (xa) : -300;
		sx -> z[2][i] = unwrap -> z[2][i];
	}
	
	/*
		Compute complex cepstrum x.
	*/
	
	if (((x = Spectrum_to_Sound (sx)) == NULL) ||
		((thee = Cepstrum_create (0, x -> xmax - x -> xmin, x -> nx)) == NULL))
			goto end;
	
	NUMdvector_copyElements (x -> z[1], thy z[1], 1, x -> nx);
	
end:

	forget (unwrap); forget (sx); forget (x);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Spectrum Cepstrum_to_Spectrum (Cepstrum me)
{
	Spectrum thee = NULL;
	Sound x = NULL;
	long i;

	if ((x = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1)) == NULL)
		return NULL;
	
	NUMdvector_copyElements	(my z[1], x -> z[1], 1, my nx);
	
	if ((thee = Sound_to_Spectrum (x, TRUE)) == NULL) goto end;
	
	for (i = 1; i <= thy nx; i++)
	{
		double ar = exp (thy z[1][i]);
		double ai = thy z[2][i];
		
		thy z[1][i] = ar * cos (ai);
		thy z[2][i] = ar * sin (ai);
	}
	
end:

	forget (x);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

/* End of file Cepstrum_and_Spectrum.c  */
