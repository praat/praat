/* FormantPathEditor.cpp
 *
 * Copyright (C) 2020-2023 David Weenink, 2022-2024 Paul Boersma
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

#include "FormantPathEditor.h"
#include "FormantPath_to_IntervalTier.h"
#include "Graphics_extensions.h"
#include "EditorM.h"
#include "Formant_extensions.h"
#include "LPC_and_Formant.h"
#include "NUM2.h"
#include "NUMselect.h"
#include "PitchTier.h"
#include "PitchTier_to_PointProcess.h"
#include "Sound_and_LPC.h"
#include "Sound_extensions.h"
#include "TextGrid_extensions.h"

Thing_implement (FormantPathEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "FormantPathEditor_prefs.h"
#include "Prefs_install.h"
#include "FormantPathEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FormantPathEditor_prefs.h"
#include "PointProcess_and_Sound.h"

void structFormantPathEditor :: v1_info () {
	FormantPathEditor_Parent :: v1_info ();
}

/*
	MelderColour markedCandidatesColour = MelderColour (0.984, 0.951, 0.020); // Melder_YELLOW
	In the selectionViewer, the following light yellow colour combines better with the chosen colours 
	for the odd	and even numbered formants (Melder_RED and Melder_PURPLE, respectively).
*/
MelderColour markedCandidatesColour =  MelderColour (0.984,0.984, 0.7);

/********** UTILITIES **********/

#define MAX_T  0.02000000001   /* Maximum interval between two voice pulses (otherwise voiceless). */

static autoPitchTier Pitch_to_PitchTier_part (Pitch me, double startTime, double endTime) {
	try {
		autoPitchTier thee = PitchTier_create (startTime, endTime);
		for (integer i = 1; i <= my nx; i ++) {
			const double time = Sampled_indexToX (me, i);
			if (time < startTime)
				continue;
			if (time > endTime)
				break;
			const double frequency = my frames [i]. candidates [1]. frequency;
			/*
				Count only voiced frames.
			*/
			if (Pitch_util_frequencyIsVoiced (frequency, my ceiling)) {
				RealTier_addPoint (thee.get(), time, frequency);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not converted to PitchTier.");
	}
}

static autoPointProcess PointProcess_extractPart (PointProcess me, double startTime, double endTime) {
	try {
		integer i1 = 1, i2 = my nt;
		while (i1 <= my nt && my t [i1] < startTime)
			i1 ++;
		while (i2 >= 1 && my t [i2] > endTime)
			i2 --;
		const integer nt = i2 - i1 + 1;
		Melder_require (nt > 0,
			U"There must be at least one point in the interval.");
		autoPointProcess thee = PointProcess_create (startTime, endTime, nt);
		thy nt = nt;
		thy t.size = nt;
		thy t.get()  <<=  my t.part (i1, i2);
		return thee;
	} catch (MelderError) {
			Melder_throw (me, U": could not extract part.");
	}
}

static autoSound FormantPathEditor_getResynthesis (FormantPathEditor me, double startTime, double endTime) {
	try {
		SoundAnalysisArea_haveVisiblePitch (my formantPathArea().get());
		Pitch pitch = my formantPathArea() -> d_pitch.get();
		autoPitchTier pitchtierPart = Pitch_to_PitchTier_part (pitch, startTime, endTime);
		
		SoundAnalysisArea_haveVisiblePulses (my formantPathArea().get());
		PointProcess pulses = my formantPathArea() -> d_pulses.get();
		autoPointProcess pulsesPart = PointProcess_extractPart (pulses, startTime, endTime);
		
		autoPointProcess pulses2 = PitchTier_Point_to_PointProcess (pitchtierPart.get(), pulsesPart.get(), MAX_T);
		const double samplingFrequency = 22050.0;
		autoSound train = PointProcess_to_Sound_pulseTrain (pulses2.get(), samplingFrequency , 0.7, 0.05, 30);
		
		autoFormant formantPart = Formant_extractPart (my formantPathArea() -> d_formant.get(), startTime, endTime);
		autoLPC lpc = Formant_to_LPC (formantPart.get(), 1.0 / samplingFrequency);
		
		autoSound sound = LPC_Sound_filter (lpc.get(), train.get(), false);
		Vector_scale (sound.get(), 0.99);
		constexpr double fadeTime = 0.005;
		Sound_fade (sound.get(), 0, sound -> xmin, fadeTime, false, true);
		Sound_fade (sound.get(), 0, sound -> xmax, -fadeTime, true, true);
		return sound;
	} catch (MelderError) {
		Melder_throw (U"Could not synthesize.");
	}
}

static void FormantPathEditor_getDrawingData (FormantPathEditor me, double *out_startTime, double *out_endTime, double *out_xCursor, double *out_yCursor) {
	double startTime = my startWindow, endTime = my endWindow;
	double xCursor;
	if (my startSelection == my endSelection) {
		startTime = my startWindow;
		endTime = my endWindow;
		xCursor = my startSelection;
	} else {
		startTime = my startSelection;
		endTime = my endSelection;
		xCursor = my tmin - 1.0;   // don't show
	}
	if (out_startTime)
		*out_startTime = startTime;
	if (out_endTime)
		*out_endTime = endTime;	
	if (out_xCursor)
		*out_xCursor = xCursor;
	if (out_yCursor)
		*out_yCursor = ( my formantPathArea() -> d_spectrogram_cursor > my formantPathArea() -> instancePref_spectrogram_viewFrom() &&
				my formantPathArea() -> d_spectrogram_cursor < my formantPathArea() -> instancePref_spectrogram_viewTo() ? my formantPathArea() -> d_spectrogram_cursor : -1000.0 );
}

static void Formant_replaceFrames (Formant target, integer beginFrame, integer endFrame, Formant source) {
	// Precondition target and source have exactly the same Sampled xmin, xmax, x1, nx, dx
	if (beginFrame == endFrame && beginFrame == 0) {
		beginFrame = 1;
		endFrame = target -> nx;
	}
	Melder_require (beginFrame <= endFrame,
		U"The start frame should not be after the end frame.");
	Melder_require (beginFrame > 0,
		U"The begin frame should be larger than zero.");
	Melder_require (endFrame <= target->nx,
		U"The end frame sould not be larger than ", target->nx);
	for (integer iframe = beginFrame ; iframe <= endFrame; iframe ++) {
		Formant_Frame targetFrame = & target -> frames [iframe];
		Formant_Frame sourceFrame = & source -> frames [iframe];
		sourceFrame -> copy (targetFrame);
	}
}

static void FormantPathEditor_effectuateCandidateSelection (FormantPathEditor me, integer candidate) {
	if (candidate > 0 && candidate <= my formantPath() -> formantCandidates.size) {
		double tmin_ = my startWindow, tmax_ = my endWindow;
		if (my startSelection < my endSelection) {
			tmin_ = my startSelection;
			tmax_ = my endSelection;
		}
		my selectedCandidate = candidate;
		Editor_save (me, U"Change ceiling");
		integer ifmin, ifmax;
		Sampled_getWindowSamples (my formantPath(), tmin_, tmax_, & ifmin, & ifmax);
		TextGrid_addInterval_force (my formantPath() -> path.get(), tmin_, tmax_, 1_integer, Melder_integer (candidate));
		Formant source = my formantPath() -> formantCandidates.at [my selectedCandidate];
		Formant_replaceFrames (my formantPathArea() -> d_formant.get(), ifmin, ifmax, source);
	}
}

/********** METHODS **********/

/***** TIER MENU *****/

static void menu_cb_candidate_modellingSettings (FormantPathEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Candidate modelling settings", U"Candidate modelling settings...")		
		SENTENCE (parameters_string, U"Coefficients by track", my default_modeler_numberOfParametersPerTrack())
		POSITIVE (varianceExponent, U"Variance exponent", U"1.25")
	EDITOR_OK
		SET_STRING (parameters_string, my instancePref_modeler_numberOfParametersPerTrack())
	EDITOR_DO
		my setInstancePref_modeler_numberOfParametersPerTrack (parameters_string);
		autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (my instancePref_modeler_numberOfParametersPerTrack());
		Melder_require (parameters.size > 0,
			U"At least one coefficient should be given.");
		const integer numberOfTracks = FormantPath_getNumberOfFormantTracks (my formantPath());
		Melder_require (parameters.size <= numberOfTracks,
			U"The number of coefficients (", parameters.size, U") should not exceed the number of tracks (", numberOfTracks, U").");
		Melder_require (num::NUMmin_e (parameters.get()) > 0.0,
			U"All coefficients should be larger than zero.");
		my setInstancePref_modeler_varianceExponent (varianceExponent);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static double FormantPath_getRoundedMaximumCeiling (FormantPath me) {
	const integer ceiling = 100 * Melder_iroundDown ((my ceilings [my ceilings.size] + 100.1) / 100.0);
	return (double) ceiling;
}

static void menu_cb_AdvancedCandidateDrawingSettings (FormantPathEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Candidate drawing settings", nullptr)
		BOOLEAN (drawEstimatedModels, U"Draw estimated models", my default_candidate_draw_estimatedModels())
		POSITIVE (yGridLineEvery_Hz, U"Hor. grid lines every (Hz)", my default_candidate_draw_yGridLineEvery_Hz())
		COMMENT (U"Set the maximum frequency for the display of the candidates...")
		POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", my default_candidate_draw_maximumFrequency())
		COMMENT (U"...or, overrule this setting by using the maximum ceiling instead...")
		BOOLEAN (useMaximumCeiling, U"Use maximum ceiling", my default_candidate_draw_useMaximumCeiling())
		COMMENT (U"If you want the Spectrogram and the candidates to have the same maximum frequency.")
		BOOLEAN  (adjustSpectrogramView, U"Adjust spectrogram view", my default_candidate_draw_adjustSpectrogramView());
		BOOLEAN (drawErrorBars, U"Draw bandwidths", my default_candidate_draw_showBandwidths())
	EDITOR_OK
		SET_BOOLEAN (drawEstimatedModels, my instancePref_candidate_draw_estimatedModels())
		SET_REAL (yGridLineEvery_Hz, my instancePref_candidate_draw_yGridLineEvery_Hz())
		SET_REAL (maximumFrequency, my instancePref_candidate_draw_maximumFrequency())
		SET_BOOLEAN (useMaximumCeiling, my instancePref_candidate_draw_useMaximumCeiling()) 
		SET_BOOLEAN (adjustSpectrogramView, my instancePref_candidate_draw_adjustSpectrogramView())
		SET_BOOLEAN (drawErrorBars, my instancePref_candidate_draw_showBandwidths())
	EDITOR_DO
		my setInstancePref_candidate_draw_estimatedModels (drawEstimatedModels);
		if (useMaximumCeiling) 
			maximumFrequency = FormantPath_getRoundedMaximumCeiling (my formantPath());
		my setInstancePref_candidate_draw_maximumFrequency (maximumFrequency);
		my setInstancePref_candidate_draw_yGridLineEvery_Hz (yGridLineEvery_Hz);
		my setInstancePref_candidate_draw_adjustSpectrogramView (adjustSpectrogramView);
		if (adjustSpectrogramView)
			my formantPathArea() -> setInstancePref_spectrogram_viewTo (maximumFrequency);
		my setInstancePref_candidate_draw_showBandwidths (drawErrorBars);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_candidates_FindPath (FormantPathEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Find path", nullptr)
		COMMENT (U"Within frame:")
		REAL (qWeight, U"F/B weight (0-1)", U"1.0")
		COMMENT (U"Between frames:")
		REAL (frequencyChangeWeight, U"Frequency change weight (0-1)", U"1.0")
		REAL (stressWeight, U"Stress weight (0-1)", U"1.0")
		REAL (ceilingChangeWeight, U"Ceiling change weight (0-1)", U"1.0")
		POSITIVE (intensityModulationStepSize, U"Intensity modulation step size (dB)", U"5.0")
		COMMENT (U"Global stress parameters:")
		POSITIVE (windowLength, U"Window length", U"0.035")
	EDITOR_OK
	EDITOR_DO
		autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (my instancePref_modeler_numberOfParametersPerTrack());
		FormantPath_pathFinder (my formantPath(), qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight,
				intensityModulationStepSize, windowLength, parameters.get(), my instancePref_modeler_varianceExponent());
		my formantPathArea() -> d_formant = FormantPath_extractFormant (my formantPath());
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_selectCandidateWithlowestStress (FormantPathEditor me, EDITOR_ARGS) {
	double startTime = my startSelection, endTime = my endSelection;
	if (my startSelection == my endSelection) {
		startTime = my startWindow;
		endTime = my endWindow;
	}
	autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (my instancePref_modeler_numberOfParametersPerTrack());
	autoVEC stresses = FormantPath_getStressOfCandidates (my formantPath(), startTime, endTime, 0, 0, parameters.get(), 
		my instancePref_modeler_varianceExponent());
	const integer minPos = NUMminPos (stresses.get());
	Editor_save (me, U"Change ceiling");
	FormantPathEditor_effectuateCandidateSelection (me, minPos);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_DrawVisibleCandidates (FormantPathEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible candidates", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		BOOLEAN (crossHairs, U"Draw cross hairs", 0)
		BOOLEAN (garnish, U"Garnish", my formantPathArea() -> default_picture_garnish());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		SET_BOOLEAN (garnish, my formantPathArea() -> classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my formantPathArea() -> setClassPref_picture_garnish (garnish);
		DataGui_openPraatPicture (me);
		Graphics_setInner (my pictureGraphics());
		double startTime, endTime, xCursor, yCursor;
		FormantPathEditor_getDrawingData (me, & startTime, & endTime, & xCursor, & yCursor);
		autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (my instancePref_modeler_numberOfParametersPerTrack());
		constexpr double xSpace_fraction = 0.1, ySpace_fraction = 0.2;
		FormantPath_drawAsGrid_inside (my formantPath(), my pictureGraphics(), startTime, endTime,
			my instancePref_candidate_draw_maximumFrequency(), 1, 5,
			my instancePref_candidate_draw_showBandwidths(), Melder_RED, Melder_PURPLE, 0, 0,
			xSpace_fraction, ySpace_fraction, my instancePref_candidate_draw_yGridLineEvery_Hz(),
			xCursor, yCursor, markedCandidatesColour, parameters.get(), true, true,
			my instancePref_modeler_varianceExponent(), my instancePref_candidate_draw_estimatedModels(), true
		);
		Graphics_unsetInner (my pictureGraphics());
		DataGui_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_play_resynthesis (FormantPathEditor me, EDITOR_ARGS) {
	PLAY_DATA
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
		double startTime,endTime;
		FormantPathEditor_getDrawingData (me, & startTime, & endTime, nullptr, nullptr);
		autoSound resynthesis = FormantPathEditor_getResynthesis (me, startTime, endTime);
		Sound_playPart (resynthesis.get(), startTime, endTime, theFunctionEditor_playCallback, me);
	PLAY_DATA_END
}

static void menu_cb_extract_resynthesis (FormantPathEditor me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		double startTime,endTime;
		FormantPathEditor_getDrawingData (me, & startTime, & endTime, nullptr, nullptr);
	
		autoSound result = FormantPathEditor_getResynthesis (me, startTime, endTime);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void INFO_DATA__ceilingOfCandidate (FormantPathEditor me, EDITOR_ARGS) {
	INFO_DATA
		if (my selectedCandidate > 0 && my selectedCandidate <= my formantPath() -> ceilings.size)
			Melder_information (Melder_double (my formantPath() -> ceilings [my selectedCandidate]));
		else
			Melder_information (U"--undefined--");
	INFO_DATA_END
}

static void INFO_DATA__stressOfFitsListing (FormantPathEditor me, EDITOR_ARGS) {
	INFO_DATA
		double startTime = my startSelection, endTime = my endSelection;
		if (my startSelection == my endSelection) {
			startTime = my startWindow;
			endTime = my endWindow;
		}
		autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (my instancePref_modeler_numberOfParametersPerTrack());
		const integer numberOfStressDecimals = 2, numberOfTimeDecimals = 6;
		autoTable stressTable = FormantPath_downTo_Table_stresses (my formantPath(), startTime, endTime, parameters.get(),
				my instancePref_modeler_varianceExponent(), numberOfStressDecimals, true, numberOfTimeDecimals);
		Table_list (stressTable.get(), false);
	INFO_DATA_END
}

/***** HELP MENU *****/

static void menu_cb_FormantPathEditorHelp (FormantPathEditor, EDITOR_ARGS) {
	Melder_help (U"FormantPathEditor");
}

static void menu_cb_AboutSpecialSymbols (FormantPathEditor, EDITOR_ARGS) {
	Melder_help (U"Special symbols");
}

static void menu_cb_PhoneticSymbols (FormantPathEditor, EDITOR_ARGS) {
	Melder_help (U"Phonetic symbols");
}

static void menu_cb_AboutTextStyles (FormantPathEditor, EDITOR_ARGS) {
	Melder_help (U"Text styles");
}

void structFormantPathEditor :: v_createMenus () {
	FormantPathEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, U"Candidates", 0);
	EditorMenu_addCommand (menu, U"Candidate modelling settings...", 0, menu_cb_candidate_modellingSettings);
	EditorMenu_addCommand (menu, U"Advanced candidate drawing settings...", 0, menu_cb_AdvancedCandidateDrawingSettings);
	EditorMenu_addCommand (menu, U"- Draw candidates to picture window:", 0, nullptr);
		EditorMenu_addCommand (menu, U"Draw visible candidates...", 1, menu_cb_DrawVisibleCandidates);
	EditorMenu_addCommand (menu, U"- Select candidates:", 0, nullptr);
		EditorMenu_addCommand (menu, U"Select candidate with lowest stress", 1, menu_cb_selectCandidateWithlowestStress);
	EditorMenu_addCommand (menu, U"Find path...", 1, menu_cb_candidates_FindPath);
	EditorMenu_addCommand (menu, U"- Query candidates:", 0, nullptr);
		EditorMenu_addCommand (menu, U"Get ceiling of candidate", 1, INFO_DATA__ceilingOfCandidate);
		EditorMenu_addCommand (menu, U"Stress of fits listing", 1, INFO_DATA__stressOfFitsListing);
	if (soundArea()) {
		EditorMenu_addCommand (menu, U"- Candidate resynthesis:", 0, nullptr);
		EditorMenu_addCommand (menu, U"Play resynthesis", 1, menu_cb_play_resynthesis);
		EditorMenu_addCommand (menu, U"Extract resynthesis", 1, menu_cb_extract_resynthesis);
	}
}

void structFormantPathEditor :: v_createMenuItems_help (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"FormantPathEditor help", '?', menu_cb_FormantPathEditorHelp);
	EditorMenu_addCommand (menu, U"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, U"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, U"About text styles", 0, menu_cb_AboutTextStyles);
}

/********** DRAWING AREA **********/

void structFormantPathEditor :: v_drawSelectionViewer () {
	static double previousStartTime, previousEndTime;
	constexpr double xSpace_fraction = 0.1, ySpace_fraction = 0.2;
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFontSize (our graphics.get(), 10.0);
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
	double startTime, endTime = endWindow, xCursor, yCursor;
	FormantPathEditor_getDrawingData (this, & startTime, & endTime, & xCursor, & yCursor);
	const integer nrow = 0, ncol = 0;
	if (startTime != previousStartTime || endTime != previousEndTime)
		our selectedCandidate = 0;
	autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (our instancePref_modeler_numberOfParametersPerTrack());
	MelderColour oddColour = MelderColour_fromColourName (our formantPathArea() -> instancePref_formant_path_oddColour());
	MelderColour evenColour = MelderColour_fromColourName (our formantPathArea() -> instancePref_formant_path_evenColour());
	/*
		Put the number of model coefficients per track at the top
	*/
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	autoMelderString infoAtTop;
	MelderString_append (& infoAtTop, U"Coefficients by track: ", our instancePref_modeler_numberOfParametersPerTrack());
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (our graphics.get(), 0.5, 1.0, infoAtTop.string);
	/*
		Now do the inset
	*/
	Graphics_setInner (our graphics.get());
	FormantPath_drawAsGrid_inside (our formantPath(), our graphics.get(), startTime, endTime,
		our instancePref_candidate_draw_maximumFrequency(), 1, 5, our instancePref_candidate_draw_showBandwidths(), oddColour, evenColour,
		nrow, ncol, xSpace_fraction, ySpace_fraction, our instancePref_candidate_draw_yGridLineEvery_Hz(), xCursor, yCursor, markedCandidatesColour,
		parameters.get(), true, true, our instancePref_modeler_varianceExponent(), our instancePref_candidate_draw_estimatedModels(), true
	);
	/*
		If we don't have clicked on a candidate, one or more have been implicitely coloured / selected.
		Find out which one so we can set the 'selectedCandidate'
	*/
	if (selectedCandidate == 0)
		selectedCandidate = FormantPath_getUniqueCandidateInInterval (our formantPath(), startTime, endTime);
	Graphics_unsetInner (our graphics.get());
	previousStartTime = startTime;
	previousEndTime = endTime;
}

void structFormantPathEditor :: v_clickSelectionViewer (double xWC, double yWC) {
	/*
		On which of the modelers was the click?
	*/
	integer numberOfRows, numberOfColumns;
	const integer numberOfCandidates = our formantPath() -> formantCandidates.size;
	getGridLayout (numberOfCandidates, & numberOfRows, & numberOfColumns);
	integer candidate = getGridCellIndex (xWC, yWC, numberOfRows, numberOfColumns);
	FormantPathEditor_effectuateCandidateSelection (this, candidate);
	FunctionEditor_redraw (this);
	Editor_broadcastDataChanged (this);
}

void structFormantPathEditor :: v_play (double startingTime, double endTime) {
	if (our soundArea()) {
		if (our clickWasModifiedByShiftKey) {
			autoSound resynthesis = FormantPathEditor_getResynthesis (this, startingTime, endTime);
			Sound_playPart (resynthesis.get(), startingTime, endTime, theFunctionEditor_playCallback, this);
			our clickWasModifiedByShiftKey = false;
		} else {
			SoundArea_play (our soundArea().get(), startingTime, endTime);
		}
	}
}


/********** EXPORTED **********/

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound soundToCopy, TextGrid textGridToCopy) {
	try {
		autoFormantPathEditor me = Thing_new (FormantPathEditor);
		if (soundToCopy)
			my soundArea() = SoundArea_create (false, soundToCopy, me.get());
		my formantPathArea() =
			soundToCopy
				?
			FormantPathArea_create (true, soundToCopy, me.get())
				:
			(autoFormantPathArea) FormantPathArea_without_Sound_create (true, nullptr, me.get())
		;
		my formantPathArea() -> _formantPath = formantPath;
		if (textGridToCopy)
			my textGridArea() = TextGridArea_create (false, textGridToCopy, me.get());
		FunctionEditor_init (me.get(), title, formantPath);
		my formantPathArea() -> d_formant = FormantPath_extractFormant (formantPath);   // BUG: should be in other places
		if (my instancePref_modeler_numberOfParametersPerTrack() [0] == U'\0')
			my setInstancePref_modeler_numberOfParametersPerTrack (my default_modeler_numberOfParametersPerTrack());
		if (my formantPathArea() -> instancePref_formant_default_colour() [0] == U'\0')
			my formantPathArea() -> setInstancePref_formant_default_colour (my formantPathArea() -> default_formant_default_colour ());
		if (my formantPathArea() -> instancePref_formant_path_oddColour() [0] == U'\0')
			my formantPathArea() -> setInstancePref_formant_path_oddColour (my formantPathArea() -> default_formant_path_oddColour ());
		if (my formantPathArea() -> instancePref_formant_path_evenColour() [0] == U'\0')
			my formantPathArea() -> setInstancePref_formant_path_evenColour (my formantPathArea() -> default_formant_path_evenColour ());
		if (my formantPathArea() -> instancePref_formant_selected_colour() [0] == U'\0')
			my formantPathArea() -> setInstancePref_formant_selected_colour (my formantPathArea() -> default_formant_selected_colour ());
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantPathEditor window not created.");
	}
}

/* End of file FormantPathEditor.cpp */
