/* FilterBank.cpp
 *
 * Copyright (C) 1993-2012, 2014 David Weenink
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

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

static double scaleFrequency (double f, int scale_from, int scale_to) {
	double fhz = NUMundefined;

	if (scale_from == scale_to) {
		return f;
	}
	if (scale_from == FilterBank_HERTZ) {
		fhz = f;
	} else if (scale_from == FilterBank_BARK) {
		fhz = BARKTOHZ (f);
	} else if (scale_from == FilterBank_MEL) {
		fhz = MELTOHZ (f);
	}

	if (scale_to == FilterBank_HERTZ || fhz == NUMundefined) {
		return fhz;
	}

	if (scale_to == FilterBank_BARK) {
		f = HZTOBARK (fhz);
	} else if (scale_to == FilterBank_MEL) {
		f = HZTOMEL (fhz);
	} else {
		return NUMundefined;
	}
	return f;
}

static wchar_t const *GetFreqScaleText (int scale) {
	wchar_t const *hertz = L"Frequency (Hz)";
	wchar_t const *bark = L"Frequency (Bark)";
	wchar_t const *mel = L"Frequency (mel)";
	wchar_t const *error = L"Frequency (undefined)";
	if (scale == FilterBank_HERTZ) {
		return hertz;
	} else if (scale == FilterBank_BARK) {
		return bark;
	} else if (scale == FilterBank_MEL) {
		return mel;
	}
	return error;
}

static int checkLimits (I, int fromFreqScale, int toFreqScale, int *fromFilter,
                        int *toFilter, double *zmin, double *zmax, int dbScale,
                        double *ymin, double *ymax) {
	iam (Matrix);

	if (*fromFilter == 0) {
		*fromFilter = 1;
	}
	if (*toFilter == 0) {
		*toFilter = my ny;
	}
	if (*toFilter < *fromFilter) {
		*fromFilter = 1;
		*toFilter = my ny;
	}
	if (*fromFilter < 1) {
		*fromFilter = 1;
	}
	if (*toFilter > my ny) {
		*toFilter = my ny;
	}
	if (*fromFilter > *toFilter) {
		Melder_warning (L"Filter numbers must be in range [1, ", Melder_integer (my ny), L"]");
		return 0;
	}

	if (*zmin < 0 || *zmax < 0) {
		Melder_warning (L"Frequencies must be positive.");
		return 0;
	}
	if (*zmax <= *zmin) {
		*zmin = scaleFrequency (my ymin, fromFreqScale, toFreqScale);
		*zmax = scaleFrequency (my ymax, fromFreqScale, toFreqScale);
	}

	if (*ymax <= *ymin) {
		*ymax = 1; *ymin = 0;
		if (dbScale) {
			*ymax = 0; *ymin = -60;
		}
	}
	return 1;
}

static double to_dB (double a, double factor, double ref_dB) {
	if (a <= 0) {
		return ref_dB;
	}
	a = factor * log10 (a);
	if (a < ref_dB) {
		a = ref_dB;
	}
	return a;
}

static void setDrawingLimits (double *a, long n, double amin, double amax, long *ibegin, long *iend) {
	*ibegin = 0;
	*iend = n + 1;

	long lower = 1;
	for (long i = 1; i <= n; i++) {
		if (a[i] == NUMundefined) {
			if (lower == 0) {
				/* high frequency part */
				*iend = i;
				break;
			}
			*ibegin = i;
			continue;
		}
		lower = 0;
		if (a[i] < amin) {
			a[i] = amin;
		} else if (a[i] > amax) {
			a[i] = amax;
		}
	}

	(*ibegin) ++;
	(*iend)--;
}

Thing_implement (FilterBank, Matrix, 2);

Thing_implement (BarkFilter, FilterBank, 2);

BarkFilter BarkFilter_create (double tmin, double tmax, long nt, double dt,
                              double t1, double fmin, double fmax, long nf, double df, long f1) {
	try {
		autoBarkFilter me = Thing_new (BarkFilter);
		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("BarkFilter not created.");
	}
}

double FilterBank_getFrequencyInHertz (I, double f, int scale_from) {
	(void) void_me;
	return scaleFrequency (f, scale_from, FilterBank_HERTZ);
}

double FilterBank_getFrequencyInBark (I, double f, int scale_from) {
	(void) void_me;
	return scaleFrequency (f, scale_from, FilterBank_BARK);
}

double FilterBank_getFrequencyInMel (I, double f, int scale_from) {
	(void) void_me;
	return scaleFrequency (f, scale_from, FilterBank_MEL);
}

int FilterBank_getFrequencyScale (I) {
	iam (FilterBank);
	return my v_getFrequencyScale ();
}

void FilterBank_drawFrequencyScales (I, Graphics g, int horizontalScale, double xmin,
                                     double xmax, int verticalScale, double ymin, double ymax, int garnish) {
	iam (FilterBank);
	int myFreqScale = FilterBank_getFrequencyScale (me);

	if (xmin < 0 || xmax < 0 || ymin < 0 || ymax < 0) {
		Melder_warning (L"Frequencies must be >= 0.");
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

	long n = 2000;
	autoNUMvector<double> a (1, n);

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	double df = (xmax - xmin) / (n - 1);

	for (long i = 1; i <= n; i++) {
		double f = xmin + (i - 1) * df;
		a[i] = scaleFrequency (f, horizontalScale, verticalScale);
	}

	long ibegin, iend;
	setDrawingLimits (a.peek(), n, ymin, ymax,	& ibegin, & iend);
	if (ibegin <= iend) {
		double fmin = xmin + (ibegin - 1) * df;
		double fmax = xmax - (n - iend) * df;
		Graphics_function (g, a.peek(), ibegin, iend, fmin, fmax);
	}
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, GetFreqScaleText (verticalScale));
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textBottom (g, 1, GetFreqScaleText (horizontalScale));
	}
}

void FilterBank_paint (FilterBank me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, int garnish) {
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax; 
	}
	if (ymax <= ymin) {
		ymin = my ymin; ymax = my ymax;
	}
	long ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx, &ixmin, &ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy, &iymin, &iymax);
	if (maximum <= minimum) {
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, &minimum, &maximum);
	}
	if (maximum <= minimum) { 
		minimum -= 1.0; maximum += 1.0;
	}
	if (xmin >= xmax || ymin >= ymax) {
		return;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_image (g, my z,
			ixmin, ixmax, Sampled_indexToX   (me, ixmin - 0.5), Sampled_indexToX   (me, ixmax + 0.5),
			iymin, iymax, SampledXY_indexToY (me, iymin - 0.5), SampledXY_indexToY (me, iymax + 0.5),
			minimum, maximum);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, GetFreqScaleText (my v_getFrequencyScale ()));
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textBottom (g, 1, L"Time (s)");
	}
}

void BarkFilter_drawSekeyHansonFilterFunctions (BarkFilter me, Graphics g,
        int toFreqScale, int fromFilter, int toFilter, double zmin, double zmax,
        int dbScale, double ymin, double ymax, int garnish) {
	if (! checkLimits (me, FilterBank_BARK, toFreqScale, & fromFilter, & toFilter,
	                   & zmin, & zmax, dbScale, & ymin, & ymax)) {
		return;
	}

	long n = 1000;
	autoNUMvector<double> a (1, n);

	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);

	for (long j = fromFilter; j <= toFilter; j++) {
		double df = (zmax - zmin) / (n - 1);
		double zMid = Matrix_rowToY (me, j);
		long ibegin, iend;

		for (long i = 1; i <= n; i++) {
			double f = zmin + (i - 1) * df;
			double z = scaleFrequency (f, toFreqScale, FilterBank_BARK);
			if (z == NUMundefined) {
				a[i] = NUMundefined;
			} else {
				z -= zMid + 0.215;
				a[i] = 7 - 7.5 * z - 17.5 * sqrt (0.196 + z * z);
				if (! dbScale) {
					a[i] = pow (10, a[i]);
				}
			}
		}

		setDrawingLimits (a.peek(), n, ymin, ymax, &ibegin, &iend);

		if (ibegin <= iend) {
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a.peek(), ibegin, iend, fmin, fmax);
		}
	}


	Graphics_unsetInner (g);

	if (garnish) {
		double distance = dbScale ? 10 : 1;
		wchar_t const *ytext = dbScale ? L"Amplitude (dB)" : L"Amplitude";
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1, distance, 1, 1, 0);
		Graphics_textLeft (g, 1, ytext);
		Graphics_textBottom (g, 1, GetFreqScaleText (toFreqScale));
	}
}

Thing_implement (MelFilter, FilterBank, 2);

MelFilter MelFilter_create (double tmin, double tmax, long nt, double dt,
                            double t1, double fmin, double fmax, long nf, double df, double f1) {
	try {
		autoMelFilter me = Thing_new (MelFilter);
		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("MelFilter not created.");
	}
}

/*
void FilterBank_drawFilters (I, Graphics g, long fromf, long tof,
	double xmin, double xmax, int xlinear, double ymin, double ymax, int ydb,
	double (*tolinf)(double f), double (*tononlin) (double f),
	double (*filteramp)(double f0, double b, double f))
{
	iam (Matrix);


}*/

void FilterBank_drawTimeSlice (I, Graphics g, double t, double fmin,
                               double fmax, double min, double max, const wchar_t *xlabel, int garnish) {
	iam (Matrix);
	Matrix_drawSliceY (me, g, t, fmin, fmax, min, max);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (xlabel) {
			Graphics_textBottom (g, 0, xlabel);
		}
	}
}

void MelFilter_drawFilterFunctions (MelFilter me, Graphics g,
                                    int toFreqScale, int fromFilter, int toFilter, double zmin, double zmax,
                                    int dbScale, double ymin, double ymax, int garnish) {
	if (! checkLimits (me, FilterBank_MEL, toFreqScale, & fromFilter, & toFilter,
	                   & zmin, & zmax, dbScale, & ymin, & ymax)) {
		return;
	}
	long n = 1000;
	autoNUMvector<double> a (1, n);

	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);

	for (long j = fromFilter; j <= toFilter; j++) {
		double df = (zmax - zmin) / (n - 1);
		double fc_mel = my y1 + (j - 1) * my dy;
		double fc_hz = MELTOHZ (fc_mel);
		double fl_hz = MELTOHZ (fc_mel - my dy);
		double fh_hz = MELTOHZ (fc_mel + my dy);
		long ibegin, iend;

		for (long i = 1; i <= n; i++) {
			// Filterfunction: triangular on a linear frequency scale AND a linear amplitude scale.
			double f = zmin + (i - 1) * df;
			double z = scaleFrequency (f, toFreqScale, FilterBank_HERTZ);
			if (z == NUMundefined) {
				a[i] = NUMundefined;
			} else {
				a[i] = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, z);
				if (dbScale) {
					a[i] = to_dB (a[i], 10, ymin);
				}
			}

		}

		setDrawingLimits (a.peek(), n, ymin, ymax,	&ibegin, &iend);

		if (ibegin <= iend) {
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a.peek(), ibegin, iend, fmin, fmax);
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		double distance = dbScale ? 10 : 1;
		wchar_t const *ytext = dbScale ? L"Amplitude (dB)" : L"Amplitude";
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1, distance, 1, 1, 0);
		Graphics_textLeft (g, 1, ytext);
		Graphics_textBottom (g, 1, GetFreqScaleText (toFreqScale));
	}
}

/*
void MelFilter_drawFilters (MelFilter me, Graphics g, long from, long to,
	double fmin, double fmax, double ymin, double ymax, int dbscale,
	int garnish)
{
	long i;
	double df = my dy;

	if (fmin >= fmax)
	{
		fmin = my ymin;
		fmax = my ymax;
	}
	if (from >= to)
	{
		from = 1;
		to = my ny;
	}
	Graphics_setWindow (g, my ymin, my ymax, 0, 1);
	Graphics_setInner (g);
	for (i = from; i <= to; i++)
	{
		double fc = my y1 + (i - 1) * df;
		double fc_hz = MELTOHZ (fc);
		double fl_hz = MELTOHZ (fc - df);
		double fh_hz = MELTOHZ (fc + df);
		*//*
			Draw triangle
		*//*
		Graphics_line (g, fl_hz, 0, fc_hz, 1);
		Graphics_line (g, fc_hz, 1, fh_hz, 0);
	}
	Graphics_unsetInner (g);
}
*/
Matrix FilterBank_to_Matrix (I) {
	iam (Matrix);
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		                                 my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

BarkFilter Matrix_to_BarkFilter (I) {
	iam (Matrix);
	try {
		autoBarkFilter thee = BarkFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		                      my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to BarkFilter.");
	}
}

MelFilter Matrix_to_MelFilter (I) {
	iam (Matrix);
	try {
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		                                       my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to MelFilter.");
	}
}

Thing_implement (FormantFilter, FilterBank, 2);

FormantFilter FormantFilter_create (double tmin, double tmax, long nt,
                                    double dt, double t1, double fmin, double fmax, long nf, double df, double f1) {
	try {
		autoFormantFilter me = Thing_new (FormantFilter);
		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FormantFilter not created.");
	}
}

void FormantFilter_drawFilterFunctions (FormantFilter me, Graphics g, double bandwidth,
                                        int toFreqScale, int fromFilter, int toFilter, double zmin, double zmax,
                                        int dbScale, double ymin, double ymax, int garnish) {
	if (! checkLimits (me, FilterBank_HERTZ, toFreqScale, & fromFilter, & toFilter,
	                   & zmin, & zmax, dbScale, & ymin, & ymax)) {
		return;
	}

	if (bandwidth <= 0) {
		Melder_warning (L"Bandwidth must be greater than zero.");
	}

	long n = 1000;
	autoNUMvector<double>a (1, n);

	Graphics_setInner (g);
	Graphics_setWindow (g, zmin, zmax, ymin, ymax);

	for (long j = fromFilter; j <= toFilter; j++) {
		double df = (zmax - zmin) / (n - 1);
		double fc = my y1 + (j - 1) * my dy;
		long ibegin, iend;

		for (long i = 1; i <= n; i++) {
			double f = zmin + (i - 1) * df;
			double z = scaleFrequency (f, toFreqScale, FilterBank_HERTZ);
			if (z == NUMundefined) {
				a[i] = NUMundefined;
			} else {
				a[i] = NUMformantfilter_amplitude (fc, bandwidth, z);
				if (dbScale) {
					a[i] = to_dB (a[i], 10, ymin);
				}
			}
		}

		setDrawingLimits (a.peek(), n, ymin, ymax,	&ibegin, &iend);

		if (ibegin <= iend) {
			double fmin = zmin + (ibegin - 1) * df;
			double fmax = zmax - (n - iend) * df;
			Graphics_function (g, a.peek(), ibegin, iend, fmin, fmax);
		}
	}


	Graphics_unsetInner (g);

	if (garnish) {
		double distance = dbScale ? 10 : 1;
		wchar_t const *ytext = dbScale ? L"Amplitude (dB)" : L"Amplitude";
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1, distance, 1, 1, 0);
		Graphics_textLeft (g, 1, ytext);
		Graphics_textBottom (g, 1, GetFreqScaleText (toFreqScale));
	}
}

FormantFilter Matrix_to_FormantFilter (I) {
	iam (Matrix);
	try {
		autoFormantFilter thee = FormantFilter_create (my xmin, my xmax, my nx, my dx, my x1,
		                         my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to FormantFilter.");
	}
}

Spectrum FormantFilter_to_Spectrum_slice (FormantFilter me, double t) {
	try {
		double sqrtref = sqrt (FilterBank_DBREF);
		double factor2 = 2 * 10 * FilterBank_DBFAC;
		autoSpectrum thee = Spectrum_create (my ymax, my ny);

		thy xmin = my ymin;
		thy xmax = my ymax;
		thy x1 = my y1;
		thy dx = my dy;   /* Frequency step. */

		long frame = Sampled_xToIndex (me, t);
		if (frame < 1) {
			frame = 1;
		}
		if (frame > my nx) {
			frame = my nx;
		}

		for (long i = 1; i <= my ny; i++) {
			/*
				power = ref * 10 ^ (value / 10)
				sqrt(power) = sqrt(ref) * 10 ^ (value / (2*10))
			*/
			thy z[1][i] = sqrtref * pow (10, my z[i][frame] / factor2);
			thy z[2][i] = 0.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Spectral slice not created.");
	}
}

Intensity FilterBank_to_Intensity (I) {
	iam (Matrix);
	try {
		autoIntensity thee = Intensity_create (my xmin, my xmax, my nx, my dx, my x1);

		double db_ref = 10 * log10 (FilterBank_DBREF);
		for (long j = 1; j <= my nx; j++) {
			double p = 0;
			for (long i = 1; i <= my ny; i++) {
				p += FilterBank_DBREF * exp (NUMln10 * my z[i][j] / 10);
			}
			thy z[1][j] = 10 * log10 (p) - db_ref;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Intensity not created.");
	}
}

void FilterBank_equalizeIntensities (I, double intensity_db) {
	iam (Matrix);
	for (long j = 1; j <= my nx; j++) {
		double p = 0;
		for (long i = 1; i <= my ny; i++) {
			p += FilterBank_DBREF * exp (NUMln10 * my z[i][j] / 10);
		}

		double delta_db = intensity_db - 10 * log10 (p / FilterBank_DBREF);

		for (long i = 1; i <= my ny; i++) {
			my z[i][j] += delta_db;
		}
	}
}

void FilterBank_and_PCA_drawComponent (I, PCA thee, Graphics g, long component, double dblevel,
                                       double frequencyOffset, double scale, double tmin, double tmax, double fmin, double fmax) {
	iam (FilterBank);
	if (component < 1 || component > thy numberOfEigenvalues) {
		Melder_throw ("Component too large.");
	}

	// Scale Intensity

	autoFilterBank fcopy = (FilterBank) Data_copy (me);
	FilterBank_equalizeIntensities (fcopy.peek(), dblevel);
	autoMatrix him = Eigen_and_Matrix_project (thee, fcopy.peek(), component);
	for (long j = 1; j <= my nx; j++) {
		fcopy -> z[component][j] = frequencyOffset + scale * fcopy -> z[component][j];
	}
	Matrix_drawRows (fcopy.peek(), g, tmin, tmax, component - 0.5, component + 0.5, fmin, fmax);
}

// Convert old types to new types

MelSpectrogram MelFilter_to_MelSpectrogram (MelFilter me) {
	try {
		autoMelSpectrogram thee = MelSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				thy z[i][j] = 4e-10 * pow (10, my z[i][j] / 10);
			}
		}
		return thee.transfer ();
	} catch (MelderError) {
		Melder_throw ("MelSpectrogram not created.");
	}
}

BarkSpectrogram BarkFilter_to_BarkSpectrogram (BarkFilter me) {
	try {
		autoBarkSpectrogram thee = BarkSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				thy z[i][j] = 4e-10 * pow (10, my z[i][j] / 10);
			}
		}
		return thee.transfer ();
	} catch (MelderError) {
		Melder_throw ("BarkSpectrogram not created.");
	}
}

Spectrogram FormantFilter_to_Spectrogram (FormantFilter me) {
	try {
		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				thy z[i][j] = 4e-10 * pow (10, my z[i][j] / 10);
			}
		}
		return thee.transfer ();
	} catch (MelderError) {
		Melder_throw ("Spectrogram not created.");
	}
}

	/* MelFilter_and_MFCC.cpp */
	
static double **NUMcosinesTable (long  n) {
	autoNUMmatrix<double> costab (1, n, 1, n);
	for (long k = 1; k <= n; k++) {
		for (long j = 1; j <= n; j++) {
			costab[k][j] = cos (NUMpi * (k - 1) * (j - 0.5) / n);
		}
	}
	return costab.transfer();
}

// x[1..n] : input, y[1..n] : output
static void NUMcosineTransform (double *x, double *y, long n, double **cosinesTable) {
	for (long k = 1; k <= n; k++) {
		y[k] = 0;
		for (long j = 1; j <= n; j++) {
			y[k] += x[j] * cosinesTable[k][j];
		}
	}
}

// x: input, y: output
static void NUMinverseCosineTransform (double *x, double *y, long n, double **cosinesTable) {
	for (long j = 1; j <= n; j++) {
		y[j] = 0.5 * x[1]; // * cosinesTable[1][j];
		for (long k = 2; k <= n; k++) {
			y[j] += x[k] * cosinesTable[k][j];
		}
		y[j] *= 2.0 / n;
	}
}

double testCosineTransform (long n) {
	try {
		autoNUMvector<double> x (1, n);
		autoNUMvector<double> y (1, n);
		autoNUMvector<double> x2 (1, n);
		autoNUMmatrix<double> cosinesTable (NUMcosinesTable (n), 1, 1);
		for (long i = 1 ; i <= n; i++) {
			x[i] = NUMrandomUniform (0, 70);
		}
		NUMcosineTransform (x.peek(), y.peek(), n, cosinesTable.peek());
		NUMinverseCosineTransform (y.peek(), x2.peek(), n, cosinesTable.peek());
		double delta = 0;
		for (long i =1 ; i <= n; i++) {
			double dif = x[i] - x2[i];
			delta += dif * dif;
		}
		delta = sqrt (delta);
		return delta;
	} catch (MelderError) {
		Melder_throw ("Test cosine transform error");
	}
}

MFCC MelFilter_to_MFCC (MelFilter me, long numberOfCoefficients) {
	try {
		autoNUMmatrix<double> cosinesTable (NUMcosinesTable (my ny), 1, 1);
		autoNUMvector<double> x (1, my ny);
		autoNUMvector<double> y (1, my ny);
		
		double fmax_mel = my y1 + (my ny - 1) * my dy;
		numberOfCoefficients = numberOfCoefficients > my ny - 1 ? my ny - 1 : numberOfCoefficients;
		Melder_assert (numberOfCoefficients > 0);
		// 20130220 new interpretation of maximumNumberOfCoefficients necessary for inverse transform 
		autoMFCC thee = MFCC_create (my xmin, my xmax, my nx, my dx, my x1, my ny - 1, my ymin, my ymax);
		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & thy frame[frame];
			for (long i = 1; i <= my ny; i++) {
				x[i] = my z[i][frame];
			}
			NUMcosineTransform (x.peek(), y.peek(), my ny, cosinesTable.peek());
			CC_Frame_init (cf, numberOfCoefficients);
			for (long i = 1; i <= numberOfCoefficients; i++) {
				cf -> c[i] = y[i + 1];
			}
			cf -> c0 = y[1];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MFCC created.");
	}
}

MelFilter MFCC_to_MelFilter (MFCC me, long first, long last) {
	try {
		long nf = my maximumNumberOfCoefficients + 1;
		autoNUMmatrix<double> cosinesTable (NUMcosinesTable (nf), 1, 1);
		autoNUMvector<double> x (1, nf);
		autoNUMvector<double> y (1, nf);

		if (first >= last) {
			first = 0; last = nf - 1;
		}

		if (first < 0 || last > nf - 1) {
			Melder_throw ("MFCC_to_MelFilter: coefficients must be in interval [0,", my maximumNumberOfCoefficients, "].");
		}
		double df = (my fmax - my fmin) / (nf + 1);
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, my fmin, my fmax, nf, df, df);

		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & my frame[frame];
			long iend = MIN (last, cf -> numberOfCoefficients);
			x[1] = first == 0 ? cf -> c0 : 0;
			for (long i = 1; i <= my maximumNumberOfCoefficients; i++) {
				x[i + 1] = i < first || i > iend ? 0 : cf -> c[i];
			}
			NUMinverseCosineTransform (x.peek(), y.peek(), nf, cosinesTable.peek());
			for (long i = 1; i <= nf; i++) {
				thy z[i][frame] = y[i];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MelFilter created.");
	}
}


MelFilter MFCC_to_MelFilter2 (MFCC me, long first_cc, long last_cc, double f1_mel, double df_mel) {
	try {
		int use_c0 = 0;
		long nf = ((my fmax - my fmin) / df_mel + 0.5);
		double fmin = MAX (f1_mel - df_mel, 0), fmax = f1_mel + (nf + 1) * df_mel;

		if (nf < 1) {
			Melder_throw ("MFCC_to_MelFilter: the position of the first filter, the distance between the filters, "
			"and, the maximum do not result in a positive number of filters.");
		}

		// Default values

		if (first_cc == 0) {
			first_cc = 1;
			use_c0 = 1;
		}
		if (last_cc == 0) {
			last_cc = my maximumNumberOfCoefficients;
		}

		// Be strict

		if (last_cc < first_cc || first_cc < 1 || last_cc > my maximumNumberOfCoefficients) {
			Melder_throw ("MFCC_to_MelFilter: coefficients must be in interval [1,", my maximumNumberOfCoefficients, "].");
		}
		autoNUMmatrix<double> dct (NUMcosinesTable (first_cc, last_cc, nf), first_cc, 1); // TODO ??
		//if ((dct = NUMcosinesTable (first_cc, last_cc, nf)) == NULL) return NULL;

		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, fmin, fmax, nf, df_mel, f1_mel);

		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & my frame[frame];
			long ie = MIN (last_cc, cf -> numberOfCoefficients);
			for (long j = 1; j <= nf; j++) {
				double t = 0;
				for (long i = first_cc; i <= ie; i++) {
					t += cf -> c[i] * dct[i][j];
				}

				// The inverse CT has a factor 1/N

				t /= nf;
				if (use_c0) {
					t +=  cf -> c0;
				}
				thy z[j][frame] = t;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MelFilter created.");
	}
}

/* Sound_and_FilterBank.cpp */

// prototypes
Sound FilterBank_as_Sound (FilterBank me);

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
static double gaussian_window_squared_correction (long n) {
	double e12 = exp (-12), denum = (e12 - 1) * (e12 - 1) * 24 * (n - 1);
	double sqrt3 = sqrt (3), sqrt2 = sqrt (2), sqrtpi = sqrt (NUMpi);
	double arg1 = 2 * sqrt3 * (n - 1) / (n + 1), arg2 = arg1 * sqrt2;
	double p2 = sqrtpi * sqrt3 * sqrt2 * (1 - NUMerfcc (arg2)) * (n + 1);
	double p1 = 4 * sqrtpi * sqrt3 * e12 * (1 - NUMerfcc (arg1)) * (n + 1);

	return (p2 - p1 + 24 * (n - 1) * e12 * e12) / denum;
}

static Matrix Sound_to_spectralpower (Sound me) {
	try {
		autoSpectrum s = Sound_to_Spectrum (me, TRUE);
		autoMatrix thee = Matrix_create (s -> xmin, s -> xmax, s -> nx, s -> dx, s -> x1, 1, 1, 1, 1, 1);
		double scale = 2.0 * s -> dx / (my xmax - my xmin);

		// factor '2' because of positive and negative frequencies
		// s -> dx : width of frequency bin
		// my xmax - my xmin : duration of sound

		double *z = thy z[1], *re = s -> z[1], *im = s -> z[2];
		for (long i = 1; i <= s -> nx; i++) {
			z[i] = scale * (re[i] * re[i] + im[i] * im [i]);
		}

		// Frequency bins at 0 Hz and nyquist don't count for two.

		z[1] *= 0.5;
		z[s -> nx] *= 0.5;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Matrix with spectral power created.");
	}
}

static int Sound_into_BarkFilter_frame (Sound me, BarkFilter thee, long frame) {
	autoMatrix pv = Sound_to_spectralpower (me);
	long nf = pv -> nx;
	autoNUMvector<double> z (1, nf);

	for (long j = 1; j <= nf; j++) {
		z[j] = HZTOBARK (pv -> x1 + (j - 1) * pv -> dx);
	}

	for (long i = 1; i <= thy ny; i++) {
		double p = 0;
		double z0 = thy y1 + (i - 1) * thy dy;
		double *pow = pv -> z[1]; // TODO ??
		for (long j = 1; j <= nf; j++) {
			// Sekey & Hanson filter is defined in the power domain.
			// We therefore multiply the power with a (and not a^2).
			// integral (F(z),z=0..25) = 1.58/9

			double a = NUMsekeyhansonfilter_amplitude (z0, z[j]);
			p += a * pow[j] ;
		}
		thy z[i][frame] = p;
	}
	return 1;
}

BarkFilter Sound_to_BarkFilter (Sound me, double analysisWidth, double dt, double f1_bark, double fmax_bark, double df_bark) {
	try {
		double t1, nyquist = 0.5 / my dx, samplingFrequency = 2 * nyquist;
		double windowDuration = 2 * analysisWidth; /* gaussian window */
		double zmax = NUMhertzToBark2 (nyquist);
		double fmin_bark = 0;
		long nt, frameErrorCount = 0;

		// Check defaults.

		if (f1_bark <= 0) {
			f1_bark = 1;
		}
		if (fmax_bark <= 0) {
			fmax_bark = zmax;
		}
		if (df_bark <= 0) {
			df_bark = 1;
		}

		fmax_bark = MIN (fmax_bark, zmax);
		long nf = floor ( (fmax_bark - f1_bark) / df_bark + 0.5);
		if (nf <= 0) {
			Melder_throw ("The combination of filter parameters is not valid.");
		}

		Sampled_shortTermAnalysis (me, windowDuration, dt, & nt, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoBarkFilter thee = BarkFilter_create (my xmin, my xmax, nt, dt, t1,
		                      fmin_bark, fmax_bark, nf, df_bark, f1_bark);

		autoMelderProgress progess (L"BarkFilter analysis");

		for (long i = 1; i <= nt; i++) {
			double t = Sampled_indexToX (thee.peek(), i);

			Sound_into_Sound (me, sframe.peek(), t - windowDuration / 2);

			Sounds_multiply (sframe.peek(), window.peek());

			if (! Sound_into_BarkFilter_frame (sframe.peek(), thee.peek(), i)) {
				frameErrorCount++;
			}

			if ( (i % 10) == 1) {
				Melder_progress ( (double) i / nt,  L"BarkFilter analysis: frame ",
					Melder_integer (i), L" from ", Melder_integer (nt), L".");
			}
		}

		if (frameErrorCount > 0) {
			Melder_warning (L"Analysis results of ", Melder_integer (frameErrorCount), L" frame(s) out of ",
				Melder_integer (nt), L" will be suspect.");
		}

		double ref = FilterBank_DBREF * gaussian_window_squared_correction (window -> nx);

		NUMdmatrix_to_dBs (thy z, 1, thy ny, 1, thy nx, ref, FilterBank_DBFAC, FilterBank_DBFLOOR);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no BarkFilter created.");
	}
}

static int Sound_into_MelFilter_frame (Sound me, MelFilter thee, long frame) {
	autoMatrix pv = Sound_to_spectralpower (me);

	double z1 = pv -> x1;
	double dz = pv -> dx;
	long nf = pv -> nx;
	double df = thy dy;
	for (long i = 1; i <= thy ny; i++) {
		double p = 0;
		double fc_mel = thy y1 + (i - 1) * df;
		double fc_hz = MELTOHZ (fc_mel);
		double fl_hz = MELTOHZ (fc_mel - df);
		double fh_hz =  MELTOHZ (fc_mel + df);
		double *pow = pv -> z[1];
		for (long j = 1; j <= nf; j++) {
			// Bin with a triangular filter the power (=amplitude-squared)

			double f = z1 + (j - 1) * dz;
			double a = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, f);
			p += a * pow[j];
		}
		thy z[i][frame] = p;
	}
	return 1;
}

MelFilter Sound_to_MelFilter (Sound me, double analysisWidth, double dt, double f1_mel, double fmax_mel, double df_mel) {
	try {
		double t1, samplingFrequency = 1 / my dx, nyquist = 0.5 * samplingFrequency;
		double windowDuration = 2 * analysisWidth; /* gaussian window */
		double fmin_mel = 0;
		double fbottom = HZTOMEL (100.0), fceiling = HZTOMEL (nyquist);
		long nt, frameErrorCount = 0;

		// Check defaults.

		if (fmax_mel <= 0 || fmax_mel > fceiling) {
			fmax_mel = fceiling;
		}
		if (fmax_mel <= f1_mel) {
			f1_mel = fbottom; fmax_mel = fceiling;
		}
		if (f1_mel <= 0) {
			f1_mel = fbottom;
		}
		if (df_mel <= 0) {
			df_mel = 100.0;
		}

		// Determine the number of filters.

		long nf = floor ((fmax_mel - f1_mel) / df_mel + 0.5);
		fmax_mel = f1_mel + nf * df_mel;

		Sampled_shortTermAnalysis (me, windowDuration, dt, &nt, &t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelFilter thee = MelFilter_create (my xmin, my xmax, nt, dt, t1, fmin_mel, fmax_mel, nf, df_mel, f1_mel);

		autoMelderProgress progress (L"MelFilters analysis");

		for (long i = 1; i <= nt; i++) {
			double t = Sampled_indexToX (thee.peek(), i);
			Sound_into_Sound (me, sframe.peek(), t - windowDuration / 2);
			Sounds_multiply (sframe.peek(), window.peek());
			if (! Sound_into_MelFilter_frame (sframe.peek(), thee.peek(), i)) {
				frameErrorCount++;
			}
			if ( (i % 10) == 1) {
				Melder_progress ((double) i / nt, L"Frame ", Melder_integer (i), L" out of ", Melder_integer (nt), L".");
			}
		}

		if (frameErrorCount) {
			Melder_warning (L"Analysis results of ", Melder_integer (frameErrorCount),
			L" frame(s) out of ", Melder_integer (nt), L" will be suspect.");
		}

		// Window correction.

		double ref = FilterBank_DBREF * gaussian_window_squared_correction (window -> nx);

		NUMdmatrix_to_dBs (thy z, 1, thy ny, 1, thy nx, ref, FilterBank_DBFAC, FilterBank_DBFLOOR);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MelFilter created.");
	}
}

/*
	Analog formant filter response :
	H(f) = i f B / (f1^2 - f^2 + i f B)
*/
static int Sound_into_FormantFilter_frame (Sound me, FormantFilter thee, long frame, double bw) {
	Melder_assert (bw > 0);
	autoMatrix pv = Sound_to_spectralpower (me);
	double z1 = pv -> x1;
	double dz = pv -> dx;
	long nf = pv -> nx;

	for (long i = 1; i <= thy ny; i++) {
		double p = 0;
		double fc = thy y1 + (i - 1) * thy dy;
		double *pow = pv -> z[1];
		for (long j = 1; j <= nf; j++) {
			// H(f) = ifB / (fc^2 - f^2 + ifB)
			// H(f)| = fB / sqrt ((fc^2 - f^2)^2 + f^2B^2)
			//|H(f)|^2 = f^2B^2 / ((fc^2 - f^2)^2 + f^2B^2)
			//         = 1 / (((fc^2 - f^2) /fB)^2 + 1)

			double f = z1 + (j - 1) * dz;
			double a = NUMformantfilter_amplitude (fc, bw, f);
			p += a * pow[j];
		}
		thy z[i][frame] = p;
	}
	return 1;
}

FormantFilter Sound_to_FormantFilter (Sound me, double analysisWidth,
                                      double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw,
                                      double minimumPitch, double maximumPitch) {
	try {
		double floor = 80, ceiling = 600;
		if (minimumPitch >= maximumPitch) {
			minimumPitch = floor; maximumPitch = ceiling;
		}
		if (minimumPitch <= 0) {
			minimumPitch = floor;
		}
		if (maximumPitch <= 0) {
			maximumPitch = ceiling;
		}

		autoPitch thee = Sound_to_Pitch (me, dt, minimumPitch, maximumPitch);
		autoFormantFilter ff = Sound_and_Pitch_to_FormantFilter (me, thee.peek(), analysisWidth, dt,
		                       f1_hz, fmax_hz, df_hz, relative_bw);
		return ff.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no FormantFilter created.");
	}
}

FormantFilter Sound_and_Pitch_to_FormantFilter (Sound me, Pitch thee, double analysisWidth, double dt,
        double f1_hz, double fmax_hz, double df_hz, double relative_bw) {
	try {
		double t1, windowDuration = 2 * analysisWidth; /* gaussian window */
		double nyquist = 0.5 / my dx, samplingFrequency = 2 * nyquist, fmin_hz = 0;
		long nt, f0_undefined = 0;

		if (my xmin > thy xmin || my xmax > thy xmax) Melder_throw
			("The domain of the Sound is not included in the domain of the Pitch.");

		double f0_median = Pitch_getQuantile (thee, thy xmin, thy xmax, 0.5, kPitch_unit_HERTZ);

		if (f0_median == NUMundefined || f0_median == 0) {
			f0_median = 100;
			Melder_warning (L"Pitch values undefined. Bandwith fixed to 100 Hz. ");
		}

		if (f1_hz <= 0) {
			f1_hz = 100;
		}
		if (fmax_hz <= 0) {
			fmax_hz = nyquist;
		}
		if (df_hz <= 0) {
			df_hz = f0_median / 2;
		}
		if (relative_bw <= 0) {
			relative_bw = 1.1;
		}

		fmax_hz = MIN (fmax_hz, nyquist);
		long nf = floor ( (fmax_hz - f1_hz) / df_hz + 0.5);

		Sampled_shortTermAnalysis (me, windowDuration, dt, &nt, &t1);
		autoFormantFilter him = FormantFilter_create (my xmin, my xmax, nt, dt, t1,
		                        fmin_hz, fmax_hz, nf, df_hz, f1_hz);

		// Temporary objects

		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelderProgress progress (L"Sound & Pitch: To FormantFilter");
		for (long i = 1; i <= nt; i++) {
			double t = Sampled_indexToX (him.peek(), i);
			double b, f0 = Pitch_getValueAtTime (thee, t, kPitch_unit_HERTZ, 0);

			if (f0 == NUMundefined || f0 == 0) {
				f0_undefined++; f0 = f0_median;
			}
			b = relative_bw * f0;
			Sound_into_Sound (me, sframe.peek(), t - windowDuration / 2);
			Sounds_multiply (sframe.peek(), window.peek());

			Sound_into_FormantFilter_frame (sframe.peek(), him.peek(), i, b);

			if ( (i % 10) == 1) {
				Melder_progress ( (double) i / nt, L"Frame ", Melder_integer (i), L" out of ",
				                   Melder_integer (nt), L".");
			}
		}

		double ref = FilterBank_DBREF * gaussian_window_squared_correction (window -> nx);
		NUMdmatrix_to_dBs (his z, 1, his ny, 1, his nx, ref, FilterBank_DBFAC, FilterBank_DBFLOOR);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("FormantFilter not created from Pitch & FormantFilter.");
	}
}

Sound FilterBank_as_Sound (FilterBank me) {
	try {
		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++)
				thy z[i][j] = my z[i][j];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Sound created.");
	}
}

Sound FilterBanks_crossCorrelate (FilterBank me, FilterBank thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound cc = Sounds_crossCorrelate ((Sound) me, (Sound) thee, scaling, signalOutsideTimeDomain);
		return cc.transfer();
	} catch (MelderError) {
		Melder_throw (me, " and ", thee, " not cross-correlated.");
	}
}

Sound FilterBanks_convolve (FilterBank me, FilterBank thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound cc = Sounds_convolve ((Sound) me, (Sound) thee, scaling, signalOutsideTimeDomain);
		return cc.transfer();
	} catch (MelderError) {
		Melder_throw (me, " and ", thee, " not convolved.");
	}
}

#undef MAX
#undef MIN

/* End of file Filterbank.cpp */
