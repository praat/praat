/* RunnerMFC.c
 *
 * Copyright (C) 2001-2003 Paul Boersma
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
 * pb 2003/03/09
 */

#include "RunnerMFC.h"
#include "EditorM.h"
#include "machine.h"

#define RunnerMFC_members Editor_members \
	Widget drawingArea; \
	Graphics graphics;
#define RunnerMFC_methods Editor_methods
class_create_opaque (RunnerMFC, Editor)

static void dataChanged (I) {
	iam (RunnerMFC);
	Graphics_updateWs (my graphics);
}

MOTIF_CALLBACK (cb_draw)
	iam (RunnerMFC);
	ExperimentMFC experiment = my data;
	long iresponse;
	#ifdef UNIX
		if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
	#endif
	Graphics_setGrey (my graphics, 0.8);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setGrey (my graphics, 0.0);
	if (experiment -> trial == 0) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setFontSize (my graphics, 24);
		Graphics_printf (my graphics, 0.5, 0.5, "%s", experiment -> startText);
	} else if (experiment -> pausing) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setFontSize (my graphics, 24);
		Graphics_printf (my graphics, 0.5, 0.5, "%s", experiment -> pauseText);
	} else if (experiment -> trial <= experiment -> numberOfTrials) {
		Graphics_setFont (my graphics, Graphics_TIMES);
		Graphics_setFontSize (my graphics, 10);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
		Graphics_printf (my graphics, 0, 1, "%ld / %ld", experiment -> trial, experiment -> numberOfTrials);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_setFontSize (my graphics, 24);
		Graphics_printf (my graphics, 0.5, 1, "%s", experiment -> runText);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		for (iresponse = 1; iresponse <= experiment -> numberOfResponseCategories; iresponse ++) {
			ResponseMFC cat = & experiment -> response [iresponse];
			Graphics_setColour (my graphics,
				cat -> category [0] == '\0' ? Graphics_SILVER :
				experiment -> responses [experiment -> trial] == iresponse ? Graphics_RED :
				experiment -> responses [experiment -> trial] == 0 ? Graphics_YELLOW : Graphics_SILVER);
			Graphics_setLineWidth (my graphics, 3.0);
			Graphics_fillRectangle (my graphics, cat -> left, cat -> right, cat -> bottom, cat -> top);
			Graphics_setColour (my graphics, Graphics_MAROON);
			Graphics_rectangle (my graphics, cat -> left, cat -> right, cat -> bottom, cat -> top);
			Graphics_text (my graphics, 0.5 * (cat -> left + cat -> right), 0.5 * (cat -> bottom + cat -> top), cat -> label);
		}
		for (iresponse = 1; iresponse <= experiment -> numberOfGoodnessCategories; iresponse ++) {
			GoodnessMFC cat = & experiment -> goodness [iresponse];
			Graphics_setColour (my graphics, experiment -> responses [experiment -> trial] == 0 ? Graphics_SILVER : Graphics_YELLOW);
			Graphics_setLineWidth (my graphics, 3.0);
			Graphics_fillRectangle (my graphics, cat -> left, cat -> right, cat -> bottom, cat -> top);
			Graphics_setColour (my graphics, Graphics_MAROON);
			Graphics_rectangle (my graphics, cat -> left, cat -> right, cat -> bottom, cat -> top);
			Graphics_text (my graphics, 0.5 * (cat -> left + cat -> right), 0.5 * (cat -> bottom + cat -> top), cat -> label);
		}
	} else {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setFontSize (my graphics, 24);
		Graphics_printf (my graphics, 0.5, 0.5, "%s", experiment -> endText);
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_resize)
	iam (RunnerMFC);
	Dimension width, height, marginWidth = 10, marginHeight = 10;
	XtVaGetValues (w, XmNwidth, & width, XmNheight, & height,
		XmNmarginWidth, & marginWidth, XmNmarginHeight, & marginHeight, NULL);
	Graphics_setWsViewport (my graphics, marginWidth, width - marginWidth, marginHeight, height - marginHeight);
	width = width - marginWidth - marginWidth;
	height = height - marginHeight - marginHeight;
	Graphics_setWsWindow (my graphics, 0, width, 0, height);
	Graphics_setViewport (my graphics, 0, width, 0, height);
	Graphics_updateWs (my graphics);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_input)
	iam (RunnerMFC);
	ExperimentMFC experiment = my data;
	MotifEvent event = MotifEvent_fromCallData (call);
	if (MotifEvent_isButtonPressedEvent (event)) {
		if (experiment -> trial == 0) {
			experiment -> trial ++;
			Editor_broadcastChange (me);
			Graphics_updateWs (my graphics);
			ExperimentMFC_playStimulus (experiment, experiment -> stimuli [1]);
		} else if (experiment -> pausing) {
			experiment -> pausing = FALSE;
			experiment -> trial ++;
			Editor_broadcastChange (me);
			Graphics_updateWs (my graphics);
			ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
		} else if (experiment -> trial <= experiment -> numberOfTrials) {
			double x, y;
			ExperimentMFC experiment = my data;
			long iresponse;
			Graphics_DCtoWC (my graphics, MotifEvent_x (event), MotifEvent_y (event), & x, & y);
			if (experiment -> responses [experiment -> trial] == 0)
			for (iresponse = 1; iresponse <= experiment -> numberOfResponseCategories; iresponse ++) {
				ResponseMFC cat = & experiment -> response [iresponse];
				if (x > cat -> left && x < cat -> right && y > cat -> bottom && y < cat -> top && cat -> category [0] != '\0') {
					experiment -> responses [experiment -> trial] = iresponse;
					if (experiment -> numberOfGoodnessCategories == 0) {
						if (experiment -> trial == experiment -> numberOfTrials) {
							experiment -> trial ++;
							Editor_broadcastChange (me);
							Graphics_updateWs (my graphics);
						} else if (experiment -> breakAfterEvery != 0 && experiment -> trial % experiment -> breakAfterEvery == 0) {
							experiment -> pausing = TRUE;
							Editor_broadcastChange (me);
							Graphics_updateWs (my graphics);
						} else {
							experiment -> trial ++;
							Editor_broadcastChange (me);
							Graphics_updateWs (my graphics);
							ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
						}
					} else {
						Editor_broadcastChange (me);
						Graphics_updateWs (my graphics);
					}
				}
			}
			if (experiment -> responses [experiment -> trial] != 0)
			for (iresponse = 1; iresponse <= experiment -> numberOfGoodnessCategories; iresponse ++) {
				GoodnessMFC cat = & experiment -> goodness [iresponse];
				if (x > cat -> left && x < cat -> right && y > cat -> bottom && y < cat -> top) {
					experiment -> goodnesses [experiment -> trial] = iresponse;
					if (experiment -> trial == experiment -> numberOfTrials) {
						experiment -> trial ++;
						Editor_broadcastChange (me);
						Graphics_updateWs (my graphics);
					} else if (experiment -> breakAfterEvery != 0 && experiment -> trial % experiment -> breakAfterEvery == 0) {
						experiment -> pausing = TRUE;
						Editor_broadcastChange (me);
						Graphics_updateWs (my graphics);
					} else {
						experiment -> trial ++;
						Editor_broadcastChange (me);
						Graphics_updateWs (my graphics);
						ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
					}
				}
			}
		}
	} else if (MotifEvent_isKeyPressedEvent (event)) {
		#if defined (macintosh)
			enum { charCodeMask = 0x000000FF };
			unsigned char key = event -> message & charCodeMask;
		#elif defined (_WIN32)
			char key = event -> key;
		#else
			char key;
			XLookupString (& event -> xkey, & key, 1, NULL, NULL);
		#endif
		if (experiment -> trial == 0) {
		} else if (experiment -> pausing) {
		} else if (experiment -> trial <= experiment -> numberOfTrials) {
			double x, y;
			ExperimentMFC experiment = my data;
			long iresponse;
			Graphics_DCtoWC (my graphics, MotifEvent_x (event), MotifEvent_y (event), & x, & y);
			if (experiment -> responses [experiment -> trial] == 0)
			for (iresponse = 1; iresponse <= experiment -> numberOfResponseCategories; iresponse ++) {
				ResponseMFC cat = & experiment -> response [iresponse];
				if (cat -> key != NULL && cat -> key [0] == key) {
					experiment -> responses [experiment -> trial] = iresponse;
					if (experiment -> numberOfGoodnessCategories == 0) {
						if (experiment -> trial == experiment -> numberOfTrials) {
							experiment -> trial ++;
							Editor_broadcastChange (me);
							Graphics_updateWs (my graphics);
						} else if (experiment -> breakAfterEvery != 0 && experiment -> trial % experiment -> breakAfterEvery == 0) {
							experiment -> pausing = TRUE;
							Editor_broadcastChange (me);
							Graphics_updateWs (my graphics);
						} else {
							experiment -> trial ++;
							Editor_broadcastChange (me);
							Graphics_updateWs (my graphics);
							ExperimentMFC_playStimulus (experiment, experiment -> stimuli [experiment -> trial]);
						}
					} else {
						Editor_broadcastChange (me);
						Graphics_updateWs (my graphics);
					}
				}
			}
		}
	}
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (RunnerMFC);
	my drawingArea = XmCreateDrawingArea (my dialog, "drawingArea", NULL, 0);
	XtVaSetValues (my drawingArea,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
		XmNbottomAttachment, XmATTACH_FORM,
		0);
	XtManageChild (my drawingArea);
}

class_methods (RunnerMFC, Editor)
	class_method (dataChanged)
	class_method (createChildren)
	us -> editable = FALSE;
	us -> scriptable = FALSE;
class_methods_end

RunnerMFC RunnerMFC_create (Widget parent, const char *title, Any data) {
	RunnerMFC me = new (RunnerMFC);
	if (! me || ! Editor_init (me, parent, 0, 0, 2000, 2000, title, data)) { forget (me); return 0; }
	if (! ExperimentMFC_start (data)) { forget (me); return Melder_errorp ("Cannot start experiment."); }
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNinputCallback, cb_input, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNresizeCallback, cb_resize, (XtPointer) me);
cb_resize (my drawingArea, (XtPointer) me, 0);
	return me;
}

/* End of file RunnerMFC.c */
