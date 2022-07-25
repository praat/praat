#ifndef _AnyTextGridEditor_h_
#define _AnyTextGridEditor_h_
/* AnyTextGridEditor.h
 *
 * Copyright (C) 1992-2005,2007-2022 Paul Boersma
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
#include "TextGridArea.h"
#include "LongSoundArea.h"
#include "SoundAnalysisArea.h"

Thing_define (AnyTextGridEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, TextGridArea, textGridArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)
	DEFINE_FunctionArea (3, SoundAnalysisArea, soundAnalysisArea)

	TextGrid textGrid() { return static_cast <TextGrid> (our data()); }
	SampledXY soundOrLongSound() { return our soundArea() ? our soundArea() -> soundOrLongSound() : nullptr; }
	Sound sound() { return our soundArea() ? our soundArea() -> sound() : nullptr; }
	LongSound longSound() { return our soundArea() ? our soundArea() -> longSound() : nullptr; }

	SpellingChecker spellingChecker;
	bool suppressRedraw;
	autostring32 findString;

	void v1_info ()
		override;
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v1_dataChanged ()
		override;
	void v_distributeAreas () override {
		if (our soundArea()) {
			const bool showAnalysis = our soundAnalysisArea() -> hasContentToShow();
			const integer numberOfTiers = our textGrid() -> tiers->size;
			const integer numberOfVisibleChannels = Melder_clippedRight (our soundOrLongSound() -> ny, 8_integer);
			const double soundY = numberOfTiers / (2.0 * numberOfVisibleChannels +
					numberOfTiers * ( showAnalysis ? 1.8 : 1.3 ));
			our textGridArea() -> setGlobalYRange_fraction (0.0, soundY);
			if (showAnalysis) {
				const double soundY2 = 0.5 * (1.0 + soundY);
				our soundAnalysisArea() -> setGlobalYRange_fraction (soundY, soundY2);
				our soundArea() -> setGlobalYRange_fraction (soundY2, 1.0);
			} else {
				our soundAnalysisArea() -> setGlobalYRange_fraction (soundY, soundY);
				our soundArea() -> setGlobalYRange_fraction (soundY, 1.0);
			}
		} else {
			our textGridArea() -> setGlobalYRange_fraction (0.0, 1.0);
		}
	}
	void v_draw () override {
		if (our soundArea()) {
			FunctionArea_prepareCanvas (our soundArea().get());
			if (our soundAnalysisArea() -> instancePref_pulses_show())
				our soundAnalysisArea() -> v_draw_analysis_pulses ();
			FunctionArea_drawInside (our soundArea().get());
			if (our soundAnalysisArea() -> hasContentToShow()) {
				FunctionArea_prepareCanvas (our soundAnalysisArea().get());
				our soundAnalysisArea() -> v_draw_analysis ();
			}
		}
		FunctionArea_drawOne (our textGridArea().get());
	}
	bool v_hasSelectionViewer ()
		override { return true; }
	void v_drawSelectionViewer ()
		override;
	bool v_hasText ()
		override { return true; }
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
		override { return U"IPA chart"; }

	#include "AnyTextGridEditor_prefs.h"
};

void AnyTextGridEditor_init (AnyTextGridEditor me, conststring32 title, TextGrid textGrid,
	SpellingChecker spellingChecker, conststring32 callbackSocket
);

/* End of file AnyTextGridEditor.h */
#endif
