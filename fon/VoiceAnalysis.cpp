/* VoiceAnalysis.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "VoiceAnalysis.h"
#include "AmplitudeTier.h"

double PointProcess_getJitter_local (PointProcess me, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	double sum = 0.0;
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 2) return undefined;
	for (long i = imin + 1; i < imax; i ++) {
		double p1 = my t [i] - my t [i - 1], p2 = my t [i + 1] - my t [i];
		double intervalFactor = p1 > p2 ? p1 / p2 : p2 / p1;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax && intervalFactor <= maximumPeriodFactor)) {
			sum += fabs (p1 - p2);
		} else {
			numberOfPeriods --;
		}
	}
	if (numberOfPeriods < 2) return undefined;
	return sum / (numberOfPeriods - 1) / PointProcess_getMeanPeriod (me, tmin, tmax, pmin, pmax, maximumPeriodFactor);
}

double PointProcess_getJitter_local_absolute (PointProcess me, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 2) return undefined;
	double sum = 0.0;
	for (long i = imin + 1; i < imax; i ++) {
		double p1 = my t [i] - my t [i - 1], p2 = my t [i + 1] - my t [i];
		double intervalFactor = p1 > p2 ? p1 / p2 : p2 / p1;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax && intervalFactor <= maximumPeriodFactor)) {
			sum += fabs (p1 - p2);
		} else {
			numberOfPeriods --;
		}
	}
	if (numberOfPeriods < 2) return undefined;
	return sum / (numberOfPeriods - 1);
}

double PointProcess_getJitter_rap (PointProcess me, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 3) return undefined;
	double sum = 0.0;
	for (long i = imin + 2; i < imax; i ++) {
		double p1 = my t [i - 1] - my t [i - 2], p2 = my t [i] - my t [i - 1], p3 = my t [i + 1] - my t [i];
		double intervalFactor1 = p1 > p2 ? p1 / p2 : p2 / p1, intervalFactor2 = p2 > p3 ? p2 / p3 : p3 / p2;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax && p3 >= pmin && p3 <= pmax
		    && intervalFactor1 <= maximumPeriodFactor && intervalFactor2 <= maximumPeriodFactor))
		{
			sum += fabs (p2 - (p1 + p2 + p3) / 3.0);
		} else {
			numberOfPeriods --;
		}
	}
	if (numberOfPeriods < 3) return undefined;
	return sum / (numberOfPeriods - 2) / PointProcess_getMeanPeriod (me, tmin, tmax, pmin, pmax, maximumPeriodFactor);
}

double PointProcess_getJitter_ppq5 (PointProcess me, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 5) return undefined;
	double sum = 0.0;
	for (long i = imin + 5; i <= imax; i ++) {
		double
			p1 = my t [i - 4] - my t [i - 5],
			p2 = my t [i - 3] - my t [i - 4],
			p3 = my t [i - 2] - my t [i - 3],
			p4 = my t [i - 1] - my t [i - 2],
			p5 = my t [i] - my t [i - 1];
		double
			f1 = p1 > p2 ? p1 / p2 : p2 / p1,
			f2 = p2 > p3 ? p2 / p3 : p3 / p2,
			f3 = p3 > p4 ? p3 / p4 : p4 / p3,
			f4 = p4 > p5 ? p4 / p5 : p5 / p4;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax && p3 >= pmin && p3 <= pmax &&
			p4 >= pmin && p4 <= pmax && p5 >= pmin && p5 <= pmax &&
			f1 <= maximumPeriodFactor && f2 <= maximumPeriodFactor && f3 <= maximumPeriodFactor && f4 <= maximumPeriodFactor))
		{
			sum += fabs (p3 - (p1 + p2 + p3 + p4 + p5) / 5.0);
		} else {
			numberOfPeriods --;
		}
	}
	if (numberOfPeriods < 5) return undefined;
	return sum / (numberOfPeriods - 4) / PointProcess_getMeanPeriod (me, tmin, tmax, pmin, pmax, maximumPeriodFactor);
}

double PointProcess_getJitter_ddp (PointProcess me, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	double rap = PointProcess_getJitter_rap (me, tmin, tmax, pmin, pmax, maximumPeriodFactor);
	return ( isdefined (rap) ? 3.0 * rap : undefined );
}

double PointProcess_Sound_getShimmer_local (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor)
{
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		return AmplitudeTier_getShimmer_local (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			return undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer (local) not computed.");
		}
	}
}

double PointProcess_Sound_getShimmer_local_dB (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor)
{
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		return AmplitudeTier_getShimmer_local_dB (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			return undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer (local, dB) not computed.");
		}
	}
}

double PointProcess_Sound_getShimmer_apq3 (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor)
{
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		return AmplitudeTier_getShimmer_apq3 (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			return undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer (apq3) not computed.");
		}
	}
}

double PointProcess_Sound_getShimmer_apq5 (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor)
{
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		return AmplitudeTier_getShimmer_apq5 (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			return undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer (apq5) not computed.");
		}
	}
}

double PointProcess_Sound_getShimmer_apq11 (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor)
{
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		return AmplitudeTier_getShimmer_apq11 (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			return undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer (apq11) not computed.");
		}
	}
}

double PointProcess_Sound_getShimmer_dda (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor)
{
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		double apq3 = AmplitudeTier_getShimmer_apq3 (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
		return
			isdefined (apq3) ? 3.0 * apq3 : undefined;
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			return undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer (dda) not computed.");
		}
	}
}

void PointProcess_Sound_getShimmer_multi (PointProcess me, Sound thee, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor, double maximumAmplitudeFactor,
	double *local, double *local_dB, double *apq3, double *apq5, double *apq11, double *dda)
{
	try {
		if (tmax <= tmin) {
			tmin = my xmin;
			tmax = my xmax;   // autowindowing
		}
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_period (me, thee, tmin, tmax, pmin, pmax, maximumPeriodFactor);
		if (local)    *local    =       AmplitudeTier_getShimmer_local    (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
		if (local_dB) *local_dB =       AmplitudeTier_getShimmer_local_dB (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
		if (apq3)     *apq3     =       AmplitudeTier_getShimmer_apq3     (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
		if (apq5)     *apq5     =       AmplitudeTier_getShimmer_apq5     (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
		if (apq11)    *apq11    =       AmplitudeTier_getShimmer_apq11    (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
		if (dda)      *dda      = 3.0 * AmplitudeTier_getShimmer_apq3     (peaks.get(), pmin, pmax, maximumAmplitudeFactor);
	} catch (MelderError) {
		if (Melder_hasError (U"Too few pulses between ")) {
			Melder_clearError ();
			if (local)    *local    = undefined;
			if (local_dB) *local_dB = undefined;
			if (apq3)     *apq3     = undefined;
			if (apq5)     *apq5     = undefined;
			if (apq11)    *apq11    = undefined;
			if (dda)      *dda      = undefined;
		} else {
			Melder_throw (me, U" & ", thee, U": shimmer measures not computed.");
		}
	}
}

void Sound_Pitch_PointProcess_voiceReport (Sound sound, Pitch pitch, PointProcess pulses, double tmin, double tmax,
	double floor, double ceiling, double maximumPeriodFactor, double maximumAmplitudeFactor, double silenceThreshold, double voicingThreshold)
{
	try {
		if (tmin >= tmax) tmin = sound -> xmin, tmax = sound -> xmax;
		/*
		 * Time domain. Should be preceded by something like "Time range of SELECTION:" or so.
		 */
		MelderInfo_write (U"   From ", Melder_fixed (tmin, 6), U" to ", Melder_fixed (tmax, 6), U" seconds");
		MelderInfo_writeLine (U" (duration: ", Melder_fixed (tmax - tmin, 6), U" seconds)");
		/*
		 * Pitch statistics.
		 */
		MelderInfo_writeLine (U"Pitch:");
		MelderInfo_writeLine (U"   Median pitch: ", Melder_fixed (Pitch_getQuantile (pitch, tmin, tmax, 0.50, kPitch_unit_HERTZ), 3), U" Hz");
		MelderInfo_writeLine (U"   Mean pitch: ", Melder_fixed (Pitch_getMean (pitch, tmin, tmax, kPitch_unit_HERTZ), 3), U" Hz");
		MelderInfo_writeLine (U"   Standard deviation: ", Melder_fixed (Pitch_getStandardDeviation (pitch, tmin, tmax, kPitch_unit_HERTZ), 3), U" Hz");
		MelderInfo_writeLine (U"   Minimum pitch: ", Melder_fixed (Pitch_getMinimum (pitch, tmin, tmax, kPitch_unit_HERTZ, 1), 3), U" Hz");
		MelderInfo_writeLine (U"   Maximum pitch: ", Melder_fixed (Pitch_getMaximum (pitch, tmin, tmax, kPitch_unit_HERTZ, 1), 3), U" Hz");
		/*
		 * Pulses statistics.
		 */
		double pmin = 0.8 / ceiling, pmax = 1.25 / floor;
		MelderInfo_writeLine (U"Pulses:");
		MelderInfo_writeLine (U"   Number of pulses: ", PointProcess_getWindowPoints (pulses, tmin, tmax, nullptr, nullptr));
		MelderInfo_writeLine (U"   Number of periods: ", PointProcess_getNumberOfPeriods (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor));
		MelderInfo_writeLine (U"   Mean period: ", Melder_fixedExponent (PointProcess_getMeanPeriod (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), -3, 6), U" seconds");
		MelderInfo_writeLine (U"   Standard deviation of period: ", Melder_fixedExponent (PointProcess_getStdevPeriod (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), -3, 6), U" seconds");
		/*
		 * Voicing.
		 */
		long imin, imax, n = Sampled_getWindowSamples (pitch, tmin, tmax, & imin, & imax), nunvoiced = n;
		for (long i = imin; i <= imax; i ++) {
			Pitch_Frame frame = & pitch -> frame [i];
			if (frame -> intensity >= silenceThreshold) {
				for (long icand = 1; icand <= frame -> nCandidates; icand ++) {
					Pitch_Candidate cand = & frame -> candidate [icand];
					if (cand -> frequency > 0.0 && cand -> frequency < ceiling && cand -> strength >= voicingThreshold) {
						nunvoiced --;
						break;   // next frame
					}
				}
			}
		}
		MelderInfo_writeLine (U"Voicing:");
		MelderInfo_write (U"   Fraction of locally unvoiced frames: ", Melder_percent (n <= 0 ? undefined : (double) nunvoiced / n, 3));
		MelderInfo_writeLine (U"   (", nunvoiced, U" / ", n, U")");
		n = PointProcess_getWindowPoints (pulses, tmin, tmax, & imin, & imax);
		long numberOfVoiceBreaks = 0;
		double durationOfVoiceBreaks = 0.0;
		if (n > 1) {
			bool previousPeriodVoiced = true;
			for (long i = imin + 1; i < imax; i ++) {
				double period = pulses -> t [i] - pulses -> t [i - 1];
				if (period > pmax) {
					durationOfVoiceBreaks += period;
					if (previousPeriodVoiced) {
						numberOfVoiceBreaks ++;
						previousPeriodVoiced = false;
					}
				} else {
					previousPeriodVoiced = true;
				}
			}
		}
		MelderInfo_writeLine (U"   Number of voice breaks: ", numberOfVoiceBreaks);
		MelderInfo_write (U"   Degree of voice breaks: ", Melder_percent (durationOfVoiceBreaks / (tmax - tmin), 3));
		MelderInfo_writeLine (U"   (", Melder_fixed (durationOfVoiceBreaks, 6), U" seconds / ", Melder_fixed (tmax - tmin, 6), U" seconds)");
		/*
		 * Jitter.
		 */
		double shimmerLocal, shimmerLocal_dB, apq3, apq5, apq11, dda;
		MelderInfo_writeLine (U"Jitter:");
		MelderInfo_writeLine (U"   Jitter (local): ", Melder_percent (PointProcess_getJitter_local (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), 3));
		MelderInfo_writeLine (U"   Jitter (local, absolute): ", Melder_fixedExponent (PointProcess_getJitter_local_absolute (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), -6, 3), U" seconds");
		MelderInfo_writeLine (U"   Jitter (rap): ", Melder_percent (PointProcess_getJitter_rap (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), 3));
		MelderInfo_writeLine (U"   Jitter (ppq5): ", Melder_percent (PointProcess_getJitter_ppq5 (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), 3));
		MelderInfo_writeLine (U"   Jitter (ddp): ", Melder_percent (PointProcess_getJitter_ddp (pulses, tmin, tmax, pmin, pmax, maximumPeriodFactor), 3));
		/*
		 * Shimmer.
		 */
		PointProcess_Sound_getShimmer_multi (pulses, sound, tmin, tmax, pmin, pmax, maximumPeriodFactor, maximumAmplitudeFactor,
			& shimmerLocal, & shimmerLocal_dB, & apq3, & apq5, & apq11, & dda);
		MelderInfo_writeLine (U"Shimmer:");
		MelderInfo_writeLine (U"   Shimmer (local): ", Melder_percent (shimmerLocal, 3));
		MelderInfo_writeLine (U"   Shimmer (local, dB): ", Melder_fixed (shimmerLocal_dB, 3), U" dB");
		MelderInfo_writeLine (U"   Shimmer (apq3): ", Melder_percent (apq3, 3));
		MelderInfo_writeLine (U"   Shimmer (apq5): ", Melder_percent (apq5, 3));
		MelderInfo_writeLine (U"   Shimmer (apq11): ", Melder_percent (apq11, 3));
		MelderInfo_writeLine (U"   Shimmer (dda): ", Melder_percent (dda, 3));
		/*
		 * Harmonicity.
		 */
		MelderInfo_writeLine (U"Harmonicity of the voiced parts only:");
		MelderInfo_writeLine (U"   Mean autocorrelation: ", Melder_fixed (Pitch_getMeanStrength (pitch, tmin, tmax, Pitch_STRENGTH_UNIT_AUTOCORRELATION), 6));
		MelderInfo_writeLine (U"   Mean noise-to-harmonics ratio: ", Melder_fixed (Pitch_getMeanStrength (pitch, tmin, tmax, Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO), 6));
		MelderInfo_writeLine (U"   Mean harmonics-to-noise ratio: ", Melder_fixed (Pitch_getMeanStrength (pitch, tmin, tmax, Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB), 3), U" dB");
	} catch (MelderError) {
		Melder_throw (sound, U" & ", pitch, U" & ", pulses, U": voice report not computed.");
	}
}

/* End of file VoiceAnalysis.cpp */
