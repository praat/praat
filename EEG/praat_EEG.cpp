/* praat_EEG.cpp
 *
 * Copyright (C) 2011-2012,2013,2014,2015 Paul Boersma
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

DIRECT2 (EEGs_concatenate) {
	autoCollection eegs = praat_getSelectedObjects ();
	autoEEG thee = EEGs_concatenate (eegs.peek());
	praat_new (thee.move(), U"chain");
END2 }

DIRECT2 (EEG_detrend) {
	LOOP {
		iam (EEG);
		EEG_detrend (me);
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_editExternalElectrodeNames, U"Edit external electrode names", 0) {
	WORD (U"External electrode 1", U"EXG1")
	WORD (U"External electrode 2", U"EXG2")
	WORD (U"External electrode 3", U"EXG3")
	WORD (U"External electrode 4", U"EXG4")
	WORD (U"External electrode 5", U"EXG5")
	WORD (U"External electrode 6", U"EXG6")
	WORD (U"External electrode 7", U"EXG7")
	WORD (U"External electrode 8", U"EXG8")
	OK2
int IOBJECT;
LOOP {
	iam (EEG);
	if (EEG_getNumberOfExternalElectrodes (me) == 8) {
		const long offsetExternalElectrode = EEG_getNumberOfCapElectrodes (me);
		SET_STRING (U"External electrode 1", my channelNames [offsetExternalElectrode + 1])
		SET_STRING (U"External electrode 2", my channelNames [offsetExternalElectrode + 2])
		SET_STRING (U"External electrode 3", my channelNames [offsetExternalElectrode + 3])
		SET_STRING (U"External electrode 4", my channelNames [offsetExternalElectrode + 4])
		SET_STRING (U"External electrode 5", my channelNames [offsetExternalElectrode + 5])
		SET_STRING (U"External electrode 6", my channelNames [offsetExternalElectrode + 6])
		SET_STRING (U"External electrode 7", my channelNames [offsetExternalElectrode + 7])
		SET_STRING (U"External electrode 8", my channelNames [offsetExternalElectrode + 8])
	}
}
DO
	LOOP {
		iam (EEG);
		if (EEG_getNumberOfExternalElectrodes (me) != 8)
			Melder_throw (U"You can do this only if there are 8 external electrodes.");
		EEG_setExternalElectrodeNames (me, GET_STRING (U"External electrode 1"), GET_STRING (U"External electrode 2"), GET_STRING (U"External electrode 3"),
			GET_STRING (U"External electrode 4"), GET_STRING (U"External electrode 5"), GET_STRING (U"External electrode 6"),
			GET_STRING (U"External electrode 7"), GET_STRING (U"External electrode 8"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_extractChannel, U"EEG: Extract channel", 0) {
	SENTENCE (U"Channel name", U"Cz")
	OK2
DO
	LOOP {
		iam (EEG);
		const char32 *channelName = GET_STRING (U"Channel name");
		autoEEG thee = EEG_extractChannel (me, channelName);
		praat_new (thee.move(), my name, U"_", channelName);
	}
END2 }

FORM (EEG_extractPart, U"EEG: Extract part", 0) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	BOOLEAN (U"Preserve times", 0)
	OK2
DO
	LOOP {
		iam (EEG);
		autoEEG thee = EEG_extractPart (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Preserve times"));
		praat_new (thee.move(), my name, U"_part");
	}
END2 }

DIRECT2 (EEG_extractSound) {
	LOOP {
		iam (EEG);
		if (! my sound) Melder_throw (me, U": I don't contain a waveform.");
		autoSound thee = EEG_extractSound (me);
		praat_new (thee.move());
	}
END2 }

DIRECT2 (EEG_extractTextGrid) {
	LOOP {
		iam (EEG);
		if (! my textgrid) Melder_throw (me, U": I don't contain marks.");
		autoTextGrid thee = EEG_extractTextGrid (me);
		praat_new (thee.move());
	}
END2 }

FORM (EEG_filter, U"Filter", 0) {
	REAL (U"Low frequency (Hz)", U"1.0")
	REAL (U"Low width (Hz)", U"0.5")
	REAL (U"High frequency (Hz)", U"25.0")
	REAL (U"High width (Hz)", U"12.5")
	BOOLEAN (U"Notch at 50 Hz", true)
	OK2
DO
	LOOP {
		iam (EEG);
		EEG_filter (me, GET_REAL (U"Low frequency"), GET_REAL (U"Low width"), GET_REAL (U"High frequency"), GET_REAL (U"High width"), GET_INTEGER (U"Notch at 50 Hz"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_getChannelName, U"Get channel name", 0) {
	NATURAL (U"Channel number", U"1")
	OK2
DO
	LOOP {
		iam (EEG);
		long channelNumber = GET_INTEGER (U"Channel number");
		if (channelNumber > my numberOfChannels)
			Melder_throw (me, U": there are only ", my numberOfChannels, U" channels.");
		Melder_information (my channelNames [channelNumber]);
	}
END2 }

FORM (EEG_getChannelNumber, U"Get channel number", 0) {
	WORD (U"Channel name", U"Cz")
	OK2
DO
	LOOP {
		iam (EEG);
		Melder_information (EEG_getChannelNumber (me, GET_STRING (U"Channel name")));
	}
END2 }

FORM (EEG_removeTriggers, U"Remove triggers", 0) {
	OPTIONMENU_ENUM (U"Remove every trigger that...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	LOOP {
		iam (EEG);
		EEG_removeTriggers (me, GET_ENUM (kMelder_string, U"Remove every trigger that..."), GET_STRING (U"...the text"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_setChannelName, U"Set channel name", 0) {
	NATURAL (U"Channel number", U"1")
	WORD (U"New name", U"BLA")
	OK2
DO
	LOOP {
		iam (EEG);
		EEG_setChannelName (me, GET_INTEGER (U"Channel number"), GET_STRING (U"New name"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_setChannelToZero, U"Set channel to zero", 0) {
	SENTENCE (U"Channel", U"Iz")
	OK2
DO
	LOOP {
		iam (EEG);
		EEG_setChannelToZero (me, GET_STRING (U"Channel"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_subtractMeanChannel, U"Subtract mean channel", 0) {
	LABEL (U"label", U"Range of reference channels:")
	NATURAL (U"From channel", U"1")
	NATURAL (U"To channel", U"32")
	OK2
DO
	LOOP {
		iam (EEG);
		EEG_subtractMeanChannel (me, GET_INTEGER (U"From channel"), GET_INTEGER (U"To channel"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_subtractReference, U"Subtract reference", 0) {
	WORD (U"Reference channel 1", U"MASL")
	WORD (U"Reference channel 2 (optional)", U"MASR")
	OK2
DO
	LOOP {
		iam (EEG);
		EEG_subtractReference (me, GET_STRING (U"Reference channel 1"), GET_STRING (U"Reference channel 2"));
		praat_dataChanged (me);
	}
END2 }

FORM (EEG_to_ERPTier_bit, U"To ERPTier (bit)", 0) {
	REAL (U"From time (s)", U"-0.11")
	REAL (U"To time (s)", U"0.39")
	NATURAL (U"Marker bit", U"8")
	OK2
DO
	LOOP {
		iam (EEG);
		int markerBit = GET_INTEGER (U"Marker bit");
		autoERPTier thee = EEG_to_ERPTier_bit (me, GET_REAL (U"From time"), GET_REAL (U"To time"), markerBit);
		praat_new (thee.move(), my name, U"_bit", markerBit);
	}
END2 }

FORM (EEG_to_ERPTier_marker, U"To ERPTier (marker)", 0) {
	REAL (U"From time (s)", U"-0.11")
	REAL (U"To time (s)", U"0.39")
	NATURAL (U"Marker number", U"12")
	OK2
DO
	LOOP {
		iam (EEG);
		uint16 markerNumber = GET_INTEGER (U"Marker number");
		autoERPTier thee = EEG_to_ERPTier_marker (me, GET_REAL (U"From time"), GET_REAL (U"To time"), markerNumber);
		praat_new (thee.move(), my name, U"_", markerNumber);
	}
END2 }

FORM (EEG_to_ERPTier_triggers, U"To ERPTier (triggers)", 0) {
	REAL (U"From time (s)", U"-0.11")
	REAL (U"To time (s)", U"0.39")
	OPTIONMENU_ENUM (U"Get every event with a trigger that", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"1")
	OK2
DO
	LOOP {
		iam (EEG);
		autoERPTier thee = EEG_to_ERPTier_triggers (me, GET_REAL (U"From time"), GET_REAL (U"To time"),
			GET_ENUM (kMelder_string, U"Get every event with a trigger that"), GET_STRING (U"...the text"));
		praat_new (thee.move(), my name, U"_trigger", GET_STRING (U"...the text"));
	}
END2 }

FORM (EEG_to_ERPTier_triggers_preceded, U"To ERPTier (triggers, preceded)", 0) {
	REAL (U"From time (s)", U"-0.11")
	REAL (U"To time (s)", U"0.39")
	OPTIONMENU_ENUM (U"Get every event with a trigger that", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"1")
	OPTIONMENU_ENUM (U"and is preceded by a trigger that", kMelder_string, DEFAULT)
	SENTENCE (U" ...the text", U"4")
	OK2
DO
	LOOP {
		iam (EEG);
		autoERPTier thee = EEG_to_ERPTier_triggers_preceded (me, GET_REAL (U"From time"), GET_REAL (U"To time"),
			GET_ENUM (kMelder_string, U"Get every event with a trigger that"), GET_STRING (U"...the text"),
			GET_ENUM (kMelder_string, U"and is preceded by a trigger that"), GET_STRING (U" ...the text"));
		praat_new (thee.move(), my name, U"_trigger", GET_STRING (U" ...the text"));
	}
END2 }

FORM (EEG_to_MixingMatrix, U"To MixingMatrix", 0) {
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK2
DO
	LOOP {
		iam (EEG);
		autoMixingMatrix thee = EEG_to_MixingMatrix (me,
			GET_INTEGER (U"Maximum number of iterations"), GET_REAL (U"Tolerance"),
			GET_INTEGER (U"Diagonalization method"));
		praat_new (thee.move(), my name);
	}
END2 }

static void cb_EEGWindow_publication (Editor editor, void *closure, Daata publication) {
	(void) editor;
	(void) closure;
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publication);
		praat_updateSelection ();
		if (Thing_isa (publication, classSpectrum) && str32equ (Thing_getName (publication), U"slice")) {
			int IOBJECT;
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.get(), IOBJECT);
				editor2.releaseToUser();
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT2 (EEG_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an EEG from batch.");
	LOOP {
		iam (EEG);
		autoEEGWindow editor = EEGWindow_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.peek(), cb_EEGWindow_publication, nullptr);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

#pragma mark EEG & TextGrid

DIRECT2 (EEG_TextGrid_replaceTextGrid) {
	EEG me = FIRST (EEG);
	EEG_replaceTextGrid (me, FIRST (TextGrid));
	praat_dataChanged (me);
END2 }

#pragma mark ERP

DIRECT2 (ERP_downto_Sound) {
	LOOP {
		iam (ERP);
		autoSound thee = ERP_downto_Sound (me);
		praat_new (thee.move());
	}
END2 }

FORM (ERP_downto_Table, U"ERP: Down to Table", 0) {
	BOOLEAN (U"Include sample number", false)
	BOOLEAN (U"Include time", true)
	NATURAL (U"Time decimals", U"6")
	NATURAL (U"Voltage decimals", U"12")
	RADIO (U"Voltage units", 1)
		OPTION (U"volt")
		OPTION (U"microvolt")
	OK2
DO
	LOOP {
		iam (ERP);
		autoTable thee = ERP_tabulate (me, GET_INTEGER (U"Include sample number"),
			GET_INTEGER (U"Include time"), GET_INTEGER (U"Time decimals"), GET_INTEGER (U"Voltage decimals"), GET_INTEGER (U"Voltage units"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (ERP_draw, U"ERP: Draw", 0) {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range", U"0.0 (= all)")
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		ERP_drawChannel_name (me, GRAPHICS, GET_STRING (U"Channel name"), GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (ERP_drawScalp, U"ERP: Draw scalp", 0) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range", U"0.2")
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		ERP_drawScalp (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), kGraphics_colourScale_GREY, GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (ERP_drawScalp_colour, U"ERP: Draw scalp (colour)", 0) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range", U"0.2")
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	RADIO_ENUM (U"Colour scale", kGraphics_colourScale, BLUE_TO_RED)
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		ERP_drawScalp (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), GET_ENUM (kGraphics_colourScale, U"Colour scale"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (ERP_drawScalp_garnish, U"ERP: Draw scalp (garnish)", 0) {
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	RADIO_ENUM (U"Colour scale", kGraphics_colourScale, BLUE_TO_RED)
	OK2
DO
	autoPraatPicture picture;
	ERP_drawScalp_garnish (GRAPHICS,
		GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), GET_ENUM (kGraphics_colourScale, U"Colour scale"));
END2 }

FORM (ERP_extractOneChannelAsSound, U"ERP: Extract one channel as Sound", 0) {
	WORD (U"Channel name", U"Cz")
	OK2
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		autoSound thee = Sound_extractChannel (me, channelNumber);
		praat_new (thee.move(), my name, U"_", channelName);
	}
END2 }

FORM (ERP_formula, U"ERP: Formula", U"ERP: Formula...") {
	LABEL (U"label1", U"! `x' is the time in seconds, `col' is the sample number.")
	LABEL (U"label2", U"x = x1   ! time associated with first sample")
	LABEL (U"label3", U"for col from 1 to ncol")
	LABEL (U"label4", U"   self [col] = ...")
	TEXTFIELD (U"formula", U"self")
	LABEL (U"label5", U"   x = x + dx")
	LABEL (U"label6", U"endfor")
	OK2
DO
	LOOP {
		iam (ERP);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the ERP may have partially changed
			throw;
		}
	}
END2 }

FORM (ERP_formula_part, U"ERP: Formula (part)", U"ERP: Formula...") {
	REAL (U"From time", U"0.0")
	REAL (U"To time", U"0.0 (= all)")
	NATURAL (U"From channel", U"1")
	NATURAL (U"To channel", U"2")
	TEXTFIELD (U"formula", U"2 * self")
	OK2
DO
	LOOP {
		iam (ERP);
		try {
			Matrix_formula_part (me,
				GET_REAL (U"From time"), GET_REAL (U"To time"),
				GET_INTEGER (U"From channel") - 0.5, GET_INTEGER (U"To channel") + 0.5,
				GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the ERP may have partially changed
			throw;
		}
	}
END2 }

FORM (ERP_getChannelName, U"Get channel name", 0) {
	NATURAL (U"Channel number", U"1")
	OK2
DO
	LOOP {
		iam (ERP);
		long channelNumber = GET_INTEGER (U"Channel number");
		if (channelNumber > my ny)
			Melder_throw (me, U": there are only ", my ny, U" channels.");
		Melder_information (my channelNames [channelNumber]);
	}
END2 }

FORM (ERP_getChannelNumber, U"Get channel number", 0) {
	WORD (U"Channel name", U"Cz")
	OK2
DO
	LOOP {
		iam (ERP);
		Melder_information (ERP_getChannelNumber (me, GET_STRING (U"Channel name")));
	}
END2 }

FORM (ERP_getMaximum, U"ERP: Get maximum", U"Sound: Get maximum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double maximum;
		Vector_getMaximumAndX (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber, GET_INTEGER (U"Interpolation") - 1, & maximum, nullptr);
		Melder_informationReal (maximum, U"Volt");
	}
END2 }

FORM (ERP_getMean, U"ERP: Get mean", U"ERP: Get mean...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double mean = Vector_getMean (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber);
		Melder_informationReal (mean, U"Volt");
	}
END2 }

FORM (ERP_getMinimum, U"ERP: Get minimum", U"Sound: Get minimum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double minimum;
		Vector_getMinimumAndX (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber, GET_INTEGER (U"Interpolation") - 1, & minimum, nullptr);
		Melder_informationReal (minimum, U"Volt");
	}
END2 }

FORM (ERP_getTimeOfMaximum, U"ERP: Get time of maximum", U"Sound: Get time of maximum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double timeOfMaximum;
		Vector_getMaximumAndX (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber, GET_INTEGER (U"Interpolation") - 1, nullptr, & timeOfMaximum);
		Melder_informationReal (timeOfMaximum, U"seconds");
	}
END2 }

FORM (ERP_getTimeOfMinimum, U"ERP: Get time of minimum", U"Sound: Get time of minimum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double timeOfMinimum;
		Vector_getMinimumAndX (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber, GET_INTEGER (U"Interpolation") - 1, nullptr, & timeOfMinimum);
		Melder_informationReal (timeOfMinimum, U"seconds");
	}
END2 }

static void cb_ERPWindow_publication (Editor editor, void *closure, Daata publication) {
	(void) editor;
	(void) closure;
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publication);
		praat_updateSelection ();
		if (Thing_isa (publication, classSpectrum) && str32equ (Thing_getName (publication), U"slice")) {
			int IOBJECT;
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.get(), IOBJECT);
				editor2.releaseToUser();
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT2 (ERP_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an ERP from batch.");
	LOOP {
		iam (ERP);
		autoERPWindow editor = ERPWindow_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.peek(), cb_ERPWindow_publication, nullptr);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

/***** ERPTier *****/

FORM (ERPTier_getChannelName, U"Get channel name", 0) {
	NATURAL (U"Channel number", U"1")
	OK2
DO
	LOOP {
		iam (ERPTier);
		long channelNumber = GET_INTEGER (U"Channel number");
		if (channelNumber > my numberOfChannels)
			Melder_throw (me, U": there are only ", my numberOfChannels, U" channels.");
		Melder_information (my channelNames [channelNumber]);
	}
END2 }

FORM (ERPTier_getChannelNumber, U"Get channel number", 0) {
	WORD (U"Channel name", U"Cz")
	OK2
DO
	LOOP {
		iam (ERPTier);
		Melder_information (ERPTier_getChannelNumber (me, GET_STRING (U"Channel name")));
	}
END2 }

FORM (ERPTier_getMean, U"ERPTier: Get mean", U"ERPTier: Get mean...") {
	NATURAL (U"Point number", U"1")
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (ERPTier);
		double mean = ERPTier_getMean (me, GET_INTEGER (U"Point number"), GET_STRING (U"Channel name"), GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (mean, U"Volt");
	}
END2 }

FORM (ERPTier_rejectArtefacts, U"Reject artefacts", 0) {
	POSITIVE (U"Threshold (V)", U"75e-6")
	OK2
DO
	LOOP {
		iam (ERPTier);
		ERPTier_rejectArtefacts (me, GET_REAL (U"Threshold"));
		praat_dataChanged (me);
	}
END2 }

FORM (ERPTier_removeEventsBetween, U"Remove events", U"ERPTier: Remove events between...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	OK2
DO
	LOOP {
		iam (ERPTier);
		AnyTier_removePointsBetween (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		praat_dataChanged (me);
	}
END2 }

FORM (ERPTier_subtractBaseline, U"Subtract baseline", 0) {
	REAL (U"From time (s)", U"-0.11")
	REAL (U"To time (s)", U"0.0")
	OK2
DO
	LOOP {
		iam (ERPTier);
		ERPTier_subtractBaseline (me, GET_REAL (U"From time"), GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END2 }

FORM (ERPTier_to_ERP, U"ERPTier: To ERP", 0) {
	NATURAL (U"Event number", U"1")
	OK2
DO
	LOOP {
		iam (ERPTier);
		autoERP thee = ERPTier_extractERP (me, GET_INTEGER (U"Event number"));
		praat_new (thee.move(), my name, U"_mean");
	}
END2 }

DIRECT2 (ERPTier_to_ERP_mean) {
	LOOP {
		iam (ERPTier);
		autoERP thee = ERPTier_to_ERP_mean (me);
		praat_new (thee.move(), my name, U"_mean");
	}
END2 }

/***** ERPTier & Table *****/

FORM (ERPTier_Table_extractEventsWhereColumn_number, U"Extract events where column (number)", 0) {
	WORD (U"Extract all events where column...", U"")
	RADIO_ENUM (U"...is...", kMelder_number, DEFAULT)
	REAL (U"...the number", U"0.0")
	OK2
DO
	ERPTier erpTier = FIRST (ERPTier);
	Table table = FIRST (Table);
	long columnNumber = Table_getColumnIndexFromColumnLabel (table, GET_STRING (U"Extract all events where column..."));
	autoERPTier thee = ERPTier_extractEventsWhereColumn_number (erpTier, table, columnNumber, GET_ENUM (kMelder_number, U"...is..."), GET_REAL (U"...the number"));
	praat_new (thee.move(), erpTier -> name);
END2 }

FORM (ERPTier_Table_extractEventsWhereColumn_text, U"Extract events where column (text)", 0) {
	WORD (U"Extract all events where column...", U"")
	OPTIONMENU_ENUM (U"...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	ERPTier erpTier = FIRST (ERPTier);
	Table table = FIRST (Table);
	long columnNumber = Table_getColumnIndexFromColumnLabel (table, GET_STRING (U"Extract all events where column..."));
	autoERPTier thee = ERPTier_extractEventsWhereColumn_string (erpTier, table, columnNumber, GET_ENUM (kMelder_string, U"..."), GET_STRING (U"...the text"));
	praat_new (thee.move(), erpTier -> name);
END2 }

/***** Help menus *****/

DIRECT2 (EEG_help) {
	Melder_help (U"EEG");
END2 }

DIRECT2 (ERPTier_help) {
	Melder_help (U"ERPTier");
END2 }

/***** file recognizers *****/

static autoDaata bdfFileRecognizer (int nread, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	bool isBdfFile = Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".bdf") ||
	                 Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".BDF");
	bool isEdfFile = Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".edf") ||
	                 Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".EDF");
	if (nread < 512 || (! isBdfFile && ! isEdfFile)) return autoDaata ();
	return EEG_readFromBdfFile (file);
}

/***** buttons *****/

void praat_TimeTier_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of AnyTier.

void praat_EEG_init ();
void praat_EEG_init () {

	Thing_recognizeClassesByName (classEEG, classERPTier, classERP, nullptr);

	Data_recognizeFileType (bdfFileRecognizer);

	praat_addAction1 (classEEG, 0, U"EEG help", nullptr, 0, DO_EEG_help);
	praat_addAction1 (classEEG, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_EEG_viewAndEdit);
	praat_addAction1 (classEEG, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Get channel name...", nullptr, 1, DO_EEG_getChannelName);
		praat_addAction1 (classEEG, 0, U"Get channel number...", nullptr, 1, DO_EEG_getChannelNumber);
	praat_addAction1 (classEEG, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Set channel name...", nullptr, 1, DO_EEG_setChannelName);
		praat_addAction1 (classEEG, 1, U"Edit external electrode names...", nullptr, 1, DO_EEG_editExternalElectrodeNames);
		praat_addAction1 (classEEG, 0, U"-- processing --", nullptr, 1, nullptr);
		praat_addAction1 (classEEG, 0, U"Subtract reference...", nullptr, 1, DO_EEG_subtractReference);
		praat_addAction1 (classEEG, 0, U"Subtract mean channel...", nullptr, 1, DO_EEG_subtractMeanChannel);
		praat_addAction1 (classEEG, 0, U"Detrend", nullptr, 1, DO_EEG_detrend);
		praat_addAction1 (classEEG, 0, U"Filter...", nullptr, 1, DO_EEG_filter);
		praat_addAction1 (classEEG, 0, U"Remove triggers...", nullptr, 1, DO_EEG_removeTriggers);
		praat_addAction1 (classEEG, 0, U"Set channel to zero...", nullptr, 1, DO_EEG_setChannelToZero);
	praat_addAction1 (classEEG, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Extract channel...", nullptr, 0, DO_EEG_extractChannel);
		praat_addAction1 (classEEG, 1, U"Extract part...", nullptr, 0, DO_EEG_extractPart);
		praat_addAction1 (classEEG, 0, U"To ERPTier -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"To ERPTier (bit)...", nullptr, 1, DO_EEG_to_ERPTier_bit);
		praat_addAction1 (classEEG, 0, U"To ERPTier (marker)...", nullptr, 1, DO_EEG_to_ERPTier_marker);
		praat_addAction1 (classEEG, 0, U"To ERPTier (triggers)...", nullptr, 1, DO_EEG_to_ERPTier_triggers);
		praat_addAction1 (classEEG, 0, U"To ERPTier (triggers, preceded)...", nullptr, 1, DO_EEG_to_ERPTier_triggers_preceded);
		praat_addAction1 (classEEG, 0, U"To ERPTier...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_EEG_to_ERPTier_bit);
		praat_addAction1 (classEEG, 0, U"To MixingMatrix...", nullptr, 0, DO_EEG_to_MixingMatrix);
	praat_addAction1 (classEEG, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Concatenate", nullptr, 0, DO_EEGs_concatenate);
	praat_addAction1 (classEEG, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Extract waveforms as Sound", nullptr, 1, DO_EEG_extractSound);
		praat_addAction1 (classEEG, 0, U"Extract marks as TextGrid", nullptr, 1, DO_EEG_extractTextGrid);

	praat_addAction1 (classERP, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_ERP_viewAndEdit);
	praat_addAction1 (classERP, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Draw...", nullptr, 1, DO_ERP_draw);
		praat_addAction1 (classERP, 0, U"Draw scalp...", nullptr, 1, DO_ERP_drawScalp);
		praat_addAction1 (classERP, 0, U"Draw scalp (colour)...", nullptr, 1, DO_ERP_drawScalp_colour);
		praat_addAction1 (classERP, 0, U"Draw scalp (garnish)...", nullptr, 1, DO_ERP_drawScalp_garnish);
	praat_addAction1 (classERP, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Down to Table...", nullptr, 1, DO_ERP_downto_Table);
	praat_addAction1 (classERP, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Get channel name...", nullptr, 1, DO_ERP_getChannelName);
		praat_addAction1 (classERP, 0, U"Get channel number...", nullptr, 1, DO_ERP_getChannelNumber);
		praat_addAction1 (classERP, 0, U"-- get shape --", nullptr, 1, nullptr);
		praat_addAction1 (classERP, 0, U"Get minimum...", nullptr, 1, DO_ERP_getMinimum);
		praat_addAction1 (classERP, 0, U"Get time of minimum...", nullptr, 1, DO_ERP_getTimeOfMinimum);
		praat_addAction1 (classERP, 0, U"Get maximum...", nullptr, 1, DO_ERP_getMaximum);
		praat_addAction1 (classERP, 0, U"Get time of maximum...", nullptr, 1, DO_ERP_getTimeOfMaximum);
		praat_addAction1 (classERP, 0, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classERP, 0, U"Get mean...", nullptr, 1, DO_ERP_getMean);
	praat_addAction1 (classERP, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Formula...", nullptr, 1, DO_ERP_formula);
		praat_addAction1 (classERP, 0, U"Formula (part)...", nullptr, 1, DO_ERP_formula_part);
	// praat_addAction1 (classERP, 0, U"Analyse -", nullptr, 0, nullptr);
		// praat_addAction1 (classERP, 0, U"To ERP (difference)", nullptr, 1, DO_ERP_to_ERP_difference);
		// praat_addAction1 (classERP, 0, U"To ERP (mean)", nullptr, 1, DO_ERP_to_ERP_mean);
	praat_addAction1 (classERP, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Down to Sound", nullptr, 1, DO_ERP_downto_Sound);
		praat_addAction1 (classERP, 0, U"Extract one channel as Sound...", nullptr, 1, DO_ERP_extractOneChannelAsSound);

	praat_addAction1 (classERPTier, 0, U"ERPTier help", nullptr, 0, DO_ERPTier_help);
	// praat_addAction1 (classERPTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_ERPTier_viewAndEdit);
	praat_addAction1 (classERPTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classERPTier);
		praat_addAction1 (classERPTier, 0, U"-- channel names --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Get channel name...", nullptr, 1, DO_ERPTier_getChannelName);
		praat_addAction1 (classERPTier, 0, U"Get channel number...", nullptr, 1, DO_ERPTier_getChannelNumber);
		praat_addAction1 (classERPTier, 0, U"-- erp --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Get mean...", nullptr, 1, DO_ERPTier_getMean);
	praat_addAction1 (classERPTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classERPTier, 0, U"Subtract baseline...", nullptr, 1, DO_ERPTier_subtractBaseline);
		praat_addAction1 (classERPTier, 0, U"Reject artefacts...", nullptr, 1, DO_ERPTier_rejectArtefacts);
		praat_addAction1 (classERPTier, 0, U"-- structure --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Remove events between...", nullptr, 1, DO_ERPTier_removeEventsBetween);
	praat_addAction1 (classERPTier, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classERPTier, 0, U"Extract ERP...", nullptr, 0, DO_ERPTier_to_ERP);
		praat_addAction1 (classERPTier, 0, U"To ERP (mean)", nullptr, 0, DO_ERPTier_to_ERP_mean);

	praat_addAction2 (classEEG, 1, classTextGrid, 1, U"Replace TextGrid", nullptr, 0, DO_EEG_TextGrid_replaceTextGrid);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract -", nullptr, 0, nullptr);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract events where column (number)...", nullptr, 1, DO_ERPTier_Table_extractEventsWhereColumn_number);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract events where column (text)...", nullptr, 1, DO_ERPTier_Table_extractEventsWhereColumn_text);

	structEEGWindow :: f_preferences ();
	structERPWindow :: f_preferences ();
}

/* End of file praat_EEG.cpp */
