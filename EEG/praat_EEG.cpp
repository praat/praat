/* praat_EEG.cpp
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

#include "praat.h"

#include "EEGWindow.h"
#include "ERPWindow.h"
#include "ERPTier.h"
#include "SpectrumEditor.h"

#undef iam
#define iam iam_LOOP

/***** EEG *****/

DIRECT (EEGs_concatenate)
	autoCollection eegs = praat_getSelectedObjects ();
	autoEEG thee = EEGs_concatenate (eegs.peek());
	praat_new (thee.transfer(), L"chain");
END

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

FORM (EEG_extractChannel, L"EEG: Extract channel", 0)
	SENTENCE (L"Channel name", L"Cz")
	OK
DO
	LOOP {
		iam (EEG);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		autoEEG thee = my f_extractChannel (channelName);
		praat_new (thee.transfer(), my name, L"_", channelName);
	}
END

FORM (EEG_extractPart, L"EEG: Extract part", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	LOOP {
		iam (EEG);
		autoEEG thee = my f_extractPart (GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Preserve times"));
		praat_new (thee.transfer(), my name, L"_part");
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

FORM (EEG_setChannelToZero, L"Set channel to zero", 0)
	SENTENCE (L"Channel", L"Iz")
	OK
DO
	LOOP {
		iam (EEG);
		my f_setChannelToZero (GET_STRING (L"Channel"));
		praat_dataChanged (me);
	}
END

FORM (EEG_subtractMeanChannel, L"Subtract mean channel", 0)
	LABEL (L"label", L"Range of reference channels:")
	NATURAL (L"From channel", L"1")
	NATURAL (L"To channel", L"32")
	OK
DO
	LOOP {
		iam (EEG);
		my f_subtractMeanChannel (GET_INTEGER (L"From channel"), GET_INTEGER (L"To channel"));
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

FORM (EEG_to_ERPTier, L"To ERPTier", 0)
	REAL (L"From time (s)", L"-0.11")
	REAL (L"To time (s)", L"0.39")
	NATURAL (L"Marker bit", L"8")
	OK
DO
	LOOP {
		iam (EEG);
		int markerBit = GET_INTEGER (L"Marker bit");
		autoERPTier thee = EEG_to_ERPTier (me, GET_REAL (L"From time"), GET_REAL (L"To time"), markerBit);
		praat_new (thee.transfer(), my name, L"_bit", Melder_integer (markerBit));
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
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.transfer(), IOBJECT);
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
		autoEEGWindow editor = EEGWindow_create (ID_AND_FULL_NAME, me);
		editor -> setPublicationCallback (cb_EEGWindow_publication, NULL);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

#pragma mark EEG & TextGrid

DIRECT (EEG_TextGrid_replaceTextGrid)
	EEG me = FIRST (EEG);
	me -> f_replaceTextGrid (FIRST (TextGrid));
	praat_dataChanged (me);
END

#pragma mark ERP

DIRECT (ERP_downto_Sound)
	LOOP {
		iam (ERP);
		autoSound thee = my f_downToSound ();
		praat_new (thee.transfer(), NULL);
	}
END

FORM (ERP_downto_Table, L"ERP: Down to Table", 0)
	BOOLEAN (L"Include sample number", false)
	BOOLEAN (L"Include time", true)
	NATURAL (L"Time decimals", L"6")
	NATURAL (L"Voltage decimals", L"12")
	RADIO (L"Voltage units", 1)
		OPTION (L"volt")
		OPTION (L"microvolt")
	OK
DO
	LOOP {
		iam (ERP);
		autoTable thee = my f_tabulate (GET_INTEGER (L"Include sample number"),
			GET_INTEGER (L"Include time"), GET_INTEGER (L"Time decimals"), GET_INTEGER (L"Voltage decimals"), GET_INTEGER (L"Voltage units"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (ERP_draw, L"ERP: Draw", 0)
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range", L"0.0 (= all)")
	REAL (L"left Voltage range (V)", L"10e-6")
	REAL (L"right Voltage range", L"-10e-6")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		me -> f_draw (GRAPHICS, GET_STRING (L"Channel name"), GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Voltage range"), GET_REAL (L"right Voltage range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (ERP_drawScalp, L"ERP: Draw scalp", 0)
	REAL (L"left Time range (s)", L"0.1")
	REAL (L"right Time range", L"0.2")
	REAL (L"left Voltage range (V)", L"10e-6")
	REAL (L"right Voltage range", L"-10e-6")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		me -> f_drawScalp (GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Voltage range"), GET_REAL (L"right Voltage range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (ERP_extractOneChannelAsSound, L"ERP: Extract one channel as Sound", 0)
	WORD (L"Channel name", L"Cz")
	OK
DO
	LOOP {
		iam (ERP);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		long channelNumber = my f_getChannelNumber (channelName);
		if (channelNumber == 0) Melder_throw (me, ": no channel named \"", channelName, "\".");
		autoSound thee = Sound_extractChannel (me, channelNumber);
		praat_new (thee.transfer(), my name, L"_", channelName);
	}
END

FORM (ERP_formula, L"ERP: Formula", L"ERP: Formula...")
	LABEL (L"label1", L"! `x' is the time in seconds, `col' is the sample number.")
	LABEL (L"label2", L"x = x1   ! time associated with first sample")
	LABEL (L"label3", L"for col from 1 to ncol")
	LABEL (L"label4", L"   self [col] = ...")
	TEXTFIELD (L"formula", L"self")
	LABEL (L"label5", L"   x = x + dx")
	LABEL (L"label6", L"endfor")
	OK
DO
	LOOP {
		iam (ERP);
		try {
			Matrix_formula (me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the ERP may have partially changed
			throw;
		}
	}
END

FORM (ERP_formula_part, L"ERP: Formula (part)", L"ERP: Formula...")
	REAL (L"From time", L"0.0")
	REAL (L"To time", L"0.0 (= all)")
	NATURAL (L"From channel", L"1")
	NATURAL (L"To channel", L"2")
	TEXTFIELD (L"formula", L"2 * self")
	OK
DO
	LOOP {
		iam (ERP);
		try {
			Matrix_formula_part (me,
				GET_REAL (L"From time"), GET_REAL (L"To time"),
				GET_INTEGER (L"From channel") - 0.5, GET_INTEGER (L"To channel") + 0.5,
				GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the ERP may have partially changed
			throw;
		}
	}
END

FORM (ERP_getChannelName, L"Get channel name", 0)
	NATURAL (L"Channel number", L"1")
	OK
DO
	LOOP {
		iam (ERP);
		long channelNumber = GET_INTEGER (L"Channel number");
		if (channelNumber > my ny)
			Melder_throw (me, ": there are only ", my ny, " channels.");
		Melder_information (my d_channelNames [channelNumber]);
	}
END

FORM (ERP_getChannelNumber, L"Get channel number", 0)
	WORD (L"Channel name", L"Cz")
	OK
DO
	LOOP {
		iam (ERP);
		Melder_information (Melder_integer (my f_getChannelNumber (GET_STRING (L"Channel name"))));
	}
END

FORM (ERP_getMaximum, L"ERP: Get maximum", L"Sound: Get maximum...")
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (ERP);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		long channelNumber = my f_getChannelNumber (channelName);
		if (channelNumber == 0) Melder_throw (me, ": no channel named \"", channelName, "\".");
		double maximum;
		Vector_getMaximumAndX (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channelNumber, GET_INTEGER (L"Interpolation") - 1, & maximum, NULL);
		Melder_informationReal (maximum, L"Volt");
	}
END

FORM (ERP_getMean, L"ERP: Get mean", L"ERP: Get mean...")
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (ERP);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		long channelNumber = my f_getChannelNumber (channelName);
		if (channelNumber == 0) Melder_throw (me, ": no channel named \"", channelName, "\".");
		double mean = Vector_getMean (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channelNumber);
		Melder_informationReal (mean, L"Volt");
	}
END

FORM (ERP_getMinimum, L"ERP: Get minimum", L"Sound: Get minimum...")
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (ERP);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		long channelNumber = my f_getChannelNumber (channelName);
		if (channelNumber == 0) Melder_throw (me, ": no channel named \"", channelName, "\".");
		double minimum;
		Vector_getMinimumAndX (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channelNumber, GET_INTEGER (L"Interpolation") - 1, & minimum, NULL);
		Melder_informationReal (minimum, L"Volt");
	}
END

FORM (ERP_getTimeOfMaximum, L"ERP: Get time of maximum", L"Sound: Get time of maximum...")
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (ERP);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		long channelNumber = my f_getChannelNumber (channelName);
		if (channelNumber == 0) Melder_throw (me, ": no channel named \"", channelName, "\".");
		double timeOfMaximum;
		Vector_getMaximumAndX (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channelNumber, GET_INTEGER (L"Interpolation") - 1, NULL, & timeOfMaximum);
		Melder_informationReal (timeOfMaximum, L"seconds");
	}
END

FORM (ERP_getTimeOfMinimum, L"ERP: Get time of minimum", L"Sound: Get time of minimum...")
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (ERP);
		const wchar_t *channelName = GET_STRING (L"Channel name");
		long channelNumber = my f_getChannelNumber (channelName);
		if (channelNumber == 0) Melder_throw (me, ": no channel named \"", channelName, "\".");
		double timeOfMinimum;
		Vector_getMinimumAndX (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channelNumber, GET_INTEGER (L"Interpolation") - 1, NULL, & timeOfMinimum);
		Melder_informationReal (timeOfMinimum, L"seconds");
	}
END

static void cb_ERPWindow_publication (Editor editor, void *closure, Data publication) {
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
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.transfer(), IOBJECT);
			}
		}
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}
DIRECT (ERP_viewAndEdit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit an ERP from batch.");
	LOOP {
		iam (ERP);
		autoERPWindow editor = ERPWindow_create (ID_AND_FULL_NAME, me);
		editor -> setPublicationCallback (cb_ERPWindow_publication, NULL);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

/***** ERPTier *****/

FORM (ERPTier_getChannelName, L"Get channel name", 0)
	NATURAL (L"Channel number", L"1")
	OK
DO
	LOOP {
		iam (ERPTier);
		long channelNumber = GET_INTEGER (L"Channel number");
		if (channelNumber > my d_numberOfChannels)
			Melder_throw (me, ": there are only ", my d_numberOfChannels, " channels.");
		Melder_information (my d_channelNames [channelNumber]);
	}
END

FORM (ERPTier_getChannelNumber, L"Get channel number", 0)
	WORD (L"Channel name", L"Cz")
	OK
DO
	LOOP {
		iam (ERPTier);
		Melder_information (Melder_integer (my f_getChannelNumber (GET_STRING (L"Channel name"))));
	}
END

FORM (ERPTier_getMean, L"ERPTier: Get mean", L"ERPTier: Get mean...")
	NATURAL (L"Point number", L"1")
	SENTENCE (L"Channel name", L"Cz")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (ERPTier);
		double mean = my f_getMean (GET_INTEGER (L"Point number"), GET_STRING (L"Channel name"), GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (mean, L"Volt");
	}
END

FORM (ERPTier_rejectArtefacts, L"Reject artefacts", 0)
	POSITIVE (L"Threshold (V)", L"75e-6")
	OK
DO
	LOOP {
		iam (ERPTier);
		my f_rejectArtefacts (GET_REAL (L"Threshold"));
		praat_dataChanged (me);
	}
END

FORM (ERPTier_removeEventsBetween, L"Remove events", L"ERPTier: Remove events between...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	OK
DO
	LOOP {
		iam (ERPTier);
		AnyTier_removePointsBetween (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		praat_dataChanged (me);
	}
END

FORM (ERPTier_subtractBaseline, L"Subtract baseline", 0)
	REAL (L"From time (s)", L"-0.11")
	REAL (L"To time (s)", L"0.0")
	OK
DO
	LOOP {
		iam (ERPTier);
		my f_subtractBaseline (GET_REAL (L"From time"), GET_REAL (L"To time"));
		praat_dataChanged (me);
	}
END

FORM (ERPTier_to_ERP, L"ERPTier: To ERP", 0)
	NATURAL (L"Event number", L"1")
	OK
DO
	LOOP {
		iam (ERPTier);
		autoERP thee = my f_extractERP (GET_INTEGER (L"Event number"));
		praat_new (thee.transfer(), my name, L"_mean");
	}
END

DIRECT (ERPTier_to_ERP_mean)
	LOOP {
		iam (ERPTier);
		autoERP thee = my f_toERP_mean ();
		praat_new (thee.transfer(), my name, L"_mean");
	}
END

/***** ERPTier & Table *****/

FORM (ERPTier_Table_extractEventsWhereColumn_number, L"Extract events where column (number)", 0)
	WORD (L"Extract all events where column...", L"")
	RADIO_ENUM (L"...is...", kMelder_number, DEFAULT)
	REAL (L"...the number", L"0.0")
	OK
DO
	ERPTier erpTier = FIRST (ERPTier);
	Table table = FIRST (Table);
	long columnNumber = Table_getColumnIndexFromColumnLabel (table, GET_STRING (L"Extract all events where column..."));
	autoERPTier thee = erpTier -> f_extractEventsWhereColumn_number (table, columnNumber, GET_ENUM (kMelder_number, L"...is..."), GET_REAL (L"...the number"));
	praat_new (thee.transfer(), erpTier -> name);
END

FORM (ERPTier_Table_extractEventsWhereColumn_text, L"Extract events where column (text)", 0)
	WORD (L"Extract all events where column...", L"")
	OPTIONMENU_ENUM (L"...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	ERPTier erpTier = FIRST (ERPTier);
	Table table = FIRST (Table);
	long columnNumber = Table_getColumnIndexFromColumnLabel (table, GET_STRING (L"Extract all events where column..."));
	autoERPTier thee = erpTier -> f_extractEventsWhereColumn_string (table, columnNumber, GET_ENUM (kMelder_string, L"..."), GET_STRING (L"...the text"));
	praat_new (thee.transfer(), erpTier -> name);
END

/***** Help menus *****/

DIRECT (EEG_help)     Melder_help (L"EEG");     END
DIRECT (ERPTier_help) Melder_help (L"ERPTier"); END

/***** file recognizers *****/

static Any bdfFileRecognizer (int nread, const char *header, MelderFile file) {
	const wchar_t *fileName = MelderFile_name (file);
	bool isBdfFile = wcsstr (fileName, L".bdf") != NULL || wcsstr (fileName, L".BDF") != NULL;
	bool isEdfFile = wcsstr (fileName, L".edf") != NULL || wcsstr (fileName, L".EDF") != NULL;
	if (nread < 512 || (! isBdfFile && ! isEdfFile)) return NULL;
	return EEG_readFromBdfFile (file);
}

/***** buttons *****/

void praat_TimeTier_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of AnyTier.

void praat_EEG_init (void);
void praat_EEG_init (void) {

	Thing_recognizeClassesByName (classEEG, classERPTier, classERP, NULL);

	Data_recognizeFileType (bdfFileRecognizer);

	praat_addAction1 (classEEG, 0, L"EEG help", 0, 0, DO_EEG_help);
	praat_addAction1 (classEEG, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_EEG_viewAndEdit);
	praat_addAction1 (classEEG, 0, L"Query -", 0, 0, 0);
		praat_addAction1 (classEEG, 0, L"Get channel name...", 0, 1, DO_EEG_getChannelName);
		praat_addAction1 (classEEG, 0, L"Get channel number...", 0, 1, DO_EEG_getChannelNumber);
	praat_addAction1 (classEEG, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classEEG, 0, L"Set channel name...", 0, 1, DO_EEG_setChannelName);
		praat_addAction1 (classEEG, 1, L"Edit external electrode names...", 0, 1, DO_EEG_editExternalElectrodeNames);
		praat_addAction1 (classEEG, 0, L"-- processing --", 0, 1, DO_EEG_detrend);
		praat_addAction1 (classEEG, 0, L"Subtract reference...", 0, 1, DO_EEG_subtractReference);
		praat_addAction1 (classEEG, 0, L"Subtract mean channel...", 0, 1, DO_EEG_subtractMeanChannel);
		praat_addAction1 (classEEG, 0, L"Detrend", 0, 1, DO_EEG_detrend);
		praat_addAction1 (classEEG, 0, L"Filter...", 0, 1, DO_EEG_filter);
		praat_addAction1 (classEEG, 0, L"Set channel to zero...", 0, 1, DO_EEG_setChannelToZero);
	praat_addAction1 (classEEG, 0, L"Analyse", 0, 0, 0);
		praat_addAction1 (classEEG, 0, L"Extract channel...", 0, 0, DO_EEG_extractChannel);
		praat_addAction1 (classEEG, 1, L"Extract part...", 0, 0, DO_EEG_extractPart);
		praat_addAction1 (classEEG, 0, L"To ERPTier...", 0, 0, DO_EEG_to_ERPTier);
	praat_addAction1 (classEEG, 0, L"Synthesize", 0, 0, 0);
		praat_addAction1 (classEEG, 0, L"Concatenate", 0, 0, DO_EEGs_concatenate);
	praat_addAction1 (classEEG, 0, L"Hack -", 0, 0, 0);
		praat_addAction1 (classEEG, 0, L"Extract waveforms as Sound", 0, 1, DO_EEG_extractSound);
		praat_addAction1 (classEEG, 0, L"Extract marks as TextGrid", 0, 1, DO_EEG_extractTextGrid);

	praat_addAction1 (classERP, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_ERP_viewAndEdit);
	praat_addAction1 (classERP, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classERP, 0, L"Draw...", 0, 1, DO_ERP_draw);
		praat_addAction1 (classERP, 0, L"Draw scalp...", 0, 1, DO_ERP_drawScalp);
	praat_addAction1 (classERP, 0, L"Tabulate -", 0, 0, 0);
		praat_addAction1 (classERP, 0, L"Down to Table...", 0, 1, DO_ERP_downto_Table);
	praat_addAction1 (classERP, 0, L"Query -", 0, 0, 0);
		praat_addAction1 (classERP, 0, L"Get channel name...", 0, 1, DO_ERP_getChannelName);
		praat_addAction1 (classERP, 0, L"Get channel number...", 0, 1, DO_ERP_getChannelNumber);
		praat_addAction1 (classERP, 0, L"-- get shape --", 0, 1, 0);
		praat_addAction1 (classERP, 0, L"Get minimum...", 0, 1, DO_ERP_getMinimum);
		praat_addAction1 (classERP, 0, L"Get time of minimum...", 0, 1, DO_ERP_getTimeOfMinimum);
		praat_addAction1 (classERP, 0, L"Get maximum...", 0, 1, DO_ERP_getMaximum);
		praat_addAction1 (classERP, 0, L"Get time of maximum...", 0, 1, DO_ERP_getTimeOfMaximum);
		praat_addAction1 (classERP, 0, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classERP, 0, L"Get mean...", 0, 1, DO_ERP_getMean);
	praat_addAction1 (classERP, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classERP, 0, L"Formula...", 0, 1, DO_ERP_formula);
		praat_addAction1 (classERP, 0, L"Formula (part)...", 0, 1, DO_ERP_formula_part);
	// praat_addAction1 (classERP, 0, L"Analyse -", 0, 0, 0);
		// praat_addAction1 (classERP, 0, L"To ERP (difference)", 0, 1, DO_ERP_to_ERP_difference);
		// praat_addAction1 (classERP, 0, L"To ERP (mean)", 0, 1, DO_ERP_to_ERP_mean);
	praat_addAction1 (classERP, 0, L"Hack -", 0, 0, 0);
		praat_addAction1 (classERP, 0, L"Down to Sound", 0, 1, DO_ERP_downto_Sound);
		praat_addAction1 (classERP, 0, L"Extract one channel as Sound...", 0, 1, DO_ERP_extractOneChannelAsSound);

	praat_addAction1 (classERPTier, 0, L"ERPTier help", 0, 0, DO_ERPTier_help);
	// praat_addAction1 (classERPTier, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_ERPTier_viewAndEdit);
	praat_addAction1 (classERPTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classERPTier);
		praat_addAction1 (classERPTier, 0, L"-- channel names --", 0, 1, 0);
		praat_addAction1 (classERPTier, 0, L"Get channel name...", 0, 1, DO_ERPTier_getChannelName);
		praat_addAction1 (classERPTier, 0, L"Get channel number...", 0, 1, DO_ERPTier_getChannelNumber);
		praat_addAction1 (classERPTier, 0, L"-- erp --", 0, 1, 0);
		praat_addAction1 (classERPTier, 0, L"Get mean...", 0, 1, DO_ERPTier_getMean);
	praat_addAction1 (classERPTier, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classERPTier, 0, L"Subtract baseline...", 0, 1, DO_ERPTier_subtractBaseline);
		praat_addAction1 (classERPTier, 0, L"Reject artefacts...", 0, 1, DO_ERPTier_rejectArtefacts);
		praat_addAction1 (classERPTier, 0, L"-- structure --", 0, 1, 0);
		praat_addAction1 (classERPTier, 0, L"Remove events between...", 0, 1, DO_ERPTier_removeEventsBetween);
	praat_addAction1 (classERPTier, 0, L"Analyse", 0, 0, 0);
		praat_addAction1 (classERPTier, 0, L"Extract ERP...", 0, 0, DO_ERPTier_to_ERP);
		praat_addAction1 (classERPTier, 0, L"To ERP (mean)", 0, 0, DO_ERPTier_to_ERP_mean);

	praat_addAction2 (classEEG, 1, classTextGrid, 1, L"Replace TextGrid", 0, 0, DO_EEG_TextGrid_replaceTextGrid);
	praat_addAction2 (classERPTier, 1, classTable, 1, L"Extract -", 0, 0, 0);
	praat_addAction2 (classERPTier, 1, classTable, 1, L"Extract events where column (number)...", 0, 1, DO_ERPTier_Table_extractEventsWhereColumn_number);
	praat_addAction2 (classERPTier, 1, classTable, 1, L"Extract events where column (text)...", 0, 1, DO_ERPTier_Table_extractEventsWhereColumn_text);

	structEEGWindow :: f_preferences ();
	structERPWindow :: f_preferences ();
}

/* End of file praat_EEG.cpp */
