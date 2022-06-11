/* FormantPathEditor.cpp
 *
 * Copyright (C) 2020-2021 David Weenink, 2022 Paul Boersma
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
#include "FormantPath_to_IntervalTier.h"
#include "EditorM.h"
#include "melder_kar.h"
#include "Sampled.h"
#include "SoundEditor.h"
#include "Sound_and_MixingMatrix.h"
#include "Sound_and_Spectrogram.h"
#include "TextGrid_Sound.h"
#include "TextGrid_extensions.h"

Thing_implement (FormantPathEditor, TimeSoundAnalysisEditor, 0);

#include "Prefs_define.h"
#include "FormantPathEditor_prefs.h"
#include "Prefs_install.h"
#include "FormantPathEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FormantPathEditor_prefs.h"

void structFormantPathEditor :: v_info () {
	FormantPathEditor_Parent :: v_info ();
}

void structFormantPathEditor :: v_reset_analysis () {
	our d_spectrogram. reset();
	our d_pitch. reset();
	our d_intensity. reset();
	our d_pulses. reset();
	// not my formant!
}

/*
	MelderColour markedCandidatesColour = MelderColour (0.984, 0.951, 0.020); // Melder_YELLOW
	In the selectionViewer, the following light yellow colour combines better with the chosen colours 
	for the odd	and even numbered formants (Melder_RED and Melder_PURPLE, respectively).
*/
MelderColour markedCandidatesColour =  MelderColour (0.984,0.984, 0.7);

void structFormantPathEditor :: v_updateMenuItems_navigation () {
	/*FormantPath formantPath = (FormantPath) our data;
	IntervalTierNavigator navigator = formantPath -> intervalTierNavigator.get();
	const bool navigationPossible = ( navigator && IntervalTierNavigator_isNavigationPossible (navigator), formantPath -> navigationTierNumber) );
	bool nextSensitive = false;
	bool previousSensitive = false;
	if (navigationPossible) {
		if (IntervalTierNavigator_getPreviousMatchingIntervalNumberFromTime (navigator, our startSelection) > 0)
			previousSensitive = true;
		if (IntervalTierNavigator_getNextMatchingIntervalNumberFromTime (navigator, our endSelection) > 0)
			nextSensitive = true;
	}
	GuiThing_setSensitive (our navigateSettingsButton, navigationPossible);
	GuiThing_setSensitive (our navigateNextButton, nextSensitive);
	GuiThing_setSensitive (our navigatePreviousButton, previousSensitive);*/
}

bool structFormantPathEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double xWC, double yWC) {
	const double spectrogramTop = our v_getBottomOfSoundArea (), spectrogramBottom = our v_getBottomOfSoundAndAnalysisArea ();
	if ((our instancePref_spectrogram_show() || our instancePref_formant_show()) && yWC < spectrogramTop && yWC > spectrogramBottom &&
			xWC > our startWindow && xWC < our endWindow)
	{
		const double yFractionFromBottomOfSpectrogram = (yWC - spectrogramBottom) / (spectrogramTop - spectrogramBottom);
		our d_spectrogram_cursor = our instancePref_spectrogram_viewFrom() +
				yFractionFromBottomOfSpectrogram  * (our instancePref_spectrogram_viewTo() - our instancePref_spectrogram_viewFrom());
	}
	return FormantPathEditor_Parent :: v_mouseInWideDataView (event, xWC, yWC);
}

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
		*out_yCursor = ( my d_spectrogram_cursor > my instancePref_spectrogram_viewFrom() &&
				my d_spectrogram_cursor < my instancePref_spectrogram_viewTo() ? my d_spectrogram_cursor : -1000.0 );
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
		my d_formant = FormantPath_extractFormant (my formantPath());
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleCandidates (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible candidates", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		BOOLEAN (crossHairs, U"Draw cross hairs", 0)
		BOOLEAN (garnish, U"Garnish", my default_function_picture_garnish());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		SET_BOOLEAN (garnish, my classPref_function_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my setClassPref_function_picture_garnish (garnish);
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

static void menu_cb_FormantColourSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant colour settings", nullptr)
		WORD (oddPathColour_string, U"Dots in F1, F3, F5", my default_formant_path_oddColour())
		WORD (evenPathColour_string, U"Dots in F2, F4", my default_formant_path_evenColour())
	EDITOR_OK
		SET_STRING (oddPathColour_string, my instancePref_formant_path_oddColour())
		SET_STRING (evenPathColour_string, my instancePref_formant_path_evenColour())
	EDITOR_DO
		my setInstancePref_formant_path_oddColour (oddPathColour_string);
		my setInstancePref_formant_path_evenColour (evenPathColour_string);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleFormantContour (FormantPathEditor me, EDITOR_ARGS_FORM) {
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
		Editor_openPraatPicture (me);
		//FormantPath formantPath = (FormantPath) my data;
		//const Formant formant = formantPath -> formant.get();
		//const Formant defaultFormant = formantPath -> formants.at [formantPath -> defaultFormant];
		Formant_drawSpeckles (my d_formant.get(), my pictureGraphics, my startWindow, my endWindow,
			my instancePref_spectrogram_viewTo(), my instancePref_formant_dynamicRange(),
			garnish
		);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_showFormants (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	my setInstancePref_formant_show (! my instancePref_formant_show());   // toggle
	GuiMenuItem_check (my formantToggle, my instancePref_formant_show());   // in case we're called from a script
	FunctionEditor_redraw (me);
}

static void INFO_DATA__formantListing (FormantPathEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		const double startTime = my startSelection, endTime = my endSelection;
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s   F1_Hz   F2_Hz   F3_Hz   F4_Hz");
		if (startTime == endTime) {
			const double f1 = Formant_getValueAtTime (my d_formant.get(), 1, startTime, kFormant_unit::HERTZ);
			const double f2 = Formant_getValueAtTime (my d_formant.get(), 2, startTime, kFormant_unit::HERTZ);
			const double f3 = Formant_getValueAtTime (my d_formant.get(), 3, startTime, kFormant_unit::HERTZ);
			const double f4 = Formant_getValueAtTime (my d_formant.get(), 4, startTime, kFormant_unit::HERTZ);
			MelderInfo_writeLine (Melder_fixed (startTime, 6), U"   ", Melder_fixed (f1, 6), U"   ", Melder_fixed (f2, 6), U"   ", Melder_fixed (f3, 6), U"   ", Melder_fixed (f4, 6));
		} else {
			integer i1, i2;
			Sampled_getWindowSamples (my d_formant.get(), startTime, endTime, & i1, & i2);
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
void structFormantPathEditor :: v_createMenuItems_formant (EditorMenu menu) {
	formantToggle = EditorMenu_addCommand (menu, U"Show formants",
			GuiMenu_CHECKBUTTON | ( instancePref_formant_show() ? GuiMenu_TOGGLE_ON : 0 ), menu_cb_showFormants);
	EditorMenu_addCommand (menu, U"Formant colour settings...", 0, menu_cb_FormantColourSettings);
	EditorMenu_addCommand (menu, U"Draw visible formant contour...", 0, menu_cb_DrawVisibleFormantContour);
	EditorMenu_addCommand (menu, U"Formant listing", 0, INFO_DATA__formantListing);
}

/***** HELP MENU *****/

static void menu_cb_FormantPathEditorHelp (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"FormantPathEditor"); }
static void menu_cb_AboutSpecialSymbols (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Special symbols"); }
static void menu_cb_PhoneticSymbols (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Phonetic symbols"); }
static void menu_cb_AboutTextStyles (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Text styles"); }

void structFormantPathEditor :: v_createMenus () {
	FormantPathEditor_Parent :: v_createMenus ();
	EditorMenu menu;
//	Editor_addCommand (this, U"Edit", U"-- search --", 0, nullptr);
//	Editor_addCommand (this, U"Edit", U"Find...", 'F', menu_cb_Find);
//	Editor_addCommand (this, U"Edit", U"Find again", 'G', menu_cb_FindAgain);

//	Editor_addCommand (this, U"Query", U"-- query interval --", 0, nullptr);
//	Editor_addCommand (this, U"Query", U"Get starting point of interval", 0, menu_cb_GetStartingPointOfInterval);
//	Editor_addCommand (this, U"Query", U"Get end point of interval", 0, menu_cb_GetEndPointOfInterval);
//	Editor_addCommand (this, U"Query", U"Get label of interval", 0, menu_cb_GetLabelOfInterval);

//	menu = Editor_addMenu (this, U"Interval", 0);
//	EditorMenu_addCommand (menu, U"-- green stuff --", 0, nullptr);
	
//	our navigateSettingsButton = EditorMenu_addCommand (menu, U"Navigation settings...", 0, menu_cb_NavigationSettings);
//	our navigateNextButton  = EditorMenu_addCommand (menu, U"Next green interval", 0, menu_cb_NextGreenInterval);
//	our navigatePreviousButton = EditorMenu_addCommand (menu, U"Previous green interval", 0, menu_cb_PreviousGreenInterval);

//	menu = Editor_addMenu (this, U"Tier", 0);
//	EditorMenu_addCommand (menu, U"-- remove tier --", 0, nullptr);
//	EditorMenu_addCommand (menu, U"-- extract tier --", 0, nullptr);

	if (our soundOrLongSound()) {
		if (our v_hasAnalysis ())
			our v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the TextGrid menus
	}
	menu = Editor_addMenu (this, U"Candidates", 0);
	EditorMenu_addCommand (menu, U"Candidate modelling settings...", 0, menu_cb_candidate_modellingSettings);
	EditorMenu_addCommand (menu, U"Advanced candidate drawing settings...", 0, menu_cb_AdvancedCandidateDrawingSettings);
	EditorMenu_addCommand (menu, U" -- drawing -- ", 0, 0);
	EditorMenu_addCommand (menu, U"Find path...", 0, menu_cb_candidates_FindPath);
	EditorMenu_addCommand (menu, U"Draw visible candidates...", 0, menu_cb_DrawVisibleCandidates);
	EditorMenu_addCommand (menu, U" -- candidate queries -- ", 0, 0);
	EditorMenu_addCommand (menu, U"Stress of fits listing", 0, INFO_DATA__stressOfFitsListing);
}

void structFormantPathEditor :: v_createHelpMenuItems (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"FormantPathEditor help", '?', menu_cb_FormantPathEditorHelp);
	EditorMenu_addCommand (menu, U"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, U"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, U"About text styles", 0, menu_cb_AboutTextStyles);
}

/***** CHILDREN *****/

void structFormantPathEditor :: v_createChildren () {
	FormantPathEditor_Parent :: v_createChildren ();
	/*if (our text)
		GuiText_setChangedCallback (our text, gui_text_cb_changed, this);*/
}

void structFormantPathEditor :: v_dataChanged () {
	//if (our textgrid) {
		/*
			Perform a minimal selection change.
			Most changes will involve intervals and boundaries; however, there may also be tier removals.
			Do a simple guess.
		*/
	//	Melder_clipRight (& our selectedTier, our textgrid -> tiers->size);
	//}
	our d_formant = FormantPath_extractFormant (our formantPath());
	our v_updateMenuItems_navigation ();
	FormantPathEditor_Parent :: v_dataChanged ();
}

/********** DRAWING AREA **********/

void structFormantPathEditor :: v_prepareDraw () {
	if (our longSound()) {
		try {
			LongSound_haveWindow (our longSound(), our startWindow, our endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void structFormantPathEditor :: v_draw () {
	Graphics_Viewport vp1;
	const bool showAnalysis = our v_hasAnalysis () &&
		(our instancePref_spectrogram_show() || our instancePref_pitch_show() || our instancePref_intensity_show() || our instancePref_formant_show()) &&
		our soundOrLongSound()
	;
	const double soundBottom = our v_getBottomOfSoundArea ();

	/*
		Draw the sound.
	*/
	if (our soundOrLongSound()) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundBottom, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		SoundArea_draw (our soundArea.get(), -1.0, 1.0);
		Graphics_resetViewport (our graphics.get(), vp1);
	}

	/*
		Draw tiers.
	*/
	//if (our textgrid) {}
	if (showAnalysis) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, /* our textgrid ? 0.3 : */ 0.0, soundBottom);
		our v_draw_analysis ();
		Graphics_resetViewport (our graphics.get(), vp1);
		/* Draw pulses. */
		if (our instancePref_pulses_show()) {
			vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundBottom, 1.0);
			our v_draw_analysis_pulses ();
			SoundArea_draw (our soundArea.get(), -1.0, 1.0);   // second time, partially across the pulses
			Graphics_resetViewport (our graphics.get(), vp1);
		}
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	/*if (our soundOrLongSound()) {
		Graphics_line (our graphics.get(), our startWindow, soundBottom, our endWindow, soundBottom);
		if (showAnalysis) {
			Graphics_line (our graphics.get(), our startWindow, soundBottom2, our endWindow, soundBottom2);
			Graphics_line (our graphics.get(), our startWindow, soundBottom, our startWindow, soundBottom2);
			Graphics_line (our graphics.get(), our endWindow, soundBottom, our endWindow, soundBottom2);
		}
	}*/

	/*
		Finally, us usual, update the menus.
	*/
	our v_updateMenuItems_file ();
	our v_updateMenuItems_navigation ();
}

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
	Graphics_setInner (our graphics.get());
	const integer nrow = 0, ncol = 0;
	if (startTime != previousStartTime || endTime != previousEndTime)
		our selectedCandidate = 0;
	autoINTVEC parameters = splitByWhitespaceWithRanges_INTVEC (our instancePref_modeler_numberOfParametersPerTrack());
	MelderColour oddColour = MelderColour_fromColourName (our instancePref_formant_path_oddColour());
	MelderColour evenColour = MelderColour_fromColourName (our instancePref_formant_path_evenColour());
	FormantPath_drawAsGrid_inside (our formantPath(), our graphics.get(), startTime, endTime,
		our instancePref_modeler_draw_maximumFrequency(), 1, 5, our instancePref_modeler_draw_showBandwidths(), oddColour, evenColour,
		nrow, ncol, xSpace_fraction, ySpace_fraction, our instancePref_modeler_draw_yGridLineEvery_Hz(), xCursor, yCursor, markedCandidatesColour,
		parameters.get(), true, true, our instancePref_modeler_varianceExponent(), our instancePref_modeler_draw_estimatedModels(), true
	);
	Graphics_unsetInner (our graphics.get());
	previousStartTime = startTime;
	previousEndTime = endTime;
}

static void FormantPathEditor_drawCeilings (FormantPathEditor me, Graphics g, double tmin, double tmax, double fmin, double fmax) {
	autoIntervalTier intervalTier = FormantPath_to_IntervalTier (my formantPath(), tmin, tmax);
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BASELINE);
	Graphics_setColour (g, Melder_RED);
	Graphics_setLineWidth (g, 3.0);
	for (integer interval = 1; interval <= intervalTier -> intervals.size; interval ++) {
		TextInterval textInterval = intervalTier -> intervals.at [interval];
		conststring32 label = textInterval -> text.get();
		if (label) {
			const integer index = Melder_atoi (label);
			if (index > 0 && index <= my formantPath() -> ceilings.size) {
				const double ceiling = my formantPath() -> ceilings [index];
				Graphics_line (g, textInterval -> xmin, ceiling, textInterval -> xmax, ceiling);
				Graphics_text (g, 0.5 * (textInterval -> xmin + textInterval -> xmax), ceiling + 50.0, Melder_fixed (ceiling, 0));
			}
		}
	}
	Graphics_setLineWidth (g, 1.0);
}

void structFormantPathEditor :: v_draw_analysis_formants () {
	if (our instancePref_formant_show()) {
		Graphics_setColour (our graphics.get(), Melder_RED);
		Graphics_setSpeckleSize (our graphics.get(), our instancePref_formant_dotSize());
		MelderColour oddColour = MelderColour_fromColourName (our instancePref_formant_path_oddColour());
		MelderColour evenColour = MelderColour_fromColourName (our instancePref_formant_path_evenColour());
	
		Formant_drawSpeckles_inside (our d_formant.get(), our graphics.get(), our startWindow, our endWindow,
			our instancePref_spectrogram_viewFrom(), our instancePref_spectrogram_viewTo(), our instancePref_formant_dynamicRange(),
			oddColour, evenColour, true
		);
		Graphics_setColour (our graphics.get(), Melder_PINK);
		FormantPathEditor_drawCeilings (this, our graphics.get(), our startWindow, our endWindow,
				our instancePref_spectrogram_viewFrom(), our instancePref_spectrogram_viewTo());
		Graphics_setColour (our graphics.get(), Melder_BLACK);
	}
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
		Formant_replaceFrames (d_formant.get(), itmin, itmax, source);
	}
	FunctionEditor_redraw (this);
	Editor_broadcastDataChanged (this);
}

void structFormantPathEditor :: v_play (double tmin_, double tmax_) {
	if (! our soundOrLongSound())
		return;
	const integer numberOfChannels = our soundOrLongSound() -> ny;
	integer numberOfMuteChannels = 0;
	Melder_assert (our soundArea -> muteChannels.size == numberOfChannels);
	for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
		if (our soundArea -> muteChannels [ichan])
			numberOfMuteChannels ++;
	const integer numberOfChannelsToPlay = numberOfChannels - numberOfMuteChannels;
	Melder_require (numberOfChannelsToPlay > 0,
		U"Please select at least one channel to play.");
	if (our longSound()) {
		if (numberOfMuteChannels > 0) {
			autoSound part = LongSound_extractPart (our longSound(), tmin_, tmax_, true);
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our soundArea -> muteChannels.get());
			Sound_MixingMatrix_playPart (part.get(), thee.get(), tmin_, tmax_, theFunctionEditor_playCallback, this);
		} else {
			LongSound_playPart (our longSound(), tmin_, tmax_, theFunctionEditor_playCallback, this);
		}
	} else {
		if (numberOfMuteChannels > 0) {
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our soundArea -> muteChannels.get());
			Sound_MixingMatrix_playPart (our sound(), thee.get(), tmin_, tmax_, theFunctionEditor_playCallback, this);
		} else {
			Sound_playPart (our sound(), tmin_, tmax_, theFunctionEditor_playCallback, this);
		}
	}
}

POSITIVE_VARIABLE (v_prefs_addFields__fontSize)
OPTIONMENU_ENUM_VARIABLE (kGraphics_horizontalAlignment, v_prefs_addFields__textAlignmentInIntervals)
OPTIONMENU_VARIABLE (v_prefs_addFields__useTextStyles)
OPTIONMENU_ENUM_VARIABLE (kTextGridEditor_showNumberOf, v_prefs_addFields__showNumberOf)
void structFormantPathEditor :: v_prefs_addFields (EditorCommand cmd) {
	UiField _radio_;
	POSITIVE_FIELD (v_prefs_addFields__fontSize, U"Font size (points)", our default_fontSize ())
	OPTIONMENU_ENUM_FIELD (kGraphics_horizontalAlignment, v_prefs_addFields__textAlignmentInIntervals,
			U"Text alignment in intervals", kGraphics_horizontalAlignment::DEFAULT)
	OPTIONMENU_FIELD (v_prefs_addFields__useTextStyles, U"The symbols %#_^ in labels", our default_useTextStyles() + 1)
		OPTION (U"are shown as typed")
		OPTION (U"mean italic/bold/sub/super")
	OPTIONMENU_ENUM_FIELD (kTextGridEditor_showNumberOf, v_prefs_addFields__showNumberOf,
			U"Show number of", kTextGridEditor_showNumberOf::DEFAULT)
}

void structFormantPathEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_OPTION (v_prefs_addFields__useTextStyles, our instancePref_useTextStyles() + 1)
	SET_REAL (v_prefs_addFields__fontSize, our instancePref_fontSize())
	SET_ENUM (v_prefs_addFields__textAlignmentInIntervals, kGraphics_horizontalAlignment, our instancePref_alignment())
	SET_ENUM (v_prefs_addFields__showNumberOf, kTextGridEditor_showNumberOf, our instancePref_showNumberOf())
}

void structFormantPathEditor :: v_prefs_getValues (EditorCommand /* cmd */) {
	our setInstancePref_useTextStyles (v_prefs_addFields__useTextStyles - 1);
	our setInstancePref_fontSize (v_prefs_addFields__fontSize);
	our setInstancePref_alignment (v_prefs_addFields__textAlignmentInIntervals);
	our setInstancePref_shiftDragMultiple (false);
	our setInstancePref_showNumberOf (v_prefs_addFields__showNumberOf);
}

void structFormantPathEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_view_timeDomain (menu);
}

void structFormantPathEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our v_hasAnalysis () && our instancePref_spectrogram_show() && our soundOrLongSound())
		Graphics_highlight (our graphics.get(), left, right, bottom + (top - bottom) * our v_getBottomOfSoundArea (), top);
	else
		Graphics_highlight (our graphics.get(), left, right, bottom, top);
}

double structFormantPathEditor :: v_getBottomOfSoundArea () {
	/*
		We want half of the screen for the spectrogram. 3/8 for the sound and 1/8 for the textgrid
	*/
	return our soundOrLongSound() ? 0.7 : 1.0;
}

double structFormantPathEditor :: v_getBottomOfSoundAndAnalysisArea () {
	return 0.0;  //our textgrid ? 0.3 : 0.0;
}

/********** EXPORTED **********/

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound sound, TextGrid textgrid) {
	try {
		autoFormantPathEditor me = Thing_new (FormantPathEditor);
		my data = formantPath;
		if (sound)
			my soundArea = SoundArea_create (me.get(), sound, true);
		if (textgrid)
			;   // BUG: implement TextGridArea
		FunctionEditor_init (me.get(), title);
		my d_formant = FormantPath_extractFormant (formantPath);
		//if (textgrid)
		//	my textgrid = Data_copy (textgrid);
		if (my instancePref_modeler_numberOfParametersPerTrack() [0] == U'\0')
			my setInstancePref_modeler_numberOfParametersPerTrack (my default_modeler_numberOfParametersPerTrack());
		if (my instancePref_formant_default_colour() [0] == U'\0')
			my setInstancePref_formant_default_colour (my default_formant_default_colour ());
		if (my instancePref_formant_path_oddColour() [0] == U'\0')
			my setInstancePref_formant_path_oddColour (my default_formant_path_oddColour ());
		if (my instancePref_formant_path_evenColour() [0] == U'\0')
			my setInstancePref_formant_path_evenColour (my default_formant_path_evenColour ());
		if (my instancePref_formant_selected_colour() [0] == U'\0')
			my setInstancePref_formant_selected_colour (my default_formant_selected_colour ());
		my selectedTier = 1;
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
