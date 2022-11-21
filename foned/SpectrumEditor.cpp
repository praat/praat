/* SpectrumEditor.cpp
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

#include "SpectrumEditor.h"
#include "EditorM.h"

Thing_implement (SpectrumEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "SpectrumEditor_prefs.h"
#include "Prefs_install.h"
#include "SpectrumEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "SpectrumEditor_prefs.h"

static void menu_cb_help_SpectrumEditor (SpectrumEditor, EDITOR_ARGS) { Melder_help (U"SpectrumEditor"); }
static void menu_cb_help_Spectrum (SpectrumEditor, EDITOR_ARGS) { Melder_help (U"Spectrum"); }

void structSpectrumEditor :: v_createMenuItems_help (EditorMenu menu) {
	SpectrumEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"SpectrumEditor help", '?', menu_cb_help_SpectrumEditor);
	EditorMenu_addCommand (menu, U"Spectrum help", 0, menu_cb_help_Spectrum);
}

autoSpectrumEditor SpectrumEditor_create (conststring32 title, Spectrum spectrum) {
	try {
		autoSpectrumEditor me = Thing_new (SpectrumEditor);
		my spectrumArea() = SpectrumArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, spectrum);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrum window not created.");
	}
}

/* End of file SpectrumEditor.cpp */
