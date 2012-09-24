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

void structMovie :: f_init (Sound sound, const wchar_t *folderName, Strings fileNames)
{
	Sampled_init (this, sound -> xmin, sound -> xmax, fileNames ? fileNames -> numberOfStrings : 0, 0.04, 0.0);
	d_sound = sound;
	d_folderName = Melder_wcsdup (folderName);
	d_fileNames = fileNames;
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
		my f_init (sound.transfer(), Melder_dirToPath (& folder), strings.transfer());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Movie object not read from file ", file, ".");
	}
}

void structMovie :: f_paintOneImageInside (Graphics graphics, long frameNumber, double a_xmin, double a_xmax, double a_ymin, double a_ymax)
{
	try {
		if (frameNumber < 1) Melder_throw ("Specified frame number is ", frameNumber, " but should be at least 1.");
		if (frameNumber > nx) Melder_throw ("Specified frame number is ", frameNumber, " but there are only ", nx, "frames.");
		Melder_assert (d_fileNames != 0);
		Melder_assert (d_fileNames -> numberOfStrings == nx);
		struct structMelderDir folder;
		Melder_pathToDir (d_folderName, & folder);
		struct structMelderFile file;
		MelderDir_getFile (& folder, d_fileNames -> strings [frameNumber], & file);
		Graphics_imageFromFile (graphics, Melder_fileToPath (& file), a_xmin, a_xmax, a_ymin, a_ymax);
	} catch (MelderError) {
		Melder_throw (this, ": image ", frameNumber, " not painted.");
	}
}

void structMovie :: f_paintOneImage (Graphics graphics, long frameNumber, double a_xmin, double a_xmax, double a_ymin, double a_ymax) {
	try {
		Graphics_setInner (graphics);
		Graphics_setWindow (graphics, 0.0, 1.0, 0.0, 1.0);
		f_paintOneImageInside (graphics, frameNumber, a_xmin, a_xmax, a_ymin, a_ymax);
		Graphics_unsetInner (graphics);
	} catch (MelderError) {
		Graphics_unsetInner (graphics);   // TODO: should be auto
		throw;
	}
}

void structMovie :: f_play (Graphics g, double tmin, double tmax, int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure)
{
	Sound_playPart (d_sound, tmin, tmax, callback, closure);
}

/* End of file Movie.cpp */
