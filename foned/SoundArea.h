#ifndef _SoundArea_h_
#define _SoundArea_h_
/* SoundArea.h
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

#include "FunctionArea.h"
#include "Sound.h"
#include "LongSound.h"
#include "SoundArea_enums.h"

Thing_define (SoundArea, FunctionArea) {
	SampledXY soundOrLongSound() { return static_cast <SampledXY> (our function()); }
	Sound sound() {
		return Thing_isa (our soundOrLongSound(), classSound) ? (Sound) soundOrLongSound() : nullptr;
	}
	LongSound longSound() {
		return Thing_isa (our soundOrLongSound(), classLongSound) ? (LongSound) soundOrLongSound() : nullptr;
	}

	/*
		One derived data cache, namely for global extrema.
		TODO: add a second cache, namely for channel extrema (or profile the scrolling speed).
	*/
	struct {
		bool valid;
		double minimum, maximum;
	} globalExtremaCache;
	void invalidateGlobalExtremaCache () {
		our globalExtremaCache. valid = false;
	}
	void validateGlobalExtremaCache () {
		if (! our globalExtremaCache. valid) {
			if (our sound()) {
				Matrix_getWindowExtrema (our sound(), 1, our sound() -> nx, 1, our sound() -> ny,
						& our globalExtremaCache. minimum, & our globalExtremaCache. maximum);
			} else if (our longSound()) {
				our globalExtremaCache. minimum = -1.0;
				our globalExtremaCache. maximum = +1.0;
			}
			our globalExtremaCache. valid = true;
		}
	}
	/*
		Maintain the derived data caches.
	*/
	void v_invalidateAllDerivedDataCaches () override {
		our invalidateGlobalExtremaCache ();
		SoundArea_Parent :: v_invalidateAllDerivedDataCaches ();
	}

	/*
		Auxiliary data.
	*/
	integer channelOffset;
	autoBOOLVEC muteChannels;
	/*
		Maintain the auxiliary data.
	*/
	void v_computeAuxiliaryData () override {
		Melder_assert (our soundOrLongSound ());
		Melder_clip (0_integer, & our channelOffset, (our soundOrLongSound() -> ny - 1) / 8 * 8);
		if (our muteChannels.size == 0 || our muteChannels.size != our soundOrLongSound() -> ny)
			our muteChannels = zero_BOOLVEC (our soundOrLongSound() -> ny);
	}

	double ymin, ymax;

	virtual conststring32 v_getChannelName (integer /* channelNumber */) { return nullptr; }

	void viewSoundAsWorldByWorld () const {
		our setViewport ();
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	}

	#include "SoundArea_prefs.h"
};

void SoundArea_drawCursorFunctionValue (SoundArea me, double yWC, conststring32 yWC_string, conststring32 units);

void SoundArea_draw (SoundArea me);

bool SoundArea_mouse (SoundArea me, Sound sound, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);

void SoundArea_init (SoundArea me, FunctionEditor editor, SampledXY soundOrLongSound, bool makeCopy);

autoSoundArea SoundArea_create (FunctionEditor editor, SampledXY soundOrLongSound, bool makeCopy);

/* End of file SoundArea.h */
#endif
