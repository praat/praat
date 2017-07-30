/* Pitch_Intensity.cpp
 *
 * Copyright (C) 1992-2011,2014,2015,2016,2017 Paul Boersma
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
	*minimum = 1e308, *maximum = -1e308;
	for (long i = 1; i <= my nx; i ++) {
		double frequency = my frame [i]. candidate [1]. frequency;
		if (frequency == 0.0) continue;   // voiceless
		if (frequency < *minimum) *minimum = frequency;
		if (frequency > *maximum) *maximum = frequency;
	}
	if (*maximum == -1e308) *maximum = 0.0;
	if (*minimum == 1e308) *minimum = 0.0;
}

void Pitch_Intensity_draw (Pitch pitch, Intensity intensity, Graphics g,
	double f1, double f2, double s1, double s2, bool garnish, int connect)
{
	if (f2 <= f1) Pitch_getExtrema (pitch, & f1, & f2);
	if (f1 == 0.0) return;   // all voiceless
	if (f1 == f2) { f1 -= 1.0; f2 += 1.0; }
	if (s2 <= s1) Matrix_getWindowExtrema (intensity, 0, 0, 1, 1, & s1, & s2);
	if (s1 == s2) { s1 -= 1.0; s2 += 1.0; }
	Graphics_setWindow (g, f1, f2, s1, s2);
	Graphics_setInner (g);
	double previousX = undefined;
	double previousY = undefined;
	long previousI = 0;
	for (long i = 1; i <= pitch -> nx; i ++) {
		double t = Sampled_indexToX (pitch, i);
		double x = pitch -> frame [i]. candidate [1]. frequency;
		double y = Sampled_getValueAtX (intensity, t, Pitch_LEVEL_FREQUENCY, kPitch_unit_HERTZ, true);
		if (x == 0) {
			continue;   // voiceless
		}
		if (connect & 1) Graphics_speckle (g, x, y);
		if ((connect & 2) && isdefined (previousX)) {
			if (previousI >= 1 && previousI < i - 1) {
				Graphics_setLineType (g, Graphics_DOTTED);
			}
			Graphics_line (g, previousX, previousY, x, y);
			Graphics_setLineType (g, Graphics_DRAWN);
		}
		previousX = x;
		previousY = y;
		previousI = i;
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
	long numberOfValidLocalMeasurements = 0;
	double sumOfLocalValues = 0.0;
	for (long iframe = 1; iframe <= my nx; iframe ++) {
		double t = Sampled_indexToX (me, iframe);
		bool localMeasurentIsValid = Pitch_isVoiced_t (thee, t);
		if (localMeasurentIsValid) {
			double localValue = my z [1] [iframe];
			sumOfLocalValues += localValue;
			numberOfValidLocalMeasurements += 1;
		}
	}
	return numberOfValidLocalMeasurements > 0 ? sumOfLocalValues / numberOfValidLocalMeasurements : undefined;
}

double Pitch_Intensity_getMeanAbsoluteSlope (Pitch thee, Intensity me) {
	long numberOfValidLocalMeasurements = 0;
	double sumOfLocalAbsoluteSlopes = 0.0;
	for (long iframe = 1; iframe < my nx; iframe ++) {
		double t1 = Sampled_indexToX (me, iframe);
		double t2 = t1 + my dx;
		bool localMeasurentIsValid = ( Pitch_isVoiced_t (thee, t1) && Pitch_isVoiced_t (thee, t2) );
		if (localMeasurentIsValid) {
			double absoluteLocalSlope = fabs (my z [1] [iframe + 1] -  my z [1] [iframe]);
			sumOfLocalAbsoluteSlopes += absoluteLocalSlope;
			numberOfValidLocalMeasurements += 1;
		}
	}
	sumOfLocalAbsoluteSlopes /= my dx;   // convert to dB per second
	return numberOfValidLocalMeasurements > 0 ? sumOfLocalAbsoluteSlopes / numberOfValidLocalMeasurements : undefined;
}

/* End of file Pitch_Intensity.cpp */
