/* Formant_extensions.c
 *
 * Copyright (C) 1993-2003 David Weenink
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
 djmw 20020813 GPL header
*/

#include "Formant_extensions.h"
#include "NUM2.h"

Formant Formant_filterQ (Formant me, double tmin, double tmax, double qmin)
{
	Formant thee = NULL;
	long i, j, *qok, itmin, itmax;

	if (tmax <= tmin)
	{
		tmin = my xmin; tmax = my xmax;
	}
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return NULL;

	qok = NUMlvector (1, my maxnFormants);
	if (qok == NULL) return NULL;
	thee = Data_copy (me);
	if (thee == NULL) goto end;
	
	for (i = itmin; i <= itmax; i++)
	{
		Formant_Frame ff = & my frame[i], ft = & thy frame[i];	
		long nf = ff -> nFormants, nft = 0;
		
		for (j = 1; j <= nf; j++)
		{
			double f = ff -> formant[j].frequency;
			double b = ff -> formant[j].bandwidth;
			if (b == 0 || f / b >= qmin)
			{
				nft++;
				qok[j] = 1;
				ft -> formant[nft].frequency = f;
				ft -> formant[nft].bandwidth = b; 
			}	
			else
			{
				qok[j] = 0;
			}
		}
		/* Copy the others to high position */
		ft -> nFormants = nft; 
		if (nf != nft)
		{
			for (j = 1; j <= nf; j++)
			{
				if (qok[j] == 0)
				{
					nft++;
					ft -> formant[nft].frequency = ff -> formant[j].frequency;
					ft -> formant[nft].bandwidth = ff -> formant[j].bandwidth; 			
				}
			}
		}
		Melder_assert (nf == nft);	
	}
	
end:
	
	NUMlvector_free (qok, 1);
	if (Melder_hasError ()) forget (thee);
	
	return thee;
}


/* End of file Formant_extensions.c */
