/* AnyTextGridEditor.cpp
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
/* Erez Volk added FLAC support in 2007 */

#include "AnyTextGridEditor.h"
#include "EditorM.h"
#include "SoundEditor.h"
#include "Sound_and_MixingMatrix.h"
#include "Sound_and_Spectrogram.h"
#include "TextGrid_Sound.h"
#include "SpeechSynthesizer_and_TextGrid.h"

Thing_implement (AnyTextGridEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "AnyTextGridEditor_prefs.h"
#include "Prefs_install.h"
#include "AnyTextGridEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "AnyTextGridEditor_prefs.h"

void structAnyTextGridEditor :: v1_info () {
	AnyTextGridEditor_Parent :: v1_info ();
	MelderInfo_writeLine (U"Selected tier: ", our textGridArea() -> selectedTier);
	MelderInfo_writeLine (U"TextGrid uses text styles: ", our textGridArea() -> instancePref_useTextStyles());
	MelderInfo_writeLine (U"TextGrid font size: ", our textGridArea() -> instancePref_fontSize());
	MelderInfo_writeLine (U"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (our textGridArea() -> instancePref_alignment()));
}

/********** METHODS **********/

/*
 * The main invariant of the TextGridEditor is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */


/***** EDIT MENU *****/

#ifndef macintosh
static void menu_cb_Cut (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_cut (my textArea);
}
static void menu_cb_Copy (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_copy (my textArea);
}
static void menu_cb_Paste (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_paste (my textArea);
}
static void menu_cb_Erase (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_remove (my textArea);
}
#endif

static void menu_cb_ConvertToBackslashTrigraphs (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Convert to Backslash Trigraphs");
	TextGrid_convertToBackslashTrigraphs (my textGrid());
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ConvertToUnicode (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Convert to Unicode");
	TextGrid_convertToUnicode (my textGrid());
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}



/***** SEARCH MENU *****/

static void findInTier (AnyTextGridEditor me) {
	checkTierSelection (my textGridArea().get(), U"find a text");
	Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
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
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, my startSelection);
					GuiText_setSelection (my textArea, position - text, position - text + str32len (my findString.get()));
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
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, point -> number);
					GuiText_setSelection (my textArea, position - text, position - text + str32len (my findString.get()));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void do_find (AnyTextGridEditor me) {
	if (my findString) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
		const char32 * const position = str32str (& label [right], my findString.get());   // CRLF BUG?
		if (position) {
			GuiText_setSelection (my textArea, position - label.get(), position - label.get() + str32len (my findString.get()));
		} else {
			findInTier (me);
		}
	}
}

static void menu_cb_Find (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find text", nullptr)
		TEXTFIELD (findString, U"Text", U"", 3)
	EDITOR_OK
	EDITOR_DO
		my findString = Melder_dup (findString);
		do_find (me);
	EDITOR_END
}

static void menu_cb_FindAgain (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_find (me);
}

static void checkSpellingInTier (AnyTextGridEditor me) {
	checkTierSelection (my textGridArea().get(), U"check spelling");
	const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		integer iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			conststring32 text = interval -> text.get();
			if (text) {
				integer position = 0;
				conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, my startSelection);
					GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		const TextTier tier = (TextTier) anyTier;
		integer ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection) + 1;
		while (ipoint <= tier -> points.size) {
			TextPoint point = tier -> points.at [ipoint];
			conststring32 text = point -> mark.get();
			if (text) {
				integer position = 0;
				conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = my endSelection = point -> number;
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, point -> number);
					GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void menu_cb_CheckSpelling (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
		integer position = right;
		conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.get(), & position);
		if (notAllowed) {
			GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
		} else {
			checkSpellingInTier (me);
		}
	}
}

static void menu_cb_CheckSpellingInInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
		integer position = right;
		conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.get(), & position);
		if (notAllowed)
			GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
	}
}

static void menu_cb_AddToUserDictionary (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		const autostring32 word = GuiText_getSelection (my textArea);
		SpellingChecker_addNewWord (my spellingChecker, word.get());
		Editor_broadcastDataChanged (me);
	}
}

/***** HELP MENU *****/

static void menu_cb_TextGridEditorHelp (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"TextGridEditor") }
static void menu_cb_AboutSpecialSymbols (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"Special symbols") }
static void menu_cb_PhoneticSymbols (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"Phonetic symbols") }
static void menu_cb_AboutTextStyles (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"Text styles") }

void structAnyTextGridEditor :: v_createMenus () {
	AnyTextGridEditor_Parent :: v_createMenus ();
	EditorMenu menu;

	#ifndef macintosh
		Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
		Editor_addCommand (this, U"Edit", U"Cut text", 'X', menu_cb_Cut);
		Editor_addCommand (this, U"Edit", U"Cut", Editor_HIDDEN, menu_cb_Cut);
		Editor_addCommand (this, U"Edit", U"Copy text", 'C', menu_cb_Copy);
		Editor_addCommand (this, U"Edit", U"Copy", Editor_HIDDEN, menu_cb_Copy);
		Editor_addCommand (this, U"Edit", U"Paste text", 'V', menu_cb_Paste);
		Editor_addCommand (this, U"Edit", U"Paste", Editor_HIDDEN, menu_cb_Paste);
		Editor_addCommand (this, U"Edit", U"Erase text", 0, menu_cb_Erase);
		Editor_addCommand (this, U"Edit", U"Erase", Editor_HIDDEN, menu_cb_Erase);
	#endif
	Editor_addCommand (this, U"Edit", U"-- encoding --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Convert entire TextGrid to backslash trigraphs", 0, menu_cb_ConvertToBackslashTrigraphs);
	Editor_addCommand (this, U"Edit", U"Convert entire TextGrid to Unicode", 0, menu_cb_ConvertToUnicode);
	Editor_addCommand (this, U"Edit", U"-- search --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Find...", 'F', menu_cb_Find);
	Editor_addCommand (this, U"Edit", U"Find again", 'G', menu_cb_FindAgain);

	if (our spellingChecker) {
		menu = Editor_addMenu (this, U"Spell", 0);
		EditorMenu_addCommand (menu, U"Check spelling in tier", GuiMenu_COMMAND | GuiMenu_OPTION | 'L', menu_cb_CheckSpelling);
		EditorMenu_addCommand (menu, U"Check spelling in interval", 0, menu_cb_CheckSpellingInInterval);
		EditorMenu_addCommand (menu, U"-- edit lexicon --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Add selected word to user dictionary", 0, menu_cb_AddToUserDictionary);
	}
}

void structAnyTextGridEditor :: v_createMenuItems_help (EditorMenu menu) {
	AnyTextGridEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"TextGridEditor help", '?', menu_cb_TextGridEditorHelp);
	EditorMenu_addCommand (menu, U"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, U"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, U"About text styles", 0, menu_cb_AboutTextStyles);
}

/***** CHILDREN *****/

static void gui_text_cb_changed (AnyTextGridEditor me, GuiTextEvent /* event */) {
	//Melder_casual (U"gui_text_cb_change 1 in editor ", Melder_pointer (me));
	if (my suppressRedraw) return;   /* Prevent infinite loop if 'draw' method or Editor_broadcastChange calls GuiText_setString. */
	//Melder_casual (U"gui_text_cb_change 2 in editor ", me);
	if (my textGridArea() -> selectedTier) {
		autostring32 text = GuiText_getString (my textArea);
		IntervalTier intervalTier;
		TextTier textTier;
		AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my textGridArea() -> selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer selectedInterval = getSelectedInterval (my textGridArea().get());
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals.at [selectedInterval];
				//Melder_casual (U"gui_text_cb_change 3 in editor ", Melder_pointer (me));
				TextInterval_setText (interval, text.get());
				//Melder_casual (U"gui_text_cb_change 4 in editor ", Melder_pointer (me));
				//FunctionEditor_redraw (me); TRY OUT 2022-06-12
				//Melder_casual (U"gui_text_cb_change 5 in editor ", Melder_pointer (me));
				Editor_broadcastDataChanged (me);
				//Melder_casual (U"gui_text_cb_change 6 in editor ", Melder_pointer (me));
			}
		} else {
			const integer selectedPoint = getSelectedPoint (my textGridArea().get());
			if (selectedPoint) {
				TextPoint point = textTier -> points.at [selectedPoint];
				point -> mark. reset();
				if (Melder_findInk (text.get()))   // any visible characters?
					point -> mark = Melder_dup_f (text.get());
				Editor_broadcastDataChanged (me);
			}
		}
	}
}

void structAnyTextGridEditor :: v_createChildren () {
	AnyTextGridEditor_Parent :: v_createChildren ();
	if (our textArea)
		GuiText_setChangedCallback (our textArea, gui_text_cb_changed, this);
}

void structAnyTextGridEditor :: v1_dataChanged () {
	/*
		Perform a minimal selection change.
		Most changes will involve intervals and boundaries; however, there may also be tier removals.
		Do a simple guess.
	*/
	Melder_clipRight (& our textGridArea() -> selectedTier, our textGrid() -> tiers->size);   // crucial: before v_updateText (bug 2022-07-23)!
	AnyTextGridEditor_Parent :: v1_dataChanged ();   // does all the updating
}

/********** IPA CHART **********/

static const conststring32 characters [12] [10] = {
	{ U"ɑ", U"ɐ", U"ɒ", U"æ", U"ʙ", U"ɓ", U"β", U"ç", U"ɕ", U"ð" },
	{ U"ɗ", U"ɖ", U"ɛ", U"ɜ", U"ə", U"ɢ", U"ɠ", U"ʛ", U"ɣ", U"ɤ" },
	{ U"ˠ", U"ʜ", U"ɦ", U"ħ", U"ʰ", U"ɧ", U"ɪ", U"ɨ", U"ı", U"ɟ" },
	{ U"ʝ", U"ʄ", U"ᴊ", U"ʲ", U"ʟ", U"ɬ", U"ɭ", U"ɮ", U"ɫ", U"ˡ" },
	{ U"ɰ", U"ɯ", U"ɱ", U"ɴ", U"ŋ", U"ɲ", U"ɳ", U"ⁿ", U"ɔ", U"ɵ" },

	{ U"ø", U"œ", U"ɶ", U"ɸ", U"ɹ", U"ʀ", U"ʁ", U"ɾ", U"ɽ", U"ɺ" },
	{ U"ɻ", U"ʃ", U"ʂ", U"θ", U"ʈ", U"ʉ", U"ʊ", U"ʌ", U"ʋ", U"ʍ" },
	{ U"ʷ", U"χ", U"ʎ", U"ʏ", U"ɥ", U"ʒ", U"ʐ", U"ʑ", U"ˑ", U"ː" },
	{ U"ʔ", U"ʡ", U"ʕ", U"ʢ", U"ˤ", U"ǃ", U"ʘ", U"ǀ", U"ǁ", U"ǂ" },
	{ U"ʤ", U"ɘ", U"ɚ", U"ɝ", U"ʱ", U"ˢ", U"ʧ", U"ɞ", U"ʦ", U"ʣ" },

	{ U"ʨ", U"ʥ", U"z̊", U"z̥", U"z̤", U"z̰", U"z̪", U"z̩", U"z̝", U"z̞" },
	{ U"ý", U"ȳ", U"ỳ", U"ÿ", U"ỹ", U"o̟", U"o̱", U"o̹", U"o̜", U"t̚" },
};

void structAnyTextGridEditor :: v_drawSelectionViewer () {
	Graphics_setWindow (our graphics.get(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFont (our graphics.get(), kGraphics_font::TIMES);
	const double pointsPerMillimetre = 72.0 / 25.4;
	const double cellWidth_points = Graphics_dxWCtoMM (our graphics.get(), 1.0) * pointsPerMillimetre;
	const double cellHeight_points = Graphics_dyWCtoMM (our graphics.get(), 1.0) * pointsPerMillimetre;
	const double fontSize = std::min (0.8 * cellHeight_points, 1.2 * cellWidth_points);
	Graphics_setFontSize (our graphics.get(), fontSize);
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer irow = 1; irow <= 12; irow ++)
		for (integer icol = 1; icol <= 10; icol ++)
			Graphics_text (our graphics.get(), 0.0 + 1.0 * icol, 13.0 - 1.0 * irow, characters [irow-1] [icol-1]);
}

void structAnyTextGridEditor :: v_clickSelectionViewer (double xWC, double yWC) {
	integer rowNumber = Melder_iceiling ((1.0 - yWC) * 12.0);
	integer columnNumber = Melder_iceiling (xWC * 10.0);
	if (rowNumber < 1 || rowNumber > 12)
		return;
	if (columnNumber < 1 || columnNumber > 10)
		return;
	conststring32 character = characters [rowNumber-1] [columnNumber-1];
	character += str32len (character) - 1;
	if (our textArea) {
		integer first = 0, last = 0;
		autostring32 oldText = GuiText_getStringAndSelectionPosition (our textArea, & first, & last);
		static MelderString newText;
		MelderString_empty (& newText);
		MelderString_ncopy (& newText, oldText.get(), first);
		MelderString_append (& newText, character);
		MelderString_append (& newText, oldText.get() + last);
		if (our textGridArea() -> selectedTier != 0) {
			IntervalTier intervalTier;
			TextTier textTier;
			AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our textGridArea() -> selectedTier], & intervalTier, & textTier);
			if (intervalTier) {
				const integer selectedInterval = getSelectedInterval (our textGridArea().get());
				if (selectedInterval != 0) {
					TextInterval interval = intervalTier -> intervals.at [selectedInterval];
					TextInterval_setText (interval, newText.string);

					our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (our textArea, newText.string);
					GuiText_setSelection (our textArea, first + 1, first + 1);
					our suppressRedraw = false;

					//FunctionEditor_redraw (this); TRY OUT 2022-06-12
					Editor_broadcastDataChanged (this);
				}
			} else {
				const integer selectedPoint = getSelectedPoint (our textGridArea().get());
				if (selectedPoint != 0) {
					TextPoint point = textTier -> points.at [selectedPoint];
					point -> mark. reset();
					if (Melder_findInk (newText.string))   // any visible characters?
						point -> mark = Melder_dup_f (newText.string);

					our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (our textArea, newText.string);
					GuiText_setSelection (our textArea, first + 1, first + 1);
					our suppressRedraw = false;

					//FunctionEditor_redraw (this); TRY OUT 2022-06-12
					Editor_broadcastDataChanged (this);
				}
			}
		}
	}
}

void structAnyTextGridEditor :: v_play (double startTime, double endTime) {
	if (our soundArea())
		SoundArea_play (our soundArea().get(), startTime, endTime);
}

void structAnyTextGridEditor :: v_updateText () {
	conststring32 newText = U"";
	trace (U"selected tier ", our textGridArea() -> selectedTier);
	if (our textGridArea() -> selectedTier != 0) {
		IntervalTier intervalTier;
		TextTier textTier;
		AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our textGridArea() -> selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer iinterval = IntervalTier_timeToIndex (intervalTier, our startSelection);
			if (iinterval) {
				TextInterval interval = intervalTier -> intervals.at [iinterval];
				if (interval -> text)
					newText = interval -> text.get();
			}
		} else {
			Melder_assert (isdefined (our startSelection));
			const integer ipoint = AnyTier_hasPoint (textTier->asAnyTier(), our startSelection);
			if (ipoint) {
				TextPoint point = textTier -> points.at [ipoint];
				if (point -> mark)
					newText = point -> mark.get();
			}
		}
	}
	if (our textArea) {
		our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
		trace (U"setting new text ", newText);
		GuiText_setString (our textArea, newText);
		const integer cursor = str32len (newText);   // at end
		GuiText_setSelection (our textArea, cursor, cursor);
		our suppressRedraw = false;
	}
}

POSITIVE_VARIABLE (v_prefs_addFields__fontSize)
OPTIONMENU_ENUM_VARIABLE (kGraphics_horizontalAlignment, v_prefs_addFields__textAlignmentInIntervals)
OPTIONMENU_VARIABLE (v_prefs_addFields__useTextStyles)
OPTIONMENU_VARIABLE (v_prefs_addFields__shiftDragMultiple)
OPTIONMENU_ENUM_VARIABLE (kTextGridArea_showNumberOf, v_prefs_addFields__showNumberOf)
OPTIONMENU_ENUM_VARIABLE (kMelder_string, v_prefs_addFields__paintIntervalsGreenWhoseLabel)
SENTENCE_VARIABLE (v_prefs_addFields__theText)
void structAnyTextGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	UiField _radio_;
	POSITIVE_FIELD (v_prefs_addFields__fontSize, U"Font size (points)", our textGridArea() -> default_fontSize())
	OPTIONMENU_ENUM_FIELD (kGraphics_horizontalAlignment, v_prefs_addFields__textAlignmentInIntervals,
			U"Text alignment in intervals", kGraphics_horizontalAlignment::DEFAULT)
	OPTIONMENU_FIELD (v_prefs_addFields__useTextStyles, U"The symbols %#_^ in labels", our textGridArea() -> default_useTextStyles() + 1)
		OPTION (U"are shown as typed")
		OPTION (U"mean italic/bold/sub/super")
	OPTIONMENU_FIELD (v_prefs_addFields__shiftDragMultiple, U"With the shift key, you drag", our textGridArea() -> default_shiftDragMultiple() + 1)
		OPTION (U"a single boundary")
		OPTION (U"multiple boundaries")
	OPTIONMENU_ENUM_FIELD (kTextGridArea_showNumberOf, v_prefs_addFields__showNumberOf,
			U"Show number of", kTextGridArea_showNumberOf::DEFAULT)
	OPTIONMENU_ENUM_FIELD (kMelder_string, v_prefs_addFields__paintIntervalsGreenWhoseLabel,
			U"Paint intervals green whose label...", kMelder_string::DEFAULT)
	SENTENCE_FIELD (v_prefs_addFields__theText, U"...the text", our textGridArea() -> default_greenString())
}
void structAnyTextGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_OPTION (v_prefs_addFields__useTextStyles, our textGridArea() -> instancePref_useTextStyles() + 1)
	SET_REAL (v_prefs_addFields__fontSize, our textGridArea() -> instancePref_fontSize())
	SET_ENUM (v_prefs_addFields__textAlignmentInIntervals, kGraphics_horizontalAlignment, our textGridArea() -> instancePref_alignment())
	SET_OPTION (v_prefs_addFields__shiftDragMultiple, our textGridArea() -> instancePref_shiftDragMultiple() + 1)
	SET_ENUM (v_prefs_addFields__showNumberOf, kTextGridArea_showNumberOf, our textGridArea() -> instancePref_showNumberOf())
	SET_ENUM (v_prefs_addFields__paintIntervalsGreenWhoseLabel, kMelder_string, our textGridArea() -> instancePref_greenMethod())
	SET_STRING (v_prefs_addFields__theText, our textGridArea() -> instancePref_greenString())
}
void structAnyTextGridEditor :: v_prefs_getValues (EditorCommand /* cmd */) {
	our textGridArea() -> setInstancePref_useTextStyles (v_prefs_addFields__useTextStyles - 1);
	our textGridArea() -> setInstancePref_fontSize (v_prefs_addFields__fontSize);
	our textGridArea() -> setInstancePref_alignment (v_prefs_addFields__textAlignmentInIntervals);
	our textGridArea() -> setInstancePref_shiftDragMultiple (v_prefs_addFields__shiftDragMultiple - 1);
	our textGridArea() -> setInstancePref_showNumberOf (v_prefs_addFields__showNumberOf);
	our textGridArea() -> setInstancePref_greenMethod (v_prefs_addFields__paintIntervalsGreenWhoseLabel);
	our textGridArea() -> setInstancePref_greenString (v_prefs_addFields__theText);
}

/********** EXPORTED **********/

void AnyTextGridEditor_init (AnyTextGridEditor me, conststring32 title, TextGrid textGrid,
	SpellingChecker spellingChecker, conststring32 callbackSocket)
{
	my spellingChecker = spellingChecker;   // set before FunctionEditor_init, which may install spellingChecker menus
	my callbackSocket = Melder_dup (callbackSocket);
	FunctionEditor_init (me, title, textGrid);

	Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
	my v_updateText ();   // to reflect changed tier selection
	if (my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		if (my startWindow == my tmin)
			my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (me, false);
	}
	if (spellingChecker)
		GuiText_setSelection (my textArea, 0, 0);
	if (my soundOrLongSound() &&
		my soundOrLongSound() -> xmin == 0.0 &&
		my textGrid() -> xmin != 0.0 &&
		my textGrid() -> xmax > my soundOrLongSound() -> xmax
	)
		Melder_warning (U"The time domain of the TextGrid (starting at ",
			Melder_fixed (my textGrid() -> xmin, 6), U" seconds) does not overlap with that of the sound "
			U"(which starts at 0 seconds).\nIf you want to repair this, you can select the TextGrid "
			U"and choose “Shift times to...” from the Modify menu "
			U"to shift the starting time of the TextGrid to zero.");
}

/* End of file AnyTextGridEditor.cpp */
