#ifndef _SoundRecorder_h_
#define _SoundRecorder_h_
/* SoundRecorder.h
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2008/06/17
 */

/* An editor-like object that allows the user to record sounds. */

#ifndef _Editor_h_
	#include "Editor.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#define SoundRecorder__parents(Klas) Editor__parents(Klas) Thing_inherit (Klas, Editor)
Thing_declare1 (SoundRecorder);

SoundRecorder SoundRecorder_create (GuiObject parent, int numberOfChannels, void *applicationContext);
/*
	Function:
		create a SoundRecorder, which is an interactive window
		for recording in 16-bit mono or stereo (SGI, MacOS, SunOS, HPUX, Linux, Windows).
*/

void SoundRecorder_prefs (void);
int SoundRecorder_getBufferSizePref_MB (void);
void SoundRecorder_setBufferSizePref_MB (int size);

#endif
/* End of file SoundRecorder.h */
