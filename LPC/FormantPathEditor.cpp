/* FormantPathEditor.cpp
 *
 * Copyright (C) 2020-2022 David Weenink, 2022 Paul Boersma
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

/*
	TODO: make width of selection viewer variable?
*/
#include "FormantPathEditor.h"
#include "EditorM.h"

Thing_implement (FormantPathEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "FormantPathEditor_prefs.h"
#include "Prefs_install.h"
#include "FormantPathEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FormantPathEditor_prefs.h"

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

/********** METHODS **********/

/***** TIER MENU *****/

static void menu_cb_candidate_modellingSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
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
	Melder_require (NUMmin (parameters.get()) > 0,
		U"All coefficients should be larger than zero.");
	my setInstancePref_modeler_varianceExponent (varianceExponent);
	FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_AdvancedCandidateDrawingSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant modeler advanced drawing settings", nullptr)
		BOOLEAN (drawEstimatedModels, U"Draw estimated models", my default_modeler_draw_estimatedModels())
		POSITIVE (yGridLineEvery_Hz, U"Hor. grid lines every (Hz)", my default_modeler_draw_yGridLineEvery_Hz())
		POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", my default_modeler_draw_maximumFrequency())
		BOOLEAN (drawErrorBars, U"Draw bandwidths", my default_modeler_draw_showBandwidths())
	EDITOR_OK
		SET_BOOLEAN (drawEstimatedModels, my instancePref_modeler_draw_estimatedModels())
		SET_REAL (yGridLineEvery_Hz, my instancePref_modeler_draw_yGridLineEvery_Hz())
		SET_REAL (maximumFrequency, my instancePref_modeler_draw_maximumFrequency())
		SET_BOOLEAN (drawErrorBars, my instancePref_modeler_draw_showBandwidths())
	EDITOR_DO
		my setInstancePref_modeler_draw_estimatedModels (drawEstimatedModels);
		my setInstancePref_modeler_draw_maximumFrequency (maximumFrequency);
		my setInstancePref_modeler_draw_yGridLineEvery_Hz (yGridLineEvery_Hz);
		my setInstancePref_modeler_draw_showBandwidths (drawErrorBars);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_candidates_FindPath (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find path", nullptr)
		LABEL (U"Within frame:")
		REAL (qWeight, U"F/B weight (0-1)", U"1.0")
		LABEL (U"Between frames:")
		REAL (frequencyChangeWeight, U"Frequency change weight (0-1)", U"1.0")
		REAL (stressWeight, U"Stress weight (0-1)", U"1.0")
		REAL (ceilingChangeWeight, U"Ceiling change weight (0-1)", U"1.0")
		POSITIVE (intensityModulationStepSize, U"Intensity modulation step size (dB)", U"5.0")
		LABEL (U"Global stress parameters:")
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

static void menu_cb_DrawVisibleCandidates (FormantPathEditor me, EDITOR_ARGS_FORM) {
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
		Editor_openPraatPicture (me);
		Graphics_setInner (my pictureGraphics);
		double startTime, endTime, xCursor, yCursor;
		FormantPathEditor_getDrawingData (me, & startTime, & endTime, & xCursor, & yCursor);
		autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (my instancePref_modeler_numberOfParametersPerTrack());
		constexpr double xSpace_fraction = 0.1, ySpace_fraction = 0.2;
		FormantPath_drawAsGrid_inside (my formantPath(), my pictureGraphics, startTime, endTime,
			my instancePref_modeler_draw_maximumFrequency(), 1, 5,
			my instancePref_modeler_draw_showBandwidths(), Melder_RED, Melder_PURPLE, 0, 0,
			xSpace_fraction, ySpace_fraction, my instancePref_modeler_draw_yGridLineEvery_Hz(),
			xCursor, yCursor, markedCandidatesColour, parameters.get(), true, true,
			my instancePref_modeler_varianceExponent(), my instancePref_modeler_draw_estimatedModels(), true
		);
		Graphics_unsetInner (my pictureGraphics);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__stressOfFitsListing (FormantPathEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
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

static void menu_cb_FormantPathEditorHelp (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"FormantPathEditor"); }
static void menu_cb_AboutSpecialSymbols (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Special symbols"); }
static void menu_cb_PhoneticSymbols (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Phonetic symbols"); }
static void menu_cb_AboutTextStyles (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Text styles"); }

void structFormantPathEditor :: v_createMenus () {
	FormantPathEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, U"Candidates", 0);
	EditorMenu_addCommand (menu, U"Candidate modelling settings...", 0, menu_cb_candidate_modellingSettings);
	EditorMenu_addCommand (menu, U"Advanced candidate drawing settings...", 0, menu_cb_AdvancedCandidateDrawingSettings);
	EditorMenu_addCommand (menu, U" -- drawing -- ", 0, 0);
	EditorMenu_addCommand (menu, U"Find path...", 0, menu_cb_candidates_FindPath);
	EditorMenu_addCommand (menu, U"Draw visible candidates...", 0, menu_cb_DrawVisibleCandidates);
	EditorMenu_addCommand (menu, U" -- candidate queries -- ", 0, 0);
	EditorMenu_addCommand (menu, U"Stress of fits listing", 0, INFO_DATA__stressOfFitsListing);
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
		our instancePref_modeler_draw_maximumFrequency(), 1, 5, our instancePref_modeler_draw_showBandwidths(), oddColour, evenColour,
		nrow, ncol, xSpace_fraction, ySpace_fraction, our instancePref_modeler_draw_yGridLineEvery_Hz(), xCursor, yCursor, markedCandidatesColour,
		parameters.get(), true, true, our instancePref_modeler_varianceExponent(), our instancePref_modeler_draw_estimatedModels(), true
	);
	Graphics_unsetInner (our graphics.get());
	previousStartTime = startTime;
	previousEndTime = endTime;
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

void structFormantPathEditor :: v_clickSelectionViewer (double xWC, double yWC) {
	/*
		On which of the modelers was the click?
	*/
	integer numberOfRows, numberOfColums;
	FormantPath_getGridDimensions (our formantPath(), & numberOfRows, & numberOfColums);
	const integer icol = 1 + (int) (xWC * numberOfColums);
	if (icol < 1 || icol > numberOfColums)
		return;
	const integer irow = 1 + (int) ((1.0 - yWC) * numberOfRows);
	if (irow < 1 || irow > numberOfRows)
		return;
	integer index = (irow - 1) * numberOfColums + icol; // left-to-right, top-to-bottom
	if (index > 0 && index <= our formantPath() -> formants.size) {
		double tmin_ = our startWindow, tmax_ = our endWindow;
		if (our startSelection < our endSelection) {
			tmin_ = our startSelection;
			tmax_ = our endSelection;
		}
		our selectedCandidate = index;
		Editor_save (this, U"Change ceiling");
		integer itmin, itmax;
		Sampled_getWindowSamples (our formantPath(), tmin_, tmax_, & itmin, & itmax);
		for (integer iframe = itmin; iframe <= itmax; iframe ++)
			our formantPath() -> path [iframe] = our selectedCandidate;
		Formant source = our formantPath() -> formants.at [our selectedCandidate];
		Formant_replaceFrames (our formantPathArea() -> d_formant.get(), itmin, itmax, source);
	}
	FunctionEditor_redraw (this);
	Editor_broadcastDataChanged (this);
}

void structFormantPathEditor :: v_play (double startingTime, double endTime) {
	if (our soundArea())
		SoundArea_play (our soundArea().get(), startingTime, endTime);
}

/********** EXPORTED **********/

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound soundToCopy, TextGrid textGridToCopy) {
	try {
		autoFormantPathEditor me = Thing_new (FormantPathEditor);
		my soundArea() = SoundArea_create (false, soundToCopy, me.get());
		my formantPathArea() = FormantPathArea_create (true, soundToCopy, me.get());
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
		if (my endWindow - my startWindow > 5.0) {
			my endWindow = my startWindow + 5.0;
			if (my startWindow == my tmin)
				my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
			FunctionEditor_marksChanged (me.get(), false);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantPathEditor window not created.");
	}
}

/* End of file FormantPathEditor.cpp */
