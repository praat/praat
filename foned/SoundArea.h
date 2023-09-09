#ifndef _SoundArea_h_
#define _SoundArea_h_
/* SoundArea.h
 *
 * Copyright (C) 2022,2023 Paul Boersma
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

#include "FunctionArea.h"
#include "Sound.h"
#include "LongSound.h"
#include "SoundArea_enums.h"

/*
	One derived data cache, namely for global extrema.
	TODO: add a second cache, namely for channel extrema (or profile the scrolling speed).
*/
struct SoundArea_GlobalExtremaCache {
	void get (Sound sound, LongSound longSound, double *out_minimum, double *out_maximum) {
		if (! _valid) {
			_compute (sound, longSound);
			_valid = true;
		}
		*out_minimum = _minimum;
		*out_maximum = _maximum;
	}
	void invalidate () {
		_valid = false;
	}
private:
	bool _valid;
	double _minimum, _maximum;
	void _compute (Sound sound, LongSound longSound) {
		if (sound) {
			Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & _minimum, & _maximum);
		} else {
			Melder_assert (longSound);
			_minimum = -1.0;
			_maximum = +1.0;
		}
	}
};

Thing_define (SoundArea, FunctionArea) {
	/*
		Accessors.
	*/
	SampledXY soundOrLongSound() const { return static_cast <SampledXY> (our function()); }
	Sound sound() const {
		return our soundOrLongSound() && Thing_isa (our soundOrLongSound(), classSound) ? (Sound) our soundOrLongSound() : nullptr;
	}
	LongSound longSound() const {
		return our soundOrLongSound() && Thing_isa (our soundOrLongSound(), classLongSound) ? (LongSound) our soundOrLongSound() : nullptr;
	}

	/*
		Derived data cache: global extrema.
	*/
private:
	SoundArea_GlobalExtremaCache _globalExtremaCache;
public:
	void getGlobalExtrema (double *out_minimum, double *out_maximum) {
		_globalExtremaCache. get (our sound(), our longSound(), out_minimum, out_maximum);
	}
	/*
		Manage all derived data caches.
	*/
protected:
	void v_invalidateAllDerivedDataCaches () override {
		_globalExtremaCache. invalidate ();
		SoundArea_Parent :: v_invalidateAllDerivedDataCaches ();
	}

	/*
		Auxiliary data: channelOffset.

		For multi-channel sounds, Praat will never show more than 8 channels at a time:
		if there are more than 8 channels, the user can scroll through them, in groups of 8.
		The value of channelOffset is determined as follows:
		- if there are at most 8 channels, channelOffset will always be 0;
		- if there are more than 8 channels, channelOffset will be an integer multiple of 8;
			for instance, if there are 30 channels, channelOffset can have the following values:
			- 0 to show channels 1 throguh 8 (the first 8 channels);
			- 8 to show channels 9 through 16 (the second 8 channels);
			- 16 to show channels 17 through 24 (the third 8 channels);
			- 24 to show channels 25 through 30 (the last 6 channels).
		At initialization, channelOffset stays at zero, i.e. only the first 8 channels are shown.
	*/
public:
	integer channelOffset;
private:
	void _computeChannelOffset () {
		Melder_assert (our soundOrLongSound());
		Melder_assert (our soundOrLongSound() -> ny > 0);
		Melder_clip (0_integer, & our channelOffset, (our soundOrLongSound() -> ny - 1) / 8 * 8);   // works correctly even during initialization (offset will stay 0)
	}
	/*
		Auxiliary data: muteChannels.

		At initialization, no channels are muted.
	*/
public:
	autoBOOLVEC muteChannels;
private:
	void _computeMuteChannels () {
		Melder_assert (our soundOrLongSound() && our soundOrLongSound() -> ny > 0);
		if (our muteChannels.size != our soundOrLongSound() -> ny)   // condition works correctly even during initialization (when size is still 0, but ny is not)
			our muteChannels = zero_BOOLVEC (our soundOrLongSound() -> ny);
	}
	/*
		Maintain all auxiliary data.
	*/
protected:
	void v_computeAuxiliaryData () override {
		SoundArea_Parent :: v_computeAuxiliaryData ();
		_computeChannelOffset ();
		_computeMuteChannels ();
	}

public:
	virtual conststring32 v_getChannelName (integer /* channelNumber */) { return nullptr; }

protected:
	void v_drawInside ()
		override;
public:
	void v1_info ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
		override;
	void v_createMenuItems_save (EditorMenu menu)
		override;
	void v_createMenuItems_edit (EditorMenu menu)
		override;
	void v_createMenus ()
		override;
	void v_updateMenuItems ()
		override;
	GuiMenuItem writeAiffButton, saveAs24BitWavButton, saveAs32BitWavButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton, writeFlacButton;
	GuiMenuItem cutButton, copyButton, pasteBeforeButton, pasteOverButton, pasteAfterButton, zeroButton, reverseButton;
	GuiMenuItem drawButton;
	GuiMenuItem publishButton, publishPreserveButton, publishWindowButton, publishOverlapButton;

	#include "SoundArea_prefs.h"
};

void SoundArea_draw (SoundArea me);

bool SoundArea_mouse (SoundArea me, GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction);

void SoundArea_play (SoundArea me, double startTime, double endTime);

DEFINE_FunctionArea_create (SoundArea, Sound)

/* End of file SoundArea.h */
#endif
