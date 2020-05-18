#ifndef _FormantEditor_h_
#define _FormantEditor_h_
/* FormantEditor.h
 *
 * Copyright (C) 2020 David Weenink
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
#include "FormantList.h"
#include "melder.h"
#include "Preferences.h"
#include "Sound.h"
#include "LPC.h"
#include "TextGrid.h"
#include "TimeSoundAnalysisEditor.h"

#include "TextGridEditor_enums.h"
/*
	We add one tier which ends with -log.
	
	The explanation following is for analyses with different maximum formant frequencies (i.e. different ceilings).
	The <name>-log tier can have multiple intervals. Each interval shows a particular analysis prefered by the user. It shows first the <ceiling> that was selected by the user, then a ';' separator and finally the number of parameters per track of the formant modeler. 
	Its content could be, for example '5000; 5 5 5', which means that the analysis with a ceiling of 5000 Hz was chosen and the Formant modeler used F1, F2, and F3 in the modeling and reserved 5 coefficients to model F1, 5 coefficients to model F2 and 5 coefficients to model F3. The <ceiling> number must match one of the possible ceilings (rounded to integer Hz values)
	An empty interval always implies the default analysis. Therefore only intervals where you want a non-default have to be specified.

	There is no need to permanently store the FormantModelers because they can easily
	be calculated whenever they are needed from the information in the tiers.
	
	We only accept a mono Sound.
	
	Menus:
	File:
		Extract Formant...
		Save Formant...
	View:
		
	Select:
		Keep all menu items
	Interval: only one item:
		Add interval on slave tier (ctrl 1)
	Boundary:
		Move to nearest zero crossing
		Add on sklave tier
		Remove
	
*/

/*
	To save / restore the state of the data part of the editor.
	We don't save the state of FormantModelerList because this part
	is always recalculated.
*/

Thing_define (FormantEditorData, Function) {
	double startWindow, endWindow, startSelection, endSelection;
	IntervalTier logTier;
	Formant formant;
	void v_copy (Daata data_to)
		override;
	//bool v_equal (Daata otherData)
	//	override;
};

Thing_define (FormantEditor, TimeSoundAnalysisEditor) {
	autoFormantList formantList;
	autoFormantModelerList formantModelerList;
	autoTextGrid logGrid; // changed by the editor
	TextGrid originalGrid;
	Graphics_Viewport selectionViewer_viewport;
	integer selectedTier, originalLogTierNumber, logTierNumber;
	bool suppressRedraw;
	autostring32 findString;
	GuiMenuItem extractSelectedTextGridPreserveTimesButton, extractSelectedTextGridTimeFromZeroButton;
	void v_info ()
		override;
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_dataChanged ()
		override;
	void v_createMenuItems_file_extract (EditorMenu menu)
		override;
	void v_createMenuItems_file_write (EditorMenu menu)
		override;
	void v_createMenuItems_file_draw (EditorMenu menu)
		override;
	void v_prepareDraw ()
		override;
	void v_draw ()
		override;
	void v_drawSelectionViewer ()
		override;
	bool v_hasText ()
		override { return false; }
	bool v_click (double xWC, double yWC, bool shiftKeyPressed)
		override;
	bool v_clickB (double xWC, double yWC)
		override;
	bool v_clickE (double xWC, double yWC)
		override;
	void v_clickSelectionViewer (double xWC, double yWC)
		override;
	void v_draw_analysis_formants ()
		override;
	void v_play (double tmin, double tmax)
		override;
	void v_updateText ()
		override {};
	void v_prefs_addFields (EditorCommand cmd)
		override;
	void v_prefs_setValues (EditorCommand cmd)
		override;
	void v_prefs_getValues (EditorCommand cmd)
		override;
	conststring32 v_selectionViewerName ()
		override { return U"Formant modelers"; }
	void v_createMenuItems_view_timeDomain (EditorMenu menu)
		override;
	void v_highlightSelection (double left, double right, double bottom, double top)
		override;
	void v_unhighlightSelection (double left, double right, double bottom, double top)
		override;
	double v_getBottomOfSoundArea ()
		override;
	double v_getBottomOfSoundAndAnalysisArea ()
		override;
	void v_updateMenuItems_file ()
		override;
	void v_createMenuItems_pitch_picture (EditorMenu menu)
		override;
	void v_createMenuItems_formant (EditorMenu menu)
		override;
	void v_restoreData ()
		override;
	#include "FormantEditor_prefs.h"
};

void FormantEditor_init (FormantEditor me, conststring32 title, Formant data, FormantList formantList, Sound sound, bool ownSound, TextGrid grid, conststring32 callbackSocket);

autoFormantEditor FormantEditor_create (conststring32 title, Formant data, FormantList formantList, Sound sound, bool ownSound, TextGrid grid,
	conststring32 callbackSocket);

/* End of file FormantEditor.h */
#endif
