/* melder.cpp
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

#include "../sys/MelderThread.h"

#include "enums_getText.h"
#include "melder_enums.h"
#include "enums_getValue.h"
#include "melder_enums.h"

/********** Exported variables. **********/

bool Melder_batch;   // don't we have a GUI?- Set once at application start-up
bool Melder_backgrounding;   // are we running a script?- Set and unset dynamically
int32 Melder_systemVersion;

static void defaultHelp (conststring32 query) {
	Melder_flushError (U"Don't know how to find help on \"", query, U"\".");
}

static void defaultSearch () {
	Melder_flushError (U"Do not know how to search.");
}

static void defaultFatal (conststring32 message) {
	MelderConsole::write (U"Fatal error: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}

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
	return 0;   /* Nothing published. */
}

/********** Current message methods: initialize to default (batch) behaviour. **********/

static struct {
	void (*help) (conststring32 query);
	void (*search) ();
	void (*fatal) (conststring32 message);
	int (*record) (double duration);
	int (*recordFromFile) (MelderFile file);
	void (*play) ();
	void (*playReverse) ();
	int (*publishPlayed) ();
}
	theMelderFunctions = {
		defaultHelp, defaultSearch,
		defaultFatal,
		defaultRecord, defaultRecordFromFile, defaultPlay, defaultPlayReverse, defaultPublishPlayed
	};

/********** PROGRESS **********/

static void defaultProgress (double /*progress*/, conststring32 /*message*/) {
}

static void * defaultMonitor (double /*progress*/, conststring32 /*message*/) {
	return NULL;
}

int MelderProgress::_depth = 0;

MelderProgress::ProgressProc MelderProgress::_p_progressProc = & defaultProgress;
MelderProgress::MonitorProc MelderProgress::_p_monitorProc = & defaultMonitor;

void Melder_progressOff () { MelderProgress::_depth --; }
void Melder_progressOn () { MelderProgress::_depth ++; }

void MelderProgress::_doProgress (double progress, conststring32 message) {
	if (! Melder_batch && MelderProgress::_depth >= 0 && Melder_debug != 14)
		MelderProgress::_p_progressProc (progress, message);
}

MelderString MelderProgress::_buffer = { 0, 0, nullptr };

void * MelderProgress::_doMonitor (double progress, conststring32 message) {
	if (! Melder_batch && MelderProgress::_depth >= 0) {
		void *result = MelderProgress::_p_monitorProc (progress, message);
		if (result) return result;
	}
	return progress <= 0.0 ? nullptr /* no Graphics */ : (void *) -1 /* any non-null pointer */;
}

void Melder_setProgressProc (MelderProgress::ProgressProc proc) {
	MelderProgress::_p_progressProc = ( proc ? proc : & defaultProgress );
}

void Melder_setMonitorProc (MelderProgress::MonitorProc proc) {
	MelderProgress::_p_monitorProc = ( proc ? proc : & defaultMonitor );
}

/********** NUMBER AND STRING COMPARISONS **********/

bool Melder_numberMatchesCriterion (double value, kMelder_number which, double criterion) {
	return
		(which == kMelder_number::EQUAL_TO && value == criterion) ||
		(which == kMelder_number::NOT_EQUAL_TO && value != criterion) ||
		(which == kMelder_number::LESS_THAN && value < criterion) ||
		(which == kMelder_number::LESS_THAN_OR_EQUAL_TO && value <= criterion) ||
		(which == kMelder_number::GREATER_THAN && value > criterion) ||
		(which == kMelder_number::GREATER_THAN_OR_EQUAL_TO && value >= criterion);
}

inline static char32 * str32str_word_optionallyCaseSensitive (conststring32 string, conststring32 find,
	bool ink, bool caseSensitive, bool startFree, bool endFree) noexcept
{
	integer length = str32len (find);
	if (length == 0) return (char32 *) string;
	conststring32 movingString = string;
	do {
		conststring32 movingFind = find;
		char32 firstCharacter = ( caseSensitive ? * movingFind ++ : Melder_toLowerCase (* movingFind ++) );   // optimization
		do {
			char32 kar;
			do {
				kar = ( caseSensitive ? * movingString ++ : Melder_toLowerCase (* movingString ++) );
				if (kar == U'\0') return nullptr;
			} while (kar != firstCharacter);
		} while (caseSensitive ? str32ncmp (movingString, movingFind, length - 1) : str32ncmp_caseInsensitive (movingString, movingFind, length - 1));
		/*
			We found a match.
		*/
		movingString --;
		if ((startFree || movingString == string || ( ink ? Melder_isHorizontalOrVerticalSpace (movingString [-1]) : ! Melder_isWordCharacter (movingString [-1]) )) &&
			(endFree || movingString [length] == U'\0' || (ink ? Melder_isHorizontalOrVerticalSpace (movingString [length]) : ! Melder_isWordCharacter (movingString [length]) )))
		{
			return (char32 *) movingString;
		} else {
			movingString ++;
		}
	} while (true);
	return nullptr;   // can never occur
}

bool Melder_stringMatchesCriterion (conststring32 value, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	if (! value) {
		value = U"";   // regard null strings as empty strings, as is usual in Praat
	}
	if (! criterion) {
		criterion = U"";   // regard null strings as empty strings, as is usual in Praat
	}
	switch (which)
	{
		case kMelder_string::UNDEFINED:
		{
			Melder_fatal (U"Melder_stringMatchesCriterion: unknown criterion.");
		}
		case kMelder_string::EQUAL_TO:
		case kMelder_string::NOT_EQUAL_TO:
		{
			bool doesMatch = str32equ_optionallyCaseSensitive (value, criterion, caseSensitive);
			return which == kMelder_string::EQUAL_TO ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS:
		case kMelder_string::DOES_NOT_CONTAIN:
		{
			bool doesMatch = !! str32str_optionallyCaseSensitive (value, criterion, caseSensitive);
			return which == kMelder_string::CONTAINS ? doesMatch : ! doesMatch;
		}
		case kMelder_string::STARTS_WITH:
		case kMelder_string::DOES_NOT_START_WITH:
		{
			bool doesMatch = str32nequ_optionallyCaseSensitive (value, criterion, str32len (criterion), caseSensitive);
			return which == kMelder_string::STARTS_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::ENDS_WITH:
		case kMelder_string::DOES_NOT_END_WITH:
		{
			integer criterionLength = str32len (criterion), valueLength = str32len (value);
			bool doesMatch = criterionLength <= valueLength &&
				str32equ_optionallyCaseSensitive (value + valueLength - criterionLength, criterion, caseSensitive);
			return which == kMelder_string::ENDS_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_WORD:
		case kMelder_string::DOES_NOT_CONTAIN_WORD:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, false, caseSensitive, false, false);
			return which == kMelder_string::CONTAINS_WORD ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_WORD_STARTING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_WORD_STARTING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, false, caseSensitive, false, true);
			return which == kMelder_string::CONTAINS_WORD_STARTING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_WORD_ENDING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_WORD_ENDING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, false, caseSensitive, true, false);
			return which == kMelder_string::CONTAINS_WORD_ENDING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_INK:
		case kMelder_string::DOES_NOT_CONTAIN_INK:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, true, caseSensitive, false, false);
			return which == kMelder_string::CONTAINS_INK ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_INK_STARTING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_INK_STARTING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, true, caseSensitive, false, true);
			return which == kMelder_string::CONTAINS_INK_STARTING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_INK_ENDING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_INK_ENDING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, true, caseSensitive, true, false);
			return which == kMelder_string::CONTAINS_INK_ENDING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::MATCH_REGEXP:
		{
			char32 *place = nullptr;
			regexp *compiled_regexp = CompileRE_throwable (criterion, ! REDFLT_CASE_INSENSITIVE);
			if (ExecRE (compiled_regexp, nullptr, value, nullptr, 0, U'\0', U'\0', nullptr, nullptr))
				place = compiled_regexp -> startp [0];
			free (compiled_regexp);
			return !! place;
		}
	}
	//return false;   // should not occur
}

void Melder_help (conststring32 query) {
	theMelderFunctions. help (query);
}

void Melder_search () {
	theMelderFunctions. search ();
}

/********** WARNING **********/

int MelderWarning::_depth = 0;

void MelderWarning::_defaultProc (conststring32 message) {
	MelderConsole::write (U"Warning: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}

MelderWarning::Proc MelderWarning::_p_currentProc = & MelderWarning::_defaultProc;

MelderString MelderWarning::_buffer { 0, 0, nullptr };

void Melder_warningOff () { MelderWarning::_depth --; }
void Melder_warningOn () { MelderWarning::_depth ++; }

void Melder_setWarningProc (MelderWarning::Proc proc) {
	MelderWarning::_p_currentProc = ( proc ? proc : & MelderWarning::_defaultProc );
}

void Melder_beep () {
	#ifdef macintosh
		AudioServicesPlayAlertSound (kSystemSoundID_UserPreferredAlert);
	#else
		fprintf (stderr, "\a");
	#endif
}

/*********** FATAL **********/

MelderThread_MUTEX (theMelder_fatal_mutex);

void Melder_message_init () {
	static bool inited = false;
	if (! inited) { MelderThread_MUTEX_INIT (theMelder_fatal_mutex); inited = true; }
}

constexpr int Melder_FATAL_BUFFER_SIZE { 2000 };
static char32 theFatalBuffer [Melder_FATAL_BUFFER_SIZE];
static const conststring32 theCrashMessage { U"Praat will crash. Notify the author (paul.boersma@uva.nl) with the following information:\n" };

void Melder_fatal (const MelderArg& arg1,
	const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7,
	const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10)
{
	MelderThread_LOCK (theMelder_fatal_mutex);
	conststring32 s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	conststring32 s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	conststring32 s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	conststring32 s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	conststring32 s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	conststring32 s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	conststring32 s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	conststring32 s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	conststring32 s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	conststring32 s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	if (length + length4  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s4);  length += length4;  }
	if (length + length5  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s5);  length += length5;  }
	if (length + length6  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s6);  length += length6;  }
	if (length + length7  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s7);  length += length7;  }
	if (length + length8  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s8);  length += length8;  }
	if (length + length9  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s9);  length += length9;  }
	if (length + length10 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s10); length += length10; }
	trace (U"FATAL: ", theFatalBuffer);
	theMelderFunctions. fatal (theFatalBuffer);
	abort ();
}

void Melder_assert_ (const char *fileName, int lineNumber, const char *condition) {
	/*
	 * This function tries to make sure that it allocates no heap memory.
	 * Hence, character conversion is done inline rather than with Melder_peek8to32(),
	 * and Melder_integer() is also avoided.
	 */
	MelderThread_LOCK (theMelder_fatal_mutex);
	static char32 fileNameBuffer [1000], conditionBuffer [1000], lineNumberBuffer [40];
	Melder_8to32_inplace (fileName, fileNameBuffer, kMelder_textInputEncoding::UTF8);
	Melder_8to32_inplace (condition, conditionBuffer, kMelder_textInputEncoding::UTF8);
	static char lineNumberBuffer8 [40];
	sprintf (lineNumberBuffer8, "%d", lineNumber);
	Melder_8to32_inplace (lineNumberBuffer8, lineNumberBuffer, kMelder_textInputEncoding::UTF8);
	str32cpy (theFatalBuffer, theCrashMessage);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"Assertion failed in file \"");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), fileNameBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"\" at line ");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), lineNumberBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U":\n   ");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), conditionBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"\n");
	trace (U"FATAL: ", theFatalBuffer);
	theMelderFunctions. fatal (theFatalBuffer);   // ...but this call will use heap memory...
	abort ();
}

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

void Melder_setHelpProc (void (*help) (conststring32 query))
	{ theMelderFunctions. help = help ? help : defaultHelp; }

void Melder_setSearchProc (void (*search) (void))
	{ theMelderFunctions. search = search ? search : defaultSearch; }

void Melder_setFatalProc (void (*fatal) (conststring32))
	{ theMelderFunctions. fatal = fatal ? fatal : defaultFatal; }

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

/* End of file melder.cpp */
