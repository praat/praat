/* melder.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

#include "MelderThread.h"

#include "enums_getText.h"
#include "melder_enums.h"
#include "enums_getValue.h"
#include "melder_enums.h"

/********** Exported variables. **********/

bool Melder_batch;   // don't we have a GUI?- Set once at application start-up
bool Melder_backgrounding;   // are we running a script?- Set and unset dynamically
bool Melder_asynchronous;
int32 Melder_systemVersion;

GuiWindow Melder_topShell;

static void defaultHelp (const char32 *query) {
	Melder_flushError (U"Don't know how to find help on \"", query, U"\".");
}

static void defaultSearch () {
	Melder_flushError (U"Do not know how to search.");
}

static void defaultWarning (const char32 *message) {
	Melder_writeToConsole (U"Warning: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (U"\n", true);
}

static void defaultFatal (const char32 *message) {
	Melder_writeToConsole (U"Fatal error: ", true);
	Melder_writeToConsole (message, true);
	Melder_writeToConsole (U"\n", true);
}

static int defaultPublish (void *anything) {
	(void) anything;
	return 0;   // nothing published
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
	void (*help) (const char32 *query);
	void (*search) ();
	void (*warning) (const char32 *message);
	void (*fatal) (const char32 *message);
	int (*publish) (void *anything);
	int (*record) (double duration);
	int (*recordFromFile) (MelderFile file);
	void (*play) ();
	void (*playReverse) ();
	int (*publishPlayed) ();
}
	theMelder = {
		defaultHelp, defaultSearch,
		defaultWarning, defaultFatal,
		defaultPublish,
		defaultRecord, defaultRecordFromFile, defaultPlay, defaultPlayReverse, defaultPublishPlayed
	};

/********** PROGRESS **********/

static int theProgressDepth = 0;
static bool theProgressCancelled = false;
void Melder_progressOff () { theProgressDepth --; }
void Melder_progressOn () { theProgressDepth ++; }

static bool waitWhileProgress (double progress, const char32 *message, GuiDialog dia, GuiProgressBar scale, GuiLabel label1, GuiLabel label2, GuiButton cancelButton) {
	#if gtk
		// Wait for all pending events to be processed. If anybody knows how to inspect GTK's
		// event queue for specific events, dump the code here, please.
		// Until then, the button click attaches a g_object data key named "pressed" to the cancelButton
		// which this function reads out in order to tell whether interruption has occurred
		while (gtk_events_pending ()) {
			trace (U"event pending");
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
		GuiThing_hide (dia);
	} else {
		if (progress <= 0.0) progress = 0.0;
		GuiThing_show (dia);   // TODO: prevent raising to the front
		const char32 *newline = str32chr (message, U'\n');
		if (newline) {
			static MelderString buffer { 0 };
			MelderString_copy (& buffer, message);
			buffer.string [newline - message] = U'\0';
			GuiLabel_setText (label1, buffer.string);
			buffer.string [newline - message] = U'\n';
			GuiLabel_setText (label2, buffer.string + (newline - message) + 1);
		} else {
			GuiLabel_setText (label1, message);
			GuiLabel_setText (label2, U"");
		}
		#if gtk
			trace (U"update the progress bar");
			GuiProgressBar_setValue (scale, progress);
			while (gtk_events_pending ()) {
				trace (U"event pending");
				gtk_main_iteration ();
			}
			trace (U"check whether the cancel button has the \"pressed\" key set");
			if (g_object_steal_data (G_OBJECT (cancelButton -> d_widget), "pressed")) {
				trace (U"the cancel button has been pressed");
				return false;   // don't continue
			}
		#elif cocoa
			GuiProgressBar_setValue (scale, progress);
			//[scale -> d_cocoaProgressBar   displayIfNeeded];
			if (theProgressCancelled) {
				theProgressCancelled = false;
				return false;
			}
		#elif motif
			GuiProgressBar_setValue (scale, progress);
			XmUpdateDisplay (dia -> d_widget);
		#endif
	}
	trace (U"continue");
	return true;
}

static GuiButton theProgressCancelButton = nullptr;

#if gtk || macintosh
static void progress_dia_close (Thing /* boss */) {
	theProgressCancelled = true;
	#if gtk
		g_object_set_data (G_OBJECT (theProgressCancelButton -> d_widget), "pressed", (gpointer) 1);
	#endif
}
static void progress_cancel_btn_press (void*, GuiButtonEvent /* event */) {
	theProgressCancelled = true;
	#if gtk
		g_object_set_data (G_OBJECT (theProgressCancelButton -> d_widget), "pressed", (gpointer) 1);
	#endif
}
#endif

static void _Melder_dia_init (GuiDialog *dia, GuiProgressBar *scale, GuiLabel *label1, GuiLabel *label2, GuiButton *cancelButton, bool hasMonitor) {
	trace (U"creating the dialog");
	*dia = GuiDialog_create (Melder_topShell, 200, 100, 400, hasMonitor ? 430 : 200, U"Work in progress",
		#if gtk || macintosh
			progress_dia_close, nullptr,
		#else
			nullptr, nullptr,
		#endif
		0);

	trace (U"creating the labels");
	*label1 = GuiLabel_createShown (*dia, 3, 403, 0, Gui_LABEL_HEIGHT, U"label1", 0);
	*label2 = GuiLabel_createShown (*dia, 3, 403, 30, 30 + Gui_LABEL_HEIGHT, U"label2", 0);

	trace (U"creating the scale");
	*scale = GuiProgressBar_createShown (*dia, 3, -3, 70, 110, 0);

	trace (U"creating the cancel button");
	*cancelButton = GuiButton_createShown (*dia, 0, 400, 170, 170 + Gui_PUSHBUTTON_HEIGHT,
		U"Interrupt",
		#if gtk
			progress_cancel_btn_press, nullptr,
		#elif macintosh
			progress_cancel_btn_press, nullptr,
		#else
			nullptr, nullptr,
		#endif
		0);
	trace (U"end");
}

static void _Melder_progress (double progress, const char32 *message) {
	if (! Melder_batch && theProgressDepth >= 0 && Melder_debug != 14) {
		static clock_t lastTime;
		static GuiDialog dia = nullptr;
		static GuiProgressBar scale = nullptr;
		static GuiLabel label1 = nullptr, label2 = nullptr;
		clock_t now = clock ();
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   // this time step must be much longer than the null-event waiting time
		{
			if (! dia)
				_Melder_dia_init (& dia, & scale, & label1, & label2, & theProgressCancelButton, false);
			if (! waitWhileProgress (progress, message, dia, scale, label1, label2, theProgressCancelButton))
				Melder_throw (U"Interrupted!");
			lastTime = now;
		}
	}
}

static MelderString theProgressBuffer = { 0 };

void Melder_progress (double progress) {
	_Melder_progress (progress, U"");
}
void Melder_progress (double progress, Melder_1_ARG) {
	MelderString_copy (& theProgressBuffer, Melder_1_ARG_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_2_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_2_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_3_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_3_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_4_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_4_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_5_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_5_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_6_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_6_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_7_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_7_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_8_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_8_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_9_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_9_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_10_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_10_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_11_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_11_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_13_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_13_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_15_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_15_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}
void Melder_progress (double progress, Melder_19_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_19_ARGS_CALL);
	_Melder_progress (progress, theProgressBuffer.string);
}

static void * _Melder_monitor (double progress, const char32 *message) {
	if (! Melder_batch && theProgressDepth >= 0) {
		static clock_t lastTime;
		static GuiDialog dia = nullptr;
		static GuiProgressBar scale = nullptr;
		static GuiDrawingArea drawingArea = nullptr;
		static GuiButton cancelButton = nullptr;
		static GuiLabel label1 = nullptr, label2 = nullptr;
		clock_t now = clock ();
		static Any graphics = nullptr;
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   // this time step must be much longer than the null-event waiting time
		{
			if (! dia) {
				_Melder_dia_init (& dia, & scale, & label1, & label2, & cancelButton, true);
				drawingArea = GuiDrawingArea_createShown (dia, 0, 400, 230, 430, nullptr, nullptr, nullptr, nullptr, nullptr, 0);
				GuiThing_show (dia);
				graphics = Graphics_create_xmdrawingarea (drawingArea);
			}
			if (! waitWhileProgress (progress, message, dia, scale, label1, label2, cancelButton))
				Melder_throw (U"Interrupted!");
			lastTime = now;
			if (progress == 0.0)
				return graphics;
		}
	}
	return progress <= 0.0 ? nullptr /* no Graphics */ : (void *) -1 /* any non-null pointer */;
}

void * Melder_monitor (double progress) {
	return _Melder_monitor (progress, U"");
}
void * Melder_monitor (double progress, Melder_1_ARG) {
	MelderString_copy (& theProgressBuffer, Melder_1_ARG_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_2_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_2_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_3_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_3_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_4_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_4_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_5_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_5_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_6_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_6_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_7_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_7_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_8_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_8_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_9_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_9_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_10_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_10_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_11_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_11_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_13_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_13_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_15_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_15_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}
void * Melder_monitor (double progress, Melder_19_ARGS) {
	MelderString_copy (& theProgressBuffer, Melder_19_ARGS_CALL);
	return _Melder_monitor (progress, theProgressBuffer.string);
}

/********** NUMBER AND STRING COMPARISONS **********/

bool Melder_numberMatchesCriterion (double value, int which_kMelder_number, double criterion) {
	return
		(which_kMelder_number == kMelder_number_EQUAL_TO && value == criterion) ||
		(which_kMelder_number == kMelder_number_NOT_EQUAL_TO && value != criterion) ||
		(which_kMelder_number == kMelder_number_LESS_THAN && value < criterion) ||
		(which_kMelder_number == kMelder_number_LESS_THAN_OR_EQUAL_TO && value <= criterion) ||
		(which_kMelder_number == kMelder_number_GREATER_THAN && value > criterion) ||
		(which_kMelder_number == kMelder_number_GREATER_THAN_OR_EQUAL_TO && value >= criterion);
}

bool Melder_stringMatchesCriterion (const char32 *value, int which_kMelder_string, const char32 *criterion) {
	if (! value) {
		value = U"";   // regard null strings as empty strings, as is usual in Praat
	}
	if (! criterion) {
		criterion = U"";   // regard null strings as empty strings, as is usual in Praat
	}
	if (which_kMelder_string <= kMelder_string_NOT_EQUAL_TO) {
		bool matchPositiveCriterion = str32equ (value, criterion);
		return ( which_kMelder_string == kMelder_string_EQUAL_TO ) == matchPositiveCriterion;
	}
	if (which_kMelder_string <= kMelder_string_DOES_NOT_CONTAIN) {
		bool matchPositiveCriterion = !! str32str (value, criterion);
		return ( which_kMelder_string == kMelder_string_CONTAINS ) == matchPositiveCriterion;
	}
	if (which_kMelder_string <= kMelder_string_DOES_NOT_START_WITH) {
		bool matchPositiveCriterion = str32nequ (value, criterion, str32len (criterion));
		return ( which_kMelder_string == kMelder_string_STARTS_WITH ) == matchPositiveCriterion;
	}
	if (which_kMelder_string <= kMelder_string_DOES_NOT_END_WITH) {
		int criterionLength = str32len (criterion), valueLength = str32len (value);
		bool matchPositiveCriterion = ( criterionLength <= valueLength && str32equ (value + valueLength - criterionLength, criterion) );
		return (which_kMelder_string == kMelder_string_ENDS_WITH) == matchPositiveCriterion;
	}
	if (which_kMelder_string == kMelder_string_MATCH_REGEXP) {
		char32 *place = nullptr;
		regexp *compiled_regexp = CompileRE_throwable (criterion, 0);
		if (ExecRE (compiled_regexp, nullptr, value, nullptr, 0, '\0', '\0', nullptr, nullptr, nullptr))
			place = compiled_regexp -> startp [0];
		free (compiled_regexp);
		return !! place;
	}
	return false;   // should not occur
}

void Melder_help (const char32 *query) {
	theMelder. help (query);
}

void Melder_search () {
	theMelder. search ();
}

/********** WARNING **********/

static int theWarningDepth = 0;
void Melder_warningOff () { theWarningDepth --; }
void Melder_warningOn () { theWarningDepth ++; }

static MelderString theWarningBuffer = { 0 };

void Melder_warning (Melder_1_ARG) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_1_ARG_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_2_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_2_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_3_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_3_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_4_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_4_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_5_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_5_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_6_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_6_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_7_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_7_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_8_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_8_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_9_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_9_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_10_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_10_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_11_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_11_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_13_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_13_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_15_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_15_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}
void Melder_warning (Melder_19_ARGS) {
	if (theWarningDepth < 0) return;
	MelderString_copy (& theWarningBuffer, Melder_19_ARGS_CALL);
	theMelder. warning (theWarningBuffer.string);
}

void Melder_beep () {
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

MelderThread_MUTEX (theMelder_fatal_mutex);

void Melder_message_init () {
	static bool inited = false;
	if (! inited) { MelderThread_MUTEX_INIT (theMelder_fatal_mutex); inited = true; }
}

constexpr int Melder_FATAL_BUFFER_SIZE { 2000 };
static char32 theFatalBuffer [Melder_FATAL_BUFFER_SIZE];
static const char32 *theCrashMessage { U"Praat will crash. Notify the author (paul.boersma@uva.nl) with the following information:\n" };

int Melder_fatal (Melder_1_ARG) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1 = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_2_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1 = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2 = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_3_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1 = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2 = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3 = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_4_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg  ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg  ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg  ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg  ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	if (length + length4  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s4);  length += length4;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_5_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	if (length + length4  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s4);  length += length4;  }
	if (length + length5  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s5);  length += length5;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_6_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	if (length + length4  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s4);  length += length4;  }
	if (length + length5  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s5);  length += length5;  }
	if (length + length6  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s6);  length += length6;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_7_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	str32cpy (theFatalBuffer, theCrashMessage);
	int64 length = str32len (theFatalBuffer);
	if (length + length1  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s1);  length += length1;  }
	if (length + length2  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s2);  length += length2;  }
	if (length + length3  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s3);  length += length3;  }
	if (length + length4  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s4);  length += length4;  }
	if (length + length5  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s5);  length += length5;  }
	if (length + length6  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s6);  length += length6;  }
	if (length + length7  < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s7);  length += length7;  }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_8_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
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
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_9_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	const char32 *s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
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
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_10_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	const char32 *s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
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
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_11_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	const char32 *s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";   int64 length11 = str32len (s11);
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
	if (length + length11 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s11); length += length11; }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_13_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	const char32 *s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";   int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";   int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";   int64 length13 = str32len (s13);
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
	if (length + length11 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s11); length += length11; }
	if (length + length12 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s12); length += length12; }
	if (length + length13 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s13); length += length13; }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_15_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	const char32 *s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";   int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";   int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";   int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";   int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";   int64 length15 = str32len (s15);
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
	if (length + length11 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s11); length += length11; }
	if (length + length12 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s12); length += length12; }
	if (length + length13 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s13); length += length13; }
	if (length + length14 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s14); length += length14; }
	if (length + length15 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s15); length += length15; }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}
int Melder_fatal (Melder_19_ARGS) {
	MelderThread_LOCK (theMelder_fatal_mutex);
	const char32 *s1  = arg1. _arg ? arg1. _arg : U"";   int64 length1  = str32len (s1);
	const char32 *s2  = arg2. _arg ? arg2. _arg : U"";   int64 length2  = str32len (s2);
	const char32 *s3  = arg3. _arg ? arg3. _arg : U"";   int64 length3  = str32len (s3);
	const char32 *s4  = arg4. _arg ? arg4. _arg : U"";   int64 length4  = str32len (s4);
	const char32 *s5  = arg5. _arg ? arg5. _arg : U"";   int64 length5  = str32len (s5);
	const char32 *s6  = arg6. _arg ? arg6. _arg : U"";   int64 length6  = str32len (s6);
	const char32 *s7  = arg7. _arg ? arg7. _arg : U"";   int64 length7  = str32len (s7);
	const char32 *s8  = arg8. _arg ? arg8. _arg : U"";   int64 length8  = str32len (s8);
	const char32 *s9  = arg9. _arg ? arg9. _arg : U"";   int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";   int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";   int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";   int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";   int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";   int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";   int64 length15 = str32len (s15);
	const char32 *s16 = arg16._arg ? arg16._arg : U"";   int64 length16 = str32len (s16);
	const char32 *s17 = arg17._arg ? arg17._arg : U"";   int64 length17 = str32len (s17);
	const char32 *s18 = arg18._arg ? arg18._arg : U"";   int64 length18 = str32len (s18);
	const char32 *s19 = arg19._arg ? arg19._arg : U"";   int64 length19 = str32len (s19);
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
	if (length + length11 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s11); length += length11; }
	if (length + length12 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s12); length += length12; }
	if (length + length13 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s13); length += length13; }
	if (length + length14 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s14); length += length14; }
	if (length + length15 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s15); length += length15; }
	if (length + length16 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s16); length += length16; }
	if (length + length17 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s17); length += length17; }
	if (length + length18 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s18); length += length18; }
	if (length + length19 < Melder_FATAL_BUFFER_SIZE) { str32cpy (theFatalBuffer + length, s19); length += length19; }
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);
	abort ();
	return 0;
}

void Melder_assert_ (const char *fileName, int lineNumber, const char *condition) {
	/*
	 * This function tries to make sure that it allocates no heap memory.
	 * Hence, character conversion is done inline rather than with Melder_peek8to32(),
	 * and Melder_integer() is also avoided.
	 */
	MelderThread_LOCK (theMelder_fatal_mutex);
	static char32 fileNameBuffer [1000], conditionBuffer [1000], lineNumberBuffer [40];
	Melder_8to32_inline (fileName, fileNameBuffer, kMelder_textInputEncoding_UTF8);
	Melder_8to32_inline (condition, conditionBuffer, kMelder_textInputEncoding_UTF8);
	static char lineNumberBuffer8 [40];
	sprintf (lineNumberBuffer8, "%d", lineNumber);
	Melder_8to32_inline (lineNumberBuffer8, lineNumberBuffer, kMelder_textInputEncoding_UTF8);
	str32cpy (theFatalBuffer, theCrashMessage);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"Assertion failed in file \"");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), fileNameBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"\" at line ");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), lineNumberBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U":\n   ");
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), conditionBuffer);
	str32cpy (theFatalBuffer + str32len (theFatalBuffer), U"\n");
	trace (U"FATAL: ", theFatalBuffer);
	theMelder. fatal (theFatalBuffer);   // ...but this call will use heap memory...
	abort ();
}

#if defined (macintosh)
static void mac_message (NSAlertStyle macAlertType, const char32 *message32) {
	static unichar messageU [4000];
	int messageLength = str32len (message32);
	int j = 0;
	for (int i = 0; i < messageLength && j <= 4000 - 3; i ++) {
		char32 kar = message32 [i];
		if (kar <= 0x00FFFF) {
			messageU [j ++] = kar;
		} else if (kar <= 0x10FFFF) {
			kar -= 0x010000;
			messageU [j ++] = 0x00D800 | (kar >> 10);
			messageU [j ++] = 0x00DC00 | (kar & 0x0003FF);
		}
	}
	messageU [j] = '\0';   // append null byte because we are going to search this string

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
		CFStringRef messageCF = CFStringCreateWithCharacters (nullptr, messageU, j);
		CreateStandardAlert (macAlertType, messageCF, nullptr, nullptr, & dialog);
		CFRelease (messageCF);
		RunStandardAlert (dialog, nullptr, nullptr);
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
		if (header) {   // make this very safe, because we can be at error time or at fatal time
			[alert setMessageText: header];
			[header release];
		}
		/*
		 * Add the rest of the message in small type.
		 */
		if (lineBreak - messageU < j) {
			NSString *rest = [[NSString alloc] initWithCharacters: lineBreak + 1   length: j - 1 - (lineBreak - messageU)];
			if (rest) {   // make this very safe, because we can be at error time or at fatal time
				[alert setInformativeText: rest];
				[rest release];
			}
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
static char * theMessageFund = nullptr;

static void gui_fatal (const char32 *message) {
	free (theMessageFund);
	#if gtk
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", Melder_peek32to8 (message));
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
		MessageBox (nullptr, Melder_peek32toW (message), L"Fatal error", MB_OK | MB_TOPMOST | MB_ICONSTOP);
	#endif
}

static void gui_error (const char32 *message) {
	bool memoryIsLow = str32str (message, U"Out of memory");
	if (memoryIsLow) {
		free (theMessageFund);
	}
	#if gtk
		trace (U"create dialog");
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", Melder_peek32to8 (message));
		trace (U"run dialog");
		gtk_dialog_run (GTK_DIALOG (dialog));
		trace (U"destroy dialog");
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		#if useCarbon
			mac_message (NSWarningAlertStyle, message);
			XmUpdateDisplay (0);
		#else
			mac_message (NSWarningAlertStyle, message);
		#endif
	#elif defined (_WIN32)
		MessageBox (nullptr, Melder_peek32toW (message), L"Message", MB_OK | MB_TOPMOST | MB_ICONWARNING);   // or (HWND) XtWindow ((GuiObject) Melder_topShell)
	#endif
	if (memoryIsLow) {
		theMessageFund = (char *) malloc (theMessageFund_SIZE);
		if (! theMessageFund) {
			#if gtk
				GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
				gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (GTK_WIDGET (dialog));
			#elif defined (macintosh)
				#if useCarbon
					mac_message (NSCriticalAlertStyle, U"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
					XmUpdateDisplay (0);
				#else
					mac_message (NSCriticalAlertStyle, U"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
				#endif
			#elif defined (_WIN32)
				MessageBox (nullptr, L"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.", L"Message", MB_OK);
			#endif
		}
	}
}

static void gui_warning (const char32 *message) {
	#if gtk
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", Melder_peek32to8 (message));
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
		MessageBox (nullptr, Melder_peek32toW (message), L"Warning", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
	#endif
}

void gui_information (const char32 *);   // BUG: no prototype
void MelderGui_create (void *parent) {
	theMessageFund = (char *) malloc (theMessageFund_SIZE);
	assert (theMessageFund);
	Melder_topShell = (GuiWindow) parent;
	Melder_setInformationProc (gui_information);
	Melder_setFatalProc (gui_fatal);
	Melder_setErrorProc (gui_error);
	Melder_setWarningProc (gui_warning);
}

int Melder_publish (void *anything) {
	return theMelder. publish (anything);
}

int Melder_record (double duration) {
	return theMelder. record (duration);
}

int Melder_recordFromFile (MelderFile file) {
	return theMelder. recordFromFile (file);
}

void Melder_play () {
	theMelder. play ();
}

void Melder_playReverse () {
	theMelder. playReverse ();
}

int Melder_publishPlayed () {
	return theMelder. publishPlayed ();
}

/********** Procedures to override message methods (e.g., to enforce interactive behaviour). **********/

void Melder_setHelpProc (void (*help) (const char32 *query))
	{ theMelder. help = help ? help : defaultHelp; }

void Melder_setSearchProc (void (*search) (void))
	{ theMelder. search = search ? search : defaultSearch; }

void Melder_setWarningProc (void (*warning) (const char32 *))
	{ theMelder. warning = warning ? warning : defaultWarning; }

void Melder_setFatalProc (void (*fatal) (const char32 *))
	{ theMelder. fatal = fatal ? fatal : defaultFatal; }

void Melder_setPublishProc (int (*publish) (void *))
	{ theMelder. publish = publish ? publish : defaultPublish; }

void Melder_setRecordProc (int (*record) (double))
	{ theMelder. record = record ? record : defaultRecord; }

void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile))
	{ theMelder. recordFromFile = recordFromFile ? recordFromFile : defaultRecordFromFile; }

void Melder_setPlayProc (void (*play) ())
	{ theMelder. play = play ? play : defaultPlay; }

void Melder_setPlayReverseProc (void (*playReverse) ())
	{ theMelder. playReverse = playReverse ? playReverse : defaultPlayReverse; }

void Melder_setPublishPlayedProc (int (*publishPlayed) ())
	{ theMelder. publishPlayed = publishPlayed ? publishPlayed : defaultPublishPlayed; }

/* End of file melder.cpp */
