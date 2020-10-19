/* Pitch_to_PitchTier.cpp
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2014-2020 Paul Boersma
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

#include "Pitch_to_PitchTier.h"

autoPitchTier Pitch_to_PitchTier (Pitch me) {
	try {
		autoPitchTier you = PitchTier_create (my xmin, my xmax);
		for (integer i = 1; i <= my nx; i ++) {
			const double frequency = my frames [i]. candidates [1]. frequency;
			/*
				Count only voiced frames.
			*/
			if (Pitch_util_frequencyIsVoiced (frequency, my ceiling)) {
				const double time = Sampled_indexToX (me, i);
				RealTier_addPoint (you.get(), time, frequency);
			}
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PitchTier.");
	}
}

static void Pitch_line (Pitch me, Graphics graphics, double tmin, double fleft, double tmax, double fright,
	int nonPeriodicLineType)
{
	/*
		f = fleft + (t - tmin) * (fright - fleft) / (tmax - tmin);
	*/
	const int lineType = Graphics_inqLineType (graphics);
	const double lineWidth = Graphics_inqLineWidth (graphics);
	const double slope = (fright - fleft) / (tmax - tmin);
	const integer imin = Melder_clippedLeft (1_integer, Sampled_xToNearestIndex (me, tmin));
	const integer imax = Melder_clippedRight (Sampled_xToNearestIndex (me, tmax), my nx);
	for (integer i = imin; i <= imax; i ++) {
		if (! Pitch_isVoiced_i (me, i)) {
			if (nonPeriodicLineType == 2)
				continue;
			Graphics_setLineType (graphics, Graphics_DOTTED);
			Graphics_setLineWidth (graphics, 0.67 * lineWidth);
		} else if (nonPeriodicLineType != 2) {
			Graphics_setLineWidth (graphics, 2 * lineWidth);
		}
		double tleft = Sampled_indexToX (me, i) - 0.5 * my dx;
		double tright = tleft + my dx;
		Melder_clipLeft (tmin, & tleft);   // has to be ordered after the previous line!
		Melder_clipRight (& tright, tmax);
		Graphics_line (graphics,
			tleft, fleft + (tleft - tmin) * slope,
			tright, fleft + (tright - tmin) * slope
		);
		Graphics_setLineType (graphics, lineType);
		Graphics_setLineWidth (graphics, lineWidth);
	}
}

void PitchTier_Pitch_draw (PitchTier me, Pitch uv, Graphics graphics,
	double tmin, double tmax, double fmin, double fmax, int nonPeriodicLineType, bool garnish, conststring32 method)
{
	if (nonPeriodicLineType == 0) {
		PitchTier_draw (me, graphics, tmin, tmax, fmin, fmax, garnish, method);
		return;
	}
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	Graphics_setWindow (graphics, tmin, tmax, fmin, fmax);
	Graphics_setInner (graphics);
	const integer imin = AnyTier_timeToHighIndex (me->asAnyTier(), tmin);
	const integer imax = AnyTier_timeToLowIndex (me->asAnyTier(), tmax);
	if (my points.size == 0) {
	} else if (imax < imin) {
		const double fleft = RealTier_getValueAtTime (me, tmin);
		const double fright = RealTier_getValueAtTime (me, tmax);
		Pitch_line (uv, graphics, tmin, fleft, tmax, fright, nonPeriodicLineType);
	} else for (integer i = imin; i <= imax; i ++) {
		const RealPoint point = my points.at [i];
		const double time = point -> number, frequency = point -> value;
		Graphics_speckle (graphics, time, frequency);
		if (i == 1)
			Pitch_line (uv, graphics, tmin, frequency, time, frequency, nonPeriodicLineType);
		else if (i == imin)
			Pitch_line (uv, graphics, time, frequency, tmin, RealTier_getValueAtTime (me, tmin), nonPeriodicLineType);
		if (i == my points.size)
			Pitch_line (uv, graphics, time, frequency, tmax, frequency, nonPeriodicLineType);
		else if (i == imax)
			Pitch_line (uv, graphics, time, frequency, tmax, RealTier_getValueAtTime (me, tmax), nonPeriodicLineType);
		else {
			RealPoint pointRight = my points.at [i + 1];
			Pitch_line (uv, graphics, time, frequency, pointRight -> number, pointRight -> value, nonPeriodicLineType);
		}
	}
	Graphics_unsetInner (graphics);
	if (garnish) {
		Graphics_drawInnerBox (graphics);
		Graphics_textBottom (graphics, true, U"Time (s)");
		Graphics_marksBottom (graphics, 2, true, true, false);
		Graphics_marksLeft (graphics, 2, true, true, false);
		Graphics_textLeft (graphics, true, U"Frequency (Hz)");
	}
}

autoPitch Pitch_PitchTier_to_Pitch (Pitch me, PitchTier tier) {
	try {
		if (tier -> points.size == 0)
			Melder_throw (U"No pitch points.");
		autoPitch you = Data_copy (me);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Pitch_Frame frame = & your frames [iframe];
			const Pitch_Candidate candidate = & frame -> candidates [1];
			if (Pitch_util_frequencyIsVoiced (candidate -> frequency, my ceiling))
				candidate -> frequency = RealTier_getValueAtTime (tier, Sampled_indexToX (me, iframe));
			candidate -> strength = 0.9;
			frame -> candidates. resize (frame -> nCandidates = 1);
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U" & ", tier, U": not converted to Pitch.");
	}
}

/* End of file Pitch_to_PitchTier.cpp */
