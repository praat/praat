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
#include "../kar/UnicodeData.h"

Thing_implement (SoundArea, FunctionArea, 0);

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

void SoundArea_draw (SoundArea me, double globalMinimum, double globalMaximum) {
	Melder_assert (!! my sound() != !! my longSound());
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
		double minimum = ( my sound() ? globalMinimum : -1.0 ), maximum = ( my sound() ? globalMaximum : 1.0 );
		if (my instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WINDOW) {
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
				Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_BOTTOM);
				Graphics_text (my graphics(), my startWindow(), minimum, Melder_float (Melder_half (minimum)));
			}
			if (! cursorVisible || isundef (cursorFunctionValue) || Graphics_dyWCtoMM (my graphics(), maximum - cursorFunctionValue) > 5.0) {
				Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_TOP);
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
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics(), Melder_CYAN);
		Graphics_innerRectangle (my graphics(), 0.0, 1.0, 0.0, 1.0);
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
					numberOfChannels, first, last, Sampled_indexToX (my longSound(), first), Sampled_indexToX (my longSound(), last));
		}
		Graphics_resetViewport (my graphics(), vp);
	}
	Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
	Graphics_rectangle (my graphics(), 0.0, 1.0, 0.0, 1.0);
}

bool SoundArea_mouse (SoundArea me, Sound sound, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	return FunctionEditor_UPDATE_NEEDED;
}

void SoundArea_init (SoundArea me, FunctionEditor editor, SampledXY soundOrLongSound, bool ownSound) {
	FunctionArea_init (me, editor, soundOrLongSound);
	if (my longSound()) {
		my cache. globalMinimum = -1.0;
		my cache. globalMaximum = 1.0;
	} else if (my sound()) {
		if (ownSound)
			my soundCopy = Data_copy (my sound());
		Matrix_getWindowExtrema (my sound(), 1, my sound() -> nx, 1, my sound() -> ny,
				& my cache. globalMinimum, & my cache. globalMaximum);
	} else {
		Melder_fatal (U"Invalid sound class in SoundArea_create().");
	}
	my muteChannels = zero_BOOLVEC (soundOrLongSound -> ny);
}

autoSoundArea SoundArea_create (FunctionEditor editor, SampledXY soundOrLongSound, bool ownSound) {
	autoSoundArea me = Thing_new (SoundArea);
	SoundArea_init (me.get(), editor, soundOrLongSound, ownSound);
	return me;
}

/* End of file SoundArea.cpp */
