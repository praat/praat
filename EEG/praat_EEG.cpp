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

DIRECT (EEG_detrend)
	LOOP {
		iam (EEG);
		my f_detrend ();
		praat_dataChanged (me);
	}
END

FORM (EEG_editExternalElectrodeNames, L"Edit external electrode names", 0)
	WORD (L"External electrode 1", L"EXG1")
	WORD (L"External electrode 2", L"EXG2")
	WORD (L"External electrode 3", L"EXG3")
	WORD (L"External electrode 4", L"EXG4")
	WORD (L"External electrode 5", L"EXG5")
	WORD (L"External electrode 6", L"EXG6")
	WORD (L"External electrode 7", L"EXG7")
	WORD (L"External electrode 8", L"EXG8")
	OK
int IOBJECT;
LOOP {
	iam (EEG);
	SET_STRING (L"External electrode 1", my d_channelNames [my d_numberOfChannels - 15])
	SET_STRING (L"External electrode 2", my d_channelNames [my d_numberOfChannels - 14])
	SET_STRING (L"External electrode 3", my d_channelNames [my d_numberOfChannels - 13])
	SET_STRING (L"External electrode 4", my d_channelNames [my d_numberOfChannels - 12])
	SET_STRING (L"External electrode 5", my d_channelNames [my d_numberOfChannels - 11])
	SET_STRING (L"External electrode 6", my d_channelNames [my d_numberOfChannels - 10])
	SET_STRING (L"External electrode 7", my d_channelNames [my d_numberOfChannels -  9])
	SET_STRING (L"External electrode 8", my d_channelNames [my d_numberOfChannels -  8])
}
DO
	LOOP {
		iam (EEG);
		my f_setExternalElectrodeNames (GET_STRING (L"External electrode 1"), GET_STRING (L"External electrode 2"), GET_STRING (L"External electrode 3"),
			GET_STRING (L"External electrode 4"), GET_STRING (L"External electrode 5"), GET_STRING (L"External electrode 6"),
			GET_STRING (L"External electrode 7"), GET_STRING (L"External electrode 8"));
		praat_dataChanged (me);
	}
END

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

FORM (EEG_filter, L"Filter", 0)
	REAL (L"Low frequency (Hz)", L"1.0")
	REAL (L"Low width (Hz)", L"0.5")
	REAL (L"High frequency (Hz)", L"25.0")
	REAL (L"High width (Hz)", L"12.5")
	BOOLEAN (L"Notch at 50 Hz", true)
	OK
DO
	LOOP {
		iam (EEG);
		my f_filter (GET_REAL (L"Low frequency"), GET_REAL (L"Low width"), GET_REAL (L"High frequency"), GET_REAL (L"High width"), GET_INTEGER (L"Notch at 50 Hz"));
		praat_dataChanged (me);
	}
END

FORM (EEG_getChannelName, L"Get channel name", 0)
	NATURAL (L"Channel number", L"1")
	OK
DO
	LOOP {
		iam (EEG);
		long channelNumber = GET_INTEGER (L"Channel number");
		if (channelNumber > my d_numberOfChannels)
			Melder_throw (me, ": there are only ", my d_numberOfChannels, " channels.");
		Melder_information (my d_channelNames [channelNumber]);
	}
END

FORM (EEG_getChannelNumber, L"Get channel number", 0)
	WORD (L"Channel name", L"Cz")
	OK
DO
	LOOP {
		iam (EEG);
		Melder_information (Melder_integer (my f_getChannelNumber (GET_STRING (L"Channel name"))));
	}
END

FORM (EEG_setChannelName, L"Set channel name", 0)
	NATURAL (L"Channel number", L"1")
	WORD (L"New name", L"BLA")
	OK
DO
	LOOP {
		iam (EEG);
		my f_setChannelName (GET_INTEGER (L"Channel number"), GET_STRING (L"New name"));
		praat_dataChanged (me);
	}
END

FORM (EEG_subtractReference, L"Subtract reference", 0)
	WORD (L"Reference channel 1", L"MASL")
	WORD (L"Reference channel 2 (optional)", L"MASR")
	OK
DO
	LOOP {
		iam (EEG);
		my f_subtractReference (GET_STRING (L"Reference channel 1"), GET_STRING (L"Reference channel 2"));
		praat_dataChanged (me);
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
	praat_addAction1 (classEEG, 0, L"Query -", 0, 0, 0);
	praat_addAction1 (classEEG, 0, L"Get channel name...", 0, 1, DO_EEG_getChannelName);
	praat_addAction1 (classEEG, 0, L"Get channel number...", 0, 1, DO_EEG_getChannelNumber);
	praat_addAction1 (classEEG, 0, L"Modify -", 0, 0, 0);
	praat_addAction1 (classEEG, 0, L"Set channel name...", 0, 1, DO_EEG_setChannelName);
	praat_addAction1 (classEEG, 1, L"Edit external electrode names...", 0, 1, DO_EEG_editExternalElectrodeNames);
	praat_addAction1 (classEEG, 0, L"-- processing --", 0, 1, DO_EEG_detrend);
	praat_addAction1 (classEEG, 0, L"Detrend", 0, 1, DO_EEG_detrend);
	praat_addAction1 (classEEG, 0, L"Filter...", 0, 1, DO_EEG_filter);
	praat_addAction1 (classEEG, 1, L"Subtract reference...", 0, 1, DO_EEG_subtractReference);
}

/* End of file praat_Sound.cpp */
