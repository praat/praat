/* Spectrum_to_Excitation.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1995/05/20
 * pb 2002/07/16 GPL
 */

#include "Spectrum_to_Excitation.h"

Excitation Spectrum_to_Excitation (I, double dbark)
{
	iam (Spectrum);
	Excitation thee = NULL;
	double *inSig = NULL, *outSig = NULL, *rFreqs = NULL, *auditoryFilter = NULL, filterArea = 0;
	long *iFreqs = NULL;
	long i, j, nbark = (int) floor (25.6 / dbark + 0.5);
	double *re = my z [1], *im = my z [2]; 

	if (! (inSig = NUMdvector (1, nbark)) || ! (outSig = NUMdvector (1, 2 * nbark)) ||
		 ! (rFreqs = NUMdvector (1, nbark + 1)) || ! (iFreqs = NUMlvector (1, nbark + 1)) ||
		 ! (auditoryFilter = NUMdvector (1, nbark))) goto end;
	for (i = 1; i <= nbark; i ++)
	{
		double bark = dbark * (i - nbark/2) + 0.474;
		filterArea += auditoryFilter [i] = pow (10, (1.581 + 0.75 * bark - 1.75 * sqrt (1 + bark * bark)));
	}
	/*for (i = 1; i <= nbark; i ++)
		auditoryFilter [i] /= filterArea;*/
	for (i = 1; i <= nbark + 1; i ++)
	{
		rFreqs [i] = Excitation_barkToHertz (dbark * (i - 1));
		iFreqs [i] = Sampled_xToNearestIndex (me, rFreqs [i]);
	}
	for (i = 1; i <= nbark; i ++)
	{
		long low = iFreqs [i], high = iFreqs [i + 1] - 1;
		if (low < 1) low = 1;
		if (high > my nx) high = my nx;
		for (j = low; j <= high; j ++)
			inSig [i] += re [j] * re [j] + im [j] * im [j];   /* Pa2 s2 */

		/* An anti-undersampling correction. */
		if (high >= low)
			inSig [i] *= 2 * (rFreqs [i + 1] - rFreqs [i]) / (high - low + 1) * my dx;
					/* Pa2: power density in this band */
	}

	/* Convolution with auditory (masking) filter. */

	for (i = 1; i <= nbark; i ++) for (j = 1; j <= nbark; j ++)
		outSig [i + j] += inSig [i] * auditoryFilter [j];

	thee = Excitation_create (dbark, nbark); if (! thee) goto end;
	for (i = 1; i <= nbark; i ++)
		thy z [1] [i] = Excitation_soundPressureToPhon (sqrt (outSig [i + nbark/2]),
			Sampled_indexToX (thee, i));

end:
	NUMdvector_free (inSig, 1);
	NUMdvector_free (outSig, 1);
	NUMdvector_free (rFreqs, 1);
	NUMlvector_free (iFreqs, 1);
	NUMdvector_free (auditoryFilter, 1);
	return thee;
}

/* End of file Spectrum_to_Excitation.c */
