#ifndef _FormantPathEditor_h_
#define _FormantPathEditor_h_
/* FormantPathEditor.h
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
#include "FormantPath.h"
#include "melder.h"
#include "Preferences.h"
#include "Sound.h"
#include "LPC.h"
#include "TextGrid.h"
#include "TextGridView.h"
#include "TimeSoundAnalysisEditor.h"

#include "TextGridEditor_enums.h"
/*
	We might add one tier named formant-log if the input textgrid does not have our specific log tier.
	
	The explanation following is for analyses with different maximum formant frequencies (i.e. different ceilings).
	The <name>-log tier can have multiple intervals. Each interval shows a particular analysis prefered by the user. It shows first the <ceiling> that was selected by the user, then a ';' separator and finally the number of parameters per track of the formant modeler. 
	Its content could be, for example '5000; 5 5 5', which means that the analysis with a ceiling of 5000 Hz was chosen and the Formant modeler used F1, F2, and F3 in the modelling and reserved 5 coefficients to model F1, 5 coefficients to model F2 and 5 coefficients to model F3. The <ceiling> number must match one of the possible ceilings (rounded to integer Hz values)
	An empty interval always implies the default analysis. Therefore only intervals where you want a non-default have to be specified.

	There is no need to permanently store the FormantModelers because they can easily
	be calculated whenever they are needed from the information in the tiers.
	
	Multichannel sounds don't make sense with respect to the analysis part. If both channels are the same sound, one is resundant.
	If two different sounds?. May be only copy channel 1?
*/

Thing_define (FormantPathEditor, TimeSoundAnalysisEditor) {
	autoTextGrid textgrid;
	autoTextGridView pathGridView;
	Graphics_Viewport selectionViewer_viewport;
	integer selectedTier, selectedCandidate;
	bool suppressRedraw;
	autostring32 findString;
	GuiMenuItem navigateSettingsButton, navigateNextButton, navigatePreviousButton;
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
	//bool v_click (double xWC, double yWC, bool shiftKeyPressed)
	//	override;
	void v_clickSelectionViewer (double xWC, double yWC)
		override;
	void v_draw_analysis_formants ()
		override;
	void v_play (double startTime, double endTime)
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
		override { return U"Formant candidates"; }
	void v_createMenuItems_view_timeDomain (EditorMenu menu)
		override;
	void v_highlightSelection (double left, double right, double bottom, double top)
		override;
	void v_unhighlightSelection (double left, double right, double bottom, double top)
		/*override*/;
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
	virtual void v_updateMenuItems_navigation ();
	
	#include "FormantPathEditor_prefs.h"
};

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound sound, TextGrid textgrid);

/* End of file FormantPathEditor.h */
#endif
