/* praat_statistics.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
	wchar_t dateOfFirstSession [Preferences_STRING_BUFFER_SIZE];
} statistics;

void praat_statistics_prefs () {
	Preferences_addLong (L"PraatShell.batchSessions", & statistics.batchSessions, 0);
	Preferences_addLong (L"PraatShell.interactiveSessions", & statistics.interactiveSessions, 0);
	Preferences_addDouble (L"PraatShell.memory", & statistics.memory, 0.0);
	Preferences_addString (L"PraatShell.dateOfFirstSession", & statistics.dateOfFirstSession [0], L"");
}

void praat_statistics_prefsChanged () {
	if (! statistics.dateOfFirstSession [0]) {
		time_t today = time (NULL);
		wchar_t *newLine;
		wcscpy (statistics.dateOfFirstSession, Melder_peekUtf8ToWcs (ctime (& today)));
		newLine = wcschr (statistics.dateOfFirstSession, '\n');
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
	MelderInfo_writeLine (L"Integer properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (L"A \"short integer\" is ",      Melder_integer (sizeof (short)       * 8), L" bits.");
	MelderInfo_writeLine (L"An \"integer\" is ",           Melder_integer (sizeof (int)         * 8), L" bits.");
	MelderInfo_writeLine (L"A \"long integer\" is ",       Melder_integer (sizeof (long)        * 8), L" bits.");
	MelderInfo_writeLine (L"A \"long long integer\" is ",  Melder_integer (sizeof (long long)   * 8), L" bits.");
	MelderInfo_writeLine (L"A pointer is ",                Melder_integer (sizeof (void *)      * 8), L" bits.");
	MelderInfo_close ();
}

void praat_reportTextProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (L"Text properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (L"Locale: ", Melder_peekUtf8ToWcs (setlocale (LC_ALL, NULL)));
	MelderInfo_close ();
}

void praat_reportGraphicalProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (L"Graphical properties of this edition of Praat on this computer:\n");
	double x, y, width, height;
	Gui_getWindowPositioningBounds (& x, & y, & width, & height);
	MelderInfo_writeLine (L"Window positioning area: x = ", Melder_double (x), L", y = ", Melder_double (y),
		L", width = ", Melder_double (width), L", height = ", Melder_double (height));
	#if defined (macintosh)
		CGDirectDisplayID screen = CGMainDisplayID ();
		CGSize screenSize_mm = CGDisplayScreenSize (screen);
		double diagonal_mm = sqrt (screenSize_mm. width * screenSize_mm. width + screenSize_mm. height * screenSize_mm. height);
		double diagonal_inch = diagonal_mm / 25.4;
		MelderInfo_writeLine (L"\nScreen size: ", Melder_double (screenSize_mm. width), L" x ", Melder_double (screenSize_mm. height),
			L" mm (diagonal ", Melder_fixed (diagonal_mm, 1), L" mm = ", Melder_fixed (diagonal_inch, 1), L" inch)");
		size_t screenWidth_pixels = CGDisplayPixelsWide (screen);
		size_t screenHeight_pixels = CGDisplayPixelsHigh (screen);
		MelderInfo_writeLine (L"Screen \"resolution\": ", Melder_integer (screenWidth_pixels), L" x ", Melder_integer (screenHeight_pixels), L" pixels");
		double resolution = 25.4 * screenWidth_pixels / screenSize_mm. width;
		MelderInfo_writeLine (L"Screen resolution: ", Melder_fixed (resolution, 1), L" pixels/inch");
	#elif defined (_WIN32)
		/*for (int i = 0; i <= 88; i ++)
			MelderInfo_writeLine (L"System metric ", Melder_integer (i), L": ", Melder_integer (GetSystemMetrics (i)));*/
	#endif
	MelderInfo_close ();
}

void praat_reportMemoryUse () {
	MelderInfo_open ();
	MelderInfo_writeLine (L"Memory use by Praat:\n");
	MelderInfo_writeLine (L"Currently in use:\n"
		L"   Strings: ", Melder_integer (MelderString_allocationCount () - MelderString_deallocationCount ()));
	MelderInfo_writeLine (L"   Arrays: ", Melder_integer (NUM_getTotalNumberOfArrays ()));
	MelderInfo_writeLine (L"   Things: ", Melder_integer (Thing_getTotalNumberOfThings ()),
		L" (objects in list: ", Melder_integer (theCurrentPraatObjects -> n), L")");
	long numberOfMotifWidgets =
		#if motif && (defined (_WIN32) || defined (macintosh))
			Gui_getNumberOfMotifWidgets ();
		#else
			0;
		#endif
	if (numberOfMotifWidgets > 0) {
		MelderInfo_writeLine (L"   Motif widgets: ", Melder_integer (numberOfMotifWidgets));
	}
	MelderInfo_writeLine (L"   Other: ",
		Melder_bigInteger (Melder_allocationCount () - Melder_deallocationCount ()
			- Thing_getTotalNumberOfThings () - NUM_getTotalNumberOfArrays ()
			- (MelderString_allocationCount () - MelderString_deallocationCount ())
			- numberOfMotifWidgets));
	MelderInfo_writeLine (
		L"\nMemory history of this session:\n"
		L"   Total created: ", Melder_bigInteger (Melder_allocationCount ()), L" (", Melder_bigInteger (Melder_allocationSize ()), L" bytes)");
	MelderInfo_writeLine (L"   Total deleted: ", Melder_bigInteger (Melder_deallocationCount ()));
	MelderInfo_writeLine (L"   Reallocations: ", Melder_bigInteger (Melder_movingReallocationsCount ()), L" moving, ",
		Melder_bigInteger (Melder_reallocationsInSituCount ()), L" in situ");
	MelderInfo_writeLine (
		L"   Strings created: ", Melder_bigInteger (MelderString_allocationCount ()), L" (", Melder_bigInteger (MelderString_allocationSize ()), L" bytes)");
	MelderInfo_writeLine (
		L"   Strings deleted: ", Melder_bigInteger (MelderString_deallocationCount ()), L" (", Melder_bigInteger (MelderString_deallocationSize ()), L" bytes)");
	MelderInfo_writeLine (L"\nHistory of all sessions from ", statistics.dateOfFirstSession, L" until today:");
	MelderInfo_writeLine (L"   Sessions: ", Melder_integer (statistics.interactiveSessions), L" interactive, ",
		Melder_integer (statistics.batchSessions), L" batch");
	MelderInfo_writeLine (L"   Total memory use: ", Melder_bigInteger (statistics.memory + Melder_allocationSize ()), L" bytes");
	MelderInfo_writeLine (L"\nNumber of fixed menu commands: ", Melder_integer (praat_getNumberOfMenuCommands ()));
	MelderInfo_writeLine (L"Number of dynamic menu commands: ", Melder_integer (praat_getNumberOfActions ()));
	MelderInfo_close ();
}

/* End of file praat_statistics.cpp */
