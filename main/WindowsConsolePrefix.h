/* WindowsPrefix.h */
/* Paul Boersma, December 23, 2002 */

/* Prefix file for Windows version (with graphical user interface). */
/* Will be included before any system headers. */
/*
	In CodeWarrior, this file has to be mentioned
	as the Prefix File in Language Settings: C/C++ Language.
*/

#include "ansi_prefix.Win32.h"

/*
 * Make sure that we know at compile time that we are in a console application.
 */
#define CONSOLE_APPLICATION
