/* praat_EEG.cpp
 *
 * Copyright (C) 2011 Paul Boersma
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

#include "praat.h"

#include "EEG.h"

#undef iam
#define iam iam_LOOP

/***** EEG *****/

DIRECT (EEG_extractSound)
	LOOP {
		iam (EEG);
		if (! my d_sound) Melder_throw (me, ": I don't contain a waveform.");
		autoSound thee = my extractSound ();
		praat_new (thee.transfer(), NULL);
	}
END

DIRECT (EEG_extractTextGrid)
	LOOP {
		iam (EEG);
		if (! my d_textgrid) Melder_throw (me, ": I don't contain marks.");
		autoTextGrid thee = my extractTextGrid ();
		praat_new (thee.transfer(), NULL);
	}
END

/***** Help menus *****/

DIRECT (EEG_help) Melder_help (L"EEG"); END

/***** file recognizers *****/

static Any bdfFileRecognizer (int nread, const char *header, MelderFile file) {
	const wchar *fileName = MelderFile_name (file);
	bool isBdfFile = wcsstr (fileName, L".bdf") != NULL || wcsstr (fileName, L".BDF") != NULL;
	bool isEdfFile = wcsstr (fileName, L".edf") != NULL || wcsstr (fileName, L".EDF") != NULL;
	if (nread < 512 || (! isBdfFile && ! isEdfFile)) return NULL;
	return EEG_readFromBdfFile (file);
}

/***** buttons *****/

void praat_EEG_init (void);
void praat_EEG_init (void) {

	Thing_recognizeClassesByName (classEEG, NULL);

	Data_recognizeFileType (bdfFileRecognizer);

	praat_addAction1 (classEEG, 0, L"EEG help", 0, 0, DO_EEG_help);
	praat_addAction1 (classEEG, 0, L"Extract waveforms as Sound", 0, 0, DO_EEG_extractSound);
	praat_addAction1 (classEEG, 0, L"Extract marks as TextGrid", 0, 0, DO_EEG_extractTextGrid);

}

/* End of file praat_Sound.cpp */
