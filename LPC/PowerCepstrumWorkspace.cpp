/* PowerCepstrumWorkspace.cpp
 *
 * Copyright (C) 2025 David Weenink
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

#include "PowerCepstrumWorkspace.h"
#include "SlopeSelector.h"
#include "Vector.h"

#include "oo_DESTROY.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_COPY.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_EQUAL.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "PowerCepstrumWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "PowerCepstrumWorkspace_def.h"

Thing_implement (PowerCepstrumWorkspace, Daata, 0);


static inline bool greaterThanOrEqual (double x, double y) {
	// x >= y means !(x < y) 
	return ( (x == 0.0 || y == 0.0) ? (x - y) : (x - y) / fabs (x) ) > -1e-12;
}

static inline bool greaterThan (double x, double y) {
	return ( (x == 0.0 || y == 0.0) ? (x - y) : (x - y) / fabs (x) ) > 1e-12;
}

static void Vector_getMaximumAndX_twoSideApproach (constVector me, double xmin, double xmax, integer channelNumber, 
	kVector_peakInterpolation peakInterpolationType, bool startHigh, double *out_maximum, double *out_xOfMaximum)
{
	Melder_assert (channelNumber >= 1 && channelNumber <= my ny);
	constVEC y = my z.row (channelNumber);
	double maximum, x;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No samples between xmin and xmax.
			Try to return the greater of the values at these two points.
		*/
		kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
				kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
		const double yleft = Vector_getValueAtX (me, xmin, channelNumber, valueInterpolationType);
		const double yright = Vector_getValueAtX (me, xmax, channelNumber, valueInterpolationType);
		maximum = std::max (yleft, yright);
		x = ( yleft == yright ? (xmin + xmax) / 2.0 : yleft > yright ? xmin : xmax );
	} else  {
		maximum = y [imin];
		x = imin;
		if (y [imax] > maximum) {
			maximum = y [imax];
			x = imax;
		}
		if (imin == 1)
			imin ++;
		if (imax == my nx)
			imax --;
		if (! startHigh) { // approach from the start
			for (integer i = imin; i <= imax; i ++) {
				if (greaterThan (y [i], y [i - 1]) && greaterThanOrEqual (y [i], y [i + 1])) {
					double i_real;
					const double localMaximum = NUMimproveMaximum (y, i, kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType), & i_real);
					if (localMaximum > maximum) {
						maximum = localMaximum;
						x = i_real;
					}
				}
			}
		} else { // approach from the end
			for (integer i = imax; i >= imin; i --) {
				if (greaterThan (y [i], y [i + 1]) && greaterThanOrEqual (y [i], y [i - 1])) {
					double i_real;
					const double localMaximum = NUMimproveMaximum (y, i, kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType), & i_real);
					if (localMaximum > maximum) {
						maximum = localMaximum;
						x = i_real;
					}
				}
			}
		}
		x = my x1 + (x - 1) * my dx;   // convert sample to x
		Melder_clip (xmin, & x, xmax);
		
	}
	if (out_maximum)
		*out_maximum = maximum;
	if (out_xOfMaximum)
		*out_xOfMaximum = x;
}

void structPowerCepstrumWorkspace :: getSlopeAndIntercept () {
	SlopeSelector_getSlopeAndIntercept (slopeSelector.get(), slope, intercept, method);
	slopeKnown = true;
}

double structPowerCepstrumWorkspace :: getTrend (double quefrency) {
	if (! slopeKnown)
		getSlopeAndIntercept ();
	const double xq = ( trendLineType == kCepstrum_trendType::EXPONENTIAL_DECAY ? log (quefrency) : quefrency );
	trenddB = slope * xq + intercept;
	return trenddB;
}

void structPowerCepstrumWorkspace :: subtractTrend () {
	if (! slopeKnown)
		getSlopeAndIntercept ();
	for (integer j = 1; j <= powercepstrum -> nx; j ++) {
		/*
			For the exponential decay function, y = slope*log(quefrency)+intercept, the PowerCepstrum's first quefrency
			value (at x1) has quefrency == 0 and therefore value y(0) is not defined. As an approximation for y(0) 
			we use y(0.5*dx). This is no problem because the value at quefrency == 0 is not relevant.
		*/
		const double quefrency = ( j == 1 && trendLineType == kCepstrum_trendType::EXPONENTIAL_DECAY ? 0.5 * powercepstrum -> dx : (j - 1) * powercepstrum -> dx );
		const double db_background = getTrend (quefrency);
		const double db_cepstrum = powercepstrum -> v_getValueAtSample (j, 1, 1);
		const double diff = Melder_clippedLeft (0.0, db_cepstrum - db_background);
		powercepstrum -> z [1] [j] = exp (diff * NUMln10 / 10.0);
	}
	trendSubtracted = true;
}

void structPowerCepstrumWorkspace :: newData (constPowerCepstrum thee) {
	Melder_assert (thy nx == powercepstrum -> nx);
	powercepstrum = thee;
	for (integer ipoint = 1, i = imin; i <= imax; i ++, ipoint ++) {
		double xval = Sampled_indexToX (thee, i);
		if (trendLineType == kCepstrum_trendType::EXPONENTIAL_DECAY)
			xval = log (xval);
		x [ipoint] = xval;
		y [ipoint] = thy v_getValueAtSample (i, 1, 1); // dB's
	}
	slopeSelector -> newDataPoints (x.get(), y.get());
	todBs ();
	peakKnown = slopeKnown = trendSubtracted = false;
}

void structPowerCepstrumWorkspace :: getPeakAndPosition () {
	Matrix thee = asdBs.get();
	double peakdBR, peakQuefrencyR;
	Vector_getMaximumAndX_twoSideApproach ((Vector) thee, qminSearchInterval, qmaxSearchInterval,
		1, peakInterpolationType, false, & peakdB, & peakQuefrency);
	Vector_getMaximumAndX_twoSideApproach ((Vector) thee, qminSearchInterval, qmaxSearchInterval,
		1, peakInterpolationType, true, & peakdBR, & peakQuefrencyR);
	const integer index = Sampled_xToIndex (thee, peakQuefrency), indexR = Sampled_xToIndex(thee, peakQuefrencyR);
	if (index != indexR && (indexR - index) <= 5) {
		double indexm = 0.5 * (index + indexR);
		peakQuefrency = thy x1 + (indexm - 1) * thy dx;
		peakdB = thy z [1] [index]; // always with flat peak
	}
	peakKnown = true;
}

void structPowerCepstrumWorkspace :: getCPP () {
	if (! slopeKnown)
		getSlopeAndIntercept ();
	if (! peakKnown)
		getPeakAndPosition ();
	trenddB = getTrend (peakQuefrency);
	cpp = peakdB - trenddB;
}

void structPowerCepstrumWorkspace :: todBs () {
	for (integer i = 1; i <= powercepstrum -> nx; i ++)
		asdBs -> z [1] [i] = powercepstrum -> v_getValueAtSample (i, 1, 1); // 10 log val^2
}

void structPowerCepstrumWorkspace :: fromdBs (PowerCepstrum me) {
	for (integer i = 1; i <= powercepstrum -> nx; i ++)
		my z [1] [i] = pow (10.0, my z [1] [i] / 10.0) - 1e-30;
}

void structPowerCepstrumWorkspace :: setMaximumNumberOfRhamonics (integer maximumNumberOfRhamonics) {
	our maximumNumberOfRhamonics = maximumNumberOfRhamonics;
	// resize when we calculate values.
}

void structPowerCepstrumWorkspace :: getNumberOfRhamonics (double qmin, double qmax) {
		integer numberOfRhamonics = 2;
		if (! peakKnown)
			getPeakAndPosition ();
		while (peakQuefrency >= qmin && peakQuefrency * numberOfRhamonics <= qmax)
			numberOfRhamonics ++;
		numberOfRhamonics --;
		numberOfRhamonics = std:: min (numberOfRhamonics, maximumNumberOfRhamonics);
}

void structPowerCepstrumWorkspace :: getRhamonicsPower (double qmin, double qmax, double f0fractionalWidth) {
	getSlopeAndIntercept ();
	getPeakAndPosition ();
	getNumberOfRhamonics (qmin, qmax);
	rhamonics.resize (numberOfRhamonics, 6_integer);
	// q, peakdB, power, q1, q2, trenddb
	for (integer rhamonic = 1; rhamonic <= numberOfRhamonics; rhamonic ++) {
		const double quefrency = rhamonic * peakQuefrency;
		const double f = 1.0 / quefrency;
		const double flow = f * (1.0 - f0fractionalWidth);
		const double fhigh = f * (1.0 + f0fractionalWidth);
		const double qlow =  1.0 / fhigh;
		const double qhigh = ( f0fractionalWidth >= 1.0 ? powercepstrum -> xmax : 1.0 / flow );
		integer iqmin, iqmax;
		Matrix_getWindowSamplesX (powercepstrum, qlow, qhigh, & iqmin, & iqmax);
		double power = 0.0;
		for (integer iq = iqmin; iq <= iqmax; iq ++)
			power += powercepstrum -> v_getValueAtSample (iq, 1, 0);
		rhamonics [rhamonic] [1] = power;
		rhamonics [rhamonic] [2] = qlow;
		rhamonics [rhamonic] [3] = quefrency;
		rhamonics [rhamonic] [4] = qhigh;
		rhamonics [rhamonic] [5] = peakdB;
	}
}

void structPowerCepstrumWorkspace :: getRhamonicPeaks (double qmin, double qmax) {
	getPeakAndPosition ();
	getNumberOfRhamonics (qmin, qmax);
	rhamonics.resize (numberOfRhamonics, 6_integer);
	rhamonics [1] [2] = peakQuefrency;
	rhamonics [1] [1] = peakdB;
	kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
			kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
	for (integer rhamonic = 2; rhamonic <= numberOfRhamonics; rhamonic ++) {
		const double quefrency = rhamonic * peakQuefrency;
		const double amplitudedB = Vector_getValueAtX ((Vector) asdBs.get(), quefrency, 1, valueInterpolationType);
		rhamonics [rhamonic] [2] = quefrency;
		rhamonics [rhamonic] [1] = amplitudedB;	
	}
}

double structPowerCepstrumWorkspace :: getRNR (double qmin, double qmax, double f0fractionalWidth) {
	getRhamonicsPower (qmin, qmax, f0fractionalWidth);
	const double sumOfRhamonics = NUMsum (rhamonics.column (3));
	double power = 0.0;
	for (integer iq = imin; iq <= imax; iq ++)
		power += powercepstrum -> v_getValueAtSample (iq, 1, 0);
	const double rnr = sumOfRhamonics / (power - sumOfRhamonics);
	return rnr;
}

void PowerCepstrumWorkspace_init (PowerCepstrumWorkspace me, constPowerCepstrum thee, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method)
{
	Function_unidirectionalAutowindow (thee, & qminFit, & qmaxFit);
	my powercepstrum = thee;
	Melder_require (qminFit >= thy xmin && qmaxFit <= thy xmax,
		U"Your quefrency range is outside the domain.");
	(void) Matrix_getWindowSamplesX (thee, qminFit, qmaxFit, & my imin, & my imax);
	Melder_clipLeft (2_integer, & my imin); // never use q=0 in fitting
	my numberOfPoints = my imax - my imin + 1;
	Melder_require (my numberOfPoints > 2,
		U"Not enough points in the selection.");
	my trendLineType = trendLineType;
	my method = ( method == kCepstrum_trendFit::LEAST_SQUARES ? kSlopeSelector_method::LEAST_SQUARES :
		(method == kCepstrum_trendFit::ROBUST_SLOW ? kSlopeSelector_method::THEILSEN : kSlopeSelector_method::SIEGEL));
	my x = raw_VEC (my numberOfPoints);
	my y = raw_VEC (my numberOfPoints);
	my asdBs = Matrix_create (thy xmin, thy xmax, thy nx, thy dx, thy x1, thy ymin, thy ymax, thy ny, thy dy, thy y1);
	my maximumNumberOfRhamonics = 15;
	my rhamonics = raw_MAT (my maximumNumberOfRhamonics, 5_integer);
	my slopeSelector = SlopeSelector_create (my x.get(), my y.get()); // only reference to the x and y vectors
	my newData (thee); // new xp and yp reference
}

autoPowerCepstrumWorkspace PowerCepstrumWorkspace_create (constPowerCepstrum thee, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method)
{
	try {
		autoPowerCepstrumWorkspace me = Thing_new (PowerCepstrumWorkspace);
		PowerCepstrumWorkspace_init (me.get(), thee, qminFit, qmaxFit, trendLineType, method);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create PowerCepstrumWorkspace");
	}
}

void PowerCepstrumWorkspace_initPeakSearchPart (mutablePowerCepstrumWorkspace me, double qminSearchInterval,
	double qmaxSearchInterval, kVector_peakInterpolation peakInterpolationType)
{
	my qminSearchInterval = qminSearchInterval;
	my qmaxSearchInterval = qmaxSearchInterval;
	my peakInterpolationType = peakInterpolationType;
}

/* End of file PowerCepstrumWorkspace.cpp */
