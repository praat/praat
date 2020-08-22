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

#include "FunctionArea.h"
#include "RealTier.h"

Thing_define (RealTierArea, FunctionArea) {
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

void RealTierArea_addPointAt (RealTierArea me, RealTier tier, double time, double desiredY);

void RealTierArea_removePoints (RealTierArea me, RealTier tier);

void RealTierArea_addPointAtCursor (RealTierArea me, RealTier tier);

void RealTierArea_updateScaling (RealTierArea me, RealTier tier);

void RealTierArea_draw (RealTierArea me, RealTier tier);

void RealTierArea_drawWhileDragging (RealTierArea me, RealTier tier);

bool RealTierArea_mouse (RealTierArea me, RealTier tier, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);

/* End of file RealTierArea.h */
#endif
