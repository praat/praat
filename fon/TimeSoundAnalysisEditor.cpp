/* TimeSoundAnalysisEditor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2002/10/11 added a screen text for unavailable pitch
 * pb 2002/11/19 default log file names include "~" on Unix and Mach
 * pb 2002/11/19 added pulses and five separate analysis menus
 * pb 2003/02/19 clearer wording
 * pb 2003/02/24 spectral slices
 * pb 2003/03/03 Extract visible spectrogram: higher degree of oversampling than in editor
 * pb 2003/03/10 undid previous change because our PostScript code now does image interpolation
 * pb 2003/03/12 queriable
 * pb 2003/04/03 Get power at cursor cross
 * pb 2003/04/11 smaller log settings dialog
 * pb 2003/05/18 more shimmer measurements
 * pb 2003/05/20 longestAnalysis replaces the pitch/formant/intensity time steps,
 *               pitch.speckle, and formant.maximumDuration
 * pb 2003/05/21 pitch floor and ceiling replace the separate ranges for viewing and analysis
 * pb 2003/05/27 spectrogram maximum and autoscaling
 * pb 2003/07/20 moved voice report to VoiceAnalysis.c
 * pb 2003/08/23 reintroduced formant.numberOfTimeSteps
 * pb 2003/09/11 make sure that analyses objects can be extracted even before first drawing
 * pb 2003/09/16 advanced pitch settings: pitch.timeStep, pitch.viewFrom, pitch.viewTo, pitch.timeStepsPerView
 * pb 2003/10/01 time step settings: timeStepStrategy, fixedTimeStep, numberOfTimeStepsPerView
 * pb 2003/11/30 Sound_to_Spectrogram_windowShapeText
 * pb 2003/12/09 moved Spectrogram settings back in sync with preferences
 * pb 2004/02/15 highlight methods
 * pb 2004/04/16 introduced pulses.maximumPeriodFactor
 * pb 2004/05/12 extended voice report with mean F0 and harmonic-to-noise ratios
 * pb 2004/07/14 introduced pulses.maximumAmplitudeFactor
 * pb 2004/10/16 C++ compatible struct tags
 * pb 2004/10/24 intensity.averagingMethod
 * pb 2004/10/27 intensity.subtractMeanPressure
 * pb 2004/11/22 simplified Sound_to_Spectrum ()
 * pb 2004/11/28 improved screen text for unavailable pitch
 * pb 2004/11/28 warning in settings dialogs for non-standard time step strategies
 * pb 2005/01/11 getBottomOfSoundAndAnalysisArea
 * pb 2005/03/02 all pref string buffers are 260 bytes long
 * pb 2005/03/07 'intensity' logging sensitive to averaging method
 * pb 2005/06/16 units
 * pb 2005/08/18 editor name in log files
 * pb 2005/09/23 interface update
 * pb 2005/12/07 scrollStep
 * pb 2006/02/27 more helpful text when analyses are not shown
 * pb 2006/09/12 better messages if analysis not available
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/10 MelderInfo
 * pb 2006/12/18 better info
 * pb 2007/01/27 changed Vector API
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/09/01 computeXXX procedures are global on behalf of the inheritors
 * pb 2007/09/02 Picture window drawing
 * pb 2007/09/08 inherit from TimeSoundEditor
 * pb 2007/09/19 info
 * pb 2007/09/21 split Query menu
 * pb 2007/11/01 direct intensity, formants, and pulses drawing
 * pb 2007/11/30 erased Graphics_printf
 * pb 2009/11/30 Move frequency cursor to...
 * pb 2010/01/15 corrected checking of "Show pitch" menu item (and so on) when the command is called from a script
 * pb 2011/03/23 C++
 * pb 2011/07/15 C++
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

#undef our
#define our ((TimeSoundAnalysisEditor_Table) my methods) ->
#define its ((TimeSoundAnalysisEditor_Table) methods) ->

static const wchar * theMessage_Cannot_compute_spectrogram = L"The spectrogram is not defined at the edge of the sound.";
static const wchar * theMessage_Cannot_compute_pitch = L"The pitch contour is not defined at the edge of the sound.";
static const wchar * theMessage_Cannot_compute_formant = L"The formants are not defined at the edge of the sound.";
static const wchar * theMessage_Cannot_compute_intensity = L"The intensity curve is not defined at the edge of the sound.";
static const wchar * theMessage_Cannot_compute_pulses = L"The pulses are not defined at the edge of the sound.";

#if defined (macintosh)
	static const wchar * LOG_1_FILE_NAME = L"~/Desktop/Pitch Log";
	static const wchar * LOG_2_FILE_NAME = L"~/Desktop/Formant Log";
	static const wchar * LOG_3_FILE_NAME = L"~/Desktop/Log script 3";
	static const wchar * LOG_4_FILE_NAME = L"~/Desktop/Log script 4";
#elif defined (WIN32)
	static const wchar * LOG_1_FILE_NAME = L"C:\\WINDOWS\\DESKTOP\\Pitch Log.txt";
	static const wchar * LOG_2_FILE_NAME = L"C:\\WINDOWS\\DESKTOP\\Formant Log.txt";
	static const wchar * LOG_3_FILE_NAME = L"C:\\WINDOWS\\DESKTOP\\Log script 3.praat";
	static const wchar * LOG_4_FILE_NAME = L"C:\\WINDOWS\\DESKTOP\\Log script 4.praat";
#else
	static const wchar * LOG_1_FILE_NAME = L"~/pitch_log";
	static const wchar * LOG_2_FILE_NAME = L"~/formant_log";
	static const wchar * LOG_3_FILE_NAME = L"~/log_script3";
	static const wchar * LOG_4_FILE_NAME = L"~/log_script4";
#endif
static const wchar * LOG_1_FORMAT = L"Time 'time:6' seconds, pitch 'f0:2' Hz";
static const wchar * LOG_2_FORMAT = L"'t1:4''tab$''t2:4''tab$''f1:0''tab$''f2:0''tab$''f3:0'";

struct logInfo {
	bool toInfoWindow, toLogFile;
	wchar fileName [Preferences_STRING_BUFFER_SIZE], format [Preferences_STRING_BUFFER_SIZE];
};

static struct {
	double longestAnalysis;
	enum kTimeSoundAnalysisEditor_timeStepStrategy timeStepStrategy;
	double fixedTimeStep;
	long numberOfTimeStepsPerView;
	struct FunctionEditor_spectrogram spectrogram;
	struct FunctionEditor_pitch pitch;
	struct FunctionEditor_intensity intensity;
	struct FunctionEditor_formant formant;
	struct FunctionEditor_pulses pulses;
	struct logInfo log [2];
	wchar logScript3 [Preferences_STRING_BUFFER_SIZE], logScript4 [Preferences_STRING_BUFFER_SIZE];
} preferences;

void TimeSoundAnalysisEditor_prefs (void) {
	Preferences_addDouble (L"FunctionEditor.longestAnalysis", & preferences.longestAnalysis, 10.0);   // seconds
	Preferences_addEnum (L"FunctionEditor.timeStepStrategy", & preferences.timeStepStrategy, kTimeSoundAnalysisEditor_timeStepStrategy, DEFAULT);
	Preferences_addDouble (L"FunctionEditor.fixedTimeStep", & preferences.fixedTimeStep, 0.01);   // seconds
	Preferences_addLong (L"FunctionEditor.numberOfTimeStepsPerView", & preferences.numberOfTimeStepsPerView, 100);
	Preferences_addBool (L"FunctionEditor.spectrogram.show", & preferences.spectrogram.show, true);
	Preferences_addDouble (L"FunctionEditor.spectrogram.viewFrom2", & preferences.spectrogram.viewFrom, 0.0);   // Hz
	Preferences_addDouble (L"FunctionEditor.spectrogram.viewTo2", & preferences.spectrogram.viewTo, 5000.0);   // Hz
	Preferences_addDouble (L"FunctionEditor.spectrogram.windowLength2", & preferences.spectrogram.windowLength, 0.005);   // Hz
	Preferences_addDouble (L"FunctionEditor.spectrogram.dynamicRange2", & preferences.spectrogram.dynamicRange, 70.0);   // dB
	Preferences_addLong (L"FunctionEditor.spectrogram.timeSteps2", & preferences.spectrogram.timeSteps, 1000);
	Preferences_addLong (L"FunctionEditor.spectrogram.frequencySteps2", & preferences.spectrogram.frequencySteps, 250);
	Preferences_addEnum (L"FunctionEditor.spectrogram.method2", & preferences.spectrogram.method, kSound_to_Spectrogram_method, DEFAULT);
	Preferences_addEnum (L"FunctionEditor.spectrogram.windowShape2", & preferences.spectrogram.windowShape, kSound_to_Spectrogram_windowShape, DEFAULT);
	Preferences_addBool (L"FunctionEditor.spectrogram.autoscaling2", & preferences.spectrogram.autoscaling, true);
	Preferences_addDouble (L"FunctionEditor.spectrogram.maximum2", & preferences.spectrogram.maximum, 100.0);   // dB/Hz
	Preferences_addDouble (L"FunctionEditor.spectrogram.preemphasis2", & preferences.spectrogram.preemphasis, 6.0);   // dB/octave
	Preferences_addDouble (L"FunctionEditor.spectrogram.dynamicCompression2", & preferences.spectrogram.dynamicCompression, 0.0);
	Preferences_addBool (L"FunctionEditor.pitch.show", & preferences.pitch.show, true);
	Preferences_addDouble (L"FunctionEditor.pitch.floor", & preferences.pitch.floor, 75.0);
	Preferences_addDouble (L"FunctionEditor.pitch.ceiling", & preferences.pitch.ceiling, 500.0);
	Preferences_addEnum (L"FunctionEditor.pitch.unit", & preferences.pitch.unit, kPitch_unit, DEFAULT);
	Preferences_addEnum (L"FunctionEditor.pitch.drawingMethod", & preferences.pitch.drawingMethod, kTimeSoundAnalysisEditor_pitch_drawingMethod, DEFAULT);
	Preferences_addDouble (L"FunctionEditor.pitch.viewFrom", & preferences.pitch.viewFrom, 0.0);   // auto
	Preferences_addDouble (L"FunctionEditor.pitch.viewTo", & preferences.pitch.viewTo, 0.0);   // auto
	Preferences_addEnum (L"FunctionEditor.pitch.method", & preferences.pitch.method, kTimeSoundAnalysisEditor_pitch_analysisMethod, DEFAULT);
	Preferences_addBool (L"FunctionEditor.pitch.veryAccurate", & preferences.pitch.veryAccurate, false);
	Preferences_addLong (L"FunctionEditor.pitch.maximumNumberOfCandidates", & preferences.pitch.maximumNumberOfCandidates, 15);
	Preferences_addDouble (L"FunctionEditor.pitch.silenceThreshold", & preferences.pitch.silenceThreshold, 0.03);
	Preferences_addDouble (L"FunctionEditor.pitch.voicingThreshold", & preferences.pitch.voicingThreshold, 0.45);
	Preferences_addDouble (L"FunctionEditor.pitch.octaveCost", & preferences.pitch.octaveCost, 0.01);
	Preferences_addDouble (L"FunctionEditor.pitch.octaveJumpCost", & preferences.pitch.octaveJumpCost, 0.35);
	Preferences_addDouble (L"FunctionEditor.pitch.voicedUnvoicedCost", & preferences.pitch.voicedUnvoicedCost, 0.14);
	Preferences_addBool (L"FunctionEditor.intensity.show", & preferences.intensity.show, false);
	Preferences_addDouble (L"FunctionEditor.intensity.viewFrom", & preferences.intensity.viewFrom, 50.0);   // dB
	Preferences_addDouble (L"FunctionEditor.intensity.viewTo", & preferences.intensity.viewTo, 100.0);   // dB
	Preferences_addEnum (L"FunctionEditor.intensity.averagingMethod", & preferences.intensity.averagingMethod, kTimeSoundAnalysisEditor_intensity_averagingMethod, DEFAULT);
	Preferences_addBool (L"FunctionEditor.intensity.subtractMeanPressure", & preferences.intensity.subtractMeanPressure, true);
	Preferences_addBool (L"FunctionEditor.formant.show", & preferences.formant.show, false);
	Preferences_addDouble (L"FunctionEditor.formant.maximumFormant", & preferences.formant.maximumFormant, 5500.0);   // Hz
	Preferences_addLong (L"FunctionEditor.formant.numberOfPoles", & preferences.formant.numberOfPoles, 10);
	Preferences_addDouble (L"FunctionEditor.formant.windowLength", & preferences.formant.windowLength, 0.025);   // seconds
	Preferences_addDouble (L"FunctionEditor.formant.dynamicRange", & preferences.formant.dynamicRange, 30.0);   // dB
	Preferences_addDouble (L"FunctionEditor.formant.dotSize", & preferences.formant.dotSize, 1.0);   // mm
	Preferences_addEnum (L"FunctionEditor.formant.method", & preferences.formant.method, kTimeSoundAnalysisEditor_formant_analysisMethod, DEFAULT);
	Preferences_addDouble (L"FunctionEditor.formant.preemphasisFrom", & preferences.formant.preemphasisFrom, 50.0);   // Hz
	Preferences_addBool (L"FunctionEditor.pulses.show", & preferences.pulses.show, false);
	Preferences_addDouble (L"FunctionEditor.pulses.maximumPeriodFactor", & preferences.pulses.maximumPeriodFactor, 1.3);
	Preferences_addDouble (L"FunctionEditor.pulses.maximumAmplitudeFactor", & preferences.pulses.maximumAmplitudeFactor, 1.6);
	Preferences_addBool (L"FunctionEditor.log1.toInfoWindow", & preferences.log[0].toInfoWindow, true);
	Preferences_addBool (L"FunctionEditor.log1.toLogFile", & preferences.log[0].toLogFile, true);
	Preferences_addString (L"FunctionEditor.log1.fileName", & preferences.log[0].fileName [0], LOG_1_FILE_NAME);
	Preferences_addString (L"FunctionEditor.log1.format", & preferences.log[0].format [0], LOG_1_FORMAT);
	Preferences_addBool (L"FunctionEditor.log2.toInfoWindow", & preferences.log[1].toInfoWindow, true);
	Preferences_addBool (L"FunctionEditor.log2.toLogFile", & preferences.log[1].toLogFile, true);
	Preferences_addString (L"FunctionEditor.log2.fileName", & preferences.log[1].fileName [0], LOG_2_FILE_NAME);
	Preferences_addString (L"FunctionEditor.log2.format", & preferences.log[1].format [0], LOG_2_FORMAT);
	Preferences_addString (L"FunctionEditor.logScript3", & preferences.logScript3 [0], LOG_3_FILE_NAME);
	Preferences_addString (L"FunctionEditor.logScript4", & preferences.logScript4 [0], LOG_4_FILE_NAME);
}

void structTimeSoundAnalysisEditor :: v_destroy () {
	destroy_analysis ();
	TimeSoundAnalysisEditor_Parent :: v_destroy ();
}

void structTimeSoundAnalysisEditor :: v_info () {
	TimeSoundAnalysisEditor_Parent :: v_info ();
	/* Spectrogram flag: */
	MelderInfo_writeLine2 (L"Spectrogram show: ", Melder_boolean (spectrogram.show));
	/* Spectrogram settings: */
	MelderInfo_writeLine3 (L"Spectrogram view from: ", Melder_double (spectrogram.viewFrom), L" Hz");
	MelderInfo_writeLine3 (L"Spectrogram view to: ", Melder_double (spectrogram.viewTo), L" Hz");
	MelderInfo_writeLine3 (L"Spectrogram window length: ", Melder_double (spectrogram.windowLength), L" seconds");
	MelderInfo_writeLine3 (L"Spectrogram dynamic range: ", Melder_double (spectrogram.dynamicRange), L" dB");
	/* Advanced spectrogram settings: */
	MelderInfo_writeLine2 (L"Spectrogram number of time steps: ", Melder_integer (spectrogram.timeSteps));
	MelderInfo_writeLine2 (L"Spectrogram number of frequency steps: ", Melder_integer (spectrogram.frequencySteps));
	MelderInfo_writeLine2 (L"Spectrogram method: ", L"Fourier");
	MelderInfo_writeLine2 (L"Spectrogram window shape: ", kSound_to_Spectrogram_windowShape_getText (spectrogram.windowShape));
	MelderInfo_writeLine2 (L"Spectrogram autoscaling: ", Melder_boolean (spectrogram.autoscaling));
	MelderInfo_writeLine3 (L"Spectrogram maximum: ", Melder_double (spectrogram.maximum), L" dB/Hz");
	MelderInfo_writeLine3 (L"Spectrogram pre-emphasis: ", Melder_integer (spectrogram.preemphasis), L" dB/octave");
	MelderInfo_writeLine2 (L"Spectrogram dynamicCompression: ", Melder_integer (spectrogram.dynamicCompression));
	/* Dynamic information: */
	MelderInfo_writeLine3 (L"Spectrogram cursor frequency: ", Melder_double (spectrogram.cursor), L" Hz");
	/* Pitch flag: */
	MelderInfo_writeLine2 (L"Pitch show: ", Melder_boolean (pitch.show));
	/* Pitch settings: */
	MelderInfo_writeLine3 (L"Pitch floor: ", Melder_double (pitch.floor), L" Hz");
	MelderInfo_writeLine3 (L"Pitch ceiling: ", Melder_double (pitch.ceiling), L" Hz");
	MelderInfo_writeLine2 (L"Pitch unit: ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, pitch.unit, Function_UNIT_TEXT_MENU));
	MelderInfo_writeLine2 (L"Pitch drawing method: ", kTimeSoundAnalysisEditor_pitch_drawingMethod_getText (pitch.drawingMethod));
	/* Advanced pitch settings: */
	MelderInfo_writeLine4 (L"Pitch view from: ", Melder_double (pitch.viewFrom), L" ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, pitch.unit, Function_UNIT_TEXT_MENU));
	MelderInfo_writeLine4 (L"Pitch view to: ", Melder_double (pitch.viewTo), L" ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, pitch.unit, Function_UNIT_TEXT_MENU));
	MelderInfo_writeLine2 (L"Pitch method: ", kTimeSoundAnalysisEditor_pitch_analysisMethod_getText (pitch.method));
	MelderInfo_writeLine2 (L"Pitch very accurate: ", Melder_boolean (pitch.veryAccurate));
	MelderInfo_writeLine2 (L"Pitch max. number of candidates: ", Melder_integer (pitch.maximumNumberOfCandidates));
	MelderInfo_writeLine3 (L"Pitch silence threshold: ", Melder_double (pitch.silenceThreshold), L" of global peak");
	MelderInfo_writeLine3 (L"Pitch voicing threshold: ", Melder_double (pitch.voicingThreshold), L" (periodic power / total power)");
	MelderInfo_writeLine3 (L"Pitch octave cost: ", Melder_double (pitch.octaveCost), L" per octave");
	MelderInfo_writeLine3 (L"Pitch octave jump cost: ", Melder_double (pitch.octaveJumpCost), L" per octave");
	MelderInfo_writeLine2 (L"Pitch voiced/unvoiced cost: ", Melder_double (pitch.voicedUnvoicedCost));
	/* Intensity flag: */
	MelderInfo_writeLine2 (L"Intensity show: ", Melder_boolean (intensity.show));
	/* Intensity settings: */
	MelderInfo_writeLine3 (L"Intensity view from: ", Melder_double (intensity.viewFrom), L" dB");
	MelderInfo_writeLine3 (L"Intensity view to: ", Melder_double (intensity.viewTo), L" dB");
	MelderInfo_writeLine2 (L"Intensity averaging method: ", kTimeSoundAnalysisEditor_intensity_averagingMethod_getText (intensity.averagingMethod));
	MelderInfo_writeLine2 (L"Intensity subtract mean pressure: ", Melder_boolean (intensity.subtractMeanPressure));
	/* Formant flag: */
	MelderInfo_writeLine2 (L"Formant show: ", Melder_boolean (formant.show));
	/* Formant settings: */
	MelderInfo_writeLine3 (L"Formant maximum formant: ", Melder_double (formant.maximumFormant), L" Hz");
	MelderInfo_writeLine2 (L"Formant number of poles: ", Melder_integer (formant.numberOfPoles));
	MelderInfo_writeLine3 (L"Formant window length: ", Melder_double (formant.windowLength), L" seconds");
	MelderInfo_writeLine3 (L"Formant dynamic range: ", Melder_double (formant.dynamicRange), L" dB");
	MelderInfo_writeLine3 (L"Formant dot size: ", Melder_double (formant.dotSize), L" mm");
	/* Advanced formant settings: */
	MelderInfo_writeLine2 (L"Formant method: ", kTimeSoundAnalysisEditor_formant_analysisMethod_getText (formant.method));
	MelderInfo_writeLine3 (L"Formant pre-emphasis from: ", Melder_double (formant.preemphasisFrom), L" Hz");
	/* Pulses flag: */
	MelderInfo_writeLine2 (L"Pulses show: ", Melder_boolean (pulses.show));
	MelderInfo_writeLine2 (L"Pulses maximum period factor: ", Melder_double (pulses.maximumPeriodFactor));
	MelderInfo_writeLine2 (L"Pulses maximum amplitude factor: ", Melder_double (pulses.maximumAmplitudeFactor));
}

void structTimeSoundAnalysisEditor :: destroy_analysis () {
	forget (spectrogram.data);
	forget (pitch.data);
	forget (intensity.data);
	forget (formant.data);
	forget (pulses.data);
}

enum {
	FunctionEditor_PART_CURSOR = 1,
	FunctionEditor_PART_SELECTION = 2
};

static const wchar *FunctionEditor_partString (int part) {
	static const wchar *strings [] = { L"", L"CURSOR", L"SELECTION" };
	return strings [part];
}

static const wchar *FunctionEditor_partString_locative (int part) {
	static const wchar *strings [] = { L"", L"at CURSOR", L"in SELECTION" };
	return strings [part];
}

static int makeQueriable (TimeSoundAnalysisEditor me, int allowCursor, double *tmin, double *tmax) {
	if (my endWindow - my startWindow > my longestAnalysis) {
		return Melder_error5 (L"Window too long to show analyses. Zoom in to at most ", Melder_half (my longestAnalysis), L" seconds "
			"or set the \"longest analysis\" to at least ", Melder_half (my endWindow - my startWindow), L" seconds.");
	}
	if (my startSelection == my endSelection) {
		if (allowCursor) {
			*tmin = *tmax = my startSelection;
			return FunctionEditor_PART_CURSOR;
		} else {
			return Melder_error1 (L"Make a selection first.");
		}
	} else if (my startSelection < my startWindow || my endSelection > my endWindow) {
		return Melder_error1 (L"Command ambiguous: a part of the selection is out of view. Either zoom or re-select.");
	}
	*tmin = my startSelection;
	*tmax = my endSelection;
	return FunctionEditor_PART_SELECTION;
}

static int menu_cb_logSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Log settings", L"Log files")
		OPTIONMENU (L"Write log 1 to", 3)
			OPTION (L"Log file only")
			OPTION (L"Info window only")
			OPTION (L"Log file and Info window")
		LABEL (L"", L"Log file 1:")
		TEXTFIELD (L"Log file 1", LOG_1_FILE_NAME)
		LABEL (L"", L"Log 1 format:")
		TEXTFIELD (L"Log 1 format", LOG_1_FORMAT)
		OPTIONMENU (L"Write log 2 to", 3)
			OPTION (L"Log file only")
			OPTION (L"Info window only")
			OPTION (L"Log file and Info window")
		LABEL (L"", L"Log file 2:")
		TEXTFIELD (L"Log file 2", LOG_2_FILE_NAME)
		LABEL (L"", L"Log 2 format:")
		TEXTFIELD (L"Log 2 format", LOG_2_FORMAT)
		LABEL (L"", L"Log script 3:")
		TEXTFIELD (L"Log script 3", LOG_3_FILE_NAME)
		LABEL (L"", L"Log script 4:")
		TEXTFIELD (L"Log script 4", LOG_4_FILE_NAME)
	EDITOR_OK
		SET_INTEGER (L"Write log 1 to", preferences.log[0].toLogFile + 2 * preferences.log[0].toInfoWindow)
		SET_STRING (L"Log file 1", preferences.log[0].fileName)
		SET_STRING (L"Log 1 format", preferences.log[0].format)
		SET_INTEGER (L"Write log 2 to", preferences.log[1].toLogFile + 2 * preferences.log[1].toInfoWindow)
		SET_STRING (L"Log file 2", preferences.log[1].fileName)
		SET_STRING (L"Log 2 format", preferences.log[1].format)
		SET_STRING (L"Log script 3", preferences.logScript3)
		SET_STRING (L"Log script 4", preferences.logScript4)
	EDITOR_DO
		preferences.log[0].toLogFile = (GET_INTEGER (L"Write log 1 to") & 1) != 0;
		preferences.log[0].toInfoWindow = (GET_INTEGER (L"Write log 1 to") & 2) != 0;
		wcscpy (preferences.log[0].fileName, GET_STRING (L"Log file 1"));
		wcscpy (preferences.log[0].format, GET_STRING (L"Log 1 format"));
		preferences.log[1].toLogFile = (GET_INTEGER (L"Write log 2 to") & 1) != 0;
		preferences.log[1].toInfoWindow = (GET_INTEGER (L"Write log 2 to") & 2) != 0;
		wcscpy (preferences.log[1].fileName, GET_STRING (L"Log file 2"));
		wcscpy (preferences.log[1].format, GET_STRING (L"Log 2 format"));
		wcscpy (preferences.logScript3, GET_STRING (L"Log script 3"));
		wcscpy (preferences.logScript4, GET_STRING (L"Log script 4"));
	EDITOR_END
}

static int do_deleteLogFile (TimeSoundAnalysisEditor me, int which) {
	structMelderFile file = { 0 };
	(void) me;
	Melder_pathToFile (preferences.log[which].fileName, & file);
	MelderFile_delete (& file);
	return 1;
}
static int menu_cb_deleteLogFile1 (EDITOR_ARGS) { EDITOR_IAM (TimeSoundAnalysisEditor); return do_deleteLogFile (me, 0); }
static int menu_cb_deleteLogFile2 (EDITOR_ARGS) { EDITOR_IAM (TimeSoundAnalysisEditor); return do_deleteLogFile (me, 1); }

static int do_log (TimeSoundAnalysisEditor me, int which) {
	wchar format [1000], *p;
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	wcscpy (format, preferences.log[which].format);
	for (p = format; *p !='\0'; p ++) if (*p == '\'') {
		/*
		 * Found a left quote. Search for a matching right quote.
		 */
		wchar_t *q = p + 1, varName [300], *r, *s, *colon;
		int precision = -1;
		double value = NUMundefined;
		const wchar_t *stringValue = NULL;
		while (*q != '\0' && *q != '\'') q ++;
		if (*q == '\0') break;   /* No matching right quote: done with this line. */
		if (q - p == 1) continue;   /* Ignore empty variable names. */
		/*
		 * Found a right quote. Get potential variable name.
		 */
		for (r = p + 1, s = varName; q - r > 0; r ++, s ++) *s = *r;
		*s = '\0';   /* Trailing null byte. */
		colon = wcschr (varName, ':');
		if (colon) {
			precision = wcstol (colon + 1, NULL, 10);
			*colon = '\0';
		}
		if (wcsequ (varName, L"time")) {
			value = 0.5 * (tmin + tmax);
		} else if (wcsequ (varName, L"t1")) {
			value = tmin;
		} else if (wcsequ (varName, L"t2")) {
			value = tmax;
		} else if (wcsequ (varName, L"dur")) {
			value = tmax - tmin;
		} else if (wcsequ (varName, L"freq")) {
			value = my spectrogram.cursor;
		} else if (wcsequ (varName, L"tab$")) {
			stringValue = L"\t";
		} else if (wcsequ (varName, L"editor$")) {
			stringValue = my name;
		} else if (wcsequ (varName, L"f0")) {
			if (! my pitch.show)
				return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
			if (! my pitch.data) {
				return Melder_error1 (theMessage_Cannot_compute_pitch);
			}
			if (part == FunctionEditor_PART_CURSOR) {
				value = Pitch_getValueAtTime (my pitch.data, tmin, my pitch.unit, 1);
			} else {
				value = Pitch_getMean (my pitch.data, tmin, tmax, my pitch.unit);
			}
		} else if (varName [0] == 'f' && varName [1] >= '1' && varName [1] <= '5' && varName [2] == '\0') {
			if (! my formant.show)
				return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
			if (! my formant.data) {
				return Melder_error1 (theMessage_Cannot_compute_formant);
			}
			if (part == FunctionEditor_PART_CURSOR) {
				value = Formant_getValueAtTime (my formant.data, varName [1] - '0', tmin, 0);
			} else {
				value = Formant_getMean (my formant.data, varName [1] - '0', tmin, tmax, 0);
			}
		} else if (varName [0] == 'b' && varName [1] >= '1' && varName [1] <= '5' && varName [2] == '\0') {
			if (! my formant.show)
				return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
			if (! my formant.data) {
				return Melder_error1 (theMessage_Cannot_compute_formant);
			}
			value = Formant_getBandwidthAtTime (my formant.data, varName [1] - '0', 0.5 * (tmin + tmax), 0);
		} else if (wcsequ (varName, L"intensity")) {
			if (! my intensity.show)
				return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
			if (! my intensity.data) {
				return Melder_error1 (theMessage_Cannot_compute_intensity);
			}
			if (part == FunctionEditor_PART_CURSOR) {
				value = Vector_getValueAtX (my intensity.data, tmin, Vector_CHANNEL_1, Vector_VALUE_INTERPOLATION_LINEAR);
			} else {
				value = Intensity_getAverage (my intensity.data, tmin, tmax, my intensity.averagingMethod);
			}
		} else if (wcsequ (varName, L"power")) {
			if (! my spectrogram.show)
				return Melder_error1 (L"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
			if (! my spectrogram.data) {
				return Melder_error1 (theMessage_Cannot_compute_spectrogram);
			}
			if (part != FunctionEditor_PART_CURSOR) return Melder_error1 (L"Click inside the spectrogram first.");
			value = Matrix_getValueAtXY (my spectrogram.data, tmin, my spectrogram.cursor);
		}
		if (NUMdefined (value)) {
			int varlen = (q - p) - 1, headlen = p - format;
			wchar_t formattedNumber [400];
			if (precision >= 0) {
				swprintf (formattedNumber, 400, L"%.*f", precision, value);
			} else {
				swprintf (formattedNumber, 400, L"%.17g", value);
			}
			int arglen = wcslen (formattedNumber);
			static MelderString buffer = { 0 };
			MelderString_ncopy (& buffer, format, headlen);
			MelderString_append2 (& buffer, formattedNumber, p + varlen + 2);
			wcscpy (format, buffer.string);
			p += arglen - 1;
		} else if (stringValue != NULL) {
			int varlen = (q - p) - 1, headlen = p - format, arglen = wcslen (stringValue);
			static MelderString buffer = { 0 };
			MelderString_ncopy (& buffer, format, headlen);
			MelderString_append2 (& buffer, stringValue, p + varlen + 2);
			wcscpy (format, buffer.string);
			p += arglen - 1;
		} else {
			p = q - 1;   /* Go to before next quote. */
		}
	}
	if (preferences.log[which].toInfoWindow) {
		MelderInfo_write1 (format);
		MelderInfo_close ();
	}
	if (preferences.log[which].toLogFile) {
		structMelderFile file = { 0 };
		wcscat (format, L"\n");
		Melder_relativePathToFile (preferences.log[which].fileName, & file);
		MelderFile_appendText (& file, format);
	}
	return 1;
}

static int menu_cb_log1 (EDITOR_ARGS) { EDITOR_IAM (TimeSoundAnalysisEditor); return do_log (me, 0); }
static int menu_cb_log2 (EDITOR_ARGS) { EDITOR_IAM (TimeSoundAnalysisEditor); return do_log (me, 1); }

static int menu_cb_logScript3 (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	DO_RunTheScriptFromAnyAddedEditorCommand (me, preferences.logScript3);
	return 1;
}
static int menu_cb_logScript4 (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	DO_RunTheScriptFromAnyAddedEditorCommand (me, preferences.logScript4);
	return 1;
}

static int menu_cb_showAnalyses (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Show analyses", 0)
		BOOLEAN (L"Show spectrogram", 1)
		BOOLEAN (L"Show pitch", 1)
		BOOLEAN (L"Show intensity", 0)
		BOOLEAN (L"Show formants", 0)
		BOOLEAN (L"Show pulses", 0)
		POSITIVE (L"Longest analysis (s)", L"5.0")
	EDITOR_OK
		SET_INTEGER (L"Show spectrogram", my spectrogram.show)
		SET_INTEGER (L"Show pitch", my pitch.show)
		SET_INTEGER (L"Show intensity", my intensity.show)
		SET_INTEGER (L"Show formants", my formant.show)
		SET_INTEGER (L"Show pulses", my pulses.show)
		SET_REAL (L"Longest analysis", my longestAnalysis)
	EDITOR_DO
		GuiMenuItem_check (my spectrogramToggle, preferences.spectrogram.show = my spectrogram.show = GET_INTEGER (L"Show spectrogram"));
		GuiMenuItem_check (my pitchToggle, preferences.pitch.show = my pitch.show = GET_INTEGER (L"Show pitch"));
		GuiMenuItem_check (my intensityToggle, preferences.intensity.show = my intensity.show = GET_INTEGER (L"Show intensity"));
		GuiMenuItem_check (my formantToggle, preferences.formant.show = my formant.show = GET_INTEGER (L"Show formants"));
		GuiMenuItem_check (my pulsesToggle, preferences.pulses.show = my pulses.show = GET_INTEGER (L"Show pulses"));
		preferences.longestAnalysis = my longestAnalysis = GET_REAL (L"Longest analysis");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_timeStepSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Time step settings", L"Time step settings...")
		OPTIONMENU_ENUM (L"Time step strategy", kTimeSoundAnalysisEditor_timeStepStrategy, DEFAULT)
		LABEL (L"", L"")
		LABEL (L"", L"If the time step strategy is \"fixed\":")
		POSITIVE (L"Fixed time step (s)", L"0.01")
		LABEL (L"", L"")
		LABEL (L"", L"If the time step strategy is \"view-dependent\":")
		NATURAL (L"Number of time steps per view", L"100")
	EDITOR_OK
		SET_ENUM (L"Time step strategy", kTimeSoundAnalysisEditor_timeStepStrategy, my timeStepStrategy)
		SET_REAL (L"Fixed time step", my fixedTimeStep)
		SET_INTEGER (L"Number of time steps per view", my numberOfTimeStepsPerView)
	EDITOR_DO
		preferences.timeStepStrategy = my timeStepStrategy = GET_ENUM (kTimeSoundAnalysisEditor_timeStepStrategy, L"Time step strategy");
		preferences.fixedTimeStep = my fixedTimeStep = GET_REAL (L"Fixed time step");
		preferences.numberOfTimeStepsPerView = my numberOfTimeStepsPerView = GET_INTEGER (L"Number of time steps per view");
		forget (my pitch.data);
		forget (my formant.data);
		forget (my intensity.data);
		forget (my pulses.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

/***** SPECTROGRAM MENU *****/

static int menu_cb_showSpectrogram (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	preferences.spectrogram.show = my spectrogram.show = ! my spectrogram.show;
	GuiMenuItem_check (my spectrogramToggle, my spectrogram.show);   // in case we're called from a script
	FunctionEditor_redraw (me);
	return 1;
}

static int menu_cb_spectrogramSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Spectrogram settings", L"Intro 3.2. Configuring the spectrogram")
		REAL (L"left View range (Hz)", L"0.0")
		POSITIVE (L"right View range (Hz)", L"5000.0")
		POSITIVE (L"Window length (s)", L"0.005")
		POSITIVE (L"Dynamic range (dB)", L"50.0")
		LABEL (L"note1", L"")
		LABEL (L"note2", L"")
	EDITOR_OK
		SET_REAL (L"left View range", my spectrogram.viewFrom)
		SET_REAL (L"right View range", my spectrogram.viewTo)
		SET_REAL (L"Window length", my spectrogram.windowLength)
		SET_REAL (L"Dynamic range", my spectrogram.dynamicRange)
		if (my spectrogram.timeSteps != 1000 || my spectrogram.frequencySteps != 250 || my spectrogram.method != 1 ||
			my spectrogram.windowShape != 5 || my spectrogram.maximum != 100.0 || ! my spectrogram.autoscaling ||
			my spectrogram.preemphasis != 6.0 || my spectrogram.dynamicCompression != 0.0)
		{
			SET_STRING (L"note1", L"Warning: you have non-standard \"advanced settings\".")
		} else {
			SET_STRING (L"note1", L"(all of your \"advanced settings\" have their standard values)")
		}
		if (my timeStepStrategy != kTimeSoundAnalysisEditor_timeStepStrategy_DEFAULT) {
			SET_STRING (L"note2", L"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (L"note2", L"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		preferences.spectrogram.viewFrom = my spectrogram.viewFrom = GET_REAL (L"left View range");
		preferences.spectrogram.viewTo = my spectrogram.viewTo = GET_REAL (L"right View range");
		preferences.spectrogram.windowLength = my spectrogram.windowLength = GET_REAL (L"Window length");
		preferences.spectrogram.dynamicRange = my spectrogram.dynamicRange = GET_REAL (L"Dynamic range");
		forget (my spectrogram.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_advancedSpectrogramSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Advanced spectrogram settings", L"Advanced spectrogram settings...")
		LABEL (L"", L"Time and frequency resolutions:")
		NATURAL (L"Number of time steps", L"1000")
		NATURAL (L"Number of frequency steps", L"250")
		LABEL (L"", L"Spectrogram analysis settings:")
		OPTIONMENU_ENUM (L"Method", kSound_to_Spectrogram_method, DEFAULT)
		OPTIONMENU_ENUM (L"Window shape", kSound_to_Spectrogram_windowShape, DEFAULT)
		LABEL (L"", L"Spectrogram view settings:")
		BOOLEAN (L"Autoscaling", 1)
		REAL (L"Maximum (dB/Hz)", L"100.0")
		REAL (L"Pre-emphasis (dB/oct)", L"6.0")
		REAL (L"Dynamic compression (0-1)", L"0.0")
	EDITOR_OK
		SET_INTEGER (L"Number of time steps", my spectrogram.timeSteps)
		SET_INTEGER (L"Number of frequency steps", my spectrogram.frequencySteps)
		SET_ENUM (L"Method", kSound_to_Spectrogram_method, my spectrogram.method)
		SET_ENUM (L"Window shape", kSound_to_Spectrogram_windowShape, my spectrogram.windowShape)
		SET_REAL (L"Maximum", my spectrogram.maximum)
		SET_INTEGER (L"Autoscaling", my spectrogram.autoscaling)
		SET_REAL (L"Pre-emphasis", my spectrogram.preemphasis)
		SET_REAL (L"Dynamic compression", my spectrogram.dynamicCompression)
	EDITOR_DO
		preferences.spectrogram.timeSteps = my spectrogram.timeSteps = GET_INTEGER (L"Number of time steps");
		preferences.spectrogram.frequencySteps = my spectrogram.frequencySteps = GET_INTEGER (L"Number of frequency steps");
		preferences.spectrogram.method = my spectrogram.method = GET_ENUM (kSound_to_Spectrogram_method, L"Method");
		preferences.spectrogram.windowShape = my spectrogram.windowShape = GET_ENUM (kSound_to_Spectrogram_windowShape, L"Window shape");
		preferences.spectrogram.maximum = my spectrogram.maximum = GET_REAL (L"Maximum");
		preferences.spectrogram.autoscaling = my spectrogram.autoscaling = GET_INTEGER (L"Autoscaling");
		preferences.spectrogram.preemphasis = my spectrogram.preemphasis = GET_REAL (L"Pre-emphasis");
		preferences.spectrogram.dynamicCompression = my spectrogram.dynamicCompression = GET_REAL (L"Dynamic compression");
		forget (my spectrogram.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_getFrequency (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	Melder_informationReal (my spectrogram.cursor, L"Hz");
	return 1;
}

static int menu_cb_getSpectralPowerAtCursorCross (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my spectrogram.show)
		return Melder_error1 (L"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
	if (! my spectrogram.data) {
		TimeSoundAnalysisEditor_computeSpectrogram (me);
		if (! my spectrogram.data) return Melder_error1 (theMessage_Cannot_compute_spectrogram);
	}
	if (part != FunctionEditor_PART_CURSOR) return Melder_error1 (L"Click inside the spectrogram first.");
	MelderInfo_open ();
	MelderInfo_write1 (Melder_double (Matrix_getValueAtXY (my spectrogram.data, tmin, my spectrogram.cursor)));
	MelderInfo_write5 (L" Pa2/Hz (at time = ", Melder_double (tmin), L" seconds and frequency = ",
		Melder_double (my spectrogram.cursor), L" Hz)");
	MelderInfo_close ();
	return 1;
}

static int menu_cb_moveFrequencyCursorTo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	if (! my spectrogram.show)
		return Melder_error1 (L"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
	EDITOR_FORM (L"Move frequency cursor to", 0)
		REAL (L"Frequency (Hz)", L"0.0")
	EDITOR_OK
		SET_REAL (L"Frequency", my spectrogram.cursor)
	EDITOR_DO
		double frequency = GET_REAL (L"Frequency");
		my spectrogram.cursor = frequency;
		FunctionEditor_redraw (me);
	EDITOR_END
}

static Sound extractSound (TimeSoundAnalysisEditor me, double tmin, double tmax) {
	Sound sound = NULL;
	if (my longSound.data) {
		if (tmin < my longSound.data -> xmin) tmin = my longSound.data -> xmin;
		if (tmax > my longSound.data -> xmax) tmax = my longSound.data -> xmax;
		sound = LongSound_extractPart (my longSound.data, tmin, tmax, TRUE);
	} else if (my sound.data) {
		if (tmin < my sound.data -> xmin) tmin = my sound.data -> xmin;
		if (tmax > my sound.data -> xmax) tmax = my sound.data -> xmax;
		sound = Sound_extractPart (my sound.data, tmin, tmax, kSound_windowShape_RECTANGULAR, 1.0, TRUE);
	}
	return sound;
}

static int menu_cb_extractVisibleSpectrogram (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	Spectrogram publish;
	if (! my spectrogram.show)
		return Melder_error1 (L"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
	if (! my spectrogram.data) {
		TimeSoundAnalysisEditor_computeSpectrogram (me);
		if (! my spectrogram.data) return Melder_error1 (theMessage_Cannot_compute_spectrogram);
	}
	publish = (Spectrogram) Data_copy (my spectrogram.data);
	if (publish == NULL) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_viewSpectralSlice (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double start = my startSelection == my endSelection ?
		my spectrogram.windowShape == 5 ? my startSelection - my spectrogram.windowLength :
		my startSelection - my spectrogram.windowLength / 2 : my startSelection;
	double finish = my startSelection == my endSelection ?
		my spectrogram.windowShape == 5 ? my endSelection + my spectrogram.windowLength :
		my endSelection + my spectrogram.windowLength / 2 : my endSelection;
	Sound sound = extractSound (me, start, finish);
	Spectrum publish;
	if (sound == NULL) return 0;
	Sound_multiplyByWindow (sound,
		my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_SQUARE ? kSound_windowShape_RECTANGULAR :
		my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_HAMMING ? kSound_windowShape_HAMMING :
		my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_BARTLETT ? kSound_windowShape_TRIANGULAR :
		my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_WELCH ? kSound_windowShape_PARABOLIC :
		my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_HANNING ? kSound_windowShape_HANNING :
		my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_GAUSSIAN ? kSound_windowShape_GAUSSIAN_2 : kSound_windowShape_RECTANGULAR);
	publish = Sound_to_Spectrum (sound, TRUE);
	forget (sound);
	if (! publish) return 0;
	static MelderString sliceName = { 0 };
	MelderString_copy (& sliceName, my data == NULL ? L"untitled" : ((Data) my data) -> name);
	MelderString_appendCharacter (& sliceName, '_');
	MelderString_append (& sliceName, Melder_fixed (0.5 * (my startSelection + my endSelection), 3));
	Thing_setName (publish, sliceName.string);
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_paintVisibleSpectrogram (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Paint visible spectrogram", 0)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", our preferences.picture.spectrogram.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		our do_pictureSelection (me, cmd);
		our preferences.picture.spectrogram.garnish = GET_INTEGER (L"Garnish");
		if (! my spectrogram.show)
			return Melder_error1 (L"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrum menu.");
		if (! my spectrogram.data) {
			TimeSoundAnalysisEditor_computeSpectrogram (me);
			if (! my spectrogram.data) return Melder_error1 (theMessage_Cannot_compute_spectrogram);
		}
		Editor_openPraatPicture (me);
		Spectrogram_paint (my spectrogram.data, my pictureGraphics, my startWindow, my endWindow, my spectrogram.viewFrom, my spectrogram.viewTo,
			my spectrogram.maximum, my spectrogram.autoscaling, my spectrogram.dynamicRange, my spectrogram.preemphasis,
			my spectrogram.dynamicCompression, our preferences.picture.spectrogram.garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** PITCH MENU *****/

static int menu_cb_showPitch (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	preferences.pitch.show = my pitch.show = ! my pitch.show;
	GuiMenuItem_check (my pitchToggle, my pitch.show);   // in case we're called from a script
	FunctionEditor_redraw (me);
	return 1;
}

static int menu_cb_pitchSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Pitch settings", L"Intro 4.2. Configuring the pitch contour")
		POSITIVE (L"left Pitch range (Hz)", L"75.0")
		POSITIVE (L"right Pitch range (Hz)", L"500.0")
		OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
		LABEL (L"opt1", L"The autocorrelation method optimizes for intonation research;")
		LABEL (L"opt2", L"and the cross-correlation method optimizes for voice research:")
		RADIO_ENUM (L"Analysis method", kTimeSoundAnalysisEditor_pitch_analysisMethod, DEFAULT)
		OPTIONMENU_ENUM (L"Drawing method", kTimeSoundAnalysisEditor_pitch_drawingMethod, DEFAULT)
		LABEL (L"note1", L"")
		LABEL (L"note2", L"")
	EDITOR_OK
		SET_REAL (L"left Pitch range", my pitch.floor)
		SET_REAL (L"right Pitch range", my pitch.ceiling)
		SET_ENUM (L"Unit", kPitch_unit, my pitch.unit)
		SET_ENUM (L"Analysis method", kTimeSoundAnalysisEditor_pitch_analysisMethod, my pitch.method)
		SET_ENUM (L"Drawing method", kTimeSoundAnalysisEditor_pitch_drawingMethod, my pitch.drawingMethod)
		if (my pitch.viewFrom != 0.0 || my pitch.viewTo != 0.0 ||
			my pitch.veryAccurate != FALSE || my pitch.maximumNumberOfCandidates != 15 ||
			my pitch.silenceThreshold != 0.03 || my pitch.voicingThreshold != 0.45 || my pitch.octaveCost != 0.01 ||
			my pitch.octaveJumpCost != 0.35 || my pitch.voicedUnvoicedCost != 0.14)
		{
			SET_STRING (L"note1", L"Warning: you have some non-standard \"advanced settings\".")
		} else {
			SET_STRING (L"note1", L"(all of your \"advanced settings\" have their standard values)")
		}
		if (my timeStepStrategy != kTimeSoundAnalysisEditor_timeStepStrategy_DEFAULT) {
			SET_STRING (L"note2", L"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (L"note2", L"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		preferences.pitch.floor = my pitch.floor = GET_REAL (L"left Pitch range");
		preferences.pitch.ceiling = my pitch.ceiling = GET_REAL (L"right Pitch range");
		preferences.pitch.unit = my pitch.unit = GET_ENUM (kPitch_unit, L"Unit");
		preferences.pitch.method = my pitch.method = GET_ENUM (kTimeSoundAnalysisEditor_pitch_analysisMethod, L"Analysis method");
		preferences.pitch.drawingMethod = my pitch.drawingMethod = GET_ENUM (kTimeSoundAnalysisEditor_pitch_drawingMethod, L"Drawing method");
		forget (my pitch.data);
		forget (my intensity.data);
		forget (my pulses.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_advancedPitchSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Advanced pitch settings", L"Advanced pitch settings...")
		LABEL (L"", L"Make view range different from analysis range:")
		REAL (L"left View range (units)", L"0.0 (= auto)")
		REAL (L"right View range (units)", L"0.0 (= auto)")
		LABEL (L"", L"Analysis settings:")
		BOOLEAN (L"Very accurate", 0)
		NATURAL (L"Max. number of candidates", L"15")
		REAL (L"Silence threshold", L"0.03")
		REAL (L"Voicing threshold", L"0.45")
		REAL (L"Octave cost", L"0.01")
		REAL (L"Octave-jump cost", L"0.35")
		REAL (L"Voiced / unvoiced cost", L"0.14")
	EDITOR_OK
		SET_REAL (L"left View range", my pitch.viewFrom)
		SET_REAL (L"right View range", my pitch.viewTo)
		SET_INTEGER (L"Very accurate", my pitch.veryAccurate)
		SET_INTEGER (L"Max. number of candidates", my pitch.maximumNumberOfCandidates)
		SET_REAL (L"Silence threshold", my pitch.silenceThreshold)
		SET_REAL (L"Voicing threshold", my pitch.voicingThreshold)
		SET_REAL (L"Octave cost", my pitch.octaveCost)
		SET_REAL (L"Octave-jump cost", my pitch.octaveJumpCost)
		SET_REAL (L"Voiced / unvoiced cost", my pitch.voicedUnvoicedCost)
	EDITOR_DO
		long maxnCandidates = GET_INTEGER (L"Max. number of candidates");
		if (maxnCandidates < 2) return Melder_error1 (L"Maximum number of candidates must be greater than 1.");
		preferences.pitch.viewFrom = my pitch.viewFrom = GET_REAL (L"left View range");
		preferences.pitch.viewTo = my pitch.viewTo = GET_REAL (L"right View range");
		preferences.pitch.veryAccurate = my pitch.veryAccurate = GET_INTEGER (L"Very accurate");
		preferences.pitch.maximumNumberOfCandidates = my pitch.maximumNumberOfCandidates = GET_INTEGER (L"Max. number of candidates");
		preferences.pitch.silenceThreshold = my pitch.silenceThreshold = GET_REAL (L"Silence threshold");
		preferences.pitch.voicingThreshold = my pitch.voicingThreshold = GET_REAL (L"Voicing threshold");
		preferences.pitch.octaveCost = my pitch.octaveCost = GET_REAL (L"Octave cost");
		preferences.pitch.octaveJumpCost = my pitch.octaveJumpCost = GET_REAL (L"Octave-jump cost");
		preferences.pitch.voicedUnvoicedCost = my pitch.voicedUnvoicedCost = GET_REAL (L"Voiced / unvoiced cost");
		forget (my pitch.data);
		forget (my intensity.data);
		forget (my pulses.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_pitchListing (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	MelderInfo_open ();
	MelderInfo_writeLine2 (L"Time_s   F0_", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, Function_UNIT_TEXT_SHORT));
	if (part == FunctionEditor_PART_CURSOR) {
		double f0 = Pitch_getValueAtTime (my pitch.data, tmin, my pitch.unit, TRUE);
		f0 = ClassFunction_convertToNonlogarithmic (classPitch, f0, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		MelderInfo_writeLine3 (Melder_fixed (tmin, 6), L"   ", Melder_fixed (f0, 6));
	} else {
		long i, i1, i2;
		Sampled_getWindowSamples (my pitch.data, tmin, tmax, & i1, & i2);
		for (i = i1; i <= i2; i ++) {
			double t = Sampled_indexToX (my pitch.data, i);
			double f0 = Sampled_getValueAtSample (my pitch.data, i, Pitch_LEVEL_FREQUENCY, my pitch.unit);
			f0 = ClassFunction_convertToNonlogarithmic (classPitch, f0, Pitch_LEVEL_FREQUENCY, my pitch.unit);
			MelderInfo_writeLine3 (Melder_fixed (t, 6), L"   ", Melder_fixed (f0, 6));
		}
	}
	MelderInfo_close ();
	return 1;
}

static int menu_cb_getPitch (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	if (part == FunctionEditor_PART_CURSOR) {
		double f0 = Pitch_getValueAtTime (my pitch.data, tmin, my pitch.unit, TRUE);
		f0 = ClassFunction_convertToNonlogarithmic (classPitch, f0, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		Melder_information4 (Melder_double (f0), L" ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, 0),
			L" (interpolated pitch at CURSOR)");
	} else {
		double f0 = Pitch_getMean (my pitch.data, tmin, tmax, my pitch.unit);
		f0 = ClassFunction_convertToNonlogarithmic (classPitch, f0, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		Melder_information6 (Melder_double (f0), L" ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, 0),
			L" (mean pitch ", FunctionEditor_partString_locative (part), L")");
	}
	return 1;
}

static int menu_cb_getMinimumPitch (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax, f0;
	int part = makeQueriable (me, FALSE, & tmin, & tmax); iferror return 0;
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	f0 = Pitch_getMinimum (my pitch.data, tmin, tmax, my pitch.unit, TRUE);
	f0 = ClassFunction_convertToNonlogarithmic (classPitch, f0, Pitch_LEVEL_FREQUENCY, my pitch.unit);
	Melder_information6 (Melder_double (f0), L" ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, 0),
		L" (minimum pitch ", FunctionEditor_partString_locative (part), L")");
	return 1;
}

static int menu_cb_getMaximumPitch (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax, f0;
	int part = makeQueriable (me, FALSE, & tmin, & tmax); iferror return 0;
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	f0 = Pitch_getMaximum (my pitch.data, tmin, tmax, my pitch.unit, TRUE);
	f0 = ClassFunction_convertToNonlogarithmic (classPitch, f0, Pitch_LEVEL_FREQUENCY, my pitch.unit);
	Melder_information6 (Melder_double (f0), L" ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, 0),
		L" (maximum pitch ", FunctionEditor_partString_locative (part), L")");
	return 1;
}

static int menu_cb_moveCursorToMinimumPitch (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the View menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	if (my startSelection == my endSelection) {
		return Melder_error1 (L"Empty selection.");
	} else {
		double time;
		Pitch_getMinimumAndTime (my pitch.data, my startSelection, my endSelection,
			my pitch.unit, 1, NULL, & time);
		if (! NUMdefined (time))
			return Melder_error1 (L"Selection is voiceless.");
		my startSelection = my endSelection = time;
		FunctionEditor_marksChanged (me);
	}
	return 1;
}

static int menu_cb_moveCursorToMaximumPitch (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the View menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	if (my startSelection == my endSelection) {
		return Melder_error1 (L"Empty selection.");
	} else {
		double time;
		Pitch_getMaximumAndTime (my pitch.data, my startSelection, my endSelection,
			my pitch.unit, 1, NULL, & time);
		if (! NUMdefined (time))
			return Melder_error1 (L"Selection is voiceless.");
		my startSelection = my endSelection = time;
		FunctionEditor_marksChanged (me);
	}
	return 1;
}

static int menu_cb_extractVisiblePitchContour (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	if (! my pitch.show)
		return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
	if (! my pitch.data) {
		TimeSoundAnalysisEditor_computePitch (me);
		if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
	}
	Pitch publish = (Pitch) Data_copy (my pitch.data);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_drawVisiblePitchContour (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Draw visible pitch contour", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"Pitch:")
		BOOLEAN (L"Speckle", 0);
		my v_form_pictureMargins (cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Speckle", my pitch.picture.speckle);
		my v_ok_pictureMargins (cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", our preferences.picture.pitch.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		preferences.pitch.picture.speckle = my pitch.picture.speckle = GET_INTEGER (L"Speckle");
		my v_do_pictureMargins (cmd);
		our do_pictureSelection (me, cmd);
		our preferences.picture.pitch.garnish = GET_INTEGER (L"Garnish");
		if (! my pitch.show)
			return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my pitch.data) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my pitch.data) return Melder_error1 (theMessage_Cannot_compute_pitch);
		}
		Editor_openPraatPicture (me);
		double pitchFloor_hidden = ClassFunction_convertStandardToSpecialUnit (classPitch, my pitch.floor, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchCeiling_hidden = ClassFunction_convertStandardToSpecialUnit (classPitch, my pitch.ceiling, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchFloor_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchCeiling_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchViewFrom_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewFrom : pitchFloor_overt;
		double pitchViewTo_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewTo : pitchCeiling_overt;
		Pitch_draw (my pitch.data, my pictureGraphics, my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt,
			our preferences.picture.pitch.garnish, GET_INTEGER (L"Speckle"), my pitch.unit);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** INTENSITY MENU *****/

static int menu_cb_showIntensity (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	preferences.intensity.show = my intensity.show = ! my intensity.show;
	GuiMenuItem_check (my intensityToggle, my intensity.show);   // in case we're called from a script
	FunctionEditor_redraw (me);
	return 1;
}

static int menu_cb_intensitySettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Intensity settings", L"Intro 6.2. Configuring the intensity contour")
		REAL (L"left View range (dB)", L"50.0")
		REAL (L"right View range (dB)", L"100.0")
		RADIO_ENUM (L"Averaging method", kTimeSoundAnalysisEditor_intensity_averagingMethod, MEAN_ENERGY)
		BOOLEAN (L"Subtract mean pressure", 1)
		LABEL (L"", L"Note: the pitch floor is taken from the pitch settings.")
		LABEL (L"note2", L"")
	EDITOR_OK
		SET_REAL (L"left View range", my intensity.viewFrom)
		SET_REAL (L"right View range", my intensity.viewTo)
		SET_ENUM (L"Averaging method", kTimeSoundAnalysisEditor_intensity_averagingMethod, my intensity.averagingMethod)
		SET_INTEGER (L"Subtract mean pressure", my intensity.subtractMeanPressure)
		if (my timeStepStrategy != kTimeSoundAnalysisEditor_timeStepStrategy_DEFAULT) {
			SET_STRING (L"note2", L"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (L"note2", L"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		preferences.intensity.viewFrom = my intensity.viewFrom = GET_REAL (L"left View range");
		preferences.intensity.viewTo = my intensity.viewTo = GET_REAL (L"right View range");
		preferences.intensity.averagingMethod = my intensity.averagingMethod = GET_ENUM (kTimeSoundAnalysisEditor_intensity_averagingMethod, L"Averaging method");
		preferences.intensity.subtractMeanPressure = my intensity.subtractMeanPressure = GET_INTEGER (L"Subtract mean pressure");
		forget (my intensity.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_extractVisibleIntensityContour (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	Intensity publish;
	if (! my intensity.show)
		return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
	if (! my intensity.data) {
		TimeSoundAnalysisEditor_computeIntensity (me);
		if (! my intensity.data) return Melder_error1 (theMessage_Cannot_compute_intensity);
	}
	publish = (Intensity) Data_copy (my intensity.data);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_drawVisibleIntensityContour (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Draw visible intensity contour", 0)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", our preferences.picture.intensity.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		our do_pictureSelection (me, cmd);
		our preferences.picture.intensity.garnish = GET_INTEGER (L"Garnish");
		if (! my intensity.show)
			return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
		if (! my intensity.data) {
			TimeSoundAnalysisEditor_computeIntensity (me);
			if (! my intensity.data) return Melder_error1 (theMessage_Cannot_compute_intensity);
		}
		Editor_openPraatPicture (me);
		Intensity_draw (my intensity.data, my pictureGraphics, my startWindow, my endWindow, my intensity.viewFrom, my intensity.viewTo,
			our preferences.picture.intensity.garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static int menu_cb_intensityListing (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my intensity.show)
		return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
	if (! my intensity.data) {
		TimeSoundAnalysisEditor_computeIntensity (me);
		if (! my intensity.data) return Melder_error1 (theMessage_Cannot_compute_intensity);
	}
	MelderInfo_open ();
	MelderInfo_writeLine1 (L"Time_s   Intensity_dB");
	if (part == FunctionEditor_PART_CURSOR) {
		double intensity = Vector_getValueAtX (my intensity.data, tmin, Vector_CHANNEL_1, Vector_VALUE_INTERPOLATION_LINEAR);
		MelderInfo_writeLine3 (Melder_fixed (tmin, 6), L"   ", Melder_fixed (intensity, 6));
	} else {
		long i, i1, i2;
		Sampled_getWindowSamples (my intensity.data, tmin, tmax, & i1, & i2);
		for (i = i1; i <= i2; i ++) {
			double t = Sampled_indexToX (my intensity.data, i);
			double intensity = Vector_getValueAtX (my intensity.data, t, Vector_CHANNEL_1, Vector_VALUE_INTERPOLATION_NEAREST);
			MelderInfo_writeLine3 (Melder_fixed (t, 6), L"   ", Melder_fixed (intensity, 6));
		}
	}
	MelderInfo_close ();
	return 1;
}

static int menu_cb_getIntensity (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my intensity.show)
		return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
	if (! my intensity.data) {
		TimeSoundAnalysisEditor_computeIntensity (me);
		if (! my intensity.data) return Melder_error1 (theMessage_Cannot_compute_intensity);
	}
	if (part == FunctionEditor_PART_CURSOR) {
		Melder_information2 (Melder_double (Vector_getValueAtX (my intensity.data, tmin, Vector_CHANNEL_1, Vector_VALUE_INTERPOLATION_LINEAR)), L" dB (intensity at CURSOR)");
	} else {
		static const wchar_t *methodString [] = { L"median", L"mean-energy", L"mean-sones", L"mean-dB" };
		Melder_information6 (Melder_double (Intensity_getAverage (my intensity.data, tmin, tmax, my intensity.averagingMethod)),
			L" dB (", methodString [my intensity.averagingMethod], L" intensity ", FunctionEditor_partString_locative (part), L")");
	}
	return 1;
}

static int menu_cb_getMinimumIntensity (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, FALSE, & tmin, & tmax); iferror return 0;
	if (! my intensity.show)
		return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
	if (! my intensity.data) {
		TimeSoundAnalysisEditor_computeIntensity (me);
		if (! my intensity.data) return Melder_error1 (theMessage_Cannot_compute_intensity);
	}
	double intensity = Vector_getMinimum (my intensity.data, tmin, tmax, NUM_PEAK_INTERPOLATE_PARABOLIC);
	Melder_information4 (Melder_double (intensity), L" dB (minimum intensity ", FunctionEditor_partString_locative (part), L")");
	return 1;
}

static int menu_cb_getMaximumIntensity (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, FALSE, & tmin, & tmax); iferror return 0;
	if (! my intensity.show)
		return Melder_error1 (L"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
	if (! my intensity.data) {
		TimeSoundAnalysisEditor_computeIntensity (me);
		if (! my intensity.data) return Melder_error1 (theMessage_Cannot_compute_intensity);
	}
	double intensity = Vector_getMaximum (my intensity.data, tmin, tmax, NUM_PEAK_INTERPOLATE_PARABOLIC);
	Melder_information4 (Melder_double (intensity), L" dB (maximum intensity ", FunctionEditor_partString_locative (part), L")");
	return 1;
}

/***** FORMANT MENU *****/

static int menu_cb_showFormants (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	preferences.formant.show = my formant.show = ! my formant.show;
	GuiMenuItem_check (my formantToggle, my formant.show);   // in case we're called from a script
	FunctionEditor_redraw (me);
	return 1;
}

static int menu_cb_formantSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Formant settings", L"Intro 5.2. Configuring the formant contours")
		POSITIVE (L"Maximum formant (Hz)", L"5500.0")
		POSITIVE (L"Number of formants", L"5.0")
		POSITIVE (L"Window length (s)", L"0.025")
		REAL (L"Dynamic range (dB)", L"30.0")
		POSITIVE (L"Dot size (mm)", L"1.0")
		LABEL (L"note1", L"")
		LABEL (L"note2", L"")
	EDITOR_OK
		SET_REAL (L"Maximum formant", my formant.maximumFormant)
		SET_REAL (L"Number of formants", 0.5 * my formant.numberOfPoles)
		SET_REAL (L"Window length", my formant.windowLength)
		SET_REAL (L"Dynamic range", my formant.dynamicRange)
		SET_REAL (L"Dot size", my formant.dotSize)
		if (my formant.method != 1 || my formant.preemphasisFrom != 50.0) {
			SET_STRING (L"note1", L"Warning: you have non-standard \"advanced settings\".")
		} else {
			SET_STRING (L"note1", L"(all of your \"advanced settings\" have their standard values)")
		}
		if (my timeStepStrategy != kTimeSoundAnalysisEditor_timeStepStrategy_DEFAULT) {
			SET_STRING (L"note2", L"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (L"note2", L"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		preferences.formant.maximumFormant = my formant.maximumFormant = GET_REAL (L"Maximum formant");
		preferences.formant.numberOfPoles = my formant.numberOfPoles = 2.0 * GET_REAL (L"Number of formants");
		preferences.formant.windowLength = my formant.windowLength = GET_REAL (L"Window length");
		preferences.formant.dynamicRange = my formant.dynamicRange = GET_REAL (L"Dynamic range");
		preferences.formant.dotSize = my formant.dotSize = GET_REAL (L"Dot size");
		forget (my formant.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_advancedFormantSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Advanced formant settings", L"Advanced formant settings...")
		RADIO_ENUM (L"Method", kTimeSoundAnalysisEditor_formant_analysisMethod, BURG)
		POSITIVE (L"Pre-emphasis from (Hz)", L"50.0")
	EDITOR_OK
		SET_ENUM (L"Method", kTimeSoundAnalysisEditor_formant_analysisMethod, my formant.method)
		SET_REAL (L"Pre-emphasis from", my formant.preemphasisFrom)
	EDITOR_DO
		preferences.formant.method = my formant.method = GET_ENUM (kTimeSoundAnalysisEditor_formant_analysisMethod, L"Method");
		preferences.formant.preemphasisFrom = my formant.preemphasisFrom = GET_REAL (L"Pre-emphasis from");
		forget (my formant.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_extractVisibleFormantContour (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	Formant publish;
	if (! my formant.show)
		return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
	if (! my formant.data) {
		TimeSoundAnalysisEditor_computeFormants (me);
		if (! my formant.data) return Melder_error1 (theMessage_Cannot_compute_formant);
	}
	publish = (Formant) Data_copy (my formant.data);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_drawVisibleFormantContour (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Draw visible formant contour", 0)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", our preferences.picture.formant.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		our do_pictureSelection (me, cmd);
		our preferences.picture.formant.garnish = GET_INTEGER (L"Garnish");
		if (! my formant.show)
			return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formant\" from the Formant menu.");
		if (! my formant.data) {
			TimeSoundAnalysisEditor_computeFormants (me);
			if (! my formant.data) return Melder_error1 (theMessage_Cannot_compute_formant);
		}
		Editor_openPraatPicture (me);
		Formant_drawSpeckles (my formant.data, my pictureGraphics, my startWindow, my endWindow,
			my spectrogram.viewTo, my formant.dynamicRange,
			our preferences.picture.formant.garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static int menu_cb_formantListing (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my formant.show)
		return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
	if (! my formant.data) {
		TimeSoundAnalysisEditor_computeFormants (me);
		if (! my formant.data) return Melder_error1 (theMessage_Cannot_compute_formant);
	}
	MelderInfo_open ();
	MelderInfo_writeLine1 (L"Time_s   F1_Hz   F2_Hz   F3_Hz   F4_Hz");
	if (part == FunctionEditor_PART_CURSOR) {
		double f1 = Formant_getValueAtTime (my formant.data, 1, tmin, 0);
		double f2 = Formant_getValueAtTime (my formant.data, 2, tmin, 0);
		double f3 = Formant_getValueAtTime (my formant.data, 3, tmin, 0);
		double f4 = Formant_getValueAtTime (my formant.data, 4, tmin, 0);
		MelderInfo_write5 (Melder_fixed (tmin, 6), L"   ", Melder_fixed (f1, 6), L"   ", Melder_fixed (f2, 6));
		MelderInfo_writeLine4 (L"   ", Melder_fixed (f3, 6), L"   ", Melder_fixed (f4, 6));
	} else {
		long i, i1, i2;
		Sampled_getWindowSamples (my formant.data, tmin, tmax, & i1, & i2);
		for (i = i1; i <= i2; i ++) {
			double t = Sampled_indexToX (my formant.data, i);
			double f1 = Formant_getValueAtTime (my formant.data, 1, t, 0);
			double f2 = Formant_getValueAtTime (my formant.data, 2, t, 0);
			double f3 = Formant_getValueAtTime (my formant.data, 3, t, 0);
			double f4 = Formant_getValueAtTime (my formant.data, 4, t, 0);
			MelderInfo_write5 (Melder_fixed (t, 6), L"   ", Melder_fixed (f1, 6), L"   ", Melder_fixed (f2, 6));
			MelderInfo_writeLine4 (L"   ", Melder_fixed (f3, 6), L"   ", Melder_fixed (f4, 6));
		}
	}
	MelderInfo_close ();
	return 1;
}

static int do_getFormant (TimeSoundAnalysisEditor me, int iformant) {
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my formant.show)
		return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
	if (! my formant.data) {
		TimeSoundAnalysisEditor_computeFormants (me);
		if (! my formant.data) return Melder_error1 (theMessage_Cannot_compute_formant);
	}
	if (part == FunctionEditor_PART_CURSOR) {
		Melder_information4 (Melder_double (Formant_getValueAtTime (my formant.data, iformant, tmin, 0)),
			L" Hz (nearest F", Melder_integer (iformant), L" to CURSOR)");
	} else {
		Melder_information6 (Melder_double (Formant_getMean (my formant.data, iformant, tmin, tmax, 0)),
			L" Hz (mean F", Melder_integer (iformant), L" ", FunctionEditor_partString_locative (part), L")");
	}
	return 1;
}
static int do_getBandwidth (TimeSoundAnalysisEditor me, int iformant) {
	double tmin, tmax;
	int part = makeQueriable (me, TRUE, & tmin, & tmax); iferror return 0;
	if (! my formant.show)
		return Melder_error1 (L"No formant contour is visible.\nFirst choose \"Show formants\" from the Formant menu.");
	if (! my formant.data) {
		TimeSoundAnalysisEditor_computeFormants (me);
		if (! my formant.data) return Melder_error1 (theMessage_Cannot_compute_formant);
	}
	if (part == FunctionEditor_PART_CURSOR) {
		Melder_information4 (Melder_double (Formant_getBandwidthAtTime (my formant.data, iformant, tmin, 0)),
			L" Hz (nearest B", Melder_integer (iformant), L" to CURSOR)");
	} else {
		Melder_information6 (Melder_double (Formant_getBandwidthAtTime (my formant.data, iformant, 0.5 * (tmin + tmax), 0)),
			L" Hz (B", Melder_integer (iformant), L" in centre of ", FunctionEditor_partString (part), L")");
	}
	return 1;
}
static int menu_cb_getFirstFormant (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getFormant (me, 1); }
static int menu_cb_getFirstBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getBandwidth (me, 1); }
static int menu_cb_getSecondFormant (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getFormant (me, 2); }
static int menu_cb_getSecondBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getBandwidth (me, 2); }
static int menu_cb_getThirdFormant (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getFormant (me, 3); }
static int menu_cb_getThirdBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getBandwidth (me, 3); }
static int menu_cb_getFourthFormant (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getFormant (me, 4); }
static int menu_cb_getFourthBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor); return do_getBandwidth (me, 4); }

static int menu_cb_getFormant (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Get formant", 0)
		NATURAL (L"Formant number", L"5")
	EDITOR_OK
	EDITOR_DO
		if (! do_getFormant (me, GET_INTEGER (L"Formant number"))) return 0;
	EDITOR_END
}

static int menu_cb_getBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Get bandwidth", 0)
		NATURAL (L"Formant number", L"5")
	EDITOR_OK
	EDITOR_DO
		if (! do_getBandwidth (me, GET_INTEGER (L"Formant number"))) return 0;
	EDITOR_END
}

/***** PULSE MENU *****/

static int menu_cb_showPulses (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	preferences.pulses.show = my pulses.show = ! my pulses.show;
	GuiMenuItem_check (my pulsesToggle, my pulses.show);   // in case we're called from a script
	FunctionEditor_redraw (me);
	return 1;
}

static int menu_cb_advancedPulsesSettings (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Advanced pulses settings", L"Advanced pulses settings...")
		POSITIVE (L"Maximum period factor", L"1.3")
		POSITIVE (L"Maximum amplitude factor", L"1.6")
	EDITOR_OK
		SET_REAL (L"Maximum period factor", my pulses.maximumPeriodFactor)
		SET_REAL (L"Maximum amplitude factor", my pulses.maximumAmplitudeFactor)
	EDITOR_DO
		preferences.pulses.maximumPeriodFactor = my pulses.maximumPeriodFactor = GET_REAL (L"Maximum period factor");
		preferences.pulses.maximumAmplitudeFactor = my pulses.maximumAmplitudeFactor = GET_REAL (L"Maximum amplitude factor");
		forget (my pulses.data);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static int menu_cb_extractVisiblePulses (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	Pitch publish;
	if (! my pulses.show)
		return Melder_error1 (L"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my pulses.data) {
		TimeSoundAnalysisEditor_computePulses (me);
		if (! my pulses.data) return Melder_error1 (theMessage_Cannot_compute_pulses);
	}
	publish = (Pitch) Data_copy (my pulses.data);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_drawVisiblePulses (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	EDITOR_FORM (L"Draw visible pulses", 0)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", our preferences.picture.pulses.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		our do_pictureSelection (me, cmd);
		our preferences.picture.pulses.garnish = GET_INTEGER (L"Garnish");
		if (! my pulses.show)
			return Melder_error1 (L"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
		if (! my pulses.data) {
			TimeSoundAnalysisEditor_computePulses (me);
			if (! my pulses.data) return Melder_error1 (theMessage_Cannot_compute_pulses);
		}
		Editor_openPraatPicture (me);
		PointProcess_draw (my pulses.data, my pictureGraphics, my startWindow, my endWindow,
			our preferences.picture.pulses.garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static int menu_cb_voiceReport (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	time_t today = time (NULL);
	Sound sound = NULL;
	double tmin, tmax;
	int part = makeQueriable (me, FALSE, & tmin, & tmax); iferror return 0;
	if (! my pulses.show)
		return Melder_error1 (L"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my pulses.data) {
		TimeSoundAnalysisEditor_computePulses (me);
		if (! my pulses.data) return Melder_error1 (theMessage_Cannot_compute_pulses);
	}
	sound = extractSound (me, tmin, tmax);
	if (! sound) return Melder_error1 (L"Selection too small (or out of memory).");
	MelderInfo_open ();
	MelderInfo_writeLine4 (L"-- Voice report for ", my name, L" --\nDate: ", Melder_peekUtf8ToWcs (ctime (& today)));
	if ((my pitch.method & 2) == 0)
		MelderInfo_writeLine1 (L"WARNING: some of the following measurements may be imprecise.\n"
			"For more precision, go to \"Pitch settings\" and choose \"Optimize for voice analysis\".\n");
	MelderInfo_writeLine2 (L"Time range of ", FunctionEditor_partString (part));
	Sound_Pitch_PointProcess_voiceReport (sound, my pitch.data, my pulses.data, tmin, tmax,
		my pitch.floor, my pitch.ceiling, my pulses.maximumPeriodFactor, my pulses.maximumAmplitudeFactor, my pitch.silenceThreshold, my pitch.voicingThreshold);
	MelderInfo_close ();
	forget (sound);
	return 1;
}

static int menu_cb_pulseListing (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundAnalysisEditor);
	long i, i1, i2;
	double tmin, tmax;
	makeQueriable (me, FALSE, & tmin, & tmax); iferror return 0;
	if (! my pulses.show)
		return Melder_error1 (L"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my pulses.data) {
		TimeSoundAnalysisEditor_computePulses (me);
		if (! my pulses.data) return Melder_error1 (theMessage_Cannot_compute_pulses);
	}
	MelderInfo_open ();
	MelderInfo_writeLine1 (L"Time_s");
	i1 = PointProcess_getHighIndex (my pulses.data, tmin);
	i2 = PointProcess_getLowIndex (my pulses.data, tmax);
	for (i = i1; i <= i2; i ++) {
		double t = my pulses.data -> t [i];
		MelderInfo_writeLine1 (Melder_fixed (t, 12));
	}
	MelderInfo_close ();
	return 1;
}

/*
static int cb_getJitter_xx (TimeSoundAnalysisEditor me, double (*PointProcess_getJitter_xx) (PointProcess, double, double, double, double, double)) {
	double minimumPeriod = 0.8 / my pitch.ceiling, maximumPeriod = 1.25 / my pitch.floor;
	if (! my pulses.show)
		return Melder_error1 (L"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my pulses.data) {
		computePulses (me);
		if (! my pulses.data) return Melder_error1 (theMessage_Cannot_compute_pulses);
	}
	if (my startSelection == my endSelection)
		return Melder_error1 (L"Make a selection first.");
	if (! queriable (me)) return 0;
	Melder_informationReal (PointProcess_getJitter_xx (my pulses.data, my startSelection, my endSelection,
		minimumPeriod, maximumPeriod, my pulses.maximumPeriodFactor), NULL);
	return 1;
}
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_local) if (! cb_getJitter_xx (me, PointProcess_getJitter_local)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_local_absolute) if (! cb_getJitter_xx (me, PointProcess_getJitter_local_absolute)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_rap) if (! cb_getJitter_xx (me, PointProcess_getJitter_rap)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_ppq5) if (! cb_getJitter_xx (me, PointProcess_getJitter_ppq5)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getJitter_ddp) if (! cb_getJitter_xx (me, PointProcess_getJitter_ddp)) return 0; END

static int cb_getShimmer_xx (TimeSoundAnalysisEditor me, double (*PointProcess_Sound_getShimmer_xx) (PointProcess, Sound, double, double, double, double, double)) {
	Sound sound = NULL;
	double minimumPeriod = 0.8 / my pitch.ceiling, maximumPeriod = 1.25 / my pitch.floor;
	if (! my pulses.show)
		return Melder_error1 (L"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	if (! my pulses.data) {
		computePulses (me);
		if (! my pulses.data) return Melder_error1 (theMessage_Cannot_compute_pulses);
	}
	if (my startSelection == my endSelection)
		return Melder_error1 (L"Make a selection first.");
	if (! queriable (me)) return 0;
	sound = extractSound (me, my startSelection, my endSelection);
	if (! sound) return Melder_error1 (L"Selection too small (or out of memory).");
	Melder_informationReal (PointProcess_Sound_getShimmer_xx (my pulses.data, sound, my startSelection, my endSelection,
		minimumPeriod, maximumPeriod, my pulses.maximumAmplitudeFactor), NULL);
	forget (sound);
	return 1;
}
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_local) if (! cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_local)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_local_dB) if (! cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_local_dB)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_apq3) if (! cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq3)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_apq5) if (! cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq5)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_apq11) if (! cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq11)) return 0; END
DIRECT (TimeSoundAnalysisEditor, cb_getShimmer_dda) if (! cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_dda)) return 0; END
*/

static void createMenuItems_view_sound (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	inherited (TimeSoundAnalysisEditor) createMenuItems_view_sound (me, menu);
	our createMenuItems_view_sound_analysis (me, menu);
}

static void createMenuItems_view_sound_analysis (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"Analysis window:", GuiMenu_INSENSITIVE, menu_cb_showAnalyses);
	EditorMenu_addCommand (menu, L"Show analyses...", 0, menu_cb_showAnalyses);
	EditorMenu_addCommand (menu, L"Time step settings...", 0, menu_cb_timeStepSettings);
	EditorMenu_addCommand (menu, L"-- sound analysis --", 0, 0);
}

void structTimeSoundAnalysisEditor :: v_createMenuItems_query (EditorMenu menu) {
	TimeSoundAnalysisEditor_Parent :: v_createMenuItems_query (menu);
	if (sound.data || longSound.data) {
		its createMenuItems_query_log (this, menu);
	}
}

static void createMenuItems_query_log (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"-- query log --", 0, NULL);
	EditorMenu_addCommand (menu, L"Log settings...", 0, menu_cb_logSettings);
	EditorMenu_addCommand (menu, L"Delete log file 1", 0, menu_cb_deleteLogFile1);
	EditorMenu_addCommand (menu, L"Delete log file 2", 0, menu_cb_deleteLogFile2);
	EditorMenu_addCommand (menu, L"Log 1", GuiMenu_F12, menu_cb_log1);
	EditorMenu_addCommand (menu, L"Log 2", GuiMenu_F12 + GuiMenu_SHIFT, menu_cb_log2);
	EditorMenu_addCommand (menu, L"Log script 3 (...)", GuiMenu_F12 + GuiMenu_OPTION, menu_cb_logScript3);
	EditorMenu_addCommand (menu, L"Log script 4 (...)", GuiMenu_F12 + GuiMenu_COMMAND, menu_cb_logScript4);
}

static void createMenus_analysis (TimeSoundAnalysisEditor me) {
	EditorMenu menu;

	menu = Editor_addMenu (me, L"Spectrum", 0);
	my spectrogramToggle = EditorMenu_addCommand (menu, L"Show spectrogram",
		GuiMenu_CHECKBUTTON | (preferences.spectrogram.show ? GuiMenu_TOGGLE_ON : 0), menu_cb_showSpectrogram);
	EditorMenu_addCommand (menu, L"Spectrogram settings...", 0, menu_cb_spectrogramSettings);
	EditorMenu_addCommand (menu, L"Advanced spectrogram settings...", 0, menu_cb_advancedSpectrogramSettings);
	EditorMenu_addCommand (menu, L"-- spectrum query --", 0, NULL);
	EditorMenu_addCommand (menu, L"Query:", GuiMenu_INSENSITIVE, menu_cb_getFrequency /* dummy */);
	EditorMenu_addCommand (menu, L"Get frequency at frequency cursor", 0, menu_cb_getFrequency);
	EditorMenu_addCommand (menu, L"Get spectral power at cursor cross", GuiMenu_F7, menu_cb_getSpectralPowerAtCursorCross);
	EditorMenu_addCommand (menu, L"-- spectrum select --", 0, NULL);
	EditorMenu_addCommand (menu, L"Select:", GuiMenu_INSENSITIVE, menu_cb_moveFrequencyCursorTo/* dummy */);
	EditorMenu_addCommand (menu, L"Move frequency cursor to...", 0, menu_cb_moveFrequencyCursorTo);
	our createMenuItems_spectrum_picture (me, menu);
	EditorMenu_addCommand (menu, L"-- spectrum extract --", 0, NULL);
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_extractVisibleSpectrogram /* dummy */);
	EditorMenu_addCommand (menu, L"Extract visible spectrogram", 0, menu_cb_extractVisibleSpectrogram);
	EditorMenu_addCommand (menu, L"View spectral slice", 'L', menu_cb_viewSpectralSlice);

	menu = Editor_addMenu (me, L"Pitch", 0);
	my pitchToggle = EditorMenu_addCommand (menu, L"Show pitch",
		GuiMenu_CHECKBUTTON | (preferences.pitch.show ? GuiMenu_TOGGLE_ON : 0), menu_cb_showPitch);
	EditorMenu_addCommand (menu, L"Pitch settings...", 0, menu_cb_pitchSettings);
	EditorMenu_addCommand (menu, L"Advanced pitch settings...", 0, menu_cb_advancedPitchSettings);
	EditorMenu_addCommand (menu, L"-- pitch query --", 0, NULL);
	EditorMenu_addCommand (menu, L"Query:", GuiMenu_INSENSITIVE, menu_cb_getFrequency /* dummy */);
	EditorMenu_addCommand (menu, L"Pitch listing", 0, menu_cb_pitchListing);
	EditorMenu_addCommand (menu, L"Get pitch", GuiMenu_F5, menu_cb_getPitch);
	EditorMenu_addCommand (menu, L"Get minimum pitch", GuiMenu_F5 + GuiMenu_COMMAND, menu_cb_getMinimumPitch);
	EditorMenu_addCommand (menu, L"Get maximum pitch", GuiMenu_F5 + GuiMenu_SHIFT, menu_cb_getMaximumPitch);
	EditorMenu_addCommand (menu, L"-- pitch select --", 0, NULL);
	EditorMenu_addCommand (menu, L"Select:", GuiMenu_INSENSITIVE, menu_cb_moveCursorToMinimumPitch /* dummy */);
	EditorMenu_addCommand (menu, L"Move cursor to minimum pitch", GuiMenu_COMMAND + GuiMenu_SHIFT + 'L', menu_cb_moveCursorToMinimumPitch);
	EditorMenu_addCommand (menu, L"Move cursor to maximum pitch", GuiMenu_COMMAND + GuiMenu_SHIFT + 'H', menu_cb_moveCursorToMaximumPitch);
	our createMenuItems_pitch_picture (me, menu);
	EditorMenu_addCommand (menu, L"-- pitch extract --", 0, NULL);
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_extractVisiblePitchContour /* dummy */);
	EditorMenu_addCommand (menu, L"Extract visible pitch contour", 0, menu_cb_extractVisiblePitchContour);

	menu = Editor_addMenu (me, L"Intensity", 0);
	my intensityToggle = EditorMenu_addCommand (menu, L"Show intensity",
		GuiMenu_CHECKBUTTON | (preferences.intensity.show ? GuiMenu_TOGGLE_ON : 0), menu_cb_showIntensity);
	EditorMenu_addCommand (menu, L"Intensity settings...", 0, menu_cb_intensitySettings);
	EditorMenu_addCommand (menu, L"-- intensity query --", 0, NULL);
	EditorMenu_addCommand (menu, L"Query:", GuiMenu_INSENSITIVE, menu_cb_getFrequency /* dummy */);
	EditorMenu_addCommand (menu, L"Intensity listing", 0, menu_cb_intensityListing);
	EditorMenu_addCommand (menu, L"Get intensity", GuiMenu_F8, menu_cb_getIntensity);
	EditorMenu_addCommand (menu, L"Get minimum intensity", GuiMenu_F8 + GuiMenu_COMMAND, menu_cb_getMinimumIntensity);
	EditorMenu_addCommand (menu, L"Get maximum intensity", GuiMenu_F8 + GuiMenu_SHIFT, menu_cb_getMaximumIntensity);
	our createMenuItems_intensity_picture (me, menu);
	EditorMenu_addCommand (menu, L"-- intensity extract --", 0, NULL);
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_extractVisibleIntensityContour /* dummy */);
	EditorMenu_addCommand (menu, L"Extract visible intensity contour", 0, menu_cb_extractVisibleIntensityContour);

	menu = Editor_addMenu (me, L"Formant", 0);
	my formantToggle = EditorMenu_addCommand (menu, L"Show formants",
		GuiMenu_CHECKBUTTON | (preferences.formant.show ? GuiMenu_TOGGLE_ON : 0), menu_cb_showFormants);
	EditorMenu_addCommand (menu, L"Formant settings...", 0, menu_cb_formantSettings);
	EditorMenu_addCommand (menu, L"Advanced formant settings...", 0, menu_cb_advancedFormantSettings);
	EditorMenu_addCommand (menu, L"-- formant query --", 0, NULL);
	EditorMenu_addCommand (menu, L"Query:", GuiMenu_INSENSITIVE, menu_cb_getFrequency /* dummy */);
	EditorMenu_addCommand (menu, L"Formant listing", 0, menu_cb_formantListing);
	EditorMenu_addCommand (menu, L"Get first formant", GuiMenu_F1, menu_cb_getFirstFormant);
	EditorMenu_addCommand (menu, L"Get first bandwidth", 0, menu_cb_getFirstBandwidth);
	EditorMenu_addCommand (menu, L"Get second formant", GuiMenu_F2, menu_cb_getSecondFormant);
	EditorMenu_addCommand (menu, L"Get second bandwidth", 0, menu_cb_getSecondBandwidth);
	EditorMenu_addCommand (menu, L"Get third formant", GuiMenu_F3, menu_cb_getThirdFormant);
	EditorMenu_addCommand (menu, L"Get third bandwidth", 0, menu_cb_getThirdBandwidth);
	EditorMenu_addCommand (menu, L"Get fourth formant", GuiMenu_F4, menu_cb_getFourthFormant);
	EditorMenu_addCommand (menu, L"Get fourth bandwidth", 0, menu_cb_getFourthBandwidth);
	EditorMenu_addCommand (menu, L"Get formant...", 0, menu_cb_getFormant);
	EditorMenu_addCommand (menu, L"Get bandwidth...", 0, menu_cb_getBandwidth);
	our createMenuItems_formant_picture (me, menu);
	EditorMenu_addCommand (menu, L"-- formant extract --", 0, NULL);
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_extractVisibleFormantContour /* dummy */);
	EditorMenu_addCommand (menu, L"Extract visible formant contour", 0, menu_cb_extractVisibleFormantContour);

	menu = Editor_addMenu (me, L"Pulses", 0);
	my pulsesToggle = EditorMenu_addCommand (menu, L"Show pulses",
		GuiMenu_CHECKBUTTON | (preferences.pulses.show ? GuiMenu_TOGGLE_ON : 0), menu_cb_showPulses);
	EditorMenu_addCommand (menu, L"Advanced pulses settings...", 0, menu_cb_advancedPulsesSettings);
	EditorMenu_addCommand (menu, L"-- pulses query --", 0, NULL);
	EditorMenu_addCommand (menu, L"Query:", GuiMenu_INSENSITIVE, menu_cb_getFrequency /* dummy */);
	EditorMenu_addCommand (menu, L"Voice report", 0, menu_cb_voiceReport);
	EditorMenu_addCommand (menu, L"Pulse listing", 0, menu_cb_pulseListing);
	/*
	EditorMenu_addCommand (menu, L"Get jitter (local)", 0, cb_getJitter_local);
	EditorMenu_addCommand (menu, L"Get jitter (local, absolute)", 0, cb_getJitter_local_absolute);
	EditorMenu_addCommand (menu, L"Get jitter (rap)", 0, cb_getJitter_rap);
	EditorMenu_addCommand (menu, L"Get jitter (ppq5)", 0, cb_getJitter_ppq5);
	EditorMenu_addCommand (menu, L"Get jitter (ddp)", 0, cb_getJitter_ddp);
	EditorMenu_addCommand (menu, L"Get shimmer (local)", 0, cb_getShimmer_local);
	EditorMenu_addCommand (menu, L"Get shimmer (local_dB)", 0, cb_getShimmer_local_dB);
	EditorMenu_addCommand (menu, L"Get shimmer (apq3)", 0, cb_getShimmer_apq3);
	EditorMenu_addCommand (menu, L"Get shimmer (apq5)", 0, cb_getShimmer_apq5);
	EditorMenu_addCommand (menu, L"Get shimmer (apq11)", 0, cb_getShimmer_apq11);
	EditorMenu_addCommand (menu, L"Get shimmer (dda)", 0, cb_getShimmer_dda);
	*/
	our createMenuItems_pulses_picture (me, menu);
	EditorMenu_addCommand (menu, L"-- pulses extract --", 0, NULL);
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_extractVisiblePulses /* dummy */);
	EditorMenu_addCommand (menu, L"Extract visible pulses", 0, menu_cb_extractVisiblePulses);
}

static void createMenuItems_spectrum_picture (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"-- spectrum draw --", 0, NULL);
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_paintVisibleSpectrogram /* dummy */);
	EditorMenu_addCommand (menu, L"Paint visible spectrogram...", 0, menu_cb_paintVisibleSpectrogram);
}

static void createMenuItems_pitch_picture (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"-- pitch draw --", 0, NULL);
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisiblePitchContour /* dummy */);
	EditorMenu_addCommand (menu, L"Draw visible pitch contour...", 0, menu_cb_drawVisiblePitchContour);
}

static void createMenuItems_intensity_picture (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"-- intensity draw --", 0, NULL);
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisibleIntensityContour /* dummy */);
	EditorMenu_addCommand (menu, L"Draw visible intensity contour...", 0, menu_cb_drawVisibleIntensityContour);
}

static void createMenuItems_formant_picture (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"-- formant draw --", 0, NULL);
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisibleFormantContour /* dummy */);
	EditorMenu_addCommand (menu, L"Draw visible formant contour...", 0, menu_cb_drawVisibleFormantContour);
}

static void createMenuItems_pulses_picture (TimeSoundAnalysisEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"-- pulses draw --", 0, NULL);
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_drawVisiblePulses /* dummy */);
	EditorMenu_addCommand (menu, L"Draw visible pulses...", 0, menu_cb_drawVisiblePulses);
}

void TimeSoundAnalysisEditor_computeSpectrogram (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my spectrogram.show && my endWindow - my startWindow <= my longestAnalysis &&
		(my spectrogram.data == NULL || my spectrogram.data -> xmin != my startWindow || my spectrogram.data -> xmax != my endWindow))
	{
		double margin = my spectrogram.windowShape == kSound_to_Spectrogram_windowShape_GAUSSIAN ? my spectrogram.windowLength : 0.5 * my spectrogram.windowLength;
		forget (my spectrogram.data);
		try {
			autoSound sound = extractSound (me, my startWindow - margin, my endWindow + margin);
			my spectrogram.data = Sound_to_Spectrogram (sound.peek(), my spectrogram.windowLength,
				my spectrogram.viewTo, (my endWindow - my startWindow) / my spectrogram.timeSteps,
				my spectrogram.viewTo / my spectrogram.frequencySteps, my spectrogram.windowShape, 8.0, 8.0);
			my spectrogram.data -> xmin = my startWindow;
			my spectrogram.data -> xmax = my endWindow;
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

static void computePitch_inside (TimeSoundAnalysisEditor me) {
	double margin = my pitch.veryAccurate ? 3.0 / my pitch.floor : 1.5 / my pitch.floor;
	forget (my pitch.data);
	try {
		autoSound sound = extractSound (me, my startWindow - margin, my endWindow + margin);
		double pitchTimeStep =
			my timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy_FIXED ? my fixedTimeStep :
			my timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy_VIEW_DEPENDENT ? (my endWindow - my startWindow) / my numberOfTimeStepsPerView :
			0.0;   // the default: determined by pitch floor
		my pitch.data = Sound_to_Pitch_any (sound.peek(), pitchTimeStep,
			my pitch.floor,
			my pitch.method == kTimeSoundAnalysisEditor_pitch_analysisMethod_AUTOCORRELATION ? 3.0 : 1.0,
			my pitch.maximumNumberOfCandidates,
			(my pitch.method - 1) * 2 + my pitch.veryAccurate,
			my pitch.silenceThreshold, my pitch.voicingThreshold,
			my pitch.octaveCost, my pitch.octaveJumpCost, my pitch.voicedUnvoicedCost, my pitch.ceiling);
		my pitch.data -> xmin = my startWindow;
		my pitch.data -> xmax = my endWindow;
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void TimeSoundAnalysisEditor_computePitch (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my pitch.show && my endWindow - my startWindow <= my longestAnalysis &&
		(my pitch.data == NULL || my pitch.data -> xmin != my startWindow || my pitch.data -> xmax != my endWindow))
	{
		computePitch_inside (me);
	}
}

void TimeSoundAnalysisEditor_computeIntensity (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my intensity.show && my endWindow - my startWindow <= my longestAnalysis &&
		(my intensity.data == NULL || my intensity.data -> xmin != my startWindow || my intensity.data -> xmax != my endWindow))
	{
		double margin = 3.2 / my pitch.floor;
		forget (my intensity.data);
		try {
			autoSound sound = extractSound (me, my startWindow - margin, my endWindow + margin);
			my intensity.data = Sound_to_Intensity (sound.peek(), my pitch.floor,
				my endWindow - my startWindow > my longestAnalysis ? (my endWindow - my startWindow) / 100 : 0.0,
				my intensity.subtractMeanPressure);
			my intensity.data -> xmin = my startWindow;
			my intensity.data -> xmax = my endWindow;
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void TimeSoundAnalysisEditor_computeFormants (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my formant.show && my endWindow - my startWindow <= my longestAnalysis &&
		(my formant.data == NULL || my formant.data -> xmin != my startWindow || my formant.data -> xmax != my endWindow))
	{
		double margin = my formant.windowLength;
		forget (my formant.data);
		try {
			autoSound sound =
				my endWindow - my startWindow > my longestAnalysis ?
					extractSound (me,
						0.5 * (my startWindow + my endWindow - my longestAnalysis) - margin,
						0.5 * (my startWindow + my endWindow + my longestAnalysis) + margin) :
					extractSound (me, my startWindow - margin, my endWindow + margin);
			double formantTimeStep =
				my timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy_FIXED ? my fixedTimeStep :
				my timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy_VIEW_DEPENDENT ? (my endWindow - my startWindow) / my numberOfTimeStepsPerView :
				0.0;   // the default: determined by analysis window length
			my formant.data = Sound_to_Formant_any (sound.peek(), formantTimeStep,
				my formant.numberOfPoles, my formant.maximumFormant,
				my formant.windowLength, my formant.method, my formant.preemphasisFrom, 50.0);
			my formant.data -> xmin = my startWindow;
			my formant.data -> xmax = my endWindow;
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void TimeSoundAnalysisEditor_computePulses (TimeSoundAnalysisEditor me) {
	autoMelderProgressOff progress;
	if (my pulses.show && my endWindow - my startWindow <= my longestAnalysis &&
		(my pulses.data == NULL || my pulses.data -> xmin != my startWindow || my pulses.data -> xmax != my endWindow))
	{
		forget (my pulses.data);
		if (my pitch.data == NULL || my pitch.data -> xmin != my startWindow || my pitch.data -> xmax != my endWindow) {
			computePitch_inside (me);
		}
		if (my pitch.data != NULL) {
			try {
				autoSound sound = extractSound (me, my startWindow, my endWindow);
				my pulses.data = Sound_Pitch_to_PointProcess_cc (sound.peek(), my pitch.data);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	}
}

static void draw_analysis (TimeSoundAnalysisEditor me) {
	double pitchFloor_hidden = ClassFunction_convertStandardToSpecialUnit (classPitch, my pitch.floor, Pitch_LEVEL_FREQUENCY, my pitch.unit);
	double pitchCeiling_hidden = ClassFunction_convertStandardToSpecialUnit (classPitch, my pitch.ceiling, Pitch_LEVEL_FREQUENCY, my pitch.unit);
	double pitchFloor_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
	double pitchCeiling_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
	double pitchViewFrom_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewFrom : pitchFloor_overt;
	double pitchViewTo_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewTo : pitchCeiling_overt;
	double pitchViewFrom_hidden = ClassFunction_isUnitLogarithmic (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit) ? log10 (pitchViewFrom_overt) : pitchViewFrom_overt;
	double pitchViewTo_hidden = ClassFunction_isUnitLogarithmic (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit) ? log10 (pitchViewTo_overt) : pitchViewTo_overt;

	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0.0, 1.0, 0.0, 1.0);

	if (my endWindow - my startWindow > my longestAnalysis) {
		Graphics_setFont (my graphics, kGraphics_font_HELVETICA);
		Graphics_setFontSize (my graphics, 10);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text3 (my graphics, 0.5, 0.67, L"(To see the analyses, zoom in to at most ", Melder_half (my longestAnalysis), L" seconds,");
		Graphics_text (my graphics, 0.5, 0.33, L"or raise the \"longest analysis\" setting with \"Show analyses\" in the View menu.)");
		Graphics_setFontSize (my graphics, 12);
		return;
	}
	TimeSoundAnalysisEditor_computeSpectrogram (me);
	if (my spectrogram.show && my spectrogram.data != NULL) {
		Spectrogram_paintInside (my spectrogram.data, my graphics, my startWindow, my endWindow, 
			my spectrogram.viewFrom, my spectrogram.viewTo, my spectrogram.maximum, my spectrogram.autoscaling,
			my spectrogram.dynamicRange, my spectrogram.preemphasis, my spectrogram.dynamicCompression);
	}
	TimeSoundAnalysisEditor_computePitch (me);
	if (my pitch.show && my pitch.data != NULL) {
		double periodsPerAnalysisWindow = my pitch.method == kTimeSoundAnalysisEditor_pitch_analysisMethod_AUTOCORRELATION ? 3.0 : 1.0;
		double greatestNonUndersamplingTimeStep = 0.5 * periodsPerAnalysisWindow / my pitch.floor;
		double defaultTimeStep = 0.5 * greatestNonUndersamplingTimeStep;
		double timeStep =
			my timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy_FIXED ? my fixedTimeStep :
			my timeStepStrategy == kTimeSoundAnalysisEditor_timeStepStrategy_VIEW_DEPENDENT ? (my endWindow - my startWindow) / my numberOfTimeStepsPerView :
			defaultTimeStep;
		int undersampled = timeStep > greatestNonUndersamplingTimeStep;
		long numberOfVisiblePitchPoints = (long) ((my endWindow - my startWindow) / timeStep);
		Graphics_setColour (my graphics, Graphics_CYAN);
		Graphics_setLineWidth (my graphics, 3.0);
		if ((my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_AUTOMATIC && (undersampled || numberOfVisiblePitchPoints < 101)) ||
		    my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_SPECKLE)
		{
			Pitch_drawInside (my pitch.data, my graphics, my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, 2, my pitch.unit);
		}
		if ((my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_AUTOMATIC && ! undersampled) ||
		    my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_CURVE)
		{
			Pitch_drawInside (my pitch.data, my graphics, my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, FALSE, my pitch.unit);
		}
		Graphics_setColour (my graphics, Graphics_BLUE);
		Graphics_setLineWidth (my graphics, 1.0);
		if ((my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_AUTOMATIC && (undersampled || numberOfVisiblePitchPoints < 101)) ||
		    my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_SPECKLE)
		{
			Pitch_drawInside (my pitch.data, my graphics, my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, 1, my pitch.unit);
		}
		if ((my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_AUTOMATIC && ! undersampled) ||
		    my pitch.drawingMethod == kTimeSoundAnalysisEditor_pitch_drawingMethod_CURVE)
		{
			Pitch_drawInside (my pitch.data, my graphics, my startWindow, my endWindow, pitchViewFrom_overt, pitchViewTo_overt, FALSE, my pitch.unit);
		}
		Graphics_setColour (my graphics, Graphics_BLACK);
	}
	TimeSoundAnalysisEditor_computeIntensity (me);
	if (my intensity.show && my intensity.data != NULL) {
		Graphics_setColour (my graphics, my spectrogram.show ? Graphics_YELLOW : Graphics_LIME);
		Graphics_setLineWidth (my graphics, my spectrogram.show ? 1.0 : 3.0);
		Intensity_drawInside (my intensity.data, my graphics, my startWindow, my endWindow,
			my intensity.viewFrom, my intensity.viewTo);
		Graphics_setLineWidth (my graphics, 1.0);
		Graphics_setColour (my graphics, Graphics_BLACK);
	}
	TimeSoundAnalysisEditor_computeFormants (me);
	if (my formant.show && my formant.data != NULL) {
		Graphics_setColour (my graphics, Graphics_RED);
		Formant_drawSpeckles_inside (my formant.data, my graphics, my startWindow, my endWindow, 
			my spectrogram.viewFrom, my spectrogram.viewTo, my formant.dynamicRange, my formant.dotSize);
		Graphics_setColour (my graphics, Graphics_BLACK);
	}
	/*
	 * Draw vertical scales.
	 */
	if (my pitch.show) {
		double pitchCursor_overt = NUMundefined, pitchCursor_hidden = NUMundefined;
		Graphics_setWindow (my graphics, my startWindow, my endWindow, pitchViewFrom_hidden, pitchViewTo_hidden);
		Graphics_setColour (my graphics, Graphics_BLUE);
		if (my pitch.data) {
			if (my startSelection == my endSelection)
				pitchCursor_hidden = Pitch_getValueAtTime (my pitch.data, my startSelection, my pitch.unit, 1);
			else
				pitchCursor_hidden = Pitch_getMean (my pitch.data, my startSelection, my endSelection, my pitch.unit);
			pitchCursor_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchCursor_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
			if (NUMdefined (pitchCursor_hidden)) {
				Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
				Graphics_text3 (my graphics, my endWindow, pitchCursor_hidden, Melder_float (Melder_half (pitchCursor_overt)), L" ",
					ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit,
						Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL));
			}
			if (! NUMdefined (pitchCursor_hidden) || Graphics_dyWCtoMM (my graphics, pitchCursor_hidden - pitchViewFrom_hidden) > 5.0) {
				Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
				Graphics_text3 (my graphics, my endWindow, pitchViewFrom_hidden - Graphics_dyMMtoWC (my graphics, 0.5),
					Melder_float (Melder_half (pitchViewFrom_overt)), L" ",
					ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit,
						Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL));
			}
			if (! NUMdefined (pitchCursor_hidden) || Graphics_dyWCtoMM (my graphics, pitchViewTo_hidden - pitchCursor_hidden) > 5.0) {
				Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
				Graphics_text3 (my graphics, my endWindow, pitchViewTo_hidden, Melder_float (Melder_half (pitchViewTo_overt)), L" ",
					ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit,
						Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL));
			}
		} else {
			Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
			Graphics_setFontSize (my graphics, 10);
			Graphics_text (my graphics, 0.5 * (my startWindow + my endWindow), 0.5 * (pitchViewFrom_hidden + pitchViewTo_hidden),
				L"(Cannot show pitch contour. Zoom out or change bottom of pitch range in pitch settings.)");
			Graphics_setFontSize (my graphics, 12);
		}
		Graphics_setColour (my graphics, Graphics_BLACK);
	}
	if (my intensity.show) {
		double intensityCursor = NUMundefined;
		int intensityCursorVisible;
		Graphics_Colour textColour;
		int alignment;
		double y;
		if (! my pitch.show) textColour = Graphics_GREEN, alignment = Graphics_LEFT, y = my endWindow;
		else if (! my spectrogram.show && ! my formant.show) textColour = Graphics_GREEN, alignment = Graphics_RIGHT, y = my startWindow;
		else textColour = my spectrogram.show ? Graphics_LIME : Graphics_GREEN, alignment = Graphics_RIGHT, y = my endWindow;
		Graphics_setWindow (my graphics, my startWindow, my endWindow, my intensity.viewFrom, my intensity.viewTo);
		if (my intensity.data) {
			if (my startSelection == my endSelection) {
				intensityCursor = Vector_getValueAtX (my intensity.data, my startSelection, Vector_CHANNEL_1, Vector_VALUE_INTERPOLATION_LINEAR);
			} else {
				intensityCursor = Intensity_getAverage (my intensity.data, my startSelection, my endSelection, my intensity.averagingMethod);
			}
		}
		Graphics_setColour (my graphics, textColour);
		intensityCursorVisible = NUMdefined (intensityCursor) && intensityCursor > my intensity.viewFrom && intensityCursor < my intensity.viewTo;
		if (intensityCursorVisible) {
			static const wchar_t *methodString [] = { L" (.5)", L" (\\muE)", L" (\\muS)", L" (\\mu)" };
			Graphics_setTextAlignment (my graphics, alignment, Graphics_HALF);
			Graphics_text3 (my graphics, y, intensityCursor, Melder_float (Melder_half (intensityCursor)), L" dB",
				my startSelection == my endSelection ? L"" : methodString [my intensity.averagingMethod]);
		}
		if (! intensityCursorVisible || Graphics_dyWCtoMM (my graphics, intensityCursor - my intensity.viewFrom) > 5.0) {
			Graphics_setTextAlignment (my graphics, alignment, Graphics_BOTTOM);
			Graphics_text2 (my graphics, y, my intensity.viewFrom - Graphics_dyMMtoWC (my graphics, 0.5),
				Melder_float (Melder_half (my intensity.viewFrom)), L" dB");
		}
		if (! intensityCursorVisible || Graphics_dyWCtoMM (my graphics, my intensity.viewTo - intensityCursor) > 5.0) {
			Graphics_setTextAlignment (my graphics, alignment, Graphics_TOP);
			Graphics_text2 (my graphics, y, my intensity.viewTo, Melder_float (Melder_half (my intensity.viewTo)), L" dB");
		}
		Graphics_setColour (my graphics, Graphics_BLACK);
	}
	if (my spectrogram.show || my formant.show) {
		static MelderString text = { 0 };
		int frequencyCursorVisible = my spectrogram.cursor > my spectrogram.viewFrom && my spectrogram.cursor < my spectrogram.viewTo;
		Graphics_setWindow (my graphics, my startWindow, my endWindow, my spectrogram.viewFrom, my spectrogram.viewTo);
		/*
		 * Range marks.
		 */
		Graphics_setLineType (my graphics, Graphics_DRAWN);
		Graphics_setColour (my graphics, Graphics_BLACK);
		if (! frequencyCursorVisible || Graphics_dyWCtoMM (my graphics, my spectrogram.cursor - my spectrogram.viewFrom) > 5.0) {
			MelderString_empty (& text);
			MelderString_append2 (& text, Melder_half (my spectrogram.viewFrom), L" Hz");
			Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_BOTTOM);
			Graphics_text (my graphics, my startWindow, my spectrogram.viewFrom - Graphics_dyMMtoWC (my graphics, 0.5), Melder_float (text.string));
		}
		if (! frequencyCursorVisible || Graphics_dyWCtoMM (my graphics, my spectrogram.viewTo - my spectrogram.cursor) > 5.0) {
			MelderString_empty (& text);
			MelderString_append2 (& text, Melder_half (my spectrogram.viewTo), L" Hz");
			Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
			Graphics_text (my graphics, my startWindow, my spectrogram.viewTo, Melder_float (text.string));
		}
		/*
		 * Cursor lines.
		 */
		Graphics_setLineType (my graphics, Graphics_DOTTED);
		Graphics_setColour (my graphics, Graphics_RED);
		if (frequencyCursorVisible) {
			double x = my startWindow, y = my spectrogram.cursor;
			Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
			Graphics_text2 (my graphics, x, y, Melder_float (Melder_half (y)), L" Hz");
			Graphics_line (my graphics, x, y, my endWindow, y);
		}
		/*
		if (my startSelection >= my startWindow && my startSelection <= my endWindow)
			Graphics_line (my graphics, my startSelection, my spectrogram.viewFrom, my startSelection, my spectrogram.viewTo);
		if (my endSelection > my startWindow && my endSelection < my endWindow && my endSelection != my startSelection)
			Graphics_line (my graphics, my endSelection, my spectrogram.viewFrom, my endSelection, my spectrogram.viewTo);*/
		/*
		 * Cadre.
		 */
		Graphics_setLineType (my graphics, Graphics_DRAWN);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_rectangle (my graphics, my startWindow, my endWindow, my spectrogram.viewFrom, my spectrogram.viewTo);
	}
}

static void draw_analysis_pulses (TimeSoundAnalysisEditor me) {
	TimeSoundAnalysisEditor_computePulses (me);
	if (my pulses.show && my endWindow - my startWindow <= my longestAnalysis && my pulses.data != NULL) {
		long i;
		PointProcess point = my pulses.data;
		Graphics_setWindow (my graphics, my startWindow, my endWindow, -1.0, 1.0);
		Graphics_setColour (my graphics, Graphics_BLUE);
		if (point -> nt < 2000) for (i = 1; i <= point -> nt; i ++) {
			double t = point -> t [i];
			if (t >= my startWindow && t <= my endWindow)
				Graphics_line (my graphics, t, -0.9, t, 0.9);
		}
		Graphics_setColour (my graphics, Graphics_BLACK);
	}
}

static int click (TimeSoundAnalysisEditor me, double xbegin, double ybegin, int shiftKeyPressed) {
	if (my pitch.show) {
		//Melder_warning3 (Melder_double (xbegin), L" ", Melder_double (ybegin));
		if (xbegin >= my endWindow && ybegin > 0.48 && ybegin <= 0.50) {
			my pitch.ceiling *= 1.26;
			forget (my pitch.data);
			forget (my intensity.data);
			forget (my pulses.data);
			return 1;
		}
		if (xbegin >= my endWindow && ybegin > 0.46 && ybegin <= 0.48) {
			my pitch.ceiling /= 1.26;
			forget (my pitch.data);
			forget (my intensity.data);
			forget (my pulses.data);
			return 1;
		}
	}
	return inherited (TimeSoundAnalysisEditor) click (me, xbegin, ybegin, shiftKeyPressed);
}

class_methods (TimeSoundAnalysisEditor, TimeSoundEditor) {
	class_method (click)
	class_method (createMenuItems_query_log)
	class_method (createMenuItems_view_sound)
	class_method (createMenuItems_view_sound_analysis)
	class_method (createMenus_analysis)
	class_method (draw_analysis)
	class_method (draw_analysis_pulses)
	class_method (createMenuItems_spectrum_picture)
	class_method (createMenuItems_pitch_picture)
	class_method (createMenuItems_intensity_picture)
	class_method (createMenuItems_formant_picture)
	class_method (createMenuItems_pulses_picture)
	us -> preferences.picture.spectrogram.garnish = true;
	us -> preferences.picture.pitch.garnish = true;
	us -> preferences.picture.intensity.garnish = true;
	us -> preferences.picture.formant.garnish = true;
	us -> preferences.picture.pulses.garnish = true;
	class_methods_end
}

void TimeSoundAnalysisEditor_init (TimeSoundAnalysisEditor me, GuiObject parent, const wchar *title, Data data, Data sound, bool ownSound) {
	TimeSoundEditor_init (me, parent, title, data, sound, ownSound);
	my longestAnalysis = preferences.longestAnalysis;
	if (preferences.log[0].toLogFile == FALSE && preferences.log[0].toInfoWindow == FALSE)
		preferences.log[0].toLogFile = TRUE, preferences.log[0].toInfoWindow = TRUE;
	if (preferences.log[1].toLogFile == FALSE && preferences.log[1].toInfoWindow == FALSE)
		preferences.log[1].toLogFile = TRUE, preferences.log[1].toInfoWindow = TRUE;
	my timeStepStrategy = preferences.timeStepStrategy;
	my fixedTimeStep = preferences.fixedTimeStep;
	my numberOfTimeStepsPerView = preferences.numberOfTimeStepsPerView;
	my spectrogram = preferences.spectrogram;
	my pitch = preferences.pitch;
	my intensity = preferences.intensity;
	my formant = preferences.formant;
	my pulses = preferences.pulses;
}

/* End of file TimeSoundAnalysisEditor.cpp */
