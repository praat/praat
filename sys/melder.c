/* melder.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
	pb 2002/03/07 GPL
	pb 2002/03/13 Mach
	pb 2002/11/30 Melder_fixed
	pb 2002/12/01 Melder_single, Melder_half
	pb 2002/12/11 MelderInfo
	pb 2003/05/13 Melder_percent
	pb 2003/05/19 Melder_atof
	pb 2003/05/19 Melder_fixed: include a minimum precision of 1 digit
	pb 2003/10/02 Melder_flushError: empty "errors" before showing the message
	pb 2003/12/29 Melder_warning: added XMapRaised because delete response is UNMAP
	pb 2004/04/04 Melder_bigInteger
	pb 2004/04/06 motif_information drains text window only, i.e. no longer updates all windows
	              (which used to cause up to seven seconds of delay in a 1-second sound window)
	pb 2004/10/24 info buffer can grow
	pb 2004/11/28 author notification in Melder_fatal
	pb 2005/03/04 number and string comparisons, including regular expressions
	pb 2005/06/16 removed enums from number and string comparisons (ints give no compiler warnings)
	pb 2005/07/19 Melder_stringMatchesCriterion: regard NULL criterion as empty string
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
	#include <Sound.h>
	#include "macport_off.h"
#endif
#ifndef CONSOLE_APPLICATION
	#include "Graphics.h"
	#include "machine.h"
	#ifdef macintosh
		#include "macport_on.h"
		#include <Events.h>
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

static int defaultPause (char *message) {
	int key;
	fprintf (stderr, "Pause: %s\nPress 'q' to stop, or anyother key to continue.\n", message);
	key = getc (stdin);
	return key != 'q' && key != 'Q';
}

static void defaultInformation (char *message) {
	printf ("%s", message);
}

static void defaultHelp (const char *query) {
	Melder_flushError ("Do not know how to find help on \"%s\".", query);
}

static void defaultSearch (void) {
	Melder_flushError ("Do not know how to search.");
}

static void defaultWarning (char *message) {
	fprintf (stderr, "Warning: %s\n", message);
}

static void defaultError (char *message) {
	fprintf (stderr, strstr (message, "You interrupted") ? "User interrupt: %s\n" : "Error: %s\n", message);
}

static void defaultFatal (char *message) {
	fprintf (stderr, "Fatal error: %s\n", message);
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
	int (*pause) (char *message);
	void (*information) (char *message);
	void (*help) (const char *query);
	void (*search) (void);
	void (*warning) (char *message);
	void (*error) (char *message);
	void (*fatal) (char *message);
	int (*publish) (void *anything);
	int (*record) (double duration);
	int (*recordFromFile) (MelderFile fs);
	void (*play) (void);
	void (*playReverse) (void);
	int (*publishPlayed) (void);
}
	theMelder = {
		defaultPause, defaultInformation, defaultHelp, defaultSearch,
		defaultWarning, defaultError, defaultFatal,
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
		MessageBox (NULL, Melder_buffer2, "Casual info", MB_OK);
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
	interruption = theMelder. pause (Melder_buffer1);
	va_end (arg);
	return interruption;
}

/********** NUMBER AND STRING COMPARISONS **********/

const char * Melder_NUMBER_text_adjective (int which_Melder_NUMBER) {
	static const char *strings [1+Melder_NUMBER_max] = { "",
		"equal to", "not equal to",
		"less than", "less than or equal to",
		"greater than", "greater than or equal to"
	};
	return strings [which_Melder_NUMBER < 0 || which_Melder_NUMBER > Melder_NUMBER_max ? 0 : which_Melder_NUMBER];
}

int Melder_numberMatchesCriterion (double value, int which_Melder_NUMBER, double criterion) {
	return
		which_Melder_NUMBER == Melder_NUMBER_EQUAL_TO && value == criterion ||
		which_Melder_NUMBER == Melder_NUMBER_NOT_EQUAL_TO && value != criterion ||
		which_Melder_NUMBER == Melder_NUMBER_LESS_THAN && value < criterion ||
		which_Melder_NUMBER == Melder_NUMBER_LESS_THAN_OR_EQUAL_TO && value <= criterion ||
		which_Melder_NUMBER == Melder_NUMBER_GREATER_THAN && value > criterion ||
		which_Melder_NUMBER == Melder_NUMBER_GREATER_THAN_OR_EQUAL_TO && value >= criterion;
}

const char * Melder_STRING_text_finiteVerb (int which_Melder_STRING) {
	static const char *strings [1+Melder_STRING_max] = { "",
		"is equal to", "is not equal to",
		"contains", "does not contain",
		"starts with", "does not start with",
		"ends with", "does not end with",
		"matches (regex)"
	};
	return strings [which_Melder_STRING < 0 || which_Melder_STRING > Melder_STRING_max ? 0 : which_Melder_STRING];
}

int Melder_stringMatchesCriterion (const char *value, int which_Melder_STRING, const char *criterion) {
	if (value == NULL) {
		value = "";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (criterion == NULL) {
		criterion = "";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (which_Melder_STRING <= Melder_STRING_NOT_EQUAL_TO) {
		int matchPositiveCriterion = strequ (value, criterion);
		return (which_Melder_STRING == Melder_STRING_EQUAL_TO) == matchPositiveCriterion;
	}
	if (which_Melder_STRING <= Melder_STRING_DOES_NOT_CONTAIN) {
		int matchPositiveCriterion = strstr (value, criterion) != NULL;
		return (which_Melder_STRING == Melder_STRING_CONTAINS) == matchPositiveCriterion;
	}
	if (which_Melder_STRING <= Melder_STRING_DOES_NOT_START_WITH) {
		int matchPositiveCriterion = strnequ (value, criterion, strlen (criterion));
		return (which_Melder_STRING == Melder_STRING_STARTS_WITH) == matchPositiveCriterion;
	}
	if (which_Melder_STRING <= Melder_STRING_DOES_NOT_END_WITH) {
		int criterionLength = strlen (criterion), valueLength = strlen (value);
		int matchPositiveCriterion = criterionLength <= valueLength && strequ (value + valueLength - criterionLength, criterion);
		return (which_Melder_STRING == Melder_STRING_ENDS_WITH) == matchPositiveCriterion;
	}
	if (which_Melder_STRING == Melder_STRING_MATCH_REGEXP) {
		char *place = NULL, *errorMessage;
		regexp *compiled_regexp = CompileRE (criterion, & errorMessage, 0);
		if (compiled_regexp == NULL) return FALSE;
		if (ExecRE (compiled_regexp, NULL, value, NULL, 0, '\0', '\0', NULL))
			place = compiled_regexp -> startp [0];
		free (compiled_regexp);
		return place != NULL;
	}
	return 0;   /* Should not occur. */
}

/***** INFO *****/

static char *foregroundBuffer, *infos;
static long foregroundBufferSize;

static void initInfo (void) {
	if (foregroundBuffer == NULL) {
		foregroundBuffer = Melder_calloc (foregroundBufferSize = 10000, 1);
		if (infos == NULL) {
			infos = foregroundBuffer;
		}
	}
	Melder_assert (foregroundBuffer != NULL && infos != NULL);
}

static void appendInfo (const char *message) {
	long oldLength, newLength;
	initInfo ();
	oldLength = strlen (infos), newLength = oldLength + strlen (message);
	if (infos == foregroundBuffer) {
		if (newLength >= foregroundBufferSize - 2) {   /* Leave room for one newline symbol and one null byte. */
			infos = foregroundBuffer = Melder_realloc (foregroundBuffer, foregroundBufferSize = newLength * 2 + 10000);
		}
		Melder_assert (infos != NULL);
		strcpy (& infos [oldLength], message);
	} else {
		if (newLength >= 30000 - 1) {   /* Leave room for one newline symbol and one null byte. */
			;   /* Unlikely to occur. */
		} else {
			strcpy (& infos [oldLength], message);
		}
	}
}

static void appendInfoLine (const char *message) {
	appendInfo (message);
	strcpy (infos + strlen (infos), "\n");
}

void MelderInfo_open (void) {
	initInfo ();
	infos [0] = '\0';
}

void MelderInfo_write1 (const char *s1) {
	appendInfo (s1);
}
void MelderInfo_write2 (const char *s1, const char *s2) {
	appendInfo (s1);
	appendInfo (s2);
}
void MelderInfo_write3 (const char *s1, const char *s2, const char *s3) {
	appendInfo (s1);
	appendInfo (s2);
	appendInfo (s3);
}
void MelderInfo_write4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	appendInfo (s1);
	appendInfo (s2);
	appendInfo (s3);
	appendInfo (s4);
}
void MelderInfo_write5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	appendInfo (s1);
	appendInfo (s2);
	appendInfo (s3);
	appendInfo (s4);
	appendInfo (s5);
}

void MelderInfo_writeLine1 (const char *s1) {
	appendInfoLine (s1);
}
void MelderInfo_writeLine2 (const char *s1, const char *s2) {
	appendInfo (s1);
	appendInfoLine (s2);
}
void MelderInfo_writeLine3 (const char *s1, const char *s2, const char *s3) {
	appendInfo (s1);
	appendInfo (s2);
	appendInfoLine (s3);
}
void MelderInfo_writeLine4 (const char *s1, const char *s2, const char *s3, const char *s4) {
	appendInfo (s1);
	appendInfo (s2);
	appendInfo (s3);
	appendInfoLine (s4);
}
void MelderInfo_writeLine5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5) {
	appendInfo (s1);
	appendInfo (s2);
	appendInfo (s3);
	appendInfo (s4);
	appendInfoLine (s5);
}

void MelderInfo_close (void) {
	initInfo ();
	if (infos == foregroundBuffer) {
		theMelder. information (infos);
	}
}

void Melder_information (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	initInfo ();
	vsprintf (infos, format, arg);
	/*
		When writing to the Info window, we must add a newline symbol,
		because a subsequent Melder_print call has to start on the next line.
		When writing to a diverted string, we must *not* add a newline symbol,
		because scripts expect returned strings without appended newlines!
	*/
	if (infos == foregroundBuffer)
		strcat (infos, "\n");
	MelderInfo_close ();
	va_end (arg);
}

void Melder_informationReal (double value, const char *units) {
	MelderInfo_open ();
	if (value == NUMundefined)
		MelderInfo_write1 ("--undefined--");
	else if (units == NULL)
		MelderInfo_write1 (Melder_double (value));
	else
		MelderInfo_write3 (Melder_double (value), " ", units);
	if (infos == foregroundBuffer)
		strcat (infos, "\n");
	MelderInfo_close ();
}

void Melder_info (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	initInfo ();
	vsprintf (Melder_buffer1, format, arg);
	if (infos == foregroundBuffer) {
		if (theMelder. information == defaultInformation) {
			printf ("%s\n", Melder_buffer1);   /* Do not print the previous lines again. */
		} else {
			appendInfoLine (Melder_buffer1);
			theMelder. information (infos);
		}
	} else {
		strcpy (infos, Melder_buffer1);   /* Without newline! */
	}
	va_end (arg);
}

void Melder_divertInfo (char *buffer) {
	initInfo ();
	infos = buffer == NULL ? foregroundBuffer : buffer;
}

void Melder_print (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	initInfo ();
	if (theMelder. information == defaultInformation) {
		vprintf (format, arg);   /* Do not print the previous lines again. */
	} else {
		vsprintf (Melder_buffer1, format, arg);
		appendInfo (Melder_buffer1);
		theMelder. information (infos);
	}
	va_end (arg);
}

void Melder_clearInfo (void) {
	initInfo ();
	if (infos == foregroundBuffer) {
		infos [0] = '\0';
		theMelder. information (infos);
	}
}

char * Melder_getInfo (void) {
	initInfo ();
	return infos;
}

void Melder_help (const char *query) {
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
		theMelder. warning (Melder_buffer2);
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

static char errors [2001];   /* Safe in low-memory situations. */

static void appendError (const char *message) {
	int length = strlen (errors), messageLength = strlen (message);
	if (length + messageLength > 2000 - 1) return;   /* 1 == length of "\n" */
	strcpy (errors + length, message);
	strcpy (errors + length + messageLength, "\n");
}

int Melder_error (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	appendError (Melder_buffer2);
	va_end (arg);
	return 0;
}

void * Melder_errorp (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vsprintf (Melder_buffer1, format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	appendError (Melder_buffer2);
	va_end (arg);
	return NULL;
}

int Melder_hasError (void) { return errors [0] != '\0'; }

void Melder_clearError (void) { errors [0] = '\0'; }

char * Melder_getError (void) { return & errors [0]; }

void Melder_flushError (const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
		appendError (Melder_buffer2);
	}
	/*
		"errors" has to be cleared *before* the message is put on the screen.
		This is because on some platforms the message dialog is synchronous
		(Melder_flushError will wait until the message dialog is closed),
		and some operating systems may force an immediate redraw event as soon as
		the message dialog is closed. We want "errors" to be empty when redrawing!
	*/
	strcpy (Melder_buffer1, errors);
	Melder_clearError ();
	theMelder. error (Melder_buffer1);
	va_end (arg);
}

int Melder_fatal (const char *format, ...) {
	const char *lead = strstr (format, "Praat cannot start up") ? "" :
		"Praat will crash. Notify the author (paul.boersma@uva.nl) with the following information:\n";
	va_list arg;
	va_start (arg, format);
	strcpy (Melder_buffer1, lead);
	vsprintf (Melder_buffer1 + strlen (lead), format, arg);
	Longchar_nativize (Melder_buffer1, Melder_buffer2, ! Melder_batch);
	theMelder. fatal (Melder_buffer2);
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
static int motif_pause (char *message) {
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
	if (! message) message = "";
	XtVaSetValues (text, motif_argXmString (XmNlabelString, message), NULL);
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

static void motif_warning (char *message) {
#ifdef _WIN32
	MessageBox (NULL, message, "Warning", MB_OK);
#else
	static Widget dia = NULL;
	if (dia == NULL)
		dia = makeMessage (XmDIALOG_WARNING, "warning", "Warning");
	XtVaSetValues (dia, motif_argXmString (XmNmessageString, message), NULL);
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
	ParamText (pmessage, "\p", "\p", "\p");
	Alert (129, NULL);
	SysError (11);
}
static void motif_error (char *message) {
	Str255 pmessage;
	int length, i;
	length = strlen (message);
	if (length == 0) return;
	if (length > 255) message [length = 255] = '\0';
	pmessage [0] = length;
	strncpy ((char *) pmessage + 1, message, 255);   /* Not strcpy! */
	for (i = 1; i <= length; i ++) if (pmessage [i] == '\n') pmessage [i] = '\r';
	ParamText (pmessage, "\p", "\p", "\p");
	Alert (130, NULL);
	XmUpdateDisplay (0);
}
#elif defined (_WIN32)
static void motif_fatal (char *message) {
	MessageBox (NULL, message, "Fatal error", MB_OK);
}
static void motif_error (char *message) {
	MessageBox (NULL, message, "Message", MB_OK);
}
#else
static void motif_error (char *message) {
	static Widget dia = NULL;
	if (dia == NULL)
		dia = makeMessage (XmDIALOG_ERROR, "error", "Message");
	XtVaSetValues (dia, motif_argXmString (XmNmessageString, message), NULL);
	XtManageChild (dia);
	XMapRaised (XtDisplay (XtParent (dia)), XtWindow (XtParent (dia)));   /* Because the delete response is UNMAP. */
}
#endif

void MelderMotif_create (void *appContext, void *parent) {
	extern void motif_information (char *);
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

#if defined(macintosh) && ! defined(__MACH__)
	/* Divert the printf in Melder_casual. */
	#ifndef __CONSOLE__
	#include <console.h>
	#endif
	short InstallConsole (short fd) { (void) fd; return 0; }
	void RemoveConsole (void) { }
	long WriteCharsToConsole (char *buffer, long n) {
		static char bufferWithZero [20000];
		strncpy (bufferWithZero, buffer, n);
		bufferWithZero [n] = '\0';
		Melder_information ("%s", bufferWithZero);
		return n;
	}
	long ReadCharsFromConsole (char *buffer, long n) { (void) buffer; (void) n; return 0; }
	extern char * __ttyname (long fd) { return fd >= 0 && fd <= 2 ? "null device" : NULL; }
#endif
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

void Melder_setPauseProc (int (*pause) (char *))
	{ theMelder. pause = pause ? pause : defaultPause; }

void Melder_setInformationProc (void (*information) (char *))
	{ theMelder. information = information ? information : defaultInformation; }

void Melder_setHelpProc (void (*help) (const char *query))
	{ theMelder. help = help ? help : defaultHelp; }

void Melder_setSearchProc (void (*search) (void))
	{ theMelder. search = search ? search : defaultSearch; }

void Melder_setWarningProc (void (*warning) (char *))
	{ theMelder. warning = warning ? warning : defaultWarning; }

void Melder_setErrorProc (void (*error) (char *))
	{ theMelder. error = error ? error : defaultError; }

void Melder_setFatalProc (void (*fatal) (char *))
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

/********** Memory allocations. **********/

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize;

#define TRACE_MALLOC  0

void * Melder_malloc (long size) {
	void *result;
	if (size <= 0)
		return Melder_errorp ("(Melder_malloc:) Can never allocate %ld bytes.", size);
	result = malloc (size);
	if (result == NULL)
		return Melder_errorp ("Out of memory: there is not enough room for another %ld bytes.", size);
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	#if TRACE_MALLOC
		Melder_casual ("malloc %ld", size);
	#endif
	return result;
}

void _Melder_free (void **ptr) {
	if (*ptr == NULL) return;
	free (*ptr);
	*ptr = NULL;
	totalNumberOfDeallocations += 1;
	#if TRACE_MALLOC
		Melder_casual ("free");
	#endif
}

void * Melder_realloc (void *ptr, long size) {
	void *result;
	if (size <= 0)
		return Melder_errorp ("(Melder_realloc:) Can never allocate %ld bytes.", size);
	result = realloc (ptr, size);   /* Will not show in the statistics... */
	if (result == NULL)
		return Melder_errorp ("Out of memory. Could not extend room to %ld bytes.", size);
	if (ptr == NULL) {   /* Is it like malloc? */
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
	} else if (result != ptr) {   /* Did realloc do a malloc-and-free? */
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
		totalNumberOfDeallocations += 1;
	}
	#if TRACE_MALLOC
		Melder_casual ("realloc %ld", size);
	#endif
	return result;
}

void * Melder_calloc (long nelem, long elsize) {
	void *result;
	if (nelem <= 0)
		return Melder_errorp ("(Melder_calloc:) "
			"Can never allocate %ld elements.", nelem);
	if (elsize <= 0)
		return Melder_errorp ("(Melder_calloc:) "
			"Can never allocate elements whose size is %ld bytes.", elsize);
	result = calloc (nelem, elsize);
	if (result == NULL)
		return Melder_errorp ("Out of memory: "
			"there is not enough room for %ld more elements whose sizes are %ld bytes each.", nelem, elsize);
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
	#if TRACE_MALLOC
		Melder_casual ("calloc %ld %ld", nelem, elsize);
	#endif
	return result;
}

char * Melder_strdup (const char *string) {
	char *result;
	long size;
	if (! string) return NULL;
	size = strlen (string) + 1;
	result = malloc (size);
	if (result == NULL)
		return Melder_errorp ("Out of memory: there is not enough room to duplicate a text of %ld characters.", size - 1);
	strcpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	#if TRACE_MALLOC
		Melder_casual ("strdup %ld", size);
	#endif
	return result;
}

double Melder_allocationCount (void) {
	return totalNumberOfAllocations;
}

double Melder_deallocationCount (void) {
	return totalNumberOfDeallocations;
}

double Melder_allocationSize (void) {
	return totalAllocationSize;
}

long Melder_killReturns_inline (char *text) {
	const char *from;
	char *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   /* Carriage return? */
			if (from [1] == '\n') {   /* Followed by linefeed? Must be a Windows text. */
				from ++;   /* Ignore carriage return. */
				*to = '\n';   /* Copy linefeed. */
			} else {   /* Bare carriage return? Must be a Macintosh text. */
				*to = '\n';   /* Change to linefeed. */
			}
		} else {
			*to = *from;
		}
	}
	*to = '\0';   /* Closing null byte. */
	return to - text;
}

#if 0
/********** NEWLINE CONVERSION ROUTINES **********/

char * Melder_linefeedsToWin (const char *text);
/*
	 Replaces all bare linefeeds (generic = Unix) or bare returns (Mac) with return / linefeed sequences (Win).
	 Remove with Melder_free.
*/
void Melder_linefeedsUnixToMac_inline (char *text);
/*
	 Replaces all bare linefeeds (generic = Unix) with bare returns (Mac).
	 Lengths of new and old strings are equal.
*/
long Melder_linefeedsToMac_inline (char *text);
/*
	 Replaces all bare linefeeds (generic = Unix) or return / linefeed sequences (Win) with bare returns (Mac).
	 Returns new length of string (equal to or less than old length).
*/

char * Melder_linefeedsToWin (const char *text) {
	const char *from;
	char *result = Melder_malloc (2 * strlen (text) + 1), *to;   /* All new lines plus one null byte. */
	if (! result) return NULL;
	for (from = text, to = result; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   /* Carriage return? */
			*to = 13;   /* Copy carriage return. */
			if (from [1] == '\n') {   /* Followed by linefeed? Must be a Windows text. */
				from ++, to ++;
				*to = '\n';   /* Copy linefeed. */
			} else {   /* Bare carriage return? Must be a Macintosh text. */
				* ++ to = '\n';   /* Insert linefeed. */
			}
		} else if (*from == '\n') {   /* Bare linefeed? Must be generic (Unix). */
			*to = 13;   /* Insert carriage return. */
			* ++ to = '\n';   /* Copy linefeed. */
		} else {
			*to = *from;
		}
	}
	*to = '\0';
	return result;
}

void Melder_linefeedsUnixToMac_inline (char *text) {
	char *p;
	for (p = text; *p != '\0'; p ++)
		if (*p == '\n')   /* Linefeed? */
			*p = 13;   /* Change to carriage return. */
}

long Melder_linefeedsToMac_inline (char *text) {
	const char *from;
	char *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   /* Carriage return? */
			if (from [1] == '\n') {   /* Followed by linefeed? Must be a Windows text. */
				*to = 13;   /* Copy carriage return. */
				from ++;   /* Ignore linefeed. */
			} else {   /* Bare carriage return? Must be a Macintosh text. */
				*to = 13;   /* Copy carriage return. */
			}
		} else if (*from == '\n') {   /* Bare linefeed? Must be generic (Unix). */
			*to = 13;   /* Change to carriage return. */
		} else {
			*to = *from;
		}
	}
	*to = '\0';   /* Closing null byte. */
	return to - text;
}
#endif

/* End of file melder.c */
