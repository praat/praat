#ifndef _Manipulation_h_
#define _Manipulation_h_
/* Manipulation.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
oo_CLASS_CREATE (Manipulation, Function);

/* How to create an Manipulation. */

Manipulation Manipulation_create (double tmin, double tmax);
int Manipulation_replaceOriginalSound (Manipulation me, Sound sound);
int Manipulation_replacePulses (Manipulation me, PointProcess pulses);
int Manipulation_replaceIntensityTier (Manipulation me, IntensityTier intensity);
int Manipulation_replacePitchTier (Manipulation me, PitchTier pitch);
int Manipulation_replaceDurationTier (Manipulation me, DurationTier duration);

Manipulation Sound_to_Manipulation (Sound me, double timeStep, double minimumPitch, double maximumPitch);
Manipulation Sound_Pitch_to_Manipulation (Sound sound, Pitch pitch);
Manipulation Sound_PointProcess_to_Manipulation (Sound sound, PointProcess point);
Manipulation Manipulation_AnyTier_to_Manipulation (Manipulation manip, AnyTier tier);

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
Sound Sound_Point_Point_to_Sound (Sound me, PointProcess source, PointProcess target, double maxT);
/*void Sound_Formant_Intensity_filter (Sound me, FormantTier formant, IntensityTier intensity);*/

Sound Manipulation_to_Sound (Manipulation me, int method);
int Manipulation_playPart (Manipulation me, double tmin, double tmax, int method);
int Manipulation_play (Manipulation me, int method);
int Manipulation_writeToTextFileWithoutSound (Manipulation me, MelderFile fs);
int Manipulation_writeToBinaryFileWithoutSound (Manipulation me, MelderFile fs);

/* The low-level synthesis routines. */

Sound Sound_Point_Pitch_Duration_to_Sound (Sound me, PointProcess pulses,
	PitchTier pitch, DurationTier duration, double maxT);

/* End of file Manipulation.h */
#endif
