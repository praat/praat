/* SPINET_to_Pitch.c
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 19970408
 djmw 20020813 GPL header
 djmw 20021106 Latest modification
*/

#include "SPINET_to_Pitch.h"
#include "Pitch_extensions.h"
#include "NUM2.h"

/*
	from Erb-scale to log2-scale
*/

Pitch SPINET_to_Pitch (SPINET me, double harmonicFallOffSlope, double ceiling, int maxnCandidates)
{
	Pitch thee = NULL;
	long i, j, k, m, nPointsPerOctave = 48;
	double fmin = NUMerbToHertz (Sampled2_rowToY (me, 1));
	double fmax = NUMerbToHertz (Sampled2_rowToY (me, my ny));
	double fminl2 = NUMlog2 (fmin), fmaxl2 = NUMlog2 (fmax);
	double points = (fmaxl2 - fminl2) * nPointsPerOctave;
	double dfl2 = (fmaxl2 - fminl2) / (points - 1);
	long nFrequencyPoints = points;
	long maxHarmonic = fmax / fmin;
	double maxStrength = 0, unvoicedCriterium = 0.45;
	double maxPower = 0, *sumspec = NULL, *power = NULL;
	double *y = NULL, *y2 = NULL, *pitch = NULL, *fl2 = NULL;
	
	if (nFrequencyPoints < 2) return Melder_errorp ("SPINET_to_Pitch: frequency range too small.");
	if (ceiling <= fmin) return Melder_errorp ("SPINET_to_Pitch: ceiling is smaller than centre "
		"frequency of lowest filter.");

	if (! (thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1,
				ceiling, maxnCandidates)) ||
		! (power = NUMdvector (1, my nx)) ||
		! (pitch = NUMdvector (1, nFrequencyPoints)) ||
		! (sumspec = NUMdvector (1, nFrequencyPoints)) ||
		! (y = NUMdvector (1, my ny)) ||
		! (y2 = NUMdvector (1, my ny)) ||
		! (fl2 = NUMdvector (1, my ny))) goto cleanup;
				
	/*
		From ERB's to log (f)
	*/
	
	for (i=1; i <= my ny; i++)
	{
		double f = NUMerbToHertz (my y1 + (i - 1) * my dy);
		fl2[i] = NUMlog2 (f);
	}
		
	/*
		Determine global maximum power in frame
	*/
	
	for (j=1; j <= my nx; j++)
	{
		double p = 0;
		for (i=1; i <= my ny; i++) p += my s[i][j];
		if (p > maxPower) maxPower = p;
		power[j] = p;
	}
	if (maxPower == 0) goto cleanup;
	
	for (j=1; j <= my nx; j++)
	{
		Pitch_Frame pitchFrame = &thy frame[j];
		
		pitchFrame->intensity = power[j] / maxPower;
		for (i=1; i <= my ny; i++) y[i] = my s[i][j];
		if (! NUMspline_d (fl2, y, my ny, 1e30, 1e30, y2)) goto cleanup; 
		for (k=1; k <= nFrequencyPoints; k++)
		{
			double f = fminl2 + (k-1) * dfl2;
			NUMsplint_d (fl2, y, y2, my ny, f, & pitch[k]);
			sumspec[k] = 0;
		}
		
		/*
			Formula (8): weighted harmonic summation.
		*/
		
		for (m=1; m <= maxHarmonic; m++)
		{
			double hm = 1 - harmonicFallOffSlope * NUMlog2 (m);
			long kb = 1 + floor (nPointsPerOctave * NUMlog2 (m));
			for (k=kb; k <= nFrequencyPoints; k++) 
			{
				if (pitch[k] > 0) sumspec[k-kb+1] += pitch[k] * hm;
			}
		}

		/*
			into Pitch object
		*/
		
		if (! Pitch_Frame_init (pitchFrame, maxnCandidates)) goto cleanup;
		pitchFrame->nCandidates = 0; /* !!!!! */
		Pitch_Frame_addPitch (pitchFrame, 0, 0, maxnCandidates); /* unvoiced */
			
		for (k=2; k <= nFrequencyPoints-1; k++)
		{
			double y1 = sumspec[k-1], y2 = sumspec[k], y3 = sumspec[k+1];
			if (y2 > y1 && y2 >= y3)
			{
				double denum = y1 - 2 * y2 + y3, tmp = y3 - 4 * y2;
				double x = dfl2 * (y1 - y3) / (2 * denum);
				double f = pow (2, fminl2 + (k - 1) * dfl2 + x);
				double strength = (2 * y1 * (4 * y2 + y3) - y1 * y1 - tmp * tmp) / (8 * denum);
				if (strength > maxStrength) maxStrength = strength;
				Pitch_Frame_addPitch (pitchFrame, f, strength, maxnCandidates);
			}	
		}
	}
	
	/*
		Scale the pitch strengths
	*/
	
	for (j=1; j <= my nx; j++)
	{
		double f0, localStrength;
		Pitch_Frame_getPitch (&thy frame[j], &f0, &localStrength);
		Pitch_Frame_resizeStrengths (&thy frame[j], localStrength / maxStrength, unvoicedCriterium);
	}
	
cleanup:
	NUMdvector_free (pitch, 1); NUMdvector_free (sumspec, 1);
	NUMdvector_free (y, 1); NUMdvector_free (y2, 1);
	NUMdvector_free (fl2, 1);NUMdvector_free (power, 1);
	if (! Melder_hasError()) return thee;
	forget (thee);
	return Melder_errorp ("SPINET_to_Pitch: not performed.");
}

/* End of file SPINET_to_Pitch.c */
