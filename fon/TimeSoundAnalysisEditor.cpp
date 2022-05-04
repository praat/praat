/* TimeSoundAnalysisEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include <time.h>
#include "TimeSoundAnalysisEditor.h"
#include "Preferences.h"
#include "EditorM.h"
#include "Sound_and_Spectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_Pitch.h"
#include "Sound_to_Intensity.h"
#include "Sound_to_Formant.h"
#include "Pitch_to_PointProcess.h"
#include "VoiceAnalysis.h"
#include "praat_script.h"

#include "enums_getText.h"
#include "TimeSoundAnalysisEditor_enums.h"
#include "enums_getValue.h"
#include "TimeSoundAnalysisEditor_enums.h"

Thing_implement_pureVirtual (TimeSoundAnalysisEditor, TimeSoundEditor, 0);

#include "EditorPrefs_define.h"
#include "TimeSoundAnalysisEditor_prefs.h"
#include "EditorPrefs_install.h"
#include "TimeSoundAnalysisEditor_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "TimeSoundAnalysisEditor_prefs.h"

static const conststring32 theMessage_Cannot_compute_spectrogram = U"The spectrogram is not defined at the edge of the sound.";
static const conststring32 theMessage_Cannot_compute_pitch = U"The pitch contour is not defined at the edge of the sound.";
static const conststring32 theMessage_Cannot_compute_formant = U"The formants are not defined at the edge of the sound.";
static const conststring32 theMessage_Cannot_compute_intensity = U"The intensity curve is not defined at the edge of the sound.";
static const conststring32 theMessage_Cannot_compute_pulses = U"The pulses are not defined at the edge of the sound.";

void structTimeSoundAnalysisEditor :: v_destroy () noexcept {
	our TimeSoundAnalysisEditor_Parent :: v_destroy ();
}

void structTimeSoundAnalysisEditor :: v_info () {
	our TimeSoundAnalysisEditor_Parent :: v_info ();
	MelderInfo_writeLine (U"Longest analysis: ", our instancePref_longestAnalysis(), U" seconds");
	if (v_hasSpectrogram ()) {
		/* Spectrogram flag: */
		MelderInfo_writeLine (U"Spectrogram show: ", our instancePref_spectrogram_show());
		/* Spectrogram settings: */
		MelderInfo_writeLine (U"Spectrogram view from: ", our instancePref_spectrogram_viewFrom(), U" Hz");
		MelderInfo_writeLine (U"Spectrogram view to: ", our instancePref_spectrogram_viewTo(), U" Hz");
		MelderInfo_writeLine (U"Spectrogram window length: ", our instancePref_spectrogram_windowLength(), U" seconds");
		MelderInfo_writeLine (U"Spectrogram dynamic range: ", our instancePref_spectrogram_dynamicRange(), U" dB");
		/* Advanced spectrogram settings: */
		MelderInfo_writeLine (U"Spectrogram number of time steps: ", our instancePref_spectrogram_timeSteps());
		MelderInfo_writeLine (U"Spectrogram number of frequency steps: ", our instancePref_spectrogram_frequencySteps());
		MelderInfo_writeLine (U"Spectrogram method: ", U"Fourier");
		MelderInfo_writeLine (U"Spectrogram window shape: ", kSound_to_Spectrogram_windowShape_getText (our p_spectrogram_windowShape));
		MelderInfo_writeLine (U"Spectrogram autoscaling: ", our instancePref_spectrogram_autoscaling());
		MelderInfo_writeLine (U"Spectrogram maximum: ", our instancePref_spectrogram_maximum(), U" dB/Hz");
		MelderInfo_writeLine (U"Spectrogram pre-emphasis: ", our instancePref_spectrogram_preemphasis(), U" dB/octave");
		MelderInfo_writeLine (U"Spectrogram dynamicCompression: ", our instancePref_spectrogram_dynamicCompression());
		/* Dynamic information: */
		MelderInfo_writeLine (U"Spectrogram cursor frequency: ", our d_spectrogram_cursor, U" Hz");
	}
	if (v_hasPitch ()) {
		/* Pitch flag: */
		MelderInfo_writeLine (U"Pitch show: ", our instancePref_pitch_show());
		/* Pitch settings: */
		MelderInfo_writeLine (U"Pitch floor: ", our instancePref_pitch_floor(), U" Hz");
		MelderInfo_writeLine (U"Pitch ceiling: ", our instancePref_pitch_ceiling(), U" Hz");
		MelderInfo_writeLine (U"Pitch unit: ", Function_getUnitText (Thing_dummyObject (Pitch), Pitch_LEVEL_FREQUENCY, (int) p_pitch_unit, Function_UNIT_TEXT_MENU));
		MelderInfo_writeLine (U"Pitch drawing method: ", kTimeSoundAnalysisEditor_pitch_drawingMethod_getText (p_pitch_drawingMethod));
		/* Advanced pitch settings: */
		MelderInfo_writeLine (U"Pitch view from: ", our instancePref_pitch_viewFrom(), U" ", Function_getUnitText (Thing_dummyObject (Pitch), Pitch_LEVEL_FREQUENCY, (int) our p_pitch_unit, Function_UNIT_TEXT_MENU));
		MelderInfo_writeLine (U"Pitch view to: ", our instancePref_pitch_viewTo(), U" ", Function_getUnitText (Thing_dummyObject (Pitch), Pitch_LEVEL_FREQUENCY, (int) our p_pitch_unit, Function_UNIT_TEXT_MENU));
		MelderInfo_writeLine (U"Pitch method: ", kTimeSoundAnalysisEditor_pitch_analysisMethod_getText (our p_pitch_method));
		MelderInfo_writeLine (U"Pitch very accurate: ", our instancePref_pitch_veryAccurate());
		MelderInfo_writeLine (U"Pitch max. number of candidates: ", our instancePref_pitch_maximumNumberOfCandidates());
		MelderInfo_writeLine (U"Pitch silence threshold: ", our instancePref_pitch_silenceThreshold(), U" of global peak");
		MelderInfo_writeLine (U"Pitch voicing threshold: ", our instancePref_pitch_voicingThreshold(), U" (periodic power / total power)");
		MelderInfo_writeLine (U"Pitch octave cost: ", our instancePref_pitch_octaveCost(), U" per octave");
		MelderInfo_writeLine (U"Pitch octave jump cost: ", our instancePref_pitch_octaveJumpCost(), U" per octave");
		MelderInfo_writeLine (U"Pitch voiced/unvoiced cost: ", our instancePref_pitch_voicedUnvoicedCost());
	}
	if (v_hasIntensity ()) {
		/* Intensity flag: */
		MelderInfo_writeLine (U"Intensity show: ", our instancePref_intensity_show());
		/* Intensity settings: */
		MelderInfo_writeLine (U"Intensity view from: ", our instancePref_intensity_viewFrom(), U" dB");
		MelderInfo_writeLine (U"Intensity view to: ", our instancePref_intensity_viewTo(), U" dB");
		MelderInfo_writeLine (U"Intensity averaging method: ", kTimeSoundAnalysisEditor_intensity_averagingMethod_getText (our p_intensity_averagingMethod));
		MelderInfo_writeLine (U"Intensity subtract mean pressure: ", our instancePref_intensity_subtractMeanPressure());
	}
	if (v_hasFormants ()) {
		/* Formant flag: */
		MelderInfo_writeLine (U"Formant show: ", our instancePref_formant_show());
		/* Formant settings: */
		MelderInfo_writeLine (U"Formant ceiling: ", our instancePref_formant_ceiling(), U" Hz");
		MelderInfo_writeLine (U"Formant number of poles: ", Melder_iround (2.0 * our instancePref_formant_numberOfFormants()));   // should be a whole number
		MelderInfo_writeLine (U"Formant window length: ", our instancePref_formant_windowLength(), U" seconds");
		MelderInfo_writeLine (U"Formant dynamic range: ", our instancePref_formant_dynamicRange(), U" dB");
		MelderInfo_writeLine (U"Formant dot size: ", our instancePref_formant_dotSize(), U" mm");
		/* Advanced formant settings: */
		MelderInfo_writeLine (U"Formant method: ", kTimeSoundAnalysisEditor_formant_analysisMethod_getText (our p_formant_method));
		MelderInfo_writeLine (U"Formant pre-emphasis from: ", our instancePref_formant_preemphasisFrom(), U" Hz");
	}
	if (v_hasPulses ()) {
		/* Pulses flag: */
		MelderInfo_writeLine (U"Pulses show: ", our instancePref_pulses_show());
		MelderInfo_writeLine (U"Pulses maximum period factor: ", our instancePref_pulses_maximumPeriodFactor());
		MelderInfo_writeLine (U"Pulses maximum amplitude factor: ", our instancePref_pulses_maximumAmplitudeFactor());
	}
}

void structTimeSoundAnalysisEditor :: v_dataChanged () {
	our v_reset_analysis ();
	our TimeSoundAnalysisEditor_Parent :: v_dataChanged ();
}

void structTimeSoundAnalysisEditor :: v_reset_analysis () {
	our d_spectrogram. reset();
	our d_pitch. reset();
	our d_intensity. reset();
	our d_formant. reset();
	our d_pulses. reset();
}

enum {
	TimeSoundAnalysisEditor_PART_CURSOR = 1,
	TimeSoundAnalysisEditor_PART_SELECTION = 2
};

static const conststring32 TimeSoundAnalysisEditor_partString (int part) {
	static const conststring32 strings [] = { U"", U"CURSOR", U"SELECTION" };
	return strings [part];
}

static const conststring32 TimeSoundAnalysisEditor_partString_locative (int part) {
	static const conststring32 strings [] = { U"", U"at CURSOR", U"in SELECTION" };
	return strings [part];
}

static int makeQueriable (TimeSoundAnalysisEditor me, int allowCursor, double *tmin, double *tmax) {
	if (my endWindow - my startWindow > my instancePref_longestAnalysis())
		Melder_throw (U"Window too long to show analyses. Zoom in to at most ", Melder_half (my instancePref_longestAnalysis()), U" seconds "
			U"or set the \"longest analysis\" to at least ", Melder_half (my endWindow - my startWindow), U" seconds (with \"Show analyses\" in the View menu).");
	if (my startSelection == my endSelection) {
		if (allowCursor) {
			*tmin = *tmax = my startSelection;
			return TimeSoundAnalysisEditor_PART_CURSOR;
		} else {
			Melder_throw (U"Make a selection first.");
		}
	} else if (my startSelection < my startWindow || my endSelection > my endWindow) {
		Melder_throw (U"Command ambiguous: a part of the selection (", my startSelection, U", ", my endSelection, U") "
			U"is outside of the window (", my startWindow, U", ", my endWindow, U"). "
			U"Either zoom or re-select.");
	}
	*tmin = my startSelection;
	*tmax = my endSelection;
	return TimeSoundAnalysisEditor_PART_SELECTION;
}

static void menu_cb_logSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Log settings", U"Log files")
		OPTIONMENU (writeLog1To, U"Write log 1 to", 3)
			OPTION (U"log file only")
			OPTION (U"Info window only")
			OPTION (U"log file and Info window")
		OUTFILE (logFile1,   U"Log file 1:",   my default_log1_fileName ())
		OUTFILE (log1format, U"Log 1 format:", my default_log1_format   ())
		OPTIONMENU (writeLog2To, U"Write log 2 to", 3)
			OPTION (U"log file only")
			OPTION (U"Info window only")
			OPTION (U"log file and Info window")
		OUTFILE (logFile2,   U"Log file 2:",   my default_log2_fileName ())
		OUTFILE (log2format, U"Log 2 format:", my default_log2_format   ())
		OUTFILE (logScript3, U"Log script 3:", my default_logScript3    ())
		OUTFILE (logScript4, U"Log script 4:", my default_logScript4    ())
	EDITOR_OK
		SET_OPTION (writeLog1To,   my instancePref_log1_toLogFile() + 2 * my instancePref_log1_toInfoWindow())
		SET_STRING (logFile1,      my p_log1_fileName)
		SET_STRING (log1format,    my p_log1_format)
		SET_OPTION (writeLog2To,   my instancePref_log2_toLogFile() + 2 * my instancePref_log2_toInfoWindow())
		SET_STRING (logFile2,      my p_log2_fileName)
		SET_STRING (log2format,    my p_log2_format)
		SET_STRING (logScript3,    my p_logScript3)
		SET_STRING (logScript4,    my p_logScript4)
	EDITOR_DO
		my setInstancePref_log1_toLogFile    ((writeLog1To & 1) != 0);
		my setInstancePref_log1_toInfoWindow ((writeLog1To & 2) != 0);
		pref_str32cpy2 (my pref_log1_fileName (), my p_log1_fileName, logFile1);
		pref_str32cpy2 (my pref_log1_format   (), my p_log1_format,   log1format);
		my setInstancePref_log2_toLogFile    ((writeLog2To & 1) != 0);
		my setInstancePref_log2_toInfoWindow ((writeLog2To & 2) != 0);
		pref_str32cpy2 (my pref_log2_fileName (), my p_log2_fileName, logFile2);
		pref_str32cpy2 (my pref_log2_format   (), my p_log2_format,   log2format);
		pref_str32cpy2 (my pref_logScript3    (), my p_logScript3,    logScript3);
		pref_str32cpy2 (my pref_logScript4    (), my p_logScript4,    logScript4);
	EDITOR_END
}

static void menu_cb_deleteLogFile1 (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		structMelderFile file { };
		Melder_pathToFile (my p_log1_fileName, & file);
		MelderFile_delete (& file);
	VOID_EDITOR_END
}

static void menu_cb_deleteLogFile2 (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		structMelderFile file { };
		Melder_pathToFile (my p_log2_fileName, & file);
		MelderFile_delete (& file);
	VOID_EDITOR_END
}

static void do_log (TimeSoundAnalysisEditor me, int which) {
	char32 format [Preferences_STRING_BUFFER_SIZE], *p;
	double tmin, tmax;
	const int part = makeQueriable (me, true, & tmin, & tmax);
	str32cpy (format, which == 1 ? my p_log1_format : my p_log2_format);
	for (p = format; *p != U'\0'; p ++) if (*p == U'\'') {
		/*
			Found a left quote. Search for a matching right quote.
		*/
		char32 *q = p + 1, varName [300], *r, *s, *colon;
		integer precision = -1;
		double value = undefined;
		conststring32 stringValue = nullptr;
		while (*q != U'\0' && *q != U'\'')
			q ++;
		if (*q == U'\0')
			break;   // no matching right quote: done with this line
		if (q - p == 1)
			continue;   // ignore empty variable names
		/*
			Found a right quote. Get potential variable name.
		*/
		for (r = p + 1, s = varName; q - r > 0; r ++, s ++) *s = *r;
		*s = U'\0';   // trailing null byte
		colon = str32chr (varName, U':');
		if (colon) {
			precision = Melder_atoi (colon + 1);
			*colon = U'\0';
		}
		if (str32equ (varName, U"time")) {
			value = 0.5 * (tmin + tmax);
		} else if (str32equ (varName, U"t1")) {
			value = tmin;
		} else if (str32equ (varName, U"t2")) {
			value = tmax;
		} else if (str32equ (varName, U"dur")) {
			value = tmax - tmin;
		} else if (str32equ (varName, U"freq")) {
			value = my d_spectrogram_cursor;
		} else if (str32equ (varName, U"tab$")) {
			stringValue = U"\t";
		} else if (str32equ (varName, U"editor$")) {
			stringValue = my name.get();
		} else if (str32equ (varName, U"f0")) {
			if (! my instancePref_pitch_show())
				Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
			if (part == TimeSoundAnalysisEditor_PART_CURSOR) {
				value = Pitch_getValueAtTime (my d_pitch.get(), tmin, my p_pitch_unit, 1);
			} else {
				value = Pitch_getMean (my d_pitch.get(), tmin, tmax, my p_pitch_unit);
			}
		} else if (varName [0] == U'f' && varName [1] >= U'1' && varName [1] <= U'5' && varName [2] == U'\0') {
			if (! my instancePref_formant_show())
				Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
			if (part == TimeSoundAnalysisEditor_PART_CURSOR) {
				value = Formant_getValueAtTime (my d_formant.get(), (int) (varName [1] - U'0'), tmin, kFormant_unit::HERTZ);
			} else {
				value = Formant_getMean (my d_formant.get(), (int) (varName [1] - U'0'), tmin, tmax, kFormant_unit::HERTZ);
			}
		} else if (varName [0] == U'b' && varName [1] >= U'1' && varName [1] <= U'5' && varName [2] == U'\0') {
			if (! my instancePref_formant_show())
				Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
			value = Formant_getBandwidthAtTime (my d_formant.get(), (int) (varName [1] - U'0'), 0.5 * (tmin + tmax), kFormant_unit::HERTZ);
		} else if (str32equ (varName, U"intensity")) {
			if (! my instancePref_intensity_show())
				Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
			if (! my d_intensity)
				Melder_throw (theMessage_Cannot_compute_intensity);
			if (part == TimeSoundAnalysisEditor_PART_CURSOR) {
				value = Vector_getValueAtX (my d_intensity.get(), tmin, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR);
			} else {
				value = Intensity_getAverage (my d_intensity.get(), tmin, tmax, (int) my p_intensity_averagingMethod);
			}
		} else if (str32equ (varName, U"power")) {
			if (! my instancePref_spectrogram_show())
				Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
			if (! my d_spectrogram)
				Melder_throw (theMessage_Cannot_compute_spectrogram);
			if (part != TimeSoundAnalysisEditor_PART_CURSOR) Melder_throw (U"Click inside the spectrogram first.");
			value = Matrix_getValueAtXY (my d_spectrogram.get(), tmin, my d_spectrogram_cursor);
		}
		if (isdefined (value)) {
			integer varlen = (q - p) - 1, headlen = p - format;
			char32 formattedNumber [400];
			if (precision >= 0)
				Melder_sprint (formattedNumber,400, Melder_fixed (value, precision));
			else
				Melder_sprint (formattedNumber,400, value);
			integer arglen = str32len (formattedNumber);
			static MelderString buffer;
			MelderString_ncopy (& buffer, format, headlen);
			MelderString_append (& buffer, formattedNumber, p + varlen + 2);
			str32cpy (format, buffer.string);
			p += arglen - 1;
		} else if (stringValue) {
			integer varlen = (q - p) - 1, headlen = p - format, arglen = str32len (stringValue);
			static MelderString buffer;
			MelderString_ncopy (& buffer, format, headlen);
			MelderString_append (& buffer, stringValue, p + varlen + 2);
			str32cpy (format, buffer.string);
			p += arglen - 1;
		} else {
			p = q - 1;   // go to before next quote
		}
	}
	if (which == 1 && my instancePref_log1_toInfoWindow()  ||  which == 2 && my instancePref_log2_toInfoWindow()) {
		MelderInfo_write (format);
		MelderInfo_close ();
	}
	if (which == 1 && my instancePref_log1_toLogFile()  ||  which == 2 && my instancePref_log2_toLogFile()) {
		structMelderFile file { };
		str32cat (format, U"\n");
		Melder_relativePathToFile (which == 1 ? my p_log1_fileName : my p_log2_fileName, & file);
		MelderFile_appendText (& file, format);
	}
}

static void menu_cb_log1 (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		do_log (me, 1);
	VOID_EDITOR_END
}
static void menu_cb_log2 (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		do_log (me, 2);
	VOID_EDITOR_END
}
static void menu_cb_logScript3 (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		DO_RunTheScriptFromAnyAddedEditorCommand (me, my p_logScript3);
	VOID_EDITOR_END
}
static void menu_cb_logScript4 (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		DO_RunTheScriptFromAnyAddedEditorCommand (me, my p_logScript4);
	VOID_EDITOR_END
}

static void menu_cb_showAnalyses (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show analyses", nullptr)
		BOOLEAN  (showSpectrogram, U"Show spectrogram",     my default_spectrogram_show ())
		BOOLEAN  (showPitch,       U"Show pitch",           my default_pitch_show       ())
		BOOLEAN  (showIntensity,   U"Show intensity",       my default_intensity_show   ())
		BOOLEAN  (showFormants,    U"Show formants",        my default_formant_show     ())
		BOOLEAN  (showPulses,      U"Show pulses",          my default_pulses_show      ())
		POSITIVE (longestAnalysis, U"Longest analysis (s)", my default_longestAnalysis  ())
	EDITOR_OK
		SET_BOOLEAN (showSpectrogram, my instancePref_spectrogram_show())
		SET_BOOLEAN (showPitch,       my instancePref_pitch_show())
		SET_BOOLEAN (showIntensity,   my instancePref_intensity_show())
		SET_BOOLEAN (showFormants,    my instancePref_formant_show())
		SET_BOOLEAN (showPulses,      my instancePref_pulses_show())
		SET_REAL    (longestAnalysis, my instancePref_longestAnalysis())
	EDITOR_DO
		my setInstancePref_spectrogram_show (showSpectrogram);
		my setInstancePref_pitch_show       (showPitch);
		my setInstancePref_intensity_show   (showIntensity);
		my setInstancePref_formant_show     (showFormants);
		my setInstancePref_pulses_show      (showPulses);
		GuiMenuItem_check (my spectrogramToggle, showSpectrogram);
		GuiMenuItem_check (my pitchToggle,       showPitch);
		GuiMenuItem_check (my intensityToggle,   showIntensity);
		GuiMenuItem_check (my formantToggle,     showFormants);
		GuiMenuItem_check (my pulsesToggle,      showPulses);
		my setInstancePref_longestAnalysis (longestAnalysis);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_timeStepSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Time step settings", U"Time step settings...")
		OPTIONMENU_ENUM (kTimeSoundAnalysisEditor_timeStepStrategy, timeStepStrategy,
				U"Time step strategy", my default_timeStepStrategy ())
		LABEL (U"")
		LABEL (U"If the time step strategy is \"fixed\":")
		POSITIVE (fixedTimeStep, U"Fixed time step (s)", my default_fixedTimeStep ())
		LABEL (U"")
		LABEL (U"If the time step strategy is \"view-dependent\":")
		NATURAL (numberOfTimeStepsPerView, U"Number of time steps per view", my default_numberOfTimeStepsPerView ())
	EDITOR_OK
		SET_ENUM (timeStepStrategy, kTimeSoundAnalysisEditor_timeStepStrategy, my p_timeStepStrategy)
		SET_REAL (fixedTimeStep, my instancePref_fixedTimeStep())
		SET_INTEGER (numberOfTimeStepsPerView, my instancePref_numberOfTimeStepsPerView())
	EDITOR_DO
		my pref_timeStepStrategy         () = my p_timeStepStrategy         = timeStepStrategy;
		my setInstancePref_fixedTimeStep (fixedTimeStep);
		my setInstancePref_numberOfTimeStepsPerView (numberOfTimeStepsPerView);
		my d_pitch. reset();
		my d_formant. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

/***** SPECTROGRAM MENU *****/

static void menu_cb_showSpectrogram (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		my setInstancePref_spectrogram_show (! my instancePref_spectrogram_show());   // toggle
		GuiMenuItem_check (my spectrogramToggle, my instancePref_spectrogram_show());   // in case we're called from a script
		FunctionEditor_redraw (me);
	VOID_EDITOR_END
}

static void menu_cb_spectrogramSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Spectrogram settings", U"Intro 3.2. Configuring the spectrogram")
		REAL     (viewFrom,     U"left View range (Hz)",  my default_spectrogram_viewFrom     ())
		POSITIVE (viewTo,       U"right View range (Hz)", my default_spectrogram_viewTo       ())
		POSITIVE (windowLength, U"Window length (s)",     my default_spectrogram_windowLength ())
		POSITIVE (dynamicRange, U"Dynamic range (dB)",    my default_spectrogram_dynamicRange ())
		MUTABLE_LABEL (note1, U"")
		MUTABLE_LABEL (note2, U"")
	EDITOR_OK
		SET_REAL (viewFrom,     my instancePref_spectrogram_viewFrom())
		SET_REAL (viewTo,       my instancePref_spectrogram_viewTo())
		SET_REAL (windowLength, my instancePref_spectrogram_windowLength())
		SET_REAL (dynamicRange, my instancePref_spectrogram_dynamicRange())
		if (my instancePref_spectrogram_timeSteps()          != Melder_atof (my default_spectrogram_timeSteps()) ||
			my instancePref_spectrogram_frequencySteps()     != Melder_atof (my default_spectrogram_frequencySteps()) ||
			my p_spectrogram_method             != my default_spectrogram_method() ||
			my p_spectrogram_windowShape        != my default_spectrogram_windowShape() ||
			my instancePref_spectrogram_maximum()            != Melder_atof (my default_spectrogram_maximum()) ||
			my instancePref_spectrogram_autoscaling()        != my default_spectrogram_autoscaling()||
			my instancePref_spectrogram_preemphasis()        != Melder_atof (my default_spectrogram_preemphasis()) ||
			my instancePref_spectrogram_dynamicCompression() != Melder_atof (my default_spectrogram_dynamicCompression()))
		{
			SET_STRING (note1, U"Warning: you have non-standard \"advanced settings\".")
		} else {
			SET_STRING (note1, U"(all of your \"advanced settings\" have their standard values)")
		}
		if (my p_timeStepStrategy != my default_timeStepStrategy ()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		Melder_require (viewFrom < viewTo,
			U"The ceiling of the spectrogram view range should be greater than the floor.");
		my setInstancePref_spectrogram_viewFrom (viewFrom);
		my setInstancePref_spectrogram_viewTo (viewTo);
		my setInstancePref_spectrogram_windowLength (windowLength);
		my setInstancePref_spectrogram_dynamicRange (dynamicRange);
		my d_spectrogram. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_advancedSpectrogramSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Advanced spectrogram settings", U"Advanced spectrogram settings...")
		LABEL (U"Time and frequency resolutions:")
		NATURAL (numberOfTimeSteps,      U"Number of time steps",      my default_spectrogram_timeSteps          ())
		NATURAL (numberOfFrequencySteps, U"Number of frequency steps", my default_spectrogram_frequencySteps     ())
		LABEL (U"Spectrogram analysis settings:")
		OPTIONMENU_ENUM (kSound_to_Spectrogram_method, method,
				U"Method", my default_spectrogram_method ())
		OPTIONMENU_ENUM (kSound_to_Spectrogram_windowShape, windowShape,
				U"Window shape", my default_spectrogram_windowShape ())
		LABEL (U"Spectrogram view settings:")
		BOOLEAN (autoscaling, U"Autoscaling", my default_spectrogram_autoscaling ())
		REAL (maximum, U"Maximum (dB/Hz)", my default_spectrogram_maximum ())
		REAL (preemphasis, U"Pre-emphasis (dB/oct)", my default_spectrogram_preemphasis ())
		REAL (dynamicCompression, U"Dynamic compression (0-1)", my default_spectrogram_dynamicCompression ())
	EDITOR_OK
		SET_INTEGER (numberOfTimeSteps,      my instancePref_spectrogram_timeSteps())
		SET_INTEGER (numberOfFrequencySteps, my instancePref_spectrogram_frequencySteps())
		SET_ENUM    (method,      kSound_to_Spectrogram_method,      my p_spectrogram_method)
		SET_ENUM    (windowShape, kSound_to_Spectrogram_windowShape, my p_spectrogram_windowShape)
		SET_BOOLEAN (autoscaling,              my instancePref_spectrogram_autoscaling())
		SET_REAL    (maximum,                  my instancePref_spectrogram_maximum())
		SET_REAL    (preemphasis,              my instancePref_spectrogram_preemphasis())
		SET_REAL    (dynamicCompression,       my instancePref_spectrogram_dynamicCompression())
	EDITOR_DO
		my setInstancePref_spectrogram_timeSteps (numberOfTimeSteps);
		my setInstancePref_spectrogram_frequencySteps (numberOfFrequencySteps);
		my pref_spectrogram_method             () = my p_spectrogram_method             = method;
		my pref_spectrogram_windowShape        () = my p_spectrogram_windowShape        = windowShape;
		my setInstancePref_spectrogram_autoscaling (autoscaling);
		my setInstancePref_spectrogram_maximum (maximum);
		my setInstancePref_spectrogram_preemphasis (preemphasis);
		my setInstancePref_spectrogram_dynamicCompression (dynamicCompression);
		my d_spectrogram. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void QUERY_EDITOR_FOR_REAL__getFrequency (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_EDITOR_FOR_REAL
		const double result = my d_spectrogram_cursor;
	QUERY_EDITOR_FOR_REAL_END (U" Hz")
}

static void QUERY_DATA_FOR_REAL__getSpectralPowerAtCursorCross (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_spectrogram_show())
			Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
		if (! my d_spectrogram) {
			TimeSoundAnalysisEditor_computeSpectrogram (me);
			if (! my d_spectrogram)
				Melder_throw (theMessage_Cannot_compute_spectrogram);
		}
		if (part != TimeSoundAnalysisEditor_PART_CURSOR)
			Melder_throw (U"Click inside the spectrogram first.");
		const double result = Matrix_getValueAtXY (my d_spectrogram.get(), tmin, my d_spectrogram_cursor);
	QUERY_DATA_FOR_REAL_END (U" Pa2/Hz (at time = ", tmin, U" seconds and frequency = ", my d_spectrogram_cursor, U" Hz)");
}

static void menu_cb_moveFrequencyCursorTo (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	if (! my instancePref_spectrogram_show())
		Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
	EDITOR_FORM (U"Move frequency cursor to", nullptr)
		REAL (frequency, U"Frequency (Hz)", U"0.0")
	EDITOR_OK
		SET_REAL (frequency, my d_spectrogram_cursor)
	EDITOR_DO
		my d_spectrogram_cursor = frequency;
		FunctionEditor_redraw (me);
	EDITOR_END
}

static autoSound extractSound (TimeSoundAnalysisEditor me, double tmin, double tmax) {
	autoSound sound;
	if (my d_longSound.data) {
		Melder_clipLeft (my d_longSound.data -> xmin, & tmin);
		Melder_clipRight (& tmax, my d_longSound.data -> xmax);
		sound = LongSound_extractPart (my d_longSound.data, tmin, tmax, true);
	} else if (my d_sound.data) {
		Melder_clipLeft (my d_sound.data -> xmin, & tmin);
		Melder_clipRight (& tmax, my d_sound.data -> xmax);
		sound = Sound_extractPart (my d_sound.data, tmin, tmax, kSound_windowShape::RECTANGULAR, 1.0, true);
	}
	return sound;
}

static void CONVERT_DATA_TO_ONE__ExtractVisibleSpectrogram (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (! my instancePref_spectrogram_show())
			Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
		if (! my d_spectrogram) {
			TimeSoundAnalysisEditor_computeSpectrogram (me);
			if (! my d_spectrogram)
				Melder_throw (theMessage_Cannot_compute_spectrogram);
		}
		autoSpectrogram result = Data_copy (my d_spectrogram.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ViewSpectralSlice (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		const double start = ( my startSelection == my endSelection ?
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::GAUSSIAN ? my startSelection - my instancePref_spectrogram_windowLength() :
			my startSelection - my instancePref_spectrogram_windowLength() / 2 : my startSelection
		);
		const double finish = ( my startSelection == my endSelection ?
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::GAUSSIAN ? my endSelection + my instancePref_spectrogram_windowLength() :
			my endSelection + my instancePref_spectrogram_windowLength() / 2 : my endSelection
		);
		autoSound sound = extractSound (me, start, finish);
		Sound_multiplyByWindow (sound.get(),
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::SQUARE ? kSound_windowShape::RECTANGULAR :
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::HAMMING ? kSound_windowShape::HAMMING :
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::BARTLETT ? kSound_windowShape::TRIANGULAR :
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::WELCH ? kSound_windowShape::PARABOLIC :
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::HANNING ? kSound_windowShape::HANNING :
			my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::GAUSSIAN ? kSound_windowShape::GAUSSIAN_2 : kSound_windowShape::RECTANGULAR
		);
		autoSpectrum result = Sound_to_Spectrum (sound.get(), true);
	CONVERT_DATA_TO_ONE_END (Melder_cat (( my data ? my data -> name.get() : U"untitled" ),
			U"_", Melder_fixed (0.5 * (my startSelection + my endSelection), 3)))
}

static void menu_cb_paintVisibleSpectrogram (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Paint visible spectrogram", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_spectrogram_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_spectrogram_picture_garnish (garnish);
		if (! my instancePref_spectrogram_show())
			Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
		if (! my d_spectrogram) {
			TimeSoundAnalysisEditor_computeSpectrogram (me);
			if (! my d_spectrogram)
				Melder_throw (theMessage_Cannot_compute_spectrogram);
		}
		Editor_openPraatPicture (me);
		Spectrogram_paint (my d_spectrogram.get(), my pictureGraphics, my startWindow, my endWindow,
			my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo(),
			my instancePref_spectrogram_maximum(), my instancePref_spectrogram_autoscaling(),
			my instancePref_spectrogram_dynamicRange(), my instancePref_spectrogram_preemphasis(),
			my instancePref_spectrogram_dynamicCompression(), garnish
		);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** PITCH MENU *****/

static void menu_cb_showPitch (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		my setInstancePref_pitch_show (! my instancePref_pitch_show());   // toggle
		GuiMenuItem_check (my pitchToggle, my instancePref_pitch_show());   // in case we're called from a script
		FunctionEditor_redraw (me);
	VOID_EDITOR_END
}

static void menu_cb_pitchSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Pitch settings", U"Intro 4.2. Configuring the pitch contour")
		POSITIVE (pitchFloor,   U"left Pitch range (Hz)",  my default_pitch_floor ())
		POSITIVE (pitchCeiling, U"right Pitch range (Hz)", my default_pitch_ceiling ())
		OPTIONMENU_ENUM (kPitch_unit, unit,
				U"Unit", my default_pitch_unit ())
		LABEL (U"The autocorrelation method optimizes for intonation research;")
		LABEL (U"and the cross-correlation method optimizes for voice research:")
		RADIO_ENUM (kTimeSoundAnalysisEditor_pitch_analysisMethod, analysisMethod,
				U"Analysis method", my default_pitch_method ())
		OPTIONMENU_ENUM (kTimeSoundAnalysisEditor_pitch_drawingMethod, drawingMethod,
				U"Drawing method", my default_pitch_drawingMethod ())
		MUTABLE_LABEL (note1, U"")
		MUTABLE_LABEL (note2, U"")
	EDITOR_OK
		SET_REAL (pitchFloor,   my instancePref_pitch_floor())
		SET_REAL (pitchCeiling, my instancePref_pitch_ceiling())
		SET_ENUM (unit, kPitch_unit, my p_pitch_unit)
		SET_ENUM (analysisMethod, kTimeSoundAnalysisEditor_pitch_analysisMethod, my p_pitch_method)
		SET_ENUM (drawingMethod, kTimeSoundAnalysisEditor_pitch_drawingMethod, my p_pitch_drawingMethod)
		if (my instancePref_pitch_viewFrom()                  != Melder_atof (my default_pitch_viewFrom()) ||
			my instancePref_pitch_viewTo()                    != Melder_atof (my default_pitch_viewTo()) ||
			my instancePref_pitch_veryAccurate()              != my default_pitch_veryAccurate() ||
			my instancePref_pitch_maximumNumberOfCandidates() != Melder_atoi (my default_pitch_maximumNumberOfCandidates()) ||
			my instancePref_pitch_silenceThreshold()          != Melder_atof (my default_pitch_silenceThreshold()) ||
			my instancePref_pitch_voicingThreshold()          != Melder_atof (my default_pitch_voicingThreshold()) ||
			my instancePref_pitch_octaveCost()                != Melder_atof (my default_pitch_octaveCost()) ||
			my instancePref_pitch_octaveJumpCost()            != Melder_atof (my default_pitch_octaveJumpCost()) ||
			my instancePref_pitch_voicedUnvoicedCost()        != Melder_atof (my default_pitch_voicedUnvoicedCost()))
		{
			SET_STRING (note1, U"Warning: you have some non-standard \"advanced settings\".")
		} else {
			SET_STRING (note1, U"(all of your \"advanced settings\" have their standard values)")
		}
		if (my p_timeStepStrategy != my default_timeStepStrategy ()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		Melder_require (pitchCeiling > pitchFloor,
			U"The pitch ceiling has to be greater than the pitch floor, so they cannot be ",
			pitchCeiling, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		my setInstancePref_pitch_floor (pitchFloor);
		my setInstancePref_pitch_ceiling (pitchCeiling);
		my pref_pitch_unit          () = my p_pitch_unit          = unit;
		my pref_pitch_method        () = my p_pitch_method        = analysisMethod;
		my pref_pitch_drawingMethod () = my p_pitch_drawingMethod = drawingMethod;
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_advancedPitchSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Advanced pitch settings", U"Advanced pitch settings...")
		LABEL   (U"Make view range different from analysis range:")
		REAL    (viewFrom,                  U"left View range (units)",   my default_pitch_viewFrom                  ())
		REAL    (viewTo,                    U"right View range (units)",  my default_pitch_viewTo                    ())
		LABEL   (U"Analysis settings:")
		BOOLEAN (veryAccurate,              U"Very accurate", false)
		NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_maximumNumberOfCandidates ())
		REAL    (silenceThreshold,          U"Silence threshold",         my default_pitch_silenceThreshold          ())
		REAL    (voicingThreshold,          U"Voicing threshold",         my default_pitch_voicingThreshold          ())
		REAL    (octaveCost,                U"Octave cost",               my default_pitch_octaveCost                ())
		REAL    (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_octaveJumpCost            ())
		REAL    (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_voicedUnvoicedCost        ())
	EDITOR_OK
		SET_REAL    (viewFrom,                  my instancePref_pitch_viewFrom())
		SET_REAL    (viewTo,                    my instancePref_pitch_viewTo())
		SET_BOOLEAN (veryAccurate,              my instancePref_pitch_veryAccurate())
		SET_INTEGER (maximumNumberOfCandidates, my instancePref_pitch_maximumNumberOfCandidates())
		SET_REAL    (silenceThreshold,          my instancePref_pitch_silenceThreshold())
		SET_REAL    (voicingThreshold,          my instancePref_pitch_voicingThreshold())
		SET_REAL    (octaveCost,                my instancePref_pitch_octaveCost())
		SET_REAL    (octaveJumpCost,            my instancePref_pitch_octaveJumpCost())
		SET_REAL    (voicedUnvoicedCost,        my instancePref_pitch_voicedUnvoicedCost())
	EDITOR_DO
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_viewFrom (viewFrom);
		my setInstancePref_pitch_viewTo (viewTo);
		my setInstancePref_pitch_veryAccurate (veryAccurate);
		my setInstancePref_pitch_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_octaveCost (octaveCost);
		my setInstancePref_pitch_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch.     reset();
		my d_intensity. reset();
		my d_pulses.    reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void INFO_DATA__pitchListing (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s   F0_", Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, Function_UNIT_TEXT_SHORT));
		if (part == TimeSoundAnalysisEditor_PART_CURSOR) {
			double f0 = Pitch_getValueAtTime (my d_pitch.get(), tmin, my p_pitch_unit, true);
			f0 = Function_convertToNonlogarithmic (my d_pitch.get(), f0, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
			MelderInfo_writeLine (Melder_fixed (tmin, 6), U"   ", Melder_fixed (f0, 6));
		} else {
			integer i, i1, i2;
			Sampled_getWindowSamples (my d_pitch.get(), tmin, tmax, & i1, & i2);
			for (i = i1; i <= i2; i ++) {
				const double t = Sampled_indexToX (my d_pitch.get(), i);
				double f0 = Sampled_getValueAtSample (my d_pitch.get(), i, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
				f0 = Function_convertToNonlogarithmic (my d_pitch.get(), f0, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
				MelderInfo_writeLine (Melder_fixed (t, 6), U"   ", Melder_fixed (f0, 6));
			}
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void QUERY_DATA_FOR_REAL__getPitch (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		double result =
			part == TimeSoundAnalysisEditor_PART_CURSOR
				? Pitch_getValueAtTime (my d_pitch.get(), tmin, my p_pitch_unit, true)
				: Pitch_getMean (my d_pitch.get(), tmin, tmax, my p_pitch_unit)
		;
		result = Function_convertToNonlogarithmic (my d_pitch.get(), result, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	QUERY_DATA_FOR_REAL_END (U" ",
		Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, 0),
		part == TimeSoundAnalysisEditor_PART_CURSOR
			? U" (interpolated pitch at CURSOR)"
			: Melder_cat (U" (mean pitch ", TimeSoundAnalysisEditor_partString_locative (part), U")")
	);
}

static void QUERY_DATA_FOR_REAL__getMinimumPitch (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		double result = Pitch_getMinimum (my d_pitch.get(), tmin, tmax, my p_pitch_unit, true);
		result = Function_convertToNonlogarithmic (my d_pitch.get(), result, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	QUERY_DATA_FOR_REAL_END (U" ", Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, 0),
			U" (minimum pitch ", TimeSoundAnalysisEditor_partString_locative (part), U")");
}

static void QUERY_DATA_FOR_REAL__getMaximumPitch (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);   // BUG
		}
		double result = Pitch_getMaximum (my d_pitch.get(), tmin, tmax, my p_pitch_unit, true);
		result = Function_convertToNonlogarithmic (my d_pitch.get(), result, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	QUERY_DATA_FOR_REAL_END (U" ", Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, 0),
			U" (maximum pitch ", TimeSoundAnalysisEditor_partString_locative (part), U")");
}

static void menu_cb_moveCursorToMinimumPitch (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the View menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		if (my startSelection == my endSelection) {
			Melder_throw (U"Empty selection.");
		} else {
			double time;
			Pitch_getMinimumAndTime (my d_pitch.get(), my startSelection, my endSelection,
				my p_pitch_unit, 1, nullptr, & time);
			if (isundef (time))
				Melder_throw (U"Selection is voiceless.");
			my startSelection = my endSelection = time;
			Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
			FunctionEditor_marksChanged (me, true);
		}
	VOID_EDITOR_END
}

static void menu_cb_moveCursorToMaximumPitch (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the View menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		if (my startSelection == my endSelection) {
			Melder_throw (U"Empty selection.");
		} else {
			double time;
			Pitch_getMaximumAndTime (my d_pitch.get(), my startSelection, my endSelection,
				my p_pitch_unit, 1, nullptr, & time);
			if (isundef (time))
				Melder_throw (U"Selection is voiceless.");
			my startSelection = my endSelection = time;
			Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
			FunctionEditor_marksChanged (me, true);
		}
	VOID_EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisiblePitchContour (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		autoPitch result = Data_copy (my d_pitch.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisiblePitchContour (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible pitch contour", nullptr)
		my v_form_pictureWindow (cmd);
		LABEL (U"Pitch:")
		BOOLEAN (speckle, U"Speckle", false)
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (speckle, my instancePref_pitch_picture_speckle())
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_pitch_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my setInstancePref_pitch_picture_speckle (speckle);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_pitch_picture_garnish (garnish);
		if (! my instancePref_pitch_show())
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch)
				Melder_throw (theMessage_Cannot_compute_pitch);
		}
		Editor_openPraatPicture (me);
		const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(), my instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(), my instancePref_pitch_ceiling(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		const double pitchFloor_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		const double pitchCeiling_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		const double pitchViewFrom_overt = ( my instancePref_pitch_viewFrom() < my instancePref_pitch_viewTo() ? my instancePref_pitch_viewFrom() : pitchFloor_overt );
		const double pitchViewTo_overt = ( my instancePref_pitch_viewFrom() < my instancePref_pitch_viewTo() ? my instancePref_pitch_viewTo() : pitchCeiling_overt );
		Pitch_draw (my d_pitch.get(), my pictureGraphics, my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt,
				garnish, speckle, my p_pitch_unit);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** INTENSITY MENU *****/

static void menu_cb_showIntensity (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		my setInstancePref_intensity_show (! my instancePref_intensity_show());   // toggle
		GuiMenuItem_check (my intensityToggle, my instancePref_intensity_show());   // in case we're called from a script
		FunctionEditor_redraw (me);
	VOID_EDITOR_END
}

static void menu_cb_intensitySettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Intensity settings", U"Intro 6.2. Configuring the intensity contour")
		REAL (viewFrom, U"left View range (dB)",  my default_intensity_viewFrom ())
		REAL (viewTo,   U"right View range (dB)", my default_intensity_viewTo   ())
		RADIO_ENUM (kTimeSoundAnalysisEditor_intensity_averagingMethod, averagingMethod,
				U"Averaging method", my default_intensity_averagingMethod ())
		BOOLEAN (subtractMeanPressure, U"Subtract mean pressure", my default_intensity_subtractMeanPressure ())
		LABEL (U"Note: the pitch floor is taken from the pitch settings.")
		MUTABLE_LABEL (note2, U"")
	EDITOR_OK
		SET_REAL (viewFrom,  my instancePref_intensity_viewFrom())
		SET_REAL (viewTo,    my instancePref_intensity_viewTo())
		SET_ENUM (averagingMethod, kTimeSoundAnalysisEditor_intensity_averagingMethod, my p_intensity_averagingMethod)
		SET_BOOLEAN (subtractMeanPressure, my instancePref_intensity_subtractMeanPressure())
		if (my p_timeStepStrategy != my default_timeStepStrategy ()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		Melder_require (viewTo > viewFrom,
			U"The ceiling of the view range should be greater than the floor.");
		my setInstancePref_intensity_viewFrom (viewFrom);
		my setInstancePref_intensity_viewTo (viewTo);
		my pref_intensity_averagingMethod      () = my p_intensity_averagingMethod      = averagingMethod;
		my setInstancePref_intensity_subtractMeanPressure (subtractMeanPressure);
		my d_intensity. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisibleIntensityContour (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (! my instancePref_intensity_show())
			Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my d_intensity) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my d_intensity)
				Melder_throw (theMessage_Cannot_compute_intensity);
		}
		autoIntensity result = Data_copy (my d_intensity.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisibleIntensityContour (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible intensity contour", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_intensity_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_intensity_picture_garnish (garnish);
		if (! my instancePref_intensity_show())
			Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my d_intensity) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my d_intensity) Melder_throw (theMessage_Cannot_compute_intensity);
		}
		Editor_openPraatPicture (me);
		Intensity_draw (my d_intensity.get(), my pictureGraphics, my startWindow, my endWindow,
				my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo(), garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__intensityListing (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_intensity_show())
			Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my d_intensity) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my d_intensity)
				Melder_throw (theMessage_Cannot_compute_intensity);
		}
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s   Intensity_dB");
		if (part == TimeSoundAnalysisEditor_PART_CURSOR) {
			const double intensity = Vector_getValueAtX (my d_intensity.get(), tmin, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR);
			MelderInfo_writeLine (Melder_fixed (tmin, 6), U"   ", Melder_fixed (intensity, 6));
		} else {
			integer i, i1, i2;
			Sampled_getWindowSamples (my d_intensity.get(), tmin, tmax, & i1, & i2);
			for (i = i1; i <= i2; i ++) {
				const double time = Sampled_indexToX (my d_intensity.get(), i);
				const double intensity = Vector_getValueAtX (my d_intensity.get(), time, Vector_CHANNEL_1, kVector_valueInterpolation :: NEAREST);
				MelderInfo_writeLine (Melder_fixed (time, 6), U"   ", Melder_fixed (intensity, 6));
			}
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void QUERY_DATA_FOR_REAL__getIntensity (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_intensity_show())
			Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my d_intensity) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my d_intensity)
				Melder_throw (theMessage_Cannot_compute_intensity);
		}
		const double result = ( part == TimeSoundAnalysisEditor_PART_CURSOR
			? Vector_getValueAtX (my d_intensity.get(), tmin, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR)
			: Intensity_getAverage (my d_intensity.get(), tmin, tmax, (int) my p_intensity_averagingMethod)
		);
	static const conststring32 methodString [] = { U"median", U"mean-energy", U"mean-sones", U"mean-dB" };
	QUERY_DATA_FOR_REAL_END (part == TimeSoundAnalysisEditor_PART_CURSOR
		? U" dB (intensity at CURSOR)"
		: Melder_cat (U" dB (", methodString [(int) my p_intensity_averagingMethod], U" intensity ",
				TimeSoundAnalysisEditor_partString_locative (part), U")")
	);
}

static void QUERY_DATA_FOR_REAL__getMinimumIntensity (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		if (! my instancePref_intensity_show())
			Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my d_intensity) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my d_intensity)
				Melder_throw (theMessage_Cannot_compute_intensity);
		}
		const double result = Vector_getMinimum (my d_intensity.get(), tmin, tmax, kVector_peakInterpolation :: PARABOLIC);
	QUERY_DATA_FOR_REAL_END (U" dB (minimum intensity ", TimeSoundAnalysisEditor_partString_locative (part), U")");
}

static void QUERY_DATA_FOR_REAL__getMaximumIntensity (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		if (! my instancePref_intensity_show())
			Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my d_intensity) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my d_intensity)
				Melder_throw (theMessage_Cannot_compute_intensity);
		}
		const double result = Vector_getMaximum (my d_intensity.get(), tmin, tmax, kVector_peakInterpolation :: PARABOLIC);
	QUERY_DATA_FOR_REAL_END (U" dB (maximum intensity ", TimeSoundAnalysisEditor_partString_locative (part), U")");
}

/***** FORMANT MENU *****/

static void menu_cb_showFormants (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		my setInstancePref_formant_show (! my instancePref_formant_show());   // toggle
		GuiMenuItem_check (my formantToggle, my instancePref_formant_show());   // in case we're called from a script
		FunctionEditor_redraw (me);
	VOID_EDITOR_END
}

static void menu_cb_formantSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant settings", U"Intro 5.2. Configuring the formant contours")
		POSITIVE (formantCeiling, U"Formant ceiling (Hz)",  my default_formant_ceiling          ())
		POSITIVE (numberOfFormants, U"Number of formants",  my default_formant_numberOfFormants ())
		POSITIVE (windowLength, U"Window length (s)",       my default_formant_windowLength     ())
		REAL     (dynamicRange, U"Dynamic range (dB)",      my default_formant_dynamicRange     ())
		POSITIVE (dotSize, U"Dot size (mm)",                my default_formant_dotSize          ())
		MUTABLE_LABEL (note1, U"")
		MUTABLE_LABEL (note2, U"")
	EDITOR_OK
		SET_REAL (formantCeiling,    my instancePref_formant_ceiling())
		SET_REAL (numberOfFormants,  my instancePref_formant_numberOfFormants())
		SET_REAL (windowLength,      my instancePref_formant_windowLength())
		SET_REAL (dynamicRange,      my instancePref_formant_dynamicRange())
		SET_REAL (dotSize,           my instancePref_formant_dotSize())
		if (my p_formant_method != my default_formant_method () || my instancePref_formant_preemphasisFrom() != Melder_atof (my default_formant_preemphasisFrom())) {
			SET_STRING (note1, U"Warning: you have non-standard \"advanced settings\".")
		} else {
			SET_STRING (note1, U"(all of your \"advanced settings\" have their standard values)")
		}
		if (my p_timeStepStrategy != my default_timeStepStrategy()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		my setInstancePref_formant_ceiling (formantCeiling);
		my setInstancePref_formant_numberOfFormants (numberOfFormants);
		my setInstancePref_formant_windowLength (windowLength);
		my setInstancePref_formant_dynamicRange (dynamicRange);
		my setInstancePref_formant_dotSize (dotSize);
		my d_formant. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_advancedFormantSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Advanced formant settings", U"Advanced formant settings...")
		RADIO_ENUM (kTimeSoundAnalysisEditor_formant_analysisMethod, method,
				U"Method", my default_formant_method ())
		POSITIVE (preemphasisFrom, U"Pre-emphasis from (Hz)", my default_formant_preemphasisFrom ())
	EDITOR_OK
		SET_ENUM (method, kTimeSoundAnalysisEditor_formant_analysisMethod, my p_formant_method)
		SET_REAL (preemphasisFrom, my instancePref_formant_preemphasisFrom())
	EDITOR_DO
		my pref_formant_method          () = my p_formant_method          = method;
		my setInstancePref_formant_preemphasisFrom (preemphasisFrom);
		my d_formant. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisibleFormantContour (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (! my instancePref_formant_show())
			Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
		if (! my d_formant) {
			TimeSoundAnalysisEditor_computeFormants (me);
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
		}
		autoFormant result = Data_copy (my d_formant.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisibleFormantContour (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible formant contour", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_formant_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_formant_picture_garnish (garnish);
		if (! my instancePref_formant_show())
			Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formant\" from the Formant menu.");
		if (! my d_formant) {
			TimeSoundAnalysisEditor_computeFormants (me);
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
		}
		Editor_openPraatPicture (me);
		Formant_drawSpeckles (my d_formant.get(), my pictureGraphics, my startWindow, my endWindow,
			my instancePref_spectrogram_viewTo(), my instancePref_formant_dynamicRange(),
			garnish
		);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__formantListing (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_formant_show())
			Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
		if (! my d_formant) {
			TimeSoundAnalysisEditor_computeFormants (me);
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
		}
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s   F1_Hz   F2_Hz   F3_Hz   F4_Hz");
		if (part == TimeSoundAnalysisEditor_PART_CURSOR) {
			const double f1 = Formant_getValueAtTime (my d_formant.get(), 1, tmin, kFormant_unit::HERTZ);
			const double f2 = Formant_getValueAtTime (my d_formant.get(), 2, tmin, kFormant_unit::HERTZ);
			const double f3 = Formant_getValueAtTime (my d_formant.get(), 3, tmin, kFormant_unit::HERTZ);
			const double f4 = Formant_getValueAtTime (my d_formant.get(), 4, tmin, kFormant_unit::HERTZ);
			MelderInfo_writeLine (Melder_fixed (tmin, 6), U"   ", Melder_fixed (f1, 6), U"   ", Melder_fixed (f2, 6), U"   ", Melder_fixed (f3, 6), U"   ", Melder_fixed (f4, 6));
		} else {
			integer i1, i2;
			Sampled_getWindowSamples (my d_formant.get(), tmin, tmax, & i1, & i2);
			for (integer i = i1; i <= i2; i ++) {
				const double t = Sampled_indexToX (my d_formant.get(), i);
				const double f1 = Formant_getValueAtTime (my d_formant.get(), 1, t, kFormant_unit::HERTZ);
				const double f2 = Formant_getValueAtTime (my d_formant.get(), 2, t, kFormant_unit::HERTZ);
				const double f3 = Formant_getValueAtTime (my d_formant.get(), 3, t, kFormant_unit::HERTZ);
				const double f4 = Formant_getValueAtTime (my d_formant.get(), 4, t, kFormant_unit::HERTZ);
				MelderInfo_writeLine (Melder_fixed (t, 6), U"   ", Melder_fixed (f1, 6), U"   ", Melder_fixed (f2, 6), U"   ", Melder_fixed (f3, 6), U"   ", Melder_fixed (f4, 6));
			}
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void do_getFormant (TimeSoundAnalysisEditor me, integer iformant, Interpreter interpreter) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_formant_show())
			Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
		if (! my d_formant) {
			TimeSoundAnalysisEditor_computeFormants (me);
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
		}
		const double result = ( part == TimeSoundAnalysisEditor_PART_CURSOR
			? Formant_getValueAtTime (my d_formant.get(), iformant, tmin, kFormant_unit::HERTZ)
			: Formant_getMean (my d_formant.get(), iformant, tmin, tmax, kFormant_unit::HERTZ)
		);
	QUERY_DATA_FOR_REAL_END (part == TimeSoundAnalysisEditor_PART_CURSOR
		? Melder_cat (U" Hz (nearest F", iformant, U" to CURSOR)")
		: Melder_cat (U" Hz (mean F", iformant, U" ", TimeSoundAnalysisEditor_partString_locative (part), U")")
	)
}
static void do_getBandwidth (TimeSoundAnalysisEditor me, integer iformant, Interpreter interpreter) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my instancePref_formant_show())
			Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
		if (! my d_formant) {
			TimeSoundAnalysisEditor_computeFormants (me);
			if (! my d_formant)
				Melder_throw (theMessage_Cannot_compute_formant);
		}
		const double result = ( part == TimeSoundAnalysisEditor_PART_CURSOR
			? Formant_getBandwidthAtTime (my d_formant.get(), iformant, tmin, kFormant_unit::HERTZ)
			: Formant_getBandwidthAtTime (my d_formant.get(), iformant, 0.5 * (tmin + tmax), kFormant_unit::HERTZ)
		);
	QUERY_DATA_FOR_REAL_END (part == TimeSoundAnalysisEditor_PART_CURSOR
		? Melder_cat (U" Hz (nearest B", iformant, U" to CURSOR)")
		: Melder_cat ( U" Hz (B", iformant, U" in centre of ", TimeSoundAnalysisEditor_partString (part), U")")
	)
}
static void QUERY_DATA_FOR_REAL__getFirstFormant (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getFormant (me, 1, interpreter);
}
static void QUERY_DATA_FOR_REAL__getFirstBandwidth (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getBandwidth (me, 1, interpreter);
}
static void QUERY_DATA_FOR_REAL__getSecondFormant (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getFormant (me, 2, interpreter);
}
static void QUERY_DATA_FOR_REAL__getSecondBandwidth (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getBandwidth (me, 2, interpreter);
}
static void QUERY_DATA_FOR_REAL__getThirdFormant (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getFormant (me, 3, interpreter);
}
static void QUERY_DATA_FOR_REAL__getThirdBandwidth (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getBandwidth (me, 3, interpreter);
}
static void QUERY_DATA_FOR_REAL__getFourthFormant (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getFormant (me, 4, interpreter);
}
static void QUERY_DATA_FOR_REAL__getFourthBandwidth (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	do_getBandwidth (me, 4, interpreter);
}

static void QUERY_DATA_FOR_REAL__getFormant (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Get formant", nullptr)
		NATURAL (formantNumber, U"Formant number", U"5")
	EDITOR_OK
	EDITOR_DO
		do_getFormant (me, formantNumber, interpreter);
	EDITOR_END
}

static void QUERY_DATA_FOR_REAL__getBandwidth (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Get bandwidth", nullptr)
		NATURAL (formantNumber, U"Formant number", U"5")
	EDITOR_OK
	EDITOR_DO
		do_getBandwidth (me, formantNumber, interpreter);
	EDITOR_END
}

/***** PULSE MENU *****/

static void menu_cb_showPulses (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT) {
	VOID_EDITOR
		my setInstancePref_pulses_show (! my instancePref_pulses_show());   // toggle
		GuiMenuItem_check (my pulsesToggle, my instancePref_pulses_show());   // in case we're called from a script
		FunctionEditor_redraw (me);
	VOID_EDITOR_END
}

static void menu_cb_advancedPulsesSettings (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Advanced pulses settings", U"Advanced pulses settings...")
		POSITIVE (maximumPeriodFactor,    U"Maximum period factor",    my default_pulses_maximumPeriodFactor    ())
		POSITIVE (maximumAmplitudeFactor, U"Maximum amplitude factor", my default_pulses_maximumAmplitudeFactor ())
	EDITOR_OK
		SET_REAL (maximumPeriodFactor,    my instancePref_pulses_maximumPeriodFactor())
		SET_REAL (maximumAmplitudeFactor, my instancePref_pulses_maximumAmplitudeFactor())
	EDITOR_DO
		my setInstancePref_pulses_maximumPeriodFactor (maximumPeriodFactor);
		my setInstancePref_pulses_maximumAmplitudeFactor (maximumAmplitudeFactor);
		my d_pulses. reset();
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisiblePulses (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (! my instancePref_pulses_show())
			Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
		if (! my d_pulses) {
			TimeSoundAnalysisEditor_computePulses (me);
			if (! my d_pulses)
				Melder_throw (theMessage_Cannot_compute_pulses);
		}
		autoPointProcess result = Data_copy (my d_pulses.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisiblePulses (TimeSoundAnalysisEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible pulses", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_pulses_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_pulses_picture_garnish (garnish);
		if (! my instancePref_pulses_show())
			Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
		if (! my d_pulses) {
			TimeSoundAnalysisEditor_computePulses (me);
			if (! my d_pulses)
				Melder_throw (theMessage_Cannot_compute_pulses);
		}
		Editor_openPraatPicture (me);
		PointProcess_draw (my d_pulses.get(), my pictureGraphics, my startWindow, my endWindow, garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__voiceReport (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		time_t today = time (nullptr);
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		if (! my instancePref_pulses_show())
			Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
		if (! my d_pulses) {
			TimeSoundAnalysisEditor_computePulses (me);
			if (! my d_pulses)
				Melder_throw (theMessage_Cannot_compute_pulses);
		}
		autoSound sound = extractSound (me, tmin, tmax);
		MelderInfo_open ();
		MelderInfo_writeLine (U"-- Voice report for ", my name.get(), U" --\nDate: ", Melder_peek8to32 (ctime (& today)));
		if (my p_pitch_method != kTimeSoundAnalysisEditor_pitch_analysisMethod::CROSS_CORRELATION)
			MelderInfo_writeLine (U"WARNING: some of the following measurements may be imprecise.\n"
				"For more precision, go to \"Pitch settings\" and choose \"Optimize for voice analysis\".\n");
		MelderInfo_writeLine (U"Time range of ", TimeSoundAnalysisEditor_partString (part));
		Sound_Pitch_PointProcess_voiceReport (sound.get(), my d_pitch.get(), my d_pulses.get(), tmin, tmax,
			my instancePref_pitch_floor(), my instancePref_pitch_ceiling(),
			my instancePref_pulses_maximumPeriodFactor(), my instancePref_pulses_maximumAmplitudeFactor(),
			my instancePref_pitch_silenceThreshold(), my instancePref_pitch_voicingThreshold()
		);
		MelderInfo_close ();
	INFO_DATA_END
}

static void INFO_DATA__pulseListing (TimeSoundAnalysisEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		double tmin, tmax;
		makeQueriable (me, false, & tmin, & tmax);
		if (! my instancePref_pulses_show())
			Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
		if (! my d_pulses) {
			TimeSoundAnalysisEditor_computePulses (me);
			if (! my d_pulses)
				Melder_throw (theMessage_Cannot_compute_pulses);
		}
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s");
		const integer i1 = PointProcess_getHighIndex (my d_pulses.get(), tmin);
		const integer i2 = PointProcess_getLowIndex (my d_pulses.get(), tmax);
		for (integer i = i1; i <= i2; i ++) {
			const double t = my d_pulses -> t [i];
			MelderInfo_writeLine (Melder_fixed (t, 12));
		}
		MelderInfo_close ();
	INFO_DATA_END
}

/*
static void cb_getJitter_xx (TimeSoundAnalysisEditor me, double (*PointProcess_getJitter_xx) (PointProcess, double, double, double, double, double)) {
	double minimumPeriod = 0.8 / my p_pitch_ceiling, maximumPeriod = 1.25 / my p_pitch_floor;
	if (! my p_pulses_show)
		Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my d_pulses) {
		computePulses (me);
		if (! my d_pulses)
			Melder_throw (theMessage_Cannot_compute_pulses);
	}
	if (my startSelection == my endSelection)
		Melder_throw (U"Make a selection first.");
	makeQueriable
	Melder_informationReal (PointProcess_getJitter_xx (my d_pulses, my startSelection, my endSelection,
		minimumPeriod, maximumPeriod, my p_pulses_maximumPeriodFactor), nullptr);
}
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_local) { cb_getJitter_xx (me, PointProcess_getJitter_local); END }
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_local_absolute) { cb_getJitter_xx (me, PointProcess_getJitter_local_absolute); END }
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_rap) { cb_getJitter_xx (me, PointProcess_getJitter_rap); END }
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_ppq5) { cb_getJitter_xx (me, PointProcess_getJitter_ppq5); END }
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_ddp) { cb_getJitter_xx (me, PointProcess_getJitter_ddp); END }

static void cb_getShimmer_xx (TimeSoundAnalysisEditor me, double (*PointProcess_Sound_getShimmer_xx) (PointProcess, Sound, double, double, double, double, double)) {
	double minimumPeriod = 0.8 / my p_pitch_ceiling, maximumPeriod = 1.25 / my p_pitch_floor;
	if (! my p_pulses_show)
		Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my d_pulses) {
		computePulses (me);
		if (! my d_pulses)
			Melder_throw (theMessage_Cannot_compute_pulses);
	}
	if (my startSelection == my endSelection)
		Melder_throw (U"Make a selection first.");
	makeQueriable
	autoSound sound = extractSound (me, my startSelection, my endSelection);
	Melder_informationReal (PointProcess_Sound_getShimmer_xx (my d_pulses, sound, my startSelection, my endSelection,
		minimumPeriod, maximumPeriod, my p_pulses_maximumAmplitudeFactor), nullptr);
}
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_local) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_local); END }
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_local_dB) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_local_dB); END }
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_apq3) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq3); END }
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_apq5) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq5); END }
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_apq11) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq11); END }
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_dda) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_dda); END }
*/

void structTimeSoundAnalysisEditor :: v_createMenuItems_view_sound (EditorMenu menu) {
	TimeSoundAnalysisEditor_Parent :: v_createMenuItems_view_sound (menu);
	v_createMenuItems_view_sound_analysis (menu);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_view_sound_analysis (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Analysis window:", GuiMenu_INSENSITIVE, menu_cb_showAnalyses);
	EditorMenu_addCommand (menu, U"Show analyses...", 0, menu_cb_showAnalyses);
	EditorMenu_addCommand (menu, U"Time step settings...", 0, menu_cb_timeStepSettings);
	EditorMenu_addCommand (menu, U"-- sound analysis --", 0, 0);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_query (EditorMenu menu) {
	TimeSoundAnalysisEditor_Parent :: v_createMenuItems_query (menu);
	if (d_sound.data || d_longSound.data)
		v_createMenuItems_query_log (menu);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_query_log (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- query log --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Log settings...", 0, menu_cb_logSettings);
	EditorMenu_addCommand (menu, U"Delete log file 1", 0, menu_cb_deleteLogFile1);
	EditorMenu_addCommand (menu, U"Delete log file 2", 0, menu_cb_deleteLogFile2);
	EditorMenu_addCommand (menu, U"Log 1", GuiMenu_F12, menu_cb_log1);
	EditorMenu_addCommand (menu, U"Log 2", GuiMenu_F12 | GuiMenu_SHIFT, menu_cb_log2);
	EditorMenu_addCommand (menu, U"Log script 3 (...)", GuiMenu_F12 | GuiMenu_OPTION, menu_cb_logScript3);
	EditorMenu_addCommand (menu, U"Log script 4 (...)", GuiMenu_F12 | GuiMenu_COMMAND, menu_cb_logScript4);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_formant (EditorMenu menu) {
	formantToggle = EditorMenu_addCommand (menu, U"Show formants",
			GuiMenu_CHECKBUTTON | (instancePref_formant_show() ? GuiMenu_TOGGLE_ON : 0), menu_cb_showFormants);
	EditorMenu_addCommand (menu, U"Formant settings...", 0, menu_cb_formantSettings);
	EditorMenu_addCommand (menu, U"Advanced formant settings...", 0, menu_cb_advancedFormantSettings);
	EditorMenu_addCommand (menu, U"-- formant query --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Query:", GuiMenu_INSENSITIVE, QUERY_EDITOR_FOR_REAL__getFrequency /* dummy */);
	EditorMenu_addCommand (menu, U"Formant listing", 0, INFO_DATA__formantListing);
	EditorMenu_addCommand (menu, U"Get first formant", GuiMenu_F1, QUERY_DATA_FOR_REAL__getFirstFormant);
	EditorMenu_addCommand (menu, U"Get first bandwidth", 0, QUERY_DATA_FOR_REAL__getFirstBandwidth);
	EditorMenu_addCommand (menu, U"Get second formant", GuiMenu_F2, QUERY_DATA_FOR_REAL__getSecondFormant);
	EditorMenu_addCommand (menu, U"Get second bandwidth", 0, QUERY_DATA_FOR_REAL__getSecondBandwidth);
	EditorMenu_addCommand (menu, U"Get third formant", GuiMenu_F3, QUERY_DATA_FOR_REAL__getThirdFormant);
	EditorMenu_addCommand (menu, U"Get third bandwidth", 0, QUERY_DATA_FOR_REAL__getThirdBandwidth);
	EditorMenu_addCommand (menu, U"Get fourth formant", GuiMenu_F4, QUERY_DATA_FOR_REAL__getFourthFormant);
	EditorMenu_addCommand (menu, U"Get fourth bandwidth", 0, QUERY_DATA_FOR_REAL__getFourthBandwidth);
	EditorMenu_addCommand (menu, U"Get formant...", 0, QUERY_DATA_FOR_REAL__getFormant);
	EditorMenu_addCommand (menu, U"Get bandwidth...", 0, QUERY_DATA_FOR_REAL__getBandwidth);
	v_createMenuItems_formant_picture (menu);
	EditorMenu_addCommand (menu, U"-- formant extract --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Extract to objects window:", GuiMenu_INSENSITIVE,
			CONVERT_DATA_TO_ONE__ExtractVisibleFormantContour /* dummy */);
	EditorMenu_addCommand (menu, U"Extract visible formant contour", 0,
			CONVERT_DATA_TO_ONE__ExtractVisibleFormantContour);
}

void structTimeSoundAnalysisEditor :: v_createMenus_analysis () {
	EditorMenu menu;

	if (v_hasSpectrogram ()) {
		menu = Editor_addMenu (this, U"Spectrum", 0);
		spectrogramToggle = EditorMenu_addCommand (menu, U"Show spectrogram",
				GuiMenu_CHECKBUTTON | (instancePref_spectrogram_show() ? GuiMenu_TOGGLE_ON : 0), menu_cb_showSpectrogram);
		EditorMenu_addCommand (menu, U"Spectrogram settings...", 0, menu_cb_spectrogramSettings);
		EditorMenu_addCommand (menu, U"Advanced spectrogram settings...", 0, menu_cb_advancedSpectrogramSettings);
		EditorMenu_addCommand (menu, U"-- spectrum query --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Query:", GuiMenu_INSENSITIVE, QUERY_EDITOR_FOR_REAL__getFrequency /* dummy */);
		EditorMenu_addCommand (menu, U"Get frequency at frequency cursor", 0, QUERY_EDITOR_FOR_REAL__getFrequency);
		EditorMenu_addCommand (menu, U"Get spectral power at cursor cross", GuiMenu_F7, QUERY_DATA_FOR_REAL__getSpectralPowerAtCursorCross);
		EditorMenu_addCommand (menu, U"-- spectrum select --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Select:", GuiMenu_INSENSITIVE, menu_cb_moveFrequencyCursorTo/* dummy */);
		EditorMenu_addCommand (menu, U"Move frequency cursor to...", 0, menu_cb_moveFrequencyCursorTo);
		v_createMenuItems_spectrum_picture (menu);
		EditorMenu_addCommand (menu, U"-- spectrum extract --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Extract to objects window:", GuiMenu_INSENSITIVE,
				CONVERT_DATA_TO_ONE__ExtractVisibleSpectrogram /* dummy */);
		EditorMenu_addCommand (menu, U"Extract visible spectrogram", 0,
				CONVERT_DATA_TO_ONE__ExtractVisibleSpectrogram);
		EditorMenu_addCommand (menu, U"View spectral slice", 'L',
				CONVERT_DATA_TO_ONE__ViewSpectralSlice);
	}

	if (v_hasPitch ()) {
		menu = Editor_addMenu (this, U"Pitch", 0);
		pitchToggle = EditorMenu_addCommand (menu, U"Show pitch",
				GuiMenu_CHECKBUTTON | (instancePref_pitch_show() ? GuiMenu_TOGGLE_ON : 0), menu_cb_showPitch);
		EditorMenu_addCommand (menu, U"Pitch settings...", 0, menu_cb_pitchSettings);
		EditorMenu_addCommand (menu, U"Advanced pitch settings...", 0, menu_cb_advancedPitchSettings);
		EditorMenu_addCommand (menu, U"-- pitch query --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Query:", GuiMenu_INSENSITIVE, QUERY_EDITOR_FOR_REAL__getFrequency /* dummy */);
		EditorMenu_addCommand (menu, U"Pitch listing", 0, INFO_DATA__pitchListing);
		EditorMenu_addCommand (menu, U"Get pitch", GuiMenu_F5, QUERY_DATA_FOR_REAL__getPitch);
		EditorMenu_addCommand (menu, U"Get minimum pitch", GuiMenu_F5 | GuiMenu_COMMAND, QUERY_DATA_FOR_REAL__getMinimumPitch);
		EditorMenu_addCommand (menu, U"Get maximum pitch", GuiMenu_F5 | GuiMenu_SHIFT, QUERY_DATA_FOR_REAL__getMaximumPitch);
		EditorMenu_addCommand (menu, U"-- pitch select --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Select:", GuiMenu_INSENSITIVE, menu_cb_moveCursorToMinimumPitch /* dummy */);
		EditorMenu_addCommand (menu, U"Move cursor to minimum pitch", GuiMenu_SHIFT | 'L', menu_cb_moveCursorToMinimumPitch);
		EditorMenu_addCommand (menu, U"Move cursor to maximum pitch", GuiMenu_SHIFT | 'H', menu_cb_moveCursorToMaximumPitch);
		v_createMenuItems_pitch_picture (menu);
		EditorMenu_addCommand (menu, U"-- pitch extract --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Extract to objects window:", GuiMenu_INSENSITIVE,
				CONVERT_DATA_TO_ONE__ExtractVisiblePitchContour /* dummy */);
		EditorMenu_addCommand (menu, U"Extract visible pitch contour", 0,
				CONVERT_DATA_TO_ONE__ExtractVisiblePitchContour);
	}

	if (v_hasIntensity ()) {
		menu = Editor_addMenu (this, U"Intensity", 0);
		intensityToggle = EditorMenu_addCommand (menu, U"Show intensity",
				GuiMenu_CHECKBUTTON | (instancePref_intensity_show() ? GuiMenu_TOGGLE_ON : 0), menu_cb_showIntensity);
		EditorMenu_addCommand (menu, U"Intensity settings...", 0, menu_cb_intensitySettings);
		EditorMenu_addCommand (menu, U"-- intensity query --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Query:", GuiMenu_INSENSITIVE, QUERY_EDITOR_FOR_REAL__getFrequency /* dummy */);
		EditorMenu_addCommand (menu, U"Intensity listing", 0, INFO_DATA__intensityListing);
		EditorMenu_addCommand (menu, U"Get intensity", GuiMenu_F8, QUERY_DATA_FOR_REAL__getIntensity);
		EditorMenu_addCommand (menu, U"Get minimum intensity", GuiMenu_F8 | GuiMenu_COMMAND, QUERY_DATA_FOR_REAL__getMinimumIntensity);
		EditorMenu_addCommand (menu, U"Get maximum intensity", GuiMenu_F8 | GuiMenu_SHIFT, QUERY_DATA_FOR_REAL__getMaximumIntensity);
		v_createMenuItems_intensity_picture (menu);
		EditorMenu_addCommand (menu, U"-- intensity extract --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Extract to objects window:", GuiMenu_INSENSITIVE,
				CONVERT_DATA_TO_ONE__ExtractVisibleIntensityContour /* dummy */);
		EditorMenu_addCommand (menu, U"Extract visible intensity contour", 0,
				CONVERT_DATA_TO_ONE__ExtractVisibleIntensityContour);
	}
	if (v_hasFormants ()) {
		menu = Editor_addMenu (this, U"Formant", 0);
		v_createMenuItems_formant (menu);
	}
	
	if (v_hasPulses ()) {
		menu = Editor_addMenu (this, U"Pulses", 0);
		pulsesToggle = EditorMenu_addCommand (menu, U"Show pulses",
				GuiMenu_CHECKBUTTON | (instancePref_pulses_show() ? GuiMenu_TOGGLE_ON : 0), menu_cb_showPulses);
		EditorMenu_addCommand (menu, U"Advanced pulses settings...", 0, menu_cb_advancedPulsesSettings);
		EditorMenu_addCommand (menu, U"-- pulses query --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Query:", GuiMenu_INSENSITIVE, QUERY_EDITOR_FOR_REAL__getFrequency /* dummy */);
		EditorMenu_addCommand (menu, U"Voice report", 0, INFO_DATA__voiceReport);
		EditorMenu_addCommand (menu, U"Pulse listing", 0, INFO_DATA__pulseListing);
		/*
		EditorMenu_addCommand (menu, U"Get jitter (local)", 0, cb_getJitter_local);
		EditorMenu_addCommand (menu, U"Get jitter (local, absolute)", 0, cb_getJitter_local_absolute);
		EditorMenu_addCommand (menu, U"Get jitter (rap)", 0, cb_getJitter_rap);
		EditorMenu_addCommand (menu, U"Get jitter (ppq5)", 0, cb_getJitter_ppq5);
		EditorMenu_addCommand (menu, U"Get jitter (ddp)", 0, cb_getJitter_ddp);
		EditorMenu_addCommand (menu, U"Get shimmer (local)", 0, cb_getShimmer_local);
		EditorMenu_addCommand (menu, U"Get shimmer (local_dB)", 0, cb_getShimmer_local_dB);
		EditorMenu_addCommand (menu, U"Get shimmer (apq3)", 0, cb_getShimmer_apq3);
		EditorMenu_addCommand (menu, U"Get shimmer (apq5)", 0, cb_getShimmer_apq5);
		EditorMenu_addCommand (menu, U"Get shimmer (apq11)", 0, cb_getShimmer_apq11);
		EditorMenu_addCommand (menu, U"Get shimmer (dda)", 0, cb_getShimmer_dda);
		*/
		v_createMenuItems_pulses_picture (menu);
		EditorMenu_addCommand (menu, U"-- pulses extract --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Extract to objects window:", GuiMenu_INSENSITIVE,
				CONVERT_DATA_TO_ONE__ExtractVisiblePulses /* dummy */);
		EditorMenu_addCommand (menu, U"Extract visible pulses", 0,
				CONVERT_DATA_TO_ONE__ExtractVisiblePulses);
	}
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_spectrum_picture (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- spectrum draw --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_paintVisibleSpectrogram /* dummy */);
	EditorMenu_addCommand (menu, U"Paint visible spectrogram...", 0, menu_cb_paintVisibleSpectrogram);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_pitch_picture (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- pitch draw --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisiblePitchContour /* dummy */);
	EditorMenu_addCommand (menu, U"Draw visible pitch contour...", 0, menu_cb_drawVisiblePitchContour);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_intensity_picture (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- intensity draw --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisibleIntensityContour /* dummy */);
	EditorMenu_addCommand (menu, U"Draw visible intensity contour...", 0, menu_cb_drawVisibleIntensityContour);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_formant_picture (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- formant draw --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisibleFormantContour /* dummy */);
	EditorMenu_addCommand (menu, U"Draw visible formant contour...", 0, menu_cb_drawVisibleFormantContour);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_pulses_picture (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- pulses draw --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisiblePulses /* dummy */);
	EditorMenu_addCommand (menu, U"Draw visible pulses...", 0, menu_cb_drawVisiblePulses);
}

void TimeSoundAnalysisEditor_computeSpectrogram (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my instancePref_spectrogram_show() && my endWindow - my startWindow <= my instancePref_longestAnalysis() &&
			(! my d_spectrogram || my d_spectrogram -> xmin != my startWindow || my d_spectrogram -> xmax != my endWindow))
	{
		const double margin = ( my p_spectrogram_windowShape == kSound_to_Spectrogram_windowShape::GAUSSIAN ?
				my instancePref_spectrogram_windowLength() : 0.5 * my instancePref_spectrogram_windowLength() );
		my d_spectrogram.reset();
		try {
			autoSound sound = extractSound (me, my startWindow - margin, my endWindow + margin);
			my d_spectrogram = Sound_to_Spectrogram (sound.get(),
				my instancePref_spectrogram_windowLength(),
				my instancePref_spectrogram_viewTo(),
				(my endWindow - my startWindow) / my instancePref_spectrogram_timeSteps(),
				my instancePref_spectrogram_viewTo() / my instancePref_spectrogram_frequencySteps(),
				my p_spectrogram_windowShape, 8.0, 8.0
			);
			my d_spectrogram -> xmin = my startWindow;
			my d_spectrogram -> xmax = my endWindow;
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

static void computePitch_inside (TimeSoundAnalysisEditor me) {
	const double margin = ( my instancePref_pitch_veryAccurate() ? 3.0 / my instancePref_pitch_floor() : 1.5 / my instancePref_pitch_floor() );
	my d_pitch. reset();
	try {
		autoSound sound = extractSound (me, my startWindow - margin, my endWindow + margin);
		const double pitchTimeStep = (
			my p_timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy::FIXED_ ? my instancePref_fixedTimeStep() :
			my p_timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy::VIEW_DEPENDENT ? (my endWindow - my startWindow) / my instancePref_numberOfTimeStepsPerView() :
			0.0   // the default: determined by pitch floor
		);
		my d_pitch = Sound_to_Pitch_any (sound.get(), pitchTimeStep,
			my instancePref_pitch_floor(),
			my p_pitch_method == kTimeSoundAnalysisEditor_pitch_analysisMethod::AUTOCORRELATION ? 3.0 : 1.0,
			my instancePref_pitch_maximumNumberOfCandidates(),
			((int) my p_pitch_method - 1) * 2 + my instancePref_pitch_veryAccurate(),
			my instancePref_pitch_silenceThreshold(), my instancePref_pitch_voicingThreshold(),
			my instancePref_pitch_octaveCost(), my instancePref_pitch_octaveJumpCost(),
			my instancePref_pitch_voicedUnvoicedCost(), my instancePref_pitch_ceiling()
		);
		my d_pitch -> xmin = my startWindow;
		my d_pitch -> xmax = my endWindow;
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void TimeSoundAnalysisEditor_computePitch (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my instancePref_pitch_show() && my endWindow - my startWindow <= my instancePref_longestAnalysis() &&
		(! my d_pitch || my d_pitch -> xmin != my startWindow || my d_pitch -> xmax != my endWindow))
	{
		computePitch_inside (me);
	}
}

void TimeSoundAnalysisEditor_computeIntensity (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my instancePref_intensity_show() && my endWindow - my startWindow <= my instancePref_longestAnalysis() &&
		(! my d_intensity || my d_intensity -> xmin != my startWindow || my d_intensity -> xmax != my endWindow))
	{
		const double margin = 3.2 / my instancePref_pitch_floor();
		my d_intensity. reset();
		try {
			autoSound sound = extractSound (me, my startWindow - margin, my endWindow + margin);
			my d_intensity = Sound_to_Intensity (sound.get(), my instancePref_pitch_floor(),
				my endWindow - my startWindow > my instancePref_longestAnalysis() ? (my endWindow - my startWindow) / 100 : 0.0,
				my instancePref_intensity_subtractMeanPressure()
			);
			my d_intensity -> xmin = my startWindow;
			my d_intensity -> xmax = my endWindow;
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void TimeSoundAnalysisEditor_computeFormants (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my instancePref_formant_show() && my endWindow - my startWindow <= my instancePref_longestAnalysis() &&
			(! my d_formant || my d_formant -> xmin != my startWindow || my d_formant -> xmax != my endWindow))
	{
		const double margin = my instancePref_formant_windowLength();
		my d_formant. reset();
		try {
			autoSound sound = ( my endWindow - my startWindow > my instancePref_longestAnalysis() ?
				extractSound (me,
					0.5 * (my startWindow + my endWindow - my instancePref_longestAnalysis()) - margin,
					0.5 * (my startWindow + my endWindow + my instancePref_longestAnalysis()) + margin
				) :
				extractSound (me, my startWindow - margin, my endWindow + margin)
			);
			const double formantTimeStep = (
				my p_timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy::FIXED_ ? my instancePref_fixedTimeStep() :
				my p_timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy::VIEW_DEPENDENT ? (my endWindow - my startWindow) / my instancePref_numberOfTimeStepsPerView() :
				0.0   // the default: determined by analysis window length
			);
			my d_formant = Sound_to_Formant_any (sound.get(), formantTimeStep,
				Melder_iround (my instancePref_formant_numberOfFormants() * 2.0), my instancePref_formant_ceiling(),
				my instancePref_formant_windowLength(), (int) my p_formant_method, my instancePref_formant_preemphasisFrom(), 50.0
			);
			my d_formant -> xmin = my startWindow;
			my d_formant -> xmax = my endWindow;
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void TimeSoundAnalysisEditor_computePulses (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my instancePref_pulses_show() && my endWindow - my startWindow <= my instancePref_longestAnalysis() &&
			(! my d_pulses || my d_pulses -> xmin != my startWindow || my d_pulses -> xmax != my endWindow))
	{
		my d_pulses. reset();
		if (! my d_pitch || my d_pitch -> xmin != my startWindow || my d_pitch -> xmax != my endWindow)
			computePitch_inside (me);
		if (my d_pitch) {
			try {
				autoSound sound = extractSound (me, my startWindow, my endWindow);
				my d_pulses = Sound_Pitch_to_PointProcess_cc (sound.get(), my d_pitch.get());
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	}
}

static void TimeSoundAnalysisEditor_v_draw_analysis (TimeSoundAnalysisEditor me) {
	/*
		d_pitch may not exist yet (if shown at all, it may be going to be created in TimeSoundAnalysisEditor_computePitch (),
		and even if that fails the user should see what the pitch settings are). So we use a dummy object.
	*/
	const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (Thing_dummyObject (Pitch),
			my instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (Thing_dummyObject (Pitch),
			my instancePref_pitch_ceiling(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	const double pitchFloor_overt = Function_convertToNonlogarithmic (Thing_dummyObject (Pitch), pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	const double pitchCeiling_overt = Function_convertToNonlogarithmic (Thing_dummyObject (Pitch), pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
	const double pitchViewFrom_overt = ( my instancePref_pitch_viewFrom() < my instancePref_pitch_viewTo() ? my instancePref_pitch_viewFrom() : pitchFloor_overt );
	const double pitchViewTo_overt = ( my instancePref_pitch_viewFrom() < my instancePref_pitch_viewTo() ? my instancePref_pitch_viewTo() : pitchCeiling_overt );
	const double pitchViewFrom_hidden = Function_isUnitLogarithmic (Thing_dummyObject (Pitch), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit) ? log10 (pitchViewFrom_overt) : pitchViewFrom_overt;
	const double pitchViewTo_hidden = Function_isUnitLogarithmic (Thing_dummyObject (Pitch), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit) ? log10 (pitchViewTo_overt) : pitchViewTo_overt;

	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_rectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);

	if (my endWindow - my startWindow > my instancePref_longestAnalysis()) {
		Graphics_setFont (my graphics.get(), kGraphics_font::HELVETICA);
		Graphics_setFontSize (my graphics.get(), 10);
		Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics.get(), 0.5, 0.67,   U"(To see the analyses, zoom in to at most ", Melder_half (my instancePref_longestAnalysis()), U" seconds,");
		Graphics_text (my graphics.get(), 0.5, 0.33,   U"or raise the \"longest analysis\" setting with \"Show analyses\" in the View menu.)");
		Graphics_setFontSize (my graphics.get(), 12);
		return;
	}
	TimeSoundAnalysisEditor_computeSpectrogram (me);
	if (my instancePref_spectrogram_show() && my d_spectrogram) {
		Spectrogram_paintInside (my d_spectrogram.get(), my graphics.get(), my startWindow, my endWindow,
			my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo(),
			my instancePref_spectrogram_maximum(), my instancePref_spectrogram_autoscaling(),
			my instancePref_spectrogram_dynamicRange(), my instancePref_spectrogram_preemphasis(),
			my instancePref_spectrogram_dynamicCompression()
		);
	}
	TimeSoundAnalysisEditor_computePitch (me);
	if (my instancePref_pitch_show() && my d_pitch) {
		const double periodsPerAnalysisWindow = ( my p_pitch_method == kTimeSoundAnalysisEditor_pitch_analysisMethod::AUTOCORRELATION ? 3.0 : 1.0 );
		const double greatestNonUndersamplingTimeStep = 0.5 * periodsPerAnalysisWindow / my instancePref_pitch_floor();
		const double defaultTimeStep = 0.5 * greatestNonUndersamplingTimeStep;
		const double timeStep = (
			my p_timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy::FIXED_ ? my instancePref_fixedTimeStep() :
			my p_timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy::VIEW_DEPENDENT ? (my endWindow - my startWindow) / my instancePref_numberOfTimeStepsPerView() :
			defaultTimeStep
		);
		const bool undersampled = ( timeStep > greatestNonUndersamplingTimeStep );
		const integer numberOfVisiblePitchPoints = (integer) ((my endWindow - my startWindow) / timeStep);
		Graphics_setColour (my graphics.get(), Melder_CYAN);
		Graphics_setLineWidth (my graphics.get(), 3.0);
		if ((my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::AUTOMATIC && (undersampled || numberOfVisiblePitchPoints < 101)) ||
		    my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::SPECKLE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics.get(), my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, 2, my p_pitch_unit);
		}
		if ((my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::AUTOMATIC && ! undersampled) ||
		    my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::CURVE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics.get(), my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, false, my p_pitch_unit);
		}
		Graphics_setColour (my graphics.get(), Melder_BLUE);
		Graphics_setLineWidth (my graphics.get(), 1.0);
		if ((my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::AUTOMATIC && (undersampled || numberOfVisiblePitchPoints < 101)) ||
		    my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::SPECKLE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics.get(), my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, 1, my p_pitch_unit);
		}
		if ((my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::AUTOMATIC && ! undersampled) ||
		    my p_pitch_drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod::CURVE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics.get(), my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, false, my p_pitch_unit);
		}
		Graphics_setColour (my graphics.get(), Melder_BLACK);
	}
	TimeSoundAnalysisEditor_computeIntensity (me);
	if (my instancePref_intensity_show() && my d_intensity) {
		Graphics_setColour (my graphics.get(), my instancePref_spectrogram_show() ? Melder_YELLOW : Melder_LIME);
		Graphics_setLineWidth (my graphics.get(), my instancePref_spectrogram_show() ? 1.0 : 3.0);
		Intensity_drawInside (my d_intensity.get(), my graphics.get(), my startWindow, my endWindow,
				my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo());
		Graphics_setLineWidth (my graphics.get(), 1.0);
		Graphics_setColour (my graphics.get(), Melder_BLACK);
	}
	
	my v_draw_analysis_formants ();
	
	/*
		Draw vertical scales.
	*/
	if (my instancePref_pitch_show()) {
		double pitchCursor_overt = undefined, pitchCursor_hidden = undefined;
		Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, pitchViewFrom_hidden, pitchViewTo_hidden);
		Graphics_setColour (my graphics.get(), Melder_BLUE);
		if (my d_pitch) {
			if (my startSelection == my endSelection)
				pitchCursor_hidden = Pitch_getValueAtTime (my d_pitch.get(), my startSelection, my p_pitch_unit, 1);
			else
				pitchCursor_hidden = Pitch_getMean (my d_pitch.get(), my startSelection, my endSelection, my p_pitch_unit);
			pitchCursor_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchCursor_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
			if (isdefined (pitchCursor_hidden)) {
				Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
				Graphics_text (my graphics.get(), my endWindow, pitchCursor_hidden,
					Melder_float (Melder_half (pitchCursor_overt)), U" ",
					Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL)
				);
			}
			if (isundef (pitchCursor_hidden) || Graphics_dyWCtoMM (my graphics.get(), pitchCursor_hidden - pitchViewFrom_hidden) > 5.0) {
				Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
				Graphics_text (my graphics.get(), my endWindow, pitchViewFrom_hidden - Graphics_dyMMtoWC (my graphics.get(), 0.5),
					Melder_float (Melder_half (pitchViewFrom_overt)), U" ",
					Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL)
				);
			}
			if (isundef (pitchCursor_hidden) || Graphics_dyWCtoMM (my graphics.get(), pitchViewTo_hidden - pitchCursor_hidden) > 5.0) {
				Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_TOP);
				Graphics_text (my graphics.get(), my endWindow, pitchViewTo_hidden,
					Melder_float (Melder_half (pitchViewTo_overt)), U" ",
					Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit, Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL)
				);
			}
		} else {
			Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
			Graphics_setFontSize (my graphics.get(), 10);
			Graphics_text (my graphics.get(), 0.5 * (my startWindow + my endWindow), 0.5 * (pitchViewFrom_hidden + pitchViewTo_hidden),
					U"(Cannot show pitch contour. Zoom out or change bottom of pitch range in pitch settings.)");
			Graphics_setFontSize (my graphics.get(), 12);
		}
		Graphics_setColour (my graphics.get(), Melder_BLACK);
	}
	if (my instancePref_intensity_show()) {
		double intensityCursor = undefined;
		MelderColour textColour;
		kGraphics_horizontalAlignment alignment;
		double y;
		if (! my instancePref_pitch_show()) {
			textColour = Melder_GREEN;
			alignment = Graphics_LEFT;
			y = my endWindow;
		} else if (! my instancePref_spectrogram_show() && ! my instancePref_formant_show()) {
			textColour = Melder_GREEN;
			alignment = Graphics_RIGHT;
			y = my startWindow;
		} else {
			textColour = ( my instancePref_spectrogram_show() ? Melder_LIME : Melder_GREEN );
			alignment = Graphics_RIGHT;
			y = my endWindow;
		}
		if (my instancePref_intensity_viewTo() > my instancePref_intensity_viewFrom()) {
			Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo());
			if (my d_intensity) {
				if (my startSelection == my endSelection) {
					intensityCursor = Vector_getValueAtX (my d_intensity.get(), my startSelection, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR);
				} else {
					intensityCursor = Intensity_getAverage (my d_intensity.get(), my startSelection, my endSelection, (int) my p_intensity_averagingMethod);
				}
			}
			Graphics_setColour (my graphics.get(), textColour);
			const bool intensityCursorVisible = ( isdefined (intensityCursor) &&
					intensityCursor > my instancePref_intensity_viewFrom() && intensityCursor < my instancePref_intensity_viewTo() );
			if (intensityCursorVisible) {
				static const conststring32 methodString [] = { U" (.5)", U" (μE)", U" (μS)", U" (μ)" };
				Graphics_setTextAlignment (my graphics.get(), alignment, Graphics_HALF);
				Graphics_text (my graphics.get(), y, intensityCursor,
					Melder_float (Melder_half (intensityCursor)), U" dB",
					my startSelection == my endSelection ? U"" : methodString [(int) my p_intensity_averagingMethod]
				);
			}
			if (! intensityCursorVisible || Graphics_dyWCtoMM (my graphics.get(), intensityCursor - my instancePref_intensity_viewFrom()) > 5.0) {
				Graphics_setTextAlignment (my graphics.get(), alignment, Graphics_BOTTOM);
				Graphics_text (my graphics.get(), y, my instancePref_intensity_viewFrom() - Graphics_dyMMtoWC (my graphics.get(), 0.5),
						Melder_float (Melder_half (my instancePref_intensity_viewFrom())), U" dB");
			}
			if (! intensityCursorVisible || Graphics_dyWCtoMM (my graphics.get(), my instancePref_intensity_viewTo() - intensityCursor) > 5.0) {
				Graphics_setTextAlignment (my graphics.get(), alignment, Graphics_TOP);
				Graphics_text (my graphics.get(), y, my instancePref_intensity_viewTo(),
						Melder_float (Melder_half (my instancePref_intensity_viewTo())), U" dB");
			}
			Graphics_setColour (my graphics.get(), Melder_BLACK);
		}
	}
	if (my instancePref_spectrogram_show() || my instancePref_formant_show()) {
		const bool frequencyCursorVisible = ( my d_spectrogram_cursor > my instancePref_spectrogram_viewFrom() && my d_spectrogram_cursor < my instancePref_spectrogram_viewTo() );
		Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo());
		/*
			Range marks.
		*/
		Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		if (! frequencyCursorVisible || Graphics_dyWCtoMM (my graphics.get(), my d_spectrogram_cursor - my instancePref_spectrogram_viewFrom()) > 5.0) {
			Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_BOTTOM);
			Graphics_text (my graphics.get(), my startWindow, my instancePref_spectrogram_viewFrom() - Graphics_dyMMtoWC (my graphics.get(), 0.5),
					Melder_float (Melder_half (my instancePref_spectrogram_viewFrom())), U" Hz");
		}
		if (! frequencyCursorVisible || Graphics_dyWCtoMM (my graphics.get(), my instancePref_spectrogram_viewTo() - my d_spectrogram_cursor) > 5.0) {
			Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_TOP);
			Graphics_text (my graphics.get(), my startWindow, my instancePref_spectrogram_viewTo(),
					Melder_float (Melder_half (my instancePref_spectrogram_viewTo())), U" Hz");
		}
		/*
			Cursor lines.
		*/
		Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
		Graphics_setColour (my graphics.get(), Melder_RED);
		if (frequencyCursorVisible) {
			const double x = my startWindow, y = my d_spectrogram_cursor;
			Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
			Graphics_text (my graphics.get(), x, y,   Melder_float (Melder_half (y)), U" Hz");
			Graphics_line (my graphics.get(), x, y, my endWindow, y);
		}
		/*
		if (our startSelection >= our startWindow && our startSelection <= our endWindow)
			Graphics_line (our graphics, our startSelection, our p_spectrogram_viewFrom, our startSelection, our p_spectrogram_viewTo);
		if (our endSelection > our startWindow && our endSelection < our endWindow && our endSelection != our startSelection)
			Graphics_line (our graphics, our endSelection, our p_spectrogram_viewFrom, our endSelection, our p_spectrogram_viewTo);*/
		/*
			Cadre.
		*/
		Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		Graphics_rectangle (my graphics.get(), my startWindow, my endWindow, my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo());
	}
}
void structTimeSoundAnalysisEditor :: v_draw_analysis () {
	TimeSoundAnalysisEditor_v_draw_analysis (this);
}

void structTimeSoundAnalysisEditor :: v_draw_analysis_formants () {
	TimeSoundAnalysisEditor_computeFormants (this);
	if (our instancePref_formant_show() && our d_formant) {
		Graphics_setSpeckleSize (our graphics.get(), our instancePref_formant_dotSize());
		Formant_drawSpeckles_inside (our d_formant.get(), our graphics.get(), our startWindow, our endWindow,
			our instancePref_spectrogram_viewFrom(), our instancePref_spectrogram_viewTo(), our instancePref_formant_dynamicRange(),
			Melder_RED, Melder_PINK, true
		);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
	}
}

void structTimeSoundAnalysisEditor :: v_draw_analysis_pulses () {
	TimeSoundAnalysisEditor_computePulses (this);
	if (our instancePref_pulses_show() && our endWindow - our startWindow <= our instancePref_longestAnalysis() && our d_pulses) {
		PointProcess point = our d_pulses.get();
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, -1.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_BLUE);
		if (point -> nt < 2000) for (integer i = 1; i <= point -> nt; i ++) {
			const double t = point -> t [i];
			if (t >= our startWindow && t <= our endWindow)
				Graphics_line (our graphics.get(), t, -0.9, t, 0.9);
		}
		Graphics_setColour (our graphics.get(), Melder_BLACK);
	}
}

bool structTimeSoundAnalysisEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		if (our instancePref_pitch_show()) {
			if (x_world >= our endWindow && y_fraction > 0.48 && y_fraction <= 0.50) {
				our setInstancePref_pitch_ceiling (our instancePref_pitch_ceiling() * 1.26);
				our d_pitch. reset();
				our d_intensity.reset();
				our d_pulses. reset();
				return FunctionEditor_UPDATE_NEEDED;
			}
			if (x_world >= our endWindow && y_fraction > 0.46 && y_fraction <= 0.48) {
				our setInstancePref_pitch_ceiling (our instancePref_pitch_ceiling() / 1.26);
				our d_pitch. reset();
				our d_intensity. reset();
				our d_pulses. reset();
				return FunctionEditor_UPDATE_NEEDED;
			}
		}
	}
	return TimeSoundAnalysisEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
}

void TimeSoundAnalysisEditor_init (TimeSoundAnalysisEditor me, conststring32 title, Function data, Sampled sound, bool ownSound) {
	TimeSoundEditor_init (me, title, data, sound, ownSound);
	/*
		Repair preferences.
	*/
	if (my instancePref_pitch_floor() >= my instancePref_pitch_ceiling()) {
		my setInstancePref_pitch_floor (Melder_atof (my default_pitch_floor()));
		my setInstancePref_pitch_ceiling (Melder_atof (my default_pitch_ceiling()));
		my p_pitch_unit = kPitch_unit::HERTZ;
	}
	if (my instancePref_spectrogram_viewFrom() >= my instancePref_spectrogram_viewTo()) {
		my setInstancePref_spectrogram_viewFrom (Melder_atof (my default_spectrogram_viewFrom()));
		my setInstancePref_spectrogram_viewTo (Melder_atof (my default_spectrogram_viewTo()));
	}
	if (my instancePref_intensity_viewFrom() >= my instancePref_intensity_viewTo()) {
		my setInstancePref_intensity_viewFrom (Melder_atof (my default_intensity_viewFrom()));
		my setInstancePref_intensity_viewTo (Melder_atof (my default_intensity_viewTo()));
	}
	if (my v_hasAnalysis ()) {
		if (my instancePref_log1_toLogFile() == false && my instancePref_log1_toInfoWindow() == false) {
			my setInstancePref_log1_toLogFile (true);
			my setInstancePref_log1_toInfoWindow (true);
		}
		if (my instancePref_log2_toLogFile() == false && my instancePref_log2_toInfoWindow() == false) {
			my setInstancePref_log2_toLogFile (true);
			my setInstancePref_log2_toInfoWindow (true);
		}
		if (! my v_hasSpectrogram ())
			my setInstancePref_spectrogram_show (false);
		if (! my v_hasPitch ())
			my setInstancePref_pitch_show (false);
		if (! my v_hasIntensity ())
			my setInstancePref_intensity_show (false);
		if (! my v_hasFormants ())
			my setInstancePref_formant_show (false);
		if (! my v_hasPulses ())
			my setInstancePref_pulses_show (false);
	}
}

/* End of file TimeSoundAnalysisEditor.cpp */
