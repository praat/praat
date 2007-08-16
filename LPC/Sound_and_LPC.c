/* Sound_and_LPC.c
 *
 * Copyright (C) 1994-2007 David Weenink
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
 djmw 20020625 GPL header
 djmw corrected a bug in Sound_into_LPC_Frame_marple that could crash praat when signal has only zero samples.
 djmw 20040303 Removed warning in Sound_to_LPC.
 djmw 20070103 Sound interface changes
*/

#include "Sound_and_LPC.h"
#include "Sound_extensions.h"
#include "Vector.h"
#include "Spectrum.h"
#include "NUM2.h"

#define LPC_METHOD_AUTO 1
#define LPC_METHOD_COVAR 2
#define LPC_METHOD_BURG 3
#define LPC_METHOD_MARPLE 4

/* Markel&Gray, LP of S, page 219
work[1..3*m+2]
r = & work[1]; // r[1..m+1]
a= & work[m+1+1]; // a[1..m+1]
rc = & work[m+1+m+1+1]; // rc[1..m]
for (i=1; i<= m+1+m+1+m;i++) work[i] = 0;
*/

#define LPC_METHOD_AUTO_WINDOW_CORRECTION 1

static int Sound_into_LPC_Frame_auto (Sound me, LPC_Frame thee, double *window_autocor)
{
	long i = 1; /* For error condition at end */
	long j, m = thy nCoefficients;
	float *r = NULL, *a = NULL, *rc = NULL, *x = my z[1];
	
	if (((r = NUMfvector (1, m + 1)) == NULL) ||
		((a = NUMfvector (1, m + 1)) == NULL) ||
		((rc = NUMfvector (1, m   )) == NULL)) goto end;
	
	for (i = 1; i <= m + 1; i++)
	{
		for (j = 1; j <= my nx - i + 1; j++)
		{
			r[i] += x[j] * x[j+i-1];
		}
	}
	if (Melder_debug == -2)
	{
		double *rr = NUMdvector (1, m + 1);
		for (i = 1; i <= m + 1; i++)
		{
			for (j = 1; j <= my nx - i + 1; j++)
			{
				rr[i] += x[j] * x[j+i-1];
			}
		}
		
		for (i = 2; i <= m + 1; i++)
		{
			r[i] = rr[i] / window_autocor[i];
		}
		r[1] = rr[1];
		NUMdvector_free (rr, 1);
	}
	if (r[1] == 0)
	{
		i = 1; /* ! */ goto end;
	}
	a[1] = 1; a[2] = rc[1] = - r[2] / r[1];
	thy gain = r[1] + r[2] * rc[1];
	for (i = 2; i <= m; i++)
	{
		float s = 0;
		for (j = 1; j <= i; j++)
		{
			s += r[i-j+2] * a[j];
		}
		rc[i] = - s / thy gain;
		for (j = 2; j <= i/2+1; j++)
		{
			float at = a[j] + rc[i] * a[i-j+2];
			a[i-j+2] += rc[i] * a[j];
			a[j] = at;
		}
		a[i+1] = rc[i]; thy gain += rc[i] * s;
		if (thy gain <= 0) goto end;
	}
	
end:

	i--;
	for (j = 1; j <= i; j++) thy a[j] = a[j+1];
	NUMfvector_free (rc, 1);
	NUMfvector_free (a, 1);
	NUMfvector_free (r, 1);
	if (i == m) return 1;
	thy nCoefficients = i;	
	for (j = i + 1; j <= m; j++) thy a[j] = 0;
	return 0;	
}

/* Markel&Gray, LP of S, page 221 
	work[1..m(m+1)/2+m+m+1+m+m+1]
	b = & work[1]
	grc = & work[m*(m+1)/2+1];
	a = & work[m*(m+1)/2+m+1];
	beta = & work [m+1)/2+m+m+1+1];
	cc = & work[m+1)/2+m+m+1+m+1]
	for (i=1; i<=m(m+1)/2+m+m+1+m+m+1;i++) work[i] = 0;
*/
static int Sound_into_LPC_Frame_covar (Sound me, LPC_Frame thee)
{
	long i = 1, j, k, n = my nx, m = thy nCoefficients; float *x = my z[1];
	float *b = NULL, *grc = NULL, *a = NULL, *beta = NULL, *cc = NULL;
	
	if (! (b =  NUMfvector (1, m * (m+1) / 2)) ||
		! (grc = NUMfvector (1, m)) ||
		! (a = NUMfvector (1, m + 1)) ||
		! (beta = NUMfvector (1, m)) ||
		! (cc = NUMfvector (1, m + 1))) goto end;

	thy gain = 0;
	for (i = m + 1; i <= n; i++)
	{
		thy gain += x[i] * x[i];
		cc[1] += x[i] * x[i-1];
		cc[2] += x[i-1] * x[i-1];
	}
	
	if (thy gain == 0)
	{
		i = 1; /* ! */ goto end;
	}
	
	b[1] = 1;
	beta[1] = cc[2];
	a[1] = 1;
	a[2] = grc[1] = -cc[1] / cc[2];
	thy gain += grc[1] * cc[1];
	
	for (i = 2; i <= m; i++) /*130*/
	{
		float s = 0; /* 20 */
		for (j = 1; j <= i; j++)
		{
			cc[i-j+2] = cc[i-j+1] + x[m-i+1] * x[m-i+j] - x[n-i+1] * x[n-i+j];
		}
		cc[1] = 0;
		for (j = m + 1; j <= n; j++)
		{
			cc[1] += x[j-i] * x[j]; /* 30 */
		}
		b[i*(i+1)/2] = 1;
		for (j = 1; j <= i - 1; j++) /* 70 */
		{
			float gam = 0;
			if (beta[j] < 0) goto end; 
			else if (beta[j] == 0) continue;
			for (k = 1; k <= j; k++)
			{
				gam += cc[k+1] * b[j*(j-1)/2+k]; /*50*/
			}
			gam /= beta[j];
			for (k = 1; k <= j; k++)
			{
				b[i*(i-1)/2+k] -= gam * b[j*(j-1)/2+k]; /*60*/
			}
		}
		
		beta[i] = 0;
		for (j = 1; j <= i; j++)
		{
			beta[i] += cc[j+1] * b[i*(i-1)/2+j]; /*80*/
		}
		if (beta[i] <= 0) goto end;
		
		for (j = 1; j <= i; j++)
		{
			s += cc[j] * a[j]; /*100*/
		}
		grc[i] = -s / beta[i];
		
		for (j = 2; j <= i; j++)
		{
			a[j] += grc[i] * b[i*(i-1)/2+j-1]; /*110*/
		}
		a[i+1] = grc[i];
		s = grc[i] * grc[i] * beta[i];
		thy gain -= s;
		if (thy gain <= 0) goto end;
	}
	
end:

	i--;
	
	for (j = 1; j <= i; j++)
	{
		thy a[j] = a[j+1];
	}
	
	NUMfvector_free (b, 1);
	NUMfvector_free (grc, 1);
	NUMfvector_free (a, 1);
	NUMfvector_free (beta, 1);
	NUMfvector_free (cc, 1);
	
	if (i == m) return 1;
	
	thy nCoefficients = i;	
	for (j = i + 1; j <= m; j++) thy a[j] = 0;
	
	return 0;	
}

static int Sound_into_LPC_Frame_burg (Sound me, LPC_Frame thee)
{
	long i; int status = 1;
	/*NUMmemcof (my z[1], my nx, thy nCoefficients, &thy gain, thy a); */
	status = NUMburg (my z[1], my nx, thy a, thy nCoefficients, &thy gain);
	thy gain *= my nx;
	for (i=1; i <= thy nCoefficients; i++) thy a[i] = -thy a[i];
	return status;
}

static int Sound_into_LPC_Frame_marple (Sound me, LPC_Frame thee, double tol1, double tol2)
{
	long k, m = 1, n = my nx, mmax = thy nCoefficients; int status = 1;
	float den, e0, h, q, q1, q2, q3, q4, q5, q6, q7, s, s1, u, v, w;
	float *a = thy a, *x = my z[1], *c = NULL, *d = NULL, *r = NULL;

	/*
	c = & work[1];
	d = & work[mmax+1+1];
	r = & work [mmax+1+mmax+1+1];
	for (k=1; k<= 3*(mmax+1); k++) work[k] = 0;	
	*/
	if (! (c = NUMfvector (1, mmax + 1)) ||
		! (d = NUMfvector (1, mmax + 1)) ||
		! (r = NUMfvector (1, mmax + 1)))
	{
		status = 0; goto end;
	}
	for (e0=0, k=1; k <= n; k++)
	{
		e0 += x[k] * x[k];
	}
	e0 *= 2;
	if (e0 == 0)
	{
		m = 0; goto end;
	}
	q1 = 1.0 / e0;
	q2 = q1 * x[1];
	v = q = q1 * x[1] * x[1];
	u = w = q1 * x[n] * x[n];
	den = 1.0 - q - w;
	q4 = 1.0 / den;
	q5 = 1.0 - q;
	q6 = 1.0 - w;
	s = h = q2 * x[n];
	thy gain = e0 * den;
	q1 = 1.0 / thy gain;
	c[1] = q1 * x[1];
	d[1] = q1 * x[n];
	for (s1 = 0.0, k = 1; k <= n - 1; k++)
	{
		s1 += x[k+1] * x[k];
	}
	r[1] = 2.0 * s1;
	a[1] = - q1 * r[1];
	thy gain *= (1.0 - a[1] * a[1]);
	while (m < mmax)
	{
		float delta, eOld = thy gain, f = x[m+1], b = x[n-m]; /*n-1 ->n-m*/
		float alf, c1, c2, c3, c4, c5, c6, y1, y2, y3, y4, y5;
		for (k = 1; k <= m; k++)
		{	/* n-1 -> n-m */
			f += x[m+1-k] * a[k];
			b += x[n-m+k] * a[k];
		}
		q1 = 1.0 / thy gain;
		q2 = q1 * f;
		q3 = q1 * b;
		for (k = m; k >= 1; k--)
		{
			c[k+1] = c[k] + q2 * a[k];
			d[k+1] = d[k] * q3 * a[k];
		}
		c[1] = q2; d[1] = q3;
		q7 = s * s;
		y1 = f * f;
		y2 = v * v;
		y3 = b * b;
		y4 = u * u;
		y5 = 2.0 * h * s;
		q += y1 * q1 + q4 * (y2 * q6 + q7 * q5 + v * y5);
		w += y3 * q1 + q4 * (y4 * q5 + q7 * q6 + u * y5);
		for (h = s = u = v = 0, k = 0; k <= m; k++)
		{
			h += x[n-m+k] * c[k+1];
			s += x[n-k] * c[k+1];
			u += x[n-k] * d[k+1];
			v += x[k+1] * c[k+1];
		}
		q5 = 1.0 - q;
		q6 = 1.0 - w;
		den = q5 * q6 - h * h;
		if (den <= 0)
		{
			status = 2; goto end; /* 2: ill-conditioning */
		}
		q4 = 1.0 / den;
		q1 *= q4;
		alf = 1.0 / (1.0 + q1 * (y1 * q6 + y3 * q5 + 2.0 * h * f * b));
 		thy gain *= alf;
 		y5 = h * s;
		c1 = q4 * (f * q6 + b * h);
		c2 = q4 * (b * q5 + h * f);
		c3 = q4 * (v * q6 + y5);
		c4 = q4 * (s * q5 + v * h);
 		c5 = q4 * (s * q6 + h * u);
 		c6 = q4 * (u * q5 + y5);
		for (k = 1; k <= m; k++)
		{
			a[k] = alf * (a[k] + c1 * c[k+1] + c2 * d[k+1]);
		}
		for (k = 1; k <= m / 2 + 1; k++)
		{
			float s1 = c[k], s2 = d[k], s3 = c[m+2-k], s4 = d[m+2-k];
			c[k] += c3 * s3 + c4 * s4;
			d[k] += c5 * s3 + c6 * s4;
        	if (m + 2 - k == k) continue;
        	c[m+2-k] += c3 * s1 + c4 * s2;
        	d[m+2-k] += c5 * s1 + c6 * s2;
		}
		m++; c1 = x[n+1-m]; c2 = x[m];
		for (delta = 0, k = m - 1; k >= 1; k--)
		{
			r[k+1] = r[k] - x[n+1-k] * c1 - x[k] * c2;
			delta += r[k+1] * a[k];
		}
		for (s1 = 0.0, k = 1; k <= n - m; k++)
		{
			s1 += x[k+m] * x[k];
		}
		r[1] = 2.0 * s1;
		delta += r[1];
		q2 = - delta / thy gain;
		a[m] = q2;
		for (k = 1; k <= m / 2; k++)
		{
			s1 = a[k];
			a[k] += q2 * a[m-k];
			if (k == m-k) continue;
			a[m-k] += q2 * s1;
		}
		y1 = q2 * q2;
		thy gain *= 1.0 - y1;
		if (y1 >= 1.0)
		{
			status = 3; goto end; /* |a[m]| > 1 */
		}
		if (thy gain < e0 * tol1)
		{
			status = 4; goto end;
		} 
		if (eOld - thy gain < eOld * tol2)
		{
			status = 5; goto end;
		}
	} 
	
end:

	thy gain *= 0.5; /* because e0 is twice the energy */
	thy nCoefficients = m;
	NUMfvector_free (c, 1);
	NUMfvector_free (d, 1);
	NUMfvector_free (r, 1);
	
	return status == 1 || status == 4 || status == 5;
}

static LPC _Sound_to_LPC (Sound me, int predictionOrder, double analysisWidth, double dt, 
	double preEmphasisFrequency, int method, double tol1, double tol2)
{
	char *proc = "Sound_to_LPC";
	Sound sound = NULL, sframe = NULL, window = NULL;
	LPC thee = NULL;
	double *window_autocor = NULL;
	double t1, samplingFrequency = 1.0 / my dx;
	double windowDuration = 2 * analysisWidth; /* gaussian window */
	long nFrames, i, frameErrorCount = 0;
	
	if (floor (windowDuration / my dx) < predictionOrder + 1)
	{
		(void) Melder_error ("%s: Analysis window duration too short.\n", proc);
		return Melder_errorp ("For a prediction order of  %d the analysis window duration has to be greater than %ls s.\n\n"
			"Please increase the analysis window duration or lower the prediction order.", predictionOrder,
			Melder_double (my dx * (predictionOrder+1)));
	}

	if (! Sampled_shortTermAnalysis (me, windowDuration, dt, & nFrames, & t1) ||
		! (sound = (Sound) Data_copy (me)) ||
		! (sframe = Sound_createSimple (1, windowDuration, samplingFrequency)) ||
		! (window = Sound_createGaussian (windowDuration, samplingFrequency)) ||
		! (thee = LPC_create (my xmin, my xmax, nFrames, dt, t1, predictionOrder, my dx))) goto end;
		 
	if (Melder_debug == -2)
	{
		/* Auto-correlation of window */
		long window_nx, j;
		window_autocor = NUMdvector (1, predictionOrder+1);
		window_nx = window -> nx;
		for (i = 1; i <= predictionOrder + 1; i++)
		{
			for (j = 1; j <= window_nx - i + 1; j++)
			{
				window_autocor[i] += window -> z[1][j] * window -> z[1][i+j-1];
			}
		}
		/* Normalize */
		for (i = 2; i<= predictionOrder + 1; i++)
		{
			window_autocor[i] /= window_autocor[1];
		}
		window_autocor[1] = 1;
	}
	Melder_progress (0.0, "LPC analysis");
	
	if (preEmphasisFrequency < samplingFrequency / 2) Sound_preEmphasis (sound, preEmphasisFrequency);	 	
	
	for (i = 1; i <= nFrames; i++)
	{
		LPC_Frame lpcframe = & thy frame[i];
		double t = Sampled_indexToX (thee, i);
		if (! LPC_Frame_init (lpcframe, predictionOrder)) goto end;
		Sound_into_Sound (sound, sframe, t - windowDuration / 2);
		(void) Vector_subtractMean (sframe);
		Sounds_multiply (sframe, window);
		if ((method == LPC_METHOD_AUTO && ! Sound_into_LPC_Frame_auto (sframe, lpcframe, window_autocor)) ||
			(method == LPC_METHOD_COVAR && ! Sound_into_LPC_Frame_covar (sframe, lpcframe)) ||
			(method == LPC_METHOD_BURG && ! Sound_into_LPC_Frame_burg (sframe, lpcframe)) ||
			(method == LPC_METHOD_MARPLE && ! Sound_into_LPC_Frame_marple (sframe, lpcframe, tol1, tol2)))
			frameErrorCount++;
		if ((i % 10) == 1 && ! Melder_progress ((double)i / nFrames, 
			"%s: LPC analysis of frame %d out of %d", proc, i, nFrames)) goto end;
	}

end:
	Melder_progress (1.0, NULL);
	if (Melder_debug == -2) NUMdvector_free (window_autocor, 1);
	forget (sound);
	forget (sframe);
	forget (window);
	if (Melder_hasError ()) forget (thee);
	return thee;
}
 
LPC Sound_to_LPC_auto (Sound me, int predictionOrder, double analysisWidth, double dt, 
	double preEmphasisFrequency)
{
	return _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, 
		preEmphasisFrequency, LPC_METHOD_AUTO, 0, 0);
}

LPC Sound_to_LPC_covar (Sound me, int predictionOrder, double analysisWidth, double dt, 
	double preEmphasisFrequency)
{
	return _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, 
		preEmphasisFrequency, LPC_METHOD_COVAR, 0, 0);
}

LPC Sound_to_LPC_burg (Sound me, int predictionOrder, double analysisWidth, double dt, 
	double preEmphasisFrequency)
{
	return _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, 
		preEmphasisFrequency, LPC_METHOD_BURG, 0, 0);
}

LPC Sound_to_LPC_marple (Sound me, int predictionOrder, double analysisWidth, double dt, 
	double preEmphasisFrequency, double tol1, double tol2)
{
	return _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, 
		preEmphasisFrequency, LPC_METHOD_MARPLE, tol1, tol2);
}

Sound LPC_and_Sound_filterInverse (LPC me, Sound thee)
{
	char *proc = "LPC_and_Sound_filterInverse";
	Sound him; float *x = thy z[1], *e; long i;
	
	if (my samplingPeriod != thy dx) Melder_warning 
		("%s: Sampling frequencies are not the same.", proc); 
	if (my xmin != thy xmin || thy xmax != my xmax) return Melder_errorp 
		("%s: Domains of LPC and Sound are not equal.", proc);
	if ((him = Data_copy (thee)) == NULL) return NULL;
	
	e = his z[1];
	for (i = 1; i <= his nx; i++)
	{
		double t = his x1 + (i - 1) * his dx; /* Sampled_indexToX (him, i) */
		long iFrame = floor ((t - my x1) / my dx + 1.5); /* Sampled_xToNearestIndex (me, t) */
		float *a; long m, j;
		if (iFrame < 1 || iFrame > my nx) { e[i] = 0; continue; }
		a = my frame[iFrame].a;
		m = i > my frame[iFrame].nCoefficients ? my frame[iFrame].nCoefficients : i-1;
		for (j=1; j <= m; j++) e[i] += a[j] * x[i-j];
	}
	return him;
}

/*
	gain used as a constant amplitude multiplyer within a frame of duration my dx.
	future alternative: convolve gain with a  smoother.
*/		
Sound LPC_and_Sound_filter (LPC me, Sound thee, int useGain)
{
	char *proc = "LPC_and_Sound_filter";
	Sound him; long i, ifirst = 0, ilast = 0; float *x;
	
	if (my samplingPeriod != thy dx) Melder_warning 
		("%s: Sampling frequencies are not the same.", proc); 
	if (my xmin != thy xmin || my xmax != thy xmax) Melder_warning
		("%s: Time domains of source and filter do not match.", proc);
	if ((him = Data_copy (thee)) == NULL) return him;
	
	x = his z[1];
	for (i = 1; i <= his nx; i++)
	{
		double t = his x1 + (i - 1) * his dx; /* Sampled_indexToX (him, i) */
		long iFrame = floor ((t - my x1) / my dx + 1.5); /* Sampled_xToNearestIndex (me, t) */
		float *a; long m, j; 
		if (iFrame < 1) { ifirst = i; continue; }
		if (iFrame > my nx) { ilast = i; break; } 
		a = my frame[iFrame].a;
		m = i > my frame[iFrame].nCoefficients ? my frame[iFrame].nCoefficients : i-1;
		for (j=1; j <= m; j++) x[i] -= a[j] * x[i-j];
	}
	/*
		Make samples before first frame and after last frame zero.
	*/
	for (i = 1; i < ifirst; i++) x[i] = 0;
	if (ilast > 0) for (i=ilast+1; i <= his nx; i++) x[i] = 0;
	if (useGain) for (i=1; i <= his nx; i++)
	{
		double t = his x1 + (i - 1) * his dx; /* Sampled_indexToX (him, i) */
		double riFrame = (t - my x1) / my dx + 1; /* Sampled_xToIndex (me, t); */
		long iFrame = floor (riFrame);
		double phase = riFrame - iFrame;
		if (iFrame < 0 || iFrame > my nx) x[i] = 0;
		else if (iFrame == 0) x[i] *= sqrt (my frame[1].gain) * phase;
		else if (iFrame == my nx) x[i] *= sqrt (my frame [my nx].gain) * (1 - phase);
		else x[i] *=
			phase * sqrt (my frame[iFrame+1].gain)+(1-phase)*sqrt (my frame[iFrame].gain);
	}
	if (Melder_hasError()) forget (him);
	return him;
}

void LPC_Frame_and_Sound_filterInverse (LPC_Frame me, Sound out, Sound source)
{
	long i, j, m, n = out -> nx > source -> nx ? source -> nx : out -> nx;
	float *e = source -> z[1], *x = out -> z[1];
	
	for (i = 1; i <= n; i++)
	{
		m = i > my nCoefficients ? my nCoefficients : i - 1;	
		for (e[i] = x[i], j = 1; j <= m; j++)
		{
			e[i] += my a[j] * x[i-j];
		}
	} 	
}

/* End of file Sound_and_LPC.c */
