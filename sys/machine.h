/* machine.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/06/17
 */

void Machine_initLookAndFeel (unsigned int argc, char **argv);
char **Machine_getXresources (void);
#ifndef UNIX
	#define Machine_getXresources()  0
#endif
int Machine_getMenuBarHeight (void);
int Machine_getMainWindowMenuBarHeight (void);
int Machine_getTitleBarHeight (void);
int Machine_getScrollBarWidth (void);
int Machine_getTextHeight (void);

/* End of file machine.h */
