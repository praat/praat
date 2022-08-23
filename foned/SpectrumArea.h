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
	
	double cursorHeight = -1000;
	GuiMenuItem publishBandButton, publishSoundButton;
private:
	double _minimum, _maximum;
public:
	double minimum() { return _minimum; }
	double maximum() { return _maximum; }

	void updateRange () {
		int result = Spectrum_getPowerDensityRange (our spectrum(), & _minimum, & _maximum);
		if (result) {
			our _minimum = our _maximum - our instancePref_dynamicRange();
		} else {
			our _minimum = -1000.0;
			our _maximum = 1000.0;
		}
	}
	void v_computeAuxiliaryData () override {
		our updateRange ();
	}
	void v1_info ()
		override;
	void v_createMenus ()
		override;
	void v_drawInside ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
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

void SpectrumArea_play (SpectrumArea me, double fromFrequency, double toFrequency);

/* End of file SpectrumArea.h */
#endif
