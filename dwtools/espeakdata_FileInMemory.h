#ifndef _espeakdata_FileInMemory_h_
#define _espeakdata_FileInMemory_h_

/* espeakdata_FileInMemory.h
 * Copyright (C) David Weenink 2012-2017, Paul Boersma 2024
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

#include "espeak_praat.h"
#include "Table.h"

inline autoStrings espeakdata_languages_names;
inline autoStrings espeakdata_voices_names;
inline autoTable espeakdata_languages_propertiesTable;
inline autoTable espeakdata_voices_propertiesTable;

/*
	Create the espeak_praat_FileInMemorySet and the four objects mentioned above.
*/
void espeakdata_praat_init ();

autoTable Table_createAsEspeakLanguageProperties ();
autoTable Table_createAsEspeakVoiceProperties ();

void espeakdata_getIndices (conststring32 language_string, conststring32 voice_string, int *p_languageIndex, int *p_voiceIndex);

#endif
