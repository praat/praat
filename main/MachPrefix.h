/* MachPrefix.h */
/* Paul Boersma, March 14, 2002 */

/* Prefix file for Mach-O version. */
/* Will be included before any system headers. */
/*
	In CodeWarrior, this file has to be mentioned
	as the Prefix File in Language Settings: C/C++ Language.
*/

#define TARGET_API_MAC_CARBON  1

/*
	By default, be a Mac.
	Otherwise, the symbol __MACH__ has to be used.
*/
#define macintosh
