#ifndef _FormantAnalyzer_h_
#define _FormantAnalyzer_h_
/* FormantAnalyzer.h
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
#include "DataModeler.h"
#include "TextGridEditor.h"
#include "Formant.h"
#include "melder.h"
#include "Preferences.h"


Thing_define (FormantAnalyzer, TextGridEditor) {
	double minimumCeiling_Hz, maximumCeiling_Hz;
	integer numberOfModels;
	autoFormant modified; // extra for "optimum" formants
	OrderedOf<structSound> resampled; // 'numberOfModels' resampled versions of “extended” intervals
	integer numberOfFormantTracks, maximumNumberOfFormantTracks = 5;
	integer numberOfParametersF1Track, numberOfParametersF2Track, numberOfParametersF3Track, numberOfParametersF4Track, numberOfParametersF5Track;
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

	#include "FormantAnalyzer_prefs.h"
};

void FormantAnalyzer_init (FormantAnalyzer me, conststring32 title, Sampled sound, bool ownSound, TextGrid grid, Formant formant, SpellingChecker spellingChecker, conststring32 callbackSocket);

autoFormantAnalyzer FormantAnalyzer_create (conststring32 title, Sampled sound, bool ownSound, Formant formant, TextGrid grid,
	SpellingChecker spellingChecker,
	conststring32 callbackSocket);

/* End of file FormantAnalyzer.h */
#endif
