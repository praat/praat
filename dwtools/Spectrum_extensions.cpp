/* Spectrum_extensions.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20010718
 djmw 20020813 GPL header
 djmw 20030929 Added a warning in Spectrum_drawPhases.
 djmw 20031023 New: Spectra_multiply, Spectrum_conjugate
 djmw 20040506 Changed warning message in Spectrum_drawPhases.
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20061218 Introduction of Melder_information<12...9>
 djmw 20071022 phase_unwrap initialize phase = 0.
 djmw 20080122 float -> double
 djmw 20080202 Warning in Spectrum_drawPhases to wchar
 djmw 20080411 Removed define NUM2pi
*/

#include "Ltas.h"
#include "Spectrum_extensions.h"
#include "Sound_and_Spectrum.h"
#include "NUM2.h"

#define SIGN(x,s) ((s) < 0 ? -fabs (x) : fabs(x))

#define THLCON 0.5
#define THLINC 1.5
#define EXP2   12

#define PPVPHA(x,y,test) ((test) ? atan2 (-(y),-(x)) : atan2 ((y),(x)))
#define PHADVT(xr,xi,yr,yi,xa) ((xa) > 0 ? ((xr)*(yr)+(xi)*(yi))/ (xa) : 0)

struct tribolet_struct {
	double thlinc, thlcon;
	double ddf, dvtmn2;
	double *x;
	integer nx, l, count;
	bool reverse_sign;
};

/*
	Perform modified Goertzel algorithm to calculate, at frequency 'freq_rad',
	the real and imaginary part of the spectrum and the d/df of the
	spectrum of x.
	Reference: Bonzanigo (1978), IEEE Trans. ASSP, Vol. 26.
*/
static void getSpectralValues (struct tribolet_struct *tbs, double freq_rad, double *xr, double *xi, double *nxr, double *nxi) {
	const double cosf = cos (freq_rad), sinf = sin (freq_rad);
	double a = 2 * cosf;
	double b, u1 = 0, u2 = u1, w1 = u1, w2 = u1;
	const double *x = tbs -> x;
	const integer nx = tbs -> nx;

	for (integer j = 1; j <= nx; j ++) {
		double xj = x [j];
		double u0 =           xj + a * u1 - u2;
		double w0 = (j - 1) * xj + a * w1 - w2;
		u2 = u1;
		u1 = u0;
		w2 = w1;
		w1 = w0;
	}

	// Bonzanigo's phase correction

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

// Find the closest unwrapped phase estimate from the two admissible phase values (a1 & a2).

static int phase_check (double pv, double *inout_phase, double thlcon) {
	const double a0 = (*inout_phase - pv) / NUM2pi;
	const integer k = Melder_ifloor (a0);   // ppgb: instead of truncation toward zero
	const double a1 = pv + k * NUM2pi;
	const double a2 = a1 + SIGN (NUM2pi, a0);
	const double a3 = fabs (a1 - *inout_phase);
	const double a4 = fabs (a2 - *inout_phase);

	if (a3 > thlcon && a4 > thlcon)
		return 0;
	*inout_phase = a3 > a4 ? a2 : a1;
	return 1;
}

/*
	Phase unwrapping based on Tribolet's adaptive integration method.
	the unwrapped phase estimate is returned.
*/
static double phase_unwrap (struct tribolet_struct *tbs, double pfreq, double ppv, double pdvt, double *pphase, double *ppdvt) {
	double sdvt [25], sppv [25];
	double freq, phase = 0.0;
	double xr, xi, xmsq, nxr, nxi;
	integer k, sindex [25], pindex = 1, sp = 1;

	sppv [sp] = ppv;
	sdvt [sp] = pdvt;
	sindex [sp] = tbs -> l + 1;

	goto p40;
p20:
	/*
		When the routine runs out of stack space, there probably is
		a zero very near the unit circle that results in a jump of
		pi in the phase.
	*/
	if ((sindex [sp] - pindex) <= 1)
		return phase;
	/*
		p30:
		Get the intermediate frequency value and compute its phase
		derivative and principal value.
	*/
	k = (sindex [sp] + pindex) / 2;
	freq = pfreq + (k - 1) * tbs -> ddf;
	getSpectralValues (tbs, freq, & xr, & xi, & nxr, & nxi);
	sindex [ ++sp] = k;
	sppv [sp] = PPVPHA (xr, xi, tbs -> reverse_sign);
	xmsq = xr * xr + xi * xi;
	sdvt [sp] = PHADVT (xr, xi, nxr, nxi, xmsq);

p40:
	/*
		Evaluate the phase increment.
		If the phase increment, reduced by the expected linear phase
		increment, is greater than the specified threshold, adapt step size.
	*/
	double delta = 0.5 * tbs -> ddf * (sindex [sp] - pindex);
	double phase_inc = delta * (*ppdvt + sdvt [sp]);

	if (fabs (phase_inc - delta * tbs -> dvtmn2) > tbs -> thlinc)
		goto p20;

	phase = *pphase + phase_inc;

	if (! phase_check (sppv [sp], &phase, tbs -> thlcon))
		goto p20;
	if (fabs (phase - *pphase) > NUMpi)
		goto p20;
	if (sp == 1)
		return phase;
	/*
		p10: Update previous estimate.
	*/
	pindex = sindex [sp];
	*pphase = phase;
	*ppdvt = sdvt [sp--];

	goto p40;
}

autoMatrix Spectrum_unwrap (Spectrum me) {
	try {
		struct tribolet_struct tbs;
		int remove_linear_part = 1;

		integer nfft = 2;
		while (nfft < my nx - 1) {
			nfft *= 2;
		}
		nfft *= 2;

		Melder_require (nfft / 2 == my nx - 1,
			U"Dimension of Spectrum should be a power of 2 - 1.");

		autoSound x = Spectrum_to_Sound (me);
		autoSound nx = Data_copy (x.get());

		for (integer i = 1; i <= x -> nx; i ++)
			nx -> z [1] [i] *= (i - 1);

		autoSpectrum snx = Sound_to_Spectrum (nx.get(), true);
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, 2, 2, 1, 1);

		tbs.thlinc = THLINC;
		tbs.thlcon = THLCON;
		tbs.x = & x -> z [1] [0];
		tbs.nx = x -> nx;
		tbs.l = Melder_ifloor (pow (2, EXP2) + 0.1);
		tbs.ddf = NUM2pi / ( (tbs.l) * nfft);
		tbs.reverse_sign = my z [1] [1] < 0;
		tbs.count = 0;
		/*
			Reuse snx : put phase derivative (d/df) in imaginary part.
		*/
		tbs.dvtmn2 = 0.0;
		for (integer i = 1; i <= my nx; i ++) {
			const double xr = my z [1] [i], xi = my z [2] [i];
			const double nxr = snx -> z [1] [i], nxi = snx -> z [2] [i];
			const double xmsq = xr * xr + xi * xi;
			const double pdvt = PHADVT (xr, xi, nxr, nxi, xmsq);
			thy z [1] [i] = xmsq;
			snx -> z [2] [i] = pdvt;
			tbs.dvtmn2 += pdvt;
		}

		tbs.dvtmn2 = (2.0 * tbs.dvtmn2 - snx -> z [2] [1] - snx -> z [2] [my nx]) / (my nx - 1);

		autoMelderProgress progress (U"Phase unwrapping");

		double pphase = 0.0, phase = 0.0;
		double ppdvt = snx -> z [2] [1];
		thy z [2] [1] = PPVPHA (my z [1] [1], my z [2] [1], tbs.reverse_sign);
		for (integer i = 2; i <= my nx; i ++) {
			const double pfreq = NUM2pi * (i - 1) / nfft;
			const double pdvt = snx -> z [2] [i];
			const double ppv = PPVPHA (my z [1] [i], my z [2] [i], tbs.reverse_sign);
			phase = phase_unwrap (&tbs, pfreq, ppv, pdvt, &pphase, &ppdvt);
			ppdvt = pdvt;
			thy z [2] [i] = pphase = phase;
			Melder_progress ( (double) i / my nx, i,
			                   U" unwrapped phases from ", my nx, U".");
		}

		const integer iphase = Melder_ifloor (phase / NUMpi + 0.1);

		if (remove_linear_part) {
			phase /= my nx - 1;
			for (integer i = 2; i <= my nx; i ++)
				thy z [2] [i] -= phase * (i - 1);
		}
		Melder_information (U"Number of spectral values: ", tbs.count);
		Melder_information (U" iphase = ", iphase);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not unwrapped.");
	}
}

void Spectrum_drawPhases (Spectrum me, Graphics g, double fmin, double fmax, double phase_min, double phase_max, int unwrap, bool /* garnish */) {
	autoMatrix thee;
	bool reverse_sign = my z [1] [1] < 0;

	if (unwrap)
		thee = Spectrum_unwrap (me);
	else {
		thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 2.0, 2, 1.0, 1.0);
		for (integer i = 1; i <= my nx; i ++) {
			thy z [2] [i] = PPVPHA (my z [1] [i], my z [2] [i], reverse_sign);
		}
	}
	Matrix_drawRows (thee.get(), g, fmin, fmax, 1.9, 2.1, phase_min, phase_max);
}

autoSpectrum Spectra_multiply (Spectrum me, Spectrum thee) {
	try {
		Melder_require (my nx == thy nx && my x1 == thy x1 && my xmax == thy xmax && my dx == thy dx,
			U"Dimensions of both spectra should be the same.");

		autoSpectrum him = Data_copy (me);

		for (integer i = 1; i <= his nx; i ++) {
			his z [1] [i] = my z [1] [i] * thy z [1] [i] - my z [2] [i] * thy z [2] [i];
			his z [2] [i] = my z [1] [i] * thy z [2] [i] + my z [2] [i] * thy z [1] [i];
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not multiplied.");
	}
}

void Spectrum_conjugate (Spectrum me) {
	for (integer i = 1; i <= my nx; i ++)
		my z [2] [i] = - my z [2] [i];
}

autoSpectrum Spectrum_resample (Spectrum me, integer numberOfFrequencies) {
	try {
		const double newSamplingFrequency = (1 / my dx) * numberOfFrequencies / my nx;
		// resample real and imaginary part !
		autoSound thee = Sound_resample ((Sound) me, newSamplingFrequency, 50);
		autoSpectrum him = Spectrum_create (my xmax, numberOfFrequencies);
		his z.all() <<= thy z.all();
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not resampled.");
	}
}

#if 0
static autoSpectrum Spectrum_shiftFrequencies2 (Spectrum me, double shiftBy, bool changeMaximumFrequency) {
	try {
		double xmax = my xmax;
		integer numberOfFrequencies = my nx, interpolationDepth = 50;
		if (changeMaximumFrequency) {
			xmax += shiftBy;
			numberOfFrequencies += (xmax - my xmax) / my dx;
		}
		autoSpectrum thee = Spectrum_create (xmax, numberOfFrequencies);
		// shiftBy >= 0
		for (integer i = 1; i <= thy nx; i ++) {
			double thyf = thy x1 + (i - 1) * thy dx;
			double myf = thyf - shiftBy;
			if (myf >= my xmin && myf <= my xmax) {
				double index = Sampled_xToIndex (me, myf);
				thy z [1] [i] = NUM_interpolate_sinc (my z.row (1), index, interpolationDepth);
				thy z [2] [i] = NUM_interpolate_sinc (my z.row (2), index, interpolationDepth);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not shifted.");
	}
}
#endif

autoSpectrum Spectrum_shiftFrequencies (Spectrum me, double shiftBy, double newMaximumFrequency, integer interpolationDepth) {
	try {
		double xmax = my xmax;
		integer numberOfFrequencies = my nx;
		if (newMaximumFrequency != 0.0) {
			numberOfFrequencies = Melder_ifloor (newMaximumFrequency / my dx) + 1;
			xmax = newMaximumFrequency;
		}
		autoSpectrum thee = Spectrum_create (xmax, numberOfFrequencies);
		for (integer i = 1; i <= thy nx; i ++) {
			const double thyf = thy x1 + (i - 1) * thy dx;
			const double myf = thyf - shiftBy;
			if (myf >= my xmin && myf <= my xmax) {
				const double index = Sampled_xToIndex (me, myf);
				thy z [1] [i] = NUM_interpolate_sinc (my z.row (1), index, interpolationDepth);
				thy z [2] [i] = NUM_interpolate_sinc (my z.row (2), index, interpolationDepth);
			}
		}
		/*
			Make imaginary part of first and last sample zero
			so Spectrum_to_Sound uses FFT if numberOfSamples was power of 2!
		*/
		double amp = sqrt (thy z [1] [1] * thy z [1] [1] + thy z [2] [1] * thy z [2] [1]);
		thy z [1] [1] = amp;
		thy z [2] [1] = 0.0;
		amp = sqrt (thy z [1] [thy nx] * thy z [1] [thy nx] + thy z [2] [thy nx] * thy z [2] [thy nx]);
		thy z [1] [thy nx] = amp;
		thy z [2] [thy nx] = 0.0;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not shifted.");
	}
}

autoSpectrum Spectrum_compressFrequencyDomain (Spectrum me, double fmax, integer interpolationDepth, int freqscale, int method) {
	try {
		const double fdomain = my xmax - my xmin, factor = fdomain / fmax ;
		//integer numberOfFrequencies = 1.0 + fmax / my dx; // keep dx the same, otherwise the "duration" changes
		const double xmax = my xmax / factor;
		const integer numberOfFrequencies = Melder_ifloor (my nx / factor); // keep dx the same, otherwise the "duration" changes
		autoSpectrum thee = Spectrum_create (xmax, numberOfFrequencies);
		thy z [1] [1] = my z [1] [1];
		thy z [2] [1] = my z [2] [1];
		const double df = freqscale == 1 ? factor * my dx : log10 (fdomain) / (numberOfFrequencies - 1);
		for (integer i = 2; i <= numberOfFrequencies; i ++) {
			const double f = my xmin + (freqscale == 1 ? (i - 1) * df : pow (10.0, (i - 1) * df));
			const double index = (f - my x1) / my dx + 1;
			double x, y;
			if (index > my nx)
				break;
			if (method == 1) {
				x = NUM_interpolate_sinc (my z.row (1), index, interpolationDepth);
				y = NUM_interpolate_sinc (my z.row (2), index, interpolationDepth);
			} else {
				x = undefined;   // ppgb: better than data from random memory
				y = undefined;
			}
			thy z [1] [i] = x;
			thy z [2] [i] = y;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not compressed.");
	}
}

/* End of file Spectrum_extensions.cpp */
