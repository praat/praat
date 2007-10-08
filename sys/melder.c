/* melder.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 */

#include <math.h>
#include <time.h>
#include <ctype.h>
#include "melder.h"
#include "longchar.h"
#include "NUM.h"
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

/********** Exported variables. **********/

int Melder_batch;   /* Don't we have a GUI?- Set once at application start-up. */
int Melder_backgrounding;   /* Are we running a script?- Set and unset dynamically. */
char Melder_buffer1 [30001], Melder_buffer2 [30001];
unsigned long Melder_systemVersion;

#ifndef CONSOLE_APPLICATION
	void *Melder_appContext;   /* XtAppContext* */
	void *Melder_topShell;   /* Widget */
#endif

static int defaultPause (wchar_t *message) {
	int key;
	fprintf (stderr, "Pause: %s\nPress 'q' to stop, or any other key to continue.\n", Melder_peekWcsToUtf8 (message));
	key = getc (stdin);
	return key != 'q' && key != 'Q';
}

static void defaultHelp (const wchar_t *query) {
	Melder_error3 (L"Do not know how to find help on \"", query, L"\".");
	Melder_flushError (NULL);
}

static void defaultSearch (void) {
	Melder_flushError ("Do not know how to search.");
}

static void defaultWarning (wchar_t *message) {
	fprintf (stderr, "Warning: %s\n", Melder_peekWcsToUtf8 (message));
}

static void defaultFatal (wchar_t *message) {
	fprintf (stderr, "Fatal error: %s\n", Melder_peekWcsToUtf8 (message));
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
	int (*pause) (wchar_t *message);
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
		defaultPause, defaultHelp, defaultSearch,
		defaultWarning, defaultFatal,
		defaultPublish,
		defaultRecord, defaultRecordFromFile, defaultPlay, defaultPlayReverse, defaultPublishPlayed
	};

/********** CASUAL **********/

void Melder_casual (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	#if defined (_WIN32) && ! defined (CONSOLE_APPLICATION)
	if (! Melder_batch) {
		MessageBox (NULL, Melder_peekUtf8ToWcs (Melder_buffer2), L"Casual info", MB_OK);
	} else
	#endif
	fprintf (stderr, "%s\n", Melder_buffer2);
	va_end (arg);
}

/********** STOPWATCH **********/

double Melder_stopwatch (void) {
	static clock_t lastTime;
	clock_t now = clock ();
	double timeElapsed = lastTime == 0 ? -1.0 : (now - lastTime) / (double) CLOCKS_PER_SEC;
	lastTime = now;
	return timeElapsed;
}

/********** PROGRESS **********/

static int theProgressDepth = 0;
void Melder_progressOff (void) { theProgressDepth --; }
void Melder_progressOn (void) { theProgressDepth ++; }

#ifndef CONSOLE_APPLICATION
static int waitWhileProgress (double progress, char *message, Widget dia, Widget scale, Widget label, Widget cancelButton) {
	#if defined (macintosh)
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
		XtUnmanageChild (dia);
	} else {
		if (progress <= 0.0) progress = 0.0;
		XtManageChild (dia);
		XtVaSetValues (label, motif_argXmString (XmNlabelString, message), NULL);
		XmScaleSetValue (scale, floor (progress * 1000.0));
		XmUpdateDisplay (dia);
	}
	return 1;
}
#endif

int Melder_progress (double progress, const char *format, ...) {
	(void) progress;
	(void) format;
	#ifndef CONSOLE_APPLICATION
	if (! Melder_batch && theProgressDepth >= 0 && Melder_debug != 14) {
		static clock_t lastTime;
		static Widget dia = NULL, scale = NULL, label = NULL, cancelButton = NULL;
		clock_t now = clock ();
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   /* This time step must be much longer than the null-event waiting time. */
		{
			int interruption;
			va_list arg;
			va_start (arg, format);
			if (format) {
				vsprintf (Melder_buffer1, format, arg);
				Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
			} else {
				Melder_buffer2 [0] = '\0';
			}
			va_end (arg);
			if (dia == NULL) {
				dia = XmCreateFormDialog (Melder_topShell, "melderProgress", NULL, 0);
				XtVaSetValues (XtParent (dia), XmNx, 200, XmNy, 100,
					XmNtitle, "Work in progress",
					XmNdeleteResponse, XmUNMAP,
					NULL);
				XtVaSetValues (dia, XmNautoUnmanage, True, NULL);
				label = XmCreateLabel (dia, "label", NULL, 0);
				XtVaSetValues (label, XmNwidth, 400, NULL);
				XtManageChild (label);
				scale = XmCreateScale (dia, "scale", NULL, 0);
				XtVaSetValues (scale, XmNy, 40, XmNwidth, 400, XmNminimum, 0, XmNmaximum, 1000,
					XmNorientation, XmHORIZONTAL,
					#if ! defined (macintosh)
						XmNscaleHeight, 20,
					#endif
					NULL);
				XtManageChild (scale);
				#if ! defined (macintosh)
					cancelButton = XmCreatePushButton (dia, "Interrupt", NULL, 0);
					XtVaSetValues (cancelButton, XmNy, 140, XmNwidth, 400, NULL);
					XtManageChild (cancelButton);
				#endif
			}
			interruption = waitWhileProgress (progress, Melder_buffer2, dia, scale, label, cancelButton);
			if (! interruption) Melder_error ("Interrupted!");
			lastTime = now;
			return interruption;
		}
	}
	#endif
	return 1;   /* Proceed. */
}

void * Melder_monitor (double progress, const char *format, ...) {
	(void) progress;
	(void) format;
	#ifndef CONSOLE_APPLICATION
	if (! Melder_batch && theProgressDepth >= 0) {
		static clock_t lastTime;
		static Widget dia = NULL, scale = NULL, label = NULL, cancelButton = NULL, drawingArea = NULL;
		clock_t now = clock ();
		static Any graphics = NULL;
		if (progress <= 0.0 || progress >= 1.0 ||
			now - lastTime > CLOCKS_PER_SEC / 4)   /* This time step must be much longer than the null-event waiting time. */
		{
			int interruption;
			va_list arg;
			va_start (arg, format);
			if (format) {
				vsprintf (Melder_buffer1, format, arg);
				Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
			} else {
				Melder_buffer2 [0] = '\0';
			}
			va_end (arg);
			if (dia == NULL) {
				dia = XmCreateFormDialog (Melder_topShell, "melderMonitor", NULL, 0);
				XtVaSetValues (XtParent (dia), XmNx, 200, XmNy, 100,
					XmNtitle, "Work in progress",
					XmNdeleteResponse, XmUNMAP,
					NULL);
				XtVaSetValues (dia, XmNautoUnmanage, True, NULL);
				label = XmCreateLabel (dia, "label", NULL, 0);
				XtVaSetValues (label, XmNwidth, 400, NULL);
				XtManageChild (label);
				scale = XmCreateScale (dia, "scale", NULL, 0);
				XtVaSetValues (scale, XmNy, 40, XmNwidth, 400, XmNminimum, 0, XmNmaximum, 1000,
					XmNorientation, XmHORIZONTAL,
					#if ! defined (macintosh) && ! defined (_WIN32)
						XmNscaleHeight, 20,
					#endif
					NULL);
				XtManageChild (scale);
				#if ! defined (macintosh)
					cancelButton = XmCreatePushButton (dia, "Interrupt", NULL, 0);
					XtVaSetValues (cancelButton, XmNy, 140, XmNwidth, 400, NULL);
					XtManageChild (cancelButton);
				#endif
				drawingArea = XmCreateDrawingArea (dia, "drawingArea", NULL, 0);
				XtVaSetValues (drawingArea, XmNy, 200, XmNwidth, 400, XmNheight, 200,
					XmNmarginWidth, 10, XmNmarginHeight, 10, NULL);
				XtManageChild (drawingArea);
				XtManageChild (dia);
				graphics = Graphics_create_xmdrawingarea (drawingArea);
			}
			interruption = waitWhileProgress (progress, Melder_buffer2, dia, scale, label, cancelButton);
			if (! interruption) Melder_error ("Interrupted!");
			lastTime = now;
			if (progress == 0.0)
				return graphics;
			if (! interruption) return NULL;
		}
	}
	#endif
	return progress <= 0.0 ? NULL /* No Graphics. */ : & progress /* Any non-NULL pointer. */;
}

/********** PAUSE **********/

int Melder_pause (const char *format, ...) {
	int interruption;
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	} else {
		Melder_buffer2 [0] = '\0';
	}
	interruption = theMelder. pause (Melder_peekUtf8ToWcs (Melder_buffer1));
	va_end (arg);
	return interruption;
}

/********** NUMBER AND STRING COMPARISONS **********/

const wchar_t * Melder_NUMBER_text_adjective (int which_Melder_NUMBER) {
	static const wchar_t *strings [1+Melder_NUMBER_max] = { L"",
		L"equal to", L"not equal to",
		L"less than", L"less than or equal to",
		L"greater than", L"greater than or equal to"
	};
	return strings [which_Melder_NUMBER < 0 || which_Melder_NUMBER > Melder_NUMBER_max ? 0 : which_Melder_NUMBER];
}

int Melder_numberMatchesCriterion (double value, int which_Melder_NUMBER, double criterion) {
	return
		(which_Melder_NUMBER == Melder_NUMBER_EQUAL_TO && value == criterion) ||
		(which_Melder_NUMBER == Melder_NUMBER_NOT_EQUAL_TO && value != criterion) ||
		(which_Melder_NUMBER == Melder_NUMBER_LESS_THAN && value < criterion) ||
		(which_Melder_NUMBER == Melder_NUMBER_LESS_THAN_OR_EQUAL_TO && value <= criterion) ||
		(which_Melder_NUMBER == Melder_NUMBER_GREATER_THAN && value > criterion) ||
		(which_Melder_NUMBER == Melder_NUMBER_GREATER_THAN_OR_EQUAL_TO && value >= criterion);
}

const wchar_t * Melder_STRING_text_finiteVerb (int which_Melder_STRING) {
	static const wchar_t *strings [1+Melder_STRING_max] = { L"",
		L"is equal to", L"is not equal to",
		L"contains", L"does not contain",
		L"starts with", L"does not start with",
		L"ends with", L"does not end with",
		L"matches (regex)"
	};
	return strings [which_Melder_STRING < 0 || which_Melder_STRING > Melder_STRING_max ? 0 : which_Melder_STRING];
}

int Melder_stringMatchesCriterion (const wchar_t *value, int which_Melder_STRING, const wchar_t *criterion) {
	if (value == NULL) {
		value = L"";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (criterion == NULL) {
		criterion = L"";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (which_Melder_STRING <= Melder_STRING_NOT_EQUAL_TO) {
		int matchPositiveCriterion = wcsequ (value, criterion);
		return (which_Melder_STRING == Melder_STRING_EQUAL_TO) == matchPositiveCriterion;
	}
	if (which_Melder_STRING <= Melder_STRING_DOES_NOT_CONTAIN) {
		int matchPositiveCriterion = wcsstr (value, criterion) != NULL;
		return (which_Melder_STRING == Melder_STRING_CONTAINS) == matchPositiveCriterion;
	}
	if (which_Melder_STRING <= Melder_STRING_DOES_NOT_START_WITH) {
		int matchPositiveCriterion = wcsnequ (value, criterion, wcslen (criterion));
		return (which_Melder_STRING == Melder_STRING_STARTS_WITH) == matchPositiveCriterion;
	}
	if (which_Melder_STRING <= Melder_STRING_DOES_NOT_END_WITH) {
		int criterionLength = wcslen (criterion), valueLength = wcslen (value);
		int matchPositiveCriterion = criterionLength <= valueLength && wcsequ (value + valueLength - criterionLength, criterion);
		return (which_Melder_STRING == Melder_STRING_ENDS_WITH) == matchPositiveCriterion;
	}
	if (which_Melder_STRING == Melder_STRING_MATCH_REGEXP) {
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

void Melder_warning (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (theWarningDepth >= 0) {
		vsprintf (Melder_buffer1, format, arg);
		Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
		theMelder. warning (Melder_peekUtf8ToWcs (Melder_buffer2));
	}
	va_end (arg);
}

void Melder_beep (void) {
	#ifdef macintosh
		SysBeep (0);
	#else
		fprintf (stderr, "\a");
	#endif
}

/*********** ERROR **********/

int Melder_fatal (const char *format, ...) {
	const char *lead = strstr (format, "Praat cannot start up") ? "" :
		"Praat will crash. Notify the author (paul.boersma@uva.nl) with the following information:\n";
	va_list arg;
	va_start (arg, format);
	strcpy (Melder_buffer1, lead);
	vsprintf (Melder_buffer1 + strlen (lead), format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	theMelder. fatal (Melder_peekUtf8ToWcs (Melder_buffer2));
	va_end (arg);
	abort ();
	return 0;   /* Make some compilers happy, some unhappy. */
}

int _Melder_assert (const char *condition, const char *fileName, int lineNumber) {
	return Melder_fatal ("Assertion failed in file \"%s\" at line %d:\n   %s\n",
		fileName, lineNumber, condition);
}

#ifndef CONSOLE_APPLICATION
static Widget makeMessage (unsigned char dialogType, const char *resourceName, const char *title) {
	Widget dialog = XmCreateMessageDialog (Melder_topShell, MOTIF_CONST_CHAR_ARG (resourceName), NULL, 0);
	XtVaSetValues (dialog,
		XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		XmNdialogType, dialogType,
		XmNautoUnmanage, True,
		NULL);
	XtVaSetValues (XtParent (dialog), XmNtitle, title, XmNdeleteResponse, XmUNMAP, NULL);
	XtUnmanageChild (XmMessageBoxGetChild (dialog, XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild (XmMessageBoxGetChild (dialog, XmDIALOG_HELP_BUTTON));
	return dialog;
}

static int pause_continued, pause_stopped;
MOTIF_CALLBACK (pause_continue_cb) pause_continued = 1; MOTIF_CALLBACK_END
MOTIF_CALLBACK (pause_stop_cb) pause_stopped = 1; MOTIF_CALLBACK_END
static int motif_pause (wchar_t *message) {
	static Widget dia = NULL, continueButton = NULL, stopButton = NULL, rc, buttons, text;
	if (dia == NULL) {
		dia = XmCreateFormDialog (Melder_topShell, "melderPause", NULL, 0);
		XtVaSetValues (XtParent (dia),
			XmNtitle, "Pause",
			XmNdeleteResponse, XmDO_NOTHING,
			NULL);
		XtVaSetValues (dia, XmNautoUnmanage, True, NULL);
		rc = XmCreateRowColumn (dia, "rc", NULL, 0);
		text = XmCreateLabel (rc, "text", NULL, 0);
		XtVaSetValues (text, XmNwidth, 400, NULL);
		XtManageChild (text);
		buttons = XmCreateRowColumn (rc, "rc", NULL, 0);
		XtVaSetValues (buttons, XmNorientation, XmHORIZONTAL, NULL);
		continueButton = XmCreatePushButton (buttons, "Continue", NULL, 0);
		XtVaSetValues (continueButton, XmNx, 10, XmNwidth, 300, NULL);
		XtAddCallback (continueButton, XmNactivateCallback, pause_continue_cb, (XtPointer) dia);
		XtManageChild (continueButton);
		stopButton = XmCreatePushButton (buttons, "Stop", NULL, 0);
		XtVaSetValues (stopButton, XmNx, 320, XmNwidth, 60, NULL);
		XtAddCallback (stopButton, XmNactivateCallback, pause_stop_cb, (XtPointer) dia);
		XtManageChild (stopButton);
		XtManageChild (buttons);
		XtManageChild (rc);
	}
	if (! message) message = L"";
	XtVaSetValues (text, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (message)), NULL);
	XtManageChild (dia);
	pause_continued = pause_stopped = FALSE;
	do {
		XEvent event;
		XtAppNextEvent (Melder_appContext, & event);
		XtDispatchEvent (& event);
	} while (! pause_continued && ! pause_stopped);
	XtUnmanageChild (dia);
	return pause_continued;
}

static void motif_warning (wchar_t *message) {
#ifdef _WIN32
	MessageBox (NULL, message, L"Warning", MB_OK);
#else
	static Widget dia = NULL;
	if (dia == NULL)
		dia = makeMessage (XmDIALOG_WARNING, "warning", "Warning");
	XtVaSetValues (dia, motif_argXmString (XmNmessageString, Melder_peekWcsToUtf8 (message)), NULL);
	XtManageChild (dia);
	XMapRaised (XtDisplay (XtParent (dia)), XtWindow (XtParent (dia)));   /* Because the delete response is UNMAP. */
#endif
}

#ifdef macintosh
static void motif_fatal (char *message)
{
	Str255 pmessage;
	int length, i;
	message [255] = '\0';
	length = strlen (message);
	pmessage [0] = length;
	strcpy ((char *) pmessage + 1, message);
	for (i = 1; i <= length; i ++) if (pmessage [i] == '\n') pmessage [i] = '\r';
	StandardAlert (kAlertStopAlert, pmessage, NULL, NULL, NULL);
	SysError (11);
}
static void motif_error (wchar_t *messageW) {
	DialogRef dialog;
	static UniChar messageU [2000+1];
	int messageLength = wcslen (messageW);
	for (int i = 0; i < messageLength; i ++) {
		messageU [i] = messageW [i];   // BUG: should convert to UTF16
	}
	CFStringRef messageCF = CFStringCreateWithCharacters (NULL, messageU, messageLength);
	CreateStandardAlert (kAlertStopAlert, messageCF, NULL, NULL, & dialog);
	CFRelease (messageCF);
	RunStandardAlert (dialog, NULL, NULL);
	XmUpdateDisplay (0);
}
#elif defined (_WIN32)
static void motif_fatal (wchar_t *message) {
	MessageBox (NULL, message, L"Fatal error", MB_OK);
}
static void motif_error (wchar_t *messageW) {
	MessageBoxW (NULL, messageW, L"Message", MB_OK);
}
#else
static void motif_error (wchar_t *messageW) {
	static Widget dia = NULL;
	static char messageA [2000+1];
	int messageLength = wcslen (messageW);
	if (dia == NULL)
		dia = makeMessage (XmDIALOG_ERROR, "error", "Message");
	for (int i = 0; i <= messageLength; i ++) {
		messageA [i] = messageW [i];
	}
	XtVaSetValues (dia, motif_argXmString (XmNmessageString, messageA), NULL);
	XtManageChild (dia);
	XMapRaised (XtDisplay (XtParent (dia)), XtWindow (XtParent (dia)));   /* Because the delete response is UNMAP. */
}
#endif

void MelderMotif_create (void *appContext, void *parent) {
	extern void motif_information (wchar_t *);
	Melder_appContext = appContext;
	Melder_topShell = (Widget) parent;
	Melder_setInformationProc (motif_information);
	Melder_setWarningProc (motif_warning);
	Melder_setErrorProc (motif_error);
	#if defined (macintosh) || defined (_WIN32)
		Melder_setFatalProc (motif_fatal);
	#endif
	Melder_setPauseProc (motif_pause);
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

void Melder_setPauseProc (int (*pause) (wchar_t *))
	{ theMelder. pause = pause ? pause : defaultPause; }

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
