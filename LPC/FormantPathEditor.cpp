/* FormantPathEditor.cpp
 *
 * Copyright (C) 2020 David Weenink
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
	TODO: 28/5/20
	1. claim somewhat more space for the selection viewer
	2. the scroll bar should be shorter, only go to the end the sound view.
	3. navigation aid buttons? next and previous buttons at the side of the navigation tier?
	4. Adapt Select menu and File menu, the navigation aid should have accelerators.
	5. The formant menu can use some logging
	6. ..
*/
#include "FormantPathEditor.h"
#include "FormantPath_to_IntervalTier.h"
#include "EditorM.h"
#include "praat.h"
#include "melder_kar.h"
#include "Sampled.h"
#include "SoundEditor.h"
#include "Sound_and_MixingMatrix.h"
#include "Sound_and_Spectrogram.h"
#include "TextGrid_Sound.h"
#include "TextGrid_extensions.h"

Thing_implement (FormantPathEditor, TimeSoundAnalysisEditor, 0);

#include "prefs_define.h"
#include "FormantPathEditor_prefs.h"
#include "prefs_install.h"
#include "FormantPathEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "FormantPathEditor_prefs.h"

void structFormantPathEditor :: v_info () {
	FormantPathEditor_Parent :: v_info ();
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
	bool navigationPossible = ( navigator && IntervalTierNavigator_isNavigationPossible (navigator) &&
		our pathGridView && TextGridView_hasTierInView (our pathGridView.get(), formantPath -> navigationTierNumber) );
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

/********** UTILITIES **********/

static double _FormantPathEditor_computeSoundY (FormantPathEditor me) {
	/*
		We want half of the screen for the spectrogram. 3/8 for the sound and 1/8 for the textgrid
	*/
	return (my d_longSound.data || my d_sound.data) ? 0.7 : 1.0;
}

static void FormantPathEditor_getDrawingData (FormantPathEditor me, double *startTime, double *endTime, double *xCursor, double *yCursor) {
	*startTime = my startWindow;
	*endTime = my endWindow;
	if (my startSelection == my endSelection) {
		*startTime = my startWindow;
		*endTime = my endWindow;
		*xCursor = my startSelection;
	} else {
		*startTime = my startSelection;
		*endTime = my endSelection;
		*xCursor = my tmin - 1.0; // don't show
	}
	*yCursor = ( my d_spectrogram_cursor > my p_spectrogram_viewFrom &&
		my d_spectrogram_cursor < my p_spectrogram_viewTo ? my d_spectrogram_cursor : -1000.0 );
}

static void checkTierSelection (FormantPathEditor me, conststring32 verbPhrase) {
	if (my selectedTier < 1 || my selectedTier > my pathGridView -> tiers -> size)
		Melder_throw (U"To ", verbPhrase, U", first select a tier by clicking anywhere inside it.");
}

static void scrollToView (FormantPathEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow), true);
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow), true);
	} else {
		FunctionEditor_marksChanged (me, true);
	}
}

/********** METHODS **********/

/*
 * The main invariant of the FormantPathEditor is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */

/***** FILE MENU *****/

static void menu_cb_ExtractSelectedTextGrid_preserveTimes (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	if (my endSelection <= my startSelection)
		Melder_throw (U"No selection.");
	autoTextGrid grid = TextGridView_to_TextGrid (my pathGridView.get());
	autoTextGrid extract = TextGrid_extractPart (grid.get(), my startSelection, my endSelection, true);
	Editor_broadcastPublication (me, extract.move());
}

static void menu_cb_ExtractSelectedTextGrid_timeFromZero (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	if (my endSelection <= my startSelection)
		Melder_throw (U"No selection.");
	autoTextGrid grid = TextGridView_to_TextGrid (my pathGridView.get());
	autoTextGrid extract = TextGrid_extractPart (grid.get(), my startSelection, my endSelection, false);
	Editor_broadcastPublication (me, extract.move());
}

void structFormantPathEditor :: v_createMenuItems_file_extract (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_file_extract (menu);
	extractSelectedTextGridPreserveTimesButton =
		EditorMenu_addCommand (menu, U"Extract selected TextGrid (preserve times)", 0, menu_cb_ExtractSelectedTextGrid_preserveTimes);
	extractSelectedTextGridTimeFromZeroButton =
		EditorMenu_addCommand (menu, U"Extract selected TextGrid (time from 0)", 0, menu_cb_ExtractSelectedTextGrid_timeFromZero);
}

static void menu_cb_WriteToTextFile (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save as TextGrid text file", nullptr)
		Melder_sprint (defaultName,300, my pathGridView -> name.get(), U".TextGrid");
	EDITOR_DO_SAVE
		autoTextGrid grid = TextGridView_to_TextGrid (my pathGridView.get());
		Data_writeToTextFile (grid.get(), file);
	EDITOR_END
}

void structFormantPathEditor :: v_createMenuItems_file_write (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_file_write (menu);
	EditorMenu_addCommand (menu, U"Save TextGrid as text file...", 'S', menu_cb_WriteToTextFile);
}

static void menu_cb_DrawVisibleTextGrid (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish ())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my pref_picture_garnish ())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = garnish;
		Editor_openPraatPicture (me);
		TextGrid_Sound_draw (my pathGridView.get(), nullptr, my pictureGraphics, my startWindow, my endWindow, true, my p_useTextStyles,
			my pref_picture_garnish ());
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleSoundAndTextGrid (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible sound and TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish ())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my pref_picture_garnish ())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = garnish;
		Editor_openPraatPicture (me);
		{// scope
			autoSound sound = my d_longSound.data ?
				LongSound_extractPart (my d_longSound.data, my startWindow, my endWindow, true) :
				Sound_extractPart (my d_sound.data, my startWindow, my endWindow,
					kSound_windowShape::RECTANGULAR, 1.0, true);
			TextGrid_Sound_draw (my pathGridView.get(), sound.get(), my pictureGraphics,
				my startWindow, my endWindow, true, my p_useTextStyles, my pref_picture_garnish ());
		}
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

void structFormantPathEditor :: v_createMenuItems_file_draw (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, U"Draw visible TextGrid...", 0, menu_cb_DrawVisibleTextGrid);
	if (d_sound.data || d_longSound.data)
		EditorMenu_addCommand (menu, U"Draw visible sound and TextGrid...", 0, menu_cb_DrawVisibleSoundAndTextGrid);
}

/***** QUERY MENU *****/
#if 0
static void menu_cb_GetStartingPointOfInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const TextGrid grid = my pathGridView.get();
	checkTierSelection (me, U"query the starting point of an interval");
	const Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const double time = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmin );
		Melder_informationReal (time, U"seconds");
	} else {
		Melder_throw (U"The selected tier is not an interval tier.");
	}
}

static void menu_cb_GetEndPointOfInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const TextGrid grid = my pathGridView.get();
	checkTierSelection (me, U"query the end point of an interval");
	const Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const double time = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmax );
		Melder_informationReal (time, U"seconds");
	} else {
		Melder_throw (U"The selected tier is not an interval tier.");
	}
}

static void menu_cb_GetLabelOfInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const TextGrid grid = my pathGridView.get();
	checkTierSelection (me, U"query the label of an interval");
	const Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const conststring32 label = ( iinterval < 1 || iinterval > tier -> intervals.size ? U"" :
				tier -> intervals.at [iinterval] -> text.get() );
		Melder_information (label);
	} else {
		Melder_throw (U"The selected tier is not an interval tier.");
	}
}
#endif
/***** PITCH MENU *****/

static void menu_cb_DrawTextGridAndPitch (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw TextGrid and Pitch separately", nullptr)
		my v_form_pictureWindow (cmd);
		LABEL (U"TextGrid:")
		BOOLEAN (showBoundariesAndPoints, U"Show boundaries and points", my default_picture_showBoundaries ());
		LABEL (U"Pitch:")
		BOOLEAN (speckle, U"Speckle", my default_picture_pitch_speckle ());
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (showBoundariesAndPoints, my pref_picture_showBoundaries ())
		SET_BOOLEAN (speckle, my pref_picture_pitch_speckle ())
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my pref_picture_garnish ())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my pref_picture_showBoundaries () = showBoundariesAndPoints;
		my pref_picture_pitch_speckle () = speckle;
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = garnish;
		if (! my p_pitch_show)
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			Melder_require (my d_pitch,
				U"Cannot compute pitch.");
		}
		Editor_openPraatPicture (me);
		double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(), my p_pitch_floor, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(), my p_pitch_ceiling, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		double pitchFloor_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		double pitchCeiling_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) my p_pitch_unit);
		double pitchViewFrom_overt = ( my p_pitch_viewFrom < my p_pitch_viewTo ? my p_pitch_viewFrom : pitchFloor_overt );
		double pitchViewTo_overt = ( my p_pitch_viewFrom < my p_pitch_viewTo ? my p_pitch_viewTo : pitchCeiling_overt );
		TextGrid_Pitch_drawSeparately (my pathGridView.get(), my d_pitch.get(), my pictureGraphics, my startWindow, my endWindow,
			pitchViewFrom_overt, pitchViewTo_overt, showBoundariesAndPoints, my p_useTextStyles, garnish,
			speckle, my p_pitch_unit
		);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** SEARCH MENU *****/

static void findInTier (FormantPathEditor me) {
	const TextGrid grid =  my pathGridView.get();
	checkTierSelection (me, U"find a text");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		integer iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			conststring32 text = interval -> text.get();
			if (text) {
				const char32 *position = str32str (text, my findString.get());
				if (position) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position - text, position - text + str32len (my findString.get()));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		integer ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection) + 1;
		while (ipoint <= tier -> points.size) {
			const TextPoint point = tier->points.at [ipoint];
			conststring32 text = point -> mark.get();
			if (text) {
				const char32 * const position = str32str (text, my findString.get());
				if (position) {
					my startSelection = my endSelection = point -> number;
					scrollToView (me, point -> number);
					GuiText_setSelection (my text, position - text, position - text + str32len (my findString.get()));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void do_find (FormantPathEditor me) {
	if (my findString) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		const char32 * const position = str32str (& label [right], my findString.get());   // CRLF BUG?
		if (position) {
			GuiText_setSelection (my text, position - label.get(), position - label.get() + str32len (my findString.get()));
		} else {
			findInTier (me);
		}
	}
}

static void menu_cb_Find (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find text", nullptr)
		TEXTFIELD (findString, U"Text:", U"")
	EDITOR_OK
	EDITOR_DO
		my findString = Melder_dup (findString);
		do_find (me);
	EDITOR_END
}

static void menu_cb_FindAgain (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_find (me);
}

/***** TIER MENU *****/


static void menu_cb_candidate_modellingSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Candidate modelling settings", U"Candidate modelling settings...")		
		SENTENCE (parameters_string, U"Coefficients by track", my default_modeler_numberOfParametersPerTrack ())
		POSITIVE (varianceExponent, U"Variance exponent", U"1.25")
	EDITOR_OK
		SET_STRING (parameters_string, my p_modeler_numberOfParametersPerTrack)
	EDITOR_DO
	pref_str32cpy2 (my pref_modeler_numberOfParametersPerTrack (), my p_modeler_numberOfParametersPerTrack, parameters_string);
	my pref_modeler_varianceExponent () = my p_modeler_varianceExponent = varianceExponent;
	FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_AdvancedCandidateDrawingSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant modeler advanced drawing settings", nullptr)
		BOOLEAN (drawEstimatedModels, U"Draw estimated models", my default_modeler_draw_estimatedModels ())
		POSITIVE (yGridLineEvery_Hz, U"Hor. grid lines every (Hz)", my default_modeler_draw_yGridLineEvery_Hz ())
		POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", my default_modeler_draw_maximumFrequency ())
		BOOLEAN (drawErrorBars, U"Draw error bars", my default_modeler_draw_showErrorBars ())
	EDITOR_OK
		SET_BOOLEAN (drawEstimatedModels, my p_modeler_draw_estimatedModels)
		SET_REAL (yGridLineEvery_Hz, my p_modeler_draw_yGridLineEvery_Hz)
		SET_REAL (maximumFrequency, my p_modeler_draw_maximumFrequency)
		SET_BOOLEAN (drawErrorBars, my p_modeler_draw_showErrorBars)
	EDITOR_DO
		my pref_modeler_draw_estimatedModels () = my p_modeler_draw_estimatedModels = drawEstimatedModels;
		my pref_modeler_draw_maximumFrequency () = my p_modeler_draw_maximumFrequency = maximumFrequency;
		my pref_modeler_draw_yGridLineEvery_Hz () = my p_modeler_draw_yGridLineEvery_Hz = yGridLineEvery_Hz;
		my pref_modeler_draw_showErrorBars () = my p_modeler_draw_showErrorBars = drawErrorBars;
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
		SENTENCE (parameters_string, U"Coefficients by track", U"3 3 3 3")
		POSITIVE (powerf, U"Power", U"1.25")
	EDITOR_OK
	EDITOR_DO
		FormantPath formantPath = (FormantPath) my data;
		autoINTVEC parameters = newINTVECfromString (parameters_string);
		FormantPath_pathFinder (formantPath, qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight, intensityModulationStepSize, windowLength, parameters.get(), powerf);
		my d_formant = FormantPath_extractFormant (formantPath);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleCandidates (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible candidates", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		BOOLEAN (crossHairs, U"Draw cross hairs", 0)
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		SET_BOOLEAN (garnish, my pref_picture_garnish ())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my pref_picture_garnish () = garnish;
		Editor_openPraatPicture (me);
		FormantPath formantPath = (FormantPath) my data;
		Graphics_setInner (my pictureGraphics);
		double startTime, endTime, xCursor, yCursor;
		FormantPathEditor_getDrawingData (me, & startTime, & endTime, & xCursor, & yCursor);

		
		autoINTVEC parameters = newINTVECfromString (my p_modeler_numberOfParametersPerTrack);
		constexpr double xSpace_fraction = 0.1, ySpace_fraction = 0.1;
		FormantPath_drawAsGrid_inside (formantPath, my pictureGraphics, startTime, endTime, my p_modeler_draw_maximumFrequency, 1, 5, my p_modeler_draw_showErrorBars, Melder_RED, Melder_PURPLE, 0, 0, xSpace_fraction, ySpace_fraction, my p_modeler_draw_yGridLineEvery_Hz, xCursor, yCursor, markedCandidatesColour, parameters.get(), true, true, my p_modeler_varianceExponent, my p_modeler_draw_estimatedModels, true);
		Graphics_unsetInner (my pictureGraphics);
		Editor_closePraatPicture (me);	
	EDITOR_END
}

static void menu_cb_FormantColourSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant colour settings", nullptr)
		WORD (oddPathColour_string, U"Dots in F1, F3, F5", my default_formant_path_oddColour ())
		WORD (evenPathColour_string, U"Dots in F2, F4", my default_formant_path_evenColour ())
	EDITOR_OK
		SET_STRING (oddPathColour_string, my p_formant_path_oddColour)
		SET_STRING (evenPathColour_string, my p_formant_path_evenColour)
	EDITOR_DO
		pref_str32cpy2 (my pref_formant_path_oddColour (), my p_formant_path_oddColour, oddPathColour_string);
		pref_str32cpy2 (my pref_formant_path_evenColour (), my p_formant_path_evenColour, evenPathColour_string);
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
		SET_BOOLEAN (garnish, my p_formant_picture_garnish)
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_formant_picture_garnish () = my p_formant_picture_garnish = garnish;
		if (! my p_formant_show)
			Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formant\" from the Formant menu.");
		Editor_openPraatPicture (me);
		//FormantPath formantPath = (FormantPath) my data;
		//const Formant formant = formantPath -> formant.get();
		//const Formant defaultFormant = formantPath -> formants.at [formantPath -> defaultFormant];
		Formant_drawSpeckles (my d_formant.get(), my pictureGraphics, my startWindow, my endWindow,
			my p_spectrogram_viewTo, my p_formant_dynamicRange,
			my p_formant_picture_garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}


static void menu_cb_showFormants (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	my pref_formant_show () = my p_formant_show = ! my p_formant_show;
	GuiMenuItem_check (my formantToggle, my p_formant_show);   // in case we're called from a script
	FunctionEditor_redraw (me);
}

void structFormantPathEditor :: v_createMenuItems_formant (EditorMenu menu) {
	formantToggle = EditorMenu_addCommand (menu, U"Show formants",
		GuiMenu_CHECKBUTTON | (pref_formant_show () ? GuiMenu_TOGGLE_ON : 0), menu_cb_showFormants);
	EditorMenu_addCommand (menu, U"Formant colour settings...", 0, menu_cb_FormantColourSettings);
	EditorMenu_addCommand (menu, U"Draw visible formant contour...", 0, menu_cb_DrawVisibleFormantContour);
}

/***** HELP MENU *****/

static void menu_cb_FormantPathEditorHelp (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"FormantPathEditor"); }
static void menu_cb_AboutSpecialSymbols (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Special symbols"); }
static void menu_cb_PhoneticSymbols (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Phonetic symbols"); }
static void menu_cb_AboutTextStyles (FormantPathEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Text styles"); }

void structFormantPathEditor :: v_createMenus () {
	FormantPathEditor_Parent :: v_createMenus ();
	EditorMenu menu;
	Editor_addCommand (this, U"Edit", U"-- search --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Find...", 'F', menu_cb_Find);
	Editor_addCommand (this, U"Edit", U"Find again", 'G', menu_cb_FindAgain);

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

	if (our d_sound.data || our d_longSound.data) {
		if (our v_hasAnalysis ())
			our v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the TextGrid menus
	}
	menu = Editor_addMenu (this, U"Candidates", 0);
	EditorMenu_addCommand (menu, U"Candidate modelling settings...", 0, menu_cb_candidate_modellingSettings);
	EditorMenu_addCommand (menu, U"Advanced candidate drawing settings...", 0, menu_cb_AdvancedCandidateDrawingSettings);
	EditorMenu_addCommand (menu, U" -- drawing -- ", 0, 0);
	EditorMenu_addCommand (menu, U"Find path...", 0, menu_cb_candidates_FindPath);
	EditorMenu_addCommand (menu, U"Draw visible candidates...", 0, menu_cb_DrawVisibleCandidates);
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
	const TextGrid grid = our pathGridView.get();
	/*
		Perform a minimal selection change.
		Most changes will involve intervals and boundaries; however, there may also be tier removals.
		Do a simple guess.
	*/
	if (our selectedTier > grid -> tiers->size)
		our selectedTier = grid -> tiers->size;
	our v_updateMenuItems_navigation ();
	FormantPathEditor_Parent :: v_dataChanged ();   // does all the updating
}

/********** DRAWING AREA **********/

void structFormantPathEditor :: v_prepareDraw () {
	if (our d_longSound.data) {
		try {
			LongSound_haveWindow (our d_longSound.data, our startWindow, our endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void structFormantPathEditor :: v_draw () {
	Graphics_Viewport vp1;
	const bool showAnalysis = v_hasAnalysis () &&
			(p_spectrogram_show || p_pitch_show || p_intensity_show || p_formant_show) &&
			(d_longSound.data || d_sound.data);
	double soundY = _FormantPathEditor_computeSoundY (this);

	/*
		Draw the sound.
	*/
	if (d_longSound.data || d_sound.data) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		Graphics_resetViewport (our graphics.get(), vp1);
	}

	/*
		Draw tiers.
	*/
	if (our textgrid) {}
	if (showAnalysis) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, (our textgrid ? 0.3 : 0.0), soundY);
		v_draw_analysis ();
		Graphics_resetViewport (our graphics.get(), vp1);
		/* Draw pulses. */
		if (p_pulses_show) {
			vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY, 1.0);
			v_draw_analysis_pulses ();
			TimeSoundEditor_drawSound (this, -1.0, 1.0);   // second time, partially across the pulses
			Graphics_resetViewport (our graphics.get(), vp1);
		}
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	/*if (our d_longSound.data || our d_sound.data) {
		Graphics_line (our graphics.get(), our startWindow, soundY, our endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (our graphics.get(), our startWindow, soundY2, our endWindow, soundY2);
			Graphics_line (our graphics.get(), our startWindow, soundY, our startWindow, soundY2);
			Graphics_line (our graphics.get(), our endWindow, soundY, our endWindow, soundY2);
		}
	}*/

	/*
		Finally, us usual, update the menus.
	*/
	v_updateMenuItems_file ();
	v_updateMenuItems_navigation ();
}

void structFormantPathEditor :: v_drawSelectionViewer () {
	static double previousStartTime, previousEndTime;
	constexpr double xSpace_fraction = 0.1, ySpace_fraction = 0.1;
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFontSize (our graphics.get(), 10.0);
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
	double startTime, endTime = endWindow, xCursor, yCursor;
	FormantPathEditor_getDrawingData (this, & startTime, & endTime, & xCursor, & yCursor);
	Graphics_setInner (our graphics.get());
	FormantPath formantPath = (FormantPath) our data;
	const integer nrow = 0, ncol = 0;
	if (startTime != previousStartTime || endTime != previousEndTime)
		our selectedCandidate = 0;
	autoINTVEC parameters = newINTVECfromString (our p_modeler_numberOfParametersPerTrack);
	MelderColour oddColour = MelderColour_fromColourName (our p_formant_path_oddColour);
	MelderColour evenColour = MelderColour_fromColourName (our p_formant_path_evenColour);
	FormantPath_drawAsGrid_inside (formantPath, our graphics.get(), startTime, endTime, 
		our p_modeler_draw_maximumFrequency, 1, 5, our p_modeler_draw_showErrorBars, oddColour, evenColour, nrow, ncol, xSpace_fraction, ySpace_fraction, our p_modeler_draw_yGridLineEvery_Hz, xCursor, yCursor, markedCandidatesColour, parameters.get(), true, true, our p_modeler_varianceExponent, our p_modeler_draw_estimatedModels, true);
	Graphics_unsetInner (our graphics.get());
	previousStartTime = startTime;
	previousEndTime = endTime;
}

static void FormantPathEditor_drawCeilings (FormantPathEditor me, Graphics g, double tmin, double tmax, double fmin, double fmax) {
	FormantPath formantPath = (FormantPath) my data;
	autoIntervalTier intervalTier = FormantPath_to_IntervalTier (formantPath, tmin, tmax);
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BASELINE);
	Graphics_setColour (g, Melder_RED);
	Graphics_setLineWidth (g, 3.0);
	for (integer interval = 1; interval <= intervalTier -> intervals.size; interval ++) {
		TextInterval textInterval = intervalTier -> intervals.at [interval];
		conststring32 label = textInterval -> text.get();
		if (label) {
			const integer index = Melder_atoi (label);
			if (index > 0 && index <= formantPath -> ceilings.size) {
				const double ceiling = formantPath -> ceilings [index];
				Graphics_line (g, textInterval -> xmin, ceiling, textInterval -> xmax, ceiling);
				Graphics_text (g, 0.5 * (textInterval -> xmin + textInterval -> xmax), ceiling + 50.0, Melder_fixed (ceiling, 0));
			}
		}
	}
	Graphics_setLineWidth (g, 1.0);
}

void structFormantPathEditor :: v_draw_analysis_formants () {
	if (our p_formant_show) {
		Graphics_setColour (our graphics.get(), Melder_RED);
		Graphics_setSpeckleSize (our graphics.get(), our p_formant_dotSize);
		MelderColour oddColour = MelderColour_fromColourName (our p_formant_path_oddColour);
		MelderColour evenColour = MelderColour_fromColourName (our p_formant_path_evenColour);
	
		Formant_drawSpeckles_inside (d_formant.get(), our graphics.get(), our startWindow, our endWindow, our p_spectrogram_viewFrom, our p_spectrogram_viewTo, our p_formant_dynamicRange, oddColour, evenColour, true);
		Graphics_setColour (our graphics.get(), Melder_PINK);
		FormantPathEditor_drawCeilings (this, our graphics.get(), our startWindow, our endWindow,
			our p_spectrogram_viewFrom, our p_spectrogram_viewTo);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
	}
}

static void Formant_replaceFrames (Formant target, integer beginFrame, integer endFrame, Formant source) {
	// Precondition target and source have exactly the same Sampled xmin, xmax, x1, nx, dx
	if (beginFrame == endFrame && beginFrame == 0) {
		beginFrame = 1;
		endFrame = target->nx;
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
	FormantPath formantPath = (FormantPath) our data;
	integer numberOfRows, numberOfColums;
	NUMgetGridDimensions (formantPath -> formants.size, & numberOfRows, & numberOfColums);
	const integer icol = 1 + (int) (xWC * numberOfColums);
	if (icol < 1 || icol > numberOfColums)
		return;
	const integer irow = 1 + (int) ((1.0 - yWC) * numberOfRows);
	if (irow < 1 || irow > numberOfRows)
		return;
	integer index = (irow - 1) * numberOfColums + icol; // left-to-right, top-to-bottom
	if (index > 0 && index <= formantPath -> formants.size) {
		double tmin_ = our startWindow, tmax_ = our endWindow;
		if (our startSelection < our endSelection) {
			tmin_ = our startSelection;
			tmax_ = our endSelection;
		}
		our selectedCandidate = index;
		Editor_save (this, U"insert interval by selection viewer");
		integer itmin, itmax;
		Sampled_getWindowSamples (formantPath, tmin_, tmax_, & itmin, & itmax);
		for (integer iframe = itmin; iframe <= itmax; iframe ++)
			formantPath -> path [iframe] = our selectedCandidate;
		Formant source = reinterpret_cast<Formant> (formantPath -> formants.at [our selectedCandidate]);
		Formant_replaceFrames (d_formant.get(), itmin, itmax, source);
	}
}

void structFormantPathEditor :: v_play (double tmin_, double tmax_) {
	if (! d_sound.data && ! d_longSound.data)
		return;
	integer numberOfChannels = ( d_longSound.data ? d_longSound.data -> numberOfChannels : d_sound.data -> ny );
	integer numberOfMuteChannels = 0;
	Melder_assert (our d_sound.muteChannels.size == numberOfChannels);
	for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
		if (our d_sound.muteChannels [ichan])
			numberOfMuteChannels ++;
	integer numberOfChannelsToPlay = numberOfChannels - numberOfMuteChannels;
	Melder_require (numberOfChannelsToPlay > 0,
		U"Please select at least one channel to play.");
	if (our d_longSound.data) {
		if (numberOfMuteChannels > 0) {
			autoSound part = LongSound_extractPart (our d_longSound.data, tmin_, tmax_, true);
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our d_sound.muteChannels.get());
			Sound_MixingMatrix_playPart (part.get(), thee.get(), tmin_, tmax_, theFunctionEditor_playCallback, this);
		} else {
			LongSound_playPart (our d_longSound.data, tmin_, tmax_, theFunctionEditor_playCallback, this);
		}
	} else {
		if (numberOfMuteChannels > 0) {
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our d_sound.muteChannels.get());
			Sound_MixingMatrix_playPart (our d_sound.data, thee.get(), tmin_, tmax_, theFunctionEditor_playCallback, this);
		} else {
			Sound_playPart (our d_sound.data, tmin_, tmax_, theFunctionEditor_playCallback, this);
		}
	}
}

POSITIVE_VARIABLE (v_prefs_addFields_fontSize)
OPTIONMENU_ENUM_VARIABLE (kGraphics_horizontalAlignment, v_prefs_addFields_textAlignmentInIntervals)
OPTIONMENU_VARIABLE (v_prefs_addFields_useTextStyles)
OPTIONMENU_ENUM_VARIABLE (kTextGridEditor_showNumberOf, v_prefs_addFields_showNumberOf)
void structFormantPathEditor :: v_prefs_addFields (EditorCommand cmd) {
	UiField _radio_;
	POSITIVE_FIELD (v_prefs_addFields_fontSize, U"Font size (points)", our default_fontSize ())
	OPTIONMENU_ENUM_FIELD (kGraphics_horizontalAlignment, v_prefs_addFields_textAlignmentInIntervals,
			U"Text alignment in intervals", kGraphics_horizontalAlignment::DEFAULT)
	OPTIONMENU_FIELD (v_prefs_addFields_useTextStyles, U"The symbols %#_^ in labels", our default_useTextStyles () + 1)
		OPTION (U"are shown as typed")
		OPTION (U"mean italic/bold/sub/super")
	OPTIONMENU_ENUM_FIELD (kTextGridEditor_showNumberOf, v_prefs_addFields_showNumberOf,
			U"Show number of", kTextGridEditor_showNumberOf::DEFAULT)
}
void structFormantPathEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_OPTION (v_prefs_addFields_useTextStyles, our p_useTextStyles + 1)
	SET_REAL (v_prefs_addFields_fontSize, our p_fontSize)
	SET_ENUM (v_prefs_addFields_textAlignmentInIntervals, kGraphics_horizontalAlignment, our p_alignment)
	SET_ENUM (v_prefs_addFields_showNumberOf, kTextGridEditor_showNumberOf, our p_showNumberOf)
}

void structFormantPathEditor :: v_prefs_getValues (EditorCommand /* cmd */) {
	our pref_useTextStyles () = our p_useTextStyles = v_prefs_addFields_useTextStyles - 1;
	our pref_fontSize () = our p_fontSize = v_prefs_addFields_fontSize;
	our pref_alignment () = our p_alignment = v_prefs_addFields_textAlignmentInIntervals;
	our pref_shiftDragMultiple () = our p_shiftDragMultiple = false;
	our pref_showNumberOf () = our p_showNumberOf = v_prefs_addFields_showNumberOf;
	FunctionEditor_redraw (this);
}

void structFormantPathEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_view_timeDomain (menu);
}

void structFormantPathEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our v_hasAnalysis () && our p_spectrogram_show && (our d_longSound.data || our d_sound.data)) {
		const double soundY = _FormantPathEditor_computeSoundY (this);
		Graphics_highlight (our graphics.get(), left, right, bottom+(top-bottom)*soundY, top);
	} else {
		Graphics_highlight (our graphics.get(), left, right, bottom, top);
	}
}

double structFormantPathEditor :: v_getBottomOfSoundArea () {
	return _FormantPathEditor_computeSoundY (this);
}

double structFormantPathEditor :: v_getBottomOfSoundAndAnalysisArea () {
	return (our textgrid ? 0.3 : 0.0);
}

void structFormantPathEditor :: v_createMenuItems_pitch_picture (EditorMenu menu) {
	FormantPathEditor_Parent :: v_createMenuItems_pitch_picture (menu);
	EditorMenu_addCommand (menu, U"Draw visible pitch contour and TextGrid...", 0, menu_cb_DrawTextGridAndPitch);
}

void structFormantPathEditor :: v_updateMenuItems_file () {
	FormantPathEditor_Parent :: v_updateMenuItems_file ();
	GuiThing_setSensitive (extractSelectedTextGridPreserveTimesButton, our endSelection > our startSelection);
	GuiThing_setSensitive (extractSelectedTextGridTimeFromZeroButton,  our endSelection > our startSelection);
}

/********** EXPORTED **********/

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound sound, TextGrid textgrid) {
	try {
		autoFormantPathEditor me = Thing_new (FormantPathEditor);
		
		TimeSoundAnalysisEditor_init (me.get(), title, formantPath, sound, false);
		my d_formant = FormantPath_extractFormant (formantPath);
		if (textgrid) {
			my textgrid = Data_copy (textgrid);
			my pathGridView = TextGridView_create (my textgrid.get());
		}
		if (my p_modeler_numberOfParametersPerTrack [0] == U'\0')
			pref_str32cpy2(my p_modeler_numberOfParametersPerTrack, my pref_modeler_numberOfParametersPerTrack (), my default_modeler_numberOfParametersPerTrack ());
		if (my p_formant_default_colour [0] == U'\0')
			pref_str32cpy2 (my p_formant_default_colour, my pref_formant_default_colour (), my default_formant_default_colour ());
		if (my p_formant_path_oddColour [0] == U'\0')
			pref_str32cpy2 (my p_formant_path_oddColour, my pref_formant_path_oddColour (), my default_formant_path_oddColour ());
		if (my p_formant_path_evenColour [0] == U'\0')
			pref_str32cpy2 (my p_formant_path_evenColour, my pref_formant_path_evenColour (), my default_formant_path_evenColour ());
		if (my p_formant_selected_colour [0] == U'\0')
			pref_str32cpy2 (my p_formant_selected_colour, my pref_formant_selected_colour (), my default_formant_selected_colour ());
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
