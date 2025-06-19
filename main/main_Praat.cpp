/* main_Praat.cpp
 *
 * Copyright (C) 1992-2008,2010-2021,2023-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

#include "../sys/praat.h"
#include "main_Praat.h"

static void logo (Graphics graphics) {
	Graphics_setWindow (graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (graphics, 0.95);
	Graphics_fillRectangle (graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFont (graphics, kGraphics_font::TIMES);
	Graphics_setFontSize (graphics, 45.0);
	Graphics_setColour (graphics, Melder_MAROON);
	Graphics_text (graphics, 0.385, 0.87, U"P");
	Graphics_text (graphics, 0.448, 0.87, U"\\s{R}");
	Graphics_text (graphics, 0.510, 0.87, U"\\s{A}");
	Graphics_text (graphics, 0.575, 0.87, U"\\s{A}");
	Graphics_text (graphics, 0.628, 0.87, U"\\s{T}");
	Graphics_setFontSize (graphics, 18.0);
	Graphics_text (graphics, 0.5, 0.75, U"%%doing phonetics by computer");
	Graphics_setFontSize (graphics, 14.0);
	Graphics_text (graphics, 0.5, 0.60, Melder_cat (U"version ", Melder_appVersionSTR(),
			U" (", Melder_appMonthSTR(), U" ", Melder_appDay(), U", ", Melder_appYear(), U")"));
	Graphics_setFontSize (graphics, 12.0);
	[[maybe_unused]] constexpr bool isArm64 =
		#if defined (__aarch64__) || defined (_M_ARM64_)
			true;
		#else
			false;
		#endif
	const conststring32 builtFor =
		#if defined (macintosh)
			isArm64 ? U"ARM64 macOS" : U"Intel64 macOS";
		#elif defined (_WIN32)
			isArm64 ? U"ARM64 Windows" : sizeof (void *) == 4 ? U"Intel32 Windows" : U"Intel64 Windows";
		#elif defined (__s390x__)
			U"s390x Linux";
		#elif defined (raspberrypi)
			U"ARMV7 Raspberry Pi";
		#elif defined (chrome)
			isArm64 ? U"ARM64 Chromebook" : U"Intel64 Chromebook";
		#elif defined (linux)
			isArm64 ? U"ARM64 Linux" : U"Intel64 Linux";
		#else
			U"";
			#error Unknown OS type.
		#endif
	Graphics_text (graphics, 0.5, 0.52, Melder_cat (U"built for ", builtFor));
	Graphics_setColour (graphics, Melder_BLACK);
	Graphics_setFont (graphics, kGraphics_font::HELVETICA);
	Graphics_setFontSize (graphics, 10.0);
	Graphics_text (graphics, 0.5, 0.35, Melder_cat (U"Copyright © 1992–", Melder_appYear(), U" by Paul Boersma and David Weenink"));
	Graphics_setFontSize (graphics, 10.0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 0.34, 0.26, U"Download:");
	Graphics_text (graphics, 0.34, 0.06, U"Source code:");
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_setFont (graphics, kGraphics_font::COURIER);
	Graphics_text (graphics, 0.36, 0.26, U"https://www.fon.hum.uva.nl/praat");
	Graphics_text (graphics, 0.36, 0.20, U"https://www.praat.org");
	Graphics_text (graphics, 0.36, 0.14, U"https://praat.github.io/praat");
	Graphics_text (graphics, 0.36, 0.06, U"https://github.com/praat/praat");
}

int main (int argc, char *argv []) {
	try {
		//TRACE
		praat_setLogo (130.0, 100.0, logo);
		Melder_stopwatch ();
		praat_init (U"" stringize (PRAAT_NAME),
			U"" stringize (PRAAT_VERSION_STR), PRAAT_VERSION_NUM,
			PRAAT_YEAR, PRAAT_MONTH, PRAAT_DAY,
			U"paul.boersma", U"uva.nl",
			argc, argv
		);
		trace (Melder_stopwatch ());
		INCLUDE_LIBRARY (praat_uvafon_init)
		trace (Melder_stopwatch ());
		praat_run ();
		trace (Melder_stopwatch ());
	} catch (MelderError) {
		Melder_flushError (U"This error message percolated all the way to the top.");   // an attempt to catch Apache errors
	}
	return 0;
}

/* End of file main_Praat.cpp */
