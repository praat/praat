#ifndef _RealTierArea_h_
#define _RealTierArea_h_
/* RealTierArea.h
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

#include "RealTier.h"
#include "FunctionEditor.h"

Thing_define (FunctionView, Thing) {
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

inline static void FunctionView_init (FunctionView me, FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	my editor = editor;
	my ymin_fraction = ymin_fraction;
	my ymax_fraction = ymax_fraction;
}

Thing_define (RealTierView, FunctionView) {
	virtual double v_minimumLegalY () { return undefined; }
	virtual double v_maximumLegalY () { return undefined; }
	virtual conststring32 v_rightTickUnits () { return U""; }
	virtual double v_defaultYmin () { return 0.0; }
	virtual double v_defaultYmax () { return 1.0; }
	virtual double v_valueToY (double value) { return value; }
	virtual double v_yToValue (double y) { return y; }

	double ymin, ymax, ycursor;
	double anchorTime = undefined, anchorY;
	bool draggingSelection;
	double dt = 0.0, dy = 0.0;
	integer firstSelected, lastSelected;

	void viewRealTierAsWorldByWorld () const {
		our setViewport ();
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	}
};

void RealTierArea_addPointAt (RealTierView me, RealTier tier, double time, double desiredY);

void RealTierArea_removePoints (RealTierView me, RealTier tier);

void RealTierArea_addPointAtCursor (RealTierView me, RealTier tier);

void RealTierArea_updateScaling (RealTierView me, RealTier tier);

void RealTierArea_draw (RealTierView me, RealTier tier);

void RealTierArea_drawWhileDragging (RealTierView me, RealTier tier);

/* End of file RealTierArea.h */
#endif
