/* Cepstrum.cpp
 *
 * Copyright (C) 1994-2016 David Weenink
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
 djmw 20010514
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrum.h"
#include "NUM2.h"
#include "Vector.h"

Thing_implement (Cepstrum, Matrix, 2);
Thing_implement (PowerCepstrum, Cepstrum, 2); // derives from Matrix therefore also version 2

double structCepstrum :: v_getValueAtSample (long isamp, long which, int units) {
	(void) units;
	if (which == 0) {
		return z[1][isamp];
	} else {
		// dB's
		return 20.0 * log10 (fabs(z[1][isamp]) + 1e-30);
	}
	return undefined;
}

double structPowerCepstrum :: v_getValueAtSample (long isamp, long which, int units) {
	(void) units;
	if (which == 0) {
		return z[1][isamp];
	} else {
		// dB's
		return 10.0 * log10 (z[1][isamp] + 1e-30); // always positive
	}
	return undefined;
}

autoCepstrum Cepstrum_create (double qmax, long nq) {
	try {
		autoCepstrum me = Thing_new (Cepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cepstrum not created.");
	}
}

autoPowerCepstrum Cepstrum_downto_PowerCepstrum (Cepstrum me ) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		for (long i = 1; i <= my nx; i++) {
			thy z[1][i] = my z[1][i] * my z[1][i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" not converted.");
	}
}

autoPowerCepstrum PowerCepstrum_create (double qmax, long nq) {
	try {
		autoPowerCepstrum me = Thing_new (PowerCepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PowerCepstrum not created.");
	}
}

static void _Cepstrum_draw (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, int power, int garnish) {
	int autoscaling = minimum >= maximum;

	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin; qmax = my xmax;
	}

	long imin, imax;
	if (! Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax)) {
		return;
	}
	autoNUMvector<double> y (imin, imax);

	for (long i = imin; i <= imax; i++) {
		y[i] = my v_getValueAtSample (i, (power ? 1 : 0), 0);
	}

	if (autoscaling) {
		NUMvector_extrema (y.peek(), imin, imax, & minimum, & maximum);
	} else {
		for (long i = imin; i <= imax; i ++) {
			if (y[i] > maximum) {
				y[i] = maximum;
			} else if (y[i] < minimum) {
				y[i] = minimum;
			}
		}
	}
	Graphics_setWindow (g, qmin, qmax, minimum, maximum);
	Graphics_function (g, y.peek(), imin, imax, Matrix_columnToX (me, imin), Matrix_columnToX (me, imax));

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Quefrency (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, power ? U"Amplitude (dB)" : U"Amplitude");
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void Cepstrum_drawLinear (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, int garnish) {
	_Cepstrum_draw (me, g, qmin, qmax, minimum, maximum, 0, garnish);
}

void PowerCepstrum_draw (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish) {
	_Cepstrum_draw (me, g, qmin, qmax, dBminimum, dBmaximum, 1, garnish);
}

void PowerCepstrum_drawTiltLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, int lineType, int method) {

	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin; qmax = my xmax;
	}

	if (dBminimum >= dBmaximum) { // autoscaling
		long imin, imax;
		if (! Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax)) {
			return;
		}
		long numberOfPoints = imax - imin + 1;
		dBminimum = dBmaximum = my v_getValueAtSample (imin, 1, 0);
		for (long i = 2; i <= numberOfPoints; i++) {
			long isamp = imin + i - 1;
			double y = my v_getValueAtSample (isamp, 1, 0);
			dBmaximum = y > dBmaximum ? y : dBmaximum;
			dBminimum = y < dBminimum ? y : dBminimum;
		}
	}

	Graphics_setWindow (g, qmin, qmax, dBminimum, dBmaximum);
	qend = qend == 0 ? my xmax : qend;
	if (qend <= qstart) {
		qend = my xmax; qstart = my xmin;
	}
	qstart = qstart < my xmin ? my xmin : qstart;
	qend = qend > my xmax ? my xmax : qend;

	double a, intercept;
	PowerCepstrum_fitTiltLine (me, qstart, qend, &a, &intercept, lineType, method);
	/*
	 * Don't draw part outside window
	 */
	double lineWidth =  Graphics_inqLineWidth (g);
	Graphics_setLineWidth (g, 2);
	if (lineType == 2) {
		long n = 500;
		double dq = (qend - qstart) / (n + 1);
		double q1 = qstart;
		if (qstart <= 0) {
			qstart = 0.1 * dq; // some small offset to avoid log(0)
			n--; 
		}
		autoNUMvector<double> y (1, n);
		
		for (long i = 1; i <= n; i++) {
			double q = q1 + (i - 1) * dq;
			y[i] = a * log (q) + intercept;
		}
		Graphics_function (g, y.peek(), 1, n, qstart, qend);
	} else {
		double y1 = a * qstart + intercept, y2 = a * qend + intercept;
		if (y1 >= dBminimum && y2 >= dBminimum) {
			Graphics_line (g, qstart, y1, qend, y2);
		} else if (y1 < dBminimum) {
			qstart = (dBminimum - intercept) / a;
			Graphics_line (g, qstart, dBminimum, qend, y2);
		} else if (y2 < dBminimum) {
			qend = (dBminimum - intercept) / a;
			Graphics_line (g, qstart, y1, qend, dBminimum);
		} else {
			// don't draw anything below lower limit?
		}
	}
	Graphics_setLineWidth (g, lineWidth);
	Graphics_unsetInner (g);
}

/* Fit line y = ax+b (lineType ==1) or y = a log(x) + b (lineType == 2) on interval [qmin,qmax]
 * method == 1 : Least squares fit
 * method == 2 : Theil's partial robust fit
 */
void PowerCepstrum_fitTiltLine (PowerCepstrum me, double qmin, double qmax, double *p_a, double *p_intercept, int lineType, int method) {
	try {
		double a, intercept;
		if (qmax <= qmin) {
			qmin = my xmin; qmax = my xmax;
		}

		long imin, imax;
		if (! Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax)) {
			return;
		}
		imin = (lineType == 2 && imin == 1) ? 2 : imin; // log(0) is undefined!
		long numberOfPoints = imax - imin + 1;
		if (numberOfPoints < 2) {
			Melder_throw (U"Not enough points for fit.");
		}
		autoNUMvector<double> y (1, numberOfPoints);
		autoNUMvector<double> x (1, numberOfPoints);
		for (long i = 1; i <= numberOfPoints; i++) {
			long isamp = imin + i - 1;
			x[i] = my x1 + (isamp - 1) * my dx;
			if (lineType == 2) {
				x[i] = log (x[i]);
			}
			y[i] = my v_getValueAtSample (isamp, 1, 0);
		}
		if (method == 3) { // try local maxima first
			autoNUMvector<double> ym (1, numberOfPoints / 2 + 1);
			autoNUMvector<double> xm (1, numberOfPoints / 2 + 1);
			long numberOfLocalPeaks = 0;
			// forget y[1] if y[2]<y[1] and y[n] if y[n-1]<y[n] !
			for (long i = 2; i <= numberOfPoints; i++) {
				if (y[i - 1] <= y[i] && y[i] > y[i + 1]) {
					ym[++numberOfLocalPeaks] = y[i];
					xm[numberOfLocalPeaks] = x[i];
				}
			}
			if (numberOfLocalPeaks > numberOfPoints / 10) {
				for (long i = 1; i <= numberOfLocalPeaks; i++) {
					x[i] = xm[i]; y[i] = ym[i];
				}
				numberOfPoints = numberOfLocalPeaks;
			}
			method = 2; // robust fit of peaks
		}
		// fit a straight line through (x,y)'s
		NUMlineFit (x.peek(), y.peek(), numberOfPoints, & a, & intercept, method);
		if (p_intercept) { *p_intercept = intercept; }
		if (p_a) { *p_a = a; }
	} catch (MelderError) {
		Melder_throw (me, U": couldn't fit a line.");
	}
}

#if 0
// Hillenbrand subtracts dB values and if the result is negative it is made zero
static void PowerCepstrum_subtractTiltLine_inline2 (PowerCepstrum me, double slope, double intercept, int lineType) {
	for (long j = 1; j <= my nx; j++) {
		double q = my x1 + (j - 1) * my dx;
		q = j == 1 ? 0.5 * my dx : q; // approximation
		double xq = lineType == 2 ? log(q) : q;
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (j, 1, 0);
		double diff = exp ((db_cepstrum - db_background) * NUMln10 / 10) - 1e-30;
		my z[1][j] = diff;
	}
}
#endif

// clip with tilt line
static void PowerCepstrum_subtractTiltLine_inline (PowerCepstrum me, double slope, double intercept, int lineType) {
	for (long j = 1; j <= my nx; j++) {
		double q = my x1 + (j - 1) * my dx;
		q = j == 1 ? 0.5 * my dx : q; // approximation
		double xq = lineType == 2 ? log(q) : q;
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (j, 1, 0);
		double diff = db_cepstrum - db_background;
		if (diff < 0) {
			diff = 0;
		}
		my z[1][j] = exp (diff * NUMln10 / 10.0) - 1e-30;
	}
}


void PowerCepstrum_subtractTilt_inline (PowerCepstrum me, double qstartFit, double qendFit, int lineType, int fitMethod) {
	double slope, intercept;
	PowerCepstrum_fitTiltLine (me, qstartFit, qendFit, &slope, &intercept, lineType, fitMethod);
	PowerCepstrum_subtractTiltLine_inline (me, slope, intercept, lineType);
}

autoPowerCepstrum PowerCepstrum_subtractTilt (PowerCepstrum me, double qstartFit, double qendFit, int lineType, int fitMethod) {
	try {
		autoPowerCepstrum thee = Data_copy (me);
		PowerCepstrum_subtractTilt_inline (thee.get(), qstartFit,  qendFit, lineType, fitMethod);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": couldn't subtract tilt line.");
	}
}

void PowerCepstrum_smooth_inline (PowerCepstrum me, double quefrencyAveragingWindow, long numberOfIterations) {
	try {
		long numberOfQuefrencyBins = (long) floor (quefrencyAveragingWindow / my dx);
		if (numberOfQuefrencyBins > 1) {
			autoNUMvector<double> qin (1, my nx);
			autoNUMvector<double> qout (1, my nx);
			for (long iq = 1; iq <= my nx; iq ++) {
				qin [iq] = my z [1] [iq];
			}
			double *xin, *xout;
			for (long k = 1; k <= numberOfIterations; k ++) {
				xin  = k % 2 == 1 ? qin.peek() : qout.peek ();
				xout = k % 2 == 1 ? qout.peek () : qin.peek();
				NUMvector_smoothByMovingAverage (xin, my nx, numberOfQuefrencyBins, xout);
			}
			for (long iq = 1; iq <= my nx; iq++) {
				my z [1] [iq] = xout [iq];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

autoPowerCepstrum PowerCepstrum_smooth (PowerCepstrum me, double quefrencyAveragingWindow, long numberOfIterations) {
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_smooth_inline (thee.get(), quefrencyAveragingWindow, numberOfIterations);
	return thee;
}

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double *p_peakdB, double *p_quefrency) {
	double peakdB, quefrency;
	autoPowerCepstrum thee = Data_copy (me);
	double lowestQuefrency = 1.0 / pitchCeiling, highestQuefrency = 1.0 / pitchFloor;
	for (long i = 1; i <= my nx; i ++) {
		thy z[1][i] = my v_getValueAtSample (i, 1, 0); // 10 log val^2
	}
	Vector_getMaximumAndX ((Vector) thee.get(), lowestQuefrency, highestQuefrency, 1, interpolation, & peakdB, & quefrency);   // FIXME cast
	if (p_peakdB) {
		*p_peakdB = peakdB;
	}
	if (p_quefrency) {
		*p_quefrency = quefrency;
	}
}

#if 0
static void Cepstrum_getZ (Cepstrum me, long imin, long imax, double peakdB, double slope, double intercept, int lineType, double *z) {
	long ndata = imax - imin + 1;
	autoNUMvector<double> dabs (1, ndata);
	for (long i = imin; i <= imax; i ++) {
		double q = my x1 + (i - 1) * my dx;
		q = ( i == 1 ? 0.5 * my dx : q );   // approximation
		double xq = ( lineType == 2 ? log (q) : q );
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (i, 1, 0);
		double diff = exp ((db_cepstrum - db_background) * NUMln10 / 10.0) - 1e-30;
		//double diff = fabs (db_cepstrum - db_background);
		dabs [i - imin + 1] = diff;
	}
	double q50 = NUMquantile (ndata, dabs.peek(), 0.5);
	double peak = exp (peakdB * NUMln10 / 10.0) - 1e-30;
	if (z) {
		*z = peak / q50;
	}
}
#endif

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	double rnr = undefined;
	double qmin = 1.0 / pitchCeiling, qmax = 1.0 / pitchFloor, peakdB, qpeak;
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, 2, &peakdB, &qpeak);
	long imin, imax;
	if (! Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax)) {
		return rnr;
	}
	long ndata = imax - imin + 1;
	if (ndata < 2) {
		return rnr;
	}	
	// how many peaks in interval ?
	long npeaks = 2;
	while (qpeak > 0 && qpeak * npeaks <= qmax) { npeaks++; }
	npeaks--;
	
	double sum = 0, sumr = 0;
	for (long i = imin; i <= imax; i++) {
		double val = my v_getValueAtSample (i, 0, 0);
		double qx = my x1 + (i - 1) * my dx;
		sum += val;
		// is qx within an interval around a multiple of the peak's q ?
		for (long j = 1; j <= npeaks; j ++) {
			double f0c = 1.0 / (j * qpeak);
			double f0clow = f0c * (1.0 - f0fractionalWidth);
			double f0chigh = f0c * (1.0 + f0fractionalWidth);
			double qclow =  1.0 / f0chigh;
			double qchigh = ( f0fractionalWidth >= 1 ? qmax : 1.0 / f0clow );
			if (qx >= qclow && qx <= qchigh) {   // yes in rahmonic interval
				sumr += val;
				break;
			}
		}
	}
	rnr = sumr >= sum ? 1000000 : sumr / (sum - sumr);
	return rnr;
}

double PowerCepstrum_getPeakProminence_hillenbrand (PowerCepstrum me, double pitchFloor, double pitchCeiling, double *qpeak) {
	double slope, intercept, quefrency, peakdB;
	PowerCepstrum_fitTiltLine (me, 0.001, 0, &slope, &intercept, 1, 1);
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_subtractTiltLine_inline (thee.get(), slope, intercept, 1);
	PowerCepstrum_getMaximumAndQuefrency (thee.get(), pitchFloor, pitchCeiling, 0, & peakdB, & quefrency);
	if (qpeak) {
		*qpeak = quefrency;
	}
	return peakdB;
}

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod, double *p_qpeak) {
	double slope, intercept, qpeak, peakdB;
	PowerCepstrum_fitTiltLine (me, qstartFit, qendFit, &slope, &intercept, lineType, fitMethod);
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, interpolation, & peakdB, & qpeak);
	double xq = lineType == 2 ? log(qpeak) : qpeak;
	double db_background = slope * xq + intercept;
	double cpp = peakdB - db_background;
	if (p_qpeak) {
		*p_qpeak = qpeak;
	}
	return cpp;
}

autoMatrix PowerCepstrum_to_Matrix (PowerCepstrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum (Matrix me) {
	try {
		if (my ny != 1) {
			Melder_throw (U"Matrix should have exactly 1 row.");
		}
		autoPowerCepstrum thee = Thing_new (PowerCepstrum);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PowerCepstrum.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum_row (Matrix me, long row) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		if (row < 1 || row > my ny) {
			Melder_throw (U"Row number should be between 1 and ", my ny, U" inclusive.");
		}
		NUMvector_copyElements (my z[row], thy z[1], 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum_column (Matrix me, long col) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		if (col < 1 || col > my nx) {
			Melder_throw (U"Column number should be between 1 and ", my nx, U" inclusive.");
		}
		for (long i = 1; i <= my ny; i++) {
			thy z[1][i] = my z[i][col];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

/* End of file Cepstrum.cpp */
