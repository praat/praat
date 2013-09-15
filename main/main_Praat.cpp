/* main_Praat.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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

#include "praat.h"
#include "praat_version.h"

static void logo (Graphics g) {
	Graphics_setWindow (g, 0, 1, 0.00, 0.80);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFont (g, kGraphics_font_TIMES);
	Graphics_setFontSize (g, 45);
	Graphics_setColour (g, Graphics_MAROON);
	Graphics_text (g, 0.385, 0.66, L"P");
	Graphics_text (g, 0.448, 0.66, L"\\s{R}");
	Graphics_text (g, 0.510, 0.66, L"\\s{A}");
	Graphics_text (g, 0.575, 0.66, L"\\s{A}");
	Graphics_text (g, 0.628, 0.66, L"\\s{T}");
	Graphics_setFontSize (g, 15);
	Graphics_text (g, 0.5, 0.55, L"%%doing phonetics by computer");
	#define xstr(s) str(s)
	#define str(s) #s
	Graphics_text (g, 0.5, 0.45, L"version " xstr(PRAAT_VERSION_STR));
	Graphics_setColour (g, Graphics_BLACK);
	Graphics_setFontSize (g, 14);
	Graphics_text (g, 0.5, 0.33, L"www.praat.org");
	Graphics_setFont (g, kGraphics_font_HELVETICA);
	Graphics_setFontSize (g, 10);
	Graphics_text (g, 0.5, 0.16, L"Copyright \\co 1992-" xstr(PRAAT_YEAR) " by Paul Boersma and David Weenink");
}

int main (int argc, char *argv []) {
	try {
		praat_setLogo (130, 80, logo);
		praat_init ("Praat", argc, argv);
		INCLUDE_LIBRARY (praat_uvafon_init)
		INCLUDE_LIBRARY (praat_contrib_Ola_KNN_init)
		praat_run ();
	} catch (MelderError) {
		Melder_flushError ("This error message percolated all the way to the top.");   // an attempt to catch Apache errors
	}
	return 0;
}

/* End of file main_Praat.cpp */
