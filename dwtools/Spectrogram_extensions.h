#ifndef _Spectrogram_extensions_h_
#define _Spectrogram_extensions_h_
/* Spectrogram_extensions.h
 *
 * Copyright (C) 2014-2018 David Weenink
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
 djmw 20140913
*/

#include "Intensity.h"
#include "MFCC.h"
#include "PCA.h"
#include "Spectrogram.h"
#include "Spectrum.h"
#include "TableOfReal.h"
#include "NUM2.h"

#define HZTOBARK(x) NUMhertzToBark2(x)
#define HZTOMEL(x)	NUMhertzToMel2(x)
#define BARKTOHZ(x) NUMbarkToHertz2(x)
#define MELTOHZ(x)	NUMmelToHertz2(x)

#define BARKTOMEL(x) HZTOMEL(BARKTOHZ(x))
#define MELTOBARK(x) HZTOBARK(MELTOHZ(x))

#define BandFilterSpectrogram_DBREF 4e-10
#define BandFilterSpectrogram_DBFAC 10
#define BandFilterSpectrogram_DBFLOOR -100

#define BandFilterSpectrogram_HERTZ 1
#define BandFilterSpectrogram_BARK  2
#define BandFilterSpectrogram_MEL   3

Thing_define (BandFilterSpectrogram, Matrix) {
	void v_info ()
		override;
	double v_getValueAtSample (integer icol, integer irow, int units)
		override;

	virtual double v_frequencyToHertz (double f) { return f; }
	virtual double v_hertzToFrequency (double hertz) { return hertz; }
	virtual conststring32 v_getFrequencyUnit () { return U"Hz"; }
};

Thing_define (BarkSpectrogram, BandFilterSpectrogram) {
	void v_info ()
		override;
	double v_frequencyToHertz (double f)
		override { return NUMbarkToHertz2 (f); }
	double v_hertzToFrequency (double hertz)
		override { return NUMhertzToBark2 (hertz); }
	conststring32 v_getFrequencyUnit ()
		override { return U"bark"; }
};

Thing_define (MelSpectrogram, BandFilterSpectrogram) {
	void v_info ()
		override;
	double v_frequencyToHertz (double f)
		override { return NUMmelToHertz2 (f); }
	double v_hertzToFrequency (double hertz)
		override { return NUMhertzToMel2 (hertz); }
	conststring32 v_getFrequencyUnit ()
		override { return U"mel"; }
};

/*
Interpretation:
	xmin, xmax, x1, dx, nx like Sampled.
	ymin, ymax lowest and highest frequencies in Barks / Mel.
    y1 mid of first filter (bark/mel).
    dy distance between filters (bark/mel).
    ny the number of filters.
 */

double BandFilterSpectrogram_getFrequencyInHertz (BandFilterSpectrogram me, double f);

void BandFilterSpectrogram_equalizeIntensities (BandFilterSpectrogram me, double intensity_db);

autoMatrix BandFilterSpectrogram_to_Matrix (BandFilterSpectrogram me, int to_dB);

autoIntensity BandFilterSpectrogram_to_Intensity (BandFilterSpectrogram me);

void BandFilterSpectrogram_drawFrequencyScale (BandFilterSpectrogram me, Graphics g,
	double xmin, double xmax, double ymin, double ymax, bool garnish);

void BandFilterSpectrogram_drawTimeSlice (BandFilterSpectrogram me, Graphics g, double t, double fmin, double fmax,
	double min, double max, conststring32 xlabel, bool garnish);

void BarkSpectrogram_drawSekeyHansonFilterFunctions (BarkSpectrogram me, Graphics g, bool xIsHertz, integer fromFilter, integer toFilter,
	double zmin, double zmax, bool yscale_dB, double ymin, double ymax, bool garnish);

void BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (BandFilterSpectrogram me, Graphics g,
	double time, double fmin, double fmax, double dBmin, double dBmax, bool garnish);

void BandFilterSpectrogram_paintImage (BandFilterSpectrogram me, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, bool garnish);

autoBarkSpectrogram BarkSpectrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double fmin, double fmax, integer nf, double df, double f1);

autoBarkSpectrogram Matrix_to_BarkSpectrogram (Matrix me);

/*
Interpretation:
	xmin, xmax, x1, dx, nx like Sampled.
	ymin, ymax lowest and highest frequencies in mels.
    y1 mid of first filter (mels).
    dy distance between filters (mel).
    ny the number of filters.
 */
autoMelSpectrogram MelSpectrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double fmin, double fmax, integer nf, double df, double f1);

autoMelSpectrogram Matrix_to_MelSpectrogram (Matrix me);

void MelSpectrogram_drawTriangularFilterFunctions (MelSpectrogram me, Graphics g, bool xIsHertz, integer fromFilter, integer toFilter,
	double zmin, double zmax, bool yscale_dB, double ymin, double ymax, bool garnish);

autoMFCC MelSpectrogram_to_MFCC (MelSpectrogram me, integer numberOfCoefficients);

autoMelSpectrogram MFCC_to_MelSpectrogram (MFCC me, integer first, integer last, bool c0);

void BandFilterSpectrogram_and_PCA_drawComponent (BandFilterSpectrogram me, PCA thee, Graphics g, integer component, double dblevel,
	double frequencyOffset, double scale, double tmin, double tmax, double fmin, double fmax);

autoMatrix Spectrogram_to_Matrix_dB (Spectrogram me, double reference, double scaleFactor, double floor_dB);
// dbs = scaleFactor * log10 (value/reference);
// if (dbs < floor_db) { dbs = floor_dB }

void BandFilterSpectrogram_into_CC (BandFilterSpectrogram me, CC thee, integer numberOfCoefficients);

void CC_into_BandFilterSpectrogram (CC me, BandFilterSpectrogram thee, integer first, integer last, bool use_c0);

#endif /* _Spectrogram_extensions_h_ */
