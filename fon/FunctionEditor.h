#ifndef _FunctionEditor_h_
#define _FunctionEditor_h_
/* FunctionEditor.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2002/11/19 added show-widgets
 * pb 2002/11/19 added pulse
 * pb 2003/05/20 longestAnalysis replaces pitch.timeSteps, pitch.speckle, formant.maximumDuration
 * pb 2003/05/21 pitch floor and ceiling replace the view and analysis ranges
 * pb 2003/05/27 spectrogram maximum and autoscaling
 * pb 2003/08/23 formant.numberOfTimeSteps
 * pb 2003/09/16 advanced pitch settings: pitch.timeStep, pitch.timeStepsPerView, pitch.viewFrom, pitch.viewTo
 * pb 2003/09/18 advanced formant settings: formant.timeStep, formant.timeStepsPerView
 * pb 2003/10/01 time step settings: timeStepStrategy, fixedTimeStep, numberOfTimeStepsPerView
 * pb 2004/02/15 highlight methods
 * pb 2004/07/14 pulses.maximumAmplitudeFactor
 * pb 2004/10/24 intensity.averagingMethod
 * pb 2004/10/27 intensity.subtractMeanPressure
 * pb 2005/01/11 getBottomOfSoundAndAnalysisArea
 */

#ifndef _Editor_h_
	#include "Editor.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _Function_h_
	#include "Function.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _LongSound_h_
	#include "LongSound.h"
#endif
#ifndef _Spectrogram_h_
	#include "Spectrogram.h"
#endif
#ifndef _Pitch_h_
	#include "Pitch.h"
#endif
#ifndef _Intensity_h_
	#include "Intensity.h"
#endif
#ifndef _Formant_h_
	#include "Formant.h"
#endif
#ifndef _PointProcess_h_
	#include "PointProcess.h"
#endif

struct FunctionEditor_spectrogram {
	/* KEEP IN SYNC WITH PREFS. */
	Spectrogram data; int show;
	/* Spectrogram settings: */
	double viewFrom, viewTo;   /* Hertz */
	double windowLength;   /* seconds */
	double dynamicRange;   /* dB */
	/* Advanced spectrogram settings: */
	long timeSteps, frequencySteps;
	int method;   /* Fourier */
	int windowShape;   /* 0=Square 1=Hamming 2=Bartlett 3=Welch 4=Hanning 5=Gaussian */
	int autoscaling;   /* yes/no */
	double maximum;   /* dB/Hz */
	double preemphasis;   /* dB/octave */
	double dynamicCompression;   /* 0..1 */
	/* Dynamic information: */
	double cursor;
};
struct FunctionEditor_pitch {
	/* KEEP IN SYNC WITH PREFS. */
	Pitch data; int show;
	/* Pitch settings: */
	double floor, ceiling; int units;
	/* Advanced pitch settings: */
	double viewFrom, viewTo;
	int method, veryAccurate;
	long maximumNumberOfCandidates; double silenceThreshold, voicingThreshold;
	double octaveCost, octaveJumpCost, voicedUnvoicedCost;
};
struct FunctionEditor_intensity {
	/* KEEP IN SYNC WITH PREFS. */
	Intensity data; int show;
	/* Intensity settings: */
	double viewFrom, viewTo;
	int averagingMethod, subtractMeanPressure;
};
struct FunctionEditor_formant {
	/* KEEP IN SYNC WITH PREFS. */
	Formant data; int show;
	/* Formant settings: */
	double maximumFormant; long numberOfPoles;
	double windowLength;
	double dynamicRange, dotSize;
	/* Advanced formant settings: */
	int method; double preemphasisFrom;
};
struct FunctionEditor_pulses {
	/* KEEP IN SYNC WITH PREFS. */
	PointProcess data; int show;
	/* Pulses settings: */
	double maximumPeriodFactor, maximumAmplitudeFactor;
};

#define FunctionEditor_members Editor_members \
	/* Subclass may change the following attributes, */ \
	/* but has to respect the invariants, */ \
	/* and has to call FunctionEditor_marksChanged () */ \
	/* immediately after making the changes. */ \
	double tmin, tmax, startWindow, endWindow; \
	double startSelection, endSelection; /* Markers. */ \
		/* These attributes are all expressed in seconds. Invariants: */ \
		/*    tmin <= startWindow < endWindow <= tmax; */	 \
		/*    tmin <= (startSelection, endSelection) <= tmax; */ \
	\
	Graphics graphics;   /* Used in the 'draw' method. */ \
	short width, height;   /* Size of drawing area in pixels. */ \
	Widget text;   /* Optional text at top. */ \
	int shiftKeyPressed;   /* Information for the 'play' method. */ \
	int playingCursor, playingSelection;   /* Information for end of play. */ \
	int numberOfFields; \
	int fieldHeight [10]; \
	struct { Sound data; int autoscaling; } sound; \
	struct { LongSound data; } longSound; \
	double longestAnalysis; \
	int timeStepStrategy; double fixedTimeStep; long numberOfTimeStepsPerView; \
	struct FunctionEditor_spectrogram spectrogram; \
	struct FunctionEditor_pitch pitch; \
	struct FunctionEditor_intensity intensity; \
	struct FunctionEditor_formant formant; \
	struct FunctionEditor_pulses pulses; \
	\
	/* Private attributes: */ \
	Widget drawingArea, scrollBar, groupButton, bottomArea; \
	int group, enableUpdates, nrect; \
	struct { double left, right, bottom, top; } rect [8]; \
	double marker [1 + 3], playCursor; \
	int numberOfMarkers; \
	Widget spectrogramToggle, pitchToggle, intensityToggle, formantToggle, pulsesToggle;

#define FunctionEditor_methods Editor_methods \
	void (*draw) (I); \
	void (*prepareDraw) (I);   /* For less flashing. */ \
	const char *format_domain, *format_short, *format_long, *format_units, *format_totalDuration, *format_window, *format_selection; \
	int hasText; \
	void (*play) (I, double tmin, double tmax); \
	int (*click) (I, double xWC, double yWC, int shiftKeyPressed); \
	int (*clickB) (I, double xWC, double yWC); \
	int (*clickE) (I, double xWC, double yWC); \
	void (*key) (I, unsigned char key); \
	int (*playCallback) (I, int phase, double tmin, double tmax, double t); \
	void (*updateText) (I); \
	void (*prefs_addFields) (I, EditorCommand cmd); \
	void (*prefs_setValues) (I, EditorCommand cmd); \
	void (*prefs_getValues) (I, EditorCommand cmd); \
	void (*viewMenuEntries) (I); \
	void (*highlightSelection) (I, double left, double right, double bottom, double top); \
	void (*unhighlightSelection) (I, double left, double right, double bottom, double top); \
	double (*getBottomOfSoundAndAnalysisArea) (I);

class_create (FunctionEditor, Editor)

/*
	Attributes:
		data: must be a Function.

	Methods:

	void draw (I);
		"draw your part of the data between startWindow and endWindow."

	void play (I, double tmin, double tmax);
		"user clicked in one of the rectangles above or below the data window."

	int click (I, double xWC, double yWC, int shiftKeyPressed);
		"user clicked in data window with the left (Mac: only) mouse button."
		'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
		'shiftKeyPressed' flags if the Shift key was held down during the click.
		Return FunctionEditor_UPDATE_NEEDED if you want a window update, i.e.,
			if your 'click' moves the cursor or otherwise changes the appearance of the data.
		Return FunctionEditor_NO_UPDATE_NEEDED if you do not want a window update, e.g.,
			if your 'click' method just 'plays' something or puts a dialog on the screen.
			In the latter case, the 'ok' callback of the dialog should
			call FunctionEditor_marksChanged if necessary.
		FunctionEditor::click moves the cursor to 'xWC', drags to create a selection, 
			or extends the selection.

	int clickB (I, double xWC, double yWC);
		"user clicked in data window with the middle mouse button (Mac: control- or option-click)."
		'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
		For the return value, see the 'click' method.
		FunctionEditor::clickB simply moves the start of the selection (B) to 'xWC',
			with the sole statement 'my startSelection = xWC'.

	int clickE (I, double xWC, double yWC);
		"user clicked in data window with the right mouse button (Mac: command-click)."
		'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
		For the return value, see the 'click' method.
		FunctionEditor::clickB simply moves the end of the selection (E) to 'xWC',
			with the sole statement 'my endSelection = xWC'.

	void key (I, unsigned char key);
		"user typed a key to the data window."
		FunctionEditor::key ignores this message.
*/
#define FunctionEditor_UPDATE_NEEDED  1
#define FunctionEditor_NO_UPDATE_NEEDED  0

int FunctionEditor_init (I, Widget parent, const char *title, Any data);
/*
	Function:
		creates an Editor with a drawing area, a scroll bar and some buttons.
	Preconditions:
		parent != NULL;
		Thing_member (data, classFunction);
	Postconditions:
		my cursorMenu contains the following entries:
			Move cursor to B
			Move cursor to E
			Move cursor to...
			Move cursor by...
		my beginMenu contains:
			Move B to cursor
			Move B to E
			Move B to...
			Move B by...
		my endMenu contains:
			Move E to cursor
			Move E to B
			Move E to...
			Move E by...
		my drawingArea is attached to the form at all sides,
		my scrollBar only to the bottom, left and right sides.
		The other members are 0.0 or NULL.
		The inheritor should call
			'XtManageChild (my dialog); XtManageChild (my shell);'
			before calling FunctionEditor_open (me).
*/ 

void FunctionEditor_marksChanged (I);
/*
	Function:
		update optional text field, the scroll bar, the drawing area and the buttons,
		from the current total time, window, cursor, and selection,
		and redraw the contents. This will be done for all the editors in the group.
	Usage:
		call this after a change in any of the markers or in the duration of the data.
*/

void FunctionEditor_shift (I, double shift);
/*
	Function:
		shift (scroll) the window through time, keeping the window length constant.
	Usage:
		call this after a search.
*/

void FunctionEditor_updateText (I);
/*
	Function:
		update the optional text widget.
	Usage:
		call this after moving the cursor, if that would have to change the text.
		The generic FunctionEditor also calls this if one of the other marks have changed.
	Behaviour:
		we just call the updateText method, which the inheritor will have to modify,
		since FunctionEditor::updateText does nothing.
*/

void FunctionEditor_redraw (I);
/*
	Function:
		update the drawing area of a single editor.
	Usage:
		calls this after she changes a view option (font, scaling, hide/show xx)
		or after any of the data have changed. In the latter case, also call Editor_broadcastChange.
	Behaviour:
		we just call Graphics_updateWs (my graphics).
*/

void FunctionEditor_enableUpdates (I, int enable);
/*
	Function:
		temporarily disable update event to cause 'draw' messages.
	Usage:
		If you call from your 'draw' method routines that may trigger expose events,
		you should bracket those routines between
			FunctionEditor_enableUpdates (me, FALSE);
		and
			FunctionEditor_enableUpdates (me, TRUE);
		This may happen if you call an analysis routine which calls Melder_progress.
*/

void FunctionEditor_ungroup (I);
/*
	Function:
		force me out of the group.
	Usage:
		Start cut or paste methods by calling this routine,
		as the grouped editors will not be synchronized
		after either of those actions. Worse, the selection
		may get outside the common interval of the editors.
*/

void FunctionEditor_prefs (void);

/* Some routines to enforce common look to all function editors. */
/* The x axis of the window is supposed to have been set to [my startWindow, my endWindow]. */
/* Preconditions: default line type, default line width. */
/* Postconditions: default line type, default line width, undefined colour, undefined text alignment. */
void FunctionEditor_drawRangeMark (I, const char *format, double yWC, int verticalAlignment);
void FunctionEditor_drawCursorFunctionValue (I, const char *format, double yWC);
void FunctionEditor_insertCursorFunctionValue (I, const char *format, double yWC, double minimum, double maximum);
void FunctionEditor_drawHorizontalHair (I, const char *format, double yWC);
void FunctionEditor_drawGridLine (I, double yWC);

void FunctionEditor_insetViewport (I);

/* End of file FunctionEditor.h */
#endif
