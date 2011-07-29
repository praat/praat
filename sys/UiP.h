/* UiP.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * Private stuff for communication between Ui.c and UiFile.cpp.
 */

#include "Ui.h"

extern bool (*theAllowExecutionHookHint) (void *closure);
extern void *theAllowExecutionClosureHint;
typedef struct structUiFileSelector *UiFileSelector;
UiFileSelector UiFileSelector_create (GuiObject parent, const char *label, int direction, const char *defaultValue);

/* End of file UiP.h */
