/* Spectrum_extensions.c
 *
 * Copyright (C) 1993-2007 David Weenink
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
 djmw 20010718
 djmw 20020813 GPL header
 djmw 20030929 Added a warning in Spectrum_drawPhases.
 djmw 20031023 New: Spectra_multiply, Spectrum_conjugate
 djmw 20040506 Changed warning message in Spectrum_drawPhases.
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20061218 Introduction of Melder_information<12...9>
*/

#include "Spectrum_extensions.h"
#include "Sound_and_Spectrum.h"
#include "NUM2.h"

#define SIGN(x,s) ((s) < 0 ? -fabs (x) : fabs(x))

#define NUM2pi 6.2831853071795864
#define THLCON 0.5
#define THLINC 1.5
#define EXP2   12

#define PPVPHA(x,y,test) ((test) ? atan2 (-(y),-(x)) : atan2 ((y),(x)))
#define PHADVT(xr,xi,yr,yi,xa) ((xa) > 0 ? ((xr)*(yr)+(xi)*(yi))/ (xa) : 0)

struct tribolet_struct
{
	double thlinc, thlcon;
	double ddf, dvtmn2;
	float *x;
	long nx, l, count;
	int reverse_sign;	
};

/*
	Perform modified Goertzel algorithm to calculate, at frequency 'freq_rad',
	the real and imaginary part of the spectrum and the d/df of the
	spectrum of x.	
	Reference: Bonzanigo (1978), IEEE Trans. ASSP, Vol. 26.
*/
static void getSpectralValues (struct tribolet_struct *tbs, double freq_rad,
	double *xr, double *xi, double *nxr, double *nxi)
{
	double cosf = cos (freq_rad), sinf = sin (freq_rad);
	double a = 2 * cosf, b, u1 = 0, u2 = u1, w1 = u1, w2 = u1;
	float *x = tbs -> x;
	long j, nx = tbs -> nx;
	
	for (j = 1; j <= nx; j++)
	{
		double xj = x[j];
		double u0 =           xj + a * u1 - u2;
		double w0 = (j - 1) * xj + a * w1 - w2;
		u2 = u1;
		u1 = u0;
		w2 = w1;
		w1 = w0;
	}

	/*
		Bonzanigo's phase correction
	*/
		
	a = freq_rad * (nx - 1);
	u1 =   cos (a);
	u2 = - sin (a);
	
	a = u1 - u2 * cosf;
	b =      u2 * sinf;
	*xr  = u1 * a - u2 * b;
	*xi  = u2 * a + u1 * b;
	
	a = w1 - w2 * cosf;
	b =      w2 * sinf;
	*nxr = u1 * a - u2 * b;
	*nxi = u2 * a + u1 * b;
	tbs -> count ++;
}

/*
	Find the closest unwrapped phase estimate from the two admissible
	phase values (a1 & a2).
*/
static int phase_check (double pv, double *phase, double thlcon)
{
	double a0 = (*phase - pv) / NUM2pi;
	long k = a0;
	double a1 = pv + k * NUM2pi;
	double a2 = a1 + SIGN (NUM2pi, a0);	
	double a3 = fabs (a1 - *phase);
	double a4 = fabs (a2 - *phase);

	if (a3 > thlcon && a4 > thlcon ) return 0;
	*phase = a3 > a4 ? a2 : a1;
	return 1;
}

/*
	Phase unwrapping based on Tribolet's adaptive integration method.
	the unwrapped phase estimate is returned.
*/
static double phase_unwrap (struct tribolet_struct *tbs, double pfreq,
	double ppv, double pdvt, double *pphase, double *ppdvt)
{
	double sdvt[25], sppv[25];
	double freq, phase, phase_inc;
	double delta, xr, xi, xmsq, nxr, nxi;
	long k, sindex[25], pindex = 1, sp = 1;
	
	sppv[sp] = ppv;
	sdvt[sp] = pdvt;
	sindex[sp] = tbs -> l + 1;

	goto p40;

p20:

	/*
		When the routine runs out of stack space, there probably is 
		a zero very near the unit circle that results in a jump of 
		pi in the phase.	
	*/

	if ((sindex[sp] - pindex) <= 1) return phase;

	/*	p30:
		Get the intermediate frequency value and compute its phase
		derivative and principal value.
	*/
	k = (sindex[sp] + pindex) / 2;
	freq = pfreq + (k - 1) * tbs -> ddf;
	getSpectralValues (tbs, freq, &xr, &xi, &nxr, &nxi);
	sindex[++sp] = k;
	sppv[sp] = PPVPHA (xr, xi, tbs -> reverse_sign);
	xmsq = xr * xr + xi * xi;
	sdvt[sp] = PHADVT (xr, xi, nxr, nxi, xmsq); 
	
p40:
	/*
		Evaluate the phase increment.
		If the phase increment, reduced by the expected linear phase
		increment, is greater than the specified threshold, adapt step size.
	*/
	
	delta = 0.5 * tbs -> ddf * (sindex[sp] - pindex);
	phase_inc = delta * (*ppdvt + sdvt[sp]);
	
	if (fabs (phase_inc - delta * tbs -> dvtmn2) > tbs -> thlinc) goto p20;

	phase = *pphase + phase_inc;
	
	if (! phase_check (sppv[sp], &phase, tbs -> thlcon)) goto p20;
	
	if (fabs (phase - *pphase) > NUMpi) goto p20;
	
	if (sp == 1) return phase;
	
	/*
		p10: Update previous estimate.
	*/
		
	pindex = sindex[sp];
	*pphase = phase;
	*ppdvt = sdvt[sp--];
	
	goto p40;
}

Matrix Spectrum_unwrap (Spectrum me)
{
	Sound x = NULL, nx = NULL;
	Spectrum snx = NULL;
	Matrix thee = NULL;
	struct tribolet_struct tbs;
	double pdvt, phase = 0, ppdvt, pphase, ppv;
	long i, nfft = 2, iphase;
	int remove_linear_part = 1;

	while (nfft < my nx - 1) nfft *= 2;
	nfft *= 2;

	if (nfft / 2 != my nx - 1) return Melder_errorp ("Spectrum_unwrapPhases: "
		" dimension of Spectrum is not (power of 2 - 1).");

	if ((x = Spectrum_to_Sound (me)) == NULL) return NULL;
			
	if ((nx = Data_copy (x)) == NULL) goto end;
	
	for (i = 1; i <= x -> nx; i++)
	{
		nx -> z[1][i] *= (i - 1);
	}
	
	if ((snx = Sound_to_Spectrum (nx, TRUE)) == NULL) goto end;
	
	if ((thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 
			1, 2, 2, 1, 1)) == NULL) goto end;
	
	/*
		Common variables.
	*/
	
	tbs.thlinc = THLINC;
	tbs.thlcon = THLCON;
	tbs.x = x -> z[1];
	tbs.nx = x -> nx;
	tbs.l =  (pow (2, EXP2) + 0.1);
	tbs.ddf = NUM2pi / ((tbs.l) * nfft);
	tbs.reverse_sign = my z[1][1] < 0;
	tbs.count = 0;
	
	/*
		Reuse snx : put phase derivative (d/df) in imaginary part.
	*/
	
	tbs.dvtmn2 = 0;
	for (i = 1; i <= my nx; i ++)
	{
		double xr = my z[1][i], xi = my z[2][i];
		double nxr = snx -> z[1][i], nxi = snx -> z[2][i];
		double xmsq = xr * xr + xi * xi;
		pdvt = PHADVT (xr, xi, nxr, nxi, xmsq);
		thy z[1][i] = xmsq;
		snx -> z[2][i] = pdvt;
		tbs.dvtmn2 += pdvt;
	}
	
	tbs.dvtmn2 = (2 * tbs.dvtmn2 - snx -> z[2][1] - snx -> z[2][my nx])
		/ (my nx - 1);
		
	Melder_progress (0.0, "Phase unwrapping");
	
	pphase = 0;
	ppdvt = snx -> z[2][1];
	thy z[2][1] = ppv = PPVPHA (my z[1][1], my z[2][1], tbs.reverse_sign);
	for (i = 2; i <= my nx; i ++)
	{
		double pfreq = NUM2pi * (i - 1) / nfft;
		pdvt = snx -> z[2][i];
		ppv = PPVPHA (my z[1][i], my z[2][i], tbs.reverse_sign);
		phase = phase_unwrap (&tbs, pfreq, ppv, pdvt, &pphase, &ppdvt);
		ppdvt = pdvt;
		thy z[2][i] = pphase = phase;
		if ((i % 10) == 1 && ! Melder_progress ((double)i / my nx, 
			"%ld phases from %ld unwrapped.", i, my nx)) goto end;
	}

	iphase = (phase / NUMpi + 0.1);
		
	if (remove_linear_part)
	{
		phase /= my nx - 1;
		for (i = 2; i <= my nx; i ++)
		{
			thy z[2][i] -= phase * (i - 1);
		}
	}
	Melder_information2 ("Number of spectral values: ", Melder_integer (tbs.count));
	Melder_information2 (" iphase = ", Melder_integer (iphase));
end:

	Melder_progress (1.0, NULL);
	forget (x); forget (nx); forget (snx);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

void Spectrum_drawPhases (Spectrum me, Graphics g, double fmin, double fmax,
	double phase_min, double phase_max, int unwrap, int garnish)
{
	Matrix thee; long i;
	int reverse_sign = my z[1][1] < 0;

	if (unwrap)
	{
		thee = Spectrum_unwrap (me);
		if (thee == NULL)
		{
		    Melder_warning ("Spectrum_drawPhases: Spectrum has not been unwrapped.");
		    return;
		}
	}
	else
	{
		if ((thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
			1, 2, 2, 1, 1)) == NULL) return;
		for (i = 1; i <= my nx; i ++)
		{
			thy z[2][i] = PPVPHA (my z[1][i], my z[2][i], reverse_sign);
		}
	}
	
	Matrix_drawRows (thee, g, fmin, fmax, 1.9, 2.1, phase_min, phase_max);
	if (garnish)
	{
	
	}	

	forget (thee);
}

Spectrum Spectra_multiply (Spectrum me, Spectrum thee)
{
	Spectrum him;
	long i;

	if (my nx != thy nx || my x1 != thy x1 || my xmax != thy xmax ||
		my dx != thy dx) return Melder_errorp 
		("Spectra_multiply: Dimensions of both spectra do not conform.");
		
	him = Data_copy (me);
	if (him == NULL) return NULL;
	
	for (i = 1; i <= his nx; i++)
	{
		his z[1][i] = my z[1][i] * thy z[1][i] - my z[2][i] * thy z[2][i];
		his z[2][i] = my z[1][i] * thy z[2][i] + my z[2][i] * thy z[1][i]; 
	}
	
	return him;
}

void Spectrum_conjugate (Spectrum me)
{
	long i;
	
	for (i = 1; i <= my nx; i++)
	{
		my z[2][i] = - my z[2][i];
	}
}


/* End of file Spectrum_extensions.c */
