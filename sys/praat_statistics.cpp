/* praat_statistics.cpp
 *
 * Copyright (C) 1992-2012,2014,2015 Paul Boersma
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

#include <time.h>
#include <locale.h>
#include "praatP.h"

static struct {
	long batchSessions, interactiveSessions;
	double memory;
	char32 dateOfFirstSession [Preferences_STRING_BUFFER_SIZE];
} statistics;

void praat_statistics_prefs () {
	Preferences_addLong (U"PraatShell.batchSessions", & statistics.batchSessions, 0);
	Preferences_addLong (U"PraatShell.interactiveSessions", & statistics.interactiveSessions, 0);
	Preferences_addDouble (U"PraatShell.memory", & statistics.memory, 0.0);
	Preferences_addString (U"PraatShell.dateOfFirstSession", & statistics.dateOfFirstSession [0], U"");
}

void praat_statistics_prefsChanged () {
	if (! statistics.dateOfFirstSession [0]) {
		time_t today = time (NULL);
		char32 *newLine;
		str32cpy (statistics.dateOfFirstSession, Melder_peek8to32 (ctime (& today)));
		newLine = str32chr (statistics.dateOfFirstSession, '\n');
		if (newLine) *newLine = '\0';
	}
	if (theCurrentPraatApplication -> batch)
		statistics.batchSessions += 1;
	else
		statistics.interactiveSessions += 1;
}

void praat_statistics_exit () {
	statistics.memory += Melder_allocationSize ();
}

void praat_reportIntegerProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Integer properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (U"A \"short integer\" is ",      sizeof (short)       * 8, U" bits.");
	MelderInfo_writeLine (U"An \"integer\" is ",           sizeof (int)         * 8, U" bits.");
	MelderInfo_writeLine (U"A \"long integer\" is ",       sizeof (long)        * 8, U" bits.");
	MelderInfo_writeLine (U"A \"long long integer\" is ",  sizeof (long long)   * 8, U" bits.");
	MelderInfo_writeLine (U"A pointer is ",                sizeof (void *)      * 8, U" bits.");
	MelderInfo_writeLine (U"A memory object size is ",     sizeof (size_t)      * 8, U" bits.");
	MelderInfo_writeLine (U"A file offset is ",            sizeof (off_t)       * 8, U" bits.");
	MelderInfo_close ();
}

void praat_reportTextProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Text properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (U"Locale: ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
	MelderInfo_close ();
}

void praat_reportGraphicalProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Graphical properties of this edition of Praat on this computer:\n");
	double x, y, width, height;
	Gui_getWindowPositioningBounds (& x, & y, & width, & height);
	MelderInfo_writeLine (U"Window positioning area: x = ", x, U", y = ", y,
		U", width = ", width, U", height = ", height);
	#if defined (macintosh)
		CGDirectDisplayID screen = CGMainDisplayID ();
		CGSize screenSize_mm = CGDisplayScreenSize (screen);
		double diagonal_mm = sqrt (screenSize_mm. width * screenSize_mm. width + screenSize_mm. height * screenSize_mm. height);
		double diagonal_inch = diagonal_mm / 25.4;
		MelderInfo_writeLine (U"\nScreen size: ", screenSize_mm. width, U" x ", screenSize_mm. height,
			U" mm (diagonal ", Melder_fixed (diagonal_mm, 1), U" mm = ", Melder_fixed (diagonal_inch, 1), U" inch)");
		size_t screenWidth_pixels = CGDisplayPixelsWide (screen);
		size_t screenHeight_pixels = CGDisplayPixelsHigh (screen);
		MelderInfo_writeLine (U"Screen \"resolution\": ", screenWidth_pixels, U" x ", screenHeight_pixels, U" pixels");
		double resolution = 25.4 * screenWidth_pixels / screenSize_mm. width;
		MelderInfo_writeLine (U"Screen resolution: ", Melder_fixed (resolution, 1), U" pixels/inch");
	#elif defined (_WIN32)
		/*for (int i = 0; i <= 88; i ++)
			MelderInfo_writeLine (U"System metric ", i, U": ", GetSystemMetrics (i));*/
	#endif
	MelderInfo_close ();
}

void praat_reportMemoryUse () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Memory use by Praat:\n");
	MelderInfo_writeLine (U"Currently in use:\n"
		U"   Strings: ", MelderString_allocationCount () - MelderString_deallocationCount ());
	MelderInfo_writeLine (U"   Arrays: ", NUM_getTotalNumberOfArrays ());
	MelderInfo_writeLine (U"   Things: ", Thing_getTotalNumberOfThings (),
		U" (objects in list: ", theCurrentPraatObjects -> n, U")");
	long numberOfMotifWidgets =
		#if motif && (defined (_WIN32) || defined (macintosh))
			Gui_getNumberOfMotifWidgets ();
		#else
			0;
		#endif
	if (numberOfMotifWidgets > 0) {
		MelderInfo_writeLine (U"   Motif widgets: ", numberOfMotifWidgets);
	}
	MelderInfo_writeLine (U"   Other: ",
		Melder_bigInteger (Melder_allocationCount () - Melder_deallocationCount ()
			- Thing_getTotalNumberOfThings () - NUM_getTotalNumberOfArrays ()
			- (MelderString_allocationCount () - MelderString_deallocationCount ())
			- numberOfMotifWidgets));
	MelderInfo_writeLine (
		U"\nMemory history of this session:\n"
		U"   Total created: ", Melder_bigInteger (Melder_allocationCount ()), U" (", Melder_bigInteger (Melder_allocationSize ()), U" bytes)");
	MelderInfo_writeLine (U"   Total deleted: ", Melder_bigInteger (Melder_deallocationCount ()));
	MelderInfo_writeLine (U"   Reallocations: ", Melder_bigInteger (Melder_movingReallocationsCount ()), U" moving, ",
		Melder_bigInteger (Melder_reallocationsInSituCount ()), U" in situ");
	MelderInfo_writeLine (
		U"   Strings created: ", Melder_bigInteger (MelderString_allocationCount ()), U" (", Melder_bigInteger (MelderString_allocationSize ()), U" bytes)");
	MelderInfo_writeLine (
		U"   Strings deleted: ", Melder_bigInteger (MelderString_deallocationCount ()), U" (", Melder_bigInteger (MelderString_deallocationSize ()), U" bytes)");
	MelderInfo_writeLine (U"\nHistory of all sessions from ", statistics.dateOfFirstSession, U" until today:");
	MelderInfo_writeLine (U"   Sessions: ", statistics.interactiveSessions, U" interactive, ",
		statistics.batchSessions, U" batch");
	MelderInfo_writeLine (U"   Total memory use: ", Melder_bigInteger ((int64) statistics.memory + Melder_allocationSize ()), U" bytes");
	MelderInfo_writeLine (U"\nNumber of fixed menu commands: ", praat_getNumberOfMenuCommands ());
	MelderInfo_writeLine (U"Number of dynamic menu commands: ", praat_getNumberOfActions ());
	MelderInfo_close ();
}

/* End of file praat_statistics.cpp */
