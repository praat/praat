/* melder_debug.cpp
 *
 * Copyright (C) 2000-2017 Paul Boersma
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
#include <math.h>   // for floor()
#ifdef linux
	#include "GuiP.h"
#endif
#include <time.h>
#include "praat_version.h"
#ifdef _WIN32
	#include "UnicodeData.h"
	#include <windows.h>
#endif

int Melder_debug = 0;
/*
Melder_debug will always be set to 0 when Praat starts up.
If Melder_debug is temporarily set to the following values
(preferably with the "Debug..." command under Praat->Technical,
 which you can use from a script),
the behaviour of Praat will temporarily change in the following ways:

1: Windows: use C-clock instead of multimedia-clock in melder_audio.cpp.
2: Windows: always reset waveOut, even when played to end, in melder_audio.cpp.
3: Windows: reset waveOut if unprepareHeader fails, and retry, in melder_audio.cpp. STAY: 20010214
4: Windows: fewer callbacks during sound play, in melder_audio.cpp. STAY: 20010214
6: Windows: info on metafile properties in Picture.cpp.
8: Windows: don't reset waveIn, in SoundRecorder.cpp.
9: flush Error in FunctionEditor_Sound_draw
10: geometric pens
11: clicked item in option menu in Ui.cpp
14: switches off the progress window in melder.cpp
15: don't use TrueType IPA fonts, but always bitmaps instead
16: Linux: open /dev/dsp without O_NDELAY
17: debugging on in Formula.cpp
18: no endian assumptions in abcio.cpp
19: show path name in UiOutfile_do
20: trace PortAudio
21: Mac: list supported document formats when printing
22: UTF-8 tests in logo
23: recognize special chunks in WAV files
24: measure buttons in drawing area
25: read crooked Manipulation files (January 2008)
26: force OT-GLA
27: force HG-GLA
28: don't use GSL in NUMfisherQ
29: use GSL in NUMinvFisherQ
30: pitch path finder: use octave jump cost across voiceless parts
31: Pitch analysis: formant pulling on
32: show info on file names in ExperimentMFC
33: trace the Pitch path finder
34: trace memory allocation and deallocation
35: debugging on for QuickTime movie file opening
37: debug autoThing
38: debug autoNUMvector
39: debug autostring
40: debug Thing_new
41: OTGrammar_getWinner: always first choice rather than random choice 
42: OTGrammar_getWinner: always last choice rather than random choice
43: trace class table initialization
44: trace Collection
45: tracing structMatrix :: read ()
46: trace GTK parent sizes in _GuiObject_position ()
47: force resampling in OTGrammar RIP
48: compute sum, mean, stdev with naive implementation in real64
49: compute sum, mean, stdev with naive implementation in real80
50: compute sum, mean, stdev with first-element offset (80 bits)
51: compute sum, mean, stdev with two cycles, as in R (80 bits)
(other numbers than 48-51: compute sum, mean, stdev with simple pairwise algorithm, base case 64 [80 bits])
181: read and write native-endian real64
900: use DG Meta Serif Science instead of Palatino
1264: Mac: Sound_record_fixedTime uses microphone "FW Solo (1264)"

(negative values are for David)

*/

/*
 * In order to make sure that Melder_casual() and trace() can be called from anywhere,
 * including e.g. from Melder_realloc() or Melder_free(),
 * they cannot use any Melder_xxx() functions.
 */

/*
 * peek32to8 substitutes for Melder_peek32to8(),
 * which can call Melder_realloc() and Melder_free();
 * also, we need no newline nativization, as Melder_32to8_inplace() does.
 */
static const char * peek32to8 (const char32 *string) {
	if (! string) return "";
	static char *buffer { nullptr };
	static int64 bufferSize { 0 };
	int64 n = str32len (string);
	int64 sizeNeeded = n * 4 + 1;
	if ((bufferSize - sizeNeeded) * (int64) sizeof (char) >= 10000) {
		free (buffer);
		buffer = nullptr;   // undangle
		bufferSize = 0;
	}
	if (sizeNeeded > bufferSize) {
		sizeNeeded = (int64) floor (sizeNeeded * 1.61803) + 100;
		buffer = (char *) realloc (buffer, (size_t) sizeNeeded * sizeof (char));
		if (buffer == nullptr) {
			bufferSize = 0;
			return "(out of memory during Melder_casual)";
		}
		bufferSize = sizeNeeded;
	}
	int64 i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32 kar = string [i];
		if (kar <= 0x00007F) {   // 7 bits
			buffer [j ++] = (char) (char8) kar;   // guarded truncation
		} else if (kar <= 0x0007FF) {   // 11 bits
			buffer [j ++] = (char) (char8) (0x0000C0 | (kar >> 6));   // the upper 5 bits yield a number between 0xC4 and 0xDF
			buffer [j ++] = (char) (char8) (0x000080 | (kar & 0x00003F));   // the lower 6 bits yield a number between 0x80 and 0xBF
		} else if (kar <= 0x00FFFF) {   // 16 bits
			buffer [j ++] = (char) (char8) (0x0000E0 | (kar >> 12));   // the upper 4 bits yield a number between 0xE0 and 0xEF
			buffer [j ++] = (char) (char8) (0x000080 | ((kar >> 6) & 0x00003F));
			buffer [j ++] = (char) (char8) (0x000080 | (kar & 0x00003F));
		} else {   // 21 bits
			buffer [j ++] = (char) (char8) (0x0000F0 | (kar >> 18));   // the upper 3 bits yield a number between 0xF0 and 0xF4 (0x10FFFF >> 18 == 4)
			buffer [j ++] = (char) (char8) (0x000080 | ((kar >> 12) & 0x00003F));   // the next 6 bits
			buffer [j ++] = (char) (char8) (0x000080 | ((kar >> 6) & 0x00003F));   // the third 6 bits
			buffer [j ++] = (char) (char8) (0x000080 | (kar & 0x00003F));   // the lower 6 bits
		}
	}
	buffer [j] = '\0';
	return buffer;
}
#ifdef _WIN32
static const char16 * peek32to16 (const char32 *string) {
	if (! string) return u"";
	static char16 *buffer { nullptr };
	static int64 bufferSize { 0 };
	int64 n = str32len (string);
	int64 sizeNeeded = n * 2 + 1;
	if ((bufferSize - sizeNeeded) * (int64) sizeof (char16) >= 10000) {
		free (buffer);
		bufferSize = 0;
	}
	if (sizeNeeded > bufferSize) {
		sizeNeeded = (int64) floor (sizeNeeded * 1.61803) + 100;
		buffer = (char16 *) realloc (buffer, (size_t) sizeNeeded * sizeof (char16));
		if (! buffer) {
			bufferSize = 0;
			return u"(out of memory during trace)";
		}
		bufferSize = sizeNeeded;
	}
	int64 i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32 kar = string [i];
		if (kar <= 0x00D7FF) {   // 16 bits
			buffer [j ++] = (char16) kar;   // guarded truncation
		} else if (kar <= 0x00DFFF) {   // 16 bits
			buffer [j ++] = (char16) UNICODE_REPLACEMENT_CHARACTER;   // forbidden for UTF-32
		} else if (kar <= 0x00FFFF) {   // 16 bits
			buffer [j ++] = (char16) kar;   // guarded truncation
		} else if (kar <= 0x10FFFF) {   // 21 bits
			kar -= 0x010000;
			buffer [j ++] = (char16) (0x00D800 | (kar >> 10));   // guarded truncation
			buffer [j ++] = (char16) (0x00DC00 | (kar & 0x0003FF));   // guarded truncation
		} else {   // 21 bits
			buffer [j ++] = (char16) UNICODE_REPLACEMENT_CHARACTER;
		}
	}
	buffer [j] = u'\0';
	return buffer;
}
#endif

bool Melder_consoleIsAnsi = false;

//extern FILE *winstdout;
void Melder_writeToConsole (const char32 *message, bool useStderr) {
	if (! message) return;
	#if defined (_WIN32)
		(void) useStderr;
		static HANDLE console = nullptr;
		if (! console) {
			console = CreateFileW (L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, 0);
			//freopen ("CONOUT$", "w", stdout);
		}
		if (Melder_consoleIsAnsi) {
			size_t n = str32len (message);
			for (integer i = 0; i < n; i ++) {
				unsigned int kar = (unsigned short) message [i];
				fputc (kar, stdout);
			}
			//CHAR* messageA = (CHAR*) peek32to8 (message);
			//WriteConsoleA (console, messageA, strlen (messageA), nullptr, nullptr);
		//} else if (Melder_consoleIsUtf8) {
			//char *messageA = Melder_peek32to8 (message);
			//fprintf (stdout, "%s", messageA);
		} else {
			WCHAR* messageW = (WCHAR*) peek32to16 (message);
			WriteConsoleW (console, messageW, wcslen (messageW), nullptr, nullptr);
		}
	#else
		FILE *f = ( useStderr ? stderr : stdout );
		for (const char32* p = message; *p != U'\0'; p ++) {
			char32 kar = *p;
			if (kar <= 0x00007F) {
				fputc ((int) kar, f);   // because fputc wants an int instead of a uint8 (guarded conversion)
			} else if (kar <= 0x0007FF) {
				fputc (0xC0 | (kar >> 6), f);
				fputc (0x80 | (kar & 0x00003F), f);
			} else if (kar <= 0x00FFFF) {
				fputc (0xE0 | (kar >> 12), f);
				fputc (0x80 | ((kar >> 6) & 0x00003F), f);
				fputc (0x80 | (kar & 0x00003F), f);
			} else {
				fputc (0xF0 | (kar >> 18), f);
				fputc (0x80 | ((kar >> 12) & 0x00003F), f);
				fputc (0x80 | ((kar >> 6) & 0x00003F), f);
				fputc (0x80 | (kar & 0x00003F), f);
			}
		}
	#endif
}

/********** CASUAL **********/

void Melder_casual (Melder_1_ARG) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_2_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_3_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_4_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_5_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_6_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_7_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_8_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_9_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (arg9. _arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_10_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (arg9. _arg, true);
	Melder_writeToConsole (arg10._arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_11_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (arg9. _arg, true);
	Melder_writeToConsole (arg10._arg, true);
	Melder_writeToConsole (arg11._arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_13_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (arg9. _arg, true);
	Melder_writeToConsole (arg10._arg, true);
	Melder_writeToConsole (arg11._arg, true);
	Melder_writeToConsole (arg12._arg, true);
	Melder_writeToConsole (arg13._arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_15_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (arg9. _arg, true);
	Melder_writeToConsole (arg10._arg, true);
	Melder_writeToConsole (arg11._arg, true);
	Melder_writeToConsole (arg12._arg, true);
	Melder_writeToConsole (arg13._arg, true);
	Melder_writeToConsole (arg14._arg, true);
	Melder_writeToConsole (arg15._arg, true);
	Melder_writeToConsole (U"\n", true);
}
void Melder_casual (Melder_19_ARGS) {
	Melder_writeToConsole (arg1. _arg, true);
	Melder_writeToConsole (arg2. _arg, true);
	Melder_writeToConsole (arg3. _arg, true);
	Melder_writeToConsole (arg4. _arg, true);
	Melder_writeToConsole (arg5. _arg, true);
	Melder_writeToConsole (arg6. _arg, true);
	Melder_writeToConsole (arg7. _arg, true);
	Melder_writeToConsole (arg8. _arg, true);
	Melder_writeToConsole (arg9. _arg, true);
	Melder_writeToConsole (arg10._arg, true);
	Melder_writeToConsole (arg11._arg, true);
	Melder_writeToConsole (arg12._arg, true);
	Melder_writeToConsole (arg13._arg, true);
	Melder_writeToConsole (arg14._arg, true);
	Melder_writeToConsole (arg15._arg, true);
	Melder_writeToConsole (arg16._arg, true);
	Melder_writeToConsole (arg17._arg, true);
	Melder_writeToConsole (arg18._arg, true);
	Melder_writeToConsole (arg19._arg, true);
	Melder_writeToConsole (U"\n", true);
}

/********** TRACE **********/

bool Melder_isTracing = false;
static structMelderFile theTracingFile { };

void Melder_tracingToFile (MelderFile file) {
	MelderFile_copy (file, & theTracingFile);
	MelderFile_delete (& theTracingFile);
}

static FILE * Melder_trace_open (const char *fileName, int lineNumber, const char *functionName) {
	FILE *f;
	#if defined (_WIN32) && ! defined (__CYGWIN__)
		f = _wfopen ((const wchar_t *) peek32to16 (theTracingFile. path), L"a");
	#else
		char utf8path [kMelder_MAXPATH+1];
		Melder_str32To8bitFileRepresentation_inplace (theTracingFile. path, utf8path);   // this Melder_xxx() function is OK to call
		f = fopen ((char *) utf8path, "a");
	#endif
	if (! f) f = stderr;   // if the file cannot be opened, we can still trace to stderr!
	if (fileName) {
		const char *slashPosition = strrchr (fileName, Melder_DIRECTORY_SEPARATOR);
		fprintf (f, "%s (%s:%d): ", functionName, slashPosition ? slashPosition + 1 : fileName, lineNumber);
	} else {
		fprintf (f, "%s: ", functionName);
	}
	return f;
}

static void Melder_trace_close (FILE *f) {
	fprintf (f, "\n");
	if (f != stderr) fclose (f);
}

void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_1_ARG) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_2_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_3_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_4_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_5_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_6_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_7_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_8_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_9_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	fprintf (f, "%s", peek32to8 (arg9. _arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_10_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	fprintf (f, "%s", peek32to8 (arg9. _arg));
	fprintf (f, "%s", peek32to8 (arg10._arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_11_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	fprintf (f, "%s", peek32to8 (arg9. _arg));
	fprintf (f, "%s", peek32to8 (arg10._arg));
	fprintf (f, "%s", peek32to8 (arg11._arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_13_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	fprintf (f, "%s", peek32to8 (arg9. _arg));
	fprintf (f, "%s", peek32to8 (arg10._arg));
	fprintf (f, "%s", peek32to8 (arg11._arg));
	fprintf (f, "%s", peek32to8 (arg12._arg));
	fprintf (f, "%s", peek32to8 (arg13._arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_15_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	fprintf (f, "%s", peek32to8 (arg9. _arg));
	fprintf (f, "%s", peek32to8 (arg10._arg));
	fprintf (f, "%s", peek32to8 (arg11._arg));
	fprintf (f, "%s", peek32to8 (arg12._arg));
	fprintf (f, "%s", peek32to8 (arg13._arg));
	fprintf (f, "%s", peek32to8 (arg14._arg));
	fprintf (f, "%s", peek32to8 (arg15._arg));
	Melder_trace_close (f);
}
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_19_ARGS) {
	if (! Melder_isTracing || MelderFile_isNull (& theTracingFile)) return;
	FILE *f = Melder_trace_open (fileName, lineNumber, functionName);
	fprintf (f, "%s", peek32to8 (arg1. _arg));
	fprintf (f, "%s", peek32to8 (arg2. _arg));
	fprintf (f, "%s", peek32to8 (arg3. _arg));
	fprintf (f, "%s", peek32to8 (arg4. _arg));
	fprintf (f, "%s", peek32to8 (arg5. _arg));
	fprintf (f, "%s", peek32to8 (arg6. _arg));
	fprintf (f, "%s", peek32to8 (arg7. _arg));
	fprintf (f, "%s", peek32to8 (arg8. _arg));
	fprintf (f, "%s", peek32to8 (arg9. _arg));
	fprintf (f, "%s", peek32to8 (arg10._arg));
	fprintf (f, "%s", peek32to8 (arg11._arg));
	fprintf (f, "%s", peek32to8 (arg12._arg));
	fprintf (f, "%s", peek32to8 (arg13._arg));
	fprintf (f, "%s", peek32to8 (arg14._arg));
	fprintf (f, "%s", peek32to8 (arg15._arg));
	fprintf (f, "%s", peek32to8 (arg16._arg));
	fprintf (f, "%s", peek32to8 (arg17._arg));
	fprintf (f, "%s", peek32to8 (arg18._arg));
	fprintf (f, "%s", peek32to8 (arg19._arg));
	Melder_trace_close (f);
}

#if defined (linux) && ! defined (NO_GUI)
static void theGtkLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	FILE *f = Melder_trace_open (nullptr, 0, "GTK");
	fprintf (f, "%s", message);
	Melder_trace_close (f);
}
static void theGlibLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	FILE *f = Melder_trace_open (nullptr, 0, "GLib");
	fprintf (f, "%s", message);
	Melder_trace_close (f);
}
static void theGlibGobjectLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	FILE *f = Melder_trace_open (nullptr, 0, "GLib-GObject");
	fprintf (f, "%s", message);
	Melder_trace_close (f);
}
#endif

void Melder_setTracing (bool tracing) {
	time_t today = time (nullptr);
	#define xstr(s) str(s)
	#define str(s) #s
	if (! tracing)
		trace (U"switch tracing off"
			U" in Praat version ", Melder_peek8to32 (xstr (PRAAT_VERSION_STR)),
			U" at ", Melder_peek8to32 (ctime (& today))
		);
	Melder_isTracing = tracing;
	#if defined (linux) && ! defined (NO_GUI)
		static guint handler_id1, handler_id2, handler_id3;
		if (tracing) {
			handler_id1 = g_log_set_handler ("Gtk",          (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGtkLogHandler,         nullptr);
			handler_id2 = g_log_set_handler ("GLib",         (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGlibLogHandler,        nullptr);
			handler_id3 = g_log_set_handler ("GLib-GObject", (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGlibGobjectLogHandler, nullptr);
		} else {
			if (handler_id1) g_log_remove_handler ("Gtk",          handler_id1);
			if (handler_id2) g_log_remove_handler ("GLib",         handler_id2);
			if (handler_id3) g_log_remove_handler ("GLib-GObject", handler_id3);
			handler_id1 = handler_id2 = handler_id3 = 0;
		}
	#endif
	if (tracing)
		trace (U"switch tracing on"
			U" in Praat version ", Melder_peek8to32 (xstr (PRAAT_VERSION_STR)),
			U" at ", Melder_peek8to32 (ctime (& today))
		);
}

/* End of file melder_debug.cpp */
