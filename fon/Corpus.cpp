/* Corpus.cpp
 *
 * Copyright (C) 2011,2026,2018,2020 Paul Boersma
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

#include "Corpus.h"

#include "oo_DESTROY.h"
#include "Corpus_def.h"
#include "oo_COPY.h"
#include "Corpus_def.h"
#include "oo_EQUAL.h"
#include "Corpus_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Corpus_def.h"
#include "oo_WRITE_TEXT.h"
#include "Corpus_def.h"
#include "oo_READ_TEXT.h"
#include "Corpus_def.h"
#include "oo_WRITE_BINARY.h"
#include "Corpus_def.h"
#include "oo_READ_BINARY.h"
#include "Corpus_def.h"
#include "oo_DESCRIPTION.h"
#include "Corpus_def.h"

Thing_implement (Corpus, Table, 0);

autoCorpus Corpus_create (conststring32 folderWithSoundFiles, conststring32 soundFileExtension,
	conststring32 folderWithAnnotationFiles, conststring32 annotationFileExtension)
{
	autoCorpus me = Thing_new (Corpus);
	my folderWithSoundFiles = Melder_dup (folderWithSoundFiles);
	if (folderWithAnnotationFiles [0] == U'\0')
		folderWithAnnotationFiles = folderWithSoundFiles;
	my folderWithAnnotationFiles = Melder_dup (folderWithAnnotationFiles);
	autoSTRVEC fileList = fileNames_STRVEC (Melder_cat (folderWithSoundFiles, U"/*.", soundFileExtension));
	Table_initWithColumnNames (me.get(), fileList.size, U"Sound Annotation");
	autoMelderString annotationFileName;
	for (integer ifile = 1; ifile <= fileList.size; ifile ++) {
		conststring32 soundFileName = fileList [ifile].get();
		Table_setStringValue (me.get(), ifile, 1, soundFileName);
		const char32 *dotLocation = str32rchr (soundFileName, U'.');
		Melder_assert (!! dotLocation);
		MelderString_ncopy (& annotationFileName, soundFileName, dotLocation - soundFileName + 1);
		MelderString_append (& annotationFileName, annotationFileExtension);
		structMelderFile annotationFile { };
		Melder_pathToFile (Melder_cat (folderWithAnnotationFiles, U"/", annotationFileName.string), & annotationFile);
		if (MelderFile_exists (& annotationFile))
			Table_setStringValue (me.get(), ifile, 2, annotationFileName.string);
	}
	return me;
}

/* End of file Corpus.cpp */
