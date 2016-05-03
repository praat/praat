#ifndef _SpectrogramEditor_h_
#define _SpectrogramEditor_h_
/* SpectrogramEditor.h
 *
 * Copyright (C) 1992-2011,2012,2015 Paul Boersma
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
#include "Spectrogram.h"

Thing_define (SpectrogramEditor, FunctionEditor) {
	double maximum;

	void v_draw ()
		override;
	bool v_click (double xWC, double yWC, bool shiftKeyPressed)
		override;
};

autoSpectrogramEditor SpectrogramEditor_create (const char32 *title, Spectrogram data);

/* End of file SpectrogramEditor.h */
#endif
