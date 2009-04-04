/* Pitch_to_PitchTier.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1998/03/19
 * pb 2002/07/16 GPL
 */

#include "Pitch_to_PitchTier.h"

PitchTier Pitch_to_PitchTier (Pitch me) {
	PitchTier thee = PitchTier_create (my xmin, my xmax);   /* Same domain, for synchronization. */
	long i;
	if (! thee) goto error;
	for (i = 1; i <= my nx; i ++) {
		double frequency = my frame [i]. candidate [1]. frequency;

		/* Count only voiced frames.
		 */
		if (frequency > 0.0 && frequency < my ceiling) {
			double time = Sampled_indexToX (me, i);
			if (! RealTier_addPoint (thee, time, frequency)) goto error;
		}
	}
	return thee;
error:
	forget (thee);
	return Melder_errorp ("(Pitch_to_PitchTier: ) Not performed.");
}

static void Pitch_line (Pitch me, Graphics g, double tmin, double fleft, double tmax, double fright,
	int nonPeriodicLineType)
{
	/*
	 * f = fleft + (t - tmin) * (fright - fleft) / (tmax - tmin);
	 */
	int lineType = Graphics_inqLineType (g);
	double lineWidth = Graphics_inqLineWidth (g);
	double slope = (fright - fleft) / (tmax - tmin);
	long imin = Sampled_xToNearestIndex (me, tmin);
	long imax = Sampled_xToNearestIndex (me, tmax), i;
	if (imin < 1) imin = 1;
	if (imax > my nx) imax = my nx;
	for (i = imin; i <= imax; i ++) {
		double tleft, tright;
		if (! Pitch_isVoiced_i (me, i)) {
			if (nonPeriodicLineType == 2) continue;
			Graphics_setLineType (g, Graphics_DOTTED);
			Graphics_setLineWidth (g, 0.67 * lineWidth);
		} else if (nonPeriodicLineType != 2) {
			Graphics_setLineWidth (g, 2 * lineWidth);
		}
		tleft = Sampled_indexToX (me, i) - 0.5 * my dx, tright = tleft + my dx;
		if (tleft < tmin) tleft = tmin;
		if (tright > tmax) tright = tmax;
		Graphics_line (g, tleft, fleft + (tleft - tmin) * slope,
			tright, fleft + (tright - tmin) * slope);
		Graphics_setLineType (g, lineType);
		Graphics_setLineWidth (g, lineWidth);
	}
}

void PitchTier_Pitch_draw (PitchTier me, Pitch uv, Graphics g,
	double tmin, double tmax, double fmin, double fmax, int nonPeriodicLineType, int garnish)
{
	long n = my points -> size, imin, imax, i;
	if (nonPeriodicLineType == 0) {
		PitchTier_draw (me, g, tmin, tmax, fmin, fmax, garnish);
		return;
	}
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	Graphics_setInner (g);
	imin = AnyTier_timeToHighIndex (me, tmin);
	imax = AnyTier_timeToLowIndex (me, tmax);
	if (n == 0) {
	} else if (imax < imin) {
		double fleft = RealTier_getValueAtTime (me, tmin);
		double fright = RealTier_getValueAtTime (me, tmax);
		Pitch_line (uv, g, tmin, fleft, tmax, fright, nonPeriodicLineType);
	} else for (i = imin; i <= imax; i ++) {
		RealPoint point = my points -> item [i];
		double t = point -> time, f = point -> value;
		Graphics_fillCircle_mm (g, t, f, 1);
		if (i == 1)
			Pitch_line (uv, g, tmin, f, t, f, nonPeriodicLineType);
		else if (i == imin)
			Pitch_line (uv, g, t, f, tmin, RealTier_getValueAtTime (me, tmin), nonPeriodicLineType);
		if (i == n)
			Pitch_line (uv, g, t, f, tmax, f, nonPeriodicLineType);
		else if (i == imax)
			Pitch_line (uv, g, t, f, tmax, RealTier_getValueAtTime (me, tmax), nonPeriodicLineType);
		else {
			RealPoint pointRight = my points -> item [i + 1];
			Pitch_line (uv, g, t, f, pointRight -> time, pointRight -> value, nonPeriodicLineType);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Frequency (Hz)");
	}
}

Pitch Pitch_PitchTier_to_Pitch (Pitch me, PitchTier tier) {
	long iframe;
	Pitch thee = NULL;
	if (tier -> points -> size == 0) return Melder_errorp ("No pitch points.");
	thee = Data_copy (me);
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Pitch_Frame frame = & thy frame [iframe];
		Pitch_Candidate cand = & frame -> candidate [1];
		if (cand -> frequency > 0.0 && cand -> frequency <= my ceiling)
			cand -> frequency = RealTier_getValueAtTime (tier, Sampled_indexToX (me, iframe));
		cand -> strength = 0.9;
		frame -> nCandidates = 1;
	}
	return thee;
}

/* End of file Pitch_to_PitchTier.c */
