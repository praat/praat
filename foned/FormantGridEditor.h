#ifndef _FormantGridEditor_h_
#define _FormantGridEditor_h_
/* FormantGridEditor.h
 *
 * Copyright (C) 2008-2018,2020,2022 Paul Boersma & David Weenink
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
#include "FormantGrid.h"
#include "RealTierArea.h"

Thing_define (FormantGridArea, RealTierArea) {
	virtual FormantGrid v_formantGrid() = 0;
	FormantGrid formantGrid() { return our v_formantGrid(); }

	bool editingBandwidths;
	integer selectedFormant;

	Function v_function() override {
		FormantGrid grid = our formantGrid();
		OrderedOf<structRealTier>* tiers = ( our editingBandwidths ? & grid -> bandwidths : & grid -> formants );
		RealTier tier = tiers->at [our selectedFormant];
		return tier;
	}
};
inline void FormantGridArea_init (FormantGridArea me, FunctionEditor editor) {
	my editingBandwidths = false;
	my selectedFormant = 1;
	RealTierArea_init (me, editor);
	my setGlobalYRange_fraction (0.0, 1.0);
}

Thing_define (FormantGridEditor, FunctionEditor) {
	autoFormantGridArea formantGridArea;

	GuiMenuItem d_bandwidthsToggle;

	void v_createMenus ()
		override;
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction)
		override;
	void v_play (double startTime, double endTime)
		override;

	virtual bool v_hasSourceMenu () { return true; }

	#include "FormantGridEditor_prefs.h"
};

Thing_define (FormantGridEditor_FormantGridArea, FormantGridArea) {
	FormantGrid v_formantGrid() override {
		Thing_cast (FormantGridEditor, editor, our _editor);
		Thing_cast (FormantGrid, grid, editor -> data);
		return grid;
	}
};

autoFormantGridEditor FormantGridEditor_create (conststring32 title, FormantGrid formantGrid);

/* End of file FormantGridEditor.h */
#endif
