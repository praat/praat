/* machine.cpp
 *
 * Copyright (C) 1992-2011,2012,2013 Paul Boersma
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
 * pb 2004/06/17 Machine_getMainWindowMenuBarHeight ()
 * pb 2011/05/15 C++
 * pb 2012/09/02 Cocoa
 */

#include "machine.h"
#include "melder.h"

#define LookAndFeel_MOTIF  0
#define LookAndFeel_SGI  1
#define LookAndFeel_CDE  2
#define LookAndFeel_SOLARIS  3
#define LookAndFeel_HP  4
#define LookAndFeel_SUN4  5
#define LookAndFeel_MAC  6
#define LookAndFeel_WIN32  7
#define LookAndFeel_LINUX  8
#define LookAndFeel_COCOA  9

static int lookAndFeel;

int Machine_getMenuBarHeight (void) {
	static int heights [] = {
		26,   /* Motif */
		24,   /* SGI */
		28,   /* CDE */
		26,   /* Solaris */
		26,   /* HP */
		26,   /* Sun4 */
		36,   /* Mac */
		0,    /* Win32 */
		30,   /* Linux */
		36    /* Cocoa */
	};
	return heights [lookAndFeel];
}

int Machine_getMainWindowMenuBarHeight (void) {
	#ifdef macintoshXXX
		return 0;
	#else
		return Machine_getMenuBarHeight ();
	#endif
}

int Machine_getTitleBarHeight (void) {
	static int heights [] = {   /* Mostly copied from menu-bar height. */
		26,   /* Motif */
		24,   /* SGI */
		28,   /* CDE */
		26,   /* Solaris */
		26,   /* HP */
		26,   /* Sun4 */
		22,   /* Mac */
		20,   /* Win32 */
		30,   /* Linux */
		22    /* Cocoa */
	};
	return heights [lookAndFeel];
}

int Machine_getScrollBarWidth (void) {
	static int widths [] = {
		22,   /* Motif */
		22,   /* SGI */
		22,   /* CDE */
		22,   /* Solaris */
		22,   /* HP */
		22,   /* Sun4 */
		16,   /* Mac */
		17,   /* Win32 */
		18,   /* Linux */
		16    /* Cocoa */
	};
	return widths [lookAndFeel];
}

int Machine_getTextHeight (void) {
	static int heights [] = {
		29,   /* Motif */
		29,   /* SGI */
		25,   /* CDE */
		29,   /* Solaris */
		29,   /* HP */
		29,   /* Sun4 */
		22,   /* Mac */
		20,   /* Win32 */
		25,   /* Linux */
		23    /* Cocoa */
	};
	return heights [lookAndFeel];
}

void Machine_initLookAndFeel (unsigned int argc, char **argv) {
	/*
	 * Determining the appropriate look-and-feel: the default depends on the client machine.
	 */
	#if defined (macintosh)
		#if useCarbon
			lookAndFeel = LookAndFeel_MAC;
		#else
			lookAndFeel = LookAndFeel_COCOA;
		#endif
		return;
	#elif defined (_WIN32)
		lookAndFeel = LookAndFeel_WIN32;
		return;
	#elif defined (linux)
		lookAndFeel = LookAndFeel_LINUX;
	#endif

	/*
	 * Command line may override the look-and-feel.
	 */
	if (argc > 1) {
		if (strequ (argv [1], "-sgi")) lookAndFeel = LookAndFeel_SGI;
		else if (strequ (argv [1], "-motif")) lookAndFeel = LookAndFeel_MOTIF;
		else if (strequ (argv [1], "-cde")) lookAndFeel = LookAndFeel_CDE;
		else if (strequ (argv [1], "-solaris")) lookAndFeel = LookAndFeel_SOLARIS;
		else if (strequ (argv [1], "-hp")) lookAndFeel = LookAndFeel_HP;
		else if (strequ (argv [1], "-sun4")) lookAndFeel = LookAndFeel_SUN4;
		else if (strequ (argv [1], "-mac")) lookAndFeel = LookAndFeel_MAC;
		else if (strequ (argv [1], "-linux")) lookAndFeel = LookAndFeel_LINUX;
		else if (strequ (argv [1], "-cocoa")) lookAndFeel = LookAndFeel_COCOA;
	}
}

/* End of file machine.cpp */

