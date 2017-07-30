/* Spectrogram_extensions.cpp
 *
 * Copyright (C) 2014-2017 David Weenink
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

#include "Eigen_and_Matrix.h"
#include "Spectrogram_extensions.h"
#include "Matrix_extensions.h"
#include "NUM2.h"

Thing_implement (BandFilterSpectrogram, Matrix, 2);

void structBandFilterSpectrogram :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of time slices (frames): ", nx);
	MelderInfo_writeLine (U"   Time step (frame distance): ", dx, U" seconds");
	MelderInfo_writeLine (U"   First time slice (frame centre) at: ", x1, U" seconds");
}

void structBarkSpectrogram :: v_info () {
	structBandFilterSpectrogram :: v_info ();
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", ymin, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"   Highest frequency: ", ymax, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"   Total bandwidth: ", ymax - ymin, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"Frequency sampling:");
	MelderInfo_writeLine (U"   Number of frequency bands (bins): ", ny);
	MelderInfo_writeLine (U"   Frequency step (bin width): ", dy, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"   First frequency band around (bin centre at): ", y1, U" ", v_getFrequencyUnit ());
}

void structMelSpectrogram :: v_info () {
	structBandFilterSpectrogram :: v_info ();
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", ymin, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"   Highest frequency: ", ymax, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"   Total bandwidth: ", ymax - ymin, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"Frequency sampling:");
	MelderInfo_writeLine (U"   Number of frequency bands (bins): ", ny);
	MelderInfo_writeLine (U"   Frequency step (bin width): ", dy, U" ", v_getFrequencyUnit ());
	MelderInfo_writeLine (U"   First frequency band around (bin centre at): ", y1, U" ", v_getFrequencyUnit ());
}

// Preconditions: 1 <= iframe <= nx; 1 <= irow <= ny
double structBandFilterSpectrogram :: v_getValueAtSample (long iframe, long ifreq, int units) {
	double val = undefined;
	if (units == 0) {
		val = z [ifreq] [iframe];
	} else {
		val = -300.0; // minimum dB value
		if (z [ifreq] [iframe] > 0.0) {
			val = 10.0 * log10 (z [ifreq] [iframe] / 4e-10); // power values
		}
	}
	return val;
}

Thing_implement (BarkSpectrogram, BandFilterSpectrogram, 1);

// dbs = scaleFactor * log10 (value/reference);
// if (dbs < floor_db) { dbs = floor_dB }
autoMatrix Spectrogram_to_Matrix_dB (Spectrogram me, double reference, double scaleFactor, double floor_dB) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				double val = floor_dB;
				if (my z[i][j] > 0) {
					val = scaleFactor * log10 (my z[i][j] / reference);
				} else if (my z[i][j] < 0) {
					Melder_throw (U"Negative power in Spectrogram.");
				}
				if (val < floor_dB) {
					val = floor_dB;
				}
				thy z[i][j] = val;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Matrix with dB values not created.");
	}
}

static double **NUMcosinesTable (long  n) {
	autoNUMmatrix<double> costab (1, n, 1, n);
	for (long k = 1; k <= n; k++) {
		for (long j = 1; j <= n; j++) {
			costab[k][j] = cos (NUMpi * (k - 1) * (j - 0.5) / n);
		}
	}
	return costab.transfer();
}
// x[1..n] : input
// y[1..n] : output
static void NUMcosineTransform (double *x, double *y, long n, double **cosinesTable) {
	for (long k = 1; k <= n; k++) {
		y[k] = 0;
		for (long j = 1; j <= n; j++) {
			y[k] += x[j] * cosinesTable[k][j];
		}
	}
}


// x: input
// y: output
static void NUMinverseCosineTransform (double *x, double *y, long n, double **cosinesTable) {
	for (long j = 1; j <= n; j++) {
		y[j] = 0.5 * x[1] * cosinesTable[1][j];
		for (long k = 2; k <= n; k++) {
			y[j] += x[k] * cosinesTable[k][j];
		}
		y[j] *= 2.0 / n;
	}
}

/* Precondition: 1. CC object has been created but individual frames not yet initialized
 *               2. Domains and number of frames conform
 * Steps:
 * 1. transform power-spectra to dB-spectra
 * 2. cosine transform of dB-spectrum
*/
void BandFilterSpectrogram_into_CC (BandFilterSpectrogram me, CC thee, long numberOfCoefficients) {
	autoNUMmatrix<double> cosinesTable (NUMcosinesTable (my ny), 1, 1);
	autoNUMvector<double> x (1, my ny);
	autoNUMvector<double> y (1, my ny);
	numberOfCoefficients = numberOfCoefficients > my ny - 1 ? my ny - 1 : numberOfCoefficients;
	Melder_assert (numberOfCoefficients > 0);
	// 20130220 new interpretation of maximumNumberOfCoefficients: necessary for the inverse transform 
	for (long frame = 1; frame <= my nx; frame++) {
		CC_Frame ccframe = (CC_Frame) & thy frame[frame];
		for (long i = 1; i <= my ny; i++) {
			x[i] = my v_getValueAtSample (frame, i, 1); // z[i][frame];
		}
		NUMcosineTransform (x.peek(), y.peek(), my ny, cosinesTable.peek());
		CC_Frame_init (ccframe, numberOfCoefficients);
		for (long i = 1; i <= numberOfCoefficients; i++) {
			ccframe -> c[i] = y[i + 1];
		}
		ccframe -> c0 = y[1];
	}
}

// Preconditions: Domains and number of frames conform
//                0 <= first <= last <= my ny-1
void CC_into_BandFilterSpectrogram (CC me, BandFilterSpectrogram thee, long first, long last, bool use_c0) {
	long nf = my maximumNumberOfCoefficients + 1;
	autoNUMmatrix<double> cosinesTable (NUMcosinesTable (nf), 1, 1);
	autoNUMvector<double> x (1, nf);
	autoNUMvector<double> y (1, nf);
	for (long frame = 1; frame <= my nx; frame++) {
		CC_Frame ccframe = (CC_Frame) & my frame[frame];
		long iend = last < ccframe -> numberOfCoefficients ? last : ccframe -> numberOfCoefficients;
		x[1] = use_c0 ? ccframe -> c0 : 0;
		for (long i = 1; i <= my maximumNumberOfCoefficients; i++) {
			x[i + 1] = i < first || i > iend ? 0 : ccframe -> c[i];
		}
		NUMinverseCosineTransform (x.peek(), y.peek(), nf, cosinesTable.peek());
		for (long i = 1; i <= nf; i++) {
			thy z[i][frame] = BandFilterSpectrogram_DBREF * pow (10, y[i] / BandFilterSpectrogram_DBFAC);
		}
	}
}

autoMelSpectrogram MFCC_to_MelSpectrogram (MFCC me, long first, long last, bool c0) {
	try {
		if (first == 0 && last == 0) { // defaults
			first = 1; last = my maximumNumberOfCoefficients;
		}
		if (first < 1) {
			first = 1;
		}
		if (last > my maximumNumberOfCoefficients) {
			last = my maximumNumberOfCoefficients;
		}
		if (first > last) {
			first = 1; last = my maximumNumberOfCoefficients;
		}
		double df = (my fmax - my fmin) / (my maximumNumberOfCoefficients + 1 + 1);
		autoMelSpectrogram thee = MelSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my fmin, my fmax, my maximumNumberOfCoefficients + 1, df, df);
		CC_into_BandFilterSpectrogram (me, thee.get(), first, last, c0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"MelSpectrogram not created.");
	}
}

autoMFCC MelSpectrogram_to_MFCC (MelSpectrogram me, long numberOfCoefficients) {
	try {
		if (numberOfCoefficients <= 0) {
			numberOfCoefficients = my ny - 1;
		}
		numberOfCoefficients = numberOfCoefficients > my ny - 1 ? my ny - 1 : numberOfCoefficients;
		// 20130220 new interpretation of maximumNumberOfCoefficients necessary for inverse transform 
		autoMFCC thee = MFCC_create (my xmin, my xmax, my nx, my dx, my x1, my ny - 1, my ymin, my ymax);
		BandFilterSpectrogram_into_CC (me, thee.get(), numberOfCoefficients);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": MFCC not created.");
	}
}

autoBarkSpectrogram BarkSpectrogram_create (double tmin, double tmax, long nt, double dt, double t1, double fmin, double fmax, long nf, double df, double f1) {
	try {
		autoBarkSpectrogram me = Thing_new (BarkSpectrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"BarkSpectrogram not created.");
	}
}

double BandFilterSpectrogram_getFrequencyInHertz (BandFilterSpectrogram me, double f) {
	return my v_frequencyToHertz (f);
}

// xmin, xmax in hz versus bark/mel or lin
void BandFilterSpectrogram_drawFrequencyScale (BandFilterSpectrogram me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish) {
	if (xmin < 0 || xmax < 0 || ymin < 0 || ymax < 0) {
		Melder_warning (U"Frequencies must be >= 0.");
		return;
	}

	// scale is in hertz
	if (xmin >= xmax) { // autoscaling
		xmin = 0;
		xmax = my v_frequencyToHertz (my ymax);
	}

	if (ymin >= ymax) { // autoscaling
		ymin = my ymin;
		ymax = my ymax;
	}

	long n = 2000;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	double dx = (xmax - xmin) / (n - 1);
	double x1 = xmin, y1 = my v_hertzToFrequency (x1);
	for (long i = 2; i <= n;  i++) {
		double x2 = x1 + dx, y2 = my v_hertzToFrequency (x2);
		if (isdefined (y1) && isdefined (y2)) {
			double xo1, yo1, xo2, yo2;
			if (NUMclipLineWithinRectangle (x1, y1, x2, y2, xmin, ymin, xmax, ymax, &xo1, &yo1, &xo2, &yo2)) {
				Graphics_line (g, xo1, yo1, xo2, yo2);
			}
		}
		x1 = x2; y1 = y2;
	}
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, Melder_cat (U"Frequency (", my v_getFrequencyUnit (), U")"));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Frequency (Hz)");
	}
}

void BandFilterSpectrogram_paintImage (BandFilterSpectrogram me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, int garnish) {
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax; 
	}
	if (ymax <= ymin) {
		ymin = my ymin; ymax = my ymax;
	}
	long ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx, &ixmin, &ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy, &iymin, &iymax);
	autoMatrix thee = Spectrogram_to_Matrix_dB ((Spectrogram) me, 4e-10, 10, -100);
	if (maximum <= minimum) {
		(void) Matrix_getWindowExtrema (thee.get(), ixmin, ixmax, iymin, iymax, &minimum, &maximum);
	}
	if (maximum <= minimum) { 
		minimum -= 1.0; maximum += 1.0;
	}
	if (xmin >= xmax || ymin >= ymax) {
		return;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_image (g, thy z,
			ixmin, ixmax, Sampled_indexToX   (thee.get(), ixmin - 0.5), Sampled_indexToX   (thee.get(), ixmax + 0.5),
			iymin, iymax, SampledXY_indexToY (thee.get(), iymin - 0.5), SampledXY_indexToY (thee.get(), iymax + 0.5),
			minimum, maximum);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, Melder_cat (U"Frequency (", my v_getFrequencyUnit (), U")"));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Time (s)");
	}
}

void BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (BandFilterSpectrogram me, Graphics g, double time, double fmin, double fmax, double dBmin, double dBmax, int garnish) {
	if (time < my xmin || time > my xmax) {
		return;
	}
	if (fmin == 0 && fmax == 0) { // autoscaling
		fmin = my ymin; fmax = my ymax;
	}
	if (fmax <= fmin) {
		fmin = my ymin; fmax = my ymax;
	}
	long icol = Matrix_xToNearestColumn (me, time);
	icol = icol < 1 ? 1 : (icol > my nx ? my nx : icol);
	autoNUMvector<double> spectrum (1, my ny);
	for (long i = 1; i <= my ny; i++) {
		spectrum[i] = my v_getValueAtSample (icol, i, 1); // dB's
	}
	long iymin, iymax;
	if (Matrix_getWindowSamplesY (me, fmin, fmax, &iymin, &iymax) < 2) { // too few values
		return;
	}
	if (dBmin == dBmax) { // autoscaling
		dBmin = spectrum[iymin]; dBmax = dBmin;
		for (long i = iymin + 1; i <= iymax; i++) {
			if (spectrum[i] < dBmin) {
				dBmin = spectrum[i];
			} else if (spectrum[i] > dBmax) {
				dBmax = spectrum[i];
			}
		}
		if (dBmin == dBmax) { 
			dBmin -= 1; dBmax += 1;
		}
	}
	Graphics_setWindow (g, fmin, fmax, dBmin, dBmax);
	Graphics_setInner (g);

	double x1 = my y1 + (iymin -1) * my dy, y1 = spectrum[iymin];
	for (long i = iymin + 1; i <= iymax - 1; i++) {
		double x2 = my y1 + (i -1) * my dy, y2 = spectrum[i];
		double xo1, yo1, xo2, yo2;
		if (NUMclipLineWithinRectangle (x1, y1, x2, y2, fmin, dBmin, fmax, dBmax, &xo1, &yo1, &xo2, &yo2)) {
			Graphics_line (g, xo1, yo1, xo2, yo2);
		}
		x1 = x2; y1 = y2;
	}
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Power (dB)");
		Graphics_textBottom (g, true, Melder_cat (U"Frequency (", my v_getFrequencyUnit (), U")"));
	}
}

void BarkSpectrogram_drawSekeyHansonFilterFunctions (BarkSpectrogram me, Graphics g, bool xIsHertz, int fromFilter, int toFilter, double zmin, double zmax, bool yscale_dB, double ymin, double ymax, int garnish) {
	double xmin = zmin, xmax = zmax;
	if (zmin >= zmax) {
		zmin = my ymin;
		zmax = my ymax;
		xmin = ( xIsHertz ? my v_frequencyToHertz (zmin) : zmin );
		xmax = ( xIsHertz ? my v_frequencyToHertz (zmax) : zmax );
	}
	if (xIsHertz) {
		zmin = my v_hertzToFrequency (xmin);
		zmax = my v_hertzToFrequency (xmax);
	}
	if (ymin >= ymax) {
		ymin = ( yscale_dB ? -60.0 : 0.0 );
		ymax = ( yscale_dB ? 0.0 : 1.0 );
	}
	fromFilter = fromFilter <= 0 ? 1 : fromFilter;
	toFilter = ( toFilter <= 0 || toFilter > my ny ? my ny : toFilter );
	if (fromFilter > toFilter) {
		fromFilter = 1;
		toFilter = my ny;
	}
	long n = xIsHertz ? 1000 : 500;
	autoNUMvector<double> xz (1, n), xhz (1, n), y (1, n);

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	double dz = (zmax - zmin) / (n - 1);
	for (long iz = 1; iz <= n; iz ++) {
		double f = zmin + (iz - 1) * dz;
		xz [iz] = f;
		xhz [iz] = my v_frequencyToHertz (f); // just in case we need the linear scale
	}
	for (long ifilter = fromFilter; ifilter <= toFilter; ifilter ++) {
		double zMid = Matrix_rowToY (me, ifilter);
		for (long iz = 1; iz <= n; iz ++) {
			double z = xz[iz] - (zMid - 0.215);
			double amp = 7.0 - 7.5 * z - 17.5 * sqrt (0.196 + z * z);
			y [iz] = ( yscale_dB ? amp : pow (10.0, amp / 10.0) );
		}
		// the drawing
		double x1 = ( xIsHertz ? xhz [1] : xz [1] ), y1 = y [1];
		for (long iz = 2; iz <= n; iz ++) {
			double x2 = ( xIsHertz ? xhz [iz] : xz [iz] ), y2 = y [iz];
			if (isdefined (x1) && isdefined (x2)) {
				double xo1, yo1, xo2, yo2;
				if (NUMclipLineWithinRectangle (x1, y1, x2, y2, xmin, ymin, xmax, ymax, & xo1, & yo1, & xo2, & yo2)) {
					Graphics_line (g, xo1, yo1, xo2, yo2);
				}
			}
			x1 = x2;
			y1 = y2;
		}
	}	
	Graphics_unsetInner (g);

	if (garnish) {
		double distance = yscale_dB ? 10.0 : 0.5;
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, distance, true, true, false);
		Graphics_textLeft (g, true, yscale_dB ? U"Amplitude (dB)" : U"Amplitude");
		Graphics_textBottom (g, true, Melder_cat (U"Frequency (", xIsHertz ? U"Hz" : my v_getFrequencyUnit (), U")"));
	}
}

Thing_implement (MelSpectrogram, BandFilterSpectrogram, 2);

autoMelSpectrogram MelSpectrogram_create (double tmin, double tmax, long nt, double dt, double t1, double fmin, double fmax, long nf, double df, double f1) {
	try {
		autoMelSpectrogram me = Thing_new (MelSpectrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MelSpectrogram not created.");
	}
}

void BandFilterSpectrogram_drawTimeSlice (BandFilterSpectrogram me, Graphics g, double t, double fmin, double fmax, double min, double max, const char32 *xlabel, int garnish) {
	Matrix_drawSliceY (me, g, t, fmin, fmax, min, max);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		if (xlabel) {
			Graphics_textBottom (g, false, xlabel);
		}
	}
}

void MelSpectrogram_drawTriangularFilterFunctions (MelSpectrogram me, Graphics g, bool xIsHertz, int fromFilter, int toFilter, double zmin, double zmax, bool yscale_dB, double ymin, double ymax, int garnish) {
	double xmin = zmin, xmax = zmax;
	if (zmin >= zmax) {
		zmin = my ymin; zmax = my ymax; // mel
		xmin = xIsHertz ? my v_frequencyToHertz (zmin) : zmin;
		xmax = xIsHertz ? my v_frequencyToHertz (zmax) : zmax;
	}
	if (xIsHertz) {
		zmin = my v_hertzToFrequency (xmin); zmax = my v_hertzToFrequency (xmax);
	}

	if (ymin >= ymax) {
		ymin = yscale_dB ? -60.0 : 0.0;
		ymax = yscale_dB ? 0.0 : 1.0;
	}
	fromFilter = fromFilter <= 0 ? 1 : fromFilter;
	toFilter = toFilter <= 0 || toFilter > my ny ? my ny : toFilter;
	if (fromFilter > toFilter) {
		fromFilter = 1; toFilter = my ny;
	}
	
	long n = xIsHertz ? 1000 : 500;
	autoNUMvector<double> xz (1, n), xhz (1,n), y (1, n);

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	
	double dz = (zmax - zmin) / (n - 1);
	for (long iz = 1; iz <= n; iz++) {
		double f = zmin + (iz - 1) * dz;
		xz[iz] = f;
		xhz[iz] = my v_frequencyToHertz (f); // just in case we need the linear scale
	}
	
	for (long ifilter = fromFilter; ifilter <= toFilter; ifilter++) {
		double zc = Matrix_rowToY (me, ifilter), zl = zc - my dy, zh = zc + my dy;
		double xo1, yo1, xo2, yo2;
		if (yscale_dB) {
			for (long iz = 1; iz <= n; iz++) {
				double z = xz[iz];
				double amp = NUMtriangularfilter_amplitude (zl, zc, zh, z);
				y[iz] = yscale_dB ? (amp > 0.0 ? 20.0 * log10 (amp) : ymin - 10.0) : amp;
			}
			double x1 = xIsHertz ? xhz[1] : xz[1], y1 = y[1];
			if (isdefined (y1)) {
				for (long iz = 1; iz <= n; iz++) {
					double x2 = xIsHertz ? xhz[iz] : xz[iz], y2 = y[iz];
					if (isdefined (y2)) {
						if (NUMclipLineWithinRectangle (x1, y1, x2, y2, xmin, ymin, xmax, ymax, &xo1, &yo1, &xo2, &yo2)) {
							Graphics_line (g, xo1, yo1, xo2, yo2);
						}
					}
					x1 = x2; y1 = y2;
				}
			}
		} else {
			double x1 = xIsHertz ? my v_frequencyToHertz (zl) : zl;
			double x2 = xIsHertz ? my v_frequencyToHertz (zc) : zc;
			if (NUMclipLineWithinRectangle (x1, 0, x2, 1, xmin, ymin, xmax, ymax, &xo1, &yo1, &xo2, &yo2)) {
				Graphics_line (g, xo1, yo1, xo2, yo2);
			}
			double x3 = xIsHertz ? my v_frequencyToHertz (zh) : zh;
			if (NUMclipLineWithinRectangle (x2, 1, x3, 0, xmin, ymin, xmax, ymax, &xo1, &yo1, &xo2, &yo2)) {
				Graphics_line (g, xo1, yo1, xo2, yo2);
			}
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, yscale_dB ? 10.0 : 0.5, true, true, false);
		Graphics_textLeft (g, true, yscale_dB ? U"Amplitude (dB)" : U"Amplitude");
		Graphics_textBottom (g, true, Melder_cat (U"Frequency (", ( xIsHertz ? U"Hz" : my v_getFrequencyUnit () ), U")"));
	}
}

autoMatrix BandFilterSpectrogram_to_Matrix (BandFilterSpectrogram me, int to_dB) {
	try {
		int units = to_dB ? 1 : 0;
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				thy z[i][j] = my v_getValueAtSample (j, i, units);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoBarkSpectrogram Matrix_to_BarkSpectrogram (Matrix me) {
	try {
		autoBarkSpectrogram thee = BarkSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1,
			my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to BarkSpectrogram.");
	}
}

autoMelSpectrogram Matrix_to_MelSpectrogram (Matrix me) {
	try {
		autoMelSpectrogram thee = MelSpectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to MelSpectrogram.");
	}
}

autoIntensity BandFilterSpectrogram_to_Intensity (BandFilterSpectrogram me) {
	try {
		autoIntensity thee = Intensity_create (my xmin, my xmax, my nx, my dx, my x1);
		for (long j = 1; j <= my nx; j++) {
			double p = 0.0;
			for (long i = 1; i <= my ny; i++) {
				p += my z[i][j]; // we add power
			}
			thy z[1][j] = BandFilterSpectrogram_DBFAC * log10 (p / BandFilterSpectrogram_DBREF);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Intensity not created.");
	}
}

void BandFilterSpectrogram_equalizeIntensities (BandFilterSpectrogram me, double intensity_db) {
	for (long j = 1; j <= my nx; j++) {
		double p = 0.0;
		for (long i = 1; i <= my ny; i++) {
			p += my z[i][j];
		}
		double delta_db = intensity_db - BandFilterSpectrogram_DBFAC * log10 (p / BandFilterSpectrogram_DBREF);
		double factor = pow (10, delta_db / 10);
		for (long i = 1; i <= my ny; i++) {
			my z[i][j] *= factor;
		}
	}
}

void BandFilterSpectrogram_and_PCA_drawComponent (BandFilterSpectrogram me, PCA thee, Graphics g, long component, double dblevel, double frequencyOffset, double scale, double tmin, double tmax, double fmin, double fmax) {
	if (component < 1 || component > thy numberOfEigenvalues) {
		Melder_throw (U"Component too large.");
	}

	// Scale Intensity

	autoBandFilterSpectrogram fcopy = Data_copy (me);
	BandFilterSpectrogram_equalizeIntensities (fcopy.get(), dblevel);
	autoMatrix mdb = Spectrogram_to_Matrix_dB ((Spectrogram) fcopy.get(), BandFilterSpectrogram_DBREF, BandFilterSpectrogram_DBFAC, BandFilterSpectrogram_DBFLOOR);
	autoMatrix him = Eigen_and_Matrix_to_Matrix_projectColumns (thee, mdb.get(), component);
	for (long j = 1; j <= my nx; j++) {
		his z[component][j] = frequencyOffset + scale * his z[component][j];
	}
	Matrix_drawRows (him.get(), g, tmin, tmax, component - 0.5, component + 0.5, fmin, fmax);
}

/*
 * MelSpectrograms_to_DTW (MelSpectrogram me, MelSpectrogram thee, dtw-params);
 * comparison on the basis of mfcc
 * BarkSpectrograms_to_DTW (BarkSpectrogram me, BarkSpectrogram thee, dtw-params);
 * comparison on the basis of bfcc!
 */


/* End of file Spectrogram_extensions.cpp */
