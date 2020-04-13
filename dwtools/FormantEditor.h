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
#include "FormantModeler.h"
#include "TextGridEditor.h"
#include "FormantListWithHistory.h"
#include "melder.h"
#include "Preferences.h"
#include "Sound.h"
#include "LPC.h"

/*
	Analysis parameter tiers have names '<Fh>_<fl>_<n>_<method>',
	where <Fh> is the upper ceiling en <Fl> lower ceiling, <n> is the number of ceilings
	and <method> is one of (auto,covar,burg, marple, robust) as these are fixed for a
	partular FormantList. 
	Also fixed for the tier are the formant analysis parameters, i.e.,
	number of formants, window length, time step, pre-emphasis, ...

	There is no need for the downsampled Sounds, and its LPC in our data structures because
	once the Formants have been calculated we can dispose of them.
	There is also no need to permanently store the FormantModelers because they can easily
	be recalculated whenever they are needed
*/

// Identical for all FormantModelers of the same segment
Thing_define (FormantModelParameters, Function) {
	integer maximumNumberOfFormantTracks = 5;
	integer numberOfFormantTracks;
	autoINTVEC numberOfParametersPerTrack; // .size = (numberOfFormantTracks)
};

Thing_define (FormantEditor, TextGridEditor) {
	autoFormant synthesis; // extra for "optimum" formants
	autoFormantListWithHistory formantList;
	OrderedOf<structFormantModeler> formantModelers; // 'numberOfModels' models
	
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
		override { return true; }
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
		override;
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

void FormantEditor_init (FormantEditor me, conststring32 title, Sound sound, bool ownSound, TextGrid grid, Formant formant, SpellingChecker spellingChecker, conststring32 callbackSocket);

autoFormantEditor FormantEditor_create (conststring32 title, Sound sound, bool ownSound, Formant formant, TextGrid grid,
	SpellingChecker spellingChecker,
	conststring32 callbackSocket);

/* End of file FormantEditor.h */
#endif
