/* VoiceAnalysis.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Sound.h"
#include "PointProcess.h"
#include "Pitch.h"

double PointProcess_getJitter_local (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getJitter_local_absolute (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getJitter_rap (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getJitter_ppq5 (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getJitter_ddp (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);

double PointProcess_Sound_getShimmer_local (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor);
double PointProcess_Sound_getShimmer_local_dB (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor);
double PointProcess_Sound_getShimmer_apq3 (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor);
double PointProcess_Sound_getShimmer_apq5 (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor);
double PointProcess_Sound_getShimmer_apq11 (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor);
double PointProcess_Sound_getShimmer_dda (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor);
void PointProcess_Sound_getShimmer_multi (PointProcess me, Sound thee, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor, double maximumAmplitudeFactor,
	double *local, double *local_dB, double *apq3, double *apq5, double *apq11, double *dda);

void Sound_Pitch_PointProcess_voiceReport (Sound sound, Pitch pitch, PointProcess pulses,
	double tmin, double tmax,
	double floor, double ceiling, double maximumPeriodFactor, double maximumAmplitudeFactor,
	double silenceThreshold, double voicingThreshold);

/* End of file VoiceAnalysis.h */
