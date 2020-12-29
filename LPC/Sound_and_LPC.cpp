/* Sound_and_LPC.cpp
 *
 * Copyright (C) 1994-2020 David Weenink
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
 djmw corrected a bug in Sound_into_LPC_Frame_marple that could crash praat when signal has only zero samples.
 djmw 20040303 Removed warning in Sound_to_LPC.
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
 djmw 20101009 Filter and inverseFilter with one frame.
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

#define LPC_METHOD_AUTO 1
#define LPC_METHOD_COVAR 2
#define LPC_METHOD_BURG 3
#define LPC_METHOD_MARPLE 4

/* Markel&Gray, LP of S, page 219
work [1..3*m+2]
r = & work [1]; // r [1..m+1]
a= & work [m+1+1]; // a [1..m+1]
rc = & work [m+1+m+1+1]; // rc [1..m]
for (i=1; i<= m+1+m+1+m;i ++) work [i] = 0;
*/

#define LPC_METHOD_AUTO_WINDOW_CORRECTION 1

static void LPC_Frame_Sound_filter (LPC_Frame me, Sound thee, integer channel) {
	const VEC y = thy z.row (channel);
	for (integer i = 1; i <= thy nx; i ++) {
		const integer m = ( i > my nCoefficients ? my nCoefficients : i - 1 );   // ppgb: what is m?
		for (integer j = 1; j <= m; j ++)
			y [i] -= my a [j] * y [i - j];
	}
}

static integer getLPCAnalysisWorkspaceSize (integer numberOfSamples, integer numberOfCoefficients, kLPC_Analysis method) {
	integer size = 0;
	if (method == kLPC_Analysis :: AUTOCORRELATION)
		size = 3 * numberOfCoefficients + 2;
	else if (method == kLPC_Analysis :: COVARIANCE)
		size = numberOfCoefficients * (numberOfCoefficients + 1) / 2 + 4 * numberOfCoefficients + 2; 
	else if (method == kLPC_Analysis :: BURG)
		size = 3 * numberOfSamples;
	else if (method == kLPC_Analysis :: MARPLE)
		size = 3 * (numberOfCoefficients + 1);
	return size;
}

static autoVEC getLPCAnalysisWorkspace (integer numberOfSamples, integer numberOfCoefficients, kLPC_Analysis method) {
	integer size = getLPCAnalysisWorkspaceSize (numberOfSamples, numberOfCoefficients, method);
	autoVEC result = raw_VEC (size);
	return result;
}

static int Sound_into_LPC_Frame_auto (Sound me, LPC_Frame thee, VEC const& workspace) {
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	const integer numberOfCoefficients = thy nCoefficients, np1 = numberOfCoefficients + 1;

	//workspace  <<=  0.0; not necessary !
	VEC r = workspace. part (1, np1); // autoVEC r = zero_VEC (numberOfCoefficients + 1);
	VEC a = workspace. part (np1 + 1, 2 * np1); // autoVEC a = zero_VEC (numberOfCoefficients + 1);
	VEC rc = workspace. part (2 * np1 + 1, 2 * np1 + numberOfCoefficients); // autoVEC rc = zero_VEC (numberOfCoefficients);
	const VECVU x = my z.row (1);
	integer i = 1; // For error condition at end
	for (i = 1; i <= numberOfCoefficients + 1; i ++)
		r [i] = NUMinner (x.part (1, my nx - i + 1), x.part (i, my nx));
	if (r [1] == 0.0) {
		i = 1; // !
		goto end;
	}
	a [1] = 1.0;
	a [2] = rc [1] = - r [2] / r [1];
	thy gain = r [1] + r [2] * rc [1];
	for (i = 2; i <= numberOfCoefficients; i ++) {
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
	if (i == numberOfCoefficients)
		return 1;
	thy a.resize (i);
	thy nCoefficients = thy a.size; // maintain invariant
	return 0; // Melder_warning ("Fewer coefficients than asked for.");
}

/*
	Markel&Gray, LP of S, page 221
	work [1..m(m+1)/2+m+m+1+m+m+1]
*/
static int Sound_into_LPC_Frame_covar (Sound me, LPC_Frame thee, VEC const& workspace) {
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	const integer n = my nx, m = thy nCoefficients;
	constVEC x = my z.row (1);
	
	workspace  <<=  0.0;
	integer start = 1, end = m * (m + 1) / 2;
	VEC b = workspace. part (start, end); // autoVEC b = zero_VEC (m * (m + 1) / 2);
	start = end + 1; end += m;
	VEC grc = workspace. part (start, end); //autoVEC grc = zero_VEC (m);
	start = end + 1; end += m;
	VEC beta = workspace. part (start, end); // autoVEC beta = zero_VEC (m);
	start = end + 1; end += m + 1;
	VEC a = workspace. part (start, end); // autoVEC a = zero_VEC (m + 1);
	start = end + 1; end += m + 1;
	VEC cc = workspace. part (start, end); // autoVEC cc = zero_VEC (m + 1);

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
		return 1;
	thy a.resize (i);
	thy nCoefficients = thy a.size;
	return 0; // Melder_warning ("Fewer coefficients than asked for.");
}

static double VECburg_buffered (VEC const& a, constVEC const& x, VEC const& workspace) {
	const integer n = x.size, m = a.size;
	for (integer j = 1; j <= m; j ++)
		a [j] = 0.0;

	VEC b1 = workspace. part (1, n); // autoVEC b1 = zero_VEC (n);
	VEC b2 = workspace. part (n + 1, 2 * n); // autoVEC b2 = zero_VEC (n);
	VEC aa = workspace. part (2 * n + 1, 2 * n + m); // autoVEC aa = zero_VEC (m);

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

static int Sound_into_LPC_Frame_burg (Sound me, LPC_Frame thee, VEC const& workspace) {
	Melder_assert (thy nCoefficients == thy a.size); // check invariant
	thy gain = VECburg_buffered (thy a.get(), my z.row(1), workspace);
	if (thy gain <= 0.0) {
		thy a.resize (0);
		thy nCoefficients = thy a.size; // maintain invariant
		return 0;
	}
	thy gain *= my nx;
	for (integer i = 1; i <= thy nCoefficients; i ++)
		thy a [i] = -thy a [i];
	return thy gain != 0.0;
}

static int Sound_into_LPC_Frame_marple (Sound me, LPC_Frame thee, double tol1, double tol2, VEC const& workspace) {
	const integer n = my nx, mmax = thy nCoefficients, mmaxp1 = mmax + 1;
	int status = 1;
	// workspace.all () << 0.0 not necessary
	constVEC x = my z.row (1);
	VEC c = workspace .part (1, mmaxp1); // autoVEC c = zero_VEC (mmax + 1);
	VEC d = workspace .part (mmaxp1 + 1, 2 * mmaxp1); // autoVEC d = zero_VEC (mmax + 1);
	VEC r = workspace .part (2 * mmaxp1 + 1, 3 * mmaxp1); // autoVEC r = zero_VEC (mmax + 1);
	double e0 = 2.0 * NUMsum2 (x);
	integer m = 1;
	if (e0 == 0.0) {
		thy a.resize (0);
		thy nCoefficients = thy a.size; // maintain invariant
		thy gain = 0.0;
		return 0; // warning no signal
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
		if (thy gain < e0 * tol1) {
			status = 4;
			goto end;
		}
		if (eOld - thy gain < eOld * tol2) {
			status = 5;
			goto end;
		}
	}
end:
	thy gain *= 0.5;   // because e0 is twice the energy
	thy a.resize (m);
	thy nCoefficients = thy a.size;   // maintain invariant
	return status == 1 || status == 4 || status == 5;
}

static void Sound_into_LPC_noThreads (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency, kLPC_Analysis method, double tol1, double tol2) {
	Melder_require (my xmin == thy xmin && my xmax == thy xmax, 
		U"The Sound and the LPC should have the same domain.");
	const double samplingFrequency = 1.0 / my dx;
	double windowDuration = 2.0 * analysisWidth; // Gaussian window
	const integer predictionOrder = thy maxnCoefficients;
	Melder_require (Melder_roundDown (windowDuration / my dx) > predictionOrder,
		U"Analysis window duration too short.\n For a prediction order of ", predictionOrder,
		U" the analysis window duration should be greater than ", my dx * (predictionOrder + 1),
		U" s. Please increase the analysis window duration.");
	/*
		Convenience: analyse the whole sound into one LPC_frame.
	*/
	if (windowDuration > my dx * my nx)
		windowDuration = my dx * my nx;
	integer numberOfFrames = thy nx;
	autoSound sound = Data_copy (me);
	autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
	autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
	for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
		const LPC_Frame lpcFrame = & thy d_frames [iframe];
		LPC_Frame_init (lpcFrame, predictionOrder);
	}

	autoVEC workspace = getLPCAnalysisWorkspace (sframe -> nx, predictionOrder, method);
	autoMelderProgress progress (U"LPC analysis");

	if (preEmphasisFrequency < samplingFrequency / 2.0)
		Sound_preEmphasis (sound.get(), preEmphasisFrequency);
	integer frameErrorCount = 0;
	for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
		const LPC_Frame lpcframe = & thy d_frames [iframe];
		const double t = Sampled_indexToX (thee, iframe);
		Sound_into_Sound (sound.get(), sframe.get(), t - 0.5 * windowDuration);
		Vector_subtractMean (sframe.get());
		Sounds_multiply (sframe.get(), window.get());
		integer status = 1;
		if (method == kLPC_Analysis :: AUTOCORRELATION)
			status = Sound_into_LPC_Frame_auto (sframe.get(), lpcframe, workspace.get());
		else if (method == kLPC_Analysis :: COVARIANCE)
			status = Sound_into_LPC_Frame_covar (sframe.get(), lpcframe, workspace.get());
		else if (method == kLPC_Analysis :: BURG)
			status = Sound_into_LPC_Frame_burg (sframe.get(), lpcframe, workspace.get());
		else if (method == kLPC_Analysis :: MARPLE)
			status = Sound_into_LPC_Frame_marple (sframe.get(), lpcframe, tol1, tol2, workspace.get());
		if (status != 0)
			frameErrorCount ++;
		if (iframe % 10 == 1)
			Melder_progress (double (iframe) / numberOfFrames, U"LPC analysis of frame ", iframe, U" out of ", numberOfFrames, U".");
	}
}

void Sound_into_LPC (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency, kLPC_Analysis method, double tol1, double tol2) {
	const integer numberOfProcessors = std::thread::hardware_concurrency ();
	if (numberOfProcessors <= 1) {
		/*
			We cannot use multithreading.
		*/
		Sound_into_LPC_noThreads (me, thee, analysisWidth, preEmphasisFrequency, method, tol1, tol2);
	}
	const double samplingFrequency = 1.0 / my dx;
	Melder_require (my xmin == thy xmin && my xmax == thy xmax, 
		U"The Sound and the LPC should have the same domain.");
	const integer predictionOrder = thy maxnCoefficients;
	double windowDuration = 2.0 * analysisWidth; // Gaussian window
	Melder_require (Melder_roundDown (windowDuration / my dx) > predictionOrder,
		U"Analysis window duration too short.\n For a prediction order of ", predictionOrder,
		U" the analysis window duration should be greater than ", my dx * (predictionOrder + 1),
		U" s. Please increase the analysis window duration.");
	/*
		Convenience: analyse the whole sound into one LPC_frame.
	*/
	if (windowDuration > my dx * my nx)
		windowDuration = my dx * my nx;
	integer numberOfFrames = thy nx;
	autoSound sound = Data_copy (me);
	autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
	/*
		Because of threading we initialise the frames beforehand.
		We initialize the coefficient vector with a size equal to the prediction order.
	*/
	for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
		const LPC_Frame lpcFrame = & thy d_frames [iframe];
		LPC_Frame_init (lpcFrame, predictionOrder);
	}
	if (preEmphasisFrequency < samplingFrequency / 2.0)
		Sound_preEmphasis (sound.get(), preEmphasisFrequency);
	
	constexpr integer maximumNumberOfThreads = 16;
	integer numberOfThreads, numberOfFramesPerThread = 25;
	NUMgetThreadingInfo (numberOfFrames, std::min (numberOfProcessors, maximumNumberOfThreads), & numberOfFramesPerThread, & numberOfThreads);
	/*
		We have to reserve all the needed working memory for each thread beforehand.
	*/
	autoSound sframe [maximumNumberOfThreads + 1];
	for (integer ithread = 1; ithread <= numberOfThreads; ithread ++)
		sframe [ithread] = Sound_createSimple (1, windowDuration, samplingFrequency);
	
	const integer workspaceSize = getLPCAnalysisWorkspaceSize (sframe [1] -> nx, predictionOrder, method);
	Melder_require (workspaceSize > 0,
		U"The workspace size is not properly defined.");
	autoMAT workspace = raw_MAT (numberOfThreads, workspaceSize);

	std::vector <std::thread> thread (numberOfThreads);
	std::atomic<integer> frameErrorCount (0);
	
	try {
		for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
			Sound soundFrame = sframe [ithread]. get(), fullsound = sound.get(), windowFrame = window.get();
			VEC threadWorkspace = workspace. row (ithread);
			const integer firstFrame = 1 + (ithread - 1) * numberOfFramesPerThread;
			const integer lastFrame = ( ithread == numberOfThreads ? numberOfFrames : firstFrame + numberOfFramesPerThread - 1 );
			
			thread [ithread - 1] = std::thread ([=, & frameErrorCount]() {
				for (integer iframe = firstFrame; iframe <= lastFrame; iframe ++) {
					const LPC_Frame lpcframe = & thy d_frames [iframe];
					const double t = Sampled_indexToX (thee, iframe);
					Sound_into_Sound (fullsound, soundFrame, t - 0.5 * windowDuration);
					Vector_subtractMean (soundFrame);
					Sounds_multiply (soundFrame, windowFrame);
					integer status = 1;
					if (method == kLPC_Analysis :: AUTOCORRELATION)
						status = Sound_into_LPC_Frame_auto (soundFrame, lpcframe, threadWorkspace);
					else if (method == kLPC_Analysis :: COVARIANCE)
						status = Sound_into_LPC_Frame_covar (soundFrame, lpcframe, threadWorkspace);
					else if (method == kLPC_Analysis :: BURG)
						status = Sound_into_LPC_Frame_burg (soundFrame, lpcframe, threadWorkspace);
					else if (method == kLPC_Analysis :: MARPLE)
						status = Sound_into_LPC_Frame_marple (soundFrame, lpcframe, tol1, tol2, threadWorkspace);
					if (status != 0)
						++ frameErrorCount;
				}
			});
		}
	} catch (MelderError) {
		for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
			if (thread [ithread - 1]. joinable ())
				thread [ithread - 1]. join ();
		}
		throw;
	}
	for (integer ithread = 1; ithread <= numberOfThreads; ithread ++)
		thread [ithread - 1]. join ();

}

static autoLPC Sound_to_LPC (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency, kLPC_Analysis method, double tol1, double tol2) {
	double windowDuration = 2.0 * analysisWidth; // Gaussian window
	Melder_require (Melder_roundDown (windowDuration / my dx) > predictionOrder,
		U"Analysis window duration too short.\n For a prediction order of ", predictionOrder,
		U" the analysis window duration should be greater than ", my dx * (predictionOrder + 1),
		U"Please increase the analysis window duration or lower the prediction order."
	);
	
	if (windowDuration > my dx * my nx)
		windowDuration = my dx * my nx;
	double t1;
	integer numberOfFrames;
	Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
	autoLPC thee = LPC_create (my xmin, my xmax, numberOfFrames, dt, t1, predictionOrder, my dx);
	Sound_into_LPC (me, thee.get(), analysisWidth, preEmphasisFrequency, method, tol1, tol2);
	return thee;
}

autoLPC Sound_to_LPC_autocorrelation (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoLPC thee = Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, kLPC_Analysis :: AUTOCORRELATION, 0.0, 0.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (auto) created.");
	}
}

autoLPC Sound_to_LPC_covariance (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoLPC thee = Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, kLPC_Analysis :: COVARIANCE, 0.0, 0.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (covar) created.");
	}
}

autoLPC Sound_to_LPC_burg (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoLPC thee = Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, kLPC_Analysis :: BURG, 0.0, 0.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (burg) created.");
	}
}

autoLPC Sound_to_LPC_marple (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency, double tol1, double tol2) {
	try {
		autoLPC thee = Sound_to_LPC (me, predictionOrder, analysisWidth, dt, preEmphasisFrequency, kLPC_Analysis :: MARPLE, tol1, tol2);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (marple) created.");
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
		autoVEC work = raw_VEC (lpc -> nCoefficients);
		if (channel > 0)
			VECfilterInverse_inplace (thy z.row (channel), lpc -> a.get(), work);
		else
			for (integer ichan = 1; ichan <= thy ny; ichan ++)
				VECfilterInverse_inplace (thy z.row (ichan), lpc -> a.get(), work);
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
