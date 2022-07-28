#ifndef _SpectrumArea_h_
#define _SpectrumArea_h_
/* SpectrumArea.h
 *
 * Copyright (C) 1992-2005,2007-2013,2015,2016,2018-2020,2022 Paul Boersma
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
#include "Sound_and_Spectrum.h"

Thing_define (SpectrumArea, FunctionArea) {
	Spectrum spectrum() { return static_cast <Spectrum> (our function()); }
	
	double minimum, maximum, cursorHeight;
	GuiMenuItem publishBandButton, publishSoundButton;

	void updateRange () {
		if (Spectrum_getPowerDensityRange (our spectrum(), & our minimum, & our maximum)) {
			our minimum = our maximum - our instancePref_dynamicRange();
		} else {
			our minimum = -1000.0;
			our maximum = 1000.0;
		}
	}
	void v1_info ()
		override;
	void v_createMenus ()
		override;
	void v_drawInside ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
		override;
	void v_createMenuItems_view (EditorMenu menu)
		override;
	void v_updateMenuItems () override {
		integer first, last;
		const integer selectedSamples = Sampled_getWindowSamples (our spectrum(), our startSelection(), our endSelection(), & first, & last);
		GuiThing_setSensitive (our publishBandButton,  selectedSamples != 0);
		GuiThing_setSensitive (our publishSoundButton, selectedSamples != 0);
	}

	#include "SpectrumArea_prefs.h"
};
DEFINE_FunctionArea_create (SpectrumArea, Spectrum)

inline static autoSpectrum Spectrum_band (Spectrum me, double fmin, double fmax) {
	autoSpectrum band = Data_copy (me);
	double *re = & band -> z [1] [0], *im = & band -> z [2] [0];
	const integer imin = Sampled_xToLowIndex (band.get(), fmin);
	const integer imax = Sampled_xToHighIndex (band.get(), fmax);
	for (integer i = 1; i <= imin; i ++)
		re [i] = 0.0, im [i] = 0.0;
	for (integer i = imax; i <= band -> nx; i ++)
		re [i] = 0.0, im [i] = 0.0;
	return band;
}

inline static autoSound Spectrum_to_Sound_part (Spectrum me, double fmin, double fmax) {
	autoSpectrum band = Spectrum_band (me, fmin, fmax);
	autoSound sound = Spectrum_to_Sound (band.get());
	return sound;
}

/* End of file SpectrumArea.h */
#endif
