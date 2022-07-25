/* TextGridArea.cpp
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

#include "TextGridArea.h"
#include "AnyTextGridEditor.h"   // BUG: should not be included
#include "TextGrid_Sound.h"
#include "EditorM.h"

Thing_implement (TextGridArea, FunctionArea, 0);

#include "enums_getText.h"
#include "TextGridArea_enums.h"
#include "enums_getValue.h"
#include "TextGridArea_enums.h"

#include "Prefs_define.h"
#include "TextGridArea_prefs.h"
#include "Prefs_install.h"
#include "TextGridArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "TextGridArea_prefs.h"


#pragma mark - TextGridArea drawing

void structTextGridArea :: v_specializedHighlightBackground () const {
	Melder_assert (our textGrid());
	const integer numberOfTiers = our textGrid() -> tiers->size;
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		const Function anyTier = our textGrid() -> tiers->at [itier];
		if (anyTier -> classInfo != classIntervalTier)
			continue;
		IntervalTier tier = (IntervalTier) anyTier;
		/*
			Highlight interval: yellow (selected) or green (matching label).
		*/
		const integer selectedInterval = ( itier == our selectedTier ? getSelectedInterval (this) : 0 );
		const integer numberOfIntervals = tier -> intervals.size;
		for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			/* mutable clip */ double startInterval = interval -> xmin, endInterval = interval -> xmax;
			if (endInterval > our startWindow() && startInterval < our endWindow()) {   // interval visible?
				const bool intervalIsSelected = ( iinterval == selectedInterval );
				const bool labelDoesMatch = Melder_stringMatchesCriterion (interval -> text.get(),
						our instancePref_greenMethod(), our instancePref_greenString(), true);
				Melder_clipLeft (our startWindow(), & startInterval);
				Melder_clipRight (& endInterval, our endWindow());
				const double bottom = 1.0 - double (itier) / numberOfTiers;
				const double top = 1.0 - double (itier - 1) / numberOfTiers;
				if (labelDoesMatch) {
					Graphics_setColour (our graphics(), Melder_LIME);
					Graphics_fillRectangle (our graphics(), startInterval, endInterval, bottom, top);
				}
				if (intervalIsSelected) {
					Graphics_setColour (our graphics(), Melder_YELLOW);
					Graphics_fillRectangle (our graphics(),
						labelDoesMatch ? 0.85 * startInterval + 0.15 * endInterval : startInterval,
						labelDoesMatch ? 0.15 * startInterval + 0.85 * endInterval : endInterval,
						labelDoesMatch ? 0.85 * bottom + 0.15 * top : bottom,
						labelDoesMatch ? 0.15 * bottom + 0.85 * top : top
					);
				}
			}
		}
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
}


#pragma mark - TextGridArea File menu

static void menu_cb_SaveWholeTextGridAsTextFile (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save whole TextGrid as text file", nullptr)
		Melder_sprint (defaultName,300, my textGrid() -> name.get(), U".TextGrid");
	EDITOR_DO_SAVE
		Data_writeToTextFile (my textGrid(), file);
	EDITOR_END
}
void structTextGridArea :: v_createMenuItems_file (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"Save TextGrid to disk:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Save whole TextGrid as text file...", 'S',
			menu_cb_SaveWholeTextGridAsTextFile, this);
	FunctionAreaMenu_addCommand (menu, U"-- after TextGrid save --", 0, nullptr, this);
}


#pragma mark - TextGridArea View menu

static void _TextGridArea_timeToInterval (TextGridArea me, double t, integer tierNumber,
	double *out_tmin, double *out_tmax)
{
	Melder_assert (isdefined (t));
	const Function tier = my textGrid() -> tiers->at [tierNumber];
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (tier, & intervalTier, & textTier);
	if (intervalTier) {
		integer iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval == 0) {
			if (t < my tmin()) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals.size;
			}
		}
		Melder_assert (iinterval >= 1);
		Melder_assert (iinterval <= intervalTier -> intervals.size);
		const TextInterval interval = intervalTier -> intervals.at [iinterval];
		*out_tmin = interval -> xmin;
		*out_tmax = interval -> xmax;
	} else {
		const integer n = textTier -> points.size;
		if (n == 0) {
			*out_tmin = my tmin();
			*out_tmax = my tmax();
		} else {
			integer ipointleft = AnyTier_timeToLowIndex (textTier->asAnyTier(), t);
			*out_tmin = ( ipointleft == 0 ? my tmin() : textTier -> points.at [ipointleft] -> number );
			*out_tmax = ( ipointleft == n ? my tmax() : textTier -> points.at [ipointleft + 1] -> number );
		}
	}
	Melder_clipLeft (my tmin(), out_tmin);
	Melder_clipRight (out_tmax, my tmax());
}
static void do_selectAdjacentTier (TextGridArea me, bool previous) {
	const integer n = my textGrid() -> tiers->size;
	if (n >= 2) {
		my selectedTier = ( previous ?
				my selectedTier > 1 ? my selectedTier - 1 : n :
				my selectedTier < n ? my selectedTier + 1 : 1 );
		double startInterval, endInterval;
		_TextGridArea_timeToInterval (me, my startSelection(), my selectedTier, & startInterval, & endInterval);
		my setSelection (startInterval, endInterval);
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
	}
}
static void menu_cb_SelectPreviousTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, true);
}
static void menu_cb_SelectNextTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, false);
}
static void do_selectAdjacentInterval (TextGridArea me, bool previous, bool shift) {
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > my textGrid() -> tiers->size)
		return;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		const integer n = intervalTier -> intervals.size;
		if (n >= 2) {
			integer iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection());
			if (shift) {
				const integer binterval = IntervalTier_timeToIndex (intervalTier, my startSelection());
				integer einterval = IntervalTier_timeToIndex (intervalTier, my endSelection());
				if (my endSelection() == intervalTier -> xmax)
					einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					const TextInterval interval = intervalTier -> intervals.at [iinterval];
					my setSelection (interval -> xmin, interval -> xmax);
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							const TextInterval interval = intervalTier -> intervals.at [einterval - 1];
							my setSelection (my startSelection(), interval -> xmin);
						}
					} else if (binterval > 1) {
						const TextInterval interval = intervalTier -> intervals.at [binterval - 1];
						my setSelection (interval -> xmin, my endSelection());
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							const TextInterval interval = intervalTier -> intervals.at [binterval];
							my setSelection (interval -> xmax, my endSelection());
						}
					} else if (einterval <= n) {
						const TextInterval interval = intervalTier -> intervals.at [einterval];
						my setSelection (my startSelection(), interval -> xmax);
					}
				}
			} else {
				iinterval = ( previous ?
						iinterval > 1 ? iinterval - 1 : n :
						iinterval < n ? iinterval + 1 : 1 );
				const TextInterval interval = intervalTier -> intervals.at [iinterval];
				my setSelection (interval -> xmin, interval -> xmax);
			}
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToView()
			FunctionEditor_scrollToView (my functionEditor(), iinterval == n ? my startSelection() : iinterval == 1 ? my endSelection() : 0.5 * (my startSelection() + my endSelection()));
		}
	} else {
		const integer n = textTier -> points.size;
		if (n >= 2) {
			integer ipoint = AnyTier_timeToHighIndex (textTier->asAnyTier(), my startSelection());
			ipoint = ( previous ?
					ipoint > 1 ? ipoint - 1 : n :
					ipoint < n ? ipoint + 1 : 1 );
			const TextPoint point = textTier -> points.at [ipoint];
			my setSelection (point -> number, point -> number);
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToView()
			FunctionEditor_scrollToView (my functionEditor(), my startSelection());
		}
	}
}
static void menu_cb_SelectPreviousInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, false);
}
static void menu_cb_SelectNextInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, false);
}
static void menu_cb_ExtendSelectPreviousInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, true);
}
static void menu_cb_ExtendSelectNextInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, true);
}
void structTextGridArea :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- TextGrid view domain --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"View TextGrid time domain:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW,
			menu_cb_SelectPreviousTier, this);
	FunctionAreaMenu_addCommand (menu, U"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW,
			menu_cb_SelectNextTier, this);
	FunctionAreaMenu_addCommand (menu, U"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW,
			menu_cb_SelectPreviousInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW,
			menu_cb_SelectNextInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW,
			menu_cb_ExtendSelectPreviousInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW,
			menu_cb_ExtendSelectNextInterval, this);
}


#pragma mark - TextGridArea Draw menu

static void menu_cb_DrawVisibleTextGrid (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		Editor_openPraatPicture (my functionEditor());
		TextGrid_Sound_draw (my textGrid(), nullptr, my functionEditor() -> pictureGraphics,
				my startWindow(), my endWindow(), true, my instancePref_useTextStyles(), garnish);
		FunctionEditor_garnish (my functionEditor());
		Editor_closePraatPicture (my functionEditor());
	EDITOR_END
}
static void addTextGridDrawMenu (TextGridArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- TextGrid draw --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Draw TextGrid to picture window:", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Draw visible TextGrid...", 0, menu_cb_DrawVisibleTextGrid, me);
}


#pragma mark - TextGridArea Extract menu

static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (my endSelection() <= my startSelection())
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection(), my endSelection(), true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (my endSelection() <= my startSelection())
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection(), my endSelection(), false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void addTextGridExtractMenu (TextGridArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- TextGrid extract --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Extract TextGrid to objects window:", 0, nullptr, me);
	my extractSelectedTextGridPreserveTimesButton = FunctionAreaMenu_addCommand (menu, U"Extract selected TextGrid (preserve times)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes, me);
	my extractSelectedTextGridTimeFromZeroButton = FunctionAreaMenu_addCommand (menu, U"Extract selected TextGrid (time from 0)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero, me);
}


#pragma mark - TextGridArea all menus

void structTextGridArea :: v_createMenus () {
	EditorMenu textGridMenu = Editor_addMenu (our functionEditor(), U"TextGrid", 0);
	addTextGridDrawMenu (this, our functionEditor() -> drawMenu);
	addTextGridDrawMenu (this, textGridMenu);
	addTextGridExtractMenu (this, our functionEditor() -> extractMenu);
	addTextGridExtractMenu (this, textGridMenu);
}
void structTextGridArea :: v_updateMenuItems () {
	TextGridArea_Parent :: v_updateMenuItems ();
	GuiThing_setSensitive (extractSelectedTextGridPreserveTimesButton, our endSelection() > our startSelection());
	GuiThing_setSensitive (extractSelectedTextGridTimeFromZeroButton,  our endSelection() > our startSelection());
}

/* End of file TextGridArea.cpp */
