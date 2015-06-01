/* sendpraat.c */
/* by Paul Boersma */
/* 1 June 2015 */

/*
 * The sendpraat subroutine (Unix with GTK; Windows; Macintosh) sends a message
 * to a running program that uses the Praat shell.
 * The sendpraat program does the same from a Unix command shell,
 * from a Windows console, or from a MacOS X terminal window.
 *
 * Newer versions of sendpraat may be found at http://www.praat.org or http://www.fon.hum.uva.nl/praat/sendpraat.html
 *
 * On Windows, this version works only with Praat version 4.3.28 (November 2005) or newer.
 * On Macintosh, this version works only with Praat version 3.8.75 (October 2000) or newer.
 * On Unix with GTK, this version works only with Praat version 5.1.33 (May 2010) or newer.
 * Newer versions of Praat may respond faster or more reliably.
 */

/*******************************************************************

   THIS CODE CAN BE COPIED, USED, AND DISTRIBUTED FREELY.
   IF YOU MODIFY IT, PLEASE KEEP MY NAME AND MARK THE CHANGES.
   IF YOU IMPROVE IT, PLEASE NOTIFY ME AT paul.boersma@uva.nl.

*******************************************************************/

#if defined (_WIN32)
	#include <windows.h>
	#include <stdio.h>
	#include <wchar.h>
	#ifdef __MINGW32__
		#define swprintf  _snwprintf
	#endif
	#define gtk 0
	#define win 1
	#define mac 0
#elif (defined (macintosh) || defined (__MACH__))
    #include <Carbon/Carbon.h>
    #include <wchar.h>
	#define gtk 0
	#define win 0
	#define mac 1
#elif defined (UNIX)
	#include <sys/types.h>
	#include <signal.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <ctype.h>
	#include <wchar.h>
	#if defined (NO_GRAPHICS)
		#define gtk 0
	#else
		#include <gtk/gtk.h>
		#define gtk 1
	#endif
	#define win 0
	#define mac 0
#else
	#include <wchar.h>
	#define gtk 0
	#define win 0
	#define mac 0
#endif

/*
 * The way to call the sendpraat subroutine from another program.
 */
char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text);
/*
 * Parameters:
 * 'display' is the Display or GdKDisplay pointer, which will be available if you call sendpraat from an X11 or GTK program.
 *    If 'display' is NULL, sendpraat will open the display by itself, and close it after use.
 *    On Windows and Macintosh, sendpraat ignores the 'display' parameter.
 * 'programName' is the name of the program that receives the message.
 *    This program must have been built with the Praat shell (the most common such programs are Praat and ALS).
 *    On Unix, the program name is usually all lower case, e.g. "praat" or "als", or the name of any other program.
 *    On Windows, you can use either "Praat", "praat", or the name of any other program.
 *    On Macintosh, 'programName' must be "Praat", "praat", "ALS", or the Macintosh signature of any other program.
 * 'timeOut' is the time (in seconds) after which sendpraat will return with a time-out error message
 *    if the receiving program sends no notification of completion.
 *    On Unix and Macintosh, the message is sent asynchronously if 'timeOut' is 0;
 *    this means that sendpraat will return OK (NULL) without waiting for the receiving program
 *    to handle the message.
 *    On Windows, the time out is ignored.
 * 'text' contains the contents of the Praat script to be sent to the receiving program.
 */

static char errorMessage [1000];
static wchar_t errorMessageW [1000];
#if gtk
	static long theTimeOut;
	static void handleCompletion (int message) { (void) message; }
	static void handleTimeOut (int message) { (void) message; sprintf (errorMessage, "Timed out after %ld seconds.", theTimeOut); }
#endif

char *sendpraat (void *display, const char *programName, long timeOut, const char *text) {
	char nativeProgramName [100];
	#if gtk
		char *home, pidFileName [256], messageFileName [256];
		FILE *pidFile;
		long pid, wid = 0;
	#elif win
		char homeDirectory [256], messageFileName [256], windowName [256];
		HWND window;
		(void) display;
		(void) timeOut;
	#elif mac
		AEDesc programDescriptor;
		AppleEvent event, reply;
		OSStatus err;
		UInt32 signature;
		(void) display;
	#endif

	/*
	 * Clean up from an earlier call.
	 */
	errorMessage [0] = '\0';

	/*
	 * Handle case differences.
	 */
	strcpy (nativeProgramName, programName);
	#if gtk
		nativeProgramName [0] = tolower (nativeProgramName [0]);
	#else
		nativeProgramName [0] = toupper (nativeProgramName [0]);
	#endif

	/*
	 * If the text is going to be sent in a file, create its name.
	 * The file is going to be written into the preferences directory of the receiving program.
	 * On Unix, the name will be something like /home/jane/.praat-dir/message.
	 * On Windows, the name will be something like C:\Users\Jane\Praat\Message.txt,
	 * or C:\Windows\Praat\Message.txt on older systems.
	 * On Macintosh, the text is NOT going to be sent in a file.
	 */
	#if gtk
		if ((home = getenv ("HOME")) == NULL) {
			sprintf (errorMessage, "HOME environment variable not set.");
			return errorMessage;
		}
		sprintf (messageFileName, "%s/.%s-dir/message", home, programName);
	#elif win
		if (GetEnvironmentVariableA ("USERPROFILE", homeDirectory, 255)) {
			;   /* Ready. */
		} else if (GetEnvironmentVariableA ("HOMEDRIVE", homeDirectory, 255)) {
			GetEnvironmentVariableA ("HOMEPATH", homeDirectory + strlen (homeDirectory), 255);
		} else {
			GetWindowsDirectoryA (homeDirectory, 255);
		}
		sprintf (messageFileName, "%s\\%s\\Message.txt", homeDirectory, programName);
	#endif

	/*
	 * Save the message file (Unix and Windows only).
	 */
	#if gtk || win
	{
		FILE *messageFile;
		if ((messageFile = fopen (messageFileName, "w")) == NULL) {
			sprintf (errorMessage, "Cannot create message file \"%s\" "
				"(no privilege to write to directory, or disk full, or program is not called %s).\n", messageFileName, programName);
			return errorMessage;
		}
		#if gtk
			if (timeOut)
				fprintf (messageFile, "#%ld\n", (long) getpid ());   /* Write own process ID for callback. */
		#endif
		fprintf (messageFile, "%s", text);
		fclose (messageFile);
	}
	#endif

	/*
	 * Where shall we send the message?
	 */
	#if gtk
		/*
		 * Get the process ID and the window ID of a running Praat-shell program.
		 */
		sprintf (pidFileName, "%s/.%s-dir/pid", home, programName);
		if ((pidFile = fopen (pidFileName, "r")) == NULL) {
			sprintf (errorMessage, "Program %s not running.", programName);
			return errorMessage;
		}
		if (fscanf (pidFile, "%ld%ld", & pid, & wid) < 1) {
			fclose (pidFile);
			sprintf (errorMessage, "Program %s not running, or disk has been full.", programName);
			return errorMessage;
		}
		fclose (pidFile);
	#elif win
		/*
		 * Get the window handle of the "Objects" window of a running Praat-shell program.
		 */
		sprintf (windowName, "PraatShell1 %s", programName);
		window = FindWindowA (windowName, NULL);
		if (! window) {
			sprintf (errorMessage, "Program %s not running (or an old version).", programName);
			return errorMessage;
		}
	#elif mac
		/*
		 * Convert the program name to a Macintosh signature.
		 * I know of no system routine for this, so I'll just translate the two most common names:
		 */
		if (! strcmp (programName, "praat") || ! strcmp (programName, "Praat") || ! strcmp (programName, "PRAAT"))
			signature = 'PpgB';
		else if (! strcmp (programName, "als") || ! strcmp (programName, "Als") || ! strcmp (programName, "ALS"))
			signature = 'CclA';
		else
			signature = 0;
		AECreateDesc (typeApplSignature, & signature, 4, & programDescriptor);
	#endif

	/*
	 * Send the message.
	 */
	#if gtk
		/*
		 * Be ready to receive notification of completion.
		 */
		if (timeOut)
			signal (SIGUSR2, handleCompletion);
		/*
		 * Notify running program.
		 */
		if (wid != 0) {   /* Praat shell version October 21, 1998 or later? Send event to window. */
			/*
			 * Notify main window.
			 */
			GdkEventClient gevent;
			g_type_init ();
			int displaySupplied = display != NULL;
			if (! displaySupplied) {
				display = gdk_display_open (getenv ("DISPLAY"));   /* GdkDisplay* */
				if (display == NULL) {
					sprintf (errorMessage, "Cannot open display %s", getenv ("DISPLAY"));
					return errorMessage;
				}
			}
			gevent. type = GDK_CLIENT_EVENT;
			gevent. window = 0;
			gevent. send_event = 1;
			gevent. message_type = gdk_atom_intern_static_string ("SENDPRAAT");
			gevent. data_format = 8;
			if (! gdk_event_send_client_message_for_display (display, (GdkEvent *) & gevent, wid)) {
				if (! displaySupplied) gdk_display_close (display);
				sprintf (errorMessage, "Cannot send message to %s (window %ld). "
					"The program %s may have been started by a different user, "
					"or may have crashed.", programName, wid, programName);
				return errorMessage;
			}
			if (! displaySupplied) gdk_display_close (display);
		}
		/*
		 * Wait for the running program to notify us of completion,
		 * but do not wait for more than 'timeOut' seconds.
		 */
		if (timeOut) {
			signal (SIGALRM, handleTimeOut);
			alarm (timeOut);
			theTimeOut = timeOut;   /* Hand an argument to handleTimeOut () in a static variable. */
			errorMessage [0] = '\0';
			pause ();
			if (errorMessage [0] != '\0') return errorMessage;
		}
	#elif win
		/*
		 * Notify the running program by sending a WM_USER message to its main window.
		 */
		if (SendMessage (window, WM_USER, 0, 0)) {
			sprintf (errorMessage, "Program %s returns error.", programName);   /* BUG? */
			return errorMessage;
		}
	#elif mac
		/*
		 * Notify the running program by sending it an Apple event of the magic class 758934755.
		 */
		AECreateAppleEvent (758934755, 0, & programDescriptor, kAutoGenerateReturnID, 1, & event);
		AEPutParamPtr (& event, 1, typeChar, text, strlen (text) + 1);
		#ifdef __MACH__
			err = AESendMessage (& event, & reply,
				( timeOut == 0 ? kAENoReply : kAEWaitReply ) | kAECanInteract | kAECanSwitchLayer,
				timeOut == 0 ? kNoTimeOut : 60 * timeOut);
		#else
			err = AESend (& event, & reply,
				( timeOut == 0 ? kAENoReply : kAEWaitReply ) | kAECanInteract | kAECanSwitchLayer,
				kAENormalPriority, timeOut == 0 ? kNoTimeOut : 60 * timeOut, NULL, NULL);
		#endif
		if (err != noErr) {
			if (err == procNotFound || err == connectionInvalid)
				sprintf (errorMessage, "Could not send message to program \"%s\".\n"
					"The program is probably not running (or an old version).", programName);
			else if (err == errAETimeout)
				sprintf (errorMessage, "Message to program \"%s\" timed out "
					"after %ld seconds, before completion.", programName, timeOut);
			else
				sprintf (errorMessage, "Unexpected sendpraat error %d.\nNotify the author.", err);
		}
		AEDisposeDesc (& programDescriptor);
		AEDisposeDesc (& event);
		AEDisposeDesc (& reply);
	#endif

	/*
	 * Notify the caller of success (NULL pointer) or failure (string with an error message).
	 */
	return errorMessage [0] == '\0' ? NULL : errorMessage;
}

wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text) {
	wchar_t nativeProgramName [100];
	#if gtk
		char *home, pidFileName [256], messageFileName [256];
		FILE *pidFile;
		long pid, wid = 0;
	#elif win
		wchar_t homeDirectory [256], messageFileName [256], windowName [256];
		HWND window;
		(void) display;
		(void) timeOut;
	#elif mac
		AEDesc programDescriptor;
		AppleEvent event, reply;
		OSStatus err;
		UInt32 signature;
		(void) display;
	#endif

	/*
	 * Clean up from an earlier call.
	 */
	errorMessageW [0] = '\0';

	/*
	 * Handle case differences.
	 */
	wcscpy (nativeProgramName, programName);
	#if gtk
		nativeProgramName [0] = tolower (nativeProgramName [0]);
	#else
		nativeProgramName [0] = toupper (nativeProgramName [0]);
	#endif

	/*
	 * If the text is going to be sent in a file, create its name.
	 * The file is going to be written into the preferences directory of the receiving program.
	 * On Unix, the name will be something like /home/jane/.praat-dir/message.
	 * On Windows, the name will be something like C:\Users\Jane\Praat\Message.txt,
	 * or C:\Windows\Praat\Message.txt on older systems.
	 * On Macintosh, the text is NOT going to be sent in a file.
	 */
	#if gtk
		if ((home = getenv ("HOME")) == NULL) {
			swprintf (errorMessageW, 1000, L"HOME environment variable not set.");
			return errorMessageW;
		}
		sprintf (messageFileName, "%s/.%ls-dir/message", home, programName);
	#elif win
		if (GetEnvironmentVariableW (L"USERPROFILE", homeDirectory, 255)) {
			;   /* Ready. */
		} else if (GetEnvironmentVariableW (L"HOMEDRIVE", homeDirectory, 255)) {
			GetEnvironmentVariableW (L"HOMEPATH", homeDirectory + wcslen (homeDirectory), 255);
		} else {
			GetWindowsDirectoryW (homeDirectory, 255);
		}
		swprintf (messageFileName, 256, L"%ls\\%ls\\Message.txt", homeDirectory, programName);
	#endif

	/*
	 * Save the message file (Unix and Windows only).
	 */
	#if gtk
		FILE *messageFile;
		if ((messageFile = fopen (messageFileName, "w")) == NULL) {
			swprintf (errorMessageW, 1000, L"Cannot create message file \"%s\" "
				L"(no privilege to write to directory, or disk full).\n", messageFileName);
			return errorMessageW;
		}
		if (timeOut)
			fwprintf (messageFile, L"#%ld\n", getpid ());   /* Write own process ID for callback. */
		fwprintf (messageFile, L"\ufeff%ls", text);
		fclose (messageFile);
	#elif win
		FILE *messageFile;
		if ((messageFile = _wfopen (messageFileName, L"w")) == NULL) {
			swprintf (errorMessageW, 1000, L"Cannot create message file \"%ls\" "
				L"(no privilege to write to directory, or disk full).\n", messageFileName);
			return errorMessageW;
		}
		fwprintf (messageFile, L"\ufeff%ls", text);
		fclose (messageFile);
	#endif

	/*
	 * Where shall we send the message?
	 */
	#if gtk
		/*
		 * Get the process ID and the window ID of a running Praat-shell program.
		 */
		sprintf (pidFileName, "%s/.%ls-dir/pid", home, programName);
		if ((pidFile = fopen (pidFileName, "r")) == NULL) {
			swprintf (errorMessageW, 1000, L"Program %ls not running.", programName);
			return errorMessageW;
		}
		if (fscanf (pidFile, "%ld%ld", & pid, & wid) < 1) {
			fclose (pidFile);
			swprintf (errorMessageW, 1000, L"Program %ls not running, or disk has been full.", programName);
			return errorMessageW;
		}
		fclose (pidFile);
	#elif win
		/*
		 * Get the window handle of the "Objects" window of a running Praat-shell program.
		 */
		swprintf (windowName, 256, L"PraatShell1 %ls", programName);
		window = FindWindowW (windowName, NULL);
		if (! window) {
			swprintf (errorMessageW, 1000, L"Program %ls not running (or an old version).", programName);
			return errorMessageW;
		}
	#elif mac
		/*
		 * Convert the program name to a Macintosh signature.
		 * I know of no system routine for this, so I'll just translate the two most common names:
		 */
		if (! wcscmp (programName, L"praat") || ! wcscmp (programName, L"Praat") || ! wcscmp (programName, L"PRAAT"))
			signature = 'PpgB';
		else if (! wcscmp (programName, L"als") || ! wcscmp (programName, L"Als") || ! wcscmp (programName, L"ALS"))
			signature = 'CclA';
		else
			signature = 0;
		AECreateDesc (typeApplSignature, & signature, 4, & programDescriptor);
	#endif

	/*
	 * Send the message.
	 */
	#if gtk
		/*
		 * Be ready to receive notification of completion.
		 */
		if (timeOut)
			signal (SIGUSR2, handleCompletion);
		/*
		 * Notify running program.
		 */
		if (wid != 0) {   /* Praat shell version October 21, 1998 or later? Send event to window. */
			/*
			 * Notify main window.
			 */
			GdkEventClient gevent;
			int displaySupplied = display != NULL;
			g_type_init ();
			if (! displaySupplied) {
				display = gdk_display_open (getenv ("DISPLAY"));   /* GdkDisplay* */
				if (display == NULL) {
					swprintf (errorMessageW, 1000, L"Cannot open display %s", getenv ("DISPLAY"));
					return errorMessageW;
				}
			}
			gevent. type = GDK_CLIENT_EVENT;
			gevent. window = 0;
			gevent. send_event = 1;
			gevent. message_type = gdk_atom_intern_static_string ("SENDPRAAT");
			gevent. data_format = 8;
			if (! gdk_event_send_client_message_for_display (display, (GdkEvent *) & gevent, wid)) {
				if (! displaySupplied) gdk_display_close (display);
				swprintf (errorMessageW, 1000, L"Cannot send message to %ls (window %ld). "
					"The program %ls may have been started by a different user, "
					"or may have crashed.", programName, wid, programName);
				return errorMessageW;
			}
			if (! displaySupplied) gdk_display_close (display);
		}
		/*
		 * Wait for the running program to notify us of completion,
		 * but do not wait for more than 'timeOut' seconds.
		 */
		if (timeOut) {
			signal (SIGALRM, handleTimeOut);
			alarm (timeOut);
			theTimeOut = timeOut;   /* Hand an argument to handleTimeOut () in a static variable. */
			errorMessageW [0] = '\0';
			pause ();
			if (errorMessageW [0] != '\0') return errorMessageW;
		}
	#elif win
		/*
		 * Notify the running program by sending a WM_USER message to its main window.
		 */
		if (SendMessage (window, WM_USER, 0, 0)) {
			swprintf (errorMessageW, 1000, L"Program %ls returns error.", programName);   /* BUG? */
			return errorMessageW;
		}
	#elif mac
		/*
		 * Notify the running program by sending it an Apple event of the magic class 758934756.
		 */
		AECreateAppleEvent (758934756, 0, & programDescriptor, kAutoGenerateReturnID, 1, & event);
		AEPutParamPtr (& event, 1, typeUnicodeText, text, wcslen (text) + 1);
		#ifdef __MACH__
			err = AESendMessage (& event, & reply,
				( timeOut == 0 ? kAENoReply : kAEWaitReply ) | kAECanInteract | kAECanSwitchLayer,
				timeOut == 0 ? kNoTimeOut : 60 * timeOut);
		#else
			err = AESend (& event, & reply,
				( timeOut == 0 ? kAENoReply : kAEWaitReply ) | kAECanInteract | kAECanSwitchLayer,
				kAENormalPriority, timeOut == 0 ? kNoTimeOut : 60 * timeOut, NULL, NULL);
		#endif
		if (err != noErr) {
			if (err == procNotFound || err == connectionInvalid)
				swprintf (errorMessageW, 1000, L"Could not send message to program \"%ls\".\n"
					L"The program is probably not running (or an old version).", programName);
			else if (err == errAETimeout)
				swprintf (errorMessageW, 1000, L"Message to program \"%ls\" timed out "
					L"after %ld seconds, before completion.", programName, timeOut);
			else
				swprintf (errorMessageW, 1000, L"Unexpected sendpraat error %d.\nNotify the author.", err);
		}
		AEDisposeDesc (& programDescriptor);
		AEDisposeDesc (& event);
		AEDisposeDesc (& reply);
	#endif

	/*
	 * Notify the caller of success (NULL pointer) or failure (string with an error message).
	 */
	return errorMessageW [0] == '\0' ? NULL : errorMessageW;
}

/*
 * To compile sendpraat as a stand-alone program, use the -DSTAND_ALONE option to the C compiler:
 */
#ifdef STAND_ALONE
/*
 * To compile on MacOS X:
cc -o sendpraat -DSTAND_ALONE -framework CoreServices -I/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers -I/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers sendpraat.c -Dmacintosh -DuseCarbon=1
 *
 * To compile on Linux:
cc -std=gnu99 -o sendpraat -DSTAND_ALONE -DUNIX `pkg-config --cflags --libs gtk+-2.0` sendpraat.c
*/
int main (int argc, char **argv) {
	int iarg, line, length = 0;
	long timeOut = 10;   /* Default. */
	char programName [64], *message, *result;
	if (argc == 1) {
		printf ("Syntax:\n");
		#if win
			printf ("   sendpraat <program> <message>\n");
		#else
			printf ("   sendpraat [<timeOut>] <program> <message>\n");
		#endif
		printf ("\n");
		printf ("Arguments:\n");
		printf ("   <program>: the name of a running program that uses the Praat shell.\n");
		printf ("   <message>: a sequence of Praat shell lines (commands and directives).\n");
		#if ! win
			printf ("   <timeOut>: the number of seconds that sendpraat will wait for an answer\n");
			printf ("              before writing an error message. A <timeOut> of 0 means that\n");
			printf ("              the message will be sent asynchronously, i.e., that sendpraat\n");
			printf ("              will return immediately without issuing any error message.\n");
		#endif
		printf ("\n");
		printf ("Usage:\n");
		printf ("   Each line is a separate argument.\n");
		printf ("   Lines that contain spaces should be put inside double quotes.\n");
		printf ("\n");
		printf ("Examples:\n");
		printf ("\n");
		#if win
			printf ("   sendpraat praat Quit\n");
		#else
			printf ("   sendpraat 0 praat Quit\n");
		#endif
		printf ("      Causes the program \"praat\" to quit (gracefully).\n");
		printf ("      This works because \"Quit\" is a fixed command in Praat's Control menu.\n");
		#if ! win
			printf ("      Sendpraat will return immediately.\n");
		#endif
		printf ("\n");
		#if win
			printf ("   sendpraat praat \"Play reverse\"\n");
		#else
			printf ("   sendpraat 1000 praat \"Play reverse\"\n");
		#endif
		printf ("      Causes the program \"praat\", which can play sounds,\n");
		printf ("      to play the selected Sound objects backwards.\n");
		printf ("      This works because \"Play reverse\" is an action command\n");
		printf ("      that becomes available in Praat's dynamic menu when Sounds are selected.\n");
		#if ! win
			printf ("      Sendpraat will allow \"praat\" at most 1000 seconds to perform this.\n");
		#endif
		printf ("\n");
		#if win
			printf ("   sendpraat praat \"execute C:\\MyDocuments\\MyScript.praat\"\n");
		#else
			printf ("   sendpraat praat \"execute ~/MyResearch/MyProject/MyScript.praat\"\n");
		#endif
		printf ("      Causes the program \"praat\" to execute a script.\n");
		#if ! win
			printf ("      Sendpraat will allow \"praat\" at most 10 seconds (the default time out).\n");
		#endif
		printf ("\n");
		printf ("   sendpraat als \"for i from 1 to 5\" \"Draw circle... 0.5 0.5 0.1*i\" \"endfor\"\n");
		printf ("      Causes the program \"als\" to draw five concentric circles\n");
		printf ("      into its Picture window.\n");
		exit (0);
	}
	iarg = 1;

	#if ! win
		/*
		 * Get time-out.
		 */
		if (isdigit (argv [iarg] [0])) timeOut = atol (argv [iarg ++]);
	#endif

	/*
	 * Get program name.
	 */
	if (iarg == argc) {
		fprintf (stderr, "sendpraat: missing program name. Type \"sendpraat\" to get help.\n");
		return 1;
	}
	strcpy (programName, argv [iarg ++]);

	/*
	 * Create the message string.
	 */
	for (line = iarg; line < argc; line ++) length += strlen (argv [line]) + 1;
	length --;
	message = malloc (length + 1);
	message [0] = '\0';
	for (line = iarg; line < argc; line ++) {
		strcat (message, argv [line]);
		if (line < argc - 1) strcat (message, "\n");
	}

	/*
	 * Send message.
	 */
	result = sendpraat (NULL, programName, timeOut, message);
	if (result != NULL)
		{ fprintf (stderr, "sendpraat: %s\n", result); exit (1); }

	exit (0);
	return 0;
}
#endif

/* End of file sendpraat.c */
