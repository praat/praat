#ifndef _FormantPathEditor_h_
#define _FormantPathEditor_h_
/* FormantPathEditor.h
 *
 * Copyright (C) 2020-2022 David Weenink, 2022 Paul Boersma
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

#include "Collection.h"
#include "FormantModelerList.h"
#include "Formant.h"
#include "FormantPathArea.h"
#include "melder.h"
#include "Preferences.h"
#include "Sound.h"
#include "LPC.h"
#include "TextGrid.h"
#include "TimeSoundAnalysisEditor.h"

#include "TextGridArea.h"

Thing_define (FormantPathEditor, TimeSoundAnalysisEditor) {
	FormantPath formantPath() { return static_cast <FormantPath> (our data()); }

	autoFormantPathArea formantPathArea;
	//autoTextGrid textgrid;
	autoFormant previousFormant;
	Graphics_Viewport selectionViewer_viewport;
	integer selectedTier, selectedCandidate;
	GuiMenuItem navigateSettingsButton, navigateNextButton, navigatePreviousButton;

	void v1_info ()
		override;
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v1_dataChanged ()
		override;
	void v_draw ()
		override;
	bool v_hasSelectionViewer ()
		override { return true; }
	void v_drawSelectionViewer ()
		override;
	bool v_hasText ()
		override { return false; }
	void v_clickSelectionViewer (double xWC, double yWC)
		override;
	void v_play (double startTime, double endTime)
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double xWC, double yWC)
		override;
	void v_updateText ()
		override {};
	conststring32 v_selectionViewerName ()
		override { return U"Formant candidates"; }
	void v_createMenuItems_view_timeDomain (EditorMenu menu)
		override;
	void v_updateMenuItems ()
		override;

	#include "FormantPathEditor_prefs.h"
};

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound sound, TextGrid textgrid);

/* End of file FormantPathEditor.h */
#endif
