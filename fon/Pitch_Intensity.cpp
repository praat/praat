/* Pitch_Intensity.cpp
 *
 * Copyright (C) 1992-2007,2011,2014-2017,2019 Paul Boersma
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

#include "Pitch_Intensity.h"

static void Pitch_getExtrema (Pitch me, double *minimum, double *maximum) {
	MelderExtremaWithInit extrema;
	for (integer i = 1; i <= my nx; i ++) {
		const double frequency = my frames [i]. candidates [1]. frequency;
		if (frequency == 0.0)
			continue;   // voiceless
		extrema.update (frequency);
	}
	if (extrema.isValid()) {
		*minimum = extrema.min;
		*maximum = extrema.max;
	} else {
		*minimum = *maximum = 0.0;
	}
}

void Pitch_Intensity_draw (Pitch pitchObject, Intensity intensityObject, Graphics g,
	double f1, double f2, double s1, double s2, bool garnish, int connect)
{
	if (f1 == f2)
		Pitch_getExtrema (pitchObject, & f1, & f2);   // autowindow
	if (f1 == 0.0)   // all voiceless?
		return;
	if (f1 == f2) {
		f1 -= 1.0;
		f2 += 1.0;
	}
	if (s1 == s2)
		Matrix_getWindowExtrema (intensityObject, 0, 0, 1, 1, & s1, & s2);   // autowindow
	if (s1 == s2) {
		s1 -= 1.0;
		s2 += 1.0;
	}
	Graphics_setWindow (g, f1, f2, s1, s2);
	Graphics_setInner (g);
	double previousPitchValue = undefined;
	double previousIntensityValue = undefined;
	integer previousPitchFrameNumber = 0;
	for (integer ipitchFrame = 1; ipitchFrame <= pitchObject -> nx; ipitchFrame ++) {
		/*
			Get pitch value.
		*/
		const bool pitchMeasurementWillBeValid = Pitch_isVoiced_i (pitchObject, ipitchFrame);
		if (! pitchMeasurementWillBeValid)
			continue;   // voiceless -> don't draw
		const Pitch_Frame pitchFrame = & pitchObject -> frames [ipitchFrame];
		constexpr integer winningPitchCandidateNumber = 1;
		const double pitchValue = pitchFrame -> candidates [winningPitchCandidateNumber]. frequency;
		/*
			Get the corresponding intensity value.
			"Corresponding" means: measure the intensity at the same time as the pitch.
		*/
		const double time = Sampled_indexToX (pitchObject, ipitchFrame);
		constexpr integer onlyIntensityChannel = 1;
		constexpr integer defaultUnit = 0;
		const double intensityValue = Sampled_getValueAtX (intensityObject, time, onlyIntensityChannel, defaultUnit, true);
		const bool intensityMeasurementIsValid = isdefined (intensityValue);
		if (! intensityMeasurementIsValid)
			continue;   // no intensity measured, e.g. at the edges of the time domain -> don't draw
		/*
			Draw.
		*/
		constexpr integer shouldSpeckle_mask = 1;
		constexpr integer shouldCurve_mask = 2;
		if (connect & shouldSpeckle_mask)
			Graphics_speckle (g, pitchValue, intensityValue);
		if ((connect & shouldCurve_mask) && isdefined (previousPitchValue)) {
			/*
				We draw a solid line if the previous point represented the previous frame,
				but a dotted line if, instead, the previous frame was voiceless.
			*/
			if (previousPitchFrameNumber >= 1 && previousPitchFrameNumber < ipitchFrame - 1)
				Graphics_setLineType (g, Graphics_DOTTED);
			Graphics_line (g, previousPitchValue, previousIntensityValue, pitchValue, intensityValue);
			Graphics_setLineType (g, Graphics_DRAWN);
		}
		/*
			Cycle.
		*/
		previousPitchValue = pitchValue;
		previousIntensityValue = intensityValue;
		previousPitchFrameNumber = ipitchFrame;
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Fundamental frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Intensity (dB)");
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

double Pitch_Intensity_getMean (Pitch thee, Intensity me) {
	integer numberOfValidLocalMeasurements = 0;
	longdouble sumOfLocalValues = 0.0;
	for (integer iframe = 1; iframe <= my nx; iframe ++) {
		const double time = Sampled_indexToX (me, iframe);
		const bool localMeasurentIsValid = Pitch_isVoiced_t (thee, time);
		if (localMeasurentIsValid) {
			double localValue = my z [1] [iframe];
			sumOfLocalValues += localValue;
			numberOfValidLocalMeasurements += 1;
		}
	}
	return numberOfValidLocalMeasurements > 0 ? double (sumOfLocalValues) / numberOfValidLocalMeasurements : undefined;
}

double Pitch_Intensity_getMeanAbsoluteSlope (Pitch thee, Intensity me) {
	integer numberOfValidLocalMeasurements = 0;
	longdouble sumOfLocalAbsoluteSlopes = 0.0;
	for (integer iframe = 1; iframe < my nx; iframe ++) {
		const double t1 = Sampled_indexToX (me, iframe);
		const double t2 = t1 + my dx;
		const bool localMeasurentIsValid = ( Pitch_isVoiced_t (thee, t1) && Pitch_isVoiced_t (thee, t2) );
		if (localMeasurentIsValid) {
			double absoluteLocalSlope = fabs (my z [1] [iframe + 1] -  my z [1] [iframe]);
			sumOfLocalAbsoluteSlopes += absoluteLocalSlope;
			numberOfValidLocalMeasurements += 1;
		}
	}
	sumOfLocalAbsoluteSlopes /= my dx;   // convert to dB per second
	return numberOfValidLocalMeasurements > 0 ? double (sumOfLocalAbsoluteSlopes) / numberOfValidLocalMeasurements : undefined;
}

/* End of file Pitch_Intensity.cpp */
