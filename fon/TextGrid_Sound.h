/* TextGrid_Sound.h
 *
 * Copyright (C) 1992-2011,2013 Paul Boersma
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

#include "TextGrid.h"
#include "Sound.h"
#include "Pitch.h"

void TextGrid_Sound_draw (TextGrid me, Sound sound, Graphics g, double tmin, double tmax,
	int showBoundaries, int useTextStyles, int garnish);
Collection TextGrid_Sound_extractAllIntervals (TextGrid me, Sound sound, long itier, int preserveTimes);
Collection TextGrid_Sound_extractNonemptyIntervals (TextGrid me, Sound sound, long itier, int preserveTimes);
Collection TextGrid_Sound_extractIntervalsWhere (TextGrid me, Sound sound,
	long itier, int which_Melder_STRING, const wchar_t *text, int preserveTimes);

void TextGrid_Pitch_draw (TextGrid grid, Pitch pitch, Graphics g,
	long itier, double tmin, double tmax, double fmin, double fmax,
	double fontSize, int useTextStyles, int horizontalAlignment, int garnish, int speckle, int yscale);
void TextGrid_Pitch_drawSeparately (TextGrid grid, Pitch pitch, Graphics g, double tmin, double tmax,
	double fmin, double fmax, int showBoundaries, int useTextStyles, int garnish, int speckle, int yscale);

void TextGrid_anySound_alignInterval (TextGrid me, Function anySound, long tierNumber, long intervalNumber,
	const wchar_t *languageName, bool includeWords, bool includePhonemes);

/* End of file TextGrid_Sound.h */
