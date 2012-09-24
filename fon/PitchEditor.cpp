/* PitchEditor.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

#include "Pitch_to_Sound.h"
#include "PitchEditor.h"
#include "EditorM.h"

Thing_implement (PitchEditor, FunctionEditor, 0);

#define HEIGHT_UNV  3.0
#define HEIGHT_INTENS  6.0
#define RADIUS  2.5

/********** MENU COMMANDS **********/

static void menu_cb_setCeiling (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	EDITOR_FORM (L"Change ceiling", 0)
		POSITIVE (L"Ceiling (Hz)", L"600")
	EDITOR_OK
		Pitch pitch = (Pitch) my data;
		SET_REAL (L"Ceiling", pitch -> ceiling)
	EDITOR_DO
		Pitch pitch = (Pitch) my data;
		Editor_save (me, L"Change ceiling");
		Pitch_setCeiling (pitch, GET_REAL (L"Ceiling"));
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_pathFinder (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	EDITOR_FORM (L"Path finder", 0)
		REAL (L"Silence threshold", L"0.03")
		REAL (L"Voicing threshold", L"0.45")
		REAL (L"Octave cost", L"0.01")
		REAL (L"Octave-jump cost", L"0.35")
		REAL (L"Voiced/unvoiced cost", L"0.14")
		POSITIVE (L"Ceiling (Hz)", L"600")
		BOOLEAN (L"Pull formants", 0)
	EDITOR_OK
		Pitch pitch = (Pitch) my data;
		SET_REAL (L"Ceiling", pitch -> ceiling)
	EDITOR_DO
		Pitch pitch = (Pitch) my data;
		Editor_save (me, L"Path finder");
		Pitch_pathFinder (pitch,
			GET_REAL (L"Silence threshold"), GET_REAL (L"Voicing threshold"),
			GET_REAL (L"Octave cost"), GET_REAL (L"Octave-jump cost"),
			GET_REAL (L"Voiced/unvoiced cost"), GET_REAL (L"Ceiling"), GET_INTEGER (L"Pull formants"));
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_getPitch (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	if (my d_startSelection == my d_endSelection) {
		Melder_informationReal (Pitch_getValueAtTime ((Pitch) my data, my d_startSelection, kPitch_unit_HERTZ, 1), L"Hz");
	} else {
		Melder_informationReal (Pitch_getMean ((Pitch) my data, my d_startSelection, my d_endSelection, kPitch_unit_HERTZ), L"Hz");
	}
}

static void menu_cb_octaveUp (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	Pitch pitch = (Pitch) my data;
	Editor_save (me, L"Octave up");
	Pitch_step (pitch, 2.0, 0.1, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_fifthUp (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	Pitch pitch = (Pitch) my data;
	Editor_save (me, L"Fifth up");
	Pitch_step (pitch, 1.5, 0.1, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_fifthDown (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	Pitch pitch = (Pitch) my data;
	Editor_save (me, L"Fifth down");
	Pitch_step (pitch, 1 / 1.5, 0.1, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_octaveDown (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	Pitch pitch = (Pitch) my data;
	Editor_save (me, L"Octave down");
	Pitch_step (pitch, 0.5, 0.1, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_voiceless (EDITOR_ARGS) {
	EDITOR_IAM (PitchEditor);
	Pitch pitch = (Pitch) my data;
	long ileft = Sampled_xToHighIndex (pitch, my d_startSelection);
	long iright = Sampled_xToLowIndex (pitch, my d_endSelection);
	if (ileft < 1) ileft = 1;
	if (iright > pitch -> nx) iright = pitch -> nx;
	Editor_save (me, L"Unvoice");
	for (long i = ileft; i <= iright; i ++) {
		Pitch_Frame frame = & pitch -> frame [i];
		for (long cand = 1; cand <= frame -> nCandidates; cand ++) {
			if (frame -> candidate [cand]. frequency == 0.0) {
				struct structPitch_Candidate help = frame -> candidate [1];
				frame -> candidate [1] = frame -> candidate [cand];
				frame -> candidate [cand] = help;
			}
		}
	}
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_PitchEditorHelp (EDITOR_ARGS) { EDITOR_IAM (PitchEditor); Melder_help (L"PitchEditor"); }
static void menu_cb_PitchHelp (EDITOR_ARGS) { EDITOR_IAM (PitchEditor); Melder_help (L"Pitch"); }

void structPitchEditor :: v_createMenus () {
	PitchEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, L"Edit", L"Change ceiling...", 0, menu_cb_setCeiling);
	Editor_addCommand (this, L"Edit", L"Path finder...", 0, menu_cb_pathFinder);

	Editor_addCommand (this, L"Query", L"-- pitch --", 0, NULL);
	Editor_addCommand (this, L"Query", L"Get pitch", GuiMenu_F5, menu_cb_getPitch);

	Editor_addMenu (this, L"Selection", 0);
	Editor_addCommand (this, L"Selection", L"Unvoice", 0, menu_cb_voiceless);
	Editor_addCommand (this, L"Selection", L"-- up and down --", 0, NULL);
	Editor_addCommand (this, L"Selection", L"Octave up", 0, menu_cb_octaveUp);
	Editor_addCommand (this, L"Selection", L"Fifth up", 0, menu_cb_fifthUp);
	Editor_addCommand (this, L"Selection", L"Fifth down", 0, menu_cb_fifthDown);
	Editor_addCommand (this, L"Selection", L"Octave down", 0, menu_cb_octaveDown);
}

void structPitchEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PitchEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"PitchEditor help", '?', menu_cb_PitchEditorHelp);
	EditorMenu_addCommand (menu, L"Pitch help", 0, menu_cb_PitchHelp);
}
	
/********** DRAWING AREA **********/

void structPitchEditor :: v_draw () {
	Pitch pitch = (Pitch) data;
	long it, it1, it2;
	double dyUnv, dyIntens;

	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_setColour (d_graphics, Graphics_WHITE);
	Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
	Graphics_setColour (d_graphics, Graphics_BLACK);
	Graphics_rectangle (d_graphics, 0, 1, 0, 1);

	dyUnv = Graphics_dyMMtoWC (d_graphics, HEIGHT_UNV);
	dyIntens = Graphics_dyMMtoWC (d_graphics, HEIGHT_INTENS);

	Sampled_getWindowSamples (pitch, d_startWindow, d_endWindow, & it1, & it2);

	/*
	 * Show pitch.
	 */
	{
		long df =
			pitch -> ceiling > 10000 ? 2000 :
			pitch -> ceiling > 5000 ? 1000 :
			pitch -> ceiling > 2000 ? 500 :
			pitch -> ceiling > 800 ? 200 :
			pitch -> ceiling > 400 ? 100 :
			50;
		double radius;
		Graphics_Viewport previous;
		previous = Graphics_insetViewport (d_graphics, 0, 1, dyUnv, 1 - dyIntens);
		Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0, pitch -> ceiling);
		radius = Graphics_dxMMtoWC (d_graphics, RADIUS);

		/* Horizontal hair at current pitch. */

		if (d_startSelection == d_endSelection && d_startSelection >= d_startWindow && d_startSelection <= d_endWindow) {
			double f = Pitch_getValueAtTime (pitch, d_startSelection, kPitch_unit_HERTZ, Pitch_LINEAR);
			if (NUMdefined (f)) {
				Graphics_setColour (d_graphics, Graphics_RED);
				Graphics_line (d_graphics, d_startWindow - radius, f, d_endWindow, f);
				Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
				Graphics_text1 (d_graphics, d_startWindow - radius, f, Melder_fixed (f, 2));
			}
		}

		/* Horizontal scaling lines. */

		Graphics_setColour (d_graphics, Graphics_BLUE);
		Graphics_setLineType (d_graphics, Graphics_DOTTED);
		Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_HALF);
		for (long f = df; f <= pitch -> ceiling; f += df) {
			Graphics_line (d_graphics, d_startWindow, f, d_endWindow, f);
			Graphics_text2 (d_graphics, d_endWindow + radius/2, f, Melder_integer (f), L" Hz");
		}
		Graphics_setLineType (d_graphics, Graphics_DRAWN);

		/* Show candidates. */

		for (it = it1; it <= it2; it ++) {
			Pitch_Frame frame = & pitch -> frame [it];
			double t = Sampled_indexToX (pitch, it);
			double f = frame -> candidate [1]. frequency;
			if (f > 0.0 && f < pitch -> ceiling) {
				Graphics_setColour (d_graphics, Graphics_MAGENTA);
				Graphics_fillCircle_mm (d_graphics, t, f, RADIUS * 2);
			}
			Graphics_setColour (d_graphics, Graphics_BLACK);
			Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_HALF);
			for (int icand = 1; icand <= frame -> nCandidates; icand ++) {
				int strength = (int) floor (10 * frame -> candidate [icand]. strength + 0.5);
				f = frame -> candidate [icand]. frequency;
				if (strength > 9) strength = 9;
				if (f > 0 && f <= pitch -> ceiling) Graphics_text1 (d_graphics, t, f, Melder_integer (strength));
			}
		}
		Graphics_resetViewport (d_graphics, previous);
	}

	/*
	 * Show intensity.
	 */
	{
		Graphics_Viewport previous;
		previous = Graphics_insetViewport (d_graphics, 0, 1, 1 - dyIntens, 1);
		Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0, 1);
		Graphics_setColour (d_graphics, Graphics_BLACK);
		Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (d_graphics, d_startWindow, 0.5, L"intens");
		Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_HALF);
		Graphics_text (d_graphics, d_endWindow, 0.5, L"intens");
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_HALF);
		for (it = it1; it <= it2; it ++) {
			Pitch_Frame frame = & pitch -> frame [it];
			double t = Sampled_indexToX (pitch, it);
			int strength = (int) floor (10 * frame -> intensity + 0.5);   // map 0.0-1.0 to 0-9
			if (strength > 9) strength = 9;
			Graphics_text1 (d_graphics, t, 0.5, Melder_integer (strength));
		}
		Graphics_resetViewport (d_graphics, previous);
	}

	if (it1 > 1) it1 -= 1;
	if (it2 < pitch -> nx) it2 += 1;

	/*
	 * Show voicelessness.
	 */
	{
		Graphics_Viewport previous;
		previous = Graphics_insetViewport (d_graphics, 0, 1, 0, dyUnv);
		Graphics_setColour (d_graphics, Graphics_BLUE);
		Graphics_line (d_graphics, d_startWindow, 1, d_endWindow, 1);
		Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (d_graphics, d_startWindow, 0.5, L"Unv");
		Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_HALF);
		Graphics_text (d_graphics, d_endWindow, 0.5, L"Unv");
		for (it = it1; it <= it2; it ++) {
			Pitch_Frame frame = & pitch -> frame [it];
			double t = Sampled_indexToX (pitch, it), tleft = t - 0.5 * pitch -> dx, tright = t + 0.5 * pitch -> dx;
			double f = frame -> candidate [1]. frequency;
			if ((f > 0.0 && f < pitch -> ceiling) || tright <= d_startWindow || tleft >= d_endWindow) continue;
			if (tleft < d_startWindow) tleft = d_startWindow;
			if (tright > d_endWindow) tright = d_endWindow;
			Graphics_fillRectangle (d_graphics, tleft, tright, 0, 1);
		}
		Graphics_setColour (d_graphics, Graphics_BLACK);
		Graphics_resetViewport (d_graphics, previous);
	}
}

void structPitchEditor :: v_play (double a_tmin, double a_tmax) {
	Pitch_hum ((Pitch) data, a_tmin, a_tmax);
}

int structPitchEditor :: v_click (double xWC, double yWC, bool dummy) {
	Pitch pitch = (Pitch) data;
	double dyUnv = Graphics_dyMMtoWC (d_graphics, HEIGHT_UNV);
	double dyIntens = Graphics_dyMMtoWC (d_graphics, HEIGHT_INTENS);
	double frequency = (yWC - dyUnv) / (1 - dyIntens - dyUnv) * pitch -> ceiling, tmid;
	double minimumDf = 1e30;
	int cand, bestCandidate = -1;

	long ibestFrame;
	Pitch_Frame bestFrame;
	ibestFrame = Sampled_xToNearestIndex (pitch, xWC);
	if (ibestFrame < 1) ibestFrame = 1;
	if (ibestFrame > pitch -> nx) ibestFrame = pitch -> nx;
	bestFrame = & pitch -> frame [ibestFrame];

	tmid = Sampled_indexToX (pitch, ibestFrame);
	for (cand = 1; cand <= bestFrame -> nCandidates; cand ++) {
		double df = frequency - bestFrame -> candidate [cand]. frequency;
		if (fabs (df) < minimumDf) {
			minimumDf = fabs (df);
			bestCandidate = cand;
		}
	}
	if (bestCandidate != -1) {
		double bestFrequency = bestFrame -> candidate [bestCandidate]. frequency;
		double distanceWC = (frequency - bestFrequency) / pitch -> ceiling * (1 - dyIntens - dyUnv);
		double dx_mm = Graphics_dxWCtoMM (d_graphics, xWC - tmid), dy_mm = Graphics_dyWCtoMM (d_graphics, distanceWC);
		if (bestFrequency < pitch -> ceiling &&   // above ceiling: ignore
		    ((bestFrequency <= 0.0 && fabs (xWC - tmid) <= 0.5 * pitch -> dx && frequency <= 0.0) ||   // voiceless: click within frame
		     (bestFrequency > 0.0 && dx_mm * dx_mm + dy_mm * dy_mm <= RADIUS * RADIUS)))   // voiced: click within circle
		{
			struct structPitch_Candidate help = bestFrame -> candidate [1];
			Editor_save (this, L"Change path");
			bestFrame -> candidate [1] = bestFrame -> candidate [bestCandidate];
			bestFrame -> candidate [bestCandidate] = help;
			FunctionEditor_redraw (this);
			broadcastDataChanged ();
			d_startSelection = d_endSelection = tmid;   // cursor will snap to candidate
			return 1;
		} else {
			return PitchEditor_Parent :: v_click (xWC, yWC, dummy);   // move cursor or drag selection
		}
	}
	return PitchEditor_Parent :: v_click (xWC, yWC, dummy);   // move cursor or drag selection
}

PitchEditor PitchEditor_create (const wchar_t *title, Pitch pitch) {
	try {
		autoPitchEditor me = Thing_new (PitchEditor);
		FunctionEditor_init (me.peek(), title, pitch);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Pitch window not created.");
	}
}

/* End of file PitchEditor.cpp */
