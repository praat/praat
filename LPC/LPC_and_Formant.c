/* LPC_and_Formant.c
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
 djmw 20030616 Formant_Frame_into_LPC_Frame: remove formant with f >= Nyquist +
 		change lpc indexing from -1..m
 djmw 20080122 float -> double
*/

#include "LPC_and_Formant.h"
#include "LPC_and_Polynomial.h"
#include "NUM2.h"

int Formant_Frame_init (Formant_Frame me, long nFormants)
{
	my nFormants = nFormants;
    return nFormants > 0 && ((my formant = 
			NUMstructvector (Formant_Formant, 1, my nFormants)) != NULL);
}

void Formant_Frame_scale (Formant_Frame me, double scale)
{
	long i;
	
	for (i = 1; i <= my nFormants; i++)
	{
		my formant[i].frequency *= scale;
		my formant[i].bandwidth *= scale;	
	}
}

int Roots_into_Formant_Frame (Roots me, Formant_Frame thee, double samplingFrequency, double margin)
{
	double f, b, fLow = margin, fHigh = samplingFrequency / 2 - margin;
	double *fc = NULL, *bc = NULL;
	long i, n = my max - my min + 1;
	
	fc = NUMdvector (1, n);
	if (fc == NULL) return 0;
	bc = NUMdvector (1, n);
	if (bc == NULL) goto end;
	
	/*
		Determine the formants and bandwidths
	*/
	 
    thy nFormants = 0;
    for (i = my min; i <= my max; i++)
    {
		if (my v[i].im < 0) continue;
		f = fabs (atan2 (my v[i].im, my v[i].re)) * samplingFrequency / 2 / NUMpi;
        if (f >= fLow && f <= fHigh)
        {
			/*b = - log (my v[i].re * my v[i].re + my v[i].im * my v[i].im) * samplingFrequency / 2 / NUMpi;*/
			b = - log (dcomplex_abs (my v[i])) * samplingFrequency / NUMpi;
			thy nFormants++;
			fc[thy nFormants] = f;
			bc[thy nFormants] = b;
		}
	}
	
	if (! Formant_Frame_init (thee, thy nFormants)) goto end;
	
	for (i = 1; i <= thy nFormants; i++)
	{
		thy formant[i].frequency = fc[i];
		thy formant[i].bandwidth = bc[i];	
	}
	
end:

	NUMdvector_free (fc, 1); 
	NUMdvector_free (bc, 1);
	return ! Melder_hasError ();   
}

int LPC_Frame_into_Formant_Frame (LPC_Frame me, Formant_Frame thee, 
	double samplingPeriod, double margin)
{
	Polynomial p;
	Roots r = NULL;
	long status = 0;
	
	thy intensity = my gain;

	if (my nCoefficients == 0) return 1;
	
    p = LPC_Frame_to_Polynomial (me);
	if (p == NULL) return 0;
	
    r = Polynomial_to_Roots (p);
    if (r == NULL) goto end;
    
    Roots_fixIntoUnitCircle (r);
    
    status = Roots_into_Formant_Frame (r, thee, 1 / samplingPeriod, margin);

end:

	forget (r); forget (p);

	return status;
}

Formant LPC_to_Formant (LPC me, double margin)
{
	Formant thee = NULL;
	double samplingFrequency = 1.0 / my samplingPeriod;
	int nmax = my maxnCoefficients;
	long err = 0, i, interval = nmax > 20 ? 1 : 10;

	if (nmax > 99) return Melder_errorp1 (L"We cannot find the roots of a polynomial "
		"of order > 99.");
	if (margin >= samplingFrequency/4) return Melder_errorp3 (L"Margin must be smaller than ", Melder_double (samplingFrequency/4), L".");
	 
	thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, (nmax+1)/2);	
	if (thee == NULL) return NULL;

	(void) Melder_progress1 (0.0, L"LPC to Formant");
	
	for (i = 1; i <= my nx; i++)
	{
		Formant_Frame formant = & thy frame[i];
		LPC_Frame lpc = & my frame[i];
		
		/*
			Initialisation of Formant_Frame is taken care of in 
			Roots_into_Formant_Frame!
		*/
		
		if (! LPC_Frame_into_Formant_Frame (lpc, formant, my samplingPeriod, margin))
		{
			err++;
		}
		
		if ((interval == 1 || (i % interval) == 1) && ! Melder_progress5 ((double)i / my nx,
			L"LPC to Formant: frame ", Melder_integer (i), L" out of ", Melder_integer (my nx), L".")) goto end;
	}
	
	Formant_sort (thee);

end:

	(void) Melder_progress1 (1.0, NULL);
	
	if (err > 0) Melder_warning4 (Melder_integer (err), L" formant frames out of ", Melder_integer (my nx), L" suspect.");
	if (Melder_hasError()) forget (thee);
	return thee;
}


int Formant_Frame_into_LPC_Frame (Formant_Frame me, LPC_Frame thee, double samplingPeriod)
{
	int i, j, m = 2, n = 2 * my nFormants;
	double *lpc, r, p, q, nyquist = 2.0 / samplingPeriod;
	
	if (my nFormants < 1) return 1;
	if ((lpc = NUMdvector (-1, n)) == NULL) return 0;
	
	lpc[0] = 1;
	for (i = 1; i <= my nFormants; i++)
	{
		double f = my formant[i].frequency;
		
		if (f > nyquist) continue;
		
		/*
			D(z): 1 + p z^-1 + q z^-2
		*/
		
		r = exp (- NUMpi * my formant[i].bandwidth * samplingPeriod);
		p = - 2 * r * cos (2 * NUMpi * f * samplingPeriod);
		q = r * r;
		
		for (j = m; j > 0; j--)
		{
			lpc[j] += p * lpc[j - 1] + q * lpc[j - 2];
		}
		
		m += 2;
	}
	
	n = thy nCoefficients < n ? thy nCoefficients : n;
	
	for (i = 1; i <= n ; i++)
	{
		thy a[i] = lpc[i];
	}
	
	thy gain = my intensity;
	NUMdvector_free (lpc, -1);
	return 1;
}

LPC Formant_to_LPC (Formant me, double samplingPeriod)
{
	LPC thee = NULL; long i, m = 2 * my maxnFormants;
	int status = 0;
	
	thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1,
		m, samplingPeriod);
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my nx; i++)
	{
		Formant_Frame f = & my frame[i];
		LPC_Frame lpc = & thy frame[i];
		m = 2 * f -> nFormants;
		
		if (! LPC_Frame_init (lpc, m) || 
			! Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod)) goto end;
	}
	
	status = 1;
	
end:
	
	if (status == 0) forget (thee);
	return thee;
}

/* End of file LPC_and_Formant.c */
