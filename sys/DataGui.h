#ifndef _DataGui_h_
#define _DataGui_h_
/* DataGui.h
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

#include "Data.h"
#include "Preferences.h"

#include "DataGui_enums.h"

extern MelderColour
	DataGuiColour_WINDOW_BACKGROUND,
	DataGuiColour_AREA_BACKGROUND,
	DataGuiColour_EDITABLE,
	DataGuiColour_EDITABLE_FRAME,
	DataGuiColour_EDITABLE_SELECTED,
	DataGuiColour_NONEDITABLE,
	DataGuiColour_NONEDITABLE_FRAME,
	DataGuiColour_NONEDITABLE_SELECTED;

Thing_declare (Editor);
Thing_declare (EditorMenu);
Thing_declare (EditorCommand);

Thing_define (DataGui, Thing) {
	/*
		Accessors.
	*/
public:
	Daata data() const { return _data; }
	bool editable() const { return _editable; }
	Editor boss() const { return _boss; }
	Graphics pictureGraphics() const { return _pictureGraphics; }
protected:
	void setData (Daata data) { _data = data; }
private:
	Daata _data;   // the data that can be displayed and edited
	bool _editable;
	Editor _boss;
	Graphics _pictureGraphics;

	friend void DataGui_init (DataGui me, Daata data, bool editable, Editor boss) {
		my _data = data;
		my _editable = editable;
		my _boss = boss;
		my v1_copyPreferencesToInstance ();
		my v9_repairPreferences ();
	}
	friend void DataGui_openPraatPicture (DataGui me);
	friend void DataGui_closePraatPicture (DataGui me);
public:
	virtual void v_createMenuItems_prefs   (EditorMenu /* fileMenu */) { }
	virtual void v_createMenuItems_save    (EditorMenu /* fileMenu */) { }
	virtual void v_createMenuItems_edit    (EditorMenu /* editMenu */) { }
	virtual void v_createMenuItems_play    (EditorMenu /* playMenu */) { }
	virtual void v_createMenuItems_help    (EditorMenu /* helpMenu */) { }
	virtual void v_createMenus () { }   // all the menus, except File, Edit, Play and Help
	virtual void v_updateMenuItems () { }

	virtual void v_prefs_addFields (EditorCommand) { }
	virtual void v_prefs_setValues (EditorCommand) { }
	virtual void v_prefs_getValues (EditorCommand) { }

	virtual void v_form_pictureWindow (EditorCommand cmd);
	virtual void v_ok_pictureWindow (EditorCommand cmd);
	virtual void v_do_pictureWindow (EditorCommand cmd);
	virtual void v_form_pictureMargins (EditorCommand cmd);
	virtual void v_ok_pictureMargins (EditorCommand cmd);
	virtual void v_do_pictureMargins (EditorCommand cmd);

	#include "DataGui_prefs.h"
	friend MelderColour DataGui_defaultForegroundColour (DataGui me, bool selected) {
		return
			my editable() ?
				selected ? DataGuiColour_EDITABLE_SELECTED : DataGuiColour_EDITABLE
			:
				selected ? DataGuiColour_NONEDITABLE_SELECTED : DataGuiColour_NONEDITABLE;
	}
};

/*
 * The following two procedures are in praat_picture.cpp.
 * They allow DataGuis to draw into the Picture window.
 */
Graphics praat_picture_datagui_open (bool eraseFirst);
void praat_picture_datagui_close ();

/* End of file DataGui.h */
#endif
