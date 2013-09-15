/* Cepstrum.cpp
 *
 * Copyright (C) 1994-2013 David Weenink
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
 djmw 20010514
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrum.h"
#include "NUM2.h"
#include "Vector.h"

static void NUMvector_gaussianBlur (double sigma, long filterLength, double *filter) {
	if (filterLength <= 1) {
		filter[1] = 1;
		return;
	}
	double sum = 0, mid = (filterLength + 1) / 2;
	for (long i = 1; i <= filterLength; i++) {
		double val = (mid - i) / sigma;
		filter[i] = exp (- 0.5 * val * val);
		sum += filter[i];
	}
	for (long i = 1; i <= filterLength; i++) {
		filter[i] /= sum;
	}
}

// filter must be normalised: sum(i=1, nfilters, filter[i]) == 1
static void NUMvector_filter (double *input, long numberOfDataPoints, double *filter, long numberOfFilterCoefficients, double *output, int edgeTreatment) {
	long nleft = (numberOfFilterCoefficients - 1) / 2;
	if (edgeTreatment == 0) { // outside values are zero
		for (long i = 1; i <= numberOfDataPoints; i++) {
			long ifrom = i - nleft, ito = i + nleft;
			ito = numberOfFilterCoefficients % 2 == 0 ? ito - 1 : ito;
			long jfrom = ifrom < 1 ? 1 : ifrom;
			long jto = ito > numberOfDataPoints ? numberOfDataPoints : ito;
			long index = ifrom < 1 ? 2 - ifrom : 1;
			double out = 0, sum = 0;
			for (long j = jfrom; j <= jto; j++, index++) {
				out += filter[index] * input[j];
				sum += filter[index];
			}
			output[i] = out / sum;
		}
	} else if (edgeTreatment == 1) { // wrap-around
		for (long i = 1; i <= numberOfDataPoints; i++) {
			double out = 0;
			for (long j = 1; j <= numberOfFilterCoefficients; j++) {
				long index = (i - nleft + j - 2) % numberOfDataPoints + 1;
				out += filter[j] * input[index];
			}
			output[i] = out;
		}
	}
	
	
}

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
	return NUMundefined;
}

double structPowerCepstrum :: v_getValueAtSample (long isamp, long which, int units) {
	(void) units;
	if (which == 0) {
		return z[1][isamp];
	} else {
		// dB's
		return 10.0 * log10 (z[1][isamp] + 1e-30); // always positive
	}
	return NUMundefined;
}

Cepstrum Cepstrum_create (double qmax, long nq) {
	try {
		autoCepstrum me = Thing_new (Cepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.peek(), 0, qmax, nq, dq, 0, 1, 1, 1, 1, 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Cepstrum not created.");
	}
}

PowerCepstrum Cepstrum_downto_PowerCepstrum (Cepstrum me ) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		for (long i = 1; i <= my nx; i++) {
			thy z[1][i] = my z[1][i] * my z[1][i];
		}
		return thee.transfer ();
	} catch (MelderError) {
		Melder_throw (me, " not converted.");
	}
}

PowerCepstrum PowerCepstrum_create (double qmax, long nq) {
	try {
		autoPowerCepstrum me = Thing_new (PowerCepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.peek(), 0, qmax, nq, dq, 0, 1, 1, 1, 1, 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PowerCepstrum not created.");
	}
}

void _Cepstrum_draw (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, int power, int garnish) {
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
		Graphics_textBottom (g, 1, L"Quefrency (s)");
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		Graphics_textLeft (g, 1, power ? L"Amplitude (dB)" : L"Amplitude");
		Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
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
void PowerCepstrum_fitTiltLine (PowerCepstrum me, double qmin, double qmax, double *a, double *intercept, int lineType, int method) {
	try {
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
			Melder_throw ("Not enough points for fit.");
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
		NUMlineFit (x.peek(), y.peek(), numberOfPoints, a, intercept, method);
	} catch (MelderError) {
		Melder_throw (me, ": couldn't fit a line.");
	}
}

// Hillenbrand subtracts dB values and if the result is negative it is made zero
void PowerCepstrum_subtractTiltLine_inline (PowerCepstrum me, double slope, double intercept, int lineType) {
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


void PowerCepstrum_subtractTilt_inline (PowerCepstrum me, double qstartFit, double qendFit, int lineType, int fitMethod) {
	double slope, intercept;
	PowerCepstrum_fitTiltLine (me, qstartFit, qendFit, &slope, &intercept, lineType, fitMethod);
	PowerCepstrum_subtractTiltLine_inline (me, slope, intercept, lineType);
}

PowerCepstrum PowerCepstrum_subtractTilt (PowerCepstrum me, double qstartFit, double qendFit, int lineType, int fitMethod) {
	try {
		autoPowerCepstrum thee = Data_copy (me);
		PowerCepstrum_subtractTilt_inline (thee.peek(), qstartFit,  qendFit, lineType, fitMethod);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": couldn't subtract tilt line.");
	}
}

void PowerCepstrum_smooth_inline2 (PowerCepstrum me, double quefrencyAveragingWindow) {
	try {
		long numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
		if (numberOfQuefrencyBins > 1) {
			autoNUMvector<double> qin (1, my nx);
			autoNUMvector<double> qout (1, my nx);
			for (long iq = 1; iq <= my nx; iq++) {
				qin[iq] = my z[1][iq];
			}
			NUMvector_smoothByMovingAverage (qin.peek(), my nx, numberOfQuefrencyBins, qout.peek());
			for (long iq = 1; iq <= my nx; iq++) {
				my z[1][iq] = qout[iq];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not smoothed.");
	}
}

void PowerCepstrum_smooth_inline (PowerCepstrum me, double quefrencyAveragingWindow, long numberOfIterations) {
	try {
		long numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
		if (numberOfQuefrencyBins > 1) {
			autoNUMvector<double> qin (1, my nx);
			autoNUMvector<double> qout (1, my nx);
			for (long iq = 1; iq <= my nx; iq++) {
				qin[iq] = my z[1][iq];
			}
			double *xin, *xout;
			for (long k = 1; k <= numberOfIterations; k++) {
				xin  = k % 2 == 1 ? qin.peek() : qout.peek ();
				xout = k % 2 == 1 ? qout.peek () : qin.peek();
				NUMvector_smoothByMovingAverage (xin, my nx, numberOfQuefrencyBins, xout);
			}
			for (long iq = 1; iq <= my nx; iq++) {
				my z[1][iq] = xout[iq];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not smoothed.");
	}
}

PowerCepstrum PowerCepstrum_smooth (PowerCepstrum me, double quefrencyAveragingWindow, long numberOfIterations) {
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_smooth_inline (thee.peek(), quefrencyAveragingWindow, numberOfIterations);
	return thee.transfer();
}

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double *peakdB, double *quefrency) {
	*peakdB = *quefrency = NUMundefined;
	autoPowerCepstrum thee = Data_copy (me);
	double lowestQuefrency = 1 / pitchCeiling, highestQuefrency = 1 / pitchFloor;
	for (long i = 1; i <= my nx; i++) {
		thy z[1][i] = my v_getValueAtSample (i, 1, 0); // 10 log val^2
	}
	Vector_getMaximumAndX ((Vector) thee.peek(), lowestQuefrency, highestQuefrency, 1, interpolation, peakdB, quefrency);
}

static void Cepstrum_getZ2 (Cepstrum me, long imin, long imax, double peakdB, long margin, long keep, double *z) {
	*z = NUMundefined;
	long npeaks = 0, n = (imax - imin) / 2 + keep;
	autoNUMvector<double> ymax (1, n);
	autoNUMvector<long> index (1, n);
	for (long i = imin + 1; i < imax; i++) {
		if (my z[1][i] > my z[1][i-1] && my z[1][i] > my z[1][i+1]) {
			ymax[++npeaks] = my z[1][i];
			index[npeaks] = i;
		}
	}
	NUMsort2<double, long> (npeaks, ymax.peek(), index.peek());
	long i = npeaks - 1, ipeak = 0;
	while (i > 0 && ipeak < keep) {
		if (labs(index[i] - index[npeaks]) > margin) {
			ipeak++;
			ymax[npeaks +ipeak] = ymax[i];
		}
		i--;
	}
	double mean, variance;
	NUMvector_avevar (&ymax[npeaks], ipeak, &mean, &variance);
	double sigma = sqrt (variance / (ipeak - 1));
	double peak = exp (peakdB * NUMln10 / 10) - 1e-30;
	*z = sigma <= 0 ? NUMundefined : peak / sigma;
}

static void Cepstrum_getZ (Cepstrum me, long imin, long imax, double peakdB, double slope, double intercept, int lineType, double *z) {
	long ndata = imax - imin + 1;
	autoNUMvector<double> dabs (1, ndata);
	for (long i = imin; i <= imax; i++) {
		double q = my x1 + (i - 1) * my dx;
		q = i == 1 ? 0.5 * my dx : q; // approximation
		double xq = lineType == 2 ? log(q) : q;
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (i, 1, 0);
		double diff = exp ((db_cepstrum - db_background) * NUMln10 / 10) - 1e-30;
		//double diff = fabs (db_cepstrum - db_background);
		dabs[i - imin + 1] = diff;
	}
	double q50 = NUMquantile (ndata, dabs.peek(), 0.5);
	double peak = exp (peakdB * NUMln10 / 10) - 1e-30;
	//*z = peakdB / q50;
	*z = peak / q50;
}

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	double rnr = NUMundefined;
	double qmin = 1 / pitchCeiling, qmax = 1 / pitchFloor, peakdB, qpeak;
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
		for (long j = 1; j <= npeaks; j++) {
			double f0c = 1 / (j * qpeak);
			double f0clow = f0c * (1 - f0fractionalWidth);
			double f0chigh = f0c * (1 + f0fractionalWidth);
			double qclow =  1 / f0chigh;
			double qchigh = f0fractionalWidth >= 1 ? qmax : 1 / f0clow;
			if (qx >= qclow && qx <= qchigh) { // yes in rhamonic interval
				sumr += val; break;
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
	PowerCepstrum_subtractTiltLine_inline (thee.peek(), slope, intercept, 1);
	PowerCepstrum_getMaximumAndQuefrency (thee.peek(), pitchFloor, pitchCeiling, 0, &peakdB, &quefrency);
	if (qpeak) {
		*qpeak = quefrency;
	}
	return peakdB;
}

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod, double *qpeak) {
	double slope, intercept, quefrency, peakdB;
	PowerCepstrum_fitTiltLine (me, qstartFit, qendFit, &slope, &intercept, lineType, fitMethod);
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, interpolation, &peakdB, &quefrency);
	double xq = lineType == 2 ? log(quefrency) : quefrency;
	double db_background = slope * xq + intercept;
	double cpp = peakdB - db_background;
	if (qpeak != NULL) {
		*qpeak = quefrency;
	}
	return cpp;
}

Matrix PowerCepstrum_to_Matrix (PowerCepstrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Matrix created.");
	}
}

PowerCepstrum Matrix_to_PowerCepstrum (Matrix me) {
	try {
		if (my ny != 1)
			Melder_throw ("Matrix must have exactly 1 row.");
		autoPowerCepstrum thee = Thing_new (PowerCepstrum);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to PowerCepstrum.");
	}
}

PowerCepstrum Matrix_to_PowerCepstrum_row (Matrix me, long row) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		if (row < 1 || row > my ny) {
			Melder_throw ("Row number must be between 1 and ", my ny, " inclusive.");
		}
		NUMvector_copyElements (my z[row], thy z[1], 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PowerCepstrum created.");
	}
}

PowerCepstrum Matrix_to_PowerCepstrum_column (Matrix me, long col) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		if (col < 1 || col > my nx) {
			Melder_throw ("Column number must be between 1 and ", my nx, " inclusive.");
		}
		for (long i = 1; i <= my ny; i++) {
			thy z[1][i] = my z[i][col];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PowerCepstrum created.");
	}
}

/* End of file Cepstrum.cpp */
