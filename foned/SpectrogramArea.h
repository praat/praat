#ifndef _SpectrogramArea_h_
#define _SpectrogramArea_h_
/* SpectrogramArea.h
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

#include "FunctionArea.h"
#include "Spectrogram.h"

Thing_define (SpectrogramArea, FunctionArea) {
	Spectrogram spectrogram() { return static_cast <Spectrogram> (our function()); }
	
	double maximum = 10000.0, frequencyCursor = 5000.0;

	void v_drawInside ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
		override;
};
DEFINE_FunctionArea_create (SpectrogramArea, Spectrogram)

/* End of file SpectrogramArea.h */
#endif
