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

inline static MelderColour DataGuiColour_BACKGROUND = Melder_WHITE;
inline static MelderColour DataGuiColour_EDITABLE = Melder_CYAN;
inline static MelderColour DataGuiColour_DEFAULT_FOREGROUND = Melder_BLACK;

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
protected:
	void setData (Daata data) { _data = data; }
private:
	Daata _data;   // the data that can be displayed and edited
	bool _editable;
	Editor _boss;

	friend void DataGui_init (DataGui me, Daata data, bool editable, Editor boss) {
		my _data = data;
		my _editable = editable;
		my _boss = boss;
		my v1_copyPreferencesToInstance ();
		my v9_repairPreferences ();
	}
public:
	virtual void v_createMenus () { }
	virtual void v_createMenuItems_file (EditorMenu /* menu */) { }
	virtual void v_updateMenuItems_file () { }

	virtual void v_form_pictureWindow (EditorCommand cmd);
	virtual void v_ok_pictureWindow (EditorCommand cmd);
	virtual void v_do_pictureWindow (EditorCommand cmd);
	virtual void v_form_pictureMargins (EditorCommand cmd);
	virtual void v_ok_pictureMargins (EditorCommand cmd);
	virtual void v_do_pictureMargins (EditorCommand cmd);

	#include "DataGui_prefs.h"
};

/* End of file DataGui.h */
#endif
