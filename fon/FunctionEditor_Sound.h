#ifndef _FunctionEditor_Sound_h_
#define _FunctionEditor_Sound_h_
/* FunctionEditor_Sound.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 2001/04/05
 * pb 2002/07/16 GPL
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _LongSound_h_
	#include "LongSound.h"
#endif

void FunctionEditor_Sound_prefs (void);

void FunctionEditor_Sound_draw (I, double globalMinimum, double globalMaximum);

void FunctionEditor_Sound_createMenus (I);

void FunctionEditor_Sound_init (I);

/* End of file FunctionEditor_Sound.h */
#endif
