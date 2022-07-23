/* SoundArea.cpp
 *
 * Copyright (C) 2022 Paul Boersma, 2007 Erez Volk (FLAC support)
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
#include "Sound_and_MixingMatrix.h"
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


#pragma mark - SoundArea info

void structSoundArea :: v1_info () {
	MelderInfo_writeLine (U"Sound scaling strategy: ", kSoundArea_scalingStrategy_getText (our instancePref_scalingStrategy()));
}

#pragma mark - SoundArea drawing

static void SoundArea_drawCursorFunctionValue (SoundArea me, double yWC, conststring32 yWC_string, conststring32 units) {
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
	if (my longSound() && my endWindow() - my startWindow() > my longSound() -> bufferLength) {
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics(), Melder_BLACK);
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my graphics(), 0.5, 0.5,   U"(window longer than ",
				Melder_float (Melder_single (my longSound() -> bufferLength)), U" seconds)");
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_TOP);
		Graphics_text (my graphics(), 0.5, 0.5, U"(zoom in to see the samples)");
		return;
	}
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


#pragma mark - SoundArea tracking

bool SoundArea_mouse (SoundArea me, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		y_fraction = my y_fraction_globalToLocal (y_fraction);
		const integer numberOfChannels = my soundOrLongSound() -> ny;
		if (event -> commandKeyPressed) {
			if (numberOfChannels > 1) {
				const integer numberOfVisibleChannels = Melder_clippedRight (numberOfChannels, 8_integer);
				Melder_assert (numberOfVisibleChannels >= 1);   // for Melder_clipped
				const integer clickedChannel = my channelOffset +
						Melder_clipped (1_integer, Melder_ifloor ((1.0 - y_fraction) * numberOfVisibleChannels + 1), numberOfVisibleChannels);
				const integer firstVisibleChannel = my channelOffset + 1;
				const integer lastVisibleChannel = Melder_clippedRight (my channelOffset + numberOfVisibleChannels, numberOfChannels);
				if (clickedChannel >= firstVisibleChannel && clickedChannel <= lastVisibleChannel) {
					my muteChannels [clickedChannel] = ! my muteChannels [clickedChannel];
					return FunctionEditor_UPDATE_NEEDED;
				}
			}
		} else {
			if (numberOfChannels > 8) {
				if (x_world >= my endWindow() && y_fraction > 0.875 && y_fraction <= 1.000 && my channelOffset > 0) {
					my channelOffset -= 8;
					return FunctionEditor_UPDATE_NEEDED;
				}
				if (x_world >= my endWindow() && y_fraction > 0.000 && y_fraction <= 0.125 && my channelOffset < numberOfChannels - 8) {
					my channelOffset += 8;
					return FunctionEditor_UPDATE_NEEDED;
				}
			}
		}
	}
	return my defaultMouseInWideDataView (event, x_world, y_fraction);
}


#pragma mark - SoundArea playing

void SoundArea_play (SoundArea me, double startTime, double endTime) {
	const integer numberOfChannels = my soundOrLongSound() -> ny;
	integer numberOfMuteChannels = 0;
	Melder_assert (my muteChannels.size == numberOfChannels);
	for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
		if (my muteChannels [ichan])
			numberOfMuteChannels ++;
	const integer numberOfChannelsToPlay = numberOfChannels - numberOfMuteChannels;
	Melder_require (numberOfChannelsToPlay > 0,
		U"Please select at least one channel to play.");
	if (numberOfMuteChannels > 0) {
		autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
		MixingMatrix_muteAndActivateChannels (thee.get(), my muteChannels.get());
		if (my longSound()) {
			autoSound part = LongSound_extractPart (my longSound(), startTime, endTime, true);
			Sound_MixingMatrix_playPart (part.get(), thee.get(), startTime, endTime, theFunctionEditor_playCallback, my functionEditor());
		} else {
			Sound_MixingMatrix_playPart (my sound(), thee.get(), startTime, endTime, theFunctionEditor_playCallback, my functionEditor());
		}
	} else {
		if (my longSound())
			LongSound_playPart (my longSound(), startTime, endTime, theFunctionEditor_playCallback, my functionEditor());
		else
			Sound_playPart (my sound(), startTime, endTime, theFunctionEditor_playCallback, my functionEditor());
	}
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

static void INFO_DATA__SoundInfo (SoundArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		Thing_info (my sound());
	INFO_DATA_END
}
static void INFO_DATA__LongSoundInfo (SoundArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		Thing_info (my longSound());
	INFO_DATA_END
}
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
	FunctionAreaMenu_addCommand (menu, U"Query sound:", 0, nullptr, me);
	if (Thing_isa (me, classLongSoundArea))
		FunctionAreaMenu_addCommand (menu, U"Info on whole LongSound", 0,
				INFO_DATA__LongSoundInfo, me);
	else
		FunctionAreaMenu_addCommand (menu, U"Info on whole Sound", 0,
				INFO_DATA__SoundInfo, me);
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
	FunctionAreaMenu_addCommand (menu, U"Select sound:", 0, nullptr, me);
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
	FunctionAreaMenu_addCommand (menu, U"Draw sound to picture window:", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Draw visible sound...", 0, menu_cb_DrawVisibleSound, me);
	my drawButton = FunctionAreaMenu_addCommand (menu, U"Draw selected sound...", 0, menu_cb_DrawSelectedSound, me);
}


#pragma mark - SoundArea Extract menu

static autoSound do_ExtractSelectedSound (SoundArea me, bool preserveTimes) {
	if (my endSelection() <= my startSelection())
		Melder_throw (U"No selection.");
	if (my longSound())
		return LongSound_extractPart (my longSound(), my startSelection(), my endSelection(), preserveTimes);
	else if (my sound())
		return Sound_extractPart (my sound(), my startSelection(), my endSelection(),
				kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
	Melder_fatal (U"No Sound or LongSound available.");
	return autoSound();   // never reached
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero (SoundArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		autoSound result = do_ExtractSelectedSound (me, false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes (SoundArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		autoSound result = do_ExtractSelectedSound (me, true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extract selected sound (windowed)", nullptr)
		WORD (name, U"Name", U"slice")
		OPTIONMENU_ENUM (kSound_windowShape, windowShape, U"Window shape", my default_extract_windowShape())
		POSITIVE (relativeWidth, U"Relative width", my default_extract_relativeWidth())
		BOOLEAN (preserveTimes, U"Preserve times", my default_extract_preserveTimes())
	EDITOR_OK
		SET_ENUM (windowShape, kSound_windowShape, my classPref_extract_windowShape())
		SET_REAL (relativeWidth, my classPref_extract_relativeWidth())
		SET_BOOLEAN (preserveTimes, my classPref_extract_preserveTimes())
	EDITOR_DO
		Melder_assert (my sound());   // no LongSound
		CONVERT_DATA_TO_ONE
			my setClassPref_extract_windowShape (windowShape);
			my setClassPref_extract_relativeWidth (relativeWidth);
			my setClassPref_extract_preserveTimes (preserveTimes);
			autoSound result = Sound_extractPart (my sound(), my startSelection(), my endSelection(),
					windowShape, relativeWidth, preserveTimes);
		CONVERT_DATA_TO_ONE_END (name)
	EDITOR_END
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedSoundForOverlap (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extract selected sound for overlap)", nullptr)
		WORD (name, U"Name", U"slice")
		POSITIVE (overlap, U"Overlap (s)", my default_extract_overlap())
	EDITOR_OK
		SET_REAL (overlap, my classPref_extract_overlap())
	EDITOR_DO
		Melder_assert (my sound());   // no LongSound
		CONVERT_DATA_TO_ONE
			my setClassPref_extract_overlap (overlap);
			autoSound result = Sound_extractPartForOverlap (my sound(), my startSelection(), my endSelection(), overlap);
		CONVERT_DATA_TO_ONE_END (name)
	EDITOR_END
}
static void addSoundExtractMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- sound extract --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Extract sound to objects window:", 0, nullptr, me);
	my publishPreserveButton = FunctionAreaMenu_addCommand (menu, U"Extract selected sound (preserve times)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes, me);
		FunctionAreaMenu_addCommand (menu, U"Extract sound selection (preserve times)", Editor_HIDDEN,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes, me);
		FunctionAreaMenu_addCommand (menu, U"Extract selection (preserve times)", Editor_HIDDEN,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes, me);
	my publishButton = FunctionAreaMenu_addCommand (menu, U"Extract selected sound (time from 0)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero, me);
		FunctionAreaMenu_addCommand (menu, U"Extract sound selection (time from 0)", Editor_HIDDEN,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero, me);
		FunctionAreaMenu_addCommand (menu, U"Extract selection (time from 0)", Editor_HIDDEN,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero, me);
		FunctionAreaMenu_addCommand (menu, U"Extract selection", Editor_HIDDEN,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero, me);
	if (! Thing_isa (me, classLongSoundArea)) {
		my publishWindowButton = FunctionAreaMenu_addCommand (menu, U"Extract selected sound (windowed)...", 0,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed, me);
			FunctionAreaMenu_addCommand (menu, U"Extract windowed sound selection...", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed, me);
			FunctionAreaMenu_addCommand (menu, U"Extract windowed selection...", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed, me);
		my publishOverlapButton = FunctionAreaMenu_addCommand (menu, U"Extract selected sound for overlap...", 0,
				CONVERT_DATA_TO_ONE__ExtractSelectedSoundForOverlap, me);
	}
}


#pragma mark - SoundArea File menu

static void do_write (SoundArea me, MelderFile file, int format, int numberOfBitsPerSamplePoint) {
	if (my startSelection() >= my endSelection())
		Melder_throw (U"No samples selected.");
	if (my longSound()) {
		LongSound_savePartAsAudioFile (my longSound(), format, my startSelection(), my endSelection(),
				file, numberOfBitsPerSamplePoint);
	} else if (my sound()) {
		integer first, last;
		const integer numberOfSamples = Sampled_getWindowSamples (my sound(),
				my startSelection(), my endSelection(), & first, & last);
		if (numberOfSamples) {
			autoSound save = Sound_create (my sound() -> ny, 0.0,
					numberOfSamples * my sound() -> dx, numberOfSamples, my sound() -> dx, 0.5 * my sound() -> dx);
			const integer offset = first - 1;
			for (integer channel = 1; channel <= my sound() -> ny; channel ++)
				for (integer i = first; i <= last; i ++)
					save -> z [channel] [i - offset] = my sound() -> z [channel] [i];
			Sound_saveAsAudioFile (save.get(), file, format, numberOfBitsPerSamplePoint);
		}
	}
}
static void menu_cb_WriteWav (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as WAV file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".wav");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_WAV, 16);
	EDITOR_END
}
static void menu_cb_SaveAs24BitWav (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as 24-bit WAV file", nullptr)
		Melder_assert (! my longSound() && my sound());
		Melder_sprint (defaultName,300, my sound() -> name.get(), U".wav");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_WAV, 24);
	EDITOR_END
}
static void menu_cb_SaveAs32BitWav (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as 32-bit WAV file", nullptr)
		Melder_assert (! my longSound() && my sound());
		Melder_sprint (defaultName,300, my sound() -> name.get(), U".wav");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_WAV, 32);
	EDITOR_END
}
static void menu_cb_WriteAiff (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as AIFF file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".aiff");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_AIFF, 16);
	EDITOR_END
}
static void menu_cb_WriteAifc (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as AIFC file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".aifc");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_AIFC, 16);
	EDITOR_END
}
static void menu_cb_WriteNextSun (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as NeXT/Sun file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".au");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_NEXT_SUN, 16);
	EDITOR_END
}
static void menu_cb_WriteNist (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as NIST file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".nist");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_NIST, 16);
	EDITOR_END
}
static void menu_cb_WriteFlac (SoundArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as FLAC file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".flac");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_FLAC, 16);
	EDITOR_END
}
static void addSoundSaveMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- sound file --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Save sound to disk:", 0, nullptr, me);
	my writeWavButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as WAV file...", 0, menu_cb_WriteWav, me);
		FunctionAreaMenu_addCommand (menu, U"Write selected sound to WAV file...", Editor_HIDDEN, menu_cb_WriteWav, me);
		FunctionAreaMenu_addCommand (menu, U"Write sound selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav, me);
		FunctionAreaMenu_addCommand (menu, U"Write selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav, me);
	if (! Thing_isa (me, classLongSoundArea)) {   // BUG: why not for LongSound?
		my saveAs24BitWavButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as 24-bit WAV file...", 0, menu_cb_SaveAs24BitWav, me);
		my saveAs32BitWavButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as 32-bit WAV file...", 0, menu_cb_SaveAs32BitWav, me);
	}
	my writeAiffButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as AIFF file...", 0, menu_cb_WriteAiff, me);
		FunctionAreaMenu_addCommand (menu, U"Write selected sound to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff, me);
		FunctionAreaMenu_addCommand (menu, U"Write sound selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff, me);
		FunctionAreaMenu_addCommand (menu, U"Write selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff, me);
	my writeAifcButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as AIFC file...", 0, menu_cb_WriteAifc, me);
		FunctionAreaMenu_addCommand (menu, U"Write selected sound to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc, me);
		FunctionAreaMenu_addCommand (menu, U"Write sound selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc, me);
		FunctionAreaMenu_addCommand (menu, U"Write selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc, me);
	my writeNextSunButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as NeXT/Sun file...", 0, menu_cb_WriteNextSun, me);
		FunctionAreaMenu_addCommand (menu, U"Write selected sound to NeXT/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun, me);
		FunctionAreaMenu_addCommand (menu, U"Write sound selection to NeXT/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun, me);
		FunctionAreaMenu_addCommand (menu, U"Write selection to NeXT/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun, me);
	my writeNistButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as NIST file...", 0, menu_cb_WriteNist, me);
		FunctionAreaMenu_addCommand (menu, U"Write selected sound to NIST file...", Editor_HIDDEN, menu_cb_WriteNist, me);
		FunctionAreaMenu_addCommand (menu, U"Write sound selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist, me);
		FunctionAreaMenu_addCommand (menu, U"Write selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist, me);
	my writeFlacButton = FunctionAreaMenu_addCommand (menu, U"Save selected sound as FLAC file...", 0, menu_cb_WriteFlac, me);
		FunctionAreaMenu_addCommand (menu, U"Write selected sound to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac, me);
		FunctionAreaMenu_addCommand (menu, U"Write sound selection to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac, me);
}


#pragma mark - SoundArea Edit menu

static void menu_cb_Copy (SoundArea me, EDITOR_ARGS_DIRECT) {
	try {
		Sound_clipboard = ( my longSound()
			? LongSound_extractPart (my longSound(), my startSelection(), my endSelection(), false)
			: Sound_extractPart (my sound(), my startSelection(), my endSelection(), kSound_windowShape::RECTANGULAR, 1.0, false)
		);
	} catch (MelderError) {
		Melder_throw (U"Sound selection not copied to clipboard.");
	}
}
static void menu_cb_Cut (SoundArea me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	try {
		integer first, last;
		const integer selectionNumberOfSamples = Sampled_getWindowSamples (my sound(),
				my startSelection(), my endSelection(), & first, & last);
		const integer oldNumberOfSamples = my sound() -> nx;
		const integer newNumberOfSamples = oldNumberOfSamples - selectionNumberOfSamples;
		if (newNumberOfSamples < 1)
			Melder_throw (U"You cannot cut all of the signal away,\n"
				U"because you cannot create a Sound with 0 samples.\n"
				U"You could consider using Copy instead."
			);

		if (selectionNumberOfSamples > 0) {
			/*
				Create without change.
			*/
			autoSound publish = Sound_create (my sound() -> ny,
				0.0, selectionNumberOfSamples * my sound() -> dx,
				selectionNumberOfSamples, my sound() -> dx, 0.5 * my sound() -> dx
			);
			for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
				integer j = 0;
				for (integer i = first; i <= last; i ++)
					publish -> z [channel] [++ j] = my sound() -> z [channel] [i];
			}
			autoMAT newData = raw_MAT (my sound() -> ny, newNumberOfSamples);
			for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
				integer j = 0;
				for (integer i = 1; i < first; i ++)
					newData [channel] [++ j] = my sound() -> z [channel] [i];
				for (integer i = last + 1; i <= oldNumberOfSamples; i ++)
					newData [channel] [++ j] = my sound() -> z [channel] [i];
				Melder_assert (j == newData.ncol);
			}
			FunctionArea_save (me, U"Cut");
			/*
				Change without error.
			*/
			my sound() -> xmin = 0.0;
			my sound() -> xmax = newNumberOfSamples * my sound() -> dx;
			my sound() -> nx = newNumberOfSamples;
			my sound() -> x1 = 0.5 * my sound() -> dx;
			my sound() -> z = newData.move();
			Sound_clipboard = publish.move();

			/*
				Start updating the markers of the FunctionEditor, respecting the invariants.
			*/
			my functionEditor() -> tmin = my sound() -> xmin;
			my functionEditor() -> tmax = my sound() -> xmax;

			/*
				Collapse the selection,
				so that the Cut operation can immediately be undone by a Paste.
				The exact position will be half-way in between two samples.
			*/
			const double cursor = my sound() -> xmin + (first - 1) * my sound() -> dx;
			my setSelection (cursor, cursor);

			/*
				Update the window.
			*/
			{
				const double t1 = (first - 1) * my sound() -> dx;
				const double t2 = last * my sound() -> dx;
				const double windowLength = my endWindow() - my startWindow();   // > 0
				if (t1 > my startWindow())
					if (t2 < my endWindow())
						my functionEditor() -> startWindow -= 0.5 * (t2 - t1);
					else
						(void) 0;
				else if (t2 < my endWindow())
					my functionEditor() -> startWindow -= t2 - t1;
				else   /* Cut overlaps entire window: centre. */
					my functionEditor() -> startWindow = my functionEditor() -> startSelection - 0.5 * windowLength;
				my functionEditor() -> endWindow = my startWindow() + windowLength;   // first try
				if (my endWindow() > my tmax()) {
					my functionEditor() -> startWindow -= my endWindow() - my tmax();   // second try
					Melder_clipLeft (my tmin(), & my functionEditor() -> startWindow);   // third try
					my functionEditor() -> endWindow = my tmax();   // second try
				} else if (my startWindow() < my tmin()) {
					my functionEditor() -> endWindow -= my startWindow() - my tmin();   // second try
					Melder_clipRight (& my functionEditor() -> endWindow, my tmax());   // third try
					my functionEditor() -> startWindow = my tmin();   // second try
				}
			}

			/*
				Force FunctionEditor to show changes.
			*/
			FunctionEditor_ungroup (my sound());
			FunctionEditor_marksChanged (my functionEditor(), false);
			FunctionArea_broadcastDataChanged (me);
		} else {
			Melder_warning (U"No samples selected.");
		}
	} catch (MelderError) {
		Melder_throw (U"Sound selection not cut to clipboard.");
	}
}
static void menu_cb_Paste (SoundArea me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	integer leftSample = Sampled_xToLowIndex (my sound(), my endSelection());
	const integer oldNumberOfSamples = my sound() -> nx;
	if (! Sound_clipboard) {
		Melder_warning (U"Clipboard is empty; nothing pasted.");
		return;
	}
	Melder_require (Sound_clipboard -> ny == my sound() -> ny,
		U"Cannot paste, because\n"
		U"the number of channels of the clipboard is not equal to\n"
		U"the number of channels of the edited sound."
	);
	Melder_require (Sound_clipboard -> dx == my sound() -> dx,
		U"Cannot paste, because\n"
		U"the sampling frequency of the clipboard is not equal to\n"
		U"the sampling frequency of the edited sound."
	);
	Melder_clip (0_integer, & leftSample, oldNumberOfSamples);
	const integer newNumberOfSamples = oldNumberOfSamples + Sound_clipboard -> nx;
	/*
		Check without change.
	*/
	autoMAT newData = raw_MAT (my sound() -> ny, newNumberOfSamples);
	for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
		integer j = 0;
		for (integer i = 1; i <= leftSample; i ++)
			newData [channel] [++ j] = my sound() -> z [channel] [i];
		for (integer i = 1; i <= Sound_clipboard -> nx; i ++)
			newData [channel] [++ j] = Sound_clipboard -> z [channel] [i];
		for (integer i = leftSample + 1; i <= oldNumberOfSamples; i ++)
			newData [channel] [++ j] = my sound() -> z [channel] [i];
		Melder_assert (j == newData.ncol);
	}
	FunctionArea_save (me, U"Paste");
	/*
		Change without error.
	*/
	my sound() -> xmin = 0.0;
	my sound() -> xmax = newNumberOfSamples * my sound() -> dx;
	my sound() -> nx = newNumberOfSamples;
	my sound() -> x1 = 0.5 * my sound() -> dx;
	my sound() -> z = newData.move();

	/*
		Start updating the markers of the FunctionEditor, respecting the invariants.
	*/
	my functionEditor() -> tmin = my sound() -> xmin;
	my functionEditor() -> tmax = my sound() -> xmax;
	Melder_clipLeft (my tmin(), & my functionEditor() -> startWindow);
	Melder_clipRight (& my functionEditor() -> endWindow, my tmax());
	my setSelection (leftSample * my sound() -> dx, (leftSample + Sound_clipboard -> nx) * my sound() -> dx);

	/*
		Force FunctionEditor to show changes.
	*/
	FunctionEditor_ungroup (my sound());
	FunctionEditor_marksChanged (my functionEditor(), false);
	FunctionArea_broadcastDataChanged (me);
}
static void addSoundEditMenu (SoundArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- cut copy paste --", 0, nullptr, me);
	if (my editable())
		my cutButton = FunctionAreaMenu_addCommand (menu, U"Cut", 'X', menu_cb_Cut, me);
	my copyButton = FunctionAreaMenu_addCommand (menu, U"Copy selection to Sound clipboard", 'C', menu_cb_Copy, me);
	if (my editable())
		my pasteButton = FunctionAreaMenu_addCommand (menu, U"Paste after selection", 'V', menu_cb_Paste, me);
}


#pragma mark - SoundArea Modify menu

static void menu_cb_SetSelectionToZero (SoundArea me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	integer first, last;
	Sampled_getWindowSamples (my sound(), my startSelection(), my endSelection(), & first, & last);
	FunctionArea_save (me, U"Set to zero");
	my sound() -> z.verticalBand (first, last)  <<=  0.0;
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_ReverseSelection (SoundArea me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	FunctionArea_save (me, U"Reverse selection");
	Sound_reverse (my sound(), my startSelection(), my endSelection());
	FunctionArea_broadcastDataChanged (me);
}
static void addSoundModifyMenu (SoundArea me, EditorMenu menu) {
	if (my editable()) {
		FunctionAreaMenu_addCommand (menu, U"-- sound modify --", 0, nullptr, me);
		FunctionAreaMenu_addCommand (menu, U"Modify sound:", 0, nullptr, me);
		my zeroButton = FunctionAreaMenu_addCommand (menu, U"Set selection to zero", 0, menu_cb_SetSelectionToZero, me);
		my reverseButton = FunctionAreaMenu_addCommand (menu, U"Reverse selection", 'R', menu_cb_ReverseSelection, me);
	}
}

#pragma mark - SoundArea all menus

void structSoundArea :: v_createMenus () {
	EditorMenu soundMenu = Editor_addMenu (our functionEditor(), U"Sound", 0);
	addSoundSettingsMenu (this, soundMenu);
	addSoundModifyMenu (this, soundMenu);
	addSoundModifyMenu (this, our functionEditor() -> editMenu);
	if (! Thing_isa (this, classLongSoundArea)) {
		addSoundQueryMenu (this, soundMenu);
		addSoundQueryMenu (this, our functionEditor() -> queryMenu);
	}
	if (! Thing_isa (this, classLongSoundArea)) {
		addSoundSelectMenu (this, soundMenu);
		addSoundSelectMenu (this, our functionEditor() -> selectMenu);
	}
	addSoundDrawMenu (this, soundMenu);
	addSoundDrawMenu (this, our functionEditor() -> drawMenu);
	addSoundExtractMenu (this, soundMenu);
	addSoundExtractMenu (this, our functionEditor() -> extractMenu);
}
void structSoundArea :: v_createMenuItems_file (EditorMenu menu) {
	addSoundSaveMenu (this, menu);
}
void structSoundArea :: v_createMenuItems_edit (EditorMenu menu) {
	addSoundEditMenu (this, menu);
}
void structSoundArea :: v_updateMenuItems () {
	integer first, last;
	const integer selectedSamples = Sampled_getWindowSamples (our soundOrLongSound(),
			our startSelection(), our endSelection(), & first, & last);
	if (our drawButton) {
		GuiThing_setSensitive (our drawButton, selectedSamples != 0);
		GuiThing_setSensitive (our publishButton, selectedSamples != 0);
		GuiThing_setSensitive (our publishPreserveButton, selectedSamples != 0);
		if (our publishWindowButton)
			GuiThing_setSensitive (our publishWindowButton, selectedSamples != 0);
		if (our publishOverlapButton)
			GuiThing_setSensitive (our publishOverlapButton, selectedSamples != 0);
	}
	GuiThing_setSensitive (our writeWavButton, selectedSamples != 0);
	if (our saveAs24BitWavButton)
		GuiThing_setSensitive (our saveAs24BitWavButton, selectedSamples != 0);
	if (our saveAs32BitWavButton)
		GuiThing_setSensitive (our saveAs32BitWavButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeAiffButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeAifcButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeNextSunButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeNistButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeFlacButton, selectedSamples != 0);

	if (our sound()) {
		if (our editable())
			GuiThing_setSensitive (cutButton, selectedSamples != 0 && selectedSamples < our sound() -> nx);
		GuiThing_setSensitive (copyButton, selectedSamples != 0);
		if (our editable()) {
			GuiThing_setSensitive (zeroButton, selectedSamples != 0);
			GuiThing_setSensitive (reverseButton, selectedSamples != 0);
		}
	}
}

/* End of file SoundArea.cpp */
