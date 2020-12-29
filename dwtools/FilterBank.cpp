/* FilterBank.cpp
 *
 * Copyright (C) 1993-2017 David Weenink, Paul Boersma 2017
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
 djmw 20030901 Added fiter function drawing and frequency scale drawing.
 djmw 20050731 +FilterBank_and_PCA_drawComponent
 djmw 20071017 Melder_error<n>
 djmw 20071201 Melder_warning<n>
 djmw 20080122 float -> double
 djmw 20110304 Thing_new
*/

#include "Eigen_and_Matrix.h"
#include "FilterBank.h"
#include "Matrix_extensions.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "Sound_to_Pitch.h"
#include "NUM2.h"

static double scaleFrequency (double f, int scale_from, int scale_to) {
	double fhz = undefined;

	if (scale_from == scale_to)
		return f;

	if (scale_from == FilterBank_HERTZ)
		fhz = f;
	else if (scale_from == FilterBank_BARK)
		fhz = BARKTOHZ (f);
	else if (scale_from == FilterBank_MEL)
		fhz = MELTOHZ (f);


	if (scale_to == FilterBank_HERTZ || isundef (fhz))
		return fhz;


	if (scale_to == FilterBank_BARK)
		f = HZTOBARK (fhz);
	else if (scale_to == FilterBank_MEL)
		f = HZTOMEL (fhz);
	else
		return undefined;
	return f;
}

static char32 const *GetFreqScaleText (int scale) {
	char32 const *hertz = U"Frequency (Hz)";
	char32 const *bark = U"Frequency (Bark)";
	char32 const *mel = U"Frequency (mel)";
	char32 const *error = U"Frequency (undefined)";
	if (scale == FilterBank_HERTZ)
		return hertz;
	else if (scale == FilterBank_BARK)
		return bark;
	else if (scale == FilterBank_MEL)
		return mel;
	return error;
}

static int checkLimits (Matrix me, int fromFreqScale, int toFreqScale, int *fromFilter, int *toFilter, double *zmin, double *zmax, int dbScale, double *ymin, double *ymax) {

	if (*fromFilter == 0)
		*fromFilter = 1;

	if (*toFilter == 0)
		*toFilter = my ny;

	if (*toFilter < *fromFilter) {
		*fromFilter = 1;
		*toFilter = my ny;
	}
	if (*fromFilter < 1) {
		*fromFilter = 1;
	}
	if (*toFilter > my ny)
		*toFilter = my ny;
	if (*fromFilter > *toFilter) {
		Melder_warning (U"Filter numbers should be in range [1, ", my ny, U"]");
		return 0;
	}

	if (*zmin < 0.0 || *zmax < 0.0) {
		Melder_warning (U"Frequencies should be positive.");
		return 0;
	}
	if (*zmax <= *zmin) {
		*zmin = scaleFrequency (my ymin, fromFreqScale, toFreqScale);
		*zmax = scaleFrequency (my ymax, fromFreqScale, toFreqScale);
	}

	if (*ymax <= *ymin) {
		*ymax = 1.0;
		*ymin = 0.0;
		if (dbScale) {
			*ymax = 0.0;
			*ymin = -60.0;
		}
	}
	return 1;
}

static double to_dB (double a, double factor, double ref_dB) {
	if (a <= 0)
		return ref_dB;
	a = factor * log10 (a);
	if (a < ref_dB)
		a = ref_dB;
	return a;
}

static void setDrawingLimits (double *a, integer n, double amin, double amax, integer *ibegin, integer *iend) {
	*ibegin = 0;
	*iend = n + 1;

	integer lower = 1;
	for (integer i = 1; i <= n; i ++) {
		if (isundef (a [i])) {
			if (lower == 0) {
				// high frequency part
				*iend = i;
				break;
			}
			*ibegin = i;
			continue;
		}
		lower = 0;
		if (a [i] < amin)
			a [i] = amin;
		else if (a [i] > amax)
			a [i] = amax;
	}

	(*ibegin) ++;
	(*iend)--;
}

Thing_implement (FilterBank, Matrix, 2);

Thing_implement (BarkFilter, FilterBank, 2);

autoBarkFilter BarkFilter_create (double tmin, double tmax, integer nt, double dt, double t1, double fmin, double fmax, integer nf, double df, double f1) {
	try {
		autoBarkFilter me = Thing_new (BarkFilter);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"BarkFilter not created.");
	}
}

double FilterBank_getFrequencyInHertz (FilterBank /* me */, double f, int scale_from) {
	return scaleFrequency (f, scale_from, FilterBank_HERTZ);
}

double FilterBank_getFrequencyInBark (FilterBank /* me */, double f, int scale_from) {
	return scaleFrequency (f, scale_from, FilterBank_BARK);
}

double FilterBank_getFrequencyInMel (FilterBank /* me */, double f, int scale_from) {
	return scaleFrequency (f, scale_from, FilterBank_MEL);
}

int FilterBank_getFrequencyScale (FilterBank me) {
	return my v_getFrequencyScale ();
}

void FilterBank_drawFrequencyScales (FilterBank me, Graphics g, int horizontalScale, double xmin, double xmax,
	int verticalScale, double ymin, double ymax, bool garnish)
{
	const int myFreqScale = FilterBank_getFrequencyScale (me);

	if (xmin < 0 || xmax < 0 || ymin < 0 || ymax < 0) {
		Melder_warning (U"Frequencies should be >= 0.");
		return;
	}

	if (xmin >= xmax) {
		double xmint = my ymin;
		double xmaxt = my ymax;
		if (ymin < ymax) {
			xmint = scaleFrequency (ymin, verticalScale, myFreqScale);
			xmaxt = scaleFrequency (ymax, verticalScale, myFreqScale);
		}
		xmin = scaleFrequency (xmint, myFreqScale, horizontalScale);
		xmax = scaleFrequency (xmaxt, myFreqScale, horizontalScale);
	}

	if (ymin >= ymax) {
		ymin = scaleFrequency (xmin, horizontalScale, verticalScale);
		ymax = scaleFrequency (xmax, horizontalScale, verticalScale);
	}

	const integer n = 2000;
	autoVEC a = zero_VEC (n);

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	const double df = (xmax - xmin) / (n - 1);

	for (integer i = 1; i <= n; i ++) {
		const double f = xmin + (i - 1) * df;
		a [i] = scaleFrequency (f, horizontalScale, verticalScale);
	}

	integer ibegin, iend;
	setDrawingLimits (a.asArgumentToFunctionThatExpectsOneBasedArray(), n, ymin, ymax,	& ibegin, & iend);
	if (ibegin <= iend) {
		const double fmin = xmin + (ibegin - 1) * df;
		const double fmax = xmax - (n - iend) * df;
		Graphics_function (g, a.asArgumentToFunctionThatExpectsOneBasedArray(), ibegin, iend, fmin, fmax);
	}
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, GetFreqScaleText (verticalScale));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, GetFreqScaleText (horizontalScale));
	}
}

void FilterBank_paint (FilterBank me, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double minimum, double maximum, bool garnish)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (ymax <= ymin) {
		ymin = my ymin;
		ymax = my ymax;
	}
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	if (xmin >= xmax || ymin >= ymax)
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_image (g, my z.part (iymin, iymax, ixmin, ixmax),
			Sampled_indexToX   (me, ixmin - 0.5), Sampled_indexToX   (me, ixmax + 0.5),
			SampledXY_indexToY (me, iymin - 0.5), SampledXY_indexToY (me, iymax + 0.5),
			minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, GetFreqScaleText (my v_getFrequencyScale ()));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Time (s)");
	}
}

void BarkFilter_drawSekeyHansonFilterFunctions (BarkFilter me, Graphics g, int toFreqScale, int fromFilter, int toFilter,
	double zmin, double zmax, int dbScale, double ymin, double ymax, bool garnish)
{
	if (! checkLimits (me, FilterBank_BARK, toFreqScale, & fromFilter, & toFilter, & zmin, & zmax, dbScale, & ymin, & ymax))
		return;
	const integer n = 1000;
	autoVEC a = zero_VEC (n);

	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);
	for (integer j = fromFilter; j <= toFilter; j ++) {
		const double df = (zmax - zmin) / (n - 1);
		const double zMid = Matrix_rowToY (me, j);

		for (integer i = 1; i <= n; i ++) {
			const double f = zmin + (i - 1) * df;
			double z = scaleFrequency (f, toFreqScale, FilterBank_BARK);
			if (isundef (z)) {
				a [i] = undefined;
			} else {
				z -= zMid + 0.215;
				a [i] = 7.0 - 7.5 * z - 17.5 * sqrt (0.196 + z * z);
				if (! dbScale)
					a [i] = pow (10.0, a [i]);
			}
		}
		integer ibegin, iend;
		setDrawingLimits (a.asArgumentToFunctionThatExpectsOneBasedArray(), n, ymin, ymax, & ibegin, & iend);
		if (ibegin <= iend) {
			const double fmin = zmin + (ibegin - 1) * df;
			const double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a.asArgumentToFunctionThatExpectsOneBasedArray(), ibegin, iend, fmin, fmax);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		const double distance = ( dbScale ? 10.0 : 1.0 );
		const conststring32 ytext = ( dbScale ? U"Amplitude (dB)" : U"Amplitude" );
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, distance, true, true, false);
		Graphics_textLeft (g, true, ytext);
		Graphics_textBottom (g, true, GetFreqScaleText (toFreqScale));
	}
}

Thing_implement (MelFilter, FilterBank, 2);

autoMelFilter MelFilter_create (double tmin, double tmax, integer nt, double dt, double t1,
	double fmin, double fmax, integer nf, double df, double f1)
{
	try {
		autoMelFilter me = Thing_new (MelFilter);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MelFilter not created.");
	}
}

/*
void FilterBank_drawFilters (I, Graphics g, integer fromf, integer tof,
	double xmin, double xmax, int xlinear, double ymin, double ymax, int ydb,
	double (*tolinf)(double f), double (*tononlin) (double f),
	double (*filteramp)(double f0, double b, double f))
{
	iam (Matrix);


}*/

void FilterBank_drawTimeSlice (FilterBank me, Graphics g, double t,
	double fmin, double fmax, double min, double max, conststring32 xlabel, bool garnish)
{
	Matrix_drawSliceY (me, g, t, fmin, fmax, min, max);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		if (xlabel)
			Graphics_textBottom (g, false, xlabel);
	}
}

void MelFilter_drawFilterFunctions (MelFilter me, Graphics g, int toFreqScale, int fromFilter, int toFilter,
	double zmin, double zmax, int dbScale, double ymin, double ymax, bool garnish)
{
	if (! checkLimits (me, FilterBank_MEL, toFreqScale, & fromFilter, & toFilter, & zmin, & zmax, dbScale, & ymin, & ymax))
		return;
	const integer n = 1000;
	autoVEC a = zero_VEC (n);

	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);

	for (integer j = fromFilter; j <= toFilter; j ++) {
		const double df = (zmax - zmin) / (n - 1);
		const double fc_mel = my y1 + (j - 1) * my dy;
		const double fc_hz = MELTOHZ (fc_mel);
		const double fl_hz = MELTOHZ (fc_mel - my dy);
		const double fh_hz = MELTOHZ (fc_mel + my dy);
		integer ibegin, iend;

		for (integer i = 1; i <= n; i ++) {
			// Filterfunction: triangular on a linear frequency scale AND a linear amplitude scale.
			const double f = zmin + (i - 1) * df;
			const double z = scaleFrequency (f, toFreqScale, FilterBank_HERTZ);
			if (isundef (z)) {
				a [i] = undefined;
			} else {
				a [i] = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, z);
				if (dbScale)
					a [i] = to_dB (a [i], 10.0, ymin);
			}
		}

		setDrawingLimits (a.asArgumentToFunctionThatExpectsOneBasedArray(), n, ymin, ymax,	&ibegin, &iend);

		if (ibegin <= iend) {
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a.asArgumentToFunctionThatExpectsOneBasedArray(), ibegin, iend, fmin, fmax);
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		const double distance = ( dbScale ? 10.0 : 1.0 );
		const conststring32 ytext = ( dbScale ? U"Amplitude (dB)" : U"Amplitude" );
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, distance, true, true, false);
		Graphics_textLeft (g, true, ytext);
		Graphics_textBottom (g, true, GetFreqScaleText (toFreqScale));
	}
}

autoMatrix FilterBank_to_Matrix (FilterBank me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all() <<= my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoBarkFilter Matrix_to_BarkFilter (Matrix me) {
	try {
		autoBarkFilter thee = BarkFilter_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all() <<= my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to BarkFilter.");
	}
}

autoMelFilter Matrix_to_MelFilter (Matrix me) {
	try {
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all() <<= my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to MelFilter.");
	}
}

Thing_implement (FormantFilter, FilterBank, 2);

autoFormantFilter FormantFilter_create (double tmin, double tmax, integer nt, double dt, double t1, double fmin, double fmax, integer nf, double df, double f1) {
	try {
		autoFormantFilter me = Thing_new (FormantFilter);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantFilter not created.");
	}
}

void FormantFilter_drawFilterFunctions (FormantFilter me, Graphics g, double bandwidth, int toFreqScale, int fromFilter, int toFilter,
	double zmin, double zmax, int dbScale, double ymin, double ymax, bool garnish)
{
	if (! checkLimits (me, FilterBank_HERTZ, toFreqScale, & fromFilter, & toFilter, & zmin, & zmax, dbScale, & ymin, & ymax))
		return;
	if (bandwidth <= 0.0)
		Melder_warning (U"Bandwidth should be greater than zero.");

	const integer n = 1000;
	autoVEC a = zero_VEC (n);

	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);

	for (integer j = fromFilter; j <= toFilter; j ++) {
		const double df = (zmax - zmin) / (n - 1);
		const double fc = my y1 + (j - 1) * my dy;

		for (integer i = 1; i <= n; i ++) {
			const double f = zmin + (i - 1) * df;
			const double z = scaleFrequency (f, toFreqScale, FilterBank_HERTZ);
			if (isundef (z)) {
				a [i] = undefined;
			} else {
				a [i] = NUMformantfilter_amplitude (fc, bandwidth, z);
				if (dbScale)
					a [i] = to_dB (a [i], 10.0, ymin);
			}
		}

		integer ibegin, iend;
		setDrawingLimits (a.asArgumentToFunctionThatExpectsOneBasedArray(), n, ymin, ymax, & ibegin, & iend);
		if (ibegin <= iend) {
			const double fmin = zmin + (ibegin - 1) * df;
			const double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a.asArgumentToFunctionThatExpectsOneBasedArray(), ibegin, iend, fmin, fmax);
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		double distance = ( dbScale ? 10.0 : 1.0 );
		conststring32 ytext = ( dbScale ? U"Amplitude (dB)" : U"Amplitude" );
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, distance, true, true, false);
		Graphics_textLeft (g, true, ytext);
		Graphics_textBottom (g, true, GetFreqScaleText (toFreqScale));
	}
}

autoFormantFilter Matrix_to_FormantFilter (Matrix me) {
	try {
		autoFormantFilter thee = FormantFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		                         my ymin, my ymax, my ny, my dy, my y1);
		thy z.all() <<= my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to FormantFilter.");
	}
}

autoSpectrum FormantFilter_to_Spectrum_slice (FormantFilter me, double t) {
	try {
		const double sqrtref = sqrt (FilterBank_DBREF);
		const double factor2 = 2.0 * 10.0 * FilterBank_DBFAC;
		autoSpectrum thee = Spectrum_create (my ymax, my ny);

		thy xmin = my ymin;
		thy xmax = my ymax;
		thy x1 = my y1;
		thy dx = my dy;   /* Frequency step. */

		integer frame = Sampled_xToNearestIndex (me, t);
		if (frame < 1)
			frame = 1;
		if (frame > my nx)
			frame = my nx;

		for (integer i = 1; i <= my ny; i ++) {
			/*
				power = ref * 10 ^ (value / 10)
				sqrt(power) = sqrt(ref) * 10 ^ (value / (2*10))
			*/
			thy z [1] [i] = sqrtref * pow (10.0, my z [i] [frame] / factor2);
			thy z [2] [i] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Spectral slice not created.");
	}
}

autoIntensity FilterBank_to_Intensity (FilterBank me) {
	try {
		autoIntensity thee = Intensity_create (my xmin, my xmax, my nx, my dx, my x1);

		const double db_ref = 10.0 * log10 (FilterBank_DBREF);
		for (integer j = 1; j <= my nx; j ++) {
			longdouble p = 0.0;
			for (integer i = 1; i <= my ny; i ++)
				p += FilterBank_DBREF * exp (NUMln10 * my z [i] [j] / 10.0);
			thy z [1] [j] = 10.0 * log10 ((double) p) - db_ref;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Intensity not created.");
	}
}

void FilterBank_equalizeIntensities (FilterBank me, double intensity_db) {
	for (integer j = 1; j <= my nx; j ++) {
		longdouble p = 0.0;
		for (integer i = 1; i <= my ny; i ++)
			p += FilterBank_DBREF * exp (NUMln10 * my z [i] [j] / 10.0);
		const double delta_db = intensity_db - 10.0 * log10 (p / FilterBank_DBREF);

		my z.column (j)  +=  delta_db;
	}
}

void FilterBank_PCA_drawComponent (FilterBank me, PCA thee, Graphics g, integer component, double dblevel, double frequencyOffset, double scale, double tmin, double tmax, double fmin, double fmax) {
	Melder_require (component > 0 && component <= thy numberOfEigenvalues, U"Component should be in the range [1, ", thy numberOfEigenvalues, U"].");
	
	// Scale Intensity

	autoFilterBank fcopy = Data_copy (me);
	FilterBank_equalizeIntensities (fcopy.get(), dblevel);
	autoMatrix him = Eigen_Matrix_to_Matrix_projectColumns (thee, fcopy.get(), component);
	for (integer j = 1; j <= my nx; j ++) {
		fcopy -> z [component] [j] = frequencyOffset + scale * fcopy -> z [component] [j];
	}
	Matrix_drawRows (fcopy.get(), g, tmin, tmax, component - 0.5, component + 0.5, fmin, fmax);
}

// Convert old types to new types

autoMelSpectrogram MelFilter_to_MelSpectrogram (MelFilter me) {
	try {
		autoMelSpectrogram thee = MelSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (integer i = 1; i <= my ny; i ++) {
			for (integer j = 1; j <= my nx; j ++) {
				thy z [i] [j] = 4e-10 * pow (10, my z [i] [j] / 10);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"MelSpectrogram not created.");
	}
}

autoBarkSpectrogram BarkFilter_to_BarkSpectrogram (BarkFilter me) {
	try {
		autoBarkSpectrogram thee = BarkSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (integer i = 1; i <= my ny; i ++) {
			for (integer j = 1; j <= my nx; j ++) {
				thy z [i] [j] = 4e-10 * pow (10, my z [i] [j] / 10);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"BarkSpectrogram not created.");
	}
}

autoSpectrogram FormantFilter_to_Spectrogram (FormantFilter me) {
	try {
		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (integer i = 1; i <= my ny; i ++)
			for (integer j = 1; j <= my nx; j ++)
				thy z [i] [j] = 4e-10 * pow (10, my z [i] [j] / 10);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Spectrogram not created.");
	}
}

	/* MelFilter_and_MFCC.cpp */

autoMFCC MelFilter_to_MFCC (MelFilter me, integer numberOfCoefficients) {
	try {
		autoMAT cosinesTable = MATcosinesTable (my ny);
		autoVEC x = raw_VEC (my ny);
		autoVEC y = raw_VEC (my ny);
		
		//double fmax_mel = my y1 + (my ny - 1) * my dy;
		numberOfCoefficients = ( numberOfCoefficients > my ny - 1 ? my ny - 1 : numberOfCoefficients );
		Melder_assert (numberOfCoefficients > 0);
		// 20130220 new interpretation of maximumNumberOfCoefficients necessary for inverse transform 
		autoMFCC thee = MFCC_create (my xmin, my xmax, my nx, my dx, my x1, my ny - 1, my ymin, my ymax);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const CC_Frame cf = & thy frame [iframe];
			x.all() <<= my z.column (iframe);
			VECcosineTransform_preallocated (y.get(), x.get(), cosinesTable.get());
			CC_Frame_init (cf, numberOfCoefficients);
			for (integer icoef = 1; icoef <= numberOfCoefficients; icoef ++)
				cf -> c [icoef] = y [icoef + 1];
			cf -> c0 = y [1];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MFCC created.");
	}
}

autoMelFilter MFCC_to_MelFilter (MFCC me, integer first, integer last) {
	try {
		const integer nf = my maximumNumberOfCoefficients + 1;
		autoMAT cosinesTable = MATcosinesTable (nf);
		autoVEC x = raw_VEC (nf);
		autoVEC y = raw_VEC (nf);

		if (first >= last) {
			first = 0;
			last = nf - 1;
		}
		Melder_require (first >= 0 && last <= nf,
			U"MFCC_to_MelFilter: coefficients should be in interval [0,", my maximumNumberOfCoefficients, U"].");
		
		const double df = (my fmax - my fmin) / (nf + 1);
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, my fmin, my fmax, nf, df, df);

		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const CC_Frame cf = & my frame [iframe];
			const integer iend = std::min (last, cf -> numberOfCoefficients);
			x [1] = ( first == 0 ? cf -> c0 : 0.0 );
			for (integer icoef = 1; icoef <= my maximumNumberOfCoefficients; icoef ++)
				x [icoef + 1] = ( icoef < first || icoef > iend ? 0.0 : cf -> c [icoef] );   // zero extrapolation
			VECinverseCosineTransform_preallocated (y.get(), x.get(), cosinesTable.get());
			thy z.column (iframe) <<= y.get();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MelFilter created.");
	}
}

/* Sound_and_FilterBank.cpp */

// prototypes
autoSound FilterBank_as_Sound (FilterBank me);

/*
	The gaussian(x) = (exp(-48*((i-(n+1)/2)/(n+1))^2)-exp(-12))/(1-exp(-12));
	For power we need the area under the square of this window:
		Integrate (gaussian(i)^2,i=1..n) =

	(sqrt(Pi)*sqrt(3)*sqrt(2)*erf(2*(n-1)*sqrt(3)*sqrt(2)/(n+1))*(n+1)+
		24*exp(-24)*(n-1)+
	-4*sqrt(Pi)*sqrt(3)*exp(-12)*erf(2*(n-1)*sqrt(3)/(n+1))*(n+1))/
	(24 * (-1+exp(-12))^2)
	To compare with the rectangular window we need to divide this by the
	window width (n-1) x 1^2.
*/
static double gaussian_window_squared_correction (integer n) {
	const double e12 = exp (-12.0), denum = (e12 - 1.0) * (e12 - 1.0) * 24.0 * (n - 1);
	const double sqrt3 = sqrt (3.0), sqrt2 = sqrt (2.0), sqrtpi = sqrt (NUMpi);
	const double arg1 = 2.0 * sqrt3 * (n - 1) / (n + 1), arg2 = arg1 * sqrt2;
	const double p2 = sqrtpi * sqrt3 * sqrt2 * (1.0 - NUMerfcc (arg2)) * (n + 1);
	const double p1 = 4.0 * sqrtpi * sqrt3 * e12 * (1.0 - NUMerfcc (arg1)) * (n + 1);

	return (p2 - p1 + 24.0 * (n - 1) * e12 * e12) / denum;
}

static autoMatrix Sound_to_spectralpower (Sound me) {
	try {
		autoSpectrum s = Sound_to_Spectrum (me, true);
		autoMatrix thee = Matrix_create (s -> xmin, s -> xmax, s -> nx, s -> dx, s -> x1, 1.0, 1.0, 1, 1.0, 1.0);
		const double scale = 2.0 * s -> dx / (my xmax - my xmin);

		// factor '2' because of positive and negative frequencies
		// s -> dx : width of frequency bin
		// my xmax - my xmin : duration of sound

		constVEC re = s -> z.row (1), im = s -> z.row (2);
		for (integer i = 1; i <= s -> nx; i ++)
			thy z [1] [i] = scale * (re [i] * re [i] + im [i] * im [i]);

		// Frequency bins at 0 Hz and nyquist don't count for two.

		thy z [1] [1] *= 0.5;
		thy z [1] [s -> nx] *= 0.5;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix with spectral power created.");
	}
}

static int Sound_into_BarkFilter_frame (Sound me, BarkFilter thee, integer frame) {
	autoMatrix pv = Sound_to_spectralpower (me);
	const integer nf = pv -> nx;

	autoVEC z = raw_VEC (nf);
	for (integer j = 1; j <= nf; j ++)
		z [j] = HZTOBARK (pv -> x1 + (j - 1) * pv -> dx);

	for (integer i = 1; i <= thy ny; i ++) {
		const double z0 = thy y1 + (i - 1) * thy dy;
		const constVEC pow = pv -> z.row (1); // TODO ??
		longdouble p = 0.0;
		for (integer j = 1; j <= nf; j ++) {
			/*
				Sekey & Hanson filter is defined in the power domain.
				We therefore multiply the power with a (and not a^2).
				integral (F(z),z=0..25) = 1.58/9
			*/
			const double a = NUMsekeyhansonfilter_amplitude (z0, z [j]);
			p += a * pow [j] ;
		}
		thy z [i] [frame] = double (p);
	}
	return 1;
}

autoBarkFilter Sound_to_BarkFilter (Sound me, double analysisWidth, double dt, double f1_bark, double fmax_bark, double df_bark) {
	try {

		const double nyquist = 0.5 / my dx, samplingFrequency = 2.0 * nyquist;
		const double windowDuration = 2.0 * analysisWidth; /* gaussian window */
		const double zmax = NUMhertzToBark2 (nyquist);
		double fmin_bark = 0.0;

		// Check defaults.

		if (f1_bark <= 0.0)
			f1_bark = 1.0;
		if (fmax_bark <= 0.0)
			fmax_bark = zmax;
		if (df_bark <= 0.0)
			df_bark = 1.0;

		Melder_clipRight (& fmax_bark, zmax);
		integer nf = Melder_iround ((fmax_bark - f1_bark) / df_bark);
		Melder_require (nf > 0, U"The combination of filter parameters is not valid.");

		double t1;
		integer numberOfFrames, frameErrorCount = 0;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoBarkFilter thee = BarkFilter_create (my xmin, my xmax, numberOfFrames, dt, t1,
		                      fmin_bark, fmax_bark, nf, df_bark, f1_bark);

		autoMelderProgress progess (U"BarkFilter analysis");

		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double t = Sampled_indexToX (thee.get(), iframe);

			Sound_into_Sound (me, sframe.get(), t - windowDuration / 2);

			Sounds_multiply (sframe.get(), window.get());

			if (! Sound_into_BarkFilter_frame (sframe.get(), thee.get(), iframe))
				frameErrorCount ++;

			if ( (iframe % 10) == 1)
				Melder_progress ( (double) iframe / numberOfFrames,  U"BarkFilter analysis: frame ",
					iframe, U" from ", numberOfFrames, U".");
		}

		if (frameErrorCount > 0)
			Melder_warning (U"Analysis results of ", frameErrorCount, U" frame(s) out of ",
				numberOfFrames, U" will be suspect.");

		double ref = FilterBank_DBREF * gaussian_window_squared_correction (window -> nx);

		NUMdmatrix_to_dBs (thy z.get(), ref, FilterBank_DBFAC, FilterBank_DBFLOOR);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no BarkFilter created.");
	}
}

static int Sound_into_MelFilter_frame (Sound me, MelFilter thee, integer frame) {
	autoMatrix pv = Sound_to_spectralpower (me);

	const double z1 = pv -> x1;
	const double dz = pv -> dx;
	const integer nf = pv -> nx;
	const double df = thy dy;
	for (integer i = 1; i <= thy ny; i ++) {
		const double fc_mel = thy y1 + (i - 1) * df;
		const double fc_hz = MELTOHZ (fc_mel);
		const double fl_hz = MELTOHZ (fc_mel - df);
		const double fh_hz =  MELTOHZ (fc_mel + df);
		const constVEC pow = pv -> z.row (1);
		double p = 0.0;
		for (integer j = 1; j <= nf; j ++) {
			// Bin with a triangular filter the power (= amplitude-squared)

			const double f = z1 + (j - 1) * dz;
			const double a = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, f);
			p += a * pow [j];
		}
		thy z [i] [frame] = p;
	}
	return 1;
}

autoMelFilter Sound_to_MelFilter (Sound me, double analysisWidth, double dt, double f1_mel, double fmax_mel, double df_mel) {
	try {
		const double samplingFrequency = 1.0 / my dx, nyquist = 0.5 * samplingFrequency;
		const double windowDuration = 2.0 * analysisWidth; /* gaussian window */
		const double fmin_mel = 0.0;
		const double fbottom = HZTOMEL (100.0), fceiling = HZTOMEL (nyquist);

		// Check defaults.

		if (fmax_mel <= 0.0 || fmax_mel > fceiling)
			fmax_mel = fceiling;
		if (fmax_mel <= f1_mel) {
			f1_mel = fbottom;
			fmax_mel = fceiling;
		}
		if (f1_mel <= 0.0)
			f1_mel = fbottom;
		if (df_mel <= 0.0)
			df_mel = 100.0;

		// Determine the number of filters.

		const integer nf = Melder_iround ((fmax_mel - f1_mel) / df_mel);
		fmax_mel = f1_mel + nf * df_mel;
		double t1;
		integer numberOfFrames, frameErrorCount = 0;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_mel, fmax_mel, nf, df_mel, f1_mel);

		autoMelderProgress progress (U"MelFilters analysis");

		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double t = Sampled_indexToX (thee.get(), iframe);
			Sound_into_Sound (me, sframe.get(), t - windowDuration / 2);
			Sounds_multiply (sframe.get(), window.get());
			if (! Sound_into_MelFilter_frame (sframe.get(), thee.get(), iframe))
				frameErrorCount ++;
			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames, U"Frame ", iframe, U" out of ", numberOfFrames, U".");
		}

		if (frameErrorCount)
			Melder_warning (U"Analysis results of ", frameErrorCount,
			U" frame(s) out of ", numberOfFrames, U" will be suspect.");

		// Window correction.

		const double ref = FilterBank_DBREF * gaussian_window_squared_correction (window -> nx);

		NUMdmatrix_to_dBs (thy z.get(), ref, FilterBank_DBFAC, FilterBank_DBFLOOR);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MelFilter created.");
	}
}

/*
	Analog formant filter response :
	H(f) = i f B / (f1^2 - f^2 + i f B)
*/
static int Sound_into_FormantFilter_frame (Sound me, FormantFilter thee, integer frame, double bw) {
	Melder_assert (bw > 0);
	autoMatrix pv = Sound_to_spectralpower (me);
	const double z1 = pv -> x1;
	const double dz = pv -> dx;
	const integer nf = pv -> nx;

	for (integer i = 1; i <= thy ny; i ++) {
		const double fc = thy y1 + (i - 1) * thy dy;
		const constVEC pow = pv -> z.row (1);
		double p = 0.0;
		for (integer j = 1; j <= nf; j ++) {
			// H(f) = ifB / (fc^2 - f^2 + ifB)
			// H(f)| = fB / sqrt ((fc^2 - f^2)^2 + f^2B^2)
			//|H(f)|^2 = f^2B^2 / ((fc^2 - f^2)^2 + f^2B^2)
			//         = 1 / (((fc^2 - f^2) /fB)^2 + 1)

			const double f = z1 + (j - 1) * dz;
			const double a = NUMformantfilter_amplitude (fc, bw, f);
			p += a * pow [j];
		}
		thy z [i] [frame] = p;
	}
	return 1;
}

autoFormantFilter Sound_to_FormantFilter (Sound me, double analysisWidth, double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw, double minimumPitch, double maximumPitch) {
	try {
		const double floor = 80.0, ceiling = 600.0;
		if (minimumPitch >= maximumPitch) {
			minimumPitch = floor;
			maximumPitch = ceiling;
		}
		if (minimumPitch <= 0.0)
			minimumPitch = floor;
		if (maximumPitch <= 0.0)
			maximumPitch = ceiling;

		autoPitch thee = Sound_to_Pitch (me, dt, minimumPitch, maximumPitch);
		autoFormantFilter ff = Sound_Pitch_to_FormantFilter (me, thee.get(), analysisWidth, dt, f1_hz, fmax_hz, df_hz, relative_bw);
		return ff;
	} catch (MelderError) {
		Melder_throw (me, U": no FormantFilter created.");
	}
}

autoFormantFilter Sound_Pitch_to_FormantFilter (Sound me, Pitch thee, double analysisWidth, double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw) {
	try {
		const double windowDuration = 2.0 * analysisWidth;   // Gaussian window
		const double nyquist = 0.5 / my dx, samplingFrequency = 2.0 * nyquist, fmin_hz = 0.0;

		Melder_require (my xmin >= thy xmin && my xmax <= thy xmax,
			U"The domain of the Sound should be included in the domain of the Pitch.");

		double f0_median = Pitch_getQuantile (thee, thy xmin, thy xmax, 0.5, kPitch_unit::HERTZ);

		if (isundef (f0_median) || f0_median == 0.0) {
			f0_median = 100.0;
			Melder_warning (U"Pitch values undefined. Bandwith fixed to 100 Hz. ");
		}

		if (f1_hz <= 0.0)
			f1_hz = 100.0;
		if (fmax_hz <= 0.0)
			fmax_hz = nyquist;
		if (df_hz <= 0.0)
			df_hz = f0_median / 2.0;
		if (relative_bw <= 0.0)
			relative_bw = 1.1;

		fmax_hz = std::min (fmax_hz, nyquist);
		const integer nf = Melder_iround ( (fmax_hz - f1_hz) / df_hz);
		double t1;
		integer numberOfFrames;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoFormantFilter him = FormantFilter_create (my xmin, my xmax, numberOfFrames, dt, t1,
			fmin_hz, fmax_hz, nf, df_hz, f1_hz);

		/*
			Temporary objects.
		*/
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelderProgress progress (U"Sound & Pitch: To FormantFilter");
		integer numberOfUndefinedPitches = 0;
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double t = Sampled_indexToX (him.get(), iframe);
			double bandwidth, f0 = Pitch_getValueAtTime (thee, t, kPitch_unit::HERTZ, 0);

			if (isundef (f0) || f0 == 0.0) {
				numberOfUndefinedPitches ++;
				f0 = f0_median;
			}
			bandwidth = relative_bw * f0;
			Sound_into_Sound (me, sframe.get(), t - windowDuration / 2.0);
			Sounds_multiply (sframe.get(), window.get());

			Sound_into_FormantFilter_frame (sframe.get(), him.get(), iframe, bandwidth);

			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames, U"Frame ", iframe, U" out of ", numberOfFrames, U".");
		}

		const double ref = FilterBank_DBREF * gaussian_window_squared_correction (window -> nx);
		NUMdmatrix_to_dBs (his z.get(), ref, FilterBank_DBFAC, FilterBank_DBFLOOR);
		return him;
	} catch (MelderError) {
		Melder_throw (U"FormantFilter not created from Pitch & FormantFilter.");
	}
}

autoSound FilterBank_as_Sound (FilterBank me) {
	try {
		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		for (integer i = 1; i <= my ny; i ++) {
			for (integer j = 1; j <= my nx; j ++)
				thy z [i] [j] = my z [i] [j];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}

autoSound FilterBanks_crossCorrelate (FilterBank me, FilterBank thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound cc = Sounds_crossCorrelate ((Sound) me, (Sound) thee, scaling, signalOutsideTimeDomain);
		return cc;
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U" not cross-correlated.");
	}
}

autoSound FilterBanks_convolve (FilterBank me, FilterBank thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound cc = Sounds_convolve ((Sound) me, (Sound) thee, scaling, signalOutsideTimeDomain);
		return cc;
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U" not convolved.");
	}
}

/* End of file Filterbank.cpp */
