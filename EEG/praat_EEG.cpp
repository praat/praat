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

#include "EEGWindow.h"
#include "SpectrumEditor.h"

#undef iam
#define iam iam_LOOP

/***** EEG *****/

DIRECT (EEG_extractSound)
	LOOP {
		iam (EEG);
		if (! my d_sound) Melder_throw (me, ": I don't contain a waveform.");
		autoSound thee = my f_extractSound ();
		praat_new (thee.transfer(), NULL);
	}
END

DIRECT (EEG_extractTextGrid)
	LOOP {
		iam (EEG);
		if (! my d_textgrid) Melder_throw (me, ": I don't contain marks.");
		autoTextGrid thee = my f_extractTextGrid ();
		praat_new (thee.transfer(), NULL);
	}
END

static void cb_EEGWindow_publication (Editor editor, void *closure, Data publication) {
	(void) editor;
	(void) closure;
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publication, NULL);
		praat_updateSelection ();
		if (Thing_member (publication, classSpectrum) && wcsequ (Thing_getName (publication), L"slice")) {
			int IOBJECT;
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.transfer(), IOBJECT); therror
			}
		}
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}
DIRECT (EEG_viewAndEdit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit an EEG from batch.");
	LOOP {
		iam (EEG);
		autoEEGWindow editor = EEGWindow_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		editor -> setPublicationCallback (cb_EEGWindow_publication, NULL);
		praat_installEditor (editor.transfer(), IOBJECT); therror
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
	praat_addAction1 (classEEG, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_EEG_viewAndEdit);
	praat_addAction1 (classEEG, 0, L"Extract waveforms as Sound", 0, 0, DO_EEG_extractSound);
	praat_addAction1 (classEEG, 0, L"Extract marks as TextGrid", 0, 0, DO_EEG_extractTextGrid);

}

/* End of file praat_Sound.cpp */
