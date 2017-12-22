/* Sound_and_LPC.cpp
 *
 * Copyright (C) 1994-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020625 GPL header
 djmw corrected a bug in Sound_into_LPC_Frame_marple that could crash praat when signal has only zero samples.
 djmw 20040303 Removed warning in Sound_to_LPC.
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
 djmw 20101009 Filter and inverseFilter with one frame.
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
work [1..3*m+2]
r = & work [1]; // r [1..m+1]
a= & work [m+1+1]; // a [1..m+1]
rc = & work [m+1+m+1+1]; // rc [1..m]
for (i=1; i<= m+1+m+1+m;i ++) work [i] = 0;
*/

#define LPC_METHOD_AUTO_WINDOW_CORRECTION 1

static void LPC_Frame_Sound_filter (LPC_Frame me, Sound thee, integer channel) {
	double *y = thy z [channel], *a = my a;

	for (integer i = 1; i <= thy nx; i ++) {
		integer m = i > my nCoefficients ? my nCoefficients : i - 1;
		for (integer j = 1; j <= m; j ++) {
			y [i] -= a [j] * y [i - j];
		}
	}
}

void LPC_Frame_Sound_filterInverse (LPC_Frame me, Sound thee, integer channel) {
	double *x = thy z [channel];
	autoNUMvector <double> y ((integer) 0, my nCoefficients);
	for (integer i = 1; i <= thy nx; i ++) {
		y [0] = x [i];
		for (integer j = 1; j <= my nCoefficients; j ++) {
			x [i] += my a [j] * y [j];
		}
		for (integer j = my nCoefficients; j > 0; j--) {
			y [j] = y [j - 1];
		}
	}
}

static int Sound_into_LPC_Frame_auto (Sound me, LPC_Frame thee) {
	integer i = 1; // For error condition at end
	integer m = thy nCoefficients;

	autoNUMvector<double> r (1, m + 1);
	autoNUMvector<double> a (1, m + 1);
	autoNUMvector<double> rc (1, m);

	double  *x = my z [1];
	for (i = 1; i <= m + 1; i ++) {
		for (integer j = 1; j <= my nx - i + 1; j ++) {
			r [i] += x [j] * x [j + i - 1];
		}
	}
	if (r [1] == 0.0) {
		i = 1; /* ! */ goto end;
	}
	a [1] = 1.0; a [2] = rc [1] = - r [2] / r [1];
	thy gain = r [1] + r [2] * rc [1];
	for (i = 2; i <= m; i ++) {
		double s = 0.0;
		for (integer j = 1; j <= i; j ++) {
			s += r [i - j + 2] * a [j];
		}
		rc [i] = - s / thy gain;
		for (integer j = 2; j <= i / 2 + 1; j ++) {
			double at = a [j] + rc [i] * a [i - j + 2];
			a [i - j + 2] += rc [i] * a [j];
			a [j] = at;
		}
		a [i + 1] = rc [i]; thy gain += rc [i] * s;
		if (thy gain <= 0) {
			goto end;
		}
	}
end:
	i--;
	for (integer j = 1; j <= i; j ++) {
		thy a [j] = a [j + 1];
	}
	if (i == m) {
		return 1;
	}
	thy nCoefficients = i;
	for (integer j = i + 1; j <= m; j ++) {
		thy a [j] = 0.0;
	}
	return 0; // Melder_warning ("Less coefficienst than asked for.");
}

/* Markel&Gray, LP of S, page 221
	work [1..m(m+1)/2+m+m+1+m+m+1]
	b = & work [1]
	grc = & work [m*(m+1)/2+1];
	a = & work [m*(m+1)/2+m+1];
	beta = & work  [m+1)/2+m+m+1+1];
	cc = & work [m+1)/2+m+m+1+m+1]
	for (i=1; i<=m(m+1)/2+m+m+1+m+m+1;i ++) work [i] = 0;
*/
static int Sound_into_LPC_Frame_covar (Sound me, LPC_Frame thee) {
	integer i = 1, n = my nx, m = thy nCoefficients;
	double *x = my z [1];

	autoNUMvector<double> b (1, m * (m + 1) / 2);
	autoNUMvector<double> grc (1, m);
	autoNUMvector<double> a (1, m + 1);
	autoNUMvector<double> beta (1, m);
	autoNUMvector<double> cc (1, m + 1);

	thy gain = 0.0;
	for (i = m + 1; i <= n; i ++) {
		thy gain += x [i] * x [i];
		cc [1] += x [i] * x [i - 1];
		cc [2] += x [i - 1] * x [i - 1];
	}

	if (thy gain == 0.0) {
		i = 1; /* ! */ goto end;
	}

	b [1] = 1.0;
	beta [1] = cc [2];
	a [1] = 1.0;
	a [2] = grc [1] = -cc [1] / cc [2];
	thy gain += grc [1] * cc [1];

	for (i = 2; i <= m; i ++) { /*130*/
		double s = 0.0; /* 20 */
		for (integer j = 1; j <= i; j ++) {
			cc [i - j + 2] = cc [i - j + 1] + x [m - i + 1] * x [m - i + j] - x [n - i + 1] * x [n - i + j];
		}
		cc [1] = 0.0;
		for (integer j = m + 1; j <= n; j ++) {
			cc [1] += x [j - i] * x [j]; /* 30 */
		}
		b [i * (i + 1) / 2] = 1.0;
		for (integer j = 1; j <= i - 1; j ++) { /* 70 */
			double gam = 0.0;
			if (beta [j] < 0.0) {
				goto end;
			} else if (beta [j] == 0.0) {
				continue;
			}
			for (integer k = 1; k <= j; k ++) {
				gam += cc [k + 1] * b [j * (j - 1) / 2 + k]; /*50*/
			}
			gam /= beta [j];
			for (integer k = 1; k <= j; k ++) {
				b [i * (i - 1) / 2 + k] -= gam * b [j * (j - 1) / 2 + k]; /*60*/
			}
		}

		beta [i] = 0.0;
		for (integer j = 1; j <= i; j ++) {
			beta [i] += cc [j + 1] * b [i * (i - 1) / 2 + j]; /*80*/
		}
		if (beta [i] <= 0.0) {
			goto end;
		}

		for (integer j = 1; j <= i; j ++) {
			s += cc [j] * a [j]; /*100*/
		}
		grc [i] = -s / beta [i];

		for (integer j = 2; j <= i; j ++) {
			a [j] += grc [i] * b [i * (i - 1) / 2 + j - 1]; /*110*/
		}
		a [i + 1] = grc [i];
		s = grc [i] * grc [i] * beta [i];
		thy gain -= s;
		if (thy gain <= 0.0) {
			goto end;
		}
	}
end:
	i--;

	for (integer j = 1; j <= i; j ++) {
		thy a [j] = a [j + 1];
	}
	if (i == m) {
		return 1;
	}

	thy nCoefficients = i;
	for (integer j = i + 1; j <= m; j ++) {
		thy a [j] = 0.0;
	}
	return 0; // Melder_warning ("Less coefficienst than asked for.");
}

static int Sound_into_LPC_Frame_burg (Sound me, LPC_Frame thee) {
	int status = NUMburg (my z [1], my nx, thy a, thy nCoefficients, &thy gain);
	thy gain *= my nx;
	for (integer i = 1; i <= thy nCoefficients; i ++) {
		thy a [i] = -thy a [i];
	}
	return status;
}

static int Sound_into_LPC_Frame_marple (Sound me, LPC_Frame thee, double tol1, double tol2) {
	integer m = 1, n = my nx, mmax = thy nCoefficients;
	int status = 1;
	double *a = thy a, *x = my z [1];

	autoNUMvector<double> c (1, mmax + 1);
	autoNUMvector<double> d (1, mmax + 1);
	autoNUMvector<double> r (1, mmax + 1);
	double e0 = 0.0;
	for (integer k = 1; k <= n; k ++) {
		e0 += x [k] * x [k];
	}
	e0 *= 2.0;
	if (e0 == 0.0) {
		m = 0; thy gain *= 0.5; /* because e0 is twice the energy */
		thy nCoefficients = m; return 0; // warning no signal
	}
	double q1 = 1.0 / e0;
	double q2 = q1 * x [1], q = q1 * x [1] * x [1], w = q1 * x [n] * x [n];
	double v = q, u = w;
	double den = 1.0 - q - w;
	double q4 = 1.0 / den, q5 = 1.0 - q, q6 = 1.0 - w;
	double h = q2 * x [n], s = h;
	thy gain = e0 * den;
	q1 = 1.0 / thy gain;
	c [1] = q1 * x [1];
	d [1] = q1 * x [n];
	double s1 = 0.0;
	for (integer k = 1; k <= n - 1; k ++) {
		s1 += x [k + 1] * x [k];
	}
	r [1] = 2.0 * s1;
	a [1] = - q1 * r [1];
	thy gain *= (1.0 - a [1] * a [1]);
	while (m < mmax) {
		double eOld = thy gain, f = x [m + 1], b = x [n - m]; /*n-1 ->n-m*/
		for (integer k = 1; k <= m; k ++) {
			/* n-1 -> n-m */
			f += x [m + 1 - k] * a [k];
			b += x [n - m + k] * a [k];
		}
		q1 = 1.0 / thy gain;
		q2 = q1 * f;
		double q3 = q1 * b;
		for (integer k = m; k >= 1; k--) {
			c [k + 1] = c [k] + q2 * a [k];
			d [k + 1] = d [k] * q3 * a [k];
		}
		c [1] = q2; d [1] = q3;
		double q7 = s * s;
		double y1 = f * f;
		double y2 = v * v;
		double y3 = b * b;
		double y4 = u * u;
		double y5 = 2.0 * h * s;
		q += y1 * q1 + q4 * (y2 * q6 + q7 * q5 + v * y5);
		w += y3 * q1 + q4 * (y4 * q5 + q7 * q6 + u * y5);
		h = s = u = v = 0.0;
		for (integer k = 0; k <= m; k ++) {
			h += x [n - m + k] * c [k + 1];
			s += x [n - k] * c [k + 1];
			u += x [n - k] * d [k + 1];
			v += x [k + 1] * c [k + 1];
		}
		q5 = 1.0 - q;
		q6 = 1.0 - w;
		den = q5 * q6 - h * h;
		if (den <= 0.0) {
			status = 2; goto end; /* 2: ill-conditioning */
		}
		q4 = 1.0 / den;
		q1 *= q4;
		double alf = 1.0 / (1.0 + q1 * (y1 * q6 + y3 * q5 + 2.0 * h * f * b));
		thy gain *= alf;
		y5 = h * s;
		double c1 = q4 * (f * q6 + b * h);
		double c2 = q4 * (b * q5 + h * f);
		double c3 = q4 * (v * q6 + y5);
		double c4 = q4 * (s * q5 + v * h);
		double c5 = q4 * (s * q6 + h * u);
		double c6 = q4 * (u * q5 + y5);
		for (integer k = 1; k <= m; k ++) {
			a [k] = alf * (a [k] + c1 * c [k + 1] + c2 * d [k + 1]);
		}
		for (integer k = 1; k <= m / 2 + 1; k ++) {
			s1 = c [k];
			double s2 = d [k], s3 = c [m + 2 - k], s4 = d [m + 2 - k];

			c [k] += c3 * s3 + c4 * s4;
			d [k] += c5 * s3 + c6 * s4;
			if (m + 2 - k == k) {
				continue;
			}
			c [m + 2 - k] += c3 * s1 + c4 * s2;
			d [m + 2 - k] += c5 * s1 + c6 * s2;
		}
		m ++; c1 = x [n + 1 - m]; c2 = x [m];
		double delta = 0;
		for (integer k = m - 1; k >= 1; k--) {
			r [k + 1] = r [k] - x [n + 1 - k] * c1 - x [k] * c2;
			delta += r [k + 1] * a [k];
		}
		s1 = 0.0;
		for (integer k = 1; k <= n - m; k ++) {
			s1 += x [k + m] * x [k];
		}
		r [1] = 2.0 * s1;
		delta += r [1];
		q2 = - delta / thy gain;
		a [m] = q2;
		for (integer k = 1; k <= m / 2; k ++) {
			s1 = a [k];
			a [k] += q2 * a [m - k];
			if (k == m - k) {
				continue;
			}
			a [m - k] += q2 * s1;
		}
		y1 = q2 * q2;
		thy gain *= 1.0 - y1;
		if (y1 >= 1.0) {
			status = 3; goto end; /* |a [m]| > 1 */
		}
		if (thy gain < e0 * tol1) {
			status = 4; goto end;
		}
		if (eOld - thy gain < eOld * tol2) {
			status = 5; goto end;
		}
	}
end:
	thy gain *= 0.5; /* because e0 is twice the energy */
	thy nCoefficients = m;
	return status == 1 || status == 4 || status == 5;
}

static autoLPC _Sound_to_LPC (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency, int method, double tol1, double tol2) {
	double t1, samplingFrequency = 1.0 / my dx;
	double windowDuration = 2 * analysisWidth; /* gaussian window */
	integer numberOfFrames, frameErrorCount = 0;
	Melder_require (Melder_roundDown (windowDuration / my dx) > predictionOrder, 
		U"Analysis window duration too short.\n For a prediction order of ", predictionOrder,
		U" the analysis window duration should be greater than ", my dx * (predictionOrder + 1), U"Please increase the analysis window duration or lower the prediction order.");
	
	// Convenience: analyse the whole sound into one LPC_frame
	if (windowDuration > my dx * my nx) {
		windowDuration = my dx * my nx;
	}
	Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
	autoSound sound = Data_copy (me);
	autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
	autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
	autoLPC thee = LPC_create (my xmin, my xmax, numberOfFrames, dt, t1, predictionOrder, my dx);

	autoMelderProgress progress (U"LPC analysis");

	if (preEmphasisFrequency < samplingFrequency / 2) {
		Sound_preEmphasis (sound.get(), preEmphasisFrequency);
	}

	for (integer i = 1; i <= numberOfFrames; i ++) {
		LPC_Frame lpcframe = (LPC_Frame) & thy d_frames [i];
		double t = Sampled_indexToX (thee.get(), i);
		LPC_Frame_init (lpcframe, predictionOrder);
		Sound_into_Sound (sound.get(), sframe.get(), t - windowDuration / 2);
		Vector_subtractMean (sframe.get());
		Sounds_multiply (sframe.get(), window.get());
		if (method == LPC_METHOD_AUTO) {
			if (! Sound_into_LPC_Frame_auto (sframe.get(), lpcframe)) {
				frameErrorCount ++;
			}
		} else if (method == LPC_METHOD_COVAR) {
			if (! Sound_into_LPC_Frame_covar (sframe.get(), lpcframe)) {
				frameErrorCount ++;
			}
		} else if (method == LPC_METHOD_BURG) {
			if (! Sound_into_LPC_Frame_burg (sframe.get(), lpcframe)) {
				frameErrorCount ++;
			}
		} else if (method == LPC_METHOD_MARPLE) {
			if (! Sound_into_LPC_Frame_marple (sframe.get(), lpcframe, tol1, tol2)) {
				frameErrorCount ++;
			}
		}
		if ((i % 10) == 1) {
			Melder_progress ( (double) i / numberOfFrames, U"LPC analysis of frame ", i, U" out of ", numberOfFrames, U".");
		}
	}
	return thee;
}

autoLPC Sound_to_LPC_auto (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoLPC thee = _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, LPC_METHOD_AUTO, 0, 0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (auto) created.");
	}
}

autoLPC Sound_to_LPC_covar (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoLPC thee = _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, LPC_METHOD_COVAR, 0, 0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (covar) created.");
	}
}

autoLPC Sound_to_LPC_burg (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoLPC thee = _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, LPC_METHOD_BURG, 0, 0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (burg) created.");
	}
}

autoLPC Sound_to_LPC_marple (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency, double tol1, double tol2) {
	try {
		autoLPC thee = _Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, LPC_METHOD_MARPLE, tol1, tol2);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (marple) created.");
	}
}

autoSound LPC_Sound_filterInverse (LPC me, Sound thee) {
	try {
		Melder_require (my samplingPeriod == thy dx, U"Sampling frequencies should be equal.");
		Melder_require (my xmin == thy xmin && thy xmax == my xmax, U"Domains of LPC and Sound should be equal.");
		
		autoSound him = Data_copy (thee);

		double *e = his z [1], *x = thy z [1];
		for (integer i = 1; i <= his nx; i ++) {
			double t = his x1 + (i - 1) * his dx;   // Sampled_indexToX (him, i)
			integer iFrame = Melder_iround ((t - my x1) / my dx + 1.0);   // Sampled_xToNearestIndex (me, t)
			double *a;
			if (iFrame < 1 || iFrame > my nx) {
				e [i] = 0.0;
				continue;
			}
			a = my d_frames [iFrame]. a;
			integer m = i > my d_frames[iFrame].nCoefficients ? my d_frames [iFrame].nCoefficients : i - 1;
			for (integer j = 1; j <= m; j ++) {
				e [i] += a [j] * x [i - j];
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/*
	gain used as a constant amplitude multiplyer within a frame of duration my dx.
	future alternative: convolve gain with a  smoother.
*/
autoSound LPC_Sound_filter (LPC me, Sound thee, bool useGain) {
	try {
		double xmin = my xmin > thy xmin ? my xmin : thy xmin;
		double xmax = my xmax < thy xmax ? my xmax : thy xmax;
		Melder_require (xmin < xmax, U"Domains of Sound [", thy xmin, U",", thy xmax, U"] and LPC [",
			my xmin, U",", my xmax, U"] should overlap.");

		// resample sound if samplings don't match
		autoSound source;
		if (my samplingPeriod != thy dx) {
			source = Sound_resample (thee, 1.0 / my samplingPeriod, 50);
			thee = source.get();   // reference copy; remove at end
		}

		autoSound him = Data_copy (thee);

		double *x = his z [1];
		integer ifirst = Sampled_xToHighIndex (thee, xmin);
		integer ilast = Sampled_xToLowIndex (thee, xmax);
		for (integer i = ifirst; i <= ilast; i ++) {
			double t = his x1 + (i - 1) * his dx;   // Sampled_indexToX (him, i)
			integer iFrame = Melder_iround ((t - my x1) / my dx + 1.0);   // Sampled_xToNearestIndex (me, t)
			if (iFrame < 1) {
				continue;
			}
			if (iFrame > my nx) {
				break;
			}
			double *a = my d_frames [iFrame].a;
			integer m = i > my d_frames [iFrame].nCoefficients ? my d_frames [iFrame].nCoefficients : i - 1;
			for (integer j = 1; j <= m; j ++) {
				x [i] -= a [j] * x [i - j];
			}
		}

		// Make samples before first frame and after last frame zero.

		for (integer i = 1; i < ifirst; i ++) {
			x [i] = 0.0;
		}

		for (integer i = ilast + 1; i <= his nx; i ++) {
			x [i] = 0.0;
		}
		if (useGain) {
			for (integer i = ifirst; i <= ilast; i ++) {
				double t = his x1 + (i - 1) * his dx; /* Sampled_indexToX (him, i) */
				double riFrame = (t - my x1) / my dx + 1; /* Sampled_xToIndex (me, t); */
				integer iFrame = Melder_ifloor (riFrame);
				double phase = riFrame - iFrame;
				if (iFrame < 0 || iFrame > my nx) {
					x [i] = 0.0;
				} else if (iFrame == 0) {
					x [i] *= sqrt (my d_frames [1].gain) * phase;
				} else if (iFrame == my nx) {
					x [i] *= sqrt (my d_frames [my nx].gain) * (1.0 - phase);
				} else x [i] *=
					    phase * sqrt (my d_frames [iFrame + 1].gain) + (1.0 - phase) * sqrt (my d_frames [iFrame].gain);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time) {
	integer frameIndex = Sampled_xToNearestIndex (me, time);
	if (frameIndex < 1) {
		frameIndex = 1;
	}
	if (frameIndex > my nx) {
		frameIndex = my nx;
	}
	if (channel > thy ny) {
		channel = 1;
	}
	Melder_require (frameIndex > 0 && frameIndex <= my nx, U"Frame should be in the range [1, ", my nx, U"].");

	if (channel > 0) {
		LPC_Frame_Sound_filter (& (my d_frames [frameIndex]), thee, channel);
	} else {
		for (integer ichan = 1; ichan <= thy ny; ichan ++) {
			LPC_Frame_Sound_filter (& (my d_frames [frameIndex]), thee, ichan);
		}
	}
}

autoSound LPC_Sound_filterWithFilterAtTime (LPC me, Sound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterInverseWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time) {
	try {
		integer frameIndex = Sampled_xToNearestIndex (me, time);
		if (frameIndex < 1) {
			frameIndex = 1;
		}
		if (frameIndex > my nx) {
			frameIndex = my nx;
		}
		if (channel > thy ny) {
			channel = 1;
		}
		if (channel > 0) {
			LPC_Frame_Sound_filterInverse (& (my d_frames [frameIndex]), thee, channel);
		} else {
			for (integer ichan = 1; ichan <= thy ny; ichan ++) {
				LPC_Frame_Sound_filterInverse (& (my d_frames [frameIndex]), thee, ichan);
			}
		}
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

autoSound LPC_Sound_filterInverseWithFilterAtTime (LPC me, Sound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterInverseWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/* End of file Sound_and_LPC.cpp */
