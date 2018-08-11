/* melder_play.cpp
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

#include "melder.h"
#ifdef _WIN32
	#include <windows.h>
#endif

#ifdef macintosh
	#include "macport_on.h"
	#include <AudioToolbox/AudioToolbox.h>
	#include "macport_off.h"
#endif

static int defaultRecord (double duration) {
	(void) duration;
	return 0;   // nothing recorded
}

static int defaultRecordFromFile (MelderFile file) {
	(void) file;
	return 0;   // nothing recorded
}

static void defaultPlay () {}

static void defaultPlayReverse () {}

static int defaultPublishPlayed () {
	return 0;   // nothing published
}

void Melder_beep () {
	#ifdef macintosh
		AudioServicesPlayAlertSound (kSystemSoundID_UserPreferredAlert);
	#else
		fprintf (stderr, "\a");
	#endif
}

/********** Current message methods: initialize to default (batch) behaviour. **********/

static struct {
	int (*record) (double duration);
	int (*recordFromFile) (MelderFile file);
	void (*play) ();
	void (*playReverse) ();
	int (*publishPlayed) ();
}
	theMelderFunctions = {
		defaultRecord, defaultRecordFromFile, defaultPlay, defaultPlayReverse, defaultPublishPlayed
	};

int Melder_record (double duration) {
	return theMelderFunctions. record (duration);
}

int Melder_recordFromFile (MelderFile file) {
	return theMelderFunctions. recordFromFile (file);
}

void Melder_play () {
	theMelderFunctions. play ();
}

void Melder_playReverse () {
	theMelderFunctions. playReverse ();
}

int Melder_publishPlayed () {
	return theMelderFunctions. publishPlayed ();
}

/********** Procedures to override message methods (e.g., to enforce interactive behaviour). **********/

void Melder_setRecordProc (int (*record) (double))
	{ theMelderFunctions. record = record ? record : defaultRecord; }

void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile))
	{ theMelderFunctions. recordFromFile = recordFromFile ? recordFromFile : defaultRecordFromFile; }

void Melder_setPlayProc (void (*play) ())
	{ theMelderFunctions. play = play ? play : defaultPlay; }

void Melder_setPlayReverseProc (void (*playReverse) ())
	{ theMelderFunctions. playReverse = playReverse ? playReverse : defaultPlayReverse; }

void Melder_setPublishPlayedProc (int (*publishPlayed) ())
	{ theMelderFunctions. publishPlayed = publishPlayed ? publishPlayed : defaultPublishPlayed; }

/* End of file melder_play.cpp */
