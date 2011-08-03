/* praat_statistics.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2002/03/09 OSX no mention of free memory
 * pb 2004/10/18 more big integers
 * pb 2005/03/02 pref string 260 bytes long
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/06 MelderString
 * pb 2006/12/26 theCurrentPraat
 * pb 2007/07/27 string deallocation size
 * pb 2007/08/12 wchar
 * pb 2007/12/09 preferences
 * pb 2008/11/26 numberOfMotifWidgets
 */

#include <time.h>
#include "praatP.h"

static struct {
	long batchSessions, interactiveSessions;
	double memory;
	wchar dateOfFirstSession [Preferences_STRING_BUFFER_SIZE];
} statistics;

void praat_statistics_prefs (void) {
	Preferences_addLong (L"PraatShell.batchSessions", & statistics.batchSessions, 0);
	Preferences_addLong (L"PraatShell.interactiveSessions", & statistics.interactiveSessions, 0);
	Preferences_addDouble (L"PraatShell.memory", & statistics.memory, 0.0);
	Preferences_addString (L"PraatShell.dateOfFirstSession", & statistics.dateOfFirstSession [0], L"");
}

void praat_statistics_prefsChanged (void) {
	if (! statistics.dateOfFirstSession [0]) {
		time_t today = time (NULL);
		wchar *newLine;
		wcscpy (statistics.dateOfFirstSession, Melder_peekUtf8ToWcs (ctime (& today)));
		newLine = wcschr (statistics.dateOfFirstSession, '\n');
		if (newLine) *newLine = '\0';
	}
	if (theCurrentPraatApplication -> batch)
		statistics.batchSessions += 1;
	else
		statistics.interactiveSessions += 1;
}

void praat_statistics_exit (void) {
	statistics.memory += Melder_allocationSize ();
}

void praat_memoryInfo (void) {
	MelderInfo_open ();
	MelderInfo_writeLine2 (L"Currently in use:\n"
		L"   Strings: ", Melder_integer (MelderString_allocationCount () - MelderString_deallocationCount ()));
	MelderInfo_writeLine2 (L"   Arrays: ", Melder_integer (NUM_getTotalNumberOfArrays ()));
	MelderInfo_writeLine5 (L"   Things: ", Melder_integer (Thing_getTotalNumberOfThings ()),
		L" (objects in list: ", Melder_integer (theCurrentPraatObjects -> n), L")");
	long numberOfMotifWidgets =
		#if motif && (defined (_WIN32) || defined (macintosh))
			Gui_getNumberOfMotifWidgets ();
		#else
			0;
		#endif
	if (numberOfMotifWidgets > 0) {
		MelderInfo_writeLine2 (L"   Motif widgets: ", Melder_integer (numberOfMotifWidgets));
	}
	MelderInfo_writeLine2 (L"   Other: ",
		Melder_bigInteger (Melder_allocationCount () - Melder_deallocationCount ()
			- Thing_getTotalNumberOfThings () - NUM_getTotalNumberOfArrays ()
			- (MelderString_allocationCount () - MelderString_deallocationCount ())
			- numberOfMotifWidgets));
	MelderInfo_writeLine5 (
		L"\nMemory history of this session:\n"
		L"   Total created: ", Melder_bigInteger (Melder_allocationCount ()), L" (", Melder_bigInteger (Melder_allocationSize ()), L" bytes)");
	MelderInfo_writeLine2 (L"   Total deleted: ", Melder_bigInteger (Melder_deallocationCount ()));
	MelderInfo_writeLine5 (L"   Reallocations: ", Melder_bigInteger (Melder_movingReallocationsCount ()), L" moving, ",
		Melder_bigInteger (Melder_reallocationsInSituCount ()), L" in situ");
	MelderInfo_writeLine5 (
		L"   Strings created: ", Melder_bigInteger (MelderString_allocationCount ()), L" (", Melder_bigInteger (MelderString_allocationSize ()), L" bytes)");
	MelderInfo_writeLine5 (
		L"   Strings deleted: ", Melder_bigInteger (MelderString_deallocationCount ()), L" (", Melder_bigInteger (MelderString_deallocationSize ()), L" bytes)");
	MelderInfo_writeLine3 (L"\nHistory of all sessions from ", statistics.dateOfFirstSession, L" until today:");
	MelderInfo_writeLine5 (L"   Sessions: ", Melder_integer (statistics.interactiveSessions), L" interactive, ",
		Melder_integer (statistics.batchSessions), L" batch");
	MelderInfo_writeLine3 (L"   Total memory use: ", Melder_bigInteger (statistics.memory + Melder_allocationSize ()), L" bytes");
	MelderInfo_writeLine9 (L"\nMemory addressing: short ", Melder_integer (sizeof (short) * 8), L" bits, int ", Melder_integer (sizeof (int) * 8), L" bits, long ",
		Melder_integer (sizeof (long) * 8), L" bits, pointer ", Melder_integer (sizeof (void *) * 8), L" bits");
	MelderInfo_writeLine2 (L"\nNumber of actions: ", Melder_integer (praat_getNumberOfActions ()));
	#ifdef macintosh
		CGDirectDisplayID screen = CGMainDisplayID ();
		CGSize screenSize_mm = CGDisplayScreenSize (screen);
		double diagonal_mm = sqrt (screenSize_mm. width * screenSize_mm. width + screenSize_mm. height * screenSize_mm. height);
		double diagonal_inch = diagonal_mm / 25.4;
		MelderInfo_writeLine9 (L"\nScreen size: ", Melder_double (screenSize_mm. width), L" x ", Melder_double (screenSize_mm. height),
			L" mm (diagonal ", Melder_fixed (diagonal_mm, 1), L" mm = ", Melder_fixed (diagonal_inch, 1), L" inch)");
		size_t screenWidth_pixels = CGDisplayPixelsWide (screen);
		size_t screenHeight_pixels = CGDisplayPixelsHigh (screen);
		MelderInfo_writeLine5 (L"Screen \"resolution\": ", Melder_integer (screenWidth_pixels), L" x ", Melder_integer (screenHeight_pixels), L" pixels");
		double resolution = 25.4 * screenWidth_pixels / screenSize_mm. width;
		MelderInfo_writeLine3 (L"Screen resolution: ", Melder_fixed (resolution, 1), L" pixels/inch");
	#endif
	MelderInfo_close ();
}

/* End of file praat_statistics.c */
