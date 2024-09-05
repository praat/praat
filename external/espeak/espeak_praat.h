#ifndef _espeak_praat_h_
#define _espeak_praat_h_

/* espeak_praat.h
 * Copyright (C) 2017-2024 David Weenink, 2024 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FileInMemory.h"
#include "Table.h"

FileInMemorySet theEspeakPraatFileInMemorySet();   // accessor to a singleton, which is created if it doesn't exist yet

/*
	The following six functions aren't here because they should be exported;
	they are here only because they each is so big that it has to be in its own file.
	Had they been smaller, the woul dhave been included as local functions in espeak_praat.cpp.
*/
void espeak_praat_FileInMemorySet_addPhon (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addRussianDict (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addFaroeseDict (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addOtherDicts (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addLanguages (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addVoices (FileInMemorySet me);

/*
	For general use.
*/
STRVEC theEspeakPraatLanguageNames();
STRVEC theEspeakPraatVoiceNames();

/*
	For developers mainly.
*/
Table theEspeakPraatLanguagePropertiesTable();
Table theEspeakPraatVoicePropertiesTable();

/*
	Create the eSpeak-Praat-FileInMemorySet and the four other objects mentioned above.
*/
void espeakdata_praat_init ();

/*
	Drop-in replacements for GetFileLength and GetVoices (using FileInMemory instead of FILE*).
*/
int espeak_praat_GetFileLength (const char *filename);
void espeak_praat_GetVoices (const char *path, int len_path_voices, int is_language_file);

/* End of file espeak_praat.h */
#endif
