#ifndef _Manipulation_h_
#define _Manipulation_h_
/* Manipulation.h
 *
 * Copyright (C) 1992-2005,2007,2011,2015,2016,2018 Paul Boersma
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
#include "PitchTier.h"
#include "DurationTier.h"
#include "LPC.h"

/* The following have to be included for compatibility. */
#include "IntensityTier.h"
#include "Intensity.h"
#include "FormantTier.h"
#include "Formant.h"
#include "Pitch.h"
#include "Image.h"

#include "Manipulation_def.h"

/* How to create an Manipulation. */

autoManipulation Manipulation_create (double tmin, double tmax);
void Manipulation_replaceOriginalSound (Manipulation me, Sound sound);
void Manipulation_replacePulses (Manipulation me, PointProcess pulses);
void Manipulation_replaceIntensityTier (Manipulation me, IntensityTier intensity);
void Manipulation_replacePitchTier (Manipulation me, PitchTier pitch);
void Manipulation_replaceDurationTier (Manipulation me, DurationTier duration);

autoManipulation Sound_to_Manipulation (Sound me, double timeStep, double minimumPitch, double maximumPitch);
autoManipulation Sound_Pitch_to_Manipulation (Sound sound, Pitch pitch);
autoManipulation Sound_PointProcess_to_Manipulation (Sound sound, PointProcess point);
autoManipulation Manipulation_AnyTier_to_Manipulation (Manipulation manip, AnyTier tier);

/* Resynthesis. */

#define Manipulation_OVERLAPADD  1
#define Manipulation_PULSES  2
#define Manipulation_PULSES_HUM  3
#define Manipulation_PITCH  4
#define Manipulation_PITCH_HUM  5
#define Manipulation_PULSES_PITCH  6
#define Manipulation_PULSES_PITCH_HUM  7
#define Manipulation_OVERLAPADD_NODUR  8
#define Manipulation_PULSES_FORMANT  9
#define Manipulation_PULSES_FORMANT_INTENSITY  10
#define Manipulation_PULSES_LPC  11
#define Manipulation_PULSES_LPC_INTENSITY  12
#define Manipulation_PITCH_LPC  13
#define Manipulation_PITCH_LPC_INTENSITY  14
#define Manipulation_PITCH_LPC_INT_DUR  15
autoSound Sound_Point_Point_to_Sound (Sound me, PointProcess source, PointProcess target, double maxT);
/*void Sound_Formant_Intensity_filter (Sound me, FormantTier formant, IntensityTier intensity);*/

autoSound Manipulation_to_Sound (Manipulation me, int method);
void Manipulation_playPart (Manipulation me, double tmin, double tmax, int method);
void Manipulation_play (Manipulation me, int method);
void Manipulation_writeToTextFileWithoutSound (Manipulation me, MelderFile file);
void Manipulation_writeToBinaryFileWithoutSound (Manipulation me, MelderFile file);

/* The low-level synthesis routines. */

autoSound Sound_Point_Pitch_Duration_to_Sound (Sound me, PointProcess pulses,
	PitchTier pitch, DurationTier duration, double maxT);

/* End of file Manipulation.h */
#endif
