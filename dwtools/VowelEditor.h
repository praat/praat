#ifndef _VowelEditor_h_
#define _VowelEditor_h_
/* VowelEditor.h
 *
 * Copyright (C) 2008-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20070130 First
 djmw 20110306 Latest modification.
*/

#include "RealTier.h"
#include "Table.h"
#include "Editor.h"

Thing_define (TrajectoryPoint, AnyPoint) {
	double f1, f2;
	struct MelderColour colour;
};

Thing_define (Trajectory, Function) {
	SortedSetOfDoubleOf<structTrajectoryPoint> points;
	
	AnyTier_METHODS
};

#include "VowelEditor_enums.h"

Thing_define (VowelEditor, Editor) {
	autoGraphics graphics;   // the drawing
	int width, height;  // size of drawing area in pixels
	autoTable marks;   // Vowel, F1, F2, Colour
	autoTrajectory trajectory;
	autoVEC extraFrequencyBandwidthPairs;
	GuiDrawingArea drawingArea;
	GuiButton playButton, reverseButton, publishButton;
	GuiText f0TextField, f0SlopeTextField, durationTextField, extendTextField;
	GuiLabel startInfo, endInfo;

	void v_destroy () noexcept
		override;
	bool v_scriptable ()
		override { return false; }
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;

	#include "VowelEditor_prefs.h"

};

autoVowelEditor VowelEditor_create (conststring32 title, Daata data);

#endif /* _VowelEditor_h_ */
