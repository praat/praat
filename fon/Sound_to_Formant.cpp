/* Sound_to_Formant.cpp
 *
 * Copyright (C) 1992-2008,2010-2012,2014-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/05/15 replaced memcof with NUMburg
 * pb 2003/09/18 default time step is 4 times oversampling
 * pb 2006/05/10 better handling of interruption in Sound_to_Formant
 * pb 2006/05/10 better handling of NULL from Polynomial_to_Roots
 * pb 2007/01/26 made compatible with stereo Sounds
 * pb 2007/03/30 changed float to double (against compiler warnings)
 * pb 2010/12/13 removed some style bugs
 * pb 2011/06/08 C++
 */

#include "Sound_to_Formant.h"
#include "NUM2.h"
#include "Polynomial.h"
#include "Roots.h"

static void burg (constVEC samples, VEC coefficients,
	Formant_Frame frame, double nyquistFrequency, double safetyMargin)
{
	double a0 = VECburg (coefficients, samples);
	(void) a0;
	/*
		Convert LP coefficients to polynomial.
	 */
	autoPolynomial polynomial = Polynomial_create (-1, 1, coefficients.size);
	for (integer i = 1; i <= coefficients.size; i ++)
		polynomial -> coefficients [i] = - coefficients [coefficients.size - i + 1];
	polynomial -> coefficients [coefficients.size + 1] = 1.0;

	/*
		Find the roots of the polynomial.
	 */
	autoRoots roots = Polynomial_to_Roots (polynomial.get());
	Roots_fixIntoUnitCircle (roots.get());

	Melder_assert (frame -> numberOfFormants == 0 && NUMisEmpty (frame -> formant.get()));

	/*
		First pass: count the formants.
		The roots come in conjugate pairs, so we need only count those above the real axis.
	 */
	for (integer iroot = 1; iroot <= roots -> numberOfRoots; iroot ++)
		if (roots -> roots [iroot].imag() >= 0.0) {
			double f = fabs (atan2 (roots -> roots [iroot].imag(), roots -> roots [iroot].real())) * nyquistFrequency / NUMpi;
			if (f >= safetyMargin && f <= nyquistFrequency - safetyMargin)
				frame -> numberOfFormants ++;
		}

	/*
		Create space for formant data.
	 */
	if (frame -> numberOfFormants > 0)
		frame -> formant = newvectorzero <structFormant_Formant> (frame -> numberOfFormants);

	/*
		Second pass: fill in the formants.
	 */
	int iformant = 0;
	for (integer iroot = 1; iroot <= roots -> numberOfRoots; iroot ++)
		if (roots -> roots [iroot].imag() >= 0.0) {
			double f = fabs (atan2 (roots -> roots [iroot].imag(), roots -> roots [iroot].real())) * nyquistFrequency / NUMpi;
			if (f >= safetyMargin && f <= nyquistFrequency - safetyMargin) {
				Formant_Formant formant = & frame -> formant [++ iformant];
				formant -> frequency = f;
				formant -> bandwidth = -
					log (norm (roots -> roots [iroot])) * nyquistFrequency / NUMpi;
			}
		}
	Melder_assert (iformant == frame -> numberOfFormants);   // may fail if some frequency is NaN
}

static int findOneZero (integer ijt, double vcx [], double a, double b, double *zero) {
	double x = 0.5 * (a + b), fa = 0.0, fb = 0.0, fx = 0.0;
	for (integer k = ijt; k >= 0; k --) {
		fa = vcx [k] + a * fa;
		fb = vcx [k] + b * fb;
	}
	if (fa * fb >= 0.0) {   // there should be a zero between a and b
		Melder_casual (
			U"There is no zero between ,", Melder_single (a),
			U" and ", Melder_single (b),
			U".\n    The function values are ", Melder_single (fa),
			U" and ", Melder_single (fb),
			U", respectively."
		);
		return 0;
	}
	do {
		fx = 0.0;
		/*x = fa == fb ? 0.5 * (a + b) : a + fa * (a - b) / (fb - fa);*/
		x = 0.5 * (a + b);   // simple bisection
		for (integer k = ijt; k >= 0; k --)
			fx = vcx [k] + x * fx;
		if (fa * fx > 0.0) {
			a = x;
			fa = fx;
		} else {
			b = x;
			fb = fx;
		}
	} while (fabs (fx) > 1e-5);
	*zero = x;
	return 1;   // OK
}

static int findNewZeroes (integer ijt, double ppORIG [], integer degree,
	double zeroes [])   // In / out
{
	static double cosa [7] [7] = {
		{  1,   0,   0,   0,   0,   0,   0 },
		{  0,   2,   0,   0,   0,   0,   0 },
		{ -2,   0,   4,   0,   0,   0,   0 },
		{  0,  -6,   0,   8,   0,   0,   0 },
		{  2,   0, -16,   0,  16,   0,   0 },
		{  0,  10,   0, -40,   0,  32,   0 },
		{ -2,   0,  36,   0, -96,   0,  64 } };
	double pp [33], newZeroes [33], px [33];
	integer const half_degree = (degree + 1) / 2;
	for (integer vt = 0; vt <= half_degree; vt ++)
		pp [vt] = ppORIG [vt];
	if (! (degree & 1))
		for (integer vt = 1; vt <= half_degree; vt ++)
			pp [vt] -= pp [vt - 1];
	for (integer i = 0; i <= half_degree; i ++)
		px [i] = cosa [half_degree] [i];
	for (integer pt = half_degree - 1; pt >= 0; pt --)
		for (integer vt = 0; vt <= half_degree; vt ++)
			px [vt] += pp [half_degree - pt] * cosa [pt] [vt];
	/* Fill an array with the new zeroes, which lie between the old zeroes. */
	newZeroes [0] = 1.0;
	for (integer i = 1; i <= half_degree; i ++) {
		if (! findOneZero (ijt, px, zeroes [i - 1], zeroes [i], & newZeroes [i])) {
			Melder_casual (
				U"Degree ", degree,
				U" not completed."
			);
			return 0;
		}
	}
	newZeroes [half_degree + 1] = -1.0;
	/*
		Grow older.
	*/
	for (integer i = 0; i <= half_degree + 1; i ++)
		zeroes [i] = newZeroes [i];
	return 1;
}

static int splitLevinson (
	constVEC samples,   // the windowed signal xw [1..nx]
	integer ncof,   // the coefficients cof [1..ncof]
	Formant_Frame frame, double nyquistFrequency)   // put the results here
{
	int result = 1;
	double rx [100], zeroes [33];
	for (integer i = 0; i <= 32; i ++)
		zeroes [i] = 0.0;
	/*
		Compute the autocorrelation of the windowed signal.
	*/
	for (integer i = 0; i < ncof; i ++) {
		rx [i] = 0.0;
		for (integer j = 1; j <= samples.size - i; j ++)
			rx [i] += samples [j] * samples [j + i];
	}
	/*
		Normalize autocorrelation;
		(should we also divide by the autocorrelation of the window?).
	*/
	for (integer i = 1; i < ncof; i ++)
		rx [i] /= rx [0]; rx [0] = 1.0;

	/*
		Compute zeroes.
	*/
	{
		double tau = 0.5 * rx [0];
		double pnu [33], pk [33], pkz [33];
		for (integer i = 0; i <= 32; i ++)
			pkz [i] = pk [i] = 0.0;
		pkz [0] = 1.0;
		pk [0] = 1.0;
		pk [1] = 1.0;
		for (integer degree = 1; degree < ncof; degree ++) {
			integer t = degree / 2;
			double tauk = rx [0] + rx [degree], alfak;
			for (integer it = 1; it <= t; it ++)
				tauk += pk [it] * ( 2 * it == degree ? rx [it] : rx [it] + rx [degree - it] );
			alfak = tauk / tau;
			tau = tauk;
			pnu [0] = 1.0;
			integer ijt = (degree + 1) / 2;
			for (integer it = ijt; it > 0; it --)
				pnu [it] = pk [it] + pk [it - 1] - alfak * pkz [it - 1];
			if (2 * ijt == degree)
				pnu [ijt + 1] = pnu [ijt];
			if (degree == 1) {
				(void) 0;
			} else if (degree == 2) {
				zeroes [0] = 1.0;   // starting values
				zeroes [1] = 0.5 - 0.5 * pnu [1];
				zeroes [2] = -1.0;
			}
			else
				if (! findNewZeroes (ijt, pnu, degree, zeroes)) {
					result = 0;
					goto loopEnd;
				}
			/*
				Grow older.
			*/
			for (integer i = 0; i <= 32; i ++) {
				pkz [i] = pk [i];
				pk [i] = pnu [i];
			}
		}
	}
loopEnd:
	/*
		First pass: count the poles.
	*/
	for (integer i = 1; i <= ncof / 2; i ++) {
		if (zeroes [i] == 0.0 || zeroes [i] == -1.0)
			break;
		frame -> numberOfFormants ++;
	}
	/*
		Create space for formant data.
	*/
	if (frame -> numberOfFormants > 0)
	    frame -> formant = newvectorzero <structFormant_Formant> (frame -> numberOfFormants);
	/*
		Second pass: fill in the poles.
	*/
	integer iformant = 0;
	for (integer i = 1; i <= ncof / 2; i ++) {
		Formant_Formant formant = & frame -> formant [++ iformant];
		if (zeroes [i] == 0.0 || zeroes [i] == -1.0)
			break;
		formant -> frequency = acos (zeroes [i]) * nyquistFrequency / NUMpi;
		formant -> bandwidth = 50.0;
	}

	return result;
}

static void Sound_preEmphasis (Sound me, double preEmphasisFrequency) {
	const double preEmphasis = exp (-2.0 * NUMpi * preEmphasisFrequency * my dx);
	for (integer channel = 1; channel <= my ny; channel ++)
		for (integer i = my nx; i >= 2; i --)
			my z [channel] [i] -= preEmphasis * my z [channel] [i - 1];
}

void Formant_sort (Formant me) {
	for (integer iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frames [iframe];
		integer n = frame -> numberOfFormants;
		for (integer i = 1; i < n; i ++) {
			double min = frame -> formant [i]. frequency;
			integer imin = i;
			for (integer j = i + 1; j <= n; j ++)
				if (frame -> formant [j]. frequency < min) {
					min = frame -> formant [j]. frequency;
					imin = j;
				}
			if (imin != i) {
				const double min_bandwidth = frame -> formant [imin]. bandwidth;
				frame -> formant [imin]. frequency = frame -> formant [i]. frequency;
				frame -> formant [imin]. bandwidth = frame -> formant [i]. bandwidth;
				frame -> formant [i]. frequency = min;
				frame -> formant [i]. bandwidth = min_bandwidth;
			}
		}
	}
}

static autoFormant Sound_to_Formant_any_inplace (Sound me, double dt_in, integer numberOfPoles,
	double halfdt_window, int which, double preemphasisFrequency, double safetyMargin)
{
	const double dt = ( dt_in > 0.0 ? dt_in : halfdt_window / 4.0 );
	const double physicalDuration = my nx * my dx;
	double dt_window = 2.0 * halfdt_window;
	integer nFrames = 1 + Melder_ifloor ((physicalDuration - dt_window) / dt);
	integer nsamp_window = Melder_ifloor (dt_window / my dx), halfnsamp_window = nsamp_window / 2;

	if (nsamp_window < numberOfPoles + 1)
		Melder_throw (U"Window too short.");
	double t1 = my x1 + 0.5 * (physicalDuration - my dx - (nFrames - 1) * dt);   // centre of first frame
	if (nFrames < 1) {
		nFrames = 1;
		t1 = my x1 + 0.5 * physicalDuration;
		dt_window = physicalDuration;
		nsamp_window = my nx;
	}
	autoFormant thee = Formant_create (my xmin, my xmax, nFrames, dt, t1, (numberOfPoles + 1) / 2);   // e.g. 11 poles -> maximally 6 formants

	autoMelderProgress progress (U"Formant analysis...");

	/* Pre-emphasis. */
	Sound_preEmphasis (me, preemphasisFrequency);

	/* Gaussian window. */
	autoVEC window = raw_VEC (nsamp_window);
	for (integer i = 1; i <= nsamp_window; i ++) {
		const double imid = 0.5 * (nsamp_window + 1), edge = exp (-12.0);
		window [i] = (exp (-48.0 * (i - imid) * (i - imid) / (nsamp_window + 1) / (nsamp_window + 1)) - edge) / (1.0 - edge);
	}

	integer maximumFrameLength = nsamp_window;
	auto frameBuffer = raw_VEC (maximumFrameLength);
	auto coefficients = raw_VEC (numberOfPoles);   // superfluous if which==2, but nobody uses that anyway
	for (integer iframe = 1; iframe <= nFrames; iframe ++) {
		const double t = Sampled_indexToX (thee.get(), iframe);
		const integer leftSample = Sampled_xToLowIndex (me, t);
		const integer rightSample = leftSample + 1;
		integer startSample = rightSample - halfnsamp_window;
		integer endSample = leftSample + halfnsamp_window;
		double maximumIntensity = 0.0;
		Melder_clipLeft (1_integer, & startSample);   // this should not be more than a rounding problem
		Melder_clipRight (& endSample, my nx);   // this should not be more than a rounding problem
		for (integer i = startSample; i <= endSample; i ++) {
			const double value = Sampled_getValueAtSample (me, i, Sound_LEVEL_MONO, 0);
			if (value * value > maximumIntensity)
				maximumIntensity = value * value;
		}
		thy frames [iframe]. intensity = maximumIntensity;
		if (maximumIntensity == 0.0)
			continue;   // Burg cannot stand all zeroes

		/* Copy a pre-emphasized window to a frame. */
		const integer actualFrameLength = endSample - startSample + 1;   // should rarely be less than nsamp_window
		VEC frame = frameBuffer.part (1, actualFrameLength);
		const integer offset = startSample - 1;
		for (integer isamp = 1; isamp <= actualFrameLength; isamp ++)
			frame [isamp] = Sampled_getValueAtSample (me, offset + isamp, Sound_LEVEL_MONO, 0) * window [isamp];

		if (which == 1) {
			burg (frame, coefficients.get(), & thy frames [iframe], 0.5 / my dx, safetyMargin);
		} else if (which == 2) {
			if (! splitLevinson (frame, numberOfPoles, & thy frames [iframe], 0.5 / my dx)) {
				Melder_clearError ();
				Melder_casual (U"(Sound_to_Formant:)"
					U" Analysis results of frame ", iframe,
					U" will be wrong."
				);
			}
		}
		Melder_progress ((double) iframe / (double) nFrames, U"Formant analysis: frame ", iframe);
	}
	Formant_sort (thee.get());
	return thee;
}

autoFormant Sound_to_Formant_any (Sound me, double dt, integer numberOfPoles, double maximumFrequency,
	double halfdt_window, int which, double preemphasisFrequency, double safetyMargin)
{
	const double nyquist = 0.5 / my dx;
	autoSound sound;
	if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1) < 1.0e-12) {
		sound = Data_copy (me);   // will be modified
	} else {
		sound = Sound_resample (me, maximumFrequency * 2, 50);
	}
	return Sound_to_Formant_any_inplace (sound.get(), dt, numberOfPoles, halfdt_window, which, preemphasisFrequency, safetyMargin);
}

autoFormant Sound_to_Formant_burg (Sound me, double dt, double nFormants, double maximumFrequency, double halfdt_window, double preemphasisFrequency) {
	try {
		return Sound_to_Formant_any (me, dt, Melder_iround (2.0 * nFormants), maximumFrequency, halfdt_window, 1, preemphasisFrequency, 50.0);
	} catch (MelderError) {
		Melder_throw (me, U": formant analysis (Burg) not performed.");
	}
}

autoFormant Sound_to_Formant_keepAll (Sound me, double dt, double nFormants, double maximumFrequency, double halfdt_window, double preemphasisFrequency) {
	try {
		return Sound_to_Formant_any (me, dt, Melder_iround (2.0 * nFormants), maximumFrequency, halfdt_window, 1, preemphasisFrequency, 0.0);
	} catch (MelderError) {
		Melder_throw (me, U": formant analysis (keep all) not performed.");
	}
}

autoFormant Sound_to_Formant_willems (Sound me, double dt, double nFormants, double maximumFrequency, double halfdt_window, double preemphasisFrequency) {
	try {
		return Sound_to_Formant_any (me, dt, Melder_iround (2.0 * nFormants), maximumFrequency, halfdt_window, 2, preemphasisFrequency, 50.0);
	} catch (MelderError) {
		Melder_throw (me, U": formant analysis (Burg) not performed.");
	}
}

/* End of file Sound_to_Formant.cpp */
