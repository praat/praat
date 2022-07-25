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
	void v_createMenuItems_draw (EditorMenu menu)
		override;
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
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
