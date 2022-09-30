#ifndef _SpectrogramEditor_h_
#define _SpectrogramEditor_h_
/* SpectrogramEditor.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015,2016,2018,2020,2022 Paul Boersma
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

#include "FunctionEditor.h"
#include "SpectrogramArea.h"

Thing_define (SpectrogramEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, SpectrogramArea, spectrogramArea)
	
	void v1_dataChanged (Editor sender) override {
		our SpectrogramEditor_Parent :: v1_dataChanged (sender);
		our spectrogramArea() -> functionChanged (static_cast <Spectrogram> (our data()));
	}
	void v_distributeAreas () override {
		our spectrogramArea() -> setGlobalYRange_fraction (0.0, 1.0);
	}
};

autoSpectrogramEditor SpectrogramEditor_create (conststring32 title, Spectrogram spectrogram);

/* End of file SpectrogramEditor.h */
#endif
