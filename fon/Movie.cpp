/* Movie.cpp
 *
 * Copyright (C) 2011-2012,2015,2016,2017 Paul Boersma
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

#include "Movie.h"

#include "oo_DESTROY.h"
#include "Movie_def.h"
#include "oo_COPY.h"
#include "Movie_def.h"
#include "oo_EQUAL.h"
#include "Movie_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Movie_def.h"
#include "oo_WRITE_TEXT.h"
#include "Movie_def.h"
#include "oo_READ_TEXT.h"
#include "Movie_def.h"
#include "oo_WRITE_BINARY.h"
#include "Movie_def.h"
#include "oo_READ_BINARY.h"
#include "Movie_def.h"
#include "oo_DESCRIPTION.h"
#include "Movie_def.h"

Thing_implement (Movie, Sampled, 0);

void structMovie :: v_info ()
{
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", nx);
	MelderInfo_writeLine (U"   Frame duration: ", dx, U" seconds");
	MelderInfo_writeLine (U"   Frame rate: ", Melder_single (1.0 / dx), U" frames per second");
	MelderInfo_writeLine (U"   First frame centred at: ", x1, U" seconds");
}

void Movie_init (Movie me, autoSound sound, conststring32 folderName, autoStrings fileNames)
{
	Sampled_init (me, sound -> xmin, sound -> xmax, fileNames ? fileNames -> numberOfStrings : 0, 0.04, 0.0);
	my d_sound = sound.move();
	my d_folderName = Melder_dup (folderName);
	my d_fileNames = fileNames.move();
}

autoMovie Movie_openFromSoundFile (MelderFile file)
{
	try {
		autoMovie me = Thing_new (Movie);
		autoSound sound = Sound_readFromSoundFile (file);
		autoMelderString fileNameHead;
		MelderString_copy (& fileNameHead, Melder_fileToPath (file));
		char32 *extensionLocation = str32rchr (fileNameHead.string, U'.');
		if (! extensionLocation)
			extensionLocation = & fileNameHead.string [fileNameHead.length];
		*extensionLocation = U'\0';
		fileNameHead.length = extensionLocation - fileNameHead.string;
		autoStrings strings = Strings_createAsFileList (Melder_cat (fileNameHead.string, U"*.png"));
		structMelderDir folder { };
		MelderFile_getParentDir (file, & folder);
		Movie_init (me.get(), sound.move(), Melder_dirToPath (& folder), strings.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Movie object not read from file ", file, U".");
	}
}

void Movie_paintOneImageInside (Movie me, Graphics graphics, integer frameNumber, double xmin, double xmax, double ymin, double ymax)
{
	try {
		if (frameNumber < 1) Melder_throw (U"Specified frame number is ", frameNumber, U" but should be at least 1.");
		if (frameNumber > my nx) Melder_throw (U"Specified frame number is ", frameNumber, U" but there are only ", my nx, U"frames.");
		Melder_assert (my d_fileNames);
		Melder_assert (my d_fileNames -> numberOfStrings == my nx);
		structMelderDir folder { };
		Melder_pathToDir (my d_folderName.get(), & folder);
		structMelderFile file { };
		MelderDir_getFile (& folder, my d_fileNames -> strings [frameNumber].get(), & file);
		Graphics_imageFromFile (graphics, Melder_fileToPath (& file), xmin, xmax, ymin, ymax);
	} catch (MelderError) {
		Melder_throw (me, U": image ", frameNumber, U" not painted.");
	}
}

void Movie_paintOneImage (Movie me, Graphics graphics, integer frameNumber, double xmin, double xmax, double ymin, double ymax) {
	try {
		Graphics_setInner (graphics);
		Graphics_setWindow (graphics, 0.0, 1.0, 0.0, 1.0);
		Movie_paintOneImageInside (me, graphics, frameNumber, xmin, xmax, ymin, ymax);
		Graphics_unsetInner (graphics);
	} catch (MelderError) {
		Graphics_unsetInner (graphics);   // TODO: should be auto
		throw;
	}
}

void Movie_play (Movie me, Graphics /* g */, double tmin, double tmax, Sound_PlayCallback callback, Thing boss)
{
	Sound_playPart (my d_sound.get(), tmin, tmax, callback, boss);
}

/* End of file Movie.cpp */
