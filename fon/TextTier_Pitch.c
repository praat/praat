/* TextTier_Pitch.c
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
 * pb 1999/10/23
 * pb 2002/07/16 GPL
 */

#include "TextTier_Pitch.h"
#include "Pitch_to_PitchTier.h"

void TextTier_Pitch_draw (TextTier tier, Pitch pitch, Graphics g,
	double tmin, double tmax, double fmin, double fmax,
	double fontSize, int useTextStyles, int garnish, int speckle, int yscale)
{
	PitchTier pitchTier = NULL;
	long imark;
	double oldFontSize = Graphics_inqFontSize (g);
	Pitch_draw (pitch, g, tmin, tmax, fmin, fmax, garnish, speckle, yscale);
	if (tmax <= tmin) { tmin = pitch -> xmin; tmax = pitch -> xmax; }
	pitchTier = Pitch_to_PitchTier (pitch);
	if (! pitchTier) goto end;
	Pitch_convertYscale (& fmin, & fmax, yscale);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (g);
	Graphics_setFontSize (g, fontSize);
	Graphics_setPercentSignIsItalic (g, useTextStyles);
	Graphics_setNumberSignIsBold (g, useTextStyles);
	Graphics_setCircumflexIsSuperscript (g, useTextStyles);
	Graphics_setUnderscoreIsSubscript (g, useTextStyles);
	for (imark = 1; imark <= tier -> points -> size; imark ++) {
		TextPoint point = tier -> points -> item [imark];
		double t = point -> time, f0;
		if (t < tmin || t > tmax) continue;
		f0 = Pitch_convertFrequency (RealTier_getValueAtTime (pitchTier, t), yscale);
		if (f0 < fmin || f0 > fmax) continue;
		Graphics_text (g, t, f0, point -> mark);
	}
	Graphics_setPercentSignIsItalic (g, TRUE);
	Graphics_setNumberSignIsBold (g, TRUE);
	Graphics_setCircumflexIsSuperscript (g, TRUE);
	Graphics_setUnderscoreIsSubscript (g, TRUE);
	Graphics_setFontSize (g, oldFontSize);
	Graphics_unsetInner (g);
end:
	forget (pitchTier);
}

/* End of file TextTier_Pitch.c */
