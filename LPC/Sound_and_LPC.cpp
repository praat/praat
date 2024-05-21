/* Sound_and_LPC.cpp
 *
 * Copyright (C) 1994-2024 David Weenink
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
 djmw 20020625 GPL header
*/

#include "Sound_and_LPC.h"
#include "Sound_extensions.h"
#include "Vector.h"
#include "Spectrum.h"
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_COPY.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SoundToLPCAnalysisWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundToLPCAnalysisWorkspace_def.h"

Thing_implement (SoundToLPCAnalysisWorkspace, SoundAnalysisWorkspace, 0);

static void Sound_and_LPC_require_equalDomainsAndSamplingPeriods (Sound me, LPC thee) {
	Melder_require (my dx == thy samplingPeriod,
			U"The sampling periods of the Sound and the LPC should be equal.");
	Melder_require (my xmin == thy xmin && thy xmax == my xmax,
			U"The domains of the Sound and the LPC should be equal.");
}

autoSoundToLPCAnalysisWorkspace SoundToLPCAnalysisWorkspace_create (Sound thee, LPC him, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	try {
		Melder_assert (thy dx == his samplingPeriod);
		Melder_assert (thy xmin == his xmin);
		Melder_assert (thy xmax = his xmax);
		autoSoundToLPCAnalysisWorkspace me = Thing_new (SoundToLPCAnalysisWorkspace);
		SoundAnalysisWorkspace_init (me.get(), thee, him, effectiveAnalysisWidth, windowShape);
		my allocateSampledFrames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCAnalysisWorkspace not created.");
	}
}

autoLPC LPC_createEmptyFromAnalysisSpecifications (Sound me, int predictionOrder, double physicalAnalysisWidth, double dt) {
	try {
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);		
		integer numberOfFrames;
		double t1;
		Sampled_shortTermAnalysis (me, physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		autoLPC thee = LPC_create (my xmin, my xmax, numberOfFrames, dt, t1, predictionOrder, my dx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": LPC not created from specification.");
	}
}

static void SoundToLPCAnalysisWorkspace_allocate_v1To5 (SoundToLPCAnalysisWorkspace me, integer v1workSize, integer v2workSize, integer v3workSize, 
	integer v4workSize, integer v5workSize)
{
	if (v1workSize > 0) {
		my v1work = raw_VEC (v1workSize);
		my v1workSize = v1workSize;
	}
	if (v2workSize > 0) {
		my v2work = raw_VEC (v2workSize);
		my v2workSize = v2workSize;
	}
	if (v3workSize > 0) {
		my v3work = raw_VEC (v3workSize);
		my v3workSize = v3workSize;
	}
	if (v4workSize > 0) {
		my v4work = raw_VEC (v4workSize);
		my v4workSize = v4workSize;
	}
	if (v5workSize > 0) {
		my v5work = raw_VEC (v5workSize);
		my v5workSize = v5workSize;
	}

}

/************************ autocorrelation method *****************************/

/* 
	Markel & Gray, Linear Prediction of Speech, page 219
*/

static void analyseOneFrame_auto (SoundAnalysisWorkspace saw, integer iframe) {
	SoundToLPCAnalysisWorkspace me = reinterpret_cast<SoundToLPCAnalysisWorkspace> (saw);
	LPC result = reinterpret_cast<LPC> (my result);
	Melder_assert (iframe > 0 && iframe <= result -> nx);
	LPC_Frame thee = & result -> d_frames [iframe];
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	const integer m = thy nCoefficients;

	VEC  r = my v1work.get();
	VEC  a = my v2work.get();
	VEC rc = my v3work.get();
	integer i = 1; // For error condition at end:
	for (i = 1; i <= m + 1; i ++)
		r [i] = NUMinner (my analysisFrame.part (1, my analysisFrame.size - i + 1), my analysisFrame.part (i, my analysisFrame.size));
	if (r [1] == 0.0) {
		i = 1; // !
		goto end;
	}
	a [1] = 1.0;
	a [2] = rc [1] = - r [2] / r [1];
	thy gain = r [1] + r [2] * rc [1];
	for (i = 2; i <= m; i ++) {
		double s = 0.0;
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
		if (thy gain <= 0.0)
			goto end;
	}
end:
	i --;
	for (integer j = 1; j <= i; j ++)
		thy a [j] = a [j + 1];
	if (i == m)
		my frameAnalysisIsOK = false;
	thy a.resize (i);
	thy nCoefficients = thy a.size; // maintain invariant
	my frameAnalysisIsOK = true;
}

void Sound_into_LPC_auto (Sound me, LPC thee, double effectiveAnalysisWidth, double preEmphasisFrequency) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCAnalysisWorkspace ws =  SoundToLPCAnalysisWorkspace_create (me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	SoundToLPCAnalysisWorkspace_allocate_v1To5 (ws.get(), thy maxnCoefficients + 1, thy maxnCoefficients + 1, thy maxnCoefficients, 0, 0);
	ws -> analyseOneFrame = analyseOneFrame_auto;
	SoundAnalysisWorkspace_analyseThreaded (ws.get(), me, preEmphasisFrequency);
}

autoLPC Sound_to_LPC_auto (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (me, predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_auto (me, thee.get(), effectiveAnalysisWidth, preEmphasisFrequency);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (auto) created.");
	}
}

/************************ covariance method *****************************/

/*
	Markel & Gray, Linear Prediction of Speech, page 221
*/

static void analyseOneFrame_covar (SoundAnalysisWorkspace saw, integer iframe) {
	SoundToLPCAnalysisWorkspace me = reinterpret_cast<SoundToLPCAnalysisWorkspace> (saw);
	LPC result = reinterpret_cast<LPC> (my result);
	Melder_assert (iframe > 0 && iframe <= result -> nx);
	LPC_Frame thee = & result -> d_frames [iframe];
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	const integer m = thy nCoefficients, n = my analysisFrameSize;
	
	VEC b = my v1work.get();		// m * (m + 1) / 2;
	VEC grc = my v2work.get();	// m
	VEC beta = my v3work.get();	// m	
	VEC a = my v4work.get();		// m + 1
	VEC cc = my v5work.get(); 	//m + 1
	VEC x = my analysisFrame.get();
	
	thy gain = 0.0;
	integer i;
	for (i = m + 1; i <= n; i ++) {
		thy gain += x [i] * x [i];
		cc [1] += x [i] * x [i - 1];
		cc [2] += x [i - 1] * x [i - 1];
	}

	if (thy gain == 0.0) {
		i = 1; // !
		goto end;
	}

	b [1] = 1.0;
	beta [1] = cc [2];
	a [1] = 1.0;
	a [2] = grc [1] = -cc [1] / cc [2];
	thy gain += grc [1] * cc [1];

	for (i = 2; i <= m; i ++) { // 130
		double s = 0.0; // 20
		for (integer j = 1; j <= i; j ++)
			cc [i - j + 2] = cc [i - j + 1] + x [m - i + 1] * x [m - i + j] - x [n - i + 1] * x [n - i + j];

		cc [1] = 0.0; // TODO NUMinner
		for (integer j = m + 1; j <= n; j ++)
			cc [1] += x [j - i] * x [j]; // 30

		b [i * (i + 1) / 2] = 1.0;
		for (integer j = 1; j <= i - 1; j ++) { // 70
			double gam = 0.0;
			if (beta [j] < 0.0)
				goto end;
			else if (beta [j] == 0.0)
				continue;

			for (integer k = 1; k <= j; k ++)
				gam += cc [k + 1] * b [j * (j - 1) / 2 + k]; // 50

			gam /= beta [j];
			for (integer k = 1; k <= j; k ++)
				b [i * (i - 1) / 2 + k] -= gam * b [j * (j - 1) / 2 + k]; // 60
		}

		beta [i] = 0.0;
		for (integer j = 1; j <= i; j ++)
			beta [i] += cc [j + 1] * b [i * (i - 1) / 2 + j]; // 80
		if (beta [i] <= 0.0)
			goto end;

		for (integer j = 1; j <= i; j ++)
			s += cc [j] * a [j]; // 100
		grc [i] = -s / beta [i];

		for (integer j = 2; j <= i; j ++)
			a [j] += grc [i] * b [i * (i - 1) / 2 + j - 1]; // 110
		a [i + 1] = grc [i];
		s = grc [i] * grc [i] * beta [i];
		thy gain -= s;
		if (thy gain <= 0.0)
			goto end;
	}
end:
	i --;
	for (integer j = 1; j <= i; j ++)
		thy a [j] = a [j + 1];
	if (i == m)
		my frameAnalysisIsOK = false;
	thy a.resize (i);
	thy nCoefficients = thy a.size;
	my frameAnalysisIsOK = true;
}

void Sound_into_LPC_covar (Sound me, LPC thee, double effectiveAnalysisWidth, double preEmphasisFrequency) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCAnalysisWorkspace ws =  SoundToLPCAnalysisWorkspace_create (me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	SoundToLPCAnalysisWorkspace_allocate_v1To5 (ws.get(), thy maxnCoefficients * (thy maxnCoefficients + 1) / 2, thy maxnCoefficients, 
		thy maxnCoefficients, thy maxnCoefficients + 1, thy maxnCoefficients + 1);
	ws -> analyseOneFrame = analyseOneFrame_covar;
	SoundAnalysisWorkspace_analyseThreaded (ws.get(), me, preEmphasisFrequency);
}

autoLPC Sound_to_LPC_covar (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (me, predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_covar (me, thee.get(), effectiveAnalysisWidth, preEmphasisFrequency);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (covar) created.");
	}
}

/************************ burg method *****************************/

static double VECburg_buffered (VEC const& a, constVEC const& x, SoundToLPCAnalysisWorkspace me) {
	const integer n = x.size, m = a.size;
	a   <<=  0.0; // necessary??
	if (n <= 2) {
		a [1] = -1.0;
		return ( n == 2 ? 0.5 * (x [1] * x [1] + x [2] * x [2]) : x [1] * x [1] );
	}
	VEC b1 = my v1work.get();	// (n);
	VEC b2 = my v2work.get();	// (n);
	VEC aa = my v3work.get();	// (m + 1);

	// (3)

	longdouble p = 0.0;
	for (integer j = 1; j <= n; j ++)
		p += x [j] * x [j];

	longdouble xms = p / n;
	if (xms <= 0.0) {
		return double (xms);	// warning empty
	}
	// (9)

	b1 [1] = x [1];
	b2 [n - 1] = x [n];
	for (integer j = 2; j <= n - 1; j ++)
		b1 [j] = b2 [j - 1] = x [j];

	for (integer i = 1; i <= m; i ++) {
		// (7)

		longdouble num = 0.0, denum = 0.0;
		for (integer j = 1; j <= n - i; j ++) {
			num += b1 [j] * b2 [j];
			denum += b1 [j] * b1 [j] + b2 [j] * b2 [j];
		}

		if (denum <= 0.0)
			return 0.0;	// warning ill-conditioned

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

static void analyseOneFrame_burg (SoundAnalysisWorkspace saw, integer iframe) {
	SoundToLPCAnalysisWorkspace me = reinterpret_cast<SoundToLPCAnalysisWorkspace> (saw);
	LPC result = reinterpret_cast<LPC> (my result);
	Melder_assert (iframe > 0 && iframe <= result -> nx);
	LPC_Frame thee = & result -> d_frames [iframe];
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	
	thy gain = VECburg_buffered (thy a.get(), my analysisFrame.get(), me);
	if (thy gain <= 0.0) {
		thy a.resize (0);
		thy nCoefficients = thy a.size; // maintain invariant
		my frameAnalysisIsOK = false;
	} else {
		thy gain *= my analysisFrame.size;
		for (integer i = 1; i <= thy nCoefficients; i ++)
			thy a [i] = - thy a [i];
		my frameAnalysisIsOK = true;
	}
}

void Sound_into_LPC_burg (Sound me, LPC thee, double effectiveAnalysisWidth, double preEmphasisFrequency) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCAnalysisWorkspace ws =  SoundToLPCAnalysisWorkspace_create (me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	SoundToLPCAnalysisWorkspace_allocate_v1To5 (ws.get(), ws -> analysisFrameSize, ws -> analysisFrameSize, thy maxnCoefficients + 1, 0, 0);
	ws -> analyseOneFrame = analyseOneFrame_burg;
	SoundAnalysisWorkspace_analyseThreaded (ws.get(), me, preEmphasisFrequency);
}

autoLPC Sound_to_LPC_burg (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (me, predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_burg (me, thee.get(), effectiveAnalysisWidth, preEmphasisFrequency);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (burg) created.");
	}
}

/************************ marple method *****************************/

static void analyseOneFrame_marple (SoundAnalysisWorkspace saw, integer iframe) {
	SoundToLPCAnalysisWorkspace me = reinterpret_cast<SoundToLPCAnalysisWorkspace> (saw);
	LPC result = reinterpret_cast<LPC> (my result);
	Melder_assert (iframe > 0 && iframe <= result -> nx);
	LPC_Frame thee = & result -> d_frames [iframe];
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	const integer mmax = thy nCoefficients, n = my analysisFrame.size;

	int status = 0;
	VEC c = my v1work.get();	// (mmax + 1);
	VEC d = my v2work.get();	// (mmax + 1);
	VEC r = my v3work.get();	// (mmax + 1);
	VEC x = my analysisFrame.get();
	
	double e0 = 2.0 * NUMsum2 (x);
	integer m = 1;
	if (e0 == 0.0) {
		thy a.resize (0);
		thy nCoefficients = thy a.size; // maintain invariant
		thy gain = 0.0;
		my frameAnalysisIsOK = false;
		return; // warning no signal
	}
	double q1 = 1.0 / e0;
	double q2 = q1 * x [1], q = q1 * x [1] * x [1], w = q1 * x [n] * x [n];
	double v = q, u = w;
	double den = 1.0 - q - w;
	double q4 = 1.0 / den, q5 = 1.0 - q, q6 = 1.0 - w;
	double h = q2 * x [n], s = h;
	thy gain = e0 * den;
	q1 = 1.0 / thy gain;
	c [1] = q1 * x [1];
	d [1] = q1 * x [n];
	double s1 = 0.0;
	for (integer k = 1; k <= n - 1; k ++)
		s1 += x [k + 1] * x [k];
	r [1] = 2.0 * s1;
	thy a [1] = - q1 * r [1];
	thy gain *= (1.0 - thy a [1] * thy a [1]);
	while (m < mmax) {
		const double eOld = thy gain;
		double f = x [m + 1], b = x [n - m]; // n-1 ->n-m
		for (integer k = 1; k <= m; k ++) {
			// n-1 -> n-m
			f += x [m + 1 - k] * thy a [k];
			b += x [n - m + k] * thy a [k];
		}
		q1 = 1.0 / thy gain;
		q2 = q1 * f;
		const double q3 = q1 * b;
		for (integer k = m; k >= 1; k--) {
			c [k + 1] = c [k] + q2 * thy a [k];
			d [k + 1] = d [k] * q3 * thy a [k];
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
			status = 2;
			goto end; // 2: ill-conditioning
		}
		q4 = 1.0 / den;
		q1 *= q4;
		const double alf = 1.0 / (1.0 + q1 * (y1 * q6 + y3 * q5 + 2.0 * h * f * b));
		thy gain *= alf;
		y5 = h * s;
		double c1 = q4 * (f * q6 + b * h);
		double c2 = q4 * (b * q5 + h * f);
		const double c3 = q4 * (v * q6 + y5);
		const double c4 = q4 * (s * q5 + v * h);
		const double c5 = q4 * (s * q6 + h * u);
		const double c6 = q4 * (u * q5 + y5);
		for (integer k = 1; k <= m; k ++)
			thy a [k] = alf * (thy a [k] + c1 * c [k + 1] + c2 * d [k + 1]);
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
			delta += r [k + 1] * thy a [k];
		}
		s1 = 0.0;
		for (integer k = 1; k <= n - m; k ++)
			s1 += x [k + m] * x [k];
		r [1] = 2.0 * s1;
		delta += r [1];
		q2 = - delta / thy gain;
		thy a [m] = q2;
		for (integer k = 1; k <= m / 2; k ++) {
			s1 = thy a [k];
			thy a [k] += q2 * thy a [m - k];
			if (k == m - k)
				continue;
			thy a [m - k] += q2 * s1;
		}
		y1 = q2 * q2;
		thy gain *= 1.0 - y1;
		if (y1 >= 1.0) {
			status = 3;
			goto end; // |a [m]| > 1
		}
		if (thy gain < e0 *  my tolerance1) {
			status = 4;
			goto end;
		}
		if (eOld - thy gain < eOld * my tolerance2) {
			status = 5;
			goto end;
		}
	}
end:
	thy gain *= 0.5;   // because e0 is twice the energy
	thy a.resize (m);
	thy nCoefficients = thy a.size;   // maintain invariant
	my frameAnalysisIsOK = ( status == 0 || status == 4 || status == 5 );
}

void Sound_into_LPC_marple (Sound me, LPC thee, double effectiveAnalysisWidth, double preEmphasisFrequency, double tol1, double tol2) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCAnalysisWorkspace ws =  SoundToLPCAnalysisWorkspace_create (me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	SoundToLPCAnalysisWorkspace_allocate_v1To5 (ws.get(), thy maxnCoefficients + 1, thy maxnCoefficients + 1, thy maxnCoefficients + 1, 0, 0);
	ws ->tolerance1 = tol1;
	ws ->tolerance2 = tol2;
	ws -> analyseOneFrame = analyseOneFrame_marple;
	SoundAnalysisWorkspace_analyseThreaded (ws.get(), me, preEmphasisFrequency);
}

autoLPC Sound_to_LPC_marple (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency,
	double tol1, double tol2)
{
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (me, predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_marple (me, thee.get(), effectiveAnalysisWidth, preEmphasisFrequency, tol1, tol2);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (marple) created.");
	}
}

/*********************** robust method ******************************/

Thing_implement (SoundToLPCRobustAnalysisWorkspace, SoundToLPCAnalysisWorkspace, 0);

static void SVD_resizeWithinOldBounds (SVD me, integer nrowmax, integer ncolmax, integer nrownew, integer ncolnew) {
	Melder_require (nrownew <= nrowmax && ncolnew <= ncolmax,
		U"The new size must fit within the old size.");
	my numberOfRows = nrownew;
	my numberOfColumns = ncolnew;
	my u.resize (nrownew, ncolnew);
	my v.resize (ncolnew, ncolnew);
	my d.resize (ncolnew);
}

autoSoundToLPCRobustAnalysisWorkspace SoundToLPCRobustAnalysisWorkspace_create (Sound thee, LPC him, double effectiveAnalysisWidth, 
	kSound_windowShape windowShape, LPC original, double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	try {
		Melder_assert (his xmin == original -> xmin);
		Melder_assert (his xmax == original -> xmax);
		Melder_assert (his nx == original -> nx);
		Melder_assert (his maxnCoefficients == original -> maxnCoefficients);
		
		autoSoundToLPCRobustAnalysisWorkspace me = Thing_new (SoundToLPCRobustAnalysisWorkspace);
		SoundAnalysisWorkspace_init (me.get(), thee, him, effectiveAnalysisWidth, windowShape);
		my original = original;
		my allocateSampledFrames (me.get());
		my error = zero_VEC (my analysisFrameSize);
		my k_stdev = k_stdev;
		my tolerance1 = tol;
		my tolerance2 = 1e-6; // tol_svd 
		my scale = 0.0;
		my iter = 0;
		my itermax = itermax;
		my wantlocation = wantlocation;
		if (! wantlocation)
			my location = location;
		my wantscale = true;
		my huber_iterations = 5;
		my predictionOrder = his maxnCoefficients;
		my sampleWeights = zero_VEC (my analysisFrameSize);
		my coefficients = raw_VEC (his maxnCoefficients);
		my covariancesw = zero_VEC (his maxnCoefficients);
		my covarmatrixw = zero_MAT (his maxnCoefficients, his maxnCoefficients);
		my svd = SVD_create (his maxnCoefficients, his maxnCoefficients);
		SVD_setTolerance (my svd.get(), my tolerance2);
		/*
			Calculate workspace size
			v1work : inversefiltering
			v2work : SVD_compute
			v3work : svdSolve
			v4work : workspace_huber
		*/
		const integer svdComputeSize = SVD_getWorkspaceSize (my svd.get());
		SoundToLPCAnalysisWorkspace_allocate_v1To5 (me.get(), my analysisFrameSize, svdComputeSize, his maxnCoefficients, my analysisFrameSize, 0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToLPCRobustAnalysisWorkspace not created.");
	}
}

void SoundToLPCRobustAnalysisWorkspace_resize (SoundToLPCRobustAnalysisWorkspace me, integer newPredictionOrder) {
	if (newPredictionOrder == my svd -> numberOfColumns)
		return;
	LPC result = reinterpret_cast<LPC> (my result);
	Melder_assert (newPredictionOrder <= result -> maxnCoefficients);
	my coefficients.resize (newPredictionOrder);
	my covariancesw.resize (newPredictionOrder);
	my covarmatrixw.resize (newPredictionOrder, newPredictionOrder);
	SVD_resizeWithinOldBounds (my svd.get(), result -> maxnCoefficients, result -> maxnCoefficients, newPredictionOrder, newPredictionOrder);
}

static void setSampleWeights (SoundToLPCRobustAnalysisWorkspace me, constVEC const& error) {
	Melder_assert (error.size == my analysisFrameSize);
	const double kstdev = my k_stdev * my scale;
	for (integer isamp = 1 ; isamp <= my analysisFrameSize; isamp ++) {
		const double absDiff = fabs (error [isamp] - my location);
		my sampleWeights [isamp] = ( absDiff <= kstdev ? 1.0 : kstdev / absDiff );
	}
}

static void setCovariances (SoundToLPCRobustAnalysisWorkspace me, constVEC const& s) {
	Melder_assert (s.size == my analysisFrameSize);
	MATVU covar = MATVU (my covarmatrixw.part (1, my predictionOrder, 1, my predictionOrder));
	for (integer i = 1; i <= my predictionOrder; i ++) {
		for (integer j = i; j <= my predictionOrder; j ++) {
			longdouble cv1 = 0.0;
			for (integer k = my predictionOrder + 1; k <= my analysisFrameSize; k ++)
				cv1 += s [k - j] * s [k - i] *  my sampleWeights [k];
			covar [i] [j] = covar [j] [i] = (double) cv1;
		}
		longdouble cv2 = 0.0;
		for (integer k = my predictionOrder + 1; k <= my analysisFrameSize; k ++)
			cv2 += s [k - i] * s [k] *  my sampleWeights [k];
		my covariancesw [i] = - cv2;
	}
}

static void solvelpc (SoundToLPCRobustAnalysisWorkspace me) {
	// we do not resize the svd-matrices (yet) therefore add zero's and svd the full matrix
	//if (my predictionOrder < my lpc -> maxnCoefficients) {
	//	my covarmatrixw.part (my predictionOrder + 1, my lpc -> maxnCoefficients, 1, my lpc -> maxnCoefficients)  <<=  0.0;
	//	my covarmatrixw.part (1, my predictionOrder, my predictionOrder + 1, my lpc -> maxnCoefficients)  <<=  0.0;
	//	my coefficients.resize (my lpc -> maxnCoefficients);
	//}
	my svd -> u.all()  <<=  my covarmatrixw.all();
	SVD_compute (my svd.get(), my v2work.get());
	SVD_solve_preallocated (my svd.get(), my covariancesw.get(), my coefficients.get(), my v3work.get());
	my coefficients.resize (my predictionOrder); // maintain invariant
}

static void analyseOneFrame_robust (SoundAnalysisWorkspace saw, integer iframe) {
	SoundToLPCRobustAnalysisWorkspace me = reinterpret_cast<SoundToLPCRobustAnalysisWorkspace> (saw);
	LPC result = reinterpret_cast<LPC> (my result);
	Melder_assert (iframe > 0 && iframe <= result -> nx);
	LPC_Frame thee = & result -> d_frames [iframe];
	Melder_assert (thy nCoefficients == thy a.size); // check invariant

	const LPC_Frame fromFrame = & my original -> d_frames [my currentFrame];
	my predictionOrder = fromFrame -> nCoefficients;
	thy a.get() <<= fromFrame -> a.get();
	thy gain = fromFrame -> gain;
	my iter = 0;
	my scale = 1e308;
	bool farFromScale = true;
	SoundToLPCRobustAnalysisWorkspace_resize (me, my predictionOrder);
	do {
		const double previousScale = my scale;
		my error.all()  <<=  my analysisFrame.get();
		VECfilterInverse_inplace (my error.get(), thy a.get(), my v1work.get());
		NUMstatistics_huber (my error.get(), & my location, my wantlocation, & my scale, my wantscale, my k_stdev, my tolerance1,
			my huber_iterations, my v4work.get());
		setSampleWeights (me, my error.get());

		setCovariances (me, my analysisFrame.get());
		/*
			Solve C a = [-] c
		*/
		try {
			solvelpc (me);
		} catch (MelderError) {
			thy a.get()  <<=  fromFrame -> a.get(); // No change could be made
			throw MelderError();
		}
		thy a.get()  <<=  my coefficients.all();
		farFromScale = ( fabs (my scale - previousScale) > std::max (my tolerance1 * fabs (my scale), NUMeps) );
	} while (++ my iter < my itermax && farFromScale);
	
	my frameAnalysisIsOK = true;
}


void LPC_and_Sound_into_LPC_robust (LPC thee, Sound me, LPC result, double effectiveAnalysisWidth, double preEmphasisFrequency, double k_stdev,
	integer itermax, double tol, bool wantlocation)
{
	try {
		Melder_assert (thy xmin == result -> xmin && thy xmax == result -> xmax);
		Melder_assert (thy nx == result -> nx && thy maxnCoefficients == result -> maxnCoefficients);
		Melder_assert (thy samplingPeriod == result -> samplingPeriod && thy x1 == result -> x1); 
		Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);	
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		double location = 0.0;
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, thy maxnCoefficients);
		autoSoundToLPCRobustAnalysisWorkspace ws = SoundToLPCRobustAnalysisWorkspace_create (me, result, effectiveAnalysisWidth,
			kSound_windowShape::GAUSSIAN_2, thee, k_stdev, itermax, tol, location, wantlocation);
		ws -> analyseOneFrame = analyseOneFrame_robust;
		SoundAnalysisWorkspace_analyseThreaded (ws.get(), me, preEmphasisFrequency);
	} catch (MelderError) {
		Melder_throw (me, U": no robust LPC calculated.");
	}	
}

autoLPC LPC_and_Sound_to_LPC_robust (LPC thee, Sound me, double effectiveAnalysisWidth, double preEmphasisFrequency, double k_stdev,
	integer itermax, double tol, bool wantlocation)
{
	try {
		Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);	
		autoLPC result = LPC_create (thy xmin, thy xmax, thy nx, thy dx, thy x1, thy maxnCoefficients, thy samplingPeriod);
		LPC_and_Sound_into_LPC_robust (thee, me, result.get(), effectiveAnalysisWidth, preEmphasisFrequency, k_stdev, itermax, tol, wantlocation);
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": no robust LPC created.");
	}	
}

void Sound_into_LPC_robust (Sound me, LPC thee, double effectiveAnalysisWidth, double preEmphasisFrequency,
	double k_stdev,	integer itermax, double tol, bool wantlocation)
{
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoLPC input = LPC_create (thy xmin, thy xmax, thy nx, thy dx, thy x1, thy maxnCoefficients, thy samplingPeriod);
	Sound_into_LPC_auto (me, input.get(), effectiveAnalysisWidth, preEmphasisFrequency);	
	LPC_and_Sound_into_LPC_robust (input.get(), me, thee, effectiveAnalysisWidth, preEmphasisFrequency, k_stdev, itermax, tol, wantlocation);
}

autoLPC Sound_to_LPC_robust (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency,
	double k_stdev,	integer itermax, double tol, bool wantlocation) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (me, predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_robust (me, thee.get(), effectiveAnalysisWidth, preEmphasisFrequency, k_stdev, itermax, tol, wantlocation);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (robust) created.");
	}
}

/*********************** analysis ******************************/

void checkLPCAnalysisParameters_e (double sound_dx, integer sound_nx, double physicalAnalysisWidth, integer predictionOrder)
{
	volatile const double physicalDuration = sound_dx * sound_nx;
	Melder_require (physicalAnalysisWidth <= physicalDuration,
		U"The effective analysis width should not exceed the duration of the sound. "
		"Either your sound is too short or your window is too long,");
	// we round the minimum duration to be able to use asserterror in testing scripts.
	conststring32 minimumDurationRounded = Melder_fixed (predictionOrder * sound_dx , 5);
	const integer approximateNumberOfSamplesPerWindow = Melder_roundDown (physicalAnalysisWidth / sound_dx);
	Melder_require (approximateNumberOfSamplesPerWindow > predictionOrder,
		U"Analysis window duration too short. For a prediction order of ", predictionOrder,
		U", the analysis window duration should be greater than ", minimumDurationRounded,
		U" s. Please increase the analysis window duration or lower the prediction order.");
}


/*********************** (inverse) filtering ******************************/

static void LPC_Frame_Sound_filter (LPC_Frame me, Sound thee, integer channel) {
	const VEC y = thy z.row (channel);
	for (integer i = 1; i <= thy nx; i ++) {
		const integer m = ( i > my nCoefficients ? my nCoefficients : i - 1 );   // ppgb: what is m?
		for (integer j = 1; j <= m; j ++)
			y [i] -= my a [j] * y [i - j];
	}
}

autoSound LPC_Sound_filterInverse (LPC me, Sound thee) {
	try {
		Melder_require (my samplingPeriod == thy dx,
			U"The sampling frequencies should be equal.");
		Melder_require (my xmin == thy xmin && thy xmax == my xmax,
			U"The domains of LPC and Sound should be equal.");
		
		autoSound him = Data_copy (thee);
		VEC source = his z.row (1);
		VEC sound = thy z.row (1);
		for (integer isamp = 1; isamp <= his nx; isamp ++) {
			const double sampleTime = Sampled_indexToX (him.get(), isamp);
			const integer frameNumber = Sampled_xToNearestIndex (me, sampleTime);
			if (frameNumber < 1 || frameNumber > my nx) {
				source [isamp] = 0.0;
				continue;
			}
			const LPC_Frame frame = & my d_frames [frameNumber];
			const integer maximumFilterDepth = frame -> nCoefficients;
			const integer maximumSoundDepth = isamp - 1;
			const integer usableDepth = std::min (maximumFilterDepth, maximumSoundDepth);
			for (integer icoef = 1; icoef <= usableDepth; icoef ++)
				source [isamp] += frame -> a [icoef] * sound [isamp - icoef];
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/*
	Gain used as a constant amplitude multiplier within a frame of duration my dx.
	future alternative: convolve gain with a smoother.
*/
autoSound LPC_Sound_filter (LPC me, Sound thee, bool useGain) {
	try {
		const double xmin = std::max (my xmin, thy xmin);
		const double xmax = std::min (my xmax, thy xmax);
		Melder_require (xmin < xmax,
			U"Domains of Sound [", thy xmin, U",", thy xmax, U"] and LPC [",
			my xmin, U",", my xmax, U"] should overlap."
		);
		/*
			Resample the sound if the sampling frequencies do not match.
		*/
		autoSound source;
		if (my samplingPeriod != thy dx) {
			source = Sound_resample (thee, 1.0 / my samplingPeriod, 50);
			thee = source.get();   // reference copy; remove at end
		}

		autoSound him = Data_copy (thee);

		const integer ifirst = std::max (1_integer, Sampled_xToHighIndex (thee, xmin));
		const integer ilast = std::min (Sampled_xToLowIndex (thee, xmax), thy nx);
		for (integer isamp = ifirst; isamp <= ilast; isamp ++) {
			const double sampleTime = Sampled_indexToX (him.get(), isamp);
			const integer frameNumber = Sampled_xToNearestIndex (me, sampleTime);
			if (frameNumber < 1 || frameNumber > my nx) {
				his z [1] [isamp] = 0.0;
				continue;
			}
			const LPC_Frame frame = & my d_frames [frameNumber];
			const integer maximumFilterDepth = frame -> nCoefficients;
			const integer maximumSourceDepth = isamp - 1;
			const integer usableDepth = std::min (maximumFilterDepth, maximumSourceDepth);
			for (integer icoef = 1; icoef <= usableDepth; icoef ++)
				his z [1] [isamp] -= frame -> a [icoef] * his z [1] [isamp - icoef];
		}
		/*
			Make samples before first frame and after last frame zero.
		*/
		for (integer isamp = 1; isamp < ifirst; isamp ++)
			his z [1] [isamp] = 0.0;
		for (integer isamp = ilast + 1; isamp <= his nx; isamp ++)
			his z [1] [isamp] = 0.0;

		if (useGain) {
			for (integer isamp = ifirst; isamp <= ilast; isamp ++) {
				const double sampleTime = Sampled_indexToX (him.get(), isamp);
				const double realFrameNumber = Sampled_xToIndex (me, sampleTime);
				const integer leftFrameNumber = Melder_ifloor (realFrameNumber);
				const integer rightFrameNumber = leftFrameNumber + 1;
				const double phase = realFrameNumber - leftFrameNumber;
				if (rightFrameNumber < 1 || leftFrameNumber > my nx)
					his z [1] [isamp] = 0.0;
				else if (rightFrameNumber == 1)
					his z [1] [isamp] *= sqrt (my d_frames [1]. gain) * phase;
				else if (leftFrameNumber == my nx)
					his z [1] [isamp] *= sqrt (my d_frames [my nx]. gain) * (1.0 - phase);
				else 
					his z [1] [isamp] *=
							phase * sqrt (my d_frames [rightFrameNumber]. gain) +
							(1.0 - phase) * sqrt (my d_frames [leftFrameNumber]. gain);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time) {
	integer frameIndex = Sampled_xToNearestIndex (me, time);
	Melder_clip (1_integer, & frameIndex, my nx);   // constant extrapolation
	if (channel > thy ny)
		channel = 1;
	Melder_require (frameIndex > 0 && frameIndex <= my nx,
		U"Frame should be in the range [1, ", my nx, U"].");

	if (channel > 0)
		LPC_Frame_Sound_filter (& my d_frames [frameIndex], thee, channel);
	else
		for (integer ichan = 1; ichan <= thy ny; ichan ++)
			LPC_Frame_Sound_filter (& my d_frames [frameIndex], thee, ichan);
}

autoSound LPC_Sound_filterWithFilterAtTime (LPC me, Sound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterInverseWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time) {
	try {
		integer frameIndex = Sampled_xToNearestIndex (me, time);
		Melder_clip (1_integer, & frameIndex, my nx);   // constant extrapolation
		if (channel > thy ny)
			channel = 1;
		LPC_Frame lpc = & my d_frames [frameIndex];
		autoVEC workspace = raw_VEC (lpc -> nCoefficients);
		if (channel > 0)
			VECfilterInverse_inplace (thy z.row (channel), lpc -> a.get(), workspace.get());
		else
			for (integer ichan = 1; ichan <= thy ny; ichan ++)
				VECfilterInverse_inplace (thy z.row (ichan), lpc -> a.get(), workspace.get());
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

autoSound LPC_Sound_filterInverseWithFilterAtTime (LPC me, Sound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterInverseWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/* End of file Sound_and_LPC.cpp */
