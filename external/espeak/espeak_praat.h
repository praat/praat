#ifndef _espeak_praat_h_
#define _espeak_praat_h_

/* espeak_io.h
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
	
#define PATH_ESPEAK_DATA "./data"   /* a relative path */

autoFileInMemorySet create_espeak_all_FileInMemorySet ();
autoFileInMemorySet create_espeak_phon_FileInMemorySet ();
autoFileInMemorySet create_espeak_russianDict_FileInMemorySet ();
autoFileInMemorySet create_espeak_faroeseDict_FileInMemorySet ();
autoFileInMemorySet create_espeak_otherDicts_FileInMemorySet ();
autoFileInMemorySet create_espeak_languages_FileInMemorySet ();
autoFileInMemorySet create_espeak_voices_FileInMemorySet ();

FileInMemorySet theEspeakPraatFileInMemorySet();   // accessor to a singleton

int espeak_praat_GetFileLength (const char *filename);

void espeak_praat_GetVoices (const char *path, int len_path_voices, int is_language_file);

void espeak_ng_data_to_bigendian (void);

/* End of file espeak_praat.h */
#endif
