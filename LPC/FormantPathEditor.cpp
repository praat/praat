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
#include "EditorM.h"
#include "praat.h"
#include "melder_kar.h"
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

// forward definitions
static void insertBoundaryOrPoint (FormantPathEditor me, integer itier, double t1, double t2, bool insertSecond);
// end forward


void structFormantPathEditor :: v_info () {
	FormantPathEditor_Parent :: v_info ();
}

void structFormantPathEditor :: v_updateMenuItems_navigation () {
	FormantPath formantPath = (FormantPath) our data;
	IntervalTierNavigator navigator = formantPath -> intervalTierNavigator.get();
	bool navigationPossible = ( navigator && IntervalTierNavigator_isNavigationPossible (navigator) &&
		our pathGridView && TextGridView_hasTierInView (our pathGridView.get(), formantPath -> navigationTierNumber) );
	bool nextSensitive = false;
	bool previousSensitive = false;
	if (navigationPossible) {
		if (IntervalTierNavigator_getPreviousMatchingIntervalNumber (navigator, our startSelection) > 0)
			previousSensitive = true;
		if (IntervalTierNavigator_getNextMatchingIntervalNumber (navigator, our endSelection) > 0)
			nextSensitive = true;
	}
	GuiThing_setSensitive (our navigateSettingsButton, navigationPossible);
	GuiThing_setSensitive (our navigateNextButton, nextSensitive);
	GuiThing_setSensitive (our navigatePreviousButton, previousSensitive);
}

void operator<<= (BOOLVECVU const& target, bool value) {
	for (integer i = 1; i <= target.size; i ++)
		target [i] = value;
}

void operator<<= (INTVECVU const& target, integer value) {
	for (integer i = 1; i <= target.size; i ++)
		target [i] = value;
}

void FormantPathEditor_setTierOrder (FormantPathEditor me, conststring32 tierNumber_string) {
	FormantPath formantPath = (FormantPath) my data;
	bool pathTierNumberFound = false;
	autoSTRVEC tokens = newSTRVECtokenize (tierNumber_string);
	autoINTVEC tierNumbers;
	const integer pathTierNumber =  formantPath -> pathTierNumber;
	for (integer itoken = 1; itoken <= tokens.size; itoken ++) {
		integer value;
		if (Melder_equ_caseInsensitive (tokens [itoken].get(), U"P")) {
			value = pathTierNumber;
			pathTierNumberFound = true;
		} else
			value = Melder_atoi (tokens [itoken].get());
		integer *next = tierNumbers.append ();
		*next = value;
	}
	Melder_require (tierNumbers.size > 0,
		U"There should at least be one tier number present.");
	/*
		The pathTier should always be present
	*/
	if (! pathTierNumberFound) {
		for (integer inum = 1; inum <= tierNumbers.size; inum ++)
			if (tierNumbers [inum] == pathTierNumber) {
				pathTierNumberFound = true;
				break;
			}
		Melder_require (pathTierNumberFound,
			U"The path tier number (", pathTierNumber, U") should be present.");
	}
	/*
		Rest of checks is done in the following routine
	*/
	const integer selectedTier = TextGridView_getOriginTierNumber (my pathGridView.get(), my selectedTier);
	TextGridView_modifyView (my pathGridView.get(), tierNumbers);
	my selectedTier = TextGridView_getViewTierNumber (my pathGridView.get(), selectedTier);
	my pathTierNumber = TextGridView_getViewTierNumber (my pathGridView.get(), pathTierNumber);
}

void FormantPathEditor_selectAll (FormantPathEditor me) {
	FormantModelerListDrawingSpecification_showAll (my formantModelerList -> drawingSpecification.get());
}

void FormantPathEditor_deselect (FormantPathEditor me) {
	my formantModelerList -> drawingSpecification -> pathModeler = 0;
}

integer FormantPathEditor_selectModelerFromIndexInMatrixGrid (FormantPathEditor me, integer index) {
	FormantModelerListDrawingSpecification drawingSpecification = my formantModelerList -> drawingSpecification.get();
	Melder_require (index > 0 && index <= drawingSpecification->numberOfModelersToDraw,
		U"Oops no match for index =", index, U".");
	return drawingSpecification -> drawingOrder [index];
}

void FormantPathEditor_selectModeler (FormantPathEditor me, integer modelIndex) {
	if (modelIndex > 0 && modelIndex <= my formantModelerList -> numberOfModelers)
		my formantModelerList -> drawingSpecification -> pathModeler = modelIndex;
}

static inline void FormantModelerList_setVarianceExponent (FormantModelerList me, double varianceExponent) {
	my varianceExponent = varianceExponent;
}

inline integer FormantPathEditor_getNumberOfVisible (FormantPathEditor me) {
	return FormantModelerListDrawingSpecification_getNumberOfShown (my formantModelerList -> drawingSpecification.get());
}

autoFormantModelerList FormantPathEditor_to_FormantModelerList (FormantPathEditor me, double startTime, double endTime, conststring32 parameters_string) {
	FormantPath formantPath = (FormantPath) my data;
	autoFormantModelerList thee = FormantPath_to_FormantModelerList (formantPath, startTime, endTime, parameters_string);
	FormantModelerListDrawingSpecification_setModelerColours (thy drawingSpecification.get(), my p_formant_path_colour, my p_formant_default_colour, my p_formant_selected_colour, U"black");
	return thee;
}

autoMelderString FormantModelerList_getSelectedModelParameterString (FormantModelerList me, integer imodel) {
	autoMelderString modelParameters;
	if (imodel > 0) {		
		FormantModeler fm = my formantModelers.at [imodel];
		MelderString_append (& modelParameters, fm -> name.get());
		MelderString_append (& modelParameters, U"; ");
		for (integer itrack = 1; itrack <= fm -> trackmodelers.size; itrack ++) {
			DataModeler track = fm -> trackmodelers.at [itrack];
			MelderString_append (& modelParameters, U" ", Melder_integer (track -> numberOfParameters));
		}
	} else
		MelderString_append (& modelParameters, U"");
	return modelParameters;
}

void FormantPathEditor_markParameterChange (FormantPathEditor me) {
	my formantModelerList -> drawingSpecification -> markOutdated = true;
}

void FormantPathEditor_unmarkParameterChange (FormantPathEditor me) {
	my formantModelerList -> drawingSpecification -> markOutdated = false;
}

bool FormantPathEditor_parametersChanged (FormantPathEditor me, integer imodel, conststring32 label) {
	if (! label || ! label [0])
		return false;
	autoMelderString modelParameters = FormantModelerList_getSelectedModelParameterString (my formantModelerList.get(), imodel);
	return Melder_stringMatchesCriterion (modelParameters.string, kMelder_string::NOT_EQUAL_TO, label, true);
}

/********** UTILITIES **********/

static void do_insertIntervalOnPathTier (FormantPathEditor me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
				my playingCursor || my playingSelection ? my playCursor : my startSelection,
				my playingCursor || my playingSelection ? my playCursor : my endSelection,
				true);
		my selectedTier = itier;
		FunctionEditor_marksChanged (me, true);
		Editor_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Interval not inserted.");
	}
}

bool FormantPathEditor_setPathTierLabel (FormantPathEditor me) {
	/*
		We only modify the interval if the shiftKey has been pressed, otherwise it would be too easy to
		modify an existing label by an accidental click;
	*/
	FormantModelerList fml = my formantModelerList.get();
	integer imodel = my formantModelerList -> drawingSpecification -> pathModeler;
	autoMelderString modelParameters = FormantModelerList_getSelectedModelParameterString (fml, imodel);
	if (modelParameters.string && modelParameters.string [0] && my shiftKeyPressed) {
		IntervalTier pathTier = (IntervalTier) my pathGridView -> tiers -> at [my pathTierNumber];
		if (my startSelection == my endSelection && my startSelection > fml -> xmin && my startSelection < fml -> xmax) {
			my startSelection = fml -> xmin;
			my endSelection = fml -> xmax;
		}
		if ((my startSelection == fml -> xmin && my endSelection == fml -> xmax) ||
			(my startSelection == my endSelection && my startSelection > fml -> xmin && 
			my startSelection < fml -> xmax)) {
			double time = 0.5 * (my startSelection + my endSelection);
			do_insertIntervalOnPathTier (me, my pathTierNumber);
			my startSelection = fml -> xmin;
			my endSelection = fml -> xmax;

			integer intervalNumber = IntervalTier_timeToIndex (pathTier, time);
			TextInterval interval = pathTier -> intervals . at [intervalNumber];
			if (interval -> xmin == fml -> xmin && interval -> xmax == fml -> xmax) {
				TextInterval_setText (interval, modelParameters.string);
				return true;
			}
		}
	}
	return false;
}

static double _FormantPathEditor_computeSoundY (FormantPathEditor me) {
	/*
		We want half of the screen for the spectrogram. 3/8 for the sound and 1/8 for the textgrid
	*/
	return 1.0 / 8.0;
}
static double _FormantPathEditor_computeSoundY2 (FormantPathEditor me) {
	/*
		We want half of the screen for the spectrogram. 3/8 for the sound and 1/8 for the textgrid
	*/
	return 5.0 / 8.0;
}

static void _AnyTier_identifyClass (Function anyTier, IntervalTier *intervalTier, TextTier *textTier) {
	if (anyTier -> classInfo == classIntervalTier) {
		*intervalTier = (IntervalTier) anyTier;
		*textTier = nullptr;
	} else {
		*intervalTier = nullptr;
		*textTier = (TextTier) anyTier;
	}
}

static integer _FormantPathEditor_yWCtoTier (FormantPathEditor me, double yWC) {
	const integer numberOfTiers =  my pathGridView -> tiers -> size;
	const double soundY = _FormantPathEditor_computeSoundY (me);
	integer tierNumber = numberOfTiers - Melder_ifloor (yWC / soundY * (double) numberOfTiers);
	if (tierNumber < 1)
		tierNumber = 1;
	if (tierNumber > numberOfTiers)
		tierNumber = numberOfTiers;
	return tierNumber;
}

static void _FormantPathEditor_timeToInterval (FormantPathEditor me, double t, integer tierNumber,
	double *tmin, double *tmax)
{
	const Function tier = my pathGridView -> tiers->at [tierNumber];
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass (tier, & intervalTier, & textTier);
	if (intervalTier) {
		integer iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval == 0) {
			if (t < my tmin) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals.size;
			}
		}
		Melder_assert (iinterval >= 1);
		Melder_assert (iinterval <= intervalTier -> intervals.size);
		const TextInterval interval = intervalTier -> intervals.at [iinterval];
		*tmin = interval -> xmin;
		*tmax = interval -> xmax;
	} else {
		const integer n = textTier -> points.size;
		if (n == 0) {
			*tmin = my tmin;
			*tmax = my tmax;
		} else {
			integer ipointleft = AnyTier_timeToLowIndex (textTier->asAnyTier(), t);
			*tmin = ipointleft == 0 ? my tmin : textTier -> points.at [ipointleft] -> number;
			*tmax = ipointleft == n ? my tmax : textTier -> points.at [ipointleft + 1] -> number;
		}
	}
	if (*tmin < my tmin)
		*tmin = my tmin;   // clip by FunctionEditor's time domain
	if (*tmax > my tmax)
		*tmax = my tmax;
}

static void checkTierSelection (FormantPathEditor me, conststring32 verbPhrase) {
	if (my selectedTier < 1 || my selectedTier > my pathGridView -> tiers -> size)
		Melder_throw (U"To ", verbPhrase, U", first select a tier by clicking anywhere inside it.");
}

static void checkTierSelection_hard (FormantPathEditor me, conststring32 verbPhrase) {
	checkTierSelection (me, verbPhrase);
	if (my selectedTier != my pathTierNumber)
		Melder_throw (U"To ", verbPhrase, U", first select the path tier.");
}

static integer getSelectedInterval (FormantPathEditor me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my pathGridView -> tiers -> size);
	const IntervalTier tier = (IntervalTier) my pathGridView -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection);
}

static integer getSelectedPoint (FormantPathEditor me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my pathGridView -> tiers -> size);
	const TextTier tier = (TextTier) my pathGridView -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classTextTier);
	return AnyTier_hasPoint (tier->asAnyTier(), my startSelection);
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

/***** VIEW MENU *****/

static void do_selectAdjacentTier (FormantPathEditor me, bool previous) {
	const integer n = my pathGridView -> tiers -> size;
	if (n >= 2) {
		my selectedTier = ( previous ?
				my selectedTier > 1 ? my selectedTier - 1 : n :
				my selectedTier < n ? my selectedTier + 1 : 1 );
		_FormantPathEditor_timeToInterval (me, my startSelection, my selectedTier, & my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_SelectPreviousTier (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, true);
}

static void menu_cb_SelectNextTier (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, false);
}

static void do_selectAdjacentInterval (FormantPathEditor me, bool previous, bool shift) {
	const TextGrid grid = my pathGridView.get();
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers->size)
		return;
	_AnyTier_identifyClass (grid -> tiers->at [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		const integer n = intervalTier -> intervals.size;
		if (n >= 2) {
			integer iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (shift) {
				const integer binterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
				integer einterval = IntervalTier_timeToIndex (intervalTier, my endSelection);
				if (my endSelection == intervalTier -> xmax)
					einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					const TextInterval interval = intervalTier -> intervals.at [iinterval];
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							const TextInterval interval = intervalTier -> intervals.at [einterval - 1];
							my endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						const TextInterval interval = intervalTier -> intervals.at [binterval - 1];
						my startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							const TextInterval interval = intervalTier -> intervals.at [binterval];
							my startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						const TextInterval interval = intervalTier -> intervals.at [einterval];
						my endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = ( previous ?
						iinterval > 1 ? iinterval - 1 : n :
						iinterval < n ? iinterval + 1 : 1 );
				const TextInterval interval = intervalTier -> intervals.at [iinterval];
				my startSelection = interval -> xmin;
				my endSelection = interval -> xmax;
			}
			scrollToView (me, iinterval == n ? my startSelection : iinterval == 1 ? my endSelection : (my startSelection + my endSelection) / 2);
		}
	} else {
		const integer n = textTier -> points.size;
		if (n >= 2) {
			integer ipoint = AnyTier_timeToHighIndex (textTier->asAnyTier(), my startSelection);
			ipoint = ( previous ?
					ipoint > 1 ? ipoint - 1 : n :
					ipoint < n ? ipoint + 1 : 1 );
			const TextPoint point = textTier -> points.at [ipoint];
			my startSelection = my endSelection = point -> number;
			scrollToView (me, my startSelection);
		}
	}
}

static void menu_cb_SelectPreviousInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, false);
}

static void menu_cb_SelectNextInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, false);
}

static void menu_cb_ExtendSelectPreviousInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, true);
}

static void menu_cb_ExtendSelectNextInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, true);
}

static void menu_cb_MoveBtoZero (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing (my d_sound.data, my startSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		Editor_save (me, U"Move start of selection to nearest zero crossing");
		my startSelection = zero;
		if (my startSelection > my endSelection)
			std::swap (my startSelection, my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveCursorToZero (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing (my d_sound.data, 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (isdefined (zero)) {
		Editor_save (me, U"Move cursor to nearest zero crossing");
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveEtoZero (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing (my d_sound.data, my endSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		Editor_save (me, U"Move end of selection to nearest zero crossing");
		my endSelection = zero;
		if (my startSelection > my endSelection)
			std::swap (my startSelection, my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

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

/***** INTERVAL MENU *****/

static void do_removeBoundariesBetween (IntervalTier me, double fromTime, double toTime) {
	if (fromTime == toTime)
		return;
	Melder_assert (fromTime < toTime);
	integer fromIntervalNumber = IntervalTier_timeToLowIndex (me, fromTime);
	integer toIntervalNumber = IntervalTier_timeToHighIndex (me, toTime);
	if (fromIntervalNumber == toIntervalNumber)
		return;
	integer numberOfBoundariesToRemove = toIntervalNumber - fromIntervalNumber;
	for (integer iint = 1; iint <= numberOfBoundariesToRemove; iint ++) {
		IntervalTier_removeLeftBoundary (me, toIntervalNumber --);
	}
	TextInterval_setText (my intervals.at [fromIntervalNumber], U"");
}

static void insertBoundaryOrPoint (FormantPathEditor me, integer itier, double t1, double t2, bool insertSecond) {
	const TextGrid grid = my pathGridView.get();
	const integer numberOfTiers = my pathGridView -> tiers -> size;
	Melder_require (itier == my pathTierNumber,
		U"You are only allowed to modify the path tier.");
	IntervalTier intervalTier = (IntervalTier) grid -> tiers->at [itier];
	Melder_assert (t1 <= t2);
	if (t1 == t2)
		return; // only intervals
	/*
		Policy:
		Insertion of an interval should always occur if the boundaries are within the window.
		"Old" intervals within the new boundaries have to be removed.
		No messages are necesary if new times are on existing boundaries.
	*/
	const bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
	const bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
	do_removeBoundariesBetween (intervalTier, t1, t2);
	if ((t1 == t2 && t1IsABoundary) || (t1IsABoundary && t2IsABoundary))
		return; // no need to do anything more
		
	autoTextInterval rightNewInterval, midNewInterval;
	const integer iinterval = IntervalTier_timeToIndex (intervalTier, t1);
	const integer iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
	Melder_require (iinterval != 0 && iinterval2 != 0,
		U"The selection is outside the time domain of the intervals.");
	const TextInterval interval = intervalTier -> intervals.at [iinterval];

	if (itier == my selectedTier) {
		/*
			Divide up the label text into left, mid and right, depending on where the text selection is.
		*/
		autostring32 text = Melder_dup (interval -> text.get());
		rightNewInterval = TextInterval_create (t2, interval -> xmax, text.get());
		midNewInterval = TextInterval_create (t1, t2, text.get());
		TextInterval_setText (interval, U"");
	} else {
		/*
			Move the text to the left of the boundary.
		*/
		rightNewInterval = TextInterval_create (t2, interval -> xmax, U"");
		midNewInterval = TextInterval_create (t1, t2, U"");
	}
	if (t1IsABoundary) {
		/*
			Merge mid with left interval.
		*/
		if (interval -> xmin != t1)
			Melder_fatal (U"Boundary unequal: ", interval -> xmin, U" versus ", t1, U".");
		interval -> xmax = t2;
		TextInterval_setText (interval, Melder_cat (interval -> text.get(), midNewInterval -> text.get()));
	} else if (t2IsABoundary) {
		/*
			Merge mid and right interval.
		*/
		if (interval -> xmax != t2)
			Melder_fatal (U"Boundary unequal: ", interval -> xmax, U" versus ", t2, U".");
		interval -> xmax = t1;
		Melder_assert (rightNewInterval -> xmin == t2);
		Melder_assert (rightNewInterval -> xmax == t2);
		rightNewInterval -> xmin = t1;
		TextInterval_setText (rightNewInterval.get(), Melder_cat (midNewInterval -> text.get(), rightNewInterval -> text.get()));
	} else {
		interval -> xmax = t1;
		if (t1 != t2)
			intervalTier -> intervals.addItem_move (midNewInterval.move());
	}
	intervalTier -> intervals.addItem_move (rightNewInterval.move());
	if (insertSecond && numberOfTiers >= 2 && t1 == t2) {
		/*
			Find the last time before t on another tier.
		*/
		double tlast = interval -> xmin;
		for (integer jtier = 1; jtier <= numberOfTiers; jtier ++) {
			if (jtier != itier) {
				double tmin, tmax;
				_FormantPathEditor_timeToInterval (me, t1, jtier, & tmin, & tmax);
				if (tmin > tlast)
					tlast = tmin;
			}
		}
		if (tlast > interval -> xmin && tlast < t1) {
			autoTextInterval newInterval = TextInterval_create (tlast, t1, U"");
			interval -> xmax = tlast;
			intervalTier -> intervals.addItem_move (newInterval.move());
		}
	}
	
	my startSelection = my endSelection = t1;
}

static void menu_cb_InsertInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Insert interval");
	do_insertIntervalOnPathTier (me, my pathTierNumber);
}

static integer FormantPathEditor_identifyModelFromIntervalLabel (FormantPathEditor me, conststring32 label) {
	/*
		Find part before ';'
	*/
	autoMelderString formantId;
	integer formantIndex = 0;
	MelderString_copy (& formantId, label);
	char32 *found = str32chr (formantId.string, U';');
	if (found) {
		*found = U'\0';
		formantIndex = FormantPath_identifyFormantIndexByCriterion ((FormantPath) my data, kMelder_string::EQUAL_TO, formantId.string, true);
	}
	return formantIndex;
}

static void menu_cb_ResetTextAndFormants (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	FormantPath formantPath = (FormantPath) (my data);
	if (my selectedTier != my pathTierNumber)
		return;
	if (my startSelection < my endSelection) {
		IntervalTier pathTier = (IntervalTier) my pathGridView -> tiers->at [my selectedTier];
		const double midtime = 0.5 * (my startSelection + my endSelection);
		const integer intervalNumber = IntervalTier_timeToIndex (pathTier, midtime);
		/*
			Identify which formant "belong" to this interval
		*/
		TextInterval textInterval = pathTier -> intervals .at [intervalNumber];
		Editor_save (me, U"Reset text and formants");
		TextInterval_removeText (textInterval);
		FormantPath_replaceFrames (formantPath, my startSelection, my endSelection, formantPath -> defaultFormant);
		FormantPathEditor_deselect (me);
		Editor_broadcastDataChanged (me);
		FunctionEditor_redraw (me);
	}
}

void menu_cb_RemoveInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection || my selectedTier != my pathTierNumber)
		return;
	/*
		We only remove an interval if the text in the left and right intervals are equal.
		The new interval will be longer than the selected one. 
	*/
	IntervalTier pathTier =  reinterpret_cast<IntervalTier> (my pathGridView -> tiers->at [my pathTierNumber]);
	if (pathTier -> intervals.size == 1)
		return;
	integer iinterval = IntervalTier_timeToIndex (pathTier, my startSelection);
	TextInterval left  = pathTier -> intervals.at [std::max (iinterval - 1, 1_integer)];
	TextInterval middle = pathTier -> intervals.at [iinterval];
	TextInterval right = pathTier -> intervals.at [std::min (iinterval + 1, pathTier -> intervals.size)];
	Editor_save (me, U"Remove interval");
	if (iinterval > 1 && iinterval < pathTier -> intervals.size) {
		Melder_require (Melder_equ (left -> text.get(), right -> text.get()),
			U"We cannot remove this interval because its left and right interval texts are not equal.");		
		TextInterval_removeText (middle);
		IntervalTier_removeLeftBoundary (pathTier, iinterval);
	} else if (iinterval == 1) {
		Melder_require (Melder_equ (right -> text.get(), middle -> text.get()) || str32len (right -> text.get()) == 0,
			U"We cannot remove this interval because the right interval text is not empty or differs from the selected interval text.");
		iinterval = 2;
	} else if (iinterval == pathTier -> intervals.size) {
		Melder_require (Melder_equ (left -> text.get(), middle -> text.get()) || str32len (left -> text.get()) == 0,
			U"We cannot remove this interval because the left interval text is not empty or differ from the selected interval text.");
	}
	middle = pathTier -> intervals.at [iinterval];
	TextInterval_removeText (middle);
	IntervalTier_removeLeftBoundary (pathTier, iinterval);
	/*
		The interval has been removed.
		The new interval may be empty or not.
		Empty implies default analysis.
	*/
	iinterval = IntervalTier_timeToIndex (pathTier, my startSelection);
	middle = pathTier -> intervals.at [iinterval];
	integer formantModel = 0;
	FormantPath formantPath = (FormantPath) my data;
	if (middle -> text && middle -> text [0])
		formantModel = FormantPathEditor_identifyModelFromIntervalLabel (me, middle -> text.get());
	if (formantModel == 0)
		formantModel = formantPath -> defaultFormant;
	FormantPath_replaceFrames (formantPath, middle -> xmin, middle -> xmax, formantModel);
	FunctionEditor_marksChanged (me, true);
	Editor_broadcastDataChanged (me);
}

void menu_cb_NavigationSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Navigation settings", nullptr)
		MUTABLE_LABEL (navigationNote, U"")
		OPTIONMENU_ENUM (kMelder_string, navigationCriterion, U"Navigation matching criterion", kMelder_string::DEFAULT)
		MUTABLE_LABEL (leftContextNote, U"")
		OPTIONMENU_ENUM (kMelder_string, leftContextCriterion, U"Left context matching criterion", kMelder_string::DEFAULT)
		MUTABLE_LABEL (rightContextNote, U"")
		OPTIONMENU_ENUM (kMelder_string, rightContextCriterion, U"Right context matching criterion", kMelder_string::DEFAULT)
		MUTABLE_LABEL (contextmatchNote, U"")
		OPTIONMENU_ENUM (kContextCombination, contextCombination, U"Context combination", kContextCombination::DEFAULT)
		BOOLEAN (matchContextOnly, U"Match context only", false)
	EDITOR_OK
		FormantPath formantPath = (FormantPath) my data;
		IntervalTierNavigator navigator = formantPath -> intervalTierNavigator.get();
		autoMelderString note;
		if (navigator -> navigationLabels)
			MelderString_append (& note, U"You have \'", navigator -> navigationLabels -> name.get(), U"\' labels installed.");
		else
			MelderString_append (& note, U"You have NO labels installed.");
		SET_STRING (navigationNote, note . string)
		MelderString_empty (& note);
		if (navigator -> leftContextLabels)
			MelderString_append (& note, U"You have \'", navigator -> leftContextLabels -> name.get(), U"\' left context labels installed.");
		else
			MelderString_append (& note, U"You have NO left context labels installed.");
		SET_STRING (leftContextNote, note . string)
		MelderString_empty (& note);
		if (navigator -> rightContextLabels)
			MelderString_append (& note, U"You have \'", navigator -> rightContextLabels -> name.get(), U"\' right context labels installed.");
		else
			MelderString_append (& note, U"You have NO right context labels installed.");
		SET_STRING (rightContextNote, note . string)
	EDITOR_DO
		FormantPath formantPath = (FormantPath) my data;
		IntervalTierNavigator navigator = formantPath -> intervalTierNavigator.get();
		navigator -> navigationCriterion = navigationCriterion;
		navigator -> leftContextCriterion = leftContextCriterion;
		navigator -> rightContextCriterion = rightContextCriterion;
		FormantPath_setNavigationContext (formantPath, contextCombination, matchContextOnly);
		FunctionEditor_marksChanged (me, true);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

void menu_cb_NextGreenInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	FormantPath formantPath = (FormantPath) my data;
	IntervalTierNavigator navigator = formantPath -> intervalTierNavigator.get();
	if (! navigator || ! navigator -> navigationLabels)
		return;
	TextInterval textInterval = IntervalTierNavigator_getNextMatchingInterval (navigator, my endSelection);
	Melder_require (textInterval,
		U"No more matching labels on the right.");
	my startSelection = textInterval -> xmin;
	my endSelection = textInterval -> xmax;
	const double selectionDuration = my endSelection - my startSelection;
	const double windowDuration = my endWindow - my startWindow;
	if (my startSelection > my startWindow && my endSelection <= my endWindow) {
		// ok 
	} else if (windowDuration > selectionDuration) {
		my startWindow = my startSelection - 0.5 * (windowDuration - selectionDuration);
		my startWindow = std::max (my tmin, my startWindow);
		my endWindow = my startWindow + windowDuration;
		my endWindow = std::min (my tmax, my endWindow);
	} else {
		const double newWindowDuration = 2.5 * selectionDuration;
		my startWindow = my startSelection - 0.5 * (newWindowDuration - selectionDuration);
		my startWindow = std::max (my tmin, my startWindow);
		my endWindow = my startWindow + newWindowDuration;
		my endWindow = std::min (my tmax, my endWindow);
	}
	my selectedTier = formantPath -> navigationTierNumber;
	FunctionEditor_marksChanged (me, true);
	Editor_broadcastDataChanged (me);
}

void menu_cb_PreviousGreenInterval (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	FormantPath formantPath = (FormantPath) my data;
	IntervalTierNavigator navigator = formantPath -> intervalTierNavigator.get();
	if (! navigator || ! navigator -> navigationLabels)
		return;
	TextInterval textInterval = IntervalTierNavigator_getPreviousMatchingInterval (navigator, my startSelection);
	Melder_require (textInterval,
		U"No more matching labels on the left.");
	my startSelection = textInterval -> xmin;
	my endSelection = textInterval -> xmax;
	const double selectionDuration = my endSelection - my startSelection;
	const double windowDuration = my endWindow - my startWindow;
	if (my startSelection > my startWindow && my endSelection <= my endWindow) {
		// ok 
	} else if (windowDuration > selectionDuration) {
		my startWindow = my startSelection - 0.5 * (windowDuration - selectionDuration);
		my startWindow = std::max (my tmin, my startWindow);
		my endWindow = my startWindow + windowDuration;
		my endWindow = std::min (my tmax, my endWindow);
	} else {
		const double newWindowDuration = 2.5 * selectionDuration;
		my startWindow = my startSelection - 0.5 * (newWindowDuration - selectionDuration);
		my startWindow = std::max (my tmin, my startWindow);
		my endWindow = my startWindow + newWindowDuration;
		my endWindow = std::min (my tmax, my endWindow);
	}
	my selectedTier = formantPath -> navigationTierNumber;
	FunctionEditor_marksChanged (me, true);
	Editor_broadcastDataChanged (me);
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

static void menu_cb_RenameLogTier (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Rename path tier", nullptr)
		SENTENCE (newName, U"New name", U"");
	EDITOR_OK
		const TextGrid grid =  my pathGridView.get();
		checkTierSelection_hard (me, U"rename a tier");
		const Function tier = grid -> tiers->at [my selectedTier];
		SET_STRING (newName, tier -> name ? tier -> name.get() : U"")
	EDITOR_DO
		const TextGrid grid =  my pathGridView.get();
		checkTierSelection_hard (me, U"rename a tier");
		const Function tier = grid -> tiers->at [my selectedTier];
		Editor_save (me, U"Rename tier");
		Thing_setName (tier, newName);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}
static void menu_cb_RemoveBoundariesBetween (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Remove boundaries between", nullptr)
		REAL (fromTime, U"left Interval (s)", U"0.1")
		REAL (toTime, U"right Interval (s)", U"0.1")
	EDITOR_OK
		SET_REAL (fromTime, my startSelection)
		SET_REAL (toTime, my endSelection)
	EDITOR_DO
		IntervalTier pathTier = (IntervalTier) my pathGridView -> tiers ->at [my pathTierNumber];
		do_removeBoundariesBetween (pathTier, fromTime, toTime);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_ViewAllTiers (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	FormantPath formantPath = (FormantPath) my data;
	const integer selectedTier = TextGridView_getOriginTierNumber (my pathGridView.get(), my selectedTier);
	TextGridView_viewAllWithSelectedOnTop (my pathGridView.get(), 0);
	my pathTierNumber = formantPath -> pathTierNumber;
	my selectedTier = TextGridView_getViewTierNumber (my pathGridView.get(), selectedTier);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ViewAllTiersPathOnTop (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const integer selectedTier = TextGridView_getOriginTierNumber (my pathGridView.get(), my selectedTier);
	TextGridView_viewAllWithSelectedOnTop (my pathGridView.get(), ((FormantPath) my data) -> pathTierNumber);
	my pathTierNumber = 1;
	my selectedTier = TextGridView_getViewTierNumber (my pathGridView.get(), selectedTier);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ViewTierSelection (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"View tier selection", nullptr)
	LABEL (U"Use original TextGrid tier numbers.")
	LABEL (U"You can use an 'P' instead of the path tier number.")
	SENTENCE (newOrder_string, U"Viewing order", U"P 1")
	EDITOR_OK
	EDITOR_DO
	FormantPathEditor_setTierOrder (me, newOrder_string);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_PublishTier (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const TextGrid grid =  my pathGridView.get();
	checkTierSelection (me, U"publish a tier");
	const Function tier = grid -> tiers->at [my selectedTier];
	autoTextGrid publish = TextGrid_createWithoutTiers (1e30, -1e30);
	TextGrid_addTier_copy (publish.get(), tier);
	Thing_setName (publish.get(), tier -> name.get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_modeler_parameterSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant modeler parameter settings", nullptr)		
		SENTENCE (parameters_string, U"Number of parameters per track", my default_modeler_numberOfParametersPerTrack ())
		POSITIVE (varianceExponent, U"Variance exponent", U"1.25")
	EDITOR_OK
		SET_STRING (parameters_string, my p_modeler_numberOfParametersPerTrack)
	EDITOR_DO
	double startTime = my formantModelerList -> xmin, endTime = my formantModelerList -> xmax;
	my formantModelerList = FormantPathEditor_to_FormantModelerList (me, startTime, endTime, parameters_string);
	FormantModelerList_setVarianceExponent (my formantModelerList.get(), varianceExponent);
	pref_str32cpy2 (my pref_modeler_numberOfParametersPerTrack (), my p_modeler_numberOfParametersPerTrack, parameters_string);
	my pref_modeler_varianceExponent () = my p_modeler_varianceExponent = varianceExponent;
	my v_drawSelectionViewer ();
	EDITOR_END
}

static void menu_cb_modeler_showBest3 (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	my pref_modeler_draw_showAllModels () = my p_modeler_draw_showAllModels = false;
	FormantModelerList_showBest3 (my formantModelerList.get());
	my v_drawSelectionViewer ();
}

static void menu_cb_ShowAllModels (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	my pref_modeler_draw_showAllModels () = my p_modeler_draw_showAllModels = true;
	FormantPathEditor_selectAll (me);
	my v_drawSelectionViewer ();
}

static void menu_cb_AdvancedModelerDrawingSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant modeler advanced drawing settings", nullptr)
		BOOLEAN (drawEstimatedTracks, U"Draw estimated tracks", my default_modeler_draw_estimatedTracks ())
		REAL (xSpaceFraction, U"Column separation (fraction)", my default_modeler_draw_xSpace_fraction ())
		REAL (ySpaceFraction, U"Row separation (fraction)", my default_modeler_draw_ySpace_fraction ())
		POSITIVE (yGridLineEvery_Hz, U"Horizontal grid lines every (Hz)", my default_modeler_draw_yGridLineEvery_Hz ())
		POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", my default_modeler_draw_maximumFrequency ())
		BOOLEAN (drawErrorBars, U"Draw error bars", my default_modeler_draw_showErrorBars ())
		REAL (errorBarWidth_s, U"Error bar width (s)", my default_modeler_draw_errorBarWidth_s ())
		REAL (xTrackShift_s, U"Shift even formant tracks by (s)", my default_modeler_draw_xTrackShift_s ())
	EDITOR_OK
		SET_BOOLEAN (drawEstimatedTracks, my p_modeler_draw_estimatedTracks)
		SET_REAL (xSpaceFraction, my p_modeler_draw_xSpace_fraction)
		SET_REAL (ySpaceFraction, my p_modeler_draw_ySpace_fraction)
		SET_REAL (yGridLineEvery_Hz, my p_modeler_draw_yGridLineEvery_Hz)
		SET_REAL (maximumFrequency, my p_modeler_draw_maximumFrequency)
		SET_BOOLEAN (drawErrorBars, my p_modeler_draw_showErrorBars)
		SET_REAL (errorBarWidth_s, my p_modeler_draw_errorBarWidth_s)
		SET_REAL (xTrackShift_s, my p_modeler_draw_xTrackShift_s)
	EDITOR_DO
	my pref_modeler_draw_estimatedTracks () = my p_modeler_draw_estimatedTracks = drawEstimatedTracks;
	my pref_modeler_draw_xSpace_fraction () = my p_modeler_draw_xSpace_fraction = xSpaceFraction;
	my pref_modeler_draw_ySpace_fraction () = my p_modeler_draw_ySpace_fraction = ySpaceFraction;
	my pref_modeler_draw_maximumFrequency () = my p_modeler_draw_maximumFrequency = maximumFrequency;
	my pref_modeler_draw_yGridLineEvery_Hz () = my p_modeler_draw_yGridLineEvery_Hz = yGridLineEvery_Hz;
	my pref_modeler_draw_showErrorBars () = my p_modeler_draw_showErrorBars = drawErrorBars;
	my pref_modeler_draw_errorBarWidth_s () = my p_modeler_draw_errorBarWidth_s = errorBarWidth_s;
	my pref_modeler_draw_xTrackShift_s () = my p_modeler_draw_xTrackShift_s = xTrackShift_s;
	my v_drawSelectionViewer ();
	EDITOR_END
}

static void menu_cb_DrawVisibleModels (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible modelers", nullptr)
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
		FormantModelerList fml = my formantModelerList.get();
		FormantModelerList_setVarianceExponent (fml, my p_modeler_varianceExponent);
		const double xCursor = (my startSelection == my endSelection ? my startSelection : fml -> xmin - 10.0 );
		const double yCursor = ( my d_spectrogram_cursor > my p_spectrogram_viewFrom && my d_spectrogram_cursor < my p_spectrogram_viewTo ? my d_spectrogram_cursor : -1000.0 );
		Graphics_setInner (my pictureGraphics);
		FormantModelerList_drawInMatrixGrid (fml, my pictureGraphics, 0, 0, kGraphicsMatrixOrigin::TOP_LEFT,
		my p_modeler_draw_xSpace_fraction, my p_modeler_draw_ySpace_fraction, 1,
		my formantModelerList -> numberOfTracksPerModel, my p_modeler_draw_maximumFrequency,
		my p_modeler_draw_yGridLineEvery_Hz, xCursor, yCursor, 0, my p_modeler_draw_showErrorBars,
		my p_modeler_draw_errorBarWidth_s, my p_modeler_draw_xTrackShift_s,
		my p_modeler_draw_estimatedTracks, garnish);
		Graphics_unsetInner (my pictureGraphics);
		Editor_closePraatPicture (me);	
	EDITOR_END
}

static void menu_cb_FormantColourSettings (FormantPathEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Formant colour settings", nullptr)
		WORD (pathColour_string, U"Dots in the path", my default_formant_path_colour ())
		WORD (defaultColour_string, U"Dots in default", my default_formant_default_colour ())
		WORD (selectedColour_string, U"Dots in selected", my default_formant_selected_colour ())
	EDITOR_OK
		SET_STRING (pathColour_string, my p_formant_path_colour)
		SET_STRING (defaultColour_string, my p_formant_default_colour)
		SET_STRING (selectedColour_string, my p_formant_selected_colour)
	EDITOR_DO
		Melder_require (! (Melder_equ (pathColour_string, defaultColour_string) || Melder_equ (pathColour_string, selectedColour_string) || Melder_equ (defaultColour_string, selectedColour_string)),
			U"All colours must be different.");
		pref_str32cpy2 (my pref_formant_path_colour (), my p_formant_path_colour, pathColour_string);
		pref_str32cpy2 (my pref_formant_default_colour (), my p_formant_default_colour, defaultColour_string);
		pref_str32cpy2 (my pref_formant_selected_colour (), my p_formant_selected_colour, selectedColour_string);
		FormantModelerListDrawingSpecification_setModelerColours (my formantModelerList -> drawingSpecification.get(), my p_formant_path_colour, my p_formant_default_colour, my p_formant_selected_colour, U"black");
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
		FormantPath formantPath = (FormantPath) my data;
		const Formant formant = formantPath -> formant.get();
		//const Formant defaultFormant = formantPath -> formants.at [formantPath -> defaultFormant];
		Formant_drawSpeckles (formant, my pictureGraphics, my startWindow, my endWindow,
			my p_spectrogram_viewTo, my p_formant_dynamicRange,
			my p_formant_picture_garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_RemoveAllTextFromTier (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	const TextGrid grid =  my pathGridView.get();
	checkTierSelection_hard (me, U"remove all text from a tier");
		
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass (grid -> tiers->at [my selectedTier], & intervalTier, & textTier);

	Editor_save (me, U"Remove text from tier");
	if (intervalTier)
		IntervalTier_removeText (intervalTier);
	else
		TextTier_removeText (textTier);

	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_showFormants (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	my pref_formant_show () = my p_formant_show = ! my p_formant_show;
	GuiMenuItem_check (my formantToggle, my p_formant_show);   // in case we're called from a script
	FunctionEditor_redraw (me);
}

static void menu_cb_BoundariesToNearestZeroCrossing (FormantPathEditor me, EDITOR_ARGS_DIRECT) {
	if (my selectedTier != my pathTierNumber)
		return;
	IntervalTier pathTier = reinterpret_cast<IntervalTier> (my pathGridView -> tiers->at [my pathTierNumber]);
	Melder_require (pathTier -> intervals.size > 1,
		U"We cannot move boundaries on an empty tier.");
	integer midIndex = IntervalTier_timeToIndex (pathTier, my startSelection), rightIndex = midIndex + 1;
	Melder_require (midIndex != 0,
		U"First select an interval on the path tier by clicking on it.");
	TextInterval leftInterval, rightInterval, midInterval = pathTier -> intervals.at [midIndex];
	if (my startSelection < my endSelection) {
		/*
			selection must be the interval
		*/
		rightIndex = IntervalTier_timeToIndex (pathTier, my endSelection);
		Melder_require (rightIndex == midIndex + 1 && (my startSelection == midInterval -> xmin && my endSelection == midInterval -> xmax),
			U"The start and end time of the interval and the selected part of the sound should be equal. Make them equal by clicking in the interval on the path tier.");
	}
	/*
		We have an interval selected
	*/
	double zeroLeft = Sound_getNearestZeroCrossing (my d_sound.data, midInterval -> xmin, 1);
	double zeroRight = Sound_getNearestZeroCrossing (my d_sound.data, midInterval -> xmax, 1);
	/*
		Don't allow crossing boundaries
	*/
	Melder_require (zeroLeft < zeroRight,
		U"The left and right boundary should not move to the same location.");
	Melder_require (zeroLeft < midInterval -> xmax,
		U"The new left boundary should not move to the right of the old right boundary.");
	Melder_require (zeroRight > midInterval -> xmin,
		U"The new right boundary should not move to the left of the old left boundary.");
	
	Editor_save (me, U"Boundaries to nearest zero crossings");
	
	if (midIndex == 1) { // Only move right boundary
		rightInterval = pathTier -> intervals.at [rightIndex];
		Melder_require (zeroRight < rightInterval -> xmax,
			U"The new right boundary should not move to the right of the following interval.");
		midInterval -> xmax = rightInterval -> xmin = zeroRight;
	} else if (midIndex == pathTier -> intervals.size) { // only move left boudary
		leftInterval = pathTier -> intervals.at [midIndex - 1];
		Melder_require (zeroLeft > leftInterval -> xmin,
			U"The new left boundary should not move to the left of the preceeding interval.");
		leftInterval -> xmax = midInterval -> xmin = zeroLeft;
	} else {
		rightInterval = pathTier -> intervals.at [rightIndex];
		leftInterval = pathTier -> intervals.at [midIndex - 1];
		Melder_require (zeroLeft > leftInterval -> xmin,
			U"The new left boundary should not move to the left of the preceeding interval.");
		Melder_require (zeroRight < rightInterval -> xmax,
			U"The new right boundary should not move to the right of the following interval.");
		leftInterval -> xmax = midInterval -> xmin = zeroLeft;
		rightInterval -> xmin = midInterval -> xmax = zeroRight;
	}
	Editor_broadcastDataChanged (me);
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

	if (our d_sound.data) {
		Editor_addCommand (this, U"Select", U"-- move to zero --", 0, 0);
		Editor_addCommand (this, U"Select", U"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (this, U"Select", U"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (this, U"Select", U"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (this, U"Select", U"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}

	Editor_addCommand (this, U"Query", U"-- query interval --", 0, nullptr);
	Editor_addCommand (this, U"Query", U"Get starting point of interval", 0, menu_cb_GetStartingPointOfInterval);
	Editor_addCommand (this, U"Query", U"Get end point of interval", 0, menu_cb_GetEndPointOfInterval);
	Editor_addCommand (this, U"Query", U"Get label of interval", 0, menu_cb_GetLabelOfInterval);

	menu = Editor_addMenu (this, U"Interval", 0);
	EditorMenu_addCommand (menu, U"Boundaries to nearest zero crossings", GuiMenu_COMMAND | '0', menu_cb_BoundariesToNearestZeroCrossing);
	EditorMenu_addCommand (menu, U"Insert interval", GuiMenu_COMMAND | '1', menu_cb_InsertInterval);
	EditorMenu_addCommand (menu, U"Reset text and formants", 0, menu_cb_ResetTextAndFormants);
	EditorMenu_addCommand (menu, U"Remove interval", 0, menu_cb_RemoveInterval);
	EditorMenu_addCommand (menu, U"-- green stuff --", 0, nullptr);
	
	our navigateSettingsButton = EditorMenu_addCommand (menu, U"Navigation settings...", 0, menu_cb_NavigationSettings);
	our navigateNextButton  = EditorMenu_addCommand (menu, U"Next green interval", 0, menu_cb_NextGreenInterval);
	our navigatePreviousButton = EditorMenu_addCommand (menu, U"Previous green interval", 0, menu_cb_PreviousGreenInterval);

	menu = Editor_addMenu (this, U"Tier", 0);
	EditorMenu_addCommand (menu, U"Rename path tier...", 0, menu_cb_RenameLogTier);
	EditorMenu_addCommand (menu, U"-- remove tier --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Remove boundaries between...", 0, menu_cb_RemoveBoundariesBetween);
	EditorMenu_addCommand (menu, U"Remove all text from tier", 0, menu_cb_RemoveAllTextFromTier);
	EditorMenu_addCommand (menu, U"-- extract tier --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Extract to list of objects:", GuiMenu_INSENSITIVE, menu_cb_PublishTier /* dummy */);
	EditorMenu_addCommand (menu, U"Extract entire selected tier", 0, menu_cb_PublishTier);
	EditorMenu_addCommand (menu, U"View all tiers", 0, menu_cb_ViewAllTiers);
	EditorMenu_addCommand (menu, U"View all tiers (path on top)", 0, menu_cb_ViewAllTiersPathOnTop);
	EditorMenu_addCommand (menu, U"View tier selection...", 0, menu_cb_ViewTierSelection);

	if (our d_sound.data || our d_longSound.data) {
		if (our v_hasAnalysis ())
			our v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the TextGrid menus
	}
	menu = Editor_addMenu (this, U"Modelers", 0);
	EditorMenu_addCommand (menu, U"Modeler parameter settings...", 0, menu_cb_modeler_parameterSettings);
	EditorMenu_addCommand (menu, U"Show best three modelers", 0, menu_cb_modeler_showBest3);
	EditorMenu_addCommand (menu, U"Show all modelers", 0, menu_cb_ShowAllModels);
	EditorMenu_addCommand (menu, U"Advanced modeler drawing settings...", 0, menu_cb_AdvancedModelerDrawingSettings);
	EditorMenu_addCommand (menu, U" -- drawing -- ", 0, 0);
	EditorMenu_addCommand (menu, U"Draw visible modelers...", 0, menu_cb_DrawVisibleModels);
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

static void do_drawIntervalTier (FormantPathEditor me, IntervalTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	integer x1DC, x2DC, yDC;
	Graphics_WCtoDC (my graphics.get(), my startWindow, 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics.get(), my endWindow, 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics.get(), my p_useTextStyles);
	Graphics_setNumberSignIsBold (my graphics.get(), my p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), my p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), my p_useTextStyles);

	/*
		Highlight interval: yellow (selected) or green (matching label).
	*/
	const integer selectedInterval = ( itier == my selectedTier ? getSelectedInterval (me) : 0 ), ninterval = tier -> intervals.size;
	IntervalTierNavigator navigator = ((FormantPath) (my data )) -> intervalTierNavigator.get();
	integer navigationTierNumber = ((FormantPath) (my data )) -> navigationTierNumber;
	for (integer iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = tier -> intervals.at [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmax > my startWindow && tmin < my endWindow) {   // interval visible?
			const bool intervalIsSelected = ( iinterval == selectedInterval );
			const bool labelDoesMatch = ( itier == TextGridView_getViewTierNumber (my pathGridView.get(), navigationTierNumber) && navigator && IntervalTierNavigator_isLabelMatch (navigator, iinterval) );
			if (tmin < my startWindow)
				tmin = my startWindow;
			if (tmax > my endWindow)
				tmax = my endWindow;
			if (labelDoesMatch) {
				Graphics_setColour (my graphics.get(), Melder_LIME);
				Graphics_fillRectangle (my graphics.get(), tmin, tmax, 0.0, 1.0);
			}
			if (intervalIsSelected) {
				if (labelDoesMatch) {
					tmin = 0.85 * tmin + 0.15 * tmax;
					tmax = 0.15 * tmin + 0.85 * tmax;
				}
				Graphics_setColour (my graphics.get(), Melder_YELLOW);
				Graphics_fillRectangle (my graphics.get(), tmin, tmax,
						labelDoesMatch ? 0.15 : 0.0, labelDoesMatch? 0.85: 1.0);
			}
		}
	}
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_line (my graphics.get(), my endWindow, 0.0, my endWindow, 1.0);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		bool cursorAtBoundary = false;
		for (integer iinterval = 2; iinterval <= ninterval; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			if (interval -> xmin == my startSelection)
				cursorAtBoundary = true;
		}
		if (! cursorAtBoundary) {
			const double dy = Graphics_dyMMtoWC (my graphics.get(), 1.5);
			Graphics_setGrey (my graphics.get(), 0.8);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics.get(), 1.0);
			Graphics_setColour (my graphics.get(), Melder_BLUE);
			Graphics_circle_mm (my graphics.get(), my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics.get(), my p_alignment, Graphics_HALF);
	for (integer iinterval = 1; iinterval <= ninterval; iinterval ++) {
		const TextInterval interval = tier -> intervals.at [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmin < my tmin)
			tmin = my tmin;
		if (tmax > my tmax)
			tmax = my tmax;
		if (tmin >= tmax)
			continue;
		const bool intervalIsSelected = ( selectedInterval == iinterval );

		/*
			Draw left boundary.
		*/
		if (tmin >= my startWindow && tmin <= my endWindow && iinterval > 1) {
			const bool boundaryIsSelected = ( my selectedTier == my pathTierNumber && my selectedTier == itier && tmin == my startSelection );
			Graphics_setColour (my graphics.get(), boundaryIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), tmin, 0.0, tmin, 1.0);

			/*
				Show alignment with cursor.
			*/
			if (tmin == my startSelection) {
				Graphics_setColour (my graphics.get(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics.get(), tmin, 0.0, tmin, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics.get(), 1.0);

		/*
			Draw label text.
		*/
		if (interval -> text && tmax >= my startWindow && tmin <= my endWindow) {
			const double t1 = std::max (my startWindow, tmin);
			const double t2 = std::min (my endWindow, tmax);
			Graphics_setColour (my graphics.get(), intervalIsSelected ? Melder_RED : Melder_BLACK);
			Graphics_textRect (my graphics.get(), t1, t2, 0.0, 1.0, interval -> text.get());
			Graphics_setColour (my graphics.get(), Melder_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics.get(), true);
	Graphics_setNumberSignIsBold (my graphics.get(), true);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), true);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), true);
}

static void do_drawTextTier (FormantPathEditor me, TextTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	const integer npoint = tier -> points.size;
	Graphics_setPercentSignIsItalic (my graphics.get(), my p_useTextStyles);
	Graphics_setNumberSignIsBold (my graphics.get(), my p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), my p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), my p_useTextStyles);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		bool cursorAtPoint = false;
		for (integer ipoint = 1; ipoint <= npoint; ipoint ++) {
			const TextPoint point = tier -> points.at [ipoint];
			if (point -> number == my startSelection)
				cursorAtPoint = true;
		}
		if (! cursorAtPoint) {
			const double dy = Graphics_dyMMtoWC (my graphics.get(), 1.5);
			Graphics_setGrey (my graphics.get(), 0.8);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics.get(), 1.0);
			Graphics_setColour (my graphics.get(), Melder_BLUE);
			Graphics_circle_mm (my graphics.get(), my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer ipoint = 1; ipoint <= npoint; ipoint ++) {
		const TextPoint point = tier -> points.at [ipoint];
		const double t = point -> number;
		if (t >= my startWindow && t <= my endWindow) {
			const bool pointIsSelected = ( itier == my selectedTier && t == my startSelection );
			Graphics_setColour (my graphics.get(), pointIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), t, 0.0, t, 0.2);
			Graphics_line (my graphics.get(), t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics.get(), 1.0);

			/*
				Wipe out the cursor where the text is going to be.
			*/
			Graphics_setColour (my graphics.get(), Melder_WHITE);
			Graphics_line (my graphics.get(), t, 0.2, t, 0.8);

			/*
				Show alignment with cursor.
			*/
			if (my startSelection == my endSelection && t == my startSelection) {
				Graphics_setColour (my graphics.get(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics.get(), t, 0.0, t, 0.2);
				Graphics_line (my graphics.get(), t, 0.8, t, 1.0);
				Graphics_setLineWidth (my graphics.get(), 1.0);
			}
			Graphics_setColour (my graphics.get(), pointIsSelected ? Melder_RED : Melder_BLUE);
			if (point -> mark)
				Graphics_text (my graphics.get(), t, 0.5, point -> mark.get());
		}
	}
	Graphics_setPercentSignIsItalic (my graphics.get(), true);
	Graphics_setNumberSignIsBold (my graphics.get(), true);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), true);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), true);
}

void structFormantPathEditor :: v_draw () {
	Graphics_Viewport vp1, vp2;
	const integer ntiers = our pathGridView -> tiers->size;
	const enum kGraphics_font oldFont = Graphics_inqFont (our graphics.get());
	const double oldFontSize = Graphics_inqFontSize (our graphics.get());
	const bool showAnalysis = v_hasAnalysis () &&
			(p_spectrogram_show || p_pitch_show || p_intensity_show || p_formant_show) &&
			(d_longSound.data || d_sound.data);
	double soundY = _FormantPathEditor_computeSoundY (this), soundY2 = _FormantPathEditor_computeSoundY2 (this);

	/*
		Draw the sound.
	*/
	if (d_longSound.data || d_sound.data) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY2, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		//Graphics_flushWs (our graphics.get());
		Graphics_resetViewport (our graphics.get(), vp1);
	}

	/*
		Draw tiers.
	*/
	if (d_longSound.data || d_sound.data) vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, soundY);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	bool isDefaultView = TextGridView_isDefaultView (our pathGridView.get());
	for (integer itier = 1; itier <= ntiers; itier ++) {
		const Function anyTier = our pathGridView -> tiers->at [itier];
		const bool tierIsSelected = ( itier == selectedTier );
		const bool isIntervalTier = ( anyTier -> classInfo == classIntervalTier );
		vp2 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0,
			1.0 - (double) itier / (double) ntiers,
			1.0 - (double) (itier - 1) / (double) ntiers);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		if (itier != 1)
			Graphics_line (our graphics.get(), our startWindow, 1.0, our endWindow, 1.0);

		/*
			Show the number and the name of the tier.
		*/
		Graphics_setColour (our graphics.get(), tierIsSelected ? Melder_RED : Melder_BLACK);
		Graphics_setFont (our graphics.get(), oldFont);
		Graphics_setFontSize (our graphics.get(), 14);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		if (isDefaultView)
			Graphics_text (our graphics.get(), our startWindow, 0.5, tierIsSelected ? U"☞ " : U"", itier, U"");
		else
			Graphics_text (our graphics.get(), our startWindow, 0.5, tierIsSelected ? U"☞ " : U"", itier, 
				U" → ", our pathGridView -> tierNumbers [itier], U"");
		Graphics_setFontSize (our graphics.get(), oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT,
				our p_showNumberOf == kTextGridEditor_showNumberOf::NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (our graphics.get(), our endWindow, 0.5, anyTier -> name.get());
		}
		if (our p_showNumberOf != kTextGridEditor_showNumberOf::NOTHING) {
			Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_TOP);
			if (our p_showNumberOf == kTextGridEditor_showNumberOf::INTERVALS_OR_POINTS) {
				integer count = isIntervalTier ? ((IntervalTier) anyTier) -> intervals.size : ((TextTier) anyTier) -> points.size;
				integer position = itier == selectedTier ? ( isIntervalTier ? getSelectedInterval (this) : getSelectedPoint (this) ) : 0;
				if (position)
					Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(", position, U"/", count, U")");
				else
					Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(", count, U")");
			} else {
				Melder_assert (our p_showNumberOf == kTextGridEditor_showNumberOf::NONEMPTY_INTERVALS_OR_POINTS);
				integer count = 0;
				if (isIntervalTier) {
					const IntervalTier tier = (IntervalTier) anyTier;
					const integer numberOfIntervals = tier -> intervals.size;
					for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
						const TextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> text && interval -> text [0] != U'\0')
							count ++;
					}
				} else {
					const TextTier tier = (TextTier) anyTier;
					const integer numberOfPoints = tier -> points.size;
					for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
						const TextPoint point = tier -> points.at [ipoint];
						if (point -> mark && point -> mark [0] != U'\0')
							count ++;
					}
				}
				Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(##", count, U"#)");
			}
		}

		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_setFont (our graphics.get(), kGraphics_font::TIMES);
		Graphics_setFontSize (our graphics.get(), p_fontSize);
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (our graphics.get(), vp2);
	}
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFont (our graphics.get(), oldFont);
	Graphics_setFontSize (our graphics.get(), oldFontSize);
	if (d_longSound.data || d_sound.data)
		Graphics_resetViewport (our graphics.get(), vp1);
	//Graphics_flushWs (our graphics.get());

	if (showAnalysis) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY, soundY2);
		v_draw_analysis ();
		//Graphics_flushWs (our graphics.get());
		Graphics_resetViewport (our graphics.get(), vp1);
		/* Draw pulses. */
		if (p_pulses_show) {
			vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY2, 1.0);
			v_draw_analysis_pulses ();
			TimeSoundEditor_drawSound (this, -1.0, 1.0);   // second time, partially across the pulses
			//Graphics_flushWs (our graphics.get());
			Graphics_resetViewport (our graphics.get(), vp1);
		}
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	if (our d_longSound.data || our d_sound.data) {
		Graphics_line (our graphics.get(), our startWindow, soundY, our endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (our graphics.get(), our startWindow, soundY2, our endWindow, soundY2);
			Graphics_line (our graphics.get(), our startWindow, soundY, our startWindow, soundY2);
			Graphics_line (our graphics.get(), our endWindow, soundY, our endWindow, soundY2);
		}
	}

	/*
		Finally, us usual, update the menus.
	*/
	v_updateMenuItems_file ();
	v_updateMenuItems_navigation ();
}

void FormantPathEditor_setSelectionViewerViewportAndWindow (FormantPathEditor me) {
	/*
		BOTTOM_MARGIN = 2; TOP_MARGIN = 3; MARGIN = 107; space = 30
		The FunctionEditor defines the selectionViewer viewport as
		Graphics_setViewport (my graphics.get(), my selectionViewerLeft + MARGIN, my selectionViewerRight - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		my v_drawSelectionViewer ();
		We need somewhat more space; idealy we could override the values above, for now they are hard-coded
	*/
	double space = 30.0, margin = 107.0;
	double vp_left = my selectionViewerLeft + 0.5 * margin ;
	double vp_right = my selectionViewerRight - 0.75 * margin;
	double vp_bottom = space;
	double vp_top = my height - space;
	Graphics_setViewport (my graphics.get(), vp_left, vp_right, vp_bottom, vp_top);
	Graphics_setWindow (my graphics.get(), vp_left, vp_right, vp_bottom, vp_top);
}

void structFormantPathEditor :: v_drawSelectionViewer () {
	/*
		BOTTOM_MARGIN = 2; TOP_MARGIN = 3; MARGIN = 107; space = 30
		The FunctionEditor defines the selectionViewer viewport as
		Graphics_setViewport (my graphics.get(), my selectionViewerLeft + MARGIN, my selectionViewerRight - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		my v_drawSelectionViewer ();
		We need somewhat more space; idealy we could override the values above, for now they are hard-coded
	*/
	double space = 30.0, margin = 107.0;
	double vp_left = selectionViewerLeft + 0.5 * margin ;
	double vp_right = selectionViewerRight - 0.75 * margin;
	double vp_bottom = space;
	double vp_top = height - space;
	Graphics_setViewport (our graphics.get(), vp_left, vp_right, vp_bottom, vp_top);
	Graphics_setWindow (our graphics.get(), vp_left, vp_right, vp_bottom, vp_top);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), vp_left, vp_right, vp_bottom, vp_top);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFont (our graphics.get(), kGraphics_font::TIMES);
	Graphics_setFontSize (our graphics.get(), 9.0);
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
	if (! our formantModelerList.get())
			return;
	double startTime = startWindow, endTime = endWindow;
	if (startSelection == endSelection) {
		
		const IntervalTier pathTier = reinterpret_cast<IntervalTier> (our pathGridView->tiers->at [our pathTierNumber]);
		const integer selectedInterval = IntervalTier_timeToIndex (pathTier, our startSelection);
		const TextInterval textInterval = pathTier -> intervals .at [selectedInterval];
		startTime = textInterval -> xmin;
		endTime = textInterval -> xmax;

	} else {
		startTime = startSelection;
		endTime = endSelection;
	}
	if (startTime != our formantModelerList -> xmin || endTime != our formantModelerList -> xmax) {
		try {
			our formantModelerList = FormantPathEditor_to_FormantModelerList (this, startTime, endTime, our p_modeler_numberOfParametersPerTrack);
		} catch (MelderError) {
			Melder_clearError ();
			Graphics_setColour (our graphics.get(), Melder_WHITE);
			Graphics_fillRectangle (our graphics.get(), vp_left, vp_right, vp_bottom, vp_top);			
			Graphics_setColour (our graphics.get(), Melder_BLACK);
			Graphics_setFontSize (our graphics.get(), 10.0);
			Graphics_text (our graphics.get(), 0.5 * (vp_left + vp_right), 0.5 * (vp_top + vp_bottom),
				U"(Not enough formant points in selected interval)");
			Graphics_setColour (our graphics.get(), Melder_BLACK);
			Graphics_setFontSize (our graphics.get(), 9.0);
			return;
		}
	}
	FormantModelerList fml = our formantModelerList.get();
	FormantModelerList_setVarianceExponent (fml, our p_modeler_varianceExponent);
	const double xCursor = ( startSelection == endSelection ? startSelection : fml -> xmin - 10.0 );
	const double yCursor = ( our d_spectrogram_cursor > our p_spectrogram_viewFrom && our d_spectrogram_cursor < our p_spectrogram_viewTo ? our d_spectrogram_cursor : -1000.0 );
	/*
		Mark RED if label of interval matches one of the modelers. This should only occur if
		a sound interval is exactly selected and matches a textgrid interval
	*/
	const IntervalTier pathTier = reinterpret_cast<IntervalTier> (our pathGridView -> tiers -> at [our pathTierNumber]);
	const integer selectedInterval = IntervalTier_timeToIndex (pathTier, our startSelection);
	const TextInterval textInterval = pathTier -> intervals .at [selectedInterval];
	const integer imodel = FormantPathEditor_identifyModelFromIntervalLabel (this, textInterval -> text.get());
	if (FormantPathEditor_parametersChanged (this, imodel, textInterval -> text.get()))
		FormantPathEditor_markParameterChange (this);
	if (imodel > 0)
		FormantPathEditor_selectModeler (this, imodel);
	FormantModelerList_markBest3 (fml);
	FormantModelerList_drawInMatrixGrid (fml, our graphics.get(), 0, 0, kGraphicsMatrixOrigin::TOP_LEFT,
		our p_modeler_draw_xSpace_fraction, our p_modeler_draw_ySpace_fraction, 1,
		our formantModelerList -> numberOfTracksPerModel, our p_modeler_draw_maximumFrequency,
		our p_modeler_draw_yGridLineEvery_Hz, xCursor, yCursor, 0, our p_modeler_draw_showErrorBars,
		our p_modeler_draw_errorBarWidth_s, our p_modeler_draw_xTrackShift_s,
		our p_modeler_draw_estimatedTracks, true);
}

static void Formant_drawSpeckles_insideOverlap (Formant me, Graphics g, double tmin_view, double tmax_view, double tmin_selection, double tmax_selection, double fmin, double fmax,
	double suppress_dB)
{
	double maximumIntensity = 0.0, minimumIntensity;
	Function_unidirectionalAutowindow (me, & tmin_view, & tmax_view);
	double tmin = std::max (tmin_view, tmin_selection);
	double tmax = std::min (tmax_view, tmax_selection);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	Graphics_setWindow (g, tmin_view, tmax_view, fmin, fmax);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		if (frame -> intensity > maximumIntensity)
			maximumIntensity = frame -> intensity;
	}
	if (maximumIntensity == 0.0 || suppress_dB <= 0.0)
		minimumIntensity = 0.0;   // ignore
	else
		minimumIntensity = maximumIntensity / pow (10.0, suppress_dB / 10.0);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		const double x = Sampled_indexToX (me, iframe);
		if (frame -> intensity < minimumIntensity)
			continue;
		for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++) {
			const double frequency = frame -> formant [iformant]. frequency;
			if (frequency >= fmin && frequency <= fmax)
				Graphics_speckle (g, x, frequency);
		}
	}
}

void structFormantPathEditor :: v_draw_analysis_formants () {
	FormantPath formantPath = (FormantPath) our data;
	const Formant formant = formantPath -> formant.get();
	const Formant defaultFormant = formantPath -> formants.at [formantPath -> defaultFormant];
	if (our p_formant_show) {
		Graphics_setColour (our graphics.get(), MelderColour_fromColourNameOrNumberStringOrRGBString (our p_formant_default_colour));
		Graphics_setSpeckleSize (our graphics.get(), our p_formant_dotSize);
		Formant_drawSpeckles_inside (defaultFormant, our graphics.get(), our startWindow, our endWindow,
			our p_spectrogram_viewFrom, our p_spectrogram_viewTo, our p_formant_dynamicRange);
		Graphics_setColour (our graphics.get(), MelderColour_fromColourNameOrNumberStringOrRGBString (our p_formant_path_colour));
		Formant_drawSpeckles_inside (formant, our graphics.get(), our startWindow, our endWindow,
			our p_spectrogram_viewFrom, our p_spectrogram_viewTo, our p_formant_dynamicRange);
		if (our startSelection < our endSelection) {
			FormantModelerListDrawingSpecification drawingSpecification = our formantModelerList -> drawingSpecification.get ();
			/*
				We also need to test our selectedModeler because the formants are drawn before the modelers are updated
			*/
			if (drawingSpecification -> selectedModeler > 0 && our selectedModeler > 0) {
				Graphics_setColour (our graphics.get(), MelderColour_fromColourNameOrNumberStringOrRGBString (our p_formant_selected_colour));
				Formant selectedFormant = formantPath -> formants.at [drawingSpecification -> selectedModeler];
				Formant_drawSpeckles_insideOverlap (selectedFormant, our graphics.get(), our startWindow, our endWindow, our startSelection, our endSelection, our p_spectrogram_viewFrom, our p_spectrogram_viewTo, our p_formant_dynamicRange);
			}
		}
		Graphics_setColour (our graphics.get(), Melder_BLACK);
	}
}

static void do_drawWhileDragging (FormantPathEditor me, double numberOfTiers, bool selectedTier [], double x, double soundY) {
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		if (selectedTier [itier]) {
			const double ymin = soundY * (1.0 - (double) itier / numberOfTiers);
			const double ymax = soundY * (1.0 - (double) (itier - 1) / numberOfTiers);
			Graphics_setLineWidth (my graphics.get(), 7.0);
			Graphics_line (my graphics.get(), x, ymin, x, ymax);
		}
	}
	Graphics_setLineWidth (my graphics.get(), 1);
	Graphics_line (my graphics.get(), x, 0.0, x, 1.01);
	Graphics_text (my graphics.get(), x, 1.01, Melder_fixed (x, 6));
}

static void do_dragBoundary (FormantPathEditor me, double xbegin, integer iClickedTier, int shiftKeyPressed) {
	const TextGrid grid =  my pathGridView.get();
	if (iClickedTier != my pathTierNumber)
		return;
	const integer numberOfTiers = grid -> tiers->size;
	double xWC = xbegin, yWC;
	double leftDraggingBoundary = my tmin, rightDraggingBoundary = my tmax;   // initial dragging range
	bool selectedTier [1000];
	const double soundY = _FormantPathEditor_computeSoundY (me);

	/*
		Determine the set of selected boundaries and points, and the dragging range.
	*/
	for (int itier = 1; itier <= numberOfTiers; itier ++) {
		selectedTier [itier] = false;   // the default
		/*
			If the user has pressed the shift key, let her drag all the boundaries and points at this time.
			Otherwise, let her only drag the boundary or point on the clicked tier.
		*/
		if (itier == iClickedTier || shiftKeyPressed == my p_shiftDragMultiple) {
			IntervalTier intervalTier;
			TextTier textTier;
			_AnyTier_identifyClass (grid -> tiers->at [itier], & intervalTier, & textTier);
			if (intervalTier && itier == my pathTierNumber) {
				integer ibound = IntervalTier_hasBoundary (intervalTier, xbegin);
				if (ibound) {
					TextInterval leftInterval = intervalTier -> intervals.at [ibound - 1];
					TextInterval rightInterval = intervalTier -> intervals.at [ibound];
					selectedTier [itier] = true;
					/*
						Prevent the user from dragging the boundary past its left or right neighbours on the same tier.
					*/
					if (leftInterval -> xmin > leftDraggingBoundary)
						leftDraggingBoundary = leftInterval -> xmin;
					if (rightInterval -> xmax < rightDraggingBoundary)
						rightDraggingBoundary = rightInterval -> xmax;
				}
			}
		}
	}

	Graphics_xorOn (my graphics.get(), Melder_MAROON);
	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at old position
	while (Graphics_mouseStillDown (my graphics.get())) {
		double xWC_new;
		Graphics_getMouseLocation (my graphics.get(), & xWC_new, & yWC);
		if (xWC_new != xWC) {
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at old position
			xWC = xWC_new;
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at new position
		}
	}
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at new position
	Graphics_xorOff (my graphics.get());

	/*
		The simplest way to cancel the dragging operation, is to drag outside the window.
	*/
	if (xWC <= my startWindow || xWC >= my endWindow)
		return;

	/*
		If the user dropped near an existing boundary in an unselected tier or near the cursor,
		we snap to that mark.
	*/
	const integer itierDrop = _FormantPathEditor_yWCtoTier (me, yWC);
	if (yWC > 0.0 && yWC < soundY && ! selectedTier [itierDrop]) {   // dropped inside an unselected tier?
		const Function anyTierDrop = grid -> tiers->at [itierDrop];
		if (anyTierDrop -> classInfo == classIntervalTier) {
			const IntervalTier tierDrop = (IntervalTier) anyTierDrop;
			for (integer ibound = 1; ibound < tierDrop -> intervals.size; ibound ++) {
				const TextInterval left = tierDrop -> intervals.at [ibound];
				if (fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - left -> xmax)) < 1.5) {   // near a boundary?
					/*
						Snap to boundary.
					*/
					xWC = left -> xmax;
				}
			}
		} else {
			const TextTier tierDrop = (TextTier) anyTierDrop;
			for (integer ipoint = 1; ipoint <= tierDrop -> points.size; ipoint ++) {
				TextPoint point = tierDrop -> points.at [ipoint];
				if (fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - point -> number)) < 1.5) {   // near a point?
					/*
						Snap to point.
					*/
					xWC = point -> number;
				}
			}
		}
	} else if (xbegin != my startSelection && fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - my startSelection)) < 1.5) {   // near the cursor?
		/*
			Snap to cursor.
		*/
		xWC = my startSelection;
	} else if (xbegin != my endSelection && fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - my endSelection)) < 1.5) {   // near the cursor?
		/*
			Snap to cursor.
		*/
		xWC = my endSelection;
	}

	/*
		We cannot move a boundary out of the dragging range.
	*/
	if (xWC <= leftDraggingBoundary || xWC >= rightDraggingBoundary) {
		Melder_beep ();
		return;
	}

	Editor_save (me, U"Drag");

	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		if (selectedTier [itier]) {
			IntervalTier intervalTier;
			TextTier textTier;
			_AnyTier_identifyClass (grid -> tiers->at [itier], & intervalTier, & textTier);
			if (intervalTier) {
				const integer numberOfIntervals = intervalTier -> intervals.size;
				for (integer ibound = 2; ibound <= numberOfIntervals; ibound ++) {
					TextInterval left = intervalTier -> intervals.at [ibound - 1], right = intervalTier -> intervals.at [ibound];
					if (left -> xmax == xbegin) {   // boundary dragged?
						left -> xmax = right -> xmin = xWC;   // move boundary to drop site
						break;
					}
				}
			} else {
				const integer iDraggedPoint = AnyTier_hasPoint (textTier->asAnyTier(), xbegin);
				if (iDraggedPoint) {
					integer dropSiteHasPoint = AnyTier_hasPoint (textTier->asAnyTier(), xWC);
					if (dropSiteHasPoint != 0) {
						Melder_warning (U"Cannot drop point on an existing point.");
					} else {
						const TextPoint point = textTier -> points.at [iDraggedPoint];
						/*
							Move point to drop site. May have passed another point.
						*/
						autoTextPoint newPoint = Data_copy (point);
						newPoint -> number = xWC;   // move point to drop site
						textTier -> points. removeItem (iDraggedPoint);
						textTier -> points. addItem_move (newPoint.move());
					}
				}
			}
		}
	}

	/*
		Select the drop site.
	*/
	if (my startSelection == xbegin)
		my startSelection = xWC;
	if (my endSelection == xbegin)
		my endSelection = xWC;
	if (my startSelection > my endSelection)
		std::swap (my startSelection, my endSelection);

	FunctionEditor_marksChanged (me, true);
	Editor_broadcastDataChanged (me);
}

bool structFormantPathEditor :: v_click (double xclick, double yWC, bool shiftKeyPressed_) {
	const TextGrid grid = our pathGridView.get();

	/*
		In answer to a click in the sound part,
		we keep the same tier selected and move the cursor or drag the "yellow" selection.
	*/
	our selectedModeler = 0;
	const double soundY = _FormantPathEditor_computeSoundY (this);
	if (yWC > soundY) {   // clicked in sound part?
		if ((our p_spectrogram_show || our p_formant_show) && yWC < 0.5 * (soundY + 1.0)) {
			our d_spectrogram_cursor = our p_spectrogram_viewFrom +
					2.0 * (yWC - soundY) / (1.0 - soundY) * (our p_spectrogram_viewTo - our p_spectrogram_viewFrom);
		}
		our FormantPathEditor_Parent :: v_click (xclick, yWC, shiftKeyPressed_);
		return FunctionEditor_UPDATE_NEEDED;
	}

	/*
		The user clicked in the grid part.
		We select the tier in which she clicked.
	*/
	const integer clickedTierNumber = _FormantPathEditor_yWCtoTier (this, yWC);

	if (xclick <= our startWindow || xclick >= our endWindow) {
		our selectedTier = clickedTierNumber;
		return FunctionEditor_UPDATE_NEEDED;
	}

	double tmin_, tmax_;
	_FormantPathEditor_timeToInterval (this, xclick, clickedTierNumber, & tmin_, & tmax_);
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass (grid -> tiers->at [clickedTierNumber], & intervalTier, & textTier);

	/*
		Get the time of the nearest boundary or point.
	*/
	double tnear = undefined;
	integer clickedLeftBoundary = 0;
	if (intervalTier) {
		const integer clickedIntervalNumber = IntervalTier_timeToIndex (intervalTier, xclick);
		if (clickedIntervalNumber != 0) {
			const TextInterval interval = intervalTier -> intervals.at [clickedIntervalNumber];
			if (xclick > 0.5 * (interval -> xmin + interval -> xmax)) {
				tnear = interval -> xmax;
				clickedLeftBoundary = clickedIntervalNumber + 1;
			} else {
				tnear = interval -> xmin;
				clickedLeftBoundary = clickedIntervalNumber;
			}
		} else {
			/*
				The user clicked outside the time domain of the intervals.
				This can occur when we are grouped with a longer time function.
			*/
			our selectedTier = clickedTierNumber;
			return FunctionEditor_UPDATE_NEEDED;
		}
	} else {
		const integer clickedPointNumber = AnyTier_timeToNearestIndex (textTier->asAnyTier(), xclick);
		if (clickedPointNumber != 0) {
			const TextPoint point = textTier -> points.at [clickedPointNumber];
			tnear = point -> number;
		}
	}
	Melder_assert (! (intervalTier && clickedLeftBoundary == 0));

	/*
		Where did the user click?
	*/
	const bool nearBoundaryOrPoint = ( isdefined (tnear) && fabs (Graphics_dxWCtoMM (our graphics.get(), xclick - tnear)) < 1.5 );
	const integer numberOfTiers = grid -> tiers->size;
	const bool nearCursorCircle = ( our startSelection == our endSelection && Graphics_distanceWCtoMM (our graphics.get(), xclick, yWC,
		our startSelection, (numberOfTiers + 1 - clickedTierNumber) * soundY / numberOfTiers - Graphics_dyMMtoWC (our graphics.get(), 1.5)) < 1.5 );

	/*
		Find out whether this is a click or a drag.
	*/
	bool drag = false;
	while (Graphics_mouseStillDown (our graphics.get())) {
		double x, y;
		Graphics_getMouseLocation (our graphics.get(), & x, & y);
		if (x < our startWindow)
			x = our startWindow;
		if (x > our endWindow)
			x = our endWindow;
		if (fabs (Graphics_dxWCtoMM (our graphics.get(), x - xclick)) > 1.5) {
			drag = true;
			break;
		}
	}

	if (nearBoundaryOrPoint) {
		/*
			Possibility 1: the user clicked near a boundary or point.
			Select or drag it.
		*/
		if (intervalTier && (clickedLeftBoundary < 2 || clickedLeftBoundary > intervalTier -> intervals.size)) {
			/*
				Ignore click on left edge of first interval or right edge of last interval.
			*/
			our selectedTier = clickedTierNumber;
		} else if (drag) {
			/*
				The tier that has been clicked becomes the new selected tier.
				This has to be done before the next Update, i.e. also before do_dragBoundary!
			*/
			our selectedTier = clickedTierNumber;
			do_dragBoundary (this, tnear, clickedTierNumber, shiftKeyPressed);
			return FunctionEditor_NO_UPDATE_NEEDED;
		} else {
			/*
				If the user clicked on an unselected boundary or point, we select it.
			*/
			if (shiftKeyPressed) {
				if (tnear > 0.5 * (our startSelection + our endSelection))
					our endSelection = tnear;
				else
					our startSelection = tnear;
			} else {
				our startSelection = our endSelection = tnear;   // move cursor so that the boundary or point is selected
			}
			our selectedTier = clickedTierNumber;
		}
	} else if (nearCursorCircle) {
		/*
			Possibility 2: the user clicked near the cursor circle.
			Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
			because we are not 'nearBoundaryOrPoint'.
		*/
		insertBoundaryOrPoint (this, clickedTierNumber, our startSelection, our startSelection, false);
		our selectedTier = clickedTierNumber;
		FunctionEditor_marksChanged (this, true);
		Editor_broadcastDataChanged (this);
		if (drag)
			Graphics_waitMouseUp (our graphics.get());
		return FunctionEditor_NO_UPDATE_NEEDED;
	} else {
		/*
			Possibility 3: the user clicked in empty space.
		*/
		if (intervalTier) {
			our startSelection = tmin_;
			our endSelection = tmax_;
		}
		selectedTier = clickedTierNumber;
	}
	if (drag)
		Graphics_waitMouseUp (our graphics.get());
	return FunctionEditor_UPDATE_NEEDED;
}

bool structFormantPathEditor :: v_clickB (double t, double yWC) {
	const double soundY = _FormantPathEditor_computeSoundY (this);
	if (yWC > soundY) {   // clicked in sound part?
		if (t < our endWindow) {
			our startSelection = t;
			if (our startSelection > our endSelection)
				std::swap (our startSelection, our endSelection);
			return FunctionEditor_UPDATE_NEEDED;
		} else {
			return structTimeSoundEditor :: v_clickB (t, yWC);
		}
	}
	const integer clickedTierNumber = _FormantPathEditor_yWCtoTier (this, yWC);
	double tmin_, tmax_;
	_FormantPathEditor_timeToInterval (this, t, clickedTierNumber, & tmin_, & tmax_);
	our startSelection = ( t - tmin_ < tmax_ - t ? tmin_ : tmax_ );   // to nearest boundary
	if (our startSelection > our endSelection)
		std::swap (our startSelection, our endSelection);
	return FunctionEditor_UPDATE_NEEDED;
}

bool structFormantPathEditor :: v_clickE (double t, double yWC) {
	const double soundY = _FormantPathEditor_computeSoundY (this);
	if (yWC > soundY) {   // clicked in sound part?
		our endSelection = t;
		if (our startSelection > our endSelection)
			std::swap (our startSelection, our endSelection);
		return FunctionEditor_UPDATE_NEEDED;
	}
	const integer clickedTierNumber = _FormantPathEditor_yWCtoTier (this, yWC);
	double tmin_, tmax_;
	_FormantPathEditor_timeToInterval (this, t, clickedTierNumber, & tmin_, & tmax_);
	our endSelection = ( t - tmin_ < tmax_ - t ? tmin_ : tmax_ );
	if (our startSelection > our endSelection)
		std::swap (our startSelection, our endSelection);
	return FunctionEditor_UPDATE_NEEDED;
}

void structFormantPathEditor :: v_clickSelectionViewer (double xWC, double yWC) {
	/*
		On which of the modelers was the click?
	*/
	FormantModelerList fml = formantModelerList.get();
	integer numberOfRows, numberOfColums;
	FormantModelerList_getMatrixGridLayout (fml, & numberOfRows, & numberOfColums);
	integer numberOfVisible = FormantPathEditor_getNumberOfVisible (this);
	const integer icol = 1 + (int) (xWC * numberOfColums);
	if (icol < 1 || icol > numberOfColums)
		return;
	const integer irow = 1 + (int) ((1.0 - yWC) * numberOfRows);
	if (irow < 1 || irow > numberOfRows)
		return;
	integer index = (irow - 1) * numberOfColums + icol; // left-to-right, top-to-bottom
	if (index > 0 && index <= numberOfVisible) {
		our selectedModeler = FormantPathEditor_selectModelerFromIndexInMatrixGrid (this, index);
		/*
			Toggle selection
		*/
		fml -> drawingSpecification -> selectedModeler = ( (fml -> drawingSpecification -> selectedModeler > 0 && fml -> drawingSpecification -> selectedModeler == our selectedModeler) ? 0 : our selectedModeler );
		if (our shiftKeyPressed) {
			fml -> drawingSpecification -> pathModeler = our selectedModeler;
			fml -> drawingSpecification -> selectedModeler = 0; // path has priority
			Editor_save (this, U"insert interval by selection viewer");
			FormantPath formantPath = (FormantPath) our data;
			if (FormantPathEditor_setPathTierLabel (this))
				FormantPath_replaceFrames (formantPath, fml -> xmin, fml -> xmax, our selectedModeler);
			FormantPathEditor_unmarkParameterChange (this);
		}
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
	EditorMenu_addCommand (menu, U"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW, menu_cb_SelectPreviousTier);
	EditorMenu_addCommand (menu, U"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW, menu_cb_SelectNextTier);
	EditorMenu_addCommand (menu, U"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_SelectPreviousInterval);
	EditorMenu_addCommand (menu, U"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_SelectNextInterval);
	EditorMenu_addCommand (menu, U"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_ExtendSelectPreviousInterval);
	EditorMenu_addCommand (menu, U"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_ExtendSelectNextInterval);
}

void structFormantPathEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our v_hasAnalysis () && our p_spectrogram_show && (our d_longSound.data || our d_sound.data)) {
		const double soundY2 = _FormantPathEditor_computeSoundY2 (this);
		//Graphics_highlight (our graphics.get(), left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_highlight (our graphics.get(), left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_highlight (our graphics.get(), left, right, bottom, top);
	}
}

void structFormantPathEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	if (our v_hasAnalysis () && our p_spectrogram_show && (our d_longSound.data || our d_sound.data)) {
		const double soundY2 = _FormantPathEditor_computeSoundY2 (this);
		//Graphics_unhighlight (our graphics.get(), left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_unhighlight (our graphics.get(), left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_unhighlight (our graphics.get(), left, right, bottom, top);
	}
}

double structFormantPathEditor :: v_getBottomOfSoundArea () {
	return _FormantPathEditor_computeSoundY (this);
}

double structFormantPathEditor :: v_getBottomOfSoundAndAnalysisArea () {
	return _FormantPathEditor_computeSoundY (this);
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

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath) {
	try {
		autoFormantPathEditor me = Thing_new (FormantPathEditor);
		
		TimeSoundAnalysisEditor_init (me.get(), title, formantPath, formantPath -> sound.get(), false);
		my pathGridView = TextGridView_create (formantPath -> path.get());
		my pathTierNumber = TextGridView_getViewTierNumber (my pathGridView.get(), formantPath -> pathTierNumber);
		if (my p_modeler_numberOfParametersPerTrack == U"")
			pref_str32cpy2(my p_modeler_numberOfParametersPerTrack, my pref_modeler_numberOfParametersPerTrack (), my default_modeler_numberOfParametersPerTrack ());
		if (my p_formant_default_colour == U"")
			pref_str32cpy2 (my p_formant_default_colour, my pref_formant_default_colour (), my default_formant_default_colour ());
		if (my p_formant_path_colour == U"")
			pref_str32cpy2 (my p_formant_path_colour, my pref_formant_path_colour (), my default_formant_path_colour ());
		if (my p_formant_selected_colour == U"")
			pref_str32cpy2 (my p_formant_selected_colour, my pref_formant_selected_colour (), my default_formant_selected_colour ());
		my formantModelerList = FormantPathEditor_to_FormantModelerList (me.get(), my startWindow, my endWindow, my p_modeler_numberOfParametersPerTrack);
		my selectedTier = my pathTierNumber;
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
