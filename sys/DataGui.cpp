/* DataGui.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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
#include "Editor.h"
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
	LABEL (U"Picture window:")
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
	LABEL (U"Margins:")
	OPTIONMENU_ENUM_FIELD (kDataGui_writeNameAtTop, v_form_pictureMargins__writeNameAtTop,
			U"Write name at top", kDataGui_writeNameAtTop::DEFAULT)
}
void structDataGui :: v_ok_pictureMargins (EditorCommand cmd) {
	SET_ENUM (v_form_pictureMargins__writeNameAtTop, kDataGui_writeNameAtTop, our classPref_picture_writeNameAtTop())
}
void structDataGui :: v_do_pictureMargins (EditorCommand /* cmd */) {
	our setClassPref_picture_writeNameAtTop (v_form_pictureMargins__writeNameAtTop);
}

MelderColour structDataGui :: Colour_BACKGROUND() { return Melder_WHITE; }
MelderColour structDataGui :: Colour_EDITABLE_LINES() { return Melder_BLUE; }
MelderColour structDataGui :: Colour_EDITABLE_FRAME() { return Melder_CYAN; }
MelderColour structDataGui :: Colour_NONEDITABLE_FOREGROUND() { return MelderColour (0.25); }

/* End of file DataGui.cpp */
