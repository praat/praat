#ifndef _ERPWindow_h_
#define _ERPWindow_h_
/* ERPWindow.h
 *
 * Copyright (C) 2012-2018,2021,2022 Paul Boersma
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

#include "FunctionEditor.h"
#include "ERPArea.h"

Thing_define (ERPWindow, FunctionEditor) {
	DEFINE_FunctionArea (1, ERPArea, erpArea)

	void v1_dataChanged (Editor sender) override {
		ERPWindow_Parent :: v1_dataChanged (sender);
		Thing_cast (ERP, erp, our data());
		our erpArea() -> functionChanged (erp);
	}
	void v_distributeAreas () override {
		our erpArea() -> setGlobalYRange_fraction (0.0, 1.0);
	}
	bool v_hasSelectionViewer ()
		override { return true; }
	void v_drawSelectionViewer ()
		override;
	void v_prefs_addFields (EditorCommand cmd)
		override;
	void v_prefs_setValues (EditorCommand cmd)
		override;
	void v_prefs_getValues (EditorCommand cmd)
		override;

	#include "ERPWindow_prefs.h"
};

/**
	Create an ERPWindow.
*/
autoERPWindow ERPWindow_create (conststring32 title, ERP data);

/* End of file ERPWindow.h */
#endif
