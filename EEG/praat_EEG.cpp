/* praat_EEG.cpp
 *
 * Copyright (C) 2011-2012,2013,2014,2015,2016 Paul Boersma
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

#include "EEGWindow.h"
#include "ERPWindow.h"
#include "ERPTier.h"
#include "SpectrumEditor.h"

#include "praat_TimeTier.h"

#undef iam
#define iam iam_LOOP

// MARK: - EEG

// MARK: Help

DIRECT (HELP_EEG_help) {
	HELP (U"EEG")
}

// MARK: View & Edit

static void cb_EEGWindow_publication (Editor /* editor */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		bool isaSpectralSlice = Thing_isa (publication.get(), classSpectrum) && str32equ (Thing_getName (publication.get()), U"slice");
		praat_new (publication.move());
		praat_updateSelection ();
		if (isaSpectralSlice) {
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
DIRECT (WINDOW_EEG_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an EEG from batch.");
	LOOP {
		iam (EEG);
		autoEEGWindow editor = EEGWindow_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_EEGWindow_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: Query

FORM (STRING_EEG_getChannelName, U"Get channel name", nullptr) {
	NATURALVAR (channelNumber, U"Channel number", U"1")
	OK
DO
	STRING_ONE (EEG)
		if (channelNumber > my numberOfChannels)
			Melder_throw (me, U": there are only ", my numberOfChannels, U" channels.");
		const char32 *result = my channelNames [channelNumber];
	STRING_ONE_END
}

FORM (INTEGER_EEG_getChannelNumber, U"Get channel number", nullptr) {
	WORDVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	INTEGER_ONE (EEG)
		long result = EEG_getChannelNumber (me, channelName);
	INTEGER_ONE_END (U"")
}

// MARK: Modify

DIRECT (MODIFY_EEG_detrend) {
	MODIFY_EACH (EEG)
		EEG_detrend (me);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_editExternalElectrodeNames, U"Edit external electrode names", nullptr) {
	WORD (U"External electrode 1", U"EXG1")
	WORD (U"External electrode 2", U"EXG2")
	WORD (U"External electrode 3", U"EXG3")
	WORD (U"External electrode 4", U"EXG4")
	WORD (U"External electrode 5", U"EXG5")
	WORD (U"External electrode 6", U"EXG6")
	WORD (U"External electrode 7", U"EXG7")
	WORD (U"External electrode 8", U"EXG8")
	OK
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
END }

FORM (MODIFY_EEG_removeTriggers, U"Remove triggers", nullptr) {
	OPTIONMENU_ENUM (U"Remove every trigger that...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK
DO
	LOOP {
		iam (EEG);
		EEG_removeTriggers (me, GET_ENUM (kMelder_string, U"Remove every trigger that..."), GET_STRING (U"...the text"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_EEG_setChannelName, U"Set channel name", nullptr) {
	NATURALVAR (channelNumber, U"Channel number", U"1")
	WORDVAR (newName, U"New name", U"BLA")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_setChannelName (me, channelNumber, newName);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_setChannelToZero, U"Set channel to zero", nullptr) {
	SENTENCE (U"Channel", U"Iz")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_setChannelToZero (me, GET_STRING (U"Channel"));
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_subtractMeanChannel, U"Subtract mean channel", nullptr) {
	LABEL (U"label", U"Range of reference channels:")
	NATURALVAR (fromChannel, U"From channel", U"1")
	NATURALVAR (toChannel, U"To channel", U"32")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_subtractMeanChannel (me, fromChannel, toChannel);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_subtractReference, U"Subtract reference", nullptr) {
	WORDVAR (referenceChannel1, U"Reference channel 1", U"MASL")
	WORDVAR (referenceChannel2, U"Reference channel 2 (optional)", U"MASR")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_subtractReference (me, referenceChannel1, referenceChannel2);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_filter, U"Filter", nullptr) {
	REALVAR (lowFrequency, U"Low frequency (Hz)", U"1.0")
	REALVAR (lowWidth, U"Low width (Hz)", U"0.5")
	REALVAR (highFrequency, U"High frequency (Hz)", U"25.0")
	REALVAR (highWidth, U"High width (Hz)", U"12.5")
	BOOLEANVAR (notchAt50Hz, U"Notch at 50 Hz", true)
	OK
DO
	MODIFY_EACH (EEG)
		EEG_filter (me, lowFrequency, lowWidth, highFrequency, highWidth, notchAt50Hz);
	MODIFY_EACH_END
}

// MARK: Extract

FORM (NEW_EEG_extractChannel, U"EEG: Extract channel", nullptr) {
	SENTENCEVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	CONVERT_EACH (EEG)
		autoEEG result = EEG_extractChannel (me, channelName);
	CONVERT_EACH_END (my name, U"_", channelName)
}

FORM (NEW_EEG_extractPart, U"EEG: Extract part", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"1.0")
	BOOLEANVAR (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_EACH (EEG)
		autoEEG result = EEG_extractPart (me, fromTime, toTime, preserveTimes);
	CONVERT_EACH_END (my name, U"_part")
}

DIRECT (NEW_EEG_extractSound) {
	CONVERT_EACH (EEG)
		if (! my sound) Melder_throw (me, U": I don't contain a waveform.");
		autoSound result = EEG_extractSound (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_EEG_extractTextGrid) {
	CONVERT_EACH (EEG)
		if (! my textgrid) Melder_throw (me, U": I don't contain marks.");
		autoTextGrid result = EEG_extractTextGrid (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_ERPTier_bit, U"To ERPTier (bit)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	NATURALVAR (markerBit, U"Marker bit", U"8")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_bit (me, fromTime, toTime, markerBit);
	CONVERT_EACH_END (my name, U"_bit", markerBit)
}

FORM (NEW_EEG_to_ERPTier_marker, U"To ERPTier (marker)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	NATURALVAR (markerNumber, U"Marker number", U"12")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_marker (me, fromTime, toTime, (uint16) markerNumber);
	CONVERT_EACH_END (my name, U"_", markerNumber)
}

FORM (NEW_EEG_to_ERPTier_triggers, U"To ERPTier (triggers)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	OPTIONMENU_ENUMVAR (getEveryEventWithATriggerThat, U"Get every event with a trigger that", kMelder_string, DEFAULT)
	SENTENCEVAR (theText, U"...the text", U"1")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_triggers (me, fromTime, toTime, getEveryEventWithATriggerThat, theText);
	CONVERT_EACH_END (my name, U"_trigger", theText)
}

FORM (NEW_EEG_to_ERPTier_triggers_preceded, U"To ERPTier (triggers, preceded)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	OPTIONMENU_ENUMVAR (getEveryEventWithATriggerThat, U"Get every event with a trigger that", kMelder_string, DEFAULT)
	SENTENCEVAR (text1, U"...the text", U"1")
	OPTIONMENU_ENUMVAR (andIsPrecededByATriggerThat, U"and is preceded by a trigger that", kMelder_string, DEFAULT)
	SENTENCEVAR (text2, U" ...the text", U"4")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_triggers_preceded (me, fromTime, toTime,
			getEveryEventWithATriggerThat, text1, andIsPrecededByATriggerThat, text2);
	CONVERT_EACH_END (my name, U"_trigger", text2)
}

// MARK: Convert

DIRECT (NEW1_EEGs_concatenate) {
	OrderedOf<structEEG> eegs;
	LOOP {
		iam (EEG);
		eegs. addItem_ref (me);
	}
	autoEEG thee = EEGs_concatenate (& eegs);
	praat_new (thee.move(), U"chain");
END }

FORM (NEW_EEG_to_MixingMatrix, U"To MixingMatrix", nullptr) {
	NATURAL (U"Maximum number of iterations", U"100")
	POSITIVE (U"Tolerance", U"0.001")
	OPTIONMENU (U"Diagonalization method", 2)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	LOOP {
		iam (EEG);
		autoMixingMatrix thee = EEG_to_MixingMatrix (me,
			GET_INTEGER (U"Maximum number of iterations"), GET_REAL (U"Tolerance"),
			GET_INTEGER (U"Diagonalization method"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: - EEG & TextGrid

DIRECT (MODIFY_EEG_TextGrid_replaceTextGrid) {
	MODIFY_FIRST_OF_TWO (EEG, TextGrid)
		EEG_replaceTextGrid (me, you);
	MODIFY_FIRST_OF_TWO_END
}

// MARK: - ERP

// MARK: View & Edit

static void cb_ERPWindow_publication (Editor /* editor */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		bool isaSpectralSlice = Thing_isa (publication.get(), classSpectrum) && str32equ (Thing_getName (publication.get()), U"slice");
		praat_new (publication.move());
		praat_updateSelection ();
		if (isaSpectralSlice) {
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
DIRECT (WINDOW_ERP_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an ERP from batch.");
	LOOP {
		iam (ERP);
		autoERPWindow editor = ERPWindow_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_ERPWindow_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: Tabulate

FORM (NEW_ERP_downto_Table, U"ERP: Down to Table", nullptr) {
	BOOLEAN (U"Include sample number", false)
	BOOLEAN (U"Include time", true)
	NATURAL (U"Time decimals", U"6")
	NATURAL (U"Voltage decimals", U"12")
	RADIO (U"Voltage units", 1)
		OPTION (U"volt")
		OPTION (U"microvolt")
	OK
DO
	LOOP {
		iam (ERP);
		autoTable thee = ERP_tabulate (me, GET_INTEGER (U"Include sample number"),
			GET_INTEGER (U"Include time"), GET_INTEGER (U"Time decimals"), GET_INTEGER (U"Voltage decimals"), GET_INTEGER (U"Voltage units"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: Draw

FORM (GRAPHICS_ERP_draw, U"ERP: Draw", nullptr) {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range", U"0.0 (= all)")
	REAL4 (fromVoltage, U"left Voltage range (V)", U"10e-6")
	REAL4 (toVoltage, U"right Voltage range", U"-10e-6")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (ERP)
		ERP_drawChannel_name (me, GRAPHICS, channelName, fromTime, toTime, fromVoltage, toVoltage, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_ERP_drawScalp, U"ERP: Draw scalp", nullptr) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range", U"0.2")
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		ERP_drawScalp (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), kGraphics_colourScale_GREY, GET_INTEGER (U"Garnish"));
	}
END }

FORM (GRAPHICS_ERP_drawScalp_colour, U"ERP: Draw scalp (colour)", nullptr) {
	REAL (U"left Time range (s)", U"0.1")
	REAL (U"right Time range", U"0.2")
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	RADIO_ENUM (U"Colour scale", kGraphics_colourScale, BLUE_TO_RED)
	BOOLEAN (U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (ERP);
		ERP_drawScalp (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), GET_ENUM (kGraphics_colourScale, U"Colour scale"), GET_INTEGER (U"Garnish"));
	}
END }

FORM (GRAPHICS_ERP_drawScalp_garnish, U"ERP: Draw scalp (garnish)", nullptr) {
	REAL (U"left Voltage range (V)", U"10e-6")
	REAL (U"right Voltage range", U"-10e-6")
	RADIO_ENUM (U"Colour scale", kGraphics_colourScale, BLUE_TO_RED)
	OK
DO
	autoPraatPicture picture;
	ERP_drawScalp_garnish (GRAPHICS,
		GET_REAL (U"left Voltage range"), GET_REAL (U"right Voltage range"), GET_ENUM (kGraphics_colourScale, U"Colour scale"));
END }

// MARK: Query

FORM (STRING_ERP_getChannelName, U"Get channel name", nullptr) {
	NATURAL (U"Channel number", U"1")
	OK
DO
	LOOP {
		iam (ERP);
		long channelNumber = GET_INTEGER (U"Channel number");
		if (channelNumber > my ny)
			Melder_throw (me, U": there are only ", my ny, U" channels.");
		Melder_information (my channelNames [channelNumber]);
	}
END }

FORM (INTEGER_ERP_getChannelNumber, U"Get channel number", nullptr) {
	WORD (U"Channel name", U"Cz")
	OK
DO
	LOOP {
		iam (ERP);
		Melder_information (ERP_getChannelNumber (me, GET_STRING (U"Channel name")));
	}
END }

FORM (REAL_ERP_getMinimum, U"ERP: Get minimum", U"Sound: Get minimum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
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
END }

FORM (REAL_ERP_getTimeOfMinimum, U"ERP: Get time of minimum", U"Sound: Get time of minimum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
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
END }

FORM (REAL_ERP_getMaximum, U"ERP: Get maximum", U"Sound: Get maximum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
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
END }

FORM (REAL_ERP_getTimeOfMaximum, U"ERP: Get time of maximum", U"Sound: Get time of maximum...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
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
END }

FORM (REAL_ERP_getMean, U"ERP: Get mean", U"ERP: Get mean...") {
	SENTENCE (U"Channel name", U"Cz")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	LOOP {
		iam (ERP);
		const char32 *channelName = GET_STRING (U"Channel name");
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double mean = Vector_getMean (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber);
		Melder_informationReal (mean, U"Volt");
	}
END }

// MARK: Modify

FORM (MODIFY_ERP_formula, U"ERP: Formula", U"ERP: Formula...") {
	LABEL (U"label1", U"! `x' is the time in seconds, `col' is the sample number.")
	LABEL (U"label2", U"x = x1   ! time associated with first sample")
	LABEL (U"label3", U"for col from 1 to ncol")
	LABEL (U"label4", U"   self [col] = ...")
	TEXTFIELD (U"formula", U"self")
	LABEL (U"label5", U"   x = x + dx")
	LABEL (U"label6", U"endfor")
	OK
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
END }

FORM (MODIFY_ERP_formula_part, U"ERP: Formula (part)", U"ERP: Formula...") {
	REAL (U"From time", U"0.0")
	REAL (U"To time", U"0.0 (= all)")
	NATURAL (U"From channel", U"1")
	NATURAL (U"To channel", U"2")
	TEXTFIELD (U"formula", U"2 * self")
	OK
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
END }

// MARK: Extract

FORM (NEW_ERP_extractOneChannelAsSound, U"ERP: Extract one channel as Sound", nullptr) {
	WORDVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	LOOP {
		iam (ERP);
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		autoSound thee = Sound_extractChannel (me, channelNumber);
		praat_new (thee.move(), my name, U"_", channelName);
	}
END }

// MARK: Convert

DIRECT (NEW_ERP_downto_Sound) {
	LOOP {
		iam (ERP);
		autoSound you = ERP_downto_Sound (me);
		praat_new (you.move(), my name);
	}
END }

// MARK: - ERPTIER

// MARK: Help

DIRECT (HELP_ERPTier_help) {
	Melder_help (U"ERPTier");
END }

// MARK: Query

FORM (STRING_ERPTier_getChannelName, U"Get channel name", nullptr) {
	NATURALVAR (channelNumber, U"Channel number", U"1")
	OK
DO
	LOOP {
		iam (ERPTier);
		if (channelNumber > my numberOfChannels)
			Melder_throw (me, U": there are only ", my numberOfChannels, U" channels.");
		Melder_information (my channelNames [channelNumber]);
	}
END }

FORM (INTEGER_ERPTier_getChannelNumber, U"Get channel number", nullptr) {
	WORDVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	INTEGER_ONE (ERPTier)
		long result = ERPTier_getChannelNumber (me, channelName);
	INTEGER_ONE_END (U"")
}

FORM (REAL_ERPTier_getMean, U"ERPTier: Get mean", U"ERPTier: Get mean...") {
	NATURALVAR (pointNumber, U"Point number", U"1")
	SENTENCEVAR (channelName, U"Channel name", U"Cz")
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (ERPTier)
		double result = ERPTier_getMean (me, pointNumber, channelName, fromTime, toTime);
	NUMBER_ONE_END (U" Volt")
}

// MARK: Modify

FORM (MODIFY_ERPTier_rejectArtefacts, U"Reject artefacts", nullptr) {
	POSITIVEVAR (threshold, U"Threshold (V)", U"75e-6")
	OK
DO
	MODIFY_EACH (ERPTier)
		ERPTier_rejectArtefacts (me, threshold);
	MODIFY_EACH_END
}

FORM (MODIFY_ERPTier_removeEventsBetween, U"Remove events", U"ERPTier: Remove events between...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	OK
DO
	MODIFY_EACH (ERPTier)
		AnyTier_removePointsBetween (me->asAnyTier(), GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
	MODIFY_EACH_END
}

FORM (MODIFY_ERPTier_subtractBaseline, U"Subtract baseline", nullptr) {
	REAL (U"From time (s)", U"-0.11")
	REAL (U"To time (s)", U"0.0")
	OK
DO
	MODIFY_EACH (ERPTier)
		ERPTier_subtractBaseline (me, GET_REAL (U"From time"), GET_REAL (U"To time"));
	MODIFY_EACH_END
}

// MARK: Analyse

FORM (NEW_ERPTier_to_ERP, U"ERPTier: To ERP", nullptr) {
	NATURAL (U"Event number", U"1")
	OK
DO
	LOOP {
		iam (ERPTier);
		autoERP thee = ERPTier_extractERP (me, GET_INTEGER (U"Event number"));
		praat_new (thee.move(), my name, U"_mean");
	}
END }

DIRECT (NEW_ERPTier_to_ERP_mean) {
	LOOP {
		iam (ERPTier);
		autoERP thee = ERPTier_to_ERP_mean (me);
		praat_new (thee.move(), my name, U"_mean");
	}
END }

// MARK: - ERPTIER & TABLE

FORM (NEW1_ERPTier_Table_extractEventsWhereColumn_number, U"Extract events where column (number)", nullptr) {
	WORD (U"Extract all events where column...", U"")
	RADIO_ENUM (U"...is...", kMelder_number, DEFAULT)
	REAL (U"...the number", U"0.0")
	OK
DO
	ERPTier erpTier = FIRST (ERPTier);
	Table table = FIRST (Table);
	long columnNumber = Table_getColumnIndexFromColumnLabel (table, GET_STRING (U"Extract all events where column..."));
	autoERPTier thee = ERPTier_extractEventsWhereColumn_number (erpTier, table, columnNumber, GET_ENUM (kMelder_number, U"...is..."), GET_REAL (U"...the number"));
	praat_new (thee.move(), erpTier -> name);
END }

FORM (NEW1_ERPTier_Table_extractEventsWhereColumn_text, U"Extract events where column (text)", 0) {
	WORD (U"Extract all events where column...", U"")
	OPTIONMENU_ENUM (U"...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK
DO
	ERPTier erpTier = FIRST (ERPTier);
	Table table = FIRST (Table);
	long columnNumber = Table_getColumnIndexFromColumnLabel (table, GET_STRING (U"Extract all events where column..."));
	autoERPTier thee = ERPTier_extractEventsWhereColumn_string (erpTier, table, columnNumber, GET_ENUM (kMelder_string, U"..."), GET_STRING (U"...the text"));
	praat_new (thee.move(), erpTier -> name);
END }

// MARK: - file recognizers

static autoDaata bdfFileRecognizer (int nread, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	bool isBdfFile = Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".bdf") ||
	                 Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".BDF");
	bool isEdfFile = Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".edf") ||
	                 Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".EDF");
	if (nread < 512 || (! isBdfFile && ! isEdfFile)) return autoDaata ();
	return EEG_readFromBdfFile (file);
}

// MARK: - buttons

void praat_EEG_init ();
void praat_EEG_init () {

	Thing_recognizeClassesByName (classEEG, classERPTier, classERP, nullptr);

	Data_recognizeFileType (bdfFileRecognizer);

	praat_addAction1 (classEEG, 0, U"EEG help", nullptr, 0, HELP_EEG_help);
	praat_addAction1 (classEEG, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_EEG_viewAndEdit);
	praat_addAction1 (classEEG, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 1, U"Get channel name...", nullptr, 1, STRING_EEG_getChannelName);
		praat_addAction1 (classEEG, 1, U"Get channel number...", nullptr, 1, INTEGER_EEG_getChannelNumber);
	praat_addAction1 (classEEG, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Set channel name...", nullptr, 1, MODIFY_EEG_setChannelName);
		praat_addAction1 (classEEG, 1, U"Edit external electrode names...", nullptr, 1, MODIFY_EEG_editExternalElectrodeNames);
		praat_addAction1 (classEEG, 0, U"-- processing --", nullptr, 1, nullptr);
		praat_addAction1 (classEEG, 0, U"Subtract reference...", nullptr, 1, MODIFY_EEG_subtractReference);
		praat_addAction1 (classEEG, 0, U"Subtract mean channel...", nullptr, 1, MODIFY_EEG_subtractMeanChannel);
		praat_addAction1 (classEEG, 0, U"Detrend", nullptr, 1, MODIFY_EEG_detrend);
		praat_addAction1 (classEEG, 0, U"Filter...", nullptr, 1, MODIFY_EEG_filter);
		praat_addAction1 (classEEG, 0, U"Remove triggers...", nullptr, 1, MODIFY_EEG_removeTriggers);
		praat_addAction1 (classEEG, 0, U"Set channel to zero...", nullptr, 1, MODIFY_EEG_setChannelToZero);
	praat_addAction1 (classEEG, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Extract channel...", nullptr, 0, NEW_EEG_extractChannel);
		praat_addAction1 (classEEG, 0, U"Extract part...", nullptr, 0, NEW_EEG_extractPart);
		praat_addAction1 (classEEG, 0, U"To ERPTier -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"To ERPTier (bit)...", nullptr, 1, NEW_EEG_to_ERPTier_bit);
		praat_addAction1 (classEEG, 0, U"To ERPTier (marker)...", nullptr, 1, NEW_EEG_to_ERPTier_marker);
		praat_addAction1 (classEEG, 0, U"To ERPTier (triggers)...", nullptr, 1, NEW_EEG_to_ERPTier_triggers);
		praat_addAction1 (classEEG, 0, U"To ERPTier (triggers, preceded)...", nullptr, 1, NEW_EEG_to_ERPTier_triggers_preceded);
		praat_addAction1 (classEEG, 0, U"To ERPTier...", nullptr, praat_DEPTH_1 + praat_HIDDEN, NEW_EEG_to_ERPTier_bit);
		praat_addAction1 (classEEG, 0, U"To MixingMatrix...", nullptr, 0, NEW_EEG_to_MixingMatrix);
	praat_addAction1 (classEEG, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Concatenate", nullptr, 0, NEW1_EEGs_concatenate);
	praat_addAction1 (classEEG, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Extract waveforms as Sound", nullptr, 1, NEW_EEG_extractSound);
		praat_addAction1 (classEEG, 0, U"Extract marks as TextGrid", nullptr, 1, NEW_EEG_extractTextGrid);

	praat_addAction1 (classERP, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_ERP_viewAndEdit);
	praat_addAction1 (classERP, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Draw...", nullptr, 1, GRAPHICS_ERP_draw);
		praat_addAction1 (classERP, 0, U"Draw scalp...", nullptr, 1, GRAPHICS_ERP_drawScalp);
		praat_addAction1 (classERP, 0, U"Draw scalp (colour)...", nullptr, 1, GRAPHICS_ERP_drawScalp_colour);
		praat_addAction1 (classERP, 0, U"Draw scalp (garnish)...", nullptr, 1, GRAPHICS_ERP_drawScalp_garnish);
	praat_addAction1 (classERP, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Down to Table...", nullptr, 1, NEW_ERP_downto_Table);
	praat_addAction1 (classERP, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Get channel name...", nullptr, 1, STRING_ERP_getChannelName);
		praat_addAction1 (classERP, 0, U"Get channel number...", nullptr, 1, INTEGER_ERP_getChannelNumber);
		praat_addAction1 (classERP, 0, U"-- get shape --", nullptr, 1, nullptr);
		praat_addAction1 (classERP, 0, U"Get minimum...", nullptr, 1, REAL_ERP_getMinimum);
		praat_addAction1 (classERP, 0, U"Get time of minimum...", nullptr, 1, REAL_ERP_getTimeOfMinimum);
		praat_addAction1 (classERP, 0, U"Get maximum...", nullptr, 1, REAL_ERP_getMaximum);
		praat_addAction1 (classERP, 0, U"Get time of maximum...", nullptr, 1, REAL_ERP_getTimeOfMaximum);
		praat_addAction1 (classERP, 0, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classERP, 0, U"Get mean...", nullptr, 1, REAL_ERP_getMean);
	praat_addAction1 (classERP, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Formula...", nullptr, 1, MODIFY_ERP_formula);
		praat_addAction1 (classERP, 0, U"Formula (part)...", nullptr, 1, MODIFY_ERP_formula_part);
	// praat_addAction1 (classERP, 0, U"Analyse -", nullptr, 0, nullptr);
		// praat_addAction1 (classERP, 0, U"To ERP (difference)", nullptr, 1, NEW_ERP_to_ERP_difference);
		// praat_addAction1 (classERP, 0, U"To ERP (mean)", nullptr, 1, NEW_ERP_to_ERP_mean);
	praat_addAction1 (classERP, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Down to Sound", nullptr, 1, NEW_ERP_downto_Sound);
		praat_addAction1 (classERP, 0, U"Extract one channel as Sound...", nullptr, 1, NEW_ERP_extractOneChannelAsSound);

	praat_addAction1 (classERPTier, 0, U"ERPTier help", nullptr, 0, HELP_ERPTier_help);
	// praat_addAction1 (classERPTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_ERPTier_viewAndEdit);
	praat_addAction1 (classERPTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classERPTier);
		praat_addAction1 (classERPTier, 0, U"-- channel names --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Get channel name...", nullptr, 1, STRING_ERPTier_getChannelName);
		praat_addAction1 (classERPTier, 0, U"Get channel number...", nullptr, 1, INTEGER_ERPTier_getChannelNumber);
		praat_addAction1 (classERPTier, 0, U"-- erp --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Get mean...", nullptr, 1, REAL_ERPTier_getMean);
	praat_addAction1 (classERPTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classERPTier, 0, U"Subtract baseline...", nullptr, 1, MODIFY_ERPTier_subtractBaseline);
		praat_addAction1 (classERPTier, 0, U"Reject artefacts...", nullptr, 1, MODIFY_ERPTier_rejectArtefacts);
		praat_addAction1 (classERPTier, 0, U"-- structure --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Remove events between...", nullptr, 1, MODIFY_ERPTier_removeEventsBetween);
	praat_addAction1 (classERPTier, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classERPTier, 0, U"Extract ERP...", nullptr, 0, NEW_ERPTier_to_ERP);
		praat_addAction1 (classERPTier, 0, U"To ERP (mean)", nullptr, 0, NEW_ERPTier_to_ERP_mean);

	praat_addAction2 (classEEG, 1, classTextGrid, 1, U"Replace TextGrid", nullptr, 0, MODIFY_EEG_TextGrid_replaceTextGrid);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract -", nullptr, 0, nullptr);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract events where column (number)...", nullptr, 1, NEW1_ERPTier_Table_extractEventsWhereColumn_number);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract events where column (text)...", nullptr, 1, NEW1_ERPTier_Table_extractEventsWhereColumn_text);

	structEEGWindow :: f_preferences ();
	structERPWindow :: f_preferences ();
}

/* End of file praat_EEG.cpp */
