/* SoundToLPCWorkspace.cpp
 *
 * Copyright (C) 2024 David Weenink
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

#include "SoundToLPCWorkspace.h"

#include "oo_DESTROY.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_COPY.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SoundToLPCWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundToLPCWorkspace_def.h"

Thing_implement (SoundToLPCWorkspace, SoundToSampledWorkspace, 0);

void structSoundToLPCWorkspace :: allocateOutputFrames (void) {
	if (! outputObjectPresent)
		return;
	LPC thee = reinterpret_cast<LPC> (output);
	for (integer iframe = 1; iframe <= thy nx; iframe ++)
		LPC_Frame_init (& thy d_frames [iframe], thy maxnCoefficients);
}

void structSoundToLPCWorkspace :: saveOutputFrame (void) {
	if (! outputObjectPresent)
		return;
	LPC me = reinterpret_cast<LPC> (output);
	outputLPCFrameRef -> copy (& my d_frames [currentFrame]);
}

bool structSoundToLPCWorkspace :: inputFrameToOutputFrame (void) {
	return true;
}

void SoundToLPCWorkspace_init (mutableSoundToLPCWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients)
{
	SoundToSampledWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape);
	my samplingPeriod = samplingPeriod;
	my maxnCoefficients = maxnCoefficients;
	/*
		We always need one frame for output
	*/
	LPC_Frame_init (& my outputLPCFrame, my maxnCoefficients);
}

void SoundToLPCWorkspace_initSkeleton (mutableSoundToLPCWorkspace me, constSound input, mutableLPC output) {
	SoundToSampledWorkspace_initSkeleton (me, input, output);
}

/************************ autocorrelation method *****************************/

Thing_implement (SoundToLPCAutocorrelationWorkspace, SoundToLPCWorkspace, 0);

/*
	Markel & Gray, Linear Prediction of Speech, page 219
*/

bool structSoundToLPCAutocorrelationWorkspace :: inputFrameToOutputFrame (void)  {
	const integer m = maxnCoefficients;
	
	LPC_Frame thee = outputLPCFrameRef;
	frameAnalysisInfo = 0;
	if (thy nCoefficients == 0) {
		frameAnalysisInfo = 6;
		return false;
	}
	VEC  r = workvectorPool -> getRawVEC (1, m + 1);
	VEC  a = workvectorPool -> getRawVEC (2, m + 1);
	VEC rc = workvectorPool -> getRawVEC (3, m);
	VEC x = soundFrameVEC;
	
	/*
		Compute the autocorrelations
	*/
	thy a.get()  <<=  0.0;
	for (integer i = 1; i <= m + 1; i ++)
		r [i] = NUMinner (x.part (1, x.size - i + 1), x.part (i, x.size));
	if (r [1] == 0.0) {
		/*
			The sound frame contains only zero's
		*/
		thy nCoefficients = 0;
		thy a.resize (thy nCoefficients); // maintain invariant
		frameAnalysisInfo = 1;
		return false;
	}
	a [1] = 1.0;
	a [2] = rc [1] = - r [2] / r [1];
	thy gain = r [1] + r [2] * rc [1];
	integer iend = 1;
	for (integer i = 2; i <= m; i ++) {
		long double s = 0.0;
		for (integer j = 1; j <= i; j ++)
			s += r [i - j + 2] * a [j];
		rc [i] = - s / thy gain;
		for (integer j = 2; j <= i / 2 + 1; j ++) {
			const double at = a [j] + rc [i] * a [i - j + 2];
			a [i - j + 2] += rc [i] * a [j];
			a [j] = at;
		}
		a [i + 1] = rc [i];
        thy gain += rc [i] * s;
		if (thy gain <= 0.0) {
			frameAnalysisInfo = 2;
			break;
		}
		iend ++;
	}
	thy a.part (1, iend)  <<=  a.part (2, iend + 1);
	thy a.resize (iend);
	thy nCoefficients = thy a.size; // maintain invariant
	return true;
}

void SoundToLPCAutocorrelationWorkspace_init (SoundToLPCAutocorrelationWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients)
{
	SoundToLPCWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	autoINTVEC workvectorSizes { my maxnCoefficients + 1, my maxnCoefficients + 1, my maxnCoefficients };
	my workvectorPool = WorkvectorPool_create (workvectorSizes.get(), true);
}

autoSoundToLPCAutocorrelationWorkspace SoundToLPCAutocorrelationWorkspace_createSkeleton (constSound input, mutableLPC output) {
	autoSoundToLPCAutocorrelationWorkspace me = Thing_new (SoundToLPCAutocorrelationWorkspace);
	SoundToLPCWorkspace_initSkeleton (me.get(), input, output);
	return me;
}

autoSoundToLPCAutocorrelationWorkspace SoundToLPCAutocorrelationWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToLPCAutocorrelationWorkspace me = SoundToLPCAutocorrelationWorkspace_createSkeleton (input, output);
		SoundToLPCAutocorrelationWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape, output -> maxnCoefficients);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCAutocorrelationWorkspace could not be created.");
	}
}

/************************ covariance method *****************************/

Thing_implement (SoundToLPCCovarianceWorkspace, SoundToLPCWorkspace, 0);

/*
	Markel & Gray, Linear Prediction of Speech, page 221
*/

bool structSoundToLPCCovarianceWorkspace :: inputFrameToOutputFrame () {
	LPC_Frame thee = outputLPCFrameRef;
	const integer m = maxnCoefficients, n = soundFrameSize;
	
	if (thy nCoefficients == 0) {
		frameAnalysisInfo = 6;
		return false;
	}
		
	VEC b = workvectorPool -> getZeroVEC (1, m * (m + 1) / 2);
	VEC grc = workvectorPool -> getRawVEC (2, m);
	VEC beta = workvectorPool -> getRawVEC (3, m);
	VEC a = workvectorPool -> getRawVEC (4, m + 1);
	VEC cc = workvectorPool -> getZeroVEC (5, m + 1);
	VEC x = soundFrameVEC;
	
	frameAnalysisInfo = 0;
	thy gain = 0.0;
	/*
		Compute the covariances
	*/
	VEC xi = x.part(m + 1, n), xim1 = x.part(m, n - 1);
	thy gain = NUMinner (xi, xi);
	cc [1] = NUMinner (xi, xim1);
	cc [2] = NUMinner (xim1, xim1);

	if (thy gain == 0.0) {
		frameAnalysisInfo = 1;
		thy nCoefficients = 0;
		thy a.resize (thy nCoefficients); //maintain invariant
		return false;
	}

	b [1] = 1.0;
	beta [1] = cc [2];
	a [1] = 1.0;
	a [2] = grc [1] = -cc [1] / cc [2];
	thy gain += grc [1] * cc [1];
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
		thy gain -= s;
		if (thy gain <= 0.0) {
			frameAnalysisInfo = 4;
			break;
		}
		iend ++;
	}
end:
	thy a.resize (iend);
	thy a.part (1, iend)  <<=  a.part (2, iend + 1);
	thy nCoefficients = thy a.size; // maintain invariant
	return true;
}

void SoundToLPCCovarianceWorkspace_init (SoundToLPCCovarianceWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients)
{
	SoundToLPCWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	autoINTVEC workvectorSizes { maxnCoefficients * (maxnCoefficients + 1) / 2, maxnCoefficients, maxnCoefficients,
			maxnCoefficients + 1, maxnCoefficients + 1
	};
	my workvectorPool = WorkvectorPool_create (workvectorSizes.get(), true);
}

autoSoundToLPCCovarianceWorkspace SoundToLPCCovarianceWorkspace_createSkeleton (constSound input, mutableLPC output) {
	autoSoundToLPCCovarianceWorkspace me = Thing_new (SoundToLPCCovarianceWorkspace);
	SoundToLPCWorkspace_initSkeleton (me.get(), input, output);
	return me;
}

autoSoundToLPCCovarianceWorkspace SoundToLPCCovarianceWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToLPCCovarianceWorkspace me = SoundToLPCCovarianceWorkspace_createSkeleton (input, output);
		SoundToLPCCovarianceWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape, output -> maxnCoefficients);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCCovarianceWorkspace could not be created.");
	}
}

/************************ burg method *****************************/

Thing_implement (SoundToLPCBurgWorkspace, SoundToLPCWorkspace, 0);

static double VECburg_buffered (VEC const& a, constVEC const& x, SoundToSampledWorkspace me) {
	const integer n = x.size, m = a.size;

	a   <<=  0.0; // necessary??
	if (n <= 2) {
		a [1] = -1.0;
		return ( n == 2 ? 0.5 * (x [1] * x [1] + x [2] * x [2]) : x [1] * x [1] );
	}
	VEC b1 = my workvectorPool -> getRawVEC (1, n);
	VEC b2 = my workvectorPool -> getRawVEC (2, n);
	VEC aa = my workvectorPool -> getRawVEC (3, m + 1);

	// (3)

	double p = NUMinner (x,x);

	if (p == 0.0) {
		my frameAnalysisInfo = 1;
		return 0.0;
	}
	// (9)

	b1 [1] = x [1];
	b2 [n - 1] = x [n];
	for (integer j = 2; j <= n - 1; j ++)
		b1 [j] = b2 [j - 1] = x [j];

	longdouble xms = p / n;
	for (integer i = 1; i <= m; i ++) {
		// (7)

		longdouble num = 0.0, denum = 0.0;
		for (integer j = 1; j <= n - i; j ++) {
			num += b1 [j] * b2 [j];
			denum += b1 [j] * b1 [j] + b2 [j] * b2 [j];
		}

		if (denum <= 0.0) {
			my frameAnalysisInfo = 1;
			return 0.0;	// warning ill-conditioned
		}
		a [i] = 2.0 * double (num / denum);

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

bool structSoundToLPCBurgWorkspace :: inputFrameToOutputFrame () {

	VEC x = soundFrameVEC;
	LPC_Frame thee = outputLPCFrameRef;

	thy gain = VECburg_buffered (thy a.get(), x, this);
	if (thy gain <= 0.0) {
		thy nCoefficients = 0;
		thy a.resize (thy nCoefficients); // maintain invariant
		return false;
	} else {
		thy gain *= soundFrame.size;
		for (integer i = 1; i <= thy nCoefficients; i ++)
			thy a [i] = - thy a [i];
		return true;
	}
}


void SoundToLPCBurgWorkspace_init (SoundToLPCBurgWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients)
{
	SoundToLPCWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	autoINTVEC workvectorSizes { my soundFrameSize, my soundFrameSize, maxnCoefficients + 1 };
	my workvectorPool = WorkvectorPool_create (workvectorSizes.get(), true);
}

autoSoundToLPCBurgWorkspace SoundToLPCBurgWorkspace_createSkeleton (constSound input, mutableLPC output) {
	autoSoundToLPCBurgWorkspace me = Thing_new (SoundToLPCBurgWorkspace);
	SoundToLPCWorkspace_initSkeleton (me.get(), input, output);
	return me;
}

autoSoundToLPCBurgWorkspace SoundToLPCBurgWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToLPCBurgWorkspace me = SoundToLPCBurgWorkspace_createSkeleton (input, output);
		SoundToLPCBurgWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape, output -> maxnCoefficients);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCBurgWorkspace could not be created.");
	}
}

/************************ marple method *****************************/

Thing_implement (SoundToLPCMarpleWorkspace, SoundToLPCWorkspace, 0);

bool structSoundToLPCMarpleWorkspace :: inputFrameToOutputFrame () {

	const integer mmax = maxnCoefficients, n = soundFrame.size;
	LPC_Frame thee = outputLPCFrameRef;
	VEC x = soundFrameVEC;
	
	frameAnalysisInfo = 0;
	VEC c = workvectorPool -> getRawVEC (1, mmax + 1);
	VEC d = workvectorPool -> getRawVEC (2, mmax + 1);
	VEC r = workvectorPool -> getRawVEC (3, mmax + 1);
	VEC a = outputLPCFrame.a.get();

	double gain = 0.0, e0 = 2.0 * NUMsum2 (x);
	integer m = 1;
	if (e0 == 0.0) {
		thy nCoefficients = 0;
		thy a.resize (thy nCoefficients); // maintain invariant
		thy gain = gain;
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
	thy gain = gain * 0.5;   // because e0 is twice the energy
	thy a.resize (m);
	thy nCoefficients = m;   // maintain invariant
	return frameAnalysisInfo == 0 || frameAnalysisInfo == 4 || frameAnalysisInfo == 5;
}

void SoundToLPCMarpleWorkspace_init (SoundToLPCMarpleWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients,
	double tol1, double tol2)
{
	SoundToLPCWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	autoINTVEC workvectorSizes { maxnCoefficients + 1, maxnCoefficients + 1, maxnCoefficients + 1 };
	my workvectorPool = WorkvectorPool_create (workvectorSizes.get(), true);
	my tol1 = tol1;
	my tol2 = tol2;
}

autoSoundToLPCMarpleWorkspace SoundToLPCMarpleWorkspace_createSkeleton (constSound input, mutableLPC output) {
	autoSoundToLPCMarpleWorkspace me = Thing_new (SoundToLPCMarpleWorkspace);
	SoundToLPCWorkspace_initSkeleton (me.get(), input, output);
	return me;
}

autoSoundToLPCMarpleWorkspace SoundToLPCMarpleWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double tol1, double tol2)
{
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToLPCMarpleWorkspace me = SoundToLPCMarpleWorkspace_createSkeleton (input, output);
		SoundToLPCMarpleWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape, output -> maxnCoefficients, tol1, tol2);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCMarpleWorkspace could not be created.");
	}
}

/*********************** robust method ******************************/

Thing_implement (SoundAndLPCToLPCRobustWorkspace, SoundToLPCWorkspace, 0);

void SoundAndLPCToLPCRobustWorkspace_resize (mutableSoundAndLPCToLPCRobustWorkspace me, integer newPredictionOrder) {
	if (newPredictionOrder == my svd -> numberOfColumns)
		return;
	Melder_assert (newPredictionOrder <= my maxnCoefficients);
	my coefficients.resize (newPredictionOrder);
	my covariancesw.resize (newPredictionOrder);
	my covarmatrixw.resize (newPredictionOrder, newPredictionOrder);
	SVD_resizeWithinOldBounds (my svd.get(), my maxnCoefficients, my maxnCoefficients, newPredictionOrder, newPredictionOrder);
}

static void SoundAndLPCToLPCRobustWorkspace_setSampleWeights (mutableSoundAndLPCToLPCRobustWorkspace me, constVEC const& error) {
	const double kstdev = my k_stdev * my scale;
	for (integer isamp = 1 ; isamp <= error.size; isamp ++) {
		const double absDiff = fabs (error [isamp] - my location);
		my sampleWeights [isamp] = ( absDiff <= kstdev ? 1.0 : kstdev / absDiff );
	}
}

static void SoundAndLPCToLPCRobustWorkspace_setCovariances (mutableSoundAndLPCToLPCRobustWorkspace me, constVEC const& s) {
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

static void SoundAndLPCToLPCRobustWorkspace_solvelpc (mutableSoundAndLPCToLPCRobustWorkspace me) {
	my svd -> u.all()  <<=  my covarmatrixw.all();
	VEC workvector1 = my workvectorPool -> getRawVEC (1, my computeSVDworksize);
	VEC workvector2 = my workvectorPool -> getRawVEC (2, my currentPredictionOrder);
	SVD_compute (my svd.get(), workvector1);
	SVD_solve_preallocated (my svd.get(), my covariancesw.get(), my coefficients.get(), workvector2);
	my coefficients.resize (my currentPredictionOrder); // maintain invariant
}

void structSoundAndLPCToLPCRobustWorkspace :: getInputFrame () {
	SoundAndLPCToLPCRobustWorkspace_Parent :: getInputFrame (); // soundFrame
	if (otherInputObjectPresent)
		otherInputLPCFrameRef = & otherInput -> d_frames [currentFrame];
}

bool structSoundAndLPCToLPCRobustWorkspace :: inputFrameToOutputFrame () {
	currentPredictionOrder = otherInputLPCFrameRef -> nCoefficients;
	otherInputLPCFrameRef -> copy (outputLPCFrameRef); // outputLPCFrame should already exist here!
	if (currentPredictionOrder == 0) // is empty frame ?
		return true;
	LPC_Frame thee = outputLPCFrameRef;
	
	VEC inout_a = thy a.part (1, currentPredictionOrder);
	iter = 0;
	scale = 1e308;
	bool farFromScale = true;
	SoundAndLPCToLPCRobustWorkspace_resize (this, currentPredictionOrder);
	VEC filterMemory = workvectorPool -> getRawVEC (3, currentPredictionOrder);
	VEC workHuber = workvectorPool -> getRawVEC (4, soundFrameVEC.size);
	frameAnalysisInfo = 0;
	do {
		const double previousScale = scale;
		error.all()  <<=  soundFrameVEC;
		VECfilterInverse_inplace (error.get(), inout_a, filterMemory);
		NUMstatistics_huber (error.get(), & location, wantlocation, & scale, wantscale, k_stdev, tol1, huber_iterations, workHuber);
		SoundAndLPCToLPCRobustWorkspace_setSampleWeights (this, error.get());

		SoundAndLPCToLPCRobustWorkspace_setCovariances (this, soundFrameVEC);
		/*
			Solve C a = [-] c
		*/
		try {
			SoundAndLPCToLPCRobustWorkspace_solvelpc (this);
			inout_a  <<=  coefficients.all();
			farFromScale = ( fabs (scale - previousScale) > std::max (tol1 * fabs (scale), NUMeps) );
		} catch (MelderError) {
			Melder_clearError (); // No change could be made
			frameAnalysisInfo = 2; // solvelpc in error
			otherInputLPCFrameRef -> copy (outputLPCFrameRef);
			return false;
		}
	} while (++ iter < itermax && farFromScale);
	frameAnalysisInfo = 3; // maximum number of iterations
	
	return true;
}

void structSoundAndLPCToLPCRobustWorkspace :: saveOutputFrame () {
	if (! outputObjectPresent)
		return;
	mutableLPC thee = reinterpret_cast<mutableLPC> (output);
	outputLPCFrame.copy (& thy d_frames [currentFrame]);
}

void SoundAndLPCToLPCRobustWorkspace_init (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	integer maxnCoefficients, double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	SoundToLPCWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	/*
		pool that depends on the sound
	*/
	my error = zero_VEC (my soundFrameSize);
	my sampleWeights = zero_VEC (my soundFrameSize);
	/*
		Initialise the working memories whose sizes depend on the otherInput LPC
	*/
	my currentPredictionOrder = my maxnCoefficients;
	LPC_Frame_init (& my otherInputLPCFrame, maxnCoefficients);
	my coefficients = raw_VEC (maxnCoefficients);
	my covariancesw = zero_VEC (maxnCoefficients);
	my covarmatrixw = zero_MAT (maxnCoefficients, maxnCoefficients);
	my svd = SVD_create (maxnCoefficients, maxnCoefficients);
	SVD_setTolerance (my svd.get(), my tol2);
	my computeSVDworksize = SVD_getWorkspaceSize (my svd.get());
	/*
		1:Compute SVD, 2:SVDSolve, 3:InverseFiltering, 4: Huber
	*/
	autoINTVEC vectorSizes {my computeSVDworksize, maxnCoefficients, maxnCoefficients, my soundFrameSize};
	my workvectorPool = WorkvectorPool_create (vectorSizes.get(), true);
	/*
		Initialise the working memory that does not depend on the existence of the LPC objects nor the input
	*/
	my minimumNumberOfFramesPerThread /= 2;
	my k_stdev = k_stdev;
	my scale = 0.0;
	my iter = 0;
	my itermax = itermax;
	my wantlocation = wantlocation;
	if (! wantlocation)
		my location = location;
	my wantscale = true;
	my huber_iterations = 5;
	my tol1 = tol;
	my tol2 = 1e-10;
}

autoSoundAndLPCToLPCRobustWorkspace SoundAndLPCToLPCRobustWorkspace_createSkeleton (
	constSound input, constLPC otherInput, mutableLPC output)
{
		autoSoundAndLPCToLPCRobustWorkspace me = Thing_new (SoundAndLPCToLPCRobustWorkspace);
		SoundToLPCWorkspace_initSkeleton (me.get(), input, output);
		if (otherInput) {
			my otherInput = otherInput;
			my otherInputObjectPresent = true;
		}
		return me;
}

autoSoundAndLPCToLPCRobustWorkspace SoundAndLPCToLPCRobustWorkspace_create (constSound input, constLPC otherInput, mutableLPC output, 
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	try {
		Sampled_assertEqualDomains (input, otherInput);
		Sampled_assertEqualDomainsAndSampling (otherInput, output);
		autoSoundAndLPCToLPCRobustWorkspace me = SoundAndLPCToLPCRobustWorkspace_createSkeleton (input, otherInput, output);
		SoundAndLPCToLPCRobustWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape,
			output -> maxnCoefficients, k_stdev,  itermax, tol, location, wantlocation);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCMarpleWorkspace could not be created.");
	}
}

Thing_implement (SoundToLPCRobustWorkspace, SoundToLPCWorkspace, 0);

void structSoundToLPCRobustWorkspace :: getInputFrame (void) {
	SoundToLPCRobustWorkspace_Parent :: getInputFrame ();
}

void structSoundToLPCRobustWorkspace :: allocateOutputFrames (void) {
	soundAndLPCToLPC -> allocateOutputFrames ();
}

bool structSoundToLPCRobustWorkspace :: inputFrameToOutputFrame (void) {
	soundToLPC -> currentFrame = currentFrame;
	soundToLPC -> soundFrameVEC = soundFrameVEC;
	bool step1 = soundToLPC -> inputFrameToOutputFrame ();
	soundAndLPCToLPC -> currentFrame = currentFrame;
	soundAndLPCToLPC -> soundFrameVEC = soundFrameVEC;
	soundAndLPCToLPC -> otherInputLPCFrameRef = soundToLPC -> outputLPCFrameRef;
	bool step2 = soundAndLPCToLPC -> inputFrameToOutputFrame ();
	outputLPCFrameRef = soundAndLPCToLPC -> outputLPCFrameRef;
	return step1 && step2;
}

void structSoundToLPCRobustWorkspace :: saveOutputFrame (void) {
	if (! outputObjectPresent)
		return;
	LPC me = reinterpret_cast<LPC> (output);
	soundAndLPCToLPC -> outputLPCFrame.copy (& my d_frames [currentFrame]);
}

void SoundToLPCRobustWorkspace_init (SoundToLPCRobustWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	integer maxnCoefficients,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation)
{
	SoundToLPCWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	SoundToLPCAutocorrelationWorkspace stla = reinterpret_cast<SoundToLPCAutocorrelationWorkspace> (my soundToLPC.get());
	SoundToLPCAutocorrelationWorkspace_init (stla, samplingPeriod, effectiveAnalysisWidth, windowShape, maxnCoefficients);
	SoundAndLPCToLPCRobustWorkspace_init (my soundAndLPCToLPC.get(), samplingPeriod, effectiveAnalysisWidth,
		windowShape, maxnCoefficients, k_stdev, itermax, tol, location, wantlocation);
}

autoSoundToLPCRobustWorkspace SoundToLPCRobustWorkspace_createSkeleton (constSound input, mutableLPC output) {
	autoSoundToLPCRobustWorkspace me = Thing_new (SoundToLPCRobustWorkspace);
	SoundToLPCWorkspace_initSkeleton (me.get(), input, output);
	my soundToLPC = SoundToLPCAutocorrelationWorkspace_createSkeleton (nullptr, nullptr);
	my soundAndLPCToLPC = SoundAndLPCToLPCRobustWorkspace_createSkeleton (nullptr, nullptr, output);
	return me;
}

autoSoundToLPCRobustWorkspace SoundToLPCRobustWorkspace_create (constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation) {
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToLPCRobustWorkspace me = SoundToLPCRobustWorkspace_createSkeleton (input, output);
		SoundToLPCRobustWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape,
			output -> maxnCoefficients, k_stdev, itermax, tol, location, wantlocation);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCRobustWorkspace not created");
	}
}

/* End of file SoundToLPCWorkspace.cpp */
