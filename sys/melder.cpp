/* melder.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
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
        #if useCarbon
            #include <Carbon/Carbon.h>
        #else
            #include <AudioToolbox/AudioToolbox.h>
        #endif
		#include "macport_off.h"
	#endif
	#include "Gui.h"
#endif

#include "enums_getText.h"
#include "melder_enums.h"
#include "enums_getValue.h"
#include "melder_enums.h"

/********** Exported variables. **********/

bool Melder_batch;   // don't we have a GUI?- Set once at application start-up
bool Melder_backgrounding;   // are we running a script?- Set and unset dynamically
char Melder_buffer1 [30001], Melder_buffer2 [30001];
unsigned long Melder_systemVersion;

#ifndef CONSOLE_APPLICATION
	GuiWindow Melder_topShell;
#endif

static void defaultHelp (const wchar_t *query) {
	Melder_error_ ("Don't know how to find help on \"", query, "\".");
	Melder_flushError (NULL);
}

static void defaultSearch (void) {
	Melder_flushError ("Do not know how to search.");
}

static void defaultWarning (const wchar_t *message) {
	Melder_writeToConsole (L"Warning: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (L"\n", true);
}

static void defaultFatal (const wchar_t *message) {
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
	void (*warning) (const wchar_t *message);
	void (*fatal) (const wchar_t *message);
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
static bool waitWhileProgress (double progress, const wchar_t *message, GuiDialog dia, GuiProgressBar scale, GuiLabel label1, GuiLabel label2, GuiButton cancelButton) {
	#if gtk
		// Wait for all pending events to be processed. If anybody knows how to inspect GTK's
		// event queue for specific events, dump the code here, please.
		// Until then, the button click attaches a g_object data key named "pressed" to the cancelButton
		// which this function reads out in order to tell whether interruption has occurred
		while (gtk_events_pending ()) {
			trace ("event pending");
			gtk_main_iteration ();
		}
	#elif defined (macintosh)
		#if useCarbon
			EventRecord event;
			while (GetNextEvent (mDownMask, & event)) {
				WindowPtr macWindow;
				int part = FindWindow (event. where, & macWindow);
				if (part == inContent) {
					if (GetWindowKind (macWindow) == userKind) {
						SetPortWindowPort (macWindow);
						GlobalToLocal (& event. where);  //CGContextConvertPointToUserSpace
						ControlPartCode controlPart;
						ControlHandle macControl = FindControlUnderMouse (event. where, macWindow, & controlPart);
						if (macControl) {
							GuiObject control = (GuiObject) GetControlReference (macControl);
							if (control == cancelButton -> d_widget) {
								FlushEvents (everyEvent, 0);
								XtUnmanageChild (dia -> d_widget);
								return false;   // don't continue
							} else {
								break;
							}
						} else {
							XtDispatchEvent ((XEvent *) & event);
						}
					} else {
						XtDispatchEvent ((XEvent *) & event);
					}
				} else {
					XtDispatchEvent ((XEvent *) & event);
				}
			}
			do { XtNextEvent ((XEvent *) & event); XtDispatchEvent ((XEvent *) & event); } while (event.what);
		#else
			NSEvent *nsEvent = [NSApp
				nextEventMatchingMask: NSAnyEventMask
				untilDate: [NSDate distantPast]
				inMode: NSDefaultRunLoopMode
				dequeue: YES
				];
			if (nsEvent) {
				NSUInteger nsEventType = [nsEvent type];
				if (nsEventType == NSKeyDown) NSBeep ();
				[[nsEvent window]  sendEvent: nsEvent];
			}
		#endif
	#elif defined (_WIN32)
		XEvent event;
		while (PeekMessage (& event, 0, 0, 0, PM_REMOVE)) {
			if (event. message == WM_KEYDOWN) {
				/*
				 * Ignore all key-down messages, except Escape.
				 */
				if (LOWORD (event. wParam) == VK_ESCAPE) {
					XtUnmanageChild (dia -> d_widget);
					return false;   // don't continue
				}
			} else if (event. message == WM_LBUTTONDOWN) {
				/*
				 * Ignore all mouse-down messages, except click in Interrupt button.
				 */
				GuiObject me = (GuiObject) GetWindowLongPtr (event. hwnd, GWLP_USERDATA);
				if (me == cancelButton -> d_widget) {
					XtUnmanageChild (dia -> d_widget);
					return false;   // don't continue
				}
			} else if (event. message != WM_SYSKEYDOWN) {
				/*
				 * Process paint messages etc.
				 */
				DispatchMessage (& event);
			}
		}
	#endif
	if (progress >= 1.0) {
		dia -> f_hide ();
	} else {
		if (progress <= 0.0) progress = 0.0;
		dia -> f_show ();   // TODO: prevent raising to the front
		const wchar_t *newline = wcschr (message, '\n');
		if (newline != NULL) {
			static MelderString buffer = { 0 };
			MelderString_copy (& buffer, message);
			buffer.string [newline - message] = '\0';
			label1 -> f_setString (buffer.string);
			buffer.string [newline - message] = '\n';
			label2 -> f_setString (buffer.string + (newline - message) + 1);
		} else {
			label1 -> f_setString (message);
			label2 -> f_setString (L"");
		}
		#if gtk
			trace ("update the progress bar");
			scale -> f_setValue (progress);
			while (gtk_events_pending ()) {
				trace ("event pending");
				gtk_main_iteration ();
			}
			trace ("check whether the cancel button has the \"pressed\" key set");
			if (g_object_steal_data (G_OBJECT (cancelButton -> d_widget), "pressed")) {
				trace ("the cancel button has been pressed");
				return false;   // don't continue
			}
		#elif cocoa
			scale -> f_setValue (progress);
			//[scale -> d_cocoaProgressBar   displayIfNeeded];
		#elif motif
			scale -> f_setValue (progress);
			XmUpdateDisplay (dia -> d_widget);
		#endif
	}
	trace ("continue");
	return true;
}

#if gtk
static void progress_dia_close (void *cancelButton) {
	g_object_set_data (G_OBJECT ((* (GuiButton *) cancelButton) -> d_widget), "pressed", (gpointer) 1);
}
static void progress_cancel_btn_press (void *cancelButton, GuiButtonEvent event) {
	(void) event;
	g_object_set_data (G_OBJECT ((* (GuiButton *) cancelButton) -> d_widget), "pressed", (gpointer) 1);
}
#endif

static void _Melder_dia_init (GuiDialog *dia, GuiProgressBar *scale, GuiLabel *label1, GuiLabel *label2, GuiButton *cancelButton, bool hasMonitor) {
	trace ("creating the dialog");
	*dia = GuiDialog_create (Melder_topShell, 200, 100, 400, hasMonitor ? 430 : 200, L"Work in progress",
		#if gtk
			progress_dia_close, cancelButton,
		#else
			NULL, NULL,
		#endif
		0);

	trace ("creating the labels");
	*label1 = GuiLabel_createShown (*dia, 3, 403, 0, Gui_LABEL_HEIGHT, L"label1", 0);
	*label2 = GuiLabel_createShown (*dia, 3, 403, 30, 30 + Gui_LABEL_HEIGHT, L"label2", 0);

	trace ("creating the scale");
	*scale = GuiProgressBar_createShown (*dia, 3, -3, 70, 110, 0);

	trace ("creating the cancel button");
	*cancelButton = GuiButton_createShown (*dia, 0, 400, 170, 170 + Gui_PUSHBUTTON_HEIGHT,
		L"Interrupt",
		#if gtk
			progress_cancel_btn_press, cancelButton,
		#else
			NULL, NULL,
		#endif
		0);
	trace ("end");
}
#endif

static void _Melder_progress (double progress, const wchar_t *message) {
	(void) progress;
	#ifndef CONSOLE_APPLICATION
	if (! Melder_batch && theProgressDepth >= 0 && Melder_debug != 14) {
		static clock_t lastTime;
		static GuiDialog dia = NULL;
		static GuiProgressBar scale = NULL;
		static GuiButton cancelButton = NULL;
		static GuiLabel label1 = NULL, label2 = NULL;
		clock_t now = clock ();
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   // this time step must be much longer than the null-event waiting time
		{
			if (dia == NULL)
				_Melder_dia_init (& dia, & scale, & label1, & label2, & cancelButton, false);
			if (! waitWhileProgress (progress, message, dia, scale, label1, label2, cancelButton))
				Melder_throw ("Interrupted!");
			lastTime = now;
		}
	}
	#endif
}

static MelderString theProgressBuffer = { 0 };

void Melder_progress (double progress, const wchar_t *s1) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	_Melder_progress (progress, theProgressBuffer.string);
}

static void * _Melder_monitor (double progress, const wchar_t *message) {
	(void) progress;
	#ifndef CONSOLE_APPLICATION
	if (! Melder_batch && theProgressDepth >= 0) {
		static clock_t lastTime;
		static GuiDialog dia = NULL;
		static GuiProgressBar scale = NULL;
		static GuiDrawingArea drawingArea = NULL;
		static GuiButton cancelButton = NULL;
		static GuiLabel label1 = NULL, label2 = NULL;
		clock_t now = clock ();
		static Any graphics = NULL;
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   // this time step must be much longer than the null-event waiting time
		{
			if (dia == NULL) {
				_Melder_dia_init (& dia, & scale, & label1, & label2, & cancelButton, true);
				drawingArea = GuiDrawingArea_createShown (dia, 0, 400, 230, 430, NULL, NULL, NULL, NULL, NULL, 0);
				dia -> f_show ();
				graphics = Graphics_create_xmdrawingarea (drawingArea);
			}
			if (! waitWhileProgress (progress, message, dia, scale, label1, label2, cancelButton))
				Melder_throw ("Interrupted!");
			lastTime = now;
			if (progress == 0.0)
				return graphics;
		}
	}
	#endif
	return progress <= 0.0 ? NULL /* no Graphics */ : & progress /* any non-NULL pointer */;
}

void * Melder_monitor (double progress, const wchar_t *s1) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_empty (& theProgressBuffer);
	MelderString_append (& theProgressBuffer, s1, s2, s3, s4, s5, s6, s7, s8, s9);
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
		wchar_t *place = NULL;
		const wchar_t *errorMessage;
		regexp *compiled_regexp = CompileRE ((regularExp_CHAR *) criterion, & errorMessage, 0);
		if (compiled_regexp == NULL) return FALSE;   // BUG: what about removing errorMessage?
		if (ExecRE (compiled_regexp, NULL, (regularExp_CHAR *) value, NULL, 0, '\0', '\0', NULL, NULL, NULL))
			place = (wchar_t *) compiled_regexp -> startp [0];
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

void Melder_warning (const MelderArg& arg1) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5) {
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8),
		arg9.type == 1 ? arg9.argW : Melder_peekUtf8ToWcs (arg9.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8),
		arg9.type == 1 ? arg9.argW : Melder_peekUtf8ToWcs (arg9.arg8));
	MelderString_append (& theWarningBuffer,
		arg10.type == 1 ? arg10.argW : Melder_peekUtf8ToWcs (arg10.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10,
	const MelderArg& arg11)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8),
		arg9.type == 1 ? arg9.argW : Melder_peekUtf8ToWcs (arg9.arg8));
	MelderString_append (& theWarningBuffer,
		arg10.type == 1 ? arg10.argW : Melder_peekUtf8ToWcs (arg10.arg8),
		arg11.type == 1 ? arg11.argW : Melder_peekUtf8ToWcs (arg11.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10,
	const MelderArg& arg11, const MelderArg& arg12, const MelderArg& arg13)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8),
		arg9.type == 1 ? arg9.argW : Melder_peekUtf8ToWcs (arg9.arg8));
	MelderString_append (& theWarningBuffer,
		arg10.type == 1 ? arg10.argW : Melder_peekUtf8ToWcs (arg10.arg8),
		arg11.type == 1 ? arg11.argW : Melder_peekUtf8ToWcs (arg11.arg8),
		arg12.type == 1 ? arg12.argW : Melder_peekUtf8ToWcs (arg12.arg8),
		arg13.type == 1 ? arg13.argW : Melder_peekUtf8ToWcs (arg13.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10,
	const MelderArg& arg11, const MelderArg& arg12, const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8),
		arg9.type == 1 ? arg9.argW : Melder_peekUtf8ToWcs (arg9.arg8));
	MelderString_append (& theWarningBuffer,
		arg10.type == 1 ? arg10.argW : Melder_peekUtf8ToWcs (arg10.arg8),
		arg11.type == 1 ? arg11.argW : Melder_peekUtf8ToWcs (arg11.arg8),
		arg12.type == 1 ? arg12.argW : Melder_peekUtf8ToWcs (arg12.arg8),
		arg13.type == 1 ? arg13.argW : Melder_peekUtf8ToWcs (arg13.arg8),
		arg14.type == 1 ? arg14.argW : Melder_peekUtf8ToWcs (arg14.arg8),
		arg15.type == 1 ? arg15.argW : Melder_peekUtf8ToWcs (arg15.arg8));
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4, const MelderArg& arg5,
	const MelderArg& arg6, const MelderArg& arg7, const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10,
	const MelderArg& arg11, const MelderArg& arg12, const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15,
	const MelderArg& arg16, const MelderArg& arg17, const MelderArg& arg18, const MelderArg& arg19, const MelderArg& arg20)
{
	if (theWarningDepth < 0) return;
	MelderString_empty (& theWarningBuffer);
	MelderString_append (& theWarningBuffer,
		arg1.type == 1 ? arg1.argW : Melder_peekUtf8ToWcs (arg1.arg8),
		arg2.type == 1 ? arg2.argW : Melder_peekUtf8ToWcs (arg2.arg8),
		arg3.type == 1 ? arg3.argW : Melder_peekUtf8ToWcs (arg3.arg8),
		arg4.type == 1 ? arg4.argW : Melder_peekUtf8ToWcs (arg4.arg8),
		arg5.type == 1 ? arg5.argW : Melder_peekUtf8ToWcs (arg5.arg8),
		arg6.type == 1 ? arg6.argW : Melder_peekUtf8ToWcs (arg6.arg8),
		arg7.type == 1 ? arg7.argW : Melder_peekUtf8ToWcs (arg7.arg8),
		arg8.type == 1 ? arg8.argW : Melder_peekUtf8ToWcs (arg8.arg8),
		arg9.type == 1 ? arg9.argW : Melder_peekUtf8ToWcs (arg9.arg8));
	MelderString_append (& theWarningBuffer,
		arg10.type == 1 ? arg10.argW : Melder_peekUtf8ToWcs (arg10.arg8),
		arg11.type == 1 ? arg11.argW : Melder_peekUtf8ToWcs (arg11.arg8),
		arg12.type == 1 ? arg12.argW : Melder_peekUtf8ToWcs (arg12.arg8),
		arg13.type == 1 ? arg13.argW : Melder_peekUtf8ToWcs (arg13.arg8),
		arg14.type == 1 ? arg14.argW : Melder_peekUtf8ToWcs (arg14.arg8),
		arg15.type == 1 ? arg15.argW : Melder_peekUtf8ToWcs (arg15.arg8),
		arg16.type == 1 ? arg16.argW : Melder_peekUtf8ToWcs (arg16.arg8),
		arg17.type == 1 ? arg17.argW : Melder_peekUtf8ToWcs (arg17.arg8),
		arg18.type == 1 ? arg18.argW : Melder_peekUtf8ToWcs (arg18.arg8));
	MelderString_append (& theWarningBuffer,
		arg19.type == 1 ? arg19.argW : Melder_peekUtf8ToWcs (arg19.arg8),
		arg20.type == 1 ? arg20.argW : Melder_peekUtf8ToWcs (arg20.arg8));
	theMelder. warning (theWarningBuffer.string);
}

void Melder_beep (void) {
	#ifdef macintosh
		#if useCarbon
			SysBeep (0);
		#else
            AudioServicesPlayAlertSound (kSystemSoundID_UserPreferredAlert);
		#endif
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
	trace ("FATAL: %s", Melder_buffer1);
	theMelder. fatal (Melder_peekUtf8ToWcs (Melder_buffer1));
	va_end (arg);
	abort ();
	return 0;   /* Make some compilers happy, some unhappy. */
}

int Melder_assert_ (const char *condition, const char *fileName, int lineNumber) {
	return Melder_fatal ("Assertion failed in file \"%s\" at line %d:\n   %s\n",
		fileName, lineNumber, condition);
}

#ifndef CONSOLE_APPLICATION

#if defined (macintosh)
static void mac_message (NSAlertStyle macAlertType, const wchar_t *messageW) {
	static unichar messageU [4000];
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

	/*
	 * Split up the message between header (will appear in bold) and rest.
	 * The split is done at the first line break, except if the first line ends in a colon,
	 * in which case the split is done at the second line break.
	 */
	UniChar *lineBreak = & messageU [0];
	for (; *lineBreak != '\0'; lineBreak ++) {
		if (*lineBreak == '\n') {
			break;
		}
	}
	if (*lineBreak == '\n' && lineBreak - messageU > 0 && lineBreak [-1] == ':') {
		for (lineBreak ++; *lineBreak != '\0'; lineBreak ++) {
			if (*lineBreak == '\n') {
				break;
			}
		}
	}
	#if useCarbon
        DialogRef dialog;
		CFStringRef messageCF = CFStringCreateWithCharacters (NULL, messageU, j);
		CreateStandardAlert (macAlertType, messageCF, NULL, NULL, & dialog);
		CFRelease (messageCF);
		RunStandardAlert (dialog, NULL, NULL);
	#elif fhgfdghdggfkdsgfXXX
		NSString *header = NULL, *rest = NULL;
		header = [[NSString alloc] initWithCharacters: messageU   length: lineBreak - messageU];   // note: init can change the object pointer!
		if (lineBreak - messageU != j) {
			rest = [[NSString alloc] initWithCharacters: lineBreak + 1   length: j - 1 - (lineBreak - messageU)];
		}
		NSRunAlertPanel (header, rest, NULL, NULL, NULL);
		[header release];
		if (rest) [rest release];
	#else
		/*
		 * Create an alert dialog with an icon that is appropriate for the level.
		 */
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setAlertStyle: macAlertType];
		/*
		 * Add the header in bold.
		 */
		NSString *header = [[NSString alloc] initWithCharacters: messageU   length: lineBreak - messageU];   // note: init can change the object pointer!
		[alert setMessageText: header];
		[header release];
		/*
		 * Add the rest of the message in small type.
		 */
		if (lineBreak - messageU != j) {
			NSString *rest = [[NSString alloc] initWithCharacters: lineBreak + 1   length: j - 1 - (lineBreak - messageU)];
			[alert setInformativeText: rest];
			[rest release];
		}
		/*
		 * Display the alert dialog and synchronously wait for the user to click OK.
		 */
		[alert runModal];
		[alert release];
	#endif
}
#endif

#define theMessageFund_SIZE  100000
static char * theMessageFund = NULL;

static void gui_fatal (const wchar_t *message) {
	free (theMessageFund);
	#if gtk
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", Melder_peekWcsToUtf8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		#if useCarbon
			mac_message (NSCriticalAlertStyle, message);
			SysError (11);
		#else
			mac_message (NSCriticalAlertStyle, message);
			SysError (11);
		#endif
	#elif defined (_WIN32)
		MessageBox (NULL, message, L"Fatal error", MB_OK | MB_TOPMOST | MB_ICONSTOP);
	#endif
}

static void gui_error (const wchar_t *message) {
	bool memoryIsLow = wcsstr (message, L"Out of memory");
	if (memoryIsLow) {
		free (theMessageFund);
	}
	#if gtk
		trace ("create dialog");
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", Melder_peekWcsToUtf8 (message));
		trace ("run dialog");
		gtk_dialog_run (GTK_DIALOG (dialog));
		trace ("destroy dialog");
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		#if useCarbon
			mac_message (NSWarningAlertStyle, message);
			XmUpdateDisplay (0);
		#else
			mac_message (NSWarningAlertStyle, message);
		#endif
	#elif defined (_WIN32)
		MessageBox (NULL, message, L"Message", MB_OK | MB_TOPMOST | MB_ICONWARNING);   // or (HWND) XtWindow ((GuiObject) Melder_topShell)
	#endif
	if (memoryIsLow) {
		theMessageFund = (char *) malloc (theMessageFund_SIZE);
		if (theMessageFund == NULL) {
			#if gtk
				GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
				gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (GTK_WIDGET (dialog));
			#elif defined (macintosh)
				#if useCarbon
					mac_message (NSCriticalAlertStyle, L"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
					XmUpdateDisplay (0);
				#else
					mac_message (NSCriticalAlertStyle, L"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
				#endif
			#elif defined (_WIN32)
				MessageBox (NULL, L"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.", L"Message", MB_OK);
			#endif
		}
	}
}

static void gui_warning (const wchar_t *message) {
	#if gtk
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", Melder_peekWcsToUtf8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		#if useCarbon
			mac_message (NSInformationalAlertStyle, message);
			XmUpdateDisplay (0);
		#else
			mac_message (NSInformationalAlertStyle, message);
		#endif
	#elif defined (_WIN32)
		MessageBox (NULL, message, L"Warning", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
	#endif
}

void gui_information (const wchar_t *);   // BUG: no prototype
void MelderGui_create (void *parent) {
	theMessageFund = (char *) malloc (theMessageFund_SIZE);
	assert (theMessageFund != NULL);
	Melder_topShell = (GuiWindow) parent;
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

int Melder_recordFromFile (MelderFile file) {
	return theMelder. recordFromFile (file);
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

void Melder_setWarningProc (void (*warning) (const wchar_t *))
	{ theMelder. warning = warning ? warning : defaultWarning; }

void Melder_setFatalProc (void (*fatal) (const wchar_t *))
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

/* End of file melder.cpp */
