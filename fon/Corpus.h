#ifndef _Corpus_h_
#define _Corpus_h_
/* Corpus.h
 *
 * Copyright (C) 2011,2018 Paul Boersma
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

#include "Table.h"

#include "Corpus_def.h"

autoCorpus Corpus_create (const char32 *folderWithSoundFiles, const char32 *soundFileExtension,
	const char32 *folderWithAnnotationFiles, const char32 *annotationFileExtension);

#endif
/* End of file Corpus.h */
