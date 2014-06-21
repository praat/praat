/* Movie.cpp
 *
 * Copyright (C) 2011-2012 Paul Boersma
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
	structData :: v_info ();
	MelderInfo_writeLine (L"Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"Time sampling:");
	MelderInfo_writeLine (L"   Number of frames: ", Melder_integer (nx));
	MelderInfo_writeLine (L"   Frame duration: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"   Frame rate: ", Melder_single (1.0 / dx), L" frames per second");
	MelderInfo_writeLine (L"   First frame centred at: ", Melder_double (x1), L" seconds");
}

void Movie_init (Movie me, Sound sound, const wchar_t *folderName, Strings fileNames)
{
	Sampled_init (me, sound -> xmin, sound -> xmax, fileNames ? fileNames -> numberOfStrings : 0, 0.04, 0.0);
	my d_sound = sound;
	my d_folderName = Melder_wcsdup (folderName);
	my d_fileNames = fileNames;
}

Movie Movie_openFromSoundFile (MelderFile file)
{
	try {
		autoMovie me = Thing_new (Movie);
		autoSound sound = Sound_readFromSoundFile (file);
		autoMelderString fileNameHead;
		MelderString_copy (& fileNameHead, Melder_fileToPath (file));
		wchar_t *extensionLocation = wcsrchr (fileNameHead.string, '.');
		if (extensionLocation == NULL)
			extensionLocation = & fileNameHead.string [fileNameHead.length];
		*extensionLocation = '\0';
		fileNameHead.length = extensionLocation - fileNameHead.string;
		autoStrings strings = Strings_createAsFileList (Melder_wcscat (fileNameHead.string, L"*.png"));
		struct structMelderDir folder;
		MelderFile_getParentDir (file, & folder);
		Movie_init (me.peek(), sound.transfer(), Melder_dirToPath (& folder), strings.transfer());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Movie object not read from file ", file, ".");
	}
}

void Movie_paintOneImageInside (Movie me, Graphics graphics, long frameNumber, double xmin, double xmax, double ymin, double ymax)
{
	try {
		if (frameNumber < 1) Melder_throw ("Specified frame number is ", frameNumber, " but should be at least 1.");
		if (frameNumber > my nx) Melder_throw ("Specified frame number is ", frameNumber, " but there are only ", my nx, "frames.");
		Melder_assert (my d_fileNames != 0);
		Melder_assert (my d_fileNames -> numberOfStrings == my nx);
		struct structMelderDir folder;
		Melder_pathToDir (my d_folderName, & folder);
		struct structMelderFile file;
		MelderDir_getFile (& folder, my d_fileNames -> strings [frameNumber], & file);
		Graphics_imageFromFile (graphics, Melder_fileToPath (& file), xmin, xmax, ymin, ymax);
	} catch (MelderError) {
		Melder_throw (me, ": image ", frameNumber, " not painted.");
	}
}

void Movie_paintOneImage (Movie me, Graphics graphics, long frameNumber, double xmin, double xmax, double ymin, double ymax) {
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

void Movie_play (Movie me, Graphics g, double tmin, double tmax, int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure)
{
	(void) g;
	Sound_playPart (my d_sound, tmin, tmax, callback, closure);
}

/* End of file Movie.cpp */
