/* Pitch_Intensity.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2005/03/08 connect
 * pb 2005/06/17 units
 */

#include "Pitch_Intensity.h"

static void Pitch_getExtrema (Pitch me, double *minimum, double *maximum) {
	long i;
	*minimum = 1e30, *maximum = -1e30;
	for (i = 1; i <= my nx; i ++) {
		double frequency = my frame [i]. candidate [1]. frequency;
		if (frequency == 0.0) continue;   /* Voiceless. */
		if (frequency < *minimum) *minimum = frequency;
		if (frequency > *maximum) *maximum = frequency;
	}
	if (*maximum == -1e30) *maximum = 0.0;
	if (*minimum == 1e30) *minimum = 0.0;
}

void Pitch_Intensity_draw (Pitch pitch, Intensity intensity, Graphics g,
	double f1, double f2, double s1, double s2, int garnish, int connect)
{
	long i, previousI = 0;
	double previousX = NUMundefined, previousY = NUMundefined;
	if (f2 <= f1) Pitch_getExtrema (pitch, & f1, & f2);
	if (f1 == 0.0) return;   /* All voiceless. */
	if (f1 == f2) { f1 -= 1.0; f2 += 1.0; }
	if (s2 <= s1) Matrix_getWindowExtrema (intensity, 0, 0, 1, 1, & s1, & s2);
	if (s1 == s2) { s1 -= 1.0; s2 += 1.0; }
	Graphics_setWindow (g, f1, f2, s1, s2);
	Graphics_setInner (g);
	for (i = 1; i <= pitch -> nx; i ++) {
		double t = Sampled_indexToX (pitch, i);
		double x = pitch -> frame [i]. candidate [1]. frequency;
		double y = Sampled_getValueAtX (intensity, t, Pitch_LEVEL_FREQUENCY, Pitch_UNIT_HERTZ, TRUE);
		if (x == 0) {
			continue;   /* Voiceless. */
		}
		if (connect & 1) Graphics_fillCircle_mm (g, x, y, 1.0);
		if ((connect & 2) && NUMdefined (previousX)) {
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
		Graphics_textBottom (g, 1, "Fundamental frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, "Intensity (dB)");
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

/* End of file Pitch_Intensity.c */
