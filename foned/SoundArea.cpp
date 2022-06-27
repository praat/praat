/* SoundArea.cpp
 *
 * Copyright (C) 2022 Paul Boersma
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

#include "SoundArea.h"
#include "LongSoundArea.h"
#include "../kar/UnicodeData.h"
#include "EditorM.h"

Thing_implement (SoundArea, FunctionArea, 0);
Thing_implement (LongSoundArea, FunctionArea, 0);

#include "enums_getText.h"
#include "SoundArea_enums.h"
#include "enums_getValue.h"
#include "SoundArea_enums.h"

#include "Prefs_define.h"
#include "SoundArea_prefs.h"
#include "Prefs_install.h"
#include "SoundArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "SoundArea_prefs.h"

void SoundArea_drawCursorFunctionValue (SoundArea me, double yWC, conststring32 yWC_string, conststring32 units) {
	Graphics_setColour (my graphics(), Melder_CYAN);
	Graphics_line (my graphics(), my startWindow(), yWC, 0.99 * my startWindow() + 0.01 * my endWindow(), yWC);
	Graphics_fillCircle_mm (my graphics(), 0.5 * (my startSelection() + my endSelection()), yWC, 1.5);
	Graphics_setColour (my graphics(), Melder_BLUE);
	Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics(), my startWindow(), yWC,   yWC_string, units);
}

void structSoundArea :: v_drawInside () {
	SoundArea_draw (this);
}

void SoundArea_draw (SoundArea me) {
	Melder_assert (!! my sound() != !! my longSound());

	if (my longSound()) {
		try {
			LongSound_haveWindow (my longSound(), my startWindow(), my endWindow());
		} catch (MelderError) {
			Melder_clearError ();
		}
	}

	const integer numberOfChannels = my soundOrLongSound() -> ny;
	const bool cursorVisible = ( my startSelection() == my endSelection() &&
			my startSelection() >= my startWindow() && my startSelection() <= my endWindow() );
	Graphics_setColour (my graphics(), Melder_BLACK);
	bool fits;
	try {
		fits = ( my sound() ? true : LongSound_haveWindow (my longSound(), my startWindow(), my endWindow()) );
	} catch (MelderError) {
		const bool outOfMemory = !! str32str (Melder_getError (), U"memory");
		if (Melder_debug == 9)
			Melder_flushError ();
		else
			Melder_clearError ();
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics(), 0.5, 0.5, outOfMemory ? U"(out of memory)" : U"(cannot read sound file)");
		return;
	}
	if (! fits) {
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics(), 0.5, 0.5, U"(window too large; zoom in to see the data)");
		return;
	}
	integer first, last;
	if (Sampled_getWindowSamples (my soundOrLongSound(),
		my startWindow(), my endWindow(), & first, & last) <= 1)
	{
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics(), 0.5, 0.5, U"(zoom out to see the data)");
		return;
	}
	const integer numberOfVisibleChannels = Melder_clippedRight (numberOfChannels, 8_integer);
	const integer firstVisibleChannel = my channelOffset + 1;
	const integer lastVisibleChannel = Melder_clippedRight (my channelOffset + numberOfVisibleChannels, numberOfChannels);
	double maximumExtent = 0.0, visibleMinimum = 0.0, visibleMaximum = 0.0;
	if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WINDOW) {
		if (my longSound())
			LongSound_getWindowExtrema (my longSound(), my startWindow(), my endWindow(), firstVisibleChannel,
					& visibleMinimum, & visibleMaximum);
		else
			Matrix_getWindowExtrema (my sound(), first, last, firstVisibleChannel, firstVisibleChannel,
					& visibleMinimum, & visibleMaximum);
		for (integer ichan = firstVisibleChannel + 1; ichan <= lastVisibleChannel; ichan ++) {
			double visibleChannelMinimum, visibleChannelMaximum;
			if (my longSound())
				LongSound_getWindowExtrema (my longSound(), my startWindow(), my endWindow(), ichan,
						& visibleChannelMinimum, & visibleChannelMaximum);
			else
				Matrix_getWindowExtrema (my sound(), first, last, ichan, ichan, & visibleChannelMinimum, & visibleChannelMaximum);
			if (visibleChannelMinimum < visibleMinimum)
				visibleMinimum = visibleChannelMinimum;
			if (visibleChannelMaximum > visibleMaximum)
				visibleMaximum = visibleChannelMaximum;
		}
		maximumExtent = visibleMaximum - visibleMinimum;
	}
	for (integer ichan = firstVisibleChannel; ichan <= lastVisibleChannel; ichan ++) {
		const double cursorFunctionValue = ( my longSound() ? 0.0 :
				Vector_getValueAtX (my sound(), 0.5 * (my startSelection() + my endSelection()), ichan, kVector_valueInterpolation :: SINC70) );
		const double ymin = (double) (numberOfVisibleChannels - ichan + my channelOffset) / numberOfVisibleChannels;
		const double ymax = (double) (numberOfVisibleChannels + 1 - ichan + my channelOffset) / numberOfVisibleChannels;
		Graphics_Viewport vp = Graphics_insetViewport (my graphics(), 0.0, 1.0, ymin, ymax);
		bool horizontal = false;
		double minimum = -1.0, maximum = +1.0;
		if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WHOLE) {
			my getGlobalExtrema (& minimum, & maximum);
		} else if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WINDOW) {
			if (numberOfChannels > 2) {
				if (my longSound())
					LongSound_getWindowExtrema (my longSound(), my startWindow(), my endWindow(), ichan, & minimum, & maximum);
				else
					Matrix_getWindowExtrema (my sound(), first, last, ichan, ichan, & minimum, & maximum);
				if (maximumExtent > 0.0) {
					const double middle = 0.5 * (minimum + maximum);
					minimum = middle - 0.5 * maximumExtent;
					maximum = middle + 0.5 * maximumExtent;
				}
			} else {
				minimum = visibleMinimum;
				maximum = visibleMaximum;
			}
		} else if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WINDOW_AND_CHANNEL) {
			if (my longSound())
				LongSound_getWindowExtrema (my longSound(), my startWindow(), my endWindow(), ichan, & minimum, & maximum);
			else
				Matrix_getWindowExtrema (my sound(), first, last, ichan, ichan, & minimum, & maximum);
		} else if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::FIXED_HEIGHT) {
			if (my longSound())
				LongSound_getWindowExtrema (my longSound(), my startWindow(), my endWindow(), ichan, & minimum, & maximum);
			else
				Matrix_getWindowExtrema (my sound(), first, last, ichan, ichan, & minimum, & maximum);
			const double channelExtent = my instancePref_scaling_height();
			const double middle = 0.5 * (minimum + maximum);
			minimum = middle - 0.5 * channelExtent;
			maximum = middle + 0.5 * channelExtent;
		} else if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::FIXED_RANGE) {
			minimum = my instancePref_scaling_minimum();
			maximum = my instancePref_scaling_maximum();
		}
		if (minimum == maximum) {
			horizontal = true;
			minimum -= 1.0;
			maximum += 1.0;
		}
		Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), minimum, maximum);
		if (horizontal) {
			Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
			const double mid = 0.5 * (minimum + maximum);
			Graphics_text (my graphics(), my startWindow(), mid, Melder_float (Melder_half (mid)));
		} else {
			if (! cursorVisible || isundef (cursorFunctionValue) || Graphics_dyWCtoMM (my graphics(), cursorFunctionValue - minimum) > 5.0) {
				Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
				Graphics_text (my graphics(), my startWindow(), minimum, Melder_float (Melder_half (minimum)));
			}
			if (! cursorVisible || isundef (cursorFunctionValue) || Graphics_dyWCtoMM (my graphics(), maximum - cursorFunctionValue) > 5.0) {
				Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
				Graphics_text (my graphics(), my startWindow(), maximum, Melder_float (Melder_half (maximum)));
			}
		}
		if (minimum < 0.0 && maximum > 0.0 && ! horizontal) {
			Graphics_setWindow (my graphics(), 0.0, 1.0, minimum, maximum);
			if (! cursorVisible || isundef (cursorFunctionValue) || fabs (Graphics_dyWCtoMM (my graphics(), cursorFunctionValue - 0.0)) > 3.0) {
				Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
				Graphics_text (my graphics(), 0.0, 0.0, U"0");
			}
			Graphics_setColour (my graphics(), Melder_CYAN);
			Graphics_setLineType (my graphics(), Graphics_DOTTED);
			Graphics_line (my graphics(), 0.0, 0.0, 1.0, 0.0);
			Graphics_setLineType (my graphics(), Graphics_DRAWN);
		}
		/*
			Garnish the drawing area of each channel.
		*/
		Graphics_setColour (my graphics(), Melder_BLACK);
		if (numberOfChannels > 1) {
			Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
			Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
			conststring32 channelName = my v_getChannelName (ichan);
			static MelderString channelLabel;
			MelderString_copy (& channelLabel, ( channelName ? U"ch" : U"Ch " ), ichan);
			if (channelName)
				MelderString_append (& channelLabel, U": ", channelName);
			MelderString_append (& channelLabel, U" ",
					( my muteChannels [ichan] ? UNITEXT_SPEAKER_WITH_CANCELLATION_STROKE : UNITEXT_SPEAKER ));
			if (ichan > 8 && ichan - my channelOffset == 1)
				MelderString_append (& channelLabel, U"      " UNITEXT_UPWARDS_ARROW);
			else if (numberOfChannels >= 8 && ichan - my channelOffset == 8 && ichan < numberOfChannels)
				MelderString_append (& channelLabel, U"      " UNITEXT_DOWNWARDS_ARROW);
			Graphics_text (my graphics(), 1.0, 0.5, channelLabel.string);
		}
		/*
			Draw a very thin separator line underneath.
		*/
		if (ichan < numberOfChannels) {
			/*Graphics_setColour (my graphics.get(), Melder_BLACK);*/
			Graphics_line (my graphics(), 0.0, 0.0, 1.0, 0.0);
		}
		/*
			Draw the samples.
		*/
		/*if (ichan == 1) FunctionEditor_SoundAnalysis_drawPulses (this);*/
		if (my sound()) {
			Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), minimum, maximum);
			if (cursorVisible && isdefined (cursorFunctionValue))
				SoundArea_drawCursorFunctionValue (me, cursorFunctionValue, Melder_float (Melder_half (cursorFunctionValue)), U"");
			Graphics_setColour (my graphics(), Melder_BLACK);
			Graphics_function (my graphics(), & my sound() -> z [ichan] [0], first, last,
					Sampled_indexToX (my sound(), first), Sampled_indexToX (my sound(), last));
		} else {
			Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), minimum * 32768, maximum * 32768);
			Graphics_function16 (my graphics(),
				my longSound() -> buffer.asArgumentToFunctionThatExpectsZeroBasedArray() - my longSound() -> imin * numberOfChannels + (ichan - 1),
				numberOfChannels, first, last, Sampled_indexToX (my longSound(), first), Sampled_indexToX (my longSound(), last)
			);
		}
		Graphics_resetViewport (my graphics(), vp);
	}
}

bool SoundArea_mouse (SoundArea me, Sound sound, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	return FunctionEditor_UPDATE_NEEDED;
}

#pragma mark - SoundArea Settings menu

static void menu_cb_soundScaling (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Sound scaling", nullptr)
		OPTIONMENU_ENUM (kSoundArea_scalingStrategy, scalingStrategy,
				U"Scaling strategy", my default_scalingStrategy())
		LABEL (U"For \"fixed height\":")
		POSITIVE (height, U"Height", my default_scaling_height())
		LABEL (U"For \"fixed range\":")
		REAL (minimum, U"Minimum", my default_scaling_minimum())
		REAL (maximum, U"Maximum", my default_scaling_maximum())
	EDITOR_OK
		SET_ENUM (scalingStrategy, kSoundArea_scalingStrategy, my instancePref_scalingStrategy())
		SET_REAL (height,  my instancePref_scaling_height())
		SET_REAL (minimum, my instancePref_scaling_minimum())
		SET_REAL (maximum, my instancePref_scaling_maximum())
	EDITOR_DO
		my setInstancePref_scalingStrategy (scalingStrategy) ;
		my setInstancePref_scaling_height (height);
		my setInstancePref_scaling_minimum (minimum);
		my setInstancePref_scaling_maximum (maximum);
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}
static void menu_cb_soundMuteChannels (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Mute channels", nullptr)
		NATURALVECTOR (channels, U"Channels to mute", WHITESPACE_SEPARATED_, U"2")
	EDITOR_OK
	EDITOR_DO
		const integer numberOfChannels = my soundOrLongSound() -> ny;
		Melder_assert (my muteChannels.size == numberOfChannels);
		for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
			my muteChannels [ichan] = false;
		for (integer ichan = 1; ichan <= channels.size; ichan ++)
			if (channels [ichan] >= 1 && channels [ichan] <= numberOfChannels)
				my muteChannels [channels [ichan]] = true;
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}
static void addSoundSettingsMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"Sound scaling...", 0, menu_cb_soundScaling, me);
	FunctionAreaMenu_addCommand (menu, U"Mute channels...", 0, menu_cb_soundMuteChannels, me);
}


#pragma mark - SoundArea Query menu

enum {
	TimeSoundEditor_PART_CURSOR = 1,
	TimeSoundEditor_PART_SELECTION = 2
};
static int makeQueriable (SoundArea me, bool allowCursor, double *tmin, double *tmax) {
	if (my startSelection() == my endSelection()) {
		if (allowCursor) {
			*tmin = *tmax = my startSelection();
			return TimeSoundEditor_PART_CURSOR;
		} else {
			Melder_throw (U"Make a selection first.");
		}
	} else if (my startSelection() < my startWindow() || my endSelection() > my endWindow()) {
		Melder_throw (U"Command ambiguous: a part of the selection (", my startSelection(), U", ", my endSelection(), U") "
			U"is outside of the window (", my startWindow(), U", ", my endWindow(), U"). "
			U"Either zoom or re-select.");
	}
	*tmin = my startSelection();
	*tmax = my endSelection();
	return TimeSoundEditor_PART_SELECTION;
}
static void INFO_DATA__getAmplitudes (SoundArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		if (! my sound())
			Melder_throw (U"No Sound object is visible (a LongSound cannot be queried).");
		MelderInfo_open ();
		if (part == TimeSoundEditor_PART_CURSOR)
			for (integer ichan = 1; ichan <= my sound() -> ny; ichan ++)
				MelderInfo_writeLine (Vector_getValueAtX (my sound(), 0.5 * (my startSelection() + my endSelection()), ichan, kVector_valueInterpolation :: SINC70),
						U" (interpolated amplitude at CURSOR in channel ", ichan, U")");
		else
			for (integer ichan = 1; ichan <= my sound() -> ny; ichan ++)
				MelderInfo_writeLine (Sampled_getMean (my sound(), my startSelection(), my endSelection(), ichan, 0, true),
						U" (mean amplitude in SELECTION in channel ", ichan, U")");
		MelderInfo_close ();
	INFO_DATA_END
}
static void addSoundQueryMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- sound query --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Query selected sound:", GuiMenu_INSENSITIVE,
			INFO_DATA__getAmplitudes /* dummy */, me);
	FunctionAreaMenu_addCommand (menu, U"Get amplitude(s)", 0,
			INFO_DATA__getAmplitudes, me);
}


#pragma mark - SoundArea Select menu

static void menu_cb_MoveStartOfSelectionToNearestZeroCrossing (SoundArea me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing (my sound(), my startSelection(), 1);   // STEREO BUG
	if (isdefined (zero)) {
		my setSelection (zero, my endSelection());
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
	}
}
static void menu_cb_MoveCursorToNearestZeroCrossing (SoundArea me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing (my sound(), 0.5 * (my startSelection() + my endSelection()), 1);   // STEREO BUG
	if (isdefined (zero)) {
		my setSelection (zero, zero);
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
	}
}
static void menu_cb_MoveEndOfSelectionToNearestZeroCrossing (SoundArea me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing (my sound(), my endSelection(), 1);   // STEREO BUG
	if (isdefined (zero)) {
		my setSelection (my startSelection(), zero);
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
	}
}
static void addSoundSelectMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- sound select --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Select:", GuiMenu_INSENSITIVE, menu_cb_MoveStartOfSelectionToNearestZeroCrossing /* dummy */, me);
	FunctionAreaMenu_addCommand (menu, U"Move start of selection to nearest zero crossing", ',',
			menu_cb_MoveStartOfSelectionToNearestZeroCrossing, me);
	FunctionAreaMenu_addCommand (menu, U"Move begin of selection to nearest zero crossing", Editor_HIDDEN,
			menu_cb_MoveStartOfSelectionToNearestZeroCrossing, me);
	FunctionAreaMenu_addCommand (menu, U"Move cursor to nearest zero crossing", '0',
			menu_cb_MoveCursorToNearestZeroCrossing, me);
	FunctionAreaMenu_addCommand (menu, U"Move end of selection to nearest zero crossing", '.',
			menu_cb_MoveEndOfSelectionToNearestZeroCrossing, me);
}


#pragma mark - SoundArea Draw menu

static void menu_cb_DrawVisibleSound (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible sound", nullptr)
		my v_form_pictureWindow (cmd);
		LABEL (U"Sound:")
		BOOLEAN (preserveTimes, U"Preserve times", my default_picture_preserveTimes());
		REAL (bottom, U"left Vertical range", my default_picture_bottom())
		REAL (top, U"right Vertical range", my default_picture_top())
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (preserveTimes, my classPref_picture_preserveTimes())
		SET_REAL (bottom,  my classPref_picture_bottom())
		SET_REAL (top,     my classPref_picture_top())
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my setClassPref_picture_preserveTimes (preserveTimes);
		my setClassPref_picture_bottom (bottom);
		my setClassPref_picture_top (top);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		if (! my soundOrLongSound())
			Melder_throw (U"There is no sound to draw.");
		autoSound publish = my longSound() ?
			LongSound_extractPart (my longSound(), my startWindow(), my endWindow(), preserveTimes) :
			Sound_extractPart (my sound(), my startWindow(), my endWindow(), kSound_windowShape::RECTANGULAR, 1.0, preserveTimes
		);
		Editor_openPraatPicture (my functionEditor());
		Sound_draw (publish.get(), my functionEditor() -> pictureGraphics, 0.0, 0.0, bottom, top, garnish, U"Curve");
		FunctionEditor_garnish (my functionEditor());
		Editor_closePraatPicture (my functionEditor());
	EDITOR_END
}
static void menu_cb_DrawSelectedSound (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw selected sound", nullptr)
		my v_form_pictureWindow (cmd);
		LABEL (U"Sound:")
		BOOLEAN (preserveTimes, U"Preserve times",       my default_picture_preserveTimes());
		REAL    (bottom,        U"left Vertical range",  my default_picture_bottom());
		REAL    (top,           U"right Vertical range", my default_picture_top());
		my v_form_pictureMargins (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (preserveTimes, my classPref_picture_preserveTimes());
		SET_REAL (bottom, my classPref_picture_bottom());
		SET_REAL (top,    my classPref_picture_top());
		my v_ok_pictureMargins (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my setClassPref_picture_preserveTimes (preserveTimes);
		my setClassPref_picture_bottom (bottom);
		my setClassPref_picture_top (top);
		my v_do_pictureMargins (cmd);
		my setClassPref_picture_garnish (garnish);
		if (! my soundOrLongSound())
			Melder_throw (U"There is no sound to draw.");
		autoSound publish = my longSound() ?
			LongSound_extractPart (my longSound(), my startSelection(), my endSelection(), preserveTimes) :
			Sound_extractPart (my sound(), my startSelection(), my endSelection(),
					kSound_windowShape::RECTANGULAR, 1.0, preserveTimes
		);
		Editor_openPraatPicture (my functionEditor());
		Sound_draw (publish.get(), my functionEditor() -> pictureGraphics, 0.0, 0.0, bottom, top, garnish, U"Curve");
		Editor_closePraatPicture (my functionEditor());
	EDITOR_END
}
static void addSoundDrawMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- sound draw --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_DrawVisibleSound /* dummy */, me);
	FunctionAreaMenu_addCommand (menu, U"Draw visible sound...", 0, menu_cb_DrawVisibleSound, me);
	my drawButton = FunctionAreaMenu_addCommand (menu, U"Draw selected sound...", 0, menu_cb_DrawSelectedSound, me);
}


#pragma mark - SoundArea all menus

void structSoundArea :: v_createMenus () {
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Sound", 0);
	addSoundSettingsMenu (this, menu);
	if (! Thing_isa (this, classLongSoundArea))
		addSoundQueryMenu (this, menu);
	if (! Thing_isa (this, classLongSoundArea))
		addSoundSelectMenu (this, menu);
	addSoundDrawMenu (this, menu);
}

/* End of file SoundArea.cpp */
