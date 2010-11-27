/* melder.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2002/03/13 Mach
 * pb 2002/12/11 MelderInfo
 * pb 2003/12/29 Melder_warning: added XMapRaised because delete response is UNMAP
 * pb 2004/04/06 motif_information drains text window only, i.e. no longer updates all windows
                 (which used to cause up to seven seconds of delay in a 1-second sound window)
 * pb 2004/10/24 info buffer can grow
 * pb 2004/11/28 author notification in Melder_fatal
 * pb 2005/03/04 number and string comparisons, including regular expressions
 * pb 2005/06/16 removed enums from number and string comparisons (ints give no compiler warnings)
 * pb 2005/07/19 Melder_stringMatchesCriterion: regard NULL criterion as empty string
 * pb 2007/05/24 more wchar_t
 * pb 2007/05/26 Melder_stringMatchesCriterionW
 * pb 2007/06/19 removed some
 * pb 2007/08/12 wchar_t in helpProc
 * pb 2007/12/02 enums
 * pb 2007/12/13 Melder_writeToConsole
 * pb 2007/12/18 Gui
 * sdk 2008/01/22 GTK
 * pb 2009/01/20 removed pause
 * fb 2010/02/26 GTK
 * pb 2010/06/22 GTK: correct hiding and showing again
 * pb 2010/07/29 removed GuiDialog_show
 * pb 2010/11/26 even Unix now has a GUI fatal window
 */

#include <math.h>
#include <time.h>
#include <ctype.h>
#include "melder.h"
#include "longchar.h"
#include "regularExp.h"
#ifdef _WIN32
	#include <windows.h>
#endif
#if defined (macintosh)
	#include "macport_on.h"
	//#include <Sound.h>
	#include "macport_off.h"
#endif
#ifndef CONSOLE_APPLICATION
	#include "Graphics.h"
	#include "machine.h"
	#ifdef macintosh
		#include "macport_on.h"
		//#include <Events.h>
		#include <Dialogs.h>
		#include <MacErrors.h>
		#include "macport_off.h"
	#endif
	#include "Gui.h"
#endif

#include "enums_getText.h"
#include "melder_enums.h"
#include "enums_getValue.h"
#include "melder_enums.h"

/********** Exported variables. **********/

int Melder_batch;   /* Don't we have a GUI?- Set once at application start-up. */
int Melder_backgrounding;   /* Are we running a script?- Set and unset dynamically. */
char Melder_buffer1 [30001], Melder_buffer2 [30001];
unsigned long Melder_systemVersion;

#ifndef CONSOLE_APPLICATION
	void *Melder_appContext;   /* XtAppContext* */
	void *Melder_topShell;   /* Widget */
#endif

static void defaultHelp (const wchar_t *query) {
	Melder_error3 (L"Do not know how to find help on \"", query, L"\".");
	Melder_flushError (NULL);
}

static void defaultSearch (void) {
	Melder_flushError ("Do not know how to search.");
}

static void defaultWarning (wchar_t *message) {
	Melder_writeToConsole (L"Warning: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (L"\n", true);
}

static void defaultFatal (wchar_t *message) {
	Melder_writeToConsole (L"Fatal error: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (L"\n", true);
}

static int defaultPublish (void *anything) {
	(void) anything;
	return 0;   /* Nothing published. */
}

static int defaultRecord (double duration) {
	(void) duration;
	return 0;   /* Nothing recorded. */
}

static int defaultRecordFromFile (MelderFile file) {
	(void) file;
	return 0;   /* Nothing recorded. */
}

static void defaultPlay (void) {}

static void defaultPlayReverse (void) {}

static int defaultPublishPlayed (void) {
	return 0;   /* Nothing published. */
}

/********** Current message methods: initialize to default (batch) behaviour. **********/

static struct {
	void (*help) (const wchar_t *query);
	void (*search) (void);
	void (*warning) (wchar_t *message);
	void (*fatal) (wchar_t *message);
	int (*publish) (void *anything);
	int (*record) (double duration);
	int (*recordFromFile) (MelderFile fs);
	void (*play) (void);
	void (*playReverse) (void);
	int (*publishPlayed) (void);
}
	theMelder = {
		defaultHelp, defaultSearch,
		defaultWarning, defaultFatal,
		defaultPublish,
		defaultRecord, defaultRecordFromFile, defaultPlay, defaultPlayReverse, defaultPublishPlayed
	};

/********** CASUAL **********/

void Melder_casual (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	fprintf (stderr, "%s\n", Melder_buffer1);
	va_end (arg);
}

/********** PROGRESS **********/

static int theProgressDepth = 0;
void Melder_progressOff (void) { theProgressDepth --; }
void Melder_progressOn (void) { theProgressDepth ++; }

#ifndef CONSOLE_APPLICATION
static int waitWhileProgress (double progress, const wchar_t *message, Widget dia, Widget scale, Widget label1, Widget label2, Widget cancelButton) {
	#if gtk
		// Wait for all pending events to be processed. If someone knows how to inspect GTK's
		// event queue for specific events, dump the code here, please.
		// Until then, the button click attaches a g_object data key named "pressed" to the cancelButton
		// which this function reads out in order to tell whether interruption has occurred
		while (gtk_events_pending ())
			gtk_main_iteration ();
	#elif defined (macintosh)
	{
		EventRecord event;
		(void) cancelButton;
		// BUG: key events are handled somewhat earlier nowadays, so the next trick does not really ignore key events (and menu commands).
		// Dangerous!
		while (GetNextEvent (keyDownMask, & event)) {
			if ((event.modifiers & cmdKey) && (event.message & charCodeMask) == '.') {
				FlushEvents (everyEvent, 0);
				XtUnmanageChild (dia);
				return 0;
			}
		}
		do { XtNextEvent ((XEvent *) & event); XtDispatchEvent ((XEvent *) & event); } while (event.what);
	}
	#elif defined (_WIN32)
	{
		XEvent event;
		while (PeekMessage (& event, 0, 0, 0, PM_REMOVE)) {
			if (event. message == WM_KEYDOWN) {
				/*
				 * Ignore all key-down messages, except Escape.
				 */
				if (LOWORD (event. wParam) == VK_ESCAPE) {
					XtUnmanageChild (dia);
					return 0;
				}
			} else if (event. message == WM_LBUTTONDOWN) {
				/*
				 * Ignore all mouse-down messages, except click in Interrupt button.
				 */
				Widget me = (Widget) GetWindowLong (event. hwnd, GWL_USERDATA);
				if (me == cancelButton) {
					XtUnmanageChild (dia);
					return 0;
				}
			} else if (event. message != WM_SYSKEYDOWN) {
				/*
				 * Process paint messages etc.
				 */
				DispatchMessage (& event);
			}
		}
	}
	#else
	{
		XEvent event;
		if (XCheckTypedWindowEvent (XtDisplay (cancelButton), XtWindow (cancelButton), ButtonPress, & event)) {
			XtUnmanageChild (dia);
			return 0;
		}
	}
	#endif
	if (progress >= 1.0) {
		GuiObject_hide (dia);
	} else {
		if (progress <= 0.0) progress = 0.0;
		GuiObject_show (dia);   // TODO: prevent raising to the front
		wchar_t *newline = wcschr (message, '\n');
		if (newline != NULL) {
			static MelderString buffer = { 0 };
			MelderString_copy (& buffer, message);
			buffer.string [newline - message] = '\0';
			GuiLabel_setString (label1, buffer.string);
			buffer.string [newline - message] = '\n';
			GuiLabel_setString (label2, buffer.string + (newline - message) + 1);
		} else {
			GuiLabel_setString (label1, message);
			GuiLabel_setString (label2, L"");
		}
		#if gtk
			// update progress bar
			gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (scale), progress);
			while (gtk_events_pending ())
				gtk_main_iteration ();
			// check whether cancelButton has the "pressed" key set
			if (g_object_steal_data (G_OBJECT (cancelButton), "pressed"))
				return 0;
		#else
			XmScaleSetValue (scale, floor (progress * 1000.0));
			XmUpdateDisplay (dia);
		#endif
	}
	return 1;
}

#if gtk
static void progress_dia_close (void *wid) {
	g_object_set_data (G_OBJECT (* (Widget *) wid), "pressed", (gpointer) 1);
}
static void progress_cancel_btn_press (void *wid, GuiButtonEvent event) {
	(void) event;
	g_object_set_data (G_OBJECT (* (Widget *) wid), "pressed", (gpointer) 1);
}
#endif

static void _Melder_dia_init (Widget *dia, Widget *scale, Widget *label1, Widget *label2, Widget *cancelButton) {
	*dia = GuiDialog_create (Melder_topShell, 200, 100, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Work in progress",
		#if gtk
			progress_dia_close, cancelButton,
		#else
			NULL, NULL,
		#endif
		0);
	
	Widget form = *dia;
	Widget buttons = GuiDialog_getButtonArea (*dia);
	
	*label1 = GuiLabel_createShown (form, 3, 403, 0, Gui_AUTOMATIC, L"label1", 0);
	*label2 = GuiLabel_createShown (form, 3, 403, 30, Gui_AUTOMATIC, L"label2", 0);
	
	#if gtk
		*scale = gtk_progress_bar_new ();
		gtk_container_add (GTK_CONTAINER (form), *scale);
		GuiObject_show (*scale);
	#elif motif
		*scale = XmCreateScale (*dia, "scale", NULL, 0);
		XtVaSetValues (*scale, XmNy, 70, XmNwidth, 400, XmNminimum, 0, XmNmaximum, 1000,
			XmNorientation, XmHORIZONTAL,
			#if ! defined (macintosh)
				XmNscaleHeight, 20,
			#endif
			NULL);
		GuiObject_show (*scale);
	#endif
	
	#if ! defined (macintosh)
		*cancelButton = GuiButton_createShown (buttons, 0, 400, 170, Gui_AUTOMATIC,
			L"Interrupt",
			#if gtk
				progress_cancel_btn_press, cancelButton,
			#else
				NULL, NULL,
			#endif
			0);
	#endif
}
#endif

static int _Melder_progress (double progress, const wchar_t *message) {
	(void) progress;
	#ifndef CONSOLE_APPLICATION
	if (! Melder_batch && theProgressDepth >= 0 && Melder_debug != 14) {
		static clock_t lastTime;
		static Widget dia = NULL, scale = NULL, label1 = NULL, label2 = NULL, cancelButton = NULL;
		clock_t now = clock ();
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   /* This time step must be much longer than the null-event waiting time. */
		{
			if (dia == NULL) {
				_Melder_dia_init (& dia, & scale, & label1, & label2, & cancelButton);
			}
			bool interruption = waitWhileProgress (progress, message, dia, scale, label1, label2, cancelButton);
			if (! interruption) Melder_error1 (L"Interrupted!");
			lastTime = now;
			return interruption;
		}
	}
	#endif
	return 1;   /* Proceed. */
}

static MelderString theProgressBuffer = { 0 };

int Melder_progress1 (double progress, const wchar_t *s1) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append1 (& theProgressBuffer, s1);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress2 (double progress, const wchar_t *s1, const wchar_t *s2) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append2 (& theProgressBuffer, s1, s2);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress3 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append3 (& theProgressBuffer, s1, s2, s3);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress4 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append4 (& theProgressBuffer, s1, s2, s3, s4);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress5 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append5 (& theProgressBuffer, s1, s2, s3, s4, s5);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress6 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append6 (& theProgressBuffer, s1, s2, s3, s4, s5, s6);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress7 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append7 (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress8 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append8 (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8);
	return _Melder_progress (progress, theProgressBuffer.string);
}
int Melder_progress9 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append9 (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	return _Melder_progress (progress, theProgressBuffer.string);
}

static void * _Melder_monitor (double progress, const wchar_t *message) {
	(void) progress;
	#ifndef CONSOLE_APPLICATION
	if (! Melder_batch && theProgressDepth >= 0) {
		static clock_t lastTime;
		static Widget dia = NULL, scale = NULL, label1 = NULL, label2 = NULL, cancelButton = NULL, drawingArea = NULL;
		clock_t now = clock ();
		static Any graphics = NULL;
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   /* This time step must be much longer than the null-event waiting time. */
		{
			if (dia == NULL) {
				_Melder_dia_init (& dia, & scale, & label1, & label2, & cancelButton);
				drawingArea = GuiDrawingArea_createShown (dia, 0, 400, 230, 430, NULL, NULL, NULL, NULL, NULL, 0);
				GuiObject_show (dia);
				graphics = Graphics_create_xmdrawingarea (drawingArea);
			}
			bool interruption = waitWhileProgress (progress, message, dia, scale, label1, label2, cancelButton);
			if (! interruption) Melder_error1 (L"Interrupted!");
			lastTime = now;
			if (progress == 0.0)
				return graphics;
			if (! interruption) return NULL;
		}
	}
	#endif
	return progress <= 0.0 ? NULL /* No Graphics. */ : & progress /* Any non-NULL pointer. */;
}

void * Melder_monitor1 (double progress, const wchar_t *s1) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append1 (& theProgressBuffer, s1);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor2 (double progress, const wchar_t *s1, const wchar_t *s2) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append2 (& theProgressBuffer, s1, s2);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor3 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append3 (& theProgressBuffer, s1, s2, s3);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor4 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append4 (& theProgressBuffer, s1, s2, s3, s4);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor5 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append5 (& theProgressBuffer, s1, s2, s3, s4, s5);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor6 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append6 (& theProgressBuffer, s1, s2, s3, s4, s5, s6);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor7 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append7 (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor8 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append8 (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor9 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append9 (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	return _Melder_monitor (progress, theProgressBuffer.string);
}

/********** NUMBER AND STRING COMPARISONS **********/

int Melder_numberMatchesCriterion (double value, int which_kMelder_number, double criterion) {
	return
		(which_kMelder_number == kMelder_number_EQUAL_TO && value == criterion) ||
		(which_kMelder_number == kMelder_number_NOT_EQUAL_TO && value != criterion) ||
		(which_kMelder_number == kMelder_number_LESS_THAN && value < criterion) ||
		(which_kMelder_number == kMelder_number_LESS_THAN_OR_EQUAL_TO && value <= criterion) ||
		(which_kMelder_number == kMelder_number_GREATER_THAN && value > criterion) ||
		(which_kMelder_number == kMelder_number_GREATER_THAN_OR_EQUAL_TO && value >= criterion);
}

int Melder_stringMatchesCriterion (const wchar_t *value, int which_kMelder_string, const wchar_t *criterion) {
	if (value == NULL) {
		value = L"";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (criterion == NULL) {
		criterion = L"";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (which_kMelder_string <= kMelder_string_NOT_EQUAL_TO) {
		int matchPositiveCriterion = wcsequ (value, criterion);
		return (which_kMelder_string == kMelder_string_EQUAL_TO) == matchPositiveCriterion;
	}
	if (which_kMelder_string <= kMelder_string_DOES_NOT_CONTAIN) {
		int matchPositiveCriterion = wcsstr (value, criterion) != NULL;
		return (which_kMelder_string == kMelder_string_CONTAINS) == matchPositiveCriterion;
	}
	if (which_kMelder_string <= kMelder_string_DOES_NOT_START_WITH) {
		int matchPositiveCriterion = wcsnequ (value, criterion, wcslen (criterion));
		return (which_kMelder_string == kMelder_string_STARTS_WITH) == matchPositiveCriterion;
	}
	if (which_kMelder_string <= kMelder_string_DOES_NOT_END_WITH) {
		int criterionLength = wcslen (criterion), valueLength = wcslen (value);
		int matchPositiveCriterion = criterionLength <= valueLength && wcsequ (value + valueLength - criterionLength, criterion);
		return (which_kMelder_string == kMelder_string_ENDS_WITH) == matchPositiveCriterion;
	}
	if (which_kMelder_string == kMelder_string_MATCH_REGEXP) {
		char *place = NULL, *errorMessage;
		regexp *compiled_regexp = CompileRE (Melder_peekWcsToUtf8 (criterion), & errorMessage, 0);
		if (compiled_regexp == NULL) return FALSE;
		if (ExecRE (compiled_regexp, NULL, Melder_peekWcsToUtf8 (value), NULL, 0, '\0', '\0', NULL, NULL, NULL))
			place = compiled_regexp -> startp [0];
		free (compiled_regexp);
		return place != NULL;
	}
	return 0;   /* Should not occur. */
}

void Melder_help (const wchar_t *query) {
	theMelder. help (query);
}

void Melder_search (void) {
	theMelder. search ();
}

/********** WARNING **********/

static int theWarningDepth = 0;
void Melder_warningOff (void) { theWarningDepth --; }
void Melder_warningOn (void) { theWarningDepth ++; }

static MelderString theWarningBuffer = { 0 };

void Melder_warning1 (const wchar_t *s1) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append1 (& theWarningBuffer, s1);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning2 (const wchar_t *s1, const wchar_t *s2) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append2 (& theWarningBuffer, s1, s2);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append3 (& theWarningBuffer, s1, s2, s3);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append4 (& theWarningBuffer, s1, s2, s3, s4);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append5 (& theWarningBuffer, s1, s2, s3, s4, s5);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append6 (& theWarningBuffer, s1, s2, s3, s4, s5, s6);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append7 (& theWarningBuffer, s1, s2, s3, s4, s5, s6, s7);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append8 (& theWarningBuffer, s1, s2, s3, s4, s5, s6, s7, s8);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append9 (& theWarningBuffer, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	theMelder. warning (theWarningBuffer.string);
}

void Melder_beep (void) {
	#ifdef macintosh
		SysBeep (0);
	#else
		fprintf (stderr, "\a");
	#endif
}

/*********** FATAL **********/

int Melder_fatal (const char *format, ...) {
	const char *lead = strstr (format, "Praat cannot start up") ? "" :
		"Praat will crash. Notify the author (paul.boersma@uva.nl) with the following information:\n";
	va_list arg;
	va_start (arg, format);
	strcpy (Melder_buffer1, lead);
	vsprintf (Melder_buffer1 + strlen (lead), format, arg);
	theMelder. fatal (Melder_peekUtf8ToWcs (Melder_buffer1));
	va_end (arg);
	abort ();
	return 0;   /* Make some compilers happy, some unhappy. */
}

int _Melder_assert (const char *condition, const char *fileName, int lineNumber) {
	return Melder_fatal ("Assertion failed in file \"%s\" at line %d:\n   %s\n",
		fileName, lineNumber, condition);
}

#ifndef CONSOLE_APPLICATION

#if defined (macintosh)
static void mac_message (int macAlertType, wchar_t *messageW) {
	DialogRef dialog;
	static UniChar messageU [4000];
	int messageLength = wcslen (messageW);
	int j = 0;
	for (int i = 0; i < messageLength && j <= 4000 - 2; i ++) {
		uint32_t kar = messageW [i];
		if (kar <= 0xFFFF) {
			messageU [j ++] = kar;
		} else if (kar <= 0x10FFFF) {
			kar -= 0x10000;
			messageU [j ++] = 0xD800 | (kar >> 10);
			messageU [j ++] = 0xDC00 | (kar & 0x3FF);
		}
	}
	CFStringRef messageCF = CFStringCreateWithCharacters (NULL, messageU, j);
	CreateStandardAlert (macAlertType, messageCF, NULL, NULL, & dialog);
	CFRelease (messageCF);
	RunStandardAlert (dialog, NULL, NULL);
}
#endif

static void gui_fatal (wchar_t *message) {
	#if gtk
		Widget dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", Melder_peekWcsToUtf8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	#elif defined (macintosh)
		mac_message (kAlertStopAlert, message);
		SysError (11);
	#elif defined (_WIN32)
		MessageBox (NULL, message, L"Fatal error", MB_OK);
	#endif
}

static void gui_error (wchar_t *message) {
	#if gtk
		Widget dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", Melder_peekWcsToUtf8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	#elif defined (macintosh)
		mac_message (kAlertStopAlert, message);
		XmUpdateDisplay (0);
	#elif defined (_WIN32)
		MessageBox (NULL, message, L"Message", MB_OK);
	#endif
}

static void gui_warning (wchar_t *message) {
	#if gtk
		Widget dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", Melder_peekWcsToUtf8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	#elif defined (macintosh)
		mac_message (kAlertNoteAlert, message);
		XmUpdateDisplay (0);
	#elif defined (_WIN32)
		MessageBox (NULL, message, L"Warning", MB_OK);
	#endif
}

void MelderGui_create (void *appContext, void *parent) {
	extern void gui_information (wchar_t *);   // BUG: no prototype
	Melder_appContext = appContext;
	Melder_topShell = (Widget) parent;
	Melder_setInformationProc (gui_information);
	Melder_setFatalProc (gui_fatal);
	Melder_setErrorProc (gui_error);
	Melder_setWarningProc (gui_warning);
}
#endif

int Melder_publish (void *anything) {
	return theMelder. publish (anything);
}

int Melder_record (double duration) {
	return theMelder. record (duration);
}

int Melder_recordFromFile (MelderFile fs) {
	return theMelder. recordFromFile (fs);
}

void Melder_play (void) {
	theMelder. play ();
}

void Melder_playReverse (void) {
	theMelder. playReverse ();
}

int Melder_publishPlayed (void) {
	return theMelder. publishPlayed ();
}

/********** Procedures to override message methods (e.g., to enforce interactive behaviour). **********/

void Melder_setHelpProc (void (*help) (const wchar_t *query))
	{ theMelder. help = help ? help : defaultHelp; }

void Melder_setSearchProc (void (*search) (void))
	{ theMelder. search = search ? search : defaultSearch; }

void Melder_setWarningProc (void (*warning) (wchar_t *))
	{ theMelder. warning = warning ? warning : defaultWarning; }

void Melder_setFatalProc (void (*fatal) (wchar_t *))
	{ theMelder. fatal = fatal ? fatal : defaultFatal; }

void Melder_setPublishProc (int (*publish) (void *))
	{ theMelder. publish = publish ? publish : defaultPublish; }

void Melder_setRecordProc (int (*record) (double))
	{ theMelder. record = record ? record : defaultRecord; }

void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile))
	{ theMelder. recordFromFile = recordFromFile ? recordFromFile : defaultRecordFromFile; }

void Melder_setPlayProc (void (*play) (void))
	{ theMelder. play = play ? play : defaultPlay; }

void Melder_setPlayReverseProc (void (*playReverse) (void))
	{ theMelder. playReverse = playReverse ? playReverse : defaultPlayReverse; }

void Melder_setPublishPlayedProc (int (*publishPlayed) (void))
	{ theMelder. publishPlayed = publishPlayed ? publishPlayed : defaultPublishPlayed; }

/* End of file melder.c */
