/* TextGridEditor.c
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
 * pb 2002/10/06 improved visibility of dragging
 * pb 2004/04/13 less flashing
 * pb 2005/01/11 better visibility of yellow line
 * pb 2005/03/02 green colouring for matching labels
 * pb 2005/05/05 show number of intervals
 * pb 2005/06/17 enums
 * pb 2005/09/23 interface update
 */

#include "TextGridEditor.h"
#include "FunctionEditor_Sound.h"
#include "FunctionEditor_SoundAnalysis.h"
#include "SpellingChecker.h"
#include "Resources.h"
#include "EditorM.h"
#include "SoundEditor.h"
#include "Sound_and_Spectrogram.h"

#define TextGridEditor_members FunctionEditor_members \
	SpellingChecker spellingChecker; \
	long selectedTier; \
	int useTextStyles, fontSize, alignment, shiftDragMultiple, suppressRedraw; \
	Widget publishButton, publishPreserveButton; \
	Widget writeWavButton, writeAiffButton, writeAifcButton, writeNextSunButton, writeNistButton; \
	char *findString, greenString [Resources_STRING_BUFFER_SIZE]; \
	int showNumberOf, greenMethod;
#define TextGridEditor_methods FunctionEditor_methods
class_create_opaque (TextGridEditor, FunctionEditor)

static struct {
	int useTextStyles, fontSize, alignment, shiftDragMultiple, showNumberOf, greenMethod;
	char greenString [Resources_STRING_BUFFER_SIZE];
}
	preferences = { FALSE, 12, Graphics_CENTRE, TRUE, 1, Melder_STRING_EQUAL_TO, "any matching string for green colouring" };

void TextGridEditor_prefs (void) {
	Resources_addInt ("TextGridEditor.useTextStyles", & preferences.useTextStyles);
	Resources_addInt ("TextGridEditor.fontSize", & preferences.fontSize);
	Resources_addInt ("TextGridEditor.alignment", & preferences.alignment);
	Resources_addInt ("TextGridEditor.shiftDragMultiple2", & preferences.shiftDragMultiple);
	Resources_addInt ("TextGridEditor.showNumberOf", & preferences.showNumberOf);
	Resources_addInt ("TextGridEditor.greenMethod", & preferences.greenMethod);
	Resources_addString ("TextGridEditor.greenString", & preferences.greenString [0]);
}

static double computeSoundY (TextGridEditor me) {
	TextGrid grid = my data;
	int ntier = grid -> tiers -> size;
	int showAnalysis = (my spectrogram.show || my pitch.show || my intensity.show || my formant.show) && (my longSound.data || my sound.data);
	return my sound.data || my longSound.data ? ntier / (2.0 + ntier * (showAnalysis ? 1.8 : 1.3)) : 1.0;
}

static void identifyTierClass (Data anyTier, IntervalTier *intervalTier, TextTier *textTier) {
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		*intervalTier = (IntervalTier) anyTier;
		*textTier = NULL;
	} else {
		*intervalTier = NULL;
		*textTier = (TextTier) anyTier;
	}
}

static int _TextGridEditor_yWCtoTier (TextGridEditor me, double yWC) {
	TextGrid grid = my data;
	int ntier = grid -> tiers -> size;
	double soundY = computeSoundY (me);
	int itier = ntier - (int) floor (yWC / soundY * (double) ntier);
	if (itier < 1) itier = 1; if (itier > ntier) itier = ntier;
	return itier;
}

static void _TextGridEditor_timeToInterval (TextGridEditor me, double t, int itier, double *tmin, double *tmax) {
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	identifyTierClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
	if (intervalTier) {
		long iinterval = IntervalTier_timeToIndex (intervalTier, t);
		TextInterval interval;
		if (iinterval == 0) {
			if (t < my tmin) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals -> size;
			}
		}
		interval = intervalTier -> intervals -> item [iinterval];
		*tmin = interval -> xmin;
		*tmax = interval -> xmax;
	} else {
		long n = textTier -> points -> size;
		if (n == 0) {
			*tmin = my tmin;
			*tmax = my tmax;
		} else {
			long ipointleft = AnyTier_timeToLowIndex (textTier, t);
			*tmin = ipointleft == 0 ? my tmin : ((TextPoint) textTier -> points -> item [ipointleft]) -> time;
			*tmax = ipointleft == n ? my tmax : ((TextPoint) textTier -> points -> item [ipointleft + 1]) -> time;
		}
	}
	if (*tmin < my tmin) *tmin = my tmin;   /* Clip by FunctionEditor's time domain. */
	if (*tmax > my tmax) *tmax = my tmax;
}

static int checkTierSelection (TextGridEditor me, const char *verbPhrase) {
	TextGrid grid = my data;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers -> size)
		return Melder_error ("To %s, first select a tier by clicking anywhere inside it.", verbPhrase);
	return 1;
}

static long getSelectedInterval (TextGridEditor me) {
	TextGrid grid = my data;
	IntervalTier tier;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	tier = grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> methods == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection);
}

DIRECT (TextGridEditor, cb_getStartingPointOfInterval)
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, "query the starting point of an interval")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals -> size ? NUMundefined :
			((TextInterval) tier -> intervals -> item [iinterval]) -> xmin;
		Melder_informationReal (time, "seconds");
	} else {
		return Melder_error ("The selected tier is not an interval tier.");
	}
END

DIRECT (TextGridEditor, cb_getEndPointOfInterval)
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, "query the end point of an interval")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals -> size ? NUMundefined :
			((TextInterval) tier -> intervals -> item [iinterval]) -> xmax;
		Melder_informationReal (time, "seconds");
	} else {
		return Melder_error ("The selected tier is not an interval tier.");
	}
END

DIRECT (TextGridEditor, cb_getLabelOfInterval)
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, "query the label of an interval")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		char *label = iinterval < 1 || iinterval > tier -> intervals -> size ? "" :
			((TextInterval) tier -> intervals -> item [iinterval]) -> text;
		Melder_information ("%s", label);
	} else {
		return Melder_error ("The selected tier is not an interval tier.");
	}
END

static long getSelectedLeftBoundary (TextGridEditor me) {
	TextGrid grid = my data;
	IntervalTier tier;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	tier = grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> methods == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my startSelection);
}

static long getSelectedPoint (TextGridEditor me) {
	TextGrid grid = my data;
	TextTier tier;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	tier = grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> methods == classTextTier);
	return AnyTier_hasPoint (tier, my startSelection);
}

/*
 * The main invariant of the TextGridEditor is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */

static void updateText (I) {
	iam (TextGridEditor);
	TextGrid grid = my data;
	char *newText = "";
	if (my selectedTier) {
		IntervalTier intervalTier;
		TextTier textTier;
		identifyTierClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (iinterval) {
				TextInterval interval = intervalTier -> intervals -> item [iinterval];
				if (interval -> text) {
					newText = interval -> text;
				}
			}
		} else {
			long ipoint = AnyTier_hasPoint (textTier, my startSelection);
			if (ipoint) {
				TextPoint point = textTier -> points -> item [ipoint];
				if (point -> mark) {
					newText = point -> mark;
				}
			}
		}
	}
	my suppressRedraw = TRUE;   /* Prevent valueChangedCallback from redrawing. */
	XmTextSetString (my text, newText);
	XmTextSetInsertionPosition (my text, strlen (newText));
	my suppressRedraw = FALSE;
}

/********** FUNCTIONEDITOR METHODS **********/

static void destroy (I) {
	iam (TextGridEditor);
	forget (my sound.data);
	FunctionEditor_SoundAnalysis_forget (me);
	inherited (TextGridEditor) destroy (me);
}

static void dataChanged (I) {
	iam (TextGridEditor);
	TextGrid grid = my data;
	/*
	 * Perform a minimal selection change.
	 * Most changes will involve intervals and boundaries; however, there may also be tier removals.
	 * Do a simple guess.
	 */
	if (grid -> tiers -> size < my selectedTier) {
		my selectedTier = grid -> tiers -> size;
	}
	inherited (TextGridEditor) dataChanged (me);   /* Does all the updating. */
}

/********** FILE MENU **********/

static void prefs_addFields (Any editor, EditorCommand cmd) {
	Any radio;
	(void) editor;
	NATURAL ("Font size (points)", "18")
	OPTIONMENU ("Text alignment in intervals", 2)
		OPTION ("Left")
		OPTION ("Centre")
		OPTION ("Right")
	OPTIONMENU ("The symbols %#_^ in labels", 1)
		OPTION ("are shown as typed")
		OPTION ("mean italic/bold/sub/super")
	OPTIONMENU ("With the shift key, you drag", 2)
		OPTION ("a single boundary")
		OPTION ("multiple boundaries")
	OPTIONMENU ("Show number of", 1)
		OPTION ("nothing")
		OPTION ("intervals or points")
		OPTION ("non-empty intervals or points")
	OPTIONMENU ("Paint intervals green whose label...", 1)
	OPTIONS_ENUM (Melder_STRING_text_finiteVerb (itext), Melder_STRING_min, Melder_STRING_max)
	SENTENCE ("...the text", "some text here for green paint")
}
static void prefs_setValues (I, EditorCommand cmd) {
	iam (TextGridEditor);
	SET_INTEGER ("The symbols %#_^ in labels", my useTextStyles + 1)
	SET_INTEGER ("Font size", my fontSize)
	SET_INTEGER ("Text alignment in intervals", my alignment + 1)
	SET_INTEGER ("With the shift key, you drag", my shiftDragMultiple + 1)
	SET_INTEGER ("Show number of", my showNumberOf)
	SET_INTEGER ("Paint intervals green whose label...", my greenMethod + 1 - Melder_STRING_min)
	SET_STRING ("...the text", my greenString)
}
static void prefs_getValues (I, EditorCommand cmd) {
 	iam (TextGridEditor);
	preferences.useTextStyles = my useTextStyles = GET_INTEGER ("The symbols %#_^ in labels") - 1;
	preferences.fontSize = my fontSize = GET_INTEGER ("Font size");
	preferences.alignment = my alignment = GET_INTEGER ("Text alignment in intervals") - 1;
	preferences.shiftDragMultiple = my shiftDragMultiple = GET_INTEGER ("With the shift key, you drag") - 1;
	preferences.showNumberOf = my showNumberOf = GET_INTEGER ("Show number of");
	preferences.greenMethod = my greenMethod = GET_INTEGER ("Paint intervals green whose label...") - 1 + Melder_STRING_min;
	strncpy (my greenString, GET_STRING ("...the text"), Resources_STRING_BUFFER_SIZE);
	my greenString [Resources_STRING_BUFFER_SIZE - 1] = '\0';
	strcpy (preferences.greenString, my greenString);
	FunctionEditor_redraw (me);
}

FORM_WRITE (TextGridEditor, cb_writeToTextFile, "Write to TextGrid text file", 0)
	sprintf (defaultName, "%s.TextGrid", ((Thing) my data) -> name);
DO_WRITE
	if (! Data_writeToTextFile (my data, file)) return 0;
END

static int do_publish (TextGridEditor me, int preserveTimes) {
	Sound publish = NULL;
	if (my endSelection <= my startSelection) return Melder_error ("No selection.");
	if (my longSound.data) {
		publish = LongSound_extractPart (my longSound.data, my startSelection, my endSelection, preserveTimes);
		iferror return 0;
	} else if (my sound.data) {
		publish = Sound_extractPart (my sound.data, my startSelection, my endSelection,
			enumi (Sound_WINDOW, Rectangular), 1.0, preserveTimes);
		iferror return 0;
	}
	Melder_assert (publish != NULL);
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

DIRECT (TextGridEditor, cb_publish)
	if (! do_publish (me, FALSE)) return 0;
END

DIRECT (TextGridEditor, cb_publishPreserve)
	if (! do_publish (me, TRUE)) return 0;
END

static int cb_writeAny (TextGridEditor me, MelderFile file, int audioFileType) {
	if (my startSelection >= my endSelection)
		return Melder_error ("No samples selected.");
	if (my longSound.data) {
		if (! LongSound_writePartToAudioFile16 (my longSound.data, audioFileType, my startSelection, my endSelection, file)) return 0;
	} else if (my sound.data) {
		/* if (! Sound_writePartToAudioFile16 (my sound.data, my startSelection, my endSelection, file)) return 0; */
	}
	return 1;
}

FORM_WRITE (TextGridEditor, cb_writeAiff, "Write selection to AIFF file", 0)
	if (my longSound.data)
		sprintf (defaultName, "%s.aiff", my longSound.data -> name);
DO_WRITE
	if (! cb_writeAny (me, file, Melder_AIFF)) return 0;
END

FORM_WRITE (TextGridEditor, cb_writeAifc, "Write selection to AIFC file", 0)
	if (my longSound.data)
		sprintf (defaultName, "%s.aifc", my longSound.data -> name);
DO_WRITE
	if (! cb_writeAny (me, file, Melder_AIFC)) return 0;
END

FORM_WRITE (TextGridEditor, cb_writeWav, "Write selection to WAV file", 0)
	if (my longSound.data)
		sprintf (defaultName, "%s.wav", my longSound.data -> name);
DO_WRITE
	if (! cb_writeAny (me, file, Melder_WAV)) return 0;
END

FORM_WRITE (TextGridEditor, cb_writeNextSun, "Write selection to NeXT/Sun file", 0)
	if (my longSound.data)
		sprintf (defaultName, "%s.au", my longSound.data -> name);
DO_WRITE
	if (! cb_writeAny (me, file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (TextGridEditor, cb_writeNist, "Write selection to NIST file", 0)
	if (my longSound.data)
		sprintf (defaultName, "%s.nist", my longSound.data -> name);
DO_WRITE
	if (! cb_writeAny (me, file, Melder_NIST)) return 0;
END

/********** EDIT MENU **********/

DIRECT (TextGridEditor, cb_cut)
	XmTextCut (my text, 0);
END

DIRECT (TextGridEditor, cb_copy)
	XmTextCopy (my text, 0);
END

DIRECT (TextGridEditor, cb_paste)
	XmTextPaste (my text);
END

DIRECT (TextGridEditor, cb_erase)
	XmTextRemove (my text);
END

DIRECT (TextGridEditor, cb_genericize)
	Editor_save (me, "Genericize");
	TextGrid_genericize (my data);
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (TextGridEditor, cb_nativize)
	Editor_save (me, "Nativize");
	TextGrid_nativize (my data);
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

/********** VIEW MENU **********/

static void cb_selectAdjacentTier (TextGridEditor me, int previous) {
	TextGrid grid = my data;
	long n = grid -> tiers -> size;
	if (n >= 2) {
		my selectedTier = previous ?
			my selectedTier > 1 ? my selectedTier - 1 : n :
			my selectedTier < n ? my selectedTier + 1 : 1;
		_TextGridEditor_timeToInterval (me, my startSelection, my selectedTier, & my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me);
	}
}

DIRECT (TextGridEditor, cb_selectPreviousTier)
	cb_selectAdjacentTier (me, TRUE);
END

DIRECT (TextGridEditor, cb_selectNextTier)
	cb_selectAdjacentTier (me, FALSE);
END

static void scrollToView (TextGridEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow));
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow));
	} else {
		FunctionEditor_marksChanged (me);
	}
}

static void cb_selectAdjacentInterval (TextGridEditor me, int previous, int shift) {
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers -> size) return;
	identifyTierClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		long n = intervalTier -> intervals -> size;
		if (n >= 2) {
			TextInterval interval;
			long iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (shift) {
				long binterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
				long einterval = IntervalTier_timeToIndex (intervalTier, my endSelection);
				if (my endSelection == intervalTier -> xmax) einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					interval = intervalTier -> intervals -> item [iinterval];
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							interval = intervalTier -> intervals -> item [einterval - 1];
							my endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						interval = intervalTier -> intervals -> item [binterval - 1];
						my startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							interval = intervalTier -> intervals -> item [binterval];
							my startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						interval = intervalTier -> intervals -> item [einterval];
						my endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = previous ?
					iinterval > 1 ? iinterval - 1 : n :
					iinterval < n ? iinterval + 1 : 1;
				interval = intervalTier -> intervals -> item [iinterval];
				my startSelection = interval -> xmin;
				my endSelection = interval -> xmax;
			}
			scrollToView (me, iinterval == n ? my startSelection : iinterval == 1 ? my endSelection : (my startSelection + my endSelection) / 2);
		}
	} else {
		long n = textTier -> points -> size;
		if (n >= 2) {
			TextPoint point;
			long ipoint = AnyTier_timeToHighIndex (textTier, my startSelection);
			ipoint = previous ?
				ipoint > 1 ? ipoint - 1 : n :
				ipoint < n ? ipoint + 1 : 1;
			point = textTier -> points -> item [ipoint];
			my startSelection = my endSelection = point -> time;
			scrollToView (me, my startSelection);
		}
	}
}

DIRECT (TextGridEditor, cb_selectPreviousInterval)
	cb_selectAdjacentInterval (me, TRUE, FALSE);
END

DIRECT (TextGridEditor, cb_selectNextInterval)
	cb_selectAdjacentInterval (me, FALSE, FALSE);
END

DIRECT (TextGridEditor, cb_extendSelectPreviousInterval)
	cb_selectAdjacentInterval (me, TRUE, TRUE);
END

DIRECT (TextGridEditor, cb_extendSelectNextInterval)
	cb_selectAdjacentInterval (me, FALSE, TRUE);
END

/********** B MENU **********/

DIRECT (TextGridEditor, cb_moveBtoZero)
	double zero = Sound_getNearestZeroCrossing (my sound.data, my startSelection);
	if (NUMdefined (zero)) {
		my startSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
END

/********** C MENU **********/

DIRECT (TextGridEditor, cb_moveCursorToZero)
	double zero = Sound_getNearestZeroCrossing (my sound.data, 0.5 * (my startSelection + my endSelection));
	if (NUMdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me);
	}
END

/********** E MENU **********/

DIRECT (TextGridEditor, cb_moveEtoZero)
	double zero = Sound_getNearestZeroCrossing (my sound.data, my endSelection);
	if (NUMdefined (zero)) {
		my endSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
END

/********** INTERVAL MENU **********/

static int insertBoundaryOrPoint (TextGridEditor me, int itier, double t, int insertSecond) {
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	int ntiers = grid -> tiers -> size;
	if (itier < 1 || itier > ntiers) return 0;
	identifyTierClass (grid -> tiers -> item [itier], & intervalTier, & textTier);

	if (intervalTier) {
		TextInterval interval, newInterval;
		long iinterval, position;
		char *text;
		if (IntervalTier_hasTime (intervalTier, t)) {
			Melder_flushError ("Cannot add a boundary at %f seconds, because there is already a boundary there.", t);
			return 0;
		}
		iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval) {
			interval = intervalTier -> intervals -> item [iinterval];
		} else {
			return 0;   /* Clicked outside time domain of intervals. */
		}

		Editor_save (me, "Add boundary");

		if (itier == my selectedTier) {
			/*
			 * Divide up the label text into left and right, depending on where the text cursor is.
			 */
			text = XmTextGetString (my text);
			position = XmTextGetInsertionPosition (my text);
			newInterval = TextInterval_create (t, interval -> xmax, text + position);
			text [position] = '\0';
			TextInterval_setText (interval, text);
			XtFree (text);
		} else {
			/*
			 * Move the text to the left of the boundary.
			 */
			newInterval = TextInterval_create (t, interval -> xmax, "");
		}
		interval -> xmax = t;
		Collection_addItem (intervalTier -> intervals, newInterval);
		if (insertSecond && ntiers >= 2) {
			/*
			 * Find the last time before t on another tier.
			 */
			double tlast = interval -> xmin, tmin, tmax;
			int jtier;
			for (jtier = 1; jtier <= ntiers; jtier ++) if (jtier != itier) {
				_TextGridEditor_timeToInterval (me, t, jtier, & tmin, & tmax);
				if (tmin > tlast) {
					tlast = tmin;
				}
			}
			if (tlast > interval -> xmin && tlast < t) {
				newInterval = TextInterval_create (tlast, t, "");
				interval -> xmax = tlast;
				Collection_addItem (intervalTier -> intervals, newInterval);
			}
		}
	} else {
		TextPoint newPoint;
		if (AnyTier_hasPoint (textTier, t)) {
			Melder_flushError ("Cannot add a point at %f seconds, because there is already a point there.", t);
			return 0;
		} 

		Editor_save (me, "Add point");

		newPoint = TextPoint_create (t, "");
		Collection_addItem (textTier -> points, newPoint);
	}
	my startSelection = my endSelection = t;
	return 1;
}

static void insertIntervalOnTier (TextGridEditor me, int itier) {
	if (! insertBoundaryOrPoint (me, itier, my playingCursor || my playingSelection ? my playCursor : my startSelection, TRUE)) return;
	my selectedTier = itier;
	FunctionEditor_marksChanged (me);
	Editor_broadcastChange (me);
}

DIRECT (TextGridEditor, cb_insertIntervalOnTier1) insertIntervalOnTier (me, 1); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier2) insertIntervalOnTier (me, 2); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier3) insertIntervalOnTier (me, 3); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier4) insertIntervalOnTier (me, 4); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier5) insertIntervalOnTier (me, 5); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier6) insertIntervalOnTier (me, 6); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier7) insertIntervalOnTier (me, 7); END
DIRECT (TextGridEditor, cb_insertIntervalOnTier8) insertIntervalOnTier (me, 8); END

/********** BOUNDARY/POINT MENU **********/

DIRECT (TextGridEditor, cb_removePointOrBoundary)
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, "remove a point or boundary")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		TextInterval left, right;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary) return Melder_error ("To remove a boundary, first click on it.");

		Editor_save (me, "Remove boundary");

		left = tier -> intervals -> item [selectedLeftBoundary - 1];
		right = tier -> intervals -> item [selectedLeftBoundary];
		left -> xmax = right -> xmax;   /* Collapse left and right intervals into left interval. */
		if (right -> text == NULL) {
			;
		} else if (left -> text == NULL) {
			TextInterval_setText (left, right -> text);
		} else {
			char *buffer = Melder_malloc (strlen (left -> text) + strlen (right -> text) + 1);
			if (! buffer) return 0;
			sprintf (buffer, "%s%s", left -> text, right -> text);
			if (! TextInterval_setText (left, buffer)) { Melder_free (buffer); return 0; }
			Melder_free (buffer);
		}
		Collection_removeItem (tier -> intervals, selectedLeftBoundary);   /* Remove right interval. */
	} else {
		TextTier tier = (TextTier) anyTier;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint) return Melder_error ("To remove a point, first click on it.");

		Editor_save (me, "Remove point");
		Collection_removeItem (tier -> points, selectedPoint);
	}
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

static int cb_movePointOrBoundary (TextGridEditor me, int where) {
	double position;
	TextGrid grid = my data;
	Data anyTier;
	if (where == 0 && my sound.data == NULL) return 1;
	if (! checkTierSelection (me, "move a point or boundary")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		static char *boundarySaveText [3] = { "Move boundary to zero crossing", "Move boundary to B", "Move boundary to E" };
		TextInterval left, right;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary) return Melder_error ("To move a boundary, first click on it.");
		left = tier -> intervals -> item [selectedLeftBoundary - 1];
		right = tier -> intervals -> item [selectedLeftBoundary];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my sound.data, left -> xmax);
		if (position == NUMundefined) return 1;
		if (position <= left -> xmin || position >= right -> xmax)
			{ Melder_beep (); return 0; }

		Editor_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = my startSelection = my endSelection = position;
	} else {
		TextTier tier = (TextTier) anyTier;
		static char *pointSaveText [3] = { "Move point to zero crossing", "Move point to B", "Move point to E" };
		TextPoint point;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint) return Melder_error ("To move a point, first click on it.");
		point = tier -> points -> item [selectedPoint];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my sound.data, point -> time);
		if (position == NUMundefined) return 1;

		Editor_save (me, pointSaveText [where]);

		point -> time = my startSelection = my endSelection = position;
	}
	FunctionEditor_marksChanged (me);   /* Because cursor has moved. */
	Editor_broadcastChange (me);
	return 1;
}

DIRECT (TextGridEditor, cb_moveToB)
	if (! cb_movePointOrBoundary (me, 1)) return 0;
END

DIRECT (TextGridEditor, cb_moveToE)
	if (! cb_movePointOrBoundary (me, 2)) return 0;
END

DIRECT (TextGridEditor, cb_moveToZero)
	if (! cb_movePointOrBoundary (me, 0)) return 0;
END

static void insertOnTier (TextGridEditor me, int itier) {
	if (! insertBoundaryOrPoint (me, itier, my playingCursor || my playingSelection ? my playCursor : my startSelection, FALSE)) return;
	my selectedTier = itier;
	FunctionEditor_marksChanged (me);
	Editor_broadcastChange (me);
}

DIRECT (TextGridEditor, cb_insertOnSelectedTier)
	insertOnTier (me, my selectedTier);
END

DIRECT (TextGridEditor, cb_insertOnTier1) insertOnTier (me, 1); END
DIRECT (TextGridEditor, cb_insertOnTier2) insertOnTier (me, 2); END
DIRECT (TextGridEditor, cb_insertOnTier3) insertOnTier (me, 3); END
DIRECT (TextGridEditor, cb_insertOnTier4) insertOnTier (me, 4); END
DIRECT (TextGridEditor, cb_insertOnTier5) insertOnTier (me, 5); END
DIRECT (TextGridEditor, cb_insertOnTier6) insertOnTier (me, 6); END
DIRECT (TextGridEditor, cb_insertOnTier7) insertOnTier (me, 7); END
DIRECT (TextGridEditor, cb_insertOnTier8) insertOnTier (me, 8); END

DIRECT (TextGridEditor, cb_insertOnAllTiers)
	TextGrid grid = my data;
	int saveTier = my selectedTier, itier;
	for (itier = 1; itier <= grid -> tiers -> size; itier ++) {
		insertOnTier (me, itier);
	}
	my selectedTier = saveTier;
END

/********** SEARCH MENU **********/

static int findInTier (TextGridEditor me) {
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, "find a text")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = tier -> intervals -> item [iinterval];
			char *text = interval -> text;
			if (text) {
				char *position = strstr (text, my findString);
				if (position) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					XmTextSetSelection (my text, position - text, position - text + strlen (my findString), 0);
					return 1;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals -> size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier, my startSelection) + 1;
		while (ipoint <= tier -> points -> size) {
			TextPoint point = tier -> points -> item [ipoint];
			char *text = point -> mark;
			if (text) {
				char *position = strstr (text, my findString);
				if (position) {
					my startSelection = my endSelection = point -> time;
					scrollToView (me, point -> time);
					XmTextSetSelection (my text, position - text, position - text + strlen (my findString), 0);
					return 1;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points -> size)
			Melder_beep ();
	}
	return 1;
}

static void doFind (TextGridEditor me) {
	if (my findString) {
		XmTextPosition left, right;
		char *label = XmTextGetString (my text);
		char *position = strstr (XmTextGetSelectionPosition (my text, & left, & right) ? label + right : label, my findString);   /* CRLF BUG? */
		if (position) {
			XmTextSetSelection (my text, position - label, position - label + strlen (my findString), 0);
		} else {
			if (! findInTier (me)) Melder_flushError (NULL);
		}
		XtFree (label);
	}
}

FORM (TextGridEditor, cb_find, "Find text", 0)
	LABEL ("", "Text:")
	TEXTFIELD ("string", "")
	OK
DO
	Melder_free (my findString);
	my findString = Melder_strdup (GET_STRING ("string"));
	doFind (me);
END

DIRECT (TextGridEditor, cb_findAgain)
	doFind (me);
END

static int checkSpellingInTier (TextGridEditor me) {
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, "check spelling")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = tier -> intervals -> item [iinterval];
			char *text = interval -> text;
			if (text) {
				int position = 0;
				char *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					XmTextSetSelection (my text, position, position + strlen (notAllowed), 0);
					return 1;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals -> size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier, my startSelection) + 1;
		while (ipoint <= tier -> points -> size) {
			TextPoint point = tier -> points -> item [ipoint];
			char *text = point -> mark;
			if (text) {
				int position = 0;
				char *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = my endSelection = point -> time;
					scrollToView (me, point -> time);
					XmTextSetSelection (my text, position, position + strlen (notAllowed), 0);
					return 1;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points -> size)
			Melder_beep ();
	}
	return 1;
}

DIRECT (TextGridEditor, cb_checkSpelling)
	if (my spellingChecker) {
		XmTextPosition left, right;
		int position = 0;
		char *label, *notAllowed;
		if (XmTextGetSelectionPosition (my text, & left, & right))
			position = right;
		label = XmTextGetString (my text);
		notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label, & position);
		if (notAllowed) {
			XmTextSetSelection (my text, position, position + strlen (notAllowed), 0);
		} else {
			if (! checkSpellingInTier (me)) Melder_flushError (NULL);
		}
		XtFree (label);
	}
END

DIRECT (TextGridEditor, cb_checkSpellingInInterval)
	if (my spellingChecker) {
		XmTextPosition left, right;
		int position = 0;
		char *label, *notAllowed;
		if (XmTextGetSelectionPosition (my text, & left, & right))
			position = right;
		label = XmTextGetString (my text);
		notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label, & position);
		if (notAllowed) {
			XmTextSetSelection (my text, position, position + strlen (notAllowed), 0);
		}
		XtFree (label);
	}
END

DIRECT (TextGridEditor, cb_addToUserDictionary)
	if (my spellingChecker) {
		char *word = XmTextGetSelection (my text);
		SpellingChecker_addNewWord (my spellingChecker, word);
		XtFree (word);
		iferror return 0;
		if (my dataChangedCallback)
			my dataChangedCallback (me, my dataChangedClosure, my spellingChecker);
	}
END

/********** TIER MENU **********/

FORM (TextGridEditor, cb_renameTier, "Rename tier", 0)
	SENTENCE ("Name", "");
	OK
TextGrid grid = my data;
Data tier;
if (! checkTierSelection (me, "rename a tier")) return 0;
tier = grid -> tiers -> item [my selectedTier];
SET_STRING ("Name", tier -> name ? tier -> name : "")
DO
	char *newName = GET_STRING ("Name");
	TextGrid grid = my data;
	Data tier;
	if (! checkTierSelection (me, "rename a tier")) return 0;
 	tier = grid -> tiers -> item [my selectedTier];

	Editor_save (me, "Rename tier");

	Thing_setName (tier, newName);

	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (TextGridEditor, cb_publishTier)
	TextGrid grid = my data;
	if (! checkTierSelection (me, "publish a tier")) return 0;

	if (my publishCallback) {
		Data anyTier = grid -> tiers -> item [my selectedTier], copy = Data_copy (anyTier);
		if (! copy) return 0;
		Thing_setName (copy, anyTier -> name); 
		my publishCallback (me, my publishClosure, copy);
	}
END

DIRECT (TextGridEditor, cb_removeAllTextFromTier)
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	if (! checkTierSelection (me, "remove all text from a tier")) return 0;
	identifyTierClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);

	Editor_save (me, "Remove text from tier");
	if (intervalTier) {
		IntervalTier_removeText (intervalTier);
	} else {
		TextTier_removeText (textTier);
	}

	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (TextGridEditor, cb_removeTier)
	TextGrid grid = my data;
	if (grid -> tiers -> size <= 1) {
		return Melder_error ("Sorry, I refuse to remove the last tier.");
	}
	if (! checkTierSelection (me, "remove a tier")) return 0;

	Editor_save (me, "Remove tier");
	Collection_removeItem (grid -> tiers, my selectedTier);

	my selectedTier = 1;
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (TextGridEditor, cb_addIntervalTier, "Add interval tier", 0)
	NATURAL ("Position", "1 (= at top)")
	SENTENCE ("Name", "")
	OK
TextGrid grid = my data;
char text [20];
sprintf (text, "%ld (= at bottom)", grid -> tiers -> size + 1);
SET_STRING ("Position", text)
SET_STRING ("Name", "")
DO
	TextGrid grid = my data;
	int position = GET_INTEGER ("Position");
	char *name = GET_STRING ("Name");
	IntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
	if (! tier) return 0;
	if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
	Thing_setName (tier, name);

	Editor_save (me, "Add interval tier");
	Ordered_addItemPos (grid -> tiers, tier, position);

	my selectedTier = position;
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (TextGridEditor, cb_addPointTier, "Add point tier", 0)
	NATURAL ("Position", "1 (= at top)")
	SENTENCE ("Name", "");
	OK
TextGrid grid = my data;
char text [20];
sprintf (text, "%ld (= at bottom)", grid -> tiers -> size + 1);
SET_STRING ("Position", text)
SET_STRING ("Name", "")
DO
	TextGrid grid = my data;
	int position = GET_INTEGER ("Position");
	char *name = GET_STRING ("Name");
	TextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
	if (! tier) return 0;
	if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
	Thing_setName (tier, name);

	Editor_save (me, "Add point tier");
	Ordered_addItemPos (grid -> tiers, tier, position);

	my selectedTier = position;
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (TextGridEditor, cb_duplicateTier, "Duplicate tier", 0)
	NATURAL ("Position", "1 (= at top)")
	SENTENCE ("Name", "")
	OK
TextGrid grid = my data;
char text [200];
if (my selectedTier) {
	sprintf (text, "%ld", my selectedTier + 1);
	SET_STRING ("Position", text)
	SET_STRING ("Name", ((AnyTier) grid -> tiers -> item [my selectedTier]) -> name)
}
DO
	TextGrid grid = my data;
	int position = GET_INTEGER ("Position");
	char *name = GET_STRING ("Name");
	AnyTier tier, newTier;
	if (! checkTierSelection (me, "duplicate a tier")) return 0;
	tier = grid -> tiers -> item [my selectedTier];
	newTier = Data_copy (tier);
	if (! newTier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
	Thing_setName (newTier, name);

	Editor_save (me, "Duplicate tier");
	Ordered_addItemPos (grid -> tiers, newTier, position);

	my selectedTier = position;
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END


/********** HELP MENU **********/

DIRECT (TextGridEditor, cb_TextGridEditorHelp) Melder_help ("TextGridEditor"); END
DIRECT (TextGridEditor, cb_AboutSpecialSymbols) Melder_help ("Special symbols"); END
DIRECT (TextGridEditor, cb_PhoneticSymbols) Melder_help ("Phonetic symbols"); END
DIRECT (TextGridEditor, cb_AboutTextStyles) Melder_help ("Text styles"); END

MOTIF_CALLBACK (cb_textChanged)
	iam (TextGridEditor);
	TextGrid grid = my data;
	if (my suppressRedraw) return;   /* Prevent infinite loop if 'draw' method calls XmTextSetString. */
	if (my selectedTier) {
		char *text = XmTextGetString (my text);
		IntervalTier intervalTier;
		TextTier textTier;
		identifyTierClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long selectedInterval = getSelectedInterval (me);
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals -> item [selectedInterval];
				TextInterval_setText (interval, text);
				FunctionEditor_redraw (me);
				Editor_broadcastChange (me);
			}
		} else {
			long selectedPoint = getSelectedPoint (me);
			if (selectedPoint) {
				TextPoint point = textTier -> points -> item [selectedPoint];
				Melder_free (point -> mark);
				if (strspn (text, " \n\t") != strlen (text))   /* Any visible characters? */
				point -> mark = Melder_strdup (text);
				FunctionEditor_redraw (me);
				Editor_broadcastChange (me);
			}
		}
		XtFree (text);
	}
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (TextGridEditor);
	inherited (TextGridEditor) createChildren (me);
	XtAddCallback (my text, XmNvalueChangedCallback, cb_textChanged, (XtPointer) me);
}

static void createMenus (I) {
	iam (TextGridEditor);
	EditorMenu menu;
	inherited (TextGridEditor) createMenus (me);

	if (my sound.data || my longSound.data) {
		Editor_addCommand (me, "File", "Copy to list of objects:", motif_INSENSITIVE, cb_publish /* dummy */);
		my publishPreserveButton = Editor_addCommand (me, "File", "Extract sound selection (preserve times)", 0, cb_publishPreserve);
		Editor_addCommand (me, "File", "Extract selection (preserve times)", Editor_HIDDEN, cb_publishPreserve);
		my publishButton = Editor_addCommand (me, "File", "Extract sound selection (time from 0)", 0, cb_publish);
		Editor_addCommand (me, "File", "Extract selection (time from 0)", Editor_HIDDEN, cb_publish);
		Editor_addCommand (me, "File", "Extract selection", Editor_HIDDEN, cb_publish);
		Editor_addCommand (me, "File", "-- write --", 0, NULL);
		Editor_addCommand (me, "File", "Copy to disk:", motif_INSENSITIVE, cb_publish /* dummy */);
	}
	Editor_addCommand (me, "File", "Write TextGrid to text file...", 'S', cb_writeToTextFile);
	if (my longSound.data) {
		my writeWavButton = Editor_addCommand (me, "File", "Write sound selection to WAV file...", 0, cb_writeWav);
		Editor_addCommand (me, "File", "Write selection to WAV file...", Editor_HIDDEN, cb_writeWav);
		my writeAiffButton = Editor_addCommand (me, "File", "Write sound selection to AIFF file...", 0, cb_writeAiff);
		Editor_addCommand (me, "File", "Write selection to AIFF file...", Editor_HIDDEN, cb_writeAiff);
		my writeAifcButton = Editor_addCommand (me, "File", "Write sound selection to AIFC file...", 0, cb_writeAifc);
		Editor_addCommand (me, "File", "Write selection to AIFC file...", Editor_HIDDEN, cb_writeAifc);
		my writeNextSunButton = Editor_addCommand (me, "File", "Write sound selection to Next/Sun file...", 0, cb_writeNextSun);
		Editor_addCommand (me, "File", "Write selection to Next/Sun file...", Editor_HIDDEN, cb_writeNextSun);
		my writeNistButton = Editor_addCommand (me, "File", "Write sound selection to NIST file...", 0, cb_writeNist);
		Editor_addCommand (me, "File", "Write selection to NIST file...", Editor_HIDDEN, cb_writeNist);
	}
	Editor_addCommand (me, "File", "-- close --", 0, NULL);

	Editor_addCommand (me, "Edit", "-- cut copy paste --", 0, NULL);
	Editor_addCommand (me, "Edit", "Cut text", 'X', cb_cut);
	Editor_addCommand (me, "Edit", "Cut", Editor_HIDDEN, cb_cut);
	Editor_addCommand (me, "Edit", "Copy text", 'C', cb_copy);
	Editor_addCommand (me, "Edit", "Copy", Editor_HIDDEN, cb_copy);
	Editor_addCommand (me, "Edit", "Paste text", 'V', cb_paste);
	Editor_addCommand (me, "Edit", "Paste", Editor_HIDDEN, cb_paste);
	Editor_addCommand (me, "Edit", "Erase text", 0, cb_erase);
	Editor_addCommand (me, "Edit", "Erase", Editor_HIDDEN, cb_erase);
	Editor_addCommand (me, "Edit", "-- encoding --", 0, NULL);
	Editor_addCommand (me, "Edit", "Genericize entire TextGrid", 0, cb_genericize);
	Editor_addCommand (me, "Edit", "Genericize", Editor_HIDDEN, cb_genericize);
	Editor_addCommand (me, "Edit", "Nativize entire TextGrid", 0, cb_nativize);
	Editor_addCommand (me, "Edit", "Nativize", Editor_HIDDEN, cb_nativize);
	Editor_addCommand (me, "Edit", "-- search --", 0, NULL);
	Editor_addCommand (me, "Edit", "Find...", 'F', cb_find);
	Editor_addCommand (me, "Edit", "Find again", 'G', cb_findAgain);

	if (my sound.data) {
		Editor_addCommand (me, "Select", "-- move to zero --", 0, 0);
		Editor_addCommand (me, "Select", "Move start of selection to nearest zero crossing", ',', cb_moveBtoZero);
		Editor_addCommand (me, "Select", "Move begin of selection to nearest zero crossing", Editor_HIDDEN, cb_moveBtoZero);
		Editor_addCommand (me, "Select", "Move cursor to nearest zero crossing", '0', cb_moveCursorToZero);
		Editor_addCommand (me, "Select", "Move end of selection to nearest zero crossing", '.', cb_moveEtoZero);
	}

	Editor_addCommand (me, "Query", "-- query interval --", 0, NULL);
	Editor_addCommand (me, "Query", "Get starting point of interval", 0, cb_getStartingPointOfInterval);
	Editor_addCommand (me, "Query", "Get end point of interval", 0, cb_getEndPointOfInterval);
	Editor_addCommand (me, "Query", "Get label of interval", 0, cb_getLabelOfInterval);

	menu = Editor_addMenu (me, "Interval", 0);
	EditorMenu_addCommand (menu, "Add interval on tier 1", motif_COMMAND | '1', cb_insertIntervalOnTier1);
	EditorMenu_addCommand (menu, "Add interval on tier 2", motif_COMMAND | '2', cb_insertIntervalOnTier2);
	EditorMenu_addCommand (menu, "Add interval on tier 3", motif_COMMAND | '3', cb_insertIntervalOnTier3);
	EditorMenu_addCommand (menu, "Add interval on tier 4", motif_COMMAND | '4', cb_insertIntervalOnTier4);
	EditorMenu_addCommand (menu, "Add interval on tier 5", motif_COMMAND | '5', cb_insertIntervalOnTier5);
	EditorMenu_addCommand (menu, "Add interval on tier 6", motif_COMMAND | '6', cb_insertIntervalOnTier6);
	EditorMenu_addCommand (menu, "Add interval on tier 7", motif_COMMAND | '7', cb_insertIntervalOnTier7);
	EditorMenu_addCommand (menu, "Add interval on tier 8", motif_COMMAND | '8', cb_insertIntervalOnTier8);

	menu = Editor_addMenu (me, "Boundary", 0);
	/*EditorMenu_addCommand (menu, "Move to B", 0, cb_moveToB);
	EditorMenu_addCommand (menu, "Move to E", 0, cb_moveToE);*/
	if (my sound.data)
		EditorMenu_addCommand (menu, "Move to nearest zero crossing", 0, cb_moveToZero);
	EditorMenu_addCommand (menu, "-- insert boundary --", 0, NULL);
	EditorMenu_addCommand (menu, "Add on selected tier", motif_ENTER, cb_insertOnSelectedTier);
	EditorMenu_addCommand (menu, "Add on tier 1", motif_COMMAND | motif_F1, cb_insertOnTier1);
	EditorMenu_addCommand (menu, "Add on tier 2", motif_COMMAND | motif_F2, cb_insertOnTier2);
	EditorMenu_addCommand (menu, "Add on tier 3", motif_COMMAND | motif_F3, cb_insertOnTier3);
	EditorMenu_addCommand (menu, "Add on tier 4", motif_COMMAND | motif_F4, cb_insertOnTier4);
	EditorMenu_addCommand (menu, "Add on tier 5", motif_COMMAND | motif_F5, cb_insertOnTier5);
	EditorMenu_addCommand (menu, "Add on tier 6", motif_COMMAND | motif_F6, cb_insertOnTier6);
	EditorMenu_addCommand (menu, "Add on tier 7", motif_COMMAND | motif_F7, cb_insertOnTier7);
	EditorMenu_addCommand (menu, "Add on tier 8", motif_COMMAND | motif_F8, cb_insertOnTier8);
	EditorMenu_addCommand (menu, "Add on all tiers", motif_COMMAND | motif_F9, cb_insertOnAllTiers);
	EditorMenu_addCommand (menu, "-- remove mark --", 0, NULL);
	EditorMenu_addCommand (menu, "Remove", motif_OPTION | motif_BACKSPACE, cb_removePointOrBoundary);

	menu = Editor_addMenu (me, "Tier", 0);
	EditorMenu_addCommand (menu, "Copy tier to list of objects", 0, cb_publishTier);
	EditorMenu_addCommand (menu, "-- add tier --", 0, NULL);
	EditorMenu_addCommand (menu, "Add interval tier...", 0, cb_addIntervalTier);
	EditorMenu_addCommand (menu, "Add point tier...", 0, cb_addPointTier);
	EditorMenu_addCommand (menu, "Duplicate tier...", 0, cb_duplicateTier);
	EditorMenu_addCommand (menu, "Rename tier...", 0, cb_renameTier);
	EditorMenu_addCommand (menu, "-- remove tier --", 0, NULL);
	EditorMenu_addCommand (menu, "Remove all text from tier", 0, cb_removeAllTextFromTier);
	EditorMenu_addCommand (menu, "Remove entire tier", 0, cb_removeTier);

	if (my spellingChecker) {
		menu = Editor_addMenu (me, "Spell", 0);
		EditorMenu_addCommand (menu, "Check spelling in tier", 'N', cb_checkSpelling);
		EditorMenu_addCommand (menu, "Check spelling in interval", 0, cb_checkSpellingInInterval);
		EditorMenu_addCommand (menu, "-- edit lexicon --", 0, NULL);
		EditorMenu_addCommand (menu, "Add selected word to user dictionary", 0, cb_addToUserDictionary);
	}

	if (my sound.data || my longSound.data) {
		FunctionEditor_SoundAnalysis_addMenus (me);
	}

	Editor_addCommand (me, "Help", "TextGridEditor help", '?', cb_TextGridEditorHelp);
	Editor_addCommand (me, "Help", "About special symbols", 0, cb_AboutSpecialSymbols);
	Editor_addCommand (me, "Help", "Phonetic symbols", 0, cb_PhoneticSymbols);
	Editor_addCommand (me, "Help", "About text styles", 0, cb_AboutTextStyles);
}

/********** DRAWING AREA **********/

static void prepareDraw (I) {
	iam (TextGridEditor);
	if (my longSound.data) {
		LongSound_haveWindow (my longSound.data, my startWindow, my endWindow);
		Melder_clearError ();
	}
}

static void drawIntervalTier (TextGridEditor me, IntervalTier tier, int itier) {
	short x1DC, x2DC, yDC;
	int selectedInterval = itier == my selectedTier ? getSelectedInterval (me) : 0, iinterval, ninterval = tier -> intervals -> size;
	Graphics_WCtoDC (my graphics, my startWindow, 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics, my endWindow, 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics, my useTextStyles);
	Graphics_setNumberSignIsBold (my graphics, my useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics, my useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics, my useTextStyles);

	/*
	 * Highlight interval: yellow (selected) or green (matching label).
	 */
	
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmax > my startWindow && tmin < my endWindow) {   /* Interval visible? */
			int selected = iinterval == selectedInterval;
			int labelMatches = Melder_stringMatchesCriterion (interval -> text, my greenMethod, my greenString);
			if (tmin < my startWindow) tmin = my startWindow;
			if (tmax > my endWindow) tmax = my endWindow;
			if (labelMatches) {
				Graphics_setColour (my graphics, Graphics_LIME);
				Graphics_fillRectangle (my graphics, tmin, tmax, 0.0, 1.0);
			}
			if (selected) {
				if (labelMatches) {
					tmin = 0.85 * tmin + 0.15 * tmax;
					tmax = 0.15 * tmin + 0.85 * tmax;
				}
				Graphics_setColour (my graphics, Graphics_YELLOW);
				Graphics_fillRectangle (my graphics, tmin, tmax, labelMatches ? 0.15 : 0.0, labelMatches? 0.85: 1.0);
			}
		}
	}
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_line (my graphics, my endWindow, 0.0, my endWindow, 1.0);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		int cursorAtBoundary = FALSE;
		for (iinterval = 2; iinterval <= ninterval; iinterval ++) {
			TextInterval interval = tier -> intervals -> item [iinterval];
			if (interval -> xmin == my startSelection) cursorAtBoundary = TRUE;
		}
		if (! cursorAtBoundary) {
			double dy = Graphics_dyMMtoWC (my graphics, 1.5);
			Graphics_setGrey (my graphics, 0.8);
			Graphics_setLineWidth (my graphics, 5.0);
			Graphics_line (my graphics, my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics, 1.0);
			Graphics_setColour (my graphics, Graphics_BLUE);
			Graphics_circle_mm (my graphics, my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics, my alignment, Graphics_HALF);
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		int selected;
		if (tmin < my tmin) tmin = my tmin; if (tmax > my tmax) tmax = my tmax;
		if (tmin >= tmax) continue;
		selected = selectedInterval == iinterval;

		/*
		 * Draw left boundary.
		 */
		if (tmin >= my startWindow && tmin <= my endWindow && iinterval > 1) {
			int selected = ( my selectedTier == itier && tmin == my startSelection );
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my graphics, 5.0);
			Graphics_line (my graphics, tmin, 0.0, tmin, 1.0);
			Graphics_setLineWidth (my graphics, 1.0);

			/*
			 * Show alignment with cursor.
			 */
			if (tmin == my startSelection) {
				Graphics_setColour (my graphics, Graphics_YELLOW);
				Graphics_line (my graphics, tmin, 0.0, tmin, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics, 1.0);

		/*
		 * Draw label text.
		 */
		if (interval -> text && tmax >= my startWindow && tmin <= my endWindow) {
			double t1 = my startWindow > tmin ? my startWindow : tmin;
			double t2 = my endWindow < tmax ? my endWindow : tmax;
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLACK);
			Graphics_textRect (my graphics, t1, t2, 0.0, 1.0, interval -> text);
			Graphics_setColour (my graphics, Graphics_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics, TRUE);
	Graphics_setNumberSignIsBold (my graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my graphics, TRUE);
}

static void drawTextTier (TextGridEditor me, TextTier tier, int itier) {
	int ipoint, npoint = tier -> points -> size;
	Graphics_setPercentSignIsItalic (my graphics, my useTextStyles);
	Graphics_setNumberSignIsBold (my graphics, my useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics, my useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics, my useTextStyles);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		int cursorAtPoint = FALSE;
		for (ipoint = 1; ipoint <= npoint; ipoint ++) {
			TextPoint point = tier -> points -> item [ipoint];
			if (point -> time == my startSelection) cursorAtPoint = TRUE;
		}
		if (! cursorAtPoint) {
			double dy = Graphics_dyMMtoWC (my graphics, 1.5);
			Graphics_setGrey (my graphics, 0.8);
			Graphics_setLineWidth (my graphics, 5.0);
			Graphics_line (my graphics, my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics, 1.0);
			Graphics_setColour (my graphics, Graphics_BLUE);
			Graphics_circle_mm (my graphics, my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics, Graphics_CENTER, Graphics_HALF);
	for (ipoint = 1; ipoint <= npoint; ipoint ++) {
		TextPoint point = tier -> points -> item [ipoint];
		double t = point -> time;
		if (t >= my startWindow && t <= my endWindow) {
			int selected = ( itier == my selectedTier && t == my startSelection );
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my graphics, 5.0);
			Graphics_line (my graphics, t, 0.0, t, 0.2);
			Graphics_line (my graphics, t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics, 1.0);

			/*
			 * Wipe out the cursor where the text is going to be.
			 */
			Graphics_setColour (my graphics, Graphics_WHITE);
			Graphics_line (my graphics, t, 0.2, t, 0.8);

			/*
			 * Show alignment with cursor.
			 */
			if (my startSelection == my endSelection && t == my startSelection) {
				Graphics_setColour (my graphics, Graphics_YELLOW);
				Graphics_line (my graphics, t, 0.0, t, 0.2);
				Graphics_line (my graphics, t, 0.8, t, 1.0);
			}
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLUE);
			if (point -> mark) Graphics_text (my graphics, t, 0.5, point -> mark);
		}
	}
	Graphics_setPercentSignIsItalic (my graphics, TRUE);
	Graphics_setNumberSignIsBold (my graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my graphics, TRUE);
}

static void draw (I) {
	iam (TextGridEditor);
	TextGrid grid = my data;
	Graphics_Viewport vp1, vp2;
	long itier, ntier = grid -> tiers -> size;
	int oldFont = Graphics_inqFont (my graphics);
	int oldFontSize = Graphics_inqFontSize (my graphics);
	int showAnalysis = (my spectrogram.show || my pitch.show || my intensity.show || my formant.show) && (my longSound.data || my sound.data);
	double soundY = computeSoundY (me), soundY2 = showAnalysis ? 0.5 * (1.0 + soundY) : soundY;

	/*
	 * Draw optional sound.
	 */
	if (my longSound.data || my sound.data) {
		vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, soundY2, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		FunctionEditor_Sound_draw (me, -1.0, 1.0);
		Graphics_flushWs (my graphics);
		Graphics_resetViewport (my graphics, vp1);
	}

	/*
	 * Draw tiers.
	 */
	if (my longSound.data || my sound.data) vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, 0.0, soundY);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
	for (itier = 1; itier <= ntier; itier ++) {
		Data anyTier = grid -> tiers -> item [itier];
		int selected = itier == my selectedTier;
		int isIntervalTier = anyTier -> methods == (Data_Table) classIntervalTier;
		vp2 = Graphics_insetViewport (my graphics, 0.0, 1.0,
			1.0 - (double) itier / (double) ntier,
			1.0 - (double) (itier - 1) / (double) ntier);
		Graphics_setColour (my graphics, Graphics_BLACK);
		if (itier != 1) Graphics_line (my graphics, my startWindow, 1.0, my endWindow, 1.0);

		/*
		 * Show the number and the name of the tier.
		 */
		Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLACK);
		Graphics_setFont (my graphics, oldFont);
		Graphics_setFontSize (my graphics, oldFontSize * 1.5);
		Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_printf (my graphics, my startWindow, 0.5, selected ? "\\pf %ld" : "%ld", itier);
		Graphics_setFontSize (my graphics, oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (my graphics, Graphics_LEFT, my showNumberOf > 1 ? Graphics_BOTTOM : Graphics_HALF);
			Graphics_printf (my graphics, my endWindow, 0.5, anyTier -> name);
		}
		if (my showNumberOf > 1) {
			Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
			if (my showNumberOf == 2) {
				long count = isIntervalTier ? ((IntervalTier) anyTier) -> intervals -> size : ((TextTier) anyTier) -> points -> size;
				Graphics_printf (my graphics, my endWindow, 0.5, "(%ld)", count);
			} else {
				long count = 0;
				if (isIntervalTier) {
					IntervalTier tier = (IntervalTier) anyTier;
					long ninterval = tier -> intervals -> size, iinterval;
					for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
						TextInterval interval = tier -> intervals -> item [iinterval];
						if (interval -> text != NULL && interval -> text [0] != '\0') {
							count ++;
						}
					}
				} else {
					TextTier tier = (TextTier) anyTier;
					long npoint = tier -> points -> size, ipoint;
					for (ipoint = 1; ipoint <= npoint; ipoint ++) {
						TextPoint point = tier -> points -> item [ipoint];
						if (point -> mark != NULL && point -> mark [0] != '\0') {
							count ++;
						}
					}
				}
				Graphics_printf (my graphics, my endWindow, 0.5, "(##%ld#)", count);
			}
		}

		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setFont (my graphics, Graphics_TIMES);
		Graphics_setFontSize (my graphics, my fontSize);
		if (isIntervalTier)
			drawIntervalTier (me, (IntervalTier) anyTier, itier);
		else
			drawTextTier (me, (TextTier) anyTier, itier);
		Graphics_resetViewport (my graphics, vp2);
	}
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_setFont (my graphics, oldFont);
	Graphics_setFontSize (my graphics, oldFontSize);
	if (my longSound.data || my sound.data) Graphics_resetViewport (my graphics, vp1);
	Graphics_flushWs (my graphics);

	if (showAnalysis) {
		vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, soundY, soundY2);
		FunctionEditor_SoundAnalysis_draw (me);
		Graphics_flushWs (my graphics);
		Graphics_resetViewport (my graphics, vp1);
		/* Draw pulses. */
		if (my pulses.show) {
			vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, soundY2, 1.0);
			FunctionEditor_SoundAnalysis_drawPulses (me);
			FunctionEditor_Sound_draw (me, -1.0, 1.0);   /* Second time, partially across the pulses. */
			Graphics_flushWs (my graphics);
			Graphics_resetViewport (my graphics, vp1);
		}
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
	if (my longSound.data || my sound.data) {
		Graphics_line (my graphics, my startWindow, soundY, my endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (my graphics, my startWindow, soundY2, my endWindow, soundY2);
			Graphics_line (my graphics, my startWindow, soundY, my startWindow, soundY2);
			Graphics_line (my graphics, my endWindow, soundY, my endWindow, soundY2);
		}
	}

	/*
	 * Finally, us usual, update the menus.
	 */
	if (my publishButton) {
		int selected = my endSelection > my startSelection ? TRUE : FALSE;
		XtSetSensitive (my publishButton, selected);
		XtSetSensitive (my publishPreserveButton, selected);
	}
	if (my longSound.data) {
		long first, last, selectedSamples;
		selectedSamples = Sampled_getWindowSamples (my longSound.data, my startSelection, my endSelection, & first, & last);
		XtSetSensitive (my writeWavButton, selectedSamples != 0);
		XtSetSensitive (my writeAiffButton, selectedSamples != 0);
		XtSetSensitive (my writeAifcButton, selectedSamples != 0);
		XtSetSensitive (my writeNextSunButton, selectedSamples != 0);
		XtSetSensitive (my writeNistButton, selectedSamples != 0);
	}
}

static void drawWhileDragging (TextGridEditor me, double numberOfTiers, int *selectedTier, double x, double soundY) {
	long itier;
	for (itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		double ymin = soundY * (1.0 - (double) itier / numberOfTiers);
		double ymax = soundY * (1.0 - (double) (itier - 1) / numberOfTiers);
		Graphics_setLineWidth (my graphics, 7);
		Graphics_line (my graphics, x, ymin, x, ymax);
	}
	Graphics_setLineWidth (my graphics, 1);
	Graphics_line (my graphics, x, 0, x, 1.01);
	Graphics_printf (my graphics, x, 1.01, "%f", x);
}

static void dragBoundary (TextGridEditor me, double xbegin, int iClickedTier, int shiftKeyPressed) {
	TextGrid grid = my data;
	int itier, numberOfTiers = grid -> tiers -> size, itierDrop;
	double xWC = xbegin, yWC;
	double leftDraggingBoundary = my tmin, rightDraggingBoundary = my tmax;   /* Initial dragging range. */
	int selectedTier [100];
	double soundY = computeSoundY (me);

	/*
	 * Determine the set of selected boundaries and points, and the dragging range.
	 */
	for (itier = 1; itier <= numberOfTiers; itier ++) {
		selectedTier [itier] = FALSE;   /* The default. */
		/*
		 * If she has pressed the shift key, let her drag all the boundaries and points at this time.
		 * Otherwise, let her only drag the boundary or point on the clicked tier.
		 */
		if (itier == iClickedTier || shiftKeyPressed == my shiftDragMultiple) {
			IntervalTier intervalTier;
			TextTier textTier;
			identifyTierClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
			if (intervalTier) {
				long ibound = IntervalTier_hasBoundary (intervalTier, xbegin);
				if (ibound) {
					TextInterval leftInterval = intervalTier -> intervals -> item [ibound - 1];
					TextInterval rightInterval = intervalTier -> intervals -> item [ibound];
					selectedTier [itier] = TRUE;
					/*
					 * Prevent her to drag the boundary past its left or right neighbours on the same tier.
					 */
					if (leftInterval -> xmin > leftDraggingBoundary) {
						leftDraggingBoundary = leftInterval -> xmin;
					}
					if (rightInterval -> xmax < rightDraggingBoundary) {
						rightDraggingBoundary = rightInterval -> xmax;
					}
				}
			} else {
				if (AnyTier_hasPoint (textTier, xbegin)) {
					/*
					 * Other than with boundaries on interval tiers,
					 * points on text tiers can be dragged past their neighbours.
					 */
					selectedTier [itier] = TRUE;
				}
			}
		}
	}

	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	Graphics_setTextAlignment (my graphics, Graphics_CENTER, Graphics_BOTTOM);
	drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
	while (Graphics_mouseStillDown (my graphics)) {
		drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
		Graphics_getMouseLocation (my graphics, & xWC, & yWC);
		drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
	}
	drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
	Graphics_xorOff (my graphics);

	/*
	 * The simplest way to cancel the dragging operation, is to drag outside the window.
	 */
	if (xWC <= my startWindow || xWC >= my endWindow) {
		return;
	}

	/*
	 * If she dropped near an existing boundary in an unselected tier or near the cursor, we snap to that mark.
	 */
	itierDrop = _TextGridEditor_yWCtoTier (me, yWC);
	if (yWC > 0.0 && yWC < soundY && ! selectedTier [itierDrop]) {   /* Dropped inside an unselected tier? */
		Data anyTierDrop = grid -> tiers -> item [itierDrop];
		if (anyTierDrop -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tierDrop = (IntervalTier) anyTierDrop;
			long ibound;
			for (ibound = 1; ibound < tierDrop -> intervals -> size; ibound ++) {
				TextInterval left = tierDrop -> intervals -> item [ibound];
				if (fabs (Graphics_dxWCtoMM (my graphics, xWC - left -> xmax)) < 1.5) {   /* Near a boundary? */
					/*
					 * Snap to boundary.
					 */
					xWC = left -> xmax;
				}
			}
		} else {
			TextTier tierDrop = (TextTier) anyTierDrop;
			long ipoint;
			for (ipoint = 1; ipoint <= tierDrop -> points -> size; ipoint ++) {
				TextPoint point = tierDrop -> points -> item [ipoint];
				if (fabs (Graphics_dxWCtoMM (my graphics, xWC - point -> time)) < 1.5) {   /* Near a point? */
					/*
					 * Snap to point.
					 */
					xWC = point -> time;
				}
			}
		}
	} else if (xbegin != my startSelection && fabs (Graphics_dxWCtoMM (my graphics, xWC - my startSelection)) < 1.5) {   /* Near the cursor? */
		/*
		 * Snap to cursor.
		 */
		xWC = my startSelection;
	} else if (xbegin != my endSelection && fabs (Graphics_dxWCtoMM (my graphics, xWC - my endSelection)) < 1.5) {   /* Near the cursor? */
		/*
		 * Snap to cursor.
		 */
		xWC = my endSelection;
	}

	/*
	 * We cannot move a boundary out of the dragging range.
	 */
	if (xWC <= leftDraggingBoundary || xWC >= rightDraggingBoundary) {
		Melder_beep ();
		return;
	}

	Editor_save (me, "Drag");

	for (itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		IntervalTier intervalTier;
		TextTier textTier;
		identifyTierClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
		if (intervalTier) {
			long ibound, numberOfIntervals = intervalTier -> intervals -> size;
			Any *intervals = intervalTier -> intervals -> item;
			for (ibound = 2; ibound <= numberOfIntervals; ibound ++) {
				TextInterval left = intervals [ibound - 1], right = intervals [ibound];
				if (left -> xmax == xbegin) {   /* Boundary dragged? */
					left -> xmax = right -> xmin = xWC;   /* Move boundary to drop site. */
					break;
				}
			}
		} else {
			long iDraggedPoint = AnyTier_hasPoint (textTier, xbegin);
			if (iDraggedPoint) {
				long dropSiteHasPoint = AnyTier_hasPoint (textTier, xWC);
				if (dropSiteHasPoint) {
					Melder_warning ("Cannot drop point on an existing point.");
				} else {
					TextPoint point = textTier -> points -> item [iDraggedPoint];
					/*
					 * Move point to drop site. May have passed another point.
					 */
					TextPoint newPoint = Data_copy (point);
					newPoint -> time = xWC;   /* Move point to drop site. */
					Collection_removeItem (textTier -> points, iDraggedPoint);
					Collection_addItem (textTier -> points, newPoint);
				}
			}
		}
	}

	/*
	 * Select the drop site.
	 */
	if (my startSelection == xbegin)
		my startSelection = xWC;
	if (my endSelection == xbegin)
		my endSelection = xWC;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	FunctionEditor_marksChanged (me);
	Editor_broadcastChange (me);
}

static int click (I, double xclick, double yWC, int shiftKeyPressed) {
	iam (TextGridEditor);
	TextGrid grid = my data;
	double tmin, tmax, x, y;
	long ntiers = grid -> tiers -> size, iClickedTier, iClickedInterval, iClickedPoint;
	int clickedLeftBoundary = 0, nearBoundaryOrPoint, nearCursorCircle, drag = FALSE;
	IntervalTier intervalTier;
	TextTier textTier;
	TextInterval interval = NULL;
	TextPoint point = NULL;
	double soundY = computeSoundY (me);
	double tnear;

	/*
	 * In answer to a click in the sound part,
	 * we keep the same tier selected and move the cursor or drag the "yellow" selection.
	 */
	if (yWC > soundY) {   /* Clicked in sound part? */
		if ((my spectrogram.show || my formant.show) && yWC < 0.5 * (soundY + 1.0)) {
			my spectrogram.cursor = my spectrogram.viewFrom +
				2.0 * (yWC - soundY) / (1.0 - soundY) * (my spectrogram.viewTo - my spectrogram.viewFrom);
		}
		inherited (TextGridEditor) click (me, xclick, yWC, shiftKeyPressed);
		return FunctionEditor_UPDATE_NEEDED;
	}

	if (xclick <= my startWindow || xclick >= my endWindow) {
		return FunctionEditor_NO_UPDATE_NEEDED;
	}

	/*
	 * She clicked in the grid part.
	 * We select the tier in which she clicked.
	 */
	iClickedTier = _TextGridEditor_yWCtoTier (me, yWC);
	_TextGridEditor_timeToInterval (me, xclick, iClickedTier, & tmin, & tmax);
	identifyTierClass (grid -> tiers -> item [iClickedTier], & intervalTier, & textTier);

	/*
	 * Get the time of the nearest boundary or point.
	 */
	tnear = NUMundefined;
	if (intervalTier) {
		iClickedInterval = IntervalTier_timeToIndex (intervalTier, xclick);
		if (iClickedInterval) {
			interval = intervalTier -> intervals -> item [iClickedInterval];
			if (xclick > 0.5 * (interval -> xmin + interval -> xmax)) {
				tnear = interval -> xmax;
				clickedLeftBoundary = iClickedInterval + 1;
			} else {
				tnear = interval -> xmin;
				clickedLeftBoundary = iClickedInterval;
			}
		} else {
			/*
			 * She clicked outside time domain of intervals.
			 * This can occur when we are grouped with a longer time function.
			 */
			my selectedTier = iClickedTier;
			return FunctionEditor_UPDATE_NEEDED;
		}
	} else {
		iClickedPoint = AnyTier_timeToNearestIndex (textTier, xclick);
		if (iClickedPoint) {
			point = textTier -> points -> item [iClickedPoint];
			tnear = point -> time;
		}
	}
	Melder_assert (! (intervalTier && ! clickedLeftBoundary));

	/*
	 * Where did she click?
	 */
	nearBoundaryOrPoint = tnear != NUMundefined && fabs (Graphics_dxWCtoMM (my graphics, xclick - tnear)) < 1.5;
	nearCursorCircle = my startSelection == my endSelection && Graphics_distanceWCtoMM (my graphics, xclick, yWC,
		my startSelection, (ntiers + 1 - iClickedTier) * soundY / ntiers - Graphics_dyMMtoWC (my graphics, 1.5)) < 1.5;

	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (my graphics)) {
		Graphics_getMouseLocation (my graphics, & x, & y);
		if (x < my startWindow) x = my startWindow;
		if (x > my endWindow) x = my endWindow;
		if (fabs (Graphics_dxWCtoMM (my graphics, x - xclick)) > 1.5) {
			drag = TRUE;
			break;
		}
	}

	if (nearBoundaryOrPoint) {
		/*
		 * Possibility 1: she clicked near a boundary or point.
		 * Select or drag it.
		 */
		if (intervalTier && (clickedLeftBoundary < 2 || clickedLeftBoundary > intervalTier -> intervals -> size)) {		
			/*
			 * Ignore click on left edge of first interval or right edge of last interval.
			 */
			my selectedTier = iClickedTier;
		} else if (drag) {
			/*
			 * The tier that has been clicked becomes the new selected tier.
			 * This has to be done before the next Update, i.e. also before dragBoundary!
			 */
			my selectedTier = iClickedTier;
			dragBoundary (me, tnear, iClickedTier, shiftKeyPressed);
			return FunctionEditor_NO_UPDATE_NEEDED;
		} else {
			/*
			 * If she clicked on an unselected boundary or point, we select it.
			 */
			if (shiftKeyPressed) {
				if (tnear > 0.5 * (my startSelection + my endSelection))
					my endSelection = tnear;
				else
					my startSelection = tnear;
			} else {
				my startSelection = my endSelection = tnear;   /* Move cursor so that the boundary or point is selected. */
			}
			my selectedTier = iClickedTier;
		}
	} else if (nearCursorCircle) {
		/*
		 * Possibility 2: she clicked near the cursor circle.
		 * Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
		 * because we are not 'nearBoundaryOrPoint'.
		 */
		insertBoundaryOrPoint (me, iClickedTier, my startSelection, FALSE);
		my selectedTier = iClickedTier;
		FunctionEditor_marksChanged (me);
		Editor_broadcastChange (me);
		if (drag) Graphics_waitMouseUp (my graphics);
		return FunctionEditor_NO_UPDATE_NEEDED;
	} else {
		/*
		 * Possibility 3: she clicked in empty space.
		 */
		if (intervalTier) {
			my startSelection = tmin;
			my endSelection = tmax;
		}
		my selectedTier = iClickedTier;
	}
	if (drag) Graphics_waitMouseUp (my graphics);
	return FunctionEditor_UPDATE_NEEDED;
}

static int clickB (I, double t, double yWC) {
	iam (TextGridEditor);
	TextGrid grid = my data;
	int itier;
	double tmin, tmax;
	double soundY = computeSoundY (me);

	if (yWC > soundY) {   /* Clicked in sound part? */
		my startSelection = t;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	itier = _TextGridEditor_yWCtoTier (me, yWC);
	_TextGridEditor_timeToInterval (me, t, itier, & tmin, & tmax);
	my startSelection = t - tmin < tmax - t ? tmin : tmax;   /* To nearest boundary. */
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

static int clickE (I, double t, double yWC) {
	iam (TextGridEditor);
	TextGrid grid = my data;
	int itier;
	double tmin, tmax;
	double soundY = computeSoundY (me);

	if (yWC > soundY) {   /* Clicked in sound part? */
		my endSelection = t;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	itier = _TextGridEditor_yWCtoTier (me, yWC);
	_TextGridEditor_timeToInterval (me, t, itier, & tmin, & tmax);
	my endSelection = t - tmin < tmax - t ? tmin : tmax;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

static void play (I, double tmin, double tmax) {
	iam (TextGridEditor);
	if (my longSound.data) {
		LongSound_playPart (my longSound.data, tmin, tmax, our playCallback, me);
	} else if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	}
}

static void viewMenuEntries (I) {
	iam (TextGridEditor);
	if (my sound.data || my longSound.data) {
		FunctionEditor_Sound_createMenus (me);
		FunctionEditor_SoundAnalysis_viewMenus (me);
	}
	Editor_addCommand (me, "View", "Select previous tier", motif_OPTION | motif_UP_ARROW, cb_selectPreviousTier);
	Editor_addCommand (me, "View", "Select next tier", motif_OPTION | motif_DOWN_ARROW, cb_selectNextTier);
	Editor_addCommand (me, "View", "Select previous interval", motif_OPTION | motif_LEFT_ARROW, cb_selectPreviousInterval);
	Editor_addCommand (me, "View", "Select next interval", motif_OPTION | motif_RIGHT_ARROW, cb_selectNextInterval);
	Editor_addCommand (me, "View", "Extend-select left", motif_SHIFT | motif_OPTION | motif_LEFT_ARROW, cb_extendSelectPreviousInterval);
	Editor_addCommand (me, "View", "Extend-select right", motif_SHIFT | motif_OPTION | motif_RIGHT_ARROW, cb_extendSelectNextInterval);
	Editor_addCommand (me, "View", "-- select interval --", 0, 0);
	if (my sound.data || my longSound.data) {
		FunctionEditor_SoundAnalysis_selectionQueries (me);
	}
}

static void highlightSelection (I, double left, double right, double bottom, double top) {
	iam (TextGridEditor);
	if (my spectrogram.show && (my longSound.data || my sound.data)) {
		TextGrid grid = my data;
		double soundY = grid -> tiers -> size / (2.0 + grid -> tiers -> size * 1.8), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_highlight (my graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_highlight (my graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_highlight (my graphics, left, right, bottom, top);
	}
}

static void unhighlightSelection (I, double left, double right, double bottom, double top) {
	iam (TextGridEditor);
	if (my spectrogram.show) {
		TextGrid grid = my data;
		double soundY = grid -> tiers -> size / (2.0 + grid -> tiers -> size * 1.8), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_unhighlight (my graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_unhighlight (my graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_unhighlight (my graphics, left, right, bottom, top);
	}
}

static double getBottomOfSoundAndAnalysisArea (I) {
	iam (TextGridEditor);
	return computeSoundY (me);
}

class_methods (TextGridEditor, FunctionEditor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createMenus)
	class_method (prepareDraw)
	class_method (draw)
	us -> hasText = TRUE;
	class_method (click)
	class_method (clickB)
	class_method (clickE)
	/*class_method (key)*/   /* The key method will never be called, because the text widget receives the key presses. */
	class_method (play)
	class_method (updateText)
	class_method (prefs_addFields)
	class_method (prefs_setValues)
	class_method (prefs_getValues)
	class_method (viewMenuEntries)
	class_method (highlightSelection)
	class_method (unhighlightSelection)
	class_method (getBottomOfSoundAndAnalysisArea)
class_methods_end

TextGridEditor TextGridEditor_create (Widget parent, const char *title, TextGrid grid, Any sound, Any spellingChecker) {
	TextGridEditor me = new (TextGridEditor); cherror

	/*
	 * Include a deep copy of the Sound, owned by the TextGridEditor, or a pointer to the LongSound.
	 */
	if (sound) {
		if (((Data) sound) -> methods == (Data_Table) classSound) {
			my sound.data = Data_copy (sound); cherror
		} else {
			my longSound.data = sound;
		}
	}
	my spellingChecker = spellingChecker;

	my useTextStyles = preferences.useTextStyles;
	my fontSize = preferences.fontSize;
	my alignment = preferences.alignment;
	my shiftDragMultiple = preferences.shiftDragMultiple;
	my showNumberOf = preferences.showNumberOf;
	my greenMethod = preferences.greenMethod;
	strcpy (my greenString, preferences.greenString);
	my selectedTier = 1;
	FunctionEditor_init (me, parent, title, grid); cherror
	FunctionEditor_Sound_init (me);
	FunctionEditor_SoundAnalysis_init (me);
	if (my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		FunctionEditor_marksChanged (me);
	}
end:
	iferror forget (me);
	return me;
}

/* End of file TextGridEditor.c */
