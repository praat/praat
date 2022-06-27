/* FunctionArea.cpp
 *
 * Copyright (C) 2022 Paul Boersma
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

#include "FunctionArea.h"
#include "EditorM.h"

Thing_implement (FunctionArea, Thing, 0);

#include "Prefs_define.h"
#include "FunctionArea_prefs.h"
#include "Prefs_install.h"
#include "FunctionArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FunctionArea_prefs.h"

GuiMenuItem FunctionAreaMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
		FunctionAreaCommandCallback commandCallback, FunctionArea commandBoss)
{
	return DataGuiMenu_addCommand (me, itemTitle, flags, commandCallback.get(), commandBoss);
}

static bool v_form_pictureSelection__drawSelectionTimes;
static bool v_form_pictureSelection__drawSelectionHairs;
void structFunctionArea :: v_form_pictureSelection (EditorCommand cmd) {
	UiForm_addBoolean (cmd -> d_uiform.get(), & v_form_pictureSelection__drawSelectionTimes, nullptr, U"Draw selection times", true);
	UiForm_addBoolean (cmd -> d_uiform.get(), & v_form_pictureSelection__drawSelectionHairs, nullptr, U"Draw selection hairs", true);
}
void structFunctionArea :: v_ok_pictureSelection (EditorCommand cmd) {
	FunctionEditor me = (FunctionEditor) cmd -> d_editor;
	SET_BOOLEAN (v_form_pictureSelection__drawSelectionTimes, my classPref_picture_drawSelectionTimes())
	SET_BOOLEAN (v_form_pictureSelection__drawSelectionHairs, my classPref_picture_drawSelectionHairs())
}
void structFunctionArea :: v_do_pictureSelection (EditorCommand cmd) {
	FunctionEditor me = (FunctionEditor) cmd -> d_editor;
	my setClassPref_picture_drawSelectionTimes (v_form_pictureSelection__drawSelectionTimes);
	my setClassPref_picture_drawSelectionHairs (v_form_pictureSelection__drawSelectionHairs);
}


/* End of file FunctionArea.cpp */
