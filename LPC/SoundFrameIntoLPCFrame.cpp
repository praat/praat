/* SoundFrameIntoLPCFrame.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "SoundFrameIntoLPCFrame.h"

#include "oo_DESTROY.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_COPY.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_EQUAL.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SoundFrameIntoLPCFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundFrameIntoLPCFrame_def.h"

Thing_implement (SoundFrameIntoLPCFrame, SoundFrameIntoSampledFrame, 0);

void structSoundFrameIntoLPCFrame :: allocateOutputFrames () {
	for (integer iframe = 1; iframe <= outputlpc -> nx; iframe ++)
		LPC_Frame_init (& outputlpc -> d_frames [iframe], outputlpc -> maxnCoefficients);
}

void structSoundFrameIntoLPCFrame :: saveOutputFrame () {
	// nothing to do
}

bool structSoundFrameIntoLPCFrame :: inputFrameToOutputFrame () {
	return true;
}

void SoundFrameIntoLPCFrame_init (mutableSoundFrameIntoLPCFrame me, constSound input, mutableLPC outputlpc,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	SoundFrameIntoSampledFrame_init (me, input, outputlpc, effectiveAnalysisWidth, windowShape);
	my outputlpc = outputlpc;
	my order = my outputlpc -> maxnCoefficients;
	my orderp1 = my order + 1;
	my a = zero_VEC (my orderp1);
}

/************************ autocorrelation method *****************************/

Thing_implement (SoundFrameIntoLPCFrameAuto, SoundFrameIntoLPCFrame, 0);

autoSoundFrameIntoLPCFrameAuto SoundFrameIntoLPCFrameAuto_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, 
	kSound_windowShape windowShape) {
	try {
		autoSoundFrameIntoLPCFrameAuto me = Thing_new (SoundFrameIntoLPCFrameAuto);
		SoundFrameIntoLPCFrame_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my r = zero_VEC (my orderp1);
		my rc = zero_VEC (my order);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoLPCFrameAuto.");
	}
}

/*
	Markel & Gray, Linear Prediction of Speech, page 219
*/
bool structSoundFrameIntoLPCFrameAuto :: inputFrameToOutputFrame ()  {
	LPC_Frame lpcf = & outputlpc -> d_frames [currentFrame];
	frameAnalysisInfo = 0;
	Melder_assert (lpcf -> nCoefficients > 0);

	VEC  x = soundFrame;
	
	/*
		Compute the autocorrelations
	*/
	lpcf -> a.get()  <<=  0.0;
	lpcf -> gain = 0.0;
	for (integer i = 1; i <= orderp1; i ++)
		r [i] = NUMinner (x.part (1, x.size - i + 1), x.part (i, x.size));
	if (r [1] == 0.0) {
		/*
			The sound frame contains only zero's
		*/
		lpcf -> nCoefficients = 0;
		lpcf -> a.resize (lpcf -> nCoefficients); // maintain invariant
		frameAnalysisInfo = 1;
		return false;
	}
	a [1] = 1.0;
	a [2] = rc [1] = - r [2] / r [1];
	double gain = r [1] + r [2] * rc [1];
	lpcf -> gain = gain;
	integer i = 1;
	for (i = 2; i <= order; i ++) {
		long double s = 0.0;
		for (integer j = 1; j <= i; j ++)
			s += r [i - j + 2] * a [j];
		rc [i] = - s / gain;
		for (integer j = 2; j <= i / 2 + 1; j ++) {
			const double at = a [j] + rc [i] * a [i - j + 2];
			a [i - j + 2] += rc [i] * a [j];
			a [j] = at;
		}
		a [i + 1] = rc [i];
		gain += rc [i] * s;
		if (gain <= 0.0) {
			frameAnalysisInfo = 2;
			break;
		}
		lpcf -> gain = gain;
	}
	-- i;
	lpcf -> a.part (1, i)  <<=  a.part (2, i + 1);
	lpcf -> a.resize (i);
	lpcf -> nCoefficients = lpcf -> a.size; // maintain invariant
	return true;
}

/************************ covariance method *****************************/

Thing_implement (SoundFrameIntoLPCFrameCovar, SoundFrameIntoLPCFrame, 0);

autoSoundFrameIntoLPCFrameCovar SoundFrameIntoLPCFrameCovar_create (constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape) 
{
	try {
		autoSoundFrameIntoLPCFrameCovar me = Thing_new (SoundFrameIntoLPCFrameCovar);
		SoundFrameIntoLPCFrame_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my order2 = my order * (my order + 1) / 2;
		my b = zero_VEC (my order2);
		my grc = zero_VEC (my order);
		my beta = zero_VEC (my order);
		my cc = zero_VEC (my orderp1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create WorkspaceLPCcovar.");
	}
}

/*
	Markel & Gray, Linear Prediction of Speech, page 221
*/
bool structSoundFrameIntoLPCFrameCovar :: inputFrameToOutputFrame () {
	LPC_Frame lpcf = & outputlpc -> d_frames [currentFrame];
	const integer n = soundFrameSize, m = order;
	
	if (lpcf -> nCoefficients == 0) {
		frameAnalysisInfo = 6;
		return false;
	}
		
	VEC x = soundFrame;
	
	frameAnalysisInfo = 0;
	double gain = 0.0;
	/*
		Compute the covariances
	*/
	VEC xi = x.part(m + 1, n), xim1 = x.part(m, n - 1);
	gain = NUMinner (xi, xi);
	cc [1] = NUMinner (xi, xim1);
	cc [2] = NUMinner (xim1, xim1);

	if (gain == 0.0) {
		frameAnalysisInfo = 1;
		lpcf -> nCoefficients = 0;
		lpcf -> gain = gain;
		lpcf -> a.resize (lpcf -> nCoefficients); //maintain invariant
		return false;
	}

	b [1] = 1.0;
	beta [1] = cc [2];
	a [1] = 1.0;
	a [2] = grc [1] = -cc [1] / cc [2];
	lpcf -> gain = gain += grc [1] * cc [1];
	integer iend = 1;
	for (integer i = 2; i <= m; i ++) { // 130
		for (integer j = 1; j <= i; j ++)
			cc [i - j + 2] = cc [i - j + 1] + x [m - i + 1] * x [m - i + j] - x [n - i + 1] * x [n - i + j];

		long double s = 0.0;
		for (integer j = m + 1; j <= n; j ++) // 30
			s += x [j - i] * x [j];
		cc [1] = s;
			
		b [i * (i + 1) / 2] = 1.0;
		for (integer j = 1; j <= i - 1; j ++) { // 70
			if (beta [j] < 0.0) {
				frameAnalysisInfo = 2;
				goto end;
			} else if (beta [j] == 0.0)
				continue;

			s = 0.0;
			for (integer k = 1; k <= j; k ++)
				s += cc [k + 1] * b [j * (j - 1) / 2 + k]; // 50
			const double gam = s / beta [j];
			for (integer k = 1; k <= j; k ++)
				b [i * (i - 1) / 2 + k] -= gam * b [j * (j - 1) / 2 + k]; // 60
		}

		s = 0.0;
		for (integer j = 1; j <= i; j ++)
			s += cc [j + 1] * b [i * (i - 1) / 2 + j]; // 80
		beta [i] = s;
		if (beta [i] <= 0.0) {
			frameAnalysisInfo = 3;
			break;
		}
		s = 0.0;
		for (integer j = 1; j <= i; j ++)
			s += cc [j] * a [j]; // 100
		grc [i] = -s / beta [i];

		for (integer j = 2; j <= i; j ++)
			a [j] += grc [i] * b [i * (i - 1) / 2 + j - 1]; // 110
		a [i + 1] = grc [i];
		s = grc [i] * grc [i] * beta [i];
		gain -= s;
		if (gain <= 0.0) {
			frameAnalysisInfo = 4;
			break;
		}
		lpcf -> gain = gain;
		iend ++;
	}
end:
	lpcf -> a.resize (iend);
	lpcf -> a.part (1, iend)  <<=  a.part (2, iend + 1);
	lpcf -> nCoefficients = lpcf -> a.size; // maintain invariant
	return true;
}

/************************ burg method *****************************/

Thing_implement (SoundFrameIntoLPCFrameBurg, SoundFrameIntoLPCFrame, 0);

autoSoundFrameIntoLPCFrameBurg SoundFrameIntoLPCFrameBurg_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	try {
		autoSoundFrameIntoLPCFrameBurg me = Thing_new (SoundFrameIntoLPCFrameBurg);
		SoundFrameIntoLPCFrame_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my b1 = zero_VEC (my soundFrameSize);
		my b2 = zero_VEC (my soundFrameSize);
		my aa = zero_VEC (my order);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoLPCFrameBurg.");
	}
}

static double VECburg_buffered (VEC const& a, constVEC const& x, SoundFrameIntoLPCFrameBurg ws) {
	const integer n = x.size, m = a.size;

	a   <<=  0.0; // always safe
	if (n <= 2) {
		a [1] = -1.0;
		return ( n == 2 ? 0.5 * (x [1] * x [1] + x [2] * x [2]) : x [1] * x [1] );
	}
	Melder_assert (n == ws -> soundFrameSize);
	VEC b1 = ws -> b1.get();
	VEC b2 = ws -> b2.get();
	VEC aa = ws -> aa.get();

	// (3)

	double p = NUMinner (x, x);

	if (p == 0.0) {
		ws -> frameAnalysisInfo = 1;
		return 0.0;
	}
	// (9)

	b1 [1] = x [1];
	for (integer j = 2; j <= n - 1; j ++)
		b1 [j] = b2 [j - 1] = x [j];
	b2 [n - 1] = x [n];

	longdouble xms = p / n;
	for (integer i = 1; i <= m; i ++) {
		// (7)

		/*
			longdouble num = 0.0, denum = 0.0;
			for (integer j = 1; j <= n - i; j ++) {
				num += b1 [j] * b2 [j];
				denum += b1 [j] * b1 [j] + b2 [j] * b2 [j];
			}
		*/
		VEC b1part = b1.part (1, n - i), b2part = b2.part (1, n - i);
		const double num = NUMinner (b1part, b2part);
		const double denum = NUMinner (b1part, b1part) + NUMinner (b2part, b2part);
		
		if (denum <= 0.0) {
			ws -> frameAnalysisInfo = 1;
			return 0.0;	// warning ill-conditioned
		}
		a [i] = 2.0 * num / denum;

		// (10)

		xms *= 1.0 - a [i] * a [i];

		// (5)

		for (integer j = 1; j <= i - 1; j ++)
			a [j] = aa [j] - a [i] * aa [i - j];

		if (i < m) {

			// (8) Watch out: i -> i+1

			for (integer j = 1; j <= i; j ++)
				aa [j] = a [j];
			for (integer j = 1; j <= n - i - 1; j ++) {
				b1 [j] -= aa [i] * b2 [j];
				b2 [j] = b2 [j + 1] - aa [i] * b1 [j + 1];
			}
		}
	}
	return double (xms);
}

bool structSoundFrameIntoLPCFrameBurg :: inputFrameToOutputFrame () {
	LPC_Frame lpcf = & outputlpc -> d_frames[currentFrame];

	lpcf -> gain = VECburg_buffered (lpcf -> a.get(), soundFrame, this);
	if (lpcf -> gain <= 0.0) {
		lpcf -> nCoefficients = 0;
		lpcf -> a.resize (lpcf -> nCoefficients); // maintain invariant
		return false;
	} else {
		lpcf -> gain *= soundFrame.size;
		for (integer i = 1; i <= lpcf -> nCoefficients; i ++)
			lpcf -> a [i] = - lpcf -> a [i];
		return true;
	}
}

/************************ marple method *****************************/

Thing_implement (SoundFrameIntoLPCFrameMarple, SoundFrameIntoLPCFrame, 0);

autoSoundFrameIntoLPCFrameMarple SoundFrameIntoLPCFrameMarple_create (constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape, double tol1, double tol2) {
	try {
		autoSoundFrameIntoLPCFrameMarple me = Thing_new (SoundFrameIntoLPCFrameMarple);
		SoundFrameIntoLPCFrame_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);		// use 'a' instead of defining 'c'
		my d = zero_VEC (my orderp1);
		my r = zero_VEC (my orderp1);
		my tol1 = tol1;
		my tol2 = tol2;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoLPCFrameMarple.");
	}
}

bool structSoundFrameIntoLPCFrameMarple :: inputFrameToOutputFrame () {
	const integer mmax = order, n = soundFrame.size;
	LPC_Frame lpcf = & outputlpc -> d_frames [currentFrame];
	VEC x = soundFrame;
	
	frameAnalysisInfo = 0;
	VEC c = a.get(); // yes 'a'
	VEC a = lpcf -> a.get();

	double gain = 0.0, e0 = 2.0 * NUMsum2 (x);
	integer m = 1;
	if (e0 == 0.0) {
		lpcf -> nCoefficients = 0;
		lpcf -> a.resize (lpcf -> nCoefficients); // maintain invariant
		lpcf -> gain = gain;
		frameAnalysisInfo = 1;
		return false;
	}
	double q1 = 1.0 / e0;
	double q2 = q1 * x [1], q = q1 * x [1] * x [1], w = q1 * x [n] * x [n];
	double v = q, u = w;
	double den = 1.0 - q - w;
	double q4 = 1.0 / den, q5 = 1.0 - q, q6 = 1.0 - w;
	double h = q2 * x [n], s = h;
	gain = e0 * den;
	q1 = 1.0 / gain;
	c [1] = q1 * x [1];
	d [1] = q1 * x [n];
	double s1 = 0.0;
	for (integer k = 1; k <= n - 1; k ++)
		s1 += x [k + 1] * x [k];
	r [1] = 2.0 * s1;
	a [1] = - q1 * r [1];
	gain *= (1.0 - a [1] * a [1]);
	while (m < mmax) {
		const double eOld = gain;
		double f = x [m + 1], b = x [n - m]; // n-1 ->n-m
		for (integer k = 1; k <= m; k ++) {
			// n-1 -> n-m
			f += x [m + 1 - k] * a [k];
			b += x [n - m + k] * a [k];
		}
		q1 = 1.0 / gain;
		q2 = q1 * f;
		const double q3 = q1 * b;
		for (integer k = m; k >= 1; k--) {
			c [k + 1] = c [k] + q2 * a [k];
			d [k + 1] = d [k] * q3 * a [k];
		}
		c [1] = q2;
		d [1] = q3;
		const double q7 = s * s;
		double y1 = f * f;
		const double y2 = v * v;
		const double y3 = b * b;
		const double y4 = u * u;
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
			frameAnalysisInfo = 2;
			goto end; // 2: ill-conditioning
		}
		q4 = 1.0 / den;
		q1 *= q4;
		const double alf = 1.0 / (1.0 + q1 * (y1 * q6 + y3 * q5 + 2.0 * h * f * b));
		gain *= alf;
		y5 = h * s;
		double c1 = q4 * (f * q6 + b * h);
		double c2 = q4 * (b * q5 + h * f);
		const double c3 = q4 * (v * q6 + y5);
		const double c4 = q4 * (s * q5 + v * h);
		const double c5 = q4 * (s * q6 + h * u);
		const double c6 = q4 * (u * q5 + y5);
		for (integer k = 1; k <= m; k ++)
			a [k] = alf * (a [k] + c1 * c [k + 1] + c2 * d [k + 1]);
		for (integer k = 1; k <= m / 2 + 1; k ++) {
			s1 = c [k];
			const double s2 = d [k], s3 = c [m + 2 - k], s4 = d [m + 2 - k];

			c [k] += c3 * s3 + c4 * s4;
			d [k] += c5 * s3 + c6 * s4;
			if (m + 2 - k == k)
				continue;
			c [m + 2 - k] += c3 * s1 + c4 * s2;
			d [m + 2 - k] += c5 * s1 + c6 * s2;
		}
		m ++;
		c1 = x [n + 1 - m];
		c2 = x [m];
		double delta = 0.0;
		for (integer k = m - 1; k >= 1; k--) {
			r [k + 1] = r [k] - x [n + 1 - k] * c1 - x [k] * c2;
			delta += r [k + 1] * a [k];
		}
		s1 = 0.0;
		for (integer k = 1; k <= n - m; k ++)
			s1 += x [k + m] * x [k];
		r [1] = 2.0 * s1;
		delta += r [1];
		q2 = - delta / gain;
		a [m] = q2;
		for (integer k = 1; k <= m / 2; k ++) {
			s1 = a [k];
			a [k] += q2 * a [m - k];
			if (k == m - k)
				continue;
			a [m - k] += q2 * s1;
		}
		y1 = q2 * q2;
		gain *= 1.0 - y1;
		if (y1 >= 1.0) {
			frameAnalysisInfo = 3;
			break; // |a [m]| > 1
		}
		if (gain < e0 * tol1) {
			frameAnalysisInfo = 4;
			break;
		}
		if (eOld - gain < eOld * tol2) {
			frameAnalysisInfo = 5;
			break;
		}
	}
end:
	lpcf -> gain = gain * 0.5;   // because e0 is twice the energy
	lpcf -> a.resize (m);
	lpcf -> nCoefficients = m;   // maintain invariant
	return frameAnalysisInfo == 0 || frameAnalysisInfo == 4 || frameAnalysisInfo == 5;
}

/*********************** robust method ******************************/

Thing_implement (LPCAndSoundFramesIntoLPCFrameRobust, SoundFrameIntoLPCFrame, 0);

autoLPCAndSoundFramesIntoLPCFrameRobust LPCAndSoundFramesIntoLPCFrameRobust_create (constLPC inputlpc, constSound input,
	mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax,
	double tol, double location, bool wantlocation)
{
	try {
		autoLPCAndSoundFramesIntoLPCFrameRobust me = Thing_new (LPCAndSoundFramesIntoLPCFrameRobust);
		SoundFrameIntoLPCFrame_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my inputlpc = inputlpc;
		my k_stdev = k_stdev;
		my itermax = itermax;
		my tol1 = tol;
		my tol2 = 1e-10;
		my location = location;
		my wantlocation = wantlocation;
		my error = raw_VEC (my soundFrameSize);
		my sampleWeights = raw_VEC (my soundFrameSize);
		my coefficients = raw_VEC (my order);
		my covariancesw = raw_VEC (my order);
		my covarmatrixw = raw_MAT (my order, my order);
		my svd = SVD_create (my order, my order);
		SVD_setTolerance (my svd.get(), my tol2);
		my computedSVDworksize = SVD_getWorkspaceSize (my svd.get());
		my svdwork1 = raw_VEC (my computedSVDworksize);
		my svdwork2 = raw_VEC (my order);
		my filterMemory = raw_VEC (my order);
		my huberwork = raw_VEC (my soundFrameSize);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoLPCFrameRobust.");
	}
}

void LPCAndSoundFramesIntoLPCFrameRobust_resize (mutableLPCAndSoundFramesIntoLPCFrameRobust me) {
	if (my currentPredictionOrder == my svd -> numberOfColumns)
		return;
	Melder_assert (my currentPredictionOrder <= my order);
	my coefficients.resize (my currentPredictionOrder);
	my covariancesw.resize (my currentPredictionOrder);
	my covarmatrixw.resize (my currentPredictionOrder, my currentPredictionOrder);
	SVD_resizeWithinOldBounds (my svd.get(), my order, my order, my currentPredictionOrder, my currentPredictionOrder);
}

static void LPCAndSoundFramesIntoLPCFrameRobust_setSampleWeights (mutableLPCAndSoundFramesIntoLPCFrameRobust me, constVEC const& error) {
	const double kstdev = my k_stdev * my scale;
	for (integer isamp = 1 ; isamp <= error.size; isamp ++) {
		const double absDiff = fabs (error [isamp] - my location);
		my sampleWeights [isamp] = ( absDiff <= kstdev ? 1.0 : kstdev / absDiff );
	}
}

static void LPCAndSoundFramesIntoLPCFrameRobust_setCovariances (mutableLPCAndSoundFramesIntoLPCFrameRobust me, constVEC const& s) {
	MATVU covar = MATVU (my covarmatrixw.part (1, my currentPredictionOrder, 1, my currentPredictionOrder));
	for (integer i = 1; i <= my currentPredictionOrder; i ++) {
		for (integer j = i; j <= my currentPredictionOrder; j ++) {
			longdouble cv1 = 0.0;
			/*
				The following inner loop will need the most CPU time of all the robust calculations
				
					for (integer k = my currentPredictionOrder + 1; k <= s.size; k ++)
						cv1 += s [k - j] * s [k - i] *  my sampleWeights [k];
				
				The following code speeds it up from 23.24% to 18.32% of the total CPU time used
				(sound with 44100 Hz sampling frequency, 0.025 s window length)
			*/
			const double *skmj = & s [my currentPredictionOrder - j];
			const double *skmi = & s [my currentPredictionOrder - i];
			const double *sw = & my sampleWeights [my currentPredictionOrder];
			for (integer k = 1; k <= s.size - my currentPredictionOrder; k ++)
				cv1 += *++skmj * *++skmi * *++sw;
			covar [i] [j] = covar [j] [i] = (double) cv1;
		}
		longdouble cv2 = 0.0;
		for (integer k = my currentPredictionOrder + 1; k <= s.size; k ++)
			cv2 += s [k - i] * s [k] *  my sampleWeights [k];
		my covariancesw [i] = - cv2;
	}
}

static void LPCAndSoundFramesIntoLPCFrameRobust_solvelpc (mutableLPCAndSoundFramesIntoLPCFrameRobust me) {
	my svd -> u.all()  <<=  my covarmatrixw.all();
	my svdwork2.resize (my currentPredictionOrder);
	SVD_compute (my svd.get(), my svdwork1.get());
	SVD_solve_preallocated (my svd.get(), my covariancesw.get(), my coefficients.get(), my svdwork2.get());
	my coefficients.resize (my currentPredictionOrder); // maintain invariant
}

bool structLPCAndSoundFramesIntoLPCFrameRobust :: inputFrameToOutputFrame () {
	LPC_Frame inputlpcf = & inputlpc -> d_frames [currentFrame];
	LPC_Frame outputlpcf = & outputlpc -> d_frames [currentFrame];
	currentPredictionOrder = inputlpcf -> nCoefficients;
	for (integer i = 1; i <= inputlpcf -> nCoefficients; i ++)
		outputlpcf -> a[i] = inputlpcf -> a [i];
	if (currentPredictionOrder == 0) // is empty frame ?
		return true;
	outputlpcf -> gain = inputlpcf -> gain;
	
	VEC inout_a = outputlpcf -> a.part (1, currentPredictionOrder);
	iter = 0;
	scale = 1e308;
	bool farFromScale = true;
	LPCAndSoundFramesIntoLPCFrameRobust_resize (this);
	filterMemory.resize (currentPredictionOrder);
	frameAnalysisInfo = 0;
	do {
		const double previousScale = scale;
		error.all()  <<=  soundFrame;
		VECfilterInverse_inplace (error.get(), inout_a, filterMemory.get());
		NUMstatistics_huber (error.get(), & location, wantlocation, & scale, wantscale, k_stdev, tol1, huber_iterations, huberwork.get());
		LPCAndSoundFramesIntoLPCFrameRobust_setSampleWeights (this, error.get());

		LPCAndSoundFramesIntoLPCFrameRobust_setCovariances (this, soundFrame);
		/*
			Solve C a = [-] c
		*/
		try {
			LPCAndSoundFramesIntoLPCFrameRobust_solvelpc (this);
			inout_a  <<=  coefficients.all();
			farFromScale = ( fabs (scale - previousScale) > std::max (tol1 * fabs (scale), NUMeps) );
		} catch (MelderError) {
			Melder_clearError (); // No change could be made
			frameAnalysisInfo = 2; // solvelpc in error
			inputlpcf -> copy (outputlpcf);
			return false;
		}
	} while (++ iter < itermax && farFromScale);
	frameAnalysisInfo = 3; // maximum number of iterations
	return true;
}

Thing_implement (SoundFrameIntoLPCFrameRobust, SoundFrameIntoLPCFrame, 0);

bool structSoundFrameIntoLPCFrameRobust :: inputFrameToOutputFrame () {
	bool step1 = soundIntoLPC -> inputFrameToOutputFrame ();
	soundIntoLPC -> saveOutputFrame ();
	bool step2 = lpcAndSoundIntoLPC -> inputFrameToOutputFrame ();
	return step1 && step2;
}

void structSoundFrameIntoLPCFrameRobust :: saveOutputFrame () {
	lpcAndSoundIntoLPC -> saveOutputFrame ();
}
autoSoundFrameIntoLPCFrameRobust SoundFrameIntoLPCFrameRobust_create (autoSoundFrameIntoLPCFrame soundIntoLPC, autoLPCAndSoundFramesIntoLPCFrameRobust lpcAndSoundIntoLPC)
{
	try {
		autoSoundFrameIntoLPCFrameRobust me = Thing_new (SoundFrameIntoLPCFrameRobust);
		my soundIntoLPC.adoptFromAmbiguousOwner (soundIntoLPC.releaseToAmbiguousOwner());
		my lpcAndSoundIntoLPC.adoptFromAmbiguousOwner (lpcAndSoundIntoLPC.releaseToAmbiguousOwner());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SoundFrameIntoLPCFrameRobust.");
	}
}
