/* DataGui.cpp
 *
 * Copyright (C) 1992-2024 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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

#include "DataGui.h"
#include "ScriptEditor.h"
#include "EditorM.h"

#include "enums_getText.h"
#include "DataGui_enums.h"
#include "enums_getValue.h"
#include "DataGui_enums.h"

Thing_implement_pureVirtual (DataGui, Thing, 0);

#include "Prefs_define.h"
#include "DataGui_prefs.h"
#include "Prefs_install.h"
#include "DataGui_prefs.h"
#include "Prefs_copyToInstance.h"
#include "DataGui_prefs.h"

BOOLEAN_VARIABLE (v_form_pictureWindow__eraseFirst)
void structDataGui :: v_form_pictureWindow (EditorCommand cmd) {
	COMMENT (U"Picture window:")
	BOOLEAN_FIELD (v_form_pictureWindow__eraseFirst, U"Erase first", true)
}
void structDataGui :: v_ok_pictureWindow (EditorCommand cmd) {
	SET_BOOLEAN (v_form_pictureWindow__eraseFirst, our instancePref_picture_eraseFirst())
}
void structDataGui :: v_do_pictureWindow (EditorCommand /* cmd */) {
	our setInstancePref_picture_eraseFirst (v_form_pictureWindow__eraseFirst);
}

OPTIONMENU_ENUM_VARIABLE (kDataGui_writeNameAtTop, v_form_pictureMargins__writeNameAtTop)
void structDataGui :: v_form_pictureMargins (EditorCommand cmd) {
	COMMENT (U"Margins:")
	OPTIONMENU_ENUM_FIELD (kDataGui_writeNameAtTop, v_form_pictureMargins__writeNameAtTop,
			U"Write name at top", kDataGui_writeNameAtTop::DEFAULT)
}
void structDataGui :: v_ok_pictureMargins (EditorCommand cmd) {
	SET_ENUM (v_form_pictureMargins__writeNameAtTop, kDataGui_writeNameAtTop, our classPref_picture_writeNameAtTop())
}
void structDataGui :: v_do_pictureMargins (EditorCommand /* cmd */) {
	our setClassPref_picture_writeNameAtTop (v_form_pictureMargins__writeNameAtTop);
}

/*
	The following definitions cannot directly refer to existing Melder colours such as Melder_WHITE,
	because that might lead to an initialization race.
	Therefore, all of these definitions have to invoke their own MelderColour initialization.
*/
MelderColour
	DataGuiColour_WINDOW_BACKGROUND = MelderColour (0.95, 0.95, 0.90),   // cream
	DataGuiColour_AREA_BACKGROUND = MelderColour (1.0, 1.0, 1.0),   // whitish; DON'T REPLACE with Melder_WHITE
	DataGuiColour_EDITABLE = MelderColour (0.0, 0.4, 0.5),   // greenish blue (almost teal)
	DataGuiColour_EDITABLE_FRAME = MelderColour (0.0, 0.8, 1.0),   // same hue as the lines
	DataGuiColour_EDITABLE_SELECTED = MelderColour (0.75, 0.35, 0.0),   // orangy
	DataGuiColour_NONEDITABLE = MelderColour (0.25),   // dark grey
	DataGuiColour_NONEDITABLE_FRAME = MelderColour (0.85),   // same hue as the lines; DON'T REPLACE with Melder_SILVER
	DataGuiColour_NONEDITABLE_SELECTED = MelderColour (0.8, 0.0, 0.0);   // reddish

void DataGui_openPraatPicture (DataGui me) {
	my _pictureGraphics = praat_picture_datagui_open (my instancePref_picture_eraseFirst());
}
void DataGui_closePraatPicture (DataGui me) {
	const Daata theDataWhoseNameWeMayWantToWriteAtTheTop = my boss() -> data();
			// same as the title of the window (may not be precise, if we are editing two objects at a time)
	if (theDataWhoseNameWeMayWantToWriteAtTheTop && my classPref_picture_writeNameAtTop() != kDataGui_writeNameAtTop::NO_) {
		Graphics_setNumberSignIsBold (my pictureGraphics(), false);
		Graphics_setPercentSignIsItalic (my pictureGraphics(), false);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics(), false);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics(), false);
		Graphics_textTop (my pictureGraphics(),
			my classPref_picture_writeNameAtTop() == kDataGui_writeNameAtTop::FAR_,
			theDataWhoseNameWeMayWantToWriteAtTheTop -> name.get()
		);
		Graphics_setNumberSignIsBold (my pictureGraphics(), true);
		Graphics_setPercentSignIsItalic (my pictureGraphics(), true);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics(), true);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics(), true);
	}
	praat_picture_datagui_close ();
}

/* End of file DataGui.cpp */
