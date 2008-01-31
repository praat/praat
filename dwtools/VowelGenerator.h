#ifndef _VowelGenerator_h_
#define _VowelGenerator_h_
/* VowelGenerator.h
 *
 * Copyright (C) 2008 David Weenink
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
 djmw 20080111 Latest modification.
*/

#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

#include "Command.h"
#include "Editor.h"
#include "portaudio.h"

struct structPosTrace
{
	double t;
	double x, y;
};

struct structF0 
{
	double start;
	double slopeOctPerSec;
	double minimum, maximum;
	double samplingFrequency, adaptFactor, adaptTime;
	long interpolationDepth;
};

struct structGrid
{
	double df1, df2;
	int text_left, text_right, text_bottom, text_top;
	double grey;
};

#define VowelGenerator_members Editor_members \
	double f1min, f1max, f2min, f2max; /* Domain of graphics F1-F2 area */ \
	int frequencyScale; /* 0: lin, 1: log, 2: bark, 3: mel */ \
	int axisOrientation; /* 0: origin topright + f1 down + f2 to left, 0: origin lb + f1 right +f2 up */ \
	int speakerType; /* 1 male, 2 female, 3 child */ \
	Graphics g; /* the drawing */ \
	short width, height; /* Size of drawing area in pixels. */ \
	TableOfReal marks; /* rowlabel, F1, F2, ... */ \
	long nptrace; \
	long maximumPosTrace; \
	struct structPosTrace *ptrace; \
	double markTraceEvery; \
	struct structF0 f0; \
	double maximumDuration; \
	Sound source, target; \
	Widget drawingArea, playButton, reverseButton, publishButton; \
	Widget f0TextField, f0SlopeTextField, durationTextField; \
	int clicking; \
	struct structGrid grid;

#define VowelGenerator_methods Editor_methods
class_create (VowelGenerator, Editor);

VowelGenerator VowelGenerator_create (Widget parent, wchar_t *title, Any data);

void VowelGenerator_prefs (void);

// click (mousedown) audio generation
// shift-click select track
// code factoriseren zodat VG en Interface afzonderlijke componenten zijn.
// VG krijgt krijgt op willekeurige tijden input en genereert audio tot een stop moment
// input: (t,f0,nf, f[1],b[1],...f[n],intensiteit)

int VowelGenerator_and_TableOfReal_setMarks (VowelGenerator me, TableOfReal thee);
// Get new reference point from table

#endif /* _VowelGenerator_h_ */
