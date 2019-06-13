/* praat_statistics.cpp
 *
 * Copyright (C) 1992-2012,2014-2018 Paul Boersma
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

#include <time.h>
#include <locale.h>
#include <thread>
#include "praatP.h"

static struct {
	integer batchSessions, interactiveSessions;
	double memory;
	char32 dateOfFirstSession [Preferences_STRING_BUFFER_SIZE];
} statistics;

void praat_statistics_prefs () {
	Preferences_addInteger (U"PraatShell.batchSessions", & statistics.batchSessions, 0);
	Preferences_addInteger (U"PraatShell.interactiveSessions", & statistics.interactiveSessions, 0);
	Preferences_addDouble (U"PraatShell.memory", & statistics.memory, 0.0);
	Preferences_addString (U"PraatShell.dateOfFirstSession", & statistics.dateOfFirstSession [0], U"");
}

void praat_statistics_prefsChanged () {
	if (! statistics.dateOfFirstSession [0]) {
		time_t today = time (nullptr);
		char32 *newLine;
		str32cpy (statistics.dateOfFirstSession, Melder_peek8to32 (ctime (& today)));
		newLine = str32chr (statistics.dateOfFirstSession, U'\n');
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

/*@praat
	report$ = Report integer properties
	sizeOfInteger = extractNumber (report$, "An indexing integer is ")
	sizeOfPointer = extractNumber (report$, "A pointer is ")
	assert sizeOfInteger = sizeOfPointer
	sizeOfFileOffset = extractNumber (report$, "A file offset is ")
	assert sizeOfFileOffset = 64
@*/
void praat_reportIntegerProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Integer properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (U"A boolean is ",                sizeof (bool)        * 8, U" bits.");
	MelderInfo_writeLine (U"A \"short integer\" is ",      sizeof (short)       * 8, U" bits.");
	MelderInfo_writeLine (U"An \"integer\" is ",           sizeof (int)         * 8, U" bits.");
	MelderInfo_writeLine (U"A \"long integer\" is ",       sizeof (long)        * 8, U" bits.");
	MelderInfo_writeLine (U"A \"long long integer\" is ",  sizeof (long long)   * 8, U" bits.");
	MelderInfo_writeLine (U"An indexing integer is ",      sizeof (integer)     * 8, U" bits.");
	MelderInfo_writeLine (U"A pointer is ",                sizeof (void *)      * 8, U" bits.");
	MelderInfo_writeLine (U"A memory object size is ",     sizeof (size_t)      * 8, U" bits.");
	MelderInfo_writeLine (U"A file offset is ",            sizeof (off_t)       * 8, U" bits.");
	MelderInfo_close ();
}

void praat_reportTextProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Text properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (U"Locale: ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
	MelderInfo_writeLine (U"A \"char\" is ",                                      8, U" bits.");
	MelderInfo_writeLine (U"A \"char16_t\" is ",           sizeof (char16_t)    * 8, U" bits.");
	MelderInfo_writeLine (U"A \"wchar_t\" is ",            sizeof (wchar_t)     * 8, U" bits.");
	MelderInfo_writeLine (U"A \"char32_t\" is ",           sizeof (char32_t)    * 8, U" bits.");
	MelderInfo_close ();
}

void praat_reportSystemProperties () {
	#define xstr(s) str(s)
	#define str(s) #s
	MelderInfo_open ();
	MelderInfo_writeLine (U"System properties of this edition of Praat on this computer:\n");
	#ifdef _WIN32
		MelderInfo_writeLine (U"_WIN32 is \"" xstr (_WIN32) "\".");
	#endif
	#ifdef WINVER
		MelderInfo_writeLine (U"WINVER is \"" xstr (WINVER) "\".");
	#endif
	#ifdef _WIN32_WINNT
		MelderInfo_writeLine (U"_WIN32_WINNT is \"" xstr (_WIN32_WINNT) "\".");
	#endif
	#ifdef _WIN32_IE
		MelderInfo_writeLine (U"_WIN32_IE is \"" xstr (_WIN32_IE) "\".");
	#endif
	#ifdef UNICODE
		MelderInfo_writeLine (U"UNICODE is \"" xstr (UNICODE) "\".");
	#endif
	#ifdef _FILE_OFFSET_BITS
		MelderInfo_writeLine (U"_FILE_OFFSET_BITS is \"" xstr (_FILE_OFFSET_BITS) "\".");
	#endif
	#ifdef macintosh
		MelderInfo_writeLine (U"macintosh is \"" xstr (macintosh) "\".");
	#endif
	#ifdef linux
		MelderInfo_writeLine (U"linux is \"" xstr (linux) "\".");
	#endif
	MelderInfo_writeLine (U"The number of processors is ", std::thread::hardware_concurrency(), U".");
	#ifdef macintosh
		MelderInfo_writeLine (U"system version is ", Melder_systemVersion, U".");
	#endif
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
	MelderInfo_writeLine (U"   Things: ", theTotalNumberOfThings,
		U" (objects in list: ", theCurrentPraatObjects -> n, U")");
	integer numberOfMotifWidgets =
	#if motif
		Gui_getNumberOfMotifWidgets ();
		MelderInfo_writeLine (U"   Motif widgets: ", numberOfMotifWidgets);
	#else
		0;
	#endif
	MelderInfo_writeLine (U"   Other: ",
		Melder_allocationCount () - Melder_deallocationCount ()
		- theTotalNumberOfThings - NUM_getTotalNumberOfArrays ()
		- (MelderString_allocationCount () - MelderString_deallocationCount ())
		- numberOfMotifWidgets);
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

void MelderCasual_memoryUse (integer message) {
	integer numberOfStrings = MelderString_allocationCount () - MelderString_deallocationCount ();
	integer numberOfArrays = NUM_getTotalNumberOfArrays ();
	integer numberOfThings = theTotalNumberOfThings;
	integer numberOfOther = Melder_allocationCount () - Melder_deallocationCount () - numberOfStrings - numberOfArrays - numberOfThings;
	Melder_casual (U"Memory ", message, U": ",
		numberOfStrings, U" strings, ", numberOfArrays, U" arrays, ", numberOfThings, U" things, ", numberOfOther, U" other.");
}

/* End of file praat_statistics.cpp */
