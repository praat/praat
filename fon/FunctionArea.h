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
	bool y_fraction_globalIsInside (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return y_pxlt >= our bottom_pxlt() && y_pxlt <= our top_pxlt();
	}
	void setViewport() const {
		Graphics_setViewport (our graphics(), our left_pxlt(), our right_pxlt(), our bottom_pxlt(), our top_pxlt());
	}
	double y_fraction_globalToLocal (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return (y_pxlt - our bottom_pxlt()) / (our top_pxlt() - our bottom_pxlt());
	}
private:
	double globalY_fraction_to_pxlt (double globalY_fraction) const {
		return our editor -> dataBottom_pxlt() +
				globalY_fraction * (our editor -> dataTop_pxlt() - our editor -> dataBottom_pxlt());
	}
	double left_pxlt() const { return our editor -> dataLeft_pxlt(); }
	double right_pxlt() const { return our editor -> dataRight_pxlt(); }
	double verticalSpacing_pxlt() const { return 11; }
	double bottom_pxlt() const {
		const double bottomSpacing_pxlt = ( our ymin_fraction == 0.0 ? 0.0 : our verticalSpacing_pxlt() );
		return globalY_fraction_to_pxlt (our ymin_fraction) + bottomSpacing_pxlt;
	}
	double top_pxlt() const {
		return globalY_fraction_to_pxlt (our ymax_fraction) - our verticalSpacing_pxlt();
	}
};

inline void FunctionArea_init (FunctionArea me, FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	my editor = editor;
	my ymin_fraction = ymin_fraction;
	my ymax_fraction = ymax_fraction;
}

/* End of file FunctionArea.h */
#endif
