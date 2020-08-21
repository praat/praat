#ifndef _FunctionArea_h_
#define _FunctionArea_h_
/* FunctionArea.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015-2018,2020 Paul Boersma
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

Thing_define (FunctionArea, Thing) {
	FunctionEditor editor;
	double ymin_fraction, ymax_fraction;
	Graphics graphics() const { return our editor -> graphics.get(); }
	double startWindow() const { return our editor -> startWindow; }
	double endWindow() const { return our editor -> endWindow; }
	double startSelection() const { return our editor -> startSelection; }
	double endSelection() const { return our editor -> endSelection; }
	void setViewport() const {
		Graphics_setViewport (our graphics(),
				our editor -> dataLeft_pxlt(), our editor -> dataRight_pxlt(), our editor -> dataBottom_pxlt(), our editor -> dataTop_pxlt());
		Graphics_insetViewport (our graphics(), 0.0, 1.0, our ymin_fraction, our ymax_fraction);
	}
};

inline static void FunctionArea_init (FunctionArea me, FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	my editor = editor;
	my ymin_fraction = ymin_fraction;
	my ymax_fraction = ymax_fraction;
}

/* End of file FunctionArea.h */
#endif
