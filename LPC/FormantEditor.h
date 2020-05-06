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
#include "../dwtools/FormantModeler.h"
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
	For each analysis we add two tiers. Both tiers have a common first part <name> that can be chosen by the user.
	The tier names are <name>-master and <name>-slave.
	
	1. The <name>-master tier has only one interval and contains the info which is fixed for the analysis that is
	associated with <name>, i.e. it first lists all sorted ceiling frequencies, then it has a ';' separator and next the <lpc-method> with its parameters are listed (with an xxxx for the maximum formant frequency).
	Its content could be, for example, '4000 4250 4500 4750 5000 5250 5500 5750 6000; burg 0.05 5.0 xxxx 0.025 50'.
	This content shows that for the analyses 9 different ceilings were used, and for each of these ceilings
	a Formant has been calculated with the Burg algorithm, with a 'Time step (s)' of 0.05, a 'Max. number of formants' of 5.0, a 'Maximum formant (Hz)' of x (where x is one of the nine ceilings), a 'Window length (s)' of 0.025, and a 'Pre-emphasis from (Hz)' of 40.0.
	
	Because the information is generic this tier can only have one interval.
	This tier cannot be modified by the user directly because this would invalidate the <name>-slave tier.
	
	2. The <name>-slave tier can have multiple intervals. Each interval shows a particular analysi prefered by the user. It shows first the <ceiling> that was selected by the user, then a ';' separatior and finally the number of parameters per track of the formant modeler.
	Its content could be, for example '5000; 3 3 2', which means that the analysis with a ceiling of 5000 Hz was chosen and the Formant modeler used F1, F2, and F3 in the modeling and reserved 3 coefficients to model F1, 3 coefficients to model F2 and 2 coefficients to model F3. The <ceiling> number must match one of the possible ceilings (rounded to integer Hz values)

	We also store the downsampled Sounds because downsampling is still relatively slow. Once it becomes faster we could throw away the resampled sounds after use.
	LPC's are not needed because they have to be recalculated for order/method changes and this is relatively fast

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

// Identical for all FormantModelers on the same segment
Thing_define (FormantModelerList, Function) {
	integer numberOfModelers, numberOfTracksPerModel;
	double varianceExponent;
	autoINTVEC numberOfParametersPerTrack; // .size = (numberOfFormantTracks)
	OrderedOf<structFormantModeler> formantModelers;
	autoINTVEC selected; // models are displayed in this order
};

Thing_define (FormantEditor, TimeSoundAnalysisEditor) {
	autoFormant synthesis; // extra for "optimum" formants
	autoFormantList formantList;
	autoFormantModelerList formantModelerList;
	autoTextGrid slave;
	Graphics_Viewport selectionViewer_viewport;
	integer selectedTier, slaveTierNumber;
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

	#include "FormantEditor_prefs.h"
};

void FormantEditor_init (FormantEditor me, conststring32 title, FormantList formantList, Sound sound, bool ownSound, TextGrid grid, conststring32 callbackSocket);

autoFormantEditor FormantEditor_create (conststring32 title, FormantList formantList, Sound sound, bool ownSound, TextGrid grid,
	conststring32 callbackSocket);

/* End of file FormantEditor.h */
#endif
