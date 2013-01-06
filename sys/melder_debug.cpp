/* melder_debug.cpp
 *
 * Copyright (C) 2000-2012 Paul Boersma
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
#include "GuiP.h"
#include "praat_version.h"

int Melder_debug = 0;

/*

If Melder_debug is set to the following values in Praat,
the behaviour of that program changes in the following way:

1: Windows: use C-clock instead of multimedia-clock in melder_audio.cpp.
2: Windows: always reset waveOut, even when played to end, in melder_audio.cpp.
3: Windows: reset waveOut if unprepareHeader fails, and retry, in melder_audio.cpp. STAY: 20010214
4: Windows: fewer callbacks during sound play, in melder_audio.cpp. STAY: 20010214
6: Windows: info on metafile properties in Picture.cpp.
8: Windows: don't reset waveIn, in SoundRecorder.cpp.
9: flush Error in FunctionEditor_Sound_draw
10: geometric pens
11: clicked item in option menu in Ui.cpp.
12: no forced update event in XmUpdateDisplay on Mac
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
1264: Mac: Sound_recordFixedTime uses microphone "FW Solo (1264)"

(negative values are for David)

*/

static bool theTracing = false;
static structMelderFile theTracingFile = { 0 };

#if gtk
static void theGtkLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	Melder_trace_ (NULL, 0, "GTK", "%s", message);
}
static void theGlibLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	Melder_trace_ (NULL, 0, "GLib", "%s", message);
}
static void theGlibGobjectLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	Melder_trace_ (NULL, 0, "GLib-GObject", "%s", message);
}
#endif

void Melder_setTracing (bool tracing) {
	time_t today = time (NULL);	
	#define xstr(s) str(s)
	#define str(s) #s
	if (! tracing)
		trace ("switch tracing off in Praat version %s at %s", xstr (PRAAT_VERSION_STR), ctime (& today));
	theTracing = tracing;
	#if gtk
		static guint handler_id1, handler_id2, handler_id3;
		if (tracing) {
			handler_id1 = g_log_set_handler ("Gtk",          (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGtkLogHandler,         NULL);
			handler_id2 = g_log_set_handler ("GLib",         (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGlibLogHandler,        NULL);
			handler_id3 = g_log_set_handler ("GLib-GObject", (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGlibGobjectLogHandler, NULL);
		} else {
			if (handler_id1) g_log_remove_handler ("Gtk",          handler_id1);
			if (handler_id2) g_log_remove_handler ("GLib",         handler_id2);
			if (handler_id3) g_log_remove_handler ("GLib-GObject", handler_id3);
			handler_id1 = handler_id2 = handler_id3 = 0;
		}
	#endif
	if (tracing)
		trace ("switch tracing on in Praat version %s at %s", xstr (PRAAT_VERSION_STR), ctime (& today));
}

bool Melder_getTracing () {
	return theTracing;
}

void Melder_tracingToFile (MelderFile file) {
	MelderFile_copy (file, & theTracingFile);
	MelderFile_delete (& theTracingFile);
}

void Melder_trace_ (const char *fileName, int lineNumber, const char *functionName, const char *format, ...) {
	if (! theTracing || MelderFile_isNull (& theTracingFile)) return;
	try {
		FILE *f = Melder_fopen (& theTracingFile, "a");
		if (fileName) {
			const char *slashPosition = strrchr (fileName, Melder_DIRECTORY_SEPARATOR);
			fprintf (f, "%s (%s:%d): ", functionName, slashPosition ? slashPosition + 1 : fileName, lineNumber);
		} else {
			fprintf (f, "%s: ", functionName);
		}
		va_list arg;
		va_start (arg, format);
		vfprintf (f, format, arg);
		va_end (arg);
		char lastCharacter = format [0] == '\0' ? '\0' : format [strlen (format) - 1];
		fprintf (f, strchr (".!?,;", lastCharacter) ? "\n" : ".\n");
		Melder_fclose (& theTracingFile, f);
	} catch (MelderError) {
		// ignore
	}
}

/* End of file melder_debug.cpp */
