/* Sound_and_LPC_robust.c
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
 djmw 20030814 First version
 djmw 20061218 To Melder_information<x> format
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
*/

#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_extensions.h"
#include "SVD.h"
#include "Vector.h"
#include "NUM2.h"

struct huber_struct
{
	Sound e;
	double k, tol, tol_svd;
	long iter, itermax;
	int wantlocation, wantscale;
	double location, scale;
	long n, p;
	double *w, *work;
	double *a;
	double **covar, *c;
	SVD svd;
};

static int huber_struct_init (struct huber_struct *hs, double windowDuration,
	long p,	double samplingFrequency, double location, int wantlocation)
{
	long n;
		
	hs -> e = Sound_createSimple (1, windowDuration, samplingFrequency);
	if (hs -> e == NULL) return 0;
	n = hs -> e -> nx;
	hs -> n = n;
	hs -> p = p;
	
	hs -> w = NUMdvector (1, n);
	if (hs -> w == NULL) return 0;
	
	hs -> work = NUMdvector (1, n);
	if (hs -> work == NULL) return 0;
	
	hs -> a = NUMdvector (1, p);
	if (hs -> a == NULL) return 0;
	
	hs -> covar = NUMdmatrix (1, p, 1, p);
	if (hs -> covar == NULL) return 0;
	
	hs -> c = NUMdvector (1, p);
	if (hs -> c == NULL) return 0;
	
	hs -> svd = SVD_create (p, p);
	if (hs -> svd == NULL) return 0;
	
	hs -> wantlocation = wantlocation;
	if (! wantlocation) hs -> location = location;
	
	hs -> wantscale = 1;
	
	return 1;	
}

static void huber_struct_destroy (struct huber_struct *hs)
{
	forget (hs -> e);
	forget (hs -> svd);
	NUMdvector_free (hs -> w, 1);
	NUMdvector_free (hs -> work, 1);
	NUMdvector_free (hs -> a, 1);
	NUMdmatrix_free (hs -> covar, 1, 1);
	NUMdvector_free (hs -> c, 1);
}

static void huber_struct_getWeights (struct huber_struct *hs, double *e)
{
	double ks = hs -> k * hs -> scale;
	double *w = hs -> w;
	long i;
	
	for (i = 1 ; i <= hs -> n; i++)
	{
		double ei = e[i] - hs -> location;
		w[i] = ei > -ks && ei < ks ? 1 : ks / fabs (ei);	
	}
}

static void huber_struct_getWeightedCovars (struct huber_struct *hs, double *s)
{
	long i, j, k, p = hs -> p, n = hs -> n;
	double tmp, *w = hs -> w, **covar = hs -> covar, *c = hs -> c;
		
	for (i = 1; i <= p; i++)
	{
		for (j = i; j <= p; j++)
		{
			tmp = 0;
			for (k = p + 1; k <= n; k++)
			{
				tmp += s[k - j] * s[k - i] * w[k];
			}
			covar[i][j] = covar[j][i] = tmp;
		}
		
		tmp = 0;
		for (k = p + 1; k <= n; k++)
		{
			tmp += s[k - i] * s[k] * w[k];
		}
		c[i] = -tmp;
	}
		
}

static int huber_struct_solvelpc (struct huber_struct *hs)
{
	SVD me = hs -> svd;
	double **covar = hs -> covar;
	long i, j, nzeros;

	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			my u[i][j] = covar[i][j];
		}
	}
	
	SVD_setTolerance (me, hs -> tol_svd);
	if (! SVD_compute (me)) return 0;
	
	nzeros = SVD_zeroSmallSingularValues (me, 0);
	
	if (! SVD_solve (me, hs -> c, hs -> a)) return 0;

	return 1;
}

int LPC_Frames_and_Sound_huber (LPC_Frame me, Sound thee,
	LPC_Frame him, void *huber)
{
	struct huber_struct *hs = (struct huber_struct *) huber;
	long p = my nCoefficients > his nCoefficients ? his nCoefficients : 
						my nCoefficients;
	long i, n = hs -> e -> nx > thy nx ? thy nx : hs -> e -> nx;
	double *e = hs -> e -> z[1], *s = thy z[1];
	double s0;

	hs -> iter = 0;
	hs -> scale = 1e38;
	hs -> p = p;
	
	do
	{
		LPC_Frame_and_Sound_filterInverse (him, thee, hs -> e);
		
		s0 = hs -> scale;
		
		if (! NUMstatistics_huber (e, n, &(hs -> location), 
			hs -> wantlocation, &(hs -> scale), hs -> wantscale, 
			hs -> k, hs -> tol, hs -> work)) return 0;

		huber_struct_getWeights (hs, e);
		
		huber_struct_getWeightedCovars (hs, s);
		
		/* Solve C a = [-] c */
		if (! huber_struct_solvelpc (hs))
		{
			/* Copy the starting lpc coeffs */
			for (i = 1; i <= p; i++) his a[i] = my a[i];
			return 0;
		}
		for (i = 1; i <= p; i++) his a[i] = hs -> a[i];
		
		(hs -> iter)++;
	} while ((hs -> iter < hs -> itermax) && (fabs (s0 - hs -> scale) > hs -> tol * s0));
	
	return 1;
}


LPC LPC_and_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth,
	double preEmphasisFrequency, double k, int itermax, double tol, 
	int wantlocation)
{
	struct huber_struct struct_huber;
	Sound sound = NULL, sframe = NULL, window = NULL;
	LPC him = NULL;
	double t1, samplingFrequency = 1.0 / my dx, dt = thy dx;
	double windowDuration = 2 * analysisWidth; /* Gaussian window */
	double location = 0, tol_svd = 0.000001;
	long nFrames, i, frameErrorCount = 0, iter = 0;
	long p = thy maxnCoefficients;

	if (my xmin != thy xmin || my xmax != thy xmax)
	{
		return Melder_errorp1 (L"Time domains differ.");
	}
	if (my dx != thy samplingPeriod)
	{
		return Melder_errorp1 (L"Sampling intervals differ.");
	}
	if (floor (windowDuration / my dx) < p + 1)
	{
		return Melder_errorp1 (L"Analysis window too short.");
	}
	if (! Sampled_shortTermAnalysis (me, windowDuration, dt, & nFrames, & t1))
	{
		return NULL;
	}
	if (nFrames != thy nx || t1 != thy x1)
	{
		return Melder_errorp1 (L"Incorrect retrieved analysis width");
	}
	
	sound = (Sound) Data_copy (me);
	if (sound == NULL) goto end;
	sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
	if (sframe == NULL) goto end;
	window = Sound_createGaussian (windowDuration, samplingFrequency);
	if (window == NULL) goto end;
	him = Data_copy (thee);
	if (him == NULL) goto end;
	if (! huber_struct_init (&struct_huber, windowDuration, p, samplingFrequency,
			location, wantlocation)) goto end;

	struct_huber.k = k;
	struct_huber.tol = tol;
	struct_huber.tol = tol_svd;
	struct_huber.itermax = itermax;
	
	Melder_progress1 (0.0, L"LPC analysis");

	Sound_preEmphasis (sound, preEmphasisFrequency);

	for (i = 1; i <= nFrames; i++)
	{
		LPC_Frame lpc = & thy frame[i], lpcto = & his frame[i];
		double t = Sampled_indexToX (thee, i);
		
		Sound_into_Sound (sound, sframe, t - windowDuration / 2);
		(void) Vector_subtractMean (sframe);
		Sounds_multiply (sframe, window);
		
		if (! LPC_Frames_and_Sound_huber (lpc, sframe, lpcto, & struct_huber))
		{
			frameErrorCount++;
		}
		
		iter += struct_huber.iter;
		
		if ((i % 10) == 1 && ! Melder_progress5 ((double)i / nFrames, L"LPC analysis of frame ",
			Melder_integer (i), L" out of ", Melder_integer (nFrames), L".")) goto end;
	}
	
	if (frameErrorCount) Melder_warning5 (L"Results of ", Melder_integer (frameErrorCount), 
		L" frame(s) out of ", Melder_integer (nFrames), L" could not be optimised.");

end:

	Melder_progress1 (1.0, NULL);
	forget (sound); forget (sframe); forget (window);
	huber_struct_destroy (&struct_huber);
	if (Melder_hasError ()) forget (him);
	
	MelderInfo_writeLine4 (L"Number of iterations: ", Melder_integer (iter),
		L"\n   Average per frame: ", Melder_double (((double) iter)/nFrames));
	return him;
}

/* End of file Sound_and_LPC_robust.c */
