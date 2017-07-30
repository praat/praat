/* PitchEditor.cpp
 *
 * Copyright (C) 1992-2011,2012,2014,2015,2016,2017 Paul Boersma
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

#include "Pitch_to_Sound.h"
#include "PitchEditor.h"
#include "EditorM.h"

Thing_implement (PitchEditor, FunctionEditor, 0);

#define HEIGHT_UNV  3.0
#define HEIGHT_INTENS  6.0
#define RADIUS  2.5

/********** MENU COMMANDS **********/

static void menu_cb_setCeiling (PitchEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Change ceiling", nullptr)
		POSITIVE (U"Ceiling (Hz)", U"600.0")
	EDITOR_OK
		Pitch pitch = (Pitch) my data;
		SET_REAL (U"Ceiling", pitch -> ceiling)
	EDITOR_DO
		Pitch pitch = (Pitch) my data;
		Editor_save (me, U"Change ceiling");
		Pitch_setCeiling (pitch, GET_REAL (U"Ceiling"));
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_pathFinder (PitchEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Path finder", nullptr)
		REAL (U"Silence threshold", U"0.03")
		REAL (U"Voicing threshold", U"0.45")
		REAL (U"Octave cost", U"0.01")
		REAL (U"Octave-jump cost", U"0.35")
		REAL (U"Voiced/unvoiced cost", U"0.14")
		POSITIVE (U"Ceiling (Hz)", U"600.0")
		BOOLEAN (U"Pull formants", false)
	EDITOR_OK
		Pitch pitch = (Pitch) my data;
		SET_REAL (U"Ceiling", pitch -> ceiling)
	EDITOR_DO
		Pitch pitch = (Pitch) my data;
		Editor_save (me, U"Path finder");
		Pitch_pathFinder (pitch,
			GET_REAL (U"Silence threshold"), GET_REAL (U"Voicing threshold"),
			GET_REAL (U"Octave cost"), GET_REAL (U"Octave-jump cost"),
			GET_REAL (U"Voiced/unvoiced cost"), GET_REAL (U"Ceiling"), GET_INTEGER (U"Pull formants"));
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_getPitch (PitchEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) {
		Melder_informationReal (Pitch_getValueAtTime ((Pitch) my data, my startSelection, kPitch_unit_HERTZ, 1), U"Hz");
	} else {
		Melder_informationReal (Pitch_getMean ((Pitch) my data, my startSelection, my endSelection, kPitch_unit_HERTZ), U"Hz");
	}
}

static void menu_cb_octaveUp (PitchEditor me, EDITOR_ARGS_DIRECT) {
	Pitch pitch = (Pitch) my data;
	Editor_save (me, U"Octave up");
	Pitch_step (pitch, 2.0, 0.1, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_fifthUp (PitchEditor me, EDITOR_ARGS_DIRECT) {
	Pitch pitch = (Pitch) my data;
	Editor_save (me, U"Fifth up");
	Pitch_step (pitch, 1.5, 0.1, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_fifthDown (PitchEditor me, EDITOR_ARGS_DIRECT) {
	Pitch pitch = (Pitch) my data;
	Editor_save (me, U"Fifth down");
	Pitch_step (pitch, 1 / 1.5, 0.1, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_octaveDown (PitchEditor me, EDITOR_ARGS_DIRECT) {
	Pitch pitch = (Pitch) my data;
	Editor_save (me, U"Octave down");
	Pitch_step (pitch, 0.5, 0.1, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_voiceless (PitchEditor me, EDITOR_ARGS_DIRECT) {
	Pitch pitch = (Pitch) my data;
	long ileft = Sampled_xToHighIndex (pitch, my startSelection);
	long iright = Sampled_xToLowIndex (pitch, my endSelection);
	if (ileft < 1) ileft = 1;
	if (iright > pitch -> nx) iright = pitch -> nx;
	Editor_save (me, U"Unvoice");
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
	Editor_broadcastDataChanged (me);
}

static void menu_cb_PitchEditorHelp (PitchEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"PitchEditor"); }
static void menu_cb_PitchHelp (PitchEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Pitch"); }

void structPitchEditor :: v_createMenus () {
	PitchEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, U"Edit", U"Change ceiling...", 0, menu_cb_setCeiling);
	Editor_addCommand (this, U"Edit", U"Path finder...", 0, menu_cb_pathFinder);

	Editor_addCommand (this, U"Query", U"-- pitch --", 0, nullptr);
	Editor_addCommand (this, U"Query", U"Get pitch", GuiMenu_F5, menu_cb_getPitch);

	Editor_addMenu (this, U"Selection", 0);
	Editor_addCommand (this, U"Selection", U"Unvoice", 0, menu_cb_voiceless);
	Editor_addCommand (this, U"Selection", U"-- up and down --", 0, nullptr);
	Editor_addCommand (this, U"Selection", U"Octave up", 0, menu_cb_octaveUp);
	Editor_addCommand (this, U"Selection", U"Fifth up", 0, menu_cb_fifthUp);
	Editor_addCommand (this, U"Selection", U"Fifth down", 0, menu_cb_fifthDown);
	Editor_addCommand (this, U"Selection", U"Octave down", 0, menu_cb_octaveDown);
}

void structPitchEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PitchEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"PitchEditor help", U'?', menu_cb_PitchEditorHelp);
	EditorMenu_addCommand (menu, U"Pitch help", 0, menu_cb_PitchHelp);
}
	
/********** DRAWING AREA **********/

void structPitchEditor :: v_draw () {
	Pitch pitch = (Pitch) our data;
	long it, it1, it2;
	double dyUnv, dyIntens;

	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Graphics_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);

	dyUnv = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_UNV);
	dyIntens = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_INTENS);

	Sampled_getWindowSamples (pitch, our startWindow, our endWindow, & it1, & it2);

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
		previous = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, dyUnv, 1.0 - dyIntens);
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, pitch -> ceiling);
		radius = Graphics_dxMMtoWC (our graphics.get(), RADIUS);

		/* Horizontal hair at current pitch. */

		if (our startSelection == our endSelection && our startSelection >= our startWindow && our startSelection <= our endWindow) {
			double f = Pitch_getValueAtTime (pitch, our startSelection, kPitch_unit_HERTZ, Pitch_LINEAR);
			if (isdefined (f)) {
				Graphics_setColour (our graphics.get(), Graphics_RED);
				Graphics_line (our graphics.get(), our startWindow - radius, f, our endWindow, f);
				Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
				Graphics_text (our graphics.get(), our startWindow - radius, f, Melder_fixed (f, 2));
			}
		}

		/* Horizontal scaling lines. */

		Graphics_setColour (our graphics.get(), Graphics_BLUE);
		Graphics_setLineType (our graphics.get(), Graphics_DOTTED);
		Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
		for (long f = df; f <= pitch -> ceiling; f += df) {
			Graphics_line (our graphics.get(), our startWindow, f, our endWindow, f);
			Graphics_text (our graphics.get(), our endWindow + 0.5 * radius, f,   f, U" Hz");
		}
		Graphics_setLineType (our graphics.get(), Graphics_DRAWN);

		/* Show candidates. */

		for (it = it1; it <= it2; it ++) {
			Pitch_Frame frame = & pitch -> frame [it];
			double t = Sampled_indexToX (pitch, it);
			double f = frame -> candidate [1]. frequency;
			if (f > 0.0 && f < pitch -> ceiling) {
				Graphics_setColour (our graphics.get(), Graphics_MAGENTA);
				Graphics_fillCircle_mm (our graphics.get(), t, f, RADIUS * 2.0);
			}
			Graphics_setColour (our graphics.get(), Graphics_BLACK);
			Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
			for (int icand = 1; icand <= frame -> nCandidates; icand ++) {
				int strength = (int) floor (10 * frame -> candidate [icand]. strength + 0.5);
				f = frame -> candidate [icand]. frequency;
				if (strength > 9) strength = 9;
				if (f > 0 && f <= pitch -> ceiling) Graphics_text (our graphics.get(), t, f, strength);
			}
		}
		Graphics_resetViewport (our graphics.get(), previous);
	}

	/*
	 * Show intensity.
	 */
	{
		Graphics_Viewport previous = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - dyIntens, 1.0);
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Graphics_BLACK);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics.get(), our startWindow, 0.5, U"intens");
		Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics.get(), our endWindow, 0.5, U"intens");
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
		for (it = it1; it <= it2; it ++) {
			Pitch_Frame frame = & pitch -> frame [it];
			double t = Sampled_indexToX (pitch, it);
			long strength = lround (10 * frame -> intensity + 0.5);   // map 0.0-1.0 to 0-9
			if (strength > 9) strength = 9;
			Graphics_text (our graphics.get(), t, 0.5,   strength);
		}
		Graphics_resetViewport (our graphics.get(), previous);
	}

	if (it1 > 1) it1 -= 1;
	if (it2 < pitch -> nx) it2 += 1;

	/*
	 * Show voicelessness.
	 */
	{
		Graphics_Viewport previous = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, dyUnv);
		Graphics_setColour (our graphics.get(), Graphics_BLUE);
		Graphics_line (our graphics.get(), our startWindow, 1.0, our endWindow, 1.0);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics.get(), our startWindow, 0.5, U"Unv");
		Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics.get(), our endWindow, 0.5, U"Unv");
		for (it = it1; it <= it2; it ++) {
			Pitch_Frame frame = & pitch -> frame [it];
			double t = Sampled_indexToX (pitch, it), tleft = t - 0.5 * pitch -> dx, tright = t + 0.5 * pitch -> dx;
			double f = frame -> candidate [1]. frequency;
			if ((f > 0.0 && f < pitch -> ceiling) || tright <= our startWindow || tleft >= our endWindow) continue;
			if (tleft < our startWindow) tleft = our startWindow;
			if (tright > our endWindow) tright = our endWindow;
			Graphics_fillRectangle (our graphics.get(), tleft, tright, 0.0, 1.0);
		}
		Graphics_setColour (our graphics.get(), Graphics_BLACK);
		Graphics_resetViewport (our graphics.get(), previous);
	}
}

void structPitchEditor :: v_play (double a_tmin, double a_tmax) {
	Pitch_hum ((Pitch) our data, a_tmin, a_tmax);
}

bool structPitchEditor :: v_click (double xWC, double yWC, bool dummy) {
	Pitch pitch = (Pitch) our data;
	double dyUnv = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_UNV);
	double dyIntens = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_INTENS);
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
		double dx_mm = Graphics_dxWCtoMM (our graphics.get(), xWC - tmid), dy_mm = Graphics_dyWCtoMM (our graphics.get(), distanceWC);
		if (bestFrequency < pitch -> ceiling &&   // above ceiling: ignore
		    ((bestFrequency <= 0.0 && fabs (xWC - tmid) <= 0.5 * pitch -> dx && frequency <= 0.0) ||   // voiceless: click within frame
		     (bestFrequency > 0.0 && dx_mm * dx_mm + dy_mm * dy_mm <= RADIUS * RADIUS)))   // voiced: click within circle
		{
			struct structPitch_Candidate help = bestFrame -> candidate [1];
			Editor_save (this, U"Change path");
			bestFrame -> candidate [1] = bestFrame -> candidate [bestCandidate];
			bestFrame -> candidate [bestCandidate] = help;
			FunctionEditor_redraw (this);
			Editor_broadcastDataChanged (this);
			our startSelection = our endSelection = tmid;   // cursor will snap to candidate
			return FunctionEditor_UPDATE_NEEDED;
		} else {
			return PitchEditor_Parent :: v_click (xWC, yWC, dummy);   // move cursor or drag selection
		}
	}
	return PitchEditor_Parent :: v_click (xWC, yWC, dummy);   // move cursor or drag selection
}

autoPitchEditor PitchEditor_create (const char32 *title, Pitch pitch) {
	try {
		autoPitchEditor me = Thing_new (PitchEditor);
		FunctionEditor_init (me.get(), title, pitch);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Pitch window not created.");
	}
}

/* End of file PitchEditor.cpp */
