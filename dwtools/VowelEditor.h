#ifndef _VowelEditor_h_
#define _VowelEditor_h_
/* VowelEditor.h
 *
 * Copyright (C) 2008-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
oo_CLASS_CREATE (Vowel, Function);

struct structVowelEditor_F0
{
	double start;
	double slopeOctPerSec;
	double minimum, maximum;
	double samplingFrequency, adaptFactor, adaptTime;
	long interpolationDepth;
};

struct structVowelEditor_F1F2Grid
{
	double df1, df2;
	int text_left, text_right, text_bottom, text_top;
	double grey;
};

Thing_define (VowelEditor, Editor) {
	// new data:
	public:
		int soundFollowsMouse, shiftKeyPressed;
		double f1min, f1max, f2min, f2max;   // domain of graphics F1-F2 area
		Matrix f3, b3, f4, b4;
		int frequencyScale;   // 0: lin, 1: log, 2: bark, 3: mel
		int axisOrientation;  // 0: origin topright + f1 down + f2 to left, 0: origin lb + f1 right +f2 up
		int marksDataset, speakerType;   // 1 = male, 2 = female, 3 = child
		int marksFontSize;
		Graphics g;   // the drawing
		short width, height;  // size of drawing area in pixels
		Table marks;   // Vowel, F1, F2, Colour...
		Vowel vowel;
		double markTraceEvery;
		structVowelEditor_F0 f0;
		double maximumDuration, extendDuration;
		Sound source, target;
		GuiDrawingArea drawingArea;
		GuiButton playButton, reverseButton, publishButton;
		GuiText f0TextField, f0SlopeTextField, durationTextField, extendTextField;
		GuiLabel startInfo, endInfo;
		structVowelEditor_F1F2Grid grid;
	// overridden methods:
		void v_destroy ();
		void v_createChildren ();
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
};

VowelEditor VowelEditor_create (const wchar_t *title, Data data);

void VowelEditor_prefs ();

#endif /* _VowelEditor_h_ */