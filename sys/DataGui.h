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

inline static MelderColour DataGuiColour_BACKGROUND = Melder_WHITE;
inline static MelderColour DataGuiColour_EDITABLE = Melder_CYAN;
inline static MelderColour DataGuiColour_DEFAULT_FOREGROUND = Melder_BLACK;

Thing_define (DataGui, Thing) {
	/*
		Accessors.
	*/
public:
	Daata data() const { return _data; }
	bool editable() const { return _editable; }
private:
	Daata _data;   // the data that can be displayed and edited
	bool _editable;

	friend void DataGui_init (DataGui me, Daata data, bool editable) {
		my _data = data;
		my _editable = editable;
		my v1_copyPreferencesToInstance ();
		my v9_repairPreferences ();
	}

protected:
	virtual void v1_copyPreferencesToInstance () { }
		/*
			derived::v1_copyPreferencesToInstance calls base::v1_copyPreferencesToInstance at *start*,
			because specifications at derived level have to override those of the base level
		*/
	virtual void v9_repairPreferences () { }
		/*
			derived::v9_repairPreferences may call base::v9_repairPreferences at *end*,
			because restrictions at base level may be laxer than restrictions at derived level
			(preferences will be overridden only if their current value violates a restriction)
		*/
};

/* End of file DataGui.h */
#endif
