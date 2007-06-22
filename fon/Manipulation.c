/* Manipulation.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/11/26 repaired a memory leak in Sound_to_Manipulation
 * pb 2006/12/30 new Sound_create API
 * pb 2007/01/27 made compatible with stereo sounds (by converting them to mono)
 * pb 2007/02/25 changed default sampling frequency to 44100 Hz
 * pb 2007/03/17 domain quantity
 */

#include "Manipulation.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_AnyTier_to_PitchTier.h"
#include "PitchTier_to_PointProcess.h"
#include "Pitch_to_PointProcess.h"
#include "PointProcess_and_Sound.h"
#include "Sound_and_LPC.h"

#define MAX_T  0.02000000001   /* Maximum interval between two voice pulses (otherwise voiceless). */

#include "oo_DESTROY.h"
#include "Manipulation_def.h"
#include "oo_COPY.h"
#include "Manipulation_def.h"
#include "oo_EQUAL.h"
#include "Manipulation_def.h"
#include "oo_WRITE_TEXT.h"
#include "Manipulation_def.h"
#include "oo_READ_TEXT.h"
#include "Manipulation_def.h"
#include "oo_WRITE_BINARY.h"
#include "Manipulation_def.h"
#include "oo_READ_BINARY.h"
#include "Manipulation_def.h"
#include "oo_DESCRIPTION.h"
#include "Manipulation_def.h"

class_methods (Manipulation, Function)
	us -> version = 4;
	class_method_local (Manipulation, destroy)
	class_method_local (Manipulation, description)
	class_method_local (Manipulation, copy)
	class_method_local (Manipulation, equal)
	class_method_local (Manipulation, writeText)
	class_method_local (Manipulation, readText)
	class_method_local (Manipulation, writeBinary)
	class_method_local (Manipulation, readBinary)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
class_methods_end

Manipulation Manipulation_create (double tmin, double tmax) {
	Manipulation me = new (Manipulation);
	if (! me) return NULL;
	my xmin = tmin, my xmax = tmax;
	if (! (my duration = DurationTier_create (tmin, tmax))) { forget (me); return NULL; }
	return me;
}

int Manipulation_replaceOriginalSound (Manipulation me, Sound sound) {
	forget (my sound);
	if (! (my sound = Sound_convertToMono (sound))) return 0;
	Vector_subtractMean (my sound);
	forget (my lpc);
	return 1;
}

int Manipulation_replacePulses (Manipulation me, PointProcess pulses) {
	forget (my pulses);
	if (! (my pulses = Data_copy (pulses))) return 0;
	return 1;
}

int Manipulation_replacePitchTier (Manipulation me, PitchTier pitch) {
	forget (my pitch);
	if (! (my pitch = Data_copy (pitch))) return 0;
	return 1;
}

int Manipulation_replaceDurationTier (Manipulation me, DurationTier duration) {
	forget (my duration);
	if (! (my duration = Data_copy (duration))) return 0;
	return 1;
}

Manipulation Sound_to_Manipulation (Sound me, double timeStep, double minimumPitch, double maximumPitch) {
	Pitch pitch = NULL;
	Manipulation thee = Manipulation_create (my xmin, my xmax); cherror
	thy sound = Sound_convertToMono (me); cherror
	Vector_subtractMean (thy sound);
	pitch = Sound_to_Pitch (thy sound, timeStep, minimumPitch, maximumPitch); cherror
	thy pulses = Sound_Pitch_to_PointProcess_cc (thy sound, pitch); cherror
	thy pitch = Pitch_to_PitchTier (pitch); cherror
	/* (DurationTier has been done at creation time) */
end:
	forget (pitch);
	iferror { Melder_errorp ("(Sound_to_Manipulation:) Not performed."); forget (thee); }
	return thee;
}

Manipulation Sound_Pitch_to_Manipulation (Sound sound, Pitch pitch) {
	Manipulation me;
	if (! (me = Manipulation_create (sound -> xmin, sound -> xmax))) return NULL;
	if (! (my sound = Sound_convertToMono (sound))) goto error;
	Vector_subtractMean (my sound);
	if (! (my pulses = Sound_Pitch_to_PointProcess_cc (my sound, pitch))) goto error;
	if (! (my pitch = Pitch_to_PitchTier (pitch))) goto error;
	return me;
error:
	forget (me);
	return Melder_errorp ("(Sound_Pitch_to_Manipulation:) Not performed.");
}

Manipulation Sound_PointProcess_to_Manipulation (Sound sound, PointProcess point) {
	Manipulation me;
	if (! (me = Manipulation_create (sound -> xmin, sound -> xmax))) return NULL;
	if (! (my sound = Sound_convertToMono (sound))) goto error;
	Vector_subtractMean (my sound);
	if (! (my pulses = Data_copy (point))) goto error;
	if (! (my pitch = PointProcess_to_PitchTier (point, MAX_T))) goto error;
	return me;
error:
	forget (me);
	return Melder_errorp ("(Sound_PointProcess_to_Manipulation:) Not performed.");
}

int Manipulation_playPart (Manipulation me, double tmin, double tmax, int method) {
	if (method == Manipulation_PSOLA) {
		Sound part, saved = my sound, played;
		long i, imin, imax;
		float *amp;
		if (! my sound) return Melder_error ("Cannot synthesize PSOLA without a sound.");
		part = Data_copy (my sound);
		if (! part) return 0;
		imin = Sampled_xToLowIndex (part, tmin), imax = Sampled_xToHighIndex (part, tmax);
		amp = part -> z [1];
		for (i = 1; i <= imin; i ++) amp [i] = 0.0;
		for (i = imax; i <= part -> nx; i ++) amp [i] = 0.0;
		my sound = part;
		played = Manipulation_to_Sound (me, Manipulation_PSOLA);
		my sound = saved;
		forget (part);
		if (! played) return 0;
		amp = played -> z [1];
		for (imin = 1; imin <= played -> nx; imin ++) if (amp [imin] != 0.0) break;
		for (imax = played -> nx; imax >= 1; imax --) if (amp [imax] != 0.0) break;
		Sound_playPart (played, played -> x1 + (imin - 1.5) * played -> dx, played -> x1 + (imax - 0.5) * played -> dx, NULL, 0);
		forget (played);
	} else {
		Sound sound = Manipulation_to_Sound (me, method);
		if (! sound) return 0;
		Sound_playPart (sound, tmin, tmax, NULL, 0);
		forget (sound);
	}
	return 1;
}

int Manipulation_play (Manipulation me, int method) {
	Sound sound = Manipulation_to_Sound (me, method);
	if (! sound) return 0;
	Sound_play (sound, NULL, NULL);
	forget (sound);
	return 1;
}

static long PointProcess_getFirstVoicedPoint (PointProcess me, double maxT) {
	long i;
	for (i = 1; i < my nt; i ++) if (my t [i + 1] - my t [i] <= maxT) return i;
	return 0;
}

static void copyRise (Sound me, double tmin, double tmax, Sound thee, double tmaxTarget) {
	long imin, imax, imaxTarget, distance, i;
	double dphase;
	imin = Sampled_xToHighIndex (me, tmin);
	if (imin < 1) imin = 1;
	imax = Sampled_xToHighIndex (me, tmax) - 1;   /* Not xToLowIndex: ensure separation of subsequent calls. */
	if (imax > my nx) imax = my nx;
	if (imax < imin) return;
	imaxTarget = Sampled_xToHighIndex (thee, tmaxTarget) - 1;
	distance = imaxTarget - imax;
	dphase = NUMpi / (imax - imin + 1);
	for (i = imin; i <= imax; i ++) {
		long iTarget = i + distance;
		if (iTarget >= 1 && iTarget <= thy nx)
			thy z [1] [iTarget] += my z [1] [i] * 0.5 * (1 - cos (dphase * (i - imin + 0.5)));
	}
}

static void copyFall (Sound me, double tmin, double tmax, Sound thee, double tminTarget) {
	long imin, imax, iminTarget, distance, i;
	double dphase;
	imin = Sampled_xToHighIndex (me, tmin);
	if (imin < 1) imin = 1;
	imax = Sampled_xToHighIndex (me, tmax) - 1;   /* Not xToLowIndex: ensure separation of subsequent calls. */
	if (imax > my nx) imax = my nx;
	if (imax < imin) return;
	iminTarget = Sampled_xToHighIndex (thee, tminTarget);
	distance = iminTarget - imin;
	dphase = NUMpi / (imax - imin + 1);
	for (i = imin; i <= imax; i ++) {
		long iTarget = i + distance;
		if (iTarget >= 1 && iTarget <= thy nx)
			thy z [1] [iTarget] += my z [1] [i] * 0.5 * (1 + cos (dphase * (i - imin + 0.5)));
	}
}

static void copyBell (Sound me, double tmid, double leftWidth, double rightWidth, Sound thee, double tmidTarget) {
	copyRise (me, tmid - leftWidth, tmid, thee, tmidTarget);
	copyFall (me, tmid, tmid + rightWidth, thee, tmidTarget);
}

static void copyBell2 (Sound me, PointProcess source, long isource, double leftWidth, double rightWidth,
	Sound thee, double tmidTarget, double maxT)
{
	/*
	 * Replace 'leftWidth' and 'rightWidth' by the lengths of the intervals in the source (instead of target),
	 * if these are shorter.
	 */
	double tmid = source -> t [isource];
	if (isource > 1 && tmid - source -> t [isource - 1] <= maxT) {
		double sourceLeftWidth = tmid - source -> t [isource - 1];
		if (sourceLeftWidth < leftWidth) leftWidth = sourceLeftWidth;
	}
	if (isource < source -> nt && source -> t [isource + 1] - tmid <= maxT) {
		double sourceRightWidth = source -> t [isource + 1] - tmid;
		if (sourceRightWidth < rightWidth) rightWidth = sourceRightWidth;
	}
	copyBell (me, tmid, leftWidth, rightWidth, thee, tmidTarget);
}

static void copyFlat (Sound me, double tmin, double tmax, Sound thee, double tminTarget) {
	long imin, imax, iminTarget;
	imin = Sampled_xToHighIndex (me, tmin);
	if (imin < 1) imin = 1;
	imax = Sampled_xToHighIndex (me, tmax) - 1;   /* Not xToLowIndex: ensure separation of subsequent calls. */
	if (imax > my nx) imax = my nx;
	if (imax < imin) return;
	iminTarget = Sampled_xToHighIndex (thee, tminTarget);
	NUMfvector_copyElements (my z [1] + imin, thy z [1] + iminTarget, 0, imax - imin);
}

Sound Sound_Point_Point_to_Sound (Sound me, PointProcess source, PointProcess target, double maxT) {
	Sound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
	long i;
	if (! thee) return NULL;

	if (source -> nt < 2 || target -> nt < 2) {   /* Almost completely voiceless? */
		NUMfvector_copyElements (my z [1], thy z [1], 1, my nx);
		return thee;
	}
		
	for (i = 1; i <= target -> nt; i ++) {
		double tmid = target -> t [i];
		double tleft = i > 1 ? target -> t [i - 1] : my xmin;
		double tright = i < target -> nt ? target -> t [i + 1] : my xmax;
		double leftWidth = tmid - tleft, rightWidth = tright - tmid;
		int leftVoiced = i > 1 && leftWidth <= maxT;
		int rightVoiced = i < target -> nt && rightWidth <= maxT;
		long isource = PointProcess_getNearestIndex (source, tmid);
		if (! leftVoiced) leftWidth = rightWidth;   /* Symmetric bell. */
		if (! rightVoiced) rightWidth = leftWidth;   /* Symmetric bell. */
		if (leftVoiced || rightVoiced) {
			copyBell2 (me, source, isource, leftWidth, rightWidth, thee, tmid, maxT);
			if (! leftVoiced) {
				double startOfFlat = i == 1 ? tleft : (tleft + tmid) / 2;
				double endOfFlat = tmid - leftWidth;
				copyFlat (me, startOfFlat, endOfFlat, thee, startOfFlat);
				copyFall (me, endOfFlat, tmid, thee, endOfFlat);
			} else if (! rightVoiced) {
				double startOfFlat = tmid + rightWidth;
				double endOfFlat = i == target -> nt ? tright : (tmid + tright) / 2;
				copyRise (me, tmid, startOfFlat, thee, startOfFlat);
				copyFlat (me, startOfFlat, endOfFlat, thee, startOfFlat);
			}
		} else {
			double startOfFlat = i == 1 ? tleft : (tleft + tmid) / 2;
			double endOfFlat = i == target -> nt ? tright : (tmid + tright) / 2;
			copyFlat (me, startOfFlat, endOfFlat, thee, startOfFlat);
		}
	}
		
	return thee;
}

Sound Sound_Point_Pitch_Duration_to_Sound (Sound me, PointProcess pulses,
	PitchTier pitch, DurationTier duration, double maxT)
{
	Sound thee = NULL;
	long ipointleft, ipointright;
	double deltat = 0, handledTime = my xmin;
	double startOfSourceNoise, endOfSourceNoise, startOfTargetNoise, endOfTargetNoise;
	double durationOfSourceNoise, durationOfTargetNoise;
	double startOfSourceVoice, endOfSourceVoice, startOfTargetVoice, endOfTargetVoice;
	double durationOfSourceVoice, durationOfTargetVoice;
	double startingPeriod, finishingPeriod, ttarget, voicelessPeriod;
	if (duration -> points -> size == 0) return Melder_errorp ("No duration points.");

	/*
	 * Create a Sound long enough to hold the longest possible duration-manipulated sound.
	 */
	thee = Sound_create (1, my xmin, my xmin + 3 * (my xmax - my xmin), 3 * my nx, my dx, my x1);
	if (! thee) return NULL;

	/*
	 * Below, I'll abbreviate the voiced interval as "voice" and the voiceless interval as "noise".
	 */
	if (pitch && pitch -> points -> size) for (ipointleft = 1; ipointleft <= pulses -> nt; ipointleft = ipointright + 1) {
		/*
		 * Find the beginning of the voice.
		 */
		startOfSourceVoice = pulses -> t [ipointleft];   /* The first pulse of the voice. */
		startingPeriod = 1.0 / RealTier_getValueAtTime (pitch, startOfSourceVoice);
		startOfSourceVoice -= 0.5 * startingPeriod;   /* The first pulse is in the middle of a period. */

		/*
		 * Measure one noise.
		 */
		startOfSourceNoise = handledTime;
		endOfSourceNoise = startOfSourceVoice;
		durationOfSourceNoise = endOfSourceNoise - startOfSourceNoise;
		startOfTargetNoise = startOfSourceNoise + deltat;
		endOfTargetNoise = startOfTargetNoise + RealTier_getArea (duration, startOfSourceNoise, endOfSourceNoise);
		durationOfTargetNoise = endOfTargetNoise - startOfTargetNoise;

		/*
		 * Copy the noise.
		 */
		voicelessPeriod = NUMrandomUniform (0.008, 0.012);
		ttarget = startOfTargetNoise + 0.5 * voicelessPeriod;
		while (ttarget < endOfTargetNoise) {
			double tsource;
			double tleft = startOfSourceNoise, tright = endOfSourceNoise;
			int i;
			for (i = 1; i <= 15; i ++) {
				double tsourcemid = 0.5 * (tleft + tright);
				double ttargetmid = startOfTargetNoise + RealTier_getArea (duration,
					startOfSourceNoise, tsourcemid);
				if (ttargetmid < ttarget) tleft = tsourcemid; else tright = tsourcemid;
			}
			tsource = 0.5 * (tleft + tright);
			copyBell (me, tsource, voicelessPeriod, voicelessPeriod, thee, ttarget);
			voicelessPeriod = NUMrandomUniform (0.008, 0.012);
			ttarget += voicelessPeriod;
		}
		deltat += durationOfTargetNoise - durationOfSourceNoise;

		/*
		 * Find the end of the voice.
		 */
		for (ipointright = ipointleft + 1; ipointright <= pulses -> nt; ipointright ++)
			if (pulses -> t [ipointright] - pulses -> t [ipointright - 1] > maxT)
				break;
		ipointright --;
		endOfSourceVoice = pulses -> t [ipointright];   /* The last pulse of the voice. */
		finishingPeriod = 1.0 / RealTier_getValueAtTime (pitch, endOfSourceVoice);
		endOfSourceVoice += 0.5 * finishingPeriod;   /* The last pulse is in the middle of a period. */
		/*
		 * Measure one voice.
		 */
		durationOfSourceVoice = endOfSourceVoice - startOfSourceVoice;

		/*
		 * This will be copied to an interval with a different location and duration.
		 */
		startOfTargetVoice = startOfSourceVoice + deltat;
		endOfTargetVoice = startOfTargetVoice +
			RealTier_getArea (duration, startOfSourceVoice, endOfSourceVoice);
		durationOfTargetVoice = endOfTargetVoice - startOfTargetVoice;

		/*
		 * Copy the voiced part.
		 */
		ttarget = startOfTargetVoice + 0.5 * startingPeriod;
		while (ttarget < endOfTargetVoice) {
			double tsource, period;
			long isourcepulse;
			double tleft = startOfSourceVoice, tright = endOfSourceVoice;
			int i;
			for (i = 1; i <= 15; i ++) {
				double tsourcemid = 0.5 * (tleft + tright);
				double ttargetmid = startOfTargetVoice + RealTier_getArea (duration,
					startOfSourceVoice, tsourcemid);
				if (ttargetmid < ttarget) tleft = tsourcemid; else tright = tsourcemid;
			}
			tsource = 0.5 * (tleft + tright);
			period = 1.0 / RealTier_getValueAtTime (pitch, tsource);
			isourcepulse = PointProcess_getNearestIndex (pulses, tsource);
			copyBell2 (me, pulses, isourcepulse, period, period, thee, ttarget, maxT);
			ttarget += period;
		}
		deltat += durationOfTargetVoice - durationOfSourceVoice;
		handledTime = endOfSourceVoice;
	}

	/*
	 * Copy the remaining unvoiced part, if we are at the end.
	 */
	startOfSourceNoise = handledTime;
	endOfSourceNoise = my xmax;
	durationOfSourceNoise = endOfSourceNoise - startOfSourceNoise;
	startOfTargetNoise = startOfSourceNoise + deltat;
	endOfTargetNoise = startOfTargetNoise + RealTier_getArea (duration, startOfSourceNoise, endOfSourceNoise);
	durationOfTargetNoise = endOfTargetNoise - startOfTargetNoise;
	voicelessPeriod = NUMrandomUniform (0.008, 0.012);
	ttarget = startOfTargetNoise + 0.5 * voicelessPeriod;
	while (ttarget < endOfTargetNoise) {
		double tsource;
		double tleft = startOfSourceNoise, tright = endOfSourceNoise;
		int i;
		for (i = 1; i <= 15; i ++) {
			double tsourcemid = 0.5 * (tleft + tright);
			double ttargetmid = startOfTargetNoise + RealTier_getArea (duration,
				startOfSourceNoise, tsourcemid);
			if (ttargetmid < ttarget) tleft = tsourcemid; else tright = tsourcemid;
		}
		tsource = 0.5 * (tleft + tright);
		copyBell (me, tsource, voicelessPeriod, voicelessPeriod, thee, ttarget);
		voicelessPeriod = NUMrandomUniform (0.008, 0.012);
		ttarget += voicelessPeriod;
	}

	/*
	 * Find the number of trailing zeroes and hack the sound's time domain.
	 */
	thy xmax = thy xmin + RealTier_getArea (duration, my xmin, my xmax);
	if (fabs (thy xmax - my xmax) < 1e-12) thy xmax = my xmax;   /* Common situation. */
	thy nx = Sampled_xToLowIndex (thee, thy xmax);
	if (thy nx > 3 * my nx) thy nx = 3 * my nx;

	return thee;
}

static Sound synthesize_psola_nodur (Manipulation me) {
	PointProcess targetPulses = NULL;
	Sound thee = NULL;
	if (! my sound) return Melder_errorp ("Cannot synthesize PSOLA without original sound.");
	if (! my pulses) return Melder_errorp ("Cannot synthesize PSOLA without pulses analysis.");
	if (! my pitch) return Melder_errorp ("Cannot synthesize PSOLA without pitch manipulation.");
	if ((targetPulses = PitchTier_Point_to_PointProcess (my pitch, my pulses, MAX_T)) != NULL)
		thee = Sound_Point_Point_to_Sound (my sound, my pulses, targetPulses, MAX_T);
	forget (targetPulses);
	if (Melder_hasError ())
		{ forget (thee); return Melder_errorp ("(Manipulation_to_Sound:) Not performed."); }
	return thee;
}

static Sound synthesize_psola (Manipulation me) {
	Sound thee = NULL;
	if (! my duration || my duration -> points -> size == 0) return synthesize_psola_nodur (me);
	if (! my sound) return Melder_errorp ("Cannot synthesize PSOLA without original sound.");
	if (! my pulses) return Melder_errorp ("Cannot synthesize PSOLA without pulses analysis.");
	if (! my pitch) return Melder_errorp ("Cannot synthesize PSOLA without pitch manipulation.");
	if (! (thee = Sound_Point_Pitch_Duration_to_Sound (my sound, my pulses, my pitch, my duration, MAX_T))) return NULL;
	if (Melder_hasError ())
		{ forget (thee); return Melder_errorp ("(Manipulation_to_Sound:) Not performed."); }
	return thee;
}

static Sound synthesize_pulses (Manipulation me) {
	if (! my pulses) return Melder_errorp ("Cannot synthesize PSOLA without pulses analysis.");
	return PointProcess_to_Sound_pulseTrain (my pulses, 44100, 0.7, 0.05, 30);
}

static Sound synthesize_pulses_hum (Manipulation me) {
	if (! my pulses) return Melder_errorp ("Cannot synthesize PSOLA without pulses analysis.");
	return PointProcess_to_Sound_hum (my pulses);
}

static Sound synthesize_pitch (Manipulation me) {
	Sound thee = NULL;
	PointProcess temp = NULL;
	if (! my pitch) return Melder_errorp ("Cannot synthesize pitch manipulation without pitch manipulation.");
	temp = PitchTier_to_PointProcess (my pitch);
	if (! temp) return NULL;
	thee = PointProcess_to_Sound_pulseTrain (temp, 44100, 0.7, 0.05, 30);
	if (! thee) { forget (temp); return NULL; }
	forget (temp);
	return thee;
}

static Sound synthesize_pitch_hum (Manipulation me) {
	Sound thee = NULL;
	PointProcess temp = NULL;
	if (! my pitch) return Melder_errorp ("Cannot synthesize pitch manipulation without pitch manipulation.");
	temp = PitchTier_to_PointProcess (my pitch);
	if (! temp) return NULL;
	thee = PointProcess_to_Sound_hum (temp);
	if (! thee) { forget (temp); return NULL; }
	forget (temp);
	return thee;
}

static Sound synthesize_pulses_pitch (Manipulation me) {
	Sound thee = NULL;
	PointProcess temp = NULL;
	if (! my pulses) return Melder_errorp ("Cannot synthesize this without pulses analysis.");
	if (! my pitch) return Melder_errorp ("Cannot synthesize this without pitch manipulation.");
	temp = PitchTier_Point_to_PointProcess (my pitch, my pulses, MAX_T);
	if (! temp) return NULL;
	thee = PointProcess_to_Sound_pulseTrain (temp, 44100, 0.7, 0.05, 30);
	if (! thee) { forget (temp); return NULL; }
	forget (temp);
	return thee;
}

static Sound synthesize_pulses_pitch_hum (Manipulation me) {
	Sound thee = NULL;
	PointProcess temp = NULL;
	if (! my pulses) return Melder_errorp ("Cannot synthesize this without pulses analysis.");
	if (! my pitch) return Melder_errorp ("Cannot synthesize this without pitch manipulation.");
	temp = PitchTier_Point_to_PointProcess (my pitch, my pulses, MAX_T);
	if (! temp) return NULL;
	thee = PointProcess_to_Sound_hum (temp);
	if (! thee) { forget (temp); return NULL; }
	forget (temp);
	return thee;
}

/*
void Sound_Formant_Intensity_filter (Sound me, FormantTier formantTier, IntensityTier intensity) {
	Sound_FormantTier_filter_inline (me, formantTier);
	if (intensity) Sound_IntensityTier_multiply_inline (me, intensity);
	NUMdeemphasize_f (my z [1], my nx, my dx, 50.0);
	Vector_scale (me, 0.99);
}

static Sound synthesize_pulses_formant (Manipulation me, int useIntensity) {
	Sound thee = NULL;
	PointProcess temp = NULL;
	if (! my pulses) return Melder_errorp ("Cannot synthesize this without pulses analysis.");
	if (! my formant) return Melder_errorp ("Cannot synthesize this without formant information.");
	thee = PointProcess_to_Sound (my pulses, 44100, 0.7, 0.05, 30);
	if (! thee) return Melder_errorp ("Manipulation_to_Sound: not performed.");
	Sound_Formant_Intensity_filter (thee, my formant, useIntensity ? my intensity : NULL);
	return thee;
}
*/

static void Sound_PointProcess_fillVoiceless (Sound me, PointProcess pulses) {
	long ipointleft, ipointright;
	double beginVoiceless = my xmin, endVoiceless;
	for (ipointleft = 1; ipointleft <= pulses -> nt; ipointleft = ipointright + 1) {
		long i1, i2, i;
		endVoiceless = pulses -> t [ipointleft] - 0.005;
		i1 = Sampled_xToHighIndex (me, beginVoiceless);
		if (i1 < 1) i1 = 1; if (i1 > my nx) i1 = my nx;
		i2 = Sampled_xToLowIndex (me, endVoiceless);
		if (i2 < 1) i2 = 1; if (i2 > my nx) i2 = my nx;
		if (i2 - i1 > 10) for (i = i1; i <= i2; i ++)
			my z [1] [i] = NUMrandomGauss (0.0, 0.3);
		for (ipointright = ipointleft + 1; ipointright <= pulses -> nt; ipointright ++)
			if (pulses -> t [ipointright] - pulses -> t [ipointright - 1] > MAX_T)
				break;
		ipointright --;
		beginVoiceless = pulses -> t [ipointright] + 0.005;
	}
	endVoiceless = my xmax;
	{
		long i1, i2, i;
		i1 = Sampled_xToHighIndex (me, beginVoiceless);
		if (i1 < 1) i1 = 1; if (i1 > my nx) i1 = my nx;
		i2 = Sampled_xToLowIndex (me, endVoiceless);
		if (i2 < 1) i2 = 1; if (i2 > my nx) i2 = my nx;
		if (i2 - i1 > 10) for (i = i1; i <= i2; i ++)
			my z [1] [i] = NUMrandomGauss (0.0, 0.3);
	}
}

static Sound synthesize_pulses_lpc (Manipulation me) {
	Sound train = NULL, result = NULL;
	if (! my lpc) {
		Sound sound10k;
		if (! my sound) return Melder_errorp ("Cannot synthesize this without LPC or original sound.");
		sound10k = Sound_resample (my sound, 10000, 50);
		if (! sound10k) return NULL;
		my lpc = Sound_to_LPC_burg (sound10k, 20, 0.025, 0.01, 50.0);
		forget (sound10k);
		if (! my lpc) return NULL;
	}
	if (! my pulses) return Melder_errorp ("Cannot synthesize this without pulses analysis.");
	train = PointProcess_to_Sound_pulseTrain (my pulses, 1 / my lpc -> samplingPeriod, 0.7, 0.05, 30);
	if (! train) return Melder_errorp ("Manipulation_to_Sound: not performed.");
	train -> dx = my lpc -> samplingPeriod;   /* To be exact. */
	Sound_PointProcess_fillVoiceless (train, my pulses);
	result = LPC_and_Sound_filter (my lpc, train, TRUE);
	forget (train);
	if (! result) return NULL;
	NUMdeemphasize_f (result -> z [1], result -> nx, result -> dx, 50.0);
	Vector_scale (result, 0.99);
	return result;
}

static Sound synthesize_pitch_lpc (Manipulation me) {
	Sound train = NULL, result = NULL;
	PointProcess temp = NULL;
	if (! my lpc) {
		Sound sound10k;
		if (! my sound) return Melder_errorp ("Cannot synthesize this without LPC or original sound.");
		sound10k = Sound_resample (my sound, 10000, 50);
		if (! sound10k) return NULL;
		my lpc = Sound_to_LPC_burg (sound10k, 20, 0.025, 0.01, 50.0);
		forget (sound10k);
		if (! my lpc) return NULL;
	}
	if (! my pitch) return Melder_errorp ("Cannot synthesize pitch manipulation without pitch manipulation.");
	if (! my pulses) return Melder_errorp ("Cannot synthesize pitch manipulation without pulses.");
	temp = PitchTier_Point_to_PointProcess (my pitch, my pulses, MAX_T);
	if (! temp) return NULL;
	train = PointProcess_to_Sound_pulseTrain (temp, 1 / my lpc -> samplingPeriod, 0.7, 0.05, 30);
	forget (temp);
	if (! train) return NULL;
	train -> dx = my lpc -> samplingPeriod;   /* To be exact. */
	Sound_PointProcess_fillVoiceless (train, my pulses);
	result = LPC_and_Sound_filter (my lpc, train, TRUE);
	forget (train);
	if (! result) return NULL;
	NUMdeemphasize_f (result -> z [1], result -> nx, result -> dx, 50.0);
	Vector_scale (result, 0.99);
	return result;
}

Sound Manipulation_to_Sound (Manipulation me, int method) {
	switch (method) {
		case Manipulation_PSOLA: return synthesize_psola (me);
		case Manipulation_PULSES: return synthesize_pulses (me);
		case Manipulation_PULSES_HUM: return synthesize_pulses_hum (me);
		case Manipulation_PITCH: return synthesize_pitch (me);
		case Manipulation_PITCH_HUM: return synthesize_pitch_hum (me);
		case Manipulation_PULSES_PITCH: return synthesize_pulses_pitch (me);
		case Manipulation_PULSES_PITCH_HUM: return synthesize_pulses_pitch_hum (me);
		case Manipulation_PSOLA_NODUR: return synthesize_psola_nodur (me);
		case Manipulation_PULSES_FORMANT: return 0;
		case Manipulation_PULSES_FORMANT_INTENSITY: return 0;
		case Manipulation_PULSES_LPC: return synthesize_pulses_lpc (me);
		case Manipulation_PULSES_LPC_INTENSITY: return 0;
		case Manipulation_PITCH_LPC: return synthesize_pitch_lpc (me);
		case Manipulation_PITCH_LPC_INTENSITY: return 0;
		default: return synthesize_psola (me);
	}
}

Manipulation Manipulation_AnyTier_to_Manipulation (Manipulation ana, AnyTier tier) {
	Manipulation result = NULL;
	PitchTier pitch = NULL;
	result = Data_copy (ana);
	if (! result) goto end;
	pitch = PitchTier_AnyTier_to_PitchTier (ana -> pitch, tier);
	if (! pitch) goto end;
	forget (result -> pitch);
	result -> pitch = pitch;
	pitch = NULL;
end:
	if (Melder_hasError ()) Melder_error ("(Manipulation_AnyTier_to_Manipulation:) Not performed.");
	return result;
}

int Manipulation_writeToTextFileWithoutSound (Manipulation me, MelderFile fs) {
	int status;
	Sound saved = my sound;
	my sound = NULL;
	status = Data_writeToTextFile (me, fs);
	my sound = saved;
	return status;
}

int Manipulation_writeToBinaryFileWithoutSound (Manipulation me, MelderFile fs) {
	int status;
	Sound saved = my sound;
	my sound = NULL;
	status = Data_writeToBinaryFile (me, fs);
	my sound = saved;
	return status;
}

/* End of file Manipulation.c */
