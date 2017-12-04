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

#include "FormantTier.h"
#include "PitchTier.h"
#include "TableOfReal.h"
#include "Editor.h"

#include "Vowel_def.h"

struct structVowelEditor_F0 {
	double start;
	double slopeOctPerSec;
	double minimum, maximum;
	double samplingFrequency, adaptFactor, adaptTime;
	integer interpolationDepth;
};

struct structVowelEditor_F1F2Grid {
	double df1, df2;
	int text_left, text_right, text_bottom, text_top;
	double grey;
};

Thing_define (VowelEditor, Editor) {
	int soundFollowsMouse, shiftKeyPressed;
	double f1min, f1max, f2min, f2max;   // domain of graphics F1-F2 area
	autoMatrix f3, b3, f4, b4;
	int frequencyScale;   // 0: lin, 1: log, 2: bark, 3: mel
	int axisOrientation;  // 0: origin topright + f1 down + f2 to left, 0: origin lb + f1 right +f2 up
	int marksDataset, speakerType;   // 1 = male, 2 = female, 3 = child
	int marksFontSize;
	autoGraphics graphics;   // the drawing
	short width, height;  // size of drawing area in pixels
	autoTable marks;   // Vowel, F1, F2, Colour...
	autoVowel vowel;
	double markTraceEvery;
	structVowelEditor_F0 f0;
	double maximumDuration, extendDuration;
	GuiDrawingArea drawingArea;
	GuiButton playButton, reverseButton, publishButton;
	GuiText f0TextField, f0SlopeTextField, durationTextField, extendTextField;
	GuiLabel startInfo, endInfo;
	structVowelEditor_F1F2Grid grid;

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
};

autoVowelEditor VowelEditor_create (const char32 *title, Daata data);

void VowelEditor_prefs ();

#endif /* _VowelEditor_h_ */
