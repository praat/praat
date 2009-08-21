/* RunnerMFC.c
 *
 * Copyright (C) 2001-2008 Paul Boersma
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
 * pb 2001/07/18
 * pb 2002/07/08 goodness
 * pb 2002/07/16 GPL
 * pb 2005/11/21 play again
 * pb 2005/12/02 response sounds are played
 * pb 2005/12/04 oops button
 * pb 2005/12/08 multiple experiments
 * pb 2006/01/19 fixed a bug that caused an assertion violation when the oops button was pressed after the experiment finished
 * pb 2006/02/23 repaired small memory leak in destroy()
 * pb 2007/08/12 wchar_t
 * pb 2008/03/31 correct error message when the second of multiple experiments cannot start
 * pb 2008/04/08 disable the OK key if no response has been given yet
 */

#include "RunnerMFC.h"
#include "EditorM.h"
#include "machine.h"

#define RunnerMFC__members(Klas) Editor__members(Klas) \
	Widget drawingArea; \
	Ordered experiments; \
	long iexperiment; \
	Graphics graphics; \
	long numberOfReplays;
#define RunnerMFC__methods(Klas) Editor__methods(Klas)
Thing_declare2 (RunnerMFC, Editor);

static void destroy (I) {
	iam (RunnerMFC);
	if (my experiments) {
		my experiments -> size = 0;   /* Give ownership back to whoever thinks they own the experiments. */
		forget (my experiments);
	}
	forget (my graphics);
	inherited (RunnerMFC) destroy (me);
}

static void dataChanged (RunnerMFC me) {
	Graphics_updateWs (my graphics);
}

static int RunnerMFC_startExperiment (RunnerMFC me) {
	my data = my experiments -> item [my iexperiment];
	if (! ExperimentMFC_start (my data)) return Melder_error1 (L"Cannot start experiment.");
	Thing_setName (me, ((ExperimentMFC) my data) -> name);
	Editor_broadcastChange (RunnerMFC_as_Editor (me));
	Graphics_updateWs (my graphics);
	return 1;
}

static void drawControlButton (RunnerMFC me, double left, double right, double bottom, double top, const wchar_t *visibleText) {
	Graphics_setColour (my graphics, Graphics_MAROON);
	Graphics_setLineWidth (my graphics, 3.0);
	Graphics_fillRectangle (my graphics, left, right, bottom, top);
	Graphics_setColour (my graphics, Graphics_YELLOW);
	Graphics_rectangle (my graphics, left, right, bottom, top);
	Graphics_text (my graphics, 0.5 * (left + right), 0.5 * (bottom + top), visibleText);
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (RunnerMFC);
	Melder_assert (event -> widget == my drawingArea);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	ExperimentMFC experiment = my data;
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
		const wchar_t *visibleText = experiment -> stimulus [experiment -> stimuli [experiment -> trial]]. visibleText;
		wchar_t *visibleText_dup = Melder_wcsdup (visibleText ? visibleText : L""), *visibleText_p = visibleText_dup;
		Graphics_setFont (my graphics, kGraphics_font_TIMES);
		Graphics_setFontSize (my graphics, 10);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
		Graphics_text3 (my graphics, 0, 1, Melder_integer (experiment -> trial), L" / ", Melder_integer (experiment -> numberOfTrials));
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_setFontSize (my graphics, 24);
		/*
		 * The run text.
		 */
		if (visibleText_p [0] != '\0') {
			wchar_t *visibleText_q = wcschr (visibleText_p, '|');
			if (visibleText_q) *visibleText_q = '\0';
			Graphics_text (my graphics, 0.5, 1.0, visibleText_p [0] != '\0' ? visibleText_p : experiment -> runText);
			if (visibleText_q) visibleText_p = visibleText_q + 1; else visibleText_p += wcslen (visibleText_p);
		} else {
			Graphics_text (my graphics, 0.5, 1.0, experiment -> runText);
		}
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		for (iresponse = 1; iresponse <= experiment -> numberOfDifferentResponses; iresponse ++) {
			ResponseMFC response = & experiment -> response [iresponse];
			Graphics_setColour (my graphics,
				response -> name [0] == '\0' ? Graphics_SILVER :
				experiment -> responses [experiment -> trial] == iresponse ? Graphics_RED :
				experiment -> ok_right > experiment -> ok_left || experiment -> responses [experiment -> trial] == 0 ?
				Graphics_YELLOW : Graphics_SILVER);
			Graphics_setLineWidth (my graphics, 3.0);
			Graphics_fillRectangle (my graphics, response -> left, response -> right, response -> bottom, response -> top);
			Graphics_setColour (my graphics, Graphics_MAROON);
			Graphics_rectangle (my graphics, response -> left, response -> right, response -> bottom, response -> top);
			Graphics_setFontSize (my graphics, response -> fontSize ? response -> fontSize : 24);
			if (visibleText_p [0] != '\0') {
				wchar_t *visibleText_q = wcschr (visibleText_p, '|');
				if (visibleText_q) *visibleText_q = '\0';
				Graphics_text (my graphics, 0.5 * (response -> left + response -> right),
					0.5 * (response -> bottom + response -> top), visibleText_p);
				if (visibleText_q) visibleText_p = visibleText_q + 1; else visibleText_p += wcslen (visibleText_p);
			} else {
				Graphics_text (my graphics, 0.5 * (response -> left + response -> right),
					0.5 * (response -> bottom + response -> top), response -> label);
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
		Melder_free (visibleText_dup);
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
	Dimension marginWidth = 10, marginHeight = 10;
	#if motif
	/* TODO */
	XtVaGetValues (event -> widget, XmNmarginWidth, & marginWidth, XmNmarginHeight, & marginHeight, NULL);
	Graphics_setWsViewport (my graphics, marginWidth, event -> width - marginWidth, marginHeight, event -> height - marginHeight);
	long width = event -> width - marginWidth - marginWidth;
	long height = event -> height - marginHeight - marginHeight;
	Graphics_setWsWindow (my graphics, 0, width, 0, height);
	Graphics_setViewport (my graphics, 0, width, 0, height);
	Graphics_updateWs (my graphics);
	#endif
}

static void do_ok (RunnerMFC me) {
	ExperimentMFC experiment = my data;
	Melder_assert (experiment -> trial >= 1 && experiment -> trial <= experiment -> numberOfTrials);
	my numberOfReplays = 0;
	if (experiment -> trial == experiment -> numberOfTrials) {
		experiment -> trial ++;
		Editor_broadcastChange (RunnerMFC_as_Editor (me));
		Graphics_updateWs (my graphics);
	} else if (experiment -> breakAfterEvery != 0 && experiment -> trial % experiment -> breakAfterEvery == 0) {
		experiment -> pausing = TRUE;
		Editor_broadcastChange (RunnerMFC_as_Editor (me));
		Graphics_updateWs (my graphics);
	} else {
		experiment -> trial ++;
		Editor_broadcastChange (RunnerMFC_as_Editor (me));
		Graphics_updateWs (my graphics);
		if (experiment -> stimuliAreSounds) {
			ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
		}
	}
}

static void do_oops (RunnerMFC me) {
	ExperimentMFC experiment = my data;
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
	Editor_broadcastChange (RunnerMFC_as_Editor (me));
	Graphics_updateWs (my graphics);
	if (experiment -> stimuliAreSounds) {
		ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
	}
}

static void do_replay (RunnerMFC me) {
	ExperimentMFC experiment = my data;
	Melder_assert (experiment -> trial >= 1 && experiment -> trial <= experiment -> numberOfTrials);
	my numberOfReplays ++;
	Editor_broadcastChange (RunnerMFC_as_Editor (me));
	Graphics_updateWs (my graphics);
	if (experiment -> stimuliAreSounds) {
		ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
	}
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (RunnerMFC);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	ExperimentMFC experiment = my data;
	if (my data == NULL) return;
	double x, y;
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & x, & y);
	if (experiment -> trial == 0) {   /* The first click of the experiment. */
		experiment -> trial ++;
		Editor_broadcastChange (RunnerMFC_as_Editor (me));
		Graphics_updateWs (my graphics);
		if (experiment -> stimuliAreSounds) {
			ExperimentMFC_playStimulus (experiment, experiment -> stimuli [1]);
		}
	} else if (experiment -> pausing) {   /* A click to leave the break. */
		if (x > experiment -> oops_left && x < experiment -> oops_right &&
			y > experiment -> oops_bottom && y < experiment -> oops_top && experiment -> trial > 1)
		{
			do_oops (me);
		} else {
			experiment -> pausing = FALSE;
			experiment -> trial ++;
			Editor_broadcastChange (RunnerMFC_as_Editor (me));
			Graphics_updateWs (my graphics);
			if (experiment -> stimuliAreSounds) {
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
					if (experiment -> responsesAreSounds) {
						ExperimentMFC_playResponse (experiment, iresponse);
					}
					if (experiment -> ok_right <= experiment -> ok_left && experiment -> numberOfGoodnessCategories == 0) {
						do_ok (me);
					} else {
						Editor_broadcastChange (RunnerMFC_as_Editor (me));
						Graphics_updateWs (my graphics);
					}
				}
			}
			if (experiment -> responses [experiment -> trial] != 0 && experiment -> ok_right > experiment -> ok_left) {
				for (iresponse = 1; iresponse <= experiment -> numberOfGoodnessCategories; iresponse ++) {
					GoodnessMFC cat = & experiment -> goodness [iresponse];
					if (x > cat -> left && x < cat -> right && y > cat -> bottom && y < cat -> top) {
						experiment -> goodnesses [experiment -> trial] = iresponse;
						Editor_broadcastChange (RunnerMFC_as_Editor (me));
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
				Melder_flushError (NULL);
				forget (me);
				return;
			}
		}
	}
}

static void gui_drawingarea_cb_key (I, GuiDrawingAreaKeyEvent event) {
	iam (RunnerMFC);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	ExperimentMFC experiment = my data;
	if (my data == NULL) return;
	if (experiment -> trial == 0) {
	} else if (experiment -> pausing) {
	} else if (experiment -> trial <= experiment -> numberOfTrials) {
		ExperimentMFC experiment = my data;
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
			do_oops (me);
		} else if (experiment -> responses [experiment -> trial] == 0) {
			for (iresponse = 1; iresponse <= experiment -> numberOfDifferentResponses; iresponse ++) {
				ResponseMFC response = & experiment -> response [iresponse];
				if (response -> key != NULL && response -> key [0] == event -> key) {
					experiment -> responses [experiment -> trial] = iresponse;
					if (experiment -> responsesAreSounds) {
						ExperimentMFC_playResponse (experiment, iresponse);
					}
					if (experiment -> ok_right <= experiment -> ok_left && experiment -> numberOfGoodnessCategories == 0) {
						do_ok (me);
					} else {
						Editor_broadcastChange (RunnerMFC_as_Editor (me));
						Graphics_updateWs (my graphics);
					}
				}
			}
		}
	}
}

static void createChildren (RunnerMFC me) {
	my drawingArea = GuiDrawingArea_createShown (my dialog, 0, 0, Machine_getMenuBarHeight (), 0,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, me, 0);
}

class_methods (RunnerMFC, Editor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	us -> editable = false;
	us -> scriptable = false;
class_methods_end

RunnerMFC RunnerMFC_create (Widget parent, const wchar_t *title, Ordered experiments) {
	RunnerMFC me = new (RunnerMFC); cherror
	Editor_init (RunnerMFC_as_parent (me), parent, 0, 0, 2000, 2000, title, NULL); cherror
	my experiments = experiments;
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);

struct structGuiDrawingAreaResizeEvent event = { my drawingArea, 0 };
event. width = GuiObject_getWidth (my drawingArea);
event. height = GuiObject_getHeight (my drawingArea);
gui_drawingarea_cb_resize (me, & event);

	my iexperiment = 1;
	RunnerMFC_startExperiment (me); cherror
end:
	iferror forget (me);
	return me;
}

/* End of file RunnerMFC.c */
