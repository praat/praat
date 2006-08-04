#ifndef _melder_h_
#define _melder_h_
/* melder.h
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
	pb 2002/03/14 Mach
	pb 2002/10/02 getenv and system
	pb 2002/11/23 MelderInfo, Melder_double
	pb 2002/11/30 Melder_fixed
	pb 2002/12/01 Melder_single, Melder_half
	pb 2003/02/07 MelderDir_copy, MelderDir_equal
	pb 2003/03/12 MelderDir_setToNull, Melder_getHomeDir
	pb 2003/05/13 Melder_percent
	pb 2003/05/19 Melder_atof
	pb 2003/09/12 MelderFile_getMacType
	pb 2003/09/12 Sound Designer II files
	pb 2003/09/14 MelderDir_relativePathToFile
	pb 2004/04/07 booleans
	pb 2004/10/16 C++ compatibility (unique name for struct tags)
	pb 2004/10/18 allocation count is double
	pb 2004/10/27 Melder_warningOff/On
	pb 2005/06/16 no enums (because of compiler warnings)
	pb 2005/11/18 URL support
	pb 2006/04/16 tokens
	pb 2006/08/02 openForWriting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	#define strequ  ! strcmp
	#define strnequ  ! strncmp
#include <stdarg.h>
#include <stddef.h>

#ifndef TRUE
	#define TRUE  1
#endif
#ifndef FALSE
	#define FALSE  0
#endif
#ifndef NULL
	#define NULL  ((void *) 0)
#endif
#ifndef _Bool
	#define _Bool  int
#endif

#if defined (macintosh) && ! defined (__MACH__)
	#define Melder_DIRECTORY_SEPARATOR  ':'
#elif defined (_WIN32)
	#define Melder_DIRECTORY_SEPARATOR  '\\'
#else
	#define Melder_DIRECTORY_SEPARATOR  '/'
#endif

#if defined (macintosh) && ! defined (__MACH__)
	typedef struct {
		short vRefNum;
		long parID;
		char name [260];
		FILE *filePointer;
		int openForWriting;
	} structMelderFile, *MelderFile;
	typedef struct {
		short vRefNum;
		long dirID;
	} structMelderDir, *MelderDir;
#else
	typedef struct {
		char path [260];
		FILE *filePointer;
		int openForWriting;
	} structMelderFile, *MelderFile;
	typedef struct {
		char path [260];
	} structMelderDir, *MelderDir;
#endif

/********** NUMBER AND STRING COMPARISON **********/

#define Melder_NUMBER_min  1
#define Melder_NUMBER_EQUAL_TO  1
#define Melder_NUMBER_NOT_EQUAL_TO  2
#define Melder_NUMBER_LESS_THAN  3
#define Melder_NUMBER_LESS_THAN_OR_EQUAL_TO  4
#define Melder_NUMBER_GREATER_THAN  5
#define Melder_NUMBER_GREATER_THAN_OR_EQUAL_TO  6
#define Melder_NUMBER_max  6
const char * Melder_NUMBER_text_adjective (int which_Melder_NUMBER);
int Melder_numberMatchesCriterion (double value, int which_Melder_NUMBER, double criterion);

#define Melder_STRING_min  1
#define Melder_STRING_EQUAL_TO  1
#define Melder_STRING_NOT_EQUAL_TO  2
#define Melder_STRING_CONTAINS  3
#define Melder_STRING_DOES_NOT_CONTAIN  4
#define Melder_STRING_STARTS_WITH  5
#define Melder_STRING_DOES_NOT_START_WITH  6
#define Melder_STRING_ENDS_WITH  7
#define Melder_STRING_DOES_NOT_END_WITH  8
#define Melder_STRING_MATCH_REGEXP  9
#define Melder_STRING_max  9
const char * Melder_STRING_text_finiteVerb (int which_Melder_STRING);
int Melder_stringMatchesCriterion (const char *value, int which_Melder_STRING, const char *criterion);

/********** NUMBER TO STRING CONVERSION **********/

/*
	The following routines return a static string, chosen from a circularly used set of 11 buffers.
	You can call at most 11 of them in one Melder_casual call, for instance.
*/
const char * Melder_integer (long value);
const char * Melder_bigInteger (double value);
const char * Melder_boolean (int value);   /* "yes" or "no" */
const char * Melder_double (double value);   /* "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats */
const char * Melder_single (double value);   /* "--undefined--" or something in the "%.8g" format */
const char * Melder_half (double value);   /* "--undefined--" or something in the "%.4g" format */
const char * Melder_fixed (double value, int precision);   /* "--undefined--" or something in the "%.*f" format*/
const char * Melder_fixedExponent (double value, int exponent, int precision);
	/* if exponent is -2 and precision is 2:   67E-2, 0.00024E-2 */
const char * Melder_percent (double value, int precision);
	/* "--undefined--" or, if precision is 3: "0" or "34.400%" of "0.014%" or "0.001%" or "0.0000007%" */

/********** STRING TO NUMBER CONVERSION **********/

int Melder_isStringNumeric (const char *string);
double Melder_atof (const char *string);
	/*
	 * "3.14e-3" -> 3.14e-3
	 * "15.6%" -> 0.156
	 * "fghfghj" -> NUMundefined
	 */

/********** STRING PARSING **********/

/*
	These functions regard a string as a sequence of tokens,
	separated (and perhaps preceded and followed) by white space.
	The tokens cannot contain spaces themselves (there are no escapes).
	Typical use:
		for (token = Melder_firstToken (string); token != NULL; token = Melder_nextToken ()) {
			... do something with the token ...
		}
*/

long Melder_countTokens (const char *string);
char *Melder_firstToken (const char *string);
char *Melder_nextToken (void);

/********** MESSAGING ROUTINES **********/

/* These functions are called like printf ().
	Default Melder does fprintf to stderr,
	except Melder_information, which does fprintf to stdout.
	These functions show generic, native, and mixed strings correctly,
	and perform quote conversion, if that flag is not off; see under "NON-ASCII CHARACTERS".
	The alphabet is Roman, so that symbols from the Symbol and Phonetic alphabets
	are not translated (by default, \mu is shown as \mu and \as as a).
*/

void Melder_casual (const char *format, ...);
/*
	Function:
		Sends a message without user interference.
	Behaviour:
		Writes to stderr on Unix, otherwise to a special window.
*/

void MelderInfo_open (void);   /* Clear the Info window in the background. */
void MelderInfo_write1 (const char *s1);   /* Write a string to the Info window in the background. */
void MelderInfo_write2 (const char *s1, const char *s2);   /* Write two strings to the Info window in the background. */
void MelderInfo_write3 (const char *s1, const char *s2, const char *s3);
void MelderInfo_write4 (const char *s1, const char *s2, const char *s3, const char *s4);
void MelderInfo_write5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5);
void MelderInfo_writeLine1 (const char *s1);   /* Write a string to the Info window in the background; add a new-line. */
void MelderInfo_writeLine2 (const char *s1, const char *s2);
void MelderInfo_writeLine3 (const char *s1, const char *s2, const char *s3);
void MelderInfo_writeLine4 (const char *s1, const char *s2, const char *s3, const char *s4);
void MelderInfo_writeLine5 (const char *s1, const char *s2, const char *s3, const char *s4, const char *s5);
void MelderInfo_close (void);   /* Flush the background info to the Info window. */

void Melder_information (const char *format, ...);   /* Clear the Info window and write formatted text to it. */
void Melder_informationReal (double value, const char *units);   /* %.17g or --undefined--; units may be NULL */
	/* Give information to stdout (batch) or to an "Info" window (interactive). */

void Melder_info (const char *format, ...);   /* Write formatted text to the Info window without clearing it; append a newline. */

void Melder_divertInfo (char *buffer);   /* NULL = back to normal. */

void Melder_print (const char *format, ...);
	/* The same as Melder_info, but without adding a new-line symbol at the end. */

void Melder_clearInfo (void);   /* Clear the Info window. */
char * Melder_getInfo (void);
void Melder_help (const char *query);

void Melder_search (void);

void Melder_warning (const char *format, ...);
	/* Give warning to stderr (batch) or to a "Warning" dialog. */
void Melder_warningOff (void);
void Melder_warningOn (void);
	
void Melder_beep (void);

int Melder_error (const char *format, ...);   /* "Queue error" */
	/* Generate, but do not yet show, an error message, return 0. */

void * Melder_errorp (const char *format, ...);
	/* Generate, but do not yet show, an error message, return NULL. */

void Melder_flushError (const char *format, ...);
	/* Send all deferred error messages to stderr (batch) or to an "Error" dialog, */
	/* including, if 'format' is not NULL, the error message generated by this routine. */

int Melder_hasError (void);
	/* Returns 1 if there is an error message in store, otherwise 0. */
#define iferror  if (Melder_hasError ())
#define cherror  iferror goto end;

void Melder_clearError (void);
	/* Cancel all stored error messages. */

char * Melder_getError (void);
	/* Returns the error string. Mainly used with strstr. */

int Melder_fatal (const char *format, ...);
	/* Give error message, abort program. */
	/* Should only be caused by programming errors. */

#ifdef NDEBUG
	#define Melder_assert(x)   ((void) 0)
#else
	/*
	#define Melder_assert(x)   ((void) ((x) || _Melder_assert (#x, __FILE__, __LINE__)))
	*/
	#define Melder_assert(x)   ((x) ? (void) (0) : (void) _Melder_assert (#x, __FILE__, __LINE__))
#endif
int _Melder_assert (const char *condition, const char *fileName, int lineNumber);
	/* Call Melder_fatal with a message based on the following template: */
	/*    "Assertion failed in file <fileName> on line <lineNumber>: <condition>" */

extern int Melder_debug;

/********** PROGRESS ROUTINES **********/

int Melder_progress (double progress, const char *format, ...);
void Melder_progressOff (void);
void Melder_progressOn (void);
/*
	Function:
		Show the progress of a time-consuming process.
	Arguments:
		'format' may be NULL.
	Batch behaviour:
		Does nothing, always returns 1.
	Interactive behaviour:
		Shows the progress of a time-consuming process:
		- if 'progress' <= 0.0, show a window with text and a Cancel button, and return 1;
		- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
		  and return 0 if user interrupts, else return 1;
		- if 'progress' >= 1, hide the window.
	Usage:
		- call with 'progress' = 0.0 before the process starts:
			  (void) Melder_progress (0.0, "Starting work...");
		- at every turn in your loop, call with 'progress' between 0 and 1,
		  and check the return value to see if the user clicked the Cancel button:
			  if (! Melder_progress (i / (n + 1.0), "Working on part %d out of %d...", i, n))
			  {
				  forget (me);   // Clean up.
				  return Melder_errorp ("Work interrupted, not finished");   // Interrupt.
			  }
		- after the process has finished, call with 'progress' = 1.0:
			  (void) Melder_progress (1.0, NULL);
*/
	
void * Melder_monitor (double progress, const char *format, ...);
/*
	Function:
		Show the progress of a time-consuming process.
	Arguments:
		'format' may be NULL.
	Batch behaviour:
		Does nothing, returns NULL if 'progress' <= 0.0 and a non-NULL pointer otherwise.
	Interactive behaviour:
		Shows the progress of a time-consuming process:
		- if 'progress' <= 0.0, show a window with text and a Cancel button and
		  room for a square drawing, and return a Graphics;
		- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
		  and return NULL if user interrupts, else return a non-NULL pointer;
		- if 'progress' >= 1, hide the window.
	Usage:
		- call with 'progress' = 0.0 before the process starts.
		- assign the return value to a Graphics:
			Graphics graphics = Melder_monitor (0.0, "Starting work...");
		- at every turn of your loop, draw something in the Graphics:
			if (graphics) {   // Always check; might be batch.
				Graphics_clearWs (graphics);   // Only if you redraw all every time.
				Graphics_polyline (graphics, ...);
				Graphics_printf (graphics, ...);
			}
		- immediately after this in your loop, call with 'progress' between 0 and 1,
		  and check the return value to see if the user clicked the Cancel button:
			if (! Melder_monitor (i / (n + 1.0), "Working on part %d out of %d...", i, n)) {
				forget (me);   // Clean up.
				return Melder_errorp ("Work interrupted, not finished");   // Interrupt.
			}
		- after the process has finished, call with 'progress' = 1.0:
			(void) Melder_monitor (1.0, NULL);
*/

int Melder_pause (const char *format, ...);
/*
	Function:
		Pause the progress of a process.
	Arguments:
		'format' may be NULL.
	Batch behaviour:
		Writes a prompt "Pause:" to stderr, followed by the message text,
		and followed by the line "Press 'q' to stop, or any other key to continue.".
		Waits for the user to hit a key to stdin.
		Returns 0 if the user hits 'q' or 'Q', otherwise 1.
	Interactive behaviour:
		Shows a window titled "Pause" with the message text and two buttons named Continue and Stop.
		Waits for the user to click one of these.
		Returns 1 if user clicks Continue, 0 if user clicks Stop.
	Usage:
		Check the return value to see if the user clicked the Stop button:
			nextPicture ();
			if (! Melder_pause ("See this beautiful picture"))
				return Melder_errorp ("Demo interrupted.");
			nextPicture ();
*/

/********** RECORD AND PLAY ROUTINES **********/

int Melder_publish (void *anything);

int Melder_record (double duration);
int Melder_recordFromFile (MelderFile fs);
void Melder_play (void);
void Melder_playReverse (void);
int Melder_publishPlayed (void);

/********** SYSTEM VERSION **********/

extern unsigned long Melder_systemVersion;
/*
	For Macintosh, this is set in the Motif emulator.
*/

/********** SCRATCH TEXT BUFFERS **********/

extern char Melder_buffer1 [30001], Melder_buffer2 [30001];
/*
	Every Melder routine uses both of these buffers:
	one for sprintfing the message,
	and one for translating this message to a native string.
	You can use these buffers,
	but be careful not to call any other routines that use them at the same time;
	the following routines are guaranteed not to call the Melder library:
	 - Mac Toolbox, XWindows, X Toolkit, Motif, and XVT routines,
		except those who dispatch events (XtDispatchEvent, xvt_process_pending_events).
	 - Longchar_*
	This means that you can use these buffers for reading and writing with
	the Longchar library.
*/

/********** ENFORCE INTERACTIVE BEHAVIOUR **********/

/* Procedures to enforce interactive behaviour of the Melder_XXXXXX routines. */

void MelderMotif_create (/* XtAppContext* */ void *appContext, /* Widget */ void *parent);
/*
	'appContext' is the XtAppContext* output from Xt(Va)AppInitialize;
		if you used Xt(Va)Initialize it should be NULL.
	'parent' is the top-level widget returned by Xt(Va)(App)Initialize.
*/

extern int Melder_batch;   /* True if run from the batch or from an interactive command-line interface. */
extern int Melder_backgrounding;   /* True if running a script. */
#ifndef CONTROL_APPLICATION
	extern void *Melder_appContext;   /* XtAppContext* */
	extern void *Melder_topShell;   /* Widget */
#endif

/********** OVERRIDE DEFAULT BEHAVIOUR **********/

/* Procedures to override default message methods. */
/* They may chage the string arguments. */
/* Many of these routines are called by MelderMotif_create and MelderXvt_create. */

void Melder_setCasualProc (void (*casualProc) (char *message));
void Melder_setProgressProc (int (*progressProc) (double progress, char *message));
void Melder_setMonitorProc (void * (*monitorProc) (double progress, char *message));
void Melder_setPauseProc (int (*pauseProc) (char *message));
void Melder_setInformationProc (void (*informationProc) (char *message));
void Melder_setHelpProc (void (*help) (const char *query));
void Melder_setSearchProc (void (*search) (void));
void Melder_setWarningProc (void (*warningProc) (char *message));
void Melder_setErrorProc (void (*errorProc) (char *message));
void Melder_setFatalProc (void (*fatalProc) (char *message));
void Melder_setPublishProc (int (*publish) (void *));
void Melder_setRecordProc (int (*record) (double));
void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile));
void Melder_setPlayProc (void (*play) (void));
void Melder_setPlayReverseProc (void (*playReverse) (void));
void Melder_setPublishPlayedProc (int (*publishPlayed) (void));

/********** MEMORY ALLOCATION ROUTINES **********/

/* These routines call malloc, free, realloc, and calloc. */
/* If out of memory, they queue an error message (like "Out of memory") and return NULL. */
/* These routines also maintain a count of the total number of blocks allocated. */

void * Melder_malloc (long size);
void * Melder_realloc (void *pointer, long size);
void * Melder_calloc (long numberOfElements, long elementSize);
char * Melder_strdup (const char *string);

#define Melder_free(pointer)  _Melder_free ((void **) & (pointer))
void _Melder_free (void **pointer);
/*
	Preconditions:
		none (*pointer may be NULL).
	Postconditions:
		*pointer == NULL;
*/

double Melder_allocationCount (void);
/*
	Returns the total number of successful calls to
	Melder_malloc, Melder_realloc (if 'ptr' is NULL), Melder_calloc, and Melder_strdup,
	since the start of the process. Mainly for debugging purposes.
*/

double Melder_deallocationCount (void);
/*
	Returns the total number of successful calls to Melder_free,
	since the start of the process. Mainly for debugging purposes.
*/

double Melder_allocationSize (void);
/*
	Returns the total number of bytes allocated in calls to
	Melder_malloc, Melder_realloc (if moved), Melder_calloc, and Melder_strdup,
	since the start of the process. Mainly for debugging purposes.
*/

/********** FILE ROUTINES **********/

#if defined (macintosh) && ! defined (__MACH__)
	void Melder_macToFile (void *macFSSpec, MelderFile file);
#endif
#if defined (__MACH__)
	void Melder_machToFile (void *void_fsref, MelderFile file);
	int Melder_fileToMach (MelderFile file, void *void_fsref);
#endif
#if defined (macintosh)
	int Melder_fileToMac (MelderFile file, void *macFSSpec);
#endif
char * MelderFile_name (MelderFile file);
char * MelderDir_name (MelderDir dir);
int Melder_pathToDir (const char *path, MelderDir dir);
int Melder_pathToFile (const char *path, MelderFile file);
int Melder_relativePathToFile (const char *path, MelderFile file);
char * Melder_dirToPath (MelderDir dir);
	/* Returns a one-time static string like "HardDisk:Paul:Praats:"
	    or an pointer internal to 'dir', like "/u/paul/praats" or "D:\Paul\Praats" */
char * Melder_fileToPath (MelderFile file);
void MelderFile_copy (MelderFile file, MelderFile copy);
void MelderDir_copy (MelderDir dir, MelderDir copy);
int MelderFile_equal (MelderFile file1, MelderFile file2);
int MelderDir_equal (MelderDir dir1, MelderDir dir2);
void MelderFile_setToNull (MelderFile file);
int MelderFile_isNull (MelderFile file);
void MelderDir_setToNull (MelderDir dir);
int MelderDir_isNull (MelderDir dir);
void MelderDir_getFile (MelderDir parent, const char *fileName, MelderFile file);
void MelderDir_relativePathToFile (MelderDir dir, const char *path, MelderFile file);
void MelderFile_getParentDir (MelderFile file, MelderDir parent);
void MelderDir_getParentDir (MelderDir file, MelderDir parent);
int MelderDir_isDesktop (MelderDir dir);
int MelderDir_getSubdir (MelderDir parent, const char *subdirName, MelderDir subdir);
void Melder_rememberShellDirectory (void);
char * Melder_getShellDirectory (void);
void Melder_getHomeDir (MelderDir homeDir);
void Melder_getPrefDir (MelderDir prefDir);
void Melder_getTempDir (MelderDir tempDir);

int MelderFile_exists (MelderFile file);
int MelderFile_readable (MelderFile file);
long MelderFile_length (MelderFile file);
int MelderFile_delete (MelderFile file);

/* The following two should be combined with each other and with Windows extension setting: */
FILE * Melder_fopen (MelderFile file, const char *type);
#if defined (macintosh)
	void MelderFile_setMacTypeAndCreator (MelderFile file, long fileType, long creator);
	unsigned long MelderFile_getMacType (MelderFile file);
#else
	#define MelderFile_setMacTypeAndCreator(f,t,c)  (void) 0
#endif
int Melder_fclose (MelderFile file, FILE *stream);
void Melder_files_cleanUp (void);

/* So these will be the future replacements for the above, as soon as we rid of text files: */
void MelderFile_open (MelderFile file);
void MelderFile_append (MelderFile file);
void MelderFile_create (MelderFile file, const char *macType, const char *macCreator, const char *winExtension);
void * MelderFile_read (MelderFile file, long nbytes);
char * MelderFile_readLine (MelderFile file);
void MelderFile_write (MelderFile file, char *buffer, long nbytes);
void MelderFile_writeLine (MelderFile file, char *buffer);
void MelderFile_rewind (MelderFile file);
void MelderFile_seek (MelderFile file, long position, int direction);
long MelderFile_tell (MelderFile file);
void MelderFile_close (MelderFile file);
/* If one of these routines fails, it closes the file and generates an error. */
/* It is often sufficient to call cherror only before other tests that could */
/* result in Melder_error (), and to call iferror at the end. */

/* A global for writing text files. Affects MelderFile_writeLine () only. */
#define MelderFile_NATIVE  1
#define MelderFile_MACINTOSH  2
#define MelderFile_UNIX  3
#define MelderFile_WINDOWS  4
void MelderFile_setLineSeparator (int systemType);

/* Read and write whole text files. */
char * MelderFile_readText (MelderFile file);
int MelderFile_writeText (MelderFile file, const char *text);
int MelderFile_appendText (MelderFile file, const char *text);

int Melder_createDirectory (MelderDir parent, const char *subdirName, int mode);

void Melder_getDefaultDir (MelderDir dir);
void Melder_setDefaultDir (MelderDir dir);
void MelderFile_setDefaultDir (MelderFile file);
void MelderFile_nativizePath (char *path);   /* Convert from Unix-style slashes. */

/* Use the following function to pass unchanged text or file names to Melder_* functions. */
/* Backslashes are replaced by "\bs". */
/* The trick is that Melder_asciiMessage returns one of 11 cyclically used static strings, */
/* so you can use up to 11 strings in a single Melder_* call. */
char * Melder_asciiMessage (const char *message);
/* The following calls Melder_asciiMessage (). */
/* On Macintosh, any directory information is stripped off. */
char * MelderFile_messageName (MelderFile file);

double Melder_stopwatch (void);

long Melder_killReturns_inline (char *text);
/*
	 Replaces all bare returns (Mac) or return / linefeed sequences (Win) with bare linefeeds (generic = Unix).
	 Returns new length of string (equal to or less than old length).
*/

/********** AUDIO **********/

void Melder_setUseInternalSpeaker (int useInternalSpeaker);
int Melder_getUseInternalSpeaker (void);
void Melder_setOutputGain (double gain);
double Melder_getOutputGain (void);
void Melder_audio_prefs (void);
long Melder_getBestSampleRate (long fsamp);
extern int Melder_isPlaying;
int Melder_play16 (const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels,
	int (*playCallback) (void *playClosure, long numberOfSamplesPlayed), void *playClosure);
int Melder_stopPlaying (int explicit);   /* Returns 1 if sound was playing. */
#define Melder_IMPLICIT  0
#define Melder_EXPLICIT  1
long Melder_getSamplesPlayed (void);
int Melder_stopWasExplicit (void);

void Melder_setMaximumAsynchronicity (int maximumAsynchronicity);
#define Melder_SYNCHRONOUS  0
#define Melder_CALLING_BACK  1
#define Melder_INTERRUPTABLE  2
#define Melder_ASYNCHRONOUS  3
int Melder_getMaximumAsynchronicity (void);

void Melder_setZeroPadding (double zeroPadding);   /* Seconds of silence before and after playing. */
double Melder_getZeroPadding (void);

/********** AUDIO FILES **********/

/* Audio file types. */
#define Melder_AIFF  1
#define Melder_AIFC  2
#define Melder_WAV  3
#define Melder_NEXT_SUN  4
#define Melder_NIST  5
#define Melder_SOUND_DESIGNER_TWO  6
#define Melder_NUMBER_OF_AUDIO_FILE_TYPES  6
char * Melder_audioFileTypeString (int audioFileType);   /* "AIFF", "AIFC", "WAV", "NeXT/Sun", "NIST", "Sound Designer II" */
char * Melder_macAudioFileType (int audioFileType);   /* "AIFF", "AIFC", "WAVE", "ULAW", "NIST", "Sd2f" */
char * Melder_winAudioFileExtension (int audioFileType);   /* ".aiff", ".aifc", ".wav", ".au", ".nist", ".sd2" */
/* Audio encodings. */
#define Melder_LINEAR_8_SIGNED  1
#define Melder_LINEAR_8_UNSIGNED  2
#define Melder_LINEAR_16_BIG_ENDIAN  3
#define Melder_LINEAR_16_LITTLE_ENDIAN  4
#define Melder_LINEAR_24_BIG_ENDIAN  5
#define Melder_LINEAR_24_LITTLE_ENDIAN  6
#define Melder_LINEAR_32_BIG_ENDIAN  7
#define Melder_LINEAR_32_LITTLE_ENDIAN  8
#define Melder_MULAW  9
#define Melder_ALAW  10
#define Melder_SHORTEN  11
#define Melder_POLYPHONE  12
int Melder_defaultAudioFileEncoding16 (int audioFileType);   /* BIG_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN, BIG_ENDIAN */
int Melder_writeAudioFileHeader16 (FILE *f, int audioFileType, long sampleRate, long numberOfSamples, int numberOfChannels);
int MelderFile_writeAudioFile16 (MelderFile file, int audioFileType, const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels);

int MelderFile_checkSoundFile (MelderFile file, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples);
/* Returns information about a just opened audio file.
 * The return value is the audio file type, or 0 if it is not a sound file or in case of error.
 * The data start at 'startOfData' bytes from the start of the file.
 */
int Melder_bytesPerSamplePoint (int encoding);
int Melder_readAudioToFloat (FILE *f, int numberOfChannels, int encoding,
	float *leftBuffer, float *rightBuffer, long numberOfSamples);
/* If rightBuffer is NULL, reads mono data or averaged stereo data into leftBuffer.
 * If rightBuffer exists, read mono data into leftBuffer or stereo data into leftBuffer and rightBuffer.
 * Buffers are base-1.
 */
int Melder_readAudioToShort (FILE *f, int numberOfChannels, int encoding, short *buffer, long numberOfSamples);
/* If stereo, buffer will contain alternating left and right values.
 * Buffer is base-0.
 */
int Melder_writeFloatToAudio (FILE *f, int encoding, const float *left, long nleft, const float *right, long nright, int warnIfClipped);
int Melder_writeShortToAudio (FILE *f, int numberOfChannels, int encoding, const short *buffer, long numberOfSamples);

void Melder_audioTrigger (void);

/********** MISCELLANEOUS **********/

char * Melder_getenv (const char *variableName);
int Melder_system (const char *command);   /* Spawn a system command; return 0 if error. */

/* End of file melder.h */
#endif
