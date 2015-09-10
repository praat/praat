/* RunnerMFC.cpp
 *
 * Copyright (C) 2001-2011,2013,2015 Paul Boersma
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
 * a selection of changes:
 * pb 2002/07/08 goodness
 * pb 2005/11/21 play again
 * pb 2005/12/02 response sounds are played
 * pb 2005/12/04 oops button
 * pb 2005/12/08 multiple experiments
 * pb 2011/03/03 reaction times for mouse clicks
 * pb 2011/04/14 reaction times for key presses
 */

#include "RunnerMFC.h"
#include "EditorM.h"
#include "machine.h"

Thing_implement (RunnerMFC, Editor, 0);

void structRunnerMFC :: v_destroy () {
	if (our experiments) {
		our experiments -> size = 0;   // give ownership back to whoever thinks they own the experiments. BUG: can be dontOwnItems
		forget (our experiments);
	}
	forget (our graphics);
	our RunnerMFC_Parent :: v_destroy ();
}

void structRunnerMFC :: v_dataChanged () {
	Graphics_updateWs (our graphics);
}

static int RunnerMFC_startExperiment (RunnerMFC me) {
	my data = (Daata) my experiments -> item [my iexperiment];
	Melder_assert (my data -> classInfo == classExperimentMFC);
	ExperimentMFC_start ((ExperimentMFC) my data);
	Thing_setName (me, ((ExperimentMFC) my data) -> name);
	Editor_broadcastDataChanged (me);
	Graphics_updateWs (my graphics);
	return 1;
}

static void drawControlButton (RunnerMFC me, double left, double right, double bottom, double top, const char32 *visibleText) {
	Graphics_setColour (my graphics, Graphics_MAROON);
	Graphics_setLineWidth (my graphics, 3.0);
	Graphics_fillRectangle (my graphics, left, right, bottom, top);
	Graphics_setColour (my graphics, Graphics_YELLOW);
	Graphics_rectangle (my graphics, left, right, bottom, top);
	Graphics_text (my graphics, 0.5 * (left + right), 0.5 * (bottom + top), visibleText);
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (RunnerMFC);
	Melder_assert (event -> widget == my d_drawingArea);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	ExperimentMFC experiment = (ExperimentMFC) my data;
	long iresponse;
	if (my data == NULL) return;
	Graphics_setGrey (my graphics, 0.8);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setGrey (my graphics, 0.0);
	if (experiment -> trial == 0) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setFontSize (my graphics, 24);
		Graphics_text (my graphics, 0.5, 0.5, experiment -> startText);
	} else if (experiment -> pausing) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setFontSize (my graphics, 24);
		Graphics_text (my graphics, 0.5, 0.5, experiment -> pauseText);
		if (experiment -> oops_right > experiment -> oops_left && experiment -> trial > 1) {
			drawControlButton (me,
				experiment -> oops_left, experiment -> oops_right, experiment -> oops_bottom, experiment -> oops_top,
				experiment -> oops_label);
		}
	} else if (experiment -> trial <= experiment -> numberOfTrials) {
		const char32 *visibleText = experiment -> stimulus [experiment -> stimuli [experiment -> trial]]. visibleText;
		autostring32 visibleText_dup = Melder_dup_f (visibleText ? visibleText : U"");
		char32 *visibleText_p = visibleText_dup.peek();
		Graphics_setFont (my graphics, kGraphics_font_TIMES);
		Graphics_setFontSize (my graphics, 10);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
		Graphics_text (my graphics, 0, 1,   experiment -> trial, U" / ", experiment -> numberOfTrials);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_setFontSize (my graphics, 24);
		/*
		 * The run text.
		 */
		if (visibleText_p [0] != U'\0') {
			char32 *visibleText_q = str32chr (visibleText_p, U'|');
			if (visibleText_q) *visibleText_q = '\0';
			Graphics_text (my graphics, 0.5, 1.0, visibleText_p [0] != '\0' ? visibleText_p : experiment -> runText);
			if (visibleText_q) visibleText_p = visibleText_q + 1; else visibleText_p += str32len (visibleText_p);
		} else {
			Graphics_text (my graphics, 0.5, 1.0, experiment -> runText);
		}
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		for (iresponse = 1; iresponse <= experiment -> numberOfDifferentResponses; iresponse ++) {
			ResponseMFC response = & experiment -> response [iresponse];
			char32 *textToDraw = response -> label;   // can be overridden
			if (visibleText_p [0] != U'\0') {
				char32 *visibleText_q = str32chr (visibleText_p, U'|');
				if (visibleText_q) *visibleText_q = U'\0';
				textToDraw = visibleText_p;   // override
				if (visibleText_q) visibleText_p = visibleText_q + 1; else visibleText_p += str32len (visibleText_p);
			}
			if (str32nequ (textToDraw, U"\\FI", 3)) {
				structMelderFile file = { 0 };
				MelderDir_relativePathToFile (& experiment -> rootDirectory, textToDraw + 3, & file);
				Graphics_imageFromFile (my graphics, Melder_fileToPath (& file), response -> left, response -> right, response -> bottom, response -> top);
			} else {
				Graphics_setColour (my graphics,
					response -> name [0] == U'\0' ? Graphics_SILVER :
					experiment -> responses [experiment -> trial] == iresponse ? Graphics_RED :
					experiment -> ok_right > experiment -> ok_left || experiment -> responses [experiment -> trial] == 0 ?
					Graphics_YELLOW : Graphics_SILVER);
				Graphics_setLineWidth (my graphics, 3.0);
				Graphics_fillRectangle (my graphics, response -> left, response -> right, response -> bottom, response -> top);
				Graphics_setColour (my graphics, Graphics_MAROON);
				Graphics_rectangle (my graphics, response -> left, response -> right, response -> bottom, response -> top);
				Graphics_setFontSize (my graphics, response -> fontSize ? response -> fontSize : 24);
				Graphics_text (my graphics, 0.5 * (response -> left + response -> right),
					0.5 * (response -> bottom + response -> top), textToDraw);
			}
			Graphics_setFontSize (my graphics, 24);
		}
		for (iresponse = 1; iresponse <= experiment -> numberOfGoodnessCategories; iresponse ++) {
			GoodnessMFC goodness = & experiment -> goodness [iresponse];
			Graphics_setColour (my graphics, experiment -> responses [experiment -> trial] == 0 ? Graphics_SILVER :
				experiment -> goodnesses [experiment -> trial] == iresponse ? Graphics_RED : Graphics_YELLOW);
			Graphics_setLineWidth (my graphics, 3.0);
			Graphics_fillRectangle (my graphics, goodness -> left, goodness -> right, goodness -> bottom, goodness -> top);
			Graphics_setColour (my graphics, Graphics_MAROON);
			Graphics_rectangle (my graphics, goodness -> left, goodness -> right, goodness -> bottom, goodness -> top);
			Graphics_text (my graphics, 0.5 * (goodness -> left + goodness -> right), 0.5 * (goodness -> bottom + goodness -> top), goodness -> label);
		}
		if (experiment -> replay_right > experiment -> replay_left && my numberOfReplays < experiment -> maximumNumberOfReplays) {
			drawControlButton (me,
				experiment -> replay_left, experiment -> replay_right, experiment -> replay_bottom, experiment -> replay_top,
				experiment -> replay_label);
		}
		if (experiment -> ok_right > experiment -> ok_left &&
		    experiment -> responses [experiment -> trial] != 0 &&
		    (experiment -> numberOfGoodnessCategories == 0 || experiment -> goodnesses [experiment -> trial] != 0))
		{
			drawControlButton (me,
				experiment -> ok_left, experiment -> ok_right, experiment -> ok_bottom, experiment -> ok_top,
				experiment -> ok_label);
		}
		if (experiment -> oops_right > experiment -> oops_left && experiment -> trial > 1) {
			drawControlButton (me,
				experiment -> oops_left, experiment -> oops_right, experiment -> oops_bottom, experiment -> oops_top,
				experiment -> oops_label);
		}
	} else {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setFontSize (my graphics, 24);
		Graphics_text (my graphics, 0.5, 0.5, experiment -> endText);
		if (experiment -> oops_right > experiment -> oops_left && experiment -> trial > 1) {
			drawControlButton (me,
				experiment -> oops_left, experiment -> oops_right, experiment -> oops_bottom, experiment -> oops_top,
				experiment -> oops_label);
		}
	}
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (RunnerMFC);
	if (my graphics == NULL) return;
	Graphics_setWsViewport (my graphics, 0, event -> width, 0, event -> height);
	Graphics_setWsWindow (my graphics, 0, event -> width, 0, event -> height);
	Graphics_setViewport (my graphics, 0, event -> width, 0, event -> height);
	Graphics_updateWs (my graphics);
}

static void do_ok (RunnerMFC me) {
	ExperimentMFC experiment = (ExperimentMFC) my data;
	Melder_assert (experiment -> trial >= 1 && experiment -> trial <= experiment -> numberOfTrials);
	my numberOfReplays = 0;
	if (experiment -> trial == experiment -> numberOfTrials) {
		experiment -> trial ++;
		Editor_broadcastDataChanged (me);
		Graphics_updateWs (my graphics);
	} else if (experiment -> breakAfterEvery != 0 && experiment -> trial % experiment -> breakAfterEvery == 0) {
		experiment -> pausing = TRUE;
		Editor_broadcastDataChanged (me);
		Graphics_updateWs (my graphics);
	} else {
		experiment -> trial ++;
		Editor_broadcastDataChanged (me);
		if (experiment -> blankWhilePlaying) {
			Graphics_setGrey (my graphics, 0.8);
			Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
			Graphics_setGrey (my graphics, 0.0);
			Graphics_flushWs (my graphics);
		}
		Graphics_updateWs (my graphics);
		if (experiment -> stimuliAreSounds) {
			autoMelderAudioSaveMaximumAsynchronicity saveMaximumAsynchronicity;
			if (experiment -> blankWhilePlaying)
				 MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_SYNCHRONOUS);
			ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
		}
	}
}

static void do_oops (RunnerMFC me) {
	ExperimentMFC experiment = (ExperimentMFC) my data;
	Melder_assert (experiment -> trial >= 2 && experiment -> trial <= experiment -> numberOfTrials + 1);
	if (experiment -> trial <= experiment -> numberOfTrials) {
		experiment -> responses [experiment -> trial] = 0;
		experiment -> goodnesses [experiment -> trial] = 0;
	}
	experiment -> trial --;
	experiment -> responses [experiment -> trial] = 0;
	experiment -> goodnesses [experiment -> trial] = 0;
	experiment -> pausing = FALSE;
	my numberOfReplays = 0;
	Editor_broadcastDataChanged (me);
	if (experiment -> blankWhilePlaying) {
		Graphics_setGrey (my graphics, 0.8);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		Graphics_setGrey (my graphics, 0.0);
		Graphics_flushWs (my graphics);
	}
	Graphics_updateWs (my graphics);
	if (experiment -> stimuliAreSounds) {
		autoMelderAudioSaveMaximumAsynchronicity saveMaximumAsynchronicity;
		if (experiment -> blankWhilePlaying)
			MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_SYNCHRONOUS);
		ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
	}
}

static void do_replay (RunnerMFC me) {
	ExperimentMFC experiment = (ExperimentMFC) my data;
	Melder_assert (experiment -> trial >= 1 && experiment -> trial <= experiment -> numberOfTrials);
	my numberOfReplays ++;
	Editor_broadcastDataChanged (me);
	if (experiment -> blankWhilePlaying) {
		Graphics_setGrey (my graphics, 0.8);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		Graphics_setGrey (my graphics, 0.0);
		Graphics_flushWs (my graphics);
	}
	Graphics_updateWs (my graphics);
	if (experiment -> stimuliAreSounds) {
		autoMelderAudioSaveMaximumAsynchronicity saveMaximumAsynchronicity;
		if (experiment -> blankWhilePlaying)
			MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_SYNCHRONOUS);
		ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
	}
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (RunnerMFC);
	if (my graphics == NULL) return;   // could be the case in the very beginning
	ExperimentMFC experiment = (ExperimentMFC) my data;
	if (my data == NULL) return;
	double reactionTime = Melder_clock () - experiment -> startingTime;
	if (! experiment -> blankWhilePlaying)
		reactionTime -= experiment -> stimulusInitialSilenceDuration;
	double x, y;
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & x, & y);
	if (experiment -> trial == 0) {   // the first click of the experiment
		experiment -> trial ++;
		Editor_broadcastDataChanged (me);
		if (experiment -> blankWhilePlaying) {
			Graphics_setGrey (my graphics, 0.8);
			Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
			Graphics_setGrey (my graphics, 0.0);
			Graphics_flushWs (my graphics);
		}
		Graphics_updateWs (my graphics);
		if (experiment -> stimuliAreSounds) {
			if (experiment -> numberOfTrials < 1) {
				Melder_flushError (U"There are zero trials in this experiment.");
				forget (me);
				return;
			}
			autoMelderAudioSaveMaximumAsynchronicity saveMaximumAsynchronicity;
			if (experiment -> blankWhilePlaying)
				MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_SYNCHRONOUS);
			ExperimentMFC_playStimulus (experiment, experiment -> stimuli [1]);   // works only if there is at least one trial
		}
	} else if (experiment -> pausing) {   // a click to leave the break
		if (x > experiment -> oops_left && x < experiment -> oops_right &&
			y > experiment -> oops_bottom && y < experiment -> oops_top && experiment -> trial > 1)
		{
			do_oops (me);
		} else {
			experiment -> pausing = FALSE;
			experiment -> trial ++;
			Editor_broadcastDataChanged (me);
			if (experiment -> blankWhilePlaying) {
				Graphics_setGrey (my graphics, 0.8);
				Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
				Graphics_setGrey (my graphics, 0.0);
				Graphics_flushWs (my graphics);
			}
			Graphics_updateWs (my graphics);
			if (experiment -> stimuliAreSounds) {
				autoMelderAudioSaveMaximumAsynchronicity saveMaximumAsynchronicity;
				if (experiment -> blankWhilePlaying)
					MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_SYNCHRONOUS);
				ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
			}
		}
	} else if (experiment -> trial <= experiment -> numberOfTrials) {
		long iresponse;
		if (x > experiment -> ok_left && x < experiment -> ok_right &&
			y > experiment -> ok_bottom && y < experiment -> ok_top &&
			experiment -> responses [experiment -> trial] != 0 &&
			(experiment -> numberOfGoodnessCategories == 0 || experiment -> goodnesses [experiment -> trial] != 0))
		{
			do_ok (me);
		} else if (x > experiment -> replay_left && x < experiment -> replay_right &&
			y > experiment -> replay_bottom && y < experiment -> replay_top && my numberOfReplays < experiment -> maximumNumberOfReplays)
		{
			do_replay (me);
		} else if (x > experiment -> oops_left && x < experiment -> oops_right &&
			y > experiment -> oops_bottom && y < experiment -> oops_top && experiment -> trial > 1)
		{
			do_oops (me);
		} else if (experiment -> responses [experiment -> trial] == 0 || experiment -> ok_right > experiment -> ok_left) {
			for (iresponse = 1; iresponse <= experiment -> numberOfDifferentResponses; iresponse ++) {
				ResponseMFC response = & experiment -> response [iresponse];
				if (x > response -> left && x < response -> right && y > response -> bottom && y < response -> top && response -> name [0] != '\0') {
					experiment -> responses [experiment -> trial] = iresponse;
					experiment -> reactionTimes [experiment -> trial] = reactionTime;
					if (experiment -> responsesAreSounds) {
						ExperimentMFC_playResponse (experiment, iresponse);
					}
					if (experiment -> ok_right <= experiment -> ok_left && experiment -> numberOfGoodnessCategories == 0) {
						do_ok (me);
					} else {
						Editor_broadcastDataChanged (me);
						Graphics_updateWs (my graphics);
					}
				}
			}
			if (experiment -> responses [experiment -> trial] != 0 && experiment -> ok_right > experiment -> ok_left) {
				for (iresponse = 1; iresponse <= experiment -> numberOfGoodnessCategories; iresponse ++) {
					GoodnessMFC cat = & experiment -> goodness [iresponse];
					if (x > cat -> left && x < cat -> right && y > cat -> bottom && y < cat -> top) {
						experiment -> goodnesses [experiment -> trial] = iresponse;
						Editor_broadcastDataChanged (me);
						Graphics_updateWs (my graphics);
					}
				}
			}
		} else if (experiment -> responses [experiment -> trial] != 0) {
			Melder_assert (experiment -> ok_right <= experiment -> ok_left);
			for (iresponse = 1; iresponse <= experiment -> numberOfGoodnessCategories; iresponse ++) {
				GoodnessMFC cat = & experiment -> goodness [iresponse];
				if (x > cat -> left && x < cat -> right && y > cat -> bottom && y < cat -> top) {
					experiment -> goodnesses [experiment -> trial] = iresponse;
					do_ok (me);
				}
			}
		}
	} else {
		if (x > experiment -> oops_left && x < experiment -> oops_right &&
			y > experiment -> oops_bottom && y < experiment -> oops_top)
		{
			do_oops (me);
			return;
		}
		if (my iexperiment < my experiments -> size) {
			my iexperiment ++;
			if (! RunnerMFC_startExperiment (me)) {
				Melder_flushError ();
				forget (me);
				return;
			}
		}
	}
}

static void gui_drawingarea_cb_key (I, GuiDrawingAreaKeyEvent event) {
	iam (RunnerMFC);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	ExperimentMFC experiment = (ExperimentMFC) my data;
	if (my data == NULL) return;
	double reactionTime = Melder_clock () - experiment -> startingTime;
	if (! experiment -> blankWhilePlaying)
		reactionTime -= experiment -> stimulusInitialSilenceDuration;
	if (experiment -> trial == 0) {
	} else if (experiment -> pausing) {
	} else if (experiment -> trial <= experiment -> numberOfTrials) {
		long iresponse;
		if (experiment -> ok_key != NULL && experiment -> ok_key [0] == event -> key &&
			experiment -> responses [experiment -> trial] != 0 &&
			(experiment -> numberOfGoodnessCategories == 0 || experiment -> goodnesses [experiment -> trial] != 0))
		{
			do_ok (me);
		} else if (experiment -> replay_key != NULL && experiment -> replay_key [0] == event -> key &&
			my numberOfReplays < experiment -> maximumNumberOfReplays)
		{
			do_replay (me);
		} else if (experiment -> oops_key != NULL && experiment -> oops_key [0] == event -> key) {
			if (experiment -> trial > 1) {
				do_oops (me);
			}
		} else if (experiment -> responses [experiment -> trial] == 0) {
			for (iresponse = 1; iresponse <= experiment -> numberOfDifferentResponses; iresponse ++) {
				ResponseMFC response = & experiment -> response [iresponse];
				if (response -> key != NULL && response -> key [0] == event -> key) {
					experiment -> responses [experiment -> trial] = iresponse;
					experiment -> reactionTimes [experiment -> trial] = reactionTime;
					if (experiment -> responsesAreSounds) {
						ExperimentMFC_playResponse (experiment, iresponse);
					}
					if (experiment -> ok_right <= experiment -> ok_left && experiment -> numberOfGoodnessCategories == 0) {
						do_ok (me);
					} else {
						Editor_broadcastDataChanged (me);
						Graphics_updateWs (my graphics);
					}
				}
			}
		}
	}
}

void structRunnerMFC :: v_createChildren () {
	our d_drawingArea = GuiDrawingArea_createShown (our d_windowForm, 0, 0, Machine_getMenuBarHeight (), 0,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
}

RunnerMFC RunnerMFC_create (const char32 *title, Ordered experiments) {
	try {
		autoRunnerMFC me = Thing_new (RunnerMFC);
		Editor_init (me.peek(), 0, 0, 2000, 2000, title, NULL);
		my experiments = experiments;
		my graphics = Graphics_create_xmdrawingarea (my d_drawingArea);

struct structGuiDrawingAreaResizeEvent event = { my d_drawingArea, 0 };
event. width  = GuiControl_getWidth  (my d_drawingArea);
event. height = GuiControl_getHeight (my d_drawingArea);
gui_drawingarea_cb_resize (me.peek(), & event);

		my iexperiment = 1;
		RunnerMFC_startExperiment (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"Experiment window not created.");
	}
}

/* End of file RunnerMFC.cpp */
