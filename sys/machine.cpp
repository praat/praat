/* machine.cpp
 *
 * Copyright (C) 1992-2005,2011-2013,2015,2016,2019,2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
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
#define LookAndFeel_CHROME  10

static int lookAndFeel;

int Machine_getMenuBarHeight () {
	static int heights [] = {
		26,   // Motif
		24,   // SGI
		28,   // CDE
		26,   // Solaris
		26,   // HP
		26,   // Sun4
		36,   // Mac
		0,    // Win32
		30,   // Linux
		36,   // Cocoa
		30,   // Chrome
	};
	return heights [lookAndFeel];
}

int Machine_getMainWindowMenuBarHeight () {
	#ifdef macintoshXXX
		return 0;
	#else
		return Machine_getMenuBarHeight ();
	#endif
}

int Machine_getTitleBarHeight () {
	static int heights [] = {   /* Mostly copied from menu-bar height. */
		26,   // Motif
		24,   // SGI
		28,   // CDE
		26,   // Solaris
		26,   // HP
		26,   // Sun4
		22,   // Mac
		20,   // Win32
		30,   // Linux
		22,   // Cocoa
		30,   // Chrome
	};
	return heights [lookAndFeel];
}

int Machine_getScrollBarWidth () {
	static int widths [] = {
		22,   // Motif
		22,   // SGI
		22,   // CDE
		22,   // Solaris
		22,   // HP
		22,   // Sun4
		16,   // Mac
		17,   // Win32
		18,   // Linux
		16,   // Cocoa
		24,   // Chrome
	};
	return widths [lookAndFeel];
}

int Machine_getTextHeight () {
	static int heights [] = {
		29,   // Motif
		29,   // SGI
		25,   // CDE
		29,   // Solaris
		29,   // HP
		29,   // Sun4
		22,   // Mac
		20,   // Win32
		25,   // Linux
		23,   // Cocoa
		31,   // Chrome
	};
	return heights [lookAndFeel];
}

int Machine_getButtonHeight () {
	static int heights [] = {
		20,   // Motif
		20,   // SGI
		20,   // CDE
		20,   // Solaris
		20,   // HP
		20,   // Sun4
		20,   // Mac
		20,   // Win32
		30,   // Linux
		20,   // Cocoa
		40,   // Chrome
	};
	return heights [lookAndFeel];
}

void Machine_initLookAndFeel (int argc, char **argv) {
	/*
		Determining the appropriate look-and-feel: the default depends on the client machine.
	*/
	#if defined (macintosh)
		lookAndFeel = LookAndFeel_COCOA;
		return;
	#elif defined (_WIN32)
		lookAndFeel = LookAndFeel_WIN32;
		return;
	#elif defined (linux)
		#if defined (chrome)
			lookAndFeel = LookAndFeel_CHROME;
		#else
			lookAndFeel = LookAndFeel_LINUX;
		#endif
	#endif

	/*
		Command line may override the look-and-feel.
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
		else if (strequ (argv [1], "-chrome")) lookAndFeel = LookAndFeel_CHROME;
	}
}

/* End of file machine.cpp */
