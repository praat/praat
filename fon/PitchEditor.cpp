/* PitchEditor.cpp
 *
 * Copyright (C) 1992-2012,2014-2020 Paul Boersma
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
		POSITIVE (ceiling, U"Ceiling (Hz)", U"600.0")
	EDITOR_OK
		Pitch pitch = (Pitch) my data;
		SET_REAL (ceiling, pitch -> ceiling)
	EDITOR_DO
		Pitch pitch = (Pitch) my data;
		Editor_save (me, U"Change ceiling");
		Pitch_setCeiling (pitch, ceiling);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_pathFinder (PitchEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Path finder", nullptr)
		REAL (silenceThreshold, U"Silence threshold", U"0.03")
		REAL (voicingThreshold, U"Voicing threshold", U"0.45")
		REAL (octaveCost, U"Octave cost", U"0.01")
		REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
		REAL (voicedUnvoicedCost, U"Voiced/unvoiced cost", U"0.14")
		POSITIVE (ceiling, U"Ceiling (Hz)", U"600.0")
		BOOLEAN (pullFormants, U"Pull formants", false)
	EDITOR_OK
		Pitch pitch = (Pitch) my data;
		SET_REAL (ceiling, pitch -> ceiling)
	EDITOR_DO
		Pitch pitch = (Pitch) my data;
		Editor_save (me, U"Path finder");
		Pitch_pathFinder (pitch, silenceThreshold, voicingThreshold,
			octaveCost, octaveJumpCost, voicedUnvoicedCost, ceiling, pullFormants);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_getPitch (PitchEditor me, EDITOR_ARGS_DIRECT) {
	Pitch pitch = (Pitch) my data;
	if (my startSelection == my endSelection) {
		Melder_informationReal (Pitch_getValueAtTime (pitch, my startSelection, kPitch_unit::HERTZ, 1), U"Hz");
	} else {
		Melder_informationReal (Pitch_getMean (pitch, my startSelection, my endSelection, kPitch_unit::HERTZ), U"Hz");
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
	const Pitch pitch = (Pitch) my data;
	const integer ileft = std::max (1_integer, Sampled_xToHighIndex (pitch, my startSelection));
	const integer iright = std::min (Sampled_xToLowIndex (pitch, my endSelection), pitch -> nx);
	Editor_save (me, U"Unvoice");
	for (integer iframe = ileft; iframe <= iright; iframe ++) {
		const Pitch_Frame frame = & pitch -> frames [iframe];
		for (integer cand = 1; cand <= frame -> nCandidates; cand ++)
			if (frame -> candidates [cand]. frequency == 0.0)
				std::swap (frame -> candidates [1], frame -> candidates [cand]);
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

	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);

	double dyUnv = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_UNV);
	double dyIntens = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_INTENS);

	integer it1, it2;
	Sampled_getWindowSamples (pitch, our startWindow, our endWindow, & it1, & it2);

	/*
		Show pitch.
	*/
	{
		double df =
			pitch -> ceiling > 10000.0 ? 2000.0 :
			pitch -> ceiling > 5000.0 ? 1000.0 :
			pitch -> ceiling > 2000.0 ? 500.0 :
			pitch -> ceiling > 800.0 ? 200.0 :
			pitch -> ceiling > 400.0 ? 100.0 :
			50.0;
		double radius;
		Graphics_Viewport previous;
		previous = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, dyUnv, 1.0 - dyIntens);
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, pitch -> ceiling);
		radius = Graphics_dxMMtoWC (our graphics.get(), RADIUS);

		/* Horizontal hair at current pitch. */

		if (our startSelection == our endSelection && our startSelection >= our startWindow && our startSelection <= our endWindow) {
			double frequency = Pitch_getValueAtTime (pitch, our startSelection, kPitch_unit::HERTZ, Pitch_LINEAR);
			if (isdefined (frequency)) {
				Graphics_setColour (our graphics.get(), Melder_RED);
				Graphics_line (our graphics.get(), our startWindow - radius, frequency, our endWindow, frequency);
				Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
				Graphics_text (our graphics.get(), our startWindow - radius, frequency, Melder_fixed (frequency, 2));
			}
		}

		/* Horizontal scaling lines. */

		Graphics_setColour (our graphics.get(), Melder_BLUE);
		Graphics_setLineType (our graphics.get(), Graphics_DOTTED);
		Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
		for (double frequency = df; frequency <= pitch -> ceiling; frequency += df) {
			Graphics_line (our graphics.get(), our startWindow, frequency, our endWindow, frequency);
			Graphics_text (our graphics.get(), our endWindow + 0.5 * radius, frequency,   frequency, U" Hz");
		}
		Graphics_setLineType (our graphics.get(), Graphics_DRAWN);

		/*
			Show candidates.
		*/
		for (integer it = it1; it <= it2; it ++) {
			const Pitch_Frame frame = & pitch -> frames [it];
			const double time = Sampled_indexToX (pitch, it);
			double frequency = frame -> candidates [1]. frequency;
			if (Pitch_util_frequencyIsVoiced (frequency, pitch -> ceiling)) {
				Graphics_setColour (our graphics.get(), Melder_MAGENTA);
				Graphics_fillCircle_mm (our graphics.get(), time, frequency, RADIUS * 2.0);
			}
			Graphics_setColour (our graphics.get(), Melder_BLACK);
			Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
			for (integer icand = 1; icand <= frame -> nCandidates; icand ++) {
				frequency = frame -> candidates [icand]. frequency;
				if (Pitch_util_frequencyIsVoiced (frequency, pitch -> ceiling)) {
					const integer strength = Melder_clippedRight (Melder_iround (10.0 * frame -> candidates [icand]. strength),
							9_integer);   // map 0.0-1.0 to 0-9
					Graphics_text (our graphics.get(), time, frequency,   strength);
				}
			}
		}
		Graphics_resetViewport (our graphics.get(), previous);
	}

	/*
		Show intensity.
	*/
	{
		Graphics_Viewport previous = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - dyIntens, 1.0);
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics.get(), our startWindow, 0.5, U"intens");
		Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics.get(), our endWindow, 0.5, U"intens");
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
		for (integer it = it1; it <= it2; it ++) {
			const Pitch_Frame frame = & pitch -> frames [it];
			const double time = Sampled_indexToX (pitch, it);
			const integer strength = Melder_clippedRight (Melder_iround (10.0 * frame -> intensity + 0.5),
					9_integer);   // map 0.0-1.0 to 1-9
			Graphics_text (our graphics.get(), time, 0.5,   strength);
		}
		Graphics_resetViewport (our graphics.get(), previous);
	}

	if (it1 > 1)
		it1 -= 1;
	if (it2 < pitch -> nx)
		it2 += 1;

	/*
		Show voicelessness.
	*/
	{
		Graphics_Viewport previous = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, dyUnv);
		Graphics_setColour (our graphics.get(), Melder_BLUE);
		Graphics_line (our graphics.get(), our startWindow, 1.0, our endWindow, 1.0);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics.get(), our startWindow, 0.5, U"Unv");
		Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics.get(), our endWindow, 0.5, U"Unv");
		for (integer it = it1; it <= it2; it ++) {
			const Pitch_Frame frame = & pitch -> frames [it];
			const double time = Sampled_indexToX (pitch, it);
			double tleft = time - 0.5 * pitch -> dx, tright = time + 0.5 * pitch -> dx;
			double frequency = frame -> candidates [1]. frequency;
			if (Pitch_util_frequencyIsVoiced (frequency, pitch -> ceiling) || tright <= our startWindow || tleft >= our endWindow)
				continue;
			Melder_clipLeft (our startWindow, & tleft);
			Melder_clipRight (& tright, our endWindow);
			Graphics_fillRectangle (our graphics.get(), tleft, tright, 0.0, 1.0);
		}
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_resetViewport (our graphics.get(), previous);
	}
}

void structPitchEditor :: v_play (double a_tmin, double a_tmax) {
	Pitch_hum ((Pitch) our data, a_tmin, a_tmax);
}

bool structPitchEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (! event -> isClick())
		return PitchEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);   // move cursor or drag selection
	const Pitch pitch = (Pitch) our data;
	const double dyUnv = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_UNV);
	const double dyIntens = Graphics_dyMMtoWC (our graphics.get(), HEIGHT_INTENS);
	const double clickedFrequency = (y_fraction - dyUnv) / (1.0 - dyIntens - dyUnv) * pitch -> ceiling;
	double minimumDf = 1e30;
	integer bestCandidate = -1;

	integer ibestFrame = Sampled_xToNearestIndex (pitch, x_world);
	Melder_clip (1_integer, & ibestFrame, pitch -> nx);
	const Pitch_Frame bestFrame = & pitch -> frames [ibestFrame];

	const double tmid = Sampled_indexToX (pitch, ibestFrame);
	for (integer icand = 1; icand <= bestFrame -> nCandidates; icand ++) {
		const Pitch_Candidate candidate = & bestFrame -> candidates [icand];
		const double df = clickedFrequency - candidate -> frequency;
		if (fabs (df) < minimumDf) {
			minimumDf = fabs (df);
			bestCandidate = icand;
		}
	}
	if (bestCandidate != -1) {
		const double bestFrequency = bestFrame -> candidates [bestCandidate]. frequency;
		const double distanceWC = (clickedFrequency - bestFrequency) / pitch -> ceiling * (1.0 - dyIntens - dyUnv);
		const double dx_mm = Graphics_dxWCtoMM (our graphics.get(), x_world - tmid), dy_mm = Graphics_dyWCtoMM (our graphics.get(), distanceWC);
		if (bestFrequency < pitch -> ceiling &&   // above ceiling: ignore
		    ((bestFrequency <= 0.0 && fabs (x_world - tmid) <= 0.5 * pitch -> dx && clickedFrequency <= 0.0) ||   // voiceless: click within frame
		     (bestFrequency > 0.0 && dx_mm * dx_mm + dy_mm * dy_mm <= RADIUS * RADIUS)))   // voiced: click within circle
		{
			Editor_save (this, U"Change path");
			std::swap (bestFrame -> candidates [1], bestFrame -> candidates [bestCandidate]);
			FunctionEditor_redraw (this);
			Editor_broadcastDataChanged (this);
			our startSelection = our endSelection = tmid;   // cursor will snap to candidate
			return FunctionEditor_UPDATE_NEEDED;
		} else {
			return PitchEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);   // move cursor or drag selection
		}
	}
	return PitchEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);   // move cursor or drag selection
}

autoPitchEditor PitchEditor_create (conststring32 title, Pitch pitch) {
	try {
		autoPitchEditor me = Thing_new (PitchEditor);
		FunctionEditor_init (me.get(), title, pitch);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Pitch window not created.");
	}
}

/* End of file PitchEditor.cpp */
