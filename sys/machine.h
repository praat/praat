#ifndef _machine_h_
#define _machine_h_
/* machine.h
 *
 * Copyright (C) 1992-2005,2011,2015,2016,2019 Paul Boersma
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

void Machine_initLookAndFeel (int argc, char **argv);
int Machine_getMenuBarHeight ();
int Machine_getMainWindowMenuBarHeight ();
int Machine_getTitleBarHeight ();
int Machine_getScrollBarWidth ();
int Machine_getTextHeight ();
int Machine_getButtonHeight ();

/* End of file machine.h */
#endif
