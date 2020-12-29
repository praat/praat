/* sendpraat.c */
/* by Paul Boersma */
/* 10 November 2020 */

/*
 * The sendpraat subroutine (Unix; Windows; Macintosh) sends a message
 * to a running Praat (or another program that uses the Praat shell).
 * The sendpraat program behaves identically from a Unix command shell,
 * from a Windows console, or from a macOS terminal window.
 *
 * Newer versions of sendpraat may be found at http://www.praat.org or http://www.fon.hum.uva.nl/praat/sendpraat.html
 *
 * On Windows, this version works only with Praat version 4.3.28 (November 2005) or newer.
 * On Macintosh, this version works only with Praat version 3.8.75 (October 2000) or newer.
 * On Unix, this version works only with Praat version 6.1.31 (November 2020) or newer.
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
	#define unix 0
	#define win 1
	#define mac 0
#elif (defined (macintosh) || defined (__MACH__))
    #include <Carbon/Carbon.h>
    #include <wchar.h>
	#define unix 0
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
	#include <X11/Xlib.h>
	#define unix 1
	#if defined (NO_GRAPHICS) || defined (NO_GUI)   /* for use inside Praat */
		#define xwin 0
	#else
		#define xwin 1
	#endif
	#define win 0
	#define mac 0
#else
	#include <wchar.h>
	#define unix 0
	#define win 0
	#define mac 0
#endif

/*
 * The way to call the sendpraat subroutine from another program.
 */
char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
/*
 * Parameters:
 * `display` is the Display pointer, which will be available if you call sendpraat from an X11 program.
 *    If `display` is NULL, sendpraat will open the display by itself, and close it after use.
 *    On Windows and Macintosh, sendpraat ignores the `display` parameter.
 * `programName` is the name of the program that receives the message.
 *    This program must have been built with the Praat shell (the most common such programs are Praat and ALS).
 *    On Unix, the program name is usually all lower case, e.g. "praat" or "als", or the name of any other program.
 *    On Windows, you can use either "Praat", "praat", or the name of any other program.
 *    On Macintosh, `programName` must be "Praat", "praat", "ALS", or the Macintosh signature of any other program.
 * `timeOut` is the time (in seconds) after which sendpraat will return with a time-out error message
 *    if the receiving program sends no notification of completion.
 *    On Unix and Macintosh, the message is sent asynchronously if `timeOut` is 0;
 *    this means that sendpraat will return OK (NULL) without waiting for the receiving program
 *    to handle the message.
 *    On Windows, the time out is ignored.
 * `text` contains the contents of the Praat script to be sent to the receiving program, encoded as UTF-8.
 */

static char errorMessage [1000];
#if unix
	static long theTimeOut;
	static void handleCompletion (int message) { (void) message; }
	static void handleTimeOut (int message) { (void) message; sprintf (errorMessage, "Timed out after %ld seconds.", theTimeOut); }
#endif

char *sendpraat (void *display, const char *programName, long timeOut, const char *text) {
	char nativeProgramName [100];
	#if unix
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
	#if unix
		nativeProgramName [0] = (char) tolower (nativeProgramName [0]);
	#else
		nativeProgramName [0] = (char) toupper (nativeProgramName [0]);
	#endif

	/*
	 * If the text is going to be sent in a file, create its name.
	 * The file is going to be written into the preferences folder of the receiving program.
	 * On Unix, the name will be something like /home/jane/.praat-dir/message.
	 * On Windows, the name will be something like C:\Users\Jane\Praat\Message.txt,
	 * or C:\Windows\Praat\Message.txt on older systems.
	 * On Macintosh, the text is NOT going to be sent in a file.
	 */
	#if unix
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
	#if unix || win
	{
		FILE *messageFile;
		if ((messageFile = fopen (messageFileName, "w")) == NULL) {
			sprintf (errorMessage, "Cannot create message file \"%s\" "
				"(no privilege to write to folder, or disk full, or program is not called %s).\n", messageFileName, programName);
			return errorMessage;
		}
		#if unix
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
	#if unix
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
	#if unix
		/*
		 * Be ready to receive notification of completion.
		 */
		if (timeOut)
			signal (SIGUSR2, handleCompletion);
		/*
		 * Notify running program.
		 */
		#if xwin
		if (wid != 0) {   /* Send event to window. */
			/*
			 * Notify main window.
			 */
			XEvent event;
			int displaySupplied = ( display != NULL );
			if (! displaySupplied) {
				display = XOpenDisplay (NULL);
				if (display == NULL) {
					sprintf (errorMessage, "Cannot open display %s.", XDisplayName (NULL));
					return errorMessage;
				}
			}
			event. type = ClientMessage;
			event. xclient. serial = 0;
			event. xclient. send_event = True;
			event. xclient. display = display;
			event. xclient. window = (Window) wid;
			event. xclient. message_type = XInternAtom (display, "SENDPRAAT", False);
			event. xclient. format = 8;   /* No byte swaps. */
			strcpy (& event. xclient.data.b [0], "SENDPRAAT");
			if (! XSendEvent (display, (Window) wid, True, KeyPressMask, & event)) {
				if (! displaySupplied)
					XCloseDisplay (display);
				sprintf (errorMessage, "Cannot send message to %s (window %ld). "
					"The program %s may have been started by a different user, "
					"or may have crashed.", programName, wid, programName);
				return errorMessage;
			}
			if (! displaySupplied)
				XCloseDisplay (display);
		} else
		#endif
		{
			/*
				No window ID given by Praat its pid file: use interrupt mechanism instead.
			*/
			if (kill (pid, SIGUSR1)) {
				sprintf (errorMessage, "Cannot send message to %s (process %ld). "
					"The program %s may have been started by a different user, "
					"or may have crashed.", programName, pid, programName);
				return errorMessage;
			}
		}
		/*
		 * Wait for the running program to notify us of completion,
		 * but do not wait for more than 'timeOut' seconds.
		 */
		if (timeOut) {
			signal (SIGALRM, handleTimeOut);
			alarm (timeOut);
			theTimeOut = timeOut;   // hand an argument to handleTimeOut () in a static variable
			errorMessage [0] = '\0';
			pause ();
			if (errorMessage [0] != '\0')
				return errorMessage;
		}
	#elif win
		/*
		 * Notify the running program by sending a WM_USER message to its main window.
		 */
		if (SendMessage (window, WM_USER, 0, 0)) {
			sprintf (errorMessage, "Program %s returns error.", programName);   // BUG?
			return errorMessage;
		}
	#elif mac
		/*
		 * Notify the running program by sending it an Apple event of the magic class 758934755.
		 */
		AECreateAppleEvent (758934755, 0, & programDescriptor, kAutoGenerateReturnID, 1, & event);
		AEPutParamPtr (& event, 1, typeChar, text, (Size) strlen (text) + 1);
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

/*
 * To compile sendpraat as a stand-alone program, use the -DSTAND_ALONE option to the C compiler:
 */
#ifdef STAND_ALONE
/*
	To compile on MacOS X:
		cc -o sendpraat -DSTAND_ALONE -framework CoreServices -I/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers -I/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers sendpraat.c -Dmacintosh

	To compile on Linux without X:
		gcc -std=gnu99 sendpraat.c -o sendpraat-signal -DSTAND_ALONE -DUNIX -DNO_GRAPHICS
	To compile on Linux with X:
		gcc -std=gnu99 sendpraat.c -o sendpraat-x -DSTAND_ALONE -DUNIX -L /usr/lib/x86_64-linux-gnu -lX11
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
		if (isdigit (argv [iarg] [0]))
			timeOut = atol (argv [iarg ++]);
	#endif

	/*
	 * Get program name.
	 */
	if (iarg == argc) {
		fprintf (stderr, "sendpraat: missing program name. Type \"sendpraat\" to get help.\n");
		exit (1);
	}
	strcpy (programName, argv [iarg ++]);

	/*
	 * Create the message string.
	 */
	for (line = iarg; line < argc; line ++)
		length += strlen (argv [line]) + 1;
	length --;
	message = malloc (length + 1);
	message [0] = '\0';
	for (line = iarg; line < argc; line ++) {
		strcat (message, argv [line]);
		if (line < argc - 1)
			strcat (message, "\n");
	}

	/*
	 * Send message.
	 */
	result = sendpraat (NULL, programName, timeOut, message);
	if (result != NULL) {
		fprintf (stderr, "sendpraat: %s\n", result);
		exit (1);
	}

	exit (0);
	return 0;
}
#endif

/* End of file sendpraat.c */
