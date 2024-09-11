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

FileInMemorySet theEspeakPraatFileInMemorySet();   // accessor to a singleton, which is created if it doesn't exist yet

/*
	The following six functions aren't here because they should be exported;
	they are here only because each of them is so big that it has to be in its own file.
	Had they been smaller, they would have been included as local functions in espeak_praat.cpp.
*/
void espeak_praat_FileInMemorySet_addPhon (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addRussianDict (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addFaroeseDict (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addOtherDicts (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addLanguages (FileInMemorySet me);
void espeak_praat_FileInMemorySet_addVoices (FileInMemorySet me);

/* End of file espeak_praat.h */
#endif
